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
    parser.add_argument('--profile', action='store_true', help='time block-level stages in an instrumented source copy')
    parser.add_argument('--workload', type=int, choices=range(3))
    parser.add_argument('--level', type=int, choices=range(10), default=6)
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
              'no_vector': args.no_vector, 'profile': args.profile, 'lto': False, 'commands': commands,
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
        source = ROOT / 'stx_zlib/src/deflate.cpp'
        if args.profile:
            instrumented = source.read_text()
            stages = ['adler', 'tokenize', 'prepare_dynamic', 'emit_tokens', 'encode_block']
            for index, stage in enumerate(stages):
                start = instrumented.index('DeflateStream::' + stage + '(')
                brace = instrumented.index('{', start)
                instrumented = (instrumented[:brace + 1] +
                                f'\n    stx_profile::Scope profile_scope{{{index}}};' + instrumented[brace + 1:])
            prefix = '''#include <chrono>
#include <cstdio>
#include <cstdint>
#include <array>
namespace stx_profile {
struct Totals final {
    std::array<std::uint64_t, 5> ns{};
    ~Totals() {
        constexpr std::array<const char*, 5> names{"adler", "tokenize", "dynamic_tree", "emit_tokens", "block_inclusive"};
        for (unsigned index = 0; index < 5; ++index)
            std::fprintf(stderr, "%s,%llu\\n", names[index], static_cast<unsigned long long>(ns[index]));
    }
};
Totals totals{};
struct Scope final {
    unsigned stage = 0;
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    ~Scope() {
        const std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        const std::chrono::nanoseconds elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
        totals.ns[stage] += static_cast<std::uint64_t>(elapsed.count());
    }
};
}
'''
            source = output / 'profile-deflate.cpp'
            source.write_text(prefix + instrumented)
        common = ['clang++', *cxx_flags, *flags, *extra, '-Wall', '-Wextra', '-Wpedantic',
                  '-I' + str(ROOT / 'stx_zlib/include'), '-I' + str(oracle)]
        executable = str(output / 'benchmark')
        command = [*common, '-Rpass=loop-vectorize', '-Rpass-missed=loop-vectorize',
                   str(source), str(ROOT / 'stx_zlib/bench/deflate.cpp'),
                   *objects, *link_flags, '-o', executable]
        commands.append(command)
        with (output / 'compiler.log').open('w') as log:
            subprocess.run(command, stdout=log, stderr=subprocess.STDOUT, check=True)
        command = [*common, '-S', str(ROOT / 'stx_zlib/src/deflate.cpp'), '-o', str(output / 'candidate.s')]
        commands.append(command)
        subprocess.run(command, check=True)
        command = [executable]
        if args.workload is not None:
            command += [str(args.workload), str(args.level)]
        commands.append(command)
        with (output / 'benchmark.csv').open('w') as log, (output / 'stages.csv').open('w') as stages_log:
            subprocess.run(command, cwd=output, stdout=log, stderr=stages_log, check=True)
        print((output / 'benchmark.csv').read_text())
        if args.profile:
            print((output / 'stages.csv').read_text())
    finally:
        (output / 'run.json').write_text(json.dumps(record, indent=2) + '\n')


if __name__ == '__main__':
    main()
