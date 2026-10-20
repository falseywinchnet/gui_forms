# Zlib phase 1 evidence — 2026-10-19

Scope: documentation and source inspection only. The
[study and threat model](../../planning/stages/Z-zlib/README.md) is awaiting
owner review. This record is not a conformance, performance or security pass
for an implementation. No replacement code exists in this phase.

## Environment and isolation

- GUI.Forms source base: `6a120a39aad43ae42ae9dcc35ecd7dbd7e220310`.
- Branch: `codex/zlib-phase-one-study`, based on `origin/main` after checking
  that a fetch did not move the base.
- Managed worktree:
  `/Users/ultimussecundai/.codex/worktrees/zlib-phase-one-study/gui_forms`.
- Local machine as reported by `sysctl`: `Mac17,5`, Apple A18 Pro,
  8,589,934,592 bytes RAM.
- `uname -sm`: `Darwin arm64`.
- `sw_vers`: macOS 26.7.1, build `25G241`.
- Preprocessor: Apple clang 21.0.0 (`clang-2100.3.34.2`), target
  `arm64-apple-darwin25.6.0`. Used only for source preprocessing; this is not
  the repository's required LLVM 22.1.8 build toolchain.
- Python 3.15.0; GitHub CLI 2.102.0 (2026-09-30).
- No Mini, Linux or Windows machine was used; no CPU-heavy build was run.
- Original checkout's two modified macOS files and untracked `.claude/`
  were left untouched. No other agent's implementation files were edited.

Environment commands actually run:

```sh
git fetch origin main
git rev-parse HEAD origin/main
git log -1 --format='%H %s'
git status --short
uname -sm
sw_vers
sysctl -n hw.model machdep.cpu.brand_string hw.memsize
clang++ --version
python3 --version
gh --version
```

## Source retrieval

The following source-only fetches succeeded. They are authorized pinned
dependencies or the named comparison library, not added production
dependencies. Sources were initially placed in `.study-sources/`, then moved
to ignored `build-zlib-study-sources/`; none is staged in this PR.

```sh
git switch -c codex/zlib-phase-one-study
mkdir -p .study-sources
git init -q .study-sources/zlib
git -C .study-sources/zlib fetch --depth=1 https://chromium.googlesource.com/chromium/src/third_party/zlib 646b7f569718921d7d4b5b8e22572ff6c76f2596
git -C .study-sources/zlib checkout -q --detach FETCH_HEAD
git init -q .study-sources/libpng
git -C .study-sources/libpng fetch --depth=1 https://skia.googlesource.com/third_party/libpng.git d5515b5b8be3901aac04e5bd8bd5c89f287bcd33
git -C .study-sources/libpng checkout -q --detach FETCH_HEAD
git init -q .study-sources/freetype
git -C .study-sources/freetype fetch --depth=1 https://gitlab.freedesktop.org/freetype/freetype.git f4205da14867c5387cd6a329b90ee10a6df6eeff
git -C .study-sources/freetype checkout -q --detach FETCH_HEAD
git init -q .study-sources/skia
git -C .study-sources/skia fetch --depth=1 --filter=blob:none https://skia.googlesource.com/skia.git 2a9b593bab4b2fd019fa494c8d401ff1fab0b883
git -C .study-sources/skia sparse-checkout set src/codec src/encode src/pdf src/utils third_party/zlib third_party/libpng gn
git -C .study-sources/skia checkout -q --detach FETCH_HEAD
git init -q .study-sources/upstream-zlib
git -C .study-sources/upstream-zlib fetch --depth=1 https://github.com/madler/zlib.git 51b7f2abdade71cd9bb0e7a373ef2610ec6f9daf
git -C .study-sources/upstream-zlib checkout -q --detach FETCH_HEAD
```

Comparison tags were resolved with these commands (no zlib-ng or libdeflate
implementation checkout was made):

```sh
git ls-remote https://github.com/madler/zlib.git refs/tags/v1.3.1 refs/tags/v1.3.1^{}
git ls-remote https://github.com/zlib-ng/zlib-ng.git refs/tags/2.2.4 refs/tags/2.2.4^{}
git ls-remote https://github.com/ebiggers/libdeflate.git refs/tags/v1.24 refs/tags/v1.24^{}
```

Results: upstream v1.3.1 peels to
`51b7f2abdade71cd9bb0e7a373ef2610ec6f9daf`; zlib-ng 2.2.4 resolves to
`860e4cff7917d93f54f5d7f0bc1d0e8b1a3cb988`; libdeflate v1.24 peels to
`96836d7d9d10e3e0d53e6edb54eb908514e336c4`.

Primary reference pages read through the web tool: RFCs 1950/1951/1952,
the W3C PNG Third Edition compression specification, the pinned Chromium
tree, Chromium's BSD licence, the comparison licences, libdeflate's README
and the Silesia corpus description. Links are in the study. No corpus or
unapproved package was downloaded.

## Active libpng subset

Source search before preprocessing:

