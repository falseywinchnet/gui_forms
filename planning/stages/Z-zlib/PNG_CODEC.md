# Owned PNG phase

Started 2026-10-19 on the owner's instruction, “lets begin the png phase.”
This advances phase 5b alongside the still-open inflate verification gate;
it does not assert that phase 2's sustained fuzz gate passed. Separate stacked
PR; production cutover remains phase 6.

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
Decode revalidates rather than trusting a caller-modifiable inspection record.
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
compressed fields; no parser-created borrow outlives the call.

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
- ICC semantic validation and agreed colour-management integration, text encoding
  semantics and all admitted ancillary policy; raw preservation alone is not
  colour-management acceptance. Static default image is the scope already used
  by the existing PNG paths; this does not introduce animation playback.
- Shared decode output conversion/premultiplication, metadata limits chosen from
  corpus evidence, and adapter review across all hosts.
- Owned encoder on the phase-3 compressor, filter selection and deterministic
  output tests. The existing stored-block writer remains a baseline, not proof
  that the new encoder phase is complete.
- Phase-6 migration/packaging/link-map evidence and full acceptance campaigns.
