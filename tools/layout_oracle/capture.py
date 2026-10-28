"""Capture Chromium's reference boxes for the layout corpus.

    python -B tools/layout_oracle/capture.py --chromium <chrome-headless-shell>
        --corpus tests/layout_corpus --output <new-or-empty-directory>
        [--fixture <name> ...] [--platform win64|linux64] [--no-sandbox]

Chromium is a test oracle only (L README, owner decision 2): this script
runs at test-authoring time or in the layout-oracle CI job, never from
GUI.Forms. It starts the pinned headless shell once, loads each fixture in
the page template, and for every profile that applies to it sets the
viewport, the device scale and the root's direction (the dir attribute of
<html>, absent for left to right), waits for fonts, and reads
getBoundingClientRect() for every element with a data-gf-id. The boxes go to <output>/<name>.xml in the
box-file format of corpus.py.

A text fixture (PLAN section 12.4) is built with each text leaf's
data-gf-text written as the element's content, so Chromium lays out a real
text node. A text-scale profile sets the root font size to 13 px times the
scale. After the boxes are read, a zero-size inline-block probe is inserted
as the first child of every text leaf with visible text: the probe's top is
the leaf's first baseline. The boxes are read again with the probes in
place, and a capture whose boxes moved is refused, so a probe never changes
what is recorded. --no-sandbox is for CI runners whose kernel
refuses Chromium's sandbox; the page is a local fixture with no script but
the capture expression.
"""

from __future__ import annotations

import argparse
import sys
import tempfile
import xml.etree.ElementTree as ElementTree
from pathlib import Path

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parent))

from corpus import (TEXT_ATTRIBUTE, TEXT_KINDS, Box, BoxSet,  # noqa: E402
                    CorpusError, Fixture, Profile, read_corpus, write_box_file)
from devtools import Browser, DevToolsError, Session  # noqa: E402

TOOL_DIRECTORY = Path(__file__).resolve().parent
REPOSITORY = TOOL_DIRECTORY.parent.parent

# The faces the template declares. Each is a bundled file (assets/fonts), so
# Chromium and GUI.Forms shape with the same file.
FONT_FACES = (
    ("Carlito", "normal", "normal", "Carlito-Regular.ttf"),
    ("Carlito", "bold", "normal", "Carlito-Bold.ttf"),
    ("Carlito", "normal", "italic", "Carlito-Italic.ttf"),
    ("Portsmouth Rapids", "normal", "normal", "PortsmouthRapids.ttf"),
    ("Portsmouth Rapids", "bold", "normal", "PortsmouthRapids-Bold.ttf"),
)

# The template's root font size, which a text-scale profile multiplies.
ROOT_FONT_SIZE = 13.0

# Called with whether to probe the text leaves' first baselines. A leaf with
# no visible text (CSS white space only) has no line box, and so no
# baseline: it gets no probe, which would make one.
CAPTURE_FUNCTION = """
async function captureLayoutFixture(probeBaselines) {
  await document.fonts.ready;
  function readBoxes() {
    const nodes = [];
    for (const element of document.querySelectorAll('[data-gf-id]')) {
      const rectangle = element.getBoundingClientRect();
      nodes.push([element.getAttribute('data-gf-id'), rectangle.x, rectangle.y,
                  rectangle.width, rectangle.height]);
    }
    return nodes;
  }
  const nodes = readBoxes();
  const baselines = {};
  const moved = [];
  if (probeBaselines) {
    const probes = [];
    for (const leaf of document.querySelectorAll('[data-gf-kind="text-block"]')) {
      if (/^[ \\t\\n\\r\\f]*$/.test(leaf.textContent)) continue;
      // The probe goes before the first visible character, so leading
      // white space still collapses away at the line's start, and a word
      // joiner after it keeps the first word on the probe's line.
      const text = leaf.firstChild;
      const start = text.data.search(/[^ \\t\\n\\r\\f]/);
      const rest = start > 0 ? text.splitText(start) : text;
      const probe = document.createElement('span');
      probe.style.cssText = 'display: inline-block; width: 0; height: 0; vertical-align: baseline';
      const joiner = document.createTextNode('\\u2060');
      leaf.insertBefore(probe, rest);
      leaf.insertBefore(joiner, rest);
      probes.push([leaf.getAttribute('data-gf-id'), probe, joiner, leaf]);
    }
    for (const entry of probes) {
      baselines[entry[0]] = entry[1].getBoundingClientRect().y;
    }
    const probed = readBoxes();
    for (let index = 0; index < nodes.length; ++index) {
      if (JSON.stringify(probed[index]) !== JSON.stringify(nodes[index])) {
        moved.push(JSON.stringify(nodes[index]) + ' to ' + JSON.stringify(probed[index]));
      }
    }
    for (const entry of probes) {
      entry[1].remove();
      entry[2].remove();
      entry[3].normalize();
    }
  }
  return {width: window.innerWidth, height: window.innerHeight,
          scale: window.devicePixelRatio, nodes: nodes, baselines: baselines, moved: moved};
}
"""


def font_face_rules(font_directory: Path) -> str:
    rules = []
    for family, weight, style, file_name in FONT_FACES:
        path = font_directory / file_name
        if not path.is_file():
            raise CorpusError(f"bundled face {path} is missing")
        rules.append(
            f'@font-face {{ font-family: "{family}"; font-weight: {weight}; '
            f'font-style: {style}; src: url("{path.resolve().as_uri()}"); }}')
    return "\n".join(rules)


def text_fragment(fragment: str) -> str:
    """The fragment with each text leaf's data-gf-text as its content.

    The serializer escapes the text; the rest of the markup changes only as
    HTML serialization requires (an empty element gains its end tag).
    """
    root = ElementTree.fromstring(fragment)
    for element in root.iter():
        if element.get("data-gf-kind") in TEXT_KINDS:
            element.text = element.get(TEXT_ATTRIBUTE, "")
    return ElementTree.tostring(root, encoding="unicode", method="html")


