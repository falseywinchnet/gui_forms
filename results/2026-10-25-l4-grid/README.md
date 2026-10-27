# Stage L, L4: grid (2026-10-25)

Phase L4 of `planning/stages/L-layout/PLAN.md`: the core grid solver and
`GridLayoutPanel`, a CSS grid container over child controls. Gate: the grid
corpus (about 40 fixtures, including every grid form the atlas uses) within
tolerance, and the placement and track limits refusing with diagnostics.

## What L4 adds

- `include/gui_forms/layout/grid_solver/grid_solver.hpp` (public) and
  `src/core/layout/grid_solver/grid_solver.cpp`: CSS Grid Layout as four
  calls over caller-owned storage (`GridLayoutWorkspace`, reused).
  - `place_grid_items` builds the explicit grid. It counts
    `repeat(auto-fill | auto-fit, ...)` against a definite extent (7.2.3.2)
    and widens the grid to the template areas. It resolves line numbers
    (negative ones included), spans and named areas, a missing area
    meaning the first implicit line (8.3.1). Then it auto-places in the
    order of 8.5: items fixed on both axes, items locked to a lane, then
    the cursor, sparse or dense, in row or column flow. Lines before the
    explicit grid add leading implicit tracks, and empty `auto-fit`
    repetitions collapse.
  - `size_grid_tracks` runs the track sizing algorithm (12.4 to 12.8) for
    columns, then for rows:
    - single-span items, then spanning items grouped by span (intrinsic,
      content-based and max-content minimums, then intrinsic and
      max-content maximums);
    - items crossing flexible tracks, distributed by fraction;
    - maximize, find the size of an `fr`, and stretch the `auto` tracks.
    Row contributions carry first-baseline shims. Between the two calls
    the caller measures heights at `grid_item_column_size`.
  - `arrange_grid_items` handles `justify-content` and `align-content`
    (falling back to safe positions on overflow), self alignment with
    `auto` margins, and right-to-left mirroring of the columns.
  - 12.1's repeat of the column sizing is not run: a control's widths
    never depend on its height.
  - Limits: at most 65,536 items, and an occupancy of at most 1,048,576
    cells (`refused_too_many_items`, `refused_too_many_cells`), refused
    before anything is laid out.
- `GridLayoutPanel` (`include/gui_forms/controls/scrollable_control/
  container_control/grid_layout_panel/`) gives the L2 records' grid
  container properties as setters. Each one validates: the track limit
  (10,000), `fr` only as a maximum, an automatic repeat only of fixed
  tracks, distinct rectangular areas, and one `GridItemLayout` per child.
  - Items size through the L1 protocol: min-content and max-content widths,
    then the height at the column width. An item stretched over fixed
    columns is asked only its height.
  - It provides `auto_size` (fit-content), `auto_scroll`,
    `layout_snapshot()` (tracks with base sizes, growth limits and
    positions; item areas and bounds) and `layout_outcome()`.
  - `maximum_grid_tracks` moved from the reader's private header to
    `layout_style.hpp`, so the panel and the reader share one limit.
- Registration: `manifests/controls.json` and `control-schema.json`, the
  generated indexes and schema page, the README table, a docstring with
  two examples (asserted in `grid_layout_panel_tests`), and the
  GridLayoutPanel group on the Gallery's CSS layout panels page:
  - named areas over `fr` tracks;
  - `repeat(auto-fill, minmax(86px, 1fr))` cards, one spanning two.
- The corpus harness reads grid containers and items whole with the L2
  reader (`data-gf-kind="grid-layout-panel"`, nested flex and grid
  containers included). There are 52 fixtures under
  `tests/layout_corpus/fixtures/grid/`, with Chromium 155.0.8059.39
  references (win64).

## Gate

| Measure | Result |
| --- | --- |
| Grid corpus | 52 fixtures, 8 profiles, 2,544 nodes, 0 over tolerance (0.05 px); largest edge difference 0.026 px (align-content-space-around, LayoutUnit rounding); 43 fixtures exact |
| Whole corpus | 129 fixtures, 6,064 nodes, 0 over tolerance; the 64 recorded differences are L0's FlowLayoutPanel ones, unchanged |
| Atlas forms | `px`, `fr`, `minmax(0, 1fr)`, `minmax(px, fr)`, `repeat(n, ...)`, `repeat(auto-fill, minmax(86px, 1fr))`, and the plan's `max-content minmax(0, 1fr)` form: one fixture each at least |
| Track limit | the reader refuses more than 10,000 tracks (value_out_of_range, L2); `set_template_columns` / `set_template_rows` / `set_auto_*` refuse them with `std::invalid_argument` |
| Placement limit | an item at lines 2000 / 2000 makes `layout_outcome()` `refused_too_many_cells` and publishes no child (grid_layout_panel_tests) |
| 10,000 children | `repeat(auto-fill, 120px)` columns: changing one item's text costs 1 measure callback (3 cache misses: item, panel, root); resizing the panel costs 0; arranging allocates 0 times |

The fixtures cover:
- fixed, `fr`, percentage and content-keyword tracks, `minmax()` and
  `fit-content()`;
- `repeat(n)` and the auto-fill and auto-fit repeats, alone and between
  fixed tracks;
- intrinsic and spanning contributions, including an item spanning a
  flexible track, and the automatic minimum of an `fr` column;
- every `align-content` value, three `justify-content` values, item and
  self alignment, and baseline;
- line, negative-line and span placement, implicit and leading implicit
  tracks, `grid-auto-rows` patterns, and column, dense, sparse and
  lane-locked flow;
- named areas, with and without line placement;
- percentage gaps, item sizes and bounds, auto margins, padding, `fr`
  rows, `minmax(30px, auto)` rows, and a nested grid.

## Correction found by the corpus

- `spanning-flexible`: an item 400 px wide spans `60px` and `1fr`.
  Chromium gives the `fr` column 340 px, and GUI.Forms gave it its share
  alone. CSS Grid 12.3 says that an item with a definite preferred size
  has its min-content contribution as its minimum contribution, whatever
  its `min-width`. The panel now passes that (rows likewise), and the
  solver header states it.

## Gallery display trace

The grid group changes the css-layout-panels digest, and adds to the
counts on two pages:
- states-and-diagnostics: the control count, 1243 to 1259;
- typography-and-inspection: the RuntimeId, #1268 to #1284; its
  generation, 993 to 1009; and the control count.

The trace diffs against L3's traces show nothing else, so those three
digests are re-recorded under the rule in `display_trace_tests.cpp`.

## Open items

- Right-to-left is covered by the solver tests only; the corpus harness
  has no `dir` attribute yet (as in L3).
- `justify-self: baseline` behaves as `start`; row-axis baselines are not
  built.
- A control's `minimum_size` bounds an item with `min-width: auto` only
  through its contributions, not when the item is stretched.

## Commands

```text
cmake --build .build/native-windows-x64/gui-forms --target gui_forms_grid_solver_tests gui_forms_grid_layout_panel_tests gui_forms_layout_corpus_tests gui_forms_layout_style_tests gui_forms_gallery_pages_tests gui_forms_doc_examples_tests
gui_forms_layout_corpus_tests tests/layout_corpus
gui_forms_gallery_pages_tests --display-trace
python -B tools/layout_oracle/capture.py --chromium <chrome-headless-shell 155.0.8059.39 win64> --corpus tests/layout_corpus --output <empty directory> --platform win64 --fixture grid/<name> ...
```
