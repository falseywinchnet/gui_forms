#!/usr/bin/env python3
"""Select one numeric publication version before any native build.

The baseline is `project(GUIForms VERSION major.minor.patch)` in CMakeLists.txt.
A push to main publishes major.minor.(patch + run number), so reruns of one
workflow run keep one version and consecutive patch numbers are not promised.
A `v*` tag must name the baseline series (same major.minor) and may not predate
the baseline patch. Manual dispatch is a dry build unless publish=true, and
publishing by dispatch is restricted to main. Pull requests build the baseline.
Every component fits a 16-bit installer field (Windows VIProductVersion).
"""
from __future__ import annotations

import argparse
import json
import os
import re
from pathlib import Path

ROOT: Path = Path(__file__).resolve().parents[1]
MAIN_REF: str = "refs/heads/main"
TAG_PREFIX: str = "refs/tags/v"
COMPONENT_LIMIT: int = 65535


def validate(value: str) -> tuple[int, int, int]:
    """Return the three numeric components of a canonical major.minor.patch."""
    if not re.fullmatch(r"(?:0|[1-9][0-9]*)\.(?:0|[1-9][0-9]*)\.(?:0|[1-9][0-9]*)", value):
        raise ValueError("Release version must be a canonical numeric major.minor.patch: " + repr(value))
    major, minor, patch = (int(part) for part in value.split("."))
    if max(major, minor, patch) > COMPONENT_LIMIT:
        raise ValueError("Release version exceeds the installer component range: " + value)
    return major, minor, patch


def baseline_from_text(source: str) -> str:
    match = re.search(r"^\s*project\(\s*\w+\s+VERSION\s+([0-9][0-9.]*)", source, re.MULTILINE)
    if match is None:
        raise ValueError("CMakeLists.txt has no project(... VERSION ...) baseline")
    version: str = match.group(1)
    validate(version)
    return version


def baseline(root: Path = ROOT) -> str:
    return baseline_from_text((root / "CMakeLists.txt").read_text(encoding="utf-8"))


def select(base: str, event: str, ref: str, run_number: int, requested: bool = False) -> dict[str, str]:
    major, minor, patch = validate(base)
    publish: bool = event == "push" and (ref == MAIN_REF or ref.startswith(TAG_PREFIX))
    if event == "workflow_dispatch" and requested:
        if ref != MAIN_REF:
            raise ValueError("Manual publication is restricted to main")
        publish = True
    if ref.startswith("refs/tags/"):
        if not ref.startswith(TAG_PREFIX):
            raise ValueError("Expected a v-prefixed release tag, got " + ref)
        version: str = ref[len(TAG_PREFIX):]
        tagged = validate(version)
        if tagged[:2] != (major, minor) or tagged[2] < patch:
            raise ValueError(
                f"Release tag v{version} must match the baseline series {major}.{minor}.x "
                f"and not predate patch {patch}")
    elif publish:
        if run_number <= 0:
            raise ValueError("Publication requires a positive workflow run number")
        version = f"{major}.{minor}.{patch + run_number}"
    else:
        version = base
    validate(version)
    return {"version": version, "tag": "v" + version, "publish": str(publish).lower()}


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--root", type=Path, default=ROOT)
    args = parser.parse_args()
    data: dict[str, str] = select(
        baseline(args.root),
        os.environ.get("GITHUB_EVENT_NAME", "local"),
        os.environ.get("GITHUB_REF", ""),
        int(os.environ.get("GITHUB_RUN_NUMBER", "0")),
        os.environ.get("PUBLISH_REQUESTED") == "true",
    )
    output: str | None = os.environ.get("GITHUB_OUTPUT")
    if output:
        with open(output, "a", encoding="utf-8") as stream:
            for key, value in data.items():
                stream.write(f"{key}={value}\n")
    print(json.dumps(data))


if __name__ == "__main__":
    main()
