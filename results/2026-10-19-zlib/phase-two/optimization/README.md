# Decoder optimization evidence (acceptance gate open)

Owner direction: optimize both owned decoders using BFFT's approach to compiler
visibility, simple control flow, useful vectorization and justified hints. This
moves measured inflate/Adler work into phase 2. The separate PNG parser review,
encoder, consolidated checksums, integration and platform gates remain required.
An improvement over the initial implementation is not proof of an optimal codec.

## What changed and why

- Stored blocks copy in bounded spans of at most 4096 bytes, after checking
  input/output capacity and quotas once. History publication splits at the
  physical ring boundary outside the copy operations.
- LZ matches specialize distance one with fill. Other matches copy a history
  seed and double the initialized output prefix with disjoint copies. This
  implements the LZ recurrence even when distance is less than match length;
  simply using memmove on old history would not.
- Nine-bit Huffman root tables replace repeated bit traversal for short codes.
  Long codes, short input and partially decoded symbols retain canonical
  traversal. Lookahead only reads live, budget-admitted bytes; consumption
  remains exact, including the final stream boundary.
- Literal runs use local bit state and preflight a bounded span for worst-case
  input, output and work. A nonliteral/long/invalid code exits without consuming
  that symbol. History, totals and checksum are published after the run.
- Adler uses independent byte and weighted-byte reductions. For n <= 4096,
  `high + n*low + 255*n*(n+1)/2` fits uint32. Reduction modulo 65521 occurs
  once per batch, while the short fallback retains scalar reductions. Raw
  deflate omits the unneeded checksum.

The API still performs no allocation and retains no input/output borrow.
Work units account for parser dispatch, lookup/extraction, bytes and bounded
construction; they are implementation-specific effort units, not CPU cycles
or stable counts across buffer schedules or optimization versions. Bulk
kernels validate quotas before writes. A returned prefix remains unverified
until stream_end. No new ISA, alignment or padding requirement is public.

## BFFT and PNG inspection

The pinned BFFT subset at `0f75ca79fbdc9176af729fc67afed59dca436ff1`
(`stx_vorbis/third_party/bfft`) supplies the reproducible design reference:
prepared state, caller storage, narrow kernels, implementation selection outside
loops, and explicit tail/alignment rules. Its SIMD backend and kernel audit
were inspected, together with the owner's named local BFFT documentation.
No BFFT code was copied into inflate and no dependency was added. No blanket
fast-math, forced inlining, branch-probability hint or unchecked restrict/alignment
claim was added. Branch probabilities from one trusted corpus would not justify
assuming the distribution of hostile compressed input.

The existing libpng pin `d5515b5b8be3901aac04e5bd8bd5c89f287bcd33` was
inspected in `arm/filter_neon_intrinsics.c`, `intel/filter_sse2_intrinsics.c`
and `pngrutil.c`. The NEON code uses full-width accesses and the enclosing
libpng allocation supplies alignment and padding (`row_bytes + 48`). Those
assumptions must not silently become requirements on a caller's exact-sized
span. The SSE2 three-byte helpers illustrate explicit narrow tails. This is
an implementation-reference inspection, not a new leak audit or a finding
that libpng violates its own allocation contract.

For the later PNG implementation:

| Operation | Kernel plan and necessary boundary |
|---|---|
| Up filter | Independent byte addition; contiguous SIMD interior and exact tail, with disjoint previous-row storage. |
| Sub filter | Channel-stride prefix recurrence; select bytes-per-pixel once, preserve dependencies between blocks. Benchmark useful RGB/RGBA specializations. |
| Average / Paeth | Keep left-pixel recurrence and exact predictor tie rules. Vectorize independent channel work where measured; do not assert independent pixels. |
| Palette/colour conversion | Validate palette indices and bit-depth/row extent before publication; select colour/depth operation outside the pixel loop. |
| Adam7 | Compute checked pass shapes first; each pass gets its own row state and first-row behavior. Scatter/gather costs need separate measurement. |
| CRC and Adler | Share the later checksum layer; measure complete PNG decode as well as isolated kernels before selecting ISA paths. |

