#!/usr/bin/env python3
"""Tests for tools/generate_library_docs.py, the API reference generator."""
from __future__ import annotations

import re
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import generate_library_docs as generator  # noqa: E402

ROOT: Path = Path(__file__).resolve().parents[1]


class ReferenceTests(unittest.TestCase):
    pages: dict[str, str] = {}
    scratch: tempfile.TemporaryDirectory

    @classmethod
    def setUpClass(cls) -> None:
        cls.scratch = tempfile.TemporaryDirectory(prefix="gui-forms-api-test-")
        output = Path(cls.scratch.name) / "api"
        generator.render(output)
        cls.pages = {path.name: path.read_text(encoding="utf-8")
                     for path in sorted(output.glob("*.md"))}

    @classmethod
    def tearDownClass(cls) -> None:
        cls.scratch.cleanup()

    def test_page_count_stays_small(self) -> None:
        # Furo renders the whole navigation tree into every page; the site
        # must not return to one page per declaration.
        self.assertIn("index.md", self.pages)
        self.assertLess(len(self.pages), 40)

    def test_every_include_is_a_public_header(self) -> None:
        for name, text in self.pages.items():
            for header in re.findall(r"#include <([^>]+)>", text):
                self.assertTrue((ROOT / "include" / header).is_file(), f"{name}: {header}")
                self.assertNotIn("/detail/", header, name)

    def test_no_description_is_inferred_from_a_signature(self) -> None:
        for name, text in self.pages.items():
            self.assertNotIn("against retained state", text, name)
            self.assertNotIn("declaration-derived", text, name)
            self.assertNotIn("OBSERVED", text, name)

    def test_registry_controls_land_on_their_category_page(self) -> None:
        self.assertIn("(api-gui-forms-button)=\n## Button\n", self.pages["commands-controls.md"])
        self.assertIn("## TreeView\n", self.pages["selection-from-a-list-controls.md"])
        self.assertIn("## ColorValueEditor\n", self.pages["value-setting-controls.md"])

    def test_owned_handlers_are_documented_and_subscribe_is_marked_deprecated(self) -> None:
        events = self.pages["events-values-and-commands.md"]
        self.assertIn("void on(Event<Arguments...>& event, Owner& owner", events)
        rows = [line for line in events.splitlines()
                if line.startswith("| `SubscriptionToken subscribe(")]
        self.assertEqual(len(rows), 4)
        for row in rows:
            self.assertIn("Deprecated.", row)

    def test_signatures_keep_default_arguments_and_drop_initializer_lists(self) -> None:
        commands = self.pages["commands-controls.md"]
        self.assertIn("`explicit Button(StableId stable_id, std::string text = {})`", commands)
        events = self.pages["events-values-and-commands.md"]
        self.assertNotIn("state_(std::make_shared", events)

    def test_struct_fields_are_listed(self) -> None:
        hosts = self.pages["application-and-hosts.md"]
        self.assertIn('std::string title{"GUI.Forms Application"};', hosts)

    def test_committed_reference_is_current(self) -> None:
        self.assertEqual(generator.check(), 0)


if __name__ == "__main__":
    unittest.main()
