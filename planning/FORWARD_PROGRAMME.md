# GUI.Forms forward programme

Date: 2026-10-12. Status: agreed direction; stage scopes and gates are a draft
for owner review. Open owner decisions are listed in section 9 and are not
assumed anywhere below.

This plan sequences the next body of work after the documentation and
house-style stage. It is written for sibling threads: read section 1 and the
stage you are assigned, then its gate. A stage is done only when its gate is
measured and recorded under `results/` or `experiments/`. The readable status
board for siblings is linked from `AGENTS.md`; this file is authoritative where
they differ.

## 1. Direction in one page

GUI.Forms gains one specification structure, articulated controls, one theme
engine, and the tools that edit them.

- **One specification.** A single registry inside GUI.Forms describes every
  control's parts, states, properties and interactions. The C++ dot API
  (properties set, interactions registered), form documents, theme documents
  and the visual tools are projections of that one structure.
- **Articulated controls.** Every control is composed of named, parameterized
  parts with states and public geometry ("addressable anatomy"), addressed by
  name, not walked as a document tree.
- **One engine.** A theme engine runs theme data against the anatomy. Theme
  data is more than colours and less than code: properties, layered effects,
  presentational state machines and bounded expressions, written in a
  CSS-syntax subset (`::part()`, `:state()`, stable IDs). Engine inputs such as
  pointer position and published application values are custom properties
  only the engine sets. The state machine's specification lives in the theme;
  the machine that runs it lives in the engine. New kinds of effect are added
  as C++ engine primitives under the house style, never as theme code.
  Unspecified or invalid theme data falls back to the built-in recipe.
- **Capability, not a fixed look.** The target is massive customization:
  funky media-player-style skins, a sheen that follows the cursor, buttons
  that pop, transparency, reflections and glass, ambient animation within a
  budget. Idle still schedules zero frames.
- **Our own default.** The default theme is GUI.Forms' own, defined by
  `DESIGN_LANGUAGE_SPECIFICATION.md` and built on the engine, never as
  hand-written paint code. It rejects the super-flat direction: controls that
  carry meaning have depth. Windows-familiar themes ship alongside it.
- **Cooperate with the compositor.** Window transparency and behind-window
  materials belong to the host window manager; GUI.Forms reports and uses the
  capability without friction and never themes the compositor.
- **Accessible by construction.** Controls are addressable, selectable and
  self-describing through the semantic tree and each platform's native
  accessibility API. GUI.Forms exposes the capability and verifies it through
  those APIs; integration testing with specific screen readers is out of scope.
- **Measure against the reference.** Every fidelity claim is a differential
  against the real thing, as stx_vorbis is checked against libvorbis: the
  design-language reference images for themes, real Windows Forms on real
  Windows for the reflection and C# work, Chromium for HTML import. Reference
  software is a test oracle only, never a runtime dependency.

Hand-editing a form or theme document is always legal. The visual tools read
and write the same documents and preserve what they did not change; generated
code is disposable (interview GF014). Nothing is clobbered.

## 2. Stage map

```text
A  Accessibility verified on every host ──────────────────────────────┐
F  Font substitution for requested families ───────────┐              │
T1 Anatomy: articulated controls + one schema ─┐       │              │
T2 Engine core ────────────────────────────────┤       │              │
T3 Theme language front end ───────────────────┤       │              │
T4 Default theme + Windows-familiar themes ────┤       │              │
T5 Compositor cooperation ─────────────────────┤ (parallel after T2)  │
L  Real dynamic layout (flex, grid, flow) ─────┤ (parallel after T1)  │
FD Form documents ─────────────────────────────┤ needs T1, T3         │
DP Damage-preserving painting (no dependencies)│                      │
R  SDRSharp reflection, current technique ─────┴── needs T4, F        │
W  Windows Forms controls, .NET 8+ only ── needs T4                   │
C  Stable installed C ABI ─────────────── needs T1, T2 ───────────────┤
S  C# sibling: Windows Forms swap-in ──── needs R, C                  │
D  Interface builder (dogfooded) ──────── needs T3, L, FD             │
H  HTML import + WYSIWYG layout designer ─ needs L, T3                │
V  WYSIWYG theme designer ─────────────── needs T4, D core ───────────┘
```

A, F, T1 and DP can start now; A and F run beside everything else. T1 to T4
are the spine. FD and DP were added on 2026-10-12 to match the public
roadmap's 0.4 release; the owner accepted their gates on 2026-10-13.

Each stage has a working brief in `planning/stages/<stage>/README.md` with
the owner's reasons, the current code, the agreed approach, house-style
points and open decisions. Read `planning/stages/README.md` first.

## 3. Foundations

### A. Accessibility verified on every host

