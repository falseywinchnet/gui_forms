# `stx_zlib` / `stx_png` focused audit record

Date: 2026-10-20. Source: `codex/png-owned-codec` at `3cff2bcb`, plus the
uncommitted protocol harness and seed generator listed below. Host: M4 Mac
mini (`m4mini` selected by `m4host`), macOS arm64, LLVM 22.1.8 from the pinned
GUI.Forms toolchain. All fuzz targets were built with ASan and UBSan; leak
detection was enabled in their execution environment. These results cover the
current proving decoder, not completed standalone libraries or a shipping
replacement for zlib/libpng.

## Scope and evidence

- Source review covered `src/core/compression/inflate.cpp`,
  `src/core/image/png_decoder.cpp`, `src/core/image/png_writer.cpp`, their
  headers, and the focused test/fuzz interfaces. No concrete memory error was
  found in that review. The decoder's stated unverified-prefix rule is
  essential: callers must discard all output on any failure.
- `clang++ --analyze` on those three translation units emitted no diagnostic.
  Its absence of findings is not proof of memory or semantic safety.
- The existing inflate CTest, PNG decoder CTest, and deterministic protocol
  CTest passed in the LLVM 22 non-sanitized build. The PNG tests exercised
  15,300 format/filter/Adam7 cases and generated a 156-file decode corpus.
- `gui_forms_png_decode_fuzz` ran 60 seconds on that corpus with
  `-rss_limit_mb=0 -max_len=65536 -timeout=5`, completing 890,048 executions
  and adding 1,450 corpus units. Exit code 0; no ASan, UBSan, or leak report.
- `gui_forms_inflate_protocol_fuzz` first ran unseeded for 60 seconds with
  `-rss_limit_mb=0 -max_len=4096 -timeout=5`: 13,236 executions, exit 0,
  no sanitizer report. With ten manually prefixed inflate seeds, it completed
  3,148 executions in 60 seconds, exit 0, no sanitizer report.
- `tools/seed_inflate_protocol.py` deterministically creates three input and
  output schedules for each of the ten checked-in inflate fixtures. All 30
  seeds replayed under the sanitizer-enabled target with `-runs=0`, exit 0.
  A 60-second mutation run on those 30 seeds completed 4,622 executions and
  added 138 units, exit 0; no sanitizer report. Its peak reported RSS was
  59 MiB. The corpus and generated mutations live in the disposable remote
  build tree, not in the source tree.
- A fresh LLVM 22 CMake tree configured with ASan and UBSan flags built the
  four focused CTest binaries. With `ASAN_OPTIONS=detect_leaks=1:halt_on_error=1`
  and `UBSAN_OPTIONS=halt_on_error=1`, `ctest --test-dir
  build/stx-audit-sanitize --output-on-failure -R
  'gui_forms_(inflate_protocol_tests|inflate_tests|png_decoder_tests|png_writer_tests)'`
  passed 4/4 in 3.15 seconds with no sanitizer report. This focused tree
  disabled Skia, native host and HarfBuzz, so it does not replace the full
  platform suite required before production cutover.
- `python3 -B tools/check_house_style.py include src demo tests examples tools
  file_manager_demoboard stx_vorbis` and the matching lifetime checker both
  reported zero findings across 1,181 files. The focused checks on the new
  protocol harness also reported zero findings. `git diff --check` was clean.

The first protocol run used libFuzzer's default RSS watchdog and exited with
a 56-byte LSan allocation attributed to `fuzzer::StartRssThread`, after 9,511
executions without a decoder finding. The reruns disabled only that watchdog
(`-rss_limit_mb=0`); leak detection stayed enabled. This is recorded as a
tool-runtime result, not suppressed evidence from the codec.

## Still open

- Full source and dependency audit of a complete owned deflate encoder and
  zlib-compatible ABI. No encoder exists in this proving implementation.
- Semantic PNG metadata and colour-profile validation; complete PNG encoder;
  exact metadata-capacity planning; differential output against pinned libpng.
- Standalone `stx_zlib` and `stx_png` libraries with installed headers,
  package exports, independent consumer tests and lifecycle documentation.
- All-host production migration, link-map proof that excluded zlib/libpng paths
  are absent, full sanitizer suite, sustained fuzz campaigns, and measured
  decode/encode optimization across representative workloads.

No claim of full security audit or production readiness is made while these
gates remain open.
