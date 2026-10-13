# Stage H: HTML import and WYSIWYG layout designer

Release: 0.7 Designers. Depends on: L (real layout), T3 (theme language);
uses F (fonts) for typography fidelity. Status 2026-10-12: the earlier
conversion is discarded; nothing new started. Authority:
`planning/FORWARD_PROGRAMME.md` §6 H.

Read [the shared brief](../README.md) first.

## Why

The owner, 2026-10-12: "revisiting the WYSIWYG HTML subspec → GUI.Forms
conversion. The previous experiments into this were dogshit. They didn't
result in identical behaviour — they resulted in essentially completely baby
level extremely deficient GUI.Forms layouts that didn't look at all like the
webpage they copied. So this is a case of: throw out what we had, build
something new." And: "also, a WYSIWYG designer. Real layout."

Web.Forms, the sibling project that held this work, is no longer maintained:
"there is nobody managing it anymore — it is free to capture."

## Why the earlier attempt failed

The failure was not where it looked. Two separate results:

1. The hand-built File Manager demoboard came out "a plainer style family",
   recorded as a negative result on 2026-08-10.
2. The Web.Forms compiler converted faithfully what it was given. Its
   fidelity oracle (`file_manager/web_forms/experiments/FIDELITY_ORACLE_001.md`)
   compared real Chromium with the native tree over 119 nodes. It found zero
   structure, state and material mismatches, with 69 nodes over the geometry
   tolerance, 23 over typography and 32 over raster. Every raster mismatch
   also had a geometry or typography mismatch.

The real loss happened before conversion. The rich concept atlas could not
pass the input subset: inline SVG, inline styles and script were rejected.
So it was rewritten as a plain shell, and the plain shell converted
faithfully. **Shrinking the input until it converts guarantees baby-level
output.** This importer accepts the rich source unchanged.

## What to keep

- The fidelity oracle: Chromium against the native retained tree, node by
  node, with structure, state, geometry, typography, material and raster kept
  separate and differences never laundered into parity. It is the stx-vorbis
  method again: measure against the reference.
- Stable-ID discipline and the no-authored-JavaScript boundary.

Throw out the rest of the conversion.

## What to deliver

1. An importer for real static HTML and CSS, including inline SVG and inline
   styles (script excluded), onto L's layout containers (flex, grid, block
   and inline flow) and T3's theme language. Static structure becomes a form
   document (FD); appearance becomes theme rules.
2. A WYSIWYG layout designer over the same documents, sharing D's editing
   core.
3. Unsupported constructs reported precisely, never silently dropped or
   simplified.

## How

- Chromium (headless) is a test oracle only (owner decision): the importer
  never calls it. It is never a runtime dependency; no browser engine
  enters GUI.Forms (`AGENTS.md`). A layout captured from Chromium at one
  size freezes it, and resizing then breaks. That is why L implements real
  layout instead.
- Inline SVG lowers to the renderer-neutral drawing vocabulary (paths,
  gradients, transforms, clips); images stay PNG only.
- Fonts requested by the page go through F's substitution table; text is
  shaped by the bundled engine.
- Keep the importer's output honest: an imported page that differs from
  Chromium shows the difference in the oracle report.

## Gate

The accepted File Manager concept atlas
(`file_manager_demoboard/reference/frontend-concept-atlas-reference.html`)
imports without re-authoring and matches Chromium per node (geometry,
typography, raster) within tolerance. Record the oracle report under
`results/`.

## Owner decisions, 2026-10-13

1. **From Web.Forms, take the fidelity oracle** into `tools/`; leave the
   compiler and HTML profile behind.
2. **Chromium in tests only.** The importer computes layout with L's
   containers; Chromium only checks it.

## Read first

- `file_manager/web_forms/experiments/FIDELITY_ORACLE_001.md`,
  `STAGE3_NATIVE_PROJECTION_001.md`, `DOGFOOD_001.md`
- `file_manager_demoboard/reference/README.md`,
  `file_manager_demoboard/VISUAL_CONSTRUCTION_SPECIFICATION.md`
- [L](../L-layout/README.md), [FD](../FD-form-documents/README.md),
  [D](../D-interface-builder/README.md) briefs
