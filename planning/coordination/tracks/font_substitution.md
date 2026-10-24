# Stage F track: font substitution (worker w4)

Updated 2026-10-23. Workspace: `D:\gui_forms_w4` (checkout `/d/gui_forms_w4/gui_forms`, read at
origin/main `8e9be54a`). Read `D:\gui_forms_workers\BRIEF.md` first (never wait on CI, touched
tests + sanitizer only, `gh pr update-branch`, stacks at most two deep, no agents).

F is the last stage of release 0.2 Verified; 0.2 is declared when F's gate is recorded.
Sources: `planning/stages/F-font-substitution/README.md`, `planning/FORWARD_PROGRAMME.md` §3 F,
AGENTS.md font guardrail, design language §6.1, `assets/fonts/ATTRIBUTION.md`,
`docs/user/text-engine.md`, ADR-004.

**Hard rule from the coordinator:** no font or asset is downloaded without the owner's explicit
approval, asked with file, source, size and licence. Microsoft reference fonts are oracles only:
read by path from `C:\Windows\Fonts`, never committed; record version and SHA-256.

## State

- F0 (plan): accepted 2026-10-23. The owner took every recommendation (1a to 11a below). The
  stage brief records them as its decisions 4 to 14 (plan decision N is brief decision N+3).
- F1 (measurement tool): PR #301, `claude/f-metrics-harness` @ 06ceaf45, on main. Owner ruling
  2026-10-23: F's reference is OUR ENGINE on both sides (reference file and substitute through the
  same pinned FreeType/HarfBuzz); never a host renderer; never propose changing the engine's metric
  table to match a host. GDI kept in reports as information for R/S only. Line-height criterion =
  candidate engine height within 1 px of reference engine height. Brief decisions 8 and 9 revised.
  Results `results/2026-10-23-f1-metrics-harness/` (26 runs, README with findings + face hashes).
  Ran: test normal + ASan/UBSan, checkers, docs check. CI pending.
- F3 (faces): PR #302, `claude/f-faces` @ 1b3641d8, on main. Arimo x4, Tinos x4, Selawik
  Regular/Semibold/Bold (renamed from selawk/selawksb/selawkb.ttf), OFL-Arimo/Tinos/Selawik.txt,
  ATTRIBUTION.md section with size, blob SHA-1, SHA-256. Committed blobs equal upstream blob SHA-1s.
  Not registered or packaged yet (F2 does it, plus install list in CMakeLists ~line 759 and
  THIRD_PARTY_NOTICES.md). Downloads live in `D:\gui_forms_w4\dev\fonts\<one folder each>`
  (the approval covered exactly those files; nothing else may be downloaded).
- Engine line metrics (answered to coordinator): FreeType 2.14.2 stock (only ftoption zlib off):
  typo if fsSelection bit 7 USE_TYPO_METRICS, else hhea (typo then win only if hhea asc and desc both
  zero). GRID_FIT_METRICS: ascender ceil, descender floor, height round; gap = max(0, height - asc -
  desc). USE_TYPO set: Cousine, Arimo, Portsmouth Mono, Portsmouth Rapids. Engine-internal note:
  `ResolvedTextLayout.line_gap` is always 0 (shape height = asc + desc; TextBox uses it) while
  `primary_line_metrics` adds the rounded gap (prepared text); differs only for faces with a big
  hhea gap (Calibri, Consolas via host bytes), not for any bundled role face or the new faces.
