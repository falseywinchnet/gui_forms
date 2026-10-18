"""Control schema tests: real-manifest check, stale-page detection, event scan and spelling."""

import tempfile
import unittest
from pathlib import Path

import control_registry as cr
import control_schema as cs


class RealTreeTests(unittest.TestCase):
    def test_committed_manifest_matches_registry_and_headers(self) -> None:
        schema = cs.load_schema()
        findings = cs.validate(schema, cr.load_registry(), cs.scan_classes())
        self.assertEqual(findings, [])

    def test_committed_page_is_current(self) -> None:
        page = cs.render_page(cs.load_schema())
        committed = cs.PAGE_PATH.read_text(encoding="utf-8").replace("\r\n", "\n")
        self.assertEqual(committed, page, "run `python3 tools/control_schema.py render`")


class SpellingTests(unittest.TestCase):
    def test_payload_drops_const_and_reference_marks(self) -> None:
        self.assertEqual(cs.interaction_from_accessor("text_changed", "const std::string&"),
                         cs.Interaction("text_changed", "std::string", False))
        self.assertEqual(cs.interaction_from_accessor("validating", "ControlValidationEvent&"),
                         cs.Interaction("validating", "ControlValidationEvent", True))
        self.assertEqual(cs.interaction_from_accessor("item_check_state_changed", "std::size_t, CheckState"),
                         cs.Interaction("item_check_state_changed", "std::size_t, CheckState", False))
        self.assertEqual(cs.interaction_from_accessor("tick", ""), cs.Interaction("tick", "", False))


class ScanTests(unittest.TestCase):
    def test_public_events_resolve_through_bases(self) -> None:
        header = """
#pragma once
namespace gui_forms {
class Widget : public Control {
public:
    [[nodiscard]] Event<std::optional<bool>>& opened() noexcept { return opened_; }
    struct Nested final {
        Event<int>& ignored() noexcept;
    };
private:
    [[nodiscard]] Event<>& hidden() noexcept { return hidden_; }
};
class FancyWidget final : public Widget {
public:
    [[nodiscard]] Event<const FancyWidget&>& changed() noexcept { return changed_; }
};
}
"""
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            target = root / "gui_forms" / "widget.hpp"
            target.parent.mkdir(parents=True)
            target.write_text(header, encoding="utf-8")
            classes = cs.scan_classes(root)
        expected = cs.expected_interactions("FancyWidget", classes)
        self.assertEqual([item.name for item in expected], ["opened", "changed"])
        self.assertEqual(expected[0].payload, "std::optional<bool>")

    def test_a_missing_interaction_is_reported(self) -> None:
        schema = cs.load_schema()
        types = [dict(record) for record in schema["types"]]
        types[0]["interactions"] = types[0]["interactions"][:-1]
        findings = cs.validate({"types": types, "vocabulary": schema["vocabulary"]},
                               cr.load_registry(), cs.scan_classes())
        self.assertEqual(len(findings), 1)
        self.assertIn(types[0]["id"], findings[0])

    def test_the_vocabulary_is_required_and_lists_the_ribbon_atoms(self) -> None:
        schema = cs.load_schema()
        vocabulary = schema["vocabulary"]
        for mark in ("dialog_launcher", "more", "overflow", "customize"):
            self.assertIn(mark, vocabulary["marks"])
        self.assertIn("accent_index", vocabulary["values"])
        for family in ("card", "menu_item", "tool_tip"):
            self.assertIn(family, vocabulary["families"])
        self.assertIn("split_button", vocabulary["semantic_roles"])
        self.assertIn("menu_button", vocabulary["semantic_roles"])
        findings = cs.validate({"types": schema["types"]}, cr.load_registry(), cs.scan_classes())
        self.assertEqual(len(findings), 1)
        self.assertIn("vocabulary", findings[0])

    def test_the_vocabulary_lists_the_docking_atoms(self) -> None:
        schema = cs.load_schema()
        vocabulary = schema["vocabulary"]
        for mark in ("pin", "dock_shape"):
            self.assertIn(mark, vocabulary["marks"])
        for state in ("modified", "pinned"):
            self.assertIn(state, vocabulary["custom_states"])
            self.assertNotIn(state, vocabulary["states"])
        for value in ("text_orientation", "dock_region"):
            self.assertIn(value, vocabulary["values"])
        for role in ("pane", "tab_panel"):
            self.assertIn(role, vocabulary["semantic_roles"])
        self.assertIn("close", vocabulary["semantic_actions"])
        for field in ("controlled_panel", "orientation"):
            self.assertIn(field, vocabulary["semantic_fields"])
        shortened = dict(vocabulary)
        del shortened["semantic_actions"]
        findings = cs.validate({"types": schema["types"], "vocabulary": shortened},
                               cr.load_registry(), cs.scan_classes())
        self.assertEqual(len(findings), 1)
        self.assertIn("semantic_actions", findings[0])


if __name__ == "__main__":
    unittest.main()
