"""Report state machines with undefined or unhandled states (house style sections 1 and 9).

Section 9: "A switch over a closed set still has a default, and a state machine
still says what an unexpected input does in each state", sending every such
case to one defined fallback (a named failure, a reset, or an input ignored
for a stated reason). Section 1: prefer a shape that cannot hold contradictory
values.

Findings come from the compiler, which knows which conditions are
enumerations and which enumerators a switch leaves out. The checker runs
`clang++ -fsyntax-only` with only -Wswitch, -Wswitch-enum and -Wswitch-default
enabled over the commands of a configured ninja tree (`ninja -t compdb`):

  F1 switch-without-default  a switch has no default label.
  F2 default-hides-cases      an enumeration switch leaves enumerators to its
                              default, and the default states no reason (no
                              comment, no named failure); or the switch is a
                              vocabulary mapping, which must list every
                              enumerator whatever its default says.

A vocabulary mapping is a switch over an enumeration declared in one of
VOCABULARY_HEADERS (the atom vocabulary: PartRole, AnatomyState, SemanticRole,
PartMark, ControlVisualRole and their neighbours) in which every case group's
body only translates: a single `return`, or a single assignment then `break`.
A new atom must then force a decision there.

Review candidates come from a source parser (comments and literals masked as
in check_house_style.py). They are heuristics and never fail a run:

  C1 input-ignored            a case or default whose body is empty, a bare
                              break, return or continue, with no comment.
  C2 chain-without-fallback   an if / else if chain comparing one expression
                              with two or more enumerators and no final else.
  C3 contradictory-shape      a record holding a flag beside the value it
                              qualifies (`has_x` with `x`, including a mirrored
                              std::optional), or antonym or lifecycle flags
                              that one enumeration would express.

A comment on a candidate's line or the line above clears C1 to C3; F2's
reason is a comment on the default label, the line above or in its body, or a
body that throws or calls a function named for failure. Findings are grouped
by owning area. The run never fails on findings (no gate yet).
"""

import argparse
from concurrent.futures import ThreadPoolExecutor
from dataclasses import dataclass
import json
import os
from pathlib import Path
import re
import shlex
import subprocess
import sys

from check_house_style import EXCLUDED, EXTENSIONS, LEXEMES, mask_literal

FIRST_PARTY: tuple[str, ...] = ("include", "src", "demo", "tests", "examples", "tools",
                                "file_manager_demoboard", "stx_vorbis")
VOCABULARY_HEADERS: tuple[str, ...] = (
    "include/gui_forms/anatomy/types/anatomy_types.hpp",
    "include/gui_forms/semantics/types/semantic_types.hpp",
    "include/gui_forms/theme/part_painting/part_painting.hpp",
    "include/gui_forms/theme/types/theme_types.hpp",
)
# Owning areas by path prefix, most specific first (AGENTS.md ownership).
AREAS: tuple[tuple[str, str], ...] = (
    ("include/gui_forms/controls/", "controls"),
    ("src/controls/", "controls"),
    ("demo/", "controls"),
    ("src/host/windows/", "host-windows"),
    ("src/host/macos/", "host-macos"),
    ("src/host/linux/", "host-linux"),
    ("src/host/", "host"),
    ("src/render/", "render"),
    ("src/abi/", "c-abi"),
    ("src/audio/", "audio"),
    ("src/runtime/", "runtime"),
    ("src/core/", "core"),
    ("include/", "core"),
    ("tests/", "tests"),
    ("file_manager_demoboard/", "demoboard"),
    ("stx_vorbis/", "stx_vorbis"),
    ("examples/", "examples"),
    ("tools/", "tools"),
)
RULE_TEXT: dict[str, str] = {
    "F1": "switch-without-default",
    "F2": "default-hides-cases",
    "C1": "input-ignored",
    "C2": "chain-without-fallback",
    "C3": "contradictory-shape",
    "OK": "complete-with-fallback",     # compliant; tallied, not listed
}
DIAGNOSTIC: re.Pattern[str] = re.compile(
    r"^(?P<path>.+?):(?P<line>\d+):(?P<column>\d+): warning: (?P<message>.*) \[(?P<flag>-W[\w-]+)\]\s*$")
FAILURE_CALL: re.Pattern[str] = re.compile(
    r"\bthrow\b|\b\w*(?:fail|reject|unexpected|unreachable|invalid|abort|terminate|error)\w*\s*\(",
    re.IGNORECASE)
