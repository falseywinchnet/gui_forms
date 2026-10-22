# stx_zlib and stx_png: general-availability record

Date: 2026-10-21. This record lists every gate from [README §10](README.md)
and the PNG phase's gates ([PNG_CODEC.md](PNG_CODEC.md)), as revised by the
owner on 2026-10-20 ([README §11](README.md)). For each gate it gives the
evidence and where that evidence lives.

"CI" means the GitHub workflows on all four platforms: macOS arm64, Linux
x64, Linux arm64 and Windows x64.

**Status as of writing: not yet GA.** The work is a stack of pull requests
that are not merged, and their CI is not yet green:

| PR | Branch | Content |
|---|---|---|
| #235 | `claude/z-s-safety` | State-machine audit, malformed-input sweeps, fuzz smoke |
| #236 | `claude/z-checksums` | Phase-4 checksum consolidation |
| #238 | `claude/z-i4-stx-png` | The stx_png package |
| #239 | `claude/z-i5-remove` | Skia's libpng and zlib removed; FreeType's zlib compiled out |
| (this) | `claude/z-ga-record` | Differential CI job and this record |

GA holds once all of them are merged and their CI is green. Two items in
the evidence column are still waiting on that CI: the first codec-free Skia
build and the first differential run.

## Gates and evidence

| Gate | Requirement (as revised) | Evidence | State |
|---|---|---|---|
| Phase 2: inflate | Bounded streaming inflate. Malformed vectors, differential against pinned zlib, sanitizers. | `stx_zlib/src/inflate.cpp`. Tests: `gui_forms_inflate_tests`, `gui_forms_inflate_protocol_tests`, `stx_zlib_core_tests`, `gui_forms_malformed_sweep_tests`. `codec-differential.yml` runs the inflate checks against pinned zlib 51b7f2ab in ordinary and strict modes on x64 and arm64. The Linux CI sanitizer job and local ASan/UBSan cover it too. | met once this PR is green |
| Phase 3: deflate | Levels, finite workspace, streaming, identity across platforms, round trips, measurements. | `stx_zlib/src/deflate.cpp`. Tests: `stx_zlib_deflate_tests` (round trips, schedules, limits), `stx_zlib_deflate_oracle` (our deflate into Python's zlib), `gui_forms_deflate_fuzz_tests` and the deflate fuzz smoke (split versus whole identical, inflate restores). Cross-host bytes: `png_writer_tests` identity hash `0x8728e27ff2636f0e`. Measurements: the phase-three results and [results/2026-10-21-png-encoder](../../../results/2026-10-21-png-encoder/README.md). | met with #231 (merged) |
| Phase 4: checksums | One CRC-32 and one Adler-32. Vector paths are optional and need measurement first. | `stx_zlib/checksum.hpp`. Tests: `stx_zlib_core_tests` (references, boundaries, continuation). Timing: [results/2026-10-21-checksums](../../../results/2026-10-21-checksums/README.md). No vector paths. | met with #236 |
| Phase 5: C shim | Dropped from GA by the owner on 2026-10-20; nothing uses it. | none | dropped |
| Phase 5b: PNG decode | Every legal colour type and depth plus Adam7; limits; failure publication; differential against pinned libpng; malformed corpus. | `stx_png/src/decoder.cpp`. Tests: `gui_forms_png_decoder_tests`, `gui_forms_png_malformed_sweep_tests` (183,358 decodes per run). `codec-differential.yml` compares samples with pinned libpng d5515b5b. | met once this PR is green |
| Ancillary-chunk policy | Owner decisions D1 (iCCP: refused for resources and GUI.Drawing, ignored for the clipboard), I6 (gAMA/cHRM converted; cICP accepted only when sRGB-equivalent) and 5A (strict). | `png_pixels`, `png_color`, `color_conversion` and `png_registry`. Tests: `png_pixels_tests`, `png_color_tests`, `png_registry_tests`, `drawing_skia_tests`. Record: [PNG_CODEC.md](PNG_CODEC.md). Language-tag registry semantics are not validated; iTXt text encoding is. | met; language tags open, see below |
| Output conversion | One kernel with a stated rounding rule; byte-identical on every host. | `stx_png/pixels.hpp`. Tests: `png_pixels_tests`, with four identity hashes over 180 images. | met with #225 (merged) |
| Encoder | Owned on stx_zlib; filter selection; deterministic; level chosen from measurement. | `stx_png/src/encoder.cpp`, default level 6. Tests: `png_writer_tests` with its identity hash. | met with #231 (merged) |
| Colour conversion | Deterministic gAMA/cHRM to sRGB, with a stated pipeline, rounding and bound. | `color_conversion`. Tests: `png_color_tests`, against host-libm and BT.2087 references, with three identity hashes. | met with #230 (merged) |
| Replace sustained campaigns | Safe by construction; audit; deterministic sweeps; differential checks; sanitizers; fuzz smoke as a build check only. | [STATE_MACHINE_AUDIT.md](STATE_MACHINE_AUDIT.md), the sweep tests, `codec-differential.yml`, `fuzz-smoke.yml` (150 s per target, x64 and arm64), the Linux CI sanitizer job, and local sanitizer runs for each PR. | met once #235 and this PR are green |
| Packaging | Installable packages; GUI.Forms consumes their targets; licences and provenance installed. | `stx_zlib/` and `stx_png/` packages. `stx-packages.yml` (standalone, installed, relocated, consumed). `GUIFormsConfig.cmake.in` finds both. Program notices are in `THIRD_PARTY_NOTICES.md`. | met once #238 is green |
| Link map | One implementation: no external zlib, no Chromium zlib, no second copy. | `tools/audit_link_map.py --forbid-foreign` in native CI over every toolkit and Skia archive; the [Windows record](../../../results/2026-10-21-link-map/README.md). | met once #239 is green |
| FreeType | Decision 3A: zlib compiled out; compressed fonts refused. | `cmake/freetype/gui_forms_ftoption.h`; `gui_forms_freetype_zlib_tests`. | met once #239 is green |
| Old paths | WIC, ImageIO PNG and TIFF, NSBitmapImageRep PNG, SkPngDecoder and SkPngEncoder, Skia's libpng and zlib, the googlesource zlib fetch. | Removed in I2, I3 and I5. Skia's archive policy forbids their symbols. | met once #239 is green |

## Open items after GA (owner's call; none blocks GA)

1. **Audit findings (a) and (c).** Encode `bits()`'s bit count in a type,
   and give the inflater phase-scoped state (a variant). Both touch the hot
   decoder and need paired measurements.
2. **PNG iTXt language tags.** Their registry semantics are not validated.
   The text encoding is.
3. **Metadata budgets.** The decoder's provisional 4 MiB metadata limit and
   its work quota are not yet chosen from corpus evidence. The pixel path
   validates metadata through a 4 KiB scratch, so the budget bounds work,
   not storage.
4. **Clipboard dimensions.** A clipboard image wider or taller than the
   encoder's 16384-pixel limit has no PNG flavour; the raw format still
   carries it.
5. **ICC profiles.** Supporting them is a later study (owner, 2026-10-20).
