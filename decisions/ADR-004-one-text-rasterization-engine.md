# ADR-004: One text rasterization engine with identical output on every platform

Status: accepted

Date: 2026-10-11

Owner approval: grand architect, 2026-10-10 and 2026-10-11 — grayscale only;
light auto-hinting; shaping with unhinted advances; sRGB-value blending with no
gamma; shared blending arithmetic on every host; rasterized glyphs held by the
retained command; vertical metrics unchanged; Windows GDI text removed.

## GIVEN constraints

- Text must render identically on macOS, Linux and Windows from the same font,
  string, size and scale.
- Grayscale anti-aliasing only. LCD colour-fringe rendering needs per-display
  subpixel-layout profiling, and current displays are Retina-class.
- HarfBuzz shaping and GUI.Forms-owned rasterization on every operating system;
  no host font rasterizer (CoreText, GDI/ClearType) decides text pixels.
- Fonts are the bundled GUI.Forms set; GUI.Forms never consults a host font
  catalog.

## Workloads and failure modes

**OBSERVED (before this decision):** six text paths with five rasterizers.
`draw_text_utf8` used CoreText through Skia on macOS and Skia's FreeType on
Linux; prepared text used GUI.Forms FreeType with the font's bytecode hinting;
text masks used unhinted GUI.Forms FreeType; `SkiaExecutor` kept its own
CoreText typefaces; the Windows host drew with GDI ClearType. A TextBox using
prepared text and a Label using ordinary text, in one Mac window, came from
different rasterizers.

**MEASURED** (`experiments/TEXT_RASTER_PROFILES_2026-10-10.md`):

- Shaping used FreeType-hinted advances at the logical size, independent of the
  rasterizer: line widths drifted −24…+9.6 px at scale 2 from the outlines.
- Bytecode hinting deformed Portsmouth Rapids "9" at 9–11 pt (that family has
  no meaningful hinting instructions) and was the slowest profile.
- CoreText output carried uniformly about 15% more ink than unhinted FreeType.
- Blending in linear light made dark-on-light text up to about half the
  perceived (CIE L*) weight of light-on-dark; sRGB-value blending kept the two
  polarities within 2–11% and within 1–5% of the designed outline area.

## Candidates

1. Keep host rasterizers (CoreText, Skia FreeType, GDI) per platform.
2. Skia's FreeType scaler everywhere, with its compile-time gamma/contrast
   preblend pinned per platform.
3. One GUI.Forms FreeType rasterizer producing coverage, composited by every
   host with one shared blending rule.

Profile candidates: unhinted; unhinted with an invented widening curve; light
auto-hinting; light auto-hinting with FreeType stem darkening; bytecode
hinting. Weight candidates: coverage gamma 1.0–1.7; sRGB-value or linear-light
blending.

## Evidence

**MEASURED:** the probe built natively for macOS arm64 and cross-built for
Windows x86-64 (LLVM 22.1.8, MinGW-w64, run under Wine 11.4 via Rosetta 2)
produced byte-identical coverage for all 180 lines (6 profiles × 2 fonts × 3
scales × 5 sizes) with integer 26.6 placement. Unhinted shaping placed
Portsmouth Rapids within ±0.1 px of its outline advances. Gamma scales both
polarities alike, so it is a stylistic weight setting, not a correction.

## Decision

Candidate 3, with this profile:

- **Shaping:** HarfBuzz over FreeType with unhinted advances
  (`FT_LOAD_NO_HINTING`) at the logical size. Kerning and GPOS are HarfBuzz's.
- **Placement:** integer 26.6 device units; baseline snapped to a whole device
  pixel; horizontal subpixel phase retained.
- **Rasterization:** FreeType light auto-hinting
  (`FT_LOAD_TARGET_LIGHT | FT_LOAD_FORCE_AUTOHINT`), grayscale
  (`FT_RENDER_MODE_NORMAL`), no stem darkening, no outline widening.
- **Blending:** coverage composited in sRGB values, gamma 1.0, using one
  arithmetic that every host implements bit-identically (Skia's raster blit is
  the reference; the Windows DIB compositor matches it, tested byte for byte).
- **Glyph memory:** rasterized glyph runs are kept with the retained command
  (`RetainedDrawCache`), released with its display chunk; no global cache or
  eviction policy unless measured memory requires one.
- **Vertical metrics:** unchanged in this decision.
- **Windows:** GDI text, ClearType and GDI font fallback are removed; HarfBuzz
  text is enabled in the default Windows build.
- **Skia:** no host font backend once every path is migrated.

## Rejected options

- Host rasterizers (1): cannot give identical output; CoreText and GDI also
  differ in weight and hinting philosophy.
- Skia's FreeType scaler (2): its mask preblend is a compile-time per-platform
  setting, and the Windows host does not use Skia.
- Bytecode hinting: deforms glyphs of fonts without meaningful instructions.
- Stem darkening and outline widening: size-dependent weight, 40–80% raster
  cost, not needed for polarity parity.
- Coverage gamma ≠ 1: a stylistic weight change, not a correction.
- Linear-light blending: polarity-dependent perceived weight.
- LCD subpixel rendering: excluded by the owner.

## Implementation phases

1. Shaping with unhinted advances.
2. Shared glyph rasterizer and shared blending rule.
3. Skia ordinary text (macOS, Linux) on the shared rasterizer.
4. Prepared text and text masks on the shared profile.
5. `SkiaExecutor` text on the shared rasterizer.
6. Windows ordinary text on the shared rasterizer; GDI text removed.
7. Skia built without a host font backend.

A cross-platform identity test, comparing rendered text against checked-in
hashes on every CI platform, enforces the decision from phase 3 onward.

## Reversal path

Each phase is an independent change. Reverting phase 3 or 6 restores the host
rasterizer for that path; reverting phase 1 restores hinted advances. Font
files and public text APIs do not change.

## Open edges

Small-size legibility (L* models lightness, not thin-stroke perception);
vertical metrics under light hinting; colored-text compositing; glyph-run
memory under large documents.
