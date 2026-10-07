#!/usr/bin/env python3
"""Build and test the standalone toolkit in the consumer-compatible CI layout."""
from __future__ import annotations
import argparse
import os
from pathlib import Path
import platform
import subprocess

ROOT: Path = Path(__file__).resolve().parents[2]


def run(*arguments: str | Path | int) -> None:
    command: list[str] = []
    argument: str | Path | int
    for argument in arguments:
        command.append(str(argument))
    print(' '.join(command), flush=True)
    subprocess.run(command, check=True)


def build_toolkit(host: str, build: Path, sdk: Path, jobs: int) -> None:
    """Build, test, then install only into the explicitly supplied build SDK."""
    toolkit: Path = build / 'gui-forms'
    options: list[str] = []
    hosts: dict[str, str] = {'WINDOWS': 'windows', 'MACOS': 'macos', 'LINUX': 'linux'}
    name: str
    for name in hosts:
        enabled: str = 'OFF'
        if hosts[name] == host:
            enabled = 'ON'
        options.append(f'-DGUI_FORMS_ENABLE_{name}_HOST={enabled}')
    if host == 'windows':
        options.extend(['-DGUI_FORMS_ENABLE_SKIA=OFF', '-DGUI_FORMS_ENABLE_HARFBUZZ_TEXT=OFF'])
    else:
        if host == 'linux':
            os.environ['GUI_FORMS_SYSTEM_BUILD_TOOLS'] = '1'
        run('sh', ROOT / 'gui_forms/third_party/fetch_skia_cpu.sh')
        run('sh', ROOT / 'gui_forms/third_party/fetch_text_stack.sh')
        skia_out: Path = build / 'skia'
        if host == 'linux':
            run('sh', ROOT / 'gui_forms/third_party/build_skia_cpu_linux.sh', skia_out)
        else:
            run('sh', ROOT / 'gui_forms/third_party/build_skia_cpu.sh', skia_out)
        options.extend(['-DGUI_FORMS_ENABLE_SKIA=ON', '-DGUI_FORMS_ENABLE_HARFBUZZ_TEXT=ON',
                        '-DGUI_FORMS_SKIA_PREBUILT=ON', f'-DGUI_FORMS_SKIA_OUT={skia_out}'])
    run('cmake', '-S', ROOT / 'gui_forms', '-B', toolkit, '-G', 'Ninja',
        '-DCMAKE_BUILD_TYPE=Release', '-DCMAKE_INSTALL_LIBDIR=lib',
        '-DCMAKE_TOOLCHAIN_FILE=' + str(ROOT / 'gui_forms/cmake/llvm22.cmake'),
        '-DCMAKE_POSITION_INDEPENDENT_CODE=ON', '-DGUI_FORMS_BUILD_GALLERY=OFF',
        '-DGUI_FORMS_BUILD_TESTS=ON', '-DGUI_FORMS_BUILD_AUDIO=ON', f'-DCMAKE_INSTALL_PREFIX={sdk}', *options)
    run('cmake', '--build', toolkit, '--parallel', jobs)
    os.environ['GUI_FORMS_FONT_DIR'] = str(ROOT / 'gui_forms/assets/fonts')
    run('ctest', '--test-dir', toolkit, '--output-on-failure', '--timeout', '120')
    run('cmake', '--install', toolkit)
    examples: Path = build / 'installed-reference-examples'
    run('cmake', '-S', ROOT / 'gui_forms/examples/reference', '-B', examples,
        '-G', 'Ninja', '-DCMAKE_BUILD_TYPE=Release', f'-DCMAKE_PREFIX_PATH={sdk}',
        '-DCMAKE_TOOLCHAIN_FILE=' + str(ROOT / 'gui_forms/cmake/llvm22.cmake'))
    run('cmake', '--build', examples, '--parallel', jobs)
    run('ctest', '--test-dir', examples, '--output-on-failure')


def main() -> None:
    parser: argparse.ArgumentParser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--jobs', type=int, default=4)
    args: argparse.Namespace = parser.parse_args()
    if args.jobs < 1:
        parser.error('--jobs must be positive')
    hosts: dict[str, str] = {'Darwin': 'macos', 'Linux': 'linux', 'Windows': 'windows'}
    host: str = hosts[platform.system()]
    architecture: str = 'x64'
    if platform.machine().lower() in ('arm64', 'aarch64'):
        architecture = 'arm64'
    build: Path = ROOT / '.build' / ('native-' + host + '-' + architecture)
    build.mkdir(parents=True, exist_ok=True)
    os.environ['BUILD_JOBS'] = str(args.jobs)
    build_toolkit(host, build, build / 'gui-forms-sdk', args.jobs)


if __name__ == '__main__':
    main()
