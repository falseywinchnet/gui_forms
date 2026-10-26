# Stage T3 plan: the theme language front end

Status 2026-10-25: revised to interview round 002, for the owner's last
review before P1. No parser code exists yet. The decisions it builds on are
in [`README.md`](README.md): "Owner decisions, interview round 001
(2026-10-25)", cited here by their letters (A1 to E3), and "Owner
decisions, interview round 002 (2026-10-25)", cited as R2-01 to R2-21.
Where this plan and a decision disagree, the decision governs.
The engine vocabulary it lowers onto is listed in
[`../T2-engine-core/CSS_COVERAGE.md`](../T2-engine-core/CSS_COVERAGE.md);
the rule types it fills are in
`include/gui_forms/engine/theme_rules/theme_rules.hpp`.

## What T3 builds

A compiler from theme text to the engine's rule tables. It reads a theme
package (a JSON manifest and CSS-syntax sheets, D3), resolves its imports
among installed packs (D2), tokenizes and parses each sheet into a tree that
keeps every byte, lowers the tree onto the engine's `ThemeRule`,
`RuleSelector` and `RuleDeclaration` records, checks them against the
primitive registry, and hands the result to the engine through atomic theme
replacement (`Window::set_theme`, `ApplicationResources::set_theme`). One
error refuses the whole theme and leaves the previous one running; a warning
drops one declaration (B2, B3). The same tokenizer, tree and lowering serve
inline styles (E1) and, later, form documents (FD decision 1).

T3 is mostly front end, but the decisions need seven things the engine
cannot say today. Each is a phase of its own below, marked
**engine change**, with T2's tests run beside T3's:

| | Engine change | Why | Phase |
|---|---|---|---|
| EC1 | authored materials: a rule can carry material fields (fills, border, radii, shadows, keylines, filters, mask, finish, glow) and the recipe's colours (text, glyph, rings), each cascading like a parameter | the `material` primitive today takes only `recipe` and `shape` choices and draws the theme's recipe table, so `background: linear-gradient(...)` on a part has no carrier | P6 |
| EC2 | a rule's specificity can be given, not only computed from its selector | `:is()` takes its most specific argument's specificity (B1) | P3 |
| EC3 | a rule can carry a media condition, evaluated per window | `@media` (D1, C3); windows share a theme but differ in scale | P8 |
| EC4 | colour values carry the colour role they came from; colour roles become vocabulary atoms; the high-contrast pass remaps by role, or by position for a literal | A4 | P8 |
| EC5 | the part's text-role size as an engine input, and a part's text role as a rule property | em units (A3); `font: <role>` | P8 |
| EC6 | two `!important` levels, a colour-only forced high-contrast pass, and inline styles owned by controls | E1 | P9 |
| EC7 | custom properties resolved per part by the cascade: a declaration that reads `var()` stays unresolved until the part's cascade, and the part cache keys on the resolved values | R2-05 | P12 |

## 1. The grammar in outline

The syntax is CSS's: CSS Syntax Module Level 3 for tokens and blocks, CSS
Selectors for the admitted selector forms, CSS Values for numbers and
functions. Only the subset below is admitted. "Error" and "warning" refer to
the classes in section 2.

### 1.1 Source

- UTF-8 only. A leading byte-order mark is kept (round trip) and ignored.
  Invalid UTF-8 or a NUL byte is an error. `@charset` is not admitted.
- Line breaks are LF, CRLF or CR; each counts as one line for diagnostics.
  The bytes are kept as written.
- At most 1 MiB per sheet (E2), checked before anything is allocated.

### 1.2 Tokens

The token kinds are CSS Syntax §4's, restricted:

| Kind | Notes |
|---|---|
| `byte_order_mark` | a leading U+FEFF, kept for the round trip (P1) |
| `whitespace`, `comment` | kept as tokens, so the tree can reproduce every byte |
| `ident`, `function` (an ident followed by `(`), `at_keyword`, `hash` | names; escapes are not admitted outside strings (R2-16) |
| `string` | `"..."` or `'...'`, at most 4 KiB (E2); an unterminated string is an error |
| `number`, `percentage`, `dimension` | the number is converted once, correctly rounded, to `double`, by a first-party conversion (`number/`); at most 19 significant digits and a magnitude below 1e40 and at least 1e-40 (zero excepted), else `number_too_precise` or `number_out_of_range` (P1) |
| `delim`, `colon`, `semicolon`, `comma`, `[`, `]`, `(`, `)`, `{`, `}` | |
| `url` | only as the function `url("resource:<name>")` with a quoted string (E8c decision on inline images, option A); an unquoted `url(...)` token is an error |

`<!--`, `-->` and `\` outside a string are errors. Bytes that form no
admitted token are an `invalid` token beside their diagnostic, so the
tokens always cover the sheet. A token is a kind and a byte range (16
bytes; a sheet of n bytes has at most n tokens, reserved before the
loop); its line and column are computed from a line-start table only
when a diagnostic needs them.

### 1.3 Sheet structure

```text
sheet        := item*
item         := at-rule | style-rule
style-rule   := selector-list "{" declaration-list "}"
declaration  := property ":" value ["!important"] [";"]
at-rule      := "@import" string ";"
              | "@media" media-query-list "{" style-rule* "}"
              | "@keyframes" ident "{" keyframe-block* "}"
keyframe-block := ("from" | "to" | percentage) ("," ...)* "{" declaration-list "}"
```

- Nesting (blocks inside blocks, functions inside functions, `:is()` inside
  `:is()`) is at most 32 deep (E2). The parser is iterative with an explicit
  stack of that depth: no recursion depth follows the input.
- At most 10,000 rules per sheet and 64 declarations per rule (E2). Rules
  made by expanding selector lists, `:is()` and media query lists count
  against the 10,000.
- `@import` comes before every other item, as in CSS; one after a rule is an
  error. An unknown at-rule is a warning, and its block is skipped (B3).
- A structural defect (a missing `}`, a declaration outside a rule, a stray
  token) is an error. CSS's own error recovery, which skips bad
  constructs, is not used: B2 makes a parse failure fatal.
- CSS nesting of style rules inside style rules is not admitted: it is a
  form of combinator (B1).

### 1.4 Selectors

A selector is one compound selector naming one part of one control (B1: no
combinators). The forms, and the `RuleSelector` field each fills:

| Form | Example | Field |
|---|---|---|
| control type | `button`, `track-bar` | `control_type` (the registered id, written kebab-case; T1 decision 8) |
| any control | `*`, or no type written | none (matches every control) |
| stable ID | `#player-play` | `stable_id` (stable IDs keep their own spelling) |
| class | `.primary` | `classes` (valid style class names) |
| part | `::part(scroll-thumb)` | `part` (the declared part name) |
| part role | `::role(face)` | `role` (R2-04) |
| custom state | `:state(drop-accepted)` | `states` |
| standard state | `:hover` `:active` `:focus` `:focus-visible` `:checked` `:indeterminate` `:disabled` `:enabled` `:read-only` `:invalid` `:open` `:default` | `states`: `hot`, `pressed`, `focused`, `focus_visible`, `checked`, `indeterminate`, `disabled`, `enabled`, `read_only`, `invalid`, `open`, `preselect` |
| either of | `button:is(:hover, :focus-visible)::part(face)` | expanded: one rule per argument |
| selector list | `button, check-box { ... }` | expanded: one rule per selector, each with its own specificity (R2-15) |

- Every other published state is written `:state(<kebab-name>)`:
  `:state(drag-proximate)`.
- `:is()` takes compound selectors (no combinators inside it either). It is
  expanded into one rule per argument, and each rule takes the specificity
  of the most specific argument, as CSS does (EC2). An `:is()` of at most 32
  arguments; nested `:is()` expansions multiply, and the product is bounded
  by the rule limit.
- `:not()`, `:where()`, `:has()`, attribute selectors, combinators
  (descendant, `>`, `+`, `~`) and nesting `&` are errors: they are not this
  language (B1).
- An unknown control type, part, state or pseudo-class name is a warning
  that drops the rule (B3, R2-08). A custom control's own state is such an
  unknown name for now (R2-13).
- Specificity is the engine's (`RuleSpecificity`): stable IDs, classes,
  control types, parts, states.

### 1.5 `!important`

`!important` follows a declaration's value, as in CSS. The compiler splits a
rule's important declarations into a sibling rule at the important level
(README, E1 mapping; EC6). Inside `@keyframes` it is an error, as in CSS.

### 1.6 Values

