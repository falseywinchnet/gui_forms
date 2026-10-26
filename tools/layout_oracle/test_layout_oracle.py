"""Tests of the layout oracle's own logic. No Chromium runs here.

    python -B -m unittest discover -s tools/layout_oracle -p test_layout_oracle.py
"""

from __future__ import annotations

import io
import json
import socket
import struct
import sys
import tempfile
import unittest
import zipfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

import compare  # noqa: E402
import corpus  # noqa: E402
import devtools  # noqa: E402
import fetch_chromium  # noqa: E402

REPOSITORY = Path(__file__).resolve().parent.parent.parent
CORPUS = REPOSITORY / "tests" / "layout_corpus"


def box_set(profile: str, **nodes: tuple[float, float, float, float]) -> corpus.BoxSet:
    return {profile: {name: corpus.Box(*values) for name, values in nodes.items()}}


class NumberFormatTests(unittest.TestCase):
    def test_integers_are_bare_and_fractions_round_trip(self) -> None:
        self.assertEqual(corpus.format_number(12.0), "12")
        self.assertEqual(corpus.format_number(-0.0), "0")
        self.assertEqual(float(corpus.format_number(333.328125)), 333.328125)
        self.assertEqual(float(corpus.format_number(1.0 / 3.0)), 1.0 / 3.0)

    def test_non_finite_values_are_refused(self) -> None:
        with self.assertRaises(corpus.CorpusError):
            corpus.format_number(float("inf"))


class ComparisonTests(unittest.TestCase):
    def test_edge_difference_is_the_largest_edge(self) -> None:
        reference = corpus.Box(10.0, 20.0, 100.0, 50.0)
        observed = corpus.Box(10.0, 20.5, 100.0, 50.0)
        self.assertAlmostEqual(corpus.edge_difference(reference, observed), 0.5)
        wider = corpus.Box(10.0, 20.0, 103.0, 50.0)
        self.assertAlmostEqual(corpus.edge_difference(reference, wider), 3.0)

    def test_within_tolerance_passes_and_over_is_listed(self) -> None:
        reference = box_set("p", a=(0, 0, 10, 10), b=(10, 0, 10, 10))
        observed = box_set("p", a=(0.04, 0, 10, 10), b=(10, 0.2, 10, 10))
        result = corpus.compare_box_sets(reference, observed, 0.05, {})
        self.assertEqual(result.nodes, 2)
        self.assertEqual([item.identity for item in result.over_tolerance], ["b"])
        self.assertAlmostEqual(result.largest_edge, 0.2)

    def test_recorded_difference_is_listed_apart_and_still_counted(self) -> None:
        reference = box_set("p", a=(0, 0, 10, 10))
        observed = box_set("p", a=(5, 0, 10, 10))
        result = corpus.compare_box_sets(reference, observed, 0.05, {"a": "known reason"})
        self.assertEqual(result.over_tolerance, ())
        self.assertEqual(len(result.recorded), 1)
        self.assertEqual(result.recorded[0].expected_reason, "known reason")

    def test_missing_nodes_on_either_side_fail(self) -> None:
        reference = box_set("p", a=(0, 0, 10, 10), b=(0, 0, 1, 1))
        observed = box_set("p", a=(0, 0, 10, 10), c=(0, 0, 1, 1))
        result = corpus.compare_box_sets(reference, observed, 0.05, {})
        missing = sorted(item.identity for item in result.over_tolerance)
        self.assertEqual(missing, ["b", "c"])
        self.assertTrue(all(item.missing for item in result.over_tolerance))


