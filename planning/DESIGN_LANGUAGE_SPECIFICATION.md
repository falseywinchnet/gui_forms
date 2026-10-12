# GUI.Forms design-language specification

Date: 2026-10-12. Status: first draft for owner review.

This document states how GUI.Forms controls look and move, in artistic,
thematic and effect terms. It is not a programming interface, source grammar,
or theme file format. Every rule names its source. A number the sources do not
supply is marked **(proposed)** and is not approved.

## 0. Citation key

| Citation | Source |
|---|---|
| (owner direction) | owner direction relayed with this drafting request, 2026-10-12 |
| (interview GFnnn) | `planning/interview/GUI_FORMS_INTERVIEW_LEDGER.md` |
| (resources §n) | `planning/interview/GUI_FORMS_RESOURCES_AND_CONFIGURATION.md` |
| (a11y round Xn / §n) | `planning/interview/GUI_FORMS_ACCESSIBILITY_AND_TEXT_ROUND_001.md` |
| (SYSTEM_MODEL §n) | `planning/DESIGN_LANGUAGE_SYSTEM_MODEL.md` |
| (CONTROL_DNA §n) | `planning/CONTROL_DNA_AND_STYLE_ADDRESSABILITY.md` |
| (REGIONS §n) | `planning/AUTHORITATIVE_CONTROL_REGIONS.md` |
| (MICROFEATURE §n) | `planning/MICROFEATURE_CONSERVATION_PROTOCOL.md` |
| (VISUAL §n) | `file_manager_demoboard/VISUAL_CONSTRUCTION_SPECIFICATION.md` |
| (atlas DNA-xnn) | design decisions inside `file_manager_demoboard/reference/frontend-concept-atlas-reference.html` |
| (atlas) | that file's stylesheet values and the reference screenshots |
| (theme types) | `include/gui_forms/theme/types/theme_types.hpp`, `src/core/theme/theme/theme.cpp` |
| (material types) | `include/gui_forms/surface_material/` |
| (text-engine), (fonts) | `docs/user/text-engine.md`, `assets/fonts/ATTRIBUTION.md` |
| (AGENTS), (master plan), (consumer profile) | `AGENTS.md`, `planning/MASTER_IMPLEMENTATION_PLAN.md`, `planning/FUTURE_APPLICATION_CONSUMER_PROFILE.md` |
| (PlaySuite file) | PlaySuite (`falseywinchnet/games`): evidence of the owner's unconstrained taste, not a rule |

Disagreements between sources are noted in place and collected in §12.

## 1. Purpose and scope

GUI.Forms has one theme engine. It works on articulated controls: every control
is a set of named, parameterized parts with states (owner direction;
CONTROL_DNA §3). This specification defines the vocabulary that engine speaks,
the limits no theme crosses, and the character of the default theme.

The default theme is GUI.Forms' own. It is built from the owner's interviews and
expressed as theme data on the engine, never as hand-written paint code (owner
direction; SYSTEM_MODEL §6 condition 1).

Out of scope: application content, document colour, game art and custom
drawing, where raw colours belong (interview GF018); the theme authoring
syntax, which GUI.Forms now owns (Web.Forms is no longer maintained; owner
direction, 2026-10-12); host window chrome.
Window-level transparency, blur and shape need the host window manager, and
GUI.Forms only reports and uses that capability (owner direction; AGENTS hard
boundaries).

### 1.1 What a theme is

A theme is immutable, data-only presentation: "more than colours, less than
code" (owner direction).

- **It may declare** colour pairs; material stacks of layers (solid, gradient,
  glow, sheen, transparency, reflection, glass); edges, keylines and shadows;
  corner geometry; typography-role selection among bundled faces; motion
  timings and easing; per-part presentational state machines; and bounded
  expressions (owner direction; resources §2.1; material types).
- **It may not declare** controls, input behaviour, event handlers, commands,
  semantic roles, filesystem or network access, executable code, or font bytes
  (resources §2.1, §2.3; CONTROL_DNA §7 gate 5; interview GF018–GF019).
- The engine is expressive enough for loud skins in the manner of early-2000s
  media and audio players, a title-bar sheen that follows the cursor, and
  buttons that pop. Historical themes such as royale or watercolour looks are
  capability references; GUI.Forms does not ship them (owner direction).
- A theme may be partial. Anything it omits falls back to the built-in theme,
  which is always present (resources §1, §2.1).

## 2. Principles

### 2.1 Affordance through depth

