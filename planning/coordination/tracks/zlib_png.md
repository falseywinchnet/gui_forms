# Track: stx_zlib and stx_png (owned compression and PNG)

Written 2026-10-21, when the first worker on this track retired (context about 880k tokens).

## Where it stands
- **Merged:** #215 (consolidation), #225 (one PNG decode path on every host), #230 (gAMA/cHRM → sRGB), #231 (owned encoder, level 6).
- **Merging (line, top first):** #240 (GA record, codec-differential.yml) → #239 (I5: Skia without libpng/zlib, FreeType without zlib, no googlesource zlib fetch) → #238 (I4: stx_png package, stx-packages.yml) → #236 (one CRC-32/Adler-32) → #235 (Z-S: state-machine audit, malformed-input sweeps, fuzz-smoke.yml).
- **Records:** `planning/stages/Z-zlib/README.md` (§11 decisions), `PNG_CODEC.md`, `STATE_MACHINE_AUDIT.md`, `GA_RECORD.md`.

## Owner decisions in force
- **Codec and hosts:** owned PNG codec (Z-2B); WIC removed; the macOS TIFF clipboard fallback removed; FreeType's zlib compiled out.
- **Validation and budgets:** strict validation; 64 KiB inflate / 1 MiB deflate workspace ceilings.
- **Verification:** no sustained fuzz campaigns. Code is safe by construction (state machines, house style §1/§9), verified by sweeps, differential CI and sanitizers, plus a light fuzz smoke run per PR on CI only.
- **Colour:** gAMA/cHRM converted deterministically to sRGB. iCCP is refused in the registry and in GUI.Drawing decode, and ignored (read as sRGB) on clipboard reads. cICP: values equivalent to sRGB are treated as sRGB; anything else is refused.
- **The zlib C shim** is dropped from GA.
- **I4:** stx_png is the codec only (decoder, encoder, 8-bit kernel; depends only on stx_zlib). Colour conversion and policy stay in GUI.Forms behind a codec-neutral seam. The owner is working on a future replacement image format: keep PNG types out of the policy layer, and build no speculative codec interface.

## Open (GA_RECORD.md), for the owner to schedule
1. Audit findings (a) and (c): hot-path type and state refactors, measured first.
2. iTXt language tags are not validated.
3. The metadata budget is not yet derived from a real corpus.
4. Clipboard images over 16384 px get no PNG.
5. ICC support remains a later study.

## Working rules
Read `D:\gui_forms_workers\BRIEF.md`, especially "Verification and CI":
- touched tests plus the sanitizer over them; no full suite;
- never wait on CI: push, report in five lines, end your turn;
- stacks at most two deep.
