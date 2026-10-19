"""Unit tests for the option resolution in ci_build.py (no build is run)."""
from __future__ import annotations
from pathlib import Path
import unittest

import ci_build


class GalleryOptionTests(unittest.TestCase):
    def test_default_is_off(self) -> None:
        self.assertFalse(ci_build.gallery_enabled(False, {}))

    def test_flag_turns_gallery_on(self) -> None:
        self.assertTrue(ci_build.gallery_enabled(True, {}))

    def test_environment_turns_gallery_on(self) -> None:
        value: str
        for value in ('1', 'true', 'ON', ' yes '):
            self.assertTrue(ci_build.gallery_enabled(False, {'GUI_FORMS_CI_BUILD_GALLERY': value}), value)

    def test_environment_other_values_keep_default(self) -> None:
        value: str
        for value in ('', '0', 'false', 'off', 'maybe'):
            self.assertFalse(ci_build.gallery_enabled(False, {'GUI_FORMS_CI_BUILD_GALLERY': value}), value)


class SanitizerOptionTests(unittest.TestCase):
    def test_every_finding_stops_its_test(self) -> None:
        options: list[str]
        environment: dict[str, str]
        options, environment = ci_build.sanitizer_options('linux', Path('lsan.supp'))
        self.assertIn('-DCMAKE_CXX_FLAGS=' + ci_build.SANITIZER_FLAGS, options)
        self.assertIn('-fno-sanitize-recover=undefined', ci_build.SANITIZER_FLAGS)
        self.assertIn('detect_leaks=1', environment['ASAN_OPTIONS'])
        self.assertIn('halt_on_error=1', environment['UBSAN_OPTIONS'])
        self.assertIn('suppressions=lsan.supp', environment['LSAN_OPTIONS'])

    def test_leaks_are_detected_where_the_runtime_has_a_leak_sanitizer(self) -> None:
        options: list[str]
        environment: dict[str, str]
        host: str
        for host in ('linux', 'macos'):
            options, environment = ci_build.sanitizer_options(host, Path('lsan.supp'))
            self.assertIn('detect_leaks=1', environment['ASAN_OPTIONS'], host)
            self.assertIn('LSAN_OPTIONS', environment, host)
        options, environment = ci_build.sanitizer_options('windows', Path('lsan.supp'))
        self.assertIn('-DCMAKE_CXX_FLAGS=' + ci_build.SANITIZER_FLAGS, options)
        self.assertIn('detect_leaks=0', environment['ASAN_OPTIONS'])
        self.assertNotIn('LSAN_OPTIONS', environment)
        self.assertIn('halt_on_error=1', environment['UBSAN_OPTIONS'])

    def test_macos_flags_join_the_toolchain_flags(self) -> None:
        options: list[str]
        environment: dict[str, str]
        options, environment = ci_build.sanitizer_options('macos', Path('lsan.supp'))
        self.assertEqual(options, ['-DCMAKE_BUILD_TYPE=RelWithDebInfo'])
        name: str
        for name in ('CFLAGS', 'CXXFLAGS', 'OBJCXXFLAGS'):
            self.assertEqual(environment[name], ci_build.SANITIZER_FLAGS, name)
        self.assertEqual(environment['LDFLAGS'], ci_build.SANITIZER_LINK_FLAGS)

    def test_linux_and_windows_keep_command_line_flags(self) -> None:
        options: list[str]
        environment: dict[str, str]
        host: str
        for host in ('linux', 'windows'):
            options, environment = ci_build.sanitizer_options(host, Path('lsan.supp'))
            self.assertIn('-DCMAKE_C_FLAGS=' + ci_build.SANITIZER_FLAGS, options, host)
            self.assertIn('-DCMAKE_EXE_LINKER_FLAGS=' + ci_build.SANITIZER_LINK_FLAGS, options, host)
            self.assertIn('-DCMAKE_SHARED_LINKER_FLAGS=' + ci_build.SANITIZER_LINK_FLAGS, options, host)
            self.assertNotIn('CXXFLAGS', environment, host)
            self.assertNotIn('LDFLAGS', environment, host)


if __name__ == '__main__':
    unittest.main()