1. A control that carries meaning or affordance has visible relief: it reads as
   raised, inset or seated before it is touched (owner direction; VISUAL §1
   "classically deep rather than flat"; VISUAL §13 rejects "flat modern web
   buttons with no physical role").
2. Depth explains physical role; colour explains semantic state. Neither stands
   in for the other (VISUAL §2).
3. Material explains behaviour: a raised face presses, a well accepts content, a
   seam drags, a floating surface overlays (atlas DNA-04, DNA-P03).
4. Losing a small relief feature is a regression even when colour, place and
   function survive: the breadcrumb without its backshadow stopped reading as a
   button (MICROFEATURE §1, §6.1).

### 2.2 Where flatness is allowed

1. Flat chrome is allowed only where content must hold attention, such as a
   paint program's tools around its canvas (owner direction).
2. A member of a bounded command plane (ribbon, toolbar) may be flat at rest if
   the plane carries relief and the member gains depth on hover, proximity or
   keyboard focus *before* activation (VISUAL §7 ribbon; atlas DNA-P03).
3. A standalone control is never flat: text fields, check boxes, scroll bars,
   sliders and standalone buttons keep relief in every shipped theme outside
   rule 1 (owner direction).
4. Content fields are calm paper, without structural texture, cards or wallpaper
   (VISUAL §7 object field; atlas DNA-O01).

### 2.3 Restraint and richness

1. Richness is geographically budgeted: rich material sits in bounded regions
   (identity band, chassis, command planes), never everywhere (atlas DNA-08;
   VISUAL §13 rejects glass or gradient "applied uniformly everywhere").
2. Decoration never outranks selection, focus, failure or object identity
   (VISUAL §13).
3. Structure is honest: one line or material change per true grouping boundary,
   no nested decorative borders (atlas DNA-03, DNA-F04).
4. Dense is not cramped (atlas DNA-09).
5. Performance is part of appearance. No decoration delays first useful content,
   input response or damage repair; under pressure ornament degrades before
   meaning (atlas DNA-14, DNA-A12; interview GF002).

### 2.4 Consistency of light

1. One fixed light governs a window. Every highlight, lowlight, shadow, sheen
   and specular edge agrees with it (VISUAL §2 "upper-left light"; PlaySuite
   paper engine: "the light is fixed", relief "lit from the upper left").
2. Light never follows the pointer, time or animation. A cursor-following sheen
   is a reflection on a surface (§9.4), not a moving light (proposed).

### 2.5 Familiar behaviour, authored appearance

Behaviour stays conventional and Forms-like; appearance is authored and need
not match any host system (atlas DNA-12; interview GF009–GF011).

## 3. Light and depth model

### 3.1 Light

| Property | Rule | Source |
|---|---|---|
| Key light | from above and to the left; azimuth 135° from +x toward the upper left | VISUAL §2; angle (proposed) |
| Cast shadow | falls downward; horizontal offset at most half the vertical, away from the light | atlas `0 2px`, `1px 2px`, `4px 5px`; ratio (proposed) |
| Top edge | 1 logical px white or near-white highlight keyline | atlas `inset 0 1px #fff` on almost every raised surface |
| Bottom/right edge | lowlight line or contact shadow | atlas `1px 1px 0 #77899a`; PlaySuite gloss |
| Inset well | inner top-left shade, outer bottom highlight | atlas `inset 1px 1px 2px …, 0 1px #fff8` |

### 3.2 Elevation levels

Shadow strength follows real overlap: plane < popup < floating window, and
internal regions use edges rather than cast shadows (atlas DNA-F06). Controls on
a plane therefore carry only contact shadows (E1–E2).

| Level | Meaning | Reference treatment (atlas) |
|---|---|---|
| E0 flush | paper, pane bodies, transparent commands at rest | none |
| E1 seated | resting raised control | top highlight plus `0 1px 1px` at 13–25 % alpha; PlaySuite `0 2px`, blur 2, about 27 % |
| E2 lifted | hot or popped control; module or card on a plane | top highlight plus `0 2px 3px` at 15–27 % |
| E3 floating | expanded result, floating capsule, completion list | `0 3px 8px` to `0 5px 10px` at 20–33 % |
| E4 overlay | popup, menu, drawer, path matrix | `0 13px 32px` about 33 % plus `0 2px 5px` about 40 % |
| E5 window | separate floating window | `0 16px 34px` about 40 % plus `0 2px 5px` about 53 %, 1 px light inner rim |

1. Hover or pop raises a part by at most one level; press returns it to E1 or
   below (VISUAL §7 ribbon). The per-level values become role tokens; their
   exact numbers are (proposed).
2. A shadow stays inside the visual overflow the parent region grants
   (REGIONS §2).

### 3.3 Relief types

| Relief | Construction | Typical parts | Source |
|---|---|---|---|
| Raised | lighter top gradient, top highlight keyline, edge stroke, contact shadow | button face, thumb, capsule | atlas; PlaySuite gloss |
| Inset | flat or darker-top face, inner top-left shade, outer bottom highlight | text well, check box, track, excerpt | atlas `.mini-input`, `.criterion-field` |
| Pressed | gradient reversed or flattened, contact shadow removed, content offset 1 logical px down-right | button, command, segment | theme types `pressed_content_offset {1,1}`; PlaySuite gloss; VISUAL §7 |
| Etched | paired dark and light 1 px lines | group rules, separators | VISUAL §7 ribbon; atlas `.command-group` |
| Seam | thin visible boundary owned by a sibling relationship, wider hit area | splitter, breadcrumb and tab joins | VISUAL §3.2, §7; REGIONS §4 |
| Embossed | small raised low-colour capsule in content | coarse size badge | VISUAL §3.4 |

Press is local: it moves the pressed part's content and relief and never moves
surrounding content or layout (VISUAL §7 ribbon).

### 3.4 Depth under scale

1. Display scale and text scale are independent inputs (a11y round §3.4).
2. Text and targets scale fully; hairlines, shadows and texture scale optically,
   so hairlines do not become rails and focus cues do not collapse (atlas
   DNA-A07).
3. Edges snap to the device grid independently of text (a11y round §4.4 rule 5).
4. **(proposed)** Keyline device width = max(1, round(logical width × min(display
   scale, 2))).
5. **(proposed)** Shadow offset and blur scale with display scale, not text scale;
   a control that grows for large text keeps its elevation.
6. **(proposed)** Procedural texture is regenerated per display scale, not
   resampled (PlaySuite wood is remade when the window changes display).

## 4. Materials

### 4.1 The two planes

1. The **style backplane** holds fills, frescoes, procedural patterns, textures,
   illustrations and wells; it may be translucent. The **control plane** above
   it owns hit testing, focus, state, events, text and control relief (interview
   GF018–GF019).
2. Backplane objects cannot replace a control, intercept its events, change its
   role or impersonate plugin UI (interview GF018–GF019).
3. The planes have separate damage and cache generations: a button change does
   not re-rasterize a fresco, and a background change does not rebuild the
   control tree (interview GF018–GF019).
4. In-window glass, translucency and reflection sample the backplane, never other
   controls (proposed, following rules 2–3).

### 4.2 Material stack

A material is an ordered stack: up to 8 fill layers (solid, linear or radial
gradient, or image stretched, tiled or nine-patch, each with opacity and
coordinate space); up to 4 outer or inset shadows; one border or per-edge
borders; up to 8 single-edge keylines with colour, width and inset; and one
corner radius (material types). The engine adds glow, sheen, reflection and glass
layers (owner direction). Every layer kind has a bounded cost and a declared
damage extent (atlas DNA-A12; interview GF002).

### 4.3 Material families

| Family | Character | Where | Source |
|---|---|---|---|
| Paper | white or near-white, quiet | content, documents, object field | VISUAL §7; PlaySuite paper |
| Pearl | white to cool chrome, shallow gloss | command planes, status rail, headings | VISUAL §2, §7 |
| Gloss | four-stop vertical gradient, hard break near mid-face (stops 0, .48, .52, 1); 1 px shine under a flat top fading before the corners; none on pills | buttons and capsules in loud themes | PlaySuite `suite.cpp` |
| Graphite | middle-value grey, never near-black; directional texture below 3 % | chassis and seams, never behind text | VISUAL §7; atlas DNA-V04 |
| Fresco | bounded gradient with radial glows and a lower specular keyline | identity band | VISUAL §7 title; atlas DNA-F02 |
| Glass well | small translucent jewel with rim highlight | app mark, identity wells | VISUAL §7 title |
| Natural | wood grain, felt pile, paper fibre, generated from parameters | backplane | PlaySuite `wood.hpp`, `shared/felt`, `shared/paper` |

### 4.4 Procedural before bitmap

1. Control faces, edges, glyphs and effects are procedural or vector (owner
   direction).
2. PNG is the only admitted image format and serves icons, object images,
   backplane frescoes and application assets (resources §1, §4; VISUAL §9;
   interview GF003).
3. Icons are drawn per visible size, never scaled from one raster (VISUAL §9;
   atlas DNA-O03).
4. Procedural materials are built-in generators; a theme supplies parameters,
   never generator code (resources §2.1).
5. **(proposed)** Expensive generators run once per scale off the input path and
   are cached; until ready, the region shows the material's solid pair
   (PlaySuite felt costs about one second per tile).
6. Bitmap nine-patch faces for third-party skins are open (§12, Q3).

### 4.5 Construction and atmosphere

A theme is a construction family (materials, relief, geometry) crossed with an
atmosphere (colour pairs). Changing either keeps control identity, commands,
content, semantic state, selection and focus (VISUAL §5–6).

## 5. Colour

### 5.1 Relational colour pairs

Ordinary control styles consume named foreground/background pair roles; raw
colours are for custom drawing and the backplane (interview GF018). A pair is
the unit of contrast. Minimum role set **(proposed)**, generalized from VISUAL
§4 and theme types:

| Group | Roles |
|---|---|
| Surfaces | ink/paper, muted ink/paper, ink/soft paper, ink/chrome, ink/chrome low |
| Relief ramp | chrome highlight, chrome, chrome middle, chrome low; ordinary line, dark line, shadow |
| Interaction | focus on any surface; selection ink/selection with selection line; on-accent/accent |
| Signal | signal/paper; success/paper; pending ink/tint; invalid ink/tint |
| Availability | disabled ink on chrome and on paper |
| Links | link and visited-link ink on paper |

### 5.2 Contrast

The sources require each pair to meet "the product's contrast gate" in its
actual state but give no number (VISUAL §4). Proposed gate:

| Pair | Minimum |
|---|---|
| body, label and value text | 4.5 : 1 **(proposed)** |
| text ≥ 14 logical px bold or ≥ 18 regular | 3 : 1 **(proposed)** |
| control boundaries and state cues; focus against both neighbours | 3 : 1 **(proposed)** |
| disabled text, plus a non-colour difference from enabled (atlas DNA-C08) | 3 : 1 **(proposed)** |
| all text in local high contrast | 7 : 1 **(proposed)** |

Contrast is measured where the text actually sits: on the gradient stop beneath
it, through translucency, and with selection applied (VISUAL §4).

### 5.3 Derived variants

1. High contrast replaces decorative material; it does not intensify the same
   gradients (VISUAL §4).
2. Two authorities: when the host requests high contrast the theme yields almost
   entirely to the host palette; a locally requested mode is a nearly colourless
   semantic reconstruction of the house system (atlas DNA-A05).
3. Variants are derived from the role model, not hand-forked (owner direction).
   **(proposed)** derivation: high contrast keeps geometry, anatomy and state
   composition, drops gradients, texture, glow, sheen, reflection, glass and cast
   shadows, and draws every edge solid; dark keeps each pair's hue and relational
   contrast, inverts surface lightness in a perceptual space, and keeps the light
   above, so highlights dim, shadows deepen, and inset stays inset.
4. A theme may refine a derived variant; it may not suppress a host
   accommodation (a11y round A6).
5. The code holds hand-authored high-contrast arrays for every role (theme
   types); see §12 conflict 4.

### 5.4 Colour never carries meaning alone

State stays evident in grayscale, without hover or motion, and with sound off
(a11y round §3.4; atlas DNA-C08, DNA-O07; VISUAL §10).

## 6. Typography

| Face | Role | Source |
|---|---|---|
| Portsmouth Rapids regular and bold, the owner's own face (MIT) | control: titles, buttons, labels, menus, captions | interview GF004; fonts; text-engine |
| Carlito, four styles | content: editable text, lists, fields, properties | text-engine; resources §2.3 (lead specimen) |
| Cousine, four styles | monospace: path editor, instrument text | text-engine |
| Noto Sans CJK JP, Arabic, Hebrew, Devanagari, Bengali, Gurmukhi; monochrome Noto Emoji | per-cluster fallback only | text-engine; fonts |

1. Product text never uses host fonts; the bundled set is the whole set
   (interview GF004; a11y round owner revision; text-engine).
2. Portsmouth is Latin-only and remains a face, not a symbol grammar; uncovered
   clusters fall back per cluster without restyling the control (interview
   GF004).
3. Themes select among installed roles; they cannot inject font bytes
   (resources §2.1, §2.3).
4. Text is grayscale, lightly hinted, and blended identically on every host
   (text-engine; see §12 conflict 6).

| Role | Face | Nominal size (logical px) | Source |
|---|---|---|---|
| title | control, bold | 20 | theme types |
| heading | control, bold | 14 | theme types |
| control label | control, regular, +0.24 px tracking | 12 | theme types |
| group caption | control, small, quiet, beneath controls | 8–10 | VISUAL §7, §8 |
| body/field | content | 12 (10–12 dense) | theme types; VISUAL §3.4 |
| metadata | content | 9–10 | theme types; VISUAL §3.5 |
| metric | content, tabular figures where available | 8–16 | VISUAL §3.4–3.5, §8 |
| instrument | monospace | 12 | theme types; a11y round §4.4 |

5. Controls ask for a role, never a family (text-engine; a11y round §4.4 rule 1).
6. Hierarchy comes from role, size, two weights, tracking and ink. The tokens
   name weight 650 for headings, but the control face has regular and bold only
   (theme types; fonts).
7. Dense desktop roles sit at 8–13 logical px (a11y round §4.5; VISUAL §8).
8. Layout measures shaped runs, never character counts (a11y round §4.4 rule 3).
9. Large text grows or reflows controls, then collapses low-priority content by
   an accessibility-specific order; essential names and actions never clip
   silently (a11y round T5; VISUAL §11).
10. Labels are translated and bidi-isolated, but the interface is never mirrored
    (atlas DNA-A08).

## 7. Shape and geometry

### 7.1 Corner radii

| Step | Radius (logical px) | Typical parts | Source |
|---|---|---|---|
| R0 | 0 | window body, panels, panes, selection fill | theme types |
| R1 | 2 | command buttons, editors, menu items, progress | theme types; atlas (most common) |
| R2 | 3 | default control face | theme types |
| R3 | 4–6 | cards, modules, popups | theme types (card 6); atlas |
| R4 | 8–10 | capsules, special wells | atlas |
| Pill | half the shorter side | floating capsule, round glyph buttons | PlaySuite capsule (radius 15, 30 px buttons) |

A part's radius never exceeds its container's (proposed). Oversized rounded cards
and mobile topology are rejected for the default theme (VISUAL §13).

### 7.2 Seams and keylines

1. A seam is 3 logical px visible with at least 9 px of hit target (VISUAL §3.2;
   theme types `splitter_width`, `splitter_hit_width`).
2. A seam belongs to the sibling relationship and has one owner; an overlap has
   no dead crack and no doubly owned point (REGIONS §3, §4 rule 4).
3. The window boundary is one dark crisp keyline; frame lines are never nested
   (atlas DNA-F04).
4. Keylines are 1 logical px unless a role says otherwise (atlas; material types).

### 7.3 Focus indicator

1. Focus is a distinct non-colour cue, independent of selection and hover (atlas
   DNA-O07; VISUAL §3.4).
2. Reference: 1 logical px dotted outline in the focus colour, inset 3 px within
   an object's selection rectangle, or 1–2 px dotted around a button (atlas
   `outline:1px dotted`, `outline-offset:-3px`; theme types `focus_width 1.0`).
