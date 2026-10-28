"""The layout fixture corpus: its manifest, its box files and their comparison.

The corpus lives in tests/layout_corpus/ (notes:planning/stages/L-layout/PLAN.md
section 6.4):

- corpus.xml, the manifest: the profiles, the tolerances, and the fixtures
  in order. Every fixture is laid out at every left-to-right profile; a
  profile with direction="rtl" applies only to the fixtures the manifest
  marks bidirectional="true", whose root is then laid out right to left;
- fixtures/<name>.html, one well-formed HTML fragment per fixture, read by
  Chromium and, as XML, by the GUI.Forms corpus test;
- references/<name>.xml, Chromium's boxes for every profile, written by
  capture.py.

A box file (a reference from Chromium, or a native snapshot written by the
GUI.Forms corpus test) holds, per profile, one box per element that has a
data-gf-id. Both are read here, and compare_box_sets() is the one
comparison used by the reports and by the reference-drift check. The C++
corpus test applies the same tolerances to the same files.
"""

from __future__ import annotations

import math
import xml.etree.ElementTree as ElementTree
from dataclasses import dataclass
from pathlib import Path

# Text-bearing control kinds; a fixture with any of them is a text fixture:
# it takes the text tolerance, its text leaves carry a first baseline, and
# the text-scale profiles apply to it. A text leaf's text is its
# data-gf-text attribute (PLAN section 12.4): the element itself is empty,
# and the page builder writes the text as its content.
TEXT_KINDS = frozenset({"text-block"})
TEXT_ATTRIBUTE = "data-gf-text"


class CorpusError(ValueError):
    """A manifest, fixture or box file that does not follow the format."""


DIRECTIONS = ("ltr", "rtl")


@dataclass(frozen=True)
class Profile:
    width: int
    height: int
    scale: float
    # The root's inline direction: "ltr" or "rtl" (HTML's dir attribute).
    direction: str = "ltr"
    # The text scale: Chromium's root font size is 13 px times it, and
    # GUI.Forms' window text scale is it. Other than 1 only for text
    # fixtures.
    text_scale: float = 1.0

    @property
    def key(self) -> str:
        key = f"{self.width}x{self.height}@{format_number(self.scale)}"
        if self.direction != "ltr":
            key += f"/{self.direction}"
        if self.text_scale != 1.0:
            key += f"/text{format_number(self.text_scale * 100.0)}"
        return key


@dataclass(frozen=True)
class Tolerances:
    box_edge: float
    text_edge: float
    baseline: float
    reference_drift: float


@dataclass(frozen=True)
class Fixture:
    name: str
    path: Path
    is_text: bool
    expected_differences: dict[str, str]
    bidirectional: bool = False


@dataclass(frozen=True)
class Corpus:
    root: Path
    profiles: tuple[Profile, ...]
    tolerances: Tolerances
    fixtures: tuple[Fixture, ...]

    def profiles_for(self, fixture: Fixture) -> tuple[Profile, ...]:
        """The manifest's profiles that apply to the fixture, in manifest order.

        A right-to-left profile applies only to a bidirectional fixture, and
        a text-scale profile only to a text fixture.
        """
        return tuple(profile for profile in self.profiles
                     if (profile.direction == "ltr" or fixture.bidirectional) and
                     (profile.text_scale == 1.0 or fixture.is_text))

    def reference_path(self, fixture: Fixture, directory: Path | None = None) -> Path:
        base = directory if directory is not None else self.root / "references"
        return base / f"{fixture.name}.xml"


@dataclass(frozen=True)
class Box:
    x: float
    y: float
    width: float
    height: float
    # A text leaf's first baseline, in the same viewport coordinates as y;
    # None for every other node and for a text leaf with no line box.
    baseline: float | None = None


# profile key -> node id -> box, in document order.
BoxSet = dict[str, dict[str, Box]]


def format_number(value: float) -> str:
    """The shortest text that reads back as the same double; integers bare."""
    if not math.isfinite(value):
        raise CorpusError(f"a box value must be finite, not {value!r}")
    if value == int(value) and abs(value) < 1.0e15:
        return str(int(value))
    return repr(value)


def _number(element: ElementTree.Element, name: str, source: Path) -> float:
    text = element.get(name)
    if text is None:
        raise CorpusError(f"{source}: <{element.tag}> has no {name}")
    try:
        value = float(text)
    except ValueError as error:
        raise CorpusError(f"{source}: {name}={text!r} is not a number") from error
    if not math.isfinite(value):
        raise CorpusError(f"{source}: {name}={text!r} is not finite")
    return value


