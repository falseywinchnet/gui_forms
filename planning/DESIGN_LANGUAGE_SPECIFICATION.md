# GUI.Forms design-language specification

Date: 2026-10-12. Status: second draft for owner review, grounded in the
owner's visual interview verdicts.

This document states how GUI.Forms controls look and move, in artistic,
thematic and effect terms. It is not a programming interface, source grammar,
or theme file format. Every rule names its source. A number the sources do not
supply is marked **(proposed)** and is not approved.

## 0. Citation key

The owner's verdicts and revisions outrank every other source except the owner
decisions in §12.3.

| Citation | Source |
|---|---|
| (owner direction) | owner direction relayed with the first drafting request, 2026-10-12 |
| (owner revision n) | owner direction during this revision, 2026-10-12: 1 palette fusion, 2 depth-map correction, 3 not a Windows skin, 4 no raised buttons |
| (Vnnn), (Mnnn), (Cnnn) | visual, material-depth and ceremonial verdicts: cue text from `visual-atlas.html`, `material-depth-atlas.html`, `ceremonial-software-atlas.html`; verdicts from `VERDICTS_001.md`, `MATERIAL_VERDICTS_001.md`, `CEREMONIAL_VERDICTS_001.md` |
| (material doctrine), (depth map), (depth loci) | named sections of `MATERIAL_VERDICTS_001.md`; loci D0–D4 of `MATERIAL_DEPTH_ROUND_001.md` |
| (progress grammar), (ceremony round) | `CEREMONIAL_VERDICTS_001.md` section; `CEREMONIAL_SOFTWARE_ROUND_001.md` scope and guardrails |
| (CRM §n) | `COLOR_RELATION_MODEL.md` |
| (PALETTE), (STYLE) | `VAPORWAVE_PALETTE_ATLAS_003.md`, `STYLE_FAMILY_ATLAS_004.md` |
| (DNA-xnn) | `DESIGN_DNA_006.md` principles and ledger (also embedded in `file_manager_demoboard/reference/frontend-concept-atlas-reference.html`) |
| (DDV-007-nn) | `DESIGN_DNA_VERDICTS_007.md` |
| (BRIEF), (ICONS), (CUES) | `FRONTEND_DESIGN_BRIEF_001.md`, `ICON_AUDIT_MIT_002.md`, `VISUAL_CUE_INVENTORY.md` |
| (Plan Paint) | the owner's shipped program (`falseywinchnet/plan-paint` at `6b901fc`): `src/forms/ribbon.cpp`, `editor.cpp`, `surface.cpp`, `interface_theme.cpp`, `src/color_palette.cpp`, `src/canvas_backing.cpp`, `docs/THEMES_AND_RECOVERY.md`, `assets/screenshots/macos-home-1.1.jpg` (`macos-help.png` and `macos-colors.png` are historical captures of its earlier frontend) |
| (PlaySuite file) | PlaySuite (`falseywinchnet/games`): evidence of the owner's unconstrained taste, not a rule |
| (interview GFnnn), (resources §n), (a11y round Xn / §n) | `planning/interview/GUI_FORMS_INTERVIEW_LEDGER.md`, `…/GUI_FORMS_RESOURCES_AND_CONFIGURATION.md`, `…/GUI_FORMS_ACCESSIBILITY_AND_TEXT_ROUND_001.md` |
| (SYSTEM_MODEL §n), (CONTROL_DNA §n), (REGIONS §n), (MICROFEATURE §n) | `planning/DESIGN_LANGUAGE_SYSTEM_MODEL.md`, `planning/CONTROL_DNA_AND_STYLE_ADDRESSABILITY.md`, `planning/AUTHORITATIVE_CONTROL_REGIONS.md`, `planning/MICROFEATURE_CONSERVATION_PROTOCOL.md` |
| (VISUAL §n), (atlas) | `file_manager_demoboard/VISUAL_CONSTRUCTION_SPECIFICATION.md`; stylesheet values of the demoboard reference atlas |
| (theme types), (material types) | `include/gui_forms/theme/types/theme_types.hpp`, `src/core/theme/theme/theme.cpp`; `include/gui_forms/surface_material/` |
| (text-engine), (fonts) | `docs/user/text-engine.md`, `assets/fonts/ATTRIBUTION.md` |
| (AGENTS), (master plan), (consumer profile) | `AGENTS.md`, `planning/MASTER_IMPLEMENTATION_PLAN.md`, `planning/FUTURE_APPLICATION_CONSUMER_PROFILE.md` |

The verdict documents live in `frontend/planning/visual/` of
`falseywinchnet/file_manager`. An ID with no letter was **accepted**; **D**
means distil or adapt, **R** reject, **U** insufficient reference. Some cues
are phrased as anti-models; accepting one (for example C152, C153) accepts it
as an anti-model. Disagreements are noted in place and collected in §12.

## 1. Purpose and scope

GUI.Forms has one theme engine. It works on articulated controls: every control
is a set of named, parameterized parts with states (owner direction;
CONTROL_DNA §3). This specification defines the vocabulary that engine speaks,
the limits no theme crosses, and the character of the default theme.

The default theme is GUI.Forms' own, built from the owner's interviews and
expressed as theme data on the engine, never as hand-written paint code (owner
direction; SYSTEM_MODEL §6 condition 1). Depth is an architectural styling
primitive, not decoration painted onto a flat widget system (material doctrine).

Out of scope: application content, document colour, game art and custom
drawing, where raw colours belong (interview GF018; CRM §5); the theme
authoring syntax (§12.1 item 1); host window chrome. Window transparency, blur
and shape need the host window manager; GUI.Forms only reports and uses that
capability (owner direction; AGENTS).

### 1.1 How the verdicts apply

The verdicts were given for File Manager; the owner directs that they shape
GUI.Forms' design language. They are translated into toolkit terms:

| File Manager term | Toolkit term |
|---|---|
| daily file list, object field | everyday work surfaces; the work pane |
| title fresco | identity band |
| ribbon, shelf | command plane, command strip |
| graphite chassis, gutters, seams | chassis |
| folder, search and properties panes | side panels, information panes |
| installer, first run, rare long tasks | ceremonial surfaces |

File Manager product rules stay out: folder-size badges (DDV-007-17), tree roots
(DDV-007-16), search scope, command membership (DDV-007-03), drag semantics
(DDV-007-06), match percentages (DDV-007-09) and the criteria rack
(DDV-007-10). One appears only as a consumer example.

### 1.2 What a theme is

A theme is immutable, data-only presentation: "more than colours, less than
code" (owner direction).

- **It may declare** colour pairs; material stacks (solid, gradient, glow,
  sheen, transparency, reflection, glass); edges, keylines and shadows; choices
  among the engine's shape parameters; typography-role selection among bundled
  faces; motion timings and easing; per-part presentational state machines; and
  bounded expressions (owner direction; resources §2.1; material types; §12.3).
- **It may not declare** controls, input behaviour, event handlers, commands,
  semantic roles, hit regions, filesystem or network access, executable code, or
  font bytes (resources §2.1, §2.3; CONTROL_DNA §7 gate 5; interview
  GF018–GF019; §12.3).
- A theme crosses two independent axes: a **construction** (materials, relief,
  edge grammar; §4.7) and an **atmosphere** (colour pairs; §5.6). Changing either
  keeps control identity, commands, content, semantic state, selection, focus,
  spacing and topology (PALETTE; STYLE; DNA-A09).
- A named scheme may bundle atmosphere, construction, typography roles, icons,
  pointers and sounds, and shows which members it will apply (M034, M035).
  Several authored schemes share geometry and behaviour (M036; V010; V198).
- Appearance settings preview a miniature, consistent interface showing active,
  inactive, selected, message, scroll-bar and typography states at once (M031,
  M032); that preview well may be deeper than its dialog (M033).
- The engine is expressive enough for loud skins in the manner of early-2000s
  media and audio players, a title-bar sheen that follows the cursor, and
  buttons that pop. Historical themes such as royale or watercolour looks are
  capability references; GUI.Forms does not ship them (owner direction).
- A theme may be partial; anything it omits falls back to the built-in theme,
  which is always present (resources §1, §2.1).

## 2. Principles

### 2.1 Affordance through depth

1. Physical, click-capable controls belong to the language (V052, V057, V216),
   but buttons are not raised in the Windows sense. An action is a slight 3D
   pill: a soft tinted gradient face, a thin border and Plan Paint's inner
   light-over-dark shading with a one-pixel contact shadow, which reads as a
   convex face resting on its plane, never as a lifted bevel (owner revision 4
   as clarified in §12.4; Plan Paint `action_material`).
   Deep heavy bevelling is rejected (V077 R).
2. A tinted flat plate means actionable, inset means editable or instrumented,
   the recessed work pane means content, shadow means real overlap (DNA-04;
   M023 as revised by owner revision 4).
3. Depth explains physical role; colour explains semantic state. A deeper
   control gets no extra chroma; a coloured selection does not flatten (CRM §6;
   VISUAL §2).
4. The doctrine is principle, not a Windows rendering: grey two-step bevels on
   every control, sunken status cells and classic dialog buttons are "too
   windowsy" (owner revision 3). The reference realization is Plan Paint (§11).
5. Losing a small relief feature is a regression even when colour, place and
   function survive (MICROFEATURE §1, §6.1).

### 2.2 Where flatness is allowed

1. Flat chrome is allowed where content must hold attention, such as a paint
   program's tools around its canvas (owner direction).
2. Command-strip members may be flat at rest when the strip carries the
   structure; hover and press bring full relief (§12.3, flat commands). A seam
   grip may be flat at rest and gain size, contrast and depth on approach
   (DDV-007-04).
3. Low relief is acceptable only when boundaries, hover, focus and input stay
   unequivocal (M076); controls may be physically distinct without gradients,
   blur or large radii (M102).
4. Standalone text fields, check boxes, scroll bars, sliders and buttons keep
   relief in every shipped theme outside rule 1 (owner direction).
5. Wide flat colour fields are an anti-model on everyday surfaces (C153).

### 2.3 Richness is geographically bounded

1. Richness is assigned to loci: the identity band carries identity, icons carry
   object material, instruments go deeper, rare ceremony may become a world
   (DNA-08; C121; C054 D).
