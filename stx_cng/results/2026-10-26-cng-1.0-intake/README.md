# CNG 1.0 intake measurement — 2026-10-26

CNG 1.0 (this package) against `stx_png` on the 2026-10-22 intake's own
workload: file size, and decode time at one and four threads.

## What was measured

- **Machine:** Windows 11 virtual machine, AMD EPYC 9354 (3.25 GHz), 4 cores
  / 8 threads, 16 GB (17,159,348,224 bytes). Pinned LLVM 22 toolchain,
  Release (`-O3`), statically linked. CPU load from other work was about 20 %
  at the start and end of the run, so absolute times carry some noise; the
  codecs were timed in interleaved rounds and share it.
- **CNG:** this package, one reused `CngEncoder` and one reused `CngDecoder`,
  decoding into a tight BGR8 or BGRA8 destination.
- **PNG:** in-tree `stx_png`, encoder levels 6 and 9, decoder with a reused
  `InflateStream` and RGBA16 output (as on 2026-10-22). The timed decode is of
  the level-6 file.
- **Inputs:** the 2026-10-22 corpus (`../2026-10-22-intake/inputs.tsv`),
  rebuilt on this machine by SHA-256 (`find_corpus.py`) from the Windows
  SystemApps icons, the repository's Gallery, screenshot and text-UI
  captures, and the Windows wallpapers converted to PNG as on 2026-10-22
  (`convert_photos.ps1`; every hash matches). 248 of the 257 listed files
  were found (`inputs.tsv`, with SHA-256); the 9 missing are 8 screenshots no
  longer in the repository (macOS and native-folder captures) and the Gallery
  `.ppm`. 16 found files are JPEG data named `.png` and are skipped, as
  before, leaving **232 images**. Opaque images are three-channel CNG and
  colour-type-2 PNG; the others four-channel and RGBA.
- **Method** (`bench.cpp`): each image is decoded by `stx_png` to 8-bit
  samples, encoded once by `stx_png` at levels 6 and 9 and once by CNG at four
  threads (single timed runs), and the CNG file must decode back to the
  samples exactly (all 232 did). Decode times are the minimum wall time over
  interleaved rounds (PNG, CNG at one thread, CNG at four threads), at least
  three and continued until 0.3 s or 200 rounds. Raw rows: `bench.tsv`;
  table: `summarize.py bench.tsv ../2026-10-22-intake/bench.tsv 4`.

Built against this package's Release tree and an installed
`stx_png`/`stx_zlib`:

```text
clang++ -std=c++20 -O3 -DNDEBUG -Istx_cng/include -I<sdk>/include bench.cpp \
    <rel>/libstx_cng.a <rel>/libstx_cng_zstd.a <sdk>/lib/libstx_png.a <sdk>/lib/libstx_zlib.a -static -o bench.exe
bench.exe <corpus> bench.tsv 4
```

## Results

Bytes are sums over the class; speeds are medians of per-image ratios
(above 1 means CNG is faster). "CNG 1.0/0.1" compares with the 0.1 sizes of
2026-10-22 for the same images.

| class | n | CNG/PNG6 bytes | CNG/PNG9 bytes | CNG 1.0/0.1 bytes | decode PNG/CNG, 1 thread | decode PNG/CNG, 4 threads | CNG 4 threads/1 |
|---|---:|---:|---:|---:|---:|---:|---:|
| icon16 | 15 | 1.287 | 1.303 | 1.145 | 0.57x | 0.58x | 1.01x |
| icon24 | 22 | 1.202 | 1.238 | 1.038 | 0.61x | 0.61x | 1.00x |
| icon32 | 15 | 1.081 | 1.117 | 0.958 | 0.67x | 0.68x | 1.01x |
| icon48 | 18 | 1.167 | 1.230 | 1.073 | 0.70x | 0.28x | 0.38x |
| icon64 | 13 | 1.031 | 1.100 | 0.947 | 0.70x | 0.33x | 0.48x |
| icon96 | 17 | 0.923 | 1.001 | 0.890 | 0.71x | 0.35x | 0.55x |
| icon256 | 19 | 0.791 | 0.900 | 0.806 | 0.71x | 0.98x | 1.42x |
| gallery | 40 | 0.732 | 0.747 | 1.046 | 1.32x | 3.87x | 2.88x |
| screenshot | 2 | 0.722 | 0.735 | 0.975 | 1.04x | 3.29x | 3.16x |
| text_ui | 26 | 1.028 | 1.053 | 1.228 | 1.13x | 2.73x | 2.44x |
| photo_crop | 11 | 0.777 | 0.825 | 0.853 | 1.22x | 3.71x | 2.87x |
| photo_full | 34 | 0.764 | 0.806 | 0.903 | 1.32x | 3.64x | 2.78x |
| all | 232 | 0.764 | 0.804 | 0.912 | | | |

