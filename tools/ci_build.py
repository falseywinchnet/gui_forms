#!/usr/bin/env python3
"""Build and test the standalone toolkit in the consumer-compatible CI layout."""
from __future__ import annotations
import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import shutil
import subprocess

import change_selection
import native_tests

ROOT: Path = Path(__file__).resolve().parents[2]


def run(*arguments: str | Path | int) -> None:
    command: list[str] = []
    argument: str | Path | int
    for argument in arguments:
        command.append(str(argument))
    print(' '.join(command), flush=True)
    subprocess.run(command, check=True)


def captured(*arguments: str | Path) -> str:
    """Run a query command and return its standard output (it must succeed)."""
    command: list[str] = []
    argument: str | Path
    for argument in arguments:
        command.append(str(argument))
    completed: subprocess.CompletedProcess[str] = subprocess.run(
        command, check=True, capture_output=True, text=True, encoding='utf-8', errors='replace')
    return completed.stdout


def tree_relative(argument: str, tree: Path) -> str | None:
    """The build-tree-relative path an argument names, or None if it names none.

    A -DNAME=value argument is read by its value. The result uses forward
    slashes, as ninja spells its outputs.
    """
    value: str = argument
    if argument.startswith('-') and '=' in argument:
        value = argument.partition('=')[2]
    if not value:
        return None
    root: str = os.path.normcase(os.path.normpath(str(tree)))
    path: str = os.path.normcase(os.path.normpath(value))
    if not path.startswith(root + os.sep):
        return None
    return os.path.relpath(os.path.normpath(value), os.path.normpath(str(tree))).replace(os.sep, '/')


def targets_for_tests(listing: dict, known_outputs: set[str], tree: Path) -> list[str] | None:
    """The ninja outputs the listed tests read, or None to build everything.

    listing is `ctest --show-only=json-v1` output for the selected tests and
    known_outputs every output ninja can build in tree. A test reads the
    build-tree files its command names (its executable, or the libraries and
    programs an audit script inspects). The whole tree is built instead when
    a test names a build-tree file ninja does not produce, or depends on other
    tests (fixtures, DEPENDS), whose products this reading cannot see, or
    has no command at all (see test_commands). A test naming no build-tree
    file needs nothing built.
    """
    targets: set[str] = set()
    test: dict
    for test in listing.get('tests', []):
        if 'command' not in test:
            return None
        property_record: dict
        for property_record in test.get('properties', []):
            if property_record.get('name') in ('FIXTURES_REQUIRED', 'DEPENDS', 'REQUIRED_FILES'):
                return None
        argument: str
        for argument in test.get('command', []):
            relative: str | None = tree_relative(argument, tree)
            if relative is None:
                continue
            if relative not in known_outputs:
                return None
            targets.add(relative)
    return sorted(targets)


def ctest_selection(tests: str, skip_labels: str) -> list[str]:
    """ctest arguments choosing tests: -R tests (none matching fails), -LE skip_labels."""
    selection: list[str] = []
    if tests:
        # A pattern that matches no test is a mistake, not a pass.
        selection.extend(['-R', tests, '--no-tests=error'])
    if skip_labels:
        selection.extend(['-LE', skip_labels])
    return selection


# One argument of a CMake command: a quoted argument (a backslash escapes the
# next character), a bracket argument [=[...]=], or an unquoted run.
CMAKE_ARGUMENT: re.Pattern[str] = re.compile(r'"((?:[^"\\]|\\.)*)"|\[(=*)\[(.*?)\]\2\]|([^\s()"]+)', re.DOTALL)
CMAKE_ESCAPE: re.Pattern[str] = re.compile(r'\\(.)', re.DOTALL)
CMAKE_CALL: re.Pattern[str] = re.compile(r'^(add_test|subdirs)\((.*)\)\s*$', re.MULTILINE)


