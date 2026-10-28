"""Deterministic examples of the uncertainty shapes reported by check.py."""

import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

import check


def rules(source: str) -> list[str]:
    """The rules of the candidates raised: those no recognized guard protects."""
    return [finding.rule for finding in check.inspect("sample.cpp", source) if not finding.guard]


def guards(source: str) -> list[str]:
    """The guard of every candidate, raised or not."""
    return [finding.guard for finding in check.inspect("sample.cpp", source)]


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


class GuardTests(unittest.TestCase):
    def test_dominating_validation_is_listed_apart(self) -> None:
        source = "int length(Buffer& values) { if (values.size() > INT_MAX) return -1; return static_cast<int>(values.size()); }"
        self.assertEqual(guards(source), ["dominating_validation"])

    def test_division_form_bound_guards_a_product(self) -> None:
        source = """bool grow(size_t wanted, size_t across) {
            if (across != 0 && wanted > maximum_cells / across) return false;
            cells.resize(wanted * across);
            return true;
        }"""
        self.assertEqual(guards(source), ["dominating_validation"])

    def test_an_emptiness_test_is_not_a_bound(self) -> None:
        source = "int length(Buffer& values) { if (values.empty()) return 0; return static_cast<int>(values.size()); }"
        self.assertEqual(rules(source), ["I05"])

    def test_a_validation_that_does_not_exit_is_not_a_guard(self) -> None:
        source = "void make(size_t width, size_t height) { if (width > 4096) log(width); storage.resize(width * height); }"
        self.assertEqual(rules(source), ["I06"])

    def test_checked_arithmetic_helper(self) -> None:
        source = """void make(size_t width, size_t height) {
            size_t bytes = 0;
            if (__builtin_mul_overflow(width, height, &bytes)) return;
            storage.resize(width * height);
        }"""
        self.assertEqual(guards(source), ["checked_arithmetic"])

    def test_named_failure_admission(self) -> None:
        source = "void shape(std::string_view line) { work.admit(line.size()); add(line.data(), static_cast<int>(line.size())); }"
        self.assertEqual(guards(source), ["named_failure"])

    def test_empty_exit_guards_an_endpoint(self) -> None:
        source = "void scan(Buffer& values) { if (values.empty()) return; consume(values.data() + values.size()); }"
        self.assertEqual(guards(source), ["dominating_validation"])

    def test_literal_extents_fit(self) -> None:
        array = "void read() { std::array<float, 8192> block{}; fill(block.data(), static_cast<int>(block.size())); }"
        vector = "void read() { std::vector<char> bytes(128); fill(bytes.data(), static_cast<int>(bytes.size())); }"
        self.assertEqual(guards(array), [])
        self.assertEqual(guards(vector), [])

    def test_standard_view_endpoint_is_valid_when_empty(self) -> None:
        source = "bool parse(std::string_view text) { const char* end = text.data() + text.size(); return end != nullptr; }"
        self.assertEqual(guards(source), [])


