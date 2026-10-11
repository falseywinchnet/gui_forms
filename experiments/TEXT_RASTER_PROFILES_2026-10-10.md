# Text rasterization profiles — 2026-10-10

## Question

**GIVEN (owner, 2026-10-10):** grayscale anti-aliasing only (no LCD colour
subpixel rendering: it needs per-display stripe profiling, and current displays
are Retina-class); HarfBuzz shaping plus GUI.Forms-owned rasterization profiles
on every operating system, for identical text output. This experiment gathers
evidence for choosing the profile. It selects nothing.

## Inventory before the change (OBSERVED)

| Path | Rasterizer | Hinting |
|---|---|---|
| `draw_text_utf8`, macOS | Skia → CoreText | none (CoreText) |
| `draw_text_utf8`, Linux | Skia → bundled FreeType | Skia default |
| Prepared text (`outline_gray_72dpi_v1`) | GUI.Forms FreeType | font bytecode (`FT_LOAD_DEFAULT`) |
| Text masks (`outline_gray`, `true_mono`) | GUI.Forms FreeType | none / mono |
| Windows host | GDI ClearType | GDI |

Shaping positions glyphs with FreeType-hinted advances at the logical size
(`hb_ft_font_set_load_flags(FT_LOAD_DEFAULT)`), independent of the rasterizer.

## Method

`benchmarks/text_raster_profile_probe.cpp` (OFF by default,
`GUI_FORMS_BUILD_TEXT_RASTER_PROFILE_PROBE`). M4 Mac mini, LLVM 22.1.8 Release,
FreeType 2.14.2, GUI.Forms 5bb7a8b plus the probe. Corpus
`Hamburgefonstiv quick brown fox 0123456789 Help` in Portsmouth Rapids and
Carlito Regular at 9, 11, 13, 15, 20 pt and scales 1, 1.5, 2. Production
HarfBuzz shaping supplies glyph positions for every row. Candidate FreeType
profiles: `unhinted`; `unhinted_widened` (invented size curve,
max(0, 0.6 − 0.02·ppem) px horizontal, half vertical — CANDIDATE, not a CoreText
model); `light_autohint`; `light_autohint_darkened` (FreeType autofitter stem
darkening); `native_hinting`. Reference: Skia `draw_text_utf8` (CoreText on this
host). Coverage is composited black on white in sRGB space and in linear light.
Metrics per line: ink relative to the reference, mean absolute difference from
the reference (0–255, over the line box), FNV-1a coverage hash, shaped width
minus unhinted outline advance at the device size, uncached raster cost.
Raw results: `TEXT_RASTER_PROFILES_2026-10-10.csv`.

## Results (MEASURED, this host only)

- **CoreText is uniformly heavier.** Unhinted FreeType ink is 0.82–0.87 of the
  CoreText reference at every size and scale. The ratio does not fall with
  size, so it does not look like size-dependent stem darkening; a coverage
  gamma/contrast adjustment is the leading HYPOTHESIS.
- **Production spacing disagrees with the outlines.** Shaped line width minus
  unhinted outline advance reaches −12.0 logical px (Portsmouth Rapids 13 pt)
  and +5.4 (Carlito 9 pt) per 47-character line, and scales exactly with device
  scale: the error is in logical-size hinted advances, not rasterization.
- **Native hinting distorts glyphs.** Portsmouth Rapids' bytecode renders "9"
  close to "3" at 9 and 11 pt at scale 1 (visible in the 1× sheet). It is also
  the slowest profile for Carlito (5.1–5.3 µs/glyph uncached).
- **Darkened profiles reach CoreText weight at small sizes.** Autofitter
  darkening gives 0.88–1.21 of reference ink; the widening curve 0.84–1.31.
- **Cost (uncached, µs/glyph):** unhinted 0.6–1.6, light autohint 1.1–2.9,
  darkened 1.6–5.4, native 3.1–5.3. A glyph cache makes these one-time costs.

## Run 2: chosen profile, unhinted shaping, gamma, cross-platform identity

**GIVEN (owner, after run 1):** light auto-hinting only; shape with unhinted
advances; Portsmouth Rapids is the owner's synthetic near-equispaced family
without meaningful bytecode hinting, so run 1's native-hinting damage reflects
absent instructions, not the face design. Native hinting is dropped.