def build_page(template: str, faces: str, fixture: Fixture) -> str:
    fragment = fixture.path.read_text(encoding="utf-8")
    if fixture.is_text:
        fragment = text_fragment(fragment)
    return template.replace("@@FONT_FACES@@", faces).replace("@@FIXTURE@@", fragment)


def direction_expression(direction: str) -> str:
    # Left to right removes the attribute, so a left-to-right capture is the
    # page exactly as the template builds it.
    if direction == "ltr":
        return "document.documentElement.removeAttribute('dir')"
    return f"document.documentElement.setAttribute('dir', '{direction}')"


def text_scale_expression(text_scale: float) -> str:
    # A scale of 1 removes the inline size, so the template's own root font
    # size applies.
    if text_scale == 1.0:
        return "document.documentElement.style.removeProperty('font-size')"
    size = ROOT_FONT_SIZE * text_scale
    return f"document.documentElement.style.fontSize = '{size!r}px'"


def evaluate(session: Session, expression: str, what: str) -> object:
    result = session.call("Runtime.evaluate", {
        "expression": expression,
        "awaitPromise": True,
        "returnByValue": True,
    })
    if "exceptionDetails" in result:
        raise DevToolsError(f"{what} failed: {result['exceptionDetails']}")
    return result["result"].get("value")


def capture_fixture(session: Session, fixture: Fixture, profiles: tuple[Profile, ...],
                    page: Path) -> BoxSet:
    session.call("Page.navigate", {"url": page.resolve().as_uri()})
    session.wait_for_event("Page.loadEventFired")
    boxes: BoxSet = {}
    for profile in profiles:
        session.call("Emulation.setDeviceMetricsOverride", {
            "width": profile.width,
            "height": profile.height,
            "deviceScaleFactor": profile.scale,
            "mobile": False,
        })
        evaluate(session, direction_expression(profile.direction), "setting the direction")
        evaluate(session, text_scale_expression(profile.text_scale), "setting the text scale")
        probe = "true" if fixture.is_text else "false"
        value = evaluate(session, f"({CAPTURE_FUNCTION})({probe})", "capture")
        if (value["width"] != profile.width or value["height"] != profile.height or
                value["scale"] != profile.scale):
            raise DevToolsError(
                f"Chromium laid out {value['width']}x{value['height']}@{value['scale']}, "
                f"not {profile.key}")
        if value["moved"]:
            raise DevToolsError(f"{fixture.name} {profile.key}: a baseline probe moved "
                                f"{'; '.join(value['moved'])}")
        baselines = value["baselines"]
        nodes: dict[str, Box] = {}
        for identity, x, y, width, height in value["nodes"]:
            baseline = baselines.get(identity)
            nodes[identity] = Box(float(x), float(y), float(width), float(height),
                                  None if baseline is None else float(baseline))
        boxes[profile.key] = nodes
    return boxes


def chromium_version(session: Session) -> str:
    product = session.call("Runtime.evaluate", {
        "expression": "navigator.userAgent", "returnByValue": True})["result"]["value"]
    marker = "HeadlessChrome/"
    start = product.find(marker)
    if start < 0:
        raise DevToolsError(f"not a headless shell: {product}")
    return product[start + len(marker):].split(" ", 1)[0]


def main(arguments: list[str]) -> int:
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("--chromium", type=Path, required=True)
    parser.add_argument("--corpus", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--fixture", action="append", default=[])
    parser.add_argument("--platform", default="")
    parser.add_argument("--fonts", type=Path, default=REPOSITORY / "assets" / "fonts")
    parser.add_argument("--no-sandbox", action="store_true")
    options = parser.parse_args(arguments)

    corpus = read_corpus(options.corpus)
    selected = [fixture for fixture in corpus.fixtures
                if not options.fixture or fixture.name in options.fixture]
    unknown = set(options.fixture) - {fixture.name for fixture in corpus.fixtures}
    if unknown:
        print(f"capture: unknown fixtures {sorted(unknown)}", file=sys.stderr)
        return 2
    if options.output.exists() and any(options.output.iterdir()) and not options.fixture:
        print(f"capture: {options.output} must be new or empty", file=sys.stderr)
        return 2
    template = (TOOL_DIRECTORY / "page_template.html").read_text(encoding="utf-8")
    faces = font_face_rules(options.fonts)

    # On Windows a Chromium helper process can still hold a profile file for
    # a moment after the browser exits; the scratch directory is then left
    # for the system's temporary-file cleanup rather than failing a capture
    # whose references are already written.
    with tempfile.TemporaryDirectory(prefix="gf-layout-oracle-", ignore_cleanup_errors=True) as scratch:
        scratch_path = Path(scratch)
        extra = ["--no-sandbox"] if options.no_sandbox else []
        browser = Browser(options.chromium, scratch_path / "profile", extra)
        try:
            session = browser.open_page()
            session.call("Page.enable")
            version = chromium_version(session)
            for fixture in selected:
                page = scratch_path / "pages" / f"{fixture.name}.html"
                page.parent.mkdir(parents=True, exist_ok=True)
                page.write_text(build_page(template, faces, fixture), encoding="utf-8")
                profiles = corpus.profiles_for(fixture)
                boxes = capture_fixture(session, fixture, profiles, page)
                attributes = {"fixture": fixture.name, "chromium": version}
                if options.platform:
                    attributes["platform"] = options.platform
                write_box_file(corpus.reference_path(fixture, options.output),
                               "layout-reference", attributes, profiles, boxes)
                print(f"captured {fixture.name}")
        finally:
            browser.close()
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