class BoxFileTests(unittest.TestCase):
    def test_write_then_read_round_trips(self) -> None:
        profiles = (corpus.Profile(320, 240, 1.0), corpus.Profile(800, 600, 2.0))
        boxes = {
            profiles[0].key: {"root": corpus.Box(0, 0, 320, 240), "a": corpus.Box(1.5, 2, 3, 4)},
            profiles[1].key: {"root": corpus.Box(0, 0, 800, 600), "a": corpus.Box(1.5, 2, 3, 4)},
        }
        with tempfile.TemporaryDirectory() as scratch:
            path = Path(scratch) / "group" / "fixture.xml"
            corpus.write_box_file(path, "layout-reference", {"fixture": "group/fixture"},
                                  profiles, boxes)
            attributes, read = corpus.read_box_file(path)
        self.assertEqual(attributes["fixture"], "group/fixture")
        self.assertEqual(read, boxes)

    def test_repeated_node_is_refused(self) -> None:
        text = ('<layout-reference><profile viewport="10x10" scale="1">'
                '<node id="a" x="0" y="0" width="1" height="1"/>'
                '<node id="a" x="0" y="0" width="1" height="1"/></profile></layout-reference>')
        with tempfile.TemporaryDirectory() as scratch:
            path = Path(scratch) / "bad.xml"
            path.write_text(text, encoding="utf-8")
            with self.assertRaises(corpus.CorpusError):
                corpus.read_box_file(path)


class CorpusTests(unittest.TestCase):
    def test_checked_in_corpus_reads_and_every_fixture_has_a_reference(self) -> None:
        checked_in = corpus.read_corpus(CORPUS)
        self.assertGreaterEqual(len(checked_in.profiles), 8)
        self.assertEqual(checked_in.tolerances.box_edge, 0.05)
        self.assertEqual(checked_in.tolerances.text_edge, 0.51)
        self.assertEqual(checked_in.tolerances.baseline, 1.0)
        for fixture in checked_in.fixtures:
            attributes, boxes = corpus.read_box_file(checked_in.reference_path(fixture))
            self.assertEqual(attributes["fixture"], fixture.name)
            self.assertEqual(attributes["chromium"],
                             json.loads((Path(fetch_chromium.PIN)).read_text())["version"])
            self.assertEqual(sorted(boxes), sorted(profile.key for profile in checked_in.profiles))

    def test_fixture_rules_are_enforced(self) -> None:
        with tempfile.TemporaryDirectory() as scratch:
            path = Path(scratch) / "f.html"
            path.write_text('<div data-gf-id="r" data-gf-kind="box">'
                            '<div data-gf-id="r" data-gf-kind="box"></div></div>', encoding="utf-8")
            with self.assertRaises(corpus.CorpusError):
                corpus.read_fixture("f", path)
            path.write_text('<div data-gf-id="r" data-gf-kind="box" '
                            'data-gf-expected-difference=" "></div>', encoding="utf-8")
            with self.assertRaises(corpus.CorpusError):
                corpus.read_fixture("f", path)
            path.write_text('<div data-gf-id="r" data-gf-kind="flow-layout-panel-compat">'
                            '<div data-gf-id="t" data-gf-kind="label">text</div></div>',
                            encoding="utf-8")
            self.assertTrue(corpus.read_fixture("f", path).is_text)

    def test_report_of_the_references_against_themselves_is_clean(self) -> None:
        checked_in = corpus.read_corpus(CORPUS)
        document, passed = compare.report(checked_in, CORPUS / "references")
        self.assertTrue(passed)
        self.assertEqual(document["totals"]["over_tolerance"], 0)
        self.assertEqual(document["totals"]["largest_edge"], 0.0)


def frame(opcode: int, payload: bytes, final: bool = True) -> bytes:
    first = (0x80 if final else 0) | opcode
    if len(payload) < 126:
        return bytes([first, len(payload)]) + payload
    return bytes([first, 126]) + struct.pack(">H", len(payload)) + payload