def cmake_arguments(text: str) -> list[str]:
    """The arguments of one CMake command's parenthesized text, unquoted."""
    arguments: list[str] = []
    match: re.Match[str]
    for match in CMAKE_ARGUMENT.finditer(text):
        if match.group(1) is not None:
            arguments.append(CMAKE_ESCAPE.sub(r'\1', match.group(1)))
        elif match.group(3) is not None:
            arguments.append(match.group(3))
        else:
            arguments.append(match.group(4))
    return arguments


def test_commands(directory: Path) -> dict[str, list[str]]:
    """Every test's command as directory's CTestTestfile.cmake (and its subdirs) records it.

    ctest --show-only=json-v1 leaves out the command of a test whose
    executable is not built yet, which is every test of a fresh tree; the
    generated file still names it.
    """
    commands: dict[str, list[str]] = {}
    test_file: Path = directory / 'CTestTestfile.cmake'
    if not test_file.is_file():
        return commands
    call: re.Match[str]
    for call in CMAKE_CALL.finditer(test_file.read_text(encoding='utf-8', errors='replace')):
        arguments: list[str] = cmake_arguments(call.group(2))
        if call.group(1) == 'subdirs':
            subdirectory: str
            for subdirectory in arguments:
                commands.update(test_commands(directory / subdirectory))
        elif arguments:
            commands[arguments[0]] = arguments[1:]
    return commands


def test_listing(toolkit: Path, tests: str, skip_labels: str) -> dict:
    """ctest's --show-only=json-v1 listing of the tests ctest_selection(tests, skip_labels) chooses.

    A test whose executable is not built yet gets its command from the
    generated test files (test_commands), so that the listing names it.
    """
    listing: dict = json.loads(captured('ctest', '--test-dir', toolkit, '--show-only=json-v1',
                                        *ctest_selection(tests, skip_labels)))
    commands: dict[str, list[str]] | None = None
    test: dict
    for test in listing.get('tests', []):
        if 'command' in test:
            continue
        if commands is None:
            commands = test_commands(toolkit)
        if test['name'] in commands:
            test['command'] = commands[test['name']]
    return listing


def selected_test_targets(toolkit: Path, listing: dict) -> list[str] | None:
    """Ask ninja which outputs the tests of listing need (see targets_for_tests)."""
    known_outputs: set[str] = set()
    line: str
    for line in captured('ninja', '-C', toolkit, '-t', 'targets', 'all').splitlines():
        output: str = line.rpartition(': ')[0]
        if output:
            known_outputs.add(output)
    return targets_for_tests(listing, known_outputs, toolkit)


def check_windows_sdk_imports(sdk: Path) -> None:
    """Fail when an installed SDK DLL imports a compiler runtime DLL.

    Every Windows binary links the LLVM runtimes and winpthreads statically
    (CMakeLists.txt), so an installed DLL imports Windows system DLLs and
    GUI.Forms' own DLLs only; the forbidden names are package_programs.py's.
    Prints each DLL's non-system imports.
    """
    from package_programs import WINDOWS_FORBIDDEN_RUNTIME_DLL, WINDOWS_SYSTEM_DLLS
    objdump: str | None = shutil.which('objdump') or shutil.which('llvm-objdump')
    if objdump is None:
        raise RuntimeError('objdump (or llvm-objdump) is needed to check the installed DLLs')
    dlls: list[Path] = sorted(sdk.rglob('*.dll'))
    if not dlls:
        raise RuntimeError('the installed SDK has no DLL under ' + str(sdk))
    dll: Path
    for dll in dlls:
        listing: str = subprocess.run([objdump, '-p', str(dll)], check=True, capture_output=True,
                                      text=True).stdout
        names: list[str] = re.findall(r'DLL Name:\s*(\S+)', listing)
        own: list[str] = []
        name: str
        for name in names:
            key: str = name.lower()
            if key in WINDOWS_SYSTEM_DLLS or key.startswith(('api-ms-win-', 'ext-ms-win-')):
                continue
            if WINDOWS_FORBIDDEN_RUNTIME_DLL.fullmatch(name):
                raise RuntimeError(f'installed {dll.name} imports the compiler runtime {name}')
            own.append(name)
        print(f'installed {dll.relative_to(sdk)}: imports {", ".join(own) if own else "system DLLs only"}', flush=True)