Today: the semantic tree exists; macOS (NSAccessibility), Windows (MSAA) and
Linux (ATK/AT-SPI) publish it; only Windows has a registered test; no host
advertises `HostCapability::accessibility`; there is no UI Automation provider
and no text-range exposure.

Work: register tests that query each host's own accessibility API (AX on
macOS, MSAA and UI Automation on Windows, AT-SPI on Linux) for every stock
control in the Gallery: role, name, state, value, actions and keyboard reach;
add a UI Automation provider; expose editable text ranges; then advertise the
capability. Themes and the engine never alter semantics (interview GF018).

Gate: on each host, an automated test enumerates the Gallery through the
native API and finds every stock control with the role, name, state and
actions the semantic tree reports, and performs each control's default action
through that API.

### F. Font substitution

Applications, Windows Forms programs and web pages request families GUI.Forms
does not bundle (Segoe UI, Tahoma, Microsoft Sans Serif, Arial, Helvetica,
Calibri, Consolas). Controls never use host fonts, so requested families map
to bundled, metric-compatible faces (Carlito for Calibri already ships).

Gate: a metrics differential for each mapped family (advance widths, line
metrics, line breaks on a fixed corpus) against the reference face, within a
recorded tolerance; licences recorded in `assets/fonts/ATTRIBUTION.md`.

## 4. The theming spine

### T1. Anatomy: articulated controls and one schema

Every stock control publishes its named parts and states with public geometry
(a track bar's track and thumb rectangles; a scroll bar's arrows, track and
thumb; every control's focus part). Parts map to the shared part roles of the
design-language specification, so a cue set on one role reaches every control.
The property registry behind `PropertyGrid` becomes the one schema for parts,
states, properties and interactions.

Gate: every control in `manifests/controls.json` lists its parts and states in
the schema; a test reads each part's geometry from a laid-out Gallery; the
PlaySuite workarounds that copied private geometry (the track bar inset) are
no longer needed.

### T2. Engine core

The cascade, in order: built-in recipe, theme sheet, class rule, stable-ID
rule, inline style, forced high contrast. Layered materials and effect
primitives; inputs as engine-set custom properties; presentational state
machines with timed, eased transitions; settling primitives (follow, spring,
decay) for momentum and pop; bounded expressions; per-part cost budgets checked
at load; optional per-part framebuffers with passthrough transparency; damage
limited to the parts whose inputs changed. An extension seam lets a new
primitive register its name, parameters, cost bound and dependencies.

Gate: a reference-image test per primitive on all three hosts with identical
pixels; the window counters report zero frames once inputs stop and every
settling primitive has settled; a part whose cost cannot be bounded falls back
to the built-in recipe; reduced motion disables motion primitives.

### T3. Theme language front end

A parser for the CSS-syntax subset: selectors by control type, stable ID,
class, `::part()` and `:state()`; no combinators; inheritance only for text and
colour roles; no layout properties. Diagnostics name the file, line and rule;
an invalid rule is dropped, not fatal. Theme documents are data only.

Gate: conformance fixtures for every selector and property form; malformed
and hostile input tests; a byte-preserving round trip of an untouched document.

### T4. Default theme and Windows-familiar themes

Build the default theme from `DESIGN_LANGUAGE_SPECIFICATION.md` as theme data;
move every stock control's hand-written paint onto the engine; ship
Windows-familiar themes (at least Classic and a modern flat look; others per
owner decision). The current only and default theme, `windows-professional`,
becomes one of the Windows-familiar themes. High-contrast and dark variants
are derived, per the specification.

Gate: the Gallery renders every stock control with no control-specific paint
code; the contrast checks of the specification pass automatically for every
shipped theme; the owner approves the reference image set.

### T5. Compositor cooperation

A host capability for non-opaque windows, per-pixel alpha and behind-window
materials (macOS vibrancy, Windows layered windows and DWM materials, X11 ARGB
visuals under a compositor). Each host reports what it supports; requests the
host cannot honour fall back as declared. In-window glass and reflections are
engine effects that sample only the style backplane.

Gate: capability reported correctly per host and per compositor state; a test
window renders with and without the capability and falls back without error.

### L. Real dynamic layout

Flexible layout should be easy. Add flex and grid semantics (and block/inline
flow for imported documents) as GUI.Forms layout containers, honouring the
read barriers and bounded passes of the existing layout system.

Gate: a corpus of layout fixtures matches Chromium's boxes within a recorded
tolerance at several window sizes, using the fidelity oracle approach of the
earlier Web.Forms experiments (`file_manager/web_forms/experiments/FIDELITY_ORACLE_001.md`).

### FD. Form documents

A compact form-document format that serializes the T1 schema: objects with
stable IDs, z-order, layers, properties, layout containers, presentational
state machines and references to named handlers. Hand edits are legal and
round-trip; generated C++ and C# are disposable (interview GF014). The syntax
is the CSS family shared with T3 (one parser, one round-trip machinery), and
the provisional `demo/gallery.dml` migrates to it (owner, 2026-10-13).

Gate: untouched documents round-trip byte for byte; a scripted
one-property edit changes only that property's bytes; every Gallery page can
be expressed as a document whose generated C++ compiles against the installed
SDK and passes its tests.

### DP. Damage-preserving painting

Paint and present each damage rectangle of the bounded set instead of their
bounding box, report partial present as an optional host extension, and
publish the presentation availability record
(`PAINT_PIPELINE_AVAILABILITY.md`).

Gate: with two distant damaged rectangles, the painted area equals
their sum on every host and pixels outside them are untouched; the
paint-pipeline acceptance tests still pass.

## 5. Reflection and the C# sibling

### W. Windows Forms control coverage

Added 2026-10-13 at the owner's request. Scope: only types and members
supported by Windows Forms on .NET 8.0 or later. Nothing that exists only in
.NET Framework (2.0 through 4.x), nothing removed in .NET Core 3.1 (`DataGrid`,
`ToolBar`, `StatusBar`, `MainMenu`, `ContextMenu`, `MenuItem`), nothing marked
obsolete; the existing GUI.Forms exclusions still apply. Rebuild the API
catalogue from the official .NET 8+ reference assemblies, then implement the
missing controls (`DataGridView`, `ListView`, the `ToolStrip` and
`StatusStrip` family first; then `MaskedTextBox`, `DomainUpDown`,
`MonthCalendar`, `NotifyIcon` and member depth), each articulated and
engine-painted from birth.

Timing (owner, 2026-10-14, revising "W starts after T4"): a new control may
be built before T4 when it uses only atoms of the vocabulary (design language
§12.7) and paints every part through the shared by-role paint path, which T4
replaces with the engine. The catalogue was regenerated from the official
surface on 2026-10-14 (`WINFORMS_API_CATALOGUE.md`).

Gate (accepted 2026-10-13): the catalogue is regenerated from the official .NET 8+
surface; every in-scope row of `CONTROL_COMPLETENESS_MATRIX.md` is implemented
and tested on every platform, or excluded or deferred with a reason; the
manifest records every in-scope type and member and the docs are generated
from it.

### R. SDRSharp reflection, current technique

The harness in `sdrsharp_reflection_system/` and the tools in `tools/` were
last run on 2026-08-10. Rebuild the generated facade against the current C
ABI; route its fonts through F; fix the paths left from the old combined
repository; make the Wine run repeatable on the Mac mini; add the missing
reference half: capture the real program on real Windows on the shadow desktop
and compare control by control. SDRSharp's visual demands (owner-drawn lists
and combos, gradient tool strips, GDI+ brushes and paths, docking, custom
range and frequency controls) become acceptance fixtures for T4. This work
stays off the documentation site.

