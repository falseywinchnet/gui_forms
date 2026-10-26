# Stage L, L5b: positioning (2026-10-25)

Phase L5 of `planning/stages/L-layout/PLAN.md`, its second part: `position:
relative` and `position: absolute` in FlexLayoutPanel, GridLayoutPanel and
BlockFlowPanel (PLAN section 2.5), the containing block as the nearest
positioned box (owner decision L-Q6, "warn on this behavior, but allow"),
`Window::layout_diagnostics()` and the containing-block dependency (section
3.3). The first part, block and inline flow, is L5a
(`results/2026-10-25-l5-block/`).

## What L5b adds

- Core arithmetic, `include/gui_forms/layout/positioning/positioning.hpp`
  and `src/core/layout/positioning/positioning.cpp`, over named records
  with no control dependency:
  - `ItemPositioning{position, inset}`, the record each container keeps
    per child;
  - `relative_position_offset`: CSS 2.1 9.4.3, the start side winning
    horizontally (left for left to right, right for right to left) and top
    vertically;
  - `resolve_absolute_axis`: one axis of an absolutely positioned box, CSS
    2.1 10.3.7 and 10.6.4: a given size, a size stretched between both
    insets, or shrink-to-fit between the min-content and max-content
    widths; auto margins sharing what is left; an over-constrained box
    keeping its start inset (its end inset in a right-to-left containing
    block); minimum over maximum.
- Layout diagnostics, `include/gui_forms/layout/diagnostics/
  layout_diagnostics.hpp`: a severity, a code and the nodes, in T3's
  nonfatal form. The one code is `containing_block_not_parent` (201).
- The three containers (`FlexItemLayout`, `GridItemLayout` and
  `BlockFlowItemLayout` each gain `positioning`; `item_positioning(child)`
  reads it):
  - A relative item is laid out in the flow and its slot offset by its
    insets, percentages of the content box.
  - An absolute item leaves the solve (it takes no space, no grid cell, no
    place on a line, and does not separate collapsing margins) and is
    placed after the in-flow items (`src/controls/scrollable_control/
    container_control/out_of_flow.hpp`): its containing block is the
    padding box of the nearest positioned box from the container up (the
    container itself when its own parent positions it; a positioned
    ancestor; or the top of the tree, the window's root control, whose box
    is the client area). The parent computes the child's rectangle in that
    box, converts it into its own coordinates and assigns it with
    `set_child_layout`; ownership never changes. The child is measured only
    on an axis whose size comes from its content.
  - An absolute nested BlockFlowPanel is a formatting-context root.
- The window (`Window::layout_diagnostics()`,
  `dropped_layout_diagnostics()`, `clear_layout_diagnostics()`):
  - A container reports each absolute child's resolution; the window
    records `containing_block_not_parent`, with the child's and the
    ancestor's stable IDs, when the resolution changes and the box is not
    the parent's. At most 1,000 records; later ones are counted.
  - Containing-block dependency: a container registers the controls whose
    arrangement can move its children's containing block, from its parent
    up to the ancestor (PLAN 3.3 names the ancestor; the controls between
    move the block relative to the container too, so they are watched
    alike). When a watched control's arranged bounds change in an arrange
    pass, or its positioning changes, each container watching it is marked
    for arrange. The dependency points down the tree, so the same top-down
    pass arranges it: no extra pass, no loop. A watched control keeps at
    most 4,096 dependents (`maximum_containing_block_dependents`); above
    that it marks its whole subtree for arrange instead, counted in
    `containing_block_subtree_arranges()`. Entries go when their control
    leaves the window.
- The corpus harness maps `position` and the insets on flex, grid and block
  flow items, and refuses what is not built (below).
- Docstrings of the three panels describe positioning; the API reference
  is regenerated.

## Gate

| Measure | Result |
| --- | --- |
| Positioning corpus | 21 fixtures, 8 profiles, 1,064 nodes, 0 over tolerance (0.05 px); largest edge difference 0.0104 px (LayoutUnit rounding); 19 fixtures exact |
| Whole corpus | 181 fixtures, 9,136 nodes, 0 over tolerance; the 64 recorded differences are L0's FlowLayoutPanel ones, unchanged |
| Resizing the ancestor | a `bottom: 0` grandchild follows in one arrange pass; the middle container's own slot is unchanged |
| Removing positioning | the child re-resolves to the next box (the window root) and a second warning is recorded; positioning it again records a third |
| Disposing the ancestor | no dependent and no watch remain |
| 4,097 dependents | 4,096 kept, then the subtree is arranged, in one arrange pass; 1,000 warnings kept, 3,097 counted |

The fixtures (`tests/layout_corpus/fixtures/position/`, Chromium
155.0.8059.39 references, win64) cover:
- flex: absolute items in the corners, stretched between insets with
  margins, percentage and `calc()` insets and sizes, shrink-to-fit flex
  containers (row and column), auto margins centring and over-constrained
  boxes, min and max sizes, relative items with every inset pair and
  percentages;
- grid: absolute items outside auto placement, relative items in cells,
  percentages of the padding box;
- block flow: absolute items between collapsing margins and inside a line,
  relative blocks and inline items, right to left (an over-constrained box
  keeping its right inset, right winning a relative offset, auto margins
  centring), an absolute block flow panel's content height and
  shrink-to-fit width;
- containing blocks beyond the parent: a positioned grandparent, the
  window's client area, a relative ancestor's own offset, an absolute
  container holding absolute items, a relative and a static container side
  by side, and a form pattern with a pinned badge and footer.

Tests: `tests/positioning_tests.cpp` (`gui_forms_positioning_tests`): the
core arithmetic, absolute and relative children of each container, the
warning and its once-per-change rule, the dependency (resize, removal,
disposal) and both limits.

## Recorded limits

- An absolute item's static position (both insets `auto` on an axis) is
  the container's content box's start corner (its end edge right to left),
  not CSS's in-flow position, nor a flex or grid container's alignment of a
  sole item. The corpus does not compare such items.
- An absolute item's containing block is the whole padding box: a grid
  item's grid-placement lines (CSS Grid 9.1) are not applied.
- Relative offsets take percentages of the container's content box: a
  grid item's are of its grid area in CSS, and a block flow item's vertical
  ones apply only to a definite height in CSS.
- The window root as the containing block takes the root's direction; a
  browser's initial containing block takes the document element's.
- A child that stops being absolute keeps its last resolution until it
  leaves the window, so becoming absolute again against the same box
  records no new warning.

## Commands

```text
cmake --build .build/native-windows-x64/gui-forms --target gui_forms_positioning_tests gui_forms_flex_layout_panel_tests gui_forms_grid_layout_panel_tests gui_forms_block_flow_panel_tests gui_forms_layout_corpus_tests gui_forms_layout_style_tests gui_forms_gallery_pages_tests
gui_forms_positioning_tests
gui_forms_layout_corpus_tests tests/layout_corpus
python -B tools/layout_oracle/capture.py --chromium <chrome-headless-shell 155.0.8059.39 win64> --corpus tests/layout_corpus --output <empty directory> --platform win64 --fixture position/<name> ...
```
