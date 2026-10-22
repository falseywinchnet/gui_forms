# Owned PNG code provenance

`png_decoder.cpp` and its private header are original first-party C++ written
for GUI.Forms against the PNG format rules and the owned inflate contract.
No libpng, libspng, LodePNG, BFFT or Airspy source/table is copied into them.
The governing format reference is the [W3C PNG recommendation of 24 June
2025](https://www.w3.org/TR/2025/REC-png-3-20250624/).

The existing first-party writer's `png_crc32` now computes the same IEEE CRC
in eight-byte groups using eight constexpr tables derived from its original
bit recurrence. Tables are generated from polynomial 0xedb88320, not imported.
This changes the shared checksum implementation used by the existing writer
as well as the candidate decoder; it does not select a new production decoder.
The registry still has its existing checksum pending the later consolidation.

Reference roles and exact inspected pins are in the Z-stage brief and the
phase-2 optimization record. In particular, libpng's NEON filters may rely on
private row padding; this decoder's spans require no such padding. Airspy was
consulted for layout/reduction principles only, and its proprietary-origin
comment precludes treating that source as a permissive import.

`tests/image/png_oracle.c` is an original test adapter to pinned libpng; oracle
sources are hash-verified, independently built and never shipped by this new
sample-decoder seam. Existing third-party notices and shipped dependencies
remain governed by their existing build. No leak-audit certification is claimed.
