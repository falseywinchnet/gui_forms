# Stage L, L6: the Windows Forms containers and right to left (2026-10-26)

Phase L6 of `planning/stages/L-layout/PLAN.md`: `FlowLayoutPanel` and
`TableLayoutPanel` keep their own algorithms (coordinator, applying L-Q2's
condition; the reasons are in `planning/stages/L-layout/README.md`,
"Decisions in force"), each gains a reused workspace, and `LayoutDirection`
reaches them and Dock and Anchor with the owner's full mirroring (option 1,
2026-10-26).

## What L6 adds

- `LayoutDirection` is a registered `Control` property, `layout_direction`
  (enumeration `inherit`, `left_to_right`, `right_to_left`; default
  `inherit`), so form documents and the property grid can set it. The
  control schema gains it on every type.
- Full mirroring, as Windows Forms' `RightToLeftLayout`. In a parent whose
  effective direction is right to left the layout is computed left to
  right and mirrored once, at output, about the parent's child viewport:
  - `Control::arrange`: `DockStyle::left` docks at the right, an anchor to
    the left keeps the distance to the right edge, a requested x is measured
    from the right edge, and the padding mirrors;
  - `ScrollableControl`: a scrolled right-to-left parent moves its
    mirrored children to the right as it scrolls (the content scrolls
    toward the left), and scrolling a control into view finds it by its
    mirror image;
  - `FlowLayoutPanel`: a `left_to_right` flow starts at the right edge, a
    vertical flow's columns start at the right, and margins mirror;
  - `TableLayoutPanel`: the first column is at the right edge, placement
    inside a cell (Dock and Anchor) mirrors, the cell borders and the
    published `cell_grid` part follow, and cell positions stay logical.
  - Left to right changes nothing. The CSS containers keep their own start
    and end model.
- Reused workspaces. `FlowLayoutPanel` keeps its children, items and lines,
  and `TableLayoutPanel` its children, items, a fixed 64 by 64 occupancy
  bitmap and its track vectors, from pass to pass in place of the per-pass
  and per-line vectors (the table's was a vector of vectors of `bool`).
  Their per-child records are reconciled by stamping the live children
  with the pass number in place of a per-pass set. A child's measure that
  lays the panel out again inside a pass gets storage of its own, so the
  outer pass's survives.

## Gate

| Measure | Result |
| --- | --- |
| Existing container tests (`tests/layout_panel_tests.cpp`) | unchanged, pass |
| L0 fixtures | whole corpus 181 fixtures, 9,136 nodes, 0 over tolerance; the 64 recorded differences are L0's, unchanged |
| New right-to-left tests (`tests/layout_direction_tests.cpp`) | Dock, Anchor and requested bounds; the flow (both horizontal flows, a vertical flow, padding and margins); scrolling; the table (cells, in-cell anchoring, borders, `cell_grid`, cell lookup); a measure that re-enters each panel |
| Other layout, dock, scroll and Gallery tests | pass |
| Gallery display traces | 3 digests re-recorded, by the new property alone: values-and-spinners' property grid shows a `LayoutDirection` row (its editor is two controls more), so states-and-diagnostics' live control count and typography-and-inspection's specimen RuntimeId and control count move by two. No page's layout changed (the Gallery lays out left to right) |

## Recorded limits

- A scrolled right-to-left parent still draws its scroll bars as left to
  right: the thumb starts at the left. The content scrolls correctly.
- A control that places its children itself after the base arrange (for
  example a tab control's pages) is mirrored only where it uses the base
  arrange; the composite controls are not reviewed for right to left here.

## Commands

```text
cmake --build .build/native-windows-x64/gui-forms --target gui_forms_layout_direction_tests gui_forms_layout_panel_tests gui_forms_layout_corpus_tests gui_forms_binding_tests gui_forms_gallery_pages_tests
gui_forms_layout_direction_tests
gui_forms_layout_panel_tests
gui_forms_layout_corpus_tests tests/layout_corpus
GUI_FORMS_CONTROL_SCHEMA=manifests/control-schema.json GUI_FORMS_CONTROL_SCHEMA_WRITE=1 gui_forms_gallery_pages_tests
```
