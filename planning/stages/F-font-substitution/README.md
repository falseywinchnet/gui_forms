# Stage F: font substitution for requested families

Release: 0.2 Verified. Depends on: nothing. Feeds R, S and H.
Status 2026-10-23: complete. The gate is recorded in
`results/2026-10-23-f-font-substitution` (CI run 40049449883 on 3dfb3137,
passed on linux-x64, linux-arm64, macos-arm64 and windows-x64). F1 (#301),
F3 (#302), F2 (#304) and F4 (#305) are done. Authority:
`planning/FORWARD_PROGRAMME.md` §3 F.

Read [the shared brief](../README.md) first.

## Why

Looking identical needs identical text metrics. Windows Forms programs ask
for Segoe UI, Tahoma, Microsoft Sans Serif, Arial, Calibri and Consolas; web
pages bring Helvetica, Arial and their own families. GUI.Forms' rule is that
product text uses bundled fonts only (interview GF004; `AGENTS.md`), so it
cannot just load what the host has. Without substitution, SDRSharp
reflection (R), the C# swap-in (S) and HTML import (H) all wrap text at
different places from their references, and every visual differential fails
for a reason that has nothing to do with the stage under test. This stage
was identified on 2026-10-12 as a hidden dependency of ways 2, 3 and 5.

## What exists today

- The united HarfBuzz/FreeType text engine with one profile everywhere
  (`docs/user/text-engine.md`; ADR-004): light auto-hinting, grayscale
  coverage, one blending rule. No LCD text (design language §12.3).
- Bundled faces (`assets/fonts/`, provenance with hashes in
  `assets/fonts/ATTRIBUTION.md` and `language-font-provenance.json`):
  Portsmouth Rapids (control role), Carlito (content role; metric-compatible
  with Calibri), Cousine (monospace; metric-compatible with Courier New),
  Noto fallback packs, monochrome Noto Emoji.
- Controls ask for a `FontRole`, never a family; fallback resolves per
  grapheme cluster.
- One existing exception: the separate GUI.Drawing C API, when a caller names
  a family it never registered, asks CoreText (macOS) or GDI (Windows) for
  that family's bytes and shapes them with the shared engine. The SDRSharp
  facade's `System.Drawing` text goes through this path today.
- `HostCapability::font_discovery` exists in the host protocol.

## What to deliver

1. A substitution table from requested family (and style) to a bundled face,
   versioned and data-driven, consulted wherever a family name enters
   GUI.Forms: GUI.Drawing, the future C# bindings, form documents and HTML
   import.
2. Bundled faces for the families that need them, each with recorded
   provenance, hash and licence.
3. A metrics differential per mapped family against the reference face.

Candidates to evaluate, not decisions:

| Requested | Candidate | Notes |
|---|---|---|
| Calibri | Carlito | ships; metric-compatible |
| Courier New | Cousine | ships; metric-compatible |
| Arial, Helvetica | Arimo (Liberation Sans lineage) | metric-compatible with Arial |
| Times New Roman | Tinos | metric-compatible |
| Segoe UI | Selawik | Microsoft's open Segoe UI fallback; check how close its metrics are |
| Tahoma, Microsoft Sans Serif | nearest OFL/MIT/Apache face | Wine's LGPL replacements are excluded (decision 1) |
| Consolas | Portsmouth Mono | the owner's face, bundled (decision 3); measure its differential |

## How

- Measure with the bundled engine itself, not a browser or the host's text
  stack: advance widths per glyph, ascent, descent, line gap, and line breaks
  on a fixed corpus at several sizes. The reference face is measured on the
  reference platform (real Windows for the Windows families, on Shadow
  Windows).
- The reference fonts are test oracles only. Never commit Microsoft font
  files; record their version and hash in the result instead.
- Fallback still resolves per cluster inside the substituted face; a missing
  cluster never restyles the whole control (interview GF004).
- House style: the table is a fixed, named record set (§1, §7: precompute
  stable tables); the metric comparison is a kernel with whole-buffer
  validation up front (§3).

## Gate

A metrics differential for each mapped family (advance widths, line metrics,
line breaks on a fixed corpus) against the reference face, within a recorded
tolerance; licences recorded in `assets/fonts/ATTRIBUTION.md`. Record the
corpus, sizes, tolerance and results under `results/`.

## Owner decisions, 2026-10-13

1. **Licences.** Bundle only OFL, MIT or Apache-licensed faces. Wine's
   LGPL Tahoma and Microsoft Sans Serif replacements are out; those families
   map to the nearest open face and the differential is recorded.
2. **GUI.Drawing host fonts.** The substitution table comes first; only an
   unmapped family falls back to the host's bytes (CoreText, GDI), so drawing
   code keeps working.
3. **Consolas** maps to **Portsmouth Mono**, the owner's own monospace face
   (MIT, version 1.0, fixed pitch). The owner asked for every Portsmouth face
   to be included; all of them are now in `assets/fonts/` with hashes and
   provenance in `assets/fonts/ATTRIBUTION.md` (from the owner's published
   1.0 downloads in `falseywinchnet/paymenottowork`). The hosts do not
   register Mono yet; wiring it into the substitution table is this stage's
   work, with its metric differential against Consolas recorded like any
   other mapping.

## Read first

- `docs/user/text-engine.md`, `decisions/ADR-004-one-text-rasterization-engine.md`
- `planning/interview/GUI_FORMS_INTERVIEW_LEDGER.md` GF004
- `assets/fonts/ATTRIBUTION.md`
- `planning/DESIGN_LANGUAGE_SPECIFICATION.md` §6.1

## Plan

F0, the plan, is accepted. F1 builds the measurement tool
(`tools/font_metrics_differential/`) and needs no new faces. F3 commits the
approved faces as files. F2 adds the substitution table, registers and
packages F3's faces, and fixes the silent host-font substitution
(decision 10). F4 reruns the measurement from the bundled copies and records
the gate with its named exceptions (decisions 16 and 17).

## Owner decisions, 2026-10-23

The owner accepted every recommendation of the F0 plan.

4. **Microsoft Sans Serif** maps to **Arimo**. Arial, Arimo's metric target,
   shares 284 of 319 Latin advances with it.
5. **Tahoma** maps to whichever of Arimo and Selawik shows the smaller corpus
   line-width deviation in F1's measurement. Both are recorded, and no face is
   added for Tahoma.
6. **Downloads.** Arimo, Tinos and Selawik are requested. Each download still
   needs the owner's approval with its exact file, pinned source, size and
   licence.
7. **Missing styles** (Selawik has no italic) resolve to the nearest style of
   the mapped family, reported as style-substituted. There is no synthetic
   bold or oblique.
8. **Tolerance.** A `metric_compatible` mapping has identical advances in font
   units over the corpus coverage, identical line breaks, and a line height
   within 1 px of the reference's. A `nearest` mapping has a mean corpus
   line-width deviation within 5% at every size; its line breaks and line
   height are recorded but not gated. Every measurement is our engine's
   (decision 9).
9. **Our engine against our engine for widths and line heights** (revised
   2026-10-23). The reference font's file goes through the same pinned
   FreeType and HarfBuzz code as the substitute's. GUI.Forms never renders
   through GDI or any other host renderer, so no host is the reference. GDI's
   measurements are kept in the results as information for stages R and S,
   outside F's gate.
10. **Unmapped families** (refines decision 2). Host bytes are accepted only
    when the face the host selected has the requested name. Otherwise the
    family resolves to the content face and the result reports `substituted`.
    Today the Windows path registers whatever face GDI selects under the
    requested name, which silently uses an arbitrary host font; F2 fixes it.
11. **Generic names.** The table maps `sans-serif` to Carlito (the content
    role), `serif` to Tinos and `monospace` to Cousine (the monospace role),
    plus aliases such as "MS Sans Serif", "Helvetica" and the Segoe UI weight
    names.
12. **Table form.** A versioned JSON data file is generated into a fixed
    record array at build time, so C# and Web.Forms read the same file.
13. **Body face (M9)** stays open. Carlito is Calibri's substitute whatever M9
    decides.
14. **Portsmouth Mono** is the Consolas target only, registered under its own
    family. Cousine remains the monospace role face (design language §6.1).

## Owner decisions, 2026-10-23 (from F1's measurements)

15. **Segoe UI maps to Selawik, classed `nearest`.** Selawik shares only a few
    advances with Segoe UI (3 of 291 for Semibold), so it cannot be
    `metric_compatible`; its corpus lines are 1 to 3 px shorter.
16. **All of Tahoma maps to Selawik** (refines decision 5). Regular is within
    the `nearest` tolerance (-1.0%). Tahoma Bold is -7.0% against Selawik Bold
    (Arimo Bold is -6.0%); it is a named exception in F4's gate rather than a
    reason to split the family.
17. **Named exceptions for the small misses.** Arimo Bold's µ (1229 against
    Arial Bold's 1180 units), Arimo Bold Italic's line breaks (4 of 90
    paragraph sets) and Carlito Bold Italic's line breaks (2 of 90, from
    kerning) are named exceptions in F4's gate. Those styles stay
    `metric_compatible`.

## Owner decision, 2026-10-23 (F4's cross-host gate)

18. **Commit metric snapshots** (owner decision F4-1, option 1). The
    snapshots hold measurements only, never font bytes: advances in font
    units, and corpus line widths, breaks and heights per size. Each also
    names its reference file's SHA-256 and version. On every host, a
    render-area ctest (`gui_forms_font_metrics_gate_tests`) measures the
    bundled candidates against the snapshots, with the class tolerances and
    the named exceptions. The Windows job also re-measures each installed
    reference whose SHA-256 matches its snapshot. The snapshot files and
    `assets/fonts/ATTRIBUTION.md` state the provenance: how the snapshots were
    measured, from which file hashes, and that no font data is included. The
    record is `results/2026-10-23-f-font-substitution`.
