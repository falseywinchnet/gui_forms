# Stage DP: damage-preserving painting

Release: 0.4 Flexible. Depends on: nothing; it can start now. It matters
more once the engine (T2) animates small parts such as a cursor sheen or an
ambient progress fill. Status 2026-10-12: planned in the paint-pipeline
records, not started. Authority: the "Damage-preserving painting" and
"Presentation availability record" rows of the milestone table in
`docs/dev/roadmap.md`, and `planning/PAINT_PIPELINE_AVAILABILITY.md`. The
forward programme has no section for this stage; this brief and the stage
index added it on 2026-10-12 to match the public roadmap.

Read [the shared brief](../README.md) first.

## Why

GUI.Forms is CPU-only (interview GF002), so the cost of a frame is the area
it paints. Today two small, distant damaged rectangles cost their whole
bounding box. That becomes a real cost once themes animate: a sheen that
follows the pointer on one button while a progress bar fills at the other
end of the window repaints everything between them. The design language
bounds ambient damage to about 10 % of the window (§9.4, proposed), which is
meaningless if presentation always takes the union.

## What exists today (`docs/user/painting-and-damage.md`)

- Each paint plane accumulates a `DamageRegion` of rectangles. It merges
  rectangles when their union fills the bounding box and collapses to the
  bounding box when its fixed capacity is exceeded. Counters:
  `damage_region_compactions`, `damage_region_collapses`,
  `maximum_damage_rectangles`.
- `Window::paint` accepts one rectangle. Without one it paints the bounding
  rectangle of accumulated damage, so gaps between damaged rectangles are
  repainted. The docs call preserving sparse regions "open work".
- Coherent latest-state presentation is done: one paint lease, one coalesced
  wake, exact presentation receipts, and failure restoring the previous
  chunks.
- An opt-in Windows transactional frame store exists for atomic candidate
  rasters; default hosts replay into the live raster.

## What to deliver

1. Painting each damage rectangle of the bounded set, not their union:
   display-chunk replay clipped per rectangle, and each chunk replayed once
   per frame even when several rectangles touch it.
2. Presenting the rectangle set where the host can consume one: the
   availability item "damage-preserving partial present" (the host consumes a
   bounded rectangle set instead of its union) reported as an optional host
   extension, never as a degraded mode.
3. The presentation availability record: the mandatory paint-contract
   version plus the optional extensions (partial present, zero-copy import,
   presentation fences) and the live diagnostic snapshot
   (`planning/PAINT_PIPELINE_AVAILABILITY.md`, M11h-P4 / M12-P4).
4. Counters that prove it: painted area against the bounding-box area per
   frame.

## How

- Keep every existing guarantee: one paint lease per window, latest-state
  rendering, exact receipts, previous chunks restored on owner-callback
  failure. A host that cannot meet coherent latest-state presentation fails
  attachment; it never advertises a degraded mode.
- Platform present paths stay in their host directories: AppKit, Win32 DIB,
  X11.
- House style §3: the per-rectangle replay chooses its clip and kernel before
  the loop, handles empty and degenerate rectangles first, and never
  allocates per rectangle; §5: the rectangle set has a fixed capacity, stated
  with its live length.

## Gate

Proposed, for owner review: with two distant damaged rectangles, the painted
area equals their sum (not their bounding box) on every host, and pixels
outside them are untouched; hosts that support partial present receive the
rectangle set; the focused model tests, renderer fault tests and host probes
of `PAINT_PIPELINE_AVAILABILITY.md` "Acceptance" still pass. Record under
`results/`.

## Open decisions (ask, do not decide)

1. This stage's gate above is a proposal.

## Read first

- `docs/user/painting-and-damage.md`
- `planning/PAINT_PIPELINE_AVAILABILITY.md`
- `planning/interview/GUI_FORMS_INTERVIEW_LEDGER.md` GF002
- `experiments/M12P3_*` presentation pressure reports
