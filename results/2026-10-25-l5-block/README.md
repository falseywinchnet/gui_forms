# Stage L, L5a: block and inline flow (2026-10-25)

Phase L5 of `planning/stages/L-layout/PLAN.md`, its first part: the core
block flow solver and `BlockFlowPanel`, a CSS block formatting context over
child controls (PLAN section 2.4). Gate: the block corpus (about 20
fixtures) within tolerance. Positioning (section 2.5, `position: relative`
and `absolute` in all three containers, `Window::layout_diagnostics()`)
is the second part, L5b.

## What L5a adds

- `include/gui_forms/layout/block_flow_solver/block_flow_solver.hpp`
  (public) and `src/core/layout/block_flow_solver/block_flow_solver.cpp`:
  one pass over caller-resolved item records into a reused
  `BlockFlowLayoutWorkspace`.
  - Block items stack with their vertical margins collapsed (CSS 2.1
    8.3.1 without floats or clearance; decision L-Q4): siblings, negative
    margins, empty blocks whose own margins adjoin, and the margins of a
    nested container's content through its top and bottom edges
    (`leading_inner`, `trailing_inner`, `collapses_through`). Horizontal
    margins follow 10.3.3: `auto` margins share the free space, are zero
    when there is none, and an over-constrained item keeps its start
    margin (left to right: left; right to left: right).
  - A run of inline-level items (atomic boxes) is broken into line boxes
    at item boundaries, greedily, and aligned by `text-align` (`start`,
    `end`, `left`, `right`, `center`; an overflowing line is
    start-aligned). Each line starts with the container's `LineStrut`;
    `vertical-align` is `baseline` (the margin box's bottom), `middle`
    (its centre at the baseline plus half the x-height), `top` and
    `bottom`.
  - `block_flow_intrinsic_width` gives the min-content and max-content
    widths (widest block; widest item or the sum of each inline run).
  - Limit: at most 65,536 items (`refused_too_many_items`).
- `BlockFlowPanel` (`include/gui_forms/controls/scrollable_control/
  container_control/block_flow_panel/`): `text_align`, `line_strut`, one
  `BlockFlowItemLayout` per child (level, sizes, auto margins,
  vertical-align), `layout_snapshot()` (items with their lines, line boxes
  with tops, heights, baselines and offsets) and `layout_outcome()`.
  - Block items fill the panel less their margins and are asked only their
    height at that width; inline items shrink to fit between their
    min-content and max-content widths. Sizes and min/max take lengths,
    percentages and `calc()` of both.
  - A BlockFlowPanel laid out as a block item of another, without
    `auto_scroll`, is not a formatting-context root: its content's
    margins collapse with its own where no padding separates them. Its
    parent reads them through `shared_margins(width)`, which answers from
    the panel's last measure at that width. Any other BlockFlowPanel (the
    window root, a flex or grid item, an inline item, a scroll container)
    is a root.
- Registration: `manifests/controls.json` and `control-schema.json`, the
  generated indexes and schema page, the README table, a docstring with
  two examples (asserted in `block_flow_panel_tests`), and the
  BlockFlowPanel group on the Gallery's CSS layout panels page (stacked
  blocks with collapsing margins and a centred block; a centred, wrapping
  line of inline buttons with one aligned on `middle`).
- The corpus harness reads block flow containers and items whole with the
  L2 reader (`data-gf-kind="block-flow-panel"`). There are 31 fixtures
  under `tests/layout_corpus/fixtures/block/`, with Chromium 155.0.8059.39
  references (win64).

## Chromium calibration

