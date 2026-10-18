# Engine core: plan

Date: 2026-10-18. Status: **proposed, awaiting the owner's acceptance**.
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

- `std::optional<PartKey> key`. This is the part's declared position (`PartIndex`), or for an item part, the item system, the part, and the item's stable key.

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
- A `PrimitiveDescriptor` names the `PartRole`s it paints, the `AnatomyState`s it reads, the value atoms it reads (mark, accent, content colour) and, once admitted, the engine inputs it reads (open question 1). Because these are the vocabulary's own enums, a primitive cannot name an atom that does not exist.
- A rule that binds a primitive to a part whose role the primitive does not paint is refused at load, and the part keeps the built-in recipe.
- The schema export gains an `engine` section listing every primitive with its atoms, parameters and version, beside `vocabulary`. `tools/control_schema.py check` covers it.
- **Gaps found so far:**
  - **Engine inputs are not atoms yet.** The schema has no `inputs` list, and the code has no input type.
  - **Density** is named in §8.4, but no code carries it.
  - **No surface is marked ceremonial.** §10 lets ceremonial surfaces exceed the ambient cap, but nothing in the code marks a surface as ceremonial.

  All three are vocabulary changes, so they are asked as open question 1. No phase before E6 needs them.

## How motion is driven

There are two drivers, and they share one clock per window.

1. **Theme-owned transitions** (E7) run on part records. A theme, or the
   built-in rule table before T3, declares a transition on a property for a
   part's states. Examples:
   - the tile fade: the `actuator` tile's opacity, from 0 to 1 over *quick* when the part appears;
   - the ribbon's dithered rise: the bulge's `amplitude`, from 0 at rest to 1 while `hot`, by exponential approach.

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
- Under reduced motion, every transition and motion value jumps to its end in the same frame, and ambient effects stop.
- While a window is occluded or minimized, its clock jumps everything to the end and schedules nothing.
- Timers, settling and damage publication run on the UI thread. The clock is cancelled by name when a window closes. A record whose control has gone is rejected by its generation.

