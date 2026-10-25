# Stage T3: theme language front end

Release: 0.3 Themeable. Depends on: T1 (part and state names), T2 (the
engine's properties, primitives, inputs and expressions). Unblocks T4, FD, D,
H, V. Status 2026-10-25: interview rounds 001 and 002 answered (below); the
phased build plan, [`PLAN.md`](PLAN.md), is revised to them and awaits the
owner's last review before P1. No parser code yet. Authority:
`planning/FORWARD_PROGRAMME.md` §4 T3; design language §12.1 item 1.

Read [the shared brief](../README.md) first.

## Why

The owner's starting idea, 2026-10-12: theming "both by inline effects as
well as by the use of a theme style sheet. Think WinCustomize-type theming
for Windows, or KDE themes, or GTK themes, but better. Done right … bold,
insane idea: inline CSS subset." The owner liked "the huge variety of styles
KDE had".

The owner called the choice of authoring model "a landmine question if we get
it wrong", and it was worked through with a scenario. The scenario was a play
button that pops when pressed, has a sheen following the cursor, glows with
the audio level, and in one window is circular through its stable ID. The
owner's conclusion: "a CSS-specific only declarative could be the option, CSS
now has state machines". Recorded resolution (design language §12.1 item 1):
themes are written in a CSS-syntax subset addressed to the named anatomy
(`::part()`, `:state()`, stable IDs), not a CSS document tree; engine inputs
are custom properties only the engine sets; the state machine's states,
transitions and timings are declared in the theme and run by the engine; new
kinds of effect are C++ engine primitives, never theme code.

Why CSS syntax fits: states are pseudo-classes (including custom `:state()`),
transitions and keyframes move between them, `calc()`, `clamp()`, `min()`
and `max()` are bounded expressions, custom properties are named inputs, and
`::part()` with `:state()` is exactly the named-parts-and-states model.
Anyone who knows CSS can read a theme, and the visual designer (V) can read
and write all of it.

What history teaches:

- Windows XP visual styles addressed everything by class, part and state
  (`Button.PushButton(Hot)`). That is the lasting idea. Their weakness was
  bitmaps everywhere.
- GTK 2's `gtkrc` used selectors, per-state colours and a C engine with
  parameters, so one engine gave dozens of looks. GTK 3 moved to a CSS subset,
  but its node names kept changing until 3.20 and broke third-party themes
  every release. A style language works only if its part names are a stable,
  versioned contract.
- KDE 3 styles were C++ `QStyle` code, and style, colours, window decoration
  and icons combined freely. QtCurve showed that one engine with a large
  parameter set could imitate most others.

## Scope

- **Selectors**: control type, stable ID (`#id`), class, `::part(name)`,
  `:state(name)` and the standard state pseudo-classes the schema publishes.
  No combinators: no descendant, child or sibling selection.
- **Properties**: the engine's style vocabulary (fills, gradients, borders,
  keylines, radii from the engine's shape parameters, shadows, effect layers,
  text role and colour roles, motion). No layout properties. Layout belongs
  to forms and the engine (stage L); the owner: "dynamic flexing layouts
  should be easier than they are. This is an engine task."
- **Inheritance** only for text and colour roles.
- **Inputs**: engine-set custom properties (`--pointer-x`, `--pointer-y`,
  published application values such as `--level`). A theme reads them and
  never sets them.
- **Expressions**: only the bounded grammar of T2.
- **Transitions and state machines**: CSS transition and keyframe syntax
  mapped onto the engine's state machines and settling primitives
  (`follow()`, `spring()`, `decay()` as engine functions).
