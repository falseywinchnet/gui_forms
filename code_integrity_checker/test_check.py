"""Deterministic examples of the uncertainty shapes reported by check.py."""

import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

import check


def rules(source: str) -> list[str]:
    return [finding.rule for finding in check.inspect("sample.cpp", source)]


class IntegrityShapeTests(unittest.TestCase):
    def test_borrow_survives_owner_growth(self) -> None:
        source = """void append(std::vector<int>& values) {
            const int* first = values.data();
            values.push_back(3);
            consume(first);
        }"""
        self.assertEqual(rules(source), ["I01"])

    def test_borrow_not_used_after_growth(self) -> None:
        source = """void append(std::vector<int>& values) {
            const int* first = values.data();
            consume(first);
            values.push_back(3);
        }"""
        self.assertEqual(rules(source), [])

    def test_borrow_rebound_after_growth(self) -> None:
        source = """void append(std::vector<int>& values) {
            const int* first = values.data();
            values.push_back(3);
            first = values.data();
            consume(first);
        }"""
        self.assertEqual(rules(source), [])

    def test_owner_reset_after_get_borrow(self) -> None:
        source = "void dispose(std::unique_ptr<T>& owner) { T* item = owner.get(); owner.reset(); consume(item); }"
        self.assertEqual(rules(source), ["I01"])

    def test_member_observer_does_not_rebind_the_owner(self) -> None:
        source = "void stop() { observer.run = run.get(); run.reset(); return; }"
        self.assertEqual(rules(source), [])

    def test_publish_through_helper_before_placement_new(self) -> None:
        source = """struct Items {
            int count;
            void* pushItem() { count++; return slot(); }
            T& add() { return *new (this->pushItem()) T; }
        };"""
        self.assertEqual(rules(source), ["I02"])

    def test_completed_construction_precedes_publish(self) -> None:
        source = """void add() {
            construct_at(slot, value);
            count++;
        }"""
        self.assertEqual(rules(source), [])

    def test_assert_only_bound(self) -> None:
        source = """int get(int index) {
            assert(index < count);
            return values[index];
        }"""
        self.assertEqual(rules(source), ["I03"])

    def test_release_guard_after_assert(self) -> None:
        source = """int get(int index) {
            assert(index < count);
            if (index >= count) return 0;
            return values[index];
        }"""
        self.assertEqual(rules(source), [])

    def test_empty_pointer_end_and_guard(self) -> None:
        bad = "void scan(Buffer& values) { consume(values.data() + values.size()); }"
        good = "void scan(Buffer& values) { if (values.empty()) return; consume(values.data() + values.size()); }"
        self.assertEqual(rules(bad), ["I04"])
        self.assertEqual(rules(good), [])

    def test_fixed_array_endpoint_has_storage_even_when_empty(self) -> None:
        source = "void write() { std::array<char, 32> buffer{}; consume(buffer.data() + buffer.size()); }"
        self.assertEqual(rules(source), [])

    def test_zero_extent_array_may_have_null_data(self) -> None:
        source = "void write() { std::array<char, 0> buffer{}; consume(buffer.data() + buffer.size()); }"
        self.assertEqual(rules(source), ["I04"])

    def test_string_endpoint_has_storage_even_when_empty(self) -> None:
        source = "void write(const std::string& text) { consume(text.data() + text.size()); }"
        self.assertEqual(rules(source), [])

    def test_narrowing_and_checked_narrowing(self) -> None:
        bad = "int length(Buffer& values) { return static_cast<int>(values.size()); }"
        good = "int length(Buffer& values) { if (values.size() > INT_MAX) return -1; return static_cast<int>(values.size()); }"
        self.assertEqual(rules(bad), ["I05"])
        self.assertEqual(rules(good), [])

    def test_extent_product_and_checked_product(self) -> None:
        bad = "void make(size_t width, size_t height) { storage.resize(width * height); }"
        good = "void make(size_t width, size_t height) { if (height != 0 && width > SIZE_MAX / height) return; storage.resize(width * height); }"
        self.assertEqual(rules(bad), ["I06"])
        self.assertEqual(rules(good), [])

    def test_named_product_reaches_storage(self) -> None:
        source = "void make(size_t width, size_t height) { size_t bytes = width * height; storage.resize(bytes); }"
        self.assertEqual(rules(source), ["I06"])

    def test_poison_boundary(self) -> None:
        bad = "void shrink() { poisonRange(nextCursor + 1, end); }"
        good = "void shrink() { poisonRange(nextCursor, end); }"
        self.assertEqual(rules(bad), ["I07"])
        self.assertEqual(rules(good), [])

    def test_comments_and_literals_are_not_code(self) -> None:
        source = 'void note() { // poisonRange(nextCursor + 1, end);\n const char* message = "values.data() + values.size()"; }'
        self.assertEqual(rules(source), [])


class CommandTests(unittest.TestCase):
    def test_json_report_and_gate_status(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "example.cpp"
            source.write_text("void shrink() { poisonRange(nextCursor + 1, end); }\n", encoding="utf-8")
            report = root / "new" / "report.json"
            command = [sys.executable, "-B", str(Path(check.__file__)), str(source), "--json", str(report), "--gate"]
            completed = subprocess.run(command, capture_output=True, text=True, check=False)
            self.assertEqual(completed.returncode, 1)
            data = json.loads(report.read_text(encoding="utf-8"))
            self.assertEqual(data[0]["rule"], "I07")
            self.assertEqual(data[0]["house_style"], "§5")
            self.assertEqual(data[0]["house_style_status"], "invariant_not_evident_in_source")

    def test_empty_selection_is_not_a_pass(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            command = [sys.executable, "-B", str(Path(check.__file__)), directory]
            completed = subprocess.run(command, capture_output=True, text=True, check=False)
            self.assertNotEqual(completed.returncode, 0)
            self.assertIn("No C or C++ files selected", completed.stderr)


if __name__ == "__main__":
    unittest.main()
