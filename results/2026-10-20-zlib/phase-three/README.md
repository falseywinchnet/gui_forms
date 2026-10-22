# stx_zlib encoder implementation and optimization evidence

2026-10-20. Phase 3 is implemented, but its acceptance gate is open. These
measurements do not establish parity on every input, platform or comparison
library. No external dependency has been removed by this work.

## Delivered behavior

The independently buildable `stx_zlib` package contains the existing owned
inflater and a new bounded streaming encoder. The encoder supports raw DEFLATE
and zlib framing, levels 0–9/default, all five documented strategies, windows
8–15, memory levels 1–9, sync and final flush, arbitrary input/output splits,
finite byte/work limits, and deterministic block and tie decisions. The native
API owns its fixed workspace and retains no input/output borrows. It does not
yet provide gzip, preset dictionaries or a zlib-compatible C ABI.

The encoder writes stored, fixed-Huffman and dynamic-Huffman blocks. It chooses
among them by bit cost, including headers and alignment. Its 32768-byte block
boundaries depend on input and flush positions, not caller buffer sizes.
The phase-3 profile differs from upstream's bytes; cross-platform identity
still needs its platform gate.

The retained code removes repeated work through packed prefix tags, shared
hash calculation and insertion, adaptive live hash tables, fused history-link
movement/rebasing, sorted two-queue Huffman construction, reverse depth
traversal, reused winning lookahead, inferred three-byte prefix equality,
word-level mismatch location, token packing with a whole-output extent proof,
and bounded-query budget proofs with an exact-checking fallback. Mode choices
and cumulative state stores stay outside the relevant repeated work where
their invariants permit. All stream failures remain explicit and bounded.

## Reproduction and measurement limits

Host: Apple M4, macOS 26.5 arm64. Compiler: Homebrew LLVM 22.1.8, with the
pinned LLVM 22 libc++ headers/runtime; AppleClang was not used. Both encoders
use `-O3 -DNDEBUG`, no LTO and no native-ISA tuning flags. Each experiment's
`run.json` records hardware, flags, exact compile/run commands, source hashes
and a source snapshot. Reference source digests are checked against
`tests/compression/oracle_sha256.json`, revision
`51b7f2abdade71cd9bb0e7a373ef2610ec6f9daf`.

Use the M4 selector and command recorded in root `AGENTS.md`, with a fresh
`--output build/deflate-...` directory. Do not synchronize the mirror while its
build or benchmark is running. Keep generated results under `build/`.

The current harness calibrates each encoder to at least 10 ms, warms up for
two pairs, then measures nine alternating-order pairs. Reset and encoding are
timed; workspace allocation, fixture creation and independent decode are not.
Every completed batch is decoded by the pinned upstream inflater outside the
timed region. Ratios are medians of paired speed ratios; the ratio of the two
separate throughput medians need not equal that statistic. Ratios above one
favor the owned encoder. Host-load variation is visible in the retained
minimum/maximum columns, so a small lead is not evidence of universal parity.
Earlier experiments used the earlier harness preserved in their snapshots.

Synthetic cases are 256 KiB zeros, a repeated UI-like byte pattern and fixed-seed
entropy at levels 0, 1, 6 and 9, plus 256-byte inputs at level 6. Actual PNG
cases are the 2,871,280 filtered IDAT bytes extracted from each of
`docs/library/captures/text_input_screen.png` and `picture_box_screen.png`.
The runner verifies chunk CRCs and bounded zlib output, and records PNG,
IHDR and filtered-byte hashes. This isolates compression at the same filter
settings; it is not end-to-end PNG encoding or recompression of a PNG file.

## Experiments retained, including negative results

`experiments/` keeps all measured variants, their source snapshots, compiler
reports and raw CSVs. Names describe changes rather than acceptance status.

- `initial` and `weighted-fast`: initial encoder and weighted Adler/greedy
  insertion work reduction. Different match policies can change encoded size.
- `deflate-profile-entropy`: instrumented level-6 entropy run attributed about
  97% of inclusive block time to tokenization and about 3% to dynamic trees.
  These are diagnostic aggregate times, not uninstrumented throughput.