- **Resources**: image assets named by resource: tiling backgrounds,
  backplane frescoes and control faces. Image faces are supported: CONV
  prepares each face size ahead of the bake (T2, "CONV, image face
  variants"). CNG is the native resource format, and PNG is for interop. No
  font bytes; a theme selects among bundled typography roles. No executable
  code, filesystem or network access.
- **Engine version**: a theme declares the engine version it needs.

The scenario, in the syntax direction agreed with the owner (illustrative,
not a frozen grammar):

```css
button::part(face) {
  background: linear-gradient(var(--face-light), var(--face-dark));
  transition: scale 90ms cubic-bezier(.3, 1.6, .5, 1);   /* the pop */
}
button:active::part(face) { scale: 0.97; }
button:hover::part(sheen) {
  background: radial-gradient(at var(--pointer-x) var(--pointer-y),
                              rgb(255 255 255 / 25%), transparent 60%);
}
#player-play::part(face) {
  shape: circle;             /* a shape parameter the engine provides */
  --glow: clamp(0, calc(var(--level) * 2), 1);
}
```

Note the last rule. A theme chooses among the engine's shape parameters; it
never sets geometry or hit regions directly (owner decision §12.3: "the engine
must specify geometry, not the theme. Therefore the engine must also specify
hit region"). The scenario used CSS `clip-path` during discussion. The
grammar must express shape as an engine parameter.

## How

- Diagnostics name the file, line and rule. Themes are compiled: an error
  refuses the whole theme and a warning drops one declaration (owner
  decisions B2 and B3 below, which supersede the earlier "an invalid rule is
  dropped, not fatal"). Omitted parts fall back to the built-in recipe.
- A byte-preserving round trip: parsing and re-serializing an untouched
  document returns identical bytes. Keep comments, whitespace and rule order
  in the parsed form. V and D depend on this ("no clobber").
- Treat every theme as hostile input: bounded nesting, token and rule counts,
  string lengths and expression sizes; no recursion depth that input
  controls. Add a fuzz target beside `fuzz/png_registry_fuzz.cpp`.
- House style: the parser fills caller-owned rule tables (§4); token, offset
  and line types are distinct (§1); failure states say what they leave
  behind (§9); no lambdas in the parser (§2, §10).
- Theme collections are immutable data assemblies with a manifest, coverage
  table and content hashes (`planning/interview/GUI_FORMS_RESOURCES_AND_CONFIGURATION.md`
  §2.1, §3). The owner settled RC001: data only.

## Gate

Conformance fixtures for every selector and property form; malformed and
hostile input tests; a byte-preserving round trip of an untouched document.
Record the fixture coverage and fuzz run under `results/`.

## Owner decisions, 2026-10-13

1. **Live switching (RC003).** Theme collections may change while a program
   runs, using the core's atomic theme replacement; focus, capture,
   selection, semantic bounds and active edits survive a swap. The theme
   designer's live stage depends on it.
2. **Trust (RC004).** External packs need a content hash for integrity, no
   signature. Themes are data-only and bounded; invalid data falls back.
3. **Names are kebab-case** (see T1).
4. **Forms share this syntax.** Form documents (FD) use the same CSS-family
   syntax and the same parser and round-trip machinery.

The file extension and media type, left open here, were decided in round
002 (item 01, below).

## Owner decisions, interview round 001 (2026-10-25)

Answered by the owner on 2026-10-25 and relayed by the coordinator. The
owner's own words are quoted verbatim. Where a decision needs a change to
the engine (T2's code), [`PLAN.md`](PLAN.md) names it as a T3 phase.

The owner's note on where the engine stands, verbatim: "i hesitate to call
the theme engine done- its blueprint is probably done. but its optimization
and full population is still in flight". T3 runs alongside that work: it
compiles onto the engine's present vocabulary and grows with it.

### A. Properties, values and colours

- **A1.** Property names are CSS's, lowered onto engine parameters, plus
  engine-named properties for what CSS lacks (shape, finish, glow
  placement, procedural materials and the like).
- **A2.** The common shorthands are accepted (`background`, `border`,
  `border-radius`, `box-shadow` lists, `transition`, `animation`). The
  parser expands them, and keeps the original text for the byte-preserving
  round trip.
- **A3.** Units: logical px, percentages where CSS defines them, and em
  relative to the part's text-role size. No rem or viewport units.
- **A4.** Colours are colour roles first (`role(...)` and its pairs), with
  literals allowed (hex, `rgb()`, `hsl()`, `color-mix()`). A literal used
  as a text or edge colour, which forced high contrast cannot remap, gets a
  WARNING.
- **A5.** Themes may define their own custom properties. Engine inputs
  (`--pointer-x`, `--pointer-y`, published application values) are a
  reserved, read-only namespace.

### B. Selectors, compilation and failure

- **B1.** The decided selector set plus `:is()`. No `:not()`. No
  combinators.
- **B2.** Themes are COMPILED. The owner: "the engine not only runs themes,
  but \"compiles\" them with a parser. this throws warnings or errors the
  same as anything else. a warning is nonfatal. an error is. a theme with an
  error thus notifies the developer in the console correctly, and refuses to
  execute."
  - The owner's clarification on the split: "unknown properties are warned,
    bad values fail."
  - **ERROR, so the whole theme is refused and the previously active theme
    or the built-in recipe stays:**
    - a bad value;
    - a parse failure (structure, strings, selector syntax);
    - an exceeded hostile-input limit;
    - an `@import` naming a theme that isn't installed or fails its hash;
    - an invalid manifest;
    - a theme setting a reserved engine input.
  - **WARNING, so the declaration is dropped and the rest applies:**
    - an unknown property or primitive, including one from a newer engine;
    - an unremappable literal colour (A4).
  - Diagnostics name the file, line and rule, and reach the developer
    through the console or log.
  - Atomic replacement means a refused theme never half-applies.
- **B3.** An unknown name is a warning and the rest applies; a part falls
  back to the built-in recipe only where no primitive survives. Tools warn
  when a theme asks for a newer engine than the one running.

### C. Motion

- **C1.** No theme-declared states in the first version: transitions and
  `@keyframes` are keyed on published states only.
- **C2.** `follow()`, `spring()` and `decay()` are written as timing
  functions in `transition`. Durations stay in time, because they are
  perceptual.
- **C3.** Reduced motion is automatic in the engine, plus
  `@media (prefers-reduced-motion)` for a theme's own calm look.

### D. Documents and packages

- **D1.** One sheet with `@media` blocks (`prefers-color-scheme`,
  `forced-colors`, `resolution`).
- **D2.** `@import` of another installed theme by name, resolved only within
  installed, hash-checked packs. No paths and no network.
- **D3.** A JSON manifest beside the CSS sheets: identity, version,
  compatible engine versions, coverage and content hashes.

### E. Inline styles, limits and the seam

- **E1.** Inline styles come from two sources through one parser:
  `control.set_style(...)` in C++, and the `style` attribute in form
  documents. The owner's rule, verbatim: "a warning is issued if the engine
  determines that a program is loading with both a theme and inline, if
  !important is used in one place and not the other, the important
  overrides, otherwise- if neither or both are important- the inline wins-
  attached styles or restyling by a system is overridden by local controls,
  and a program designer can always be sure inlined !important rules are
  honored."
  - The owner's follow-up on high contrast: forced high contrast still wins
    on COLOUR (the user's palette), as browsers do. For everything else
    (shape, radii, motion, effects), inline `!important` is honoured.
  - **How it maps onto the engine's cascade** (the worker's reading, for
    the owner's review). Today `CascadeLevel` orders, lowest first:
    `built_in`, `theme_sheet`, `class_rule`, `stable_id_rule`,
    `inline_style`, `forced_high_contrast`; within a level the more
    specific selector wins, then the later rule. There is no `!important`,
    and the forced high-contrast pass replaces the whole drawing of every
    primitive-drawn part with the built-in recipe's high-contrast records
    (`ThemeEngine`, `PartDrawing::forced_high_contrast`). The rule above
    needs this order, lowest first:

    | Precedence | Declarations | Level |
    |---|---|---|
    | 1 | the built-in recipe | `built_in` |
    | 2 | theme sheet, normal | `theme_sheet`, `class_rule`, `stable_id_rule` |
    | 3 | inline style, normal | `inline_style` |
    | 4 | theme sheet, `!important` | new: `important_theme_sheet` |
    | 5 | forced high contrast, non-colour | `forced_high_contrast` |
    | 6 | inline style, `!important`, non-colour | new: `important_inline_style` |
    | 7 | forced high contrast, colour | `forced_high_contrast`, over every level |

    - Rows 2 and 3: neither important, so the inline style wins. Rows 3
      and 4: the important one wins whichever side it is on. Rows 4 and 6:
      both important, so the inline style wins.
    - A theme-sheet rule's level comes from its selector: a stable ID puts
      it at `stable_id_rule`, else a class at `class_rule`, else
      `theme_sheet`. This orders exactly as specificity already does (ids
      first, then classes), so the three levels change nothing for one
      sheet. The important level needs no sublevels: specificity, then
      source order, orders within it.
    - Forced high contrast keeps the user's palette: every colour, at any
      level, comes from the high-contrast colours. Only inline
      `!important` declarations that are not colours (shape, radii,
      motion, effects) survive the pass; theme `!important` does not
      (confirmed in round 002, item 09).
    - **The engine change** (PLAN.md, phase P9): `CascadeLevel` gains
      `important_theme_sheet` after `inline_style` and
      `important_inline_style` after `forced_high_contrast`; the compiler
      splits each rule's `!important` declarations into a sibling rule
      with the same selector, label and source order at the important
      level; and the forced high-contrast pass changes from "replace the
      drawing" to "replace every colour, and keep the non-colour
      declarations of `important_inline_style`". The warning for a
      program that loads both a theme and inline styles is a T3
      diagnostic, issued once per window.
- **E2.** The limits: 1 MiB per sheet, 10,000 rules, 64 declarations per
  rule, nesting 32 deep, 4 KiB per string and 16 imports, with expressions
  within T2's operation budget. Exceeding one is an ERROR (B2).
- **E3.** The parser is internal now, with a documented seam, and becomes
  public when stage V starts.

## Owner decisions, interview round 002 (2026-10-25)

The owner's answers to the 21 open questions of [`PLAN.md`](PLAN.md)
(first version, #316), relayed by the coordinator from the interview form's
export of 2026-10-25. The owner's notes are quoted verbatim. PLAN.md,
"Decisions", now carries each as decided, and its phases follow them.

Accepted as recommended:

- **01.** Sheets are `.theme.css`; the manifest is `theme.json`; the
  manifest's `format` field names the dialect (`"gui-forms-theme/1"`).
  `text/css` on the wire; nothing is discovered by extension.
- **03.** Design language §5.4's pairs become `ColourRole` vocabulary
  atoms. A theme's palette is a `:root` block of role declarations in the
  sheet. `role(<pair>)` gives the foreground in `color` and the background
  in `background`; `role(<pair> foreground | background)` picks one
  anywhere.
- **04.** A part role is addressed as `::role(<role>)`.
- **06.** The published application values a theme reads are listed in its
  manifest.
- **07.** Engine-named properties are plain names, versioned with the
  engine. A keyword the engine lacks is an error, or a warning when the
  sheet or rule declares a newer engine.
- **08.** As the plan proposed: an unknown control type, part, state or
  pseudo-class name is a warning that drops the rule;
  `primitive_role_mismatch` is an error; `primitive_over_budget` and
  `primitive_cost_unbounded` are warnings; a property refused by decision
  (`clip-path`, `rotate`, layout properties) is a warning,
  `property_not_admitted`.
- **09.** Theme `!important` does not survive forced high contrast; only
  inline `!important` non-colour declarations do.
- **11.** A percentage radius is a percentage of the shorter side (50 % on
  any box is a pill, a stated difference from CSS's ellipse).
- **12.** Content hashes are SHA-256, first-party, with the standard test
  vectors.
- **13.** Custom control states are deferred: `:state()` of a custom state
  is an unknown name (a warning) for now.
- **14.** The derived limits are accepted as starting values: 32 `:is()`
  arguments, 1,024 custom properties per sheet, 4,096 tokens per
  substituted value, 64 `@keyframes` per sheet, 1,000 diagnostic records.
  Exceeding one is an error.
- **15.** Selector lists are admitted: one rule per selector, each with its
  own specificity.
- **16.** CSS escapes only inside strings; names are plain kebab-case ASCII.
- **17.** `ease-in` and `ease-out` lower to their exact CSS
  `cubic-bezier()`.
- **18.** The first version groups a transition with the refinements of its
  own rule only. Joining curves and state values across rules is a later
  engine phase, if themes need it.
- **19.** `@media` is evaluated per window: each rule carries its condition,
  and parts are re-resolved on a change.
- **20.** The 16 imports are counted over a theme's whole import closure,
  each installed theme once.

Changed or clarified:

- **02. Materials: each material field cascades on its own** (option 1),
  and the engine assembles the part's material from the winning fields.
  The owner's note: "clarify if 3 is an expansion of 1 for me. "
  - The answer, recorded: option 3 (CSS writing into the theme's recipe
    table by recipe family and surface state) is not an expansion of
    option 1. Option 1 works per part, with the part's built-in recipe as
    the cascade floor: a field no rule sets keeps the recipe's value. A
    broad selector such as `button:hover::part(face)` (or
    `::role(face):hover`) already reaches every part that family
    addressing would, and narrower selectors refine it. Option 3 would only
    add a second, coarser route to the same parts.
- **05. Custom properties: the full CSS cascade per part** (option 3). A
  theme's custom properties are resolved per part by the same cascade as
  properties, including `!important` under the E1 rule. The owner's note:
  "if i understand this correctly, this is tangential to stylish/userstyles
  type stylesheet problems. I think I want 1, but i might want 3. check
  with me." The coordinator checked; the owner's answer is option 3.
  - The owner's reason: restyling a program by a system or user sheet (the
    Stylish/userstyles problem). Example: a restyle sheet's
    `button { --radius: 8px; }` changes buttons only, and every rule that
    reads `var(--radius)` on those parts follows.
  - This is an engine change: values that read a custom property stay
    unresolved until each part's cascade, and the part cache keys on the
    resolved values. PLAN.md adds it as engine change EC7, phase P12.
  - It raised one new question, how a deferred value is checked when it
    reads several names or comes from an inline style compiled apart from
    the theme (PLAN.md, section 8.2). Answered in the addendum below.
- **10. Inline styles: option (a)**, a declaration list plus optional part
  blocks with the control's own selector implied. The control owns the
  compiled result, so no stable ID is needed and it survives theme
  replacement. The C++ name is `Control::set_inline_style(std::string_view)`
  (the recommendation), leaving WinForms' `set_style(ControlStyles, bool)`
  as it is.
- **21. The smaller set: accepted, with one change.** The owner's note:
  "named colours from css are admitted." Every CSS named colour is a
  literal colour, so a named colour used as a text or edge colour gets the
  same `literal_colour_unremappable` warning as any other literal. The rest
  stands: `initial` means the built-in recipe's value; `isolation` is
  accepted and changes nothing; `animation-direction` and
  `animation-fill-mode` other than their defaults are errors in the first
  version; the diagnostic log line of PLAN.md section 2; and
  `inline_with_theme` is issued once per window.

### Round 002 addendum (2026-10-25)

The owner's answers to PLAN.md section 8.2, relayed by the coordinator.
Both are decided.

1. **At most 256 candidate combinations per declaration that reads
   `var()`.** Beyond that is an ERROR.
2. **An inline style that the theme reads as the wrong type is refused.**
   When an inline style defines a custom property that the theme reads as
   the wrong type, the INLINE STYLE is refused as an error:
   `Control::set_inline_style()` returns an error result and the
   diagnostic reaches the console. The theme stays active and unchanged,
   and no declaration is half-applied. The same rule applies to the form
   document's `style` attribute.
   - The reason: it keeps the owner's "bad values fail" rule, because the
     newly added piece is the one at fault.
   - The owner chose this (the coordinator's recommendation) over the
     plan's proposed fallback, which would have dropped the declaration at
     resolution with a warning.

## Read first

- `planning/DESIGN_LANGUAGE_SPECIFICATION.md` §1.2, §8.2, §9.6, §12.1 item 1,
  §12.3
- `planning/interview/GUI_FORMS_RESOURCES_AND_CONFIGURATION.md` §1–§3, §8
- `planning/CONTROL_DNA_AND_STYLE_ADDRESSABILITY.md` §5, §7 gate 5
- `planning/DESIGN_LANGUAGE_SYSTEM_MODEL.md` (the earlier HTML/CSS framing,
  superseded where it describes a CSS document tree)
- [T1](../T1-anatomy/README.md) and [T2](../T2-engine-core/README.md) briefs
