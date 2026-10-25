# Stage T3 plan: the theme language front end

Status 2026-10-25: proposed, for the owner's review. No parser code exists
yet. The decisions it builds on are in [`README.md`](README.md), "Owner
decisions, interview round 001 (2026-10-25)", cited here by their letters
(A1 to E3). Where this plan and a decision disagree, the decision governs.
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

T3 is mostly front end, but the decisions need six things the engine
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
| `whitespace`, `comment` | kept as tokens, so the tree can reproduce every byte |
| `ident`, `function` (an ident followed by `(`), `at_keyword`, `hash` | names; escapes are not admitted outside strings (proposed, open question 16) |
| `string` | `"..."` or `'...'`, at most 4 KiB (E2); an unterminated string is an error |
| `number`, `percentage`, `dimension` | the number is converted once, correctly rounded, to `double` |
| `delim`, `colon`, `semicolon`, `comma`, `[`, `]`, `(`, `)`, `{`, `}` | |
| `url` | only as the function `url("resource:<name>")` with a quoted string (E8c decision on inline images, option A); an unquoted `url(...)` token is an error |

`<!--`, `-->` and `\` outside a string are errors. A token is a kind and a
byte range; its line and column are computed from a line-start table only
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
| part role | `::role(face)` | `role` (proposed, open question 4) |
| custom state | `:state(drop-accepted)` | `states` |
| standard state | `:hover` `:active` `:focus` `:focus-visible` `:checked` `:indeterminate` `:disabled` `:enabled` `:read-only` `:invalid` `:open` `:default` | `states`: `hot`, `pressed`, `focused`, `focus_visible`, `checked`, `indeterminate`, `disabled`, `enabled`, `read_only`, `invalid`, `open`, `preselect` |
| either of | `button:is(:hover, :focus-visible)::part(face)` | expanded: one rule per argument |
| selector list | `button, check-box { ... }` | expanded: one rule per selector (proposed, open question 15) |

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
  that drops the rule (B3; open question 8 asks the owner to confirm this
  reading).
- Specificity is the engine's (`RuleSpecificity`): stable IDs, classes,
  control types, parts, states.

### 1.5 `!important`

`!important` follows a declaration's value, as in CSS. The compiler splits a
rule's important declarations into a sibling rule at the important level
(README, E1 mapping; EC6). Inside `@keyframes` it is an error, as in CSS.

### 1.6 Values

- **Numbers and units** (A3): plain numbers; `px` (logical units); `%` only
  where CSS defines percentages (gradient stops, colour alpha, opacity,
  `background-position`, `background-size`, keyframe offsets, and radii, see
  open question 11); `em`, relative to the part's text-role size (EC5);
  `ms` and `s` for durations; `deg`, `rad`, `turn` for angles; `dppx` and
  `x` in `@media (resolution)`. `rem`, `vw`, `vh`, `vmin`, `vmax`, `ch`,
  `ex` and every other unit are errors.
- **Colours** (A4):
  - roles first: `role(<role>)` or `role(<pair> foreground)` and
    `role(<pair> background)` (open question 3 has the vocabulary and the
    form);
  - literals: `#rgb`, `#rgba`, `#rrggbb`, `#rrggbbaa`, `rgb()`, `rgba()`,
    `hsl()`, `hsla()` (both comma and space forms), `transparent`, and
    `color-mix(in srgb, a p%, b)`. Named colours other than `transparent`
    and `currentcolor` are not admitted (proposed; they are a large table
    with no relation to the role model).
  - A literal in a text or edge position (`color`, `border-color`, keyline
    and ring colours) is a warning, `literal_colour_unremappable`: forced
    high contrast replaces it with its position's high-contrast colour,
    losing the relation the author meant. The declaration still applies.
- **Custom properties** (A5): `--name: <tokens>` and `var(--name)`,
  `var(--name, fallback)`. They are substituted at compile time (open
  question 5 has the scope). Engine inputs are reserved names:
  `--pointer-x`, `--pointer-y`, `--time-in-state`, `--display-scale`,
  `--text-scale`, and every published application value the manifest
  declares (open question 6). A theme that declares one of them is an error.
  A `var()` of an undefined name with no fallback is a bad value (error),
  and so is a cycle.
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
  (open question 7).

### 1.7 Lowering CSS names onto the engine