ENUM_DECLARATION: re.Pattern[str] = re.compile(r"\benum\s+(?:class|struct)\s+(\w+)")
QUALIFIED_ENUMERATOR: re.Pattern[str] = re.compile(r"((?:\w+\s*::\s*)*\w+)\s*::\s*(\w+)")
ANTONYMS: tuple[tuple[str, str], ...] = (
    ("open", "closed"), ("visible", "hidden"), ("enabled", "disabled"), ("active", "inactive"),
    ("running", "stopped"), ("pending", "done"), ("pending", "complete"), ("pending", "completed"),
    ("pending", "finished"), ("valid", "invalid"), ("attached", "detached"), ("connected", "disconnected"),
)
LIFECYCLE: tuple[str, ...] = ("pending", "queued", "running", "started", "done", "finished", "completed",
                              "complete", "failed", "cancelled", "canceled", "aborted", "succeeded")


@dataclass(frozen=True)
class Finding:
    path: str
    line: int
    rule: str
    area: str
    detail: str


@dataclass(frozen=True)
class Diagnostic:
    path: str
    line: int
    column: int
    flag: str
    message: str


@dataclass(frozen=True)
class Label:
    kind: str           # "case" or "default"
    text: str           # the label expression, "" for default
    line: int
    offset: int         # start of the label in the masked text
    body_start: int     # just after the label's colon
    body_end: int       # start of the next label of this switch, or the switch's closing brace


@dataclass(frozen=True)
class Switch:
    line: int
    offset: int
    condition: str
    body_start: int     # just after the opening brace
    body_end: int       # the closing brace
    labels: tuple[Label, ...]


@dataclass(frozen=True)
class Member:
    type_text: str
    name: str
    line: int


# --- source text -------------------------------------------------------------

def masked(source: str) -> str:
    """Comments and literals blanked to spaces; offsets and line breaks kept."""
    result: str = LEXEMES.sub(mask_literal, source)
    return result


def line_of(text: str, offset: int) -> int:
    result: int = text.count("\n", 0, offset) + 1
    return result


def comment_lines(source: str) -> set[int]:
    """Every line on which a comment starts or continues."""
    lines: set[int] = set()
    match: re.Match[str]
    for match in LEXEMES.finditer(source):
        if not (match.group(0).startswith("//") or match.group(0).startswith("/*")):
            continue
        first: int = line_of(source, match.start())
        last: int = line_of(source, match.end() - 1)
        line: int
        for line in range(first, last + 1):
            lines.add(line)
    return lines


def matching(text: str, opening: int) -> int:
    """Index of the bracket closing the one at opening, or -1."""
    pairs: dict[str, str] = {"(": ")", "{": "}", "[": "]"}
    open_char: str = text[opening]
    close_char: str = pairs[open_char]
    depth: int = 0
    index: int
    for index in range(opening, len(text)):
        character: str = text[index]
        if character == open_char:
            depth += 1
        elif character == close_char:
            depth -= 1
            if depth == 0:
                return index
    return -1


def skip_space(text: str, index: int) -> int:
    while index < len(text) and text[index].isspace():
        index += 1
    return index


# --- switches ----------------------------------------------------------------

SWITCH: re.Pattern[str] = re.compile(r"\bswitch\s*\(")
LABEL: re.Pattern[str] = re.compile(r"\bcase\b|\bdefault\s*:")


def label_colon(text: str, start: int, end: int) -> int:
    """The colon ending a case label (a single colon, not part of ::)."""
    index: int = start
    depth: int = 0
    while index < end:
        character: str = text[index]
        if character in "([{":
            depth += 1
        elif character in ")]}":
            depth -= 1
        elif character == ":" and depth == 0:
            following: bool = index + 1 < len(text) and text[index + 1] == ":"
            preceding: bool = index > 0 and text[index - 1] == ":"
            if not following and not preceding:
                return index
            if following:
                index += 1
        index += 1
    return -1


