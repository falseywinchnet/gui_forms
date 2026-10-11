"""One numeric version per workflow run, from the project() baseline."""
from __future__ import annotations

import unittest

import publication_version as pv


class ValidateTests(unittest.TestCase):
    def test_canonical_versions_parse(self) -> None:
        self.assertEqual(pv.validate("0.1.0"), (0, 1, 0))
        self.assertEqual(pv.validate("12.0.65535"), (12, 0, 65535))

    def test_non_canonical_versions_are_rejected(self) -> None:
        for bad in ("1.0", "01.0.0", "1.0.0.0", "v1.0.0", "1.0.0-rc1", ""):
            with self.assertRaises(ValueError, msg=bad):
                pv.validate(bad)

    def test_components_fit_installer_fields(self) -> None:
        with self.assertRaisesRegex(ValueError, "component range"):
            pv.validate("0.1.65536")


class BaselineTests(unittest.TestCase):
    def test_project_version_is_read(self) -> None:
        self.assertEqual(pv.baseline_from_text("cmake_minimum_required(VERSION 3.25)\nproject(GUIForms VERSION 0.1.0 LANGUAGES C CXX)\n"), "0.1.0")

    def test_missing_version_is_an_error(self) -> None:
        with self.assertRaisesRegex(ValueError, "baseline"):
            pv.baseline_from_text("project(GUIForms LANGUAGES CXX)\n")

    def test_repository_baseline_is_canonical(self) -> None:
        pv.validate(pv.baseline())


class SelectTests(unittest.TestCase):
    def test_pull_request_builds_the_baseline(self) -> None:
        self.assertEqual(pv.select("0.1.0", "pull_request", "refs/pull/7/merge", 42),
                         {"version": "0.1.0", "tag": "v0.1.0", "publish": "false"})

    def test_main_push_publishes_patch_plus_run_number(self) -> None:
        self.assertEqual(pv.select("0.1.0", "push", pv.MAIN_REF, 42),
                         {"version": "0.1.42", "tag": "v0.1.42", "publish": "true"})
        self.assertEqual(pv.select("0.1.3", "push", pv.MAIN_REF, 2)["version"], "0.1.5")

    def test_main_push_needs_a_run_number(self) -> None:
        with self.assertRaisesRegex(ValueError, "run number"):
            pv.select("0.1.0", "push", pv.MAIN_REF, 0)

    def test_release_tag_in_series_publishes_that_version(self) -> None:
        self.assertEqual(pv.select("0.1.0", "push", "refs/tags/v0.1.9", 5),
                         {"version": "0.1.9", "tag": "v0.1.9", "publish": "true"})

    def test_release_tag_outside_series_or_predating_is_rejected(self) -> None:
        with self.assertRaisesRegex(ValueError, "series"):
            pv.select("0.1.0", "push", "refs/tags/v0.2.0", 5)
        with self.assertRaisesRegex(ValueError, "series"):
            pv.select("0.1.4", "push", "refs/tags/v0.1.3", 5)
        with self.assertRaisesRegex(ValueError, "v-prefixed"):
            pv.select("0.1.0", "push", "refs/tags/release-1", 5)

    def test_dispatch_is_dry_unless_requested_on_main(self) -> None:
        self.assertEqual(pv.select("0.1.0", "workflow_dispatch", pv.MAIN_REF, 9)["publish"], "false")
        self.assertEqual(pv.select("0.1.0", "workflow_dispatch", pv.MAIN_REF, 9, requested=True),
                         {"version": "0.1.9", "tag": "v0.1.9", "publish": "true"})
        with self.assertRaisesRegex(ValueError, "restricted to main"):
            pv.select("0.1.0", "workflow_dispatch", "refs/heads/feature", 9, requested=True)

    def test_other_branches_build_the_baseline(self) -> None:
        self.assertEqual(pv.select("0.1.0", "push", "refs/heads/feature", 9)["publish"], "false")


if __name__ == "__main__":
    unittest.main()
