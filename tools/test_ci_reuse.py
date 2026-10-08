"""Validation reuse must not turn a stale or untrusted result into a green job."""
from __future__ import annotations
import hashlib
import io
import json
import os
from pathlib import Path
import tempfile
import tarfile
from typing import Any
import unittest
from unittest.mock import patch
import zipfile

import ci_reuse


def fixture_identity() -> dict[str, Any]:
    return {'schema': 1, 'tree': 'tree', 'compiler': 'compiler', 'environment': 'environment',
            'platform': 'linux-x64', 'proof': 'proof', 'revision': 'commit', 'run_id': '10'}


def zipped_receipt(receipt: dict[str, Any]) -> bytes:
    stream: io.BytesIO = io.BytesIO()
    with zipfile.ZipFile(stream, 'w') as archive:
        archive.writestr('receipt.json', json.dumps(receipt))
    return stream.getvalue()


class FixtureAPI:
    def __init__(self) -> None:
        self.identity: dict[str, Any] = fixture_identity()
        self.receipt: dict[str, Any] = self.identity.copy()
        self.receipt.update(artifact_id=9, archive_sha256='a' * 64)
        self.data: bytes = zipped_receipt(self.receipt)
        self.origin: dict[str, Any] = {'id': 10, 'repository_id': 1, 'head_repository_id': 1}
        self.run: dict[str, Any] = {'path': '.github/workflows/native.yml', 'head_sha': 'commit',
            'event': 'pull_request', 'head_repository': {'full_name': 'owner/repo'}}
        self.job: dict[str, Any] = {'name': 'native (ubuntu-24.04, linux-x64, bash)',
            'conclusion': 'success', 'status': 'completed'}
        self.tree: str = 'tree'
        self.expired: bool = False
        self.payload_run: int = 10
        self.calls: list[str] = []
        self.pending: bool = False

    def request(self, endpoint: str) -> Any:
        self.calls.append(endpoint)
        if 'artifacts?per_page=' in endpoint:
            if ('gui-inputs-' in endpoint) != self.pending:
                return {'artifacts': []}
            return {'artifacts': [{'id': 5, 'workflow_run': self.origin,
                'expired': self.expired, 'size_in_bytes': len(self.data),
                'digest': 'sha256:' + hashlib.sha256(self.data).hexdigest()}]}
        if '/git/commits/' in endpoint:
            return {'tree': {'sha': self.tree}}
        if '/jobs?' in endpoint:
            return {'jobs': [self.job]}
        if endpoint.endswith('/runs/10'):
            return self.run
        if endpoint.endswith('/artifacts/9'):
            return {'expired': False, 'workflow_run': {'id': self.payload_run},
                    'name': 'gui-forms-cache-linux-x64'}
        raise AssertionError(endpoint)

    def complete(self, seconds: float) -> None:
        self.pending = False
        self.job['conclusion'] = 'success'
        self.job['status'] = 'completed'


