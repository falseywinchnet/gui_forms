# Owned PNG encoder: default compression level (2026-10-21)

The default level (`default_png_compression_level` in
`src/core/image/png_writer.hpp`) is chosen from these measurements.

## Workload
- **Images:** the 43 decodable PNG files in `docs/library/captures/` and
  `docs/images/`, which are gallery and control screenshots. They are 92.65 MB
  of straight RGBA8 after decoding.
  - Thirteen `.png` files under `captures/` hold JPEG data; the tool skips them.
  - SHA-256 over the sorted files' bytes:
    `b337defec8cdef78ea7e0340a5b444f8b092b8d3121f5dcb9ebdee19d6be37cf`.
  - The files as committed, written by other encoders, total 14.86 MB.
- **What was timed:** each image is decoded once with the owned decoder, then
  encoded 3 times at each level 0–9 with `encode_png` as RGBA with straight
  alpha.
  - `median_ms_total` sums each file's median time; `min_ms_total` sums each
    file's minimum.
  - The time includes filter selection, deflate and chunk writing.
- **Command:**
  `gui_forms_png_encode_benchmark 3 docs/library/captures/*.png docs/images/*.png`.
- **Machine:** AMD EPYC 9354 (the workers share 8 cores), Windows 11,
  LLVM 22.1.8, `-O3` Release. The tool is single-threaded.
  - **Load:** the machine showed 100 % CPU load from other workers' builds
    during the run. Times are therefore pessimistic, and only comparable within
    this table. The sizes are exact and deterministic.

## Results
| level | encoded bytes | ratio | median s | min s |
|---|---|---|---|---|
| 0 | 92,693,148 | 1.0004 | 0.29 | 0.25 |
| 1 | 16,740,073 | 0.1807 | 1.71 | 1.64 |
| 2 | 16,408,188 | 0.1771 | 2.07 | 1.90 |
| 3 | 15,958,456 | 0.1722 | 2.54 | 2.37 |
| 4 | 15,221,049 | 0.1643 | 2.17 | 2.02 |
| 5 | 14,858,545 | 0.1604 | 2.73 | 2.54 |
| 6 | 14,560,811 | 0.1572 | 3.51 | 3.44 |
| 7 | 14,466,893 | 0.1561 | 4.96 | 4.79 |
| 8 | 14,358,704 | 0.1550 | 11.35 | 10.40 |
| 9 | 14,289,344 | 0.1542 | 32.07 | 31.24 |

## Choice: level 6
- **Size:** level 6 is 2.0 % smaller than the files as committed. Level 4 is
  2.4 % larger than them.
- **Above level 6:** levels 7–9 save another 0.6–1.9 % at 1.4 to 9 times the
  time.
- **Level 4:** it is the speed knee, 38 % faster than level 6 for 4.5 % more
  bytes.
- **Behaviour kept:** level 6 is the level the replaced SkPngEncoder paths
  used (`fZLibLevel = 6` in GUI.Drawing, and the encoder default in the
  clipboard and raster paths).
- **Throughput:** about 26 MB/s of RGBA under load, or about 0.3 s for a
  1920 × 1080 frame capture.

Callers that want speed can pass a lower `compression_level`. If the owner
prefers level 4, changing the default is one line, and the identity hash in
`png_writer_tests` must be updated with it.
