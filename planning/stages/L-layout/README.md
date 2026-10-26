# Stage L: real dynamic layout (flex, grid, flow)

Release: 0.4 Flexible. Depends on: T1. Unblocks FD, D and H. Status
2026-10-25: plan accepted ([`PLAN.md`](PLAN.md)); interview round 001
answered (below); L0 next. T1's gate is recorded
(`results/2026-10-13-t1-anatomy-gate`). Authority: `planning/FORWARD_PROGRAMME.md` §4 L.

Read [the shared brief](../README.md) first.

## Why

The owner, 2026-10-12: "dynamic flexing layouts should be easier than they
are. This is an engine task." Layout stays out of themes (T3) and belongs to
forms and the engine.

HTML import (H) needs real layout. The earlier Web.Forms conversion produced,
in the owner's words, "completely baby level extremely deficient GUI.Forms
layouts that didn't look at all like the webpage they copied". The owner
wants "real layout", and the interface builder (D) needs layout that is easy
to author visually.

## What exists today

Containers (`docs/user/layout.md`; headers under
`include/gui_forms/controls/scrollable_control/container_control/`):

- `Panel` and `GroupBox`: absolute placement plus Dock and Anchor. Dock
  consumes children backmost first.
- `FlowLayoutPanel`: deterministic flow with wrapping and breaks, "not
  flexbox".
- `TableLayoutPanel`: absolute, auto and weighted tracks, spans, growth,
  Dock and Anchor inside cells, and resolved-track inspection.
- `SplitContainer`, `MasterDetailView`: two panes with a focusable seam.
- `ScaledPanel`, `ScaledGroupBox`: proportional design space.
- `ResponsiveTrackPanel`: nested fixed, content-measured and weighted tracks
  with priority-authored collapse.

The layout model is synchronous state with a geometry read barrier
(interview GF006, option L4): setters mark typed dirtiness, a read of
geometry forces a bounded layout pass, nested update scopes defer it, and
passes are bounded. The docs page says it plainly: no constraint or
grid-template layout, no CSS.

The earlier experiments left one excellent tool, which the owner decided to
copy into `tools/`: the browser/native fidelity oracle (`file_manager/web_forms/experiments/FIDELITY_ORACLE_001.md` in the
`falseywinchnet/file_manager` repository). It compares a real Chromium
layout against the GUI.Forms retained tree node by node, keeping structure,
state, geometry, typography, material and raster differences separate. On
the File Manager source it found zero structure, state and material
mismatches over 119 nodes, but 69 nodes over the geometry tolerance, 23 over
typography and 32 over raster.

## What to deliver

1. Flex and grid semantics as GUI.Forms layout containers: direction, wrap,
   grow, shrink, basis, alignment and gaps; grid tracks with `fr`, `minmax`,
   auto, explicit and automatic placement, and spans.
2. Block and inline flow for imported documents (needed by H).
3. All within the existing contract: typed dirtiness, read barriers, bounded
   passes, nested update scopes, deterministic order, inspectable resolved
   geometry.
4. A layout fixture corpus run against Chromium with the fidelity oracle at
   several window sizes.

## How

- Interview GF008 (2026-08) said composition uses "a GUI.Forms-owned flex
  algorithm", that "flex" describes resizing behaviour, and that GUI.Forms
  makes no promise to reproduce every CSS Flexbox edge case. The forward
  programme's gate now measures against Chromium. Where a CSS edge case and a
  principled GUI.Forms rule disagree, record the difference in the fixture
  with its reason, and raise any pattern of them with the owner. Never
  silently fold differences into a pass.
- Chromium is a test oracle at build or test time only, never a runtime
  dependency. No browser engine enters GUI.Forms (`AGENTS.md`).
- Text measurement goes through the bundled engine, and family requests
  through F's substitution table. Without F, typography differences swamp
  the geometry comparison.
- Keep the oracle's honesty: a difference stays visible in the report and is
  never laundered into a parity claim (`FIDELITY_ORACLE_001.md`).
- House style: tracks, lines and items are named records with explicit
  capacity versus live length (§1); the layout pass fills caller-owned
  geometry arrays (§4); no allocation per item in the arrange loop (§3);
  sizes are wrapped logical units, never bare doubles mixed with device
  pixels (§1).

## Gate

