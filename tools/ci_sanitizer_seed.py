#!/usr/bin/env python3
"""Seed the sanitizer job's compiler cache from the newest run of the nearest branch.

Actions caches are scoped to a branch: a pull request sees only main's and its
own, and a push to main sees only main's. So the sanitizer job on main could
not reuse the objects of the pull request that tested the very tree being
merged, and a stacked pull request could not reuse its base's. Each sanitizer
job therefore uploads its .ccache as an artifact, and the next job seeds from
the newest one produced for the nearest branch:

- a pull request: its own head branch (an earlier push), then its base branch
  when that is not main (the pull request it is stacked on);
- a push to main: the head branches of the pull requests merged into it.

Without such an artifact the job keeps main's Actions cache. A seed is only a
compiler cache: ccache content-checks every hit, and every test still runs.
"""
from __future__ import annotations
import argparse
import json
import os
from pathlib import Path, PurePosixPath
import shutil
import subprocess
import tarfile
from typing import Any, TextIO

ROOT: Path = Path(__file__).resolve().parents[2]
ARTIFACT_NAME: str = 'gui-sanitize-ccache-linux-x64'
ARCHIVE_NAME: str = 'gui-sanitize-ccache.tar'
DOWNLOAD: Path = ROOT / '.ci' / 'sanitizer-seed'


def api(endpoint: str) -> Any:
    return json.loads(subprocess.check_output(['gh', 'api', endpoint], text=True))


def output(name: str, value: str) -> None:
    stream: TextIO
    with Path(os.environ['GITHUB_OUTPUT']).open('a', encoding='utf-8') as stream:
        stream.write(name + '=' + value + '\n')


def candidate_branches(event: str, head_ref: str, base_ref: str, merged_heads: list[str]) -> list[str]:
    """The branches whose sanitizer objects are nearest this run, nearest first."""
    branches: list[str] = []
    if event == 'pull_request':
        if head_ref:
            branches.append(head_ref)
        if base_ref and base_ref != 'main' and base_ref not in branches:
            branches.append(base_ref)
        return branches
    head: str
    for head in merged_heads:
        if head and head != 'main' and head not in branches:
            branches.append(head)
    return branches


def choose_artifact(artifacts: list[dict[str, Any]], branch: str, current_run: int) -> dict[str, Any] | None:
    """The newest unexpired seed artifact an earlier run of this repository made for branch.

    Artifacts from forks (head repository differs) and from this or later
    runs are refused.
    """
    chosen: dict[str, Any] | None = None
    artifact: dict[str, Any]
    for artifact in artifacts:
        if artifact.get('name') != ARTIFACT_NAME or artifact.get('expired'):
            continue
        origin: dict[str, Any] = artifact.get('workflow_run') or {}
        if origin.get('head_branch') != branch:
            continue
        if origin.get('head_repository_id') != origin.get('repository_id'):
            continue
        if int(origin.get('id', current_run)) >= current_run:
            continue
        if chosen is None or int(origin['id']) > int(chosen['workflow_run']['id']):
            chosen = artifact
    return chosen


def merged_head_branches(repository: str, revision: str) -> list[str]:
    """Head branches of the pull requests GitHub associates with a commit on main."""
    pulls: list[dict[str, Any]] = api('repos/' + repository + '/commits/' + revision + '/pulls')
    heads: list[str] = []
    pull: dict[str, Any]
    for pull in pulls:
        head: dict[str, Any] = pull.get('head') or {}
        if pull.get('merged_at') and (head.get('repo') or {}).get('full_name') == repository:
            heads.append(str(head.get('ref', '')))
    return heads


def find_seed() -> bool:
    repository: str = os.environ['GITHUB_REPOSITORY']
    event: str = os.environ.get('GITHUB_EVENT_NAME', '')
    current_run: int = int(os.environ['GITHUB_RUN_ID'])
    merged: list[str] = []
    if event == 'push':
        merged = merged_head_branches(repository, os.environ['GITHUB_SHA'])
    branches: list[str] = candidate_branches(event, os.environ.get('GITHUB_HEAD_REF', ''),
                                             os.environ.get('GITHUB_BASE_REF', ''), merged)
    endpoint: str = 'repos/' + repository + '/actions/artifacts?per_page=100&name=' + ARTIFACT_NAME
    artifacts: list[dict[str, Any]] = api(endpoint)['artifacts']
    branch: str
    for branch in branches:
        artifact: dict[str, Any] | None = choose_artifact(artifacts, branch, current_run)
        if artifact is None:
            continue
        run: dict[str, Any] = api('repos/' + repository + '/actions/runs/' + str(artifact['workflow_run']['id']))
        if run.get('path') != '.github/workflows/native.yml':
            continue
        output('artifact-id', str(artifact['id']))
        output('producer-run', str(artifact['workflow_run']['id']))
        print('Seeding sanitizer objects from run ' + str(artifact['workflow_run']['id']) + ' of ' + branch
              + '; every test still runs', flush=True)
        return True
    print('No seed for ' + (', '.join(branches) or 'this run') + '; keeping the restored Actions cache', flush=True)
    return False


def safe_member(name: str) -> bool:
    """Whether a seed archive member stays inside .ccache/."""
    path: PurePosixPath = PurePosixPath(name)
    return bool(path.parts) and not path.is_absolute() and '..' not in path.parts and path.parts[0] == '.ccache'


def restore_seed() -> None:
    """Replace .ccache with the downloaded seed; the archive is checked before anything is removed."""
    archive: Path = DOWNLOAD / ARCHIVE_NAME
    with tarfile.open(archive) as payload:
        entry: tarfile.TarInfo
        for entry in payload.getmembers():
            if not safe_member(entry.name) or not (entry.isfile() or entry.isdir()):
                raise ValueError('Unexpected sanitizer seed member: ' + entry.name)
        shutil.rmtree(ROOT / '.ccache', ignore_errors=True)
        payload.extractall(ROOT, filter='data')
    archive.unlink()


def pack_seed() -> None:
    """Write .ccache to gui-sanitize-ccache.tar at the workspace root (uncompressed: entries already are)."""
    with tarfile.open(ROOT / ARCHIVE_NAME, 'w') as payload:
        payload.add(ROOT / '.ccache', arcname='.ccache')


def main() -> None:
    parser: argparse.ArgumentParser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('command', choices=('find', 'restore', 'pack'))
    args: argparse.Namespace = parser.parse_args()
    if args.command == 'restore':
        restore_seed()
        return
    if args.command == 'pack':
        pack_seed()
        return
    found: bool = False
    try:
        found = find_seed()
    except (subprocess.CalledProcessError, ValueError, KeyError, OSError) as error:
        print('Sanitizer seed unavailable; keeping the restored Actions cache: ' + str(error), flush=True)
    output('seeded', str(found).lower())


if __name__ == '__main__':
    main()