2. A coloured identity region over a dense white information field is the house
   balance (C132, C144; V006). An illustrated identity world may end sharply at
   a severe utility surface (M001); a saturated field may sit beside plain white
   data without tinting it (M003).
3. Breathing room, illustration and creative-suite richness belong to ceremony,
   never everyday work surfaces (C044, C054 D; §10).
4. Glass or gradient applied uniformly is rejected; decoration never outranks
   selection, focus, failure or object identity (VISUAL §13).

### 2.4 Bright optimism

1. Bright optimism wins over dark editorial prestige: Visual Studio blues,
   Office warmth, Watercolor fields, compact side art (ceremonial synthesis;
   C031, C043, C062). Editorial black, portfolio montage and Longhorn darkness
   are rejected (C051 R, C052 R, C091 R); controlled mystery stays desirable for
   rare long operations (C093).
2. Dark primary surfaces lie outside the direction; a dark charcoal transient
   menu with soft corners, mild gradient, explicit boundaries and clear hover
   and focus is within bounds (M071). The derived dark variant is an owner
   decision (§12.3); it reaches whole windows (§12.4).
3. Warm and cool are not fused as stock: warm caption stock or apricot beside
   blue chrome is jarring; neutral ivory panel stock on blue works (owner
   revision 1). Warmth enters as state, signal or rare ceremony (§5.5).

### 2.5 Honest structure, composed density

1. One line or material change per true grouping boundary (DNA-03). Nested
   hairline borders, gradient category bars, luminous collapsible headers,
   alternating rows and the web-table feeling are rejected (V022 R, V021 R,
   V016 R, V211 R, V023 R, V033 R); plain category bands are accepted (V017).
   Groups are etched or geometrically bounded, not carded (M022; depth map).
2. Dense is not cramped (DNA-09): dense rhythm is redrawn, not pixel-scaled
   (V031); little dead space (V069); dense professional controls without apology
   (V082 D). Composed and breathable, not spacious: large calm margins and
   dead-space inflation are rejected (M055 R, V212 R; V213 D).

### 2.6 One light

One global light, from the upper left, governs every highlight, shadow, bevel,
icon inset and selection edge (M101; DNA-V03; VISUAL §2; PlaySuite paper engine:
"the light is fixed"). It never follows the pointer, time or animation; a
cursor-following sheen is a reflection (§9.3), not a moving light (proposed).

### 2.7 Familiar behaviour, authored appearance

Behaviour stays conventional and Forms-like; appearance is authored and need not
match any host (DNA-12; interview GF009–GF011). A custom-rendered system keeps
conventional behaviour while owning every material (M056, C053); custom
appearance is not native-control imitation (ceremonial synthesis; owner
revision 3).

### 2.8 Performance is part of appearance

No decoration delays first useful content, input response or damage repair;
under pressure ornament degrades before meaning (DNA-14, DNA-A12; interview
GF002). Polish is not worth slower startup or a heavier renderer (C143).

### 2.9 Limits no theme crosses

1. A theme changes style and resources only, never semantics, role, commands,
   input behaviour, hit regions, interactive order or data (resources §2.1;
   CONTROL_DNA §7 gate 5; interview GF018–GF019; REGIONS §3; §12.3).
2. Native accessibility publication is never turned off (a11y round A3, §3.2).
3. Host high contrast, text scale, display scale, reduced motion and focus
   visibility are authoritative; themes specialize, never suppress them (a11y
   round A6; DDV-007-13).
4. Every pair meets the contrast gate (§5.7) in its actual state.
5. Focus stays visible with a non-colour cue (§7.3).
6. No state relies on colour, motion, sound or hover alone (a11y round §3.4;
   VISUAL §10, §12; DDV-007-12; CRM §4 step 6).
7. Text scaling grows or reflows controls; essential names and actions never
   clip silently (a11y round T5; DDV-007-02).
8. A theme or role that fails to load falls back to the built-in theme for that
   role (resources §1, §2.1).

## 3. Light and depth

### 3.1 Material doctrine

GUI.Forms uses a complete, systematic physical grammar (material doctrine; the
Windows 95 appearance family M101–M105 accepted in full):

| Rule | Source |
|---|---|
| one global light | M101 |
| action plates and inset edit wells; plates are flat and tinted, not raised (owner revision 4) | M023 |
| a precise two-step edge, outer contrast plus inner highlight, as the standard boundary; consistent one-pixel highlight and shadow logic suffices | M103, M024 |
| physical distinction without gradients, blur or large radii as the floor | M102 |
| etched semantic groupings | M022 |
| visibly mechanical spinner actuators attached to their field | M025 |
| disabled loses contrast and depth without disappearing | M104 |
| default action marked by a second dark perimeter, not colour, glow or pulse | M105; V116 R |
| deeper instruments inside shallower command structure | M066, M033 |
| window and plane shadows only where they establish real layering | M051; DNA-F06 |
| one surface may combine recessed content, inset instruments and flat tinted actions | M026; owner revision 4 |

**Rendering.** In the house look an action plate is a slight pill, exactly
Plan Paint's `action_material`: a `#D2E4FB` to `#A3C1EC` face, a thin `#6384B2` border, a white highlight inside the top and left edge, a darker blue line (`#315686` at about 53 %) inside the bottom and right, and a one-pixel contact shadow below (`#294B77` at about 33 %), with 3 logical px corners (owner revisions 3 and 4; §12.4).
The deep depth of the house look is recess: entry wells and the work pane sink;
nothing is lifted by a bevel. Grey square bevels,
raised buttons, sunken status cells and classic dialog buttons belong only to
the Windows-familiar Classic theme (§12.3).

### 3.2 Depth map

Depth is assigned by locus (depth map; depth loci D0–D4), as corrected by the
owner (owner revision 2):