- **Numbers and units** (A3): plain numbers; `px` (logical units); `%` only
  where CSS defines percentages (gradient stops, colour alpha, opacity,
  `background-position`, `background-size`, keyframe offsets, and radii,
  where a percentage is of the shorter side, R2-11); `em`, relative to the part's text-role size (EC5);
  `ms` and `s` for durations; `deg`, `rad`, `turn` for angles; `dppx` and
  `x` in `@media (resolution)`. `rem`, `vw`, `vh`, `vmin`, `vmax`, `ch`,
  `ex` and every other unit are errors.
- **Colours** (A4):
  - roles first (R2-03): design language §5.4's pairs are `ColourRole`
    atoms. `role(<pair>)` gives the pair's foreground in `color` and its
    background in `background`; `role(<pair> foreground)` and
    `role(<pair> background)` pick one anywhere. A theme's palette is a
    `:root` block of role declarations
    (`:root { ink-on-paper: #1b2733 on #ffffff; }`);
  - literals: `#rgb`, `#rgba`, `#rrggbb`, `#rrggbbaa`, `rgb()`, `rgba()`,
    `hsl()`, `hsla()` (both comma and space forms), `transparent`, and
    `color-mix(in srgb, a p%, b)`, and every CSS named colour (R2-21),
    which is a literal like any other.
  - A literal in a text or edge position (`color`, `border-color`, keyline
    and ring colours) is a warning, `literal_colour_unremappable`: forced
    high contrast replaces it with its position's high-contrast colour,
    losing the relation the author meant. The declaration still applies.
- **Custom properties** (A5, R2-05): `--name: <tokens>` and `var(--name)`,
  `var(--name, fallback)`. They cascade per part like any property,
  `!important` included (section 1.11, EC7). Engine inputs are reserved
  names: `--pointer-x`, `--pointer-y`, `--time-in-state`,
  `--display-scale`, `--text-scale`, and every published application value
  the manifest lists (R2-06). A theme that declares one of them is an
  error. A `var()` of a name that no rule in the compilation defines, with
  no fallback, is a bad value (error), and so is a cycle.
- **Expressions**: `calc()`, `clamp()`, `min()`, `max()` (CSS), and the
  engine functions `mix(a, b, t)`, `smoothstep(e0, e1, x)`,
  `choose(c, a, b)`, `less(a, b)`, `less-equal`, `greater`,
  `greater-equal`, `equal`, `not-equal`, `state(<state>)` (1 or 0) and
  `param(<parameter>)` (another number parameter of the same primitive).
  - They compile to the engine's postfix `ExpressionInstruction` programs.
    An expression whose program would exceed 64 instructions or a stack of
    16 (`maximum_expression_operations`, `maximum_expression_stack`) is an
    error (E2).
  - Constant subexpressions are folded with the engine's own evaluator, so
    folding gives the same bits as evaluating.
  - A value that reads an input or a state becomes an expression
    declaration (`RuleValueKind::expression`); one that reads neither is
    folded to a number.
- **Images**: `url("resource:<name>")`, a resource the theme's manifest
  lists; it resolves to an `ImageId` at load (CSS_COVERAGE §10). A name the
  manifest does not list is an error.
- **Keywords**: a known property's keyword that the engine does not have
  (`shape: hexagon`) is a bad value (error), unless the sheet or rule
  declares a newer engine than the running one, when it is a warning
  (R2-07).

### 1.7 Lowering CSS names onto the engine

Property names are CSS's where CSS has the property (A1), written kebab-case;
engine-named properties fill what CSS lacks, unprefixed as in the agreed
scenario (`shape: circle`; R2-07). Shorthands are expanded into
longhands by the lowering; the tree keeps the shorthand as written (A2).

A rule that declares only material fields selects the `material` primitive
implicitly; `primitive: <name>` selects another, and its parameters are
written by their registered names in kebab-case (`amplitude`). The
CSS_COVERAGE sections, and what each lowers to:

| CSS_COVERAGE | CSS form | Lowers to | Work |
|---|---|---|---|
| §1 fills | `background-color`; `background-image` with `linear-`, `radial-`, `conic-gradient()` and their `repeating-` forms; `background` (shorthand, up to 8 layers) | `MaterialFillLayer` solid, linear, radial, conic, with repeat spreads | EC1 |
| §1 | procedural backgrounds (`felt()`, `wood-grain()` and the rest) | E10's engine functions, admitted as E10 registers them; an unknown one is a warning (an unknown primitive, B2) | E10 |
| §2 borders | `border`, `border-<edge>`, `border-width`, `border-color`, `border-style` (`solid`, `dashed`, `dotted`) | `SurfaceMaterial::border`, `MaterialBorderEdges`, `MaterialBorderStyle` | EC1 |
| §2 | `border-style: double`, `groove`, `ridge` | keyline pairs (`MaterialKeyline`), no engine change | T3 |
| §2 | `border-radius` (1 to 4 values) | `corner_radius`, `corner_radii`; elliptical radii (`/`) are an error | EC1 |
| §2 | `shape` (engine-named: `rectangle`, `rounded`, `pill`, `circle`, the caps) | the `shape` choice | present |
| §2 | `outline`, `outline-offset` | the focus ring fields (`focus_ring`, `focus_width`, `focus_offset`) | EC1 |
| §2 | `border-image` | a nine-patch image fill | EC1 |
| §3 shadows | `box-shadow` (up to 4, `inset` admitted) | `MaterialShadow` | EC1 |
| §3 | `text-shadow` | `ControlTextShadow` (data only until the text stage draws it) | EC1 |
| §4 opacity | `opacity` | static: a one-state machine (CSS_COVERAGE §4); in a transition: `opacity_property` | T3 |
| §5 filters | `filter: brightness() contrast() saturate() grayscale() sepia() invert() hue-rotate()` | one `MaterialColourMatrix`, composed in source order | EC1 |
| §5 | `filter: blur()`, `drop-shadow()` | `filter_blur`, `drop_shadow` (or an outer `MaterialShadow` for a box-shaped part) | EC1 |
| §5 | `filter: url()` | error (not admitted, RC004) | |
| §6 backdrop | `backdrop-filter: blur() <colour functions>` with the part's `background-color` as tint | the `glass` primitive's blur, recipe matrix and tint | present |
| §7 blending | `mix-blend-mode: normal`, `multiply`, `screen` | `blend_property` | present |
| §7 | `mix-blend-mode` other modes | error (not admitted, CSS_COVERAGE §7) | |
| §7 | `background-blend-mode` | `MaterialFillLayer::blend` | EC1 |
| §7 | `isolation` | accepted, no effect: every blend already draws in its own group | T3 |
| §8 finish | `surface-finish` (engine-named, 0 to 1) | `SurfaceMaterial::finish` | EC1 |
| §9 light | `glow`, `glow-placement: under | over` (engine-named) | `MaterialGlow` | EC1 |
| §9 | a `radial-gradient()` positioned at `var(--pointer-x) var(--pointer-y)` | the `sheen` primitive's parameters; any other gradient reading the pointer is an error, because the engine never re-bakes a material per pointer move (E7r) | T3 |
| §9 | pop (`scale` and `face` with a transition) | transitions, section 1.8 | present |
| §10 images | `background-image: url("resource:...")`, `background-size`, `background-repeat`, `background-position` | image fill, `MaterialImageMode`, `image_scale`, `image_anchor`, `image_offset` | EC1 |
| §11 masks | `mask-image` (an image or a gradient) | `SurfaceMaterial::mask` | EC1 |
| §11 | `clip-path` | warning, `property_not_admitted`: geometry is the engine's (§12.3) | |
| §12 transforms | `scale`, `translate` | `scale_property`, `translate_x_property`, `translate_y_property`; static values become one-state machines | present |
| §12 | `rotate`, `transform`, `transform-origin` | warning, `property_not_admitted` | |
| §13 motion | section 1.8 | | present |
| §14 colour | `color` | the recipe's `text` | EC1 |
| §14 | `font: <role>` (`ui`, `field`, `caption`, `heading`, `title`, `monospace`) | the part's text role | EC5 |
| §14 | text decoration, letter spacing, text transform | warning, `property_not_admitted` (the typography stage owns them) | |
| | layout properties (`width`, `margin`, `padding`, `display`, `position`, `font-size` and the rest) | warning, `property_not_admitted`: layout belongs to forms and stage L | |

`inherit` is admitted only on `color` and `font` (the T3 scope: inheritance
only for text and colour roles); elsewhere it is a bad value. `initial`
means the built-in recipe's value.

### 1.8 Motion