3. It scales with text and display scale and never drops below 1 device pixel
   (atlas DNA-A07); its contrast is at least 3 : 1 against both neighbours
   (proposed).
4. A theme restyles focus through the focus part (colour, radius, inset, dash,
   glow) but cannot remove it or make it hover-only (a11y round A6; PlaySuite
   `TOOLKIT_REQUESTS.md` asks for exactly this recipe).
5. Whether focus shows on pointer focus or only after keyboard use is open (§12,
   Q6).

### 7.4 Hit targets and density

| Token | Logical px | Source |
|---|---|---|
| control height compact / standard / large | 24 / 30 / 36 | theme types |
| navigation buttons and wells | 28 high | VISUAL §3.3 |
| touch target | 40 | theme types |
| pointer target in dense chrome | 20 × 20 **(proposed)** | |
| window minimum | 150 × 150 | a11y round §3.4; VISUAL §11 |
| spacing ladder | 2, 4, 6, 8, 12, 16, 24 | theme types |

1. A hit region may exceed its visual (VISUAL §7; atlas DNA-P03). A theme may
   change silhouette, but the hit region never drops below these minimums and
   interactive order belongs to the parent region (REGIONS §2–3; extent of theme
   control open, §12 Q7).
2. Density (compact, standard, large) changes spacing and height, never anatomy
   (theme types; proposed for the engine).