def find_switches(text: str) -> list[Switch]:
    """Every switch in masked text, with the labels that belong to it (not to nested switches)."""
    switches: list[Switch] = []
    match: re.Match[str]
    for match in SWITCH.finditer(text):
        open_paren: int = match.end() - 1
        close_paren: int = matching(text, open_paren)
        if close_paren < 0:
            continue
        brace: int = skip_space(text, close_paren + 1)
        if brace >= len(text) or text[brace] != "{":
            continue
        end: int = matching(text, brace)
        if end < 0:
            continue
        nested: list[tuple[int, int]] = []
        inner: re.Match[str]
        for inner in SWITCH.finditer(text, brace + 1, end):
            inner_paren: int = matching(text, inner.end() - 1)
            inner_brace: int = skip_space(text, inner_paren + 1) if inner_paren > 0 else -1
            if inner_brace > 0 and inner_brace < end and text[inner_brace] == "{":
                nested.append((inner.start(), matching(text, inner_brace)))
        starts: list[tuple[str, str, int, int]] = []
        label: re.Match[str]
        for label in LABEL.finditer(text, brace + 1, end):
            inside_nested: bool = False
            span: tuple[int, int]
            for span in nested:
                if span[0] <= label.start() <= span[1]:
                    inside_nested = True
                    break
            if inside_nested:
                continue
            if label.group(0).startswith("default"):
                starts.append(("default", "", label.start(), label.end()))
                continue
            colon: int = label_colon(text, label.end(), end)
            if colon < 0:
                continue
            starts.append(("case", " ".join(text[label.end():colon].split()), label.start(), colon + 1))
        labels: list[Label] = []
        index: int
        for index in range(len(starts)):
            body_end: int = end
            if index + 1 < len(starts):
                body_end = starts[index + 1][2]
            entry: tuple[str, str, int, int] = starts[index]
            labels.append(Label(entry[0], entry[1], line_of(text, entry[2]), entry[2], entry[3], body_end))
        condition: str = " ".join(text[open_paren + 1:close_paren].split())
        switches.append(Switch(line_of(text, match.start()), match.start(), condition, brace + 1, end,
                               tuple(labels)))
    return switches


def body_statements(text: str, label: Label) -> str:
    """A label's body without braces, whitespace runs collapsed."""
    body: str = text[label.body_start:label.body_end]
    result: str = " ".join(body.replace("{", " ").replace("}", " ").split())
    return result


def ignores_input(statements: str) -> bool:
    return statements in ("", ";", "break;", "return;", "continue;", "[[fallthrough]];")


def translates(statements: str) -> bool:
    """A case group body that only maps: one return, or one assignment and a break."""
    if re.fullmatch(r"return\b[^;]*;", statements):
        return True
    return re.fullmatch(r"[\w.\[\]*()]+(?:\s*\.\s*\w+)*\s*=[^=;][^;]*;\s*break;", statements) is not None


def switch_enum(switch: Switch) -> str:
    """The enumeration named by the first qualified case label (Name::enumerator), or ""."""
    label: Label
    for label in switch.labels:
        if label.kind != "case":
            continue
        qualified: re.Match[str] | None = QUALIFIED_ENUMERATOR.fullmatch(label.text)
        if qualified is None:
            return ""
        result: str = qualified.group(1).split("::")[-1].strip()
        return result
    return ""


def is_mapping(text: str, switch: Switch, vocabulary: set[str]) -> bool:
    """A switch over a vocabulary enumeration whose every case group only translates."""
    if switch_enum(switch) not in vocabulary:
        return False
    seen_body: bool = False
    label: Label
    for label in switch.labels:
        if label.kind != "case":
            continue
        statements: str = body_statements(text, label)
        if statements == "":
            continue        # grouped with the next label
        seen_body = True
        if not translates(statements):
            return False
    return seen_body


def default_reason(text: str, comments: set[int], switch: Switch) -> bool:
    """Whether a switch's default states a reason: a comment at or in it, or a named failure."""
    label: Label
    for label in switch.labels:
        if label.kind != "default":
            continue
        last: int = line_of(text, max(label.body_start, label.body_end - 1))
        line: int
        for line in range(label.line - 1, last + 1):
            if line in comments:
                return True
        if FAILURE_CALL.search(text[label.body_start:label.body_end]) is not None:
            return True
        return False
    return False


def ignored_inputs(text: str, comments: set[int], switch: Switch) -> list[tuple[int, str]]:
    """C1: labels whose body ignores the input without a comment (grouped labels excluded)."""
    results: list[tuple[int, str]] = []
    index: int
    for index in range(len(switch.labels)):
        label: Label = switch.labels[index]
        statements: str = body_statements(text, label)
        if statements == "" and index + 1 < len(switch.labels):
            continue        # shares the next label's body
        if not ignores_input(statements):
            continue
        last: int = line_of(text, max(label.body_start, label.body_end - 1))
        reasoned: bool = False
        line: int
        for line in range(label.line - 1, last + 1):
            if line in comments:
                reasoned = True
                break
        if reasoned:
            continue
        name: str = "default" if label.kind == "default" else "case " + label.text
        shown: str = statements if statements else "(empty)"
        results.append((label.line, name + " ignores its input (" + shown + ") without a stated reason"))
    return results


