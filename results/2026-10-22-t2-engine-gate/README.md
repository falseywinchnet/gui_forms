# T2 engine gate evidence (E11)

T2's gate, from `planning/stages/T2-engine-core/README.md` ("Gate"), has four
parts:

1. a reference image per primitive on every host, with identical pixels;
2. zero frames in the window counters once inputs stop and every settling
   primitive has settled;
3. a part whose cost cannot be bounded falls back to the built-in recipe;
4. reduced motion disables motion primitives.

This directory holds the template that CI fills (`record.template.md`) and
an interim record (`interim/`). The sealed record (`record.md` and
`comparison.json`) will come from a run on main after the last T2 primitive
lands, in its own pull request.

## Interim record, 12 primitives, run 39910508309

`interim/record.md` and `interim/comparison.json` come from CI run
39910508309, on #280's test merge commit `5a87ab47` (PR head `9422ea57`
merged onto main `773e54b6`). All four hosts ran: 12 primitives and 144
images, each with the same hash on every host, and every idle counter at 0.

It is not the gate's seal. The gate covers every primitive, and E9.6
(G1-G4) and E10 were still adding features then. Those features are layers
and parameters of the registered primitives rather than primitives, so the
gate now draws them through the feature samples too (below); the sealing
run follows.

## How the evidence is made

`tests/engine_gate_tests.cpp` (CTest `gui_forms_engine_gate_tests`) covers
parts 1 and 2 for every primitive in `builtin_primitive_registry()`. It lists
none by hand, so a primitive registered later is covered from its
registration on.

- **Reference images (part 1).** Each primitive is declared by a theme rule
  on a 96 by 28 part of its first role, radius 4, under the professional
  theme. It is drawn through the window at scales 1, 1.25, 1.5 and 2, at
  rest, hot and pressed. The hash is FNV-1a over the extent and bytes of
  every part raster composited. A second window must draw the same bytes.
  The manifest also records the window's other drawing calls and the
  engine's refusals.
- **Feature samples (part 1, the features).** Most of the engine's
  features are layers and parameters of a primitive, not primitives, so a
  primitive drawn with its defaults does not show them: conic and repeating
  fills, per-corner radii and shapes, dashed and dotted borders, colour
  filters, part blur, drop shadows, blends, the finish, masks, image
  position, the six procedural patterns, glow placements, glass's colour
  filter, the sheen at the pointer, and motion. Each sample in
  `engine_samples::builtin_feature_samples()`
  (`src/core/engine/feature_samples/`) is a theme that draws the same part
  with some of them; the test draws every listed sample, as it draws every
  registered primitive, at the four scales:
  - at rest, once the window has settled (procedural patterns generated,
    idle keyframes baked);
  - a pointer sample also with its part hot and the pointer over it;
  - a motion sample also 32, 80, 144 and 224 ms after its part turns hot,
    and once it has settled.

  A sample image's hash covers the part rasters and every command the window
  gave the painter, with its geometry, opacity, colour and paint group, so
  that a scale, a translation, a cross-fade or a blend shows in it. The test
  also requires of each sample: its theme compiles cleanly; nothing is
  refused, every image mask is drawn and every procedural pattern is
  generated; every image composites a part raster; a driven sample's images
  show its drive; a cross-fade shows both faces in some frame; no two samples
  draw alike at a scale; a second window draws the same images; and its
  window, once settled, runs no frame.
- **The census.** The test reads the themes the samples write and requires
  every feature of the engine's vocabulary in at least one sample: every
  value of the fill kinds, spreads, image modes, coordinate spaces, linear
  geometries, procedural patterns, fill and part blends, shapes, border
  styles, glow placements, keyline edges, presentational properties, curve
  kinds, easings and step positions, and the features that are no
  enumeration (fill opacity, image position, border edges, corner radii,
  outer and inset shadows, colour filter, drop shadow, part blur, finish,
  solid, gradient and image masks, glass's colour filter, the sheen at the
  pointer, motion delay and keyframe stops). The names are switches built
  with `-Werror=switch`: a value added to one of those enumerations fails
  the build until it is named, and the test until a sample draws it. A
  fling's decay curve is no transition's curve and is left to its own tests.
