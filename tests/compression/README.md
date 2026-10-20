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
2 wrapper modes, each with one-byte input/output. Window 8 is covered by the
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
