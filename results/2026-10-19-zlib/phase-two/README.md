# Phase 2: bounded streaming inflate (gate open)

The measurements below describe the initial scalar baseline at `123f13cd`.
The owner subsequently requested optimization; the [optimization evidence](optimization/README.md)
records the newer kernels, workspace, checks, measurements and remaining gaps.
Do not apply the baseline coverage or workspace numbers to the optimized source.

Owner direction: reuse suitable non-GPL source material and rewrite both PNG
and deflate in the complete house style. This authorizes implementation and
revises the previously accepted retained-libpng choice. The original separate
PR/owner-review gates remain. This phase implements inflate first; the PNG
codec, deflate writer, C shim and production cutover are later phases.

## Implementation and provenance

Native C++20 streaming raw-deflate/zlib decoder, fixed caller-owned workspace,
no allocation or exceptions in processing, explicit failure/progress records,
finite input/output/work budgets, strict advertised-window enforcement, portable
Adler verification, and terminal errors until reset. Output remains an unverified
prefix until completion. It handles stored/fixed/dynamic blocks, overlapping LZ
matches, incomplete/empty permitted distance alphabets and arbitrary byte splits.

Canonical Huffman construction and decoding derive from Mark Adler's puff at
upstream zlib `51b7f2abdade71cd9bb0e7a373ef2610ec6f9daf`; the altered-source
marking and original licence are retained and installed. See
[provenance](../../../src/core/compression/PROVENANCE.md). No alternative library
is linked into production. CMake compiles the new source and registers the native
test/fuzz targets; no production caller uses it yet.

## Environment and commands

Mac mini M4, arm64, Darwin 25.5.0, Homebrew clang/clang++ 22.1.8. Oracle reports
zlib 1.3.1 and is checked by per-file SHA-256, not that version string alone.
The isolated experiment uses the host clang configuration/runtime; it is not
represented as a macOS-14 deployment/package validation. The full-project
attempt uses the repository toolchain and the already installed pinned
LLVM 22.1.8 macOS-14 runtime.

Executed via `/Users/ultimussecundai/.local/bin/m4build`, from the authoritative
worktree, with `PATH=/opt/homebrew/opt/llvm@22/bin:/opt/homebrew/bin:/usr/bin:/bin`:

```text
python3 -B tools/validate_inflate.py --oracle build-zlib-study-sources/upstream-zlib --output build/inflate-study-strict --strict --seconds 60
python3 -B tools/validate_inflate.py --oracle build-zlib-study-sources/upstream-zlib --output build/inflate-study-ordinary --seconds 60
```

The script records commands and configures both oracle and candidate with
`-O1 -g -fsanitize=address,undefined -fno-sanitize-recover=undefined
-fno-omit-frame-pointer`, and C++20 for project code. Fuzz compilation additionally
uses `-fsanitize=fuzzer`. ASAN_OPTIONS enables leak detection and halt-on-error;
UBSAN_OPTIONS halts on errors. No leak suppressions. Original entry commands and
machine/compiler metadata are in the two `*-run.json` files; the script now also
records source digests before compilation. `source-and-corpus-sha256.json`
identifies the reviewed core, final tests and checked-in seed corpus. Extra
lifecycle/malformed tests were added after the smoke campaigns and rerun under
the instrumented coverage executable; decoder/fuzzer sources did not change.

Log content is retained with trailing whitespace normalized for Git.

## Observed results

- Native conformance/lifecycle fixtures pass under ASan/UBSan/LSan.
- Both ordinary and strict pinned-oracle test runs pass 700 positive cases each:
  levels 0–9, windows 9–15, all five strategies, raw and zlib framing, one-byte
  input/output. This is decoding of oracle-produced streams, not a claim that
  the not-yet-written owned encoder passes round trips.
- Strict differential fuzz smoke: **291102 executions, 61 seconds**; no
  sanitizer report, crash or accept/output/boundary disagreement.