The probe now places glyphs in integer 26.6 device units and shapes its own
line with HarfBuzz over FreeType using `FT_LOAD_NO_HINTING` (the chosen
production change). Rows: `light_hinted_advances` (today's spacing),
`light`, `light_gamma_1.2|1.45|1.7` (coverage c → 255·(c/255)^(1/γ)),
`light_darkened` (autofitter stem darkening). Raw results:
`TEXT_RASTER_PROFILES_2026-10-10_run2_macos.csv`,
`TEXT_RASTER_PROFILES_2026-10-10_run2_windows_wine.csv`.

**MEASURED:**

- **Unhinted shaping removes the spacing error.** Portsmouth Rapids placed
  width matches unhinted outline advances within ±0.1 px at every size and
  scale (−24…+9.6 px before at 2×). Carlito's remaining −1.0…−4.7 px is its
  kerning (GPOS), which the outline sum excludes.
- **A coverage gamma near 1.45 reproduces CoreText's weight uniformly.**
  Carlito ink relative to CoreText: 1.00 0.99 0.97 0.95 1.00 (1×) and
  1.00 0.99 0.96 0.95 0.95 (2×) across 9–20 pt; Portsmouth Rapids is matched
  near γ 1.2–1.45. This supports the run-1 HYPOTHESIS that CoreText's extra
  weight is a uniform coverage adjustment rather than size-dependent stem
  darkening. Gamma adds no per-glyph cost (table lookup); autofitter darkening
  adds 40–80% and is strongly size-dependent.
- **Identical output across operating systems and architectures.** The probe
  built natively for macOS arm64 and cross-built for Windows x86-64 (LLVM
  22.1.8 clang targeting MinGW-w64, LLD, run under Wine 11.4 via Rosetta 2)
  produced byte-identical coverage hashes for all 180 lines (6 profiles × 2
  fonts × 3 scales × 5 sizes) and identical placement values.
- Mean difference from the CoreText reference rises to about 15 for the
  unhinted-shaping rows because the reference still uses today's hinted
  positions; ink ratio is the comparable weight measure.

## Run 3: theme independence as the criterion for blending and gamma

**Question.** Choosing a coverage gamma by eye had no principled basis. The
proposed rule: text weight must not depend on theme polarity (GUI.Forms has
light, dark and high-contrast themes), and among choices meeting that, prefer
the font's designed weight (its exact outline area).

**Method.** The `light` profile's coverage (unhinted spacing, no gamma) is
composited black on white and white on black, blending either in sRGB values
or in linear light, at gamma 1.0, 1.2, 1.45 and 1.7. Perceived weight is the
CIE L* distance of each displayed pixel from the background, normalized by the
full ink-to-background distance, summed over the line and divided by the exact
outline area (ungamma'd coverage sum). Raw results:
`TEXT_RASTER_PROFILES_2026-10-10_run3_polarity.csv`.

**MEASURED (means over 2 fonts × 3 scales × 5 sizes; ranges in parentheses):**

| Blend | Gamma | On light / area | On dark / area | Dark ÷ light |
|---|---|---|---|---|
| sRGB values | 1.0 | 0.975 | 1.025 | 1.051 (1.020–1.110) |
| sRGB values | 1.45 | 1.108 | 1.160 | 1.046 (1.021–1.091) |
| linear light | 1.0 | 0.693 | 1.304 | 1.946 (1.299–3.114) |
| linear light | 1.45 | 0.788 | 1.418 | 1.829 (1.296–2.683) |

**Correction.** The earlier reasoned claim that linear-light blending keeps
weight equal across polarity holds physically but not perceptually: in L*,
linear blending makes dark-on-light text up to about half the weight of
light-on-dark. sRGB-value blending is nearly polarity-neutral because the sRGB
transfer curve approximates lightness perception.

**Result.** sRGB-value blending at gamma 1.0 keeps perceived weight within
1–5% of the designed outline area and within 2–11% between polarities
(largest at 9 pt, scale 1). Gamma multiplies both polarities alike and does
not change parity; under this criterion it is a stylistic weight choice, not
a correction. CoreText's measured weight corresponds to about gamma 1.45.
Limitation: L* models lightness, not the spatial-frequency response to thin
strokes; small-size legibility remains a separate, unmeasured question.

## Not yet measured

Linux (expected identical: same FreeType, integer placement); the owner's
choice of gamma; compositing color space for colored text; vertical metrics
under light hinting; glyph-cache memory and steady-state cost.