## 8. Control anatomy vocabulary

### 8.1 Why

Richness fails to repeat when a control's internal anatomy cannot be addressed
compositionally (SYSTEM_MODEL §1). Every control is a composed system of named
parts (SYSTEM_MODEL §2; CONTROL_DNA §3), addressed by stable role name rather
than by position in a document tree (owner direction). Spelling in any source
syntax is not decided here (MICROFEATURE §6.3).

### 8.2 Shared part roles

| Part | What it is | Examples |
|---|---|---|
| region | granted bounds; not painted | every control |
| shadow | contact or cast shadow, including backshadow | button, segment, popup |
| face | main material surface | button, tab, thumb, capsule |
| edge | outer boundary stroke and silhouette | every bounded part |
| keyline | inner highlight or lowlight on one edge | raised top highlight |
| sheen | specular or reflective layer over a face | gloss band, cursor sheen |
| seam | parent-owned shared boundary | splitter, breadcrumb or tab join |
| cap | leading or trailing terminal shape | chevron tip, pill end |
| well | inset receptacle | text box, address well, check box, groove |
| track | path of a ranged value | scroll bar, slider, progress |
| fill | value portion of a track | progress, slider's filled side |
| thumb | draggable actuator on a track | scroll thumb, slider knob |
| indicator | mark of a binary or tri-state value | check mark, radio dot, switch knob |
| glyph | vector symbol | arrows, chevrons, close |
| icon | image slot (PNG or vector) | command, object icon |
| label | text in the control role | caption, menu text |
| content | text or content in the content role | field value, list item |
| caret | text insertion point | editors |
| disclosure | open/close affordance | menu arrow, tree expander, `./` |
| grip | seam or panel actuator | collapse tab, resize grip |
| focus, default, selection, validation, drop | state cues | see §8.5 |

