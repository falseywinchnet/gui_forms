# T2 closing gate check: CSS coverage

Date: 2026-10-22. Status: reviewed by the owner on 2026-10-22. It changes
no engine behaviour.

**Owner decisions, 2026-10-22.** E9.6 builds every missing row except
reflection: rows 1 to 9 and 11 to 14 of "Missing rows, by stage".
- `text-shadow` (row 5) is data on the text slot only. Its glyph coverage
  belongs to the text stage.
- Reflection (row 10) moves to T5, beside full-fidelity glass, and is
  removed from T2's E9.
- E9.6 is built as separable PRs on main, each stacked at most two deep:
  fills and shape; filters and blend; finish, mask and position; motion.

## Why

The owner, before T2 closes: "there's a lot of effects and appearances like
gloss vs matt that css can represent and that the engine should carry the
properties for." T3 (`planning/stages/T3-theme-language/README.md`, "Scope")
admits the engine's style vocabulary in a CSS-family syntax and no layout
properties. This inventory lists every CSS visual property T3 is expected to
admit and says what carries it in the engine today.

## How to read it

- **Engine carrier**: the engine property, material layer, primitive or rule
  field that the CSS property compiles to. Names are the C++ ones in
  `include/gui_forms/`.
- **Status**:
  - *present*: built and tested on main or in an open T2 PR;
  - *planned*: in the T2 plan (E10 procedural materials, E11 gate evidence);
  - *missing*: nothing carries it. The "Smallest addition" column then
    proposes the least engine change and the stage that owns it.
- **Stages named for additions**:
  - *E9.6*: the T2 effects follow-up the owner admitted, after E9 and before the gate;
  - *E10*, *E11*: the existing plan;
  - *T3*: parser only (no engine change);
  - *T5*: the host, which owns the backplane renderer upgrade.

Where a CSS form is not admitted by decision, the row says so, with the
decision.

## 1. Fills and gradients

| CSS | Engine carrier | Status | Smallest addition |
|---|---|---|---|
| `background-color` | `MaterialFillLayer::solid` | present | |
| `linear-gradient()`, angle or endpoints, any stop count | `MaterialFillLayer::linear`, `linear_css_angle` | present | |
| `repeating-linear-gradient()` | `MaterialFillLayer::repeating_linear` | present | |
| `radial-gradient()`, centre and radii | `MaterialFillLayer::radial` | present | |
| `repeating-radial-gradient()` | none | missing | Apply the layer's `GradientSpreadMode::repeat` to radial fills too. The bake reads `spread` for linear fills only today; E9.6 |
| `conic-gradient()`, `repeating-conic-gradient()` | none | missing | A `conic_gradient` fill kind: t from the angle about a centre, with a deterministic engine `atan2` beside `engine_math`'s sine and cosine; E9.6 |
| layered `background` (up to 8 layers) | `SurfaceMaterial::fills`, `maximum_fill_layers` = 8 | present | |
| layer opacity | `MaterialFillLayer::opacity`, colour alpha | present | |
| procedural backgrounds (felt, wood grain, paper fibre, pinstripe, brushed metal, fresco) | engine functions in `background` | planned (E10) | |

## 2. Borders, outlines and radii

| CSS | Engine carrier | Status | Smallest addition |
|---|---|---|---|
| `border` (uniform width and colour) | `SurfaceMaterial::border` | present | |
| `border-top` and the other single edges | `MaterialBorderEdges` (square-cornered) | present | |
| inner highlight and lowlight lines | `MaterialKeyline` (edge, width, inset), up to 8 | present | |
| `border-style: dashed`, `dotted`, `double`, `groove`, `ridge` | none (solid only) | missing | `double`, `groove` and `ridge` are keyline pairs, so T3 can lower them with no engine change. `dashed` and `dotted` need a dash pattern on the border, the period in device pixels. E9.6, if the owner wants them |
| `border-radius`, one value | `SurfaceMaterial::corner_radius` | present | |
| `border-radius`, per corner or elliptical | none | missing | Four radii on the material's rounded shape (`PartShape`). The kernel's signed distance takes a radius per quadrant. E9.6 |
| shape keywords (circle, pill, caps) | `PartShapeKind`, the relief primitives' `shape` choice | present | Not on the material primitive yet. Give it the same `shape` choice; E9.6 |
| `outline`, `outline-offset` (focus) | `ControlVisualRecipe::focus_ring`, `focus_width`, `focus_offset` | present | |
| the default action's perimeter | `ControlVisualRecipe::default_ring`, `default_width` | present | |
| `border-image` | nine-patch image fill (`MaterialImageMode::nine_patch`) | present (as a fill) | |