A corpus of layout fixtures matches Chromium's boxes within a recorded
tolerance at several window sizes, using the fidelity oracle approach of the
earlier Web.Forms experiments. Record the corpus, sizes, tolerance and
per-node report under `results/`.

## Owner decisions, 2026-10-13

1. **The fidelity oracle moves into this repository** under `tools/`. The
   Web.Forms compiler and HTML profile stay behind.
2. **Chromium is a test oracle only**, never called by GUI.Forms code at
   import or run time.

## Owner decisions, interview round 001 (2026-10-25)

The owner answered the 15 questions of [`PLAN.md`](PLAN.md) §9 on
2026-10-25 (relayed by the coordinator). Fourteen follow the
recommendation; questions 6 and 10 do not.

| | Decision |
|---|---|
| L-Q1 | Three typed containers over one shared solver library: `FlexLayoutPanel`, `GridLayoutPanel`, `BlockFlowPanel` |
| L-Q2 | `FlowLayoutPanel` and `TableLayoutPanel` keep their behaviour; their insides are rebuilt on the new solvers wherever their existing tests pass unchanged |
| L-Q3 | Flex automatic minimum size as CSS: an item's minimum is its content minimum unless `min-width` or `min-height` says otherwise |
| L-Q4 | Block margin collapsing by CSS 2.1 §8.3.1, without floats |
| L-Q5 | Floats left out of L; HTML import reports them as unsupported |
| L-Q6 | **An absolutely positioned child's containing block is the nearest positioned ancestor, as CSS.** The owner, verbatim: "warn on this behavior, but allow." See below |
| L-Q7 | An inherited `LayoutDirection` on every control; new containers first, existing containers and Dock/Anchor in L6 |
| L-Q8 | Layout is independent of display scale; edges are snapped to device pixels only when painting, by edge rounding |
| L-Q9 | Fixtures are well-formed HTML fragments read by both Chromium and the core XML reader; tolerance 0.05 logical px per edge for box fixtures, 0.51 px per edge and 1.0 px on the first baseline for text fixtures |
| L-Q10 | **Both Chrome for Testing 155.0.8059.39 packages are approved**: `chrome-headless-shell-linux64.zip` (124,203,329 bytes) for the Linux CI job, and `chrome-headless-shell-win64.zip` (124,737,213 bytes) for local reference runs on the owner's Windows machine. See below |
| L-Q11 | References are checked into the repository; every host compares against them without Chromium; a Linux job regenerates and checks them and is not a required check |
| L-Q12 | The solvers lay out records; item systems realize only visible items; no virtualizing container |
| L-Q13 | DK's binary split solver is kept as it is |
| L-Q14 | Layout values: `px`, `%`, `fr`, the content keywords and linear `calc()`; `em` resolved to px when importing; `min()`, `max()`, `clamp()` later |
| L-Q15 | `grid-template-areas` and `grid-area` in phase L4; named lines left out |

**L-Q6 in detail** (owner and coordinator, 2026-10-25):

- Ownership does not change. The direct parent still owns the child and
  assigns its layout slot; only the containing block used to compute the
  child's geometry is the ancestor's padding box. The slot is converted
  into the parent's client coordinates before it is assigned.
- When the containing block is not the direct parent, the layout pass
  issues a structured warning (`containing_block_not_parent`), in the same
  nonfatal-diagnostic vocabulary T3 uses: a severity, a code, the child's
  and the ancestor's stable IDs. It is queryable, not only logged, and is
  reported once per change of resolution, not on every pass.
- Re-layout follows the ancestor's geometry. The dependency is explicit
  (the ancestor records its out-of-flow dependents during arrange) and
  bounded (a counted limit per ancestor, and it runs inside the same
  top-down pass), and it is tested.

**L-Q10 in detail**: each package is pinned by version and SHA-256, and
its hash is verified before use. Each is unpacked into its own new, empty
directory outside the repository, and its paths are passed as arguments.
Nothing else is downloaded.

## Decisions in force

**Container names** (owner, 2026-10-26):
- The Windows Forms-exact containers become `FlowLayoutPanelCompat` and
  `TableLayoutPanelCompat`, and keep their own algorithms (below).
- The CSS containers are named after their CSS models, as panels:
  `FlexLayoutPanel` becomes `FlexPanel`, `GridLayoutPanel` becomes
  `GridPanel`, and `BlockFlowPanel` becomes `BlockPanel`. The owner's
  reason: "if they're going to be used as panels, i suggest panels so that
  they're not confused with the property". (These supersede the shorter
  `Flex`, `Grid` and `Block` first proposed.)