- **`transition`** (shorthand and longhands): the property (`opacity`,
  `scale`, `translate`, `face`, or an animated parameter), the duration in
  `ms` or `s` (C2: durations stay in time), the timing function and the
  delay. The engine clamps every duration to its 400 ms ceiling.
- **Timing functions**: `linear`, `ease`, `ease-in-out` (smoothstep),
  `cubic-bezier()`, `steps(n, jump-start | jump-end)`, and the engine's
  `follow(<time constant>)`, `spring(<response>, <damping>)`,
  `decay(<time constant>)` (C2). `ease-in` and `ease-out` match the
  lower to their exact CSS `cubic-bezier()` (R2-17).
- **States** come only from published states (C1). The compiler builds a
  part's presentational state machine (`PropertyTransition`) from the rule
  that declares the transition and the rules that refine it: the same
  selector with more states, declaring the same property. Their states are
  ordered most precedent first, because the engine's machine takes the
  first state that matches. The first version groups only these
  refinements (R2-18): another rule's state value, such as
  `.primary:hover { scale: 1.05 }` under a transition declared on `button`,
  gets its own instant machine. Joining curves and state values across
  rules is a later engine phase, if themes need it.
- **`@keyframes`** with `animation` on a state rule: `from` becomes the
  state's `enter_from`, the stops between become `PresentationState::stops`
  (at most 4; more is an error), and `to` becomes the target.
  `animation-iteration-count: infinite` becomes an `AmbientEffect` whose
  period is the duration, with the keyframes compiled to an expression of
  `time_in_state`; its frame rate and resting pose are engine-named
  (`animation-frame-rate`, at most 30; `animation-rest-pose`, a time within
  the period). `animation-direction` and `animation-fill-mode` other than
  their defaults are errors in the first version.
- **Reduced motion** is the engine's (C3): nothing in a theme can opt out.
  `@media (prefers-reduced-motion: reduce)` lets a theme choose its own calm
  look on top.

### 1.9 `@media`, `@import`, `@keyframes`

- **`@media`** (D1): `prefers-color-scheme: light | dark`,
  `forced-colors: active | none`, `prefers-reduced-motion: reduce |
  no-preference`, and `resolution` with `min-` and `max-` (in `dppx` or
  `x`). Conditions join with `and`; a comma list expands the block once per
  query. `not`, `only`, `or` and media types other than `all` and `screen`
  are errors. An unknown media feature is a warning (the block is skipped).
  The condition is carried on each rule and evaluated per window, and parts
  are re-resolved on a change (EC3, R2-19).
- **`@import "<installed theme name>";`** (D2): resolved only among
  installed packs whose hashes check; no paths, no URLs, no network. The
  imported theme's rules come before the importing sheet's, as in CSS. A
  name not installed, a hash that fails, or a cycle is an error. At most 16
  imports (E2), counted over the whole import closure of one theme, each
  installed theme once (R2-20).
- **`@keyframes <name>`**: names are unique per sheet; a second definition
  replaces the first, as in CSS. An `animation` naming no defined keyframes
  is a bad value (error).

### 1.10 Inline styles

Inline styles use the same tokenizer, parser and lowering (E1). The text is
a declaration list for the control, optionally followed by part blocks with
the control's own selector implied:

```css
background: role(accent background); border-radius: 6px !important;
::part(face):hover { scale: 1.04; transition: scale 90ms ease; }
```

Sources: the control's C++ call and the form document's `style` attribute.
The control owns its compiled inline style; it survives theme replacement.
A defect in it refuses that inline style only, with the control's previous
inline style kept. The C++ entry point is
`Control::set_inline_style(std::string_view)` (R2-10); WinForms'
`set_style(ControlStyles, bool)` is unchanged.

### 1.11 Custom properties through the cascade (EC7)

R2-05: a theme's custom properties are resolved per part by the same
cascade as properties, so a restyle sheet's `button { --radius: 8px; }`
changes buttons only, and every rule that reads `var(--radius)` on a
button's parts follows. A selector with no part (`button`) matches every
part of the control, so no inheritance between parts is needed; custom
properties do not inherit from one control to another (inheritance stays
only for text and colour roles).

- **Compiled form.** A custom property definition is a declaration of its
  own kind: a name, its tokens, its level (`!important` splits it as any
  declaration). A declaration that reads `var()` is kept as a deferred
  declaration: its tokens, the custom properties it reads, and the value
  kind its property needs.
- **Checked at compile time** (B2: bad values fail). Every definition of a
  name visible to the compilation is known, so the compiler substitutes
  each candidate definition (and the fallback) into each deferred
  declaration that reads it and lowers the result: a candidate that gives
  a bad value, an expression over budget or more than 4,096 tokens is an
  error, as it would be written out. Candidates multiply when one value
  reads several names: at most 256 combinations per declaration; more is
  an error (R2-addendum 1).
- **Resolved per part.** The part's cascade picks the winning definition of
  each name, then substitutes into the winning deferred declarations and
  lowers them with the same lowering code. Resolution runs on discrete
  changes (states, theme, media, inline style), never per frame, and the
  lowered result is cached by the identities of the winning definitions,
  so parts that resolve alike lower once.
- **Cache.** The part raster cache already keys on resolved values
  (parameters and, after EC1, the material assembly), so deferred values
  need no new key: two parts whose `--radius` resolves alike share a bake.
- **Across compilations** (R2-addendum 2). An inline style compiles apart
  from the theme. When it is compiled, each of its custom property
  definitions is checked against every deferred declaration of the active
  theme that reads the name and could match the control's parts. A
  definition that gives one of them a bad value refuses the inline style as
  an error: `Control::set_inline_style` returns an error result, the
  diagnostic (`variable_kind_mismatch`, naming both sources) reaches the
  console, the control keeps its previous inline style, and the theme stays
  active and unchanged. Nothing is half-applied. A form document's `style`
  attribute follows the same rule. The reverse case is refused the same
  way: a theme installed later (including by live switch) whose deferred
  declarations read an existing inline definition as the wrong type is
  refused for that program, the previous theme stays, and the error names
  the inline style's control and property; another program without that
  inline style accepts the same theme. The owner's reason: it keeps "bad
  values fail", because the newly added piece is the one at fault, and a
  program designer's inline rules are honoured.

## 2. Diagnostics

Every diagnostic has:

- a **severity**: `error` (the theme, or the inline style, is refused) or
  `warning` (one declaration, or one rule, is dropped);
- a **code**: a stable number and snake_case name, such as
  `E403 value_out_of_range`, for search and tests;
