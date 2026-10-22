# Owned PNG phase: first sample-decoder increment

2026-10-19. Owner explicitly authorized starting PNG while phase 2's longer
verification gate remains open. This is the separate phase-5b draft, stacked
on the owned inflater. No production decoder/host adapter is switched.

The [reviewed contract](../../../planning/stages/Z-zlib/PNG_CODEC.md) and
[source provenance](../../../src/core/image/PNG_PROVENANCE.md) define the
boundary: full static PNG sample reconstruction, not a finished colour-managed
PNG replacement. Decode/encode integration and sustained acceptance remain open.

## Implementation and review

- All 15 legal colour/depth combinations; packed grayscale/indexed values,
  16-bit samples, all five filters, Adam7, palette/tRNS, top-down straight RGBA16.
- Full-file signature/chunk bounds/type/critical-order/CRC preflight; one IDAT
  inflater consumes chunks directly. No concatenated IDAT or full filtered
  image buffer. Exact pass-size arithmetic and two reusable rows.
- Caller-owned storage, pairwise overlap rejection, no decode allocation or
  retained borrow. Limits are copied at entry so output cannot rewrite policy.
  Prefixes remain unverified until complete success; failure discards them.
- iCCP/zTXt/iTXt compressed fields share aggregate metadata output/work limits
  across resets. Ancillary descriptors preserve encoded offsets and decompressed
  offsets. This validates compression, not ICC semantics or all text encodings.
- Colour/depth/transparency mode selection precedes pixel processing. Up's
  independent byte additions are compiler-vectorized; dependent filters keep
  exact left-neighbour dependencies and Paeth ties. No platform intrinsics.
- The existing writer's IEEE CRC now uses eight constexpr slice tables (8 KiB)
  and exact live eight-byte groups/tails. The registry's separate CRC remains
  unchanged for later consolidation. This shared helper affects existing writer
  execution, but output bytes are unchanged and its native tests still run.

## Reproducible evidence

Use the exact M4 commands in root `AGENTS.md` and the runners' `run.json` command
arrays. Local authoritative sources were mirrored through `m4host`/`m4build`.
Machine: Apple M4 Mac mini, macOS arm64, LLVM 22.1.8. Oracle sources are pinned
libpng d5515b5b8be3901aac04e5bd8bd5c89f287bcd33 and upstream zlib
51b7f2abdade71cd9bb0e7a373ef2610ec6f9daf; manifests check every used source.
No new software was installed. Source hashes accompany each experiment.

The native test matrix contains 9,000 cases with independent source/filter
construction. The optional C oracle compares lossless samples against libpng
without gamma conversion. Extra checks cover opaque selectors, dynamic Huffman
IDAT split into single-byte chunks, malformed/truncated images, palette indices,
exact storage/aliases, compressed metadata output/work reset limits, and a
settings/output alias regression. CRC has independent bitwise alignment/tail
and long-buffer comparisons, plus the standard IEEE check vector.

The sanitizer experiment instruments candidate and oracle with ASan/UBSan and
leak detection enabled, no suppressions. Its fuzz corpus begins with 150 generated
valid PNGs; CRC-repair mutation reaches decoder internals. Fuzz smoke durations
are not counted as the 24-hour gate. The LLVM RSS-monitor-thread leak workaround
is documented in `tests/image/README.md`; long campaigns still need external
RSS supervision. The initial matrix/full-suite run and the bulk-CRC full-suite
run both passed 159 macOS tests. Final policy-copy verification is recorded in
the final logs; earlier results are not substituted for that run.
Final results: **159/159 full macOS tests, 160.84 seconds; 9,000 oracle matrix
cases plus additional regressions; 4,584,550 fuzz executions in 61 seconds**
(412 MiB reported peak RSS). No sanitizer or leak report. Both whole-tree
scanners passed 1,181 C++ files; the new fuzz source passed separately. Generated
API documentation check passed. The exact full-suite helper is preserved in
`full-suite-driver.sh`; it configures the pinned runtime and logical CI workspace
needed by the hashed M4 mirror path. Its dependent fetch/build/test commands
and results are in `full-suite.log`.