def _parse_profile(element: ElementTree.Element, source: Path) -> Profile:
    viewport = element.get("viewport", "")
    parts = viewport.split("x")
    if len(parts) != 2 or not parts[0].isdigit() or not parts[1].isdigit():
        raise CorpusError(f"{source}: viewport {viewport!r} is not WIDTHxHEIGHT")
    width = int(parts[0])
    height = int(parts[1])
    scale = _number(element, "scale", source)
    if not (1 <= width <= 8192 and 1 <= height <= 8192 and 0.5 <= scale <= 4.0):
        raise CorpusError(f"{source}: profile {viewport}@{scale} is out of range")
    direction = element.get("direction", "ltr")
    if direction not in DIRECTIONS:
        raise CorpusError(f"{source}: profile direction {direction!r} is not ltr or rtl")
    text_scale = 1.0
    if element.get("text-scale") is not None:
        text_scale = _number(element, "text-scale", source)
    if not 0.5 <= text_scale <= 4.0:
        raise CorpusError(f"{source}: profile text-scale {text_scale} is out of range")
    return Profile(width, height, scale, direction, text_scale)


def _walk(element: ElementTree.Element, is_text: list[bool], expected: dict[str, str],
          identities: set[str], source: Path) -> None:
    kind = element.get("data-gf-kind")
    identity = element.get("data-gf-id")
    if identity is not None:
        if identity in identities:
            raise CorpusError(f"{source}: data-gf-id {identity!r} repeats")
        identities.add(identity)
        reason = element.get("data-gf-expected-difference")
        if reason is not None:
            if not reason.strip():
                raise CorpusError(f"{source}: {identity}: an expected difference needs a reason")
            expected[identity] = reason
    if kind in TEXT_KINDS:
        is_text[0] = True
        if element.get(TEXT_ATTRIBUTE) is None:
            raise CorpusError(f"{source}: {identity}: a {kind} needs {TEXT_ATTRIBUTE}")
        if len(element) != 0 or (element.text or "") != "":
            raise CorpusError(f"{source}: {identity}: a {kind} is empty; its text is {TEXT_ATTRIBUTE}")
    elif element.get(TEXT_ATTRIBUTE) is not None:
        raise CorpusError(f"{source}: {identity}: only a text leaf carries {TEXT_ATTRIBUTE}")
    for child in element:
        _walk(child, is_text, expected, identities, source)


def read_fixture(name: str, path: Path, bidirectional: bool = False) -> Fixture:
    try:
        root = ElementTree.parse(path).getroot()
    except ElementTree.ParseError as error:
        raise CorpusError(f"{path}: not well-formed: {error}") from error
    if root.get("data-gf-id") is None or root.get("data-gf-kind") is None:
        raise CorpusError(f"{path}: the root element needs data-gf-id and data-gf-kind")
    is_text = [False]
    expected: dict[str, str] = {}
    _walk(root, is_text, expected, set(), path)
    return Fixture(name, path, is_text[0], expected, bidirectional)


def read_corpus(root: Path) -> Corpus:
    manifest = root / "corpus.xml"
    try:
        document = ElementTree.parse(manifest).getroot()
    except (OSError, ElementTree.ParseError) as error:
        raise CorpusError(f"{manifest}: {error}") from error
    if document.tag != "layout-corpus":
        raise CorpusError(f"{manifest}: the root element must be <layout-corpus>")
    profiles = tuple(_parse_profile(element, manifest) for element in document.iter("profile"))
    if not profiles:
        raise CorpusError(f"{manifest}: no profiles")
    if len({profile.key for profile in profiles}) != len(profiles):
        raise CorpusError(f"{manifest}: a profile repeats")
    tolerance = document.find("tolerance")
    if tolerance is None:
        raise CorpusError(f"{manifest}: no <tolerance>")
    tolerances = Tolerances(
        box_edge=_number(tolerance, "box-edge", manifest),
        text_edge=_number(tolerance, "text-edge", manifest),
        baseline=_number(tolerance, "baseline", manifest),
        reference_drift=_number(tolerance, "reference-drift", manifest),
    )
    fixtures = []
    names: set[str] = set()
    for element in document.iter("fixture"):
        name = element.get("name", "")
        if not name or name in names or ".." in name or name.startswith("/"):
            raise CorpusError(f"{manifest}: fixture name {name!r} is empty, repeated or unsafe")
        names.add(name)
        marked = element.get("bidirectional", "false")
        if marked not in ("true", "false"):
            raise CorpusError(f"{manifest}: {name}: bidirectional is true or false")
        fixtures.append(read_fixture(name, root / "fixtures" / f"{name}.html", marked == "true"))
    return Corpus(root, profiles, tolerances, tuple(fixtures))


def read_box_file(path: Path) -> tuple[dict[str, str], BoxSet]:
    """The root attributes and the boxes of a reference or native snapshot."""
    try:
        document = ElementTree.parse(path).getroot()
    except (OSError, ElementTree.ParseError) as error:
        raise CorpusError(f"{path}: {error}") from error
    if document.tag not in ("layout-reference", "layout-native"):
        raise CorpusError(f"{path}: not a layout box file")
    boxes: BoxSet = {}
    for profile_element in document.findall("profile"):
        profile = _parse_profile(profile_element, path)
        nodes: dict[str, Box] = {}
        for node in profile_element.findall("node"):
            identity = node.get("id")
            if identity is None or identity in nodes:
                raise CorpusError(f"{path}: a node id is missing or repeated")
            baseline = None
            if node.get("baseline") is not None:
                baseline = _number(node, "baseline", path)
            nodes[identity] = Box(_number(node, "x", path), _number(node, "y", path),
                                  _number(node, "width", path), _number(node, "height", path),
                                  baseline)
        if profile.key in boxes:
            raise CorpusError(f"{path}: profile {profile.key} repeats")
        boxes[profile.key] = nodes
    return dict(document.attrib), boxes