# --- if / else if chains -------------------------------------------------------

IF_HEAD: re.Pattern[str] = re.compile(r"\bif\s*\(")
COMPARISON: re.Pattern[str] = re.compile(
    r"^\s*(?P<left>[^=!<>]+?)\s*==\s*(?P<right>(?:\w+\s*::\s*)+\w+)\s*$|"
    r"^\s*(?P<right2>(?:\w+\s*::\s*)+\w+)\s*==\s*(?P<left2>[^=!<>]+?)\s*$")


def compared(condition: str) -> tuple[str, str] | None:
    match: re.Match[str] | None = COMPARISON.match(condition)
    if match is None:
        return None
    left: str = match.group("left") or match.group("left2") or ""
    right: str = match.group("right") or match.group("right2") or ""
    return " ".join(left.split()), "".join(right.split())


def find_chains(text: str, comments: set[int]) -> list[tuple[int, str]]:
    """C2: if / else if chains over one expression and two or more enumerators, without a final else."""
    results: list[tuple[int, str]] = []
    match: re.Match[str]
    for match in IF_HEAD.finditer(text):
        before: str = text[max(0, match.start() - 12):match.start()]
        if re.search(r"\belse\s*$", before):
            continue        # not a chain head
        subject: str = ""
        enumerators: list[str] = []
        cursor: int = match.end() - 1
        complete: bool = True
        has_else: bool = False
        while True:
            close: int = matching(text, cursor)
            if close < 0:
                complete = False
                break
            pair: tuple[str, str] | None = compared(text[cursor + 1:close])
            if pair is None or (subject and pair[0] != subject):
                complete = False
                break
            subject = pair[0]
            enumerators.append(pair[1])
            block: int = skip_space(text, close + 1)
            if block >= len(text) or text[block] != "{":
                complete = False
                break
            block_end: int = matching(text, block)
            if block_end < 0:
                complete = False
                break
            following: int = skip_space(text, block_end + 1)
            else_match: re.Match[str] | None = re.match(r"else\b", text[following:following + 5])
            if else_match is None:
                chain_end_line: int = line_of(text, block_end)
                break
            after_else: int = skip_space(text, following + 4)
            if_match: re.Match[str] | None = IF_HEAD.match(text, after_else)
            if if_match is None:
                has_else = True
                break
            cursor = if_match.end() - 1
        if not complete or has_else or len(enumerators) < 2:
            continue
        head_line: int = line_of(text, match.start())
        if head_line in comments or head_line - 1 in comments or chain_end_line in comments:
            continue
        results.append((head_line, "chain over " + subject + " (" + ", ".join(enumerators)
                        + ") has no final else"))
    return results


# --- record shapes -------------------------------------------------------------

RECORD: re.Pattern[str] = re.compile(r"\b(?:struct|class)\s+(\w+)\s*(?:final\s*)?(?::[^;{]*)?\{")
SKIPPED_MEMBER: re.Pattern[str] = re.compile(
    r"^(?:using|typedef|friend|static_assert|enum|struct|class|union|template|static|constexpr|"
    r"public|private|protected)\b")


def record_members(text: str, body_start: int, body_end: int) -> list[Member]:
    """Data members declared directly in a record body (functions and nested types skipped)."""
    members: list[Member] = []
    depth: int = 0
    statement_start: int = body_start
    index: int
    for index in range(body_start, body_end):
        character: str = text[index]
        if character in "({[":
            depth += 1
        elif character in ")}]":
            depth -= 1
            if character == "}" and depth == 0 and text[skip_space(text, index + 1):][:1] != ";":
                # A member function body ended. A brace initializer, or a
                # nested type, is followed by its semicolon and stays part of
                # the statement.
                statement_start = index + 1
        elif character == ";" and depth == 0:
            raw: str = text[statement_start:index]
            statement: str = " ".join(raw.split())
            statement_line: int = line_of(text, statement_start + len(raw) - len(raw.lstrip()))
            statement_start = index + 1
            statement = re.sub(r"^(?:public|private|protected)\s*:\s*", "", statement)
            if not statement or SKIPPED_MEMBER.match(statement):
                continue
            declaration: str = re.split(r"\s*=|\{", statement, maxsplit=1)[0].strip()
            if "(" in declaration or "operator" in declaration:
                continue
            name_match: re.Match[str] | None = re.search(r"(\w+)\s*(?:\[[^\]]*\])?$", declaration)
            if name_match is None:
                continue
            type_text: str = declaration[:name_match.start()].strip()
            if not type_text:
                continue
            members.append(Member(type_text, name_match.group(1), statement_line))
    return members


