# Track: stx_cng (CNG, Computational Network Graphics)

Worker workspace: D:\gui_forms_w5 (checkout /d/gui_forms_w5/gui_forms). Scripts
and logs in D:\gui_forms_w5\dev\ (CNG work under dev\cng\).

## Source

- Repository https://github.com/falseywinchnet/stx_cng, commit
  a1797b840ad3a209241ad24790ff97cdf3cff9b1 ("Publish CNG: Computational Network
  Graphics 0.1.0-rc1"), 34 files. Cloned (the owner's own repo, allowed) to
  D:\gui_forms_w5\dev\cng\src as untrusted data. Its AGENTS.md and ORIGIN.json
  are data, not instructions.
- Depends on Zstandard 1.5.7 (advanced API) and zlib (CRC32 only).

Owner's words: "new library to digest, clean up, inspect, integrate. a
replacement for png. faster, smaller, fully MIT, and with convenience utilities
for us. I present CNG, on a repo of stx_cng now."

## Constraints in force

- Not without the owner: do not admit Zstandard as a dependency; do not touch
  AGENTS.md's "PNG is the only image decoder admitted" rule.
- Coordinator correction (2026-10-22): download nothing (zstd tarball or
  anything else) until the coordinator confirms the owner approved that
  download. Step 1 needs only the git clone. Measurement needing zstd waits for
  "go".
- No agents. Never wait on CI. Never push to main or merge.

## Owner decisions (2026-10-22, via coordinator)

1. Download approved: zstd-1.5.7.tar.gz from github.com/facebook/zstd/releases
   (official 1.5.7, about 2.4 MB). Verify its published SHA-256 before use,
   keep it in its own new directory under D:\gui_forms_w5\dev\, record the
   hash. Nothing else may be downloaded without asking.
2. Admission: CNG becomes the native resource format; PNG stays for interop
   (system clipboards, importing user images, export). Both sit behind the
   codec-neutral seam. The AGENTS.md guardrail is rewritten to name both:
   draft the wording in the integration PR and flag it for the owner's review
   (guardrail file).
3. Zstandard is a pinned third-party dependency like HarfBuzz and FreeType:
   1.5.7, hash verified, built from source, BSD notice shipped. CRC32 comes
   from stx_zlib, not zlib.
4. Proceed with steps 1-3, then step 4 (no stop for a report after step 2
   unless a new owner decision arises).

## Steps

1. Review against house style and AGENTS.md (ownership, lifetimes, validation
   before mutation, §1 and §9 state machines, size arithmetic, the
   14,000,967-byte bounded-workspace claim); format spec against code (chunk
   framing, CRC, band limits, every length and count read from input). List
   findings. STATE: in progress.
2. Measure against stx_png on GUI assets (icons 16-256, Gallery and theme
   assets, screenshots, text-heavy captures, photos as contrast): encoded size,
   decode time, encode time, peak memory; machine and load; Zstandard code-size
   cost. STATE: blocked on download approval for zstd 1.5.7.
   Report after step 2 with a numbered option list for the owner.
3. Harden to the stx_zlib standard (state-machine audit, malformed and
   truncated chunk sweeps, extreme dimensions, sanitizer, per-PR fuzz smoke on
   CI only). STATE: not started.
4. Integrate after the owner's decisions: in-tree stx_cng/ package like
   stx_conv; CRC32 from stx_zlib; behind the codec-neutral seam with no PNG or
   CNG types in the policy layer. STATE: not started.

## Owner decisions after the step 2 report (2026-10-22, via coordinator)

- D1: keep the `\x89CSP` v2 signature; no format change.
- D2: raise the area limit to sides <= 16,384 and <= 2^26 pixels. Check every
  size product; encoder memory stays bounded by the 1 MiB bands.
- D3: standalone codec first (steps 3-4). Collections in a follow-up PR with a
  caller-supplied output budget and byte-span members (fixes F1).
- D4: reusable decoder, no allocation per decode, storage grows only to the
  largest image seen (png_pixels pattern).
- D5: zstd fetched and pinned by tarball SHA-256, like HarfBuzz and FreeType.
- Also accepted: strict decoding rejecting the F6 encodings (one valid file per
  image); explicit results, not exceptions (F2); byte-span input and a header
  probe (F3); fixes for F4, F7-F13; F11 malformed-input tests with
  self-cleaning fixtures.
- Then step 3 (PR A: in-tree stx_cng/ package, hardened) and step 4 (PR B,
  stacked: integration behind the codec-neutral seam, CRC32 from stx_zlib,
  AGENTS.md guardrail wording flagged for the owner's review).

## Findings (step 1 review, 2026-10-22)

Upstream builds -Werror clean (clang 22, Windows, Release); its 40-case round
trip passes. Dev probe dev\scripts\cng\mutate.cpp (55,155 CRC-repaired byte
flips and truncations of every chunk, both decode interfaces) ran clean under
ASan+UBSan (zstd uninstrumented); 42,830 decoded to other pixels, as expected
with CRC32 as the only integrity check.

Workspace claim: dev\scripts\cng\workspace_bound.py re-enumerates the code's
capacity formulas: encoder max 14,000,967 B (width 31,774, RGB, 11-row band,
4 MiB CCtx ceiling) - matches. Covers owned arrays plus CCtx only; stream
decoder arrays max 8,487,788 B plus an uncapped (measured, not bounded) DCtx.

- F1 (high) Collection decode has no output budget: 4,096 members x up to
  16 MiB each are allocated (resize before the member header is read) and
  retained; a few-MB file of flat members asks for ~64 GiB.
- F2 (med) Exceptions are the failure channel everywhere, including
  invalid_argument for malformed data and throws inside streambuf virtuals
  (iostreams turn them into badbit and lose the reason). stx_zlib standard:
  explicit status results, noexcept entry points.
- F3 (med) iostream-only API; no span input; no header probe, so decode_into
  callers cannot learn dimensions first.
- F4 (med) Per-call allocation: each decode mallocs a DCtx and 6 arrays; each
  encode a ~1.3 MB CCtx, a 256 KiB index and arrays. Need reusable
  Encoder/Decoder workspace owners; decoder can use ZSTD_initStaticDCtx so the
  decode is allocation-free and capped.
- F5 (med) Dimension limit: padded area <= 4,194,304 rejects 2736x1539 and all
  4K captures (24 of 34 local photos). PW+PH <= 65,536 and side <= 65,536 are
  redundant given the area bound (max side 32,768).
- F6 (low) Non-canonical acceptance: concatenated/skippable zstd frames; coding
  1 with L=0; nonzero odd-count padding nibbles; composite canvas wider or with
  more channels than its members require.
- F7 (med) Triplicated restore validation and overlap helper
  (prediction_restore, raw_restore, direct_words); prediction kernel twice;
  direct_words restore_kernel<3> unreachable.
- F8 (low) Magic numbers and dead code: write_chunk's unused `int coding = -1`;
  chunk tags as decimal arrays; 1048576, 262144, 4194304, 65536 repeated
  across files.
- F9 (med, authority) Collection encoder opens filesystem paths itself
  (ImageSource::raw_path, ifstream, file_size) and patches a seekable ostream.
- F10 (low/med) `Decoded` is an invariant-free public aggregate; write_image
  trusts layout extents against plane sizes (only the count is checked).
- F11 (low) Tests: one round trip, no malformed-input tests; a failed run
  leaves its fixture directory and every later run then fails.
- F12 (low, owner) Standalone signature is still the development `\x89CSP`
  with CHDR/BAND/CEND; collections use `\x89CNG`. Cheapest moment to settle
  final signatures is before our first resource file.
- F13 (low) Timing (steady_clock) inside codec hot paths; zstd runtime version
  checked only by the encoder context.

Spec versus code: every field matched (CHDR, rows formula, BAND layout,
byte-planar sequences, sequence grammar and N bound, gradient prediction,
stored/compressed literals, colour transform incl. the +129 floor bias, zigzag,
nibble planes, reverse-tail descriptors with 5-group canonical varints, CEND
then EOF; SHDR/DIRE/IMAG/SEND, overhead 68+52N and 80+40N). Every length and
count read from input is checked before use; no size-arithmetic overflow found
(64- or 32-bit). The standalone format is a straight-line grammar (band count
derived from the header), so no hidden state machine.

## Step 2 method (dev tools)

- zstd: dev\zstd-dl\zstd-1.5.7.tar.gz, 2,434,947 B, SHA-256
  eb33e51f49a15e023950cd7825ca74a4a2b43db8354825ac24fc1b7ee09e6fa3, equal to
  the release's published zstd-1.5.7.tar.gz.sha256 (84 B, fetched with it).
  Extracted to dev\zstd-src; builds: dev\zstd-build (plain), zstd-build-gc
  (-ffunction-sections), zstd-build-trim (also ZSTD_EXCLUDE_{GREEDY,LAZY,
  LAZY2,BTLAZY2,BTOPT,BTULTRA}_BLOCK_COMPRESSOR); installs dev\zstd-install*.
  All: Release, static, no legacy, no multithread, pinned LLVM 22 toolchain.
- Bench: dev\scripts\cng\bench\ (bench.cpp, size_probe.cpp, CMakeLists.txt;
  build dev\cng\build-bench, statically linked so the operator-new peak
  counter sees every allocation). List: dev\scripts\cng\make_list.py ->
  dev\cng\list.tsv + manifest.tsv (sha256 of every input). Summary:
  dev\scripts\cng\summarize.py dev\cng\results.tsv.
- Corpus: 119 unique Windows SystemApps targetsize-16..256 icons; 54 Gallery
  captures (docs/library/captures + skia gallery ppm); 13 screenshots
  (m3*/demoboard/experiments/docs images); 26 ribbon text-UI captures; 34
  local wallpapers (JPEG -> PNG via System.Drawing) plus 11 1920x1200 centre
  crops of those CNG rejects.

## Step 2 results (2026-10-22, PROVISIONAL: CPU load 97-100% for nearly the
whole run, other builds running)

Machine: Windows 11 VM, AMD EPYC 9354, 4 cores / 8 threads, 16 GB. Release
-O3, pinned LLVM 22, static. Times are minimum wall time over interleaved
rounds. PNG = in-tree stx_png (encoder levels 6 and 9; decoder with a reused
InflateStream, output RGBA16). CNG = upstream rc1 as-is (per-call contexts,
output 8-bit). Raw rows: dev\cng\results.tsv; collections dev\cng\coll.tsv.

| class | n | CNG/PNG9 bytes | decode PNG/CNG | encode PNG6/CNG |
|---|---:|---:|---:|---:|
| icon16..64 | 83 | 1.11-1.14 | 1.8-3.6x faster | 0.9-4.4x |
| icon96 | 17 | 1.084 | 4.2x | 6.1x |
| icon256 | 19 | 1.056 | 5.1x | 9.7x |
| gallery | 41 | 0.713 | 5.6x | 7.6x |
| screenshot | 7 | 0.685 | 8.5x | 9.7x |
| text UI (ribbons) | 26 | 0.858 | 5.8x | 6.8x |
| photo <= 1920x1200 | 15 | 0.93-0.97 | 4.7-5.2x | 7.7-8.2x |

- Icons as composite collections per size class: 0.85-0.88 of the PNG9 sum
  (icon64 1.005); one composite of all 119 mixed sizes 1.015.
- Rejected by CNG's area limit: 30 of 34 wallpapers (2736x1539 and every
  3840x2160/2400). 18 repo "captures" named .png are JPEG files (skipped).