def write_box_file(path: Path, tag: str, attributes: dict[str, str],
                   profiles: tuple[Profile, ...], boxes: BoxSet) -> None:
    """Writes a box file with fixed formatting, so references diff cleanly."""
    lines = ['<?xml version="1.0" encoding="UTF-8"?>']
    attribute_text = "".join(
        f' {name}="{_escape(value)}"' for name, value in attributes.items())
    lines.append(f"<{tag}{attribute_text}>")
    for profile in profiles:
        direction = "" if profile.direction == "ltr" else f' direction="{profile.direction}"'
        text_scale = ("" if profile.text_scale == 1.0
                      else f' text-scale="{format_number(profile.text_scale)}"')
        lines.append(f'  <profile viewport="{profile.width}x{profile.height}" '
                     f'scale="{format_number(profile.scale)}"{direction}{text_scale}>')
        for identity, box in boxes[profile.key].items():
            baseline = "" if box.baseline is None else f' baseline="{format_number(box.baseline)}"'
            lines.append(
                f'    <node id="{_escape(identity)}" x="{format_number(box.x)}" '
                f'y="{format_number(box.y)}" width="{format_number(box.width)}" '
                f'height="{format_number(box.height)}"{baseline}/>')
        lines.append("  </profile>")
    lines.append(f"</{tag}>")
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="\n")


def _escape(text: str) -> str:
    return (text.replace("&", "&amp;").replace("<", "&lt;")
            .replace(">", "&gt;").replace('"', "&quot;"))


@dataclass(frozen=True)
class NodeDifference:
    profile: str
    identity: str
    # The largest of the four edge differences (left, top, right, bottom).
    edge: float
    reference: Box | None
    observed: Box | None
    expected_reason: str | None
    # The first-baseline difference: 0 where neither side has a baseline,
    # infinite where only one has.
    baseline: float = 0.0

    @property
    def missing(self) -> bool:
        return self.reference is None or self.observed is None


@dataclass(frozen=True)
class Comparison:
    nodes: int
    over_tolerance: tuple[NodeDifference, ...]
    recorded: tuple[NodeDifference, ...]
    largest_edge: float
    largest_baseline: float = 0.0


def edge_difference(reference: Box, observed: Box) -> float:
    left = abs(reference.x - observed.x)
    top = abs(reference.y - observed.y)
    right = abs((reference.x + reference.width) - (observed.x + observed.width))
    bottom = abs((reference.y + reference.height) - (observed.y + observed.height))
    return max(left, top, right, bottom)


def baseline_difference(reference: Box, observed: Box) -> float:
    """0 when neither box has a baseline; infinite when only one has."""
    if reference.baseline is None and observed.baseline is None:
        return 0.0
    if reference.baseline is None or observed.baseline is None:
        return math.inf
    return abs(reference.baseline - observed.baseline)


def compare_box_sets(reference: BoxSet, observed: BoxSet, tolerance: float,
                     expected: dict[str, str],
                     baseline_tolerance: float | None = None) -> Comparison:
    """Every node of every profile, compared edge by edge and by baseline.

    A node over tolerance (or missing on one side) is listed. One that
    carries a recorded expected difference is listed separately and still
    counted; nothing is folded into a pass. The baseline tolerance is the
    edge tolerance unless given.
    """
    baseline_bound = tolerance if baseline_tolerance is None else baseline_tolerance
    over: list[NodeDifference] = []
    recorded: list[NodeDifference] = []
    largest = 0.0
    largest_baseline = 0.0
    nodes = 0
    for profile in sorted(set(reference) | set(observed)):
        reference_nodes = reference.get(profile, {})
        observed_nodes = observed.get(profile, {})
        identities = list(reference_nodes)
        for identity in observed_nodes:
            if identity not in reference_nodes:
                identities.append(identity)
        for identity in identities:
            nodes += 1
            reference_box = reference_nodes.get(identity)
            observed_box = observed_nodes.get(identity)
            baseline = 0.0
            if reference_box is None or observed_box is None:
                edge = math.inf
            else:
                edge = edge_difference(reference_box, observed_box)
                largest = max(largest, edge)
                baseline = baseline_difference(reference_box, observed_box)
                if math.isfinite(baseline):
                    largest_baseline = max(largest_baseline, baseline)
            if edge <= tolerance and baseline <= baseline_bound:
                continue
            difference = NodeDifference(profile, identity, edge, reference_box, observed_box,
                                        expected.get(identity), baseline)
            if difference.expected_reason is not None:
                recorded.append(difference)
            else:
                over.append(difference)
    return Comparison(nodes, tuple(over), tuple(recorded), largest, largest_baseline)