| Locus | Depth | Source |
|---|---|---|
| Entry fields | **deepest**: inset well, local occlusion shadow, clear editing boundary and focus | owner revision 2; depth map; M023; PALETTE rule 4 |
| Work pane | **deepest**: a recessed inset plane sunk below the chrome around it; its content stays calm, with explicit selection and grounded objects | owner revision 2 (supersedes the depth map's "calm plane"); M012 R; Plan Paint canvas surround |
| Instrument | inset, deeper than commands; may use dark or live-data material | depth map; M061, M066, M021 |
| Action | flat tinted plate, not raised; press darkens the face | owner revision 4; M023; DNA-C09 |
| Command-strip member | flat or quietly shaded at rest; shallow relief on hover and press | §12.3, flat commands |
| Grouping | etched or geometrically bounded, not carded | depth map; M022 |
| Side panels | **not raised**: flat at chassis level, or receding | owner revision 2 |
| Chassis | flat; seams and grips carry the mechanics | STYLE; DDV-007-04 |
| Window, popup | shadow and separation proportional to actual stacking | depth map; M051; DNA-F06, DNA-F07 |
| Ceremony | may become a complete illustrated or procedural world | depth map; D4; §10 |

1. Command plates and buttons are flat; nothing rises above the chassis, and
   no side panel competes with the work pane (owner revisions 2 and 4).
2. The work pane has an inner top-left shade and a crisp boundary around calm
   paper or a canvas. Objects on it are grounded by geometry, grouping or an
   authored surface, never floating on bare white and never carded (M012 R;
   VISUAL §7).

### 3.3 Light and edges

| Property | Rule | Source |
|---|---|---|
| Key light | upper left; azimuth 135° from +x | M101; angle (proposed) |
| Cast shadow | down and slightly right; horizontal offset at most half the vertical | Plan Paint sheet shadow (1.5, 2.5); ratio (proposed) |
| Plate edge | one thin coloured border, with Plan Paint's inner light top/left and dark bottom/right lines inside it | owner revision 4 as clarified in §12.4; Plan Paint |
| Plate face | soft vertical tinted gradient, lighter at the top, following the key light | M101; Plan Paint |
| Inset well | inner top shade line, outer bottom highlight, thin border | Plan Paint gallery well; atlas |

### 3.4 Depth levels

Shadow strength follows real overlap; internal regions use edges rather than
cast shadows; a control carries only Plan Paint's one-pixel contact shadow (DNA-F06; M051; §12.4).

| Level | Meaning | Reference treatment |
|---|---|---|
| S2 deep recess | entry well; work pane | inner top-left shade, crisp boundary; Plan Paint surround, sheet shadow (1.5, 2.5) blur 6 about 29 % |
| S1 recess | instrument, gallery and preview wells | 1 px border, inner top line 14–29 %, white bottom highlight (Plan Paint gallery) |
| E0 flush | chassis, side panels, resting flat members, paper inside a well | none |
| E1 plate | resting control on its plane | slight pill: tinted gradient face, thin border, inner light/dark lines, one-pixel contact shadow (§12.4) |
| E2 selected tab | the selected tab joins the ribbon field | lighter face, open bottom edge (Plan Paint tab) |
| E3 floating | expanded result, floating capsule, completion list | `0 3px 8px` to `0 5px 10px` at 20–33 % (atlas) |
| E4 overlay | popup, menu, drawer | `0 13px 32px` about 33 % plus `0 2px 5px` about 40 % (atlas) |
| E5 window | separate floating window | `0 16px 34px` about 40 % plus `0 2px 5px` about 53 %, 1 px light inner rim (atlas) |

Hover brightens a plate and darkens its border; press darkens the face; neither raises nor lowers it
(VISUAL §7; DNA-C08, DNA-C09). A shadow stays inside the overflow its parent
grants (REGIONS §2). Values are starting tokens (§12.3).

### 3.5 Relief types

| Relief | Construction | Typical parts | Source |
|---|---|---|---|
| Pill plate | soft vertical tinted gradient, one thin coloured border, inner light top/left and dark bottom/right lines, one-pixel contact shadow | push button, chip, thumb | owner revision 4 as clarified in §12.4; Plan Paint `action_material` |
| Bulge | no border, line or shadow: the plane itself swells, as if a fingertip were pushed into cloth from behind; a highlight on the slope facing the upper-left light, shade on the slope facing away, a faint crown; each holds near its peak across the bulge and falls off over a narrow band at the rim: intensity × (1 − t^q) with q about 2.2 over the half-width t (a tighter slope means a larger q; a curve concentrated at the centre, such as (1 − t)^p, reads looser and is wrong) and melt back into the plane before the part's bounds; pressed, it becomes a dent (light and shade swapped); the active tool is the same bulge in gold | ribbon command buttons | owner, §12.4 |
| Dithered bulge | the bulge field above, subsampled onto a regular lattice (1 logical px dots every 2 px) with an ordered 4×4 threshold: a light dot where the lit slope exceeds the threshold, a dark dot where the shaded slope does, the plane showing between; no edge; pressed and active as for Bulge | ribbon command buttons (studies); a theme choice | owner, §12.4 |
| Inset well | flat or lightly darker-top face, inner top shade, outer bottom highlight, thin border | text well, check box, track, gallery | M023; Plan Paint well |
| Pressed | face darkened, inner lines inverted (dark top/left, light bottom/right), contact shadow removed | button, command, segment | Plan Paint pressed tile |
| Active tool | warm gold gradient plate, deeper gold border, flat | selected tool, current choice in a strip | Plan Paint gold; CRM pair B-prime |
| Default | second dark perimeter around the plate | default button | M105; theme types `default_ring` |
| Disabled | contrast reduced, faint border, paler face | any | M104; Plan Paint disabled tile |
| Etched | shade and light 1 px line pair, fading at both ends | dividers, group boxes | M022; Plan Paint `paint_separator` |
| Seam | thin parent-owned boundary, wider hit area | splitter, breadcrumb and tab joins | VISUAL §3.2; REGIONS §4; DDV-007-04 |
| Actuator | separate attached plates with their own press | spinner and scroll arrows | M025 |
| Embossed | small raised low-colour capsule in content | coarse badge (consumer example: DDV-007-17) | VISUAL §3.4 |

Press is local: it never moves surrounding content or layout (VISUAL §7).
Pressed, hot, default, disabled and focused are distinct states of the control
contract (CUES V020; M104, M105).

### 3.6 Depth under scale

1. Display scale and text scale are independent (a11y round §3.4). Text and
   targets scale fully; hairlines, shadows, texture and optical details scale by
   function (DDV-007-14; DNA-A07). Edges snap to the device grid independently of
   text (a11y round §4.4 rule 5).
2. **(proposed)** Keyline device width = max(1, round(logical width × min(display
   scale, 2))). Shadows scale with display scale, not text scale; a control grown
   for large text keeps its elevation. Procedural texture is regenerated per
   display scale, not resampled (PlaySuite wood is remade per display).

## 4. Materials

### 4.1 The two planes

1. The **style backplane** holds fills, frescoes, patterns, textures,
   illustrations and wells, and may be translucent; the **control plane** above
   it owns hit testing, focus, state, events, text and control relief (interview
   GF018–GF019).
2. Backplane objects cannot replace a control, intercept its events, change its
   role or impersonate plugin UI; the planes keep separate damage and cache
   generations (interview GF018–GF019).
3. In-window glass, translucency and reflection sample the backplane, never other
   controls (proposed).

### 4.2 Material stack

A material is an ordered stack: up to 8 fill layers (solid, linear or radial
gradient, or image stretched, tiled or nine-patch, each with opacity and
coordinate space); up to 4 outer or inset shadows; one border or per-edge
borders; up to 8 single-edge keylines; one corner radius (material types). The
engine adds glow, sheen, reflection and glass (owner direction). Every layer
kind has a bounded cost and declared damage extent (DNA-A12; interview GF002).
Image fills serve backplane and client assets, never control faces (§12.3).

### 4.3 Material families

| Family | Character | Where | Source |
|---|---|---|---|
| Paper | white or near-white, no structural texture | inside the work pane, documents, wells | V006, V194; VISUAL §7 |
| Cobalt plane | pale blue vertical gradient, light top line, shade line below | command planes | Plan Paint ribbon; STYLE Office Pearl adapted |
| Pearl | low-contrast pearl-grey bounded plane, never borderless | status strip, dialogs, information panes | M053; DNA-V05 |
| Ivory stock | neutral low-chroma ivory, never warm-tinted beside blue | side panels, captions, explanation regions | owner revision 1 |
| Tile | soft blue-gradient flat tinted plate (§3.5) | command and action buttons | Plan Paint; owner revision 4 |
| Gold | warm gold gradient plate | active or selected tool | Plan Paint gallery recipe |
| Gloss | four-stop gradient with a hard break near mid-face (0, .48, .52, 1); 1 px shine under a flat top | loud themes; candy controls adapted | PlaySuite `suite.cpp`; V122 D |
| Felt, graphite | middle-value grey, sub-3 % texture, never near-black, never behind body text | work-pane surround, chassis, seams | Plan Paint surround; DNA-V04, DNA-P10, DNA-V12 |
| Fresco | three-stop structural gradient plus three broad low-frequency blooms, lower specular keyline | identity band | V003; PALETTE; VISUAL §7 |
| Glass well | small translucent jewel with rim highlight | app mark, identity wells | VISUAL §7 |
| Instrument black | black measurement well in a light utility, one phosphor colour for live data | meters, scopes, readouts | M061, M062 |
| Brushed metal | fine directional grain | frame around utility content | V118 |
| Pinstripe | fine horizontal stripe field | dialog or pane field in selected themes | V117 D |
| Natural | wood grain, felt pile, paper fibre from parameters | backplane | PlaySuite `wood.hpp`, `shared/felt`, `shared/paper` |

Instrument black and the dark transient menu (M071) are the only dark materials
the default admits on a light surface.

### 4.4 Procedural pattern arsenal

Static patterns for ceremony and other bounded ornamental zones (M111–M144;
motion was not judged):

| Pattern | Rule | Source |
|---|---|---|
| Pipes | glossy tubular geometry in independent colours, right-angle networks; may become a faint fresco or etched motif; ceremony or bounded zones only | M111–M114 |
| Maze | synthetic brick, stone, wood and glass; low-poly architecture for a rare empty-state or installer vignette; period texture-mapping character distilled | M121–M124 (§12.1 item 9) |
| Flower Box | one small iridescent polygonal object, easter-egg level; violent face colours and black void rejected | M131 D, M132 R, M133 R, M134 |
| Flying Windows | a house object repeated at radically different scales, in a rare frozen ceremonial texture; sparse multicolour objects on black rejected; branding restrained | M141, M142 R, M143, M144 D |

### 4.5 Procedural before bitmap

1. Control faces, edges, glyphs and effects are procedural or vector (owner
   direction; §12.3).
2. PNG is the only admitted image format. Image atlases and PNG serve client and
   theme assets such as icons, a tiling background or a backplane fresco, never
   control faces (resources §1, §4; §12.3).
3. Procedural materials are built-in generators; a theme supplies parameters,
   never generator code (resources §2.1). Plan Paint generates its work-pane
   surround as a seeded tile and offers thirteen felts, velvets and brushed
   finishes as user choices (Plan Paint `canvas_backing.cpp`).
4. **(proposed)** Expensive generators run once per scale off the input path and
   are cached; until ready the region shows the material's solid pair (PlaySuite
   felt costs about one second per tile).

### 4.6 Translucency, glass and reflection

1. Translucency appears only where it clarifies layer and depth, never as a
   constant surface effect (C104); dark or translucent side structure is
   rejected (V101 R).
2. Aurora-like composited light may be a transient background material, chiefly
   in ceremony (C101).
3. Blur is a bounded CPU operation cached against the backplane's damage
   generation (interview GF002, GF018–GF019); radius at most 8 logical px
   **(proposed)**. Reflection is a bounded copy of the part's own content or the
   backplane within its grant (owner direction; bound proposed).
4. Window transparency and blur are a reported host capability; without it the
   window is opaque (owner direction; AGENTS).

### 4.7 Construction families and the House Composite

The construction axis changes how an interface is built, edged, grouped,
weighted and lit, with atmosphere and topology held constant (STYLE). The twelve
families are atoms: a theme selects one or combines named atoms from two or
three (STYLE review discipline).

| No. | Family | Construction | Named risk |
|---:|---|---|---|
| 01 | Watercolor Instrument | square shallow controls; aurora fresco; restrained etched boundaries | too cautious |
| 02 | House Composite | zoned synthesis, below | seams between material systems |
| 03 | Studio 2003 | menu stock, horizontal labelled commands, flatter precise panels | generic enterprise |
| 04 | Office Pearl | glossed groups, small-radius plaques, bevel rhythm | excessive gloss |
| 05 | Aqua Technical | pinstripe pearl, gel buttons, large soft edges, deeper shadows | toy-like; costs density |
| 06 | Workshop Graphite | dark tool chassis around a white field | darkness overtakes optimism |
| 07 | Machined Workstation | hard grooves, explicit relief, segmented title rail, inset wells | period skin |
| 08 | QNX Precision | thin edges, compact horizontal tools, terse captions | lacks deluxe depth |
| 09 | Object Desk | heavy object frames, group boxes, forceful focus | bureaucratic double relief |
| 10 | MSN Jewel | glossy command objects, luminous destination header | friendliness outruns seriousness |
| 11 | Delphi Laboratory | gripped micro-toolbars, clipped captions, dense controls | crowds ordinary work |
| 12 | Sky Pearl | bounded pearl planes, rounded internal controls, soft shadows | anonymous polished chrome |

A construction may vary title construction, command orientation and plate
visibility, edge grammar, shadow and inset strength, captions, actuator material,
focus force and light/dark balance (STYLE). It chooses among the engine's shape
parameters; it never sets geometry or hit regions (§12.3).

**House Composite** is the owner-directed zoning (STYLE; DNA-V02; VISUAL §2),
generalized:

| Zone | Source language | Toolkit rule |
|---|---|---|
| Identity band, command planes | Watercolor fresco; Office ribbon anatomy | fresco band; tab rail; command plane with thin dividers and group labels |
| Chassis | Workshop Graphite, corrected | middle-value textured graphite, never black; seams 3 px or thinner, wider hit targets |
| Side panels, information panes | Studio 2003 | compact captions, fine blue-grey edges, compact section bars, factual density, neutral ivory stock (owner revision 1) |
| Work pane | classic object field | recessed inset plane with white paper or a canvas (owner revision 2) |
| Objects | classic-iOS material lineage | upper-left light, local gradients, occlusion, restrained shadow, specular rim, per-size drawing |
| Status | technical desktop instrument | flat pale strip, separated factual cells, small soft chips, no sunken cells (owner revision 3; Plan Paint) |