## 3. Shadows

| CSS | Engine carrier | Status | Smallest addition |
|---|---|---|---|
| `box-shadow` (outer: offset, blur, spread, colour), up to 4 | `MaterialShadow`, `inset = false` | present | |
| `box-shadow: inset` | `MaterialShadow`, `inset = true` | present | |
| a popup's cast shadow | `overlay_shadow` primitive with the theme's overlay layers (E9.1) | present | |
| `text-shadow` | none | missing | A text shadow on the text role (offset, blur, colour), drawn under the text slot's glyphs. The blur must be first-party so it is identical on every host. It belongs with the engine's text slots: E9.6 for the data and the slot, and the text stage (M4/M9) for the glyph coverage |

## 4. Opacity

| CSS | Engine carrier | Status | Smallest addition |
|---|---|---|---|
| `opacity` on a part, animated | the presentational `opacity` property (`opacity_property`), through a transition | present | |
| `opacity` on a part, static | a transition with one state | present (indirect) | T3 lowers a static `opacity` to a one-state machine; no engine change |
| colour alpha, layer opacity | colours and `MaterialFillLayer::opacity` | present | |

## 5. Filter functions

None of the filters exists as a post-process on a part's raster. Glass (§6)
blurs the backplane, not the part.

| CSS | Engine carrier | Status | Smallest addition |
|---|---|---|---|
| `filter: brightness()`, `contrast()`, `saturate()`, `grayscale()`, `sepia()`, `invert()` | none | missing | One colour-matrix step at the end of the material bake: a 4 × 5 matrix in premultiplied colour, composed from the listed functions at compile time, one multiply per pixel, costed in `material_bake_policy.hpp`. E9.6 |
| `filter: hue-rotate()` | none | missing | The same matrix (hue rotation is linear in RGB); E9.6 |
| `filter: blur()` on the part itself | none | missing | A separable integer box blur over the part's raster: glass's kernel, with the margin taken from the part's outsets and capped at 8. E9.6 |
| `filter: drop-shadow()` | `box-shadow` for box-shaped parts | missing for other shapes | Box-shaped parts: T3 lowers it to an outer `MaterialShadow`, with no engine change. A shadow of a part's own alpha (a glyph or an irregular shape) needs a blur of the baked alpha, the part-blur kernel above; E9.6 |
| `filter: url()` | not admitted | | Themes are data only, with no external references (T3, RC004) |

## 6. Backdrop

| CSS | Engine carrier | Status | Smallest addition |
|---|---|---|---|
| `backdrop-filter: blur()` | `glass` primitive (E9.3): three integer box passes, blur 0 to 8, over the backplane image | present | |
| a backdrop tint (the usual `background-color` over the blur) | glass's `tint` parameter | present | |
| backdrop rounded clip | glass's `corner`, or the part's own radius | present | |
| `backdrop-filter: brightness()`, `saturate()` and the other colour filters | none | missing | The colour matrix of §5, applied by glass to the blurred backplane before the tint; E9.6. Decided (coordinator, 2026-10-22): glass gains a `recipe` choice, and the recipe material's `colour_filter` is the matrix. T3 lowers the backdrop-filter's colour functions by composing them into one matrix in source order. |
| what the backdrop contains | the backplane plane only: the window's backplane material and the backplane-plane engine part rasters. Text and app-drawn backplane commands are skipped and counted (owner, E9-B option 1) | present | A host `render_backplane` that replays the whole plane is T5's upgrade behind the same borrow |
| a backdrop of other controls | not admitted | | Glass samples the backplane, never the control plane (design language §4.6 rule 3) |

## 7. Blending

