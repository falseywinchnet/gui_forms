# Inflate verification

`inflate_tests.cpp` runs in the ordinary CTest suite without an external codec.
It covers stored/fixed/dynamic blocks, one-byte buffers, exact tails, truncated
input, malformed alphabets/repeats/symbols/distances, wrapper checks, dictionaries,
quotas, history wrap, overlap rejection and reset/failure behavior.
`inflate_fixture.hpp` was generated with Python zlib 1.2.12 from 40000 bytes
whose byte at index i is i modulo 13. It is generated fixture data, not codec
source. The checked-in 1x1 red RGBA PNG in `assets/fixtures/compression` was
constructed from PNG fields with the same Python zlib producer. The fuzz corpus
contains its IDAT, raw stored/fixed/dynamic streams, dictionary and reserved-block
cases. Corpus byte zero selects raw (even) or zlib (odd) framing.

`tools/validate_inflate.py` builds an isolated, instrumented oracle experiment
with LLVM 22.1.8, ASan/UBSan/LSan, explicit source digests and no suppressions:

```sh
python3 -B tools/validate_inflate.py --oracle build-zlib-study-sources/upstream-zlib --output build/inflate-study-strict --strict --seconds 60
python3 -B tools/validate_inflate.py --oracle build-zlib-study-sources/upstream-zlib --output build/inflate-study-ordinary --seconds 60
```

Provide upstream zlib at `51b7f2abdade71cd9bb0e7a373ef2610ec6f9daf`; the script
never fetches it. M4 execution uses `m4build` with LLVM 22.1.8 on PATH. The oracle
adds 700 positive streaming cases: 10 levels, 7 windows (9–15), 5 strategies and
2 wrapper modes, each with both one-byte input/output and whole-buffer decoding. Window 8 is covered by the
native contract tests; upstream deflate's window-8 behavior is not relabelled
as a window-8 oracle. The fuzz harness compares whole-buffer and byte-split
execution, then checks acceptance, exact output and stream boundary against
the selected oracle. Resource-limit results are separated from format errors.

Use `--seconds 86400` for each accepted 24-hour campaign on each architecture.
Short smoke campaigns do not pass that gate. Fuzzer coverage counters are not
source-region or branch coverage; collect and inspect the latter separately.
The GUI.Forms decoder and oracle are instrumented in the experiment; this does
not substitute for full-project sanitizer runs via `tools/ci_build.py` on
Linux, macOS and Windows, or the later PNG/shim integration campaigns.

The M4 smoke run exposed a shutdown leak in LLVM libFuzzer's detached RSS
monitor. The validation runner disables that thread with `-rss_limit_mb=0`
(and the malloc limit with `-malloc_limit_mb=0`), while retaining ASan/UBSan,
LSan and libFuzzer leak detection without suppressions. The fuzzer therefore
has no process-RSS watchdog; the candidate's finite per-stream budgets remain
active. Preserve the original failure and use external process supervision for
long campaigns. LLVM 22.1.8 `FuzzerDriver.cpp:StartRssThread` documents this
zero-limit path. This is not a source-level fix to libFuzzer.

Bulk-kernel tests also exercise 19 match distances against 19 output chunk
sizes, history wrap, exact trailing bytes, 300 chunk sizes on the dynamic
fixture, and maximal stored blocks of 0xff around Adler's 4096-byte batch
boundary. Two additional seeds contain 16384 bytes of i modulo 13, encoded
with Python zlib 1.2.12 in raw/zlib framing, within the fuzzer's output cap.

`tools/benchmark_inflate.py` builds the original owned implementation, current
implementation and a compiler-vectorization-disabled ablation with the same
release flags, each also timing pinned upstream zlib. See the
[measurement record](https://github.com/malkuth-software/notes/blob/gui_forms/results/2026-10-19-zlib/phase-two/optimization/README.md)
for limitations. On an M4 mirror, provide `--baseline` pointing to the exported
`123f13cd` header/source; the runner verifies their hashes. The normal checkout
can omit that option to read the same revision from Git.

## Fuzzing and sweeps

The sustained campaign is retired (owner revision of decision 6A,
2026-10-20). Evidence comes from the deterministic malformed-input sweeps in
the normal suite (`gui_forms_malformed_sweep_tests`,
`gui_forms_png_malformed_sweep_tests`), run under the sanitizers, and from the
whole-suite sanitizer job. `.github/workflows/fuzz-smoke.yml` runs a few
minutes of libFuzzer per target per change on GitHub's runners, including
the pinned-zlib oracle in ordinary and strict modes. That run is a build
check, not a certificate, and it never runs on the owner's machine.