Property names are CSS's where CSS has the property (A1), written kebab-case;
engine-named properties fill what CSS lacks, unprefixed as in the agreed
scenario (`shape: circle`; open question 7). Shorthands are expanded into
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
  nearest engine easing (CSS_COVERAGE "nearest-easing matching"); open
  question 17 asks whether to lower them to their exact `cubic-bezier()`
  instead.
- **States** come only from published states (C1). The compiler builds a
  part's presentational state machine (`PropertyTransition`) from the rule
  that declares the transition and the rules that refine it: the same
  selector with more states, declaring the same property. Their states are
  ordered most precedent first, because the engine's machine takes the
  first state that matches. The limitation and the alternative are open
  question 18.
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
  The condition is carried on each rule and evaluated per window (EC3; open
  question 19).
- **`@import "<installed theme name>";`** (D2): resolved only among
  installed packs whose hashes check; no paths, no URLs, no network. The
  imported theme's rules come before the importing sheet's, as in CSS. A
  name not installed, a hash that fails, or a cycle is an error. At most 16
  imports (E2), counted over the whole import closure of one theme, each
  installed theme once (open question 20).
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
inline style kept. The form, the C++ name (`Control::set_style` already
exists for WinForms' `ControlStyles`) and the ownership are open question 10.

## 2. Diagnostics

Every diagnostic has:

- a **severity**: `error` (the theme, or the inline style, is refused) or
  `warning` (one declaration, or one rule, is dropped);
- a **code**: a stable number and snake_case name, such as
  `E401 value_out_of_range`, for search and tests;
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
gui-forms-theme=error|code:E401 value_out_of_range|file:aurora/main.theme.css|line:12|column:9|rule:button:active::part(face)|property:scale|detail:3
```

The classes (proposed codes; the numbering is the record, names may be
refined in P1):

| Class | Severity | Codes |
|---|---|---|
| E1xx lexical | error | `invalid_utf8`, `nul_byte`, `unterminated_string`, `unterminated_comment`, `escape_not_admitted`, `bad_url`, `token_not_admitted` |
| E2xx structure | error | `unexpected_token`, `unbalanced_block`, `declaration_outside_rule`, `import_after_rules`, `important_in_keyframes`, `nesting_not_admitted` |
| E3xx selector | error | `selector_syntax`, `combinator_not_admitted`, `pseudo_class_not_admitted` (`:not()` and the rest), `attribute_selector_not_admitted` |
| E4xx value | error | `value_syntax`, `value_wrong_kind`, `value_out_of_range`, `unit_not_admitted`, `value_form_not_admitted`, `keyword_unknown`, `too_many_layers`, `expression_invalid` (with the engine's `ExpressionDefect`), `variable_undefined`, `variable_cycle`, `keyframes_undefined`, `transition_invalid`, `resource_unknown` |
| E5xx limit | error | `sheet_too_large`, `too_many_rules`, `too_many_declarations`, `nesting_too_deep`, `string_too_long`, `too_many_imports`, `expression_over_budget`, and the derived limits of section 6 |
| E6xx package | error | `manifest_invalid`, `sheet_hash_mismatch`, `import_not_installed`, `import_hash_mismatch`, `import_cycle`, `published_value_invalid` |
| E7xx reserved | error | `reserved_input_set` |
| W1xx unknown name | warning | `unknown_property`, `unknown_primitive`, `unknown_function`, `unknown_at_rule`, `unknown_media_feature`, `unknown_selector_name`, `property_not_admitted` |
| W2xx colour | warning | `literal_colour_unremappable` |
| W3xx version | warning | `needs_newer_engine` (the sheet or a rule), `keyword_from_newer_engine` |
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
(`primitive_over_budget`, `primitive_cost_unbounded`) and
`primitive_role_mismatch` are open question 8.

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
type per directory: `source_text`, `tokenizer`, `syntax_tree`, `selector`,
`value`, `lowering`, `diagnostics`, `package`), internal until stage V
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

**P2. The syntax tree and the round trip.** The parser (rules, declaration
lists, at-rule blocks, `!important`), the nesting, rule and declaration
limits, the E2xx codes, and the serializer. Shared with FD. Gate: the
round-trip corpus (every fixture, plus real-world CSS files' formatting:
comments everywhere, CRLF, a byte-order mark, no final newline) returns
identical bytes; every structural defect is located. 2 PRs.

**P3. Selectors.** Every form of section 1.4, names looked up through the
schema (`registered_name`) and `manifests/control-schema.json`, standard
pseudo-classes, `:is()` and selector-list expansion, level assignment, and
EC2 (**engine change**: `ThemeRule` may carry its specificity). Gate: one
conformance fixture per selector form and per refused form; specificity and
level of each. 1 to 2 PRs.

**P4. Values and expressions.** Numbers and units (em parsed, lowered in
P8), colour literals and `color-mix()`, custom properties with the
substitution limits, reserved inputs (E7xx), and expressions compiled to
`ExpressionInstruction` programs with constant folding. Gate: value
fixtures for every form; budget overruns are errors; folded and unfolded
programs give the same bits. 2 PRs.

**P5. Lowering onto today's engine.** `primitive`, `shape`, `blend`,
primitive parameters by name, static `opacity`, `scale` and `translate`,
the unknown-name warnings and the version warnings (W1xx, W3xx). This is
the first end-to-end compile: a sheet in, a `Theme` out, through
`Window::set_theme`. Gate: a sheet equal to an existing C++-authored test
theme compiles to equal `ThemeRuleSheet` records, and the parts it draws
hash equal. 1 PR.

**P6. Authored materials (engine change EC1) and their CSS.** The engine
first: a rule's material fields cascade one by one, as parameters do, and
the material primitive bakes the material assembled from the winning
fields, cached by that assembly's identity (open question 2). Then the CSS
of CSS_COVERAGE §1 to §3, §5, §7 to §11 and §14 `color`, with the
shorthands. Gate: a fixture per row; for each, a sheet and the equivalent
C++ material bake to identical pixels; the material cache's counters show
one bake per distinct assembly. 3 PRs.

**P7. Motion.** `transition`, the timing functions, delays, `@keyframes`,
`animation`, ambient loops, and the state-machine grouping. Gate: a fixture
per timing function and keyframe form; the pop scenario of the README runs
from a sheet with the same frames as the C++ rules in
`gui_forms_engine_motion_curve_tests`; zero bakes while a transition runs
(E7r's counter). 2 PRs.

**P8. Colour roles, em, text roles and `@media` (engine changes EC3, EC4,
EC5).** Colour roles as vocabulary atoms and the theme's palette (open
question 3); colour values that remember their role; the high-contrast
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
control-owned inline styles, the C++ entry point, and `inline_with_theme`.
FD's `style` attribute calls the same entry point when FD lands. Gate: a
cascade fixture for each case of E1 (neither important, theme only, inline
only, both), each under normal and forced high contrast; a refused inline
style keeps the previous one. 2 PRs.

**P10. Packages, imports and atomic replacement.** The manifest (a bounded
JSON reader under `src/core/`, fuzzed, in the pattern of the XML reader),
content hashes (open question 12), the installed-pack index, `@import`
closure, and refusal semantics through the public entry point. Gate:
package fixtures for each E6xx code; a refused theme leaves the running
`Theme` object installed and every window's focus, capture, selection and
active edit unchanged; a good swap preserves them too (RC003). 2 to 3 PRs.

**P11. The gate.** Complete the conformance corpus and its coverage table,
the malformed and hostile suite, a recorded fuzz run, and the user
documentation of the language (`docs/user/theme-language.md`). Record under
`results/<date>-t3-theme-language-gate/`. 1 to 2 PRs.

Order: P1, P2, P3 and P4 in sequence; P5 after P4; P6, P7 and P8 after P5,
in any order (P8 last is simplest, since colour roles touch P6's fields);
P9 after P5; P10 after P5; P11 last. FD can start from P2.

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
   it is refused; and the derived limits, proposed here (open question 14):
   at most 32 arguments in one `:is()`; at most 1,024 custom properties per
   sheet; a substituted value at most 4,096 tokens (no expansion
   explosion through nested `var()`); at most 64 `@keyframes` per sheet; at
   most 1,000 diagnostic records kept. Inputs built to be slow (deep
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

## 8. Open questions

Numbered, each with a recommendation. The owner answers; nothing here is
decided.

1. **File extension and media type** (left open on 2026-10-13).
   (a) `.css`, `text/css`: every editor highlights it, but it says "browser
   CSS". (b) `.gftheme`, `text/x.gui-forms.theme`: unambiguous, no
   highlighting. (c) `.theme.css` for sheets, with the manifest's `format`
   field (`"gui-forms-theme/1"`) naming the dialect, `text/css` on the wire,
   and `theme.json` for the manifest. **Recommend (c)**: editors highlight
   it, the inner suffix says what it is, and the manifest lists its sheets,
   so nothing is discovered by extension. Inline styles and form documents
   need no extension.
2. **How a rule carries a material (EC1).** (a) Each material field
   cascades on its own, as parameters do, and the engine assembles the
   part's material from the winning fields, cached by the assembly's
   identity. (b) A rule carries a whole material, and the last matching
   rule's material wins whole, so a `:hover` rule restates every layer. (c)
   CSS writes into the theme's recipe table by recipe family and state.
   **Recommend (a)**: it is CSS's behaviour (a hover rule changes only its
   background); (b) surprises every CSS author, and (c) addresses families,
   not parts.
3. **Colour roles: vocabulary and form.** No colour-role atom exists in code
   (design language §5.4's set is proposed). (a) §5.4's pairs become
   `ColourRole` atoms; a theme's palette is a `:root` block of role
   declarations (`ink-on-paper: #1b2733 on #ffffff;`); `role(<pair>)`
   gives the foreground in `color` and the background in `background`, and
   `role(<pair> foreground | background)` picks one anywhere. (b) The same
   atoms, with the palette in the manifest JSON. (c) `role()` reads only the
   recipe's own colours (`text`, `muted-text`, `glyph`, `focus-ring`,
   `default-ring`, `accent(n)`), with no new atoms. **Recommend (a)**: A4
   says roles "and its pairs", the palette stays in the sheet the designer
   edits, and the atoms are a vocabulary decision the owner makes once.