## 5. Colour

### 5.1 The pair is the primitive

A foreground is never chosen apart from its background. A token names a
relation, such as violet-on-white; "purple text" alone is incomplete (CRM §1;
DNA-11). A pair records background and foreground in OKLCH, its phase family,
its contrast class (body, large, instrument, disabled, decorative) and its gamut
policy (CRM §1). Ordinary controls consume pair roles; raw colours remain for
custom drawing and the backplane (CRM §5; interview GF018).

### 5.2 The phase diamond

The owner's four example relations describe offsets between background phase
and text phase (CRM §2). Plan Paint realizes each as a three-colour family with
an accent (Plan Paint `themed_color`):

| Family | Background → foreground | Plan Paint family, accent |
|---|---|---|
| A | white → violet `#9261B3` | Glue, `#B3C76C` |
| B | black → green `#82B361` | Phosphor, `#B36182` |
| A-prime | parchment `#EEE7CB` → teal `#356B62` | Camo, `#B96955` |
| B-prime | blue `#3056A1` → gold `#FFD27A` | Royal, `#C76073` |

The diamond seeds a family generator; four samples do not suit every size,
weight, display or mode (CRM §2). Pair B is the dark variant's reference
(§12.3). Pair B-prime is the relation behind the gold active-tool plate on blue
chrome (Plan Paint gold, about `#FFEDBA` to `#FFCF66`). Plan Paint carries six
more families on the model: Afterglow, Arcade, Lagoon, Porcelain, Bordeaux,
Ochre.

### 5.3 Seed palette and derivation

1. Plan Paint's MS Paint-style default colour swatches come from the Office
   2007–2010 default theme row (`#1F497D`, `#EEECE1`, `#4F81BD`, `#C0504D`,
   `#9BBB59`, `#8064A2`, `#4BACC6`, `#F79646` with tints; Plan Paint
   `office_color`). That row is a swatch set for colour pickers, not a seed for
   themes; theme colours derive from the colour pairs, the phase diamond and
   the atmospheres (CRM §1–§2; §12.4).
2. Derivation (CRM §4): choose an anchor hue; derive the pair in OKLCH by a
   declared phase relation; adjust lightness and chroma jointly for the contrast
   class; gamut-map the pair together; derive hover, pressed, selected, disabled,
   active-window and inactive-window variants by transforming the pair together;
   verify meaning in monochrome and high contrast. Small body text may need a
   stronger derivative; a decorative pair never passes as body.
3. Chrome hue is a parameter. Plan Paint rotates its authored blue chrome in
   OKHSL, keeping lightness, saturation, gradient geometry and alpha; icons,
   document pixels, neutral surfaces, gold states and high-contrast recipes keep
   their colours (Plan Paint THEMES_AND_RECOVERY). The engine offers this as an
   atmosphere derivation (proposed).

### 5.4 Role set

Minimum role set **(proposed)**, each a pair (VISUAL §4; theme types):

| Group | Roles |
|---|---|
| Surfaces | ink on paper, muted ink on paper, ink on soft paper, ink on chrome, ink on chrome low, ink on ivory stock |
| Relief ramp | chrome highlight, chrome, chrome middle, chrome low; ordinary line, dark line, shadow |
| Interaction | focus on any surface; selection ink on selection with selection line; ink on gold; on-accent on accent |
| Signal | signal on paper; success on paper; pending ink on tint; invalid ink on tint |
| Availability | disabled ink on chrome and on paper |
| Instrument | live-data phosphor on instrument black |
| Command text | command-link ink on paper |

Links are house command text, never browser-blue underlined hyperlinks (M043 R,
M005 R); a small typographic link is acceptable only as a real, stable command
(C124). This replaces the first draft's link and visited-link roles.

### 5.5 Allocation: scarce and relational

| Rule | Source |
|---|---|
| 1. Colour concentrates in the identity band, selection, focus, object icons, live data and rare signals, not every rail | DNA-V06; PALETTE; V006 |
| 2. Chrome surrenders nearly all colour to the objects it presents, yet is tinted and shaded, never plain grey or undecorated | M014; V061 D; V202 R, V204 R, V217 R, V215 R |
| 3. Structure is not tinted with sparing pastel or sprinkled with accents against neutral machinery | V219 R, V076 R, V085 R |
| 4. Selection is a deliberate semantic pair with an explicit boundary, never neutral grey; active is blue, inactive explicitly greyer | M065 R; DNA-O07; V009 |
| 5. Green is a small affirmative accent; orange a rare warmth counterpoint; one saturated phosphor colour is reserved for live data | V013 D; V014; M062 |
| 6. In the default, warm hues never serve as stock, captions or ambient bloom beside blue chrome; they enter as state (gold active tool), signal (coral) or rare ceremony | owner revision 1; C041 |

### 5.6 Atmospheres

The atmosphere axis carries twelve pair families in one house system,
independent of construction; **Sapphire Dusk** is the default (PALETTE;
DNA-V01). Geometry, spacing, density, topology, affordance and meaning stay
constant across atmospheres (PALETTE; V010; V198; M036).

| No. | Family | Fresco A → B → C | Paper | Mid chrome | Selection | Accent |
|---:|---|---|---|---|---|---|
| 01 | Encarta Cobalt | `#07558D → #2784B8 → #65509A` | `#FFFFFF` | `#CEDDE8` | `#D7EAFF` | `#356B9A` |
| 02 | Amethyst CRT | `#25195F → #7647B5 → #C64B9E` | `#FFFEFE` | `#D5C8E4` | `#EADCFF` | `#70439A` |
| 03 | Miami Ledger | `#075965 → #18A9AC → #D75F9C` | `#FFFFFF` | `#C3DCDA` | `#CFF5EF` | `#1F7D80` |
| 04 | Orchid Relay | `#4B1E59 → #98508E → #DC6F83` | `#FFFDFD` | `#DBC7D7` | `#F4DCE9` | `#87436F` |
| 05 | Aqua Memory | `#075679 → #1599AC → #4B88C8` | `#FFFFFF` | `#C5DDE2` | `#D3F2F4` | `#267884` |
| 06 | Apricot Modem | `#5B2C4C → #BE5D65 → #E99A68` | `#FFFEFA` | `#DFCFBE` | `#F7E2D2` | `#8D4E60` |
| 07 | Mulberry Glass | `#2D1745 → #78366F → #9F5DB9` | `#FFFEFE` | `#D4C3D6` | `#ECD9EF` | `#7B3D78` |
| 08 | Viridian Laser | `#075552 → #26956E → #4F6EC0` | `#FFFFFF` | `#C7DBD1` | `#D6EFE3` | `#2D7963` |
| 09 | **Sapphire Dusk** | `#17347F → #3A68CB → #D96872` | `#FFFFFF` | `#C9D4E5` | `#DBE6FF` | `#3E61A3` |
| 10 | Rose Quartz | `#682143 → #C4527E → #6152AA` | `#FFFDFD` | `#DBC6D1` | `#F4DCE8` | `#934765` |
| 11 | Electric Iris | `#31206F → #7042C3 → #22AEC5` | `#FFFFFF` | `#D0C9E1` | `#E5DDFF` | `#6546A0` |
| 12 | Phosphor Violet | `#2B4247 → #4C9571 → #76509B` | `#FFFFFF` | `#CBD5D2` | `#DCEBE3` | `#487662` |