**The opacity input owed to the docking tiles.** D3 (#144) publishes the
tiles' 100 ms fade as `expanding` but cannot draw it, because `PartPaint` has
no opacity. (The request named this "DK-52". No decision with that number is
recorded on any branch, and this plan takes it to mean the D3 gap.) In this
plan, opacity is not something a control supplies:
- It is an engine presentational property: one multiplier on the part's whole composite, applied in the shared blend (E2).
- A transition drives it (E7), keyed on the part's appearance. That appearance is a new `PartKey` in the window's part table, or the published `expanding` state.
- D3 changes only to pass its tiles' keys.

The alternative would be a control-supplied `opacity` field in `PartPaint`.
That is a new value atom, which would need the owner, and it would put motion
back into controls. This plan does not take it.

## Numerics and identical pixels

The gate asks for identical pixels on every host. CI builds and tests on four
platforms: macOS arm64, Linux x64, Linux arm64 and Windows x64, all with the
pinned LLVM 22. Identity is designed in, not hoped for:

- **One precision.** Every kernel computes in `double` and quantizes once, at the end, to 8-bit premultiplied values with `floor(v·255 + 0.5)`. There is no fast-math.
- **No fused multiply-add.** Clang contracts `a·b + c` into an FMA by default on arm64, but not on baseline x86-64. Every engine kernel source turns contraction off with `#pragma clang fp contract(off)`, and arithmetic kernels live in `.cpp` files, not in headers that other translation units would compile under their own flags.
- **No host libm in kernels.** `pow`, `exp`, `log` and the trigonometric functions differ between macOS libm, glibc and the Windows runtime. The engine has its own `power`, `exponential` and `logarithm`. They are built only from `+ − × ÷` and `sqrt` (IEEE-exact) and from `frexp` and `ldexp` (exact), each with a stated error bound.
- **One blend.** Part rasters composite by premultiplied source-over, using the arithmetic that ADR-004 already made bit-identical across the Skia raster and the Windows DIB compositor for text. A composite test compares the bytes on every host, as `text_identity_raster_tests` does.
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
- **The dithered lattice at fractional scales.** The reference leaves this rule to the engine. The lattice stays at exactly 2 logical px. A dot at logical (x, y) covers device pixels `[floor(x·s), floor(x·s) + D)` on each axis, with `D = max(1, floor(s + 0.5))`. Its tone is sampled at the dot's logical position, as the reference formula reads. At integer scales the lattice is exact. At fractional scales a dot's position may vary by one device pixel.
- Nothing is wired into `paint_part` yet, and no control's pixels change.

Tests:
- registry refusals;
- parameter refusals;
- destination validation;
- amplitude 0 paints nothing;
- dots only on the lattice;
- at least three tones on each side;
- the dent mirrors the bulge;
- the cross-host hash table at scales 1, 1.25, 1.5 and 2;
- the math functions against bounds and their own cross-host hash.

**E2. The entry point, part keys, the part cache and the composite.**
- `ThemeEngine` is owned by the application's theme, so every window shares one (DK W0 made the theme application-scoped).
- `paint_part` calls `ThemeEngine::paint`, with today's body as the built-in recipe.
- `PartKey` joins `PartPaint`, and the window's `EnginePartTable` keeps its records.
- Records are released when their control detaches, and stale generations are rejected.
- A retained `Painter` command composites a part raster at device-aligned bounds, with an opacity, through the shared blend. It is implemented in the recording painter, the Skia adapter and the Windows DIB compositor.
- Per-part framebuffers: the cache keeps a part's raster while its resolved style and inputs are unchanged, reuses the storage, and stays within a fixed memory cap. Passthrough transparency is the premultiplied alpha of the raster itself.

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

**E6. Inputs and damage by dependency** (needs open question 1).
- Engine inputs as engine-set custom properties: part-local pointer position while the part is hot, time since state entry, display and text scale, density, accessibility flags, and named values the application publishes. Nothing but the engine writes them.
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
  - over at most 10 % of the window on everyday surfaces, or the effect is refused at load;
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
- The engine's shape parameters (corner radius, circle, pill, cap). The engine derives the hit region from the geometry it computes, which is T1's hand-off. Themes choose among shapes and never set hit regions.
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
- Reflection, a bounded copy of the part's own content or of the backplane within its grant.
- Glass, a bounded blur of radius at most 8 logical px that samples only the backplane, cached against the backplane's damage generation.

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
- E4 needs E2. E6 needs E4, E5 and open question 1. E7 needs E3, E4 and E6.
- E8, E9 and E10 need E2 for their rasters and E4 for their rules. They can run in parallel once both exist.
- T3 can start against E4's rule tables and E5's expression program. T4 needs E4, E7 and E8. T5 needs E9's backplane sampling.
- DK D8 and RB phase 7 take their motion from E3 and E7.

## Open questions for the owner

1. **Admit the engine inputs as atoms** (§12.7: adding an atom is an owner decision).

   Proposed set:
   - `pointer_x` and `pointer_y`: part-local logical units, published only while the part is hot;
   - `time_in_state`: milliseconds since the part entered its current state set;
   - `display_scale` and `text_scale`;
   - `density`: compact, standard or touch;
   - published application values: named, numeric, read-only, written by the application and never by a theme.

   The proposed set also includes:
   - a `ceremonial` flag that the application sets on a surface. It lifts the ambient region cap for that surface, because §10 lets the application decide when a surface is ceremonial.
   - High contrast and reduced motion stay states, as they are.

   **Recommendation:** admit this set with these names and units. E6 waits for this; E1 to E5 do not.
2. **Reduced motion and the activity object.** Today `MotionPolicy` keeps animating under reduced motion, at 0.35 speed with half the excursion. §9.5 (proposed) and the T2 gate say ambient effects stop and an activity object holds a static pose beside its stage text. **Recommendation:** §9.5 governs the engine, and the existing `MotionPolicy` consumers move to it in T4.
3. **Cost budget tokens.**
   - Each primitive declares its bound in operations per device pixel, plus a fixed cost.
   - A part's cost is the sum over its stack, multiplied by its device area.
   - **Proposed starting tokens:**
     - a part's stack at most 512 operations per device pixel;
     - an animated part at most 4 million operations per frame;
     - a window's ambient effects together within the accepted 30 fps and 10 % tokens.
   - **Recommendation:** accept these as starting tokens, to be revisited against the specimen sheet. E4 enforces only "cannot be bounded" until the owner answers.
4. **A presentational scale for loud themes.** §9.1 rejects scale transitions, and the T3 scenario shows `scale: 0.97` on press. **Recommendation:** the engine offers it only as a capability:
   - the part is drawn scaled about its centre and must stay inside its granted overflow;
   - the hit region and layout do not change;
   - the default theme does not use it.

## Notes for T4 (not questions)

- **The bulge's shade field does not melt inside the bounds.** The rendering reference's shade ellipse is centred at (0.60, 0.64) with radii (0.50, 0.48), so it reaches 1.10 w and 1.12 h. At the middle of the bottom edge it is still at about 47 % of its peak. That matches "deepest at the lower right corner", but not §3.5's "melt back into the plane before the part's bounds". E1 implements the formula exactly, as the approved studies drew it. The specimen sheet settles which reading is right.
- **The dithered bulge's deepest shaded step never appears with the default gains.** Sampled over the whole box, the tone of the reference fields runs from about −0.63 to +1.05 with lit, shade and crown gains of 1.0, 0.85 and 0.25. The third shaded step (`rgb(88, 122, 186)`) needs at least −0.68, so the light default shows three lit steps and only two shaded ones. The dark variant's gains (0.9, 1.0, 0.2) reach −0.80 and show all three. E1 implements the reference as written, and its tests record this. T4 may raise the shade gain if the owner wants the third step.
- **Hover brightening is not part of E1's bulge primitives.** The studies use brightness 1.03–1.04 and saturation 1.15. This is a theme-level filter for E4 and E7.

## Gate

The gate is T2's, from the programme:
- a reference-image test per primitive on every host, with identical pixels;
- zero frames in the window counters once inputs stop and every settling primitive has settled;
- a part whose cost cannot be bounded falls back to the built-in recipe;
- reduced motion disables motion primitives.

These are recorded under `results/` with the hash comparison per host (E11).
Each phase's tests pass on every CI platform.
