# Engine core: plan

Date: 2026-10-18, revised 2026-10-19. Status: **accepted** (GIVEN Z1-1),
revised to the owner's answers in `planning/interview/T2_INTERVIEW_ROUND_001.md`.
The GIVEN list is in `README.md`, and each decision is cited here by its ID.
Sources:
- the T2 brief (`README.md`) and `planning/FORWARD_PROGRAMME.md` §4 T2;
- the design language (`planning/DESIGN_LANGUAGE_SPECIFICATION.md`): §1.2, §2.6–§2.9, §3, §4, §5.8, §8, §9, §12.3 and §12.7;
- the rendering reference (`../T4-default-theme/RENDERING_REFERENCE.md`);
- interview GF002, GF007 and GF018–GF019;
- the paint pipeline (`docs/user/painting-and-damage.md`, `planning/PAINT_PIPELINE_AVAILABILITY.md`);
- the consumers already waiting for motion: DK's plan (stamp, tile fade, flyout slide) and RB's decisions 15 and 23 (the dithered rise).

Where this plan disagrees with a recorded decision, the decision governs, and
this plan is fixed to match. The starting tokens accepted for T2 are used as
given and are not changed here:
- light angle 135°;
- overshoot at most 8 %;
- blur radius at most 8 logical px;
- ambient effects at most 30 fps, over at most 10 % of the window;
- a 400 ms ceiling on any transition.

## What the engine is

The engine is the C++ machine that themes program. Theme data selects
primitives, sets their parameters, and declares transitions. The engine:
- resolves that data for each part;
- rasterizes the part with first-party kernels;
- composites the result into the window;
- runs the part's motion;
- reports which parts must be repainted when an input changes.

Controls never call the engine directly. They paint every part through
`paint_part`, and `paint_part` calls the engine.

| Type | Kind | What it holds |
|---|---|---|
| `PartRaster`, `PartRasterView` | engine storage | Premultiplied RGBA8 pixels in device pixels, with a stated row stride. A view borrows caller-supplied storage. A raster held in the cache has one owner and an invalidation generation. |
| `EnginePrimitive` | protocol (virtual) | One kind of effect. It declares its name, the engine version that introduced it, its parameters, the part roles it paints, the states and inputs it reads, its cost bound and its damage extent. It rasterizes into a view. |
| `PrimitiveRegistry` | fixed table | The registered primitives, looked up by name. Registration is explicit. A duplicate or invalid descriptor is refused, and the refusal changes nothing. |
| `PrimitiveParameterBlock` | value record | One part's parameter values, checked against the primitive's declarations before any pixel is touched. |
| `ThemeEngine` | component, one per application theme | The rule tables (the cascade), the registry, and the derived high-contrast pass. It is the entry point that `paint_part` calls. |
| `EnginePartTable` | per window | One record per painted part, keyed by `PartKey`. Each record holds: the part's last states and the time it entered each one; its active transitions; the inputs its resolved style reads; and its cached raster. |
| `MotionClock` | per window | One frame request, armed only while some transition or motion value is unsettled. Both theme-owned transitions and control-owned motion values are sampled from it. |
| `MotionValue` | value owned by a control | A settling or timed value (follow, spring, decay, or eased over a duration). A control drives presentational motion with it (a stamp's size, a flyout's offset) in place of its own timer. |
| `EngineExpression` | bounded program | The bounded expressions of the brief, compiled from theme data (T3 parses the syntax). It has a fixed operation budget and statically known inputs. |

## What `paint_part` hands to the engine

Today `PartPaint` carries:
- the part's role, bounds and states;
- the recipe family;
- the mark, text, font and alignment;
- the image;
- an optional corner radius;
- an optional accent.

The content colour joins these once #153 merges (W decision 7). The engine's
entry point takes the same description:

```cpp
void ThemeEngine::paint(Painter& painter, const Control& control, const PartPaint& part);
```

From `control` the engine reads:
- what selectors address: the control's type id, its stable ID and its classes;
- the presentation settings: display scale, text scale, high contrast, reduced motion and participation.

T2 adds one optional field to `PartPaint`. It is identity, not vocabulary:

- `std::optional<PartKey> key`. It is the control's stable ID, the declared part, and for an item part the item's stable ID (GIVEN H3-1). An item without a stable ID falls back to its index; the fallback is counted in the window's metrics and reported, because motion then follows the position rather than the item.

A part painted without a key is drawn statically. It gets no transitions, no
time since state entry and no cached raster. Every existing control therefore
keeps working unchanged, and each control gains motion when it passes its keys.
That is a one-line change per call site, made as each control moves onto the
engine (T4).

