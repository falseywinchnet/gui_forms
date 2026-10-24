#!/usr/bin/env python3
"""Carry main's test receipt to a release tag of the same tree.

A `v*` tag names a commit main has already built and tested: native.yml's
programs job for each platform ran every test of the programs build on that
tree (a push to main never narrows the selection). `find` looks for that
job. It takes the newest native.yml push run on main whose head commit has
this checkout's tree and whose programs job for --platform succeeded, then
copies that job's ctest receipt (ctest-<platform>.xml, from the run's
gui-forms-receipts-<platform> artifact) into --dist, with a comment naming
the run. The tag's programs job then skips its own ctest run. It still
rebuilds from the compiler cache, packages under the tag's version and
installs and launches what it packaged: the version enters only the
package metadata, so the tested binaries are the ones packaged.

The receipt is taken only when the job succeeded and the receipt reports at
least one test and no failure or error. Otherwise nothing is written, the
step output reused is false, and the tag runs every test as before. A main
run whose programs job has not finished is not waited for.
"""
from __future__ import annotations
import argparse
import io
import json
import os
from pathlib import Path
import subprocess
import sys
from typing import Any
import xml.etree.ElementTree as ElementTree
import zipfile

ROOT: Path = Path(__file__).resolve().parents[1]
RUNS_READ: int = 50


class GitHubRuns:
    """native.yml's push runs on main, their jobs and artifacts, through `gh api`."""

    def __init__(self, repository: str) -> None:
        self.repository: str = repository

    def api(self, endpoint: str) -> Any:
        return json.loads(subprocess.check_output(['gh', 'api', 'repos/' + self.repository + '/' + endpoint], text=True))

    def main_runs(self) -> list[dict[str, Any]]:
        listing: dict[str, Any] = self.api('actions/workflows/native.yml/runs?branch=main&event=push&per_page='
                                           + str(RUNS_READ))
        runs: list[dict[str, Any]] = listing['workflow_runs']
        return runs

    def commit_tree(self, revision: str) -> str:
        commit: dict[str, Any] = self.api('git/commits/' + revision)
        tree: str = commit['tree']['sha']
        return tree

    def jobs(self, run: int) -> list[dict[str, Any]]:
        listing: dict[str, Any] = self.api('actions/runs/' + str(run) + '/jobs?per_page=100')
        jobs: list[dict[str, Any]] = listing['jobs']
        return jobs

    def artifact(self, run: int, name: str) -> int | None:
        listing: dict[str, Any] = self.api('actions/runs/' + str(run) + '/artifacts?name=' + name)
        artifact: dict[str, Any]
        for artifact in listing.get('artifacts', []):
            if artifact.get('name') == name and not artifact.get('expired'):
                return int(artifact['id'])
        return None

    def download(self, artifact: int) -> bytes:
        return subprocess.check_output(['gh', 'api', 'repos/' + self.repository + '/actions/artifacts/'
                                        + str(artifact) + '/zip'])


def programs_job(jobs: list[dict[str, Any]], platform: str) -> dict[str, Any] | None:
    """The run's programs job for platform: "programs-<platform> / programs (...)", or the
    older "programs / programs (...)" of the single matrix call."""
    found: list[dict[str, Any]] = []
    job: dict[str, Any]
    for job in jobs:
        name: str = str(job.get('name', ''))
        if ' / programs (' in name and ', ' + platform + ',' in name:
            found.append(job)
    if len(found) != 1:
        return None
    return found[0]


def receipt_in_zip(data: bytes, platform: str) -> str | None:
    wanted: str = 'ctest-' + platform + '.xml'
    with zipfile.ZipFile(io.BytesIO(data)) as archive:
        name: str
        for name in archive.namelist():
            if name.rsplit('/', 1)[-1] == wanted:
                return archive.read(name).decode('utf-8')
    return None


