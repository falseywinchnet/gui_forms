# Stage L, L3: flex (2026-10-25)

Phase L3 of `planning/stages/L-layout/PLAN.md`: the core flex solver and
`FlexLayoutPanel`, a CSS flex container over child controls. Gate: the flex
corpus (about 40 fixtures) within tolerance at every profile, and the
10,000-child counts of section 5.

## What L3 adds

- `include/gui_forms/layout/flex_solver/flex_solver.hpp` (public) and
  `src/core/layout/flex_solver/flex_solver.cpp`: CSS Flexbox section 9 as
  two calls over caller-owned storage. `resolve_flex_main_sizes` sorts by
  `order` (only when an order is nonzero), breaks lines and runs the
  freeze loop (9.7); the caller then measures each item's hypothetical
  cross size at its final main size; `place_flex_items` sizes the lines,
  applies `align-content`, stretches, resolves auto margins, justifies,
  aligns across (with first-baseline alignment) and maps the flow-relative
  result to physical boxes for `row-reverse`, `column-reverse`,
  `wrap-reverse` and right-to-left. `FlexLayoutWorkspace` holds the items,
  lines and order and is reused; at most 65,536 items, refused before any
  storage is touched.
- `FlexLayoutPanel` (`include/gui_forms/controls/scrollable_control/
  container_control/flex_layout_panel/`): the container properties of the
  L2 records, one `FlexItemLayout` per child (dropped when the child
  leaves), content-sized items through the L1 constraint protocol
  (max-content for the content size, min-content for the automatic
  minimum, a height at the final width), `auto_size` as fit-content,
  `auto_scroll`, and `layout_snapshot()` with the solver's base,
  hypothetical and final sizes, lines and freeze reasons.
- Registration: `manifests/controls.json` and `control-schema.json`, the
  generated control indexes and schema page, the README table, a docstring
  with two examples (both asserted in `flex_layout_panel_tests`), and two
  specimens on a new Gallery page, CSS layout panels (`css-layout-panels`,
  in the Layout group), with its test. The page will also hold the grid
  (L4) and block-flow (L5) specimens; the Retained Layout Panels page has
  no room left within its 660 px body.
- The corpus harness reads flex fixtures whole with the L2 reader
  (`data-gf-kind="flex-layout-panel"`), and 51 fixtures under
  `tests/layout_corpus/fixtures/flex/` with Chromium 155.0.8059.39
  references.

## One core change: `Control::measures_children_in_arrange`

The window's measure pass measured every measure-dirty control against its
parent's available size. A flex item's content query is max-content, so a
changed label was measured twice: once by the window (an at-most answer
nothing reads) and once by the panel. The new virtual, false by default,
lets a container whose arrange asks its children its own questions leave a
dirty direct child to that arrange; the dirtiness has already emptied the
child's cache, so nothing stale is answered. Only `FlexLayoutPanel` returns
true, so every other container's measure pass is unchanged.

## A correction found by the corpus

The solver first fell back to centre for `space-around` and `space-evenly`
with negative free space (Flexbox 1's wording). Chromium starts them at the
start, as CSS Box Alignment 3 specifies: the fallbacks are *safe* centre
and safe flex-start, and safe alignment never overflows the start. The
solver now does the same; a lone item with positive space is still
centred (`flex/justify-overflow-fallback`, solver test).

## Gate

Flex corpus: 51 fixtures, 8 profiles, 2,200 nodes; 0 over tolerance and no
recorded differences. Largest edge difference 0.0375 logical px
(`flex/wrap-gaps`: a 2% column gap of 320 px is 6.4 px, which Chromium
holds as 410/64 px; six gaps accumulate 0.0375). 40 fixtures are exact.
The whole corpus: 77 fixtures, 3,520 nodes, 0 over tolerance; the 64
recorded differences are the flow and table fixtures' existing ones.

Counts at 10,000 children (`gui_forms_flex_layout_panel_tests`), a wrapping
row of labels in a plain root:

| Workload | Measured |
| --- | --- |
| Change one item's text, then lay out | 1 measure callback (the changed item's); 3 cache misses (the item, the panel, the root) |
| Resize the panel from 1200 to 900 px wide | 0 item measure callbacks |
| Arrange again at the same bounds | 0 allocations |

## Open items

- Resizing the *window* still marks the whole tree measure-dirty
  (`Window::resize` uses `invalidation::bounds` on the root's subtree), so
  it empties every cache: at 10,000 labels a window resize costs 10,000
  callbacks. With the cache, only the root's own measure depends on the
  client size. Changing it touches every container's measure pass, so it
  is left for L7's gate with the counts above as the baseline.
- No text fixtures: `Label` has no baseline query yet and draws its text
  inside a 4 px inset, so text items wait for L5 (inline layout), which
  defines both. `align-items: baseline` is covered for boxes, whose
  synthesized baseline is the border-box bottom, as in Chromium.
- Right-to-left is covered by the solver tests only; the corpus harness has
  no `dir` attribute yet.

## Gallery display trace: digests re-recorded

The new page adds a navigation entry, so every page's display trace
changed, and `gui_forms_gallery_display_trace_tests` (T2 E2) was
re-recorded under the coordinator's rule: only when page-by-page trace
diffs show intended changes alone. The diffs, against a run of the same
tree with FlexLayoutPanel demonstrated on no page (which reproduces every
committed digest, both live-surfaces ones included):

- css-layout-panels: the new page.
- Every page: the navigation sidebar only, meaning its new entry, its
  taller paper and its scroll offset and thumb.
- states-and-diagnostics, beyond the sidebar: the live control count
  (1226 to 1243) and the paint chunks reused (297 to 296).
- typography-and-inspection, beyond the sidebar: the inspected specimen's
  RuntimeId (#1251 to #1268), its generation (972 to 993) and the control
  count.

Nothing else changed. The non-Windows live-surfaces digest is the Windows
trace with `BGRA` replaced by `RGBA`, hashed as the test hashes it; the
same computation reproduces both previous digests.

## Commands

```text
cmake --build .build/native-windows-x64/gui-forms --target gui_forms_flex_solver_tests gui_forms_flex_layout_panel_tests gui_forms_layout_corpus_tests gui_forms_gallery_pages_tests gui_forms_doc_examples_tests
gui_forms_gallery_pages_tests --display-trace
gui_forms_layout_corpus_tests tests/layout_corpus
python -B tools/layout_oracle/capture.py --chromium <chrome-headless-shell 155.0.8059.39 win64> --corpus tests/layout_corpus --output <empty directory> --platform win64 --fixture flex/<name> ...
```