- Ordinary differential fuzz smoke: **302423 executions, 61 seconds**; same
  result. This does not prove ordinary/strict tolerance disagreements absent
  from all inputs; each observed future discrepancy still needs resolution.
- Native workspace: **35120 bytes**. No heap allocation by the decoder.
  Unsanitized LLVM `-O1 -fstack-usage` reports all first-party decoder frames
  static; summing all frames conservatively gives **400 bytes**, with no
  recursion. Workspace plus this core-frame bound is **35520 bytes**, below
  65536. These measurements exclude caller buffers and runtime/sanitizer
  infrastructure. The instrumented `.su` file is retained separately and has
  dynamic markers from sanitizer instrumentation; it is not a static bound.
- Instrumented ordinary fixtures (without optional oracle) cover **95.76% of
  regions, 98.29% of lines and 90.65% of branches**, all 13 functions executed,
  in `inflate.cpp`. Raw coverage and annotated source are retained. Remaining
  uncovered lines are defensive unreachable phase/default exits; uncovered
  branches still include combinations of argument/budget checks and require
  review before the gate closes. No 100% coverage claim.
- Both full first-party checkers: **1175 files, 0 findings**; new fuzz source
  checked explicitly: **1 file, 0 findings**. These are spelling/lifetime-rule
  scanners, not semantic proof. An initial invocation incorrectly supplied a
  Python file to the C++ checker and was rejected; corrected scopes passed.

Work accounting charges every processing state, input byte, bit extraction and
emitted byte, with fixed bounded charges for table construction and repeats.
Buffer-resumption attempts also spend work; resource exhaustion is deliberately
separated from malformed-input differential results. This scalar baseline
makes no throughput or compression-ratio claim.

## Negative evidence and unresolved gates

1. The first fuzz smoke completed **331872 executions in 61 seconds**, then
   failed shutdown leak detection: **56 bytes in two allocations** originating
   in LLVM libFuzzer's detached RSS monitor, per the retained stack trace.
   `fuzz-initial-runtime-leak.log` is a failure, never counted as a pass.
   The reruns set `-rss_limit_mb=0 -malloc_limit_mb=0` to avoid creating that
   monitor; leak detection stays enabled without suppressions. The LLVM source
   [StartRssThread](https://github.com/llvm/llvm-project/blob/llvmorg-22.1.8/compiler-rt/lib/fuzzer/FuzzerDriver.cpp)
   has an explicit zero-limit return. This removes the process RSS watchdog,
   not the decoder's budgets. Long campaigns need external supervision.
2. The direct full-project `tools/ci_build.py --sanitize --tests
   'gui_forms_inflate.*'` attempt failed before build because the M4 mirror has
   a hashed basename while the helper expects a `gui_forms` checkout. Retained
   in `full-project-attempt.log`.
3. The corrected logical-layout attempt began compiling pinned Skia, but a
   subsequent mirror sync removed remote-only dependency files and interrupted
   it. This was an execution mistake, not a codec failure. The failed log is
   retained. The dependency checkouts created by this experiment were restored
   from their existing pins, declared patches reapplied by the fetch script,
   and the build retried without further sync. Do not overlap m4build syncs
   with active builds; AGENTS.md now records that constraint.
4. The restored full-project macOS sanitizer build succeeded and its targeted
   CTest passed: 1/1 `gui_forms_inflate_tests`, 0.74 seconds. This builds the
   toolkit using the prescribed toolchain/runtime; the full suite was not run.
   The successful log is `full-project-retry.log`.
5. Full project/platform validation and the accepted 24-hour campaigns on x64
   and arm64 remain required. The two passing one-minute smoke runs total
   **122 seconds (0.0339 hours)**, not 24 hours. No background campaign is
   implied by this report. No Linux/Windows local pass, full sanitizer-matrix
   closure, PNG differential pixel result, benchmarks or production acceptance
   is claimed here.

The phase remains a draft until the required gates are met. Style conformance
makes ownership and state transitions inspectable; sanitizer and differential
results are separate evidence and remain mandatory.