- ANSWERED 2026-10-23 (owner 1a, 2a, 3a; brief decisions 15 to 17, commit 60411ce2 on #301):
  Segoe UI -> Selawik is `nearest`; ALL of Tahoma -> Selawik, Tahoma Bold (-7.0%) a named
  exception in F4's gate; Arimo Bold U+00B5, Arimo BoldItalic 4/90 and Carlito BoldItalic 2/90
  line breaks are named exceptions in F4, those styles stay `metric_compatible`.
- Leave the TextBox zero-line-gap inconsistency alone: the coordinator logs it separately.
- F2 (table): PR #304, `claude/f-substitution-table` @ 66431cd2, stacked on #302 (base
  `claude/f-faces`; retarget to main when #302 merges). Contents:
  `assets/fonts/font-substitution.json` (schema v1: families+faces, substitutions with class and
  evidence and F4 named exceptions, aliases, generics); `cmake/FontSubstitution.cmake` validates at
  configure and generates `generated/font_substitution/font_substitution_table.inc` at build;
  public `include/gui_forms/drawing/font_substitution/font_substitution.hpp`
  (`gui_drawing::resolve_font_family`, `content_font_resolution`, nearest face for a missing style,
  no synthesis) in `gui_drawing_core`; raster module `src/abi/drawing_skia_c_api.cpp` `ensure_font`
  (table -> bundled face into the requested slot + executor alias; unmapped -> host bytes only when
  the host face has the requested name (GDI GetTextFaceW match / CoreText mandatory name; slot from
  the face's own OS/2 weight+fsSelection) -> else Carlito, source `content`); executor now 4 slots
  (bold=1, italic=2) + `alias_family`, CoreText locate moved out of the executor; reporting:
  `GUI_DRAWING_TRACE_FONTS=1` line per request + `gd_api_v0.font_resolve` (appended to ABI 0.2) and
  `gd_raster_service_v0.resolve_font`; raster module ships every table face + licences + table +
  ATTRIBUTION.md; THIRD_PARTY_NOTICES rows for Arimo, Tinos, Selawik. Ran: new table test, C11 test,
  drawing core/trace/boundary tests (normal + sanitizer), checkers, docs regenerated. Skia executor +
  raster tests compile only on macOS/Linux CI (no local Skia). CI pending.
- F2 scope notes: hosts unchanged (the retained core never requests a family by name; roles only),
  so Portsmouth Mono is reachable by family through GUI.Drawing, not a host role. FontCommand and
  the font dialog carry requested names only. "Segoe UI Black" is deliberately not in the table
  (no bundled heavy face): unmapped -> host (Windows) or content.
- Local build note: main needs `stx_cng/third_party/zstd-1.5.7.tar.gz`; copied from the shared
  `D:\gui_forms_workers\cache\` (SHA-256 eb33e51f..., verified by configure). Ignored by git.

## F3 download request (APPROVED 2026-10-23 and done; kept for provenance)

Arimo, googlefonts/arimo @ 4a6255f269916ae7ad3fc2706b0935e7621396b8 (fonts last built there,
2026-04-27), OFL 1.1, `https://raw.githubusercontent.com/googlefonts/arimo/<commit>/<path>`:
fonts/ttf/Arimo-Regular.ttf 478712 (blob d6b78c111a336f8c42e4cf5b6546c7b3386146cd),
Arimo-Bold.ttf 485872 (6453fed1dd86ed03fd2a5b4060d26e01ad4f57a4), Arimo-Italic.ttf 502332
(1e7b31c2b59ae634d4e1fd0acf52ff7c2cf7c796), Arimo-BoldItalic.ttf 508508
(e7c5924ae3e8fbd2585c4ef73f620caa22b2a3ba), OFL.txt 4383 (d2963d749510d1b7c0dd7952c1ab74b0a7d97aef).
Tinos, googlefonts/tinos @ 3b4482a99b80ea5fc75f187b1be3120a3f5905b3 (2026-04-27), OFL 1.1:
fonts/ttf/Tinos-Regular.ttf 521588 (893a1b525962984481aee5ce275516d384eb4865), Tinos-Bold.ttf
597880 (887fc97a95d30a08190217b6075a5e0a0f92bec7), Tinos-Italic.ttf 565016
(5aed4f3757537bf334ca89dd6893063a3ef9ecc9), Tinos-BoldItalic.ttf 578976
(a9abd086816f68d6eb3672f1d0babd1db1807d4a), OFL.txt 4383 (e91137c77e543b84502bc37822aadff61b2de4e8).
Selawik, microsoft/Selawik: the repository holds only sources; binaries exist only in release 1.01,
`https://github.com/microsoft/Selawik/releases/download/1.01/Selawik_Release.zip`, 530652 bytes,
uploaded 2015-12-08, no published digest; tag 1.01 = b65ff9ce7c4673e75ec5c622924a6b6f61f2f876;
LICENSE.txt 4509 (blob ac8fdb956049621af2aecd3a8253c38789748c4a), OFL 1.1.
SHA-256 is not published for any of these; the blob SHA-1s pin the bytes. On approval: download
each into its own empty directory, verify size and git blob SHA-1 (`git hash-object`), then record
SHA-256 in ATTRIBUTION.md. For the zip, record its SHA-256 and each extracted file's.

## Line-height question: RESOLVED 2026-10-23 by the owner

Our engine against our engine; GDI is information for R and S. See State above.

## Decisions in force (owner, 2026-10-13)

1. Licences: OFL, MIT or Apache only. Wine's LGPL replacements are out.
2. The substitution table comes first; host bytes only for unmapped families.
3. Consolas maps to Portsmouth Mono (MIT, 1.0, fixed pitch, 575 units). Hosts do not register
   Mono yet.

## Faces today

Bundled (`assets/fonts/`): Portsmouth Rapids (5 styles, control roles), Portsmouth (5), Portsmouth
Mono (1), Carlito (4, OFL, commit 3a810cab, lead body candidate, not the M9 decision), Cousine (4,
OFL, commit c0fbdb43), Noto fallback packs (CJK JP, Arabic, Hebrew, Devanagari, Bengali, Gurmukhi,
monochrome Emoji). Hosts register Portsmouth Rapids, Carlito and Cousine; GUI.Drawing's packaged
registration (`src/abi/drawing_skia_c_api.cpp` `register_packaged_fonts`) registers only Portsmouth
Rapids Regular and Bold.

Missing: Arimo (Arial, Helvetica, and the candidate for Microsoft Sans Serif), Tinos (Times New
Roman), Selawik (Segoe UI). Tahoma has no open metric-compatible face in the admitted licences;
it maps to the nearest measured face.

## Preliminary static measurement (2026-10-23)

Python reading font tables directly (`hmtx` via `cmap` 3/1), per character over U+0020..007E,
U+00A0..017F, unweighted. This is a screening, not the gate. Scripts:
`C:\Users\Shadow\AppData\Local\Temp\claude\C--Users-Shadow-gui-forms\005dc99f-1086-463f-978e-5723b295c49c\scratchpad\advances.py`
and `fontmetrics.py` (session scratchpad, not durable).

| Reference (version) | Candidate | Advances identical | Mean | Median abs |
|---|---|---|---|---|
| Calibri 6.27 | Carlito 1.104 | 318 of 318 (bold too) | 0 | 0 |
| Courier New 6.95 | Cousine 1.241 | 319 of 319 | 0 | 0 |
| Consolas 7.01 | Portsmouth Mono 1.0 | 0 (uniform) | +4.58% | 4.58% |
| Consolas 7.01 | Cousine | 0 (uniform) | +9.15% | 9.15% |
| Microsoft Sans Serif 7.04 | Arial 7.06 (Arimo's target) | 284 of 319 | +0.60% | 0 |
| Tahoma 7.05 | Arial (Arimo's target) | 0 | +0.92% | 9.94% |
| Tahoma 7.05 | Segoe UI (Selawik's target) | 0 | -3.68% | 5.13% |
| Tahoma 7.05 | Carlito | 0 | -11.0% | 8.73% |
| Tahoma 7.05 | Portsmouth Rapids | 0 | -2.14% | 10.5% |

Vertical metrics (units per em; hhea ascent/descent/gap; win ascent/descent):
Calibri hhea 1536/-512/452, win 1950/550; Carlito hhea 1950/-550/0, win 1950/550 (same total
2500, so Carlito's hhea equals what GDI reports for Calibri). Courier New = Cousine (1705/615).
Consolas hhea 1521/-527/350 (1.171 em); Portsmouth Mono 950/-300/0 per 1000 (1.25 em, +6.8%).
Segoe UI hhea 2210/-514/0. Tahoma 2049/-423/0. Microsoft Sans Serif 1888/-430/0.
Consequence: the vertical oracle must be what Windows reports (win metrics via GDI), not FreeType
reading the reference file's hhea, or Calibri and Carlito would wrongly disagree.

## Plan

### F0. Plan and decisions (this file).

### F1. Measurement harness (no new faces needed)

- `tools/font_metrics_differential/` (C++20, house style): a kernel over the bundled
  HarfBuzz/FreeType engine with the ADR-004 profile. Input: a face file by path, the corpus, the
  sizes. Output: JSON with per-glyph advances (font units), ascent, descent, line gap, line height
  at each size, shaped widths of every corpus line, and line breaks at fixed widths. Whole-buffer
  validation up front (file size, table directory, corpus UTF-8) before any measurement.
- A Windows-only GDI oracle in the same tool (Win32 types stay in the tool's Windows file):
  `GetTextMetricsW`, `GetCharABCWidthsW`, `GetTextExtentExPointW` at the same pixel sizes,
  grayscale antialiasing. It records what a WinForms author saw (line height and extents) with the
  real font. No .NET is installed here and none will be; System.Drawing evidence stays with the
  M11g tool under Wine.
- Reference fonts read from `C:\Windows\Fonts` by path; the report records file name, version
  string and SHA-256, never the bytes.
- Corpus (checked in, `tools/font_metrics_differential/corpus/`): every character of
  U+0020..007E, U+00A0..017F singly; WinForms-style UI strings (Gallery captions, menu items,
  dialog buttons, the M11g specimen strings); paragraphs in English, French, German and Polish
  wrapped at three widths. Sizes: 11, 12, 13, 16, 24 px at scale 1 (8.25, 9, 10, 12, 18 pt at 96
  dpi) and 22, 24 px for scale 2.
- Tests: a small `ctest` over bundled faces only (Carlito vs Carlito is exact; the kernel rejects
  a truncated buffer), so CI never needs Microsoft fonts.

### F2. Substitution table and its entry points

- A versioned data file (proposed `assets/fonts/font-substitution.json`, schema version 1):
  records of requested family (case-folded, with aliases such as "MS Sans Serif", "Helvetica",
  CSS generics), weight range and italic, mapped to a bundled family and face file, a class
  (`metric_compatible` or `nearest`) and the differential record that justifies it. Generated at
  build time into a fixed named record array (no parsing at run time); C# and Web.Forms can read
  the same file.
- Resolution order at every family entry point: registered family, then the table, then (unmapped
  only) host bytes under the decision-7 rule. Resolution reports the requested family, the
  resolved family and a `substituted` status (never silent); trace via `GUI_DRAWING_TRACE_FONTS`.
- Entry points for the 0.2 gate: GUI.Drawing `font_create` / `ensure_platform_font`
  (`src/abi/drawing_skia_c_api.cpp`), the C ABI (`include/gui_forms/drawing_c_api.h`),
  `FontCommand` and the font dialog. C# bindings, form documents and HTML import consume the data
  file when they land. `FontRole` is unchanged: roles never carry families.
- Register Portsmouth Mono on all three hosts and in GUI.Drawing's packaged set; package every
  table target face with GUI.Drawing (today it ships only Portsmouth Rapids).

### F3. New faces (only after owner approval of each download)

Pinned by upstream commit and SHA-256 like Carlito and Cousine; licence text copied as
`OFL-<Family>.txt`; `ATTRIBUTION.md` and `language-font-provenance.json` updated; hosts register
them. Sizes are stated exactly in the approval request (read from repository metadata, not by
downloading).

| Face | Source | Licence | Covers | Notes |
|---|---|---|---|---|
| Arimo (4 styles) | github.com/googlefonts/arimo | SIL OFL 1.1 (Chrome OS core fonts, relicensed from Apache 2.0; verify at the pinned commit) | Arial, Helvetica, Microsoft Sans Serif | Metric-compatible with Arial; same lineage as the bundled Cousine. Liberation Sans 2.x (OFL) is the same metrics and an acceptable alternative. |
| Tinos (4 styles) | github.com/googlefonts/tinos | SIL OFL 1.1 (as Arimo) | Times New Roman, CSS `serif` | Metric-compatible with Times New Roman. |
| Selawik (Regular, Semibold, Bold; Light and Semilight optional) | github.com/microsoft/Selawik | SIL OFL 1.1 | Segoe UI | Microsoft's open Segoe UI fallback; its metric compatibility is claimed and F1 verifies it. No italic styles as far as known; verify. |

Excluded: Wine faces (LGPL), DejaVu (Bitstream Vera licence, not in the admitted set).

### F4. Gate

- For every mapped family and style: the F1 differential at every corpus size within its class
  tolerance (decision 5), shaped by the bundled engine, with GDI oracle evidence alongside.
- Licences, versions and hashes in `ATTRIBUTION.md`; reference font versions and hashes in the
  record.
- Corpus, sizes, tolerance, tool command, environment and results under
  `results/<date>-f-font-substitution/`; the table consulted at the 0.2 entry points with tests.
- Then FORWARD_PROGRAMME §3 F, the stage board (`planning/stages/README.md`) and the F brief are
  updated, and the coordinator declares 0.2 Verified.

## Decisions put to the owner (F0, 2026-10-23): ALL ACCEPTED as recommended (1a..11a)

1. Microsoft Sans Serif: (a) Arimo; (b) Carlito; (c) unmapped. Recommend (a): Arial shares 284 of
   319 advances with it.
2. Tahoma: (a) whichever of Arimo and Selawik shows the smaller corpus line-width deviation in F1,
   both recorded; (b) Selawik; (c) Arimo; (d) unmapped. Recommend (a); no face is added for
   Tahoma.
3. Downloads to request: (a) Arimo, Tinos and Selawik; (b) Arimo and Selawik only (Times New Roman
   is in the brief's table but not in FORWARD_PROGRAMME's list). Recommend (a).
4. Missing styles (Selawik has no italic): (a) the nearest style in the mapped family, reported as
   style-substituted, no synthesis; (b) a synthetic oblique; (c) borrow Arimo Italic. Recommend (a).
5. Tolerance: (a) `metric_compatible` has identical advances in font units over the corpus
   coverage, identical line breaks, and line height within 1 px of the GDI oracle; `nearest` has a
   mean corpus line-width deviation within 5% at every size, with line breaks and line height
   recorded but not gated; (b) one per-mapping bound declared before measuring. Recommend (a).
   Portsmouth Mono measures +4.6% against Consolas, which fits.
6. REVISED 2026-10-23 by the owner: our engine against our engine for widths and line heights; GDI
   is information for R and S only (brief decision 9). Original text:
   6. Oracle for the gate: (a) the bundled engine on both faces for widths, and GDI for vertical
   metrics, with the GDI extents recorded as evidence; (b) GDI for everything. Recommend (a): it
   isolates the face from the rasterizer, and vertical metrics need GDI's view (see Calibri's hhea).
7. Unmapped families (refines decision 2). Today Windows registers whatever face GDI selects under
   the requested name, even when `GetTextFaceW` names another family, which silently uses an
   arbitrary host font; Linux has no host path. (a) accept host bytes only when the selected face
   name equals the request, otherwise resolve to the content face and report `substituted`; (b)
   keep today's behaviour; (c) drop the host path entirely. Recommend (a).
8. Generic names: (a) `sans-serif` to Carlito (the content role), `serif` to Tinos, `monospace`
   to Cousine (the monospace role), plus aliases ("MS Sans Serif", "Helvetica", Segoe UI weight
   names); (b) generics left to callers. Recommend (a).
9. Table form: (a) a versioned JSON data file generated into a fixed record array at build time;
   (b) a hand-written C++ record array. Recommend (a): C# and Web.Forms read the same file.
10. Body face M9: (a) F leaves M9 open, and Carlito is the Calibri substitute whatever M9 decides;
    (b) F also settles Carlito as the M9 body face. Recommend (a).
11. Portsmouth Mono's scope: (a) the Consolas target only, registered under its own family; (b) it
    also replaces Cousine as the monospace role face (a design-language §6.1 change). Recommend (a).

## F4 (asked 2026-10-23 by the coordinator: cross-host gate through CI, own PR on main after #304)

DECIDED 2026-10-23 (owner, F4-1 = option 1, brief decision 18 to be recorded in F4's PR so #301's
merge is not disturbed): commit metric snapshots (measurements only, never font bytes: advances in
font units; corpus line widths, breaks and heights per size; each reference file's SHA-256 and
version); a render-area ctest gate on every host measures the bundled candidates against them with
the class tolerances and named exceptions; the Windows job also re-measures any installed reference
whose SHA-256 matches its snapshot; provenance stated in the snapshot files and ATTRIBUTION.md.
Order: coordinator merges #301, #302, retargets and merges #304, then tells me; F4 goes on main.
Original question (F4-1): the Microsoft reference faces exist only on
Windows (GitHub macOS runners have Arial/Times New Roman/Courier New/Tahoma/Microsoft Sans Serif in
other versions; Linux has none), so "engine against engine on all three hosts" needs either
(1, recommended) committed reference measurement snapshots (our engine's measurements of the
hash-pinned Windows reference files: advances, line widths, breaks, line heights; no font bytes)
plus a ctest gate that measures the bundled candidates on every CI host against them, with the
named exceptions; Windows CI also re-measures installed references whose SHA-256 matches;
(2) the gate on the Windows runner only with its installed fonts, macOS/Linux checking candidate
measurement digests; (3) Windows + macOS references, Linux digests only.
Also blocked on order: F4 needs the F1 tool (#301) and the table (#304, on #302); branch it once
#301 and #302 are on main and #304 is retargeted (then F4 on #304 is two deep).

## F4 state (2026-10-23)

- DONE: CI run 40049449883 on 3dfb3137 green on all four hosts; follow-up commit records it (results
  README marked "F gate recorded 2026-10-23", F brief + stage board complete, roadmap 0.2 F bullet done).
  Awaiting the coordinator merge of #305, which completes 0.2. Remaining: FORWARD_PROGRAMME section 3 F if asked.

- PUSHED: PR #305 `claude/f-gate` @ 3dfb3137, on main (rebased after #304 merged as 188e27fa; gate
  re-run locally on the rebased tree: passes). Next: when CI is green, add the run id and per-host
  results to results/2026-10-23-f-font-substitution/README.md "Cross-host record" (follow-up commit).

- #304 Linux fix pushed: 081d5eb8 on `claude/f-substitution-table` (head was 1a8a0352, main merged).
  `cmake/generate_font_substitution.cmake` now sets `cmake_minimum_required(VERSION 3.25)`: a -P script
  has no policies, so CMake 3 on Linux read if(IN_LIST) as OLD CMP0057. F4 must be rebased onto
  main after #304 merges (the fix comes with it).

- Local branch `claude/f-gate` @ 79d216ba in D:/gui_forms_w4/gui_forms (NOT pushed: waits for the
  coordinator's word that #304 is on main). Built on 0f724bc1 = #304 head 66431cd2 merged with
  #301's 60411ce2. When #304 lands: `git fetch; git rebase --onto origin/main 0f724bc1 claude/f-gate`
  (only 79d216ba is F4's own), rerun the gate locally, push, PR on main.
- Contents: `tools/font_metrics_differential/metric_snapshot.cpp` (schema
  gui_forms.font_metrics_snapshot.v1, strict reader, CRLF tolerant; `measurement_digest`,
  `corpus_digest` = SHA-256 of parsed entries joined with LF, so checkout line endings never matter);
  tool `--snapshot` mode (`--request-family`, `--request-style`); 23 snapshots in
  `tools/font_metrics_differential/reference_snapshots/` (Calibri/Arial/Times New Roman/Courier New
  x4, Microsoft Sans Serif, Tahoma x2, Segoe UI x2, Segoe UI Semibold, Consolas; styles a candidate
  lacks are not gated, decision 7); table exceptions with limits (`criterion` advances|line_breaks|
  line_height|line_width, numeric `limit`) generated as `generated_gate_exceptions`, public
  `gui_drawing::FontGateException`, `FontGateCriterion`, `font_gate_exceptions()`,
  `font_gate_criterion_name()`; ctest `gui_forms_font_metrics_gate_tests` (area render) checks corpus
  digest, table resolution (source substitution, same face style, same file), candidate digest equal
  to the snapshot's, criteria with exceptions (stale exception fails, limit holds), coverage (every
  substitution has a regular snapshot, every exception is exercised), Windows re-measure by SHA-256.
- Exception limit units: advances = differing code points; line_breaks = differing sets over all
  sizes; line_height = px; line_width = largest |mean deviation| at any size. Limits: Calibri bi
  line_breaks 2, Arial b advances 1, Arial bi line_breaks 4, Tahoma b line_width 0.071 (measured
  0.0705).
- Record: `results/2026-10-23-f-font-substitution/README.md` + `gate-windows-local.txt`; brief
  decision 18 appended to the stage brief; ATTRIBUTION.md "Metric snapshots" provenance; tool README.
- Ran locally: gate + differential + substitution tests (normal and ASan/UBSan), house style,
  lifetime, docs check (regenerated library.drawing.rst). Gate locally: 23 snapshots, 4 exceptions
  held, 23 references re-measured.

## Next step

1. Answer CI failures on #301, #302 and #304 when woken (#304's Skia parts are CI-only).
2. When #304 is on main: rebase claude/f-gate (above), push, open F4's PR on main, report.
3. When F4's CI is green on all four native runners: add the run to the record's "Cross-host
   record", update FORWARD_PROGRAMME §3 F, the stage board and the brief (with F2's scope notes);
   then the coordinator declares 0.2.