Gate: a per-control visual differential between the reflected run and the
real Windows run, within a recorded tolerance, for the listed controls.

### C. Stable installed C ABI

The ABI is GUI.Forms' own. It is versioned, installed with the SDK, has an
error model, exposes the schema of T1 and the engine of T2, and is a superset
of what the C# Forms surface needs.

Gate: the breaking-change audit and the two-consumer conformance condition in
`docs/dev/roadmap.md` "Before 1.0".

### S. C# sibling: Windows Forms swap-in

Generated C# bindings over C, presented as a `System.Windows.Forms`-shaped
package: a project swaps its Windows Forms reference for the package and runs
on macOS, Linux and Windows. It looks the same as Windows Forms; behaviour
follows GUI.Forms' principled choices (its own event loop and ordering), not
Windows Forms' accidents. Coverage is driven by a corpus of real applications
measured with the capture tool, not the full member catalogue, much of which
is obsolete .NET 2.0-era duplication. Developed on the shadow desktop with
Visual Studio; a sibling thread dogfoods it only after R, C and the binding
layer exist. GUI.Forms itself still never depends on .NET.

Gate: each corpus application builds by swapping the package reference and
runs on all three platforms; a visual differential against the same
application on real Windows Forms is within tolerance.

## 6. Tools

### D. Interface builder

A standalone, dogfooded builder written in GUI.Forms that edits form
documents (the schema's serialization: objects, z-order, layers, properties,
state machines, stable IDs), generates disposable C++ and C# code, and
integrates with Visual Studio through a thin extension. Prior art to study:
Delphi and Lazarus form files and inspector, Xcode Interface Builder, Qt
Designer, GtkBuilder tools, and component-and-variant design tools.

