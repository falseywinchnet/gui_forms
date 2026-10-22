#!/usr/bin/env python3
"""Compare exact match indexes with identical answers and query schedules."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import shutil
import subprocess

from benchmark_deflate import png_filtered

ROOT = Path(__file__).resolve().parents[1]


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--png', type=Path, action='append', default=[])
    parser.add_argument('--png-bytes', type=int, default=131072,
                        help='maximum extracted filter-byte prefix per PNG; recorded explicitly')
    args = parser.parse_args()
    if args.png_bytes < 1 or args.png_bytes > 16 * 1024 * 1024:
        parser.error('--png-bytes must be in 1..16777216')
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    compiler = subprocess.check_output(['clang++', '--version'], text=True)
    if 'clang version 22.1.8' not in compiler:
        raise RuntimeError('LLVM 22.1.8 required')
    flags = ['-std=c++20', '-O3', '-DNDEBUG', '-Wall', '-Wextra', '-Wpedantic', '-Wconversion', '-Wshadow']
    if platform.system() == 'Darwin':
        runtime = Path(os.environ['GUI_FORMS_LLVM_RUNTIME'])
        flags += ['-stdlib=libc++', '-nostdinc++', '-isystem', str(runtime / 'include/c++/v1'),
                  '-L' + str(runtime / 'lib'), '-Wl,-rpath,' + str(runtime / 'lib'), '-lunwind']
    sources = ['stx_zlib/bench/match_index.cpp', 'tools/benchmark_match_index.py', 'tools/benchmark_deflate.py']
    hashes = {}
    for name in sources:
        hashes[name] = digest(ROOT / name)
        destination = output / 'source' / name
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(ROOT / name, destination)
    workloads = []
    arguments = []
    for index, path in enumerate(args.png):
        full = output / f'png-{index}.filtered'
        record = png_filtered(path.resolve(), full)
        prefix = output / f'png-{index}.prefix'
        prefix.write_bytes(full.read_bytes()[:args.png_bytes])
        record['measured_prefix_bytes'] = prefix.stat().st_size
        record['measured_prefix_sha256'] = digest(prefix)
        workloads.append(record)
        arguments.append(str(prefix))
    executable = output / 'match-index'
    build = ['clang++', *flags, str(ROOT / sources[0]), '-o', str(executable)]
    command = [str(executable), *arguments]
    record = {'host': platform.platform(), 'machine': platform.machine(), 'compiler': compiler,
              'source_sha256': hashes, 'workloads': workloads, 'commands': [build, command],
              'samples': 9, 'minimum_sample_seconds': 0.01,
              'hardware_counters': 'not measured', 'production_encoder': 'unchanged'}
    try:
        with (output / 'compiler.log').open('w') as log:
            subprocess.run(build, stdout=log, stderr=subprocess.STDOUT, check=True)
        with (output / 'results.csv').open('w') as csv, (output / 'workspace.txt').open('w') as log:
            completed = subprocess.run(command, stdout=csv, stderr=log)
        record['exit_code'] = completed.returncode
        completed.check_returncode()
        print((output / 'results.csv').read_text(), end='')
    finally:
        (output / 'run.json').write_text(json.dumps(record, indent=2) + '\n')


if __name__ == '__main__':
    main()