Sources: CONTROL_DNA §3.3–3.5; MICROFEATURE §6.3; theme types (`text`,
`muted_text`, `glyph`, `focus_ring`, `default_ring`); PlaySuite
`GUI_FORMS_CONTROLS_PROPOSAL.md` step 4 (`button.face`, `scroll.track`,
`scroll.thumb`, `scroll.up_button`, `check.box`).

### 8.3 Mapping examples

```text
button       shadow, face (+keyline, sheen), edge, icon?, label, disclosure?, focus, default
check box    well, indicator, label, focus
scroll bar   track (well), thumb (face, edge), arrow buttons (each a button system)
slider       track (well), fill, thumb (shadow, face, edge, sheen), focus
text box     well, content, caret, selection, validation, focus
tab          face, edge, cap, seam (parent-owned), label, focus
breadcrumb   parent region: segment button systems x N with cap, seam and shadow;
             terminal operator (label, disclosure); history drawer projection
```

Composites reuse simpler systems: a chevron segment is a button in a
parent-owned overlap relationship; a scroll arrow is a button (CONTROL_DNA §3.6,
§4.1–4.2; PlaySuite proposal step 4 "one scroll bar").

### 8.4 Cue propagation

1. A theme binds appearance to (part role, state), not (control, part). A cue set
   made once, such as the focus treatment or the pressed relief, applies to every
   part with that role in every control (owner direction; SYSTEM_MODEL §5).
