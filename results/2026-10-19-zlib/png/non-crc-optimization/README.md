# PNG optimization and independent CRC evaluation

Evaluation performed 2026-10-20 against the owned codec at `38390afbb5781e14ec653a3a614253f7179250b5`.
This directory remains under the phase's original 2026-10-19 evidence root.
Production CRC remains slicing by eight. Candidate circuit measurements do not
constitute a production switch, universal optimum, completed leak audit, or
acceptance of the whole PNG phase.

## Computational suitability decision

**Retain the degree-261 quartet as the bounded-memory large-IDAT candidate. Do
not use it as the sole CRC algorithm. Retain direct degree-300/128 as a competing
bulk-throughput candidate; reject the simple scaled degree-261/128 form as the
preferred implementation for these measured workloads.** This selects an
algorithmic direction, not a production dispatch threshold or universal optimum.

The verified baseline-ISA run uses `-mcpu=generic -march=armv8-a`. Candidate
assembly contains ordinary 128-bit XORs and no EOR3, BCAX, CRC32 or PMULL. Values
below are medians of paired speedups over the current PNG slicing-by-eight CRC;
setup, seed, flush, final reduction and tails are included.

| CRC span | braid5×8 | degree-261 quartet | direct degree-300/128 |
| --- | ---: | ---: | ---: |
| 4 KiB | 2.69× | 1.40× | 0.52× |
| 16 KiB | 2.60× | 2.70× | 1.58× |
| 32 KiB | 2.73× | 3.67× | 2.54× |
| 65,539 B | 2.33× | 3.91× | 3.00× |
| 128 KiB | 2.47× | 3.35× | 4.11× |
| 256 KiB | 2.99× | 3.65× | 4.42× |
| 1 MiB | 2.68× | 3.76× | 4.64× |

At 65,539 B the quartet's paired range was 3.34–4.64×. At 1 MiB the quartet
range was 3.25–5.99× and direct-300 range 3.48–5.65×; overlapping ranges prevent
claiming a precise crossover or a statistically established ranking everywhere.
At 512 B the quartet was only 0.237× baseline speed, and at 17 B only 0.00522×.
Small headers and tails must avoid polynomial initialization/termination. The
braiding/slicing family remains useful there.

The actual generated IDAT CRC spans are **65,539 B**, repeated as needed, plus
a tail of 277–543 B and a legal empty-IDAT CRC span of 4 B. The red-pixel asset
uses a 17 B IDAT CRC span. Thus a 512 KiB decoded-image workload does not justify
selecting a kernel from its 512 KiB contiguous-CRC ranking. The measured quartet
hybrid (slice8 below 64 B, braid4×8 below 32 KiB, quartet above) achieved these
whole-decode paired medians on the verified baseline-ISA build:

| Filter-0 PNG | Plain | Adam7 |
| --- | ---: | ---: |
| indexed8 | 1.24× | 1.14× |
| RGBA8 | 1.69× | 1.83× |
| RGBA16 | 1.81× | 1.60× |

The tiny asset was 1.03× with a 0.84–1.99× range: no reliable tiny-image speedup.
Indexed8 Adam7 also had a range crossing one. The 32 KiB threshold is an explicit
trial, not a fitted universal optimum, and the hybrid is not asserted to have
optimal small-chunk dispatch. The larger RGBA results support practical PNG
suitability under these conditions. They do not establish photographic-image,
cold-cache, x64, or concurrent-worker performance.

Keep production CRC unchanged until those qualifications and the outstanding
register-allocation question are resolved. The quartet is compatible with
independent segment computation and shared immutable reduction/combination work;
its measured memory advantage is real. Parallel throughput and an optimal worker
budget remain unmeasured. The deployed Chorba implementation also remains a
reference, so this comparison does not establish supremacy over every available
CRC implementation.

## Changes and bounds

