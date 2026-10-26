"""Compare layout box files: references against a native snapshot, or
regenerated references against the checked-in ones.

    python -B tools/layout_oracle/compare.py report --corpus tests/layout_corpus
        --native <dir written by gui_forms_layout_corpus_tests --native-out>
        [--output report.json]
    python -B tools/layout_oracle/compare.py drift --corpus tests/layout_corpus
        --regenerated <dir written by capture.py>

report lists every node over tolerance and every recorded difference, per
fixture and profile, with totals; its exit status is nonzero when a node is
over tolerance without a recorded reason. drift checks that a fresh Chromium
capture still matches the checked-in references within the manifest's
reference-drift bound (one LayoutUnit); the layout-oracle CI job runs it.
Nothing is folded into a pass: recorded differences are listed and counted.
"""

from __future__ import annotations

import argparse
import json
import math
import sys
from pathlib import Path

if __package__ in (None, ""):
    sys.path.insert(0, str(Path(__file__).resolve().parent))

from corpus import (Comparison, Corpus, CorpusError, NodeDifference,  # noqa: E402
                    compare_box_sets, read_box_file, read_corpus)


def _box_text(difference: NodeDifference, side: str) -> object:
    box = difference.reference if side == "reference" else difference.observed
    if box is None:
        return None
    return {"x": box.x, "y": box.y, "width": box.width, "height": box.height}


def _difference_record(difference: NodeDifference) -> dict[str, object]:
    return {
        "profile": difference.profile,
        "id": difference.identity,
        "edge": None if math.isinf(difference.edge) else difference.edge,
        "missing": difference.missing,
        "reference": _box_text(difference, "reference"),
        "observed": _box_text(difference, "observed"),
        "reason": difference.expected_reason,
    }


def report(corpus: Corpus, native: Path) -> tuple[dict[str, object], bool]:
    fixtures: list[dict[str, object]] = []
    passed = True
    totals = {"nodes": 0, "over_tolerance": 0, "recorded": 0, "largest_edge": 0.0}
    for fixture in corpus.fixtures:
        tolerance = corpus.tolerances.text_edge if fixture.is_text else corpus.tolerances.box_edge
        _, reference = read_box_file(corpus.reference_path(fixture))
        _, observed = read_box_file(native / f"{fixture.name}.xml")
        comparison: Comparison = compare_box_sets(reference, observed, tolerance,
                                                  fixture.expected_differences)
        passed = passed and not comparison.over_tolerance
        totals["nodes"] += comparison.nodes
        totals["over_tolerance"] += len(comparison.over_tolerance)
        totals["recorded"] += len(comparison.recorded)
        totals["largest_edge"] = max(totals["largest_edge"], comparison.largest_edge)
        fixtures.append({
            "fixture": fixture.name,
            "class": "text" if fixture.is_text else "box",
            "bidirectional": fixture.bidirectional,
            "tolerance": tolerance,
            "nodes": comparison.nodes,
            "largest_edge": comparison.largest_edge,
            "over_tolerance": [_difference_record(item) for item in comparison.over_tolerance],
            "recorded": [_difference_record(item) for item in comparison.recorded],
        })
    document = {
        "schema": "gui-forms.layout-oracle-report/1",
        "profiles": [profile.key for profile in corpus.profiles],
        "bidirectional_fixtures": sum(1 for fixture in corpus.fixtures if fixture.bidirectional),
        "tolerances": {
            "box_edge": corpus.tolerances.box_edge,
            "text_edge": corpus.tolerances.text_edge,
            "baseline": corpus.tolerances.baseline,
        },
        "totals": totals,
        "fixtures": fixtures,
    }
    return document, passed


def drift(corpus: Corpus, regenerated: Path) -> bool:
    passed = True
    largest = 0.0
    for fixture in corpus.fixtures:
        checked_in_attributes, checked_in = read_box_file(corpus.reference_path(fixture))
        fresh_attributes, fresh = read_box_file(corpus.reference_path(fixture, regenerated))
        if checked_in_attributes.get("chromium") != fresh_attributes.get("chromium"):
            print(f"{fixture.name}: Chromium {fresh_attributes.get('chromium')} is not the "
                  f"checked-in {checked_in_attributes.get('chromium')}")
            passed = False
        comparison = compare_box_sets(checked_in, fresh, corpus.tolerances.reference_drift, {})
        largest = max(largest, comparison.largest_edge)
        for difference in comparison.over_tolerance:
            print(f"{fixture.name} {difference.profile} #{difference.identity}: moved "
                  f"{difference.edge} from the checked-in reference")
            passed = False
    print(f"drift: {len(corpus.fixtures)} fixtures, largest movement {largest}, "
          f"{'within' if passed else 'OVER'} {corpus.tolerances.reference_drift}")
    return passed


def main(arguments: list[str]) -> int:
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    commands = parser.add_subparsers(dest="command", required=True)
    report_parser = commands.add_parser("report")
    report_parser.add_argument("--corpus", type=Path, required=True)
    report_parser.add_argument("--native", type=Path, required=True)
    report_parser.add_argument("--output", type=Path)
    drift_parser = commands.add_parser("drift")
    drift_parser.add_argument("--corpus", type=Path, required=True)
    drift_parser.add_argument("--regenerated", type=Path, required=True)
    options = parser.parse_args(arguments)
    try:
        corpus = read_corpus(options.corpus)
        if options.command == "drift":
            return 0 if drift(corpus, options.regenerated) else 1
        document, passed = report(corpus, options.native)
    except CorpusError as error:
        print(f"compare: {error}", file=sys.stderr)
        return 2
    text = json.dumps(document, indent=1, sort_keys=False) + "\n"
    if options.output is not None:
        options.output.write_text(text, encoding="utf-8")
    totals = document["totals"]
    print(f"report: {totals['nodes']} nodes, {totals['over_tolerance']} over tolerance, "
          f"{totals['recorded']} recorded, largest edge difference {totals['largest_edge']}")
    return 0 if passed else 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
