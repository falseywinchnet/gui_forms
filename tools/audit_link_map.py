#!/usr/bin/env python3
"""Report which compression and PNG implementations a linked binary carries.

Reads each binary's symbol table with llvm-nm (or nm) and classifies:
  owned     stx_zlib / stx_png symbols (C++ names containing those namespaces);
  zlib      a C zlib: inflate, deflate, adler32, crc32 and their internals, or
            Chromium's Cr_z_-prefixed copy;
  freetype  FreeType's bundled gzip inflater (its internal ft_gzip_* functions;
            the public FT_Gzip_Uncompress and FT_Stream_OpenGzip remain as
            stubs returning Unimplemented_Feature when it is compiled out);
  libpng    png_* entry points.
With --forbid-foreign the exit status is 1 when any zlib, freetype or libpng
symbol is present, so CI can prove one implementation. Without it the
report is informational (the state before stage I5 removes them).
"""
from __future__ import annotations

import argparse
import re
import shutil
import subprocess
import sys
from pathlib import Path

ZLIB_NAMES: frozenset[str] = frozenset({
    'inflate', 'inflateInit_', 'inflateInit2_', 'inflateEnd', 'inflateReset', 'inflate_fast',
    'inflate_table', 'deflate', 'deflateInit_', 'deflateInit2_', 'deflateEnd', 'deflateBound',
    'adler32', 'adler32_z', 'crc32', 'crc32_z', 'zcalloc', 'zcfree', 'zlibVersion', 'compress',
    'compress2', 'uncompress', 'uncompress2', '_tr_init', '_tr_flush_block', '_tr_stored_block'})
LIBPNG_PATTERN: re.Pattern[str] = re.compile(r'^png_(create|read|write|set|get|destroy|init)_')
FREETYPE_PATTERN: re.Pattern[str] = re.compile(r'^ft_gzip_')


def symbol_names(path: Path) -> list[str]:
    tool: str | None = shutil.which('llvm-nm') or shutil.which('nm')
    if tool is None:
        raise RuntimeError('neither llvm-nm nor nm is on PATH')
    result: subprocess.CompletedProcess[str] = subprocess.run(
        [tool, '--defined-only', str(path)], text=True, capture_output=True, check=False)
    if result.returncode != 0:
        raise RuntimeError(f'{tool} failed on {path}: {result.stderr.strip()}')
    names: list[str] = []
    line: str
    for line in result.stdout.splitlines():
        fields: list[str] = line.split()
        if len(fields) >= 3:
            names.append(fields[2])
    return names


def classify(names: list[str]) -> dict[str, set[str]]:
    found: dict[str, set[str]] = {'owned': set(), 'zlib': set(), 'freetype': set(), 'libpng': set()}
    name: str
    for name in names:
        # Mach-O prefixes C symbols with an underscore.
        plain: str = name[1:] if name.startswith('_') and name[1:2].isalpha() else name
        if 'stx_zlib' in plain or 'stx_png' in plain:
            found['owned'].add(plain)
        elif plain in ZLIB_NAMES or plain.startswith('Cr_z_'):
            found['zlib'].add(plain)
        elif FREETYPE_PATTERN.match(plain):
            found['freetype'].add(plain)
        elif LIBPNG_PATTERN.match(plain):
            found['libpng'].add(plain)
    return found


def main() -> int:
    parser: argparse.ArgumentParser = argparse.ArgumentParser(description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('binaries', nargs='+', type=Path)
    parser.add_argument('--forbid-foreign', action='store_true',
                        help='fail when a zlib, FreeType gzip or libpng symbol is present')
    args: argparse.Namespace = parser.parse_args()
    failed: bool = False
    path: Path
    for path in args.binaries:
        found: dict[str, set[str]] = classify(symbol_names(path))
        print(f'{path}:')
        kind: str
        for kind in ('owned', 'zlib', 'freetype', 'libpng'):
            names: list[str] = sorted(found[kind])
            # Owned symbols are counted; foreign ones are named, up to six.
            sample: str = ''
            if kind != 'owned':
                sample = ', '.join(names[:6]) + (' ...' if len(names) > 6 else '')
            print(f'  {kind:9} {len(names):5}  {sample}')
        foreign: int = len(found['zlib']) + len(found['freetype']) + len(found['libpng'])
        if args.forbid_foreign and foreign != 0:
            failed = True
    if failed:
        print('foreign compression or PNG code is linked', file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