The Sub filter now uses a 16-byte, channel-strided prefix scan for strides 1
and 4 only. First bytes, short rows and exact tails are separate. Arithmetic is
modulo 256; no padded input or speculative reads are required. The explicit Clang
loop-hint trial produced ARM `add.16b` and generic x64 SSE2/no-AVX byte shifts
and `paddb`, but the direct comparison rejected the hint. The retained scan is
unhinted portable C++; measured ARM lowering uses scalar instruction parallelism.
The no-vector control also disables the hint when evaluating its historical
snapshot. SIMD spelling alone did not establish a performance win.

Rejected trials are retained with source snapshots. Contiguous RGBA expansion
had mixed gains and regressions without vectorization. Applying the Sub scan to
all strides regressed RGBA16 (roughly 0.80–0.95× in the all-strides trial).
Strides 2, 3 and 6 have not earned selection. No filter or format is removed.

Writer review found two independent safety defects: overflow in the required
input extent when an untrusted row stride was multiplied, and invalidation of an
input pixel borrow when it overlapped the output vector being cleared. Validation
now proves the extent by division and detects overlap before clearing. Tests
exercise multiplication/addition overflow, both overlapping offsets, valid
one-row inputs whose unused stride is huge, and invalid channel-order values.
Failure leaves output empty.

## Measurement method

The PNG benchmark links a renamed historical decoder beside the candidate. Both
share the current inflate and CRC implementations, so their paired difference
isolates non-CRC decoder changes. Each mode calibrates to 50 ms per batch; two
warmup and seven retained rounds rotate execution order. All output is compared
with pinned libpng outside timing. Ratios are medians of paired ratios, not
ratios of separately aggregated medians.

CRC trials calibrate to 25 ms with the same warmup/retained count and rotating
order. The complete CRC call is charged: initialization, seed injection, flush,
terminal map, exact tails and dispatch. PNG CRC substitution changes only the
checksum invoked by the decoder. The seven PNG inputs comprise six generated
256×256 stored-block PNGs (indexed8, RGBA8 and RGBA16, plain and Adam7) and the
repository red-pixel asset. This exposes checksum cost; compressed production
images with substantial inflate work may show a smaller whole-decode benefit.

Measurements run sequentially on the nearby M4 Mac mini, LLVM 22.1.8. Other user
workloads remain running; some rounds have wide ranges. JSON records include
compiler, source/workload hashes, dependency pins and exact commands. CRC records
also include load snapshots. x64 is assembly inspection, not performance data.
No cold-cache, concurrent-worker scaling or x64 timing claim is made.

## CRC construction and evidence

The sibling's degree-261 recurrence is implemented independently in `compact.cpp`.
The unchanged first-party slicing/braiding kernels were copied from sibling
commit `3a0ce443`; no external implementation was copied into production.
The circuit has 209 quotient words plus 52 auxiliary words. Its four-step version
uses thirteen named 128-bit auxiliary values and an exact 209-word quotient ring.
The three possible split starts wrap explicitly. A 52-word zero flush and
terminal reduction are required and are measured. The terminal table is 4 KiB,
immutable and shared across calls. The final variant reuses existing state during
termination instead of allocating a second quotient ring.