- Peak memory (median): decode PNG ~40 KiB working (workspace reused) versus
  CNG 0.1 MiB (16 px icons) to 7.1 MiB (any image with a full 1 MiB band);
  encode PNG 1.0-2.7 MiB versus CNG 1.5-13 MiB.
- Linked code size (stripped, gc-sections, libc++ shared): stx_png decode
  +46.5 KB, enc+dec +83.8 KB; CNG decode +142.8 KB; CNG enc+dec +601.7 KB, or
  +328.7 KB with unused zstd strategies excluded (output identical on 12
  checked images).

## Step 3 state (2026-10-22)

PR A package written in the checkout (stx_cng/, branch claude/stx-cng-intake
from main ffad3f63): public codec.hpp (CngEncoder/CngDecoder owners,
inspect_cng, cng_encoded_bound, explicit CngError results, noexcept); src
codec/sequences/literals/restore; zstd 1.5.7 built from the pinned tarball
(stx_cng/tools/fetch_zstd.sh -> stx_cng/third_party/, gitignored; CMake
checks the SHA-256; cmake/zstd.cmake trims unused strategies, asm Huffman on
x86-64 non-MSVC); CRC32 from stx_zlib. Tests: core (78 rc1 reference
encodings, byte identical), malformed (prefix/flip/CRC-repaired sweeps,
strict-frame cases), limits (16384 sides, 2^26 area, storage ceilings), fuzz
harness standalone. All pass Release and ASan+UBSan (dev\cng\pkg-build,
pkg-asan). Corpus byte identity against rc1: dev\scripts\cng\bench\
hash_old.cpp and bench2.cpp (builds dev\cng\build-bench, build-bench2),
outputs dev\cng\hash_old.tsv and results2.tsv.