class WebSocketTests(unittest.TestCase):
    def connection(self) -> tuple[devtools.WebSocket, socket.socket]:
        client, server = socket.socketpair()
        self.addCleanup(client.close)
        self.addCleanup(server.close)
        websocket = devtools.WebSocket.__new__(devtools.WebSocket)
        websocket._socket = client
        return websocket, server

    def test_fragmented_text_and_ping_are_handled(self) -> None:
        websocket, server = self.connection()
        long_text = "x" * 300
        server.sendall(frame(0x1, b"hel", final=False) + frame(0x9, b"p") +
                       frame(0x0, long_text.encode(), final=True))
        self.assertEqual(websocket.receive_text(), "hel" + long_text)
        pong = server.recv(64)
        self.assertEqual(pong[0], 0x80 | 0xA)
        self.assertTrue(pong[1] & 0x80, "client frames are masked")
        server.close()

    def test_sent_frames_are_masked_and_decode(self) -> None:
        websocket, server = self.connection()
        websocket.send_text("hello")
        data = server.recv(64)
        self.assertEqual(data[0], 0x81)
        length = data[1] & 0x7F
        mask = data[2:6]
        payload = bytes(byte ^ mask[index % 4] for index, byte in enumerate(data[6:6 + length]))
        self.assertEqual(payload, b"hello")
        server.close()

    def test_close_and_masked_server_frames_are_errors(self) -> None:
        websocket, server = self.connection()
        server.sendall(bytes([0x81, 0x80 | 1]) + b"\0\0\0\0" + b"a")
        with self.assertRaises(devtools.DevToolsError):
            websocket.receive_text()
        server.close()
        websocket, server = self.connection()
        server.sendall(frame(0x8, b""))
        with self.assertRaises(devtools.DevToolsError):
            websocket.receive_text()
        server.close()

    def test_non_local_endpoints_are_refused(self) -> None:
        with self.assertRaises(devtools.DevToolsError):
            devtools.WebSocket("ws://example.com:9222/devtools/browser/x")


class FetchTests(unittest.TestCase):
    def package_for(self, data: bytes) -> dict[str, object]:
        import hashlib

        return {"bytes": len(data), "sha256": hashlib.sha256(data).hexdigest()}

    def test_size_and_hash_are_verified(self) -> None:
        with tempfile.TemporaryDirectory() as scratch:
            archive = Path(scratch) / "a.zip"
            archive.write_bytes(b"content")
            fetch_chromium.verify(archive, self.package_for(b"content"))
            with self.assertRaises(fetch_chromium.FetchError):
                fetch_chromium.verify(archive, self.package_for(b"other!!"))
            with self.assertRaises(fetch_chromium.FetchError):
                fetch_chromium.verify(archive, self.package_for(b"longer content"))

    def test_members_outside_the_directory_are_refused(self) -> None:
        with tempfile.TemporaryDirectory() as scratch:
            archive = Path(scratch) / "a.zip"
            buffer = io.BytesIO()
            with zipfile.ZipFile(buffer, "w") as bundle:
                bundle.writestr("../escape.txt", "no")
            archive.write_bytes(buffer.getvalue())
            destination = Path(scratch) / "out"
            destination.mkdir()
            with self.assertRaises(fetch_chromium.FetchError):
                fetch_chromium.unpack(archive, destination)
            self.assertFalse((Path(scratch) / "escape.txt").exists())

    def test_approval_is_required_and_the_repository_is_refused(self) -> None:
        self.assertEqual(fetch_chromium.main(["--platform", "linux64", "--unpack", "x"]), 1)
        with self.assertRaises(fetch_chromium.FetchError):
            fetch_chromium.require_empty_directory(REPOSITORY / "build-chromium")

    def test_pin_names_both_approved_packages(self) -> None:
        pin = json.loads(fetch_chromium.PIN.read_text(encoding="utf-8"))
        self.assertEqual(pin["version"], "155.0.8059.39")
        self.assertEqual(pin["packages"]["linux64"]["bytes"], 124203329)
        self.assertEqual(pin["packages"]["win64"]["bytes"], 124737213)
        for package in pin["packages"].values():
            self.assertEqual(len(package["sha256"]), 64)


if __name__ == "__main__":
    unittest.main()