The direct degree-300 control has delays 145, 183, 211 and 300 in reciprocal
orientation. This is a recurrence-level control based on the published
[Chorba construction](https://arxiv.org/html/2412.16398v1), not a measurement of the
deployed Chorba implementation. The full reciprocal polynomial is `0x1db710641`.
The 32-, 64- and 128-bit scaled degree-261 variants are separate measurements.

The initial quartet retained helper calls. Inlining removed calls but an indexed
auxiliary array still spilled. Naming thirteen auxiliary values improved the
schedule; the hinted version exposes 128-bit XORs. Default Apple-target lowering
also uses `eor3.16b`, so an additional baseline Armv8-A trial excludes that extra
instruction when judging common SIMD suitability. The first attempt with `-march=armv8-a` alone failed that restriction: the
result still contained EOR3 because the default Apple CPU remained selected.
The corrected command selects `-mcpu=generic` too and rejects extra-ISA opcodes
in a preflight assembly check before spending time on benchmarks. Generic CPU
selection changes tuning too; this is a conservative ISA control, not an isolated
measurement of the EOR3 instruction.
A no-call result alone is not
proof of the theoretical fifteen-register schedule, particularly on x64.

All candidates are checked against pinned upstream zlib. Small messages also use
an independently expressed bit oracle. Coverage includes randomized contents and
seeds for streaming candidates, 16 alignments, exact allocation tails with ASan
redzones, split-call seeds,
constant inputs, the IEEE check value and threshold/ring boundaries. Four
independent segments combine using one prepared immutable shift operator and
match the serial checksum. This proves that construction's combination semantics;
it does not establish a thread-pool throughput win or an optimal worker budget.

## Sub selection

The final decoder is byte-for-byte `selected-decoder.cpp.txt`: stride 1 and 4
prefix scans without the explicit Clang hint. Against the historical scalar
recurrence, the retained `selected-benchmark/vector.csv` paired medians are
1.5752×/1.7501× for indexed8 plain/Adam7 and 1.5345×/1.4292× for RGBA8 plain/Adam7.
The unchanged RGBA16 controls are 0.9767×/0.9951×. The RGBA8 Adam7 range crosses
one (0.8368–1.5946), so the median is not a universal win on every round.

The direct hint ablation gave these ratios of hinted to unhinted throughput:

| Sub workload | Median | Retained range |
| --- | ---: | ---: |
| indexed8 plain | 0.9278× | 0.7296–1.2346× |
| indexed8 Adam7 | 0.8701× | 0.7480–0.9988× |
| RGBA8 plain | 0.9232× | 0.7628–1.1073× |
| RGBA8 Adam7 | 0.7834× | 0.7308–0.9381× |

The no-vector hint-ablation control disables the hint on both variants and
shows scheduling variation even for equivalent source. That noise limits close
rankings, but provides no reason to retain a hint that loses every retained
round in both affected Adam7 cases. The already-independent Up filter continues
to vectorize; this decision concerns the dependent Sub scan only.

After measurement, harness cleanup names chunk offsets/counts in `ChunkSpan`
instead of positional pairs and names elapsed seconds before returning them.
The retained harness subsequently passed the same 150,444 checks under both
sanitizers and optimization (`crc-retained-check/`); its source hashes match
the committed sources. The PNG benchmark harness also passes LLVM 22 syntax
checking. The measured harnesses are preserved in `crc-final/source-study.cpp.txt` and
`measured-png_benchmark.cpp.txt`; these changes do not alter the CRC algorithms.

CRC trials hold the hinted decoder constant and use filter-0 PNGs, so the Sub
hint is not executed in those end-to-end CRC comparisons. Source hashes record
that prototype exactly; the production Sub selection is a separate experiment.

## State budget and compiler outcome

The final source removes the duplicate terminal ring. These are per-candidate
costs, not the total footprint of the experiment binary containing every rival:

| Candidate | Shared terminal table | Logical mutable state | M4 default-target frame | x64 SSE2 frame |
| --- | ---: | ---: | ---: | ---: |
| degree-261 quartet | 4,096 B | 1,044 B | 1,136 B | 1,160 B |
| degree-261 scaled 128-bit | 4,096 B | 4,176 B | 4,288 B | 4,296 B |
| direct degree-300 scaled 128-bit | 4,096 B | 4,800 B | 4,864 B | 4,840 B |

Compiler frame figures come from `crc-final/*.su`; logical state excludes
cursors and ABI saves. The earlier hinted quartet frame was 2,208 B before
terminal reuse. This reduction is demonstrated by generated code, independent
of noisy timing differences between separate runs. The prepared equal-length
combination matrix is 128 B and can be shared read-only; the experiment also
contains the general immutable shift-power table. Each active worker still owns
its mutable recurrence state and finalized CRC.

The final x64 body has explicit 16-byte auxiliary spills/reloads at offsets
`-912(%rbp)` and `-944(%rbp)`. It uses XMM operations without YMM/ZMM registers.
The proposed fifteen-register schedule has therefore **not** been realized
portably by this implementation/compiler. Ring-boundary paths and register
allocation remain optimization work. Some sanitizer-build XOR hints also produce
Clang transformation warnings; they are retained, not treated as proof of SIMD.
Correctness checks pass independently of whether a hint is honored.

## Evidence index

| Directory or snapshot | Purpose |
| --- | --- |
| `baseline-png_decoder.cpp.txt` | Exact historical decoder used for non-CRC paired comparisons |
| `contiguous-trial/`, `prefix-trial/` | Early expansion and prefix experiments; not selected production code |
| `sub-final/`, `sub-check/` | All-strides scan and negative RGBA16 evidence |
| `selected-benchmark/`, `selected-check/` | Unhinted stride-1/4 selection |
| `hinted-benchmark/`, `hinted-check/` | Rejected explicit-hint scan: correct but slower than the unhinted scan |
| `crc-initial/`, `crc-inline/`, `crc-registers/` | Helper-call, inlined-array and named-auxiliary lowering experiments |
| `crc-hinted/` | Four-lane hint with the older duplicate terminal state |
| `crc-final/` | Final source, terminal-state reuse, 32/128 KiB coverage, default Apple target |
| `crc-armv8-requested/` | Rejected ISA control: `-march` alone still emitted Apple-target EOR3 |
| `crc-baseline-isa/` | Same C++ source with generic CPU and baseline Armv8-A, verified in assembly |
| `sub-hint-ablation/` | Hint compared directly with the unhinted selected scan |
| `crc-retained-check/` | Final named-field harness correctness, source hashes and ISA preflight |

## Verification history and remaining limits

The unhinted selected Sub version passed 15,300 libpng differential cases and
4,111,125 fuzz executions in 61 seconds. Earlier all-strides data and failures of
performance qualification remain in `sub-final/` and `sub-check/`.

`full-suite-first.log` records an intermittent 408-byte/three-allocation LSan
report from AppKit/HIToolbox initialization in `gui_forms_application_native_tests`.
The next run, `full-suite-selected.log`, passed all 159 tests in 123.52 seconds
without any suppression change. That later pass does not explain or erase the
first report. The selected run predates the writer fixes and final vector hint;
the final-source suite has a separate record.

`full-suite-hinted.log` passed all 159 tests in 143.16 seconds with both writer
fixes and the subsequently rejected Sub hint. `hinted-check/` passed 15,300 differential cases
and 4,078,859 fuzz executions in 61 seconds (peak RSS 404 MiB), under ASan/UBSan
with leak detection. Both first-party spelling and lifetime checkers scanned
1,186 files, including this experiment, with zero findings.

`full-suite-retained.log` is the final production selection: the unhinted Sub
scan together with the writer fixes passed **159/159 tests in 161.32 seconds**.
`retained-production.json` identifies those sources and the command. No
suppression was added for any run. The earlier intermittent framework leak
remains an unexplained failed observation.

The final CRC source passed **150,444 checks in each sanitizer and optimized
run**, for both the default-target and the corrected baseline-ISA configurations.
The sanitizer runs used ASan, UBSan and LSan without leak suppressions.

The PNG phase still requires sustained fuzzing, full platform acceptance for the
new commit, ancillary semantic/colour-management policy, the compressor-backed
encoder and production cutover. Neither house style nor a finite sanitizer run
is proof of leak freedom. Current PR checks on an older head must not be counted
as verification of these local changes.

Text logs retain every diagnostic; trailing horizontal whitespace is normalized
for the repository diff check.
