# Stage F: font substitution for requested families

Release: 0.2 Verified. Depends on: nothing. Feeds R, S and H.
Status 2026-10-12: not started. Authority: `planning/FORWARD_PROGRAMME.md`
§3 F.

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
| Consolas | Portsmouth Mono | the owner's face (decision 3); measure its differential |

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
   ("Portsmouth Mono exists"). Licence: MIT, like Portsmouth Rapids (owner,
   2026-10-13). Version 1.0, `PortsmouthMono.ttf`, SHA-256
   `6129c819de5046e4ec97c7ee8d915bf195d106b98680d9b17901741a1d7314ad`. It is
   not yet in `assets/fonts/`. The owner expected it in the original
   `falseywinchnet/file_manager` repository, but no branch or commit there has
   ever contained it (only Portsmouth Rapids, under the old `gui_forms/`
   subtree). The copy found in the owner's repositories is in
   `falseywinchnet/paymenottowork` at
   `public/instruments/portsmouth/assets/downloads/Portsmouth-Mono-1.0.zip`
   (the face plus its `PROJECT.md`; commit `3dbc684`). Ask the owner which
   repository is the canonical source before bundling, then add it with its
   licence text and provenance recorded as for Portsmouth Rapids.

## Read first

- `docs/user/text-engine.md`, `decisions/ADR-004-one-text-rasterization-engine.md`
- `planning/interview/GUI_FORMS_INTERVIEW_LEDGER.md` GF004
- `assets/fonts/ATTRIBUTION.md`
- `planning/DESIGN_LANGUAGE_SPECIFICATION.md` §6.1
