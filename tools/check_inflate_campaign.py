#!/usr/bin/env python3
"""Account only complete, unique, matching inflate shards with intact evidence."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

from run_inflate_campaign import PLAN, corpus_hashes, digest, reported_fuzz_time


def inspect(plan: dict, plan_digest: str, directories: list[Path]) -> dict:
    expected = {(architecture, mode, shard) for architecture in plan['architectures']
                for mode in plan['oracle_modes'] for shard in range(plan['shards_per_configuration'])}
    observed = set()
    totals = {}
    for directory in directories:
        record = json.loads((directory / 'run.json').read_text())
        key = (record.get('architecture'), record.get('oracle_mode'), record.get('shard'))
        if key not in expected or key in observed:
            raise RuntimeError('Unexpected or duplicate shard: ' + str(key))
        observed.add(key)
        if record.get('state') != 'completed' or record.get('smoke') is not False:
            raise RuntimeError('Shard is not a completed campaign run: ' + str(key))
        if record.get('campaign') != plan['campaign'] or record.get('plan_sha256') != plan_digest:
            raise RuntimeError('Campaign identity mismatch: ' + str(key))
        if record.get('source_sha256') != plan['source_sha256']:
            raise RuntimeError('Source mismatch: ' + str(key))
        duration = plan['seconds_per_shard']
        if record.get('qualifying_fuzz_seconds') != duration or record.get('requested_fuzz_seconds') != duration:
            raise RuntimeError('Unqualified duration: ' + str(key))
        operations = record.get('operations', [])
        if not operations or any(item.get('state') != 'passed' or item.get('exit_code') != 0 for item in operations):
            raise RuntimeError('Missing or failed operations: ' + str(key))
        evidence = record.get('evidence_sha256', {})
        required = {'tests.log', 'merge.log', 'fuzz.log', 'replay.log', 'coverage.json', 'coverage.txt'}
        if set(evidence) != required:
            raise RuntimeError('Incomplete evidence inventory: ' + str(key))
        for name, expected_digest in evidence.items():
            if digest(directory / name) != expected_digest:
                raise RuntimeError('Evidence hash mismatch: ' + name)
        executions, seconds = reported_fuzz_time((directory / 'fuzz.log').read_text())
        if seconds < duration or seconds != record.get('fuzz_reported_seconds') or executions != record.get('fuzz_executions'):
            raise RuntimeError('Fuzzer completion mismatch: ' + str(key))
        if not record.get('final_corpus_sha256') or corpus_hashes(directory / 'corpus') != record['final_corpus_sha256']:
            raise RuntimeError('Corpus mismatch: ' + str(key))
        coverage = record.get('candidate_coverage', {})
        if not coverage.get('regions', {}).get('covered') or not coverage.get('branches', {}).get('count'):
            raise RuntimeError('Missing candidate source coverage: ' + str(key))
        exported = json.loads((directory / 'coverage.json').read_text())
        files = exported['data'][0]['files']
        candidate = [item for item in files if item['filename'].replace('\\', '/').endswith('/src/core/compression/inflate.cpp')]
        if len(candidate) != 1 or candidate[0]['summary'] != coverage:
            raise RuntimeError('Candidate coverage export mismatch: ' + str(key))
        name = key[0] + '/' + key[1]
        entry = totals.setdefault(name, {'seconds': 0, 'executions': 0, 'shards': []})
        entry['seconds'] += duration
        entry['executions'] += executions
        entry['shards'].append(key[2])
    missing = sorted(expected - observed)
    return {'campaign': plan['campaign'], 'plan_sha256': plan_digest,
            'state': 'accounted' if not missing else 'incomplete', 'configurations': totals,
            'missing': missing, 'scope': 'Fuzz duration accounting only; coverage review and phase acceptance remain separate.'}


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--evidence', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    plan = json.loads(PLAN.read_text())
    directories = [path.parent for path in sorted(args.evidence.rglob('run.json'))]
    try:
        result = inspect(plan, digest(PLAN), directories)
    except (RuntimeError, KeyError, ValueError, OSError) as error:
        result = {'campaign': plan['campaign'], 'state': 'failed', 'error': str(error)}
    args.output.write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result, indent=2))
    if result['state'] != 'accounted':
        raise SystemExit(1)


if __name__ == '__main__':
    main()
