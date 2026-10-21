# Random-data maintenance refactor

Date: 2026-10-20. Objective: move the exact radix match index closer to chain
throughput on random bytes, retaining its answers and difficult-input behavior.
This is an index experiment; production encoder performance remains unmeasured
for this candidate. The baseline is the exhaustive chain in `match_index.cpp`,
not upstream zlib or the production encoder's bounded search policy.

## Diagnosis

The original random result was 0.3711 times chain throughput. Repeating with
longer timing samples gave 0.3858. The counted random workload contains 131072
bytes and 130642 greedy queries. The baseline performs 262140 index writes;
the original candidate performed 1202584. Random input rarely produces a useful
match, but the candidate still created and removed radix metadata.

Generated LLVM 22.1.8 arm64 assembly also exposed a computational difference:
the chain's per-position operation was inlined, whereas the radix called its
large `advance` function on every byte, with an 80-byte register-save prologue.
Inlining helped, but did not eliminate the maintenance work. Introducing small
groups initially left another per-byte call to `insert_small`; inlining that
helper and rejecting a differing third byte before word comparison helped more.
These are assembly observations, not measured cycle or cache-miss attribution.

## Representation and algorithm changes

The exact two-byte directory and critical-bit algorithm remain. Small groups
use at most four newest-first source links. A fifth distinct live key promotes
the group into the existing radix representation. This is an index invariant,
not image classification or an approximate search cutoff. Four is a tested
engineering threshold; these runs do not prove it optimal.

The useful expiration change is shared by every small group:

1. Store its absolute newest position in the directory.
2. Store older links as 16-bit ring indices. Recover each source as the strictly
   older position congruent to that index.
3. Stop before following an expired source. Its link storage may have been
   recycled, so the age check precedes reading that storage.
4. Maintain explicit expiration metadata only for actual tree leaves.

The source at distance exactly 32768 is still queried before its slot is
recycled. Equal-length ties keep the nearest source. Identical complete keys
can replace older copies because the newer key dominates both lifetime and
tie breaking. Tree promotion marks all four retained leaves before the tree
kernel begins. When contraction leaves a singleton, it returns to the absolute
head representation. Existing trees otherwise contract naturally; there is no
periodic rebuild or image-dependent policy switch.

Leaf absolute positions are reconstructed from ring slots in tree paths, so
the old 32-bit position array becomes a byte flag array. For lazy small groups
the flags are zero; only tree leaves participate in the removal ring. Parent
storage holds older links for small groups and parent links for tree leaves.
The high half of the link space identifies internal branches. A high bit in
the 32-bit directory entry distinguishes a linked root from an absolute source;
this experiment limits input to 16 MiB and does not establish position rebasing.

The later duplicate shortcut shifts a certified full distance-one match:
257 bytes are already equal; checking the newly entering byte proves the full
258-byte key equal. It can replace the previous key immediately, retaining its
older link and returning the necessarily best length and nearest distance.

The non-aliasing experiment applies `__restrict__` to the separately allocated
index reference in the bulk run kernel, equally for chain and candidate.
The input, query schedule, answer storage, and counter record are disjoint from
that index. It introduces no SIMD target or alignment assumption.

## Experiments, including negative results

All rows use the original 131072-byte random sample, nine alternating pairs,
and at least 0.05 seconds per timed sample. Ratio is the median of paired chain
time divided by candidate time. Absolute measurements from different runs are
not paired before/after evidence; the host exhibited substantial variation.

