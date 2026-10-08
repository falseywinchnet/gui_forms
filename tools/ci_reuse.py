#!/usr/bin/env python3
"""Reuse successful native validation only for an identical tree and environment."""
from __future__ import annotations
import argparse
import hashlib
import io
import json
import os
from pathlib import Path, PurePosixPath
import subprocess
import tarfile
import time
from typing import Any, BinaryIO, TextIO
import zipfile

SOURCE: Path = Path(__file__).resolve().parents[1]
ROOT: Path = SOURCE.parent
STATE: Path = ROOT / '.ci'


def git_field(format_code: str) -> str:
    # Avoid revision operators containing braces: MSYS2's native-process
    # argument conversion can turn HEAD^{tree} into HEAD^tree.
    return subprocess.check_output(
        ['git', '-C', str(SOURCE), 'show', '-s', '--format=' + format_code, 'HEAD'], text=True).strip()


def file_digest(path: Path) -> str:
    stream: BinaryIO
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def write_json(path: Path, value: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2) + '\n', encoding='utf-8')


def output(name: str, value: str) -> None:
    stream: TextIO
    with Path(os.environ['GITHUB_OUTPUT']).open('a', encoding='utf-8') as stream:
        stream.write(name + '=' + value + '\n')


def api(endpoint: str) -> Any:
    return json.loads(subprocess.check_output(['gh', 'api', endpoint], text=True))


def artifact_name(kind: str, identity: dict[str, Any]) -> str:
    return 'gui-' + kind + '-' + identity['platform'] + '-' + identity['tree'] + '-' + identity['environment']


def trusted_run(run: dict[str, Any], repository: str) -> bool:
    return (run.get('path') == '.github/workflows/native.yml'
            and run.get('head_repository', {}).get('full_name') == repository
            and run.get('event') in ('push', 'pull_request', 'workflow_dispatch'))


def matching_receipt(receipt: dict[str, Any], identity: dict[str, Any], run_id: int) -> bool:
    if receipt.get('schema') != 2 or receipt.get('run_id') != str(run_id):
        return False
    key: str
    for key in ('tree', 'compiler', 'environment', 'platform'):
        if receipt.get(key) != identity[key]:
            return False
    digest: Any = receipt.get('archive_sha256')
    artifact_id: Any = receipt.get('artifact_id')
    valid: bool = (isinstance(digest, str) and len(digest) == 64
                   and type(artifact_id) is int and artifact_id > 0)
    return valid


def receipt_from_zip(data: bytes, digest: str) -> dict[str, Any]:
    if 'sha256:' + hashlib.sha256(data).hexdigest() != digest:
        raise ValueError('Validation artifact digest mismatch')
    with zipfile.ZipFile(io.BytesIO(data)) as archive:
        entry: zipfile.ZipInfo = archive.getinfo('receipt.json')
        if entry.file_size > 256 * 1024:
            raise ValueError('Validation receipt exceeds its bound')
        result: Any = json.loads(archive.read(entry))
        if not isinstance(result, dict):
            raise ValueError('Invalid validation receipt')
        return result


def lookup(identity: dict[str, Any], wait_seconds: int) -> bool:
    repository: str = os.environ['GITHUB_REPOSITORY']
    prefix: str = 'repos/' + repository + '/actions/'
    current_run: int = int(os.environ['GITHUB_RUN_ID'])
    deadline: float = time.monotonic() + wait_seconds
    while True:
        pending: bool = False
        kind: str
        for kind in ('validated', 'inputs'):
            listing: dict[str, Any] = api(prefix + 'artifacts?per_page=30&name=' + artifact_name(kind, identity))
            artifact: dict[str, Any]
            for artifact in listing['artifacts']:
                producer: int = int(artifact['workflow_run']['id'])
                # Older producers only: simultaneous PR/merge jobs cannot wait
                # on each other. Rerunning the same run performs fresh validation.
                if producer >= current_run or artifact['expired']:
                    continue
                run: dict[str, Any] = api(prefix + 'runs/' + str(producer))
                if not trusted_run(run, repository):
                    continue
                origin: dict[str, Any] = artifact['workflow_run']
                if origin['head_repository_id'] != origin['repository_id']:
                    continue
                commit: dict[str, Any] = api('repos/' + repository + '/git/commits/' + run['head_sha'])
                if commit['tree']['sha'] != identity['tree']:
                    continue
                jobs: list[dict[str, Any]] = api(prefix + 'runs/' + str(producer) + '/jobs?per_page=100')['jobs']
                native: list[dict[str, Any]] = []
                job: dict[str, Any]
                for job in jobs:
                    if job['name'].startswith('native (') and ', ' + identity['platform'] + ',' in job['name']:
                        native.append(job)
                if len(native) != 1:
                    continue
                if kind == 'inputs':
                    pending = pending or native[0]['status'] in ('queued', 'in_progress')
                    continue
                if native[0]['conclusion'] != 'success' or artifact['size_in_bytes'] > 256 * 1024:
                    continue
                data: bytes = subprocess.check_output(
                    ['gh', 'api', prefix + 'artifacts/' + str(artifact['id']) + '/zip', '--allow-escape-sequences'])
                receipt: dict[str, Any] = receipt_from_zip(data, artifact.get('digest', ''))
                if not matching_receipt(receipt, identity, producer):
                    continue
                payload: dict[str, Any] = api(prefix + 'artifacts/' + str(receipt['artifact_id']))
                if (payload['expired'] or payload['workflow_run']['id'] != producer
                        or payload['name'] != 'gui-forms-cache-' + identity['platform']):
                    continue
                write_json(STATE / 'reused.json', receipt)
                output('artifact-id', str(receipt['artifact_id']))
                output('producer-run', str(producer))
                print('Reusing successful native validation from run ' + str(producer), flush=True)
                return True
        if not pending or time.monotonic() >= deadline:
            return False
        print('Waiting for the older identical-tree/environment job instead of recompiling it', flush=True)
        time.sleep(min(30, max(0, deadline - time.monotonic())))


