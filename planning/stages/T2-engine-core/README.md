# Stage T2: engine core

Release: 0.3 Themeable. Depends on: T1 (anatomy and schema). Unblocks T3,
T4, T5, C. Status 2026-10-19: interview round 001 answered and the phased
build plan ([`PLAN.md`](PLAN.md)) accepted, revised to the decisions below;
E1 (#161) is revised to them, then E2 starts. T1's gate is recorded
(`results/2026-10-13-t1-anatomy-gate`). T1 hands T2 hit regions derived from
part geometry and text published as layout slots (T1 brief, "Hand-off").
Authority:
`planning/FORWARD_PROGRAMME.md` §4 T2.

Read [the shared brief](../README.md) first.

## Why

The owner wants "the *capability* for massive customization": early-2000s
media-player and audio-player skins "that were all kinds of funky", a subtle
sheen that follows the cursor "like the Windows title bar on Windows 7",
"buttons that pop", partial or total transparency, reflections, glass. Royale
and watercolour-style themes are "capability shaping, not examples we'd ship
with". The capability should not live in any one place, "but rather where it
belongs".

The owner described the engine as "an interpreter and itself a program, so
to speak": for example "subsetting a high speed high efficiency tiny
framebuffer right into the output window for a control, complete with
passthrough transparency — the developer just imports the control. The engine
is allowed to obfuscate the work that engines do. Themes involve data that
can program the engine, which is more than colours, but less than code."

The owner rejected a theme scripting language. ActionScript "was great", but
"we have some ability to just do that in C++ without overhead". Settled on
2026-10-12: "the escape hatch is the engine itself is C++ and laid out to make
it easy to extend correctly. Constrained to our house style rules, it's
capable of being closer to script than core." And: "the engine is the
machine, and the state machine is part specification part interaction. The
last part lives in the engine, the first in the CSS." Effects need "the
system window manager" only for behind-window work (T5); everything inside
the window is the engine's.

Why not scripts (the analysis the owner accepted): a script hatch brings back
idle frames (a timer loop is the perpetual redraw `AGENTS.md` forbids), puts
code in downloaded files, lets themes drift into behaviour (interview GF018),
makes the visual designer unable to edit themes, forces the engine to assume
anything can change at any time, and requires an interpreter, a garbage
collector and a language to version forever. "Declarative now, scripts later"
is a trap: every interesting theme would use the hatch and lose the
guarantees. The hatch is new first-party C++ primitives, versioned.

## The atom vocabulary (owner ruling, 2026-10-14)

The engine does not define what can be addressed, and neither do controls:
both declare against the vocabulary of part roles, states, value types and
engine inputs (design language §12.7). Each primitive registers which roles
it paints and which states and inputs it reads, in those words. New controls
are already painted through `paint_part`
(`gui_forms/theme/part_painting/part_painting.hpp`), which draws a part from
its role, states and the theme records; T2's engine is what that path calls
once T4 replaces its body, so the engine's entry point takes the same
description (control, part role, bounds, states, content).

## What exists today

- `SurfaceMaterial` (`include/gui_forms/surface_material/types/`): up to 8
  fill layers (solid, linear or radial gradient, image stretched, tiled or
  nine-patch), up to 4 outer or inset shadows, a border or per-edge borders,
  up to 8 single-edge keylines, one corner radius. Painted by
  `surface_material_painting.hpp`; exercised by the layered-material lab.
- Theme records (`theme_types.hpp`): 11 visual roles × 7 surface states of
  `ControlVisualRecipe`, hand-authored high-contrast recipes, spacing,
  geometry, typography, motion (`quick 100`, `standard 180`,
  `emphasized 280`, `busy_cycle 1400` ms) and structure tokens.
- `AnimationTimeline` with `EasingCurve`, `AnimationSpec` and `MotionPolicy`
  (`include/gui_forms/animation/animation_timeline/`); presentation-level
  `reduced_motion` (`include/gui_forms/window/presentation/`).
- The retained paint pipeline: display chunks, typed dirtiness, paint planes
  (the backplane/control-plane split of GF018–GF019), bounded damage, paint
  leases, presentation receipts, per-frame counters. Idle windows already
  schedule zero frames.
- `PaintFramebuffer` and live surfaces for CPU pixel work.

## What to deliver

1. **The cascade**, in this order: built-in recipe, theme sheet, class rule,
   stable-ID rule, inline style, forced high contrast. Anything unspecified or
   invalid falls back to the built-in recipe for that part.
2. **Layered materials and effect primitives.** At least what the design
   language requires (§3.5, §4.2–§4.6, §9.3): pill plate, bulge, dithered
   bulge, inset well, pressed, gold active plate, default perimeter,
   disabled, etched divider, seam, actuator, gloss (four-stop with a hard
   break), glow, sheen, pop, translucency, glass (bounded blur
   sampling only the backplane, radius at most 8 logical px, proposed),
   procedural materials (felt, wood grain, paper fibre, pinstripe, brushed
   metal, fresco with blooms) generated per display scale and cached.
   PlaySuite's gloss, lift and rings are capability requirements (§12.3),
   not defaults. Reflection moved to T5, beside full-fidelity glass (owner,
   2026-10-22).
3. **Inputs as engine-set custom properties**: pointer position relative to
   the part (`--pointer-x`, `--pointer-y`), hover, press, focus, value, time
   since state entry, display and text scale, density, accessibility flags,
   and named values the application publishes (for example `--level`).
   Nothing but the engine writes them; nothing in a theme writes back to the
   application.
4. **Presentational state machines** per part: states, transitions and
   timings declared in the theme, run by the engine, driven by the published
   states of T1. They cannot create observable states or delay behaviour.
5. **Settling primitives**: `follow`, `spring`, `decay`, for momentum and
   pop; each has a defined settled condition.
6. **Bounded expressions**: arithmetic, `clamp`, `min`, `max`, `mix`,
   `smoothstep`, comparisons, choose-one-of-two. No variables, loops,
   recursion or timers; a fixed operation budget; identical pixels for
   identical inputs.
7. **Per-part cost budgets** checked at load; a part whose cost cannot be
   bounded falls back to the built-in recipe.
8. **Optional per-part framebuffers** with passthrough transparency.
9. **Damage limited to parts whose inputs changed.** The engine knows
   statically which inputs each part reads.
10. **An extension seam**: a new primitive registers its name, parameters,
    cost bound and dependencies, follows the house style and arrives with a
    reference-image test. A theme declares the engine version it needs; on an
    older engine it falls back safely.

## How

- Motion rules (design language §9): reactive, direct, local, interruptible;
  every animation retargets from its current value. Durations: instant 0 ms
  press onset, quick 90–100, standard 150–180, emphasized 280, ceiling 400 ms
  (proposed). Easings: smoothstep `3t² − 2t³`, exponential approach (fixed
  fraction of the gap per about 75 ms), linear, ease, overshoot at most 8 %
  (proposed). Rejected: spatial stage movement, guiding paths, cross-fades and
  scale transitions, pulsing default actions.
- One light, upper left (§2.6). A cursor sheen is a reflection, never a
  moving light.
- Ambient effects declare region, period and frame cap: at most 30 fps,
  damage at most 10 % of the window on everyday surfaces (proposed); they
  stop when occluded or minimized, under reduced motion and when the
  application's motion switch is off. When nothing animates, nothing is
  scheduled or painted.
- Reduced motion (§9.5): transitions become immediate end states, ambient
  effects stop, sheen rests static, pop changes elevation without motion, an
  activity object holds a static pose.
- High contrast (§5.8): under the host's mode the host palette and metrics
  win; GUI.Forms' own mode is a near-colourless rebuild that keeps geometry
  and state composition and drops gradients, texture, glow, sheen,
  reflection, glass and cast shadows. Derive it; do not hand-author it per
  role as today.
- Two planes (§4.1): in-window glass, translucency and reflection sample the
  backplane only, never other controls; planes keep separate damage and cache
  generations.
- CPU only (interview GF002): every layer kind has a bounded cost and a
  declared damage extent. No GPU API may be compiled, linked or probed. Skia
  stays behind `src/render/skia/`.
- House style matters most here:
  - §3 kernels: choose primitive, format and implementation before the pixel
    loop; handle empty parts and edges apart from the interior; validate the
    destination buffer once; no allocation, logging, locking or callbacks per
    pixel.
  - §1, §3 numerics: one precision per primitive; no fast-math; identical
    output across hosts is a gate.
  - §5: part framebuffers and caches have one owner and an invalidation
    generation; reuse storage.
  - §7: the primitive registry is an explicit protocol, not a universal
    callable framework or a map of `std::function`.
  - §8: timers, settling and damage publication run on the UI thread;
    cancellation and shutdown are named.
  - §10: no lambdas, `auto` or `->`.
- The ribbon-button bulge and its dithered form are specified precisely in
  [`../T4-default-theme/RENDERING_REFERENCE.md`](../T4-default-theme/RENDERING_REFERENCE.md);
  implement them as primitives with reference images.

## Gate

The closing gate check (owner, 2026-10-22) also needs the CSS coverage
inventory, `CSS_COVERAGE.md`, to be reviewed. It lists every CSS visual
property T3 admits, with its engine carrier and its status, and proposes the
smallest engine addition for each missing row.

A reference-image test per primitive on all three hosts with identical
pixels; the window counters report zero frames once inputs stop and every
settling primitive has settled; a part whose cost cannot be bounded falls back
to the built-in recipe; reduced motion disables motion primitives. Record
under `results/` with the hash comparison per host.

## Owner decisions, interview round 001 (2026-10-19)

Recorded in `planning/interview/T2_INTERVIEW_ROUND_001.md`, answered on
`interview_round_001_source.html`.

### Scope and the gate
- **GIVEN A1-1.** Phase order: E1 and E2 (kernels and the composite) first,
  then E3 (motion).
- **GIVEN A2-2.** The identity gate hashes each primitive's raster bytes and
  the composite into the window through each host's painter.
- **GIVEN A3-1.** #159 and #161 are revised to these answers.

### Atoms
- **GIVEN B1-1.** The engine-input atoms:
  - `pointer_x` and `pointer_y` (published only while the part is hot);
  - `time_in_state`, `display_scale` and `text_scale`;
  - published application values (named, numeric, read-only).
- **GIVEN B2-1.** Pointer inputs are in part-local logical units.
- **GIVEN B3-1.** An application-set ceremonial flag on a surface lifts the
  10 % ambient cap for that surface only.
- **GIVEN B4-2.** No density atom until stage L or T4 needs it.

### Opacity and motion
- **GIVEN C1-1.** Part opacity is an engine presentational property driven by
  transitions, with no new atom.
- **GIVEN C2-1.** The built-in rules draw the docking tile fade once
  transitions exist (E7).
- **GIVEN D1-1.** Motion a control computes runs on control-owned motion
  values, on one clock per window. They replace the stamp's and the flyout's
  timers.
- **GIVEN D2-1.** Reduced motion follows §9.5:
  - transitions jump to their end;
  - ambient effects stop;
  - the activity object holds a static pose.

  `MotionPolicy::reduced` follows in T4.
- **GIVEN D3-1.** The ribbon rise is an exponential approach, a fixed
  fraction of the remaining gap per about 75 ms.
- **GIVEN D4-1.** In an occluded or minimized window, every transition jumps
  to its end and nothing is scheduled.
- **GIVEN D5-1.** A presentational scale is a capability only:
  - drawn about the part's centre, inside its granted overflow;
  - the hit region and layout are unchanged;
  - the default does not use it.

### Shadow, content colour and budgets
- **GIVEN E1-1.** The overlay shadow is an engine primitive with a
  first-party blur (E9), keeping today's values, outsets and high-contrast
  rule.
- **GIVEN E2-1.** Content colour is drawn exactly as given.
- **GIVEN F1-1.** Per-part cost is at most 512 declared operations per device
  pixel, and 4 million per frame for an animated part. Beyond that, the part
  falls back to the built-in recipe.
- **GIVEN F2-1.** Cached part rasters use at most 8 MB per window, evicting
  the least recently painted first.

### The rendering reference
- **GIVEN G1-3.** Dither quantization is rescaled to the part's tone range
  (rule in `RENDERING_REFERENCE.md` §3).
- **GIVEN G2-1.** The shade field keeps the approved formula; §3.5's wording
  is corrected.
- **GIVEN G3-3.** Dithered parts are drawn at the nearest integer scale (rule
  in `RENDERING_REFERENCE.md` §3).
- **GIVEN G4-1.** No hover lift. A ribbon button is flat at rest, and hot
  raises its bulge (D3-1); hover is the rise.

### Vocabulary
- **GIVEN H1-1.** An `image` part role: content pictures drawn exactly as
  given.
- **GIVEN H2-1.** A grip primitive fills its part's box with the dot pattern.
- **GIVEN H3-1.** A part's identity across frames is the control's stable
  ID, the declared part, and the item's stable ID. Items without one fall
  back to their index, and the fallback is reported.

### The plan
- **GIVEN Z1-1.** E1 to E11 are the T2 plan, revised by these decisions.

## Decisions taken in E3 (#201; they stand, coordinator, 2026-10-20)

Built in `include/gui_forms/engine/motion_clock/` and `motion_value/`. Where
the plan left a number or a form open, E3 chose:

1. **The follow time constant is 75 ms.** The gap shrinks by a factor of e
   each 75 ms, so 63 % of what remains is covered (D3-1 said only "a fixed
   fraction").
2. **The settled tolerance is half a device pixel** at the window's scale.
   A spring must also move less than half a device pixel over one 60 Hz
   frame.
3. **The easings:**
   - the timed exponential approach is (1 − e^(−5p)) / (1 − e^(−5));
   - ease is cubic ease-in-out;
   - overshoot is back-out with c = 1.5, whose single overshoot is exactly
     8 %.
4. **The 400 ms ceiling clamps every duration parameter**, the time
   constants of follow, spring and decay included. A settling value is not
   cut off at 400 ms; it ends when its settled condition holds.
5. **A spring's response is 1/ω**, the inverse of its natural angular
   frequency.
6. **The flyout slide stays linear**, so DK's behaviour is unchanged. The
   design language would give an opening surface smoothstep; that choice is
   DK D8's or T4's.

## Decisions taken in E7 (#216; reviewed by the owner, 2026-10-21)

1. **Opacity on parts the built-in recipe draws: changed by the owner.** E7
   first drew each of the recipe's commands at the part's opacity
   (`detail::OpacityPainter`), so overlapping commands showed through one
   another mid-fade. The owner, verbatim: "Opacity fading is a problem. But
   getting it wrong and going back and correcting it would be worse. Let's
   add a group layer instead." And: "there are other things that group
   layer may be needed for." So:
   - a part at partial opacity is drawn as one group, then composited once
     at that opacity through the shared ADR-004 blend;
   - the group is a general Painter facility, built in the recording, Skia
     and Windows DIB painters, for E9's translucency, glass, reflection and
     pop too;
   - it has an explicit lifetime and nesting, an opacity and a composite
     mode (source-over only for now), and bounds given up front so that its
     storage is sized and reused;
   - opacity 1 stays exact, and 0 draws nothing.
2. **A checked ribbon toggle and an open menu keep today's plate**, rather
   than the bulge: accepted.
3. **The rise covers ribbon command faces and collapsed group buttons** of
   every size (RB decision 23); not the tab row, the File tab, launchers or
   split arrows: accepted.
4. **A theme transition settles within half an 8-bit step of its
   property's range** (1/510 of it); the rise therefore runs 30 frames of
   16 ms: accepted.
5. **The ambient area cap is checked per effect, against the part's
   extent, when the part is drawn**; the frame cap when the sheet loads:
   accepted.
6. **The application's motion switch is `PresentationSettings::ambient_motion`**
   (default on). It stops ambient effects only; transitions follow reduced
   motion: accepted.
7. **Ceremonial is a `Control` property** (`set_ceremonial`), inherited by
   descendants: accepted.

## Decisions for E8c, the material primitive (owner direction, 2026-10-22)

1. **A refused material falls back.** The `material` primitive refuses a part
   whose material is missing or invalid, whose image fill is unavailable, or
   whose estimated bake exceeds the bound. The part is then drawn by the
   built-in recipe, and each refusal is counted once per part
   (`engine_primitive_refusals` in the metrics snapshot). The bound and the
   costs are set in one place, `material_bake_policy.hpp`.
2. **Image fills are supported, never refused for being images.** No stock
   theme uses them. Resampling is separable and done in premultiplied
   colour. When an image is shrunk, its kernel is widened by the shrink
   factor and the weights are normalized.
   - **The filter is decided (owner, 2026-10-22): stx_conv (CONV) prepares
     face sizes; nothing resamples inside a bake.** Built as its own PR
     after E9.2; see "CONV, image face variants" below.
     - *Preparation.* An image face's sized variant is produced by CONV
       through the internal seam `src/core/interpolation/interpolation.hpp`
       (`resample_image`, binary64, clamped to the valid premultiplied
       range). It samples at pixel centres when enlarging and averages
       pixel areas when shrinking, as the face study chose.
     - *Cache.* Variants are cached by (resource, target device size,
       scale) in the engine's shared cache. The engine owns them, and a
       theme or resource change invalidates them. They are produced in idle
       frames or once on first need, like the keyframes, and counted by
       their own counter.
     - *Bake.* The material bake only composites the prepared variant. The
       4 ms bound applies to the composite, not to resampling, so image
       faces are never refused for resampling cost.
     - *Filter.* Mitchell–Netravali and `MaterialResamplingFilter` are
       removed, since nothing would use them.
     - *Hashes.* The image-fill reference hashes are updated. A test shows
       that a variant is generated once and reused across bakes.
   - Nothing may be imported or copied from Plan Paint, because GUI.Forms
     depends on no sources outside its tree.
3. **CSS inline images are named by resource.** An image is registered
   once as a resource (a decoded PNG), and a material layer names it. This
   is not built yet.
4. **Bake cost and live cost are separate.** A material is baked once per
   invalidation (state, size, scale or theme) and cached. Compositing it
   afterwards is a blit, plus a group blend where one applies. The bake is
   bounded at 4 ms on the reference machine, a quarter of a 60 Hz frame.
   The owner has accepted that bound. Before E7r, the per-layer costs are
   to be measured again on a quiet machine, recording the load at the time
   of measurement.
5. **Nothing rasterizes per frame.** The material reads no inputs. The
   retained forms of E7's animated parameters, and slicing, are phase E7r
   (after E8c, before E9).

### E8c bake costs

Measured with `gui_forms_engine_material_tests --measure`. Each figure is
the median of nine bakes of a 240-unit square part at scale 1 (a 256 × 256
destination including the outsets), minus the base material's bake, divided
by the destination's pixels. The per-tap figure comes from a 64 × 64 image
magnified and a 1024 × 1024 image shrunk over the same box. The decode
figure comes from a 64 × 64 PNG.

The build was Release (clang 22, -O2) on Windows 11, on an AMD EPYC 9354
virtual machine with 8 logical CPUs. The runs were on 2026-10-22, while
other builds held the CPUs at 88–97 % load. There were seven runs, and the
table gives the minimum, median and maximum per pixel (per tap for
`image_tap`, per source pixel for `image_decode`), in nanoseconds:

| Cost | Min | Median | Max | Used |
|---|---|---|---|---|
| base (the square box) | 23.4 | 24.6 | 24.9 | 26 |
| base, rounded box | 24.3 | 26.0 | 27.6 | (in base) |
| solid fill | 7.5 | 9.2 | 22.8 | 10 |
| linear fill | 12.0 | 14.7 | 18.1 | 15 |
| radial fill | 17.3 | 21.1 | 29.0 | 22 |
| each gradient stop past two | 0.22 | 0.24 | 0.37 | 0.5 |
| outer shadow | 61.6 | 68.1 | 89.7 | 69 |
| inset shadow | 55.7 | 59.1 | 82.4 | 60 |
| border (rounded) | 22.4 | 25.4 | 39.1 | 26 |
| border edge | 9.9 | 10.7 | 18.5 | 11 |
| keyline | 7.4 | 12.3 | 17.2 | 13 |
| image composite | −2.5 | 5.1 | 15.9 | 6 |
| image tap | 3.4 | 3.9 | 4.3 | 4 |
| image decode | −96 | −3.9 | 81 | 10 |

The "used" column rounds each median up. The three image figures are below
the noise at this workload: single runs measured below zero. They therefore
carry conservative figures rather than measurements. The machine's load
makes every figure pessimistic. Since CONV (below) the bake only composites
a prepared variant: `image_tap` and `image_decode` are retired, and
`--measure` reports only the composite.

What the bound admits:

- A material with a shadow, a gradient, a border and a keyline costs about
  150 ns per device pixel, so 4 ms admits about 26 000 pixels. That is a
  100 × 24 button at scale 2, with its outsets.
- A plain solid region costs about 36 ns per pixel, so 4 ms admits about
  110 000 pixels, a region of roughly 320 × 320 at scale 1.
- Larger parts are refused and drawn by the built-in recipe until E7r's
  slice cache bakes a reference box once and composes larger parts from
  its corners, edges and centre.

## Decisions for E7r, the retained forms (owner, 2026-10-22)

Nothing in the engine bakes per frame. The forms are: (a) a state change
cross-fades its two baked faces through a group; (b) a moving scale stretches
the cached raster and bakes sharp at rest; (c) animated parameters step
through baked keyframes; (d) pointer and time inputs reuse those keyframes,
and ambient effects loop through a bounded set of baked frames. Slicing
assembles any size of a sliceable material from one reference bake.

- **A. When keyframes bake: in idle time, and on demand.** The 10 keyframes
  of an animated parameter bake in idle frames after the part first paints,
  never all at once in one frame (first paint included). A transition or a
  pointer move that reaches a keyframe not yet baked bakes only that one, the
  first time it is reached. Idle bakes and on-demand bakes are counted
  separately. The proof: once the idle bakes are done, a running transition
  bakes nothing; before that, it bakes at most one keyframe per keyframe it
  reaches for the first time.
- **B. Where they are held: a shared 4 MB cache** for slice references and
  keyframes, beside the 8 MB part cache.
- **C. Stepping: to the nearest keyframe** (10 levels evenly spaced over the
  parameter's declared range, the ends exact).
- **D. Pointer inputs: option 1.** An animated parameter driven by the
  pointer steps through the same 10 shared keyframes, so moving the pointer
  bakes nothing once they are baked. No overlay seam is built until a real
  primitive needs one.

### What E7r built

- **Slices.** `EnginePrimitive::slice_plan` declares, for each axis, whether
  it slices. A material plans this from its arcs, border, keylines, edges
  and shadows (3.5σ). An axis whose edges fall on fractional device pixels
  is kept whole. One reference bake per (material, state, scale) serves
  every size that slices, and the result is byte-identical to a whole bake.
  Counters: `slice_bakes`, `slice_hits`, `slice_whole_fallbacks`,
  `slice_axis_fallbacks`.
- **Shared bakes.** Slice references and keyframes live in one LRU cache of
  `shared_bake_budget_bytes` (4 MB), beside the 8 MB part cache. A theme
  change clears it.
- **(c) Keyframes.** `animated_keyframe_levels` is 10. While its transition
  runs, or while the pointer drives it, a parameter steps to the nearest
  level. The levels bake in idle frames: one keyframe per frame in which no
  motion runs and nothing waits to be painted. They are queued on the first
  paint and whenever the states change, at most `maximum_idle_keyframes`
  (64) at a time. A level reached before it is baked bakes on demand.
  Counters: `keyframe_idle_bakes`, `keyframe_demand_bakes`, `keyframe_hits`,
  `idle_keyframes_queued`.
- **(b) Scale.** While the scale moves, the part's raster at its own size
  is stretched about its centre through `Painter::draw_part_raster_scaled`,
  which samples bilinearly in the shared blend. At rest the part is baked
  sharp at its scaled size.
- **(a) Face cross-fade.** It is declared as the transition `face`, which
  has a curve and no states. When the part's states change and its face
  changes with them, the old face is drawn at 1 − t and the new one in a
  `PaintGroupComposite::plus` group at t, both inside one isolated group.
- **(d) Ambient effects.** The time is stepped to at most
  `maximum_ambient_loop_frames` (24) frames per period, each a shared bake.
  Pointer-driven parameters use the keyframes (D). No overlay seam exists.
- **Proof.** `gui_forms_engine_retained_tests` runs opacity, scale, face
  and an animated parameter together. Once the idle bakes are done, no
  frame bakes while they run. The ribbon rise and fall in
  `gui_forms_engine_motion_tests` bake nothing after the idle frames, and at
  most one keyframe per frame before them.

## E9, light

### Decisions for E9 (owner, 2026-10-22)

- **E9-A, option 1: an overlay seam for sheen only.** The face is baked
  once, and the sheen band once per (size, state, scale). The band is
  composited at the pointer offset, clipped to the part's shape. Damage is
  limited to the part's extent.
- **E9-B, option 1: a cached backplane image.** The window keeps one per
  requested region: the backplane plane alone, keyed to its damage
  generation, read through a target borrow. Glass blurs within the 8 px
  cap, and the cache follows E7r's rules.
  - **Who rasterizes it (owner, 2026-10-22, option 1).** The core bakes
    the backplane image from the window's backplane material and the
    backplane plane's engine part rasters. Text and other app-drawn
    backplane commands are absent from glass. The commands it skips are
    counted, so that it shows when glass omits something. Option 2, a host
    `render_backplane` capability that replays the whole backplane plane,
    is T5's upgrade behind the same borrow.
- **E9-C, option 1: no new primitives.** Gloss is a material preset. Glow is
  one new material layer kind, soft and additive, bounded by the grant. Pop
  is rule data only.

- **E9.6 (owner, 2026-10-22).** It is a T2 effects follow-up that covers
  the CSS coverage inventory's missing rows (`CSS_COVERAGE.md`) except
  reflection: rows 1 to 9 and 11 to 14.
  - `text-shadow` (row 5) is data on the text slot only. Its glyph
    coverage belongs to the text stage.
  - Reflection (row 10) moves to T5, beside full-fidelity glass, and is
    removed from E9.
  - It is built as separable PRs on main, each stacked at most two deep:
    fills and shape; filters and blend; finish, mask and position;
    motion.

### E9.1, the overlay shadow

- **The primitive.** `overlay_shadow` (GIVEN E1-1) draws a `shadow` part
  with the material kernel. The engine lends the theme's overlay shadow
  layers, or the high-contrast ones, as a material of one transparent fill
  and those outer shadows, at the part's corner radius (its own, else its
  family recipe's). A theme that casts none draws nothing. A built-in rule
  selects it for every shadow part, replacing `Painter::draw_box_shadow`, so
  the shadow has the same bytes on every host. Its outsets are 32 logical
  units. The damage outsets controls report are unchanged.
- **Refusals.** It refuses an inset shadow, a shadow that reaches past 32
  units (spread, offset and 3.5σ', plus two device pixels), and a bake over
  the bound. A refused shadow is drawn by `draw_box_shadow` and counted.
- **The kernel.** An outer shadow's rectangle integral is separable. The
  kernel evaluates it once per column and once per row. The rounded edge's
  integral is taken only within the shape's corner squares and outward
  from them. Elsewhere the rounded shape is its rectangle, and a blurred
  rectangle never exceeds the integral across its nearest edge. The bytes
  are unchanged: every material reference hash still matches, and so does
  the overlay shadow's. A solid fill of alpha 0 is not drawn, and the box's
  coverage is not computed when no layer reads it.
- **Proof.** `gui_forms_engine_light_tests` checks scales 1, 1.5 and 2. The
  engine composites one raster and no box shadow, and the raster equals a
  whole bake byte for byte, sliced or not. Its scale-1 hash is checked on
  every CI platform. High contrast casts nothing. A shadow too wide falls
  back, counted. `gui_forms_overlay_shadow_tests` sees the raster first in
  each tooltip's and context menu's operations.

### E9.2, the sheen behind the overlay seam

- **The seam (E9-A).** `EnginePrimitive::pointer_overlay` returns a
  `PointerOverlay`: the raster's logical size and its rest, a fraction of
  the part's width. The engine bakes that raster through the part's
  record, keyed like any other: once per (size, state, scale), never per
  pointer position. It composites the raster with its centre across at the
  part-local pointer and its top at the part's top. The raster is clipped
  to the part's rounded shape: the part's own radius, else its family
  recipe's. The part reads the pointer inputs for damage only, so a move
  while the part is hot damages the part's extent and bakes nothing.
- **Rest.** Without the pointer (the part is not hot), the band rests at
  `rest`. Under reduced motion it always rests, and the part does not read
  the pointer, so a move damages nothing (§9.5).
- **Scale.** An overlay follows the part's presentational scale, so that a
  popped or scaled part keeps its sheen aligned with its face. At rest it
  is baked at the scaled size and clipped to the scaled face with the
  part's own radius, as the face is baked. While the scale moves, the band
  at the part's own size is stretched by the same factor, top to bottom of
  the face, and the clip's radius scales with it. The move bakes nothing
  (E7r form b).
- **The sheen.** The `sheen` primitive is the seam's only user. Its band
  is `width` logical units wide (2 to 96, default 24) and the part's height
  tall. Its colour's alpha is weighted by (1 − t²)² across the band, with
  no transcendental function, and is uniform down the part. Its colour
  defaults to white at alpha 96. `rest` defaults to 0.25, towards the light.
  No built-in rule selects it (§9.3 item 5: no theme must carry it).
- **Proof.** `gui_forms_engine_light_tests`:
  - the band's bytes, hash 0xd4c2e5fcf3e31bf5 at scale 1;
  - the rest place, the rounded clip, and the band following the pointer;
  - a move within the part damages exactly the part and composites the same raster;
  - leaving the control returns the band to rest;
  - reduced motion, and scale 2;
  - a pop to 1.2: the band stretches with the face while the scale moves
    and bakes nothing, then rests baked at 29 × 36 device pixels, clipped
    to the popped face.

### E9.3, glass over the backplane

- **The borrow (E9-B).** A primitive that samples the backplane says so
  through `EnginePrimitive::backplane_margin`: the device pixels its kernel
  reads past the raster on every side. The engine then lends it
  `PrimitiveTarget::backplane`, an image of the raster's extent grown by
  that margin, with the backplane's damage generation and the image's
  top-left pixel in window device space. The borrow lasts for that paint
  only. A primitive that is lent no image, or one of another size, draws
  without it.
- **The image (owner, option 1).** `Window::engine_backplane` makes it on
  first need. It bakes the window's backplane material over the region with
  the material kernel (`bake_material_region`). It then composites the
  backplane plane's `draw_part_raster` and `draw_part_raster_scaled`
  commands over that, with nearest sampling, following saves, groups,
  translations and clips. Every other command (text, app drawing) is
  skipped and counted in `backplane_skipped_commands`. Option 2, a host
  `render_backplane` that replays the whole plane, is T5's upgrade behind
  the same borrow.
- **The cache.** The engine table holds images per region (device origin,
  extent and scale) at the window's backplane generation. An image from an
  older generation is dropped when next asked for. Images share the 4 MB
  budget and its least-recently-used order with shared bakes and image
  variants. Counters: `backplane_bakes`, `backplane_hits`,
  `backplane_images` and `backplane_skipped_commands`.
- **Damage.** Damage to the backplane plane advances the generation and
  damages every keyed part that sampled the backplane within it. A theme
  change and an activation change advance it and damage every such part.
  A part's record compares the generation, margin and origin, so moved or
  stale glass is baked again. Glass is never shared or keyframed, since
  its raster depends on where it lies. A part drawn in the backplane plane
  never samples it, so glass cannot read itself.
- **The kernel.** `glass` (roles region, face and well) blurs with three
  integer box passes per axis. Each box is w = 2h + 1 wide, the odd integer
  nearest √(4σ² + 1) for σ = blur × scale / 2, and each pass rounds once.
  The margin is 3h. Its tint (default white at alpha 64) is composited
  over the blur, and the result is clipped by the rounded-rect coverage:
  `corner` (0 to 64), or the part's own radius when it has one. `blur` is
  0 to 8 logical units, default 6. A rule asking for more is rejected when
  the theme compiles (`parameter_out_of_range`), and the default applies.
  No built-in rule selects glass.
- **Proof.** `gui_forms_engine_glass_tests`:
  - the kernel: the box sizes and margins, a uniform backplane unchanged,
    the tint alone without a backplane, blurred stripes, and a backplane of
    the wrong size unread; hash 0x748e797fedc763e5 at scale 1;
  - glass over a backplane-plane part raster shows it, and shows the
    window material elsewhere; one image is made and held, and the skipped
    commands are counted;
  - a control-plane cover under the glass leaves its bytes unchanged;
  - control-plane damage keeps the image, and backplane damage away from
    the glass bakes nothing; backplane damage under the glass makes its
    image again, and the glass shows the change;
  - a blur of 9 is rejected at compile and draws as the default blur.

### E9.4, the gloss preset and the glow layer (E9-C)

- **No new primitive.** Both are material data, baked by the `material`
  primitive and painted by `paint_surface_material` on the built-in path.
  No built-in theme carries either (§9.3 item 5).
- **Gloss.** `gloss_material(GlossColours, corner_radius)`, in
  `surface_material/presets/`, builds the §4.3 gloss from the ordinary
  layers:
  - one vertical gradient with stops at 0, 0.48, 0.52 and 1 (the hard
    break: `above_break` to `below_break` within 4 % of the face's height);
  - a 1-unit shine keyline under the top edge, inside the border, clipped
    to the corners;
  - and a 1-unit border.
  A corner radius outside [0, 4096] is refused (`std::invalid_argument`).
  Its vertical gradient slices across (E7r) like any other.
- **Glow.** `MaterialGlow{color, blur_radius, spread}`, at most
  `SurfaceMaterial::maximum_glows` (2) per material:
  - *The shape.* The box grown by `spread` and blurred by `blur_radius`
    (σ = blur / 2, as a shadow's), at no offset. Its corner radius is the
    box's plus the spread.
  - *Additive.* The engine bakes it with the outer shadow's separable
    kernel. It then adds its premultiplied colour to the outer shadows
    beneath it, each channel at most 1. The fills, inset shadows, border
    and keylines are drawn over it, so it is a halo that the face covers.
    `paint_surface_material` draws it as a shadow at no offset after the
    outer shadows. That differs from the bake only where a glow lies over
    a shadow, since adding light over nothing is compositing it.
  - *Bounded by the grant.* Its reach, spread + 2 × blur (four σ, where
    its blur is below one 8-bit step), is at most
    `SurfaceMaterial::maximum_glow_reach`, 8 logical units: the material's
    outsets, which a static assertion ties to it. A material whose glow
    reaches further is invalid, so the primitive refuses it and the theme
    rejects it. The built-in path's visual outsets include the reach.
  - *Cost.* Each glow is costed as an outer shadow (`outer_shadow`, and
    `outer_corner` at its rounded corners), since it runs the same kernel.
  - *Inspection.* The visual-inspection snapshot lists the glows.
- **Placement (owner, E9.4 follow-up).** `MaterialGlow::placement` is
  `under` (the halo above, for the current selection) or `over` (light for
  other emphasis, such as the default button):
  - *Over, in the bake.* Its coverage, the blurred grown box, is
    evaluated with the outer layers' cached integrals. It is then added
    over the finished face, after the keylines, times the box's coverage,
    so nothing outside the face changes.
  - *Over, on the built-in path.* `paint_surface_material` draws it last
    as a shadow at no offset, clipped to the face. That is source-over,
    not added.
  - *Both.* The same reach cap applies to both placements. An over glow
    adds no visual outsets.
  - *Stock rules.* The proposed stock rules for both are in the design
    language, §9.3 item 3, for the owner's review, and stock looks are
    unchanged. The owner admitted the default button's over glow beside its
    perimeter (2026-10-22). M105 now says glow is never the only signal.
  - *Proof.* `engine_material_tests`:
    - an under glow leaves the face unchanged and brightens a dark
      surround;
    - an over glow brightens the face and adds nothing outside it;
    - the cap and the placement's validity;
    - the built-in order;
    - byte-stable renders of both placements over a light and a dark
      background at scales 1 and 2 (eight hashes).
    An over glow in the layered slice material assembles to the whole
    bake's bytes.
- **Proof.** `gui_forms_engine_material_tests`:
  - the glow lies outside the box and under the face, and fades within
    its reach;
  - its alpha adds to a shadow's beneath it, and the shadow does not
    darken it;
  - a reach of 8 is valid and fills the outsets; a reach past 8, a
    negative blur and a third glow are invalid;
  - the built-in path draws the shadow, then the glow, then the fill;
  - the gloss's stops, shine and border, the step across the break and
    the shine under the top border;
  - reference hashes for both at scale 1 and 2.

  `gui_forms_engine_slice_tests`: a glow in the layered material, and the
  gloss, assemble from slices to the whole bake's bytes.

### E9.5, pop as rule data, and the proof (E9-C)

- **No engine change.** A pop (§9.3 item 1) is a theme's data alone:
  - the material primitive draws the face from the theme's recipe for the
    part's family, so the hot and selected recipes carry the risen look: a
    brighter fill, a deeper shadow and a glow (E9.4);
  - a `scale` transition grows the part while it is hot, selected or
    focus-visible, within the overflow its control grants (E8b);
  - a `face` cross-fade blends the resting and risen faces (E7r form a).
  No built-in rule pops anything.
- **Proof.** `gui_forms_engine_pop_tests` pops the middle of three gloss
  tiles to 1.08 within a grant of 4:
  - the theme compiles cleanly and the registry still holds 12
    primitives;
  - the risen face's shadow is deeper below it, and it glows above its top;
  - the change bakes at most the risen face and the face to stretch;
  - while the pop moves, nothing is baked; only the popped part is painted,
    within its grant and its material's outsets; the face is stretched
    towards the popped scale;
  - at rest it is baked sharp once at its popped size, and the window then
    runs no frames;
  - the neighbours keep their bounds;
  - selection pops it as hover does;
  - reduced motion jumps to the popped size without stretching;
  - a part granted no overflow never grows.
- **E9 as a whole.** Every light effect follows E7r's retained forms:
  - shadow, gloss, glow and glass are baked once per state, size and scale;
  - sheen composites one band baked once;
  - pop stretches cached rasters.
  Each effect's suite shows that nothing is baked while it moves or
  follows the pointer, and that damage stays within the part. The
  per-primitive reference hashes are:
  - overlay shadow 0x12c9cfd27f7ea098;
  - sheen band 0xd4c2e5fcf3e31bf5;
  - glass kernel 0x748e797fedc763e5;
  - glow 0x047bc7d0bde0012d and 0xda184a1654d76385;
  - gloss 0x4507eda3b5101fcb and 0xce445232e6045c6d.
  CI checks them on every platform. The quiet-machine re-measure of the
  bake costs is still open.

### E9 shadow costs

The outer shadow was measured again after the separable kernel, with
`gui_forms_engine_material_tests --measure`. A new workload, `outer_corner`,
measures the cost per pixel within a rounded shadow's corners. It uses a box
rounded to a circle, where every pixel is a corner pixel. The figure is net
of the separable cost.

These two figures are **provisional** until the quiet-machine re-measure,
which records the CPU load. The machine was the same, on 2026-10-22. The
load was not sampled. It was lighter than in the E8c runs: the inset
shadow, unchanged, measured 47.5–48.7 ns against 59.1 earlier. So each
median is scaled by 1.25 and rounded up. There were seven runs, in
nanoseconds per pixel:

| Cost | Min | Median | Max | Used |
|---|---|---|---|---|
| outer shadow (separable) | 4.53 | 4.83 | 5.24 | 7 |
| outer shadow, corner pixel | 20.98 | 21.71 | 22.02 | 28 |

The estimate counts every pixel at `outer_shadow`. It adds `outer_corner`
for each pixel within a rounded shadow's corner span on both axes: its
radius plus its reach (3.5σ'), and one pixel, on each side.

The overlay tooltip of 120 × 40 at scale 2 now bakes within the bound. It
was refused at 1.5 and 2 under the old per-pixel cost of 69 ns.

## CONV, image face variants

The owner's decision of 2026-10-22 (E8c item 2): an image face's sized
variant is prepared by CONV, and the bake only composites it.

- **The seam.** `EnginePrimitive::image_variants` names the variants a
  bake needs, one per image fill (`ImageVariantRequest`: the image, its
  source size, the variant's device extent and scale, and a nine-patch's
  source slices and fixed device lengths). The engine lends them in
  `PrimitiveTarget::variants` (`LentImageVariants`). A fill whose variant
  is not lent draws nothing.
- **What a variant is.** A stretched or nine-patch image's variant is the
  material box's size, rounded; a tile's is one tile. The material maps
  each destination pixel's centre to the variant pixel it falls on,
  proportionally within its nine-patch segment or its tile, so a variant
  of the box's size composites one to one.
- **Preparation.** `make_image_variant` (`src/core/engine/primitives/
  material/image_variant.cpp`) resamples each axis through the
  interpolation seam in binary64: `resample_line` with pixel-area sampling
  when the axis shrinks, `interpolate_line_at` at the pixel centres,
  (o + 0.5) × s / t − 0.5 clamped to the first and last pixel, when it
  enlarges, and a copy when it keeps its length. This is the face study's
  CONV centre composition; `resample_image` takes one sampling for both
  axes, so it is not used. The vertical axis goes first when the height
  shrinks. A nine-patch's nine regions are resampled apart. The colour is
  then clamped to alpha and quantized once.
- **The cache.** `EnginePartTable` holds the variants beside the shared
  bakes, matched on everything but the fill layer. Their bytes count in
  `shared_bytes` against `shared_bake_budget_bytes`, and eviction takes the
  least recently used of both lists. A theme change (`invalidate_rasters`)
  or a new revision of the window's images drops them. Counters:
  `image_variant_bakes`, `image_variant_hits`, `image_variants`.
- **When.** A variant is made on the first bake that needs it, a keyframe's
  idle bake included, so an idle keyframe prepares its variant in the idle
  frame. Otherwise it is made synchronously on first need.
- **Removed.** Mitchell–Netravali, `MaterialResamplingFilter`, the tap
  planner, and the `image_tap` and `image_decode` costs. The bake estimate
  counts only the composite (`image`, 6 ns per pixel).
- **Proof.** `gui_forms_engine_material_tests`: the image-fill reference
  hashes, updated (stretch, PNG equal to BGRA, tile, nine-patch, shrink);
  `image_variant_made_once`: two bakes make one variant and reuse it, the
  table's bake matches the primitive's byte for byte, a new image revision
  makes it again, and a theme change drops it; `image_variant_outside_the_bound`:
  a 1024 × 1024 image on a 240-unit part has the same estimate as a 4 × 4
  one, is accepted and bakes, its variant made on first need.

## E9.6, the effects follow-up (in progress)

The owner admitted rows 1 to 9 and 11 to 14 of `CSS_COVERAGE.md`'s "Missing
rows" (2026-10-22). They are built as separable PRs on main: fills and
shape; filters and blend; finish, mask and position; motion.

### E9.6 G1, fills and shape (rows 1 to 4)

- **Repeating radial gradients.** A radial fill takes the `repeat` and
  `reflect` spreads as a linear one does
  (`MaterialFillLayer::repeating_radial`).
- **Conic gradients.** `MaterialFillKind::conic_gradient`, with the
  factories `conic(center, from_degrees, stops)` and
  `repeating_conic(center, from_degrees, period_degrees, stops)`:
  - the stops sweep clockwise from up, past the from-angle
    (`angle_degrees`, as CSS `from`), over `period_degrees` (360, or less
    when repeating);
  - the angle is `engine_arctangent2`, a new deterministic `atan2` in
    `engine_math` (+, −, ×, ÷ and a 13-term series; within 4 × 10⁻¹⁵ of
    the C library, and the same bits on every host);
  - a period outside (0, 360] is invalid.
- **Per-corner radii.** `SurfaceMaterial::corner_radii`, four
  `MaterialCornerRadii` corners in logical units, each drawn at most half
  the shorter side. Every layer follows them: the box, the outer and inset
  shadows and the glows (each corner grown by the spread), and the
  border's inner edge. A part's own radius still overrides them. Single
  edges stay square-cornered: `corner_radii` with `border_edges` is
  invalid, as a non-zero `corner_radius` is.
- **The `shape` parameter.** The material primitive's second parameter,
  one of `part_shape_names` (default `rounded_rectangle`, which takes the
  material's radii). A pill, a circle or a capped end replaces the
  material's radii, and the part's hit region follows the shape
  (`EnginePrimitive::shape`). `PartShape` gains `per_corner` and `corners`,
  and `part_corner_radii` is public.
- **Dashed and dotted borders.** `MaterialBorder::style`, a
  `MaterialBorderStyle`:
  - a dash is three widths with a gap of two; a dot is one width across,
    one every two widths;
  - each runs along the border's centre line, clockwise from the end of
    the top-left corner, with its period stretched so that a whole number
    go round;
  - each is anti-aliased: a dash by the pixel's span along the line, a dot
    by its distance from the dot's centre, both within the solid ring.
  A dashed or dotted border varies all the way round, so it is never
  sliced (E7r).
- **The built-in path.** `paint_surface_material` has no conic gradient,
  no radial spread, no per-corner radii and no dashes. It draws a conic
  gradient as its first stop, a repeating radial gradient padded, the
  material's `corner_radius`, and every border solid. A theme that needs
  them selects the `material` primitive for the part.
- **Slicing reach clamps the radius.** A corner is drawn at most half the
  box's shorter side, so the slice reach takes the largest corner radius
  clamped to that half. Before, a uniform radius past it (say 32 on a part
  32 high) reached its full length and made the reference needlessly
  long. No pixel changes, and the slice tests prove the assembled bytes.
  A radius-32 part 120 × 32 at scale 2 of the layered test material now
  slices and is accepted (it was estimated at 4.35 ms and refused). The
  shadows' corner test still uses the unclamped radius, so such a part
  evaluates more corner pixels than a pill, and its estimate counts them.
  A layered part 120 × 64 at scale 1.5 (a pill or a radius of 32), at
  4.09 ms against the 4 ms bound, is the case to re-check after the
  quiet-machine re-measure.
- **A uniform material is unchanged.** A material without per-corner radii,
  drawn as a rounded rectangle, keeps its exact arithmetic: every reference
  image of `gui_forms_engine_material_tests` is unchanged.
- **Cost.** A conic fill and a dashed or dotted border each have a cost
  (`conic`, `border_pattern`); see "E9.6 G1 costs".
- **Inspection.** The snapshot reports `corner_radii`, a fill's
  `period_degrees`, `conic_gradient`, and a border's `style`.
- **Proof.** `gui_forms_engine_fill_shape_tests`:
  - the shape parameter's choices and hit regions;
  - repeating and reflected radial spreads past the radii;
  - a conic sweep's start, end, quarter turn, from-angle and repeat;
  - per-corner radii at each corner, four equal corners as one radius, and
    a part's own radius over them;
  - a pill as half-height corners, a circle and the caps;
  - dashes, gaps and dots along an edge, all within the solid ring;
  - validation of each, and reference hashes at scale 1 and 2.

  `gui_forms_engine_math_tests`: the arctangent against the C library and
  its table hash. `gui_forms_engine_slice_tests`: per-corner radii and a
  pill assemble from slices to the whole bake's bytes.

### E9.6 G1 costs

Measured with `gui_forms_engine_material_tests --measure` on the E8c
machine, 2026-10-22, with the CPU load sampled at 100 % (other builds).
The unchanged inset shadow measured 40.6–63.6 ns (median 56.9), as in the
E8c runs, so the medians are used unscaled, rounded up. Both are
**provisional** until the quiet-machine re-measure. Seven runs, in
nanoseconds per pixel:

| Cost | Min | Median | Max | Used |
|---|---|---|---|---|
| conic fill (2 stops) | 49.22 | 51.14 | 58.69 | 52 |
| dashed pattern, per ring pixel on an arc | 51.11 | 53.04 | 58.95 | 54 |

The pattern's workload is a border 64 wide on a box rounded to a circle,
so every ring pixel finds its place on an arc with the arctangent: the
dearest case. On a straight edge it costs about a tenth of that. The
estimate charges it per destination pixel, an upper bound.

### E9.6 G4, motion (rows 13 and 14)

- **Cubic Bézier easing.** `MotionCurve::cubic_bezier(duration, x1, y1,
  x2, y2)`, CSS's `cubic-bezier()`:
  - each x is clamped to [0, 1], so x rises with the curve's parameter;
    each y to [−8 %, 108 %], the overshoot bound; a coordinate that is not
    finite takes the straight curve's;
  - the parameter is found by 40 bisections (within 2⁻⁴⁰ on every host;
    CSS_COVERAGE proposed Newton steps, which can leave the interval
    where the slope is flat), and the velocity is y′/x′ there, 0 where x
    stands still;
  - `motion_curve_easing(curve, progress)` evaluates a curve's own
    easing; `motion_easing` keeps the closed set's.
- **Steps.** `MotionCurve::stepped(duration, steps, position)`, CSS's
  `steps(n, jump-end | jump-start)`: n clamped to [1, 32], the value
  floored to n levels, its velocity 0.
- **Delay.** `MotionCurve::delayed(delay)`, CSS's `transition-delay`: the
  value stands at its origin, still, then moves. Delay and duration
  together stay within the 400 ms ceiling (the delay is clamped to the
  room left). The delay is taken off in clock ticks, so a delayed move
  ends exactly at its delay plus its duration.
- **Multi-stop keyframes.** `MotionValue::animate_through(stops, target)`,
  up to `maximum_motion_stops` (4) `MotionStop`s (an offset strictly
  within (0, 1), increasing, and a value): a timed move passes each
  stop's value at its offset, each piece eased by the curve. In theme
  rules, `PresentationState::stops` play when the move into the state
  begins: on entering it from another state, or on appearing in it from
  `enter_from`. They need a timed curve and values within the property's
  range; otherwise the transition is `transition_invalid`. An animated
  parameter shows each stop at its nearest keyframe level, as any moving
  value does (E7r), so the stops bake nothing new.
- **Translation.** `translate_x` and `translate_y`, engine properties like
  scale (CSS `translate`): within ±32 logical units at load, at rest 0.
  The part is drawn moved, its raster composited at the offset, so a move
  bakes nothing; its hit region, layout and text slot stay put. The move
  is clamped on each side to the overflow its control grants, less what
  its scale as shown takes there, so a part granted none never moves; a
  part that translates records its extent grown by the grant, as one that
  scales does. Glass samples the backplane where the part is drawn; the
  sheen stays under the pointer.
- **Six transitions a part.** `maximum_part_transitions` is now 6:
  opacity, scale, face, the two translations, and a parameter, resolved
  in that order.
- **Unchanged bits.** A curve with no delay and a move with no stops
  evaluate as before: the motion clock's reference hash passes.
- **Proof.** `gui_forms_engine_motion_curve_tests`: CSS `ease` at half
  time, the straight Bézier, ease-in-out's monotony and symmetry, the
  clamps; steps at both positions and in motion; the delay held, then
  ending at 144 ms on 16 ms frames; a move through a stop, retained by
  `animate_to` the same target, and the refusals; a reference hash over
  the new curves; the rule checks and the six-transition order; a pad
  translated 8 across and 4 up with its hit region and layout unchanged;
  the grant's clamps, alone and after a scale; and stops played on
  entering hot, not on leaving.

## Open decisions (ask, do not decide)

The proposed numbers (light angle 135°, overshoot 8 %, blur cap 8 px,
ambient 30 fps over at most 10 % of the window, 400 ms ceiling) are accepted
as starting tokens and revisited after a native specimen sheet (§12.3). Ask
before changing them.

## Read first

- `planning/DESIGN_LANGUAGE_SPECIFICATION.md` §1.2, §2.6–§2.9, §3, §4, §5.8,
  §8.2, §9, §12.1 item 1, §12.3
- `planning/interview/GUI_FORMS_INTERVIEW_LEDGER.md` GF002, GF007,
  GF018–GF019
- `planning/interview/GUI_FORMS_RESOURCES_AND_CONFIGURATION.md` §1–§2
- `docs/user/painting-and-damage.md`, `docs/user/update-scopes-and-dirtiness.md`
- `planning/PAINT_PIPELINE_AVAILABILITY.md`
