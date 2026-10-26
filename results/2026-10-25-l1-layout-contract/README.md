# Stage L, L1: the measure and arrange contract (2026-10-25)

Phase L1 of `planning/stages/L-layout/PLAN.md` (sections 3, 4 and 10). It
adds the contract the L3 to L5 solvers are written against and changes no
existing container's layout.

## What L1 adds

- `include/gui_forms/layout/types/layout_types.hpp`: `LogicalLength`,
  `AvailableExtentKind` (definite, at most, min-content, max-content),
  `AvailableExtent`, `LayoutConstraint`, `valid_layout_constraint`,
  `measure_available_size`, and `LayoutDirection`.
- `Control::measure_for_layout(const LayoutConstraint&)`: the cached measure
  entry. Four entries per control (one min-content width, one max-content
  width, the two most recent other queries), each stamped with the control's
  measure generation. Every addition of `Dirty::measure` to a control's own
  flags goes through one private `add_dirty`, which advances the generation,
  so no entry outlives the invalidation that made it stale. A detached
  control's measure invalidation advances its ancestors' generations as
  well. An answer whose own callback invalidated the control is not stored.
- `Control::measure_intrinsic` (protected, virtual): answers a miss.
- `Label::measure_intrinsic`: min-content from the widest unbreakable
  segment, max-content from the widest unwrapped paragraph, both with the
  4 px inset.
- Layout boundaries: `Control::size_depends_on_content()` and
  `Control::sizes_children_from_content()`, both `true` by default.
  `Window::mark_layout_ancestors` stops marking measure at the first ancestor
  for which neither holds, which it marks for arrange only.
- `Control::layout_direction`, `set_layout_direction` and
  `effective_layout_direction`.
- The window's measure pass calls `measure_for_layout`, so its answer is kept
  for the parent's arrange instead of discarded.
- `MetricsSnapshot`: `measure_cache_hits`, `measure_cache_misses`,
  `maximum_measures_per_control_pass`, `layout_boundary_stops` and
  `layout_measure_clamps`.

## Refinements of the plan's wording

Recorded here, and folded into `PLAN.md` once the plan PR has landed:

1. **The base mapping of min-content.** Section 3.1 had min-content pass a
   zero extent to `measure(Size)`. Every `measure(Size)` clamps its answer
   to `available`, so that would report zero for every fixed-size control
   and let a flex item with `min-width: auto` shrink to nothing. The base
   now passes the practical unbounded extent for both content kinds: a
   control that cannot rewrap reports its preferred size as both its
   min-content and its max-content size, as a replaced element does in CSS.
   Only text that wraps (`Label`) overrides it. `ButtonBase` and its family
   (`Button`, `CheckBox`, `RadioButton`, `LinkLabel`) and the single-line
   `TextBox` never rewrap, so the base answer is already theirs.
2. **Layout boundaries are opt-in.** Whether a control's size reads its
   children is that control's own knowledge: `TableLayoutPanel::measure`
   reads them only when auto-sized, and an application control's `measure`
   may read anything. Inferring it for every existing control from
   `auto_size` would change today's propagation, which L1 does not need.
   Both predicates default to `true`, which keeps it exactly; the L3 to L5
   containers state their answers, and L6 decides the existing containers'.
3. **`maximum_measures_per_control_pass`** counts calls through
   `measure_for_layout` only. Today's containers call `measure` and
   `get_preferred_size` directly; they move to the entry when L6 rebases
   them on the solvers.
4. **`LayoutDirection` is not yet a registered property** (no
   `PropertyDescriptor`, schema entry or changed event). Registering it adds
   it to every type's schema; that is done in L6, when the existing
   containers and Dock/Anchor adopt it.
5. **Wrapper types.** L1 adds `LogicalLength`, the one its contract uses.
   The solver index types (`ItemIndex`, `LineIndex`, `TrackIndex`,
   `GridLine`, `FlexFactor`, `FractionFactor`) arrive with their solvers.

## How paint snaps today (section 4's record)

Layout publishes unsnapped logical doubles. The three CPU painters turn a
fractional edge into device pixels differently, and none rounds each edge:

| Painter | Rectangle fill | Effect at a shared fractional edge |
|---|---|---|
| `SkiaRaster` (`src/render/skia/raster/skia_raster.cpp`) | antialiased (`make_paint` sets antialiasing on every fill and stroke) under a canvas scale of the device scale | both neighbours cover the edge pixel partially; source-over leaves a lighter seam |
| `DibPainter` (`src/host/windows/application/windows_host.cpp`) | `pixel_rect` floors the left and top edge and ceils the right and bottom edge, full coverage | both neighbours cover the edge pixel fully; the later paint wins |
| `CoreGraphicsRaster` (`src/render/coregraphics/raster/coregraphics_raster.cpp`) | `CGContextFillRect` with the context's antialiasing | as Skia |