Gate: hand edits and builder edits round-trip: a document edited by hand, then
in the builder, keeps every untouched byte; the builder can author a Gallery
page that compiles and passes its tests.

### H. HTML import and WYSIWYG layout designer

Throw out the earlier conversion. Import real static HTML and CSS, including
inline SVG and inline styles (script excluded), onto L's layout containers and
T3's theme language, and offer it as a WYSIWYG designer. The earlier failure
came from shrinking the input until it converted; this importer must accept
the rich source unchanged.

Gate: the accepted File Manager concept atlas imports without re-authoring and
matches Chromium per node (geometry, typography, raster) within tolerance.

### V. WYSIWYG theme designer

A theme designer in the spirit of a timeline-and-stage animation tool, reusing
Plan Paint's editors (gradients, curves, colour). Cues set on one control
propagate to every part with the same role; a control given a stable ID in the
interface builder is found in the theme designer, starts from its type's
template, and saves as a rule keyed on that ID.

Gate: the default theme's button cues are authored in the designer and
propagated; the saved theme passes T3 conformance and T4's reference images.

## 7. Working rules for siblings

- Read `planning/stages/README.md` and your stage's brief before starting.
- Follow `AGENTS.md`, the house style, and the build method in
  `docs/building/fast-builds.md`; do not improvise CI, caching or toolchains.
- Record each gate's measurement with workload, machine and commands.
- Keep the stage boundary: do not start a stage before its dependencies'
  gates are recorded. A, F and T1 have no dependencies.
- Raise owner decisions as direct option lists; do not decide them.

## 8. Related documents

- `DESIGN_LANGUAGE_SPECIFICATION.md`: what the default theme and the engine
  must express.
- `interview/`: the owner's original answers.
- `MASTER_IMPLEMENTATION_PLAN.md`, `CONTROL_COMPLETENESS_MATRIX.md`: the
  earlier programme and ledgers.
- `docs/dev/roadmap.md`: the public summary.

## 9. Owner decisions

Decided 2026-10-12 and recorded in `DESIGN_LANGUAGE_SPECIFICATION.md` §12.3:
atlases and PNG only for client and theme assets, never control faces; the
united text engine's shipped profile is authoritative (no LCD text); Carlito
is the body face; the proposed contrast gate; PlaySuite's character but not
its specifics, with its effects as engine capability requirements; geometry
and hit regions defined by the engine, never by a theme; Classic and a modern
flat look as the first Windows-familiar themes; command-strip commands may be
flat at rest when dividers and strip shading carry the structure, with quiet
relief through shading for faint buttons; text fields draw focus whenever
focused, other controls only after keyboard use.

Also decided: in the default theme only progress bars animate ambiently; a
derived dark variant follows the host preference and uses complementary
colours (reference: Plan Paint's Phosphor black and green); high contrast has
two authorities (the host palette under host high contrast, GUI.Forms' own
near-colourless rebuild otherwise); the proposed tokens are the starting
tokens, revisited after a native specimen sheet.

Decided in later rounds (`DESIGN_LANGUAGE_SPECIFICATION.md` §12.4, §12.5):
the default follows the Pearl Ribbon study moved toward Plan Paint's royale
blue, with Plan Paint's tab stack, ribbon buttons as a cloth bulge with no
edge, push buttons as Plan Paint's action plate, near-white `#F4F4F5` stock
and no fresco band; the derived dark variant drawn in the studies is accepted
and reaches whole windows; every theme keeps high-colour icons.

Decided 2026-10-13: the default's ribbon buttons ship as the dithered
bulge; no interim default before T4; XP-style and Windows 7-style themes
follow Classic and modern flat; part names are kebab-case and custom controls
may declare states; live theme switching, with hash-only pack integrity; form
documents use the CSS-family syntax shared with themes and the Gallery DML
migrates; the FD and DP gates are accepted; ADR-013 is retired; the 1.0
consumers are PlaySuite and Plan Paint, then the interface builder and the
demoboard program; the C# layer lives in its own repository with an
open-source corpus proposed for approval; fonts are OFL, MIT or Apache only,
the substitution table precedes host fonts in GUI.Drawing, and Consolas maps
to the owner's Portsmouth Mono; the Web.Forms fidelity oracle moves into
`tools/` and Chromium stays a test oracle; the SDRSharp spectrum and
waterfall route is deferred to careful later work; and the resource questions
RC002, RC005 and RC006 are answered (parent-locale fallback, live validated
configuration edits, one settings file per plugin; see
`interview/GUI_FORMS_RESOURCES_AND_CONFIGURATION.md` §8). Each stage brief
records its decisions.