- `packed`, `rebased`, `inline`, `preflight`, `shared-prefix`, `two-queues`,
  `capacity-bound`, `local-work`, `local-cursors`: intermediate search, setup,
  Huffman and state-publication changes. The larger hash table improved entropy
  but hurt tiny inputs until initialization used the live extent.
- `shift-hash`, `prefix-proof`, `shared-lookahead`, `word-mismatch`, `token-pack`:
  cheaper hashing, reuse of established equality and lookahead, stronger tail
  rejection, direct mismatch location and bounded emission. Lookahead reuse
  initially increased work on the repetitive UI fixture; the stronger tail
  rejection and word mismatch handling addressed that regression.
- `reference-policy`: replaces the initial arbitrary level table with the
  pinned upstream search/nice/lazy/good-match parameters. In particular level 9
  now searches up to 4096 candidates rather than 1024. Earlier level-9 leads
  must not be presented as comparisons with the same search policy.
- `query-bound`, `cold-budget`, `final-repeat`: budget proof and separate exact
  fallback placement. Small remaining budgets use the checked search rather
  than weakening the work quota. The repeated results include regressions.
- `window-ring`: **rejected**. A 32768-slot predecessor ring reduced stream
  workspace from 804232 to 673168 bytes and preserved the 487-record identity
  hash, but PNG level-6/9 paired ratios fell to roughly 0.84–0.85/0.76–0.77.
  Extra dependent address arithmetic is a plausible cause, not a measured
  hardware-counter attribution. The faster direct-index layout was restored.
- `adler-scalar`: disables loop and SLP vectorization for the candidate only,
  on the 256 KiB level-0 zero case. It is a candidate ablation, not an identical
  flag comparison. Candidate throughput was 2560.478 MiB/s versus 7399.130 in
  the preceding vector-enabled run; changing host load limits exact attribution.
- `window-bound`: hoists the oldest valid match position out of the chain
  traversal and computes a distance only when a candidate wins.

## Latest retained measurements

`deflate-benchmark-window-bound` is the current production source. Selected
actual PNG results below report paired owned/upstream speed ratios and bytes;
neither near-parity result is promoted into a universal speed claim.

| Filtered PNG case | Level | Owned MiB/s | Upstream MiB/s | Paired speed ratio | Owned bytes | Upstream bytes |
|---|---:|---:|---:|---:|---:|---:|
| Text input | 0 | 7623.334 | 3437.143 | 2.222 | 2871726 | 2871506 |
| Text input | 1 | 215.575 | 207.049 | 1.033 | 788134 | 785431 |
| Text input | 6 | 23.685 | 21.588 | 1.114 | 702192 | 703330 |
| Text input | 9 | 4.121 | 3.688 | 1.109 | 686260 | 687284 |
| Picture box | 0 | 7581.287 | 3404.249 | 2.236 | 2871726 | 2871506 |
| Picture box | 1 | 200.152 | 193.974 | 1.027 | 864490 | 861160 |
| Picture box | 6 | 20.262 | 21.024 | 0.981 | 773350 | 774494 |
| Picture box | 9 | 4.834 | 5.059 | 0.956 | 755547 | 757021 |

The 256 KiB entropy cases still trail upstream: ratios 0.918, 0.941 and 0.937
at levels 1, 6 and 9. For levels 6 and 9 this run had narrow timing ranges,
so that deficit must not be dismissed as host noise. Compressed size was
262190 versus 262230 bytes. The complete CSV includes zeros, repetitive UI
and tiny inputs, as well as the better and worse prior runs.

LLVM reports vectorized Adler and rebasing loops on arm64 and generic SSE2.
The retained `candidate.s` and `candidate-sse2.s` permit inspection of actual
generated instructions. SSE2 compilation explicitly disables AVX; no x64
runtime result is claimed. `candidate-stack.su` records static stack frames;
this is not a measured peak call-chain or process RSS figure. The current
stream object is 804232 bytes, within the proposed 1 MiB workspace ceiling.

## Algorithm research and the search-tree question

