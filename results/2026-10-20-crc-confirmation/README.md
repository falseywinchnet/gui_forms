# Independent CRC suitability confirmation

2026-10-20. Current decoder source: `4648d73003f96d0175e2cd95c2c9f7ed1c4c709f`.
This repeats the PNG chat's independent implementation and measurement against
current sources, following the owner's instruction to determine suitability here
computationally. The sibling's mathematical proposal is an input to the
experiment, not an acceptance decision. No production CRC change is made.

## Decision

The degree-261 quartet is suitable as the **bounded-state large-IDAT candidate**.
It is unsuitable as an unconditional CRC replacement. Retain the direct
300-degree/128-bit implementation as a bulk competitor, and slicing/braiding for
short and medium calls. The evidence establishes useful alternatives under
explicit constraints; it does not establish a globally optimal CRC algorithm,
an optimal dispatch boundary, or an optimal parallel worker count.

Fresh whole-call paired speedups over the current PNG slicing-by-eight CRC:

| CRC span | Braid 5×8 | Degree-261 quartet | Direct degree-300/128 |
| --- | ---: | ---: | ---: |
| 512 B | 1.32× | 0.224× | 0.0631× |
| 4 KiB | 2.74× | 1.48× | 0.539× |
| 16 KiB | 2.74× | 2.42× | 1.73× |
| 32 KiB | 2.93× | 3.15× | 2.29× |
| 65,539 B | 2.43× | 3.78× | 3.12× |
| 128 KiB | 2.27× | 3.76× | 3.54× |
| 1 MiB | 2.89× | 3.69× | 4.77× |

At 17 bytes, the unconditionally selected quartet is only 0.00607× baseline
throughput. Setup and final reduction overwhelm its interior savings. The
65,539-byte quartet range is 2.63–4.18×; the 1 MiB direct-300 range is
4.07–13.00×. Wide ranges reflect host scheduling variation. The prior run ranked
direct-300 ahead at 128 KiB; this run reverses those medians. Neither supports
fixing an exact crossover there. Equivalent quartet/hybrid large-call bodies
also receive different timings; close median rankings are not proof of wins.

The quartet owns 1,044 bytes of logical mutable state versus 4,800 bytes for
direct-300/128, a 78.25% reduction. Both share a 4,096-byte immutable terminal
table. These are algorithmic payloads, not total frames or decoder memory.
The existing generated-code evidence still shows x64 auxiliary spills: the
proposed spill-free fifteen-register interior is not achieved by this C++
implementation. Lower arithmetic/store counts in the ideal schedule cannot be
substituted for actual compiled costs.

## Whole PNG decode

The tested hybrid uses slicing below 64 bytes, braid4×8 below 32 KiB, and the
quartet above. These thresholds are ablations, not an optimized dispatch policy.
Only the CRC implementation changes between timed decoder variants.

| Generated 256×256 filter-0 image | Plain paired median | Adam7 paired median |
| --- | ---: | ---: |
| Indexed8 | 1.15× | 1.21× |
| RGBA8 | 2.00× | 1.90× |
| RGBA16 | 1.65× | 1.72× |

The tiny red-pixel fixture was 1.06× with a 0.83–2.49× range, which establishes no
reliable improvement. Indexed8 Adam7 and RGBA16 Adam7 ranges also cross one.
These PNGs use stored DEFLATE blocks and expose checksum cost; they do not
predict the same gain for photographic inputs with substantial inflate work.
The large IDAT CRC spans are 65,539 bytes, with short tails and small ancillary
chunks. Decoded image size is not the CRC call size.

## Parallel and shared work

Each tested CRC candidate correctly combines four independently calculated
segments through one prepared, immutable 128-byte shift matrix. A worker may
own its mutable recurrence state while sharing the terminal table and shift
matrix. This demonstrates computational correctness of independent segmentation
and shared combination work, not a measured concurrency speedup.

Splitting a call repeats initialization and finalization and adds combination
and scheduling work. The short-call measurements are direct evidence that these
costs matter. Parallel execution therefore needs both elapsed-time and aggregate
CPU-time measurements, together with an explicit worker/state budget. It is not
accepted merely because the recurrence can be split.

## Verification and reproduction

Both ASan/UBSan/LSan and optimized builds passed 150,444 checks apiece against
pinned upstream zlib, with an independent bit oracle on small inputs. Cases
include random seeds, sixteen alignments, exact allocation tails, split calls,
constant inputs, threshold/ring boundaries and shared segment combination. No
leak suppression was used. This finite campaign is not a proof of leak freedom.

An additional independent GF(2) long division checked these denominators
against the reciprocal IEEE generator `0x1db710641`:

- Quartet: exponents 0, 12, 52, 150, 202, 261.
- Direct-300: exponents 0, 145, 183, 211, 300.

Both exact remainders are zero (`polynomial-check.json`). This checks the
polynomial relationship; full-call oracle checks additionally exercise seed,
flush, finalization and byte-tail semantics.

The M4 run uses LLVM 22.1.8 and `-mcpu=generic -march=armv8-a`, with a preflight
check excluding EOR3, BCAX, CRC32 and PMULL from candidate assembly. x64 SSE2
assembly is inspected without AVX, but x64 execution performance is unmeasured.
The native upstream zlib hardware CRC control is ineligible for selection.

Each timing includes initialization, seed injection, flush, terminal reduction,
exact tails and dispatch. Calibration targets 25 ms, then two warmup and seven
retained rounds rotate candidate order. Tables report medians of paired ratios.
The CSV files preserve ranges. Sources, commands, compiler and input hashes are
recorded in `run.json`. The machine was not exclusively reserved.

```sh
/Users/ultimussecundai/.local/bin/m4build -- env PATH=/opt/homebrew/opt/llvm@22/bin:/opt/homebrew/bin:/usr/bin:/bin python3 -B experiments/png-crc-suitability/run.py --zlib build-zlib-study-sources/upstream-zlib --workloads build/png-hinted-check/workloads --output build/png-crc-current-confirmation --baseline-isa
```

This confirms the existing computational direction. Cold-cache behavior, actual
worker scaling, x64 timing, a broader compressed-image corpus and the deployed
Chorba implementation remain unmeasured here. They prevent a universal optimum
or production-selection claim. The wider PNG/inflate phase gates remain open.
