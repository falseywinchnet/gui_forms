# Exact DEFLATE match-index study

The subsequent [capped radix experiment](RADIX.md) implements an exact prefix
index and then shares adjacent-key traversal and defers common-path maxima.
Its source, negative first variants, boundary checks and measurements are
retained separately from this recency-tree study.

The owner's follow-up ruled out applying a static optimal search tree directly
to dynamic range extrema. This experiment evaluates the concrete alternative:
a lexicographic tree whose parents are newer than their descendants, with
three-byte hash buckets and shared prefix bounds during root insertion.

The experimental C++ is in `stx_zlib/bench/match_index.cpp`; reproduce it with
`tools/benchmark_match_index.py`. It is separate from the production encoder.
Both indexes return the longest match capped at 258 bytes, then the nearest
source, within a 32768-byte window. Overlap is allowed. They insert every
eligible position, including positions skipped by the query schedule. Neither
uses a search-depth cutoff or early nice-length exit below the allowed maximum.

The baseline chain is exhaustive. The production encoder intentionally has
bounded level policies, packed prefix tags and different position bookkeeping.
These results are not throughput comparisons against that encoder or upstream
zlib. No production change or universal speedup follows from this study alone.

## Measurements

Each case contains 131072 bytes. The dense schedule queries every position;
the greedy schedule queries only at exact-match phrase boundaries. Maintenance
at skipped positions is included. Every tree answer is compared with the
chain's exact length and distance. Independent scalar enumeration checks the
first 1024 positions, selected window boundaries, sampled positions and the
tail. Deterministic operation counters run separately from timed code.

LLVM 22.1.8, `-O3 -DNDEBUG`, pinned libc++ runtime, Apple M4 arm64. No
architecture-specific intrinsics, native-ISA flags or parallel execution.
Nine alternating pairs follow warm-up; the calibrated runs repeat short
cases to at least 10 ms per timing sample. Ratios are medians of paired
chain-time/tree-time ratios, so values above one favor the tree. Timing ranges
are retained; very short cases show substantial host variation.

The first calibrated run without adjacent-query sharing found:

| Workload | Every-byte tree ratio | Greedy tree ratio |
|---|---:|---:|
| Entropy | 0.813 | 0.916 |
| Uniform | 1.266 | 0.117 |
| Long prefixes with late mismatches | 89.663 | 0.264 |
| Crowded hash buckets | 66.787 | 33.900 |

For dense late-mismatch queries, candidate visits fell from 3586908877 to
17391375. For greedy queries on the same bytes, the chain visited 9641168
candidates while the tree still visited 17391375 nodes to maintain its index.
This reversal is the main finding: query sparsity changes the winning method.

The initial 131072-byte prefixes of the two repository PNG captures produced
identical operation counts. Treat these as one workload, not two independent
PNG samples. Follow-up measurements take explicitly recorded interior slices
at filtered-byte offset 1048576; input and sample hashes are retained.

The distinct interior PNG samples, without adjacent-query sharing:

| Workload | Every-byte tree ratio | Greedy tree ratio | Greedy paired range |
|---|---:|---:|---:|
| Text input capture | 90.862 | 1.503 | 1.478–1.516 |
| Picture box capture | 62.480 | 1.654 | 1.516–2.268 |

With sharing enabled, the corresponding greedy ratios were 1.366 and 1.500.
All exact answer comparisons passed in each completed run. The two variants
were measured in separate runs, so cross-run timing differences are not a
paired estimate of the sharing optimization itself. The within-run tree/chain
ratios are paired. No hardware-counter measurement or universal optimum is
claimed.

Retained records:

- `match-index-calibrated/`: synthetic baseline and the duplicated PNG prefix.
- `match-index-shared/`: sharing enabled, synthetic cases and distinct interiors.
- `match-index-interior-no-sharing/`: same distinct interiors, sharing disabled.

Each directory includes its source snapshots, compiler output, commands,
source/workload hashes, result CSV and measured index object sizes.

## Adjacent-query sharing

A match at position i, source s, length l proves the same bytes at i+k and s+k
for l-k bytes when k is smaller than l. The experiment carries this proof
and uses it only when traversal actually reaches source s+k. It does not
initialize the chosen answer from an out-of-order source, so nearer equal
matches retain priority. Both indexes receive the optimization. The tree
also retains its ordinary lower/upper prefix bounds between visited nodes.

Counts named `compared` count byte lanes examined, including all eight lanes
of a word comparison. They are not retired instructions, cache misses or
physical memory traffic. `reused` records additional certified prefix bytes;
`writes` counts index-array stores, excluding carried-proof bookkeeping.
Hardware cycles, instruction counts, cache misses and branch misses have not
been measured. Wall-clock time includes all of that bookkeeping.

For uniform dense queries, sharing reduced compared byte lanes from 33783162
to 131071 in both indexes, with unchanged candidate visits: 99.61% less repeated
comparison work. It did not remove index maintenance. On the distinct PNG
interiors, tree comparisons fell from 51211533 to 42596237 (text input) and
33102308 to 31719717 (picture box), but the separate sharing run was slower.
The extra per-node bookkeeping is therefore not promoted as a default encoder
optimization on the strength of fewer compared bytes alone.

Decision: keep the production index unchanged. The recency-priority tree is a
concrete candidate for a more thorough matching mode, with a measured benefit
on these exact PNG queries and a substantial loss on sparse uniform and
late-mismatch workloads. It must compete inside the encoder against the
existing bounded chain, at stated compression quality and settings, before
selection. The current experiment does not justify switching every level.

## Integration constraints

The tree has no general balancing guarantee. Its recency invariant permits
nearest-match selection during the lexicographic search; ordinary rotations
cannot be added without preserving that invariant or changing the query.
All links use absolute 32-bit positions in a bounded experiment. Production
integration still requires long-stream rebasing, canonical block-boundary
lookahead handling, work-limit accounting and a complete workspace budget.
The prototype has all input bytes available; it does not establish those
streaming properties.

The original index arrays occupy 393216 bytes for the chain and 524288 bytes
for the tree. Carrying a match adds a 12-byte record to each. These figures
are index object sizes, not total encoder memory or benchmark process RSS.

## Sources and origin

The owner supplied the lexicographic/recency dominance proof and the warning
about capping before nearest-match tie breaking. The experiment is written
in house style from that description. Libdeflate's
[binary-tree match-finder documentation](https://github.com/ebiggers/libdeflate/blob/master/lib/bt_matchfinder.h)
was consulted for the related existing design and skipped-position cost;
its implementation was not imported into this experiment.
The dynamic result remains
[Sumiyoshi–Mieno–Inenaga 2024](https://arxiv.org/html/2408.03008v1).
Neither paper bounds actual CPU work for this implementation and workload.