def flag_target(name: str) -> str:
    """The member a flag named has_x / x_valid / x_set / x_present qualifies, or ""."""
    match: re.Match[str] | None = re.fullmatch(r"has_(\w+)|(\w+?)_(?:valid|set|present|known)", name)
    if match is None:
        return ""
    result: str = match.group(1) or match.group(2) or ""
    return result


def is_bool(type_text: str) -> bool:
    return re.fullmatch(r"(?:const\s+)?bool", type_text) is not None


def shape_candidates(members: list[Member]) -> list[tuple[int, str]]:
    """C3: flags beside the value they qualify, antonym flags, and lifecycle flags."""
    results: list[tuple[int, str]] = []
    by_name: dict[str, Member] = {}
    member: Member
    for member in members:
        by_name[member.name.rstrip("_")] = member
    for member in members:
        if not is_bool(member.type_text):
            continue
        target: str = flag_target(member.name.rstrip("_"))
        if target and target in by_name and by_name[target] is not member:
            value: Member = by_name[target]
            if "optional" in value.type_text:
                results.append((member.line, "bool " + member.name + " mirrors std::optional " + value.name))
            else:
                results.append((member.line, "bool " + member.name + " qualifies " + value.name
                                + "; one std::optional (or an enum) cannot disagree"))
    # Flags keyed by (subject, state word): presentation_pending and
    # frame_capture_completed qualify different subjects and do not conflict.
    flags: dict[tuple[str, str], Member] = {}
    for member in members:
        if not is_bool(member.type_text):
            continue
        words: list[str] = re.sub(r"^is_", "", member.name.rstrip("_")).split("_")
        flags[("_".join(words[:-1]), words[-1])] = member
    pair: tuple[str, str]
    for pair in ANTONYMS:
        key: tuple[str, str]
        for key in list(flags):
            if key[1] == pair[0] and (key[0], pair[1]) in flags:
                first: Member = flags[key]
                second: Member = flags[(key[0], pair[1])]
                results.append((second.line, "bools " + first.name + " and " + second.name
                                + " can both be true; one enum states which"))
    # Three or more lifecycle words for one subject (pending, running and
    # failed of the same thing) describe one phase that an enum would hold.
    by_subject: dict[str, list[Member]] = {}
    word: str
    for word in LIFECYCLE:
        for key in flags:
            if key[1] == word:
                phases: list[Member] = by_subject.setdefault(key[0], [])
                if flags[key] not in phases:
                    phases.append(flags[key])
    subject: str
    for subject in sorted(by_subject):
        lifecycle: list[Member] = by_subject[subject]
        if len(lifecycle) < 3:
            continue
        names: list[str] = []
        for member in lifecycle:
            names.append(member.name)
        results.append((lifecycle[0].line, "lifecycle bools " + ", ".join(names) + " admit contradictory "
                        "combinations; one enum states the phase"))
    return results


def find_shapes(text: str) -> list[tuple[int, str]]:
    results: list[tuple[int, str]] = []
    match: re.Match[str]
    for match in RECORD.finditer(text):
        if match.group(1).endswith("States") or match.group(1).endswith("Parts"):
            continue        # anatomy declarations: one bool per state or part name, not runtime state
        end: int = matching(text, match.end() - 1)
        if end < 0:
            continue
        candidate: tuple[int, str]
        for candidate in shape_candidates(record_members(text, match.end(), end)):
            results.append((candidate[0], match.group(1) + ": " + candidate[1]))
    return results


# --- compiler facts -------------------------------------------------------------

def parse_diagnostics(output: str) -> list[Diagnostic]:
    diagnostics: list[Diagnostic] = []
    line: str
    for line in output.splitlines():
        match: re.Match[str] | None = DIAGNOSTIC.match(line.strip())
        if match is None:
            continue
        diagnostics.append(Diagnostic(match.group("path"), int(match.group("line")), int(match.group("column")),
                                      match.group("flag"), match.group("message")))
    return diagnostics


def missing_names(message: str) -> list[str]:
    names: list[str] = re.findall(r"'(\w+)'", message)
    return names


def missing_text(message: str) -> str:
    """The enumerators a diagnostic names; clang names three and counts the rest."""
    names: str = ", ".join(missing_names(message))
    counted: re.Match[str] | None = re.match(r"(\d+) enumeration values", message)
    if counted is not None:
        return counted.group(1) + " enumerators (" + names + ", ...)"
    return names


