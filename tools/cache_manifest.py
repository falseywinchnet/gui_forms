#!/usr/bin/env python3
"""Describe a successful cache producer; this is not a consumer test receipt."""
import json
import os
from pathlib import Path
import subprocess
import sys


def main() -> None:
    source: Path = Path(__file__).resolve().parents[1]
    revision_text: str = subprocess.check_output(
        ['git', '-C', str(source), 'rev-parse', 'HEAD'], text=True)
    revision: str = revision_text.strip()
    manifest: dict[str, object] = {
        'schema': 1, 'repository': 'falseywinchnet/gui_forms',
        'revision': revision, 'platform': sys.argv[1], 'compiler': sys.argv[2],
        'run_id': os.environ.get('GITHUB_RUN_ID', ''),
        'profile': 'native Release plus separately tested development text/host profiles',
        'validation': 'provider CTest passed; consumer compilation and tests remain required'}
    destination: Path = source.parent / 'cache-manifest.json'
    destination.write_text(json.dumps(manifest, indent=2) + '\n', encoding='utf-8')


if __name__ == '__main__':
    main()
