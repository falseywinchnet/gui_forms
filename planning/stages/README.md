# Stage briefs

Date: 2026-10-12. Status: working briefs for the stages of
`planning/FORWARD_PROGRAMME.md`. The programme is authoritative for scope,
dependencies and gates. `planning/DESIGN_LANGUAGE_SPECIFICATION.md` is
authoritative for appearance. Where a brief and either file disagree, those
files win; fix the brief.

Each stage has its own folder. Its `README.md` tells a worker who has none of
the owner's conversation history why the stage exists, what it delivers,
where the code stands today, how the owner wants it done, what the gate is,
and which mistakes have already been made. Read this page first, then your
stage's brief, then the documents the brief lists under "Read first".

## Contents

1. [Stage index](#stage-index)
2. [The owner and how to work with them](#the-owner-and-how-to-work-with-them)
3. [The programme in one page](#the-programme-in-one-page)
4. [House style for code](#house-style-for-code)
5. [Building, testing and recording a gate](#building-testing-and-recording-a-gate)
6. [Branches, pull requests and the public site](#branches-pull-requests-and-the-public-site)
7. [Lessons already paid for](#lessons-already-paid-for)
8. [Where the sources live](#where-the-sources-live)
9. [Glossary](#glossary)

## Stage index

Releases are the public roadmap's chevrons (`docs/dev/roadmap.md`,
`tools/make_roadmap_svg.py`). Everything listed is before 1.0.

| Stage | Brief | Release | Depends on (gates recorded) | Status 2026-10-23 |
|---|---|---|---|---|
| A | [Accessibility verified on every host](A-accessibility/README.md) | 0.2 Verified | none | complete 2026-10-23: client tests read the whole Gallery through MSAA and UI Automation (`results/2026-10-19-a-w1-msaa-gallery`, `results/2026-10-20-a-w3-uia-gallery`), AT-SPI (`results/2026-10-20-a-l2-linux-gallery`) and AX (`results/2026-10-20-a-macos`); editable text ranges on all three hosts (#263, #271, #273); every native host advertises the capability (#277); the macOS AX client settles before it reads (#249, #266, #275) |
| F | [Font substitution](F-font-substitution/README.md) | 0.2 Verified | none | complete 2026-10-23: gate recorded in `results/2026-10-23-f-font-substitution` (CI run 40049449883, all four native hosts); it declares 0.2. Earlier: plan accepted (brief decisions 4 to 17); F1 measurement tool `tools/font_metrics_differential`, first runs `results/2026-10-23-f1-metrics-harness`, engine against engine (Cousine and Tinos exact; Carlito and Arimo exact but for kerning and one glyph; Selawik nearest only; Tahoma Bold has no face within 5%); Arimo, Tinos and Selawik downloaded with approval, F3 adds them |
| T1 | [Anatomy: articulated controls and one schema](T1-anatomy/README.md) | 0.2 Verified | none | complete 2026-10-13 (`results/2026-10-13-t1-anatomy-gate`): 60 of 60 controls; item systems, per-part states, interactions, schema export and one-name-three-spellings merged; owner signed off, and declared T1 sealed once the follow-on DP and host work (#80, #81) was in |
| T2 | [Engine core](T2-engine-core/README.md) | 0.3 Themeable | T1 | sealed 2026-10-23 (`results/2026-10-23-t2-engine-gate/SEALED.md`; gate run 40027309396: 4 hosts, 144 images, 308 sample images, 77 features, 0 differences, 0 busy idle counters): E1 to E11 plus E7r, E9.6 and E10; bake costs measured on a quiet machine (#293) |
| T3 | [Theme language front end](T3-theme-language/README.md) | 0.3 Themeable | T1, T2 | rounds 001 and 002 answered; plan awaits last review |
| T4 | [Default theme and Windows-familiar themes](T4-default-theme/README.md) | 0.3 Themeable | T1, T2, T3 | design decided, build not started |
| T5 | [Compositor cooperation](T5-compositor/README.md) | 0.3 Themeable | T2 | plan accepted ([PLAN.md](T5-compositor/PLAN.md)), decisions T5-Q1 to T5-Q16 (2026-10-25), C0 next; reflection and full-fidelity glass moved here from T2 (owner decision 2026-10-23) |
| L | [Real dynamic layout](L-layout/README.md) | 0.4 Flexible | T1 | plan accepted 2026-10-25 (`L-layout/PLAN.md`, interview round 001 in the brief); L0 next |
| FD | [Form documents](FD-form-documents/README.md) | 0.4 Flexible | T1, T3 | not started; format decided |
| DP | [Damage-preserving painting](DP-damage-preserving-painting/README.md) | 0.4 Flexible | none | complete 2026-10-25 (#320): rectangle sets painted on every host, paint contract version 2 (#59 to #78), X11 direct live frames (#80); gate records `results/2026-10-13-dp-gate-windows`, `results/2026-10-25-dp-gate-x11` and `results/2026-10-25-dp-gate-macos`; the owner accepted macOS's copy (AppKit's union over unchanged pixels) as recorded |
| SK | [Universal Skia: one painter, one pixel format](SK-universal-skia/README.md) | 0.3 Themeable | none | owner decision 2026-10-26: Skia mandatory on every platform, DibPainter and CoreGraphicsRaster removed, RGBA8 premultiplied sRGB everywhere; brief written, SK1 (the build) next |
| W | [Windows Forms control coverage (.NET 8+ only)](W-winforms-controls/README.md) | 0.4 Flexible to 0.6 Bindings | T4, or the by-role paint path (ruling 2026-10-14) | started 2026-10-14: catalogue regenerated from the official surface; by-role paint path in place |
| R | [SDRSharp reflection](R-sdrsharp-reflection/README.md) | not on the public site | T4, F | last run 2026-08-10 |
| C | [Stable installed C ABI](C-c-abi/README.md) | 0.5 ABI candidate | T1, T2 | experimental ABI 0.26, not installed |
| S | [C# sibling: Windows Forms swap-in](S-csharp-swap-in/README.md) | 0.6 Bindings | R, C | not started |
| RB | [Ribbon framework: study, interview, decisions, plan](RB-ribbon/README.md) | to be placed by its plan | none to start | complete 2026-10-14: study, interview round 001, 27 decisions and the plan, accepted by the owner; the ribbon is built in W (`RB-ribbon/PLAN.md`), phase 0 first |
| DK | [Docking and windowing: study, interview, decisions, plan](DK-docking/README.md) | to be placed by its plan | none to start | started 2026-10-14 (admitted by the owner, matrix row 43): complete 2026-10-17: studies, interview round 001 (43 decisions, look studies published) and the plan, accepted by the owner; built in a DK window-layer stage (W0) and in W (`DK-docking/PLAN.md`) |
| DG | [Diagram control: study, interview, decisions, plan](DG-diagram/README.md) | to be placed by its plan | none to start | started 2026-10-19 (matrix row 55): study written; the interview waits for the owner |
| RT | [`stx_rtf2md`: the RTF and rich-markdown library](RT-rich-text/PLAN.md) | to be placed by its plan | none to start; W's `RichTextBox` depends on its RT1 | plan accepted 2026-10-26 (owner decisions in PLAN.md section 12; built by the SwiftEdit worker under W-Q1); RT1 next |
| Z | [Owned zlib replacement: study and threat model](Z-zlib/README.md) | 0.2 Verified (done; owner decision 2026-10-23) | none for study; owner acceptance before code | complete: stx_zlib and stx_png in production on every host, WIC decoding removed, FreeType built without zlib (GA record #240, `Z-zlib/GA_RECORD.md`); then stx_conv (CONV interpolation, the face-variant generator) and CNG (the native resource format, PNG kept for interchange; #270, #276) |
| D | [Interface builder](D-interface-builder/README.md) | 0.7 Designers | T3, L, FD | not started |
| H | [HTML import and layout designer](H-html-import/README.md) | 0.7 Designers | L, T3 | earlier attempt discarded |
| V | [Theme designer](V-theme-designer/README.md) | 0.7 Designers | T4, D core | not started |
| 1.0 | [Release gate](release-1.0/README.md) | 1.0 Stable | all of the above | conditions defined |

A, F, T1 and DP could start at once. A and F run beside everything else. T1
to T4 are the spine: almost every later stage reads the anatomy and the
engine.

**Where to pick up (2026-10-23).** A, T1, T2 and Z are complete: A's
client tests read the Gallery through each host's own accessibility API and
every native host advertises the capability; T2 is sealed with its gate
identical on four hosts. **F, font substitution, is complete**: its gate is recorded in
`results/2026-10-23-f-font-substitution`, so 0.2 Verified (A, F and T1) is
complete. The next spine stage is
**T3, the theme language front end**, now unblocked by T2; **T5** and **C**
are also unblocked (T5 now carries reflection and full-fidelity glass), and
**L** has been ready since T1. **W** continues under the by-role paint path
(ruling 2026-10-14), and **DP** is complete (2026-10-25). Read `planning/stages/T2-engine-core/README.md` and its gate record
before starting T3, T4, T5 or C, and `planning/stages/T1-anatomy/README.md`
"Hand-off" before T3, L, FD or C.

When a stage starts or a gate is recorded, update the status column here, the
stage brief's status line, and the stage board linked from `AGENTS.md`.

## The owner and how to work with them

- **Who.** The repository owner (GitHub `falseywinchnet`) designs GUI.Forms,
  Plan Paint, PlaySuite and File Manager, and drew the typeface Portsmouth
  Rapids, which GUI.Forms bundles under MIT. Do not reopen its licence.
- **Decisions.** Owner decisions are asked directly, as a short numbered
  option list with a recommendation, and answered tersely. Never decide an
  open question yourself; never leave it buried in a document. Each brief
  lists its open decisions.
- **Visual descriptions.** When the owner describes a look in words and you
  are not certain what they mean, ask for a physical analogy or a reference
  before drawing anything. The ribbon-button "bubble" took a dozen wrong
  iterations before the owner said "pretend I have my finger stuck into a
  sheet of cloth from behind it", after which it took two.
- **Interview first.** Every look, theme or design-language question starts
  from the owner's recorded verdicts (see [Where the sources
  live](#where-the-sources-live)), cited by ID. Studies made from other
  material were rejected with "this didn't consult the interviews".
- **Plan Paint is the reference realization** of the house look. Take its
  values from its source and screenshots (sample pixels), never approximate.
- **Do the work yourself.** The owner asked not to hand design or planning
  work to fresh sub-agents without context ("stop using amnesiatic
  workers"). These briefs exist so that a worker who must start cold still
  starts with the context.
- **Language.** Plain, literal English in documents and reports; no
  marketing; say what was measured and what was not.

## The programme in one page

The owner's forward programme, discussed and agreed on 2026-10-12, has five
"ways" plus a half. The stages above implement them.

1. **Way 1: theming done right.** Controls are articulated by design: every
   control is a composition of named, parameterized parts with states and
   public geometry, addressed by name ("addressable anatomy"; not a CSS
   document tree). One schema inside GUI.Forms describes parts, states,
   properties and interactions; the C++ dot API, form documents, theme
   documents and the visual tools are projections of it. One engine runs
   theme data against that anatomy. Theme data is "more than colours, less
   than code": properties, layered effects, presentational state machines and
   bounded expressions, written in a CSS-syntax subset (`::part()`,
   `:state()`, stable IDs). The engine is the machine; the state machine is
   part specification (in the theme) and part interaction (in the engine).
   New kinds of effect are C++ engine primitives under the house style, never
   theme code. The target is the capability for massive customization (funky
   media-player skins, a cursor-following sheen, buttons that pop,
   transparency, reflections, glass), while an idle window still schedules
   zero frames. The default theme is GUI.Forms' own, from the interviews,
   built on the engine. Windows-familiar themes ship beside it.
2. **Way 2: SDRSharp reflection** rerun with current technique. Emulating
   the styles SDRSharp exercised in Windows Forms is an acceptance test of
   way 1.
3. **Way 3: a C# sibling.** A Forms-shaped C# package over GUI.Forms' own C
   ABI, which supersets what Forms needs, so a Windows Forms project swaps its
   reference and runs on macOS and Linux. It must look the same; behaviour
   follows GUI.Forms' principled choices (its own event loop and ordering).
4. **Way 4: an interface builder** written in GUI.Forms, with a thin Visual
   Studio extension. Hand edits are legal and survive; nothing is clobbered.
5. **Way 5: HTML import rebuilt** on real layout, with a WYSIWYG designer.
   The earlier conversion was thrown out.
6. **Way 5.5: a WYSIWYG theme designer** that dogfoods Plan Paint's
   editors and propagates cues from one exemplar control to every part with
   the same role.

Three further commitments run through all of it:

- **Accessible by construction.** Controls are addressable, selectable and
  self-describing through each platform's native accessibility API.
  GUI.Forms exposes the capability and verifies it through those APIs;
  testing with specific screen readers is out of scope ("we don't bother
  with integration").
- **Cooperate with the compositor.** Window transparency and behind-window
  materials belong to the system window manager; GUI.Forms reports and uses
  the capability without friction and never themes the compositor.
- **Measure against the reference.** Every fidelity claim is a differential
  against the real thing, as `stx_vorbis` is checked against libvorbis:
  design-language reference images for themes, real Windows Forms on real
  Windows for reflection and C#, Chromium for HTML. Reference software is a
  test oracle, never a runtime dependency.

Fixed boundaries from `AGENTS.md` that every stage inherits: C++20 (with
Objective-C++ only in the macOS host); no GPU of any kind; Skia only behind
the private CPU renderer adapter; no .NET, Java, browser engine or Godot
dependency in GUI.Forms; PNG the only image decoder; platform types only in
their host directories; retained core, no immediate-mode API, no perpetual
redraw loop; bundled fonts only for product text; the core treats Windows,
macOS and Linux equally. On Linux the supported path is the X11 host, which
also runs under XWayland; a Wayland-native adapter is not planned and must
not be listed as a gap. FreeBSD is a desired, untested target.

## House style for code

All first-party C++ and Objective-C++ follows
`planning/PROGRAMMING_HOUSE_STYLE.md`. Read it in full before writing code;
`AGENTS.md` has a one-line summary per section. The points below are the ones
that most often decide a design in this programme, with the section number.

| Rule | Section | What it means here |
|---|---|---|
| Spell every type; no `auto`, trailing returns, structured bindings | 1, 10 | Part geometry, rule records and handles are named types at every declaration |
| Wrapper types for confusable quantities | 1 | Logical px and device px, part role and part index, stable ID and tree index, byte offset and character position are distinct types with stated conversions |
| One numeric precision per computation | 1, 3 | Effect kernels pick double (or a declared fixed-point) once; identical pixels on every host depend on it; no blanket fast-math |
| No lambdas or anonymous callbacks | 2, 8, 10 | Predicates, visitors, listeners and engine primitives are named types or functions |
| Calculate, then return; one operation per expression | 2 | Paint and layout code names its intermediate rectangles and colours |
| Choose mode before the loop; edges apart from the interior; no per-element allocation | 3 | Raster effects (bulge, dither, blur, gradients) select their kernel per part, then run a clean inner loop |
| State inputs, outputs and permitted mutation; fill caller storage | 4 | Engine passes fill caller-owned buffers; parsers fill caller-owned rule tables |
| Every datum has an owner and lifetime; reuse storage | 5 | Per-part framebuffers belong to the engine's part cache with a stated invalidation generation |
| Parents own children; back links observe; reject stale identities | 6 | Generational handles across the C ABI (interview GF017); themes never hold controls |
| Named registration protocols, explicit opt-in; no universal callable frameworks | 7 | The engine's primitive registry is an explicit protocol, not a map of `std::function` |
| Name event order, reentrancy, revocation, execution context | 8 | Accessibility callbacks, animation timers and compositor notifications state which thread they run on and how they reach the UI thread |
| Say what failure leaves behind; validate before mutating | 9 | An invalid theme rule is dropped with a diagnostic; the old theme stays until the new one is fully built |
| Foreign interfaces state types, counts, ownership, errors | 9 | The C ABI, platform accessibility APIs and the C# bindings |
| `(*pointer).member`, not `->`; hand-written comparisons, no `= default` | 10 | See `include/gui_forms/theme/types/theme_types.hpp` for the house form of `operator==` |
| Meaningful source boundaries; no accessors that add no invariant; remove stale comments | 11 | One substantial type per directory, as the tree already does (`src/controls/<family>/<control>/`) |

Shape of house code (illustrative names, not an existing API):

```cpp
// Not house style: auto, ->, a lambda, and a chain that hides a type.
auto parts = control->parts();
std::sort(parts.begin(), parts.end(), [](auto& a, auto& b) { return a.z < b.z; });

// House style: spelled types, dot access through (*pointer), a named predicate.
struct PartsInPaintOrder final {
    bool operator()(const PartRecord& left, const PartRecord& right) const {
        return left.paint_order < right.paint_order;
    }
};

const std::span<const PartRecord> published = (*control).parts();
std::vector<PartRecord> ordered(published.begin(), published.end());
std::sort(ordered.begin(), ordered.end(), PartsInPaintOrder{});
```

Match the surrounding code's naming and comment density. Before committing,
run the spelling check over the whole tree, as CI's `house-style` job does:

```bash
python3 -B tools/check_house_style.py include src demo tests examples tools file_manager_demoboard stx_vorbis
```

The checker covers spelling only. Ownership, lifetime, initialization and
naming still need a reviewer reading against the full document.

## Building, testing and recording a gate

- **The build method is part of the product.** One pinned LLVM 22 toolchain,
  one fixed directory layout, one `ccache` configuration, reuse of
  already-tested trees and published per-platform object caches
  (`docs/building/fast-builds.md`, `docs/building/reusing-ci-caches.md`).
  Copy it exactly. Do not "simplify" CI, swap toolchains or build against an
  alternative configuration; an earlier session that improvised broke the
  build for days. The ideal loop is edit, commit, forget: push and let the
  four CI platforms build and test.
- **Build against what ships.** Audio uses the shipped `stx_vorbis`, never
  `stb_vorbis`; text uses the united HarfBuzz/FreeType engine everywhere.
  A program or test built against something the product does not ship is not
  evidence.
- **Local machines.** The owner's MacBook cannot configure the project (it
  has AppleClang 21 only). Syntax-check there with
  `/usr/bin/clang++ -std=c++20 -fsyntax-only -Iinclude -Isrc -Idemo <file>`
  (add `-x objective-c++ -fobjc-arc` for `.mm`). Real builds and native GUI
  runs happen in CI or on the owner's Mac mini M4 bench; ask the owner before
  using the bench. A Windows machine (Shadow Windows, a remote desktop) is
  used for real-Windows references and Visual Studio work.
- **Tests.** A behaviour is done when a test in `tests/` exercises it and CI
  runs it on every platform. A working demonstration is evidence only for the
  behaviours it exercises.
- **Recording a gate.** A gate is a measurement, recorded in a dated file
  under `results/` (raw output) or `experiments/` (a report), with the
  workload, the machine, the exact commands and the numbers. "Fast" or
  "lightweight" needs a recorded workload and measurement. Then update the
  stage index above and the stage board.
- **Demonstration programs.** The Gallery, Explore and Correspondence
  programs are known to be unattractive. Do not restyle them by hand: they
  show what the toolkit produces, so the fix belongs in the toolkit (stage
  T4). The owner will lead a later demo rework.

## Branches, pull requests and the public site

- Never push to `main`. Work on a `claude/<topic>` branch, open a pull
  request against `main` on `falseywinchnet/gui_forms`, and let the owner
  merge. The owner deletes merged branches.
- Ask before anything outward-facing: repository settings, releases,
  publishing artifacts other than your own stage board updates.
- The documentation site (https://gui-forms.readthedocs.io/, Sphinx with
  MyST and the PyData theme, built with `sphinx-build -W`) is public. Keep the
  Windows Forms compatibility facade, the SDRSharp harness and its friend
  use-case off it. The public roadmap is `docs/dev/roadmap.md`; its chevron
  image comes from `STAGES` in `tools/make_roadmap_svg.py`, and the two are
  edited together.
- Never name or copy a specific third-party animation product's model in
  GUI.Forms designs or documents. Describe the capability instead.
- Preserve user files and unrelated working-tree changes.

## Lessons already paid for

- **Read the verdicts before drawing.** Theme studies built from the
  PlaySuite atlas instead of the interview verdicts were rejected outright.
- **Not a Windows skin.** Grey two-step bevels, sunken status cells and
  classic dialog buttons are "too windowsy" for the house look; they belong
  only to the Classic theme. Warm caption stock next to blue chrome is
  "jarring". Side panels are never raised; depth is greatest in entry fields
  and the work pane.
- **A tab is not a button.** A request for a "faint bubble" on ribbon
  buttons was misread as a curve on the selected tab, which upset the owner.
  Tabs copy Plan Paint's tab-stack shadowing exactly; the bulge belongs to
  ribbon buttons.
- **An edge fails the bulge.** Any border, line or hard shadow on a ribbon
  button bulge fails it automatically.
- **"Tighter" means a larger exponent.** For a falloff `1 − t^q`, a tighter
  slope holds near the peak longer and drops in a narrower rim: increase q.
  A curve like `(1 − t)^p` concentrates at the centre and reads looser.
- **A two-colour dither is wrong.** Dithering a smooth field needs a
  multi-tone ramp of at least three steps per side.
- **Ask before reworking a layout.** The public roadmap graphic went through
  a grid, a staircase and horizontal rows before the owner's "chevrons along
  the top, each stage a vertical section" was understood. One clarifying
  question would have saved three rounds.
- **Docs that disagree with the build are worse than none.** The owner
  checks documentation against what the code and CI actually do.
- **Planning records age.** Older documents under `planning/` describe
  earlier states (the master plan still lists a Wayland adapter; `AGENTS.md`
  still names a sibling Web.Forms project that nobody maintains any more and
  whose work GUI.Forms may absorb; ADR-013's panel reservation was retired
  on 2026-10-13). Check the code and tests before relying on a record.

## Where the sources live

| What | Where |
|---|---|
| The programme, its gates and open decisions | `planning/FORWARD_PROGRAMME.md` |
| The design language (appearance rules, owner decisions §12.3–§12.5) | `planning/DESIGN_LANGUAGE_SPECIFICATION.md` |
| Owner's architecture interview (GF001–GF020) and companion records | `planning/interview/` |
| Owner's visual verdicts (V, M, C cue IDs, colour relation model, atmospheres, style families, DDV-007) | `frontend/planning/visual/` in `falseywinchnet/file_manager` (owner's local clone: `~/file_manager`) |
| File Manager concept atlas (DNA principles) | `file_manager_demoboard/reference/frontend-concept-atlas-reference.html` |
| Plan Paint, the reference realization | `falseywinchnet/plan-paint` at `6b901fc`: `src/forms/ribbon.cpp` (`action_material`, `make_tab_theme`, `paint_separator`), `src/color_palette.cpp`, `src/canvas_backing.cpp`, `assets/screenshots/macos-home-1.1.jpg` |
| PlaySuite, evidence of the owner's unconstrained taste and a GUI.Forms consumer | `falseywinchnet/games` (owner's local clone: `~/Developer/playsuite-games`) |
| Earlier design-language drafts | `planning/DESIGN_LANGUAGE_SYSTEM_MODEL.md`, `planning/CONTROL_DNA_AND_STYLE_ADDRESSABILITY.md`, `planning/AUTHORITATIVE_CONTROL_REGIONS.md`, `planning/MICROFEATURE_CONSERVATION_PROTOCOL.md` |
| Earlier implementation programme | `planning/MASTER_IMPLEMENTATION_PLAN.md`, `planning/CONTROL_COMPLETENESS_MATRIX.md`, `planning/PAINT_PIPELINE_AVAILABILITY.md` |
| Decision records | `decisions/` (published under `docs/dev/adr/`) |
| Public roadmap | `docs/dev/roadmap.md` |
| Stage board for sibling threads | linked from `AGENTS.md`; read it with the Artifact tool |
| Published theme studies (visual reference for T4) | https://claude.ai/artifact/VLga27SkyE1rUCPPGdJmPb, source copied in `T4-default-theme/theme_studies_source.html` |

## Glossary

- **Anatomy, part, part role.** A control's named pieces (face, edge, well,
  track, thumb, focus …). A part role is the shared vocabulary name of the
  design language §8.1; a cue set on one role reaches every part with it.
- **Cue.** A visual treatment (material, edge, motion) bound to a part role
  and state.
- **Engine.** The single theme engine that runs theme data against the
  anatomy (stage T2).
- **Built-in recipe.** The always-present fallback appearance for any part a
  theme omits or gets wrong.
- **Backplane, control plane.** The style-only layer under controls and the
  interactive layer above it (interview GF018–GF019).
- **Gate.** The measured condition that closes a stage.
- **Stable ID.** A control's persistent identity, used by tests, inspection,
  form documents and theme rules.
- **Shadow Windows.** The owner's remote Windows desktop, used for real
  Windows references and Visual Studio.
- **M4.** The owner's Mac mini build and GUI test bench.
- **House Composite, atmosphere, construction.** Design-language terms:
  §4.7 and §5.6 of the specification.
