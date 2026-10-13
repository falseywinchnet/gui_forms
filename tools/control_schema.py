"""Check and render the GUI.Forms control schema (manifests/control-schema.json).

The control schema is the stage T1 projection of the run-time registry: every
registry type's parts, states, item systems, properties and interactions.
``gui_forms_gallery_pages_tests`` writes it from a laid-out Gallery and fails
when the committed copy is stale. This module has two jobs:

- ``check``: prove the manifest names exactly the types of
  ``manifests/controls.json`` in the same order, that every property and
  part name is snake_case, and that every type's
  interactions are exactly the public events its header declares, base types
  first (so a new event cannot be left out of the schema);
- ``render``: write ``docs/controls/generated/schema.md`` from the manifest,
  or prove the committed page is current with ``--check``.

Standard library only; Python 3.12+.
"""

from __future__ import annotations

import argparse
import json
import re
import sys
from collections.abc import Mapping, Sequence
from dataclasses import dataclass
from pathlib import Path

import control_registry

ROOT: Path = Path(__file__).resolve().parent.parent
SCHEMA_PATH: Path = ROOT / "manifests" / "control-schema.json"
INCLUDE_ROOT: Path = ROOT / "include"
PAGE_PATH: Path = ROOT / "docs" / "controls" / "generated" / "schema.md"

_CLASS_HEAD: re.Pattern[str] = re.compile(r"^(class|struct)\s+(\w+)(\s+final)?\s*(:[^{;]*)?\{", re.M)
_ACCESS: re.Pattern[str] = re.compile(r"(public|protected|private)\s*:")
_ACCESSOR_TAIL: re.Pattern[str] = re.compile(r"\s*&\s*(\w+)\s*\(\s*\)")
_SNAKE: re.Pattern[str] = re.compile(r"[a-z][a-z0-9]*(_[a-z0-9]+)*")


class SchemaError(Exception):
    """The manifest cannot be read."""


@dataclass(frozen=True)
class Interaction:
    name: str
    payload: str
    mutable_payload: bool


@dataclass(frozen=True)
class ScannedClass:
    bases: tuple[str, ...]
    events: tuple[Interaction, ...]


# --------------------------------------------------------------------------- scan


def _matching(text: str, start: int, opening: str, closing: str) -> int:
    """Index of the bracket closing the one at ``start``, or -1."""
    depth: int = 0
    for index in range(start, len(text)):
        if text[index] == opening:
            depth += 1
        elif text[index] == closing:
            depth -= 1
            if depth == 0:
                return index
    return -1


def _split_arguments(text: str) -> list[str]:
    arguments: list[str] = []
    depth: int = 0
    current: list[str] = []
    for character in text:
        if character == "<":
            depth += 1
        elif character == ">":
            depth -= 1
        if character == "," and depth == 0:
            arguments.append("".join(current).strip())
            current = []
        else:
            current.append(character)
    tail: str = "".join(current).strip()
    if tail:
        arguments.append(tail)
    return arguments


def interaction_from_accessor(accessor: str, arguments: str) -> Interaction:
    """The schema record of ``Event<arguments>& accessor()``.

    The name is the accessor's name; the payload lists the argument
    types without ``const`` or reference marks; the payload is mutable when an
    argument is a non-const reference.
    """
    spelled: list[str] = []
    mutable: bool = False
    for argument in _split_arguments(" ".join(arguments.split())):
        constant: bool = argument.startswith("const ")
        bare: str = argument[len("const "):] if constant else argument
        reference: bool = bare.endswith("&")
        bare = bare.rstrip("&").strip()
        if reference and not constant:
            mutable = True
        spelled.append(bare)
    return Interaction(accessor, ", ".join(spelled), mutable)