1. Every atmosphere shares one material stack: upper-left light; fresco of a
   three-stop gradient and three blooms; flat tinted plates with one thin
   border (owner revision 4; the atlas's raised plates are superseded); inset wells with local occlusion; deeper borders only
   at plane boundaries; semantic selection as a pair; white or pearl paper with
   no coloured wallpaper; object icons independent of chrome tint (PALETTE).
2. Paper stays white, pearl or almost imperceptibly tinted (V194); in a blue
   atmosphere pearl stays near-neutral (owner revision 1).
3. A graphite-blue Ergonomic alternate with identical geometry is a house
   variant (V010, V198).
4. Values are starting tokens; ink and accent were measured against paper for
   every family (PALETTE contrast check), and the state-by-state gate (§5.7)
   still applies.

### 5.7 Contrast

| Pair | Minimum |
|---|---|
| body, label and value text | 4.5 : 1 |
| text ≥ 14 logical px bold or ≥ 18 regular | 3 : 1 |
| control boundaries and state cues; focus against both neighbours | 3 : 1 |
| disabled text, plus a non-colour difference from enabled (DNA-C08; M104) | 3 : 1 |
| all text in local high contrast | 7 : 1 |

The gate is accepted (§12.3). Contrast is measured where text actually sits: on
the gradient stop beneath it, through translucency, with selection applied
(VISUAL §4). The pair's contrast class selects the row (CRM §1, §4).

### 5.8 Derived variants

1. Variants are derived from the role model, not hand-forked (owner direction);
   a theme may refine a derived variant, never suppress a host accommodation
   (a11y round A6).
2. **High contrast.** Two authorities (DDV-007-13; §12.3): under the host's mode
   the theme yields almost completely to host colours and metrics; GUI.Forms'
   own mode is a near-colourless semantic reconstruction with no texture
   dependence and explicit boundaries, focus, selection and authority. High
   contrast replaces decorative material rather than intensifying it (VISUAL
   §4). **(proposed)** It keeps geometry, anatomy and state composition, drops
   gradients, texture, glow, sheen, reflection, glass and cast shadows, and
   draws every edge solid.
3. **Dark.** Derived by rule from the light theme, following the host's dark
   preference and user-overridable; the light stays upper left, inset stays
   inset, highlights dim and shadows deepen; complementary colours with pair B
   (black `#000000`, green `#82B361`, accent `#B36182`) as reference (§12.3;
   §5.2). It reaches whole windows (§12.4).

### 5.9 Colour never carries meaning alone

State stays evident in grayscale, without hover or motion, and with sound off
(a11y round §3.4; DNA-C08, DNA-O07; VISUAL §10; CRM §4).

## 6. Typography and iconography

### 6.1 Faces

| Face | Role | Source |
|---|---|---|
| Portsmouth Rapids regular and bold, the owner's own face (MIT) | control: titles, buttons, labels, menus, captions, throughout chrome | interview GF004; DDV-007-05; Plan Paint; fonts |
| Carlito, four styles | content: values, explanatory text, object labels, properties, lists | §12.3; DDV-007-05 ("Tahoma/Calibri-like humanist"); text-engine |
| Cousine, four styles | monospace: path editor, instrument text | text-engine |
| Noto Sans CJK JP, Arabic, Hebrew, Devanagari, Bengali, Gurmukhi; monochrome Noto Emoji | per-cluster fallback only | text-engine; fonts |

1. The bundled set is the whole set; product text never uses host fonts, and
   themes cannot inject font bytes (interview GF004; DDV-007-05; resources §2.3).
2. Portsmouth Rapids never becomes the body face (DDV-007-05). It is Latin-only;
   uncovered clusters fall back per cluster without restyling the control
   (interview GF004).
3. Text follows the united engine's shipped profile: light auto-hinting,
   grayscale coverage, one blending rule; no LCD text (§12.3; text-engine).

### 6.2 Roles

| Role | Face | Nominal size (logical px) | Source |
|---|---|---|---|
| title | control, bold | 20 | theme types |
| heading | control, bold | 14 | theme types |
| control label | control, regular, +0.24 px tracking | 12 (Plan Paint ribbon: 14) | theme types; Plan Paint |
| group caption | control, small, quiet, beneath its group under a thin divider | 8–10 | VISUAL §7; DNA-C04; Plan Paint |
| body/field | content | 12 (10–12 dense) | theme types; VISUAL §3.4 |
| metadata | content | 9–10 | theme types; VISUAL §3.5 |
| metric | content, tabular figures where available | 8–16 | VISUAL §3.4–3.5, §8 |
| instrument | monospace | 12 | theme types; a11y round §4.4 |

1. Controls ask for a role, never a family (text-engine; a11y round §4.4).
   Layout measures shaped runs, never character counts (a11y round §4.4 rule 3).
2. Hierarchy comes from role, modest size steps, two weights, alignment,
   placement, tracking and ink (DNA-V08). The tokens name weight 650 for
   headings, but the control face has regular and bold only (theme types).
3. Metro-style large type with reduced chrome is rejected on everyday surfaces
   (C151 R). Display identity type and ordinary type share a window only in
   separate zones (M006); large plain-language stage headings belong to first
   run (C083).
4. Dense roles sit at 8–13 logical px (a11y round §4.5; VISUAL §8). Sentence case
   for explanation, compact title case for commands, uppercase only for tiny
   structural labels **(proposed)** (DNA-V09). Descriptions are concise and
   factual, never promotional (M046).
5. Large text grows or reflows controls, then collapses low-priority content by
   declared priority (a11y round T5; DDV-007-02).
6. Labels are translated and bidi-isolated; application topology is not mirrored
   (DDV-007-15; DNA-A08).

### 6.3 Labelled commands and material icons

| Rule | Source |
|---|---|
| Commands stay text-labelled and spatially stable; a label stays on every toolbar object even when its icon is recognizable | V203, V201, V209; M083 |
| Icon-only tool strips and small icons floating on a near-flat plane are rejected | M064 R, M072 R; BRIEF |
| A toolbar may read as distinct labelled destination objects over plain utility machinery; colour-family division is distilled; sequences are not stacks of plates | M081, M085, M087; M086 D; M092 R |
| Large command icons appear only where recognition repays the space | DNA-C06 |
| High-colour material icons with perspective and identity are required; monochrome glyphs replacing them are an anti-model | V011; M011; C152 |
| Fluent Color supplies semantic coverage and an original House Material layer the visible objects; Fluent Emoji 3D and Meteocons are material references only; every asset is revision-pinned and MIT | ICONS; DNA-O03, DNA-A11 |
| An object has classic-iOS depth: upper-left light, local material gradients, occlusion, restrained cast shadow, specular edge | ICONS round 003; M101 |
| Icons are redrawn at 16, 24, 32 and 48 px, never scaled from one master; objects take a mild perspective, commands may be frontal **(proposed)** | DNA-O04; VISUAL §9; DNA-V10 |
| Rare kinds may have eccentric handcrafted icons; vector-like extreme silhouettes are rejected | V195; V078 R |
| Status uses tiny glyphs with real state, base plus overlay plus text, colour redundant **(proposed)**; no emblem stacks on object icons | V025; DNA-V11; V214 R |
| Monochrome glyphs remain for small command and status verbs, arrows and chevrons, never object identity | ICONS |
| The pointer is a high-contrast physical pointer, not a thin outline | M095 |
| Icons are PNG assets; stock control glyphs are vector | §4.5 |

## 7. Shape and geometry

### 7.1 Corner radii

Edges are unequivocal and nearly square, softened by colour rather than inflated
geometry (V064; V007 D). The house look softens plates by 2–3 logical px (owner
revision 3; Plan Paint tiles 3, gold plates 2, tabs 4).

| Step | Radius (logical px) | Typical parts | Source |
|---|---|---|---|
| R0 | 0 | window body, panels, panes, wells, selection fill | theme types; V064 |
| R1 | 2 | gold plate, editors, menu items, progress | theme types; Plan Paint |
| R2 | 3 | default control face, command tile | theme types; Plan Paint |
| R3 | 4–6 | tabs, modules, popups | Plan Paint tab; theme types (card 6) |
| R4 | 8–10 | capsules, special wells; selected contexts only | atlas; V220 |
| Pill | half the shorter side | floating capsule, round glyph buttons; loud themes | PlaySuite capsule |

1. Soft corners with a mild vertical gradient are the closest permissible
   approach to modern styling (M075); rounded internal tabs may sit in nearly
   square windows and panels (M052).
2. Large soft geometry is accepted in selected contexts (V220), not as default;
   oversized rounded cards and mobile topology are rejected (VISUAL §13).
3. A part's radius never exceeds its container's (proposed). The window corner
   takes 3–4 px optical softening, internal role edges staying crisp
   **(proposed)** (DNA-F05).

### 7.2 Seams, dividers and keylines

1. A seam is 3 logical px or thinner, with a wider hit target (STYLE; DNA-P02;
   theme types `splitter_width`, `splitter_hit_width`). It belongs to the sibling
   relationship and has one owner; no dead crack, no doubly owned point
   (REGIONS §3, §4 rule 4).
2. A panel-collapse actuator is a small flat grip on the seam at rest; pointer
   proximity or keyboard focus grows its size, contrast and depth before
   activation; it is discoverable without hover, and keyboard users get the same
   expanded state (DDV-007-04; DNA-P03).
3. Command-plane dividers are thin etched pairs fading at both ends, with the
   group label beneath (Plan Paint `paint_separator`; DNA-C05).
4. The window boundary is one thin dark keyline, navy in blue atmospheres, never
   nested (V004 D; DNA-F04). Keylines are 1 logical px unless a role says
   otherwise (material types).

### 7.3 Focus indicator

1. Focus is a distinct non-colour cue, independent of selection and hover: a
   precise dotted rectangle that leaves the object's material unchanged (M015;
   DNA-O07, DNA-O08).
2. Reference: 1 logical px dotted outline in the focus colour, inset 3 px within
   a selection rectangle, or 1–2 px dotted around a button (atlas; theme types
   `focus_width 1.0`).
3. A text field draws focus whenever it has focus; every other control only after
   keyboard use (§12.3).
4. It scales with text and display scale, never below 1 device pixel, at 3 : 1
   against both neighbours (DNA-A07; §5.7).
5. A theme restyles focus through the focus part (colour, radius, inset, dash,
   glow) but cannot remove it or make it hover-only (a11y round A6; PlaySuite
   `TOOLKIT_REQUESTS.md`).

### 7.4 Hit targets and density

| Token | Logical px | Source |
|---|---|---|
| control height compact / standard / large | 24 / 30 / 36 | theme types |
| navigation buttons and wells | 28 high | VISUAL §3.3 |
| touch target | 40 | theme types |
| pointer target in dense chrome | 20 × 20 **(proposed)** | |
| window minimum | 150 × 150 logical units | DDV-007-02; a11y round §3.4 |
| spacing ladder | 2, 4, 6, 8, 12, 16, 24 | theme types |

1. The engine defines each part's geometry and derives its hit region; a hit
   region may exceed its visual, never falls below these minimums, and
   interactive order belongs to the parent region (§12.3; REGIONS §2–3).
2. Density changes spacing and height, never anatomy; an atmosphere never
   changes density (theme types; PALETTE).
3. Shrinking is content-aware first: spacing, pane allocation, command-group
   composition and preview extent adapt while keeping relationships; then
   regions collapse by declared priority; controls never shrink below usable
   targets (DDV-007-02; DNA-C12).
4. Scroll bars stay visible and physical, crisp tinted thumbs in inset tracks
   rather than disappearing hints, without becoming prominent instruments (V052; V058 R).

## 8. Control anatomy and states

### 8.1 Parts

Richness fails to repeat when anatomy cannot be addressed compositionally
(SYSTEM_MODEL §1). Every control is a composed system of named parts, addressed
by stable role name rather than tree position (SYSTEM_MODEL §2; CONTROL_DNA §3;
owner direction; §12.1 item 1).

