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


def gallery_enabled(flag: bool, environ: dict[str, str]) -> bool:
    """Resolve whether the demo programs are configured.

    The default stays OFF: CI builds only the toolkit and its tests. The
    release pipeline passes --gallery or sets GUI_FORMS_CI_BUILD_GALLERY to
    1/true/on/yes to add the Gallery and lab executables.
    """
    if flag:
        return True
    value: str = environ.get('GUI_FORMS_CI_BUILD_GALLERY', '').strip().lower()
    return value in ('1', 'true', 'on', 'yes')


SANITIZER_FLAGS: str = '-fsanitize=address,undefined -fno-sanitize-recover=undefined -fno-omit-frame-pointer'


def sanitizer_options(host: str, lsan_suppressions: Path) -> tuple[list[str], dict[str, str]]:
    """CMake options and test environment for the sanitized suite on host.

    Address and undefined-behaviour sanitizers everywhere, and the leak
    sanitizer where it runs inside AddressSanitizer: Linux and macOS (the
    MinGW runtime on Windows has none). Every report stops its test, so any
    finding fails the run. Third-party sources built by CMake (HarfBuzz,
    FreeType) are instrumented too; the prebuilt Skia archive is not.
    """
    detect_leaks: str = '0'
    if host in ('linux', 'macos'):
        detect_leaks = '1'
    options: list[str] = [
        '-DCMAKE_BUILD_TYPE=RelWithDebInfo',
        '-DCMAKE_C_FLAGS=' + SANITIZER_FLAGS,
        '-DCMAKE_CXX_FLAGS=' + SANITIZER_FLAGS,
        '-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address,undefined',
        '-DCMAKE_SHARED_LINKER_FLAGS=-fsanitize=address,undefined',
    ]
    environment: dict[str, str] = {
        'ASAN_OPTIONS': 'detect_leaks=' + detect_leaks + ':halt_on_error=1:abort_on_error=0:print_summary=1',
        'UBSAN_OPTIONS': 'print_stacktrace=1:halt_on_error=1:print_summary=1',
    }
    if detect_leaks == '1':
        environment['LSAN_OPTIONS'] = 'suppressions=' + str(lsan_suppressions) + ':print_suppressions=0'
    return options, environment


def build_toolkit(host: str, build: Path, sdk: Path, jobs: int, gallery: bool = False,
                  sanitize: bool = False, tests: str = '') -> None:
    """Build, test, then install only into the explicitly supplied build SDK.

    With sanitize, the toolkit is built in its own directory with the
    sanitizers and only tested: nothing is installed and the installed-SDK
    examples are not built. tests, a ctest -R pattern, narrows that run; empty
    runs the whole suite.
    """
    toolkit: Path = build / ('gui-forms-sanitize' if sanitize else 'gui-forms')
    options: list[str] = []
    gallery_option: str = 'ON' if gallery else 'OFF'
    miniaudio: Path = toolkit / '_deps/gui_forms_miniaudio-src'
    if (miniaudio / 'miniaudio.h').is_file():
        # Source was restored from the pinned dependency cache. Audio.cmake
        # checks both decoder/header digests before compiling it. A missing
        # FetchContent stamp must not discard and clone these same bytes again.
        options.append(f'-DFETCHCONTENT_SOURCE_DIR_GUI_FORMS_MINIAUDIO={miniaudio}')
    hosts: dict[str, str] = {'WINDOWS': 'windows', 'MACOS': 'macos', 'LINUX': 'linux'}
    name: str
    for name in hosts:
        enabled: str = 'OFF'
        if hosts[name] == host:
            enabled = 'ON'
        options.append(f'-DGUI_FORMS_ENABLE_{name}_HOST={enabled}')
    if host == 'windows':
        # ADR-004: the Win32 host draws text with the shared HarfBuzz/FreeType engine.
        run('sh', ROOT / 'gui_forms/third_party/fetch_text_stack.sh')
        options.extend(['-DGUI_FORMS_ENABLE_SKIA=OFF', '-DGUI_FORMS_ENABLE_HARFBUZZ_TEXT=ON'])
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
    timeout: str = '120'
    if sanitize:
        sanitizer_cmake: list[str]
        sanitizer_environment: dict[str, str]
        sanitizer_cmake, sanitizer_environment = sanitizer_options(host, ROOT / 'gui_forms/tools/sanitizer/lsan.supp')
        options.extend(sanitizer_cmake)
        os.environ.update(sanitizer_environment)
        timeout = '900'
    run('cmake', '-S', ROOT / 'gui_forms', '-B', toolkit, '-G', 'Ninja',
        '-DCMAKE_BUILD_TYPE=Release', '-DCMAKE_INSTALL_LIBDIR=lib',
        '-DCMAKE_TOOLCHAIN_FILE=' + str(ROOT / 'gui_forms/cmake/llvm22.cmake'),
        '-DCMAKE_POSITION_INDEPENDENT_CODE=ON', f'-DGUI_FORMS_BUILD_GALLERY={gallery_option}',
        '-DGUI_FORMS_BUILD_TESTS=ON', '-DGUI_FORMS_BUILD_AUDIO=ON',
        '-DGUI_FORMS_AUDIO_VORBIS_BACKEND=stx', f'-DCMAKE_INSTALL_PREFIX={sdk}', *options)
    run('cmake', '--build', toolkit, '--parallel', jobs)
    os.environ['GUI_FORMS_FONT_DIR'] = str(ROOT / 'gui_forms/assets/fonts')
    selection: list[str] = []
    if tests:
        selection = ['-R', tests]
    run('ctest', '--test-dir', toolkit, '--output-on-failure', '--timeout', timeout, *selection)
    if sanitize:
        return
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
    parser.add_argument('--sanitize', action='store_true',
                        help='build in gui-forms-sanitize with ASan and UBSan (and LSan on Linux '
                             'and macOS) and only test')
    parser.add_argument('--tests', default='',
                        help='with --sanitize, run only the tests matching this ctest -R pattern')
    parser.add_argument('--gallery', action='store_true',
                        help='also configure GUI_FORMS_BUILD_GALLERY=ON '
                             '(or set GUI_FORMS_CI_BUILD_GALLERY=1); default OFF')
    args: argparse.Namespace = parser.parse_args()
    if args.jobs < 1:
        parser.error('--jobs must be positive')
    if args.tests and not args.sanitize:
        parser.error('--tests narrows the sanitized run only')
    hosts: dict[str, str] = {'Darwin': 'macos', 'Linux': 'linux', 'Windows': 'windows'}
    host: str = hosts[platform.system()]
    architecture: str = 'x64'
    if platform.machine().lower() in ('arm64', 'aarch64'):
        architecture = 'arm64'
    build: Path = ROOT / '.build' / ('native-' + host + '-' + architecture)
    build.mkdir(parents=True, exist_ok=True)
    os.environ['BUILD_JOBS'] = str(args.jobs)
    build_toolkit(host, build, build / 'gui-forms-sdk', args.jobs,
                  gallery_enabled(args.gallery, dict(os.environ)), args.sanitize, args.tests)


if __name__ == '__main__':
    main()