- a **location**: the file (the sheet's name inside its package, such as
  `aurora/main.theme.css`; `inline:<control path>` for `set_style`; the form
  document's path for a `style` attribute), the byte offset, and the line
  and column, 1-based, the column counted in Unicode scalar values;
- the **rule** (the rule's label: its selector text as written, and its
  line) and the **property**, where there is one;
- one **detail** argument (the unknown name, the limit, the defect).

They are structured records (`ThemeDiagnostic`) returned with the
compilation, so tests and the designer (V) read them as data. Each is also
written once to the log on standard error in the library's trace form, so a
developer sees it in the console (B2):

```text
gui-forms-theme=error|code:E403 value_out_of_range|file:aurora/main.theme.css|line:12|column:9|rule:button:active::part(face)|property:scale|detail:3
```

The classes (proposed codes; the numbering is the record, names may be
refined in P1):

| Class | Severity | Codes |
|---|---|---|
| E1xx lexical | error | `invalid_utf8` (E101), `nul_byte`, `unterminated_string`, `unterminated_comment`, `escape_not_admitted`, `bad_url`, `token_not_admitted`, `number_too_precise`, `number_out_of_range` (E109); numbered in this order in P1 |
| E2xx structure | error | `unexpected_token` (E201), `unbalanced_block`, `declaration_outside_rule`, `import_after_rules`, `important_in_keyframes`, `nesting_not_admitted` (E206); numbered in this order in P2 |
| E3xx selector | error | `selector_syntax` (E301), `combinator_not_admitted`, `pseudo_class_not_admitted` (`:not()` and the rest), `attribute_selector_not_admitted` (E304); numbered in this order in P3 |
| E4xx value | error | `value_syntax`, `value_wrong_kind`, `value_out_of_range`, `unit_not_admitted`, `value_form_not_admitted`, `keyword_unknown`, `too_many_layers`, `expression_invalid` (with the engine's `ExpressionDefect`), `variable_undefined`, `variable_cycle`, `keyframes_undefined`, `transition_invalid`, `resource_unknown`, `variable_kind_mismatch` |
| E5xx limit | error | `sheet_too_large`, `too_many_rules`, `too_many_declarations`, `nesting_too_deep`, `string_too_long`, `too_many_imports`, `expression_over_budget`, `too_many_variable_combinations`, and the derived limits of section 6 |
| E6xx package | error | `manifest_invalid`, `sheet_hash_mismatch`, `import_not_installed`, `import_hash_mismatch`, `import_cycle`, `published_value_invalid` |
| E7xx reserved | error | `reserved_input_set` |
| W1xx unknown name | warning | `unknown_property`, `unknown_primitive`, `unknown_function`, `unknown_at_rule` (W104, P2), `unknown_media_feature`, `unknown_selector_name`, `property_not_admitted`; numbered W101 to W107 in this order; then `primitive_over_budget` W108 and `primitive_cost_unbounded` W109 (P5) |
| W2xx colour | warning | `literal_colour_unremappable` |
| W3xx version | warning | `needs_newer_engine` (the sheet or a rule), `keyword_from_newer_engine`; W301 and W302 |
| W4xx inline | warning | `inline_with_theme`, once per window (E1) |

The engine's own `RuleDiagnosticKind`s still run after T3 (the engine
compiles what T3 hands it). T3 checks the same conditions first, so for a
parsed theme they map onto the classes above: the value defects
(`parameter_out_of_range`, `parameter_wrong_kind`, `parameter_not_finite`,
`primitive_value_not_a_name`, `expression_invalid`,
`expression_reads_colour`, `transition_*`, `ambient_*`, `blend_invalid`) are
E4xx; `unknown_primitive` and `unknown_parameter` are W1xx;
`needs_newer_engine` is W3xx. `level_not_authored` and
`selector_missing_level_field` cannot arise from text and are tested as
never reached. `duplicate_declaration` does not arise: as in CSS, the later
of two declarations of one property in one rule wins, with no diagnostic.
The engine's refusals that a theme author cannot fix
(`primitive_over_budget`, `primitive_cost_unbounded`) are W1xx warnings,
and `primitive_role_mismatch` is an E4xx error (R2-08).

At most 1,000 diagnostic records are kept per compilation; the rest are
counted (a derived limit, section 6).

## 3. Data flow

```text
package bytes (manifest JSON + sheets + resources)
   |  manifest read and checked (D3); every hash checked; imports resolved (D2)
   v
sheet text (UTF-8, at most 1 MiB)              borrowed for the compilation
   |  tokenize
   v
ThemeTokenBuffer                                caller-owned, reused
   (kind + ByteOffset + ByteLength per token, contiguous over every byte)
   |  parse (iterative, explicit stack of 32)
   v
ThemeSyntaxTree                                 caller-owned, reused
   (nodes: sheet, rule, selector, declaration, at-rule, value; each a
    TokenRange; whitespace and comments stay where they are)
   |  serialize(tree) == text   <- the byte-preserving round trip
   |  lower: selectors, values, shorthands, var(), !important, motion
   v
ThemeRuleSheet + authored material table + diagnostics
   (the engine's ThemeRule / RuleSelector / RuleDeclaration records)
   |  any error: stop here; nothing is built; the running theme stays
   v
CompiledThemeRules::compile_with_built_in(sheet, registry)
   |
   v
Theme (immutable) --> Window::set_theme / ApplicationResources::set_theme
   (atomic replacement; focus, capture, selection and edits survive, RC003)
```

- Each stage fills storage its caller owns and sizes once: the token buffer
  holds at most one token per byte, so a 1 MiB sheet needs at most about
  12 MiB of tokens; the tree's node table is bounded by the token count. A
  compilation of many sheets reuses the same buffers.
- The tree keeps bytes, not meaning: the lowering reads it and writes the
  engine's records. Serializing an untouched tree concatenates its tokens'
  bytes, so it returns the input exactly. A later editor (V, D) replaces
  one node's token range and copies every other byte.
- Nothing reaches the engine until the whole package has compiled without an
  error, so a refused theme never half-applies (B2). The previous `Theme`
  object stays installed, or the built-in theme where none was.

## 4. The cascade with inline styles and `!important`

The mapping and the engine change are written out in the README under E1.
In short, lowest first: built-in; theme normal (`theme_sheet`,
`class_rule`, `stable_id_rule`, by selector); inline normal
(`inline_style`); theme important (new `important_theme_sheet`); forced
high contrast for everything but colour; inline important, non-colour (new
`important_inline_style`); forced high contrast for colour, over all. P9
builds it.

## 5. Phases

Each phase is one to three PRs on main, each with its own gate; stacks stay
at most two deep. Code lives in `src/core/theme_language/` (one substantial
type per directory: `source_text`, `number`, `tokenizer`, `syntax_tree`, `selector`,
`value`, `lowering`, `custom_properties`, `diagnostics`, `package`),
internal until stage V
(E3). The documented seam is one public entry point,
`include/gui_forms/theme/theme_compiler/theme_compiler.hpp`: compile a
package, get a `Theme` or a refusal, with the diagnostics.

**P1. Source text, tokens and diagnostics.** The wrapper types
(`ByteOffset`, `ByteLength`, `TokenIndex`, `LineNumber`, `ColumnNumber`),
the line-start table, the tokenizer into a caller-owned buffer, the
`ThemeDiagnostic` record and its log line, the E1xx codes and the size and
string limits. The number conversion is checked on all three CI hosts
(`std::from_chars` for `double` where every standard library has it, or a
first-party correctly rounded conversion). The fuzz target starts here:
`fuzz/theme_sheet_fuzz.cpp`, standalone mode as a test beside
`png_registry_fuzz.cpp`. Gate: a token fixture per kind; every lexical
defect with its line and column; tokens cover the input exactly. 2 PRs.
*Done in one PR:* the conversion is first-party (no repository code uses
`from_chars` for `double`): Clinger's fast path, else exact long division
with ties to even, checked against fixed bit patterns on every host and
against the host's `strtod`. Diagnostics keep the first 1,000 records and
count the rest.

**P2. The syntax tree and the round trip.** The parser (rules, declaration
lists, at-rule blocks, `!important`), the nesting, rule and declaration
limits, the E2xx codes, and the serializer. Shared with FD. Gate: the
round-trip corpus (every fixture, plus real-world CSS files' formatting:
comments everywhere, CRLF, a byte-order mark, no final newline) returns
identical bytes; every structural defect is located. 2 PRs.
*Done in one PR:* the tree stores its nodes in document order, each
followed by its descendants and carrying its subtree's end, so no node
links to another; each node is a token range plus a content range. The
nesting limit counts every open `{`, `(`, `[` and function, a rule's
block included. At-rule names and `!important` match ASCII
case-insensitively, as in CSS. An unknown at-rule (W104) is kept with its
block unparsed, and like any item other than `@import` it ends the
imports. An empty value parses; P4 judges it. The parser stops at the
first error and closes the nodes it has open there, so even a stopped
tree writes back the sheet. In the code, a warning's enumerator is 1000
plus its number, keeping W104 apart from E104.

**P3. Selectors.** Every form of section 1.4, names looked up through the
schema (`registered_name`) and `manifests/control-schema.json`, standard
pseudo-classes, `:is()` and selector-list expansion, level assignment, and
EC2 (**engine change**: `ThemeRule` may carry its specificity). Gate: one
conformance fixture per selector form and per refused form; specificity and
level of each. 1 to 2 PRs.
*Done in one PR:*
- **EC2:** `ThemeRule::specificity`, when set, replaces the computed one.
  A class or stable-ID rule with a given specificity needs a class or an
  ID in that specificity rather than in its selector. An inline style still
  needs its selector's stable ID.
- **Level from specificity:** a rule's level comes from its specificity, so
  `:is(#a, button)` puts its `button` expansion at `stable_id_rule` with the
  ID's specificity. This keeps the README's reason for the three levels: they
  order exactly as specificity does. Taken from the selector instead, they
  would contradict CSS's `:is()`.
- **Vocabulary:** the lowering takes the vocabulary as the caller's span of
  `ControlAnatomy` records. A part named with a type must be declared by
  that type (its own parts or its item systems'); a part named without a
  type must be declared by some type.
- **Spelling:** type, part, role and state names are lower-case kebab-case,
  so `track_bar` or `Button` names nothing. Pseudo-class names fold ASCII
  case, as in CSS.
- **Unknown names (W106):** an unknown name drops the whole written rule, as
  CSS drops a rule with an invalid selector, and reading goes on, so a later
  error still refuses.
- **Refused names:** a fixed list of CSS pseudo-classes (`:not()`, `:where()`,
  `:has()`, the structural ones and others) and pseudo-elements
  (`::before` and others) is E303. Any other name is unknown.
- **Limits:** E509 is the 32-argument `:is()` limit. E510
  `selector_too_long` is a new derived limit: at most 32 simple selectors in
  one expanded selector. It bounds the expansion to a constant per rule. It
  is a proposed starting value under R2-14, to confirm with the owner.
  Expansion counts against the rule budget (E502).

**P4. Values and expressions.** Numbers and units (em parsed, lowered in
P8), colour literals, named colours and `color-mix()`, custom property
definitions and `var()` parsed with the substitution limits and the
compile-time candidate check of section 1.11, reserved inputs (E7xx), and
expressions compiled to `ExpressionInstruction` programs with constant
folding. Until P12 resolves custom properties per part, a `var()` with
exactly one candidate definition is substituted at compile time, and one
with several is refused with a temporary `not_yet_built` error, so no
interim meaning ships. Gate: value fixtures for every form; budget overruns
are errors; folded and unfolded programs give the same bits. 2 PRs.

*Done in one PR* (one PR per phase, owner, 2026-10-25):
- **Codes.** The E4xx codes it raises take their numbers in the order of
  section 2's table: `value_syntax` E401, `value_wrong_kind` E402,
  `value_out_of_range` E403, `unit_not_admitted` E404,
  `value_form_not_admitted` E405, `keyword_unknown` E406 (defined here for
  the layout stage's use, raised from P5), `expression_invalid` E408,
  `variable_undefined` E409, `variable_cycle` E410 and `resource_unknown`
  E413. E407, E411, E412 and E414 keep their numbers for the phases that
  raise them. `unknown_property` W101 and `property_not_admitted` W107 are
  defined here too, for the layout stage, and raised from P5. The
  diagnostics source is the single definition of the code list; other
  stages raise its codes and define none of their own. Also `expression_over_budget` E507,
  `too_many_custom_properties` E511, `substitution_too_long` E512,
  `reserved_input_set` E701, and the temporary `not_yet_built` E999, which
  P12 removes.
- **Custom properties.** One table per sheet holds every definition in a
  style rule (inside `@media` too), sorted by name. A `var()` is replaced by
  its name's single definition, a reserved input, or its fallback. The
  4,096-token bound counts every token the substitution passes over once:
  whitespace and comments, each `var()`'s own tokens and a fallback it
  skips. So its work is bounded however the definitions nest, and a cycle
  is found by a flag per name rather than a search. Until P12, two definitions of one name anywhere in the sheet (in
  one rule too) refuse the `var()` that reads it, so the candidate check
  of section 1.11 has only one candidate to check.
- **Values.** A value is a flat list of components in written order, each
  function followed by its arguments. rgb(), rgba(), hsl(), hsla() and
  color-mix() become colours as they close; each rounds to 8-bit channels
  half up. color-mix() follows CSS Color 5: premultiplied interpolation in
  srgb, the percentage normalisation, and a sum below 100% scaling the
  alpha; other spaces are E405. A named colour stays a keyword, which
  `named_colour()` resolves. Other functions stay generic for the
  lowering. These are E405: the other colour spaces, relative colours,
  the CSS math functions outside section 1.6, `attr()`, `env()` and
  similar. An empty value is E401.
- **Expressions** compile to the engine's postfix programs in one pass,
  with precedence and an explicit stack. CSS's rules hold: `+` and `-`
  need whitespace on both sides; units must agree for `+`, `-`, `min()`,
  `max()` and comparisons; a product has one plain-number factor; a
  quotient's divisor is a plain number or has the dividend's unit (giving a
  plain number). `clamp(MIN, VAL, MAX)` compiles to `max(MIN, min(VAL,
  MAX))`, as CSS defines it, not to the engine's clamp, which differs when
  MIN exceeds MAX. The engine inputs' units: the pointer px,
  `--time-in-state` seconds, the scales and published values plain numbers.
  Each constant operation is folded with the engine's `EngineExpression` as
  it is applied, unless it gives the fallback. A value that reads nothing
  is a number; one whose evaluation gives the fallback (`calc(1px / 0)`)
  is E403. An unknown name in `state()` or `param()` is E401 for now; P5
  decides whether a W1xx warning suits it better.
- The fuzz harness also substitutes and reads every value of a complete
  tree: a refusal adds one error and keeps nothing; a read value nests,
  its programs compile, and a second read gives the same value.

**P5. Lowering onto today's engine.** `primitive`, `shape`, `blend`,
primitive parameters by name, static `opacity`, `scale` and `translate`,
the unknown-name warnings and the version warnings (W1xx, W3xx). This is
the first end-to-end compile: a sheet in, a `Theme` out, through
`Window::set_theme`. Gate: a sheet equal to an existing C++-authored test
theme compiles to equal `ThemeRuleSheet` records, and the parts it draws
hash equal. 1 PR.

*Done in one PR:*
- **The compiler** (`compiler/`, internal, E3) runs the stages in order
  and lowers each style rule once: its selectors, then its `primitive`
  declarations (the last usable one selects the primitive, so a parameter
  may be written before it), then its other declarations. Within a rule
  the later declaration of a property wins, in its place, with no
  diagnostic. Each expanded selector gets one copy of the rule's records,
  labelled with the selector list as written (clipped to 64 bytes); a
  rule left with nothing to declare gives no rule. A given specificity
  (EC2) is set only where CSS's differs from the engine's own count.
  The first error refuses the sheet, which is then empty.
- **Properties.** A parameter is written by its registered name in
  kebab-case: a number takes a plain number or px, or an expression (its
  fallback the parameter's default; `param()` of a colour or a choice is
  E402); a colour takes a colour or a named colour; a choice takes its
  keyword in kebab-case, matched in any case (`shape: cap-start`).
  `opacity` (a number or a percentage, 0 to 1), `scale` (one number or
  percentage, or `none`; one per axis is E405) and `translate` (one or two
  px lengths, or `none`; a percentage or a depth is E405) become one-state
  machines reached at once; out-of-range values are E403, not clamped as
  CSS clamps opacity, so a refusal is never silent. `mix-blend-mode`
  lowers to `blend`; CSS's other modes are E405. `isolation` is accepted
  and adds nothing. `initial` is the default (the built-in recipe for
  `primitive`, `built-in` naming it too); `inherit` is E402 until P8;
  `unset`, `revert` and `revert-layer` are E405. An expression reading an
  input in a presentational property is E405 until motion (P7). An
  unknown name in `state()` or `param()` stays E401 (P4's question): the
  states are the engine's fixed vocabulary and the parameters the rule's
  own primitive's, so an unknown one is a mistake in the sheet, not a name
  from a newer engine.
- **Refused until their phases** (E999): `!important` (P9), `@import`
  (P10), `@media` and `@keyframes` (P7), and the properties of P6 to P8:
  the `background`, `border`, `outline`, `mask`, `transition` and
  `animation` families, `box-shadow`, `text-shadow`, `filter`,
  `backdrop-filter`, `surface-finish`, `glow`, `glow-placement`, `color`
  and `font`.
- **Warnings.** An unknown property is W101 and a known CSS property the
  language leaves out W107 (a table of layout, typography and geometry
  properties, and the `column`, `flex`, `font`, `grid`, `inset`, `margin`,
  `overflow`, `overscroll`, `padding`, `scroll`, `text-decoration` and
  `transform` families); neither value is read. A parameter with no
  `primitive` in its rule is W101. An unknown primitive is W102 and its
  parameters are dropped with it, silently. A generic function as a value
  is W103. The engine's budget refusals take their own codes,
  `primitive_over_budget` W108 and `primitive_cost_unbounded` W109,
  appended to the W1xx row. A role the primitive cannot paint is E402
  (R2-08).
- **Engine versions.** When the declared engine (the manifest's, P9;
  until then the caller's) is newer than the running one, W301 is reported
  once (detail `major.minor`), a keyword the running engine lacks is W302
  and dropped, and the sheet is handed to the engine at the running
  version, so the rest applies (B3, R2-07). Otherwise the declared version
  is kept.
- **Lowered size** (a new derived limit, `lowered_sheet_too_large` E513,
  for owner confirmation): the records a sheet lowers to count at most
  16 MiB, estimated as 64 bytes a record plus the bytes of its strings and
  16 a program instruction, the selectors (P3) included. Without it a
  selector list expanding into 10,000 rules, each copying a long stable ID
  or 64 expression declarations, would build gigabytes from a 1 MiB sheet.
- Gate: the sheet `tests/theme_language/theme_compiler_tests.cpp` writes
  equals its hand-built `ThemeRuleSheet` (a bulge with an expression
  reading the pointer and a custom property, colours, a blend and a static
  opacity; a hover rule with a later-wins parameter, a scale and a
  translation; a material with a recipe and a pill shape), and the two
  themes' windows draw identical part rasters, offsets and groups at rest,
  hot at two pointer positions, and away. Every sheet the tests compile
  also compiles in the engine with no diagnostic.

**P6. Authored materials (engine change EC1) and their CSS.** The engine
first: a rule's material fields cascade one by one, as parameters do, and
the material primitive bakes the material assembled from the winning
fields, cached by that assembly's identity (R2-02). A field no rule sets
keeps the part's built-in recipe value, the cascade's floor. Then the CSS
of CSS_COVERAGE §1 to §3, §5, §7 to §11 and §14 `color`, with the
shorthands. Gate: a fixture per row; for each, a sheet and the equivalent
C++ material bake to identical pixels; the material cache's counters show
one bake per distinct assembly. 3 PRs.

*P6a, the engine (EC1), done:*
- **Fields.** A `ThemeRule` carries `materials`, a list of
  `MaterialDeclaration`s, each one `MaterialField` and its value:
  `background_colour`, `fills` (the image layers), `fill_blend_modes`,
  `shadows`, `glows`, the border's twelve longhands (each edge's style,
  width and colour), `keylines`, the four corners (`corner_top_left` to
  `corner_bottom_left`, in CSS's order), `colour_filter`, `drop_shadow`,
  `filter_blur`, `finish` and `mask`. The fields are CSS's longhands, so
  each cascades as CSS cascades it. The border cascades by CSS longhand, so that `border-color` in one rule and `border-width`
  in another combine as CSS has them: the compiler (P6b) lowers `border`
  and its shorthands to the longhands. The fills are two fields (owner
  C6, 2026-10-26), CSS's `background-color` and `background-image`,
  which combine across rules as CSS combines them: the colour layer
  under the image layers, composed at assembly. The floor's bottom layer,
  when solid, is its colour and its other layers its image layers; a
  colour of alpha 0 adds no layer, and with no layer at all the fill is
  one transparent solid. `fill_blend_modes` is `background-blend-mode`:
  the modes in CSS's order (the top image layer's first), repeated over
  the image layers; the colour layer keeps normal.
- **Corners.** Each corner cascades on its own, as `border-top-left-radius`
  does; the compiler lowers `border-radius` to the four. At assembly each
  corner is its declaration's, else the floor's. Four equal corners are
  `corner_radius` alone; unequal ones are `corner_radii`, with
  `corner_radius` the smallest, which the built-in path rounds every
  corner by.
- **`initial`** (`MaterialDeclaration::initial`) keeps the floor's value
  and still wins over the rules beneath it.
- **Percentage radii** (R2-11): a corner marked `relative` is a
  fraction of the box's shorter side, resolved when the material is
  assembled for a part.
- **Checking.** Each field is checked alone, written into the default
  material, by `valid_surface_material`; one no material admits is the new
  engine diagnostic `material_field_invalid`, and a second declaration of
  a field in one rule is `duplicate_declaration`. Fields count as
  declarations, so a rule of fields alone is not empty.
- **Cascade.** Each field cascades on its own over every matching rule,
  whatever primitive the rule selects (`Resolution::materials`, the
  winning declaration of each field). Fields change a part only where its
  primitive draws a theme material.
- **Assembly.** The floor is the material of the recipe the primitive
  names, for the part's state. `assemble_material` writes the winning
  fields over it. The edges assemble together: four equal edges are one
  uniform border, which keeps a dash and rounded corners; unequal ones are
  single edges, which a material admits only with square corners. Each
  edge starts as the floor's (an edge the floor lacks has the default
  border's width and colour and is not drawn), takes its declared style,
  width and colour, and is drawn unless its style is none or its width 0.
  An
  assembly no material admits (eight keylines and a finish, say) is
  refused by the primitive and drawn by the built-in recipe, the refusal
  counted, as any refusal is.
- **Cache.** A keyed part keeps its assembly on its record
  (`AssembledPartMaterial`), assembled again only when its identity
  changes: the compiled rules' serial and the winning declarations'
  addresses, the floor by value (a recipe may be replaced and another
  occupy its address), and the box's shorter side. A copy of compiled rules takes a new serial; a move
  keeps it. A part without a key assembles at each paint.
  `engine_material_assemblies` counts assemblies.
- **The family recipe.** The material primitive's `recipe` gains a last
  choice, `family`, the part's own recipe family, and it is the default:
  a material rule naming no recipe now keeps its part's own recipe as its
  floor, not the button's (owner C5, 2026-10-26).
  `EnginePrimitive::theme_material` takes the part's family for it. The
  engine version is 0.3.
- **Implicit material** (owner C7, 2026-10-26; built in P6b): a rule
  that declares material fields and no `primitive` selects `material`.
- Gate: `gui_forms_engine_authored_material_tests`. A part with fields
  from two rules bakes the same pixels as the same material written into
  the theme's recipe in C++, in two states; a colour and image layers
  set in different rules, with the colour's rule the less and then the
  more specific, bake as the same layers written in C++. A repaint assembles and bakes
  nothing; a state change assembles and bakes once. The checks, the
  per-field cascade, the edge assembly, the serials, the family default and
  the refused assembly are tested too.

*P6b, the CSS of fills, borders, radii and shadows, done:*
- **The lowering** (`src/core/theme_language/material/`) reads a material
  property's value into a result record (`MaterialLowering`): the fields
  it sets, or the problem the compiler reports. Each CSS longhand is one
  field, so a shorthand sets several, and within a rule the later
  declaration of a field wins, whichever property set it. A shorthand's
  omitted part is `initial` (the floor's value), the language's reading of
  CSS's initial value. `initial` on any of them resets each field it sets.
- **Fills** (§1, §7): `background-color`; `background-image` with
  `linear-`, `radial-` and `conic-gradient()` and their `repeating-`
  forms, up to 8 layers (E407 `too_many_layers` past them), CSS's first
  layer on top; `background` with a colour (transparent when it names
  none) and images; `background-blend-mode`. A layer's position, size,
  repeat, attachment, origin and clip, and image resources, are P6c's
  (E999). A colour hint, a `none` among other layers, a three- or
  four-value position and CSS's blend modes the engine lacks are E405;
  `currentcolor` and `em` wait for P8 (E999).
- **Authored gradients** (`AuthoredGradient`, engine): a gradient as
  written, resolved as CSS places it for the part's box: the stops placed
  by CSS's fixup and clipped to the drawn range, sides and corners, the
  four radial extents for circles and ellipses, positions and radii as a
  fraction of the box plus logical units, repeating periods (a radial
  one rotated to begin at its centre), CSS's average colour for a period
  of no length. Coincident stops (CSS's hard stops) are 2^-20 apart, which
  the engine's stops require: under a tenth of a device pixel on a
  gradient shorter than 100,000 device pixels. A gradient the same for
  every box is resolved when the sheet compiles, in normalized
  coordinates or a CSS angle, exactly the layer C++ writes; the others
  (a corner, a circle, a length) when the material is assembled, in
  logical units. The assembly keys on the box only where a field reads
  it (`material_fields_read_box`), so a resize assembles nothing new
  otherwise.
- **Borders** (§2): `border`, the four edges' shorthands, `border-style`,
  `border-width` and `border-color` (one to four values) and the twelve
  longhands; `thin`, `medium` and `thick` are 1, 3 and 5 px; `none` and
  `hidden` draw no line. `double`, `groove` and `ridge` are P6c's keyline
  pairs (E999); `inset` and `outset` are E405.
- **Radii** (§2): `border-radius` (one to four) and the four corners'
  longhands; a percentage is a fraction of the shorter side (R2-11);
  elliptical radii are E405.
- **Shadows** (§3): `box-shadow`, `none` or up to 4, `inset` admitted,
  CSS's first on top; a shadow without a colour waits for `currentcolor`
  (E999).
- **Implicit material** (owner C7): a rule with material fields and no
  `primitive` selects `material`.
- Gate: `gui_forms_theme_material_css_tests`. Each row (the fill forms,
  the blend mode, a uniform, a dashed and rounded, single-edge and
  four-value border, per-corner and percentage radii, outer and inset
  shadows) compiles from a sheet and bakes the pixels of the same
  material written in C++, which differ from the floor's. The gradient
  resolution, the lowering of each property and 28 refusals are tested
  too.

*P6c, filters, finish, glows and gradient masks, done:*
- **Filters** (§5): `filter: none` or a list of CSS's functions. The
  engine applies a material's filters in one order (the colour filter,
  the drop shadow beneath, the blur), so a list lowers when that order
  draws it. The colour functions compose into one matrix in source order
  (CSS's amounts, percentages admitted, negative amounts E403); `blur()`s
  combine in quadrature into `filter_blur` = 2σ; one `drop-shadow()`,
  with a colour (without one it waits for `currentcolor`, E999), whose
  colour the colour functions written after it also turn, rounded to 8
  bits. A `drop-shadow()` after `blur()`, a second `drop-shadow()`,
  `opacity()` after `drop-shadow()` (the engine's shadow follows the
  filtered alpha, so it would fade twice) and an SVG filter reference are
  E405. The colour functions are alpha-preserving affine maps of straight
  colour (opacity a linear one), which commute with compositing and with
  the blur; the order the engine draws differs from CSS's only where a
  function clamps, as one matrix already differs from CSS's per-function
  clamping (`MaterialColourMatrix`).
- **Finish** (§8): `surface-finish`, 0 to 1 or a percentage.
- **Glows** (§9): `glow: none` or up to 2 of a colour with a blur and an
  optional spread (E407 past them); `glow-placement: [under | over]#`, a
  new field (`glow_placements`) applied over the assembled glows in their
  written order and repeated over them as CSS repeats a short list, so a
  placement alone re-places the floor's glows.
- **Masks** (§11): `mask-image: none` or one gradient (E407 for more; an
  image resource is P6d's, E999), resolved like a fill's: when the sheet
  compiles when it is fixed, else for the part's box at assembly. The
  mask is the engine's (E9.6): outside the box it extends at its edge
  value, so a shadow fades with the face beside it, where CSS's default
  `mask-clip` would clip the shadow away.
- Gate: seven more rows in `gui_forms_theme_material_css_tests` (colour
  functions, combined blurs, a drop shadow turned by a later function, a
  finish, two placed glows, a fixed and a sized mask) bake the pixels of
  the same C++ material, plus the lowering of each property and 21 more
  refusals.

*P6d, image layers, done:*
- **Binding** (`ThemeImageResource`, in `ThemeCompileInput::images`): an
  image resource the manifest lists is bound to the engine's image, its
  pixel size and its density (source pixels per logical unit, its own
  size). P10's loader binds them; until then the caller does. A listed
  resource with no image is the wrong kind (E402).
- **Images** (§10): `url("resource:...")` in `background-image`,
  `background` and `mask-image` is the bound image, tiled at its own size
  from the box's corner: CSS's initial size, repetition and position.
- **Placement**: `background-position`, `background-size` and
  `background-repeat` are three new fields (`fill_positions`,
  `fill_sizes`, `fill_repeats`), cascaded apart and composed at assembly
  as the blend modes are: in CSS's order, repeated over the image layers,
  read by the image fills. The engine draws a gradient layer over the
  whole box, so these do not move or size a gradient; the `background`
  shorthand refuses a gradient layer written with them (E405), and the
  longhands leave a gradient as it is.
  - A position is CSS's of one or two values: its percentage is the
    fill's anchor in the room the box leaves around the image (CSS's
    reading), within 0 to 100% (E403 past them), its length the offset.
  - A size keeps the image's proportions: `auto`, one length or
    percentage across (`<l>` or `<l> auto`) or down (`auto <l>`),
    `cover`, `contain`, and of two lengths only `100% 100%`, which
    stretches the image over the box; other pairs and a zero size are
    E405. A size of the box is resolved at assembly. A size the engine's
    image scale cannot draw (1/8 to 8 source pixels a unit) makes the
    assembled material invalid, so the part draws its built-in recipe.
  - A repetition is `repeat` or `no-repeat` on both axes; the latter is
    the engine's new `MaterialImageMode::single`, one tile placed as a
    tile is and nothing around it (the material primitive and the
    built-in path both draw it). `repeat-x`, `repeat-y`, `space` and
    `round` are E405.
  - A layer's attachment, origin and clip are E405: the engine draws
    every layer over the whole box.
- Masks take an image the same way, tiled at its own size; the mask's own
  size, repetition and position longhands are not built (E999).
- Gate: six more rows (a tiled image, a placed single image of a set
  size over a colour, `contain` centred, a single image half the box
  wide, a stretched image, an image mask) bake the pixels of the same C++
  material, with the image in the window's resources; plus the lowering
  of each property, `cover` at assembly, nine more refusals, and the
  single mode's pixels in the engine's own image test.

*P6e, border lines and border images, done:*
- **Lines** (§2): `double`, `groove` and `ridge` are a solid edge drawn
  as two keylines in place of the border edge (`MaterialEdgeLines`, a
  style field's second member, so each edge's style still cascades as
  one field). A double edge is two lines a third of its width, a third
  apart; a groove or ridge two halves, one in the edge's colour and one
  in its shade, each channel halved and rounded down (CSS leaves the
  shades to the engine): a groove's outer half is the shade on the top
  and left and the colour on the bottom and right, a ridge's the
  reverse. The pairs follow the declared keylines in CSS's edge order,
  so the corner square two edges share shows the later edge's line; they
  count toward the material's 8 keylines, past which the assembled
  material is invalid and the part draws its built-in recipe. The
  keylines are clipped to a rounded box, as keylines are.
- **Border images** (§2): `border-image: none` or a bound image with
  `<slice>{1,4} fill / <width>{1,4}` and optionally `stretch`, a new
  field (`border_image`) holding the engine's nine-patch, drawn as the
  top fill layer in place of the border (its edges and their lines), as
  CSS draws a border image in place of the border's style. The
  nine-patch fills its middle and stretches at one scale, so the
  language admits what it draws: `fill` and the widths written (`auto`,
  the slices at the image's own size, or lengths), each width in the
  same proportion to its slice; slices are image pixels or percentages
  of the image. A missing `fill` or width, a width in border widths, a
  gradient source, an outset and `repeat`, `round` and `space` are
  E405; the longhands wait (E999).
- Gate: four more rows (a double border, a groove border, one ridge
  edge, a border image over a declared border) bake the pixels of the
  same C++ material; plus the lowering of the styles and of a
  percentage-sliced border image, and seven more refusals.

*Still to build in P6:* `backdrop-filter` to the glass primitive, the
sheen's pointer gradient, and §14's `color`, `outline` and
`text-shadow`, which need a carrier for the recipe's own fields (owner
decision pending, T3 track).

**P7. Motion.** `transition`, the timing functions, delays, `@keyframes`,
`animation`, ambient loops, and the state-machine grouping. Gate: a fixture
per timing function and keyframe form; the pop scenario of the README runs
from a sheet with the same frames as the C++ rules in
`gui_forms_engine_motion_curve_tests`; zero bakes while a transition runs
(E7r's counter). 2 PRs.

**P8. Colour roles, em, text roles and `@media` (engine changes EC3, EC4,
EC5).** Colour roles as vocabulary atoms and the theme's `:root` palette
(R2-03); colour values that remember their role; the high-contrast
remap by role, or by position for a literal; the text-role size input and
em; `font: <role>`; media conditions on rules, evaluated per window and
re-resolved on a change (a discrete event: nothing per frame). Gate:
role and literal fixtures under forced high contrast; the
`literal_colour_unremappable` warning; em at two text scales; one sheet
switching light and dark, and two windows at different scales from one
theme. 3 PRs.

**P9. Inline styles and `!important` (engine change EC6).** The two
important levels, the split of important declarations, the colour-only
high-contrast pass that keeps inline important non-colour declarations,
control-owned inline styles, `Control::set_inline_style`, and
`inline_with_theme` (once per window).
FD's `style` attribute calls the same entry point when FD lands. Gate: a
cascade fixture for each case of E1 (neither important, theme only, inline
only, both), each under normal and forced high contrast; a refused inline
style keeps the previous one. 2 PRs.

**P10. Packages, imports and atomic replacement.** The manifest (a bounded
JSON reader under `src/core/`, fuzzed, in the pattern of the XML reader),
SHA-256 content hashes, first-party with the standard test vectors
(R2-12), the installed-pack index, `@import`
closure, and refusal semantics through the public entry point. Gate:
package fixtures for each E6xx code; a refused theme leaves the running
`Theme` object installed and every window's focus, capture, selection and
active edit unchanged; a good swap preserves them too (RC003). 2 to 3 PRs.

**P12. Custom properties through the cascade (engine change EC7).** The
engine first: custom property definitions and deferred declarations in the
compiled rules, the per-part resolution of section 1.11 with `!important`
under E1, and the cache of lowered results by winning definitions. Then
the compiler drops P4's temporary refusal. Gate: the owner's restyle
example (a second sheet's `button { --radius: 8px; }` changes buttons and
nothing else, through every rule reading `var(--radius)`); an inline
`!important` definition beating a theme's; a counter showing one lowering
per distinct resolution and none while a transition runs; candidate
errors at compile time. 2 PRs.

**P11. The gate.** Complete the conformance corpus and its coverage table,
the malformed and hostile suite, a recorded fuzz run, and the user
documentation of the language (`docs/user/theme-language.md`). Record under
`results/<date>-t3-theme-language-gate/`. 1 to 2 PRs.

Order: P1, P2, P3 and P4 in sequence; P5 after P4; P6, P7 and P8 after P5,
in any order (P8 last is simplest, since colour roles touch P6's fields);
P9 after P5; P10 after P5; P12 after P6 and P9 (it resolves material
fields and needs the important levels); P11 last. FD can start from P2.

## 6. The final gate

Per the brief, recorded under `results/`:

1. **Conformance fixtures** for every selector form (section 1.4), every
   property form (each row of section 1.7, which follows CSS_COVERAGE), every
   value form (section 1.6), every at-rule form, and `!important` at both
   sources. Each fixture is a sheet, its expected compiled records or its
   expected diagnostics, and for drawing rows a reference hash. A coverage
   table lists each form and its fixture, and fails CI when a form has none.
2. **Malformed input**: one fixture per error and warning code, checking the
   code, the line, the column and that the running theme is untouched.
3. **Hostile input**: each owner limit (E2) at its value passes and one past
   it is refused; and the derived limits, accepted as starting values
   (R2-14):
   at most 32 arguments in one `:is()`; at most 1,024 custom properties per
   sheet; a substituted value at most 4,096 tokens (no expansion
   explosion through nested `var()`); at most 256 candidate combinations
   per declaration that reads `var()` (R2-addendum 1); at most 64
   `@keyframes` per sheet; at most 1,000 diagnostic records kept. Inputs built to be slow (deep
   nesting at the limit, maximal expansions) finish within a fixed count of
   operations per input byte, measured by a counter, never by time.
4. **Fuzzing**: `fuzz/theme_sheet_fuzz.cpp`, built standalone as a ctest
   test on every host (like `png_registry_fuzz.cpp`) and under libFuzzer
   where the compiler has it, with a seed corpus in
   `fuzz/corpus/theme_sheet/`. Invariants checked on every input: no
   sanitizer report; tokens cover the input in order; parsing the same
   bytes twice gives equal results; a sheet that parses round-trips to
   identical bytes; every limit holds. The recorded run states its input
   count and corpus, not a duration.
5. **The byte-preserving round trip** of every untouched fixture and of the
   T4 default theme's sheets once they exist.

## 7. House-style points

- **Caller-owned tables** (§4): the tokenizer, parser, lowering and
  diagnostics each fill storage their caller owns and sizes once; nothing
  allocates per token or per node in the inner loops.
- **Distinct types** (§1): `ByteOffset`, `ByteLength`, `TokenIndex`,
  `NodeIndex`, `LineNumber` and `ColumnNumber` are wrapper types; a byte
  offset is never passed as a column, and the column counts scalar values,
  not bytes.
- **No lambdas** (§2, §10): the parser's states are a named enumeration and
  a loop over the token buffer; predicates (sorting rules, matching names)
  are named function objects.
- **Bounded loops** (§3): every loop is over a buffer whose length was
  validated up front, or an explicit stack of fixed depth; no recursion
  follows the input. Limits are checked before the storage they bound is
  touched.
- **Failure states** (§9): a refused compilation leaves the caller's
  previous theme, and the caller's buffers hold nothing that the next
  compilation reads; an error never leaves a partial `ThemeRuleSheet`
  reachable.
- **Effort, not time**: every limit is a count (bytes, tokens, rules,
  nesting, instructions, imports); no wall-clock timeout anywhere in the
  compiler. Only perceptual durations in the language (transitions,
  keyframes) are in time (C2).
- **Spelling** (§10): no `auto`, `->`, structured bindings or `= default`
  comparisons; `(*pointer).member`.
- **Foreign data** (§9): the manifest and sheets are untrusted input; the
  JSON reader states its limits as the XML reader does.

## 8. Decisions and open questions

### 8.1 Decided in round 002

The 21 questions of the first version are decided; the README records each
with the owner's words. In short:

| | Decision |
|---|---|
| R2-01 | `.theme.css` sheets, a `theme.json` manifest, the dialect named in the manifest's `format` field; `text/css` on the wire |
| R2-02 | each material field cascades on its own; the engine assembles the part's material, with the built-in recipe as the floor (EC1) |
| R2-03 | §5.4's pairs become `ColourRole` atoms; the palette is a `:root` block |
| R2-04 | `::role(<role>)` |
| R2-05 | custom properties resolve per part through the full cascade, `!important` included (EC7, P12) |
| R2-06 | published values a theme reads are listed in the manifest |
| R2-07 | engine-named properties are plain names; an unknown keyword is an error, or a warning under a newer declared engine |
| R2-08 | unknown selector names warn and drop the rule; `primitive_role_mismatch` errors; the budget refusals warn; refused properties warn (`property_not_admitted`) |
| R2-09 | theme `!important` does not survive forced high contrast |
| R2-10 | inline styles: declarations plus part blocks, owned by the control; `Control::set_inline_style(std::string_view)` |
| R2-11 | a percentage radius is of the shorter side |
| R2-12 | SHA-256, first-party |
| R2-13 | custom control states deferred (an unknown name for now) |
| R2-14 | the derived limits of section 6 |
| R2-15 | selector lists admitted, each selector with its own specificity |
| R2-16 | escapes only inside strings |
| R2-17 | `ease-in` and `ease-out` lower to their exact `cubic-bezier()` |
| R2-18 | a transition groups the refinements of its own rule only; joining across rules is a later engine phase |
| R2-19 | `@media` conditions evaluated per window |
| R2-20 | the 16 imports counted over the whole import closure |
| R2-21 | the smaller set, with every CSS named colour admitted as a literal |

### 8.2 Decided in the round 002 addendum

R2-05 raised one question the first version did not have. The owner
answered it on 2026-10-25 (README, "Round 002 addendum"):

- **R2-addendum 1.** At most 256 candidate combinations per declaration
  that reads `var()`; beyond that is an error
  (`too_many_variable_combinations`).
- **R2-addendum 2.** An inline style whose custom property the active theme
  reads as the wrong type is refused as an error: `set_inline_style`
  returns an error result, the diagnostic reaches the console, the theme
  stays active and unchanged, and nothing is half-applied. The form
  document's `style` attribute follows the same rule. This is the plan's
  option (d), not its recommended fallback (c).
- **R2-addendum 3.** The reverse case: a newly installed theme (including
  by live switch) that reads, as the wrong type, a custom property an
  existing inline style defines is refused for that program. The previous
  theme stays, and the error names the conflicting inline style (control
  and property). Another program without that inline style accepts the
  same theme.

The question as it was put:

1. **Checking deferred custom properties.** Inside one compilation every
   candidate definition is checked (section 1.11). Two things remain.
   First, the candidates a value can take multiply when it reads several
   names: (a) bound the product at 256 per declaration, beyond which the
   declaration is an error (a derived limit, like R2-14's), or (b) check
   each name's candidates on their own and accept that a combination may
   fail at resolution. Second, an inline style is compiled apart from the
   theme, so its definition of `--radius` cannot be checked against a theme
   installed later: (c) at resolution, a deferred declaration whose winning
   substitution is a bad value falls back to the next declaration down the
   cascade (finally the built-in recipe), is counted, and the later of the
   two compilations reports a warning, `variable_kind_mismatch`, naming
   both sources; or (d) refuse the later compilation as an error.
   **Recommend (a) and (c)**: (a) keeps every value in one compilation
   checked before anything runs, as B2 wants, and (c) keeps a theme swap
   from being refused because of an application's inline style, which the
   theme author cannot see.

## 9. Size

Twelve phases, P1 to P12, of about 22 to 26 PRs in all: P1 2, P2 2, P3 1
to 2, P4 2, P5 1, P6 3, P7 2, P8 3, P9 2, P10 2 to 3, P12 2, P11 1 to 2.
Of them, about 9 change the engine (EC1 to EC7) and run T2's tests with
T3's. Round 002 added P12 (EC7, 2 PRs); no other phase changed size. No
timing is claimed.