def _public_events(body: str, public_by_default: bool) -> list[Interaction]:
    """Public ``Event<...>& name()`` accessors declared directly in a class body."""
    events: list[Interaction] = []
    public: bool = public_by_default
    depth: int = 0
    index: int = 0
    while index < len(body):
        character: str = body[index]
        if character == "{":
            depth += 1
        elif character == "}":
            depth -= 1
        elif depth == 0 and (index == 0 or not (body[index - 1].isalnum() or body[index - 1] == "_")):
            access: re.Match[str] | None = _ACCESS.match(body, index)
            if access is not None:
                public = access.group(1) == "public"
                index = access.end()
                continue
            if body.startswith("Event<", index):
                close: int = _matching(body, index + len("Event"), "<", ">")
                tail: re.Match[str] | None = _ACCESSOR_TAIL.match(body, close + 1)
                if tail is not None and public:
                    events.append(interaction_from_accessor(tail.group(1), body[index + len("Event<"):close]))
                index = close + 1
                continue
        index += 1
    return events


def scan_classes(include_root: Path = INCLUDE_ROOT) -> dict[str, ScannedClass]:
    """Every namespace-level class under include/gui_forms with its bases and public events."""
    classes: dict[str, ScannedClass] = {}
    for header in sorted((include_root / "gui_forms").rglob("*.hpp")):
        source: str = control_registry._strip_comments(header.read_text(encoding="utf-8"))
        for match in _CLASS_HEAD.finditer(source):
            name: str = match.group(2)
            close: int = _matching(source, match.end() - 1, "{", "}")
            body: str = source[match.end():close]
            events: list[Interaction] = _public_events(body, match.group(1) == "struct")
            bases: tuple[str, ...] = control_registry._base_names(match.group(4))
            if name not in classes or events or bases:
                classes[name] = ScannedClass(bases=bases, events=tuple(events))
    return classes


def expected_interactions(type_name: str, classes: Mapping[str, ScannedClass]) -> list[Interaction]:
    """A type's interactions as its headers declare them: every base's, then its own."""
    scanned: ScannedClass | None = classes.get(type_name)
    if scanned is None:
        return []
    collected: list[Interaction] = []
    for base in scanned.bases:
        collected.extend(expected_interactions(base, classes))
    collected.extend(scanned.events)
    return collected


# -------------------------------------------------------------------------- check


def load_schema(path: Path = SCHEMA_PATH) -> dict[str, object]:
    try:
        raw: object = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        raise SchemaError(f"{path}: {error}") from error
    if not isinstance(raw, dict) or not isinstance(raw.get("types"), list):
        raise SchemaError(f"{path}: expected an object with a 'types' list")
    return raw


def _interactions_of(record: Mapping[str, object]) -> list[Interaction]:
    found: list[Interaction] = []
    for raw in record.get("interactions", []):
        found.append(Interaction(str(raw["name"]), str(raw["payload"]), bool(raw["mutable_payload"])))
    return found


def validate(schema: Mapping[str, object], registry: control_registry.Registry,
             classes: Mapping[str, ScannedClass]) -> list[str]:
    """Every finding; an empty list means the manifest is consistent."""
    findings: list[str] = []
    types: list[Mapping[str, object]] = list(schema["types"])  # type: ignore[arg-type]
    registry_ids: list[str] = [record.id for record in registry.records]
    schema_ids: list[str] = [str(record.get("id")) for record in types]
    if schema_ids != registry_ids:
        missing: list[str] = [item for item in registry_ids if item not in schema_ids]
        extra: list[str] = [item for item in schema_ids if item not in registry_ids]
        findings.append("the schema's types differ from manifests/controls.json "
                        f"(missing: {missing or 'none'}; extra: {extra or 'none'}; "
                        "or the order differs)")
    by_id: dict[str, control_registry.ControlRecord] = {record.id: record for record in registry.records}
    for record in types:
        identity: str = str(record.get("id"))
        registered: control_registry.ControlRecord | None = by_id.get(identity)
        if registered is not None and record.get("type") != registered.type:
            findings.append(f"{identity}: type {record.get('type')!r} is not {registered.type!r}")
        kind: object = record.get("kind")
        if kind == "control" and not record.get("parts"):
            findings.append(f"{identity}: a control declares no parts")
        for item in record.get("properties", []):  # type: ignore[union-attr]
            if not _SNAKE.fullmatch(str(item.get("name"))):
                findings.append(f"{identity}: property {item.get('name')!r} is not snake_case")
        for part in record.get("parts", []):  # type: ignore[union-attr]
            if not _SNAKE.fullmatch(str(part.get("name"))):
                findings.append(f"{identity}: part {part.get('name')!r} is not snake_case")
        expected: list[Interaction] = expected_interactions(str(record.get("type")), classes)
        exported: list[Interaction] = _interactions_of(record)
        if exported != expected:
            missing_names: list[str] = [item.name for item in expected if item not in exported]
            extra_names: list[str] = [item.name for item in exported if item not in expected]
            findings.append(f"{identity}: interactions differ from the public events in the headers "
                            f"(not exported: {missing_names or 'none'}; not in headers: "
                            f"{extra_names or 'none'}; or the order or a payload differs)")
    return findings


