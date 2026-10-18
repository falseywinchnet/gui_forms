# Stage T2: engine core

Release: 0.3 Themeable. Depends on: T1 (anatomy and schema). Unblocks T3,
T4, T5, C. Status 2026-10-18: started; the phased build plan
([`PLAN.md`](PLAN.md)) is proposed and awaits the owner's acceptance. T1's
gate is recorded (`results/2026-10-13-t1-anatomy-gate`). T1 hands T2 hit regions derived from
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
