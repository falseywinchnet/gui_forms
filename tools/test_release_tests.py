#!/usr/bin/env python3
"""Tests for tools/release_tests.py: carrying an earlier run's ctest receipt to a tag or main push of the same tree."""
from __future__ import annotations
import io
import json
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


def receipts_zip(platform: str, text: str, revision: str) -> bytes:
    stream: io.BytesIO = io.BytesIO()
    with zipfile.ZipFile(stream, 'w') as archive:
        archive.writestr('ctest-' + platform + '.xml', text)
        archive.writestr('packages-' + platform + '.json', json.dumps({'revision': revision}))
    return stream.getvalue()


class FakeRuns:
    def __init__(self) -> None:
        self.repository: str = 'owner/repo'
        self.runs: list[dict[str, Any]] = []
        self.trees: dict[str, str] = {}
        self.run_jobs: dict[int, list[dict[str, Any]]] = {}
        self.receipts: dict[int, bytes] = {}
        self.pull_requests: list[dict[str, Any]] = []
        self.merge_groups: list[dict[str, Any]] = []
        self.parents: dict[str, list[str]] = {}

    def add(self, run: int, revision: str, tree: str, job: dict[str, Any] | None, receipt: bytes | None) -> None:
        self.runs.append({'id': run, 'path': '.github/workflows/native.yml', 'event': 'push', 'head_branch': 'main',
                          'head_sha': revision, 'head_repository': {'full_name': 'owner/repo'}})
        self.trees[revision] = tree
        self.run_jobs[run] = [job] if job is not None else []
        if receipt is not None:
            self.receipts[run] = receipt

    def add_pull_request(self, run: int, head: str, merge: str, tree: str, job: dict[str, Any] | None,
                         receipt: bytes | None) -> None:
        """A pull-request run on head whose programs job checked out the test merge commit merge."""
        self.pull_requests.append({'id': run, 'path': '.github/workflows/native.yml', 'event': 'pull_request',
                                   'head_branch': 'feature', 'head_sha': head,
                                   'head_repository': {'full_name': 'owner/repo'}})
        self.trees[merge] = tree
        self.run_jobs[run] = [job] if job is not None else []
        if receipt is not None:
            self.receipts[run] = receipt

    def add_merge_group(self, run: int, revision: str, tree: str, job: dict[str, Any] | None,
                        receipt: bytes | None) -> None:
        """A merge-group run whose commit is revision."""
        self.merge_groups.append({'id': run, 'path': '.github/workflows/native.yml', 'event': 'merge_group',
                                  'head_branch': 'gh-readonly-queue/main/pr-7-' + 'a' * 40, 'head_sha': revision,
                                  'head_repository': {'full_name': 'owner/repo'}})
        self.trees[revision] = tree
        self.run_jobs[run] = [job] if job is not None else []
        if receipt is not None:
            self.receipts[run] = receipt

    def main_runs(self) -> list[dict[str, Any]]:
        return self.runs

    def merge_group_runs(self, revision: str) -> list[dict[str, Any]]:
        return self.merge_groups

    def pull_request_runs(self, head: str) -> list[dict[str, Any]]:
        return self.pull_requests

    def commit_tree(self, revision: str) -> str:
        return self.trees.get(revision, 'unknown-tree')

    def commit_parents(self, revision: str) -> list[str]:
        return self.parents.get(revision, [])

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
        fake.add(30, 'other', 'tree-b', programs('linux-x64'), receipts_zip('linux-x64', PASSING, 'other'))
        fake.add(20, 'tagged', 'tree-a', programs('linux-x64'), receipts_zip('linux-x64', PASSING, 'tagged'))
        fake.add(10, 'older', 'tree-a', programs('linux-x64'), receipts_zip('linux-x64', PASSING, 'older'))
        run: int | None
        dist: Path
        run, dist = self.find(fake)
        self.assertEqual(run, 20)
        text: str = (dist / 'ctest-linux-x64.xml').read_text(encoding='utf-8')
        self.assertTrue(text.startswith('<?xml version="1.0" encoding="UTF-8"?>\n<!-- carried from native.yml run 20'))
        self.assertTrue(release_tests.receipt_passed(text))

    def test_the_older_matrix_call_name_is_found(self) -> None:
        fake: FakeRuns = FakeRuns()
        fake.add(20, 'tagged', 'tree-a', programs('linux-x64', caller='programs'),
                 receipts_zip('linux-x64', PASSING, 'tagged'))
        self.assertEqual(self.find(fake)[0], 20)

    def test_a_failed_job_falls_to_an_older_run(self) -> None:
        fake: FakeRuns = FakeRuns()
        fake.add(20, 'tagged', 'tree-a', programs('linux-x64', 'failure'), receipts_zip('linux-x64', PASSING, 'tagged'))
        fake.add(10, 'again', 'tree-a', programs('linux-x64'), receipts_zip('linux-x64', PASSING, 'again'))
        self.assertEqual(self.find(fake)[0], 10)

    def test_a_running_or_missing_job_carries_nothing(self) -> None:
        fake: FakeRuns = FakeRuns()
        fake.add(20, 'tagged', 'tree-a', programs('linux-x64', None),  # type: ignore[arg-type]
                 receipts_zip('linux-x64', PASSING, 'tagged'))
        fake.add(10, 'other', 'tree-a', None, None)
        run: int | None
        dist: Path
        run, dist = self.find(fake)
        self.assertIsNone(run)
        self.assertFalse(dist.exists())

    def test_another_platforms_job_is_not_taken(self) -> None:
        fake: FakeRuns = FakeRuns()
        fake.add(20, 'tagged', 'tree-a', programs('linux-arm64'), receipts_zip('linux-arm64', PASSING, 'tagged'))
        self.assertIsNone(self.find(fake)[0])

    def test_a_failing_or_empty_receipt_is_not_carried(self) -> None:
        fake: FakeRuns = FakeRuns()
        fake.add(30, 'one', 'tree-a', programs('linux-x64'), receipts_zip('linux-x64', FAILING, 'one'))
        fake.add(20, 'two', 'tree-a', programs('linux-x64'), receipts_zip('linux-x64', EMPTY, 'two'))
        fake.add(10, 'three', 'tree-a', programs('linux-x64'), None)
        self.assertIsNone(self.find(fake)[0])

    def test_a_run_of_another_repository_is_not_trusted(self) -> None:
        fake: FakeRuns = FakeRuns()
        fake.add(20, 'tagged', 'tree-a', programs('linux-x64'), receipts_zip('linux-x64', PASSING, 'tagged'))
        fake.runs[0]['head_repository'] = {'full_name': 'fork/repo'}
        self.assertIsNone(self.find(fake)[0])


    def test_a_run_that_packaged_another_tree_is_not_carried(self) -> None:
        fake: FakeRuns = FakeRuns()
        fake.add(20, 'tagged', 'tree-a', programs('linux-x64'), receipts_zip('linux-x64', PASSING, 'elsewhere'))
        fake.trees['elsewhere'] = 'tree-b'
        self.assertIsNone(self.find(fake)[0])


