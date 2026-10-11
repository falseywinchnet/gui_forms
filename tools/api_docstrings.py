"""Read the hand-written numpydoc-style docstrings in docs/library/docstrings/.

Each ``<TypeName>.rst`` file documents one public type in the layout of a
numpydoc docstring: a one-line summary, optional extended paragraphs, then
sections introduced by a title underlined with dashes. Bodies are
reStructuredText; single backticks mark code and are resolved to
cross-references by the renderer.

Recognised sections:

- ``Parameters``: constructor parameters.
- ``Properties``: getter/setter pairs, named by the getter.
- ``Events``: ``Event<...>& name()`` accessors.
- ``Methods``: other public member functions.
- ``See Also``: ``Name : description`` lines.
- ``Notes`` and ``Examples``: free text.

An entry in a list section is a line at column 0, ``name`` or
``name : classifier``, followed by its description indented by four spaces.
"""
from __future__ import annotations

import re
from dataclasses import dataclass, field
from pathlib import Path

LIST_SECTIONS: tuple[str, ...] = ("Parameters", "Properties", "Events", "Methods")
TEXT_SECTIONS: tuple[str, ...] = ("Notes", "Examples")
ALL_SECTIONS: tuple[str, ...] = LIST_SECTIONS + ("See Also",) + TEXT_SECTIONS


@dataclass
class Entry:
    name: str
    classifier: str
    description: str


@dataclass
class Docstring:
    name: str
    path: Path
    summary: str
    extended: str
    sections: dict[str, list[Entry]] = field(default_factory=dict)
    see_also: list[Entry] = field(default_factory=list)
    text: dict[str, str] = field(default_factory=dict)

    def entry(self, section: str, name: str) -> Entry | None:
        for item in self.sections.get(section, []):
            if item.name == name:
                return item
        return None


class DocstringError(ValueError):
    pass


def _dedent(lines: list[str]) -> str:
    while lines and not lines[0].strip():
        lines = lines[1:]
    while lines and not lines[-1].strip():
        lines = lines[:-1]
    widths = [len(line) - len(line.lstrip(" ")) for line in lines if line.strip()]
    width = min(widths) if widths else 0
    return "\n".join(line[width:] if line.strip() else "" for line in lines)


def _entries(lines: list[str], path: Path, section: str) -> list[Entry]:
    entries: list[Entry] = []
    current: list[str] = []
    head: tuple[str, str] | None = None
    for line in lines + [""]:
        if line and not line.startswith(" "):
            if head is not None:
                entries.append(Entry(head[0], head[1], _dedent(current)))
            name, _, classifier = line.partition(" : ")
            name = name.strip()
            if not re.fullmatch(r"[A-Za-z_~][\w:<>]*", name):
                raise DocstringError(f"{path}: {section}: bad entry name {line!r}")
            head = (name, classifier.strip())
            current = []
        elif head is not None:
            current.append(line)
        elif line.strip():
            raise DocstringError(f"{path}: {section}: text before the first entry: {line!r}")
    if head is not None:
        entries.append(Entry(head[0], head[1], _dedent(current)))
    return entries


def parse(path: Path) -> Docstring:
    lines = path.read_text(encoding="utf-8").splitlines()
    # Find section titles: a known title followed by a line of dashes.
    starts: list[tuple[int, str]] = []
    for index in range(len(lines) - 1):
        title = lines[index].strip()
        underline = lines[index + 1].strip()
        if underline and set(underline) == {"-"} and title:
            if title not in ALL_SECTIONS:
                raise DocstringError(f"{path}:{index + 1}: unknown section {title!r}")
            if len(underline) < len(title):
                raise DocstringError(f"{path}:{index + 2}: underline shorter than {title!r}")
            starts.append((index, title))
    preamble = lines[:starts[0][0]] if starts else lines
    paragraphs = _dedent(preamble).split("\n\n", 1)
    summary = " ".join(paragraphs[0].split())
    if not summary:
        raise DocstringError(f"{path}: the first line must be a one-sentence summary")
    extended = paragraphs[1].strip() if len(paragraphs) > 1 else ""
    result = Docstring(path.stem, path, summary, extended)
    seen: set[str] = set()
    for position, (start, title) in enumerate(starts):
        if title in seen:
            raise DocstringError(f"{path}: section {title!r} appears twice")
        seen.add(title)
        end = starts[position + 1][0] if position + 1 < len(starts) else len(lines)
        body = lines[start + 2:end]
        if title in LIST_SECTIONS:
            result.sections[title] = _entries(body, path, title)
        elif title == "See Also":
            result.see_also = _entries(body, path, title)
        else:
            result.text[title] = _dedent(body)
    order = [title for _, title in starts]
    expected = [title for title in ALL_SECTIONS if title in order]
    if order != expected:
        raise DocstringError(f"{path}: sections must appear in the order {list(ALL_SECTIONS)}")
    return result


def load_all(directory: Path) -> dict[str, Docstring]:
    docstrings: dict[str, Docstring] = {}
    for path in sorted(directory.glob("*.rst")):
        docstring = parse(path)
        docstrings[docstring.name] = docstring
    return docstrings


def code_blocks(text: str) -> list[str]:
    """The bodies of ``.. code-block:: cpp`` directives in ``text``."""
    blocks: list[str] = []
    lines = text.splitlines()
    index = 0
    while index < len(lines):
        if lines[index].strip() == ".. code-block:: cpp":
            index += 1
            while index < len(lines) and lines[index].strip().startswith(":"):
                index += 1
            body: list[str] = []
            while index < len(lines) and (not lines[index].strip() or lines[index].startswith("   ")):
                body.append(lines[index])
                index += 1
            blocks.append(_dedent(body))
            continue
        index += 1
    return blocks