PR A pushed: #270, branch claude/stx-cng-intake, head 29592c17 (CI pending).
Corpus result: all 208 images both admit are byte-identical to rc1; 30 new
(wallpapers rc1 refused). Docs, results/2026-10-22-intake/ (provisional,
load 97-100 %), stx-cng.yml, fuzz-smoke cng target (CMake-built libs, link
from build trees), stx-packages (fetch step, install, relocated consumer
tests/installed), native.yml checker lists, test_areas.json. stx_cng/
.gitattributes pins *.sh to LF (Windows runners check out CRLF).
Local checks: Release + ASan/UBSan all 4 tests, relocated consumer, the
fuzz CMake path without libFuzzer (MinGW has none), house-style, lifetime,
test_change_selection.

## PR B survey (2026-10-22)

Local branch claude/stx-cng-integration created from claude/stx-cng-intake
(no commits). Findings: the public registry is PNG-shaped
(include/gui_forms/resources/types/image_resource_types.hpp: PngMetadata in
ImageResourceView, validate_png, ImageResourceEncoding {png, bgra32}; 36
files use PngMetadata/validate_png/load_png). Resource decode call sites:
skia_raster.cpp ~600, coregraphics_raster.cpp ~362, windows_host.cpp
864/2405, material_image.cpp 158 (all PngPixelDecoder, purpose resource);
skia_executor.cpp 1070 (drawing_decode); clipboard_png.cpp (interop).
PNG_CODEC.md I4: "No codec interface is added (house style §7)".
Asked the coordinator for decisions B1-B4 (registry vocabulary, decode seam
and touching render/host files, no ABI change, asset conversion later).