def merged_main(fake: FakeRuns) -> None:
    """main's merge commit 'merge' of 'previous' and the pull request's head 'head', all of tree-a."""
    fake.trees['merge'] = 'tree-a'
    fake.trees['head'] = 'tree-a'
    fake.parents['merge'] = ['previous', 'head']


class MergedTests(unittest.TestCase):
    def merged(self, fake: FakeRuns, platform: str = 'linux-x64') -> tuple[int | None, Path]:
        directory: tempfile.TemporaryDirectory[str] = tempfile.TemporaryDirectory()
        self.addCleanup(directory.cleanup)
        dist: Path = Path(directory.name) / 'dist'
        run: int | None = release_tests.merged(platform, dist, 'merge', 'tree-a', fake)  # type: ignore[arg-type]
        return run, dist

    def test_the_merge_group_of_mains_commit_is_carried_first(self) -> None:
        fake: FakeRuns = FakeRuns()
        merged_main(fake)
        fake.add_pull_request(50, 'head', 'test-merge', 'tree-a', programs('linux-x64'),
                              receipts_zip('linux-x64', PASSING, 'test-merge'))
        fake.add_merge_group(60, 'merge', 'tree-a', programs('linux-x64'), receipts_zip('linux-x64', PASSING, 'merge'))
        run: int | None
        dist: Path
        run, dist = self.merged(fake)
        self.assertEqual(run, 60)
        text: str = (dist / 'ctest-linux-x64.xml').read_text(encoding='utf-8')
        self.assertIn('<!-- carried from native.yml merge-group run 60, tree tree-a', text)

    def test_a_queued_merge_of_a_head_behind_main_is_carried(self) -> None:
        # The pull request's head is behind main: its tree differs, so only
        # the merge group's run tested main's tree.
        fake: FakeRuns = FakeRuns()
        fake.trees['merge'] = 'tree-a'
        fake.trees['head'] = 'tree-b'
        fake.parents['merge'] = ['previous', 'head']
        fake.add_merge_group(60, 'merge', 'tree-a', programs('linux-x64'), receipts_zip('linux-x64', PASSING, 'merge'))
        self.assertEqual(self.merged(fake)[0], 60)

    def test_a_merge_group_of_another_commit_fork_or_failure_is_not_carried(self) -> None:
        fake: FakeRuns = FakeRuns()
        fake.add_merge_group(60, 'other', 'tree-a', programs('linux-x64'), receipts_zip('linux-x64', PASSING, 'other'))
        fake.trees['merge'] = 'tree-a'
        self.assertIsNone(self.merged(fake)[0])
        fork: FakeRuns = FakeRuns()
        fork.add_merge_group(60, 'merge', 'tree-a', programs('linux-x64'), receipts_zip('linux-x64', PASSING, 'merge'))
        fork.merge_groups[0]['head_repository'] = {'full_name': 'fork/repo'}
        self.assertIsNone(self.merged(fork)[0])
        failed: FakeRuns = FakeRuns()
        failed.add_merge_group(60, 'merge', 'tree-a', programs('linux-x64', 'failure'),
                               receipts_zip('linux-x64', PASSING, 'merge'))
        self.assertIsNone(self.merged(failed)[0])

    def test_the_merged_pull_requests_receipt_is_carried_and_named(self) -> None:
        fake: FakeRuns = FakeRuns()
        merged_main(fake)
        fake.add_pull_request(50, 'head', 'test-merge', 'tree-a', programs('linux-x64'),
                              receipts_zip('linux-x64', PASSING, 'test-merge'))
        run: int | None
        dist: Path
        run, dist = self.merged(fake)
        self.assertEqual(run, 50)
        text: str = (dist / 'ctest-linux-x64.xml').read_text(encoding='utf-8')
        self.assertIn('<!-- carried from native.yml pull-request run 50, tree tree-a', text)
        self.assertIn("tests not in the validated list of this tree's native job", text)
        self.assertTrue(release_tests.receipt_passed(text))

    def test_a_push_that_merges_nothing_runs_every_test(self) -> None:
        fake: FakeRuns = FakeRuns()
        merged_main(fake)
        fake.parents['merge'] = ['previous']
        fake.add_pull_request(50, 'head', 'test-merge', 'tree-a', programs('linux-x64'),
                              receipts_zip('linux-x64', PASSING, 'test-merge'))
        self.assertIsNone(self.merged(fake)[0])

    def test_a_head_of_another_tree_is_not_carried(self) -> None:
        fake: FakeRuns = FakeRuns()
        merged_main(fake)
        fake.trees['head'] = 'tree-b'
        fake.add_pull_request(50, 'head', 'test-merge', 'tree-a', programs('linux-x64'),
                              receipts_zip('linux-x64', PASSING, 'test-merge'))
        self.assertIsNone(self.merged(fake)[0])

    def test_a_run_whose_test_merge_had_another_tree_is_not_carried(self) -> None:
        fake: FakeRuns = FakeRuns()
        merged_main(fake)
        fake.add_pull_request(60, 'head', 'stale-merge', 'tree-b', programs('linux-x64'),
                              receipts_zip('linux-x64', PASSING, 'stale-merge'))
        fake.add_pull_request(50, 'head', 'test-merge', 'tree-a', programs('linux-x64'),
                              receipts_zip('linux-x64', PASSING, 'test-merge'))
        self.assertEqual(self.merged(fake)[0], 50)

    def test_a_fork_or_another_head_is_not_trusted(self) -> None:
        fake: FakeRuns = FakeRuns()
        merged_main(fake)
        fake.add_pull_request(60, 'head', 'test-merge', 'tree-a', programs('linux-x64'),
                              receipts_zip('linux-x64', PASSING, 'test-merge'))
        fake.pull_requests[0]['head_repository'] = {'full_name': 'fork/repo'}
        fake.add_pull_request(50, 'other-head', 'test-merge', 'tree-a', programs('linux-x64'),
                              receipts_zip('linux-x64', PASSING, 'test-merge'))
        self.assertIsNone(self.merged(fake)[0])


class ReceiptTests(unittest.TestCase):
    def test_malformed_receipts_do_not_pass(self) -> None:
        self.assertFalse(release_tests.receipt_passed('not xml'))
        self.assertFalse(release_tests.receipt_passed('<testsuites tests="3" failures="0"/>'))
        self.assertFalse(release_tests.receipt_passed('<testsuite tests="3" failures="0" errors="2"/>'))
        self.assertTrue(release_tests.receipt_passed('<testsuite tests="3" failures="0"/>'))


if __name__ == '__main__':
    unittest.main()