| CSS | Engine carrier | Status | Smallest addition |
|---|---|---|---|
| `mix-blend-mode: normal` | `PaintGroupComposite::source_over` | present | |
| `mix-blend-mode: plus-lighter` | `PaintGroupComposite::plus` (the face cross-fade uses it) | present | |
| `mix-blend-mode: multiply`, `screen`, `overlay`, `darken`, `lighten` and the rest | none | missing | `multiply` and `screen` first: two more `PaintGroupComposite` values in the shared CPU blend, which every painter already routes through. The separable modes follow the same pattern; the non-separable ones (`hue`, `saturation`, `color`, `luminosity`) are not proposed. E9.6 |
| `background-blend-mode` (between a material's own layers) | none (layers composite source-over; glows add) | missing | A blend mode per fill layer, using the same blend functions inside the material bake; E9.6 |
| `isolation: isolate` | paint groups (`PaintGroup::isolated`) | present | |

## 8. Surface finish

| CSS | Engine carrier | Status | Smallest addition |
|---|---|---|---|
| gloss | `gloss_material(GlossColours, radius)` preset (E9.4): a four-stop break at 0.48 to 0.52, a 1-unit shine and a border | present (as a preset) | |
| matte | a plain gradient or solid material | present (as authored) | |
| `surface-finish` from matte to gloss as one parameter | none | missing | `SurfaceMaterial::finish` in [0, 1] (owner: one parameter, not separate presets). The bake derives the finish from the material's base gradient: 0 is the material as authored; above 0 it sharpens the mid-face break (the gap from 0.48–0.52 scaled towards the authored stops), lifts the top half's luminance and adds the shine keyline at an alpha proportional to the finish. `gloss_material` becomes finish 1 over its colours, and the preset stays a convenience. E9.6 |

## 9. Light effects

| CSS (engine extension) | Engine carrier | Status | Smallest addition |
|---|---|---|---|
| glow `under` (current selection) | `MaterialGlow`, `MaterialGlowPlacement::under`; reach ≤ 8 | present (#272, placement in #278) | |
| glow `over` (other emphasis) | `MaterialGlow`, `MaterialGlowPlacement::over`; clipped to the face | present (#278) | |
| sheen (`radial-gradient` at `--pointer-x`) | `sheen` primitive behind the overlay seam (E9.2) | present | T3 maps the scenario's pointer-following gradient to the sheen's parameters, not to a per-frame gradient |
| pop | rule data: `scale` and `face` transitions with the hot or selected recipe (E9.5, #274) | present | |
| reflection (formerly in T2 plan E9) | none | moved to T5 | A bounded copy of the part's own raster or of the backplane, mirrored within the grant, faded by a linear alpha. Owner, 2026-10-22: it moves to T5, beside full-fidelity glass |
| translucency (T2 plan E9) | colour alpha, layer and part opacity; glass for frosted | present (by composition) | |

## 10. Image layers

| CSS | Engine carrier | Status | Smallest addition |
|---|---|---|---|
| `background-image: url()` naming a theme resource | `MaterialFillLayer` image fill by `ImageId` | present (by id) | Names: a theme names a resource, and the collection's manifest resolves it to an `ImageId` at load. This is E8c decision 3, not built yet. It is T3 work on the engine's existing id, with no new engine code |
| `background-size`, `background-repeat` | `MaterialImageMode` stretch, tile and nine-patch; `image_scale` | present | |
| `background-position` | none for images (gradients place by coordinates) | missing | An image origin offset on the image fill, applied when the variant's maps are filled; E9.6 |
| sized image faces | CONV variants (`image_variant`): one per (resource, device size, scale), made outside the bake | present | |
| image formats | the resource registry decodes PNG today. CNG is the native resource format and PNG is for interop (owner direction); CNG's codec is in `stx_cng` | present (PNG); CNG wiring follows the intake | |

## 11. Masks and clipping

| CSS | Engine carrier | Status | Smallest addition |
|---|---|---|---|
| `clip-path` | not admitted as geometry. The engine owns shape and hit region (design language §12.3; T3 brief). The `shape` parameters are the admitted form | present (as shape parameters) | |
| `mask-image` (an alpha mask) | none | missing | A `mask` image layer: an image fill whose alpha multiplies the box's coverage rather than compositing colour. It reuses the CONV variants and the image maps; E9.6 |
| `mask-image: linear-gradient()` (a fade-out) | none | missing | The same mask layer kind with a gradient source; E9.6 |

## 12. Presentational transforms

These are only presentational: hit region and layout are unchanged.

| CSS | Engine carrier | Status | Smallest addition |
|---|---|---|---|
| `scale` | presentational `scale` (`scale_property`), 0.5 to 2, within the granted overflow | present | |
| `translate` (a press nudge or a lift) | `ControlVisualRecipe::visual_offset` and `pressed_content_offset` (static, per state); animated: `translate_x` and `translate_y` (±32), within the grant less the scale's share, the raster composited at the offset | present (E9.6 G4) | |
| `rotate`, `skew`, 3D transforms | none | missing | Not proposed for controls (axis-aligned hit regions and pixel-exact parts). For activity glyphs, an ambient rotation of a baked raster would be its own primitive; ask the owner |
| `transform-origin` | the centre (scale) | present (fixed) | |

## 13. Transitions, keyframes and timing functions

| CSS | Engine carrier | Status | Smallest addition |
|---|---|---|---|
| `transition-property`, `transition-duration` | `PropertyTransition`, `MotionCurve::timed`, a 400 ms ceiling | present | |
| `transition-timing-function`: `linear`, `ease-in-out` | `MotionEasing::linear`, `ease`, `smoothstep` | present | |
| the overshoot ("back-out") | `MotionEasing` overshoot (8 %) | present | |
| `cubic-bezier()` of any control points | `MotionCurve::cubic_bezier`: x clamped to [0, 1], y to the 8 % overshoot, solved by 40 bisections | present (E9.6 G4) | |
| `steps()` | `MotionCurve::stepped`: n in [1, 32], `jump-end` or `jump-start` | present (E9.6 G4) | |
| `transition-delay` | `MotionCurve::delayed`, delay and duration within the 400 ms ceiling | present (E9.6 G4) | |
| `follow()`, `spring()`, `decay()` (engine functions) | `MotionCurve::follow`, `spring`, `decay` | present | |
| state-driven `@keyframes` (enter from, then rest) | `PresentationState::enter_from` and `target` | present | |
| multi-stop `@keyframes` | `PresentationState::stops`, up to 4, played on entering the state (`MotionValue::animate_through`); a parameter shows each at its nearest E7r keyframe | present (E9.6 G4) | |
| looping `animation` (`animation-iteration-count: infinite`) | `AmbientEffect` (period, frame rate, pose) with `time_in_state` expressions | present | |
| `prefers-reduced-motion` | engine-wide: every transition jumps and ambient effects rest | present | |

## 14. Colour roles and text roles

| CSS | Engine carrier | Status | Smallest addition |
|---|---|---|---|
| `color` (text) and the inherited text colour | `ControlVisualRecipe::text`, `muted_text` | present | |
| glyph colour | `ControlVisualRecipe::glyph` | present | |
| accent colour | the theme's accent ramp | present | |
| ring colours | `focus_ring`, `default_ring` | present | |
| named colour roles as custom properties (`--face-light`) | none in the engine (T3 resolves variables at compile time) | present (T3) | |
| font role (`font: caption` and the like) | `ThemeTypographyTokens` roles: ui, field, caption, heading, title, monospace | present | |
| text decoration, letter spacing, text transform | none | missing | Out of T2's vocabulary. They belong to text roles in the typography stage (M4/M9), with no engine change |
| forced high contrast | the engine's own pass (`CascadeLevel::forced_high_contrast`) and the high-contrast recipes | present | |

## Missing rows, by stage

E9.6, a T2 effects follow-up before the gate. Each item is small and
independent. The owner admitted all of them except row 10:

1. `repeating-radial-gradient`: the repeat spread on radial fills.
2. `conic-gradient`: a conic fill kind and a deterministic `atan2`.
3. Per-corner radii on the material's rounded shape, and the `shape`
   choice on the material primitive.
4. Dashed and dotted borders, if wanted.
5. `text-shadow` on the text slot: data only. Its glyph coverage belongs
   to the text stage.
6. The colour-matrix filters on parts, and on glass's backdrop.
7. `filter: blur()` and an alpha `drop-shadow()` on a part's own raster.
8. `multiply` and `screen` group composites, and per-layer blend modes.
9. `surface-finish` as one matte-to-gloss parameter.
10. Reflection: moved to T5 (owner, 2026-10-22). Not in E9.6.
11. `background-position` for image fills.
12. A `mask` layer kind: image or gradient alpha.
13. An animated presentational `translate`.
14. `cubic-bezier`, `steps()`, `transition-delay`, and multi-stop keyframes.

T3, parser only: lowering static `opacity`, `double`, `groove` and `ridge`
borders, box `drop-shadow`, and nearest-easing matching; resource names to
`ImageId`.

Not proposed: `rotate`, `skew` and 3D transforms on controls; the
non-separable blend modes; `filter: url()`; `clip-path` geometry.
