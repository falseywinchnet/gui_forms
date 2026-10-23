# Build size plan, 2026-10-23

The owner asked to "plan to reduce build sizes". This record measures where
the repository's build storage goes and weighs five ways to reduce it. It
changes no CI behaviour; each option that is accepted lands as its own PR.

Each option below gives its saving in GB, its cost in cache hit rate and its
cost in wall time, with the measurement behind each figure. A
[recommended order](#recommended-order) closes the record.

## Where the storage is

There are two budgets, and they behave differently.

**The Actions cache** is limited to 10 GB per repository. GitHub evicts the
least recently used entries when a save takes it past the limit. On
2026-10-23, after the coordinator deleted four superseded entries, it held
9.6 GB in 21 entries:

| Family | What it holds | Entries | Size each |
| --- | --- | --- | --- |
| `gui-objects-v2-<platform>` | native jobs' ccache, keyed by tree | 4 platforms | 0.57–0.66 GB |
| `gui-programs-objects-v1-<platform>` | release.yml programs ccache, keyed by source hash | 4 platforms | 0.62–0.74 GB |
| `gui-sanitize-v1-lines` | sanitizer ccache and third_party, keyed with the commit | 1 | 1.12 GB |
| `gui-deps-v3-<platform>` | third-party sources and the Skia output | 4 platforms, plus pull-request copies | 0.20–0.49 GB |
| `linux-debs-v1-*` | apt packages for the Linux jobs | 4 | 0.025 GB |
| `gui-runtimes-v1-macos-arm64` | macOS runtime prefix | 1 | 0.002 GB |

Every push to main writes a new generation. From the jobs of the 26 completed
main push runs of native.yml on 2026-10-23, and the programs saves of
release.yml, the saves per push are:

| Save | Share of pushes that save | Size |
| --- | --- | --- |
| sanitizer | 21 of 26 | 1.07 GB |
| programs objects | each programs build, 4 platforms | about 0.65 GB each |
| native objects | the jobs that built (not promoted), about 2.8 per push | 0.59 GB each |

So a push to main saves about 5 GB, half the limit, and there were 13 main
pushes on 2026-10-23. The cache therefore turns over about six times a day.
The newest generation is the only one any later job restores, so the cache
is mostly superseded generations waiting for eviction.

That turnover has costs:

- Small, still-current entries get evicted alongside the stale ones: the
  macOS `gui-objects-v2` entry, the ARM64 `linux-debs-v1-toolkit` entry and,
  earlier, the programs debs. The programs debs miss cost 10 to 12 minutes of
  apt install in each affected job.
- The `gui-deps-v3` save is not limited to main. A pull request's merge ref
  saves its own copy (`refs/pull/295/merge` held 471 MB), which no other ref
  can restore but which still counts against the limit.

**Release assets** are outside the 10 GB limit but still storage the
repository holds:

| Release kind | Count | Assets |
| --- | --- | --- |
| `build-<sha>` compiler-cache pre-releases (`tools/publish_cache.py`) | 116 | 172.7 GB |
| `v*` releases | — | 27.2 GB |

Of the `build-*` total, the ccache archives are about 34–38 GB per platform.
Every pre-release also carries its own copy of the Windows toolchain, 30.6 GB
in all. `tools/prune_cache_releases.py` can prune them but runs only as a
weekly dry report.

## Options

### (a) Prune superseded generations on main

**Change.** After a save on main succeeds, delete the older entries of the
same family and platform (`gh cache delete` by key prefix, keeping the key
just saved). Limit the `gui-deps-v3` save to main, as the object caches
already are.

**Saving.** Whatever the superseded generations occupy at the time: up to
4–5 GB of the 10 GB after a busy afternoon of merges, and the pull-request
deps copies (0.47–0.49 GB each). The steadier benefit is that LRU eviction
stops firing, so the small current entries (debs, runtimes) stay put.

**Hit rate.** No cost. Every family's restore-keys already pick the newest
entry, which is the one kept. A job running for an older main commit in the
same minutes restores the newer generation, which is what it does today when
it loses the race.

**Wall time.** A few seconds of API calls per saving job.

**Needs.** `actions: write` on the saving jobs, and a guard so a failed save
deletes nothing.

### (b) One object cache per platform for native and programs

**Overlap, measured.** A configured Windows native tree has 709 compiled
objects and a programs tree 807. Every one of the 709 native objects appears
in the programs tree with the identical compile command; programs adds 98
objects (about 12%). So a ccache filled by a programs build serves the whole
native build. release.yml already falls back to `gui-objects-v2` in its
restore-keys and unions the native artifact's ccache into its own.

**Change.** Native and programs restore and save one family per platform.
One job per push saves it (the programs job, since its set contains the
other).

**Saving.** The four `gui-objects-v2` entries: about 2.4 GB when all four
are present (1.8 GB on 2026-10-23, with macOS evicted).

**Hit rate.** None measured: the shared cache is a superset of what native
uses. Its 500 MB ccache limit has to hold both sets, so this lands after (d),
which measures the working set.

**Wall time.** Native restores the 98 extra objects' entries. Restores take
0.1 to 0.4 minutes today, and a 12% larger archive moves that by seconds.

### (c) Third-party builds as pinned release assets

**Change.** Pack the third-party sources and the Skia output once per pinned
dependency set as a release asset, as the Windows toolchain already is, and
fetch it by pinned hash instead of restoring `gui-deps-v3`.

**Saving.** The `gui-deps-v3` family, about 1.5 GB (four platforms at
0.20–0.49 GB), plus the third_party part of the sanitizer cache.

**Hit rate.** It stops being a cache, so it can't miss through eviction. A
miss today costs about 54 seconds of fetching; the Skia rebuild itself took
3 seconds with a warm ccache.

**Wall time.** A release download is about as fast as a cache restore. The
cost is the work: a pack job like the toolchain's, its pin in the build, and
its own pruning.

### (d) ccache compression and contents

**Measured.**

- The caches are full: the native and programs ccaches sit at their 500 MB
  limit and the sanitizer at 0.9 GB of its 1 GB, with 11,000 to 13,500 files
  each.
- One Windows Release generation of the objects compresses to 20.8 MB at
  ccache's default zstd level 1. Level 5 saves 6% of that and level 10 saves
  10%, so compression level is not worth changing.
- Release objects carry no debug info (0.12 MB of 29.6 MB of sections), so
  debug info in cached Release objects is not the cost either.

So the caches are large because they keep many old generations of the same
objects up to the size limit, not because each entry is large.

**Change.** Before each save on a main push, run
`ccache --evict-older-than <seconds since the job started>`, which removes
every entry this job neither created nor hit. Pushes to main build in full
(change selection narrows only pull requests), so what remains is the whole
current working set.

A local trial with ccache 4.14.1 confirmed the behaviour the change relies
on. A hit refreshes the entry's timestamp, so `--evict-older-than` kept the
hit entry and removed the one the job did not use. Cache archives keep file
timestamps through save and restore, so the age test also works across jobs.

**Saving.** One generation is about 20–25 MB compressed for a Release
platform. The eight object entries would fall from about 0.6 GB each to
somewhere in 0.05–0.1 GB (allowing headroom for platform differences and the
manifest files), saving about 4.5–5 GB. The sanitizer cache would fall from
about 0.9 GB to about 0.3 GB of ccache.

The same change shrinks the native jobs' `gui-forms-cache` artifacts
(530–643 MB each) and the `build-*` cache assets, which pack the same ccache
directory.

**Hit rate.** On main, none: the next push's sources differ from this one's
only by its own change, and its unchanged objects are all in the current
working set. A pull request built on an older main can miss more than today,
since today's cache may still hold its base's objects. That needs measuring in
the trial, and the fallback is a window instead of the job age (for example
`--evict-older-than 2d`), which keeps recent generations at a few times the
size of one.

**Wall time.** Seconds for the eviction, and smaller saves and restores.

### (e) Build output sizes

**Measured on a Windows programs tree (Release, 806 objects).**

- The objects take 77 MB on disk, but only 29.6 MB of that is section
  contents. The rest is COFF symbol tables, relocations and long COMDAT names.
- Of the section bytes, text is 22.6 MB (76%), read-only data 3.8 MB, unwind
  tables 2.4 MB and debug info 0.1 MB.
- By target, `gui_forms_controls` has the most section bytes at 5.7 MB. The
  gallery model and the core have 2.0 and 1.9 MB, and HarfBuzz 1.8 MB.
- Standard library templates are 4.15 MB of the 8.5 MB of COMDAT text.
  Functions emitted in more than one object repeat 5.02 MB of copies, which
  the linker folds.
- The static archives total 56 MB, `gui_forms_controls` 19.7 MB of it.
- The 235 test executables are statically linked and total 881 MB per tree.
  This is the largest single output.

**Measured on the local sanitizer tree (741 objects).** It holds 1.07 GB of
objects, 86% of the section bytes debug info. That tree predates the move to
line tables (`gui-sanitize-v1-lines`), so the figure overstates the CI cache.
It shows only that debug info decides the sanitizer's size, which line tables
already address.

**Cheap reductions, measured on one test executable (13.28 MB).**

| Link change | Size | Change |
| --- | --- | --- |
| `--gc-sections` | 12.40 MB | −6.6% |
| `--gc-sections` and `-s` | 9.09 MB | −32% |

The build does not use `-ffunction-sections` or `-fdata-sections` today, so
`--gc-sections` alone can discard little. With both added the saving is
likely larger, which a trial would measure.

**Saving.** About 0.28 GB per tree from stripping and collecting sections in
test executables, applied to the Release and native trees. That affects job
disk, artifacts and any uploaded tree, not the Actions cache (the cache holds
ccache, not linked outputs). Leave sanitizer test executables unstripped,
since their reports need symbols.

**Hit rate.** None for (e) alone. Adding `-ffunction-sections` changes every
compile command, so it costs one full cold build per platform when it lands.

**Wall time.** Link time changes little for `--gc-sections`, and stripping is
seconds.

## Release assets (owner decisions)

These are outside the cache limit and are the larger store. Each is the
owner's call:

1. Run `tools/prune_cache_releases.py --apply` on its weekly schedule instead
   of only reporting, keeping the newest pre-releases per platform. This frees
   most of the 172.7 GB.
2. Stop copying the Windows toolchain into every `build-*` pre-release, and
   point them at the one pinned toolchain release. This saves 30.6 GB now and
   about 0.26 GB per pre-release from then on.
3. Leave `v*` releases (27.2 GB) as they are; they are the published record.

## Recommended order

1. **(d) Evict entries the job did not use, before each save on main.** This
   is the largest cache saving (about 4.5–5 GB of objects, 0.6 GB of
   sanitizer) and a one-line step per saving job. It also shrinks the
   artifacts and release assets. Trial it on one platform first and compare
   the next pull requests' hit rates.
2. **(a) Prune superseded generations on main, and stop pull-request deps
   saves.** Once (d) makes each generation small, this keeps one generation
   per family and stops LRU eviction of the small current entries.
3. **(b) Merge the native and programs object caches per platform.** This
   saves about 2.4 GB before (d), about 0.3 GB after it. It removes a
   duplicate family and simplifies restore-keys. It lands after (d), because
   the shared cache's size limit is set from (d)'s measured working set.
4. **(e) `--gc-sections` and stripping for non-sanitizer test executables**,
   with a trial of `-ffunction-sections` and `-fdata-sections`. This saves
   about 0.28 GB per tree on disk and in artifacts, with one cold build.
5. **(c) Third-party builds as release assets.** This saves about 1.5 GB of
   cache but is the most work. Once (a) to (d) bring the cache to about 2 GB,
   it is no longer needed for the limit. Do it only if eviction returns.

The two release-asset decisions above sit beside this order. They free far
more storage than any cache option and need no CI change beyond the pruning
schedule.

## Method

- Cache entries: `gh api repos/falseywinchnet/gui_forms/actions/caches`,
  2026-10-23 21:25 UTC.
- Saves per push: the jobs and steps of every completed main push run of
  native.yml since 2026-10-23 00:00 UTC, counting successful
  `actions/cache/save` steps per job.
- Overlap: object lists from configured native and programs trees
  (`ninja -t targets all`, objects only), compared by path and by compile
  command (`ninja -t commands`).
- Object sizes: `llvm-size -A` over every object, grouped by section kind and
  by target directory. Template share: section sizes of the `.text$<symbol>`
  COMDATs from `llvm-objdump -h`, demangled with `llvm-cxxfilt`.
- Compression: one generation's objects packed into a fresh ccache at levels
  1, 5 and 10 (`ccache -X`), comparing `ccache -s` sizes.
- Executable sizes: one test executable relinked with `-Wl,--gc-sections`,
  then with `-s` added.
- Eviction trial: a fresh ccache with two entries aged three hours, one hit,
  then `ccache --evict-older-than 1h`. The hit entry stayed and the other was
  removed (ccache 4.14.1).
- Release assets: `gh release list` and `gh api .../releases`, summing asset
  sizes by release-name prefix.