| Archived run | Main change | Median ratio | Paired range | Candidate index writes |
|---|---|---:|---:|---:|
| radix-random-profile | Instrumentation and longer samples | 0.3858 | 0.3176–0.4812 | 1202584 |
| radix-random-inline | Inline per-position operation | 0.4620 | 0.3898–0.4933 | 1202584 |
| radix-random-small | Small groups, helper still called | 0.4027 | 0.3530–0.5396 | 827047 |
| radix-random-small-inline | Inline helper and third-byte rejection | 0.6472 | 0.5560–0.7280 | 827047 |
| radix-random-expiry | Prune irrelevant expiration work | 0.5333 | 0.5001–0.6547 | 690242 |
| radix-random-implicit | Reconstruct positions from ring indices | 0.6020 | 0.4527–0.7659 | 690242 |
| radix-random-singleton | Inline singleton positions | 0.5609 | 0.4713–0.6453 | 472604 |
| radix-random-split | Outline substantial tree kernel | 0.5546 | 0.4061–0.7002 | 472604 |
| radix-random-lazy | Lazy expiration for all small groups | 0.7944 | 0.5770–1.0924 | 279374 |

Smaller metadata and fewer stores did not consistently yield better timings.
Singleton storage alone did not solve the problem; small-group expiration was
the more consequential simplification. The initial lazy result must not be
quoted as stable parity: the expanded suite repeated that sample at 0.6979.

## Expanded measurements

The `radix-lazy-suite` archive adds three fixed xorshift seeds at each of
131072 and 1048576 bytes. All 1052 boundary fixtures passed, and every scheduled
length and distance agreed with the exhaustive chain. Independent scalar
enumeration also checks short fixtures and selected window boundaries.

| Workload | Median ratio | Paired range |
|---|---:|---:|
| Original random sample, 128 KiB | 0.6979 | 0.5631–0.9543 |
| Seed 12345678, 128 KiB | 0.6350 | 0.5932–0.7016 |
| Seed 9e3779b9, 128 KiB | 0.6649 | 0.5885–0.7024 |
| Seed deadbeef, 128 KiB | 0.6546 | 0.5969–0.7356 |
| Seed 12345678, 1 MiB | 0.8213 | 0.5534–0.8988 |
| Seed 9e3779b9, 1 MiB | 0.7842 | 0.4373–1.0167 |
| Seed deadbeef, 1 MiB | 0.7838 | 0.6786–0.9220 |
| Uniform | 0.0660 | 0.0650–0.0664 |
| Late mismatch | 1.1451 | 1.1380–1.1487 |
| Crowded hash buckets | 39.7848 | 36.2492–42.4674 |
| Text-input PNG interior | 2.0985 | 1.5745–2.2392 |
| Picture-box PNG interior | 1.4197 | 1.3899–1.7012 |

The uniform regression motivated restoring the shifted full-key proof before
the generic small-group walk. PNG samples remain 131072 filtered bytes at
offset 1048576. Each archive records exact source snapshots, hashes, compiler,
commands, byte samples, ordinary compiler output, counters, and exit status.

## Common-loop refactor

`radix-lazy-restrict` restores the duplicate shortcut and enables the disjoint
index annotation. Its random medians span 0.5973–0.8610 on 128 KiB and
0.7632–0.8041 on 1 MiB. This does not demonstrate a clear annotation benefit;
the final run leaves that option disabled.

`radix-lazy-cold` additionally excludes the eager-list paths at compile time
when lazy lists are selected, and outlines tree expiration. Small groups in
this variant never use that explicit deletion path. All answers still agree;
the operation counts are unchanged from the preceding duplicate-shortcut run.

| Cold-expiration run workload | Median ratio | Paired range |
|---|---:|---:|
| Original random sample, 128 KiB | 0.7806 | 0.5798–1.0659 |
| Seed 12345678, 128 KiB | 0.8178 | 0.6351–1.2548 |
| Seed 9e3779b9, 128 KiB | 0.7524 | 0.6663–0.8508 |
| Seed deadbeef, 128 KiB | 0.7644 | 0.6744–0.8968 |
| Seed 12345678, 1 MiB | 0.7595 | 0.4656–0.8765 |
| Seed 9e3779b9, 1 MiB | 0.7195 | 0.6781–0.9658 |
| Seed deadbeef, 1 MiB | 0.7783 | 0.5927–1.0321 |
| Uniform | 0.1592 | 0.0656–0.2191 |
| Late mismatch | 1.1376 | 0.9177–1.2410 |
| Crowded hash buckets | 40.0882 | 25.0521–42.3182 |
| Text-input PNG interior | 2.0145 | 1.8334–2.1385 |
| Picture-box PNG interior | 1.4143 | 1.2654–1.6387 |