No owned PNG filter kernel or end-to-end PNG performance is claimed in this PR.

## Measurement method

`tools/benchmark_inflate.py` verifies the pinned upstream zlib source and the
initial owned source at `123f13cd` by SHA-256. LLVM 22.1.8 builds all timed
implementations with `-O3 -DNDEBUG`, C++20 for owned code, no LTO and no explicit
architecture flags. The candidate is also built with
`-fno-vectorize -fno-slp-vectorize` as an ablation. The upstream comparison
uses its ordinary build, not INFLATE_STRICT, and does not carry our resource
quota checks. It is the upstream pin, not Chromium zlib or zlib-ng.

The whole-buffer benchmark uses 256 KiB zero, deterministic UI-pattern and
high-entropy bytes, compressed by the pinned oracle using level 0, level 6,
and level 6/Huffman-only, raw and zlib framing. Tiny 256-byte versions expose
setup cost. These are synthetic warm-cache workloads, not Silesia, real UI
raster/PNG coverage, one-byte throughput, or an end-to-end application result.
Fixture creation, allocation, file output and byte verification are outside
the timer; reset and required success/progress checks are inside. Stream and
output storage are reused. Each implementation calibrates a batch to at least
50 ms, runs two warmup batches, and reports five batches' median and range.
Machine, flags, source/corpus hashes and exact commands are retained in run.json.

The `bulk-first` measurements used only 12 iterations per sample. Their fastest
cases were too short and showed large warmup variation; they are retained as
preliminary evidence and are not used for final speed claims. `lookup` contains
the next intermediate result with 50 ms calibration. The first final runner
attempt failed before timing because the mirrored `.git` worktree file points
to a path on the authoritative host; the runner now accepts a hash-verified
baseline export. The decoder's preceding strict/ordinary smoke runs passed.

## Airspy reference and common-width direction

The owner additionally named Downloads' `airspy_neon_ddc.cpp` and required the
common SSE/NEON width, without programming separate SIMD platforms. Both local
copies were read/compared and are byte-identical: 452 lines, SHA-256
`7cb80a48f3ef46036b244368e724c2502b1f35be525fa0f965be54a57814d7ed`.
The file identifies its scalar material as proprietary; no source, FIR tables
or licensing assumptions from it are copied into GUI.Forms.

The transferable design is a prepared data layout that removes hot-loop
permutations, several independent accumulators to hide dependency latency,
and late horizontal reduction. Its 128-bit shape fits the processing stride;
wide-register emulation is not inherently better. Its benchmark comments are
not our measurements. Unrolling, always-inline/hot attributes and fast-math
from that floating-point experiment are not adopted without codec evidence.

The codec's integer reductions remain portable C++, with common 128-bit lane
groups as the SIMD design target and exact scalar tails. The compiler may use
multiple independent 128-bit groups; this is not a 256/512-bit algorithm or an
ISA-specific source fork. ARM assembly inspection and an SSE2-only assembly
build are separate evidence from executable tests; no x64 runtime performance
is inferred from cross-compilation.

The literal-run trial identified a real tiny fixed-Huffman regression (about
19–26% versus the original implementation). Its raw results are retained in
`literal-run-trial`. The corrected implementation prepares the immutable fixed
alphabets at compile time and copies the bounded tables into the workspace;
stream-dependent dynamic alphabets still receive runtime validation. The first
trial completed timing but its metadata step failed because a restricted PATH
omitted `/usr/sbin/sysctl`; the final runner uses that explicit system path.

The earlier PR CI also exposed a missing API-documentation routing entry for
the new compression header. The compression reference page and type summaries
now participate in the existing generator; its 11 tests pass locally.

## Final measured results

Apple M4, 16 GiB, Darwin 25.5.0, LLVM 22.1.8. Median MiB/s, uncompressed
bytes per second. Strategy 0 is default and 2 is Huffman-only. All large inputs
are 256 KiB; tiny inputs are 256 bytes. Exact spreads for every case are in
`final/{baseline,candidate,no-vector}.csv`.