```sh
rg -n '\b(inflate\w*|deflate\w*|crc32\w*|adler32\w*|zlibVersion|zError|compress\w*|uncompress\w*)\s*\(' .study-sources/libpng/png*.c
rg -n 'ADLER|MAXIMUM_INFLATE|SET_OPTION|WRITE_CUSTOMIZE_ZTXT|READ_iCCP|READ_iTXt|READ_zTXt|WRITE_FLUSH' .study-sources/libpng/scripts/pnglibconf.h.prebuilt .study-sources/libpng/pngpriv.h .study-sources/skia/src/codec/SkPngCodec.cpp
```

Negative result: the first preprocessing attempt failed because the sparse
Skia checkout had no `third_party/externals/libpng` checkout, required by
Skia's relative include of `scripts/pnglibconf.h.prebuilt`. Partial output
from that failed attempt was discarded as evidence. A second run with the
dependency symlink succeeded but emitted 15 macro-redefinition warnings:
`-DPNG_SET_OPTION_SUPPORTED` gives value `1`, while the pinned header gives
an empty definition. The final run used the equivalent empty command-line
definition and completed with no diagnostics. No source was edited.

Commands that established the dependency layout:

```sh
mkdir -p .study-sources/skia/third_party/externals .study-sources/preprocessed
ln -s ../../../libpng .study-sources/skia/third_party/externals/libpng
mv .study-sources build-zlib-study-sources
```

Final successful preprocessing and extraction, exactly as run:

```sh
for unit in png pngerror pngget pngmem pngpread pngread pngrio pngrtran pngrutil pngset pngtrans pngwio pngwrite pngwtran pngwutil; do
  clang -E -P -DPNG_SET_OPTION_SUPPORTED= -Ibuild-zlib-study-sources/skia/third_party/libpng -Ibuild-zlib-study-sources/libpng -Ibuild-zlib-study-sources/zlib build-zlib-study-sources/libpng/$unit.c -o build-zlib-study-sources/preprocessed/$unit.i || exit 1
done
rg -n 'Cr_z_(crc32|inflate[A-Za-z0-9_]*|deflate[A-Za-z0-9_]*)\(' build-zlib-study-sources/preprocessed | rg -v ':extern ' > results/2026-10-19-zlib/libpng-active-calls.txt
```

Result: all 15 units preprocessed; ten distinct called symbols, recorded in
[libpng-active-calls.txt](libpng-active-calls.txt). This is source evidence,
not a link trace or a cross-platform build. `PNG_INTEL_SSE` and Arm filtering
units change PNG filters, not this C compression-call subset; a future
integration still needs real generated graphs/link maps on every platform.
The extraction is paired with the broad source search and manual conditional
review; it is not offered as a general-purpose C call-graph analyzer.

The call excerpts originate in the pinned libpng sources, copyright Cosmin
Truta, Glenn Randers-Pehrson and earlier libpng contributors, under its
[licence](https://skia.googlesource.com/third_party/libpng/+/d5515b5b8be3901aac04e5bd8bd5c89f287bcd33/LICENSE).
The `Cr_z_` spellings result from the pinned Chromium zlib header. These
inspection excerpts are not an implementation import.

## Corpus inventory and checks

```sh
git ls-files '*.png' '*.PNG' | wc -l
git ls-files 'assets/*.png' 'assets/**/*.png' 'assets/*.PNG' 'assets/**/*.PNG' | wc -l
python3 -B tools/check_house_style.py include src demo tests examples tools file_manager_demoboard stx_vorbis
python3 -B tools/check_lifetime_rules.py include src demo tests examples tools file_manager_demoboard stx_vorbis
```

Observed tracked PNG count: **93** across the repository, **0** under
`assets/`. This count is not a decoded/validated corpus inventory and includes
historical result images. The future corpus manifest must choose applicable
images, hash them, and handle provenance and duplicate content explicitly.

Checker output:

```text
1171 files; 0 spelling findings/review candidates. Semantic review still required.
1171 files; 0 lifetime-rule findings. Semantic review still required.
```

The checkers' scopes are first-party code, not Markdown style. No code file
was changed. The existing fuzz target is outside those command arguments;
new fuzz code must be checked explicitly as the study requires.

| Verification category | Phase 1 result |
|---|---|
| Source/build configuration inventory | Completed at the recorded pins; no final binary/link-map claim |
| House-style / lifetime spelling checks | Both 0 findings over 1171 files |
| Conformance / round trips / differential execution | Not run; replacement implementation does not exist |
| libFuzzer | 0 hours; no target added; coverage not measured |
| ASan / UBSan / LSan | Not run for documentation-only phase; no platform pass claimed |
| Throughput / compression ratio / peak memory | Not measured; no speed claim |
| Platform migration / dependency removal | Not performed |
| Owner gate | Pending review and decisions |

Other limitations retained: the owner's earlier HTTP 503 was not reproduced;
FreeType's archive retention/reachability was not measured; the external
Claude Artifact stage board has no available tool in this session; proposed
workspace ceilings and fuzz duration have not yet been approved.

The study's source pointers, required verification commands and owner choices
are the hand-off for the next phase. The owner relays this report to the
coordinating session; no direct message was sent there.