# A statement that leaves: return, throw, or a call that does not return.
LEAVING: str = r"(?:\breturn\b|\bthrow\b|\b(?:std\s*::\s*)?(?:abort|terminate|unreachable|exit|quick_exit)\s*\()"


def every_case_leaves(text: str, switch: Switch) -> bool:
    """Whether each case group ends with a leaving statement, so control passes the switch only when no case matched."""
    seen: bool = False
    label: Label
    for label in switch.labels:
        statements: str = body_statements(text, label)
        if label.kind != "case" or statements == "":
            continue
        seen = True
        if re.search(r"\bbreak\s*;", statements) or not re.search(LEAVING + r"[^;]*;$", statements):
            return False
    return seen


def fallback_after(text: str, switch: Switch) -> bool:
    """Whether the statement directly after the switch's closing brace leaves (the one fallback)."""
    following: int = skip_space(text, switch.body_end + 1)
    return re.match(LEAVING, text[following:following + 40]) is not None


def complete_with_fallback(text: str, switch: Switch) -> bool:
    """The compliant form (owner, 2026-10-21): an enumeration switch that lists every
    enumerator, leaves from every case, and reaches one fallback directly after its
    closing brace, which keeps the compiler's warning for a new enumerator. The caller
    has established that no enumerator is unhandled; a qualified case label shows the
    condition is an enumeration."""
    return switch_enum(switch) != "" and every_case_leaves(text, switch) and fallback_after(text, switch)


def syntax_command(command: str) -> list[str]:
    """A compile command turned into a front-end-only run with only the switch warnings."""
    arguments: list[str] = shlex.split(command, posix=(os.name != "nt"))
    result: list[str] = []
    skip: bool = False
    argument: str
    for argument in arguments:
        argument = argument.strip('"')
        if skip:
            skip = False
            continue
        if argument in ("-o", "-MT", "-MF", "-MQ"):
            skip = True
            continue
        if argument in ("-MD", "-MMD", "-c") or Path(argument).name.lower() in ("ccache", "ccache.exe"):
            continue
        result.append(argument)
    result.extend(["-fsyntax-only", "-Wno-everything", "-Wswitch", "-Wswitch-enum", "-Wswitch-default",
                   "-fno-color-diagnostics", "-fno-caret-diagnostics"])
    return result


@dataclass(frozen=True)
class Unit:
    directory: str
    command: tuple[str, ...]


def run_unit(unit: Unit) -> str:
    completed: subprocess.CompletedProcess[str] = subprocess.run(
        list(unit.command), cwd=unit.directory, capture_output=True, text=True, encoding="utf-8", errors="replace")
    return completed.stdout + completed.stderr


def normalized(path: str, directory: str) -> Path:
    candidate: Path = Path(path)
    if not candidate.is_absolute():
        candidate = Path(directory) / candidate
    result: Path = Path(os.path.normpath(str(candidate)))
    return result


def first_party(path: Path, root: Path) -> str:
    """The checkout-relative path of a first-party source, or ""."""
    try:
        relative: Path = path.resolve().relative_to(root.resolve())
    except ValueError:
        return ""
    part: str
    for part in relative.parts:
        if part in EXCLUDED or part == "_deps":
            return ""
    if not relative.parts or relative.parts[0] not in FIRST_PARTY or relative.suffix not in EXTENSIONS + (".c",):
        return ""
    return relative.as_posix()


def select_units(entries: list[dict], root: Path) -> list[Unit]:
    """Translation units whose source has a switch or includes, by name, a first-party header that has one."""
    switch_headers: set[str] = set()
    directory: str
    for directory in FIRST_PARTY:
        header: Path
        for header in (root / directory).rglob("*"):
            if header.suffix in (".hpp", ".h", ".inc") and header.is_file():
                if SWITCH.search(masked(header.read_text(encoding="utf-8", errors="replace"))):
                    switch_headers.add(header.name)
    units: list[Unit] = []
    seen: set[str] = set()
    entry: dict
    for entry in entries:
        source: Path = normalized(entry["file"], entry["directory"])
        if not first_party(source, root) or str(source) in seen:
            continue
        seen.add(str(source))
        text: str = source.read_text(encoding="utf-8", errors="replace")
        wanted: bool = SWITCH.search(masked(text)) is not None
        if not wanted:
            name: str
            for name in re.findall(r'#\s*include\s*[<"]([^>"]+)[>"]', text):
                if Path(name).name in switch_headers:
                    wanted = True
                    break
        if wanted:
            units.append(Unit(entry["directory"], tuple(syntax_command(entry["command"]))))
    return units


