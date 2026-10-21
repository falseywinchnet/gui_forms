#!/usr/bin/env python3
"""Independently test complete CRC circuits and measure their PNG suitability."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools'))
from validate_png import verify


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--zlib', type=Path, required=True)
    parser.add_argument('--workloads', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--baseline-isa', action='store_true', help='Restrict candidate C++ to baseline Armv8-A NEON on this arm64 host')
    parser.add_argument('--verify-only', action='store_true', help='Run correctness and assembly checks without timing workloads')
    arguments = parser.parse_args()
    output = arguments.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    zlib = arguments.zlib.resolve()
    pin = verify(zlib, ROOT / 'tests/compression/oracle_sha256.json')
    compiler = subprocess.check_output(['clang++', '--version'], text=True)
    if 'clang version 22.1.8' not in compiler:
        raise RuntimeError('LLVM 22.1.8 required')
    if arguments.baseline_isa and platform.machine().lower() not in ('arm64', 'aarch64'):
        raise RuntimeError('--baseline-isa requires an arm64 host')
    candidate_flags = ['-mcpu=generic', '-march=armv8-a'] if arguments.baseline_isa else []
    source = ROOT / 'experiments/png-crc-suitability'
    names = ['compact.cpp', 'compact.hpp', 'crc_kernels.cpp', 'crc_kernels.hpp', 'study.cpp', 'run.py']
    sources = [source / name for name in names]
    sources.extend(ROOT / name for name in ['src/core/image/png_decoder.cpp',
        'src/core/image/png_decoder.hpp', 'src/core/image/png_writer.cpp',
        'stx_zlib/src/inflate.cpp', 'include/gui_forms/compression/inflate.hpp'])
    record = {'compiler': compiler, 'platform': platform.platform(), 'upstream_zlib': pin['revision'],
              'sources': {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest() for p in sources},
              'candidate_flags': candidate_flags, 'verification_only': arguments.verify_only,
              'uptime_start': subprocess.check_output(['uptime'], text=True),
              'commands': [], 'status': 'incomplete', 'timing': '25ms calibration, rotating order, 2 warmup + 7 paired rounds'}
    workloads = output / 'workloads'
    workloads.mkdir(exist_ok=True)
    for path in arguments.workloads.glob('*filter0.png'):
        shutil.copyfile(path, workloads / path.name)
    shutil.copyfile(ROOT / 'assets/fixtures/compression/red-pixel.png', workloads / 'asset-red-pixel.png')
    record['workloads'] = {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in workloads.glob('*.png')}
    includes = ['-I' + str(ROOT / 'include'), '-I' + str(ROOT / 'stx_zlib/include'), '-I' + str(zlib)]
    env = dict(os.environ)
    env['ASAN_OPTIONS'] = 'detect_leaks=1:halt_on_error=1'
    env['UBSAN_OPTIONS'] = 'halt_on_error=1:print_stacktrace=1'
    env.pop('LSAN_OPTIONS', None)
    try:
        if arguments.baseline_isa:
            preflight = output / 'baseline-preflight.s'
            command = ['clang++', '-std=c++20', '-O3', *candidate_flags, *includes,
                       '-S', str(source / 'compact.cpp'), '-o', str(preflight)]
            record['commands'].append(command)
            subprocess.run(command, check=True)
            extra_isa = re.findall(r'^\s*(?:eor3|bcax|crc32\w*|pmull2?)\b.*$',
                                   preflight.read_text(), re.MULTILINE)
            if extra_isa:
                raise RuntimeError('baseline ISA preflight found extra instructions: ' + str(extra_isa[:3]))
            record['baseline_isa_preflight'] = 'No EOR3, BCAX, CRC32 or PMULL instructions in candidate assembly'
        for mode, flags in [('sanitize', ['-O1', '-g', '-fsanitize=address,undefined',
                                         '-fno-sanitize-recover=undefined', '-fno-omit-frame-pointer']),
                            ('optimized', ['-O3'])]:
            oracle = output / ('oracle-' + mode + '.o')
            command = ['clang', *flags, *includes, '-c', str(zlib / 'crc32.c'), '-o', str(oracle)]
            record['commands'].append(command)
            subprocess.run(command, check=True)
            portable_oracle = output / ('oracle-generic-' + mode + '.o')
            # Rename only exported CRC functions; preserve upstream types and
            # disable its architecture CRC opcode branch explicitly.
            symbols = ['crc32_z', 'crc32', 'get_crc_table', 'crc32_combine64',
                       'crc32_combine', 'crc32_combine_gen64', 'crc32_combine_gen', 'crc32_combine_op']
            renames = ['-D' + name + '=portable_' + name for name in symbols]
            command = ['clang', *flags, *includes, '-U__ARM_FEATURE_CRC32', *renames,
                       '-c', str(zlib / 'crc32.c'), '-o', str(portable_oracle)]
            record['commands'].append(command)
            subprocess.run(command, check=True)
            decoder = output / ('decoder-' + mode + '.o')
            command = ['clang++', '-std=c++20', *flags, *candidate_flags, *includes,
                       '-Dpng_crc32=experimental_crc', '-Ddecode_png_samples=crc_decode_png_samples',
                       '-Dinspect_png_samples=crc_inspect_png_samples', '-c',
                       str(ROOT / 'src/core/image/png_decoder.cpp'), '-o', str(decoder)]
            record['commands'].append(command)
            subprocess.run(command, check=True)
            binary = output / ('study-' + mode)
            command = ['clang++', '-std=c++20', *flags, *candidate_flags, *includes, '-fconstexpr-steps=4000000',
                       str(source / 'compact.cpp'), str(source / 'crc_kernels.cpp'), str(source / 'study.cpp'),
                       str(ROOT / 'src/core/image/png_decoder.cpp'), str(ROOT / 'src/core/image/png_writer.cpp'),
                       str(ROOT / 'stx_zlib/src/inflate.cpp'), str(decoder), str(oracle), str(portable_oracle), '-o', str(binary)]
            record['commands'].append(command)
            subprocess.run(command, check=True)
            command = [str(binary), 'verify']
            record['commands'].append(command)
            with (output / (mode + '-checks.log')).open('w') as log:
                subprocess.run(command, env=env, stdout=log, stderr=subprocess.STDOUT, check=True)
            if mode == 'optimized' and not arguments.verify_only:
                for kind in ['crc', 'png']:
                    command = [str(binary), kind, str(workloads)]
                    record['commands'].append(command)
                    with (output / (kind + '.csv')).open('w') as log:
                        subprocess.run(command, stdout=log, check=True)
        targets = [('arm64', candidate_flags)]
        if platform.system() == 'Darwin':
            sdk = subprocess.check_output(['xcrun', '--show-sdk-path'], text=True).strip()
            targets.append(('x64-inspection', ['-target', 'x86_64-apple-macos14', '-isysroot', sdk,
                                               '-march=x86-64', '-mtune=generic', '-msse2', '-mno-avx']))
        for name, flags in targets:
            command = ['clang++', '-std=c++20', '-O3', *includes, *flags, '-S',
                       '-Rpass=loop-vectorize', '-Rpass=slp-vectorizer', '-fstack-usage',
                       str(source / 'compact.cpp'), '-o', str(output / (name + '.s'))]
            record['commands'].append(command)
            with (output / (name + '-remarks.txt')).open('w') as log:
                subprocess.run(command, stderr=log, check=True)
        record['uptime_end'] = subprocess.check_output(['uptime'], text=True)
        record['status'] = 'passed'
    finally:
        (output / 'run.json').write_text(json.dumps(record, indent=2) + '\n')


if __name__ == '__main__':
    main()