def check_installed_notices(sdk: Path) -> None:
    """Fail unless the installed SDK carries THIRD_PARTY_NOTICES.md with the
    FreeType credit and no GPL text (the rules of package_programs.py)."""
    from package_programs import FREETYPE_CREDIT, GPL_TITLE, NOTICES_NAME
    licences: Path = sdk / 'share' / 'licenses' / 'GUIForms'
    notices: Path = licences / NOTICES_NAME
    if not notices.is_file() or FREETYPE_CREDIT not in notices.read_text(encoding='utf-8'):
        raise RuntimeError(f'the installed SDK lacks {notices} or its FreeType credit')
    path: Path
    for path in sorted(licences.rglob('*')):
        if 'gpl' in path.name.lower():
            raise RuntimeError(f'the installed SDK ships {path.name}')
        if path.is_file() and GPL_TITLE.search(path.read_text(encoding='utf-8', errors='replace')):
            raise RuntimeError(f'the installed {path.name} carries a GNU General Public License text')
    print(f'installed notices checked: {notices.relative_to(sdk)}', flush=True)


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


# The sanitized build type carries -g, and ccache then adds the build
# directory to every object's hash (debug information records it), so no two
# checkouts could share a sanitized object. -fdebug-compilation-dir=. records
# "." instead; ccache recognizes the option and leaves the directory out, so
# objects of identical sources hit across checkouts with the same layout.
# Reports still name file and line, relative to the build directory.
SANITIZER_FLAGS: str = ('-fsanitize=address,undefined -fno-sanitize-recover=undefined -fno-omit-frame-pointer '
                        '-fdebug-compilation-dir=.')


SANITIZER_LINK_FLAGS: str = '-fsanitize=address,undefined'


# Debug information of the sanitized build type (owner, 2026-10-21). Line
# tables are the default: every report frame keeps file, line and column,
# objects are about 58% smaller and programs about 70% smaller than with -g,
# and compiles about 28% faster (measured on the tool_strip test program).
# Frames inlined into our code then lose their namespace and parameter types,
# and a debugger loses locals; `full` restores -g while hunting a defect.
DEBUG_INFO_FLAGS: dict[str, str] = {'lines': '-gline-tables-only', 'full': '-g'}
DEBUG_INFO_VARIABLE: str = 'GUI_FORMS_SANITIZER_DEBUG_INFO'


def debug_info_mode(flag: str, environ: dict[str, str]) -> str:
    """The sanitizer debug-information mode: the flag, else the environment, else `lines`."""
    mode: str = flag or environ.get(DEBUG_INFO_VARIABLE, '').strip().lower() or 'lines'
    if mode not in DEBUG_INFO_FLAGS:
        raise ValueError(DEBUG_INFO_VARIABLE + ' must be lines or full, not ' + repr(mode))
    return mode


