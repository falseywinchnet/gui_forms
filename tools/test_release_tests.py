#!/usr/bin/env python3
"""Tests for tools/release_tests.py: carrying main's ctest receipt to a tag of the same tree."""
from __future__ import annotations
import io
from pathlib import Path
import sys
import tempfile
from typing import Any
import unittest
import zipfile

sys.path.insert(0, str(Path(__file__).resolve().parent))
import release_tests  # noqa: E402

PASSING: str = ('<?xml version="1.0" encoding="UTF-8"?>\n<testsuite name="(empty)"\n\ttests="224"\n\tfailures="0"\n'
                '\tdisabled="0"\n\tskipped="0"\n\t>\n\t<testcase name="a" status="run"/>\n</testsuite>\n')
FAILING: str = PASSING.replace('failures="0"', 'failures="1"')
EMPTY: str = PASSING.replace('tests="224"', 'tests="0"')


def receipts_zip(platform: str, text: str) -> bytes:
    stream: io.BytesIO = io.BytesIO()
    with zipfile.ZipFile(stream, 'w') as archive:
        archive.writestr('ctest-' + platform + '.xml', text)
        archive.writestr('packages-' + platform + '.json', '{}')
    return stream.getvalue()


class FakeRuns:
    def __init__(self) -> None:
        self.repository: str = 'owner/repo'
        self.runs: list[dict[str, Any]] = []
        self.trees: dict[str, str] = {}
        self.run_jobs: dict[int, list[dict[str, Any]]] = {}
        self.receipts: dict[int, bytes] = {}

    def add(self, run: int, revision: str, tree: str, job: dict[str, Any] | None, receipt: bytes | None) -> None:
        self.runs.append({'id': run, 'path': '.github/workflows/native.yml', 'event': 'push', 'head_branch': 'main',
                          'head_sha': revision, 'head_repository': {'full_name': 'owner/repo'}})
        self.trees[revision] = tree
        self.run_jobs[run] = [job] if job is not None else []
        if receipt is not None:
            self.receipts[run] = receipt

    def main_runs(self) -> list[dict[str, Any]]:
        return self.runs

    def commit_tree(self, revision: str) -> str:
        return self.trees[revision]

    def jobs(self, run: int) -> list[dict[str, Any]]:
        return self.run_jobs[run]

    def artifact(self, run: int, name: str) -> int | None:
        if run in self.receipts:
            return run
        return None

    def download(self, artifact: int) -> bytes:
        return self.receipts[artifact]


def programs(platform: str, conclusion: str = 'success', caller: str = 'programs-linux-x64') -> dict[str, Any]:
    return {'name': caller + ' / programs (ubuntu-24.04, ' + platform + ', LINUX, bash)', 'conclusion': conclusion}


class FindTests(unittest.TestCase):
    def find(self, fake: FakeRuns, platform: str = 'linux-x64') -> tuple[int | None, Path]:
        directory: tempfile.TemporaryDirectory[str] = tempfile.TemporaryDirectory()
        self.addCleanup(directory.cleanup)
        dist: Path = Path(directory.name) / 'dist'
        run: int | None = release_tests.find(platform, dist, 'tree-a', fake)  # type: ignore[arg-type]
        return run, dist

    def test_the_newest_passing_run_of_the_tree_is_carried(self) -> None:
        fake: FakeRuns = FakeRuns()
        fake.add(30, 'other', 'tree-b', programs('linux-x64'), receipts_zip('linux-x64', PASSING))
        fake.add(20, 'tagged', 'tree-a', programs('linux-x64'), receipts_zip('linux-x64', PASSING))
        fake.add(10, 'older', 'tree-a', programs('linux-x64'), receipts_zip('linux-x64', PASSING))
        run: int | None
        dist: Path
        run, dist = self.find(fake)
        self.assertEqual(run, 20)
        text: str = (dist / 'ctest-linux-x64.xml').read_text(encoding='utf-8')
        self.assertTrue(text.startswith('<?xml version="1.0" encoding="UTF-8"?>\n<!-- carried from native.yml run 20'))
        self.assertTrue(release_tests.receipt_passed(text))

    def test_the_older_matrix_call_name_is_found(self) -> None:
        fake: FakeRuns = FakeRuns()
        fake.add(20, 'tagged', 'tree-a', programs('linux-x64', caller='programs'), receipts_zip('linux-x64', PASSING))
        self.assertEqual(self.find(fake)[0], 20)

    def test_a_failed_job_falls_to_an_older_run(self) -> None:
        fake: FakeRuns = FakeRuns()
        fake.add(20, 'tagged', 'tree-a', programs('linux-x64', 'failure'), receipts_zip('linux-x64', PASSING))
        fake.add(10, 'again', 'tree-a', programs('linux-x64'), receipts_zip('linux-x64', PASSING))
        self.assertEqual(self.find(fake)[0], 10)

    def test_a_running_or_missing_job_carries_nothing(self) -> None:
        fake: FakeRuns = FakeRuns()
        fake.add(20, 'tagged', 'tree-a', programs('linux-x64', None), receipts_zip('linux-x64', PASSING))  # type: ignore[arg-type]
        fake.add(10, 'other', 'tree-a', None, None)
        run: int | None
        dist: Path
        run, dist = self.find(fake)
        self.assertIsNone(run)
        self.assertFalse(dist.exists())

    def test_another_platforms_job_is_not_taken(self) -> None:
        fake: FakeRuns = FakeRuns()
        fake.add(20, 'tagged', 'tree-a', programs('linux-arm64'), receipts_zip('linux-arm64', PASSING))
        self.assertIsNone(self.find(fake)[0])

    def test_a_failing_or_empty_receipt_is_not_carried(self) -> None:
        fake: FakeRuns = FakeRuns()
        fake.add(30, 'one', 'tree-a', programs('linux-x64'), receipts_zip('linux-x64', FAILING))
        fake.add(20, 'two', 'tree-a', programs('linux-x64'), receipts_zip('linux-x64', EMPTY))
        fake.add(10, 'three', 'tree-a', programs('linux-x64'), None)
        self.assertIsNone(self.find(fake)[0])

    def test_a_run_of_another_repository_is_not_trusted(self) -> None:
        fake: FakeRuns = FakeRuns()
        fake.add(20, 'tagged', 'tree-a', programs('linux-x64'), receipts_zip('linux-x64', PASSING))
        fake.runs[0]['head_repository'] = {'full_name': 'fork/repo'}
        self.assertIsNone(self.find(fake)[0])


class ReceiptTests(unittest.TestCase):
    def test_malformed_receipts_do_not_pass(self) -> None:
        self.assertFalse(release_tests.receipt_passed('not xml'))
        self.assertFalse(release_tests.receipt_passed('<testsuites tests="3" failures="0"/>'))
        self.assertFalse(release_tests.receipt_passed('<testsuite tests="3" failures="0" errors="2"/>'))
        self.assertTrue(release_tests.receipt_passed('<testsuite tests="3" failures="0"/>'))


if __name__ == '__main__':
    unittest.main()
