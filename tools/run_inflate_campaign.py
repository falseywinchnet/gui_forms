#!/usr/bin/env python3
"""Run one supervised, source-locked inflate fuzz shard; preserve all evidence."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import shutil
import signal
import subprocess
import time
import zlib

ROOT = Path(__file__).resolve().parents[1]
PLAN = ROOT / 'planning/stages/Z-zlib/INFLATE_CAMPAIGN.json'


def digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def write_record(path: Path, record: dict) -> None:
    temporary = path.with_suffix('.tmp')
    temporary.write_text(json.dumps(record, indent=2) + '\n')
    temporary.replace(path)


def tree_rss(table: str, root_pid: int) -> int:
    rows = []
    for line in table.splitlines():
        values = line.split()
        if len(values) != 3:
            raise RuntimeError('Unrecognized process RSS record')
        rows.append(tuple(int(value) for value in values))
    descendants = {root_pid}
    while True:
        previous = len(descendants)
        for pid, parent, rss in rows:
            if parent in descendants:
                descendants.add(pid)
        if len(descendants) == previous:
            break
    return sum(rss * 1024 for pid, parent, rss in rows if pid in descendants)


def resident_bytes(pid: int) -> int:
    table = subprocess.check_output(['ps', '-axo', 'pid=,ppid=,rss='], text=True, timeout=10)
    return tree_rss(table, pid)


def stop_group(process: subprocess.Popen) -> None:
    try:
        os.killpg(process.pid, signal.SIGTERM)
    except ProcessLookupError:
        pass
    try:
        process.wait(timeout=10)
    except subprocess.TimeoutExpired:
        try:
            os.killpg(process.pid, signal.SIGKILL)
        except ProcessLookupError:
            pass
        process.wait()


def supervised(command: list[str], log: Path, cwd: Path, environment: dict,
               seconds: float, rss_limit: int, checkpoint: Path, record: dict) -> dict:
    operation = {'command': command, 'log': str(log), 'state': 'running', 'rss_peak_sampled': 0}
    record['operations'].append(operation)
    write_record(checkpoint, record)
    print('Starting:', log.name, flush=True)
    start = time.monotonic()
    heartbeat = start
    with log.open('w') as stream:
        process = None
        try:
            process = subprocess.Popen(command, cwd=cwd, env=environment, stdout=stream,
                                       stderr=subprocess.STDOUT, start_new_session=True)
            operation['pid'] = process.pid
            while process.poll() is None:
                elapsed = time.monotonic() - start
                if elapsed > seconds:
                    raise RuntimeError('External wall-time limit exceeded: ' + log.name)
                rss = resident_bytes(process.pid)
                operation['rss_peak_sampled'] = max(operation['rss_peak_sampled'], rss)
                if rss > rss_limit:
                    raise RuntimeError('External RSS limit exceeded: ' + log.name)
                if time.monotonic() - heartbeat >= 60:
                    operation['elapsed_seconds'] = elapsed
                    write_record(checkpoint, record)
                    print(log.name, round(elapsed), 'seconds; sampled RSS', rss, flush=True)
                    heartbeat = time.monotonic()
                try:
                    process.wait(timeout=1)
                except subprocess.TimeoutExpired:
                    pass
            operation['exit_code'] = process.returncode
            if process.returncode != 0:
                raise RuntimeError('Process failed: ' + log.name + ': ' + str(process.returncode))
            operation['state'] = 'passed'
        except BaseException as error:
            if process is not None:
                stop_group(process)
            operation['state'] = 'failed'
            operation['error'] = str(error)
            operation['exit_code'] = None if process is None else process.returncode
            raise
        finally:
            operation['elapsed_seconds'] = time.monotonic() - start
            write_record(checkpoint, record)
    return operation


def seed_corpus(destination: Path) -> dict:
    destination.mkdir()
    for path in sorted((ROOT / 'fuzz/corpus/inflate').iterdir()):
        if path.is_file():
            shutil.copyfile(path, destination / digest(path))
    random = 0x20261020
    entropy = bytearray()
    for index in range(32768):
        random ^= (random << 13) & 0xffffffff
        random ^= random >> 17
        random ^= (random << 5) & 0xffffffff
        entropy.append(random & 255)
    patterns = [bytes(32768), bytes(range(256)) * 128,
                (b'GUI.Forms bounded inflate text and repeated records\n' * 1024)[:32768], bytes(entropy)]
    for pattern in patterns:
        for size in (0, 17, 257, 4096, 32768):
            for window in (-15, 15):
                for level in (0, 1, 6, 9):
                    for strategy in (zlib.Z_DEFAULT_STRATEGY, zlib.Z_FILTERED,
                                     zlib.Z_HUFFMAN_ONLY, zlib.Z_RLE, zlib.Z_FIXED):
                        encoder = zlib.compressobj(level, zlib.DEFLATED, window, 8, strategy)
                        stream = encoder.compress(pattern[:size]) + encoder.flush()
                        data = bytes([int(window > 0)]) + stream
                        name = hashlib.sha256(data).hexdigest()
                        (destination / name).write_bytes(data)
    return corpus_hashes(destination)


def corpus_hashes(directory: Path) -> dict:
    return {path.name: digest(path) for path in sorted(directory.iterdir()) if path.is_file()}


def reported_fuzz_time(log: str) -> tuple[int, int]:
    matches = re.findall(r'^Done (\d+) runs in (\d+) second\(s\)', log, re.MULTILINE)
    if len(matches) != 1:
        raise RuntimeError('Missing or ambiguous libFuzzer completion record')
    executions, seconds = matches[0]
    return int(executions), int(seconds)


def verify_source_lock(plan: dict, root: Path) -> dict:
    actual = {name: digest(root / name) for name in plan['source_sha256']}
    if not actual or actual != plan['source_sha256']:
        raise RuntimeError('Campaign source lock mismatch; do not qualify another source revision')
    return actual


def interrupted(signum: int, frame: object) -> None:
    raise InterruptedError('Campaign interrupted by signal ' + str(signum))


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--oracle', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--oracle-mode', choices=('ordinary', 'strict'), required=True)
    parser.add_argument('--shard', type=int, required=True)
    parser.add_argument('--seconds', type=int, help='Short validation only; never credited as a full shard')
    arguments = parser.parse_args()
    plan = json.loads(PLAN.read_text())
    if not 0 <= arguments.shard < plan['shards_per_configuration']:
        parser.error('Shard outside campaign plan')
    duration = plan['seconds_per_shard'] if arguments.seconds is None else arguments.seconds
    if not 1 <= duration <= plan['seconds_per_shard']:
        parser.error('Duration must be positive and no longer than the planned shard')
    output = arguments.output.resolve()
    if output.exists():
        parser.error('Output already exists; preserve it and choose a new directory')
    evidence = output / 'evidence'
    evidence.mkdir(parents=True)
    build = output / 'build'
    build.mkdir()
    profiles = evidence / 'profiles'
    profiles.mkdir()
    failures = evidence / 'failures'
    failures.mkdir()
    checkpoint = evidence / 'run.json'
    record = {'campaign': plan['campaign'], 'plan_sha256': digest(PLAN), 'state': 'preparing',
              'host': platform.platform(), 'machine': platform.machine(),
              'oracle_mode': arguments.oracle_mode, 'shard': arguments.shard,
              'requested_fuzz_seconds': duration, 'qualifying_fuzz_seconds': 0,
              'smoke': arguments.seconds is not None, 'operations': [],
              'started_unix_seconds': time.time(),
              'github_run_id': os.environ.get('GITHUB_RUN_ID'),
              'github_run_attempt': os.environ.get('GITHUB_RUN_ATTEMPT'),
              'github_sha': os.environ.get('GITHUB_SHA'),
              'python_zlib_compile': zlib.ZLIB_VERSION, 'python_zlib_runtime': zlib.ZLIB_RUNTIME_VERSION}
    environment = dict(os.environ)
    environment.update(ASAN_OPTIONS='detect_leaks=1:halt_on_error=1',
                       UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1',
                       LLVM_PROFILE_FILE=str(profiles / '%p.profraw'))
    environment.pop('LSAN_OPTIONS', None)
    write_record(checkpoint, record)
    try:
        if os.name != 'posix':
            raise RuntimeError('This supervisor requires POSIX process groups and ps RSS sampling')
        signal.signal(signal.SIGTERM, interrupted)
        record['source_sha256'] = verify_source_lock(plan, ROOT)
        oracle = arguments.oracle.resolve()
        oracle_manifest = json.loads((ROOT / 'tests/compression/oracle_sha256.json').read_text())
        for name, expected in oracle_manifest['files'].items():
            if digest(oracle / name) != expected:
                raise RuntimeError('Oracle hash mismatch: ' + name)
        record['oracle_revision'] = oracle_manifest['revision']
        versions = {}
        for tool in ('clang', 'clang++', 'llvm-profdata', 'llvm-cov'):
            versions[tool] = subprocess.check_output([tool, '--version'], text=True)
            if '22.1.8' not in versions[tool]:
                raise RuntimeError('Pinned LLVM 22.1.8 required: ' + tool)
        record['tool_versions'] = versions
        machine = platform.machine().lower()
        if machine in ('arm64', 'aarch64'):
            record['architecture'] = 'arm64'
        elif machine in ('x86_64', 'amd64'):
            record['architecture'] = 'x64'
        else:
            raise RuntimeError('Unplanned architecture: ' + machine)
        if record['architecture'] not in plan['architectures']:
            raise RuntimeError('Architecture outside campaign plan')
        common = ['-O1', '-g', '-fsanitize=address,undefined', '-fno-sanitize-recover=undefined',
                  '-fno-omit-frame-pointer', '-fprofile-instr-generate', '-fcoverage-mapping']
        includes = ['-I' + str(ROOT / 'include'), '-I' + str(oracle)]
        objects = []
        for name in ('adler32', 'crc32', 'deflate', 'inflate', 'inffast', 'inftrees', 'trees', 'zutil'):
            obj = build / (name + '.o')
            flags = ['-DINFLATE_STRICT'] if arguments.oracle_mode == 'strict' else []
            command = ['clang', *common, *includes, *flags, '-c', str(oracle / (name + '.c')), '-o', str(obj)]
            supervised(command, evidence / ('compile-' + name + '.log'), build, environment,
                       600, plan['rss_limit_bytes'], checkpoint, record)
            objects.append(str(obj))
        candidate = str(ROOT / 'src/core/compression/inflate.cpp')
        arguments_cpp = ['clang++', '-std=c++20', *common, *includes, '-DGUI_FORMS_INFLATE_ORACLE=1', candidate]
        test = build / 'inflate-tests'
        supervised([*arguments_cpp, str(ROOT / 'tests/compression/inflate_tests.cpp'), *objects, '-o', str(test)],
                   evidence / 'compile-tests.log', build, environment, 600, plan['rss_limit_bytes'], checkpoint, record)
        supervised([str(test)], evidence / 'tests.log', build, environment,
                   600, plan['rss_limit_bytes'], checkpoint, record)
        fuzz = build / 'inflate-fuzz'
        supervised([*arguments_cpp, '-fsanitize=fuzzer', str(ROOT / 'fuzz/inflate_fuzz.cpp'), *objects, '-o', str(fuzz)],
                   evidence / 'compile-fuzz.log', build, environment, 600, plan['rss_limit_bytes'], checkpoint, record)
        seeds = build / 'seeds'
        record['initial_corpus_sha256'] = seed_corpus(seeds)
        corpus = evidence / 'corpus'
        corpus.mkdir()
        flags = ['-max_len=65536', '-timeout=20', '-rss_limit_mb=0', '-malloc_limit_mb=0',
                 '-artifact_prefix=' + str(failures) + '/', '-print_final_stats=1']
        supervised([str(fuzz), *flags, '-merge=1', str(corpus), str(seeds)], evidence / 'merge.log',
                   build, environment, 1200, plan['rss_limit_bytes'], checkpoint, record)
        record['merged_corpus_sha256'] = corpus_hashes(corpus)
        seed = plan['seed_base'] + arguments.shard + (100 if arguments.oracle_mode == 'strict' else 0)
        if record['architecture'] == 'arm64':
            seed += 1000
        record['state'] = 'fuzzing'
        operation = supervised([str(fuzz), *flags, '-seed=' + str(seed), '-max_total_time=' + str(duration), str(corpus)],
                               evidence / 'fuzz.log', build, environment, duration + 120,
                               plan['rss_limit_bytes'], checkpoint, record)
        executions, reported_seconds = reported_fuzz_time((evidence / 'fuzz.log').read_text())
        record['fuzz_executions'] = executions
        record['fuzz_reported_seconds'] = reported_seconds
        record['fuzz_process_seconds'] = operation['elapsed_seconds']
        if reported_seconds < duration:
            raise RuntimeError('Fuzzer stopped before the requested duration')
        record['final_corpus_sha256'] = corpus_hashes(corpus)
        # Separate deterministic replay supplies coverage without mixing the
        # native test binary's unrelated instrumentation profiles into this map.
        replay = evidence / 'replay.profraw'
        replay_environment = dict(environment, LLVM_PROFILE_FILE=str(replay))
        record['state'] = 'coverage'
        supervised([str(fuzz), *flags, '-runs=0', str(corpus)], evidence / 'replay.log', build,
                   replay_environment, 1200, plan['rss_limit_bytes'], checkpoint, record)
        profile = evidence / 'replay.profdata'
        supervised(['llvm-profdata', 'merge', '-sparse', str(replay), '-o', str(profile)],
                   evidence / 'coverage-merge.log', build, environment, 120, plan['rss_limit_bytes'], checkpoint, record)
        supervised(['llvm-cov', 'export', str(fuzz), '-instr-profile=' + str(profile), candidate],
                   evidence / 'coverage.json', build, environment, 120, plan['rss_limit_bytes'], checkpoint, record)
        supervised(['llvm-cov', 'show', str(fuzz), '-instr-profile=' + str(profile), '-show-branches=count', candidate],
                   evidence / 'coverage.txt', build, environment, 120, plan['rss_limit_bytes'], checkpoint, record)
        coverage = json.loads((evidence / 'coverage.json').read_text())
        files = coverage['data'][0]['files']
        selected = [item for item in files if Path(item['filename']).resolve() == Path(candidate)]
        if len(selected) != 1:
            raise RuntimeError('Missing or ambiguous candidate coverage')
        record['candidate_coverage'] = selected[0]['summary']
        verify_source_lock(plan, ROOT)
        record['evidence_sha256'] = {name: digest(evidence / name) for name in
                                     ('tests.log', 'merge.log', 'fuzz.log', 'replay.log', 'coverage.json', 'coverage.txt')}
        record['state'] = 'smoke_passed' if record['smoke'] else 'completed'
        if not record['smoke']:
            record['qualifying_fuzz_seconds'] = duration
    except BaseException as error:
        record['state'] = 'failed'
        record['error'] = str(error)
        raise
    finally:
        write_record(checkpoint, record)
    print(record['state'], checkpoint, flush=True)


if __name__ == '__main__':
    main()