2. **(proposed)** Resolution, most specific first: control instance, control
   family, part role, theme base, built-in theme. A refinement inherits whatever
   it does not restate.
3. Leading, middle, trailing and sole roles come from the parent's ordered
   membership, never from coordinates (REGIONS §4 rule 3).

### 8.5 State set

| Group | States | Source |
|---|---|---|
| interaction | normal, hot/proximate, pressed, captured | CONTROL_DNA §3.7; VISUAL §10 |
| focus, default | focused, focus-visible; default action | CONTROL_DNA §3.7 |
| value | selected, checked, indeterminate, current | CONTROL_DNA §3.7 |
| availability | enabled, disabled, unavailable, read-only | CONTROL_DNA §3.7; atlas DNA-C11 |
| validity | invalid, pending/staged | theme types; VISUAL §10 |
| disclosure | open, closed, expanding, collapsing, interrupted | CONTROL_DNA §3.7 |
| drag | drop accepted, drop rejected | VISUAL §10 |
| window | key, secondary participating, drag-proximate, deactivated | VISUAL §10; atlas DNA-F03 |
| environment | host or local high contrast, reduced motion, display scale, text scale, density | CONTROL_DNA §3.7; atlas DNA-A05 |

1. **(proposed)** Surface precedence is disabled > pressed > hot > normal. Focus,
   default, selection, validation and drop are independent layers that add to the
   surface (atlas DNA-O07; VISUAL §3.4).
2. Hot, pressed, focused, disabled and default are mutually distinguishable in
   grayscale and keyboard-only use (atlas DNA-C08).
3. Inactive is not unavailable: a secondary window recedes moderately and stays
   usable for drag; strong recession is for genuinely deactivated boards; inactive
   selection loses focus force but stays visible (VISUAL §10; atlas DNA-F03,
   DNA-O07).
4. A theme selects published states; it cannot invent a state visible to program
   code or accessibility (CONTROL_DNA §3.7).
5. Today the code addresses seven surface states (normal, hot, pressed, pending,
   invalid, disabled, deactivated) plus selected, focused, defaulted and
   high-contrast flags (theme types); the rest of the table is not yet reachable.

## 9. Motion and effects

### 9.1 Character

1. Motion is reactive, direct, local and interruptible; navigation changes state
   at once and settles without choreography (atlas DNA-T08; VISUAL §12).
2. Motion never delays input, never hides the destination, and loses nothing when
   disabled (atlas DNA-T08).
3. Spatial stage movement and cross-fade or scale navigation are rejected; ambient
   and local activity are accepted (atlas DNA-T08).
4. Progress is honest: known work shows amount, phase and component; unknown work
   shows an activity object and its exact stage, never a fake percentage (atlas
   DNA-T05).
5. Every animation retargets from its current value when interrupted (VISUAL §12;
   REGIONS §9).

### 9.2 Timing and easing

| Token | Duration | Use | Source |
|---|---|---|---|
| instant | 0 ms onset | pressed state appears in the input's frame | (proposed) |
| quick | 90–100 ms | hover, proximity, seam tab, result expansion | theme types `quick 100`; VISUAL §12 |
| standard | 150–180 ms | capsule, pane disclosure | theme types `standard 180`; PlaySuite capsule (about 155 ms open, 180 close) |
| emphasized | 280 ms | larger owned transitions | theme types `emphasized 280` |
| busy cycle | 1400 ms | one period of an activity object | theme types `busy_cycle 1400` |
| ceiling | 400 ms | longest state transition | (proposed) |

| Easing | Shape | Use | Source |
|---|---|---|---|
| smoothstep | 3t² − 2t³ | opening and closing surfaces | PlaySuite `capsule.cpp` |
| exponential approach | closes a fixed fraction of the gap per unit time (time constant about 75 ms) | lifts and pops that retarget | PlaySuite `shelf.cpp` (rate 13/s) |
| linear | constant rate | size-tracking height and border changes | atlas |
| ease | standard ease-in-out | opacity, small transforms | atlas |
| overshoot | one overshoot of at most 8 %, then settle | pop in loud themes | (proposed) |

### 9.3 Effects

1. **Pop.** A part rises one elevation level and may grow its shadow and brighten
   its face on hover, selection or focus, within its granted overflow and without
   moving neighbours or layout (owner direction; PlaySuite shelf boxes lift when
   selected; REGIONS §2).
2. **Sheen.** A specular band that follows the pointer within the part, like a
   cursor-following title-bar sheen. It redraws only when the pointer moves, and
   only within its own extent (owner direction; §2.4).
3. **Glow.** A soft coloured emission bounded by the part's visual grant (owner
   direction; VISUAL §7 title glows).
