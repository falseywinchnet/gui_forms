"""Unit tests for the option resolution in ci_build.py (no build is run)."""
from __future__ import annotations
from pathlib import Path
import re
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
        # Without it ccache hashes the build directory into every -g object.
        self.assertIn('-fdebug-compilation-dir=.', ci_build.SANITIZER_FLAGS)
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


CHECKOUT: Path = Path(__file__).resolve().parents[1]
FIRST_PARTY_DIRECTORIES: tuple[str, ...] = ('include', 'src', 'demo', 'tests', 'examples',
                                            'file_manager_demoboard', 'stx_vorbis')
SOURCE_SUFFIXES: tuple[str, ...] = ('.c', '.cc', '.cpp', '.h', '.hpp', '.inc', '.m', '.mm')


def is_module_rule(pattern: str) -> bool:
    """Whether a suppression names a library (a path or a library file) rather than a function."""
    return (pattern.startswith('/') or '.so' in pattern or pattern.endswith('.dylib')
            or '.framework' in pattern)


def function_rules(suppressions: str) -> list[str]:
    """The function-name patterns among a LeakSanitizer suppression file's leak: rules."""
    rules: list[str] = []
    line: str
    for line in suppressions.splitlines():
        stripped: str = line.strip()
        if not stripped.startswith('leak:'):
            continue
        pattern: str = stripped[len('leak:'):]
        if not is_module_rule(pattern):
            rules.append(pattern)
    return rules


def rule_expression(pattern: str) -> re.Pattern[str]:
    """LeakSanitizer matches a pattern anywhere in a frame's name, with * as a wildcard."""
    pieces: list[str] = []
    piece: str
    for piece in pattern.split('*'):
        pieces.append(re.escape(piece))
    return re.compile('.*'.join(pieces))


class LeakSuppressionTests(unittest.TestCase):
    def test_no_function_rule_can_match_a_first_party_frame(self) -> None:
        # A rule suppresses a leak when it matches any frame of the allocation
        # stack, so a system function pattern that also occurs in first-party
        # source would hide our leaks there (house style section 5: never
        # suppress a GUI.Forms leak). Comments count too: the check is textual.
        suppressions: str = (CHECKOUT / 'tools/sanitizer/lsan.supp').read_text(encoding='utf-8')
        rules: list[str] = function_rules(suppressions)
        self.assertTrue(rules)
        expressions: list[tuple[str, re.Pattern[str]]] = []
        rule: str
        for rule in rules:
            expressions.append((rule, rule_expression(rule)))
        collisions: list[str] = []
        directory: str
        for directory in FIRST_PARTY_DIRECTORIES:
            source: Path
            for source in sorted((CHECKOUT / directory).rglob('*')):
                if source.suffix not in SOURCE_SUFFIXES or not source.is_file():
                    continue
                text: str = source.read_text(encoding='utf-8', errors='replace')
                expression: tuple[str, re.Pattern[str]]
                for expression in expressions:
                    if expression[1].search(text) is not None:
                        collisions.append(f'{source.relative_to(CHECKOUT)}: {expression[0]}')
        self.assertEqual(collisions, [])

    def test_module_and_function_rules_are_told_apart(self) -> None:
        lines: list[str] = ['# note', 'leak:libX11.so', 'leak:/ColorSync.framework/',
                            'leak:/libxpc.dylib', 'leak:fetch_cache', 'leak:-[NSThread init]']
        rules: list[str] = function_rules('\n'.join(lines))
        self.assertEqual(rules, ['fetch_cache', '-[NSThread init]'])
        self.assertIsNotNone(rule_expression('CA::Transaction::create').search('x CA::Transaction::create y'))
        self.assertIsNotNone(rule_expression('a*b').search('xa1by'))


if __name__ == '__main__':
    unittest.main()