4. **Addressing a part role.** Design language §8.2 binds appearance to
   (part role, state), and `RuleSelector::role` exists, but the decided
   selector set has no role form. (a) `::role(<role>)`. (b) `::part()`
   matches a name or a role. (c) No role form in the first version.
   **Recommend (a)**: explicit, and (b) is ambiguous where a part's name
   equals another role.
5. **The scope of a theme's custom properties.** (a) Global only: defined in
   `:root`, substituted everywhere. (b) Global plus rule-local: a rule's own
   `--x` shadows the global one inside that rule; no per-part cascade. (c)
   Full CSS: each part sees the cascaded value, resolved per part, which
   needs the engine to carry deferred values. **Recommend (b)** now, with
   (c) only if a real theme needs it; the scenario's `--glow` is (b).
6. **Declaring published values.** The theme must name the application
   values it reads (`ThemeRuleSheet::published_values`), and A5 makes them
   read-only. (a) The manifest lists them; `var(--level)` reads one. (b) A
   reserved prefix (`--app-level`). (c) Any `var()` the theme does not
   define is a published value. **Recommend (a)**: explicit and static; (c)
   turns a typo into a silent input.
7. **Engine-named properties and newer keywords.** (a) Unprefixed, as in the
   agreed scenario (`shape: circle`), versioned with the engine; a keyword
   the engine lacks is an error, or a warning when the sheet declares a
   newer engine. (b) A `-gf-` prefix on every engine-named property.
   **Recommend (a)**: it is what the owner saw; the warning rule keeps
   newer themes loadable on older engines (B3).