| Part | What it is | Examples |
|---|---|---|
| region | granted bounds; not painted | every control |
| shadow | contact or cast shadow, including backshadow | button, segment, popup |
| face | main material surface | button, tab, thumb, capsule |
| edge | outer boundary stroke and silhouette | every bounded part |
| keyline | inner highlight or lowlight on one edge | the lower specular line of the identity band |
| perimeter | second outer ring | default action (M105) |
| sheen | specular or reflective layer over a face | gloss band, cursor sheen |
| seam | parent-owned shared boundary | splitter, breadcrumb or tab join |
| divider, caption | etched group line; group label beneath | command-plane groups |
| cap | leading or trailing terminal shape | chevron tip, pill end |
| well | inset receptacle | text box, gallery, check box, groove, work pane |
| track, fill, thumb | ranged-value path, its value portion, its draggable actuator | scroll bar, slider, progress |
| actuator | attached mechanical plate | spinner and scroll arrows (M025) |
| indicator | binary or tri-state mark | check mark, radio dot, switch knob |
| activity | authored indeterminate-progress object | busy indicator (C164) |
| glyph, icon | vector symbol; image slot | arrows, close; command and object icons |
| label, content | text in the control role; text in the content role | caption, menu text; field value, list item |
| caret | insertion point | editors |
| disclosure | open/close affordance | menu arrow, tree expander |
| grip | seam or panel actuator | collapse grip, resize grip |
| chip | small flat tinted status cell | size, zoom (Plan Paint status strip) |
| focus, default, selection, validation, drop | state cues | §8.4 |

Sources: CONTROL_DNA §3.3–3.5; MICROFEATURE §6.3; theme types (`text`,
`muted_text`, `glyph`, `focus_ring`, `default_ring`); PlaySuite
`GUI_FORMS_CONTROLS_PROPOSAL.md` step 4; Plan Paint.

```text
button         face (+sheen), edge, perimeter?, icon?, label, disclosure?, focus
check box      well, indicator, label, focus
spin box       well, content, caret, actuator x 2, focus
scroll bar     track (well), thumb (face, edge), arrow actuators (button systems)
slider         track (well), fill, thumb (shadow, face, edge, sheen), focus
text box       well, content, caret, selection, validation, focus
progress       track (well), fill | activity, label (phase)
tab            face, edge, cap, seam (parent-owned), label, focus
command plane  face, groups of member button systems, divider, caption
breadcrumb     segment button systems x N with cap, seam and shadow; terminal operator
```

Composites reuse simpler systems: a chevron segment is a button in a
parent-owned overlap; a scroll arrow is a button (CONTROL_DNA §3.6, §4.1–4.2).

### 8.2 Cue propagation

1. A theme binds appearance to (part role, state), not (control, part); a cue set
   made once applies to every part with that role (owner direction;
   SYSTEM_MODEL §5).
2. **(proposed)** Resolution, most specific first: control instance, control
   family, part role, theme base, built-in theme; a refinement inherits what it
   does not restate.
3. Leading, middle, trailing and sole roles come from the parent's ordered
   membership, never coordinates (REGIONS §4 rule 3).

### 8.3 Participation

A window or board is in one of three participation states (DDV-007-01;
DNA-F03):

1. **Key**: owns keyboard focus.
2. **Secondary participating**: readable, usable and drop-capable; it recedes
   modestly, title chroma and shadow reduced between the quiet and strong
   candidates, and never looks disabled. Drag proximity restores destination
   emphasis without moving focus.
3. **Deactivated**: genuinely non-participating, with a stronger unavailable
   treatment.

Inactive selection loses focus force but stays visible (DNA-O07). Host
activation behaviour stays platform-owned (DDV-007-01).

### 8.4 State set

| Group | States | Source |
|---|---|---|
| interaction | normal, hot/proximate, pressed, captured | CONTROL_DNA §3.7; VISUAL §10 |
| focus, default | focused, focus-visible; default action | CONTROL_DNA §3.7; M105 |
| value | selected, checked, indeterminate, current, active tool | CONTROL_DNA §3.7; Plan Paint |
| availability | enabled, disabled, unavailable, read-only | CONTROL_DNA §3.7; DNA-C11; M104 |
| validity | invalid, pending/staged | theme types; VISUAL §10 |
| disclosure | open, closed, expanding, collapsing, interrupted | CONTROL_DNA §3.7 |
| drag | drop accepted, drop rejected | VISUAL §10 |
| participation | key, secondary participating, drag-proximate, deactivated | §8.3 |
| environment | host or local high contrast, reduced motion, display scale, text scale, density | CONTROL_DNA §3.7; DDV-007-13 |

1. **(proposed)** Surface precedence is disabled > pressed > hot > normal; focus,
   default, selection, validation and drop are independent layers (DNA-O07).
2. Hot, pressed, focused, disabled and default are mutually distinguishable in
   grayscale and keyboard-only use (DNA-C08; M104, M105).
3. A theme selects published states; it cannot invent a state visible to code or
   accessibility (CONTROL_DNA §3.7).
4. The code addresses seven surface states (normal, hot, pressed, pending,
   invalid, disabled, deactivated) plus selected, focused, defaulted and
   high-contrast flags (theme types); the rest is not yet reachable.

## 9. Motion, effects and sound

### 9.1 Character

1. Motion is reactive, direct, local and interruptible; the interface changes
   state promptly and settles (DNA-T08; DNA-10; C113; VISUAL §12). Windows 7's
   quieter ceremony is the ceiling (C111).
2. Spatial stage movement, guiding curves and paths, and cross-fade or scale
   transitions are rejected (C074 R, C082 R, C102 R); so is a pulsing default
   action (V116 R).
3. Motion never delays input, never hides the destination, and loses nothing
   when disabled (DNA-T08; DNA-A06). Every animation retargets from its current
   value when interrupted (VISUAL §12; REGIONS §9).

### 9.2 Timing and easing

| Token | Duration | Use | Source |
|---|---|---|---|
| instant | 0 ms onset | pressed state in the input's frame | (proposed); DNA-14 |
| quick | 90–100 ms | hover, proximity, seam grip, result expansion | theme types `quick 100`; VISUAL §12 |
| standard | 150–180 ms | capsule, pane disclosure | theme types `standard 180`; PlaySuite capsule |
| emphasized | 280 ms | larger owned transitions | theme types `emphasized 280` |
| busy cycle | 1400 ms | one period of an activity object | theme types `busy_cycle 1400` |
| ceiling | 400 ms | longest state transition | (proposed); C111, C113 |

| Easing | Shape | Use | Source |
|---|---|---|---|
| smoothstep | 3t² − 2t³ | opening and closing surfaces | PlaySuite `capsule.cpp` |
| exponential approach | fixed fraction of the gap per unit time (about 75 ms) | lifts and pops that retarget | PlaySuite `shelf.cpp` |
| linear | constant rate | size-tracking height and border changes | atlas |
| ease | ease-in-out | opacity, small transforms | atlas |
| overshoot | one overshoot of at most 8 %, then settle | engine capability for loud themes; unused by the default | (proposed); C113 |

### 9.3 Effects

1. **Pop.** A part rises one level and may grow its shadow and brighten on hover,
   selection or focus, within its granted overflow, never moving neighbours
   (owner direction; PlaySuite shelf; REGIONS §2).
2. **Sheen.** A specular band following the pointer within the part; it redraws
   only when the pointer moves, within its own extent (owner direction; §2.6).
3. **Glow.** A soft coloured emission bounded by the part's grant (owner
   direction; VISUAL §7); never the default-action cue (M105).
4. **Translucency, glass, reflection** follow §4.6.
5. Gloss, lift and rings are engine capabilities; no theme must carry them
   globally (§12.3). All effects render on the CPU and are bounded deliberately
   (interview GF002).

### 9.4 Ambient animation and idle

1. In the default theme only progress bars animate ambiently; nothing else
   animates at rest; themes may do more within the engine's budget (§12.3).
2. An ambient effect declares region, period and frame cap. **(proposed)** At
   most 30 frames per second, damage limited to the region, which on everyday
   surfaces is at most 10 % of the window; ceremony follows §10.1.
3. It stops when the window is occluded or minimized, under reduced motion, and
   when the application's motion switch is off (a11y round A6; PlaySuite Motion
   switch; occlusion rule proposed). Long-task animation stops completely once
   the task completes (C114 D).
4. When nothing animates, nothing is scheduled or painted; there is no perpetual
   redraw loop (AGENTS; master plan; consumer profile; VISUAL §12).

### 9.5 Reduced motion

The host's preference is authoritative; a theme may specialize it, not suppress
it (a11y round A6). Transitions become immediate end states with all
information kept (VISUAL §12; DNA-A06). **(proposed)** Ambient effects stop,
sheen rests static, pop changes elevation without motion, and an activity object
holds a static pose beside its stage text.

### 9.6 State machines and expressions

1. A theme may declare a presentational state machine per part, such as a press
   that overshoots and settles. Its states, transitions and timings live in the
   theme; the machine runs in the engine; its inputs are the published states of
   §8.4; it cannot create observable states or delay behaviour (§12.1 item 1;
   CONTROL_DNA §3.7).
2. **(proposed)** Expressions read published state, time since state entry,
   part-local pointer position while hot, display and text scale, density,
   accessibility flags and theme parameters; they write presentational values
   only, with no loops or recursion, a fixed operation budget, and identical
   pixels for identical inputs (resources §2.2).
3. New kinds of effect are engine primitives, never theme code (§12.1 item 1).

### 9.7 Sound

Interaction sound marks changes of state or location (panel collapse and
restore, location change, operation state, committed settings), not raw
activation; presses, menu opening, hover and focus movement are silent
(DDV-007-12; VISUAL §12). Sound is a scheme member (M034); whether it defaults
on is the application's choice (File Manager: on). Meaning is complete with
sound off (DDV-007-12).

## 10. Ceremonial grammar

Ceremony is software creating a temporary visual world around work:
installation, setup, first run, completion and rare long operations (ceremony
round). GUI.Forms supplies the materials, controls and motion vocabulary; the
application decides when a surface is ceremonial. The grammar has five strands:
geography, world, motion, instrumentation and ceremony; the everyday work
surface is its counterpart.

### 10.1 Geography, world and motion