def receipt_passed(text: str) -> bool:
    """Whether a ctest JUnit receipt reports at least one test and no failure or error."""
    try:
        root: ElementTree.Element = ElementTree.fromstring(text)
    except ElementTree.ParseError:
        return False
    if root.tag != 'testsuite':
        return False
    tests: str = root.get('tests', '0')
    failures: str = root.get('failures', '0')
    errors: str = root.get('errors', '0')
    if not tests.isdigit() or not failures.isdigit() or not errors.isdigit():
        return False
    return int(tests) > 0 and int(failures) == 0 and int(errors) == 0


def with_provenance(text: str, run: int, tree: str) -> str:
    """The receipt with a comment after its XML declaration naming the run it came from."""
    comment: str = ('<!-- carried from native.yml run ' + str(run) + ' on main, tree ' + tree
                    + ': the tag has the same tree, so these are its tests -->')
    if text.startswith('<?xml'):
        end: int = text.index('?>') + 2
        return text[:end] + '\n' + comment + text[end:]
    return comment + '\n' + text


def find(platform: str, dist: Path, tree: str, runs: GitHubRuns) -> int | None:
    """Copy main's passing receipt for this tree into dist; the run it came from, or None."""
    run: dict[str, Any]
    for run in runs.main_runs():
        if run.get('path') != '.github/workflows/native.yml' or run.get('event') != 'push':
            continue
        if run.get('head_branch') != 'main' or run.get('head_repository', {}).get('full_name') != runs.repository:
            continue
        if runs.commit_tree(str(run['head_sha'])) != tree:
            continue
        run_id: int = int(run['id'])
        job: dict[str, Any] | None = programs_job(runs.jobs(run_id), platform)
        if job is None or job.get('conclusion') != 'success':
            print('Run ' + str(run_id) + ' has this tree but no successful programs job for ' + platform, flush=True)
            continue
        artifact: int | None = runs.artifact(run_id, 'gui-forms-receipts-' + platform)
        if artifact is None:
            print('Run ' + str(run_id) + ' has no unexpired receipts for ' + platform, flush=True)
            continue
        text: str | None = receipt_in_zip(runs.download(artifact), platform)
        if text is None or not receipt_passed(text):
            print('Run ' + str(run_id) + "'s ctest receipt for " + platform + ' is missing or not a pass', flush=True)
            continue
        dist.mkdir(parents=True, exist_ok=True)
        (dist / ('ctest-' + platform + '.xml')).write_text(with_provenance(text, run_id, tree), encoding='utf-8')
        print('Carrying the tests of main run ' + str(run_id) + ' for tree ' + tree, flush=True)
        return run_id
    print('No main run tested tree ' + tree + ' on ' + platform + '; the tag runs every test', flush=True)
    return None


def checkout_tree() -> str:
    tree: str = subprocess.check_output(['git', '-C', str(ROOT), 'rev-parse', 'HEAD^{tree}'], text=True).strip()
    return tree


def write_output(name: str, value: str) -> None:
    path: str = os.environ.get('GITHUB_OUTPUT', '')
    if path:
        with open(path, 'a', encoding='utf-8') as output:
            output.write(name + '=' + value + '\n')


def main(argv: list[str] | None = None) -> int:
    parser: argparse.ArgumentParser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    commands: Any = parser.add_subparsers(dest='command', required=True)
    find_parser: argparse.ArgumentParser = commands.add_parser('find', help="carry main's receipt for this tree")
    find_parser.add_argument('--platform', required=True)
    find_parser.add_argument('--dist', type=Path, required=True)
    find_parser.add_argument('--repo', default=os.environ.get('GITHUB_REPOSITORY'))
    args: argparse.Namespace = parser.parse_args(argv)
    if not args.repo:
        parser.error('find needs --repo or GITHUB_REPOSITORY')
    run: int | None = find(args.platform, args.dist, checkout_tree(), GitHubRuns(args.repo))
    write_output('reused', 'true' if run is not None else 'false')
    write_output('run', str(run) if run is not None else '')
    return 0


if __name__ == '__main__':
    sys.exit(main())
