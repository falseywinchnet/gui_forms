#!/usr/bin/env python3
"""The toolkit's configure options and host preparation, without the harness.

tools/ci_build.py (CI, through template_harness) and tools/build_sdk.py (a
consumer's build of the source SDK, which carries no harness) configure
GUI.Forms with these, so the SDK a consumer builds is the build CI tests.
This module therefore imports nothing from template_harness: run and the
pinned-checkout check are its own, and match harness/ci_build_steps.py's.
"""
from __future__ import annotations
import os
from pathlib import Path
import re
import subprocess


def run(*arguments: str | Path | int) -> None:
    command: list[str] = []
    argument: str | Path | int
    for argument in arguments:
        command.append(str(argument))
    print(' '.join(command), flush=True)
    subprocess.run(command, check=True)


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


# fetch_linebreak.sh pins its one checkout as revision=<commit>.
LINE_BREAK_PIN_PATTERN: re.Pattern[str] = re.compile(r'^revision=([0-9a-f]{40})$', re.MULTILINE)


def fetch_line_break(script: Path) -> None:
    """Fetch libunibreak, which the text engine links (owner, 2026-10-26).

    Skipped, as fetch_text_stack is, when the checkout is already at the
    pinned commit; the script would only re-run `git checkout --detach`.
    """
    pins: list[str] = LINE_BREAK_PIN_PATTERN.findall(script.read_text(encoding='utf-8'))
    checkout: Path = script.parent / 'libunibreak'
    if len(pins) == 1 and (checkout / '.git').exists():
        completed: subprocess.CompletedProcess[str] = subprocess.run(
            ['git', '-C', str(checkout), 'rev-parse', 'HEAD'], capture_output=True, text=True)
        if completed.returncode == 0 and completed.stdout.strip() == pins[0]:
            print(f'{script.name}: the checkout is at its pinned commit', flush=True)
            return
    run('sh', script)


HOST_OPTION_NAMES: dict[str, str] = {'WINDOWS': 'windows', 'MACOS': 'macos', 'LINUX': 'linux'}


def prepare_host(host: str, source: Path, skia_out: Path, fetch_text: bool) -> None:
    """Fetch and build what host's configure reads, before configuring source.

    On macOS and Linux that is the pinned Skia source, built into skia_out;
    with fetch_text, also the pinned text sources (HarfBuzz, FreeType and
    libunibreak). A source SDK archive already holds the text sources
    (tools/build_sdk.py passes fetch_text False). On Linux it sets
    GUI_FORMS_SYSTEM_BUILD_TOOLS=1 in this process's environment, so Skia
    builds with the system gn and ninja.
    """
    third_party: Path = source / 'third_party'
    if host == 'windows':
        # ADR-004: the Win32 host draws text with the shared HarfBuzz/FreeType engine.
        if fetch_text:
            fetch_text_stack(third_party / 'fetch_text_stack.sh')
            fetch_line_break(third_party / 'fetch_linebreak.sh')
        return
    if host == 'linux':
        os.environ['GUI_FORMS_SYSTEM_BUILD_TOOLS'] = '1'
    run('sh', third_party / 'fetch_skia_cpu.sh')
    if fetch_text:
        fetch_text_stack(third_party / 'fetch_text_stack.sh')
        fetch_line_break(third_party / 'fetch_linebreak.sh')
    if host == 'linux':
        run('sh', third_party / 'build_skia_cpu_linux.sh', skia_out)
    else:
        run('sh', third_party / 'build_skia_cpu.sh', skia_out)


def host_options(host: str, skia_out: Path) -> list[str]:
    """The configure options that select host's native host and renderer.

    Windows draws with the Win32 host and no Skia; macOS and Linux link the
    Skia archives prepare_host built into skia_out. Every host uses the
    HarfBuzz/FreeType text engine.
    """
    options: list[str] = []
    name: str
    for name in HOST_OPTION_NAMES:
        enabled: str = 'OFF'
        if HOST_OPTION_NAMES[name] == host:
            enabled = 'ON'
        options.append(f'-DGUI_FORMS_ENABLE_{name}_HOST={enabled}')
    if host == 'windows':
        options.extend(['-DGUI_FORMS_ENABLE_SKIA=OFF', '-DGUI_FORMS_ENABLE_HARFBUZZ_TEXT=ON'])
    else:
        options.extend(['-DGUI_FORMS_ENABLE_SKIA=ON', '-DGUI_FORMS_ENABLE_HARFBUZZ_TEXT=ON',
                        '-DGUI_FORMS_SKIA_PREBUILT=ON', f'-DGUI_FORMS_SKIA_OUT={skia_out}'])
    return options


def toolkit_options(source: Path, prefix: Path, gallery: bool, tests: bool) -> list[str]:
    """The configure options every toolkit build shares, ahead of host_options.

    One Release build with the pinned LLVM toolchain file, position-
    independent code, the audio service with the stx Vorbis decoder, and
    prefix as the install prefix; gallery and tests switch the demonstration
    programs and the test suite. tools/build_sdk.py configures with these
    too (gallery and tests off), so the SDK a consumer builds is this build.
    """
    gallery_option: str = 'ON' if gallery else 'OFF'
    tests_option: str = 'ON' if tests else 'OFF'
    return ['-DCMAKE_BUILD_TYPE=Release', '-DCMAKE_INSTALL_LIBDIR=lib',
            '-DCMAKE_TOOLCHAIN_FILE=' + str(source / 'cmake/llvm22.cmake'),
            '-DCMAKE_POSITION_INDEPENDENT_CODE=ON', f'-DGUI_FORMS_BUILD_GALLERY={gallery_option}',
            f'-DGUI_FORMS_BUILD_TESTS={tests_option}', '-DGUI_FORMS_BUILD_AUDIO=ON',
            '-DGUI_FORMS_AUDIO_VORBIS_BACKEND=stx', f'-DCMAKE_INSTALL_PREFIX={prefix}']