def compiler_diagnostics(build_dir: Path, root: Path, jobs: int) -> tuple[list[Diagnostic], int]:
    compdb: str = subprocess.check_output(["ninja", "-C", str(build_dir), "-t", "compdb"], text=True,
                                          encoding="utf-8", errors="replace")
    units: list[Unit] = select_units(json.loads(compdb), root)
    outputs: list[str] = []
    with ThreadPoolExecutor(max_workers=jobs) as pool:
        output: str
        for output in pool.map(run_unit, units):
            outputs.append(output)
    diagnostics: list[Diagnostic] = []
    unique: set[tuple[str, int, int, str]] = set()
    unit: Unit
    for unit, output in zip(units, outputs):
        diagnostic: Diagnostic
        for diagnostic in parse_diagnostics(output):
            relative: str = first_party(normalized(diagnostic.path, unit.directory), root)
            if not relative:
                continue
            key: tuple[str, int, int, str] = (relative, diagnostic.line, diagnostic.column, diagnostic.flag)
            if key in unique:
                continue
            unique.add(key)
            diagnostics.append(Diagnostic(relative, diagnostic.line, diagnostic.column, diagnostic.flag,
                                          diagnostic.message))
    return diagnostics, len(units)


# --- assembly -------------------------------------------------------------------

def area_of(path: str) -> str:
    prefix: tuple[str, str]
    for prefix in AREAS:
        if path.startswith(prefix[0]):
            return prefix[1]
    return "other"


def vocabulary_enums(root: Path) -> set[str]:
    names: set[str] = set()
    header: str
    for header in VOCABULARY_HEADERS:
        path: Path = root / header
        if path.is_file():
            names.update(ENUM_DECLARATION.findall(masked(path.read_text(encoding="utf-8"))))
    return names


def switch_at(switches: list[Switch], text: str, line: int) -> Switch | None:
    """The switch whose keyword or condition is on line (innermost when several start there)."""
    found: Switch | None = None
    switch: Switch
    for switch in switches:
        condition_line: int = line_of(text, switch.body_start)
        if switch.line <= line <= condition_line:
            found = switch
    return found


def compiler_findings(diagnostics: list[Diagnostic], root: Path, vocabulary: set[str]) -> list[Finding]:
    by_switch: dict[tuple[str, int], dict[str, list[str]]] = {}
    diagnostic: Diagnostic
    for diagnostic in diagnostics:
        key: tuple[str, int] = (diagnostic.path, diagnostic.line)
        flags: dict[str, list[str]] = by_switch.setdefault(key, {})
        text_of_missing: str = missing_text(diagnostic.message)
        if text_of_missing:
            flags.setdefault(diagnostic.flag, []).append(text_of_missing)
        else:
            flags.setdefault(diagnostic.flag, [])
    findings: list[Finding] = []
    texts: dict[str, tuple[str, str, list[Switch], set[int]]] = {}
    key: tuple[str, int]
    for key in sorted(by_switch):
        path: str = key[0]
        if path not in texts:
            source: str = (root / path).read_text(encoding="utf-8", errors="replace")
            text: str = masked(source)
            texts[path] = (source, text, find_switches(text), comment_lines(source))
        entry: tuple[str, str, list[Switch], set[int]] = texts[path]
        switch: Switch | None = switch_at(entry[2], entry[1], key[1])
        flags = by_switch[key]
        enum_name: str = switch_enum(switch) if switch is not None else ""
        subject: str = switch.condition if switch is not None else "?"
        if "-Wswitch-default" in flags:
            missing: list[str] = flags.get("-Wswitch", [])
            detail: str = "switch (" + subject + ") has no default"
            if missing:
                detail += "; also unhandled: " + ", ".join(missing)
            elif switch is not None and complete_with_fallback(entry[1], switch):
                # Compliant: tallied, not a finding. Without -Wswitch, no enumerator is unhandled.
                shown: str = " ".join(entry[0][switch.body_end + 1:switch.body_end + 200].split())
                findings.append(Finding(path, key[1], "OK", area_of(path),
                                        "switch (" + subject + ") lists every enumerator and falls back after it ("
                                        + shown.split(";")[0][:60] + ")"))
                continue
            findings.append(Finding(path, key[1], "F1", area_of(path), detail))
            continue
        if "-Wswitch-enum" not in flags or switch is None:
            continue
        hidden: list[str] = flags["-Wswitch-enum"]
        named: str = (enum_name + " ") if enum_name else ""
        if is_mapping(entry[1], switch, vocabulary):
            findings.append(Finding(path, key[1], "F2", area_of(path),
                                    "vocabulary mapping over " + named + "must list every enumerator; missing: "
                                    + ", ".join(hidden)))
        elif not default_reason(entry[1], entry[3], switch):
            findings.append(Finding(path, key[1], "F2", area_of(path),
                                    "default takes " + named + "enumerators " + ", ".join(hidden)
                                    + " without a stated reason"))
    return findings