# ------------------------------------------------------------------------- render


def _code(text: str) -> str:
    return f"`{text}`"


def _states_text(states: Sequence[object]) -> str:
    return ", ".join(_code(str(state)) for state in states) if states else "none"


def _parts_table(parts: Sequence[Mapping[str, object]]) -> list[str]:
    lines: list[str] = ["| Part | Role | Reuses | Own states |", "|---|---|---|---|"]
    for part in parts:
        reuses: object = part.get("composed_from")
        own: Sequence[object] = part.get("states", [])  # type: ignore[assignment]
        lines.append(f"| {_code(str(part['name']))} | {part['role']} | "
                     f"{_code(str(reuses)) if reuses else ''} | "
                     f"{', '.join(str(state) for state in own)} |")
    return lines


def _common_names(types: Sequence[Mapping[str, object]], key: str) -> list[str]:
    """Names every control lists under ``key``, in the first control's order."""
    controls: list[Mapping[str, object]] = [record for record in types if record.get("kind") == "control"]
    if not controls:
        return []
    first: list[str] = [str(item["name"]) for item in controls[0].get(key, [])]  # type: ignore[union-attr]
    common: list[str] = []
    for name in first:
        if all(any(str(item["name"]) == name for item in record.get(key, [])) for record in controls):  # type: ignore[union-attr]
            common.append(name)
    return common