`paint_part` keeps its signature and its fallback: the body it has today
becomes the **built-in recipe**. The engine draws a part only when a rule
selects a registered primitive for it, the parameters validate, and the cost is
within budget. In every other case the built-in recipe draws the part. Until a
theme sheet exists, nothing selects a primitive, so pixels stay exactly as they
are today. This keeps the 2026-10-13 decision that there is no interim default
before T4.

## How the vocabulary atoms are consumed

The engine declares against the vocabulary and never defines it (§12.7):
- A `PrimitiveDescriptor` names the `PartRole`s it paints, the `AnatomyState`s it reads, the value atoms it reads (mark, accent, content colour) and the engine inputs it reads (E6, GIVEN B1-1). Because these are the vocabulary's own enums, a primitive cannot name an atom that does not exist.
- A rule that binds a primitive to a part whose role the primitive does not paint is refused at load, and the part keeps the built-in recipe.
- The schema export gains an `engine` section listing every primitive with its atoms, parameters and version, beside `vocabulary`. `tools/control_schema.py check` covers it.
- **Atoms admitted by the owner (§12.12), added where their phase needs them:**
  - the engine inputs (GIVEN B1-1, B2-1), in E6: `pointer_x` and `pointer_y` in part-local logical units (published only while the part is hot), `time_in_state`, `display_scale`, `text_scale`, and published application values (named, numeric, read-only);
  - the `ceremonial` surface flag (GIVEN B3-1), in E7;
  - the `image` part role (GIVEN H1-1), in E2: a content picture drawn exactly as given, never tinted or resized by a theme;
  - the grip primitive (GIVEN H2-1), in E8: it fills its part's box with the dot pattern, so NavigationPane's grip becomes one part.
- **Not admitted:** density, which waits for stage L or T4 (GIVEN B4-2). Part opacity is an engine property, not an atom (GIVEN C1-1).

## How motion is driven

There are two drivers, and they share one clock per window.

1. **Theme-owned transitions** (E7) run on part records. A theme, or the
   built-in rule table before T3, declares a transition on a property for a
   part's states. Examples:
   - the tile fade: the `actuator` tile's opacity, from 0 to 1 over *quick* when the part appears;
   - the ribbon's dithered rise: the bulge's `amplitude`, from 0 at rest to 1 while `hot`, by exponential approach, a fixed fraction of the remaining gap per about 75 ms (GIVEN D3-1). A ribbon button is flat at rest, and hover is the rise: there is no separate brightness lift (GIVEN G4-1).

   When a part's published states change, the engine starts the transition
   from the value currently shown. Every transition retargets in this way and
   never restarts. Nothing in the control changes: the control publishes its
   states, as it already does.