| Strand | Rule | Source |
|---|---|---|
| Geography | Ceremony is bounded in place and time: breathing room, stronger illustration and creative-suite richness are legal in installation and rare ceremony, never on everyday work surfaces | C044, C054 D |
| Geography | Accepting an installer's fresco does not accept its wizard layout; a transition licenses no constant motion in ordinary navigation; a full-screen world never becomes a home page | ceremony round guardrails |
| Geography | A compact wizard stays legitimate: permanent illustrated side art and a small modal surface suit short bounded operations; a plain progress dialog is the ceiling for routine maintenance | C061, C063; C161 |
| Geography | Accepted branding is translated into the house style, never imitated | ceremony round guardrails |
| World | A serious installer may own the full visual field, with a panoramic fresco persisting while lower content changes and a large illustration region carrying no controls | C001, C071; C002; C032 |
| World | It reads as an engineering instrument, not consumer onboarding: friendly and optimistic without mascot, chatty prose or personified assistant | C011; C043, C084 |
| World | Deep blue-violet gloss may build a rare setup world even where ordinary chrome is lighter; blue-violet framing around pale technical panels is adapted; warm orange, amber and red may counterweight the cool base | C031; C003 D; C041 |
| World | High-saturation retro-futurist art coexists with plain, legible controls; a setup shell may suppress native appearance while keeping conventional behaviour | C062; C053 |
| World | Illustration may change by stage while command placement stays fixed; related capabilities may appear as a field of distinct product objects | C023; C042 |
| World | Watercolor-like ambient fields are adapted for exceptional empty or setup states; the pattern arsenal (§4.4) is legal here | V001 D |
| Motion | Ambient motion is preferred to navigational choreography: slow motion in the brand region, colour slow enough to read as atmosphere, text and controls arriving as stable islands | ceremonial synthesis; C033, C072, C073 |
| Motion | One ambient flourish is enough and the rest stays nearly static; the field may stay calm while one local progress element carries the activity | C112; C103 |
| Motion | Installation may have a friendly choreographed arrival that ordinary launch does not; between stages panels are replaced in place | C081; C074 R, C082 R, C102 R |
| Motion | Illustrative activity may run during long work and stops completely at completion | C004 D; C114 D |

Ceremonial surfaces may animate in this way while their work runs; the
progress-only ambient rule applies to the default theme's everyday surfaces
(§12.4).

### 10.2 Instrumentation: the progress grammar

The accepted combination (progress grammar; DNA-T05–T07):

1. **exact current phase or operation**, which matters more than an untrustworthy
   time estimate (C162);
2. **honest component or stage identity**: component names and current work stay
   visible in the decorative world (C005); a component list beats a large
   generic percentage (C012); a stable stage list marks the active stage
   (C022 D);
3. **an expandable technical trace**, the default view staying concise and never
   a scrolling log (C163, C024);
4. **a small bespoke activity object** where progress cannot be measured, never a
   circular spinner (C164); a generic quiet animation beside phase text is not
   enough (C013 R);
5. **an explicit completion state** that settles or lingers briefly rather than
   vanishing mechanically (C034; DNA-T07).

Known work shows amount, phase and component; unknown work shows the activity
object and exact stage, never a fake percentage (DNA-T05, DNA-T06). The progress
control's fill and activity parts carry this grammar and are the default
theme's only ambient animation (§8.1; §12.3).

### 10.3 Ceremony and everyday work surfaces

| Rule | Source |
|---|---|
| Confirmation, failure, licence, progress and completion share one stable geometric grammar; completion may receive its own ceremony | C014; C034 |
| A blocking failure uses a material modal naming the failing object or subsystem; a batch of conflicts uses a persistent surface for inspection without serial modal churn | DDV-007-07 (generalized); DNA-T04 |
| Success normally changes the affected object rather than announcing itself | DNA-06; DNA-T03 |
| Topic tabs do not organize start or setup surfaces | C141 R |
| A serious tool may carry a richly coloured identity band without styling every control that way | C121, C132, C144 |
| Structured information is presented natively, without web-page composition; news, RSS, promotional or editorial video and home-page abstraction are anti-models | C131, C123; C134, C154 |
| Recent-work history may be compact, explicit and dominant without becoming a home page, in explicit stable regions; illustrated regions serve as compact gateways to concrete work | C133; C122 D; C142 |

## 11. The default theme's character

The default is GUI.Forms' own, built from the interviews on the engine and "a
bit tweaked to look nicer" than its references (owner direction).

1. **Target.** An unplaceable, high-definition 1998–2007 information instrument:
   future-vaporwave retro at modern density and precision, "Microsoft Encarta
   '00 ultra HD deluxe premier edition" richness, vaporwave atmosphere,
   professional seriousness and clicky physical depth (PALETTE; BRIEF; VISUAL
   §1). Not pixel art, dark-theme software, candy vaporwave, generic modern web
   chrome, a clone of one historical system, or a Windows skin (VISUAL §1; owner
   revision 3).
2. **Register.** Bright optimism: Visual Studio blues, Office warmth as state,
   Watercolor fields (§2.4).
3. **Reference zoning.** House Composite in Sapphire Dusk (§4.7, §5.6): identity
   band and command planes above, a graphite chassis, side panels flat in
   neutral ivory stock, a recessed work pane, high-colour material objects, a
   flat pale status strip.
4. **Reference realization.** Plan Paint shows the look built on GUI.Forms
   (owner revision 3). Reference values, not tokens; tokens descend from the
   role model (§5.4) and are revisited after a native specimen sheet (§12.3):

   | Element | Plan Paint realization |
   |---|---|
   | tab rail | 27 px, solid pale blue `#9BB5D8`; selected tab face `#E8F2FF` runs into the plane without a closing edge |
   | File tab | solid blue gradient `#528BE0` to `#173F88`, white text |
   | command plane | pale blue gradient `#E8F2FF`, `#CADEFB`, `#A5C4F1`, `#769FDF`; white top line, shade line beneath |
   | command tile | `#D2E4FB` to `#A3C1EC`, radius 3, border `#6384B2` (hot `#355E96`), inner white top/left and `#315686` bottom/right lines, one-pixel contact shadow, ink `#153765` |
   | active tool | gold `#FFEDBA` to `#FFCF66`, border `#C49332`, radius 2 |
   | gallery well | white `#F8FBFF` to `#FFFFFF`, thin blue-grey border `#94ACC8`, faint inner top shade |
   | groups | labels `#485B70` beneath thin etched dividers fading at both ends |
   | status | flat pale strip with small soft chips |
   | type | Portsmouth Rapids throughout the chrome |
   | work pane | canvas recessed in a textured mid-grey felt surround; sheet shadow (1.5, 2.5) blur 6 |

5. **Identity.** As in Plan Paint: the solid blue File tab, the tab rail and
   the command-plane gradient carry identity; the default has no fresco band
   (§12.4). A Watercolor fresco (V003; three-stop gradient with broad blooms) stays
   available to themes and to ceremony.
6. **Atmosphere.** Sapphire Dusk roles: chrome ramp `#FAFDFF`, `#E7EDF6`,
   `#C9D4E5`, `#AEBBD1`; ink `#1D2D4B` on white paper; selection `#DBE6FF` with
   line `#436FBD`; focus `#294F91`; accent `#3E61A3`; coral signal `#DF6B65`
   (VISUAL §4; PALETTE).
7. **Unconstrained taste, as evidence.** Working unconstrained, the owner builds
   gloss buttons in chrome, gold, navy and smoke with a hard mid-face break and a
   top shine; a warm gold ring 2 px outside the face for hover and focus;
   procedural wood, felt and paper; a pill capsule opening with smoothstep
   easing; boxes that lift with a growing shadow when selected (PlaySuite
   `suite.cpp`, `capsule.cpp`, `shelf.cpp`). The default takes this character,
   not its specifics (§12.3).
8. **Reference study.** The default follows the published Pearl Ribbon study,
   moved toward Plan Paint's royale blue (§12.4): a near-white-to-pale-blue
   command plane, flat tinted plates, gold for the active tool, neutral ivory
   side planes, a recessed work pane. Tabs copy Plan Paint's tab stack
   (`make_tab_theme`): faces `#9BB6DA` at rest, `#ACC6E7` hot, `#E8F2FF`
   selected, with a top wash fading out by 72 % of the height; a white top line
   and a double right keyline (`#D1E0F5` inside `#496C9F`); every tab casts a
   hard 2 px `#496C9F` edge and a soft (3, 1) shadow to its right; the selected
   tab runs into the ribbon with no bottom edge. Tabs abut on a flat `#93ADD0`
   rail; the selected tab is slightly larger. Every ribbon button is a bulge in
   the ribbon cloth (§3.5 Bulge), and the command plane deepens from near-white
   to royale blue toward its foot.
9. **Beside it.** Windows-familiar themes ship too: Classic and a modern flat
   look first, a few more later (§12.3). Literal Windows rendering lives only
   there (owner revision 3).

## 12. Conflicts and open questions

### 12.1 Conflicts

1. **Authoring model.** The owner addresses parts by name, not a CSS tree;
   SYSTEM_MODEL §2 says an ideal theme is "nearly entirely" CSS over Web.Forms
   HTML. **Resolved 2026-10-12 (owner):** themes are written in a CSS-syntax
   subset addressed to the named anatomy (`::part()`, `:state()`, stable IDs),
   not a CSS document tree. Engine inputs (pointer position, published
   application values) are custom properties only the engine sets. A
   presentational state machine is part specification and part interaction:
   its states, transitions and timings are declared in the theme; the machine
   that runs them lives in the engine. New kinds of effect are added as C++
   engine primitives, never as theme code.
2. **Code.** The code's only and default theme is `windows-professional`, and
   it hand-authors high-contrast arrays per role (theme types); the owner wants
   GUI.Forms' own default and derived variants. Plan Paint builds its look as
   overrides of `windows-professional` (Plan Paint `ribbon.cpp`).
3. **Location.** The `planning/interview/` records exist on branch
   `claude/style-summary-and-interview-ledger`, not yet on `main`.
4. **Work-pane depth (owner revision 2 over the depth map).** The depth map
   gives content a "calm plane" and the first draft made paper flush. The work
   pane is now a recessed inset plane and, with entry fields, the deepest locus;
   side panels are not raised. Content inside the pane stays calm.
5. **Warm stock (owner revision 1 over STYLE and VISUAL).** House Composite's
   Studio 2003 zone names "warm-neutral caption stock" (STYLE; DNA-P04,
   DNA-P09) and the reference fresco carries an apricot bloom (VISUAL §7).
   Owner revision 1 wins: neutral ivory stock, no apricot beside blue chrome.
   Warmth as state, signal and rare ceremony survives (C041, V014; §5.5); the
   coral endpoint is asked in Q3.