def render_page(schema: Mapping[str, object]) -> str:
    types: list[Mapping[str, object]] = list(schema["types"])  # type: ignore[arg-type]
    common_properties: list[str] = _common_names(types, "properties")
    common_interactions: list[str] = _common_names(types, "interactions")
    lines: list[str] = [
        "# Control schema",
        "",
        "Generated from `manifests/control-schema.json` by `python3 tools/control_schema.py render`;",
        "do not edit. The manifest is written by `gui_forms_gallery_pages_tests` from the run-time",
        "registry (stage T1), so this page lists what the library publishes.",
        "",
        "Every name here is a registered name, stored in snake_case (owner decision 8). Theme",
        "and form documents, which are CSS-family, write the same words in kebab-case",
        "(`scroll_thumb` is `::part(scroll-thumb)`, `auto_size_mode` is `auto-size-mode`);",
        "PropertyGrid and C# show them in PascalCase (`AutoSizeMode`). C++ reads the anatomy as",
        "typed records (`(*bar).parts().thumb`) and registers handlers through each type's events",
        "(`(*button).clicked()`). An interaction's payload is the handler's argument types; *by",
        "reference* marks a payload the handler receives by mutable reference.",
        "",
        "Every control also has these properties: "
        + ", ".join(_code(name) for name in common_properties) + ".",
        "",
        "Every control also raises these interactions: "
        + ", ".join(_code(name) for name in common_interactions) + ".",
        "",
    ]
    for record in types:
        identity: str = str(record["id"])
        lines.append(f"## {_code(identity)} ({record['type']}, {record['kind']})")
        lines.append("")
        if record.get("kind") == "control":
            lines.extend(_parts_table(record.get("parts", [])))  # type: ignore[arg-type]
            lines.append("")
            states: list[str] = [str(state) for state in record.get("states", [])]  # type: ignore[union-attr]
            custom: list[str] = [str(state) for state in record.get("custom_states", [])]  # type: ignore[union-attr]
            lines.append(f"States: {_states_text(states)}."
                         + (f" Application-defined: {_states_text(custom)}." if custom else ""))
            lines.append("")
            for system in record.get("item_systems", []):  # type: ignore[union-attr]
                lines.append(f"Item system {_code(str(system['name']))}:")
                lines.append("")
                lines.extend(_parts_table(system.get("parts", [])))
                lines.append("")
                item_custom: list[str] = [str(state) for state in system.get("custom_states", [])]
                lines.append(f"Item states: {_states_text(system.get('states', []))}."
                             + (f" Application-defined: {_states_text(item_custom)}." if item_custom else ""))
                lines.append("")
            own_properties: list[str] = [
                f"{_code(str(item['name']))} ({item['kind']})"
                for item in record.get("properties", [])  # type: ignore[union-attr]
                if str(item["name"]) not in common_properties]
            lines.append("Properties beyond every control's: "
                         + (", ".join(own_properties) if own_properties else "none") + ".")
            lines.append("")
        own_interactions: list[Mapping[str, object]] = [
            item for item in record.get("interactions", [])  # type: ignore[union-attr]
            if record.get("kind") != "control" or str(item["name"]) not in common_interactions]
        if own_interactions:
            lines.append("| Interaction | Payload |")
            lines.append("|---|---|")
            for item in own_interactions:
                payload: str = _code(str(item["payload"])) if item["payload"] else "none"
                if item["mutable_payload"]:
                    payload += ", by reference"
                lines.append(f"| {_code(str(item['name']))} | {payload} |")
        else:
            lines.append("Interactions beyond every control's: none." if record.get("kind") == "control"
                         else "Interactions: none.")
        lines.append("")
    return "\n".join(lines)


# ---------------------------------------------------------------------------- cli


def _command_check(args: argparse.Namespace) -> int:
    schema: dict[str, object] = load_schema(args.schema)
    registry: control_registry.Registry = control_registry.load_registry()
    findings: list[str] = validate(schema, registry, scan_classes())
    for finding in findings:
        print(f"control schema: {finding}", file=sys.stderr)
    if findings:
        return 1
    print(f"control schema: {len(schema['types'])} types OK")  # type: ignore[arg-type]
    return 0


def _command_render(args: argparse.Namespace) -> int:
    page: str = render_page(load_schema(args.schema))
    if args.check:
        current: str = args.out.read_text(encoding="utf-8") if args.out.exists() else ""
        if current.replace("\r\n", "\n") != page:
            print(f"control schema: {args.out} is stale; run `python3 tools/control_schema.py render`",
                  file=sys.stderr)
            return 1
        print(f"control schema: {args.out} is current")
        return 0
    args.out.write_text(page, encoding="utf-8", newline="\n")
    print(f"control schema: wrote {args.out}")
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--schema", type=Path, default=SCHEMA_PATH, help="control schema manifest")
    subparsers = parser.add_subparsers(dest="command", required=True)
    check = subparsers.add_parser("check", help="validate the manifest against the registry and headers")
    check.set_defaults(run=_command_check)
    render = subparsers.add_parser("render", help="write or check docs/controls/generated/schema.md")
    render.add_argument("--out", type=Path, default=PAGE_PATH, help="output page")
    render.add_argument("--check", action="store_true", help="fail when the committed page is stale")
    render.set_defaults(run=_command_render)
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args: argparse.Namespace = build_parser().parse_args(argv)
    try:
        return int(args.run(args))
    except SchemaError as error:
        print(f"control schema: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
