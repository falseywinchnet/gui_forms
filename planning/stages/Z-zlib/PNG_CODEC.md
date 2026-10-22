# Owned PNG phase

Started 2026-10-19 on the owner's instruction, “lets begin the png phase.”
This advances phase 5b alongside the still-open inflate verification gate;
it does not assert that phase 2's sustained fuzz gate passed. Separate stacked
PR; production cutover remains phase 6.

The owner named the eventual independent libraries `stx_zlib` and `stx_png`
on 2026-10-20. `stx_vorbis` is the packaging and API precedent. The current
`gui_forms::compression` and `gui_forms::image` seams are proving interfaces,
not evidence that either standalone library or its production cutover exists.

## Contract reviewed before implementation

The first implementation boundary is a private, lossless **sample decoder**.
It reconstructs every legal static PNG colour/depth combination and Adam7,
returning top-down straight-alpha RGBA16 in native integer storage. Eight-bit
values expand by 257; packed grayscale expands exactly to 0..65535. Transparency
keys compare original samples before expansion. There is no premultiplication,
gamma correction, ICC transformation or silent 16-bit truncation at this seam.
That separates byte decoding from the eventual shared colour-management policy.
It is not yet a replacement for any production colour-managed renderer.

Input is an immutable whole-file span. Inspection validates signature, chunk
bounds/types/order/CRCs and computes exact output and row-storage requirements.
An optional measurement call uses caller-owned reusable scratch to validate
compressed metadata and report its exact decoded byte count before output
allocation. Its UTF-8 state spans scratch boundaries, and all compressed
fields share the metadata and work quotas. It does not validate IDAT or
palette indices. Decode revalidates the source independently rather than
trusting a caller-modifiable measurement result.
IDAT payloads feed one owned inflate stream without concatenation. Two reusable
rows suffice; each Adam7 pass resets the previous row. Exact filtered size is
sum((ceil(pass_width * channels * depth / 8) + 1) * pass_height) over nonempty
passes. A final drain verifies the zlib trailer and rejects excess output,
compressed tails, dictionaries and missing data. CRC is IEEE PNG CRC-32, not
CRC32C. CRC/order validation precedes pixel writes.

All storage belongs to the caller and must be disjoint: encoded bytes, inflate
workspace, two rows, RGBA16 samples, metadata bytes and metadata descriptors.
No allocations, callbacks or retained borrows inside decode. Output prefixes
are unverified on failure and must be discarded; only success publishes them.
Finite encoded/chunk/dimension/pixel/filtered-byte limits bound parsing and
reconstruction; an aggregate inflate work budget survives metadata resets.
Compressed iCCP/zTXt/iTXt content shares one metadata-output budget. Descriptors
retain encoded offsets for all ancillary chunks and decoded offsets for those
compressed fields; no parser-created borrow outlives the call. Text chunks reject
embedded NULs; iTXt translated keywords and text require well-formed UTF-8.
Raw text is checked during parsing and inflated text immediately after decoding,
without normalization or a repeated raw-text scan. Uncompressed iTXt ignores
its compression-method byte as PNG requires. Language-tag registry/semantic
validation remains open. Compressed metadata storage exhaustion can follow
partial image/metadata output, which the caller must discard.

Filters select their kernel once per row. Up has independent byte additions;
Sub uses a channel-strided 16-byte prefix scan for byte strides 1 and 4,
seeded from preceding reconstructed bytes. Separate stages add modulo 256; exact
tails and other strides retain the scalar recurrence. An explicit 128-bit Clang
hint produced SIMD instructions but lost to the unhinted scan in a direct paired
comparison; it was removed. Stride 8 and contiguous RGBA expansion trials were
also rejected on measured regressions. Average/Paeth retain their true left dependencies
and Paeth's exact tie order. Common 128-bit portable loops may be vectorized after bounds proof.
No speculative wide loads, platform intrinsics, blind branch hints or blanket
fast-math. Start with measured scalar/dependency-correct kernels, then compare
compiler output and benchmark actual rows before selecting acceleration.