class ReviewTests(unittest.TestCase):
    def run_checker(self, source: str, reviews: dict[str, object] | None) -> tuple[int, list[dict[str, object]]]:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            path = root / "example.cpp"
            path.write_text(source, encoding="utf-8")
            review_path = root / "reviews.json"
            if reviews is not None:
                review_path.write_text(json.dumps(reviews), encoding="utf-8")
            report = root / "report.json"
            # Relative to the run directory, so a recorded path matches across runs.
            command = [sys.executable, "-B", str(Path(check.__file__).resolve()), path.name, "--json", str(report),
                       "--reviews", str(review_path), "--gate"]
            completed = subprocess.run(command, capture_output=True, text=True, check=False, cwd=root)
            data = json.loads(report.read_text(encoding="utf-8")) if report.exists() else []
            return completed.returncode, data

    def test_reviewed_candidate_passes_the_gate(self) -> None:
        source = "void make(size_t width, size_t height) {\n    storage.resize(width * height);\n}\n"
        status, data = self.run_checker(source, None)
        self.assertEqual(status, 1)
        reviews = {"reviews": [{"path": data[0]["path"], "rule": "I06", "evidence": data[0]["evidence"],
                                "decision": "guarded", "guard": "dominating_validation",
                                "reason": "callers bound both sides"}]}
        status, data = self.run_checker(source, reviews)
        self.assertEqual(status, 0)
        self.assertEqual(data[0]["status"], "reviewed")
        self.assertEqual(data[0]["decision"], "guarded")
        self.assertEqual(data[0]["reason"], "callers bound both sides")

    def test_edited_line_is_raised_again(self) -> None:
        source = "void make(size_t width, size_t height) {\n    storage.resize(width * height);\n}\n"
        status, data = self.run_checker(source, None)
        reviews = {"reviews": [{"path": data[0]["path"], "rule": "I06", "evidence": data[0]["evidence"],
                                "decision": "false_shape"}]}
        edited = "void make(size_t width, size_t depth) {\n    storage.resize(width * depth);\n}\n"
        status, data = self.run_checker(edited, reviews)
        self.assertEqual(status, 1)
        self.assertEqual(data[0]["status"], "raised")

    def test_excluded_scope_is_not_read(self) -> None:
        source = "void make(size_t width, size_t height) {\n    storage.resize(width * height);\n}\n"
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "skia").mkdir()
            (root / "skia" / "raster.cpp").write_text(source, encoding="utf-8")
            (root / "core").mkdir()
            (root / "core" / "plain.cpp").write_text("void plain() {}\n", encoding="utf-8")
            review_path = root / "reviews.json"
            review_path.write_text(json.dumps({"excluded_scopes": ["skia/"]}), encoding="utf-8")
            command = [sys.executable, "-B", str(Path(check.__file__).resolve()), "skia", "core",
                       "--reviews", str(review_path), "--gate"]
            completed = subprocess.run(command, capture_output=True, text=True, check=False, cwd=root)
            self.assertEqual(completed.returncode, 0)
            self.assertIn("1 files (1 excluded), 0 candidates", completed.stdout)

    def test_only_excluded_files_is_an_empty_selection(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "skia").mkdir()
            (root / "skia" / "raster.cpp").write_text("void raster() {}\n", encoding="utf-8")
            review_path = root / "reviews.json"
            review_path.write_text(json.dumps({"excluded_scopes": ["skia/"]}), encoding="utf-8")
            command = [sys.executable, "-B", str(Path(check.__file__).resolve()), "skia", "--reviews", str(review_path)]
            completed = subprocess.run(command, capture_output=True, text=True, check=False, cwd=root)
            self.assertNotEqual(completed.returncode, 0)
            self.assertIn("No C or C++ files selected", completed.stderr)

    def test_decision_whose_line_is_gone_is_stale(self) -> None:
        source = "void make(size_t width, size_t height) {\n    storage.resize(width * height);\n}\n"
        status, data = self.run_checker(source, None)
        reviews = {"reviews": [{"path": data[0]["path"], "rule": "I06", "evidence": data[0]["evidence"],
                                "decision": "defect", "reason": "unbounded product"}]}
        status, data = self.run_checker("void make() {}\n", reviews)
        self.assertEqual(status, 1)
        self.assertEqual(data, [])

    def test_decision_for_a_removed_file_is_stale_and_one_out_of_scope_is_not(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "core").mkdir()
            (root / "core" / "plain.cpp").write_text("void plain() {}\n", encoding="utf-8")
            review_path = root / "reviews.json"
            gone = {"path": "core/removed.cpp", "rule": "I06", "evidence": "x", "decision": "defect"}
            elsewhere = {"path": "other/kept.cpp", "rule": "I06", "evidence": "x", "decision": "defect"}
            review_path.write_text(json.dumps({"reviews": [gone, elsewhere]}), encoding="utf-8")
            command = [sys.executable, "-B", str(Path(check.__file__).resolve()), "core",
                       "--reviews", str(review_path), "--gate"]
            completed = subprocess.run(command, capture_output=True, text=True, check=False, cwd=root)
            self.assertEqual(completed.returncode, 1)
            self.assertIn("core/removed.cpp: stale defect decision", completed.stdout)
            self.assertNotIn("other/kept.cpp", completed.stdout)
            self.assertIn("1 stale decisions", completed.stdout)

    def test_fixed_is_not_a_decision(self) -> None:
        status, _ = self.run_checker("void make() {}\n", {"reviews": [{"path": "x.cpp", "rule": "I06",
                                                                      "evidence": "x", "decision": "fixed"}]})
        self.assertEqual(status, 2)

    def test_unknown_decision_is_an_error(self) -> None:
        status, _ = self.run_checker("void make() {}\n", {"reviews": [{"path": "x.cpp", "rule": "I06",
                                                                      "evidence": "x", "decision": "accepted"}]})
        self.assertEqual(status, 2)


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
            self.assertEqual(data[0]["house_style"], "§1")
            self.assertEqual(data[0]["status"], "raised")
            self.assertEqual(data[0]["house_style_status"], "invariant_not_evident_in_source")

    def test_empty_selection_is_not_a_pass(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            command = [sys.executable, "-B", str(Path(check.__file__)), directory]
            completed = subprocess.run(command, capture_output=True, text=True, check=False)
            self.assertNotEqual(completed.returncode, 0)
            self.assertIn("No C or C++ files selected", completed.stderr)


if __name__ == "__main__":
    unittest.main()
