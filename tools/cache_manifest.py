#!/usr/bin/env python3
"""Describe a successful cache producer; this is not a consumer test receipt."""
import json
import hashlib
import platform
import re
import shutil
import os
from pathlib import Path
import subprocess
import sys


def main() -> None:
    source: Path = Path(__file__).resolve().parents[1]
    revision_text: str = subprocess.check_output(
        ['git', '-C', str(source), 'rev-parse', 'HEAD'], text=True)
    revision: str = revision_text.strip()
    compiler_path: str | None = shutil.which('clang')
    if compiler_path is None:
        raise RuntimeError('clang is missing')
    compiler_text: str = subprocess.check_output(['clang', '--version'], text=True)
    match: re.Match[str] | None = re.search(r'clang version (22\.1\.[0-9]+)', compiler_text)
    if match is None:
        raise RuntimeError('The cache requires LLVM 22.1.x')
    manifest: dict[str, object] = {
        'schema': 2, 'repository': 'falseywinchnet/gui_forms',
        'revision': revision, 'platform': sys.argv[1], 'compiler': sys.argv[2],
        'compiler_version': match.group(1), 'compiler_version_text': compiler_text,
        'compiler_sha256': hashlib.sha256(Path(compiler_path).read_bytes()).hexdigest(),
        'compiler_launchers': {'CC': 'clang', 'CXX': 'clang++', 'OBJCXX': 'clang++', 'launcher': 'ccache'},
        'runner': os.environ.get('GUI_FORMS_RUNNER', ''),
        'runner_image_version': os.environ.get('ImageVersion', ''),
        'architecture': platform.machine(),
        'layout': {'source': 'gui_forms/', 'build': '.build/native-' + sys.argv[1] + '/gui-forms/'},
        'ccache': {'basedir': 'workspace root containing gui_forms/', 'compiler_check': 'content',
                   'version': subprocess.check_output(['ccache', '--version'], text=True)},
        'minimum_platform': '14.0' if sys.argv[1] == 'macos-arm64' else
                            ('Windows 10 (0x0A00)' if sys.argv[1] == 'windows-x64' else 'Ubuntu 24.04'),
        'run_id': os.environ.get('GITHUB_RUN_ID', ''),
        'profile': 'native Release plus separately tested development text/host profiles',
        'native_options': {'build_type': 'Release', 'cxx_standard': 20, 'pic': True,
                           'audio': True, 'tests': True, 'gallery': False,
                           'skia': sys.argv[1] != 'windows-x64',
                           'harfbuzz': sys.argv[1] != 'windows-x64'},
        'validation': 'provider CTest passed; consumer compilation and tests remain required'}
    if sys.argv[1] == 'macos-arm64':
        manifest['sdk_version'] = subprocess.check_output(['xcrun', '--show-sdk-version'], text=True).strip()
        runtime: Path = Path(os.environ['GUI_FORMS_LLVM_RUNTIME'])
        manifest['runtime'] = json.loads((runtime / 'runtime-manifest.json').read_text(encoding='utf-8'))
        manifest['runtime_layout'] = '.build/toolchain/llvm-22.1.8-macos14'
    destination: Path = source.parent / 'cache-manifest.json'
    destination.write_text(json.dumps(manifest, indent=2) + '\n', encoding='utf-8')


if __name__ == '__main__':
    main()