6. **Ceremonial ambient motion (verdicts beside a 2026-10-12 decision).** C033,
   C072, C101, C112 and C004 D accept slow ambient life over a setup world; the
   owner decision keeps the default theme's ambient animation to progress bars.
   The decision stands; Q2 asks whether a ceremonial surface counts as default
   chrome.
7. **Dark variant (verdicts beside a 2026-10-12 decision).** The verdicts keep
   dark primary surfaces outside the direction (M071 synthesis; C091 R; C051 R;
   V051 R; VISUAL §13 "dark body fields"), and the frontend brief left a dark
   atmosphere open (BRIEF VIS-FE-012). The decision stands; its reference is the
   owner's own phase-diamond pair B (CRM §2), so it extends the colour model.
   Q4 asks its reach.
8. **Modern flat theme (verdicts beside a 2026-10-12 decision).** Classic is
   well supported (M101–M105, M102). A modern flat look meets anti-models: wide
   flat colour fields (C153), monochrome glyphs for icons (C152), icon-only
   strips (M064 R), native neutrality (V215 R). The decision stands; the theme
   must still keep M076 (§2.2 rule 3). Q6 asks its icons.
9. **Reconciled in place.** These tensions between verdict rounds, and between
   verdicts and owner revision 3, are resolved in the sections named:

   | Tension | Resolution |
   |---|---|
   | M123's cue calls literal room imagery "too representational" (accepted); the material synthesis accepts Maze "including literal room/corridor imagery" | §4.4 follows the synthesis, confined to ceremony vignettes |
   | M101–M105 accepted in full; owner revision 3 rejects literal Windows rendering | doctrine kept as principle; the two-step edge (M103) survives in Plan Paint's tinted, soft form; literal rendering lives in Classic (§3.1) |
   | V064 square corners; owner revision 3 points to 2–3 px plates; M075, M052 admit soft corners | containers square, plates softened (§7.1) |
   | M014, V061 D give colour to objects; V202 R, V204 R, V217 R reject plain grey chrome | chrome tinted and shaded, never competing with objects (§5.5 rule 2) |
   | V052 accepts crisp raised scroll tracks; V058 R rejects prominent instrument scroll bars | visible and physical but quiet (§7.4 rule 4) |
   | DNA-F06 keeps cast shadows for real layers; MICROFEATURE §6 needs a breadcrumb backshadow; PlaySuite buttons cast shadows | controls carry no shadows (owner revision 4); a breadcrumb backshadow is a real layer edge, not a control shadow (M051; §3.4) |

### 12.2 Questions for the owner

None open. New questions are added here as they arise.

### 12.3 Owner decisions, 2026-10-12

These supersede the conflicting sources and options of the first draft.

- **Bitmaps.** Image atlases and PNG are allowed for client
  and theme assets, for example a custom tiling background. They are never
  used for control faces such as buttons; controls stay procedural.
- **LCD text.** Resolved by the united HarfBuzz/FreeType text
  engine: its shipped profile (light auto-hinting, grayscale coverage, one
  blending rule; `docs/user/text-engine.md`) is the default and is
  authoritative. LCD subpixel rendering stays excluded; the earlier
  accessibility-round admission is superseded.
- **Body face.** Carlito.
- **Contrast gate.** The proposed gate (§5.7) is accepted.
- **PlaySuite.** The default theme takes PlaySuite's character, not its
  specifics. Its effects (gloss, lift, rings) are capability requirements on
  the engine, which must support them; no theme has to carry them globally.
- **Reshaping.** Geometry belongs to the engine and the control
  anatomy, not to the theme, so the engine also defines each part's hit
  region. A theme chooses among the shape parameters the engine provides; the
  engine derives the hit region from the geometry it computes. Themes never
  set hit regions directly.
- **Windows-familiar themes.** Classic and a modern flat look first; a
  few more may be added.
- **Flat commands.** Commands in a command strip may be flat at
  rest, as Plan Paint's ribbon is, provided the strip carries the structure:
  dividers between groups and shading on the strip. A faint button that needs
  some presence at rest gets quiet relief through shading, not a full face.
  Hover and press bring full relief.
- **No raised buttons (owner revision 4).** Plan Paint has no raised buttons:
  nothing is lifted by a Windows-style bevel. Every button, command tile, chip
  and actuator is a slight 3D pill drawn as Plan Paint draws it (§3.1
  Rendering; clarified in §12.4). Hover brightens it and press darkens it and
  inverts its inner shading. Depth comes from recess only: entry fields and
  the work pane. Raised buttons survive only in the Windows-familiar Classic
  theme.
- **Focus.** A text field draws its focus ring whenever it has focus,
  however focus arrived. Every other control draws its focus ring only after
  keyboard use.
- **Ambient animation.** In the default theme only progress bars
  animate ambiently; nothing else animates at rest. Themes may do more within
  the engine's budget.
- **Dark variant.** The default ships a dark variant, derived by rule
  from the light theme (the light stays upper left, inset stays inset), that
  follows the host's dark preference and can be overridden by the user. It
  makes use of complementary colours; the reference is Plan Paint's
  "Phosphor" pair, a soothing black `#000000` with soft green `#82b361`,
  accented by its complement `#b36182` (Plan Paint `src/color_palette.cpp`).
- **High contrast.** Two authorities, as in §5.8 rule 2: under the host's
  high-contrast mode the host palette wins; GUI.Forms' own high-contrast mode
  is a separate, near-colourless rebuild.
- **Starting tokens.** The proposed numbers are accepted as starting
  tokens and revisited once the engine renders a native specimen sheet.

### 12.4 Owner decisions, second round, 2026-10-12

- **Default reference.** Pearl Ribbon, moved a little toward Plan Paint's
  royale blue. Every ribbon button gets the faint pill or bubble effect: a
  bulge in the ribbon cloth, as if a fingertip were pushed into it from behind
  (§3.5 Bulge). It has no edge of any kind; an edge fails it. Tabs emulate Plan
  Paint's tab shadowing exactly (§11 item 8). Push buttons keep Plan Paint's
  plate (§3.1 Rendering).
- **Dark variant.** The derived dark form drawn in the studies (Phosphor pair:
  black with green `#82B361`, complement `#B36182` for the File tab and active
  tool) is accepted.
- **Icons in the modern flat theme (Q6).** High-colour material icons, as in
  every theme; a monochrome glyph set is too trivial and lacks character (C152
  holds everywhere).
- **Ceremonial motion.** The ceremonial verdicts concern progress and
  animations that show activity going on; animation is allowed. Rare
  ceremonial surfaces (installation, first run, long tasks) may carry slow
  ambient life and an activity object while work runs, within the motion rules
  of §9 and stopping at completion (C033, C072, C101, C112, C114 D). The
  progress-only rule of §12.3 governs everyday surfaces of the default theme.
- **Identity.** As in Plan Paint: no fresco band in the default; the File tab,
  tab rail and command plane carry identity.
- **Dark variant.** It reaches whole windows.
- **Office palette.** It is only the origin of the MS Paint-style default colour
  swatches, not a seed for themes; nothing is pinned.

### 12.5 Owner decisions, third round, 2026-10-12

- **Ribbon button rendering.** The smooth cloth bulge is accepted as an
  alternative to flat ribbon buttons. The owner then asked for it to be
  replaced with dithering: the same bulge subsampled as a lattice of dots
  (§3.5 Dithered bulge), which the published studies now draw. Flat, smooth
  bulge and dithered bulge are engine-level renderings of one part role; which
  one the default theme ships is the owner's call.

## Appendix A. Anti-models

Rejected for the default and, unless a row says otherwise, every shipped house
theme.

| Anti-model | Source |
|---|---|
| deep heavy Motif bevelling | V077 R |
| literal Windows 95/XP rendering as the house look: grey two-step bevels everywhere, sunken status cells, classic dialog buttons | owner revision 3 (Classic excepted) |
| flat modern web buttons with no physical role; wide flat colour fields | VISUAL §13; C153 |
| Metro typography: large type with reduced chrome | C151 R |
| monochrome glyphs replacing material icons; icon-only strips; small icons on a near-flat plane | C152; M064 R, M072 R |
| neutral grey selection | M065 R |
| browser-blue underlined hyperlinks | M043 R, M005 R |
| gradient category bars; gradient or luminous collapsible headers | V021 R, V016 R, V211 R |
| nested hairline borders; alternating rows; web-table feeling | V022 R, V023 R, V033 R |
| cards around ordinary content; objects floating weightless on bare white | VISUAL §13; M012 R |
| large calm margins; dead-space inflation | M055 R, V212 R |
| plain grey chrome; undecorated utility; native visual neutrality | V202 R, V217 R, V215 R |
| sparing pastel structure; accents scattered on neutral machinery | V219 R, V076 R, V085 R |
| warm caption stock or apricot paired with blue chrome | owner revision 1 |
| dark or translucent side structure; dark body fields; black-white-grey structural base | V101 R; VISUAL §13; V051 R |
| editorial black/white/red; portfolio montage; Longhorn darkness | C051 R, C052 R, C091 R |
| translucency as a constant effect; glass or gradient everywhere | C104; VISUAL §13 |
| spatial stage transitions; guiding curves; cross-fades and scale changes | C074 R, C082 R, C102 R |
| pulsing default action | V116 R |
| generic quiet animation as the only progress cue; circular spinners | C013 R, C164 |
| web-page composition; news and RSS; promotional video; home-page abstraction | C123, C134, C154 |
| topic tabs on start or setup surfaces | C141 R |
| mascots and personified assistants | C043, C084 |
| purple structural rail; cyan branded modal plate; stacked command plates; local rainbow materials | M082 R, M084 R, M092 R, M094 R |
| emblems layered on object icons; vector-like extreme silhouettes | V214 R, V078 R |
| folder-like title tabs; very narrow title bars; large activity regions above content | V066 R, V062 R, V102 R |
| one raster icon scaled to every size; retro pixelation; arbitrary host fonts | VISUAL §13; DNA-O04 |
| decoration that outranks selection, focus, failure or object identity | VISUAL §13 |