8. **Classifying what B2 does not name.** (a) An unknown control type, part,
   state or pseudo-class name is a warning that drops the rule (B3: "an
   unknown name"). (b) `primitive_role_mismatch` (a known primitive on a
   part role it cannot paint) is an error, a bad value. (c)
   `primitive_over_budget` and `primitive_cost_unbounded` are warnings: the
   author cannot fix a registry fact, and the part falls back as E8c
   decided. (d) A property the language refuses by decision (`clip-path`,
   `rotate`, layout properties) is a warning, `property_not_admitted`.
   **Recommend (a) to (d) as written.**
9. **Theme `!important` under forced high contrast.** (a) It does not
   survive the pass; only inline `!important` non-colour declarations do.
   (b) Theme `!important` non-colour declarations survive too.
   **Recommend (a)**: the owner's guarantee is to the program designer's
   inline rules.
10. **Inline styles: form, ownership and name.** (a) A declaration list,
    then optional part blocks with the control's selector implied (section
    1.10); the control owns the compiled result, so no stable ID is needed
    and it survives theme replacement; the `inline_with_theme` warning once
    per window. (b) Declarations only, applied to every part of the control.
    And the C++ name: `Control::set_style(std::string_view)` overloads the
    existing WinForms `set_style(ControlStyles, bool)`; `set_inline_style`
    does not. **Recommend (a)**, with the owner choosing the name;
    `set_inline_style` is clearer, `set_style` is what the owner wrote.
