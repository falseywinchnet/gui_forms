# Checksum consolidation: inflate timing check (2026-10-21)

Phase 4 moved inflate's inline Adler-32 into the shared
`stx_zlib::adler32_update` in `stx_zlib/src/checksum.cpp`, which is a
separate translation unit. This check looks for a cost from that move.

- **Workload:** the zlib streams (concatenated IDAT payloads) of the 43
  decodable PNGs in `docs/library/captures/` and `docs/images/`.
  They inflate to 71,368,552 bytes.
- **Builds:** inflate before the change (`stx_zlib/src/inflate.cpp` at the
  parent commit) and after it (plus `checksum.cpp`), both with clang++ 22.1.8
  `-O3 -DNDEBUG`, Windows x64, on the shared workers' machine under load.
- **Method:** six alternating pairs. Each run decodes the whole set 9 times
  and reports the minimum and median.

| pair | before min / median (ms) | after min / median (ms) |
|---|---|---|
| 1 | 604.4 / 612.7 | 591.0 / 607.8 |
| 2 | 581.8 / 594.3 | 570.9 / 597.1 |
| 3 | 576.7 / 585.2 | 573.7 / 582.3 |
| 4 | 578.5 / 597.1 | 575.1 / 586.3 |
| 5 | 571.6 / 584.6 | 578.5 / 590.3 |
| 6 | 576.1 / 590.4 | 570.7 / 597.7 |

The difference is inside the run-to-run spread; the mean of the minima is
581.5 ms before and 576.7 ms after. Deflate's Adler-32 was the same kernel
inline and is now one call per input copy of at most 32 KiB; it was not
timed. The checksum values are unchanged: the PNG encode and decode identity
hashes still pass.
