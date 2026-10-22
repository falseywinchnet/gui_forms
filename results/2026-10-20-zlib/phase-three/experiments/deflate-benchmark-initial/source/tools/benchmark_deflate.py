#!/usr/bin/env python3
"""Compare the owned encoder with source-pinned zlib, preserving build evidence."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--oracle', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--no-vector', action='store_true')
    args = parser.parse_args()
    oracle = args.oracle.resolve()
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    manifest = json.loads((ROOT / 'tests/compression/oracle_sha256.json').read_text())
    for name, expected in manifest['files'].items():
        if digest(oracle / name) != expected:
            raise RuntimeError('oracle digest mismatch: ' + name)
    compiler = subprocess.check_output(['clang++', '--version'], text=True)
    if 'clang version 22.1.8' not in compiler:
        raise RuntimeError('LLVM 22.1.8 required')
    flags = ['-O3', '-DNDEBUG']
    cxx_flags = ['-std=c++20']
    link_flags = []
    if platform.system() == 'Darwin':
        runtime = Path(os.environ['GUI_FORMS_LLVM_RUNTIME'])
        cxx_flags += ['-stdlib=libc++', '-nostdinc++', '-isystem', str(runtime / 'include/c++/v1')]
        link_flags += ['-L' + str(runtime / 'lib'), '-Wl,-rpath,' + str(runtime / 'lib'), '-lunwind']
    commands = []
    sources = ['stx_zlib/src/deflate.cpp', 'stx_zlib/include/stx_zlib/deflate.hpp',
               'stx_zlib/include/stx_zlib/inflate.hpp', 'stx_zlib/bench/deflate.cpp',
               'tools/benchmark_deflate.py']
    hashes = {}
    for name in sources:
        hashes[name] = digest(ROOT / name)
        destination = output / 'source' / name
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(ROOT / name, destination)
    record = {'host': platform.platform(), 'machine': platform.machine(), 'compiler': compiler,
              'oracle_revision': manifest['revision'], 'source_sha256': hashes,
              'flags': flags, 'cxx_flags': cxx_flags, 'link_flags': link_flags,
              'no_vector': args.no_vector, 'lto': False, 'commands': commands,
              'sampling': 'calibrate >=25ms, two warmup batches, five samples; reset included; allocation excluded'}
    if platform.system() == 'Darwin':
        record['hardware'] = subprocess.check_output(
            ['/usr/sbin/sysctl', 'machdep.cpu.brand_string', 'hw.memsize'], text=True)
    try:
        objects = []
        for source in ('adler32', 'crc32', 'deflate', 'inflate', 'inffast', 'inftrees', 'trees', 'zutil'):
            target = str(output / (source + '.o'))
            command = ['clang', *flags, '-I' + str(oracle), '-c', str(oracle / (source + '.c')), '-o', target]
            commands.append(command)
            subprocess.run(command, check=True)
            objects.append(target)
        extra = ['-fno-vectorize', '-fno-slp-vectorize'] if args.no_vector else []
        common = ['clang++', *cxx_flags, *flags, *extra, '-Wall', '-Wextra', '-Wpedantic',
                  '-I' + str(ROOT / 'stx_zlib/include'), '-I' + str(oracle)]
        executable = str(output / 'benchmark')
        command = [*common, '-Rpass=loop-vectorize', '-Rpass-missed=loop-vectorize',
                   str(ROOT / 'stx_zlib/src/deflate.cpp'), str(ROOT / 'stx_zlib/bench/deflate.cpp'),
                   *objects, *link_flags, '-o', executable]
        commands.append(command)
        with (output / 'compiler.log').open('w') as log:
            subprocess.run(command, stdout=log, stderr=subprocess.STDOUT, check=True)
        command = [*common, '-S', str(ROOT / 'stx_zlib/src/deflate.cpp'), '-o', str(output / 'candidate.s')]
        commands.append(command)
        subprocess.run(command, check=True)
        commands.append([executable])
        with (output / 'benchmark.csv').open('w') as log:
            subprocess.run([executable], cwd=output, stdout=log, check=True)
        print((output / 'benchmark.csv').read_text())
    finally:
        (output / 'run.json').write_text(json.dumps(record, indent=2) + '\n')


if __name__ == '__main__':
    main()
