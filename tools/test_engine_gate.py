#!/usr/bin/env python3
"""Tests for engine_gate.py: reading manifests, comparing hosts and filling the record."""
from __future__ import annotations
import json
from pathlib import Path
import tempfile
import unittest

import engine_gate


def manifest_text(rows: list[str], counters: list[str] | None = None, registry: int = 2) -> str:
    lines: list[str] = [engine_gate.HEADER, '# primitive\tname\tstate\tscale\trasters\tpainter_calls\trefusals\thash',
                        'registry\t' + str(registry)]
    lines.extend(rows)
    lines.extend(counters or ['counter\tribbon.idle_frames_run\t0'])
    return '\n'.join(lines) + '\n'


BULGE: str = 'primitive\tbulge\trest\t1.00\t1\t51\t0\t58212a64ba6d4616'
GLASS: str = 'primitive\tglass\trest\t1.00\t1\t51\t0\t8002a0c7d2724c39'


def write_evidence(root: Path, manifests: dict[str, str]) -> None:
    """Lays manifests out as the downloaded evidence artifacts are: one directory per artifact."""
    platform: str
    text: str
    for platform, text in manifests.items():
        directory: Path = root / ('gui-forms-evidence-' + platform) / '.ci'
        directory.mkdir(parents=True)
        (directory / ('engine-gate-' + platform + '.tsv')).write_text(text, encoding='utf-8')
        (directory / 'inputs.json').write_text('{}', encoding='utf-8')


class ParseTests(unittest.TestCase):
    def test_rows_and_counters_are_read(self) -> None:
        manifest: engine_gate.Manifest = engine_gate.parse_manifest(manifest_text([BULGE, GLASS]))
        self.assertEqual(manifest.registry, 2)
        self.assertEqual(manifest.images[('bulge', 'rest', '1.00')], ('1', '51', '0', '58212a64ba6d4616'))
        self.assertEqual(manifest.counters, {'ribbon.idle_frames_run': 0})

    def test_a_foreign_file_or_a_malformed_row_is_refused(self) -> None:
        with self.assertRaises(ValueError):
            engine_gate.parse_manifest('not a manifest\n')
        with self.assertRaises(ValueError):
            engine_gate.parse_manifest(manifest_text(['primitive\tbulge\trest']))
        with self.assertRaises(ValueError):
            engine_gate.parse_manifest(manifest_text([BULGE, BULGE]))


class CompareTests(unittest.TestCase):
    def run_compare(self, manifests: dict[str, str], require_all: bool = False) -> tuple[int, dict, str]:
        with tempfile.TemporaryDirectory() as scratch:
            root: Path = Path(scratch)
            write_evidence(root / 'evidence', manifests)
            arguments: list[str] = ['compare', '--evidence', str(root / 'evidence'), '--out', str(root / 'out'),
                                    '--commit', 'abc123', '--run', 'https://example.invalid/run/1']
            if require_all:
                arguments.append('--require-all')
            status: int = engine_gate.main(arguments)
            result: dict = json.loads((root / 'out' / 'comparison.json').read_text(encoding='utf-8'))
            record: str = (root / 'out' / 'record.md').read_text(encoding='utf-8')
        return status, result, record

    def test_identical_hosts_pass_and_fill_the_record(self) -> None:
        text: str = manifest_text([BULGE, GLASS])
        manifests: dict[str, str] = {}
        platform: str
        for platform in engine_gate.PLATFORMS:
            manifests[platform] = text
        status: int
        result: dict
        record: str
        status, result, record = self.run_compare(manifests, require_all=True)
        self.assertEqual(status, 0)
        self.assertTrue(result['identical'])
        self.assertEqual(result['primitives'], ['bulge', 'glass'])
        self.assertEqual(result['missing'], [])
        self.assertIn('`abc123`', record)
        self.assertIn('| linux-arm64 | 2 | 2 | yes |', record)
        self.assertNotIn('{{', record)

    def test_one_differing_hash_fails_and_names_the_host(self) -> None:
        changed: str = GLASS.replace('8002a0c7d2724c39', '0000000000000001')
        status: int
        result: dict
        record: str
        status, result, record = self.run_compare({
            'windows-x64': manifest_text([BULGE, GLASS]), 'linux-x64': manifest_text([BULGE, GLASS]),
            'macos-arm64': manifest_text([BULGE, changed])})
        self.assertEqual(status, 1)
        self.assertEqual(len(result['differences']), 1)
        self.assertEqual(result['differences'][0]['primitive'], 'glass')
        self.assertIn('| macos-arm64 | 2 | 2 | no (see differences) |', record)
        self.assertIn('| windows-x64 | 2 | 2 | yes |', record)

    def test_a_primitive_missing_on_one_host_is_a_difference(self) -> None:
        status: int
        result: dict
        status, result, _ = self.run_compare({
            'windows-x64': manifest_text([BULGE, GLASS]), 'linux-x64': manifest_text([BULGE], registry=1)})
        self.assertEqual(status, 1)
        self.assertFalse(result['registry_agrees'])
        self.assertEqual(result['differences'][0]['hosts']['linux-x64'], None)

    def test_a_busy_idle_counter_fails(self) -> None:
        busy: str = manifest_text([BULGE, GLASS], ['counter\tprimitive.glass.idle_frames_run\t3'])
        status: int
        result: dict
        status, result, _ = self.run_compare({'windows-x64': busy})
        self.assertEqual(status, 1)
        self.assertEqual(result['busy_idle_counters'][0]['counter'], 'primitive.glass.idle_frames_run')

    def test_missing_hosts_fail_only_when_all_are_required(self) -> None:
        manifests: dict[str, str] = {'windows-x64': manifest_text([BULGE, GLASS])}
        status: int
        result: dict
        record: str
        status, result, record = self.run_compare(manifests)
        self.assertEqual(status, 0)
        self.assertEqual(result['missing'], ['macos-arm64', 'linux-x64', 'linux-arm64'])
        self.assertIn('no manifest', record)
        status, result, record = self.run_compare(manifests, require_all=True)
        self.assertEqual(status, 1)

    def test_two_manifests_for_one_platform_are_refused(self) -> None:
        with tempfile.TemporaryDirectory() as scratch:
            root: Path = Path(scratch)
            write_evidence(root / 'a', {'linux-x64': manifest_text([BULGE])})
            write_evidence(root / 'b', {'linux-x64': manifest_text([BULGE])})
            with self.assertRaises(ValueError):
                engine_gate.find_manifests(root)

    def test_the_template_names_every_placeholder_the_tool_fills(self) -> None:
        template: str = engine_gate.TEMPLATE.read_text(encoding='utf-8')
        name: str
        for name in ('commit', 'run', 'verdict', 'primitives', 'image_count', 'host_table', 'differences',
                     'counter_table'):
            self.assertIn('{{' + name + '}}', template)


if __name__ == '__main__':
    unittest.main()