4. **Translucency and in-window glass.** A translucent face composites over the
   backplane; blur is a bounded CPU operation cached against the backplane's
   damage generation (interview GF002, GF018–GF019). Blur radius at most 8 logical
   px **(proposed)**.
5. **Reflection.** A bounded mirrored or specular copy of the part's own content
   or the backplane, within its grant (owner direction; bound proposed).
6. **Window transparency and blur** are a reported host capability; without it
   the window is opaque (owner direction; AGENTS).
7. All effects render on the CPU. Large translucent layers, blur, scaling and
   full-window animation are expensive and bounded deliberately (interview GF002).

### 9.4 Ambient animation and idle

1. Ambient animation is allowed (owner direction; atlas DNA-T08).
2. An ambient effect declares its region, period and frame cap.
   **(proposed)** At most 30 frames per second, damage limited to the region, and
   the region at most 10 % of the window.
3. It stops when the window is occluded or minimized, under reduced motion, and
   when the application's motion switch is off (a11y round A6; PlaySuite master
   Motion switch; occlusion rule proposed).
4. When nothing is animating, no frames are scheduled and nothing paints; there
   is no perpetual redraw loop (AGENTS; master plan scheduler; consumer profile
   games; VISUAL §12). A declared ambient effect is the only frame source without
   input or work. A still pointer over a sheen and a finished pop schedule nothing.
5. The reference identity band has no animated aurora (VISUAL §7); ambient
   animation in the default theme is open (§12, Q2).

### 9.5 Reduced motion

1. The host's preference is authoritative; a theme may specialize it, not
   suppress it (a11y round A6).
2. Transitions become immediate changes to the end state with all information
   kept (VISUAL §12; PlaySuite capsule and shelf jump to target).
3. **(proposed)** Ambient effects stop, sheen rests static, pop changes elevation
   without motion.

### 9.6 State machines and expressions

1. A theme may declare a presentational state machine per part, such as a press
   that overshoots and settles. Its inputs are the published states of §8.5; it
   cannot create observable states or delay behaviour (owner direction;
   CONTROL_DNA §3.7; atlas DNA-T08).
2. **(proposed)** Expressions read published state, time since state entry,
   part-local pointer position while hot, display scale, text scale, density,
   accessibility flags and theme parameters; they write presentational values
   only; they have no loops or recursion, a fixed operation budget, and identical
   inputs always give identical pixels.
3. The built-in engine evaluates them, as language assemblies use a bounded
   message program instead of executable formatters (resources §2.2).

## 10. Accessibility constraints no theme may violate

1. A theme changes style and resources only, never semantics, role, commands,
   input behaviour or data (resources §2.1; CONTROL_DNA §7 gate 5; interview
   GF018–GF019).
2. Native accessibility publication is part of the ordinary product; no theme
   turns it off (a11y round A3, §3.2).
3. Host high contrast, text scale, display scale, reduced motion and focus
   visibility are authoritative; themes may specialize, never suppress them
   (a11y round A6).
4. Every pair meets the contrast gate (§5.2) in its actual state.
5. Focus stays visible with a non-colour cue during keyboard navigation (§7.3;
   atlas DNA-O07).
6. No state relies on colour, motion, sound or hover alone (a11y round §3.4;
   VISUAL §10, §12).
7. Text scaling grows or reflows controls; essential names and actions never clip
   silently (a11y round T5).
8. A theme cannot shrink hit regions below §7.4 or change interactive order
   (REGIONS §3; proposed for themes).
9. A theme or role that fails to load falls back to the built-in theme for that
   role (resources §1, §2.1).

## 11. The default theme's character

The default is GUI.Forms' own, built from the interviews on the engine and "a
bit tweaked to look nicer" than its references (owner direction). From the
accepted File Manager reference (VISUAL; atlas):

1. **Lineage.** Future-vaporwave retro from an unplaceable 1998–2007 lineage at
   modern density and precision; professional and industrial; neither pixel art,
   dark-theme software, candy vaporwave, nor a clone of one historical system
   (VISUAL §1).
2. **Relief.** Upper-left light; raised faces with a 1 px white top keyline and a
   soft contact shadow; wells shaded top-left; local 1 px press; etched group
   rules (atlas; VISUAL §7).
3. **Atmosphere.** Sapphire Dusk: pearl chrome ramp `#fafdff`, `#e7edf6`,
   `#c9d4e5`, `#aebbd1`; ink `#1d2d4b` on white paper; selection `#dbe6ff` with
   line `#436fbd`; focus `#294f91`; accent `#3e61a3`; coral signal `#df6b65`
   (VISUAL §4).
4. **Identity band.** Sapphire-to-coral fresco with cyan, violet and apricot glows,
   a lower specular keyline, a crisp dark boundary, white title text with a 1 px
   shadow; identity sits above content (VISUAL §7; atlas DNA-F02).
5. **Command planes** are pearl with a thin coloured top filament, commands flat at
   rest and raised on hover; the **chassis** is middle-value graphite with sub-3 %
   texture and 3 px seams, never black bars (VISUAL §7; atlas DNA-V04).
