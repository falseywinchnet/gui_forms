# Capped radix match-index experiment

Date: 2026-10-20. This experiment implements the owner's request to try shared
prefix information as a replacement for independent candidate traversal.
Production encoder integration remains a separate gate.

This records the initial radix/finger experiments. The subsequent random-data
maintenance investigation, revised representation, and current measurements
are in [RANDOM_REFACTOR.md](RANDOM_REFACTOR.md). Object sizes and “final” results
below refer to the archived finger-shortcuts version, not the later refactor.

## Algorithm

`stx_zlib/bench/radix_index.hpp` implements a critical-bit tree over 258-byte
keys. Each retained source is a leaf. An internal branch records the first bit
at which two key groups differ. Split bit positions strictly increase on a
downward path, so a path cannot grow with the length of a collision chain.
There are at most 32768 leaves and 32767 branches. Links are 16-bit values;
each branch occupies 12 bytes. Storage is allocated before processing.

One descent selects a representative with maximal common prefix. Comparing
the query with that representative determines the winning whole-byte length.
Ascending through splits inside or beyond its mismatch byte identifies the
entire equal-length group. Its newest source resolves the nearest-distance
tie. Sources with identical complete keys are replaced by the newest one,
which remains legal at least as long as the source it replaces.

The source exactly 32768 bytes behind the query is removed before its leaf
slot is recycled and compared separately when it could improve the answer.
All other indexed sources are nearer, so an equal-length oldest match loses.
Deleting the oldest leaf contracts its parent. Surviving ancestors cannot
lose their newest source, so expiration needs no upward maximum repair.

The second variant partitions the index through an exact two-byte directory.
This avoids traversing the first sixteen bits and is the same algorithm for
all inputs. No image classification or search cutoff is used.

## Shared traversal and deferred maxima

The `--radix-finger` variant adds an insertion finger at the preceding input
position's leaf. It compares the current key with that preceding key and finds
their first differing bit. The previous distance-one match supplies a shifted
lower bound for this comparison.

Branches above their divergence belong to both paths. They need neither a
second descent nor another stored newest-source update. Only the departed path
is finalized with the previous position as its newest source. The common path
has an implicit newest source: the most recently inserted leaf.

Queries account for this implicit state explicitly. If the preceding source
shares the maximal whole-byte prefix, it wins the nearest-source tie outright.
If a longer match exists, its winning group excludes that preceding source,
so its stored newest-source value has already been finalized. No approximate
maximum or probabilistic pruning is used.

If the differing bit precedes the selected subtree's first branch, every key
in that subtree has the same mismatch there. The new branch can be inserted
above the subtree without descending through it. Otherwise the finger enters
the appropriate branch and continues ordinary descent.

This is shared work between adjacent keys, not a general constant-time update
claim. Arbitrary input can repeatedly enter and leave long paths; the complete
traversal and maintenance costs remain part of the measurements.

## Verification and measurement

`tools/benchmark_match_index.py --radix-finger` selects this variant. The older
`--radix` selection uses eager ancestor maxima and begins each search at its
directory root. Archived source snapshots preserve the initial global-root
version too.

The harness compares exact lengths and distances against an exhaustive chain
at every scheduled query. Scalar brute-force comparison independently checks
short fixtures and selected window boundaries. There are 1052 deterministic
boundary fixtures: all lengths 0 through 260 plus 513 bytes for four patterns,
and four multi-window periodic cases. They cover short lookahead, zero padding,
overlap, small alphabets, nearest ties, and distances 32767 and 32768.

Timing uses the same query schedule for both indexes. Dense schedules query
every byte; greedy schedules query exact-match phrase boundaries. Every input
position still incurs maintenance. The existing benchmark description in
`README.md` specifies the alternating pairs, calibration, compiler and counters.
CSV columns retain their historical `tree_*` names; `run.json` identifies the
actual candidate index and whether the finger is enabled.

Counters are algorithmic instrumentation, not hardware events. `visits`
counts tree traversal steps (including upward walks) or chain candidates.
`compared` counts byte lanes in comparisons and bytes sampled for branch
decisions. `writes` counts explicit index stores as instrumented, excluding
scalar cursor/proof bookkeeping, initial clearing and allocator counters.
`reused` counts certified prefixes skipped at individual comparisons; it can
count the same input bytes at different operations and is not unique bytes.
The directory's two-byte proof is included in radix `reused` values.

## Limits

The prototype borrows an immutable, fully available input buffer. It has bounded
index storage and a sliding legal-source window, but does not yet implement
incremental input arrival, production block boundaries, or position rebasing.
Virtual zero padding applies only beyond the actual end of this experimental
input. Production integration must establish how growing lookahead affects
keys; it cannot silently treat a temporary input boundary as end of stream.