- **Idle frames (part 2).** After each primitive's window has settled,
  including its idle keyframe bakes, 125 polls of 16 ms (two seconds) must
  show:
  - no work in the poll and no damage;
  - no new `scheduled_frame_requests`, `paint_passes`,
    `motion_clock_ticks` or `engine_ambient_frames`.

  The same is checked for each feature sample's window, and for the ribbon
  after its rise has been driven on, off, on and off again, while it was
  still moving.
- **Reduced motion, on the ribbon (part 4).** Under reduced motion, hovering
  the ribbon runs no motion clock tick, and the rise jumps to its end.

With `GUI_FORMS_ENGINE_GATE_MANIFEST=<file>` the test writes a manifest
(`engine-gate-<platform>.tsv`):
- `primitive` rows: name, state, scale, rasters, other drawing calls,
  refusals, hash;
- a `samples` row (how many are listed) and `sample` rows: name, primitive,
  frame, scale, rasters, other drawing calls, refusals, hash;
- `feature` rows: each feature of the census and the first sample drawing
  it;
- `counter` rows: the idle and reduced-motion counters.

Each native CI job sets that variable, and its evidence artifact
(`gui-forms-evidence-<platform>`) carries the file. The `engine-gate` job in
`native.yml` downloads the four artifacts and runs
`python tools/engine_gate.py compare`. That fails on any hash that differs
between hosts, any primitive, sample or feature missing on one host, or any
non-zero idle counter, and uploads the filled record as the
`t2-engine-gate-record` artifact.

A native job that promoted (a push to main) or reused an earlier
validation of the same tree runs no test. It carries the validation run's
manifest instead (`engine_gate.py carry`), marked with a comment naming that
run. A host with no manifest at all, or no manifest found anywhere, fails
the comparison: a gate that compares fewer hosts than it covers proves
nothing about the missing ones.

## Parts 3 and 4: the primitives' own tests

These tests run on every CI platform in the native and sanitizer jobs. The
gate record lists them; it does not repeat them.

**Cost fallback (part 3).** A refused part is drawn by the built-in recipe
and counted in `engine_primitive_refusals`.

| Test | Function | What it proves |
|---|---|---|
| `gui_forms_engine_material_tests` | `engine_counts_refusals` | A 3000 by 3000 material, whose bake would exceed its bound, is refused. It composites no raster, falls back to the built-in recipe, and the refusal is counted once, when it begins. |
| `gui_forms_engine_material_tests` | `refusals` | The material primitive refuses targets and parameters it cannot bound. |
| `gui_forms_engine_light_tests` | `too_wide_falls_back` | An overlay shadow reaching past the primitive's outsets is drawn by the host's blur (`Painter::draw_box_shadow`), counted. |
| `gui_forms_engine_glass_tests` | `over_the_cap_falls_back` | A glass blur past the 8 px cap is a load diagnostic, and the part falls back to the default. |
| `gui_forms_engine_motion_tests` | `ambient_effects` | An ambient effect over 10 % of the window, or over its frame cap, is refused, and the part rests at its pose. |

**Reduced motion (part 4).**

| Test | Function | What it proves |
|---|---|---|
| `gui_forms_engine_motion_tests` | `ribbon_without_motion` | The ribbon's rise is at its end at once, and nothing is scheduled. |
| `gui_forms_engine_motion_tests` | `tile_fade` | A docking tile's fade shows opaque at once. |
| `gui_forms_engine_motion_tests` | `ambient_effects` | An ambient effect stops, holds its static pose, and resumes when reduced motion is turned off. |
| `gui_forms_engine_light_tests` | `sheen_rests_under_reduced_motion` | The sheen rests, and a pointer move within the part damages nothing. |
| `gui_forms_engine_gate_tests` | `ribbon_without_motion` | Hovering runs no motion clock tick, and the rise jumps to its end (counted in the manifest). |

## Not covered here

- **Composite through each host's painter.** The A2-2 identity gate also
  hashes the composite into the window through each host's painter. The
  manifest hashes the engine's part rasters, the bytes every host
  composites; the host painters' composites are compared by the host
  painter tests, not here.
- **The specimen sheet.** E11's native specimen sheet of every primitive for
  the owner is a separate deliverable.