6. **Content** is white paper; objects carry identity by their own colour and
   material, with no cards (VISUAL §7).
7. **Type, focus, motion.** Portsmouth Rapids for controls and titles, the bundled
   body face for content (§6); dotted focus independent of selection (§7.3);
   brief local motion, no idle frames (§9).
8. **Unconstrained taste, as evidence.** Working unconstrained, the owner builds gloss
   buttons in chrome, gold, navy and smoke tones with a hard mid-face break and a
   top shine; a warm gold ring 2 px outside the face for hover and focus;
   procedural wood, felt and paper; a floating pill capsule that opens with
   smoothstep easing; and boxes that lift with a growing shadow when selected
   (PlaySuite `suite.cpp`, `capsule.cpp`, `shelf.cpp`). How much of this enters
   the default is open (§12, Q1).
9. **Beside it, not instead of it.** Windows-familiar themes ship too: Classic, a
   modern flat Windows look, and a few more (owner direction).

## 12. Conflicts and open questions

### 12.1 Conflicts between sources

1. **Authoring model.** The owner addresses parts by name, not a CSS tree;
   SYSTEM_MODEL §2 says an ideal theme is "nearly entirely" CSS over Web.Forms
   HTML. This document states capabilities and leaves syntax open.
2. **Bitmaps.** The owner keeps control rendering procedural; resources §2.1 lets
   themes carry "PNG and precompiled bitmap assets"; the PlaySuite proposal
   admits nine-slice skins; the material engine supports nine-patch fills.
3. **Default theme.** The owner's default is GUI.Forms' own; the code's only and
   default theme is `windows-professional` (theme types).
4. **High contrast.** The owner wants derived variants; the code hand-authors
   high-contrast arrays per role (theme types).
5. **Ambient animation.** The owner allows it; VISUAL §7 forbids an animated
   aurora and §12 perpetual animation; AGENTS forbids a perpetual redraw loop; the
   atlas accepts ambient activity. §9.4 admits only declared, budgeted effects.
6. **LCD text.** a11y round T3 admits LCD on suitable opaque targets; text-engine
   is grayscale only and records LCD as deliberately excluded.
7. **Body face.** text-engine assigns Carlito to content; the ledger, resources
   §2.3 and fonts still call the body face unselected; AGENTS keeps Lucida Grande
   for historical Gallery content.
8. **Flat commands.** The owner gives affordance-bearing controls depth; the
   accepted ribbon's commands are flat at rest (VISUAL §7). §2.2 rule 2
   reconciles them, pending confirmation.
9. **Shadows on controls.** atlas DNA-F06 keeps cast shadows for real layers;
   MICROFEATURE §6 requires a breadcrumb backshadow and PlaySuite buttons cast
   shadows. §3.2 limits controls to contact shadows.
10. **Hover and focus.** PlaySuite uses one gold ring for both; atlas DNA-O07 and
    DNA-C08 require a distinct focus cue.
11. **Location.** The `planning/interview/` records exist on branch
    `claude/style-summary-and-interview-ledger`, not yet on `main`.

### 12.2 Questions for the owner

**Q1. How much of the PlaySuite look enters the default theme?** A. None;
PlaySuite stays an application skin. B. Its gloss buttons and lift become the
default construction under Sapphire. C. Only its gold focus ring and lift. D. It
ships as a separate named theme.

**Q2. Does the default theme have ambient animation?** A. None. B. One subtle
effect in the identity band. C. Any rich region, within the §9.4 budget.

**Q3. May themes use bitmap faces for controls?** A. Never; PNG only for icons,
objects and backplane. B. Third-party skins may use nine-patch faces; shipped
themes stay procedural. C. Any theme may, with a procedural fallback.

**Q4. Is a dark variant of the default shipped?** A. No; high contrast only.
B. Yes, derived per §5.3, as a user choice. C. Yes, derived, following the host's
dark preference.

**Q5. What contrast gate applies?** A. The §5.2 proposal. B. 7 : 1 for all body
text in every theme. C. §5.2 for shipped themes; third-party themes warned, not
blocked.

**Q6. When is focus drawn?** A. Always on the focused control. B. Only after
keyboard use, as in PlaySuite. C. After keyboard use, and always in high contrast
or when the host requests focus visibility.

**Q7. How far may a theme reshape a control?** A. Visual silhouette only.
B. Silhouette and hit region within the granted bounds and §7.4 minimums.
C. Those plus arrangement of parts inside the control, semantics fixed.

**Q8. Which Windows-familiar themes ship?** A. Classic and modern flat only.
B. Those plus an XP-era look. C. Those plus a Windows 7-era glass look using host
window transparency where available.

**Q9. Do the two high-contrast authorities stand (§5.3 rule 2)?** A. Yes.
B. Both modes use the house reconstruction with host colours mapped in. C. Host
mode only; no local mode.

**Q10. Are the proposed numbers acceptable starting tokens?** A. Yes. B. Yes,
except elevation alphas, pending a native specimen review. C. No; choose from a
native specimen sheet first.