Clips are already edge-rounded: Skia's non-antialiased `clipRect` and the
text clip both round each edge to the nearest pixel.

At integer logical edges and integer scales all three agree. Fractional
edges come from fractional scales (1.25, 1.5) and from distributed free space
(the corpus's `space-around` and `space-evenly` fixtures). Section 4's rule,
round each edge so neighbours share an edge, therefore differs from all
three. The painters belong to the Skia and host owners (`AGENTS.md`), so L1
records the difference and does not change them.

**Decided (coordinator, 2026-10-25):** edge rounding is required (owner
decision L-Q8) and is made in a separate PR off track L, by the standby
worker after DP closes, with the golden updates. Layout keeps publishing
unsnapped logical boxes. This section is that PR's brief.

### Brief for the painter change

The code that changes:

- `src/render/skia/raster/skia_raster.cpp`: `make_paint` (antialiasing on
  for every fill and stroke) and the rectangle entry points `fill_rect`,
  `stroke_rect`, `fill_rounded_rect` and `stroke_rounded_rect`, which draw
  at logical coordinates under `canvas.scale(scale)`. Edge rounding means
  mapping each edge to device pixels, rounding it, and drawing the rounded
  device rectangle without antialiasing. Rounded corners and paths keep
  their antialiasing.
- `src/host/windows/application/windows_host.cpp`: `DibPainter::pixel_rect`
  (floor left and top, ceil right and bottom) and its users, `fill_rect`
  first, plus the clip rectangles at the two `std::floor(clip.x * scale_)`
  sites.
- `src/render/coregraphics/raster/coregraphics_raster.cpp`:
  `CoreGraphicsRaster::fill_rect` and `to_cg_rect`.

The tests that read painted pixels, by painter. Their expected pixels can
move wherever an edge is fractional in device pixels: at scale 1.25 or 1.5,
or with a fractional logical edge at any scale. Tests drawn only at integer
edges and integer scales keep their expectations.

| Painter | Tests |
|---|---|
| `SkiaRaster` | `canvas_raster_tests`, `engine_gate_tests` (and the cross-host engine-gate manifests, `results/2026-10-22-t2-engine-gate/` and `results/2026-10-23-t2-engine-gate/`), `layered_material_raster_tests`, `opaque_live_surface_raster_tests`, `part_composite_identity_tests`, `prepared_text_skia_tests`, `retained_draw_cache_raster_tests`, `text_identity_raster_tests`, `src/render/skia/skia_raster_smoke.cpp` |
| `DibPainter` | `windows_canvas_raster_tests`, `windows_content_pixels_tests`, `windows_control_snapshot_tests`, `windows_damage_rectangles_tests`, `windows_part_composite_tests`, `windows_text_tests`, and the DP record `results/2026-10-13-dp-gate-windows/` |
| `CoreGraphicsRaster` | `layered_material_raster_tests` (its macOS branch), `src/render/coregraphics/coregraphics_raster_smoke.cpp` |

Display traces (`tests/gallery_pages/display_trace_tests.cpp`) record
logical commands, not pixels, and do not move. Text glyph coverage
(`glyph_run_raster_tests`) does not go through the rectangle path. The list
comes from searching `tests/` for the three painters. The PR that makes
the change must run each listed test before and after, and record which
expectations moved and why.

## Gate

Shadow Windows (Windows 11, x64), workspace `D:\gui_forms_w4`, LLVM 22,
the Release tree and the sanitizer tree of the worker scripts.

- `gui_forms_layout_contract_tests`, 11 tests:
  - the constraint values and their validation;
  - the base mapping;
  - hits, misses and generations, including an ancestor made stale by its
    child;
  - slot replacement;
  - the window's answer being kept;
  - the per-control count in each pass (one window answer plus three
    solver questions: 4, and 4 again in the next pass);
  - a layout boundary: only the leaf re-measured, only the boundary
    re-arranged, one stop counted. Under a content-sizing parent,
    propagation continues;
  - detached invalidation;
  - self-invalidation and clamps;
  - Label's min-content (widest word plus the inset, three lines),
    max-content and fixed-width answers, and its height laid out at its
    own min-content width;
  - direction inheritance, override, reparenting, no-op and refusal;
  - identical logical boxes for a flow, docked and anchored tree at scales
    1, 1.25, 1.5 and 2.
- Unchanged and passing, plain and under ASan and UBSan:
  `gui_forms_layout_panel_tests`, `gui_forms_scrollable_control_tests`,
  `gui_forms_dock_layout_tests`, `gui_forms_core_tests` and
  `gui_forms_basic_controls_tests`.
- Also passing (plain): `gui_forms_doc_examples_tests`,
  `gui_forms_inspection_controls_tests`, `gui_forms_visual_inspection_tests`
  and `gui_forms_gallery_pages_tests`, which checks the control schema.
- `generate_library_docs.py --check`, `check_house_style.py` and
  `check_lifetime_rules.py`: 0 findings.
