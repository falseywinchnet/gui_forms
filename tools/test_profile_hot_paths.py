#!/usr/bin/env python3
"""Tests for profile_hot_paths.py: reading perf report listings and naming areas.

The listings are in the shape `run` asks perf for (`perf report --stdio -t TAB
--sort dso,symbol`): comment lines, then one row per function with its
percentages, object and symbol. The inclusive listing (--children) also
carries call-tree lines, which have no percentage column of their own.
"""
from __future__ import annotations
import unittest

import profile_hot_paths
from profile_hot_paths import HotFunction

SELF_LISTING: str = '\n'.join([
    '# To display the perf.data header info, please use --header/--header-only options.',
    '#',
    '# Samples: 41K of event \'cpu-clock:ppp\'',
    '# Overhead\tShared Object\tSymbol',
    '#',
    '    21.50%\tgui_forms_paint_profile_workload\t[.] portable::lowp::blit_row_s32a_opaque',
    '    12.25%\tgui_forms_paint_profile_workload\t[.] SkRasterPipeline::run',
    '     8.00%\tgui_forms_paint_profile_workload\t[.] gui_forms::Window::paint(gui_forms::Painter&, gui_forms::Rect)',
    '     4.10%\tgui_forms_paint_profile_workload\t[.] hb_buffer_t::sort',
    '     3.00%\tgui_forms_paint_profile_workload\t[.] FT_Load_Glyph',
    '     2.00%\tlibc.so.6\t[.] __memmove_avx_unaligned_erms',
    '     1.50%\t[kernel.kallsyms]\t[k] clear_page_erms',
    '     0.75%\tgui_forms_paint_profile_workload\t[.] std::vector<int, std::allocator<int> >::push_back',
    '     0.50%\tgui_forms_paint_profile_workload\t[.] std::vector<gui_forms::detail::DisplayCommand>::_M_realloc_append',
    '     0.40%\tgui_forms_paint_profile_workload\t[.] (anonymous namespace)::measure(gui_forms::Window&)',
    '     0.30%\tgui_forms_paint_profile_workload\t[.] aaa_fill_path(SkPathRaw const&)',
    '',
])

CHILDREN_LISTING: str = '\n'.join([
    '# Children\tSelf\tShared Object\tSymbol',
    '    64.00%\t8.00%\tgui_forms_paint_profile_workload\t[.] gui_forms::Window::paint(gui_forms::Painter&, gui_forms::Rect)',
    '            |',
    '            ---gui_forms::Window::paint(gui_forms::Painter&, gui_forms::Rect)',
    '                     --52.00%--SkRasterPipeline::run',
    '    33.75%\t12.25%\tgui_forms_paint_profile_workload\t[.] SkRasterPipeline::run',
    '',
])


class ParseTests(unittest.TestCase):
    def test_rows_keep_perf_order_and_strip_the_privilege_mark(self) -> None:
        rows: list[HotFunction] = profile_hot_paths.parse_report(SELF_LISTING)
        self.assertEqual(len(rows), 11)
        self.assertEqual(rows[0].symbol, 'portable::lowp::blit_row_s32a_opaque')
        self.assertEqual(rows[0].self_percent, 21.5)
        self.assertEqual(rows[0].inclusive_percent, 21.5)
        self.assertEqual(rows[6].dso, '[kernel.kallsyms]')
        self.assertEqual(rows[6].symbol, 'clear_page_erms')

    def test_children_rows_read_inclusive_then_self(self) -> None:
        rows: list[HotFunction] = profile_hot_paths.parse_report(CHILDREN_LISTING)
        self.assertEqual(len(rows), 2)
        self.assertEqual(rows[0].inclusive_percent, 64.0)
        self.assertEqual(rows[0].self_percent, 8.0)

    def test_merge_gives_self_rows_their_inclusive_share(self) -> None:
        merged: list[HotFunction] = profile_hot_paths.merge_inclusive(
            profile_hot_paths.parse_report(SELF_LISTING), profile_hot_paths.parse_report(CHILDREN_LISTING))
        self.assertEqual(merged[2].symbol, 'gui_forms::Window::paint(gui_forms::Painter&, gui_forms::Rect)')
        self.assertEqual(merged[2].self_percent, 8.0)
        self.assertEqual(merged[2].inclusive_percent, 64.0)
        # A function the inclusive listing left out keeps its self share.
        self.assertEqual(merged[3].inclusive_percent, merged[3].self_percent)

    def test_symbol_padding_and_later_columns_are_dropped(self) -> None:
        # perf 6.17 pads the symbol column, and an IPC column follows it.
        line: str = ' 30.83% \tgui_forms_paint_profile_workload\t[.] sse2::rect_memset32(unsigned int*)      \t-      -'
        rows: list[HotFunction] = profile_hot_paths.parse_report(line)
        self.assertEqual(rows[0].symbol, 'sse2::rect_memset32(unsigned int*)')
        self.assertEqual(rows[0].self_percent, 30.83)

    def test_a_listing_without_rows_is_empty(self) -> None:
        self.assertEqual(profile_hot_paths.parse_report('# no samples\n\n'), [])


class AreaTests(unittest.TestCase):
    def test_areas_by_symbol_then_object(self) -> None:
        rows: list[HotFunction] = profile_hot_paths.parse_report(SELF_LISTING)
        areas: list[str] = []
        row: HotFunction
        for row in rows:
            areas.append(row.area)
        self.assertEqual(areas, ['skia', 'skia', 'gui_forms', 'harfbuzz', 'freetype', 'runtime', 'kernel', 'other',
                                 'gui_forms', 'workload', 'skia'])

    def test_area_shares_sum_self_shares_largest_first(self) -> None:
        shares: dict[str, float] = profile_hot_paths.area_shares(profile_hot_paths.parse_report(SELF_LISTING))
        self.assertEqual(list(shares)[0], 'skia')
        self.assertEqual(shares['skia'], 34.05)
        self.assertEqual(shares['gui_forms'], 8.5)

    def test_markdown_table_escapes_pipes_and_lists_top_rows(self) -> None:
        rows: list[HotFunction] = [HotFunction('operator|(a, b)', '/x/libfoo.so', 5.0, 9.0, 'other')]
        table: str = profile_hot_paths.markdown_table('paint', rows, 10)
        self.assertIn('operator\\|(a, b)', table)
        self.assertIn('| 1 | 5.00% | 9.00% | other |', table)
        self.assertIn('libfoo.so', table)


if __name__ == '__main__':
    unittest.main()