class ReuseTests(unittest.TestCase):
    def lookup(self, fixture: FixtureAPI) -> bool:
        with tempfile.TemporaryDirectory() as directory:
            with patch.dict(os.environ, {'GITHUB_REPOSITORY': 'owner/repo', 'GITHUB_RUN_ID': '20'}), \
                 patch.object(ci_reuse, 'STATE', Path(directory)), \
                 patch.object(ci_reuse, 'api', side_effect=fixture.request), \
                 patch.object(ci_reuse.subprocess, 'check_output', return_value=fixture.data), \
                 patch.object(ci_reuse, 'output'):
                return ci_reuse.lookup(fixture.identity, 0)

    def test_successful_same_tree_other_commit_can_be_reused(self) -> None:
        fixture: FixtureAPI = FixtureAPI()
        fixture.identity['revision'] = 'merge-commit'
        self.assertTrue(self.lookup(fixture))

    def test_changed_receipt_inputs_are_rejected(self) -> None:
        key: str
        for key in ('tree', 'compiler', 'environment', 'platform', 'proof'):
            with self.subTest(key=key):
                identity: dict[str, Any] = fixture_identity()
                receipt: dict[str, Any] = identity.copy()
                receipt.update(artifact_id=9, archive_sha256='a' * 64)
                receipt[key] = 'different'
                self.assertFalse(ci_reuse.matching_receipt(receipt, identity, 10))

    def test_changed_actual_tree_is_rejected_even_with_forged_receipt(self) -> None:
        fixture: FixtureAPI = FixtureAPI()
        fixture.tree = 'different'
        self.assertFalse(self.lookup(fixture))

    def test_foreign_workflow_and_fork_are_rejected(self) -> None:
        fixture: FixtureAPI = FixtureAPI()
        fixture.run['path'] = '.github/workflows/other.yml'
        self.assertFalse(self.lookup(fixture))
        fixture = FixtureAPI()
        fixture.origin['head_repository_id'] = 2
        self.assertFalse(self.lookup(fixture))
        fixture = FixtureAPI()
        fixture.run['head_repository']['full_name'] = 'attacker/fork'
        self.assertFalse(self.lookup(fixture))

    def test_failed_pending_and_expired_results_are_rejected(self) -> None:
        conclusion: str | None
        for conclusion in ('failure', 'cancelled', None):
            fixture: FixtureAPI = FixtureAPI()
            fixture.job['conclusion'] = conclusion
            self.assertFalse(self.lookup(fixture))
        fixture = FixtureAPI()
        fixture.expired = True
        self.assertFalse(self.lookup(fixture))

    def test_payload_must_belong_to_validated_run(self) -> None:
        fixture: FixtureAPI = FixtureAPI()
        fixture.payload_run = 99
        self.assertFalse(self.lookup(fixture))

    def test_no_wait_cycle_or_self_reuse(self) -> None:
        producer: int
        for producer in (20, 21):
            fixture: FixtureAPI = FixtureAPI()
            fixture.origin['id'] = producer
            self.assertFalse(self.lookup(fixture))
            self.assertFalse(any('/runs/' in endpoint for endpoint in fixture.calls))

    def test_corrupted_receipt_zip_is_rejected(self) -> None:
        data: bytes = zipped_receipt(fixture_identity())
        with self.assertRaisesRegex(ValueError, 'digest mismatch'):
            ci_reuse.receipt_from_zip(data, 'sha256:' + '0' * 64)

    def test_waits_for_older_identical_job_then_reuses_it(self) -> None:
        fixture: FixtureAPI = FixtureAPI()
        fixture.pending = True
        fixture.job.update(status='in_progress', conclusion=None)
        with tempfile.TemporaryDirectory() as directory:
            with patch.dict(os.environ, {'GITHUB_REPOSITORY': 'owner/repo', 'GITHUB_RUN_ID': '20'}), \
                 patch.object(ci_reuse, 'STATE', Path(directory)), \
                 patch.object(ci_reuse, 'api', side_effect=fixture.request), \
                 patch.object(ci_reuse.subprocess, 'check_output', return_value=fixture.data), \
                 patch.object(ci_reuse, 'output'), \
                 patch.object(ci_reuse.time, 'sleep', side_effect=fixture.complete) as sleeper:
                self.assertTrue(ci_reuse.lookup(fixture.identity, 60))
                sleeper.assert_called_once()

    def test_bounded_receipt_read(self) -> None:
        data: bytes = zipped_receipt({'oversized': 'x' * (256 * 1024)})
        with self.assertRaisesRegex(ValueError, 'bound'):
            ci_reuse.receipt_from_zip(data, 'sha256:' + hashlib.sha256(data).hexdigest())

    def test_proof_changes_for_mechanism_but_not_ordinary_source(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root: Path = Path(directory)
            (root / 'CMakeLists.txt').write_text('contract')
            original: str = ci_reuse.proof_contract(root)
            (root / 'ordinary.cpp').write_text('implementation')
            self.assertEqual(ci_reuse.proof_contract(root), original)
            (root / 'CMakeLists.txt').write_text('changed contract')
            self.assertNotEqual(ci_reuse.proof_contract(root), original)

    def test_archive_restore_checks_digest_manifest_and_paths(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root: Path = Path(directory)
            state: Path = root / '.ci'
            (state / 'download').mkdir(parents=True)
            archive: Path = state / 'download/gui-forms-cache-linux-x64.tar.gz'
            manifest: dict[str, str] = {'revision': 'commit', 'compiler': 'compiler', 'platform': 'linux-x64'}
            data: bytes = json.dumps(manifest).encode()
            entry: tarfile.TarInfo = tarfile.TarInfo('cache-manifest.json')
            entry.size = len(data)
            with tarfile.open(archive, 'w:gz') as payload:
                payload.addfile(entry, io.BytesIO(data))
            receipt: dict[str, Any] = fixture_identity()
            receipt.update(archive_sha256=hashlib.sha256(archive.read_bytes()).hexdigest(),
                           cache_proof=[{'name': 'proof fixture'}], proof_identity={'proof': 'proof'})
            ci_reuse.write_json(state / 'reused.json', receipt)
            with patch.object(ci_reuse, 'ROOT', root), patch.object(ci_reuse, 'STATE', state):
                ci_reuse.restore_archive(fixture_identity())
                self.assertEqual(json.loads((root / 'cache-manifest.json').read_text()), manifest)
                self.assertTrue((state / 'proof/receipt.json').is_file())
                archive.write_bytes(archive.read_bytes() + b'corrupt')
                with self.assertRaisesRegex(ValueError, 'digest mismatch'):
                    ci_reuse.restore_archive(fixture_identity())
                with tarfile.open(archive, 'w:gz') as payload:
                    entry.name = '../escape'
                    payload.addfile(entry, io.BytesIO(data))
                receipt['archive_sha256'] = hashlib.sha256(archive.read_bytes()).hexdigest()
                ci_reuse.write_json(state / 'reused.json', receipt)
                with self.assertRaisesRegex(ValueError, 'Unexpected cache archive member'):
                    ci_reuse.restore_archive(fixture_identity())


if __name__ == '__main__':
    unittest.main()