- The plain names `FlowLayoutPanel` and `TableLayoutPanel` go away.
- When a Windows Forms design or form document names `FlowLayoutPanel` or
  `TableLayoutPanel`, the importer maps it to the `Compat` container, so
  old layouts render the same.
- The purpose, in the owner's words: "the point would be to discourage the
  use in general by adding Compat". So:
  - the `Compat` containers' docs open by saying they exist for Windows
    Forms migration, and point to `FlexPanel` and `GridPanel` for new work;
  - the Gallery shows them in a clearly labelled compatibility group, not
    beside `FlexPanel` and `GridPanel`;
  - form documents and the interface builder offer `FlexPanel` and
    `GridPanel` first.
- The importer's table is the registry's `windows_forms_name` field
  (`manifests/controls.json`, checked by `tools/control_registry.py`;
  coordinator, 2026-10-26): `FlowLayoutPanel` and `TableLayoutPanel` map to
  the `Compat` records. The C# facade keeps the Windows Forms names.
- The solvers' helper types keep their algorithm names (`FlexItemLayout`,
  `GridItemLayout`, `BlockFlowItemLayout`, `block_flow_solver`), and the
  historical records under `results/` and `planning/` keep the old
  container names: they are history, not stale names to fix.

**The Windows Forms containers are not rebased on the solvers**
(coordinator, 2026-10-25, applying L-Q2's condition "wherever their
existing tests pass unchanged"). Neither container can be rebased without
changing what its tests pin. Both keep their own algorithm, gain a reused
workspace in place of their per-pass and per-line vectors, and adopt
`LayoutDirection` (phase L6).

`FlowLayoutPanel` differs from CSS flex in four behaviours:
1. A forced break (`set_flow_break`) ends a line even with `wrap_contents`
   off. Pinned by `test_flow_direction_break_visibility_and_resize`.
2. A single line takes the panel's whole cross extent, while several lines
   pack at the start at their content's extent. Pinned by
   `test_flow_alignment_projects_relational_flex_geometry` and L0's
   recorded corpus differences.
3. Items grow (`set_flex_grow`) only when arranged; a measure reports the
   content without growth. Pinned by
   `test_flow_alignment_projects_relational_flex_geometry` and
   `test_flow_auto_size_mode_preserves_authored_extent`.
4. With `wrap_contents` off, main-axis alignment still applies within the
   panel's width. Pinned by
   `test_flow_alignment_projects_relational_flex_geometry`.

Matching them would need four Windows-Forms-only modes inside the CSS flex
solver.

`TableLayoutPanel` has two gaps:
1. Its `Percent` styles are weights over the space left after the
   `Absolute` and `AutoSize` tracks, not percentages of the panel. An `fr`
   track would only approximate them.
2. Its distribution of a spanning item's size over `AutoSize` tracks is
   not CSS Grid's intrinsic sizing (section 11.5).

Both gaps are pinned by `test_table_mixed_tracks_spans_and_lookup` and
`test_table_growth_hidden_children_and_fixed_overflow` (all in
`tests/layout_panel_tests.cpp`).

**Right-to-left layout is full mirroring** (owner, 2026-10-26, option 1),
as Windows Forms' `RightToLeftLayout`. In a parent whose effective
`LayoutDirection` is right to left:
- a child's requested x is measured from the right edge;
- `Dock` left docks right, and `Anchor` left anchors right;
- `FlowLayoutPanelCompat` and `TableLayoutPanelCompat` start at the right;
- the CSS containers keep their own start and end model.

The layout is computed left to right and mirrored once, at output, about
the parent's child viewport, so padding and margins mirror with it. Left
to right changes nothing.

## Read first

- `docs/user/layout.md`
- `planning/interview/GUI_FORMS_INTERVIEW_LEDGER.md` GF006, GF008
- `file_manager/web_forms/experiments/FIDELITY_ORACLE_001.md` and
  `STAGE3_NATIVE_PROJECTION_001.md`: not in this repository, so not read
  (`AGENTS.md`: this subtree never inspects sources outside it); the
  summary above under "What exists today" is what L designs from
- `experiments/M12P11_LAYOUT_TRANSACTIONS.md`,
  `experiments/M12P12_MUTATION_SAFE_LAYOUT.md`