| Workload | Original owned | Optimized owned | Relative to original | Pinned upstream |
|---|---:|---:|---:|---:|
| ui; level 6; strategy 0; zlib | 265.8 | 2534.8 | 9.54x | 1816.2 |
| ui; level 6; strategy 2; zlib | 83.5 | 227.6 | 2.73x | 382.3 |
| entropy; level 0; strategy 0; zlib | 187.9 | 6475.8 | 34.46x | 3728.3 |
| zero; level 6; strategy 0; zlib | 289.5 | 4664.4 | 16.11x | 782.4 |
| tiny-ui; level 6; strategy 0; zlib | 122.5 | 1311.3 | 10.70x | 1417.6 |
| tiny-zero; level 6; strategy 0; zlib | 139.7 | 1973.4 | 14.12x | 1653.4 |
| entropy; level 0; strategy 0; raw | 191.7 | 24122.5 | 125.84x | 80143.7 |

All measured candidate workloads improve over the original owned source. This
is not a claim to beat upstream everywhere: literal-heavy Huffman-only UI
data, raw stored blocks and tiny UI streams remain slower than upstream here.
The vectorization-disabled comparison attributes roughly 1.9x on the stored
zlib workloads to compiler vectorization, and about 1.36x on default-compressed
zlib UI data. Not every ablation favors the vectorized binary: raw level-0
entropy is about 18% slower than its no-vector build in this run, despite no
checksum on that path; the cause is unresolved. Retain that result for wider
workload/alignment testing rather than attributing it to noise without evidence.

Compiler reports and assembly show 128-bit NEON load/accumulate groups and late
`addv` reductions in the Adler kernel. The separate generic x64 build shows
SSE2 `pmuludq`/`paddd`/`pshufd` operations on XMM registers, with no YMM/ZMM
usage. The first x64 attempt needed an explicit macOS SDK sysroot; its failed
compiler log is retained. The first successful Apple-target assembly
(`candidate-sse2.s`, a historical filename) allowed later SSE operations; the
corrected `candidate-x64-baseline.s` adds `-march=x86-64 -mtune=generic`. The
runner now applies that correction. Neither x64 assembly build was executed.

## Final verification and limits

- Strict oracle smoke: 210394 executions in 61 seconds; ordinary oracle smoke:
  233787 executions in 61 seconds. Both pass ASan/UBSan/LSan without reports or
  differential disagreement. Final-source smoke time is 122 seconds, not 24
  hours; earlier trials are separate and do not extend that source's campaign.
- Each oracle configuration passes 700 generated streams, each decoded both
  one-byte-at-a-time and whole-buffer, plus the native boundary/error suite.
- Workspace is 38192 bytes, up 3072 bytes for bounded root tables. The `-O3`
  decoder frames are all static and sum conservatively to 816 bytes; workspace
  plus that overestimate is 39008 bytes. Immutable fixed tables occupy another
  3272 bytes of shared read-only storage. Caller buffers/runtime are separate.
- Native-fixture coverage: 93.63% regions, 91.17% lines, 88.55% branches; 17 of
  18 functions executed. These raw figures include the compile-time fixed-tree
  factory, which deliberately has no runtime execution. Other uncovered
  branches still require review. No full-coverage claim.
- Full macOS project build and sanitizer CTest: 158/158 passed in 138.04 seconds.
  The prebuilt Skia archive is outside the helper's sanitizer instrumentation;
  the new decoder is instrumented. `final/full-suite.log` retains the command
  and results. This is not a Linux/Windows sanitizer result.
- House-style and lifetime scans: 1176 first-party files, zero findings each;
  the fuzz target separately passes both scans. API-reference generator: all
  11 tests pass after adding the compression page routing.

All log/assembly text is retained with trailing whitespace normalized for Git.
The final runner's only change after the numeric run adds the generic x64
architecture flags to its optional assembly inspection; the timed decoder,
header and benchmark source hashes match the retained run metadata.

The phase gate remains open: sustained x64/arm64 fuzzing, the remaining
platform sanitizer matrix, broader corpus/alignment/streaming benchmarks, and
review of unvisited branches are still required. PNG is an inspected design
reference here; its owned implementation, differential pixels and complete
PNG throughput belong to its separately reviewed phase. No production cutover
or reference-library removal is made.
