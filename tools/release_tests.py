#!/usr/bin/env python3
"""Carry an earlier run's test receipt to a release tag or main push of the same tree.

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

`merged` does the same for a push to main that merges a pull request
(the owner's choice of 2026-10-23, planning/BUILD_SIZE_PLAN_2026-10-23.md).
Through the merge queue, main's new commit is the one its merge group's
native.yml run tested, so that run is taken first. Merged without the
queue, a pull request is up to date with main, so the merge commit's
tree is its head's (the second parent), and the pull request's run on
that head tested this tree. Either run's programs job ran the tests the
native job's validated list does not name (tools/native_tests.py), and
the native job, which main's reuses, passed the rest. `merged` copies
that programs job's receipt, with a comment naming the run and saying
what it covers, and main's programs job skips its ctest run. Main thus trusts native_tests.py's name match between the
native and programs builds; the daily scheduled native.yml run on main
reuses nothing and runs every test, as the backstop for it. A tag of that
commit then carries main's receipt, comment and all.

The receipt is taken only when the job succeeded and the receipt reports at
least one test and no failure or error. Otherwise nothing is written, the
step output reused is false, and the job runs every test as before. A run
whose programs job has not finished is not waited for.
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

    def pull_request_runs(self, head: str) -> list[dict[str, Any]]:
        listing: dict[str, Any] = self.api('actions/workflows/native.yml/runs?event=pull_request&head_sha=' + head
                                           + '&per_page=' + str(RUNS_READ))
        runs: list[dict[str, Any]] = listing['workflow_runs']
        return runs

    def merge_group_runs(self, revision: str) -> list[dict[str, Any]]:
        listing: dict[str, Any] = self.api('actions/workflows/native.yml/runs?event=merge_group&head_sha=' + revision
                                           + '&per_page=' + str(RUNS_READ))
        runs: list[dict[str, Any]] = listing['workflow_runs']
        return runs

    def commit_tree(self, revision: str) -> str:
        commit: dict[str, Any] = self.api('git/commits/' + revision)
        tree: str = commit['tree']['sha']
        return tree

    def commit_parents(self, revision: str) -> list[str]:
        commit: dict[str, Any] = self.api('git/commits/' + revision)
        parents: list[str] = []
        parent: dict[str, Any]
        for parent in commit.get('parents', []):
            parents.append(str(parent['sha']))
        return parents

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


def packaged_revision(data: bytes, platform: str) -> str | None:
    """The source revision packages-<platform>.json in a receipts zip records, or None."""
    wanted: str = 'packages-' + platform + '.json'
    with zipfile.ZipFile(io.BytesIO(data)) as archive:
        name: str
        for name in archive.namelist():
            if name.rsplit('/', 1)[-1] == wanted:
                record: Any = json.loads(archive.read(name).decode('utf-8'))
                if isinstance(record, dict) and isinstance(record.get('revision'), str):
                    return str(record['revision'])
                return None
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


def main_comment(run: int, tree: str) -> str:
    comment: str = ('<!-- carried from native.yml run ' + str(run) + ' on main, tree ' + tree
                    + ': the tag has the same tree, so these are its tests -->')
    return comment


def pull_request_comment(run: int, tree: str) -> str:
    comment: str = ('<!-- carried from native.yml pull-request run ' + str(run) + ', tree ' + tree
                    + ': main merged that head, so these are its tests. They are the tests not in the'
                    + " validated list of this tree's native job (tools/native_tests.py), which passed the rest -->")
    return comment


def merge_group_comment(run: int, tree: str) -> str:
    comment: str = ('<!-- carried from native.yml merge-group run ' + str(run) + ', tree ' + tree
                    + ": main's commit is that merge group's, so these are its tests. They are the tests not in"
                    + " the validated list of this tree's native job (tools/native_tests.py), which passed the rest -->")
    return comment


def with_provenance(text: str, comment: str) -> str:
    """The receipt with comment, which names the run it came from, after its XML declaration."""
    if text.startswith('<?xml'):
        end: int = text.index('?>') + 2
        return text[:end] + '\n' + comment + text[end:]
    return comment + '\n' + text


def carry(platform: str, dist: Path, run_id: int, tree: str, comment: str, runs: GitHubRuns) -> bool:
    """Copy run_id's passing programs receipt for platform into dist under comment; whether it did.

    The run's packages receipt names the revision it built and packaged (a
    pull request's test merge commit, or a push's own commit); its tree
    must be tree.
    """
    job: dict[str, Any] | None = programs_job(runs.jobs(run_id), platform)
    if job is None or job.get('conclusion') != 'success':
        print('Run ' + str(run_id) + ' has this tree but no successful programs job for ' + platform, flush=True)
        return False
    artifact: int | None = runs.artifact(run_id, 'gui-forms-receipts-' + platform)
    if artifact is None:
        print('Run ' + str(run_id) + ' has no unexpired receipts for ' + platform, flush=True)
        return False
    data: bytes = runs.download(artifact)
    revision: str | None = packaged_revision(data, platform)
    if revision is None or runs.commit_tree(revision) != tree:
        print('Run ' + str(run_id) + ' did not package tree ' + tree + ' for ' + platform, flush=True)
        return False
    text: str | None = receipt_in_zip(data, platform)
    if text is None or not receipt_passed(text):
        print('Run ' + str(run_id) + "'s ctest receipt for " + platform + ' is missing or not a pass', flush=True)
        return False
    dist.mkdir(parents=True, exist_ok=True)
    (dist / ('ctest-' + platform + '.xml')).write_text(with_provenance(text, comment), encoding='utf-8')
    return True


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
        if not carry(platform, dist, run_id, tree, main_comment(run_id, tree), runs):
            continue
        print('Carrying the tests of main run ' + str(run_id) + ' for tree ' + tree, flush=True)
        return run_id
    print('No main run tested tree ' + tree + ' on ' + platform + '; the tag runs every test', flush=True)
    return None


def queued(platform: str, dist: Path, revision: str, tree: str, runs: GitHubRuns) -> int | None:
    """Copy the passing receipt of the merge group whose commit is revision into dist; its run, or None."""
    run: dict[str, Any]
    for run in runs.merge_group_runs(revision):
        if run.get('path') != '.github/workflows/native.yml' or run.get('event') != 'merge_group':
            continue
        if run.get('head_sha') != revision or run.get('head_repository', {}).get('full_name') != runs.repository:
            continue
        run_id: int = int(run['id'])
        if not carry(platform, dist, run_id, tree, merge_group_comment(run_id, tree), runs):
            continue
        print('Carrying the tests of merge-group run ' + str(run_id) + ' for tree ' + tree, flush=True)
        return run_id
    return None


def merged(platform: str, dist: Path, revision: str, tree: str, runs: GitHubRuns) -> int | None:
    """Copy the passing receipt of the merge group or merged pull request of this tree into dist; its run, or None."""
    queued_run: int | None = queued(platform, dist, revision, tree, runs)
    if queued_run is not None:
        return queued_run
    parents: list[str] = runs.commit_parents(revision)
    if len(parents) != 2:
        print(revision + ' does not merge a pull request; main runs every test', flush=True)
        return None
    head: str = parents[1]
    if runs.commit_tree(head) != tree:
        print('The merged head ' + head + ' has another tree; main runs every test', flush=True)
        return None
    run: dict[str, Any]
    for run in runs.pull_request_runs(head):
        if run.get('path') != '.github/workflows/native.yml' or run.get('event') != 'pull_request':
            continue
        if run.get('head_sha') != head or run.get('head_repository', {}).get('full_name') != runs.repository:
            continue
        run_id: int = int(run['id'])
        if not carry(platform, dist, run_id, tree, pull_request_comment(run_id, tree), runs):
            continue
        print('Carrying the tests of pull-request run ' + str(run_id) + ' for tree ' + tree, flush=True)
        return run_id
    print('No pull-request run tested tree ' + tree + ' on ' + platform + '; main runs every test', flush=True)
    return None


def checkout_field(format_code: str) -> str:
    # git show --format, not rev-parse HEAD^{tree}: MSYS2's argument
    # conversion can turn the braces into HEAD^tree (tools/ci_reuse.py).
    field: str = subprocess.check_output(['git', '-C', str(ROOT), 'show', '-s', '--format=' + format_code, 'HEAD'],
                                         text=True).strip()
    return field


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
    merged_parser: argparse.ArgumentParser = commands.add_parser(
        'merged', help="carry the merged pull request's receipt for this tree")
    command_parser: argparse.ArgumentParser
    for command_parser in (find_parser, merged_parser):
        command_parser.add_argument('--platform', required=True)
        command_parser.add_argument('--dist', type=Path, required=True)
        command_parser.add_argument('--repo', default=os.environ.get('GITHUB_REPOSITORY'))
    args: argparse.Namespace = parser.parse_args(argv)
    if not args.repo:
        parser.error(args.command + ' needs --repo or GITHUB_REPOSITORY')
    runs: GitHubRuns = GitHubRuns(args.repo)
    run: int | None
    if args.command == 'find':
        run = find(args.platform, args.dist, checkout_field('%T'), runs)
    else:
        run = merged(args.platform, args.dist, checkout_field('%H'), checkout_field('%T'), runs)
    write_output('reused', 'true' if run is not None else 'false')
    write_output('run', str(run) if run is not None else '')
    return 0


if __name__ == '__main__':
    sys.exit(main())
