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
   break), glow, sheen, pop, translucency, reflection, glass (bounded blur
   sampling only the backplane, radius at most 8 logical px, proposed),
   procedural materials (felt, wood grain, paper fibre, pinstripe, brushed
   metal, fresco with blooms) generated per display scale and cached.
   PlaySuite's gloss, lift and rings are capability requirements (§12.3),
   not defaults.
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