The finger index occupies 720908 bytes on the measured M4 build. Replacing the
production encoder's two 262144-byte index arrays in its 804232-byte workspace
would arithmetically yield 1000852 bytes, below 1 MiB by 47724 bytes. This is a
budget estimate, not a measured integrated encoder layout. Additional streaming
state and any required key-buffer changes still have to fit.

These runs are ordinary deterministic algorithm checks and performance
experiments. They do not establish the separate automated memory integrity
review gate or cross-platform behavior. No architecture-specific SIMD or
parallel execution is used.

## Retained variants

- `radix-first/`: original single-root tree, complete exact comparisons passed.
- `radix-directory/`: exact two-byte directory and oldest-candidate rejection,
  complete exact comparisons passed.
- `radix-finger-check/`: deferred maxima and shared traversal; all 1052 boundary
  fixtures passed with a clean LLVM 22.1.8 compilation.
- `radix-finger/`: complete dense and greedy comparison with deferred maxima.
- `radix-finger-shortcuts/`: final version, greedy timing plus all boundary
  fixtures. Exact adjacent duplicate keys replace the preceding leaf directly;
  a first-byte mismatch rejects an adjacent prediction immediately; already
  dominated expired leaves avoid directory writes.
- `legacy-boundary-check/`: the original recency-tree matcher also passes all
  1052 fixtures through the expanded harness, with an empty compiler log.

The first run exposed the maintenance problem: greedy late-mismatch input
incurred 120848696 instrumented writes for 131072 bytes and ran much slower
than the exhaustive chain. The directory reduced random-input traversal, but
left 119130904 writes on that case. This motivated the finger/deferred-maxima
algorithm rather than selecting workloads around the original tree.

## Results and decision

Apple M4 arm64, Homebrew LLVM 22.1.8 and the pinned libc++ runtime, C++20 `-O3`,
nine alternating calibrated pairs. Each timed workload is 131072 bytes. PNG
samples are distinct filtered-byte interiors at offset 1048576. Their hashes,
source snapshots, exact commands, compiler logs and object sizes are retained
in each run. All completed runs returned zero; every scheduled query matched
the exhaustive chain. The finger and final runs also passed all 1052 boundary
fixtures. The final compiler log is empty.

Final greedy ratios are chain time divided by candidate time. Greater than one
favors the radix candidate. Ratios are medians of paired samples, not ratios
of separately selected median durations.

| Workload | Final ratio | Paired range |
|---|---:|---:|
| Random bytes | 0.3711 | 0.2653–0.5176 |
| Uniform bytes | 0.1902 | 0.0894–0.2201 |
| Long prefixes / late mismatch | 1.1874 | 0.9553–1.3744 |
| Crowded hash buckets | 33.3276 | 25.2173–43.8329 |
| Text-input PNG interior | 1.9107 | 1.8733–1.9575 |
| Picture-box PNG interior | 1.3933 | 1.3729–1.4350 |

The late-mismatch timing spread crosses parity; its median alone is not a
stable speedup claim. Random and uniform input still lose. Runs occurred at
different times, so absolute before/after timings are not paired estimates.
Instrumented operation counts provide the direct evidence of work eliminated:

| Greedy workload | Eager directory visits | Final visits | Eager writes | Final writes |
|---|---:|---:|---:|---:|
| Late mismatch | 234749615 | 3486315 | 119130904 | 3566281 |
| Text-input PNG interior | 69499727 | 2701957 | 36193825 | 2835382 |
| Picture-box PNG interior | 26801532 | 3300421 | 15060953 | 3382684 |

The first finger run also measured dense queries: median ratios were 346.0891
on late mismatch, 66.0833 on crowded buckets, 100.7692 on the text sample and
46.5441 on the picture sample. Dense queries expose exhaustive-chain cost and
must not be presented as whole-encoder throughput. Final shortcut changes were
measured only under greedy timing; boundary verification still queried every
position.

The original root-directory refinement did not cure repetitive-path cost.
The shared insertion finger and deferred ancestor maxima did: they remove
repeated maintenance of the common path, rather than merely scheduling it
differently. The final uniform run has zero counted branch/candidate visits,
yet still performs 655349 instrumented writes versus the chain's 262140.
This isolates remaining per-position bookkeeping even after search disappears.
Random input also pays for more metadata and updates than the cheap chain.

Decision: retain the implementation and its negative evidence as an exact
algorithm candidate; do not replace the production encoder yet. The next
optimization target is common-case insertion/recycling and metadata traffic,
including whether dominated equivalent keys can retain stable leaf identities.
The current evidence does not justify image classification, a workload switch,
reference-speed parity, a universal optimum, or the total streaming gate.