def restore_archive(identity: dict[str, Any]) -> None:
    receipt: dict[str, Any] = json.loads((STATE / 'reused.json').read_text())
    archive: Path = STATE / 'download' / ('gui-forms-cache-' + identity['platform'] + '.tar.gz')
    if file_digest(archive) != receipt['archive_sha256']:
        raise ValueError('Reused cache archive digest mismatch')
    with tarfile.open(archive) as payload:
        entry: tarfile.TarInfo
        for entry in payload.getmembers():
            name: PurePosixPath = PurePosixPath(entry.name)
            if (not name.parts or name.is_absolute() or '..' in name.parts or not
                (name.parts[0] == '.ccache' or entry.name == 'cache-manifest.json'
                 or entry.name == '.build/toolchain/llvm-22.1.8-macos14'
                 or entry.name.startswith('.build/toolchain/llvm-22.1.8-macos14/'))):
                raise ValueError('Unexpected cache archive member: ' + entry.name)
        payload.extractall(ROOT, filter='data')
    manifest: dict[str, Any] = json.loads((ROOT / 'cache-manifest.json').read_text())
    if (manifest['revision'] != receipt['revision'] or manifest['compiler'] != identity['compiler']
            or manifest['platform'] != identity['platform']):
        raise ValueError('Cache manifest does not match successful validation')
    if identity['platform'] == 'macos-arm64':
        from macos_runtime_manifest import verify
        verify(Path(os.environ['GUI_FORMS_LLVM_RUNTIME']))


def main() -> None:
    parser: argparse.ArgumentParser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('command', choices=('identity', 'lookup', 'restore', 'record'))
    parser.add_argument('--platform', default=os.environ.get('GUI_FORMS_PLATFORM'))
    parser.add_argument('--compiler', default=os.environ.get('GUI_FORMS_COMPILER_ID'))
    parser.add_argument('--environment')
    parser.add_argument('--artifact-id', type=int)
    parser.add_argument('--wait-seconds', type=int, default=1800)
    args: argparse.Namespace = parser.parse_args()
    if args.command == 'identity':
        identity: dict[str, Any] = {'schema': 2, 'tree': git_field('%T'), 'revision': git_field('%H'),
            'platform': args.platform, 'compiler': args.compiler, 'environment': args.environment,
            'run_id': os.environ['GITHUB_RUN_ID']}
        write_json(STATE / 'inputs.json', identity)
        output('tree', identity['tree'])
        output('inputs-name', artifact_name('inputs', identity))
        output('receipt-name', artifact_name('validated', identity))
        return
    identity = json.loads((STATE / 'inputs.json').read_text())
    if args.command == 'lookup':
        reused: bool = False
        try:
            reused = lookup(identity, args.wait_seconds)
        except (subprocess.CalledProcessError, ValueError, KeyError, OSError, zipfile.BadZipFile) as error:
            print('Receipt unavailable or rejected; native validation will run: ' + str(error), flush=True)
        output('reused', str(reused).lower())
    elif args.command == 'restore':
        restore_archive(identity)
    elif args.command == 'record':
        receipt: dict[str, Any] = identity.copy()
        receipt['artifact_id'] = args.artifact_id
        archive: Path = ROOT / ('gui-forms-cache-' + identity['platform'] + '.tar.gz')
        receipt['archive_sha256'] = file_digest(archive)
        receipt['validation_run'] = identity['run_id']
        receipt['validation_revision'] = identity['revision']
        if (STATE / 'reused.json').exists():
            reused_receipt: dict[str, Any] = json.loads((STATE / 'reused.json').read_text())
            receipt['validation_run'] = reused_receipt['validation_run']
            receipt['validation_revision'] = reused_receipt['validation_revision']
        write_json(STATE / 'receipt.json', receipt)
        output('receipt-name', artifact_name('validated', identity))
        summary: str = 'Fresh native validation passed.'
        if (STATE / 'reused.json').exists():
            summary = 'Reused successful native validation; no compilation or native tests repeated.'
        if (STATE / 'promotion.json').exists():
            summary = 'Promoted tested bytes; compiler/runtime/environment identity remains that of the validating producer.'
        summary += '\n\nTree: `' + identity['tree'] + '`; validation run: ' + receipt['validation_run']
        summary += '; validated source: `' + receipt['validation_revision'] + '`.\n'
        stream: TextIO
        with Path(os.environ['GITHUB_STEP_SUMMARY']).open('a', encoding='utf-8') as stream:
            stream.write(summary)


if __name__ == '__main__':
    main()
