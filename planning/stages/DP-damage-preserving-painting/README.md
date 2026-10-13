# Stage DP: damage-preserving painting

Release: 0.4 Flexible. Depends on: nothing; it can start now. It matters
more once the engine (T2) animates small parts such as a cursor sheen or an
ambient progress fill. Status 2026-10-13: rectangle-set paint on every host
(#59, #60) and presentation counters (#71) merged; the Windows live frame
sequence (#72) confirmed on screen by the owner; the availability record and
transactional partial present on Windows on `claude/dp-availability-record`.
Partial present is offered by both Windows profiles and by macOS; the X11
frame sequence (live frames as damage, rectangle-set present, a 16 ms live
clock) is on `claude/dp-x11-frame-sequence`, awaiting CI and PlaySuite's Lima
runs. Decision 3's amendment of `PAINT_PIPELINE_AVAILABILITY.md` follows once
X11 is verified. Authority: the "Damage-preserving painting" and
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

Accepted by the owner on 2026-10-13: with two distant damaged rectangles, the painted
area equals their sum (not their bounding box) on every host, and pixels
outside them are untouched; hosts that support partial present receive the
rectangle set; the focused model tests, renderer fault tests and host probes
of `PAINT_PIPELINE_AVAILABILITY.md` "Acceptance" still pass. Record under
`results/`.

## Owner decisions, 2026-10-13

1. The gate above is accepted.
2. **One frame sequence on every host** (owner, with PlaySuite, 2026-10-13:
   "i want to use one approach on all platforms"). The core defines it once
   and every host follows it:
   - *gather*: one bounded rectangle set from the model's damage, live
     frames' damage (with the overlay shares of #69) and whatever the
     operating system exposed; native regions are inputs, never
     replacements;
   - *compose*: retained content and live frames painted into the host's
     CPU raster in z-order, overlays over the frame in the same pass;
   - *present*: exactly that rectangle set copied from the raster to the
     screen.
   Hosts supply only "copy these rectangles" and "this was exposed". The
   screen is then always a copy of the raster, so frame captures are
   evidence of what users see. This removes the class of bug PlaySuite
   chased this week (raster right, screen wrong, on one host), instead of
   patching instances.
3. **Partial present becomes mandatory** once the transactional Windows
   profile presents rectangle sets correctly (unchanged pixels carried into
   the candidate, only the rectangles presented, the all-or-nothing swap
   kept). Until then it stays an optional extension, and
   `PAINT_PIPELINE_AVAILABILITY.md` is amended when it changes. Done
   2026-10-13: the transactional profile (#73) and X11 (#76) present
   rectangle sets, the document is amended, and `paint_contract_version` is 2;
   a host that claims it without partial present cannot attach.
4. **Measure before settling the copy cost.** The cost in question is a
   second copy of live frames, which scales with each frame's changed area.
   PlaySuite measures Stillwater, Mowing and Maze 95 before and after with
   `tools/measure_frames.py` (falseywinchnet/games) on macOS, Windows and
   X11 (a Lima guest on the M4), at scale 1 and on a Retina display. A
   finding while adding the counters: the Windows and macOS hosts already
   draw live frames into their raster before copying them to the screen,
   so that second copy exists there today.
5. **Counters first**: every host reports what it copies to the screen,
   retained and live apart (`retained_pixels_presented`,
   `retained_rectangles_presented`, `live_frames_presented`,
   `live_pixels_presented`, `live_rectangles_presented`,
   `live_present_duration_nanoseconds`), so the comparison is a number. The
   availability record follows, so PlaySuite can check per host whether
   sparse damage reaches the screen.

## Read first

- `docs/user/painting-and-damage.md`
- `planning/PAINT_PIPELINE_AVAILABILITY.md`
- `planning/interview/GUI_FORMS_INTERVIEW_LEDGER.md` GF002
- `experiments/M12P3_*` presentation pressure reports