def sanitizer_options(host: str, lsan_suppressions: Path,
                      debug_info: str = 'lines') -> tuple[list[str], dict[str, str]]:
    """CMake options and the configure and test environment for the sanitized suite on host.

    Address and undefined-behaviour sanitizers everywhere, and the leak
    sanitizer where it runs inside AddressSanitizer: Linux and macOS (the
    MinGW runtime on Windows has none). Every report stops its test, so any
    finding fails the run. Third-party sources built by CMake (HarfBuzz,
    FreeType) are instrumented too; the prebuilt Skia archive is not.

    On macOS cmake/llvm22.cmake puts the pinned libc++ headers and runtime
    link flags in CMAKE_<LANG>_FLAGS_INIT, which a -DCMAKE_<LANG>_FLAGS on
    the command line replaces (the tree then compiles against Homebrew's
    libc++ headers). There the flags go through CFLAGS, CXXFLAGS,
    OBJCXXFLAGS and LDFLAGS instead, which CMake joins with the _INIT values
    when it first configures the tree; OBJCXXFLAGS also instruments the
    Objective-C++ host. Being read at the first configure only, they reach a
    macOS tree configured earlier only after that tree is deleted.

    debug_info (`lines` or `full`) sets the build type's own flags,
    CMAKE_<LANG>_FLAGS_RELWITHDEBINFO, on every host and every configure, so
    changing the mode rebuilds an existing tree with the other flags.
    """
    detect_leaks: str = '0'
    if host in ('linux', 'macos'):
        detect_leaks = '1'
    build_type_flags: str = '-O2 ' + DEBUG_INFO_FLAGS[debug_info] + ' -DNDEBUG'
    options: list[str] = ['-DCMAKE_BUILD_TYPE=RelWithDebInfo',
                          '-DCMAKE_C_FLAGS_RELWITHDEBINFO=' + build_type_flags,
                          '-DCMAKE_CXX_FLAGS_RELWITHDEBINFO=' + build_type_flags]
    if host == 'macos':
        options.append('-DCMAKE_OBJCXX_FLAGS_RELWITHDEBINFO=' + build_type_flags)
    environment: dict[str, str] = {
        'ASAN_OPTIONS': 'detect_leaks=' + detect_leaks + ':halt_on_error=1:abort_on_error=0:print_summary=1',
        'UBSAN_OPTIONS': 'print_stacktrace=1:halt_on_error=1:print_summary=1',
    }
    if host == 'macos':
        environment['CFLAGS'] = SANITIZER_FLAGS
        environment['CXXFLAGS'] = SANITIZER_FLAGS
        environment['OBJCXXFLAGS'] = SANITIZER_FLAGS
        environment['LDFLAGS'] = SANITIZER_LINK_FLAGS
    else:
        options.extend([
            '-DCMAKE_C_FLAGS=' + SANITIZER_FLAGS,
            '-DCMAKE_CXX_FLAGS=' + SANITIZER_FLAGS,
            '-DCMAKE_EXE_LINKER_FLAGS=' + SANITIZER_LINK_FLAGS,
            '-DCMAKE_SHARED_LINKER_FLAGS=' + SANITIZER_LINK_FLAGS,
        ])
    if detect_leaks == '1':
        environment['LSAN_OPTIONS'] = 'suppressions=' + str(lsan_suppressions) + ':print_suppressions=0'
    return options, environment


# fetch_text_stack.sh pins each checkout as <directory>_revision=<commit>.
PIN_PATTERN: re.Pattern[str] = re.compile(r'^(\w+)_revision=([0-9a-f]{40})$', re.MULTILINE)


def pinned_checkouts_current(script: Path) -> bool:
    """Whether every checkout the fetch script pins is already at its pinned commit.

    The script would then only re-run `git checkout --detach` of the commit
    HEAD already names, which changes nothing (it keeps local edits either
    way) but rescans thousands of files. A script without readable pins, a
    missing checkout or a failing git counts as not current.
    """
    pins: list[tuple[str, str]] = PIN_PATTERN.findall(script.read_text(encoding='utf-8'))
    if not pins:
        return False
    pin: tuple[str, str]
    for pin in pins:
        checkout: Path = script.parent / pin[0]
        if not (checkout / '.git').exists():
            return False
        completed: subprocess.CompletedProcess[str] = subprocess.run(
            ['git', '-C', str(checkout), 'rev-parse', 'HEAD'], capture_output=True, text=True)
        if completed.returncode != 0 or completed.stdout.strip() != pin[1]:
            return False
    return True


def fetch_text_stack(script: Path) -> None:
    if pinned_checkouts_current(script):
        print(f'{script.name}: every checkout is at its pinned commit', flush=True)
        return
    run('sh', script)