## Performance scope

`benchmark-initial/` retains the initial experiment before bulk CRC and
transparency specialization. Its original decoder/header/writer/benchmark driver
and runner are preserved as `.txt` source snapshots, verified against its recorded
SHA-256 values; overlay them at their original paths in an isolated checkout to
replay that baseline. Unchanged inflater/oracle sources retain their pins.
`benchmark/` contains the current source hashes,
compiler reports, assembly and complete measurement CSVs. These are private
experimental measurements, not release or cross-platform acceptance.

Thirty workloads: 256×256 indexed8, RGBA8 and RGBA16; every filter; plain and
Adam7. Their deflate blocks are stored, deliberately isolating PNG work rather
than claiming a realistic compression corpus. Both decoders output RGBA16;
throughput counts those output bytes. Buffers/workspace are reused for the
owned path. Libpng's native create/destroy and row-table allocation are timed.
Two warmup batches precede five batches of 32 images; CSVs retain median and
range. Each workload's output is independently compared after timing.

The sanitizer oracle uses portable-C libpng. The benchmark oracle enables its
NEON implementation on M4; the candidate is portable C++. Its no-vector variant
turns off loop and SLP vectorization throughout candidate C++, including inflate,
so changes cannot be attributed to one filter alone. Compiler reports/assembly
show byte-wide 128-bit operations for Up on arm64 and XMM/SSE2 on x64. The x64
result is assembly inspection only; no x64 execution is claimed.

Initial measurements exposed a complete-decode bottleneck around the shared
byte-at-a-time CRC. Bulk CRC improved many workloads substantially; libpng still
wins several noninterlaced cases, particularly RGBA16. Timing ranges and oracle
variation also caution against small-difference claims. No “optimal decoder”
claim is made. Real compressed assets, isolated filter timings, contiguous
sample conversion, colour conversion and x64 runtime measurements remain work.


Representative final measurements (RGBA16 output MiB/s):

| Workload | Owned median | Owned range | libpng median |
|---|---:|---:|---:|
| color3-depth8-adam7-0-filter2.png | 5692.35 | 2246.77–5700.20 | 3950.66 |
| color6-depth16-adam7-0-filter0.png | 847.15 | 659.78–927.33 | 1215.61 |
| color6-depth16-adam7-0-filter2.png | 761.21 | 649.92–826.86 | 1206.46 |
| color6-depth8-adam7-0-filter0.png | 1968.49 | 1379.77–2951.80 | 1419.93 |
| color6-depth8-adam7-1-filter2.png | 1519.50 | 1358.01–2226.99 | 561.28 |

## Open phase gates and negative observations

- ICC semantic validation/colour transforms, text encoding checks and remaining
  known ancillary-chunk semantics must be completed before production use.
  Preserving an opaque ancillary payload is not semantic format validation.
- Encoder work on the owned compressor, shared pixel conversion/premultiplication,
  corpus-derived metadata policy, and platform adapter/cutover work remain open.
- Sustained per-architecture fuzzing, coverage analysis and broader performance
  workloads remain open. Cross-platform CI is additional evidence, not an
  independent security/leak audit or a substitute for those campaigns.
- Whole-image scratch comprises output storage, two rows, caller-provided
  metadata and the existing 38,192-byte inflater. The <64 KiB inflater bound
  does not claim the entire PNG image allocation fits in that size.
- The M4 helper's mirror synchronization removes remote-only dependency working
  files while retaining their Git databases. The full-suite helper restores only
  this experiment's generated pinned dependency trees before the declared fetch
  scripts reapply patches. This causes repeated dependency rebuilds; it is a
  build-host limitation, not a decoder performance result. No synchronization
  overlapped an active build.
