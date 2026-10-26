"""Capture Chromium's reference boxes for the layout corpus.

    python -B tools/layout_oracle/capture.py --chromium <chrome-headless-shell>
        --corpus tests/layout_corpus --output <new-or-empty-directory>
        [--fixture <name> ...] [--platform win64|linux64] [--no-sandbox]

Chromium is a test oracle only (L README, owner decision 2): this script
runs at test-authoring time or in the layout-oracle CI job, never from
GUI.Forms. It starts the pinned headless shell once, loads each fixture in
the page template, and for every profile of the manifest sets the viewport
and device scale, waits for fonts, and reads getBoundingClientRect() for
every element with a data-gf-id. The boxes go to <output>/<name>.xml in the
box-file format of corpus.py. --no-sandbox is for CI runners whose kernel
refuses Chromium's sandbox; the page is a local fixture with no script but
the capture expression.
"""

from __future__ import annotations

import argparse
import sys
import tempfile
from pathlib import Path

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parent))

from corpus import Box, BoxSet, Corpus, CorpusError, Fixture, read_corpus, write_box_file  # noqa: E402
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

CAPTURE_EXPRESSION = """
(async function captureLayoutFixture() {
  await document.fonts.ready;
  const nodes = [];
  for (const element of document.querySelectorAll('[data-gf-id]')) {
    const rectangle = element.getBoundingClientRect();
    nodes.push([element.getAttribute('data-gf-id'), rectangle.x, rectangle.y,
                rectangle.width, rectangle.height]);
  }
  return {width: window.innerWidth, height: window.innerHeight,
          scale: window.devicePixelRatio, nodes: nodes};
})()
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


def build_page(template: str, faces: str, fixture: Fixture) -> str:
    fragment = fixture.path.read_text(encoding="utf-8")
    return template.replace("@@FONT_FACES@@", faces).replace("@@FIXTURE@@", fragment)


def capture_fixture(session: Session, corpus: Corpus, page: Path) -> BoxSet:
    session.call("Page.navigate", {"url": page.resolve().as_uri()})
    session.wait_for_event("Page.loadEventFired")
    boxes: BoxSet = {}
    for profile in corpus.profiles:
        session.call("Emulation.setDeviceMetricsOverride", {
            "width": profile.width,
            "height": profile.height,
            "deviceScaleFactor": profile.scale,
            "mobile": False,
        })
        result = session.call("Runtime.evaluate", {
            "expression": CAPTURE_EXPRESSION,
            "awaitPromise": True,
            "returnByValue": True,
        })
        if "exceptionDetails" in result:
            raise DevToolsError(f"capture failed: {result['exceptionDetails']}")
        value = result["result"]["value"]
        if (value["width"] != profile.width or value["height"] != profile.height or
                value["scale"] != profile.scale):
            raise DevToolsError(
                f"Chromium laid out {value['width']}x{value['height']}@{value['scale']}, "
                f"not {profile.key}")
        nodes: dict[str, Box] = {}
        for identity, x, y, width, height in value["nodes"]:
            nodes[identity] = Box(float(x), float(y), float(width), float(height))
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
                boxes = capture_fixture(session, corpus, page)
                attributes = {"fixture": fixture.name, "chromium": version}
                if options.platform:
                    attributes["platform"] = options.platform
                write_box_file(corpus.reference_path(fixture, options.output),
                               "layout-reference", attributes, corpus.profiles, boxes)
                print(f"captured {fixture.name}")
        finally:
            browser.close()
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
