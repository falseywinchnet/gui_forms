"""Find source shapes that leave storage or object lifetime uncertain.

This is a source-level review aid. A finding identifies a house-style contract
that the code does not make locally evident; it is not a claim about runtime
behavior. The checker deliberately does not execute inputs or infer intent.

Every candidate ends in one status:

- raised: nothing explains it yet; it needs a review;
- guarded: a guard the checker recognizes protects it (GUARDS below);
- reviewed: a recorded decision in reviews.json covers it (defect, guarded or
  false_shape), so it is not raised again.

Paths under a scope reviews.json excludes (the Skia renderer and its C ABI,
which are being replaced) are not read at all.

The gate fails only on raised candidates, so a reviewed scope stays clean while
new code is checked.
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


# House-style sections in the importance order of 2026-10-27 (notes main
# HOUSE_STYLE.md): §1 Memory use, §2 Failure, §3 Types and values, §8 Repeated
# kernels.
RULES = {
    "I01": Rule("borrow used after owner changes", "§1", "Finish reads before changing the owner, or show that storage remains live."),
    "I02": Rule("live count published before construction", "§1, §2", "Construct first; publish only the completed prefix, including on failure."),
    "I03": Rule("assertion is the visible access bound", "§1, §2", "Establish the access bound in Release before forming the address."),
    "I04": Rule("empty buffer may form a pointer endpoint", "§8, §1", "Handle empty input before pointer arithmetic, or use a length-bearing view."),
    "I05": Rule("size narrowed without a visible bound", "§3, §1", "Check representability before conversion; keep capacity and live count distinct."),
    "I06": Rule("raw extent product at storage sink", "§8, §1", "Check the whole shape calculation before allocation or copy."),
    "I07": Rule("poison begins after named boundary", "§1", "Verify whether the first argument is already the exclusive live end."),
}

# What protects a site the rules would otherwise raise.
GUARDS = {
    # A checked helper computes the quantity: checked_multiply,
    # __builtin_mul_overflow, ckd_mul, SafeMath, FitsIn.
    "checked_arithmetic": "a checked arithmetic helper computes the quantity",
    # An earlier `if` in the same function bounds an operand and leaves
    # (return, throw, break, continue) when it is out of range.
    "dominating_validation": "an earlier validation bounds an operand and exits",
    # An earlier call admits or charges the operand and fails by name
    # (admit, charge, require, validate, check).
    "named_failure": "an earlier named admission fails for an operand out of range",
}
REVIEW_DECISIONS = ("defect", "guarded", "false_shape")
DEFAULT_REVIEWS = Path(__file__).resolve().parent / "reviews.json"


@dataclass(frozen=True)
class Finding:
    path: str
    line: int
    rule: str
    evidence: str
    related_line: int | None = None
    # The GUARDS key that protects the site, or "" when none was recognized.
    guard: str = ""

    def record(self) -> dict[str, object]:
        result: dict[str, object] = asdict(self)
        result["title"] = RULES[self.rule].title
        result["house_style"] = RULES[self.rule].style
        result["house_style_status"] = "invariant_not_evident_in_source"
        result["review"] = RULES[self.rule].question
        return result


@dataclass(frozen=True)
class Review:
    path: str
    rule: str
    evidence: str
    decision: str
    guard: str
    reason: str


@dataclass(frozen=True)
class Reviews:
    # Path prefixes (a directory ending in "/") or files never read.
    excluded_scopes: tuple[str, ...]
    entries: tuple[Review, ...]


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


def report(path: str, source: str, offset: int, rule: str, related: int | None = None, guard: str = "") -> Finding:
    related_line = None if related is None else line_at(source, related)
    return Finding(path, line_at(source, offset), rule, evidence(source, offset), related_line, guard)


# Guards ---------------------------------------------------------------------

CHECKED_HELPER = re.compile(r"\b(?:checked_\w+|\w+_checked|__builtin_(?:add|sub|mul)_overflow|ckd_(?:add|sub|mul)|"
                            r"SafeMath\w*|FitsIn\w*)\s*\(")
LIMIT_NAME = re.compile(r"\b(?:numeric_limits|SIZE_MAX|INT_MAX|UINT_MAX|INT32_MAX|UINT32_MAX|SafeMath|FitsIn|checked)\b")
EXIT = re.compile(r"\b(?:return|throw|break|continue)\b")
# Casts and limits use angle brackets that are not comparisons.
ANGLE_TYPES = re.compile(r"\b(?:\w+_cast|numeric_limits|std\s*::\s*\w+)\s*<[^<>;]*>")
RELATION = re.compile(r"<=|>=|<|>")
ADMISSION = r"\b(?:admit|charge\w*|require\w*|validate\w*|check\w*)\s*\([^;]*\b"


def balanced_close(text: str, open_index: int, left: str, right: str) -> int:
    depth = 0
    for index in range(open_index, len(text)):
        if text[index] == left:
            depth += 1
        elif text[index] == right:
            depth -= 1
            if depth == 0:
                return index
    return -1


def exiting_conditions(prefix: str) -> list[str]:
    """The conditions of `if` statements in prefix whose controlled statement leaves."""
    result: list[str] = []
    for match in re.finditer(r"\bif\s*\(", prefix):
        close = balanced_close(prefix, match.end() - 1, "(", ")")
        if close < 0:
            continue
        rest = prefix[close + 1:].lstrip()
        if rest.startswith("{"):
            body_end = balanced_close(rest, 0, "{", "}")
            controlled = rest if body_end < 0 else rest[:body_end + 1]
        else:
            semicolon = rest.find(";")
            controlled = rest if semicolon < 0 else rest[:semicolon + 1]
        if EXIT.search(controlled):
            result.append(prefix[match.end():close])
    return result


def recognized_guard(prefix: str, operands: list[str]) -> str:
    """The GUARDS key that protects a site whose quantity uses operands, or ""."""
    if CHECKED_HELPER.search(prefix[-600:]):
        return "checked_arithmetic"
    names = [name for name in operands if name and name not in {"this", "sizeof"}]
    for condition in exiting_conditions(prefix):
        plain = ANGLE_TYPES.sub(" ", condition)
        mentions = any(re.search(r"\b" + re.escape(name) + r"\b", plain) for name in names)
        # Only a bound counts: an emptiness test does not make a size fit.
        if mentions and RELATION.search(plain):
            return "dominating_validation"
    if LIMIT_NAME.search(prefix[-600:]):
        return "dominating_validation"
    for name in names:
        if re.search(ADMISSION + re.escape(name) + r"\b", prefix):
            return "named_failure"
    return ""


def product_operands(text: str) -> list[str]:
    operands: list[str] = []
    for match in re.finditer(r"\b([A-Za-z_]\w*)\s*\*\s*([A-Za-z_]\w*)\b", text):
        operands.extend([match.group(1), match.group(2)])
    return operands


def constant_extent(declarations: str, owner: str) -> bool:
    """Whether owner is declared with a literal extent: std::array<T, N>, or a vector built with N elements."""
    name = re.escape(owner)
    array = re.search(r"\bstd\s*::\s*array\s*<[^;]*?,\s*[1-9][0-9]*[uUlL]*\s*>\s*" + name + r"\b", declarations)
    sized = re.search(r"\bstd\s*::\s*vector\s*<[^;]*?>\s*" + name + r"\s*\(\s*[1-9][0-9]*[uUlL]*\s*\)", declarations)
    return array is not None or sized is not None


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
        # C++ defines a null pointer plus zero ([expr.add]), and a standard
        # view or vector's empty data() is at worst null: the endpoint of an
        # empty std::string_view, std::span or std::vector is valid. Custom
        # buffers stay candidates.
        standard_view = re.compile(r"\bstd\s*::\s*(?:string_view|wstring_view|u8string_view|span\s*<[^;]*?>|vector\s*<[^;]*?>)"
                                   r"\s*(?:const\s*)?[*&]?\s*" + re.escape(owner) + r"\b")
        if standard_view.search(declarations):
            continue
        guard = ""
        if re.search(r"\bif\s*\(\s*" + re.escape(owner) + r"\s*\.\s*empty\s*\(\s*\)\s*\)\s*(?:\{[^{}]*\breturn\b|return\b)", preceding):
            guard = "dominating_validation"
        result.append(report(path, source, function.start + match.start(), "I04", None, guard))
    return result


def narrowing_and_extents(path: str, source: str, function: Function) -> list[Finding]:
    result: list[Finding] = []
    narrowing = re.compile(r"\bstatic_cast\s*<\s*(?:int|int32_t|uint32_t|short|unsigned)\s*>\s*\(\s*(?P<expr>(?P<owner>[A-Za-z_]\w*)\s*\.\s*(?:size|length|count|capacity)\s*\(\s*\))\s*\)")
    for match in narrowing.finditer(function.text):
        prefix = function.text[:match.start()]
        owner = match.group("owner")
        declarations = source[max(0, function.start - 500):function.start] + prefix
        # A literal extent fits the narrow type: std::array<T, 8192>, or a
        # vector constructed with 128 elements.
        if constant_extent(declarations, owner):
            continue
        guard = recognized_guard(prefix, [owner])
        result.append(report(path, source, function.start + match.start(), "I05", None, guard))
    sink = re.compile(r"\b(?:malloc|calloc|realloc|memcpy|memmove|reserve|resize|allocate|SkMalloc|sk_malloc_throw)\s*\((?P<args>[^;()]{0,180})\)")
    for match in sink.finditer(function.text):
        arguments = match.group("args")
        if re.search(r"\b[A-Za-z_]\w*\s*\*\s*[A-Za-z_]\w*\b", arguments) is None:
            continue
        guard = recognized_guard(function.text[:match.start()], product_operands(arguments))
        result.append(report(path, source, function.start + match.start(), "I06", None, guard))
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
        guard = recognized_guard(function.text[:match.start()], product_operands(match.group(0)))
        result.append(report(path, source, function.start + match.start(), "I06",
                             function.start + match.end() + use.start(), guard))
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


def normalized_path(path: str) -> str:
    return path.replace("\\", "/").removeprefix("./")


def normalized_evidence(text: str) -> str:
    return " ".join(text.split())


def load_reviews(path: Path | None) -> Reviews:
    """Read the recorded decisions; a missing default file means no decisions yet."""
    if path is None or not path.exists():
        return Reviews((), ())
    data = json.loads(path.read_text(encoding="utf-8"))
    entries: list[Review] = []
    for item in data.get("reviews", []):
        decision = item["decision"]
        if decision not in REVIEW_DECISIONS:
            raise ValueError(f"{path}: unknown decision {decision!r} for {item['path']}")
        guard = item.get("guard", "")
        if guard and guard not in GUARDS:
            raise ValueError(f"{path}: unknown guard {guard!r} for {item['path']}")
        if item["rule"] not in RULES:
            raise ValueError(f"{path}: unknown rule {item['rule']!r} for {item['path']}")
        entries.append(Review(normalized_path(item["path"]), item["rule"], normalized_evidence(item["evidence"]),
                              decision, guard, item.get("reason", "")))
    scopes = tuple(normalized_path(scope) for scope in data.get("excluded_scopes", []))
    return Reviews(scopes, tuple(entries))


def excluded(path: Path, scopes: tuple[str, ...]) -> bool:
    """Whether a selected file lies in an excluded scope: the scope itself, or under a scope directory."""
    text = normalized_path(path.as_posix())
    scope: str
    for scope in scopes:
        directory = scope.rstrip("/") + "/"
        if text == scope.rstrip("/") or text.startswith(directory):
            return True
    return False


def classify(finding: Finding, reviews: Reviews) -> tuple[str, Review | None]:
    """The finding's status (reviewed, guarded or raised) and the review that decided it."""
    path = normalized_path(finding.path)
    key = (path, finding.rule, normalized_evidence(finding.evidence))
    for review in reviews.entries:
        if (review.path, review.rule, review.evidence) == key:
            return "reviewed", review
    if finding.guard:
        return "guarded", None
    return "raised", None


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("paths", nargs="+", help="explicit C/C++ files or directories")
    parser.add_argument("--json", type=Path, help="write the complete structured report")
    parser.add_argument("--reviews", type=Path, default=DEFAULT_REVIEWS,
                        help="recorded decisions and excluded scopes (default: reviews.json beside this script)")
    parser.add_argument("--gate", action="store_true",
                        help="fail on raised candidates: those neither guarded nor reviewed")
    arguments = parser.parse_args()
    try:
        selected = collect(arguments.paths)
        reviews = load_reviews(arguments.reviews)
    except (FileNotFoundError, ValueError, KeyError) as error:
        parser.error(str(error))
    paths: list[Path] = [path for path in selected if not excluded(path, reviews.excluded_scopes)]
    if not paths:
        parser.error("No C or C++ files selected; empty scope is not a pass.")
    findings: list[Finding] = []
    for path in paths:
        findings.extend(inspect(path.as_posix(), path.read_text(encoding="utf-8-sig", errors="replace")))
    counts: dict[str, int] = {"raised": 0, "guarded": 0, "reviewed": 0}
    records: list[dict[str, object]] = []
    apart: list[str] = []
    for finding in findings:
        status, review = classify(finding, reviews)
        counts[status] += 1
        record = finding.record()
        record["status"] = status
        if review is not None:
            record["decision"] = review.decision
            record["reason"] = review.reason
            if review.guard:
                record["guard"] = review.guard
        records.append(record)
        rule = RULES[finding.rule]
        if status == "raised":
            related = "" if finding.related_line is None else f" (related line {finding.related_line})"
            print(f"{finding.path}:{finding.line}: {finding.rule} {rule.title} [house style {rule.style}]{related}")
            print(f"  {finding.evidence}")
            print(f"  Review: {rule.question}")
        else:
            detail = finding.guard if status == "guarded" else (review.decision if review is not None else "")
            apart.append(f"  {status:8} {detail:22} {finding.path}:{finding.line}: {finding.rule}")
    if apart:
        print("Listed apart (guarded or reviewed):")
        for line in apart:
            print(line)
    print(f"code integrity: {len(paths)} files ({len(selected) - len(paths)} excluded), {len(findings)} candidates: "
          f"{counts['raised']} raised, {counts['guarded']} guarded, {counts['reviewed']} reviewed")
    if arguments.json is not None:
        arguments.json.parent.mkdir(parents=True, exist_ok=True)
        arguments.json.write_text(json.dumps(records, indent=2) + "\n", encoding="utf-8")
    return 1 if arguments.gate and counts["raised"] else 0


if __name__ == "__main__":
    sys.exit(main())