11. **Percentage radii.** The engine draws circular corners only. (a) A
    percentage is of the shorter side, so 50 % on any box is a pill (a
    stated difference from CSS's ellipse). (b) Percentage radii are a bad
    value. **Recommend (a)**: A3 admits percentages where CSS defines them,
    and the shape keywords already cover circles and pills exactly.
12. **Content hashes.** (a) SHA-256, first-party (no new dependency), with
    the standard test vectors. (b) BLAKE3. **Recommend (a)**: universal
    tooling can produce the manifest's hashes.
13. **Custom control states.** T1 decision 2 lets a custom control declare
    states, but `RuleSelector::states` holds only the closed `AnatomyState`
    set. (a) Defer: in the first version `:state()` of a custom state is an
    unknown name (a warning). (b) Extend the selector in P3. **Recommend
    (a)**, consistent with C1's first version.
14. **The derived limits** of section 6 (32 `:is()` arguments, 1,024 custom
    properties, 4,096 tokens per substituted value, 64 `@keyframes`, 1,000
    diagnostic records). They are not owner numbers. **Recommend accepting
    them** as starting values, each an error when exceeded, as E2's are.
15. **Selector lists** (`a, b { }`). B1 names `:is()` but not the comma.
    (a) Admit them, one rule per selector, each with its own specificity, as
    CSS does. (b) Refuse them; `:is()` covers the need. **Recommend (a)**:
    every CSS author writes them.
16. **Escapes.** (a) CSS escapes only inside strings; names are plain
    kebab-case ASCII. (b) Full CSS escapes everywhere. **Recommend (a)**:
    names are registered words (T1 decision 8), and escapes in names only
    hide them.
17. **`ease-in` and `ease-out`.** The engine has no easing of that name.
    (a) Lower them to their exact CSS `cubic-bezier()` (now present). (b)
    The nearest closed easing, as CSS_COVERAGE first proposed. **Recommend
    (a)**: exact, and no engine change.
18. **Transitions across rules.** CSS cascades a part's `transition` and its
    property values separately, so `.primary:hover { scale: 1.05 }` animates
    with a `transition` declared on `button`. The engine keeps a
    transition's states and curve in one record. (a) The first version
    groups only refinements of the transition rule's own selector (more
    states, same everything else); another rule's state value gets its own
    instant machine. (b) An engine change that cascades curves and state
    values separately and joins them per part. **Recommend (a)** first, and
    (b) as a later phase if themes need it.
19. **Where `@media` is evaluated (EC3).** (a) Each rule carries its
    condition and the engine evaluates it per window, re-resolving parts on
    a change. (b) The compiler compiles one rule set per environment and
    swaps them. **Recommend (a)**: windows share a theme but not a scale,
    and a change is a discrete event either way.
20. **Counting the 16 imports.** (a) Over the whole import closure of one
    theme, each installed theme counted once. (b) Per sheet. **Recommend
    (a)**: it bounds the total work, which is the limit's purpose.
21. **Smaller proposals, as one set.** Named colours other than
    `transparent` and `currentcolor` are not admitted; `initial` means the
    built-in recipe's value; `isolation` is accepted and changes nothing;
    `animation-direction` and `animation-fill-mode` other than their
    defaults are errors in the first version; the log line of section 2;
    `inline_with_theme` once per window. **Recommend accepting the set**;
    the owner may pull out any item.

## 9. Size

Eleven phases, P1 to P11, of about 20 to 24 PRs in all: P1 2, P2 2, P3 1
to 2, P4 2, P5 1, P6 3, P7 2, P8 3, P9 2, P10 2 to 3, P11 1 to 2. Of them,
about 7 change the engine (EC1 to EC6) and run T2's tests with T3's. No
timing is claimed.
