"""Summarize a native verification results directory as Markdown.

run_macos.sh and run_linux.sh call it; it reads every JSON result the
probes wrote under the directory and prints one table row per result, with
the test suite's summary line first. It changes nothing.
"""

import json
from pathlib import Path
import sys


def suite_line(results: Path) -> str:
    report: Path = results / 'ctest.txt'
    if not report.is_file():
        return 'not run'
    line: str
    for line in report.read_text(errors='replace').splitlines():
        if 'tests passed' in line or 'tests failed' in line:
            return line.strip()
    return 'no summary line (see ctest.txt)'


def describe(document: dict[str, object]) -> tuple[str, str, str]:
    probe: str = str(document.get('probe', '?'))
    if probe == 'frame_drag_forensic':
        name: str = 'frame drag ' + str(document.get('profile', '?')) + ' (' + str(document.get('input_path', '?')) + ')'
        result: str = str(document.get('verdict', '?'))
        detail: str = str(document.get('error', ''))
        latency: object = document.get('latency_ms', {})
        if isinstance(latency, dict) and not detail:
            frame: object = latency.get('request_to_frame', {})
            if isinstance(frame, dict):
                detail = 'request to frame p95 ' + str(frame.get('p95', '?')) + ' ms'
        return name, result, detail
    if probe == 'session':
        return 'session', str(document.get('result', '?')), str(document.get('reason', ''))
    name = str(document.get('check', '?'))
    return name, str(document.get('result', '?')), str(document.get('reason', ''))


def main() -> int:
    if len(sys.argv) != 2:
        print('usage: summarize.py RESULTS_DIRECTORY', file=sys.stderr)
        return 2
    results: Path = Path(sys.argv[1])
    print('# Native verification results\n')
    environment: Path = results / 'environment.txt'
    if environment.is_file():
        print('```text')
        print(environment.read_text(errors='replace').rstrip())
        print('```\n')
    print('Test suite: ' + suite_line(results) + '\n')
    print('| Where | Check | Result | Detail | File |')
    print('|---|---|---|---|---|')
    path: Path
    for path in sorted(results.rglob('*.json')):
        relative: Path = path.relative_to(results)
        where: str = relative.parts[0] if len(relative.parts) > 1 else '.'
        try:
            document: object = json.loads(path.read_text())
        except (OSError, ValueError) as error:
            print('| ' + where + ' | ' + path.name + ' | unreadable | ' + str(error) + ' | ' + relative.as_posix() + ' |')
            continue
        if not isinstance(document, dict):
            continue
        name, result, detail = describe(document)
        detail = detail.replace('|', '/')
        print('| ' + where + ' | ' + name + ' | ' + result + ' | ' + detail + ' | ' + relative.as_posix() + ' |')
    return 0


if __name__ == '__main__':
    sys.exit(main())