# Environment that changes what a first configure records (compilers,
# launchers, and the macOS sanitizer and runtime flags).
CONFIGURE_ENVIRONMENT: tuple[str, ...] = (
    'CC', 'CXX', 'OBJCXX', 'CFLAGS', 'CXXFLAGS', 'OBJCXXFLAGS', 'LDFLAGS',
    'CMAKE_C_COMPILER_LAUNCHER', 'CMAKE_CXX_COMPILER_LAUNCHER', 'CMAKE_OBJCXX_COMPILER_LAUNCHER',
    'GUI_FORMS_LLVM_RUNTIME', 'GUI_FORMS_SYSTEM_BUILD_TOOLS')


CONFIGURE_RECORD_NAME: str = 'ci_build-configure.json'


def configure_record(configure: list[str], environ: dict[str, str], cache_text: bytes) -> str:
    """What one configure of a tree depended on: its command, environment and resulting cache."""
    environment: dict[str, str] = {}
    name: str
    for name in CONFIGURE_ENVIRONMENT:
        environment[name] = environ.get(name, '')
    record: dict = {'command': configure, 'environment': environment,
                    'cmake_cache_sha256': hashlib.sha256(cache_text).hexdigest()}
    return json.dumps(record, indent=1, sort_keys=True)


def configure_tree(toolkit: Path, configure: list[str]) -> None:
    """Configure toolkit, unless the same configure already produced its current cache.

    After a configure the record of its command, environment and
    CMakeCache.txt digest is kept in the tree. When all three still match,
    configuring again would change nothing, so only ninja's own regeneration
    step runs, which re-runs CMake if a CMake input changed since. Any other
    change to the cache (a manual cmake -D, a different profile) or a missing
    record configures again.
    """
    cache: Path = toolkit / 'CMakeCache.txt'
    record_path: Path = toolkit / CONFIGURE_RECORD_NAME
    if (toolkit / 'build.ninja').is_file() and cache.is_file() and record_path.is_file():
        expected: str = configure_record(configure, dict(os.environ), cache.read_bytes())
        if record_path.read_text(encoding='utf-8') == expected:
            print(f'{toolkit.name}: configured by this command already; regenerating only if CMake inputs changed',
                  flush=True)
            run('cmake', '--build', toolkit, '--target', 'build.ninja')
            return
    record_path.unlink(missing_ok=True)
    run(*configure)
    record_path.write_text(configure_record(configure, dict(os.environ), cache.read_bytes()), encoding='utf-8')


def changed_tests_listing(changed: list[str], listing: dict) -> dict | None:
    """The part of listing the changed paths select (change_selection.py), or None for every test."""
    selection: change_selection.Selection = change_selection.select(
        changed, change_selection.load_rules(), listing, ROOT / 'gui_forms')
    path: str
    for path in changed:
        print(f'changed {path}: {selection.reasons[path]}', flush=True)
    if selection.every_test:
        return None
    return {'tests': change_selection.selected_tests(listing, selection)}