2. **Control-owned motion values** (E3) are for motion that is presentational
   but computed by the control:
   - DK D3's stamp, which shrinks to about 100 units in about 220 ms and settles into its target;
   - D5's flyout, which slides for about 180 ms.

   Each of these runs its own `activate_surface` or `schedule_ui_timer` loop
   today, with a hand-rolled smoothstep (`dock_site_drag.cpp`,
   `dock_site_auto_hide.cpp`, in #144 and #145). From E3 the control owns a `MotionValue`, sets
   its target, and reads its current value during layout and paint. The
   window's `MotionClock` advances every unsettled value, invalidates what each
   value's owner declared, and disconnects its single frame request once
   everything has settled.

Shared rules:
- Every duration is clamped to the 400 ms ceiling.
- Easings are smoothstep, exponential approach (a fixed fraction of the gap per about 75 ms), linear, ease, and overshoot of at most 8 %.
- Each settling primitive has a stated settled condition. When the condition is met, the value snaps to its exact target, so the last frame is byte-identical to the static rendering.
- Under reduced motion (§9.5, GIVEN D2-1), every transition and motion value jumps to its end in the same frame, ambient effects stop, and an activity object holds a static pose beside its stage text. Today's `MotionPolicy::reduced` (0.35 speed, half the excursion) moves to this in T4.
- While a window is occluded or minimized, its clock jumps everything to the end and schedules nothing (GIVEN D4-1).
- A presentational scale is a capability, never a default (GIVEN D5-1). It is drawn about the part's centre, inside its granted overflow, with the hit region and layout unchanged.
- Timers, settling and damage publication run on the UI thread. The clock is cancelled by name when a window closes. A record whose control has gone is rejected by its generation.

**The opacity owed to the docking tiles** (GIVEN C1-1, C2-1). D3 (#144)
publishes the tiles' 100 ms fade as `expanding` but cannot draw it, because
`PartPaint` has no opacity. Opacity is not something a control supplies:
- It is an engine presentational property: one multiplier on the part's whole composite, applied in the shared blend (E2).
- A transition drives it (E7), keyed on the part's appearance. That appearance is a new `PartKey` in the window's part table, or the published `expanding` state.
- D3 changes only to pass its tiles' keys.

The built-in rules draw the fade once transitions exist (E7), before T4.

## Numerics and identical pixels

The gate asks for identical pixels on every host. CI builds and tests on four
platforms: macOS arm64, Linux x64, Linux arm64 and Windows x64, all with the
pinned LLVM 22. Identity is designed in, not hoped for:

- **One precision.** Every kernel computes in `double` and quantizes once, at the end, to 8-bit premultiplied values with `floor(v·255 + 0.5)`. There is no fast-math.
- **No fused multiply-add.** Clang contracts `a·b + c` into an FMA by default on arm64, but not on baseline x86-64. Every engine kernel source turns contraction off with `#pragma clang fp contract(off)`, and arithmetic kernels live in `.cpp` files, not in headers that other translation units would compile under their own flags.
- **No host libm in kernels.** `pow`, `exp`, `log` and the trigonometric functions differ between macOS libm, glibc and the Windows runtime. The engine has its own `power`, `exponential` and `logarithm`. They are built only from `+ − × ÷` and `sqrt` (IEEE-exact) and from `frexp` and `ldexp` (exact), each with a stated error bound.
- **One blend.** Part rasters composite by premultiplied source-over, using the arithmetic that ADR-004 already made bit-identical across the Skia raster and the Windows DIB compositor for text. A composite test compares the bytes on every host, as `text_identity_raster_tests` does. The identity gate covers both the primitive's raster bytes and this composite through each host's painter (GIVEN A2-2).
- **The reference images** are FNV-1a hashes of each primitive's raster for a fixed set of parameters and display scales. The expected table is checked on all four platforms. Setting `GUI_FORMS_ENGINE_IDENTITY_PRINT=1` prints the observed hashes instead of checking them.

The built-in recipe (today's `Painter` path) is not an engine primitive, and
the identity gate does not cover it. It is replaced in T4.

## Phases

Each phase ends with tests in `tests/` that CI runs on every platform, and its
own pull request (or stack).

**E1. Deterministic kernels, the primitive registry and the bulge pair.**
- `PartRasterView` over caller-supplied storage, validated once before any pixel is written. Size arithmetic is checked, and a short buffer or an odd stride is refused with nothing written.
- The engine's deterministic `power`, `exponential` and `logarithm`.
- The registry protocol: descriptors, parameter declarations, parameter blocks, validation, and refusals that change nothing.
- The first two primitives, from the rendering reference §2 and §3:
  - `bulge`, the smooth form;
  - `dithered_bulge`.

  Each has its dent when `pressed` and its gold form for `active_tool`. Each has an `amplitude` parameter from 0 to 1, which is the input the ribbon's rise animates in E7.
- **Rescaled quantization** (GIVEN G1-3, rendering reference §3): each side of the dithered tone is divided by the part's own full-amplitude extreme (P for the lit side, N for the shaded side), computed once per part geometry, so every step is used whatever the gains, and the rise still grows the dither.
- **Fractional display scales** (GIVEN G3-3, rendering reference §3): a dithered part is drawn at its nearest integer scale k = max(1, floor(s + 0.5)), on a lattice of period 2k device px with k × k dots, sampling the fields at logical (2n + 1)·k / s. At integer scales this is the studies' rule exactly.
- **The shade field** keeps the reference formula, as the approved studies drew it (GIVEN G2-1).
- Nothing is wired into `paint_part` yet, and no control's pixels change.

Tests:
- registry refusals;
- parameter refusals;
- destination validation;
- amplitude 0 paints nothing, and the dot count grows with amplitude;
- dots only on the lattice;
- all three tones on each side, for the light default too (G1-3);
- the integer-scale lattice equals the old rule at scales 1 and 2 (G3-3);
- the dent mirrors the bulge;
- the cross-host hash table at scales 1, 1.25, 1.5 and 2;
- the math functions against bounds and their own cross-host hash.

**E2. The entry point, part keys, the part cache and the composite.**
- `ThemeEngine` is owned by the application's theme, so every window shares one (DK W0 made the theme application-scoped).
- `paint_part` calls `ThemeEngine::paint`, with today's body as the built-in recipe.
- `PartKey` (GIVEN H3-1) joins `PartPaint`, and the window's `EnginePartTable` keeps its records.
- The `image` part role (GIVEN H1-1) and content colour (GIVEN E2-1) are drawn exactly as given; a theme styles only what surrounds them.
- Records are released when their control detaches, and stale generations are rejected.
- A retained `Painter` command composites a part raster at device-aligned bounds, with an opacity, through the shared blend. It is implemented in the recording painter, the Skia adapter and the Windows DIB compositor.
- Per-part framebuffers: the cache keeps a part's raster while its resolved style and inputs are unchanged, reuses the storage, and stays within 8 MB per window, evicting the least recently painted parts first (GIVEN F2-1). Passthrough transparency is the premultiplied alpha of the raster itself.

Tests:
- every Gallery page records the same display commands before and after the change;
- the composite is byte-identical on every host;
- stale keys are refused;
- the cache is reused across unchanged frames;
- the cache is invalidated by generation.

**E3. The motion clock and control-owned motion values.**
- `MotionClock`, one per window.
- `MotionValue`, with:
  - `follow` (exponential approach);
  - `spring` (critically damped by default; overshoot only when a theme asks for it, at most 8 %);
  - `decay` (momentum with friction);
  - timed transitions with the five easings.

  Each settled condition is stated in device units at the window's scale.
- Reduced motion, the 400 ms ceiling and occlusion are handled as in "How motion is driven".
- The waiting consumers move onto the clock: D3's stamp and its settling, and D5's flyout slide.

Tests:
- zero frames once every value has settled;
- one frame request per window, however many values are moving;
- a retarget mid-flight keeps the value continuous;
- reduced motion reaches the end state in one frame;
- an occluded window schedules nothing;
- DK's existing D3 and D5 traces are unchanged except for frame counts, which may only fall.

**E4. The cascade.**
- Rule tables in memory, in the order: built-in recipe, theme sheet, class rule, stable-ID rule, inline style, forced high contrast. T3 fills them by parsing, and FD fills the inline styles. Until then they are filled by C++ data in tests.
- Selectors cover control type, stable ID, class, part (by name or role) and states, with no combinators.
- Within one level, specificity counts the ID, the classes, the type, the part and the states, and among equals the later rule wins.
- Controls gain a `classes` property, which the class rule needs.
- Loading validates everything before it replaces anything. An invalid declaration is dropped with a diagnostic naming its rule. A part whose primitive is unknown, invalid or over budget keeps the built-in recipe. The old sheet stays in force until the new one is completely built.
- A theme declares the engine version it needs, and on an older engine the parts that need a newer one fall back.
- Cost budgets (GIVEN F1-1): each primitive declares its bound in operations per device pixel plus a fixed cost; a part's stack is at most 512 operations per device pixel, and an animated part at most 4 million per frame. A part over budget, or whose cost cannot be bounded, keeps the built-in recipe.
- Forced high contrast is derived as a rule pass, replacing today's hand-authored arrays:
  - under the host's mode, the host palette and metrics win;
  - GUI.Forms' own mode keeps geometry and state composition, drops gradients, texture, glow, sheen, reflection, glass and cast shadows, and draws every edge solid.
- The schema gains its `engine` export.

Tests:
- cascade-order fixtures, one per level;
- specificity ties;
- invalid rules dropped with diagnostics;
- an over-budget part falls back;
- version fallback;
- derived high contrast against today's arrays for the existing families.

**E5. Bounded expressions.**
- An expression program with a fixed operation budget, supporting:
  - arithmetic;
  - `clamp`, `min`, `max`, `mix` and `smoothstep`;
  - comparisons;
  - choose-one-of-two.

  There are no variables, loops, recursion or timers.
- Inputs, states and parameters are read by index, and the set of inputs read is extracted statically.
- Evaluation uses the same numerics as the kernels.

Tests:
- budget refusal at load;
- determinism hashes;
- dependency extraction;
- division by zero and non-finite values give the declared fallback.

**E6. Inputs and damage by dependency.**
- Engine inputs as engine-set custom properties (GIVEN B1-1, B2-1): part-local pointer position in logical units while the part is hot, time since state entry, display and text scale, and named values the application publishes. Nothing but the engine writes them. Density is not among them (GIVEN B4-2).
- Each resolved part knows which inputs it reads (from its primitive and its expressions). A change to an input damages only the parts that read it, within their declared extent.

Tests:
- a pointer moving over a part that reads no pointer input damages nothing;
- a sheen part is damaged only within its own extent;
- a published value reaches only its readers;
- the counters in `MetricsSnapshot` prove it.

**E7. Presentational state machines, transitions and ambient effects.**
- Per-part state machines are declared in data and driven by the published states. They cannot create observable states or delay behaviour.
- Transitions on resolved properties include opacity (the tile fade) and the bulge amplitude (the ribbon's rise).
- Ambient effects declare a region, a period and a frame cap:
  - at most 30 fps;
  - over at most 10 % of the window on everyday surfaces, or the effect is refused at load; a surface the application marks ceremonial (GIVEN B3-1) lifts this cap for that surface only;
  - stopped when the window is occluded or minimized, under reduced motion, and when the application's motion switch is off.
- Under reduced motion, sheen rests static, pop changes elevation without motion, and an activity object holds a static pose.

Tests:
- the tile fade and the ribbon rise drawn frame by frame against reference hashes;
- zero frames after settling;
- an ambient effect over budget is refused;
- an ambient effect stops when occluded and under reduced motion.

**E8. Relief primitives, shapes and hit regions.**
- The relief set of §3.5:
  - pill plate (Plan Paint's `action_material`);
  - inset well;
  - pressed;
  - the gold active plate;
  - the default perimeter;
  - disabled;
  - etched divider;
  - seam;
  - actuator.
- The material stack of §4.2 rendered by engine kernels.
- The grip primitive (GIVEN H2-1), filling its part's box with the dot pattern.
- The engine's shape parameters (corner radius, circle, pill, cap). The engine derives the hit region from the geometry it computes, which is T1's hand-off. Themes choose among shapes and never set hit regions.
- The presentational scale capability (GIVEN D5-1).
- Text is published as layout slots, which is the other half of T1's hand-off.

Tests:
- one reference image per primitive at four scales;
- hit tests against derived shapes;
- text slots stay inside their parts under text scale.

**E9. Light and translucency primitives.**
- Gloss, a four-stop gradient with a hard break.
- Glow.
- Sheen, a specular reflection that follows the pointer, never a moving light.
- Pop.
- Translucency.
- Reflection moved to T5, beside full-fidelity glass (owner, 2026-10-22).
- Glass, a bounded blur of radius at most 8 logical px that samples only the backplane, cached against the backplane's damage generation.
- The overlay shadow (GIVEN E1-1): an engine primitive with the first-party blur, identical on every host, keeping `ThemeDefinition::overlay_shadow`'s two layers, their damage outsets and the high-contrast rule (none). It replaces `paint_part`'s `shadow` role path through `Painter::draw_box_shadow`.

Tests:
- reference images;
- glass never samples the control plane;
- a blur radius over the cap falls back;
- a sheen part is repainted only when the pointer moves within it.

**E10. Procedural materials.**
- Felt, wood grain, paper fibre, pinstripe, brushed metal, and fresco with blooms.
- Each is generated per display scale, off the input path, and cached. Until it is ready, the region shows the material's solid pair.

Tests:
- reference images per scale;
- generation never runs inside a paint pass;
- a scale change regenerates the material and does not resample it.

**E11. Gate evidence.**
- Run the reference-image tests on every CI platform and record the per-host hash comparison under `results/`.
- Record the zero-frame counters after motion stops.
- Record the fallbacks for parts whose cost cannot be bounded.
- Record that reduced motion disables motion primitives.
- Produce a native specimen sheet of every primitive for the owner. The starting tokens are revisited against this sheet (§12.3).

## Order and dependencies

- E1 comes first. E2 needs E1. E3 needs E2 and can run beside E4. E5 needs only E1's numerics.
- E4 needs E2. E6 needs E4 and E5. E7 needs E3, E4 and E6.
- E8, E9 and E10 need E2 for their rasters and E4 for their rules. They can run in parallel once both exist.
- T3 can start against E4's rule tables and E5's expression program. T4 needs E4, E7 and E8. T5 needs E9's backplane sampling.
- DK D8 and RB phase 7 take their motion from E3 and E7.

## Decided (2026-10-19)

The open questions of the first draft (engine inputs, reduced motion, cost
budgets, presentational scale) and the T4 notes on the reference (the shade
field, the third shaded step, hover) were answered in interview round 001.
The answers are woven into the phases above and listed in `README.md`.

## Gate

The gate is T2's, from the programme:
- a reference-image test per primitive on every host, with identical pixels;
- zero frames in the window counters once inputs stop and every settling primitive has settled;
- a part whose cost cannot be bounded falls back to the built-in recipe;
- reduced motion disables motion primitives.

These are recorded under `results/` with the hash comparison per host (E11).
Each phase's tests pass on every CI platform.
