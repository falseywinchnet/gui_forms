"""Find source shapes that leave storage or object lifetime uncertain.

This is a source-level review aid. A finding identifies a house-style contract
that the code does not make locally evident; it is not a claim about runtime
behavior. The checker deliberately does not execute inputs or infer intent.
"""

from __future__ import annotations

import argparse
from dataclasses import asdict, dataclass
import json
from pathlib import Path
import re
import sys


SOURCE_SUFFIXES = {".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".inc", ".m", ".mm"}
SKIP_DIRECTORIES = {".git", ".build", "build", "baseline", "third_party", "vendor", "node_modules"}
CONTROL_NAMES = {"if", "for", "while", "switch", "catch", "sizeof", "alignof", "decltype", "requires"}
CHANGING_METHODS = ("reserve", "resize", "push_back", "emplace_back", "insert", "erase",
                    "assign", "clear", "reset", "release", "swap", "shrink_to_fit", "append")
LEXEMES = re.compile(
    r'//[^\n]*|/\*[\s\S]*?\*/|'
    r'\b[0-9][0-9A-Fa-fxXbB]*(?:\'[0-9A-Fa-f]+)+(?:[uUlLfF]*)|'
    r'(?:u8|u|U|L)?R"([^\s()\\]{0,16})\([\s\S]*?\)\1"|'
    r'(?:u8|u|U|L)?"(?:\\[\s\S]|[^"\\])*"|'
    r"(?:u8|u|U|L)?'(?:\\[\s\S]|[^'\\])*'"
)


@dataclass(frozen=True)
class Rule:
    title: str
    style: str
    question: str


RULES = {
    "I01": Rule("borrow used after owner changes", "§5", "Finish reads before changing the owner, or show that storage remains live."),
    "I02": Rule("live count published before construction", "§5, §9", "Construct first; publish only the completed prefix, including on failure."),
    "I03": Rule("assertion is the visible access bound", "§5, §9", "Establish the access bound in Release before forming the address."),
    "I04": Rule("empty buffer may form a pointer endpoint", "§3, §5", "Handle empty input before pointer arithmetic, or use a length-bearing view."),
    "I05": Rule("size narrowed without a visible bound", "§1, §5", "Check representability before conversion; keep capacity and live count distinct."),
    "I06": Rule("raw extent product at storage sink", "§3, §5", "Check the whole shape calculation before allocation or copy."),
    "I07": Rule("poison begins after named boundary", "§5", "Verify whether the first argument is already the exclusive live end."),
}


@dataclass(frozen=True)
class Finding:
    path: str
    line: int
    rule: str
    evidence: str
    related_line: int | None = None

    def record(self) -> dict[str, object]:
        result: dict[str, object] = asdict(self)
        result["title"] = RULES[self.rule].title
        result["house_style"] = RULES[self.rule].style
        result["house_style_status"] = "invariant_not_evident_in_source"
        result["review"] = RULES[self.rule].question
        return result


@dataclass(frozen=True)
class Function:
    name: str
    start: int
    end: int
    text: str


@dataclass(frozen=True)
class Statement:
    start: int
    text: str


def mask_match(match: re.Match[str]) -> str:
    return re.sub(r"[^\n]", " ", match.group(0))


def masked(source: str) -> str:
    return LEXEMES.sub(mask_match, source)


def line_at(source: str, offset: int) -> int:
    return source.count("\n", 0, offset) + 1


def balanced_open(text: str, close: int, left: str, right: str) -> int:
    depth = 0
    for index in range(close, -1, -1):
        if text[index] == right:
            depth += 1
        elif text[index] == left:
            depth -= 1
            if depth == 0:
                return index
    return -1


def functions(text: str) -> list[Function]:
    """Bound ordinary function bodies without interpreting C++ types."""
    result: list[Function] = []
    index = 0
    while index < len(text):
        if text[index] != "{":
            index += 1
            continue
        prefix = text[max(0, index - 160):index].rstrip()
        signature = re.search(r"\)\s*(?:const\s*)?(?:noexcept\s*)?(?:override\s*)?(?:final\s*)?(?:&&?\s*)?$", prefix)
        if signature is None:
            index += 1
            continue
        close_parenthesis = max(0, index - 160) + signature.start()
        open_parenthesis = balanced_open(text, close_parenthesis, "(", ")")
        if open_parenthesis < 0:
            index += 1
            continue
        name_match = re.search(r"([A-Za-z_~][A-Za-z_0-9~]*)\s*$", text[max(0, open_parenthesis - 100):open_parenthesis])
        if name_match is None or name_match.group(1) in CONTROL_NAMES:
            index += 1
            continue
        depth = 1
        end = index + 1
        while end < len(text) and depth:
            if text[end] == "{":
                depth += 1
            elif text[end] == "}":
                depth -= 1
            end += 1
        if depth == 0:
            result.append(Function(name_match.group(1), index, end, text[index:end]))
            index = end
        else:
            index += 1
    return result


def statements(body: str, base: int) -> list[Statement]:
    """Keep statement offsets; semicolons inside calls and for heads are ignored."""
    result: list[Statement] = []
    start = 0
    parenthesis = 0
    for index, character in enumerate(body):
        if character == "(":
            parenthesis += 1
        elif character == ")":
            parenthesis -= 1
        elif character == ";" and parenthesis == 0:
            result.append(Statement(base + start, body[start:index + 1]))
            start = index + 1
    return result


def evidence(source: str, offset: int) -> str:
    line = source.splitlines()[line_at(source, offset) - 1].strip()
    return line[:180]


def report(path: str, source: str, offset: int, rule: str, related: int | None = None) -> Finding:
    related_line = None if related is None else line_at(source, related)
    return Finding(path, line_at(source, offset), rule, evidence(source, offset), related_line)


def borrow_after_change(path: str, source: str, text: str, function: Function) -> list[Finding]:
    result: list[Finding] = []
    parts = statements(function.text, function.start)
    origin = re.compile(r"(?<![.>\w])(?P<borrow>[A-Za-z_]\w*)\s*=\s*(?P<owner>[A-Za-z_]\w*)\s*\.\s*(?:data|begin|c_str|get)\s*\(\s*\)|"
                        r"(?<![.>\w])(?P<borrow2>[A-Za-z_]\w*)\s*=\s*&\s*(?P<owner2>[A-Za-z_]\w*)\s*\[\s*0\s*\]")
    for position, part in enumerate(parts):
        match = origin.search(part.text)
        if match is None:
            continue
        borrow = match.group("borrow") or match.group("borrow2")
        owner = match.group("owner") or match.group("owner2")
        change = re.compile(r"\b" + re.escape(owner) + r"\s*\.\s*(?:" + "|".join(CHANGING_METHODS) + r")\s*\(")
        assignment = re.compile(r"\b" + re.escape(borrow) + r"\s*=(?!=)")
        borrow_use = re.compile(r"\b" + re.escape(borrow) + r"\b")
        changed_at: int | None = None
        for next_part in parts[position + 1:]:
            if assignment.search(next_part.text):
                break
            if changed_at is not None and re.search(r"\breturn\b", next_part.text):
                break
            if changed_at is None:
                mutation = change.search(next_part.text)
                if mutation is not None:
                    changed_at = next_part.start + mutation.start()
            elif borrow_use.search(next_part.text):
                result.append(report(path, source, next_part.start + borrow_use.search(next_part.text).start(),
                                     "I01", changed_at))
                break
    return result


def construction_before_publish(path: str, source: str, text: str, found: list[Function]) -> list[Finding]:
    result: list[Finding] = []
    published_helpers: dict[str, int] = {}
    publication = re.compile(r"\b(?:setMetadata|setCount|setSize|setLiveCount)\s*\(|\b(?:fCount|fSize|fLength|count|size)\s*(?:\+\+|\+=\s*1)")
    construction = re.compile(r"\b(?:new\s*\(|construct_at\s*\(|uninitialized_(?:copy|move|default_construct|value_construct)\s*\()")
    for function in found:
        publish = publication.search(function.text)
        if publish is None:
            continue
        construct = construction.search(function.text)
        if construct is not None and publish.start() < construct.start():
            result.append(report(path, source, function.start + construct.start(), "I02", function.start + publish.start()))
        if re.search(r"\breturn\b[^;]*(?:\bptr\s*\(|\bdata\s*\(|\bslot\b|\baddress\b)", function.text):
            published_helpers[function.name] = function.start + publish.start()
    if not published_helpers:
        return result
    for function in found:
        for helper, location in published_helpers.items():
            pattern = re.compile(r"\bnew\s*\(\s*(?:this\s*->\s*|\(\s*\*\s*this\s*\)\s*\.\s*)?" +
                                 re.escape(helper) + r"\s*\(")
            for match in pattern.finditer(function.text):
                result.append(report(path, source, function.start + match.start(), "I02", location))
    return result


def assert_only_bounds(path: str, source: str, function: Function) -> list[Finding]:
    result: list[Finding] = []
    assertion = re.compile(r"\b(?:assert|SkASSERT|DCHECK|Q_ASSERT)\s*\(\s*(?P<condition>[^;]*?)\s*\)\s*;")
    for match in assertion.finditer(function.text):
        condition = match.group("condition")
        variables = re.findall(r"\b([A-Za-z_]\w*)\s*(?:<|<=|>=|>)", condition)
        for variable in variables:
            if variable in {"this", "true", "false", "nullptr"}:
                continue
            later = function.text[match.end():match.end() + 500]
            access = re.search(r"\[\s*" + re.escape(variable) + r"\s*\]|\b(?:data|ptr|pixels)\s*\+\s*" + re.escape(variable) + r"\b", later)
            if access is None:
                continue
            before = later[:access.start()]
            if re.search(r"\bif\s*\([^)]*\b" + re.escape(variable) + r"\b[^)]*\)", before):
                continue
            result.append(report(path, source, function.start + match.start(), "I03",
                                 function.start + match.end() + access.start()))
            break
    return result


def empty_pointer_endpoints(path: str, source: str, function: Function) -> list[Finding]:
    result: list[Finding] = []
    pattern = re.compile(r"\b(?P<owner>[A-Za-z_]\w*)\s*\.\s*data\s*\(\s*\)\s*\+\s*(?:0\b|(?P=owner)\s*\.\s*(?:size|length|count)\s*\(\s*\))")
    for match in pattern.finditer(function.text):
        preceding = function.text[:match.start()]
        owner = match.group("owner")
        # std::string has a valid terminator. A fixed std::array with a
        # positive literal extent has storage even before any element is used.
        # std::array<T, 0> is deliberately left as a candidate.
        declarations = source[max(0, function.start - 500):function.start] + preceding
        owning_type = re.compile(r"\bstd\s*::\s*(?:array\s*<[^>,]+,\s*[1-9][0-9]*[uUlL]*\s*>|string\b|basic_string\s*<[^>]*>)"
                                 r"\s*(?:const\s*)?[*&]?\s*" + re.escape(owner) + r"\b")
        if owning_type.search(declarations):
            continue
        if re.search(r"\bif\s*\(\s*" + re.escape(owner) + r"\s*\.\s*empty\s*\(\s*\)\s*\)\s*(?:\{[^{}]*\breturn\b|return\b)", preceding):
            continue
        result.append(report(path, source, function.start + match.start(), "I04"))
    return result


def narrowing_and_extents(path: str, source: str, function: Function) -> list[Finding]:
    result: list[Finding] = []
    narrowing = re.compile(r"\bstatic_cast\s*<\s*(?:int|int32_t|uint32_t|short|unsigned)\s*>\s*\(\s*(?P<expr>[A-Za-z_]\w*\s*\.\s*(?:size|length|count|capacity)\s*\(\s*\))\s*\)")
    for match in narrowing.finditer(function.text):
        preceding = function.text[max(0, match.start() - 600):match.start()]
        if re.search(r"\b(?:numeric_limits|INT_MAX|UINT_MAX|INT32_MAX|UINT32_MAX|checked|FitsIn)\b", preceding):
            continue
        result.append(report(path, source, function.start + match.start(), "I05"))
    sink = re.compile(r"\b(?:malloc|calloc|realloc|memcpy|memmove|reserve|resize|allocate|SkMalloc|sk_malloc_throw)\s*\((?P<args>[^;()]{0,180})\)")
    for match in sink.finditer(function.text):
        arguments = match.group("args")
        if re.search(r"\b[A-Za-z_]\w*\s*\*\s*[A-Za-z_]\w*\b", arguments) is None:
            continue
        preceding = function.text[max(0, match.start() - 600):match.start()]
        if re.search(r"\b(?:numeric_limits|SIZE_MAX|checked|SafeMath|FitsIn)\b", preceding):
            continue
        result.append(report(path, source, function.start + match.start(), "I06"))
    # A separate size assignment can hide the same raw product from the sink.
    # Trace only a single local name; deeper dataflow needs a compiler front end.
    product = re.compile(r"\b(?P<name>[A-Za-z_]\w*)\s*=\s*[A-Za-z_]\w*\s*\*\s*[A-Za-z_]\w*\s*;")
    for match in product.finditer(function.text):
        name = match.group("name")
        later = function.text[match.end():]
        use = re.search(r"\b(?:malloc|calloc|realloc|memcpy|memmove|reserve|resize|allocate|SkMalloc|sk_malloc_throw)"
                        r"\s*\([^;]{0,180}\b" + re.escape(name) + r"\b[^;]{0,180}\)", later)
        if use is None:
            continue
        preceding = function.text[max(0, match.start() - 600):match.start()]
        if re.search(r"\b(?:numeric_limits|SIZE_MAX|checked|SafeMath|FitsIn)\b", preceding):
            continue
        result.append(report(path, source, function.start + match.start(), "I06",
                             function.start + match.end() + use.start()))
    return result


def poison_boundary(path: str, source: str, function: Function) -> list[Finding]:
    result: list[Finding] = []
    pattern = re.compile(r"\b(?:poisonRange|poison_range|__asan_poison_memory_region)\s*\(\s*[A-Za-z_]\w*\s*\+\s*1\s*[,)]")
    for match in pattern.finditer(function.text):
        result.append(report(path, source, function.start + match.start(), "I07"))
    return result


def inspect(path: str, source: str) -> list[Finding]:
    text = masked(source)
    found = functions(text)
    result: list[Finding] = construction_before_publish(path, source, text, found)
    function: Function
    for function in found:
        result.extend(borrow_after_change(path, source, text, function))
        result.extend(assert_only_bounds(path, source, function))
        result.extend(empty_pointer_endpoints(path, source, function))
        result.extend(narrowing_and_extents(path, source, function))
        result.extend(poison_boundary(path, source, function))
    unique: dict[tuple[str, int, str], Finding] = {}
    finding: Finding
    for finding in result:
        unique[(finding.path, finding.line, finding.rule)] = finding
    return sorted(unique.values(), key=lambda item: (item.path, item.line, item.rule))


def collect(paths: list[str]) -> list[Path]:
    selected: set[Path] = set()
    for value in paths:
        path = Path(value)
        if not path.exists():
            raise FileNotFoundError(path)
        if path.is_file():
            if path.suffix not in SOURCE_SUFFIXES:
                raise ValueError("Expected a C or C++ source file: " + value)
            selected.add(path)
            continue
        for candidate in path.rglob("*"):
            if candidate.suffix not in SOURCE_SUFFIXES or not candidate.is_file():
                continue
            if any(part in SKIP_DIRECTORIES for part in candidate.relative_to(path).parts):
                continue
            selected.add(candidate)
    return sorted(selected)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("paths", nargs="+", help="explicit C/C++ files or directories")
    parser.add_argument("--json", type=Path, help="write the complete structured report")
    parser.add_argument("--gate", action="store_true", help="fail on every finding; use after reviewing baseline findings")
    arguments = parser.parse_args()
    try:
        paths = collect(arguments.paths)
    except (FileNotFoundError, ValueError) as error:
        parser.error(str(error))
    if not paths:
        parser.error("No C or C++ files selected; empty scope is not a pass.")
    findings: list[Finding] = []
    for path in paths:
        findings.extend(inspect(str(path), path.read_text(encoding="utf-8-sig", errors="replace")))
    for finding in findings:
        rule = RULES[finding.rule]
        related = "" if finding.related_line is None else f" (related line {finding.related_line})"
        print(f"{finding.path}:{finding.line}: {finding.rule} {rule.title} [house style {rule.style}]{related}")
        print(f"  {finding.evidence}")
        print(f"  Review: {rule.question}")
    print(f"code integrity: {len(paths)} files, {len(findings)} review candidates")
    if arguments.json is not None:
        arguments.json.parent.mkdir(parents=True, exist_ok=True)
        arguments.json.write_text(json.dumps([finding.record() for finding in findings], indent=2) + "\n", encoding="utf-8")
    return 1 if arguments.gate and findings else 0


if __name__ == "__main__":
    sys.exit(main())