def build_toolkit(host: str, build: Path, sdk: Path, jobs: int, gallery: bool = False,
                  sanitize: bool = False, tests: str = '', test_jobs: int = 1,
                  debug_info: str = 'lines', skip_labels: str = '', changed_from: str = '') -> None:
    """Build, test, then install only into the explicitly supplied build SDK.

    With sanitize, the toolkit is built in its own directory with the
    sanitizers and only tested: nothing is installed and the installed-SDK
    examples are not built. tests, a ctest -R pattern, narrows that run to
    the matching tests and the build to the outputs they read (fails when it
    matches none); empty builds everything and runs the whole suite.
    test_jobs is how many tests ctest runs at once (1, the default, runs
    them one after another). debug_info is the sanitized build's debug
    information, `lines` or `full` (see DEBUG_INFO_FLAGS). skip_labels, a
    ctest -LE pattern, leaves out the tests with matching labels (for
    example gallery_accessibility, the Gallery-wide accessibility tests)
    from the run and from a narrowed build.
    changed_from, a ref, narrows the sanitized run further to the tests the
    paths changed since it can affect (tools/change_selection.py, the areas
    of tools/test_areas.json) and the build to what they read; when no
    changed path is read by any test, nothing is configured, built or run.
    """
    changed: list[str] = []
    if changed_from:
        changed = change_selection.changed_paths(ROOT / 'gui_forms', changed_from)
        if not change_selection.any_path_read(changed, change_selection.load_rules()):
            print(f'no test reads the {len(changed)} paths changed since {changed_from}; nothing to build or run',
                  flush=True)
            return
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
        fetch_text_stack(ROOT / 'gui_forms/third_party/fetch_text_stack.sh')
        options.extend(['-DGUI_FORMS_ENABLE_SKIA=OFF', '-DGUI_FORMS_ENABLE_HARFBUZZ_TEXT=ON'])
    else:
        if host == 'linux':
            os.environ['GUI_FORMS_SYSTEM_BUILD_TOOLS'] = '1'
        run('sh', ROOT / 'gui_forms/third_party/fetch_skia_cpu.sh')
        fetch_text_stack(ROOT / 'gui_forms/third_party/fetch_text_stack.sh')
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
        sanitizer_cmake, sanitizer_environment = sanitizer_options(
            host, ROOT / 'gui_forms/tools/sanitizer/lsan.supp', debug_info)
        options.extend(sanitizer_cmake)
        os.environ.update(sanitizer_environment)
        timeout = '900'
    configure: list[str] = [
        'cmake', '-S', str(ROOT / 'gui_forms'), '-B', str(toolkit), '-G', 'Ninja',
        '-DCMAKE_BUILD_TYPE=Release', '-DCMAKE_INSTALL_LIBDIR=lib',
        '-DCMAKE_TOOLCHAIN_FILE=' + str(ROOT / 'gui_forms/cmake/llvm22.cmake'),
        '-DCMAKE_POSITION_INDEPENDENT_CODE=ON', f'-DGUI_FORMS_BUILD_GALLERY={gallery_option}',
        '-DGUI_FORMS_BUILD_TESTS=ON', '-DGUI_FORMS_BUILD_AUDIO=ON',
        '-DGUI_FORMS_AUDIO_VORBIS_BACKEND=stx', f'-DCMAKE_INSTALL_PREFIX={sdk}', *options]
    configure_tree(toolkit, configure)
    # A narrowed run builds only what its tests read, not every program in the tree.
    selection: list[str] = ctest_selection(tests, skip_labels)
    targets: list[str] | None = None
    if tests or changed_from:
        listing: dict = test_listing(toolkit, tests, skip_labels)
        if changed_from:
            changed_listing: dict | None = changed_tests_listing(changed, listing)
            if changed_listing is not None:
                listing = changed_listing
                if not listing['tests']:
                    print(f'no test is affected by the paths changed since {changed_from}; nothing to build or run',
                          flush=True)
                    return
                names: list[str] = []
                test: dict
                for test in listing['tests']:
                    names.append(test['name'])
                print(f'{len(names)} tests are affected by the paths changed since {changed_from}', flush=True)
                selection = ctest_selection(change_selection.name_pattern(names), skip_labels)
        targets = selected_test_targets(toolkit, listing)
    if targets is None:
        run('cmake', '--build', toolkit, '--parallel', jobs)
    elif targets:
        print(f'{len(targets)} build outputs serve the selected tests', flush=True)
        run('cmake', '--build', toolkit, '--parallel', jobs, '--target', *targets)
    else:
        print('the selected tests read no build output; nothing to build', flush=True)
    os.environ['GUI_FORMS_FONT_DIR'] = str(ROOT / 'gui_forms/assets/fonts')
    if sanitize:
        run('ctest', '--test-dir', toolkit, '--output-on-failure', '--timeout', timeout,
            '--parallel', test_jobs, *selection)
        return
    # The JUnit report names the passed tests the cache manifest records
    # (tools/native_tests.py); the programs job does not run them again.
    run('ctest', '--test-dir', toolkit, '--output-on-failure', '--timeout', timeout,
        '--parallel', test_jobs, *selection, '--output-junit', native_tests.JUNIT_NAME)
    run('cmake', '--install', toolkit)
    check_installed_notices(sdk)
    if host == 'windows':
        check_windows_sdk_imports(sdk)
    examples: Path = build / 'installed-reference-examples'
    run('cmake', '-S', ROOT / 'gui_forms/examples/reference', '-B', examples,
        '-G', 'Ninja', '-DCMAKE_BUILD_TYPE=Release', f'-DCMAKE_PREFIX_PATH={sdk}',
        '-DCMAKE_TOOLCHAIN_FILE=' + str(ROOT / 'gui_forms/cmake/llvm22.cmake'))
    run('cmake', '--build', examples, '--parallel', jobs)
    run('ctest', '--test-dir', examples, '--output-on-failure')


