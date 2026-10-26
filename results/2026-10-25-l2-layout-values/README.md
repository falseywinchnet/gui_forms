# Stage L, L2: the layout value parser (2026-10-25)

Phase L2 of `planning/stages/L-layout/PLAN.md`: the admitted declarations of
sections 2.2 to 2.5, read over the theme language's tokens (T3 P1) into
typed style records, with exact diagnostics for refused forms. No container
reads the records yet; L3 to L5 do.

## What L2 adds

- `include/gui_forms/layout/style/layout_style.hpp` (public): the typed
  records. `LengthPercentage` (a length and a percentage summed, so linear
  `calc()` is exact), `SizeValue`, `AutoLengthPercentage`, `BoxSides`,
  `DisplayKind`, `PositionKind`, `LayoutAlignment` and `BoxAlignment`,
  `FlexDirection`, `FlexWrap`, `FlexFactor` and `FractionFactor` (unitless
  wrapper types, never added to lengths), the flex container and item
  records, the grid track, track list, template area, auto-flow and line
  records, `TextAlignment`, `InlineVerticalAlignment`, and `LayoutStyle`.
  Every field starts at its CSS initial value.
- `src/core/layout/style_reader/` (private, like the theme language, T3
  decision E3): `LayoutStyleReader::read_inline_style`, which applies an
  inline style's declarations on top of a given style, and
  `describe_layout_style`, the canonical longhand description the tests
  compare against.
- `tests/layout_style_tests.cpp` (`gui_forms_layout_style_tests`).

## Diagnostics

L defines no codes. The reader uses the theme language's single code list
(`src/core/theme_language/diagnostics/theme_diagnostics.hpp`, owned by T3),
which gained E406 `keyword_unknown`, W101 `unknown_property` and W107
`property_not_admitted` for this phase (PR #332):

| Code | When |
| --- | --- |
| E1xx | A lexical defect, from the tokenizer; the read ends. |
| E201 | A declaration that is not `<property>: <value>`, or a brace in a value. |
| E202 | An unclosed or unmatched parenthesis or bracket. |
| E401 | A malformed value: extra or missing components, `calc()` without spaces around `+` or `-`, a non-rectangular or ragged `grid-template-areas`, a nested or second automatic `repeat()`. |
| E402 | The wrong kind: a length where a number belongs, a non-integer `order` or line, a unitless non-zero length, a fraction as a track minimum, an automatic repeat beside intrinsic or flexible tracks, mixed `calc()` operands. |
| E403 | Out of range: a negative padding, size, gap, factor or fraction; line 0; `span 0`; an `order` beyond 32 bits; more than 10,000 tracks or `grid-template-areas` rows; division by zero. |
| E404 | Any unit but `px` (and `fr` in tracks). |
| E405 | A CSS form L leaves out: `inline-flex`, `inline-grid`, `contents`, the table and other display types, two-keyword `display`, `fixed` and `sticky`, `min()`, `max()`, `clamp()`, `var()`, `env()`, `attr()`, `!important`, the global keywords, `content-box`, `stretch` and bare `fit-content`, content keywords in `flex-basis`, `subgrid`, `masonry`, named grid lines, `left`/`right`, `safe`/`unsafe`, `first`/`last` baselines, `legacy`, `justify`, length `vertical-align`. |
| E406 | A keyword the property does not define. |
| E503 | More than 64 declarations. |
| E504 | Brackets nested past 32. |
| W101 | An unknown property, custom properties and vendor prefixes included; the declaration is dropped. |
| W107 | A CSS layout property L leaves out (`float`, `grid`, `grid-template`, `aspect-ratio`, logical properties, `overflow` and others); the declaration is dropped. |

An error refuses the whole style and leaves the caller's style unchanged
(T3 decision B2). The read continues past value errors so each is reported;
a structural error (E201, E202, E503, E504) ends it, since what follows has
no reliable declaration boundary. Each diagnostic names the file
(`inline:<control>`), line and column of the offending token, the property
as written, and the offending text or the reason.

## Decisions within the plan

- The `flex` shorthand reads CSS's whole grammar (`<grow> <shrink>? ||
  <basis>`, a unitless zero after both factors being the basis). PLAN 2.2
  lists the common forms; the others (`flex: 30px`, `flex: 1 0`) fill the
  same record, so leaving them out would refuse valid CSS for nothing.
- `grid-template-areas` is bounded by the track limit (at most 10,000 rows,
  E403) and, for columns, by the tokenizer's 4 KB string limit (E505, so at
  most 2,048 cells a row), not by the 1,048,576-cell occupancy limit of
  PLAN section 5, which is auto-placement's: a 1 MB inline style cannot
  write that many cells, so a cell limit would be unreachable.
- A `calc()` result is kept as written; a negative size from `calc()` is
  clamped where it is used, as CSS does. Literal negatives are E403.
- `box-sizing: border-box` is admitted and records nothing, since every
  GUI.Forms box is a border box (PLAN 2.5).
- A lone `<n>fr` track is CSS's `minmax(auto, <n>fr)`.
- A style that mixes theme declarations (`color`) with layout ones reports
  W101 for them. Separating the two is FD's job when form documents carry
  both.

## Gate

PLAN L2: one conformance fixture per property and per refused form.

`gui_forms_layout_style_tests`, argument `tests/layout_corpus`:

- 69 accepted cases. Each is read, described canonically and compared with
  its expected text, then read back from that text to an equal style. A
  check requires every one of the 58 admitted properties to be written by
  some case.
- 104 refused cases, each with the exact code, line, column, property and
  detail of its one diagnostic, and the style left unchanged: E401 19,
  E402 15, E403 12, E404 5, E405 33, E406 12, E201 3, E202 3, and one
  lexical (E103).
- Every value error in a style is reported (three in one style, on two
  lines); the five warnings drop their declarations and the rest apply.
- Limits: 64 declarations admitted and the 65th refused (E503); 32 levels
  of brackets admitted and 33 refused (E504); `repeat(10000, 1px)` expands
  to 10,000 tracks and one more track is refused; 10,000 template rows
  admitted and 10,001 refused; a 2,048-cell row admitted and a longer
  string refused (E505).
- The layout corpus: all 165 `style` attributes of its 26 fixtures read
  with no diagnostic.
- Mutation: each accepted case with each byte deleted, and each byte
  replaced by each of 14 structural characters (29,850 reads, 24,834
  refused). Every refused read leaves the style exactly as it was, and no
  applied read carries an error.

Commands (Windows, LLVM 22, the workspace's Release tree and its
sanitizer tree):

```text
cmake --build .build/native-windows-x64/gui-forms --target gui_forms_layout_style_tests
gui_forms_layout_style_tests tests/layout_corpus
sh sanitize.sh <workspace> 'layout_style|layout_corpus'
python3 -B tools/check_house_style.py include src tests tools
python3 -B tools/check_lifetime_rules.py include src tests tools
python3 -B tools/generate_library_docs.py --check
```