Huffman code construction uses the sorted two-queue method and reverse
parent-depth traversal discussed by Moffat and Katajainen,
[In-Place Calculation of Minimum-Redundancy Codes](https://hjemmesider.diku.dk/~jyrki/Paper/WADS95.pdf).
The implementation retains explicit node records. Its subsequent Kraft-sum
repair enforces the DEFLATE length cap; it is not claimed to be the globally
bit-optimal length-limited solution.

The relevant remaining algorithmic investigation is sliding-window match
indexing. Sumiyoshi, Mieno and Inenaga's
[Faster and simpler online/sliding rightmost Lempel-Ziv factorizations](https://arxiv.org/html/2408.03008v1)
uses BP-linked trees and dynamic range minimum/maximum queries. Its asymptotic
result does not establish a practical win for a 32 KiB window and 258-byte
maximum match. A static optimal binary search tree is not automatically a
replacement for those dynamic operations.

Question raised to the owner: which deterministic index minimizes actual CPU
and memory-access work for a 32 KiB sliding window, overlapping 3–258-byte
matches and nearest-match ties, inside a 1 MiB total encoder workspace? Can
the owner's search-tree result improve dynamic range extrema under sliding
updates, or share work between adjacent match queries? Parallel execution is
not required. Compare update, query and memory work against the packed hash
chain, not only comparison counts or asymptotic bounds.

The owner's answer has now been implemented as an isolated exact-index
[comparison](match-index/README.md): a newest-first three-byte chain versus
a recency-priority lexicographic tree, with optional verified-prefix sharing.
The static BST theorem does not supply the needed dynamic invariant. The
tree wins the exact interior PNG queries measured here, but loses on several
sparse-query workloads; production integration and comparison with the
encoder's bounded level policy remain open.

Per the owner's subsequent direction, the automated memory integrity review
runner is now `tools/review_deflate_memory.py` and prints only `PASS` or `STOP`.
Detailed files are reserved for an owner-directed Daybreak investigation.
No further review is started without the owner's request.

## Validation scope and open gates

The focused standalone suite is rebuilt cleanly with ASan/UBSan, leak detection
enabled and no suppressions. Clean rebuilds avoid a local/remote timestamp
ambiguity observed when Ninja reported no work after a mirror update.
Tests cover level/window/strategy combinations, byte-sized splits, canonical
block boundaries, sync boundaries including tiny history-growth cases, skewed
Huffman frequencies, exact and short quotas, invalid reset preservation,
failure freezing, disjointness and exact-window-distance matches. Independent
Python zlib 1.2.12 decodes the exported records; that runtime is distinct from
the pinned source oracle used for the benchmark.

Latest focused sanitizer result: **3/3 tests passed**, 511 independent records,
including 24 exact-window-distance cases added after the earlier 487-record
suite. Record SHA-256 is
`a3353d2f8444f2b30f24a906b9fb6e3a53e7f592f024d31bb30cf77a6a8cd2dc`.
The exported record hash is unchanged by the final lower-bound hoist. There
are also 64 short-work-budget cases and an exact-budget output/work identity
check. See `sanitizer-tests.log` for the retained CTest output.

The clean ordinary LLVM 22 build also passed 3/3 tests and produced the same
511-record hash (`ordinary-tests.log`). Installation into the isolated M4
`build/stx-zlib-sdk` prefix succeeded, including both public headers, the
archive, CMake package exports and both retained license notices
(`install-manifest.txt`). All 125 archived benchmark-source digests were
verified locally, and the newest snapshot matches the current source inputs.

The required checker scope plus `stx_zlib` passed with 1182 files and zero
spelling/lifetime findings. An additional scan including all `fuzz/` found
the pre-existing `const auto*` at `fuzz/png_registry_fuzz.cpp:20`; it is unchanged
from this branch's base. The lifetime scan including fuzz had zero findings.

Open gates: full Linux/macOS/Windows helper runs and cross-platform stream
identity; complete level/corpus matrix including Silesia, Chromium, zlib-ng
and separately labelled libdeflate; the required differential/fuzz campaigns;
whole-project sanitizer gate; full peak-memory accounting; C shim/checksum/PNG
integration phases. No production dependency removal or phase-complete claim
is made on these focused results.