The random sample's visits fell from 211049 to 59337 and counted comparison
lanes from 611492 to 65467; its writes fell from 1202584 to 279374 (76.8%).
The chain performs 56939 visits and 262140 writes. Similar counts do not imply
similar CPU cost: compact-link age reconstruction, extra conditions, dependent
loads, and different code layout remain plausible contributors. No hardware
profile here distinguishes them. The long-prefix paired range still crosses
parity, and uniform data remains much slower despite reducing its index writes
to 262396, close to the chain's 262140.

Decision: retain this exact refactor and the negative results; parity is not
achieved. Further work should measure the remaining common-path costs and the
small-group promotion threshold, with the same exactness and mixed-workload
requirements. Do not infer an optimal threshold or a universal average from
seven deterministic random samples. The cold-expiration run's medians are closer
to parity, but its timing variation does not justify a precise before/after
speedup factor or attributing gains to outlining alone.

The assembly-emission commands in these performance archives also passed link
arguments, producing only unused-link-argument warnings. The runner now keeps
link arguments out of `-S` compilation; the follow-up verification archive
checks that runner correction without repeating performance measurements.

## Final local-state refinement

The retained source also keeps the small group's temporary head in a local
variable. It publishes the tree scratch root only on promotion. No unrelated
operation needs to observe the small-group cursor. This follows the house
kernel rule to accumulate locally and publish only needed intermediate state.
It does not change the algorithmic counters, which exclude scalar bookkeeping.

The `radix-lazy-local` archive measures this source with the corrected runner.
There is no clear isolated timing gain from the local-variable change; host
variation remains too large. It is retained for its simpler mutation contract,
not as a claimed additional speedup. These are the latest measurements:

| Latest workload | Median ratio | Paired range |
|---|---:|---:|
| Original random sample, 128 KiB | 0.7970 | 0.6736–0.9194 |
| Seed 12345678, 128 KiB | 0.7073 | 0.6699–0.7524 |
| Seed 9e3779b9, 128 KiB | 0.7291 | 0.6517–1.1290 |
| Seed deadbeef, 128 KiB | 0.7036 | 0.4958–0.9356 |
| Seed 12345678, 1 MiB | 0.7304 | 0.6211–1.0382 |
| Seed 9e3779b9, 1 MiB | 0.7505 | 0.5494–1.2313 |
| Seed deadbeef, 1 MiB | 0.7336 | 0.5820–0.7585 |
| Uniform | 0.1628 | 0.1245–0.2143 |
| Late mismatch | 1.0500 | 0.9657–1.3212 |
| Crowded hash buckets | 40.8729 | 31.2526–44.3009 |
| Text-input PNG interior | 1.8984 | 1.3604–1.9791 |
| Picture-box PNG interior | 1.4322 | 1.1172–1.6842 |

All 1052 deterministic fixtures and all scheduled exact-answer comparisons
passed. The final compiler log is empty. Source snapshots are SHA-256 checked
against the recorded manifest. House spelling and lifetime checks pass over
the benchmark C++ sources; Python parsing and `git diff --check` also pass.
No automated memory integrity review was run. The retained source is still an
experimental matcher, and approximately 0.70–0.80 times baseline throughput
across these random samples is progress toward, not attainment of, parity.

## Memory and remaining gates

Measured candidate index size is 753676 bytes. The arithmetic replacement
estimate is `804232 - 524288 + 753676 = 1033620` bytes, leaving 14956 bytes below
1 MiB. That is not an integrated streaming encoder layout. Input staging,
lookahead arrival, rebasing, and any additional streaming state remain gates.

The operations counters exclude scalar cursor/proof bookkeeping and initial
clearing. They do not measure physical memory traffic. Hardware counters were
not collected. These are ordinary exact-answer and performance checks, not an
automated memory integrity review. No production matcher replacement or
reference-speed claim is justified by these index measurements alone.
