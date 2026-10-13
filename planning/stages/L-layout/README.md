# Stage L: real dynamic layout (flex, grid, flow)

Release: 0.4 Flexible. Depends on: T1. Unblocks FD, D and H. Status
2026-10-13: not started; ready, since T1's gate is recorded
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

## Read first

- `docs/user/layout.md`
- `planning/interview/GUI_FORMS_INTERVIEW_LEDGER.md` GF006, GF008
- `file_manager/web_forms/experiments/FIDELITY_ORACLE_001.md` and
  `STAGE3_NATIVE_PROJECTION_001.md`
- `experiments/M12P11_LAYOUT_TRANSACTIONS.md`,
  `experiments/M12P12_MUTATION_SAFE_LAYOUT.md`