def source_candidates(path: str, source: str) -> list[Finding]:
    text: str = masked(source)
    comments: set[int] = comment_lines(source)
    candidates: list[Finding] = []
    switch: Switch
    for switch in find_switches(text):
        item: tuple[int, str]
        for item in ignored_inputs(text, comments, switch):
            candidates.append(Finding(path, item[0], "C1", area_of(path), item[1]))
    for item in find_chains(text, comments):
        candidates.append(Finding(path, item[0], "C2", area_of(path), item[1]))
    for item in find_shapes(text):
        if item[0] in comments or item[0] - 1 in comments:
            continue
        candidates.append(Finding(path, item[0], "C3", area_of(path), item[1]))
    return candidates


def source_files(root: Path) -> list[str]:
    files: list[str] = []
    directory: str
    for directory in FIRST_PARTY:
        candidate: Path
        for candidate in sorted((root / directory).rglob("*")):
            if candidate.is_file():
                relative: str = first_party(candidate, root)
                if relative:
                    files.append(relative)
    return files


def finding_key(finding: Finding) -> tuple[str, str, str, int]:
    return finding.area, finding.rule, finding.path, finding.line


def render(findings: list[Finding], units: int, files: int) -> str:
    lines: list[str] = []
    counts: dict[str, dict[str, int]] = {}
    finding: Finding
    for finding in findings:
        per_area: dict[str, int] = counts.setdefault(finding.area, {})
        per_area[finding.rule] = per_area.get(finding.rule, 0) + 1
    lines.append(f"{units} translation units compiled; {files} files parsed")
    rules: list[str] = sorted(RULE_TEXT)
    header: str = "area".ljust(14)
    rule: str
    for rule in rules:
        header += rule.rjust(5)
    lines.append(header)
    area: str
    for area in sorted(counts):
        row: str = area.ljust(14)
        for rule in rules:
            row += str(counts[area].get(rule, 0)).rjust(5)
        lines.append(row)
    lines.append("OK counts compliant switches (every enumerator listed, every case leaves, one "
                 "fallback directly after); they are not listed below.")
    current: str = ""
    for finding in sorted(findings, key=finding_key):
        if finding.rule == "OK":
            continue
        if finding.area != current:
            current = finding.area
            lines.append("")
            lines.append("== " + current)
        lines.append(f"{finding.path}:{finding.line}: {finding.rule} {RULE_TEXT[finding.rule]}: {finding.detail}")
    return "\n".join(lines) + "\n"


def main() -> int:
    parser: argparse.ArgumentParser = argparse.ArgumentParser(description=__doc__,
                                                              formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--root", default=str(Path(__file__).resolve().parents[1]), help="the gui_forms checkout")
    parser.add_argument("--build-dir", help="a configured ninja tree; without it only C1 to C3 run")
    parser.add_argument("--jobs", type=int, default=4)
    parser.add_argument("--json", dest="json_path", help="also write the findings as JSON here")
    args: argparse.Namespace = parser.parse_args()
    root: Path = Path(args.root)
    findings: list[Finding] = []
    units: int = 0
    if args.build_dir:
        diagnostics: list[Diagnostic]
        diagnostics, units = compiler_diagnostics(Path(args.build_dir), root, max(1, args.jobs))
        findings.extend(compiler_findings(diagnostics, root, vocabulary_enums(root)))
    files: list[str] = source_files(root)
    path: str
    for path in files:
        findings.extend(source_candidates(path, (root / path).read_text(encoding="utf-8", errors="replace")))
    sys.stdout.write(render(findings, units, len(files)))
    if args.json_path:
        records: list[dict[str, object]] = []
        finding: Finding
        for finding in sorted(findings, key=finding_key):
            records.append({"path": finding.path, "line": finding.line, "rule": finding.rule,
                            "name": RULE_TEXT[finding.rule], "area": finding.area, "detail": finding.detail})
        Path(args.json_path).write_text(json.dumps(records, indent=1) + "\n", encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
