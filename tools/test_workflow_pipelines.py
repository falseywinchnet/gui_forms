#!/usr/bin/env python3
"""Tests that a workflow step piping a checker through `tee` keeps the checker's failure.

A shell pipeline's status is its last command's, so `checker | tee log`
passes when the checker fails unless the step sets `pipefail` first. The
state-machine gate once passed with eight findings this way. This file checks
every workflow step that pipes into `tee`, and checks the behaviour itself
with a real bash.

The producer in the behaviour test is this interpreter (sys.executable), never
`python3` by name: on a Windows machine `python3` can be the Microsoft Store
alias, which exits 9009, and bash reports that as 49 (9009 modulo 256). That
was the "local bash failure in the state-machine tee test".
"""
from __future__ import annotations
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import unittest

WORKFLOWS: Path = Path(__file__).resolve().parents[1] / ".github" / "workflows"
STEP_START: re.Pattern[str] = re.compile(r"^\s*- (?:name|uses):", re.MULTILINE)
TEE: re.Pattern[str] = re.compile(r"\|\s*tee\b")


def tee_steps(workflow: str) -> list[str]:
    """The text of each step in workflow whose commands pipe into tee."""
    starts: list[int] = [match.start() for match in STEP_START.finditer(workflow)]
    starts.append(len(workflow))
    steps: list[str] = []
    index: int
    for index in range(len(starts) - 1):
        step: str = workflow[starts[index]:starts[index + 1]]
        if TEE.search(step) is not None:
            steps.append(step)
    return steps


def pipefail_precedes_every_tee(step: str) -> bool:
    """Whether `set -o pipefail` comes before the step's first pipe into tee."""
    first_tee: re.Match[str] | None = TEE.search(step)
    setting: int = step.find("set -o pipefail")
    return first_tee is not None and 0 <= setting < first_tee.start()


class WorkflowPipelineTests(unittest.TestCase):
    def test_every_tee_step_sets_pipefail_first(self) -> None:
        checked: int = 0
        path: Path
        for path in sorted(WORKFLOWS.glob("*.yml")):
            step: str
            for step in tee_steps(path.read_text(encoding="utf-8")):
                checked += 1
                first_line: str = step.strip().splitlines()[0]
                self.assertTrue(pipefail_precedes_every_tee(step), f"{path.name}: {first_line}")
        # The state-machine gate pipes through tee; an empty scan is not a pass.
        self.assertGreater(checked, 0)

    def test_the_rule_rejects_a_step_without_pipefail(self) -> None:
        step: str = "      - name: Check\n        run: |\n          checker | tee out.txt\n"
        self.assertEqual(tee_steps(step), [step])
        self.assertFalse(pipefail_precedes_every_tee(step))
        late: str = "      - name: Check\n        run: |\n          checker | tee out.txt\n          set -o pipefail\n"
        self.assertFalse(pipefail_precedes_every_tee(late))

    def test_pipefail_keeps_the_producer_failure_through_tee(self) -> None:
        bash: str | None = shutil.which("bash")
        if bash is None:
            self.skipTest("no bash on PATH")
        with tempfile.TemporaryDirectory() as directory:
            output: Path = Path(directory) / "findings.txt"
            producer: str = Path(sys.executable).as_posix()
            command: str = ("set -o pipefail\n'" + producer + "' -c 'print(\"finding\"); raise SystemExit(1)'"
                            " | tee '" + output.as_posix() + "'")
            completed: subprocess.CompletedProcess[str] = subprocess.run(
                [bash, "-c", command], capture_output=True, text=True, encoding="utf-8", check=False)
            self.assertEqual(completed.returncode, 1, completed.stderr)
            self.assertEqual(output.read_text(encoding="utf-8").replace("\r\n", "\n"), "finding\n")


if __name__ == "__main__":
    unittest.main()
