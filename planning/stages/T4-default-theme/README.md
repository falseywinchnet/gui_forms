# Stage T4: the default theme and Windows-familiar themes

Release: 0.3 Themeable. Depends on: T1, T2, T3. Unblocks R, V and the demo
rework. Status 2026-10-12: design decided (design language §11, §12.3–§12.5);
build not started. Authority: `planning/FORWARD_PROGRAMME.md` §4 T4 and
`planning/DESIGN_LANGUAGE_SPECIFICATION.md`.

Read [the shared brief](../README.md) first, then
[`RENDERING_REFERENCE.md`](RENDERING_REFERENCE.md) in this folder.

## Why

The owner, 2026-10-12: "we need to figure out a better way to make this look
as a default." The industry "moved in a bad direction towards super-flat
looks that are essentially minimal but useless to the user. No or little
depth." Flat is acceptable for "controls on top of a canvas, where the canvas
is not meant to occupy attention"; for other controls it is not. "Really our
reference just needs implementation on top of the engine as opposed to
hand-written, and to be a bit tweaked to look nicer." The default is
GUI.Forms' own, "inspired from the interviews", and needs "a unified design
language which is explicit and well specified … a formal specification in
artistic, thematic, and effect terms". That specification now exists.

The owner also wants "to be able to rapidly default to a style familiar to
Windows users that looks correct", so Windows-familiar themes ship beside the
default.

Where today's look came from: the only theme, `windows-professional`
(`windows_professional_theme_definition()` in `src/core/theme/theme/theme.cpp`),
arrived on 2026-08-06 in commit `b7388c1` as "a Windows 7/10-inspired
default". It is not in the owner's interviews; an agent's interpretation
became the only theme. The owner's reaction on learning this: "the code is
wrong. windows-professional? Where is this specified?" Plan Paint builds its
own look as overrides of `windows-professional` (Plan Paint `ribbon.cpp`).

## What the default looks like (decided)

Summarized from design language §3, §4, §5, §11 and §12.3–§12.5; the
rendering reference has the numbers.

- **Reference.** The published Pearl Ribbon study, moved a little toward
  Plan Paint's royale blue. Plan Paint is the reference realization: "study
  Plan Paint more".
