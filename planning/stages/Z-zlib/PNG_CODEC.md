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

### I6: gAMA and cHRM converted to sRGB (owner, 2026-10-20)

The owner accepted converting gAMA and cHRM to sRGB on 2026-10-20, in a
first-party, deterministic conversion, while still refusing iCCP. Full ICC
support is a later study. The coordinator accepted the three detailed points
the same day:
- a cICP chunk with sRGB-equivalent values means sRGB, and any other cICP is
  refused;
- gAMA 45455 is converted literally, with no "approximately sRGB" special case;
- cHRM without gAMA assumes the sRGB transfer function.

The conversion is implemented in `src/core/image/png_color`. Every host uses
it through `PngPixelDecoder`, after decoding and before the 8-bit kernel.

**Precedence** follows the PNG specification (third edition, 4.3): cICP,
then iCCP, then sRGB, then cHRM and gAMA. An iCCP chunk that the purpose's
rule ignores (clipboard reads) means sRGB, so gAMA beside it is not applied.
The registry also refuses a cICP that is not sRGB-equivalent at load time
(`unsupported_color_profile`). It reports a malformed cICP (wrong length,
duplicate, or after PLTE/IDAT) as `invalid_color_metadata`.

**Pipeline.** All arithmetic is in doubles, with floating-point contraction
off, using the engine's deterministic `engine_power` and no host libm:
1. **Decode.** linear = (sample / 65535) ^ (100000 / gAMA). Without a usable
   gAMA, the sRGB decoding curve is used.
2. **Gamut matrix.** M = inverse(sRGB primaries) × Bradford(image white → D65)
   × image primaries, built from the cHRM chromaticities with each primary
   scaled to the image white. The matrix step is skipped when there is no
   usable cHRM.
3. **Clip.** Each channel is clipped to [0, 1].
4. **Encode.** The result is the 16-bit code found by binary search over
   65535 precomputed thresholds, each the sRGB decoding of a midpoint between
   adjacent codes.
   - Rounding: the nearest code; a value exactly on a threshold takes the
     upper code.
   - Error bound: within one 16-bit code of the exact result.
   - Tables: images of 8 bits or fewer use a 256-entry decode table, since
     every such sample is a multiple of 257; 16-bit images use 65536 entries.
     Without a matrix, a per-sample output table replaces the per-pixel search.

**Degenerate values are ignored, as if the chunk were absent, and reported.**
- A gAMA value outside 16..625000000. That is the range the previous libpng
  paths accepted, so files they honoured keep their gamma.
- A cHRM with any y = 0, x + y > 1, a singular primary matrix, or a white
  point outside the primaries' triangle.

The PNG specification allows a decoder to ignore an erroneous ancillary
chunk. Structural errors (length, order, duplicates, gAMA 0) remain refused,
per decision 5A.

**Tests** (`gui_forms_png_color_tests`) compare against references computed
with the host's `std::pow` (the references may use it; the code may not):
- gAMA 45455 alone, gAMA 1.0, and Rec.709 cHRM with gAMA 45455: within one
  8-bit code;
- Rec.2020 cHRM with gAMA 45455, against the four-decimal ITU-R BT.2087
  matrix: within two 8-bit codes;
- cHRM without gAMA: within one code.
They also cover precedence, cICP, an ignored iCCP, degenerate values, and
identity hashes over three conversion cases × four formats × Adam7 (checked
on all four CI platforms): `0x44de97c1f2726691`, `0xf284508a59809f75`,
`0x66e109f833f56831`.

### I3: one owned encoder on every host (2026-10-21)

Every PNG encode now goes through `encode_png` in `src/core/image/png_writer`:
the Windows frame capture, the Skia raster capture, GUI.Drawing's
`encode_png`, and the clipboard writes on Linux and macOS (through
`detail::encode_clipboard_png`). `SkPngEncoder` and `NSBitmapImageRep` PNG
encoding are no longer called. The stored-block writer is replaced:
- each row's filter is the one with the minimum sum of |signed byte|, with a
  tie taking the lower filter type; level 0 writes filter 0;
- the IDAT is compressed by stx_zlib at the requested level, default 6
  (measured in [results/2026-10-21-png-encoder](../../../results/2026-10-21-png-encoder/README.md));
- premultiplied sources are unpremultiplied with a stated nearest rule;
- an sRGB chunk is written.

The output is deterministic. `png_writer_tests` checks an identity hash over
six cases (RGBA and BGRA; straight, premultiplied and opaque; levels 0, 1, 6
and 9) on all four CI platforms. It also checks round trips through the owned
decoder at every level, and that a premultiplied image survives encoding and
decoding exactly.