Summed decode time over the 232 images: PNG 7,457 ms, CNG at one thread
5,549 ms, CNG at four threads 2,047 ms.

Median decode times (microseconds):

| class | PNG | CNG, 1 thread | CNG, 4 threads |
|---|---:|---:|---:|
| icon16 | 8.6 | 16.1 | 16.0 |
| icon48 | 32.4 | 44.1 | 123.4 |
| icon256 | 368.5 | 580.1 | 413.7 |
| gallery | 9,616 | 7,284 | 2,484 |
| text_ui | 2,872 | 2,504 | 1,039 |
| photo_full | 179,445 | 134,718 | 46,441 |

Encoding (single runs, not the subject of this measurement): summed over the
corpus, PNG level 6 took 40.6 s and CNG at four threads 267.3 s.

## Storage

`storage_probe.cpp` (`storage.txt`), on a 1,024 x 256 image of 64 full
segments, owners fresh for each call:

| Owner | Image | Bytes | Per sample |
|---|---|---:|---:|
| Decoder, 1 thread | any | 552,488 | — |
| Decoder, 4 threads | any | 614,144 | — |
| Decoder, 64 threads | any | 1,847,264 | — |
| Encoder | BGR noise | 6,059,696 | 7.71 |
| Encoder | BGR flat | 1,841,824 | 2.34 |
| Encoder | BGRA noise | 7,115,870 | 6.79 |
| Encoder | BGRA flat | 1,842,362 | 1.76 |

Over the whole corpus the decoder stayed at 614,144 bytes (four threads) and
the encoder grew to 209,770,188 bytes for the largest image (3840 x 2400 BGR,
7.6 bytes per sample).

## Reading

- **Size.** CNG 1.0 is 23.6 % smaller than PNG level 6 and 19.6 % smaller
  than level 9 over the corpus, and 8.8 % smaller than CNG 0.1. It wins on
  photographs, Gallery captures and large icons; it loses on icons of 48
  pixels and below (up to 30 % larger: its fixed parts, the duplicated header,
  directory, tables and mosaic, are a large share of a small file) and is
  about even on text-UI captures, where it is also 23 % larger than 0.1.
- **Decode at one thread.** CNG is 4 to 32 % faster than PNG on images
  above 256 x 256 (medians per class) and takes 1.4 to 1.75 times PNG's time
  on icons: a file costs a fixed amount (decompressing and building 38
  frequency tables) of about 8 µs more than PNG's.
- **Decode at four threads.** Large images scale 2.4 to 3.2 times over one
  thread, and CNG then decodes them 2.7 to 3.9 times faster than PNG (which
  has no parallel decode). For small images with more than one segment,
  starting threads costs more than it saves (icon48 to icon96 take 1.8 to
  2.6 times as long at four threads as at one); a single-segment image runs on
  the calling thread alone. Callers should use one thread below a few hundred
  thousand samples. GUI.Forms' seam decodes at one thread.
- **Encoder storage** grows with the image, up to about 8 bytes per sample
  (`docs/HARDENING.md`, finding F3); the decoder's is fixed by the thread
  count.
