"""Exercise campaign failure accounting and process supervision without a build."""
from __future__ import annotations
import json
import os
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

import check_inflate_campaign as accounting
import run_inflate_campaign as campaign


class CampaignTests(unittest.TestCase):
    def test_rss_includes_descendants_in_any_order(self):
        table = '40 30 4\n99 1 1000\n20 10 2\n30 20 3\n10 1 1\n'
        self.assertEqual(campaign.tree_rss(table, 10), 10 * 1024)
        self.assertEqual(campaign.tree_rss(table, 999), 0)
        with self.assertRaises((RuntimeError, ValueError)):
            campaign.tree_rss('10 1 bad', 10)

    def test_completion_requires_one_real_completion_line(self):
        self.assertEqual(campaign.reported_fuzz_time('Done 123 runs in 61 second(s)\n'), (123, 61))
        for text in ('', 'Done 123 runs', 'prefix Done 1 runs in 1 second(s)',
                     'Done 1 runs in 1 second(s)\nDone 2 runs in 2 second(s)'):
            with self.assertRaises(RuntimeError):
                campaign.reported_fuzz_time(text)

    def test_source_changes_are_rejected(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            source = root / 'source.cpp'
            source.write_text('first')
            plan = {'source_sha256': {'source.cpp': campaign.digest(source)}}
            self.assertEqual(campaign.verify_source_lock(plan, root), plan['source_sha256'])
            source.write_text('second')
            with self.assertRaises(RuntimeError):
                campaign.verify_source_lock(plan, root)
            with self.assertRaises(RuntimeError):
                campaign.verify_source_lock({'source_sha256': {}}, root)

    def exercise_process(self, command, expected_error=None, seconds=10, rss=2**40):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            record = {'operations': []}
            if expected_error:
                with self.assertRaises(expected_error):
                    campaign.supervised(command, root / 'log', root, dict(os.environ), seconds, rss, root / 'run.json', record)
            else:
                campaign.supervised(command, root / 'log', root, dict(os.environ), seconds, rss, root / 'run.json', record)
            saved = json.loads((root / 'run.json').read_text())
            operation = saved['operations'][0]
            self.assertEqual(operation['state'], 'failed' if expected_error else 'passed')
            self.assertIn('elapsed_seconds', operation)
            return operation

    def test_process_success_failure_and_missing_executable(self):
        self.assertEqual(self.exercise_process([sys.executable, '-c', 'print(123)'])['exit_code'], 0)
        self.assertEqual(self.exercise_process([sys.executable, '-c', 'raise SystemExit(7)'], RuntimeError)['exit_code'], 7)
        self.assertIsNone(self.exercise_process(['/nonexistent-inflate-campaign-command'], OSError)['exit_code'])

    def test_timeout_terminates_child(self):
        operation = self.exercise_process([sys.executable, '-c', 'import time; time.sleep(30)'], RuntimeError, seconds=0.02)
        self.assertNotEqual(operation['exit_code'], 0)
        self.assertIn('wall-time', operation['error'])

    def test_memory_limit_terminates_child(self):
        with patch.object(campaign, 'resident_bytes', return_value=2048):
            operation = self.exercise_process([sys.executable, '-c', 'import time; time.sleep(30)'], RuntimeError, rss=1024)
        self.assertIn('RSS limit', operation['error'])

    def fixture(self, root):
        plan = {'campaign': 'test', 'architectures': ['x64'], 'oracle_modes': ['ordinary'],
                'shards_per_configuration': 1, 'seconds_per_shard': 10, 'source_sha256': {'test': 'digest'}}
        record = {'campaign': 'test', 'architecture': 'x64', 'oracle_mode': 'ordinary', 'shard': 0,
                  'state': 'completed', 'smoke': False, 'plan_sha256': 'plan', 'source_sha256': plan['source_sha256'],
                  'qualifying_fuzz_seconds': 10, 'requested_fuzz_seconds': 10, 'fuzz_reported_seconds': 11,
                  'fuzz_executions': 100, 'operations': [{'state': 'passed', 'exit_code': 0}],
                  'candidate_coverage': {'regions': {'covered': 1}, 'branches': {'count': 1}}}
        (root / 'corpus').mkdir()
        (root / 'corpus/seed').write_bytes(b'abc')
        record['final_corpus_sha256'] = campaign.corpus_hashes(root / 'corpus')
        for name in ('tests.log', 'merge.log', 'replay.log', 'coverage.json', 'coverage.txt'):
            (root / name).write_text('fixture')
        (root / 'coverage.json').write_text(json.dumps({'data': [{'files': [{'filename': '/repo/src/core/compression/inflate.cpp', 'summary': record['candidate_coverage']}]}]}))
        (root / 'fuzz.log').write_text('Done 100 runs in 11 second(s)\n')
        record['evidence_sha256'] = {p.name: campaign.digest(p) for p in root.iterdir() if p.is_file()}
        (root / 'run.json').write_text(json.dumps(record))
        return plan, record

    def test_accounting_rejects_missing_duplicate_smoke_failed_short_or_changed_runs(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            plan, record = self.fixture(root)
            self.assertEqual(accounting.inspect(plan, 'plan', [root])['state'], 'accounted')
            self.assertEqual(accounting.inspect(plan, 'plan', [])['state'], 'incomplete')
            with self.assertRaises(RuntimeError):
                accounting.inspect(plan, 'plan', [root, root])
            for field, value in [('smoke', True), ('state', 'failed'), ('qualifying_fuzz_seconds', 9),
                                 ('source_sha256', {}), ('plan_sha256', 'different'), ('fuzz_reported_seconds', 9)]:
                changed = dict(record)
                changed[field] = value
                (root / 'run.json').write_text(json.dumps(changed))
                with self.assertRaises(RuntimeError, msg=field):
                    accounting.inspect(plan, 'plan', [root])
            (root / 'run.json').write_text(json.dumps(record))
            (root / 'fuzz.log').write_text('tampered')
            with self.assertRaises(RuntimeError):
                accounting.inspect(plan, 'plan', [root])


if __name__ == '__main__':
    unittest.main()