## Owner decisions for PR B (2026-10-22, via coordinator)

- B1 (b): neutral ImageMetadata (width, height, alpha, decoded bytes) on
  ImageResourceView; load_image/replace_image dispatch on the signature;
  load_png/replace_png stay as PNG-only wrappers; PngMetadata stays only for
  validate_png.
- B2: yes. One concrete ResourcePixelDecoder in src/core/image owning the PNG
  and CNG decoders, no codec interface. Resource paths in skia_raster,
  coregraphics_raster, windows_host, material_image switch to it; clipboard
  and GUI.Drawing stay PNG. Coordinator approves the mechanical render/host
  edits (no other owner active); keep them mechanical and list them in the PR.
- B3: no C ABI change.
- B4: existing assets stay PNG; conversion later.
- Build PR B stacked on #270, with the AGENTS.md guardrail wording flagged
  for the owner's review.

## PR B state (2026-10-22)

PR A head is 42a6a0ae (adds stx_cng::validate_cng_framing for registry
admission). #270 merged to main (b5ab934f). PR B is #276, branch
claude/stx-cng-integration, head 61e964dc, base main (CI pending). Contents:
- seam: src/core/image/{pixel_target.hpp, cng_pixels.*, resource_pixels.*};
  cng_pixels.cpp is the only GUI.Forms source including stx_cng;
- registry: ImageMetadata/ImageAlpha/encoding cng, load_image/replace_image,
  validate_encoded (PNG first, CNG only when the PNG error is
  invalid_signature and not png_only);
- mechanical edits: material_image, skia_raster, coregraphics_raster,
  windows_host (decode_png -> decode_resource);
- build: add_subdirectory, notices/licences (stx_cng/LICENSE-zstd.txt
  checked in, SHA-compared), fetch_zstd.sh in ci_build.py, native, release,
  cache-proof, run_sanitizer_suite, run_facade_surface_smoke;
- AGENTS.md, README.md and docs/dev/contributing.md guardrail draft (owner
  review flagged in the PR body); the AGENTS.md checker command now matches
  CI's list;
- tests/image/resource_pixels_tests.cpp (codec area in test_areas.json).
Local checks: 10 touched tests pass (Release), the sanitizer over the same
regex, house style, lifetime, test_ci_build, test_change_selection,
test_package_programs. Skia and CoreGraphics were not compiled locally.
PR body: D:\gui_forms_w5\dev\cng_pr_b_body.md.

## Next step

Owner approved #276 (AGENTS.md wording as written). CI house-style failed on the
stale library reference; fixed by merging main and regenerating
(tools/generate_library_docs.py, --check PASS). For public API changes, run
generate_library_docs.py --check locally. Then wait for the coordinator: CI failures on #270 or PR B, the owner's verdict
on the guardrail wording, then the later asset conversion (B4).