def main() -> None:
    parser: argparse.ArgumentParser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--jobs', type=int, default=4)
    parser.add_argument('--test-jobs', type=int, default=1,
                        help='tests ctest runs at once (default 1: one after another)')
    parser.add_argument('--debug-info', choices=tuple(DEBUG_INFO_FLAGS), default='',
                        help='with --sanitize, the debug information: lines (-gline-tables-only, the default) '
                             'or full (-g); also ' + DEBUG_INFO_VARIABLE)
    parser.add_argument('--skip-labels', default='',
                        help='leave out tests whose labels match this ctest -LE pattern, such as '
                             'gallery_accessibility (the Gallery-wide accessibility tests); CI passes none')
    parser.add_argument('--sanitize', action='store_true',
                        help='build in gui-forms-sanitize with ASan and UBSan (and LSan on Linux '
                             'and macOS) and only test')
    parser.add_argument('--tests', default='',
                        help='with --sanitize, build and run only what the tests matching this ctest -R pattern need')
    parser.add_argument('--changed-from', default='',
                        help='with --sanitize, build and run only the tests the paths changed since this ref '
                             'can affect (tools/change_selection.py); every test when a path touches everything')
    parser.add_argument('--gallery', action='store_true',
                        help='also configure GUI_FORMS_BUILD_GALLERY=ON '
                             '(or set GUI_FORMS_CI_BUILD_GALLERY=1); default OFF')
    args: argparse.Namespace = parser.parse_args()
    if args.jobs < 1:
        parser.error('--jobs must be positive')
    if args.test_jobs < 1:
        parser.error('--test-jobs must be positive')
    if args.tests and not args.sanitize:
        parser.error('--tests narrows the sanitized run only')
    if args.changed_from and not args.sanitize:
        parser.error('--changed-from narrows the sanitized run only')
    if args.debug_info and not args.sanitize:
        parser.error('--debug-info applies to the sanitized build only')
    debug_info: str = 'lines'
    try:
        debug_info = debug_info_mode(args.debug_info, dict(os.environ))
    except ValueError as error:
        parser.error(str(error))
    hosts: dict[str, str] = {'Darwin': 'macos', 'Linux': 'linux', 'Windows': 'windows'}
    host: str = hosts[platform.system()]
    architecture: str = 'x64'
    if platform.machine().lower() in ('arm64', 'aarch64'):
        architecture = 'arm64'
    build: Path = ROOT / '.build' / ('native-' + host + '-' + architecture)
    build.mkdir(parents=True, exist_ok=True)
    os.environ['BUILD_JOBS'] = str(args.jobs)
    build_toolkit(host, build, build / 'gui-forms-sdk', args.jobs,
                  gallery_enabled(args.gallery, dict(os.environ)), args.sanitize, args.tests,
                  args.test_jobs, debug_info, args.skip_labels, args.changed_from)


if __name__ == '__main__':
    main()