Specification reference: [W3C PNG, 24 June 2025](https://www.w3.org/TR/2025/REC-png-3-20250624/),
sections 5, 6, 8–11. Existing pinned libpng remains the test oracle; no external
PNG implementation is copied. Prior BFFT/Airspy observations remain design
references, with provenance and restrictions in the phase-2 optimization record.

## Phase completion gates still required

- Full malformed/valid corpus, pinned-libpng differential samples, fuzz targets,
  leak/undefined-behaviour checks, platform suite and measured filter kernels.
- ICC semantic validation and agreed colour-management integration, language-tag
  validation and all remaining admitted ancillary policy; raw preservation alone is not
  colour-management acceptance. Static default image is the scope already used
  by the existing PNG paths; this does not introduce animation playback.
- Shared decode output conversion/premultiplication, metadata limits chosen from
  corpus evidence, and adapter review across all hosts.
- Owned encoder on the phase-3 compressor, filter selection and deterministic
  output tests. The existing stored-block writer remains a baseline, not proof
  that the new encoder phase is complete.
- Phase-6 migration/packaging/link-map evidence and full acceptance campaigns.

## Integration record

### I2: one decode path on every host (2026-10-21)

Every PNG decode now goes through `src/core/image/png_pixels`: the owned
sample decoder, then one RGBA16 to 8-bit kernel whose rounding rule is stated
in `png_pixels.hpp` (exact nearest integer, in integer arithmetic; ties cannot
occur). The callers are:

| Path | Before | Now (purpose) |
|---|---|---|
| Windows renderer (`windows_host.cpp`) | WIC, premultiplied BGRA | owned, BGRA premultiplied (`resource`); WIC and `windowscodecs` removed |
| Skia renderer (`skia_raster.cpp`) | `SkPngDecoder` + skcms | owned, RGBA premultiplied (`resource`) |
| GUI.Drawing decode (`skia_executor.cpp`) | `SkPngDecoder` + skcms, a straight-alpha repair pass | owned, BGRA premultiplied (`drawing_decode`); the repair pass is gone |
| Linux clipboard read | `SkPngDecoder`, no colour transform | owned, RGBA straight (`clipboard`), in `src/core/host/image/clipboard_png` |
| macOS clipboard read | ImageIO drawn into an sRGB context; TIFF fallback | the same core function; TIFF fallback removed (decision 4A) |
| CoreGraphics raster | ImageIO | owned, RGBA premultiplied (`resource`); ImageIO no longer linked |

The pixel path uses `decode_png_samples_discarding_metadata`. It validates
compressed iCCP, zTXt and iTXt once through a 4 KiB scratch, with the same
checks and quotas, and stores nothing.

**Colour policy before I2, by path** (measured or read from the pinned
sources). The Skia paths converted gAMA/cHRM (and, in GUI.Drawing decode,
iCCP) to sRGB through skcms. The macOS clipboard and CoreGraphics paths let
ColorSync convert. WIC ignored gAMA and cHRM: probe PNGs decoded through a
WIC-backed decoder with a plain Pbgra32 conversion gave the same pixels with
and without gAMA 1.0, a Rec.2020 cHRM, or 16-bit samples. The Linux clipboard
ignored every colour chunk. The registry refused iCCP. The hosts disagreed,
so no single path could keep all of these behaviours.

**Decision D2, option 1 (coordinator, 2026-10-20).** I2 passes the samples
through everywhere. sRGB, gAMA and cHRM are validated structurally and
leave the samples unchanged. Stage I6 adds the owner's gAMA/cHRM conversion
everywhere. I2 and I6 are merged together, so main never carries the
pass-through alone.

**Decision D1, option 2 (owner, 2026-10-20).** iCCP is refused, with
`unsupported_color_profile`, for registry resources (`validate_png`, and the
`resource` purpose) and for GUI.Drawing decode (`drawing_decode`; the
executor reports `decode_failed`). Clipboard reads (`clipboard`, Linux and
macOS; Windows has no PNG clipboard path) ignore the profile and treat the
samples as sRGB. `icc_profile_rule` in `png_pixels.cpp` is the one switch
point.

**Identity.** `gui_forms_png_pixels_tests` checks four FNV-1a digests, one
per target layout, over 180 generated images: every colour type and depth,
Adam7, tRNS, and filters cycling 0–4. CI runs it on all four platforms, so a
pass there shows byte-identical decoded pixels. It also checks:
- the rounding rule, exhaustively for 8-bit sources and on a 16-bit lattice;
- both sides of D1;
- metadata validation without storage, including the shared work quota.

**Memory.** For a W × H image the decoder briefly holds 8·W·H bytes of
RGBA16 samples beside the caller's 4·W·H destination. Each batch (one
registry synchronization) keeps one decoder and frees it afterwards.
