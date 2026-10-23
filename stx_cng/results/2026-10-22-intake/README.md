# Intake measurement — 2026-10-22 (provisional)

**Provisional:** the machine's CPU load was 97 to 100 % for the whole run
(other builds were running). Ratios between codecs measured in interleaved
rounds share that load; absolute times do not mean much. Re-measure on an idle
machine before quoting speed.

## What was measured

- **Machine:** Windows 11 virtual machine, AMD EPYC 9354, 4 cores / 8
  threads, 16 GB. Pinned LLVM 22 toolchain, Release (`-O3`), statically
  linked.
- **CNG:** this package (hardened, reused `CngEncoder` and `CngDecoder`,
  8-bit output). The rc1 columns compare with upstream 0.1.0-rc1 as-is
  (per-call contexts, iostreams), measured in the same way on 2026-10-22.
- **PNG:** in-tree `stx_png`, encoder level 6 (level 9 sizes from the rc1
  run), decoder with a reused `InflateStream` and RGBA16 output.
- **Inputs:** 238 images, SHA-256 of each in `inputs.tsv` (which also lists
  the 19 rows that did not load, all JPEG files named `.png`): Windows
  SystemApps icons at target sizes 16 to 256, Gallery captures, screenshots,
  ribbon text-UI captures, wallpapers (`photo_full`), and 1920 x 1200 centre
  crops of wallpapers (`photo_crop`). Opaque images use 3 channels for CNG and
  colour type 2 for PNG; others RGBA.
- **Timing:** minimum wall time over at least three interleaved rounds (PNG
  decode, CNG decode, PNG encode, CNG encode per round), continued until
  0.3 s or 200 rounds per image. Raw rows: `bench.tsv`.

## Byte identity with rc1

Every image both versions admit (208) encodes to the same bytes (same size
and CRC-32) as rc1. The other 30 are wallpapers rc1 refused (2736 x 1539 and
3840 x 2160 or 2400, above its padded 4,194,304-pixel extent); they are new.
Every CNG file decoded back to its samples exactly.

## Results

Bytes are sums over the class. Speeds are medians of per-image ratios
(higher means CNG faster); "rc1/new" above 1 means this package is faster
than rc1.

| class | n | CNG/PNG6 bytes | CNG/PNG9 bytes | decode PNG/CNG | encode PNG6/CNG | CNG decode rc1/new | CNG encode rc1/new |
|---|---:|---:|---:|---:|---:|---:|---:|
| icon16 | 15 | 1.101 | 1.101 | 2.9x | 3.4x | 1.45 | 3.05 |
| icon24 | 22 | 1.091 | 1.107 | 3.0x | 4.9x | 1.36 | 2.79 |
| icon32 | 15 | 1.107 | 1.144 | 3.2x | 4.8x | 1.09 | 2.19 |
| icon48 | 18 | 1.063 | 1.112 | 3.7x | 7.0x | 0.95 | 1.94 |
| icon64 | 13 | 1.053 | 1.117 | 3.7x | 8.0x | 0.94 | 1.89 |
| icon96 | 17 | 1.000 | 1.070 | 4.5x | 9.9x | 0.96 | 1.70 |
| icon256 | 19 | 0.928 | 1.017 | 6.6x | 15.7x | 1.00 | 1.41 |
| gallery | 41 | 0.699 | 0.713 | 7.1x | 8.6x | 1.05 | 0.97 |
| screenshot | 7 | 0.674 | 0.617 | 12.1x | 16.6x | 1.20 | 1.66 |
| text_ui | 26 | 0.837 | 0.858 | 8.9x | 10.6x | 1.48 | 1.57 |
| photo_crop | 11 | 0.910 | 0.967 | 4.7x | 8.4x | 1.40 | 1.45 |
| photo_full (30 new) | 34 | 0.846 | 0.929 (4 rc1) | 5.4x | 8.3x | 1.18 | 1.18 |

Storage after the whole corpus (each owner grown to its largest band, the
2736 x 1539 RGB wallpapers): encoder 7,874,940 bytes, decoder 6,405,196
bytes, Zstandard contexts included. Both are below the enumerated maxima in
`docs/HARDENING.md`. Small icons alone need about 1.57 MB (encoder, mostly the
level-3 compression context) and 0.1 MB (decoder).

## Reading

- On GUI assets above icon size, CNG files are 14 to 38 % smaller than PNG
  at level 9 and decode 7 to 12 times faster than `stx_png`.
- Small icons (16 to 64) are 5 to 14 % larger than PNG 9. Packed into one
  composite collection per size class (rc1 measurement, step 2), each class
  but 64 came to 0.85 to 0.88 of the PNG 9 sum (64: 1.005); collections are a
  later change.
- Removing per-call contexts made small-image encoding 2 to 3 times faster
  than rc1 and small-image decoding up to 1.45 times faster; large images are
  dominated by the kernels, which are unchanged.