Before the solver was written, probe fixtures (not checked in) settled the
behaviours the specifications leave to the engine:
- A line box with an inline-block of height 10 under the page's Carlito
  13px `line-height: normal` is 15 high with the box 2 down: the strut is
  12 above and 3 below (Chromium's rounded metrics). At `font-size: 0` the
  strut is a point on the baseline, and `line-height: 20px` makes it 10
  above and 10 below.
- With `top` and `bottom` boxes on one line, only the tallest of them
  extends the line (a `top` box its descent, a `bottom` box its ascent):
  for a 20 baseline box with a 40 `bottom` box and a 30 `top` box the
  baseline box sits 20 down, and with a 40 `top` box and a 30 `bottom`
  box it sits at the top.
- A last child's bottom margin passes through a nested auto-height block
  only while that block's used height is its content's height: a
  `min-height` that raises it keeps the margin inside, a `max-height`
  that cuts it drops the margin altogether, and a `min-height` it exceeds
  lets the margin through.

## Gate

| Measure | Result |
| --- | --- |
| Block corpus | 31 fixtures, 8 profiles, 2,008 nodes, 0 over tolerance (0.05 px); largest edge difference 0.022 px (percent-heights, LayoutUnit rounding); 29 fixtures exact |
| Whole corpus | 160 fixtures, 8,072 nodes, 0 over tolerance; the 64 recorded differences are L0's FlowLayoutPanel ones, unchanged |
| 10,000 children | labels as block items: changing one item's text costs 1 measure callback (3 cache misses: item, panel, root); a height change costs 0; arranging allocates 0 times |

The fixtures cover:
- stacking and filling; sibling, nested (top and bottom), empty and
  chained-empty margin collapsing; padding that separates margins;
  `min-height`, `max-height` and definite heights against a last margin;
- auto margins (both, one side), over-constrained margins, percentage and
  `calc()` widths, min and max widths and heights, percentage heights;
- inline runs that wrap, items of varied sizes, every `text-align`
  value and overflowing lines, `vertical-align` `middle`, `top` and
  `bottom` (the tallest-box rule both ways), the `line-height` strut,
  inline margins, blocks between inline runs, shrink-to-fit inline
  panels;
- right to left lines and blocks (`direction: rtl`);
- a flex container as a block item, block flow panels as grid items, and
  a form pattern.

## Gallery display trace

The block group, and the page header's new BlockFlowPanel entry (which
makes the header taller and the page body a little shorter, so the page's
specimens scale with it), change the css-layout-panels digest. They add to
the counts on two pages:
- states-and-diagnostics: the control count, 1259 to 1272;
- typography-and-inspection: the RuntimeId, #1284 to #1297; its
  generation, 1009 to 1022; and the control count.

The trace diffs against L4's traces show nothing else, so those three
digests are re-recorded under the rule in `display_trace_tests.cpp`.

## Open items and recorded limits

- Item baselines are the bottom of the margin box, CSS's baseline of an
  inline-block without line boxes. An inline item that is itself a block
  flow panel holding inline content, or a text control, would have its
  last line's baseline in CSS; no Control baseline query exists yet.
- The strut is an explicit `line_strut`; it is not derived from the
  panel's font. The corpus sets inline content at `font-size: 0` (with a
  px `line-height` for a nonzero strut), which the harness takes out of
  the style and turns into the strut; `direction` likewise becomes
  `set_layout_direction`. Neither is a layout value of the L2 reader.
- A control's margins are nonnegative (a core rule), so the panel never
  sees a negative margin; the solver handles them and its tests cover
  them. A source's negative margins need an importer decision (H).
- A nested panel with `auto_size` sized by a non-block parent resolves
  percentage heights in its arrange but not in its measure.
- Positioning is L5b.

## Commands

```text
cmake --build .build/native-windows-x64/gui-forms --target gui_forms_block_flow_solver_tests gui_forms_block_flow_panel_tests gui_forms_layout_corpus_tests gui_forms_gallery_pages_tests gui_forms_doc_examples_tests
gui_forms_layout_corpus_tests tests/layout_corpus
gui_forms_gallery_pages_tests --display-trace
python -B tools/layout_oracle/capture.py --chromium <chrome-headless-shell 155.0.8059.39 win64> --corpus tests/layout_corpus --output <empty directory> --platform win64 --fixture block/<name> ...
```