- **Depth.** Recess only: entry fields and the work pane are deepest. Side
  panels are flat, never raised ("why are the left and right panels
  raised?"). Nothing is lifted by a Windows bevel.
- **Push buttons.** Plan Paint's action plate, the slight pill with inner
  light-over-dark lines and a one-pixel contact shadow.
- **Ribbon buttons.** A bulge in the ribbon cloth with no edge. Its
  dithered form, a multi-tone lattice, is what the studies show. Flat, smooth
  and dithered are engine renderings of one part role; which ships is the
  owner's call.
- **Tabs.** Plan Paint's tab stack and shadowing exactly.
- **Command strips.** Members may be flat at rest because dividers and strip
  shading carry the structure ("we account for this with dividers in Plan
  Paint, as well as shading. A quiet relief can be shading for faint
  buttons"). Hover and press bring full relief.
- **Stock.** Neutral near-white `#F4F4F5` on blue chrome. "Warm on cool is
  jarring."
- **Identity.** The File tab, tab rail and command plane carry identity; no
  fresco band.
- **Icons.** High-colour material icons in every theme.
- **Not a Windows skin.** Studies that fused Windows 95/XP bevels, sunken
  status cells and classic dialog buttons were rejected as "too windowsy".
- **Focus.** Text fields show focus whenever focused; other controls only
  after keyboard use. Host high contrast or focus-visibility forces it on.
  The ring is a dotted, non-colour cue that never collapses once shown.
- **Animation.** In the default theme only progress bars animate ambiently.
  Ceremonial surfaces (installation, first run, long tasks) may carry slow
  ambient life and an activity object while work runs ("those verdicts
  literally concern progress bars and animations that simulate activity
  going on, of course animations are allowed"), stopping at completion.
- **Dark variant.** Derived, whole windows, following the host preference,
  from the Phosphor pair (black, green `#82B361`, complement `#B36182`). The
  owner: "the dark theme is good. Doesn't miss."
- **High contrast.** Two authorities: under the host's mode the host palette
  wins; GUI.Forms' own mode is a near-colourless rebuild.
- **Type.** Portsmouth Rapids for chrome; Carlito for content; Cousine for
  monospace. The united text engine's profile applies; there is no LCD text.
- **Contrast gate.** 4.5:1 for text, 3:1 for large text, boundaries and focus
  against both neighbours, 7:1 in local high contrast. Measured where text
  actually sits.
- **Tokens.** The proposed numbers are starting tokens, revisited after the
  engine renders a native specimen sheet.

## Windows-familiar themes

Classic (the Windows 2000 look: pure procedural bevels, the cheapest proof
that the engine works) and a modern flat look first; "we could add a few"
later. Literal Windows rendering, raised buttons and sunken cells live only
here. The modern flat theme still keeps boundaries, hover, focus and input
unequivocal (M076) and still uses high-colour icons. `windows-professional`
becomes one of these themes rather than the default. XP- and Aero-style looks
are artwork-heavy. If they come later, they are recreated, never Microsoft's
bitmaps.

SDRSharp's visual demands are acceptance fixtures here (see R): owner-drawn
lists and combos, `ToolStripProfessionalRenderer` gradients, GDI+ linear,
path-gradient and hatch brushes, paths and regions, docking, splitters, and
custom range and frequency controls.

## How

- Build the default as theme data (T3) on the engine (T2). Move every stock
  control's hand-written paint (for example
  `src/controls/range_control/range_control_rendering.cpp`) onto the engine.
  No control-specific paint code may remain for stock controls.
- Variants are derived from the role model, not hand-forked. Today's
  hand-authored high-contrast arrays per role go away.
- Sample Plan Paint, never approximate. Clone `falseywinchnet/plan-paint`,
  check out `6b901fc`, read `src/forms/ribbon.cpp`, `editor.cpp`,
  `surface.cpp`, `interface_theme.cpp`, `src/color_palette.cpp`,
  `src/canvas_backing.cpp`, and sample `assets/screenshots/macos-home-1.1.jpg`.
- Read the owner's visual verdicts before any visual judgement: the
  ceremonial atlas responses especially, then material, visual, colour
  relation model, palette atlas, style families and DDV-007 (paths in the
  shared brief). Cite IDs.
- Produce the native specimen sheet early: every stock control in every
  state, light, dark and high contrast, captured by the hosts' frame capture.
  The owner revisits tokens against it and approves the reference set.
- Do not restyle the demonstration programs. They change only because the
  toolkit's default changes.
- If the owner's description of a look is unclear, ask for an analogy before
  iterating (see the shared brief's lessons).

## Gate

The Gallery renders every stock control with no control-specific paint code;
the contrast checks of the specification pass automatically for every
shipped theme; the owner approves the reference image set. Record the
specimen sheet, contrast report and approval under `results/`.

## Open decisions (ask, do not decide)

1. **Ribbon-button rendering in the default**: flat, smooth bulge or
   dithered bulge (§12.5).
2. **An interim default.** Asked on 2026-10-12 and not yet answered: create a
   `gui-forms` default now with today's recipe system, following the
   specification as far as recipes allow, and demote `windows-professional`
   to a named alternative; or wait for T4 on the engine.
3. **Which further Windows-familiar themes** follow Classic and modern flat.

## Read first

- [`RENDERING_REFERENCE.md`](RENDERING_REFERENCE.md) and
  [`theme_studies_source.html`](theme_studies_source.html)
- `planning/DESIGN_LANGUAGE_SPECIFICATION.md` in full; Appendix A lists the
  anti-models
- `file_manager/frontend/planning/visual/CEREMONIAL_VERDICTS_001.md`,
  `MATERIAL_VERDICTS_001.md`, `VERDICTS_001.md`, `COLOR_RELATION_MODEL.md`,
  `VAPORWAVE_PALETTE_ATLAS_003.md`, `STYLE_FAMILY_ATLAS_004.md`,
  `DESIGN_DNA_VERDICTS_007.md`
- `include/gui_forms/theme/types/theme_types.hpp`,
  `src/core/theme/theme/theme.cpp`
