#!/usr/bin/env python3
"""Build repeatable API-sequence fuzz seeds from the checked-in inflate corpus."""

from __future__ import annotations

import argparse
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / 'fuzz' / 'corpus' / 'inflate'
CONTROL_BYTES = 64


def controls(name: str, schedule: int) -> bytearray:
    result = bytearray(CONTROL_BYTES)
    if name.startswith('zlib') or name.startswith('png'):
        result[0] = 1
    result[1] = 7  # Admit the full 32 KiB history window.
    if schedule == 1:
        # One-byte input/output calls and repeated invalid resets exercise
        # partial symbols without replacing the live stream's state.
        for call in range(26):
            result[12 + 2 * call] = 0x81
            result[13 + 2 * call] = 1
    elif schedule == 2:
        # Declare EOF on a later empty call and use an output quota chosen
        # independently of each call's destination extent.
        result[2] = 0x0a
        result[6] = 32
        for call in range(26):
            result[12 + 2 * call] = 2
            result[13 + 2 * call] = 0 if call % 2 == 0 else 4
    return result


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('directory', type=Path, help='output libFuzzer corpus directory')
    arguments = parser.parse_args()
    destination: Path = arguments.directory
    destination.mkdir(parents=True, exist_ok=True)
    count = 0
    for source in sorted(SOURCE.iterdir()):
        if not source.is_file():
            continue
        payload = source.read_bytes()
        for schedule in range(3):
            seed = controls(source.name, schedule)
            output = destination / f'{source.name}-protocol-{schedule}'
            output.write_bytes(seed + payload)
            count += 1
    print(f'Wrote {count} protocol seeds from {SOURCE}')


if __name__ == '__main__':
    main()
