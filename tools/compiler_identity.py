#!/usr/bin/env python3
"""Emit a compiler/SDK identity; source identity is a separate cache dimension."""
import hashlib
import os
from pathlib import Path
import platform
import subprocess
from typing import TextIO


def main() -> None:
    commands: list[list[str]] = [['clang', '--version'], ['ccache', '--version']]
    if platform.system() == 'Darwin':
        commands.append(['xcrun', '--show-sdk-version'])
    values: list[str] = [os.environ.get('ImageVersion', ''), platform.machine()]
    command: list[str]
    for command in commands:
        values.append(subprocess.check_output(command, text=True))
    text: str = '\n'.join(values)
    digest: str = hashlib.sha256(text.encode('utf-8')).hexdigest()
    destination: Path = Path(os.environ['GITHUB_OUTPUT'])
    stream: TextIO
    with destination.open('a', encoding='utf-8') as stream:
        stream.write('identity=' + digest + '\n')


if __name__ == '__main__':
    main()
