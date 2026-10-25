# Stage L plan: real dynamic layout (flex, grid, flow)

Status 2026-10-25: proposed, for the owner's review. No code until the
open questions in section 9 are answered. Authority:
`planning/FORWARD_PROGRAMME.md` §4 L and [`README.md`](README.md); where
this plan and a recorded owner decision disagree, the decision governs.

Decisions this plan builds on:

- README "Owner decisions, 2026-10-13": the fidelity oracle moves into
  this repository under `tools/`; Chromium is a test oracle only, never
  called by GUI.Forms at import or run time.
- Layout belongs to forms and the engine, never to themes. The owner,
  2026-10-12: "dynamic flexing layouts should be easier than they are.
  This is an engine task." T3 already refuses layout properties in themes
  (`property_not_admitted`, T3 PLAN §1.7).
- Interview GF006, option L4: synchronous state with a geometry read
  barrier, nested update scopes, bounded passes. GF008: application
  composition uses a GUI.Forms-owned flex algorithm; "flex" names resizing
  behaviour, not a promise to reproduce every CSS edge case.
- DK decision 5: stage L may adopt or replace DK's binary split solver.
- Standing rules: work is bounded by effort (counts), never wall-clock;
  our engine is the reference, never a host's layout; house style is
  mandatory.

## Contents

1. [What exists today](#1-what-exists-today)
2. [What L builds](#2-what-l-builds)
3. [The measure and arrange contract](#3-the-measure-and-arrange-contract)
4. [Right-to-left, display scale and text-driven sizes](#4-right-to-left-display-scale-and-text-driven-sizes)
5. [Bounded effort for large item counts](#5-bounded-effort-for-large-item-counts)
6. [The oracle](#6-the-oracle)
7. [Form documents (FD) and the interface builder (D)](#7-form-documents-fd-and-the-interface-builder-d)
8. [Phases, gates and the stage gate](#8-phases-gates-and-the-stage-gate)
9. [Open questions for the owner](#9-open-questions-for-the-owner)
10. [House-style points](#10-house-style-points)
11. [Size](#11-size)

## 1. What exists today

Read from the code on main (`d6111581`), not only from the docs.

- **The pass.** `Window::ensure_layout` runs measure, then arrange, over
  dirty subtrees, at most `maximum_layout_passes` (4) times per flush;
  further dirtiness records `bounded_pass_limit_hits`
  (`src/core/window/window.cpp`). Suspended subtrees are skipped without
  losing their dirtiness (M12P11). Layout traverses a snapshot of child
  identities and revalidates membership after each callback (M12P12).
- **The flags.** `Dirty` (`include/gui_forms/dirty.hpp`) has `measure` and
  `arrange` bits and named invalidation sets (`invalidation::bounds`,
  `text_content`, ...). Properties declare effects through
  `invalidate_declared`; GF007 makes an undeclared effect a development
  failure.
- **The contract.** `Size measure(Size available)` and
  `void arrange(Rect final_bounds)`; `arrange_self` plus
  `set_child_layout` assign parent-owned slots without touching the
  child's `requested_bounds`. `get_preferred_size(proposed)` maps a zero
  extent to a practical unbounded 1e9. The window discards the result of
  its own `measure` call, and containers call `measure` again through
  `preferred_child_size` during their own measure and arrange: there is no
  measure cache, so a child can be measured several times per pass.
- **No intrinsic-size queries.** A control cannot be asked for its
  min-content or max-content width. `Label::measure` wraps at the available
  width, which approximates "measure at a definite width" only.
- **Geometry** is `Rect`/`Size`/`Insets` of `double` in logical pixels
  (`include/gui_forms/types/geometry/geometry.hpp`); the window's `scale`
  converts to device pixels at paint.
- **Containers.** `Panel`/`GroupBox` (absolute plus Dock and Anchor;
  Dock backmost first). `FlowLayoutPanel` already has four directions,
  wrapping, breaks, item spacing, main alignment
  (start/center/end/space-between/around/evenly), cross alignment with
  stretch, and a per-child `flex_grow`, kept in per-property maps keyed by
  runtime id; it allocates per line (`std::vector<Line>` of
  `std::vector<Item>`) on every pass. `TableLayoutPanel` has absolute,
  percent and auto-size tracks, spans, growth styles and resolved-track
  inspection (`column_widths()`, `row_heights()`).
  `ResponsiveTrackPanel` has fixed, content and weighted tracks with
  priority collapse. `SplitContainer`, `MasterDetailView`, `ScaledPanel`.
- **Docking.** `solve_dock_split(span, ratio, first_minimum,
  second_minimum)` (`include/gui_forms/docking/dock_split_solver/`)
  divides one span between two children; `minimums_met` reports when both
  minimums cannot be kept (DK decision 23).
- **Right-to-left** exists only as `FlowDirection::right_to_left`. There
  is no `RightToLeft` property on `Control`, and Dock, Anchor and
  `TableLayoutPanel` do not mirror.
- **Readers.** The core has a first-party XML reader
  (`include/gui_forms/xml/xml_reader/`, DK decision 43). It has no JSON
  reader and no CSS tokenizer yet (T3 P1 builds the tokenizer).

The earlier fidelity oracle and its reports are not in this repository.
What this repository records of it is the summary in [`README.md`](README.md):
it compared a real Chromium layout against the GUI.Forms retained tree node
by node, kept structure, state, geometry, typography, material and raster
differences separate, and never folded a difference into a parity claim
(on the File Manager source: zero structure, state and material
mismatches over 119 nodes; 69 nodes over the geometry tolerance, 23 over
typography, 32 over raster). This plan designs the oracle of section 6
from that approach and from what is in this repository; it uses no code
or text from the earlier one.

## 2. What L builds

### 2.1 One layout library, three containers

The recommended shape (question 1) is one renderer-free layout library in
the core with three solvers, and three containers that realize them:

| Solver (core, `src/core/layout/`) | Container (controls) | Covers |
|---|---|---|
| `flex_solver` | `FlexLayoutPanel` | CSS flexbox subset, 2.2 |
| `grid_solver` | `GridLayoutPanel` | CSS grid subset, 2.3 |
| `block_flow_solver` | `BlockFlowPanel` | block and inline flow, 2.4 |

- The solvers take named input records (container style, one item record
  per child) and fill caller-owned output arrays (one `Rect` per item, plus
  a resolution record for inspection). They have no control, painter or
  host dependency and are tested headless, like `DockLayout`.
- The containers own the per-child layout style (one record per child,
  `FlexItemStyle`, `GridItemStyle`, keyed by runtime id and reconciled on
  removal, as `FlowLayoutPanel` does today), gather item measurements
  through the contract of section 3, call the solver, and assign slots with
  `set_child_layout`.
- Each container follows the W checklist: anatomy (no parts beyond the
  panel's own; resolved lines and tracks are inspection data, not parts),
  `SemanticRole::group` as the existing panels use, registry entry,
  Gallery specimen, docstring, schema.
- Item systems (ToolStrip, ObjectView, the ribbon) can call the solvers on
  records directly, without child controls (section 5).

### 2.2 Flex subset

Admitted (CSS Flexible Box Layout Level 1 semantics, with the CSS names):

- `flex-direction`: `row`, `row-reverse`, `column`, `column-reverse`.
- `flex-wrap`: `nowrap`, `wrap`, `wrap-reverse`.
- `flex-grow`, `flex-shrink`, `flex-basis` (`auto`, `content`, a length,
  a percentage); the `flex` shorthand (`none`, `auto`, `<grow>`,
  `<grow> <shrink> <basis>`) and `flex-flow`.
- The resolution algorithm of §9.7 (freezing, the min/max violation loop),
  which is bounded: each round freezes at least one item, so at most n
  rounds for n items on a line.
- Automatic minimum size: `min-width: auto` / `min-height: auto` resolves
  to the content-based minimum (§4.5), the reason sources write
  `min-width: 0` (question 3).
- `justify-content`: `flex-start`, `flex-end`, `center`, `space-between`,
  `space-around`, `space-evenly`, `start`, `end`.
- `align-items`, `align-self`: `stretch`, `flex-start`, `flex-end`,
  `center`, `baseline` (first baseline), `start`, `end`.
- `align-content`: the `justify-content` values plus `stretch`.
- `row-gap`, `column-gap`, `gap`.
- `order`, with a stable sort by (order, child index).
- `min-width`, `max-width`, `min-height`, `max-height` per item, mapped to
  the existing `Control` minimum and maximum sizes.
- Margins on items (the existing `margin`), including `auto` margins on the
  main axis, which absorb free space before `justify-content` (§8.1).

Left out, each reported precisely by FD and H rather than dropped:
`last baseline`, `safe`/`unsafe` alignment, `visibility: collapse` struts,
vertical writing modes, `aspect-ratio`, absolutely positioned children's
flex static position, and the intrinsic sizing of multi-line column flex
containers (where browsers themselves differ). A difference between a CSS
edge case and a principled GUI.Forms rule is recorded in the fixture with
its reason (README "How").

### 2.3 Grid subset

Admitted (CSS Grid Layout Level 1, with the CSS names):

- `grid-template-columns`, `grid-template-rows` with track sizes: length,
  percentage, `fr`, `auto`, `min-content`, `max-content`,
  `minmax(min, max)`, `fit-content(length)`; `repeat(n, ...)`,
  `repeat(auto-fill, ...)` and `repeat(auto-fit, ...)`.
- `grid-template-areas` with named areas, and `grid-area` naming one
  (question 15).
- Line-based placement: `grid-column`, `grid-row`,
  `grid-column-start/end`, `grid-row-start/end` with line numbers
  (negative included) and `span n`.
- Automatic placement: `grid-auto-flow` `row`, `column`, `dense`;
  implicit tracks sized by `grid-auto-rows`, `grid-auto-columns`.
- `row-gap`, `column-gap`, `gap`.
- `justify-items`, `align-items`, `justify-self`, `align-self`
  (`stretch`, `start`, `end`, `center`, `baseline`), `place-items`,
  `place-self`; `justify-content`, `align-content` for track alignment.
- The track sizing algorithm of §12.3 to §12.7, including spanning items'
  intrinsic contributions, with §12.1's single column-then-row-then-column
  repeat;
  no loop to convergence.

Left out: named grid lines, `subgrid`, masonry, `grid-template` and `grid`
shorthands, `order` inside grid auto-placement beyond the stable child
order, and the legacy `grid-gap` names.

The File Manager concept atlas, which H must import unchanged, uses only
admitted grid forms: counted on 2026-10-25, 36 `grid-template-columns`
declarations with `px`, `fr`, `minmax(0, 1fr)`, `minmax(px, fr)`,
`repeat(n, ...)` and one `repeat(auto-fill, minmax(86px, 1fr))`; 46 flex
containers, 69 gaps, 30 absolutely positioned boxes and 7 `calc()`.

### 2.4 Block and inline flow (for H)

`BlockFlowPanel` lays children out as a CSS block formatting context:

- block-level children stack on the block axis, `width: auto` fills the
  container, margins (with collapsing, question 4), min and max sizes;
- inline content: a run of inline-level children (text controls and
  atomic inline boxes, which are `inline-block` children and images) is
  broken into line boxes by the text engine's line-break opportunities,
  aligned by `text-align` (`start`, `end`, `left`, `right`, `center`;
  `justify` left out) and on their baselines (`vertical-align: baseline`,
  `middle`, `top`, `bottom`);
- text inside one text control stays that control's own layout; L does not
  split one control's text across another container's lines.

Left out: floats and clearance (question 5), `inline` boxes with borders
or padding broken across lines, `display: contents`, `table` display
types, `column` multicol, `white-space` beyond `normal`, `nowrap` and
`pre`.

### 2.5 Positioning and lengths, shared by all three

- `position: absolute` with `inset`, `left`, `top`, `right`, `bottom`,
  `width`, `height`: out of flow, placed in the container's padding box
  after the in-flow items (question 6 on the containing block).
  `position: relative` offsets an item after layout without moving its
  neighbours.
- Lengths: `px` (logical units), `%` of the containing box where CSS
  defines one, `fr` in grid tracks, the content keywords, and linear
  `calc()` of a length and a percentage (`calc(100% - 24px)`); `min()`,
  `max()` and `clamp()` left out of L (question 14).
- Box sizing: a GUI.Forms control's width and height are its border box
  (padding included), as `requested_bounds` is today. Fixtures set
  `box-sizing: border-box`; H converts content-box sources.
- Border widths are paint, owned by themes, and cannot move layout. A
  source's border width that should take space becomes padding in the form
  (the importer's job; the fixtures keep borders at zero except one group
  that measures exactly this mapping).

### 2.6 Flow and the Windows Forms containers

`FlowLayoutPanel` and `TableLayoutPanel` are the Windows Forms containers
and keep their documented behaviour: `FlowLayoutPanel` never shrinks
items, honours `FlowBreak` and `WrapContents`, and its directions map onto
flex `row`, `row-reverse`, `column` and `column-reverse` with `wrap`;
`TableLayoutPanel`'s percent tracks share the space left after absolute and
auto-size tracks, which is grid's `fr` over `minmax(auto, ...)` tracks.
The recommendation (question 2) is to rebase both onto the new solvers
once the solvers pass the oracle, with their existing tests unchanged as
the contract; where a Windows Forms behaviour cannot be expressed by the
solver, that container keeps its own code for it. `ResponsiveTrackPanel`
(priority collapse) and `SplitContainer` stay as they are.

DK's split solver (question 13): a binary split with a stored ratio and
proportional fallback when minimums are not met is not a flex line (flex
has no stored ratio and shrinks by basis, not by minimum), so the
recommendation is to adopt it as it stands and not replace it.

## 3. The measure and arrange contract

The model stays GF006 L4: setters mark typed dirtiness, a geometry read
flushes, update scopes defer, passes are bounded. L adds four things.

### 3.1 Intrinsic size queries

A layout constraint per axis, as a named record:

```cpp
enum class AvailableExtentKind : std::uint8_t {
    definite,     // lay out to exactly this extent (a final or stretched size)
    at_most,      // fit within this extent (today's `available`)
    min_content,  // the narrowest the content can be without overflow
    max_content,  // the content's size with no line breaking
};

struct AvailableExtent final {
    AvailableExtentKind kind{AvailableExtentKind::at_most};
    LogicalLength extent{};   // read only for definite and at_most
};

struct LayoutConstraint final {
    AvailableExtent width{};
    AvailableExtent height{};
};
```

- A new virtual `Size measure_intrinsic(const LayoutConstraint& constraint)`
  on `Control`. The base implementation maps the query onto today's
  `measure(Size)`: `max_content` and `min_content` pass the practical
  unbounded extent and zero respectively, so every existing control keeps
  working unchanged.
- Text-bearing controls (`Label`, the `ButtonBase` family, `CheckBox`,
  `RadioButton`, single-line `TextBox`, `LinkLabel`) implement it from the
  text engine: max-content is the unwrapped line, min-content the widest
  unbreakable segment, and height for a definite width is the wrapped
  height. That is the measurement flex basis, automatic minimums and grid
  intrinsic tracks need.

### 3.2 A measure cache per control

- Each control keeps a fixed-capacity cache of its last results (proposed
  four entries: min-content width, max-content width, and the two most
  recent definite-width heights), keyed by the constraint and stamped with
  a measure generation. `Dirty::measure` on the control advances the
  generation, which empties the cache without freeing it.
- The window's own measure result is kept, not discarded, so a container's
  later query for the same constraint is a cache hit.
- Counters in `MetricsSnapshot`: `measure_cache_hits`,
  `measure_cache_misses`, and the largest number of measure callbacks one
  control received in one pass. A test reads them; "fast" is never claimed
  without them.

### 3.3 Invalidation

- A child's measure dirtiness propagates to its parent's measure only when
  the parent's own size depends on its content on that axis (auto-sized,
  or content-sized by its own parent). Otherwise the parent is marked for
  arrange only. A container whose size is definite on both axes is a
  layout boundary: measure dirtiness stops there.
- Container properties declare their effects: a gap, direction or track
  list change is `invalidation::bounds` on the container; a per-child style
  change (grow, placement, alignment) is measure on the container and
  arrange on its children, never a re-measure of the child itself.
- Order is deterministic: items in (order, child index); grid
  auto-placement in document order; absolute children after in-flow ones.

### 3.4 Passes and inspection

- The solvers are single-pass per flush except where the CSS algorithm
  defines a fixed repeat (flex's freeze loop, bounded by the item count;
  grid §12.1's one repeat). Nothing iterates to convergence; the window's
  four-pass bound is unchanged.
- Each container publishes its last resolution as an inspectable snapshot:
  `FlexLayoutSnapshot` (lines; per item the base size, hypothetical size,
  final size and frozen reason) and `GridLayoutSnapshot` (per track the
  base size, growth limit and final size; per item its area). D draws
  lines and tracks from these; tests read them.

## 4. Right-to-left, display scale and text-driven sizes

- **Right-to-left** (question 7). A `LayoutDirection` property on
  `Control`, `inherit` by default, `left_to_right` at the window root
  (Windows Forms' `RightToLeft` with `Inherit`; CSS `direction`). The
  solvers work in start/end coordinates and mirror the inline axis once, at
  output: `x = container_inner_width - start - extent`. Flex `row`, grid
  column lines, `justify-*` `start`/`end`, block `text-align: start` and
  `left`/`right` insets follow CSS. Bidirectional text inside a text
  control stays the text engine's. The existing containers and Dock/Anchor
  adopt the property in phase L6.
- **Display scale** (question 8). Layout is computed in logical units and
  is identical at every scale: text measurement for layout uses the shipped
  engine's unhinted advances, so a scale change never dirties layout.
  Arranged bounds stay unsnapped logical doubles; device-pixel snapping is
  the painter's, by rounding each edge (not the width), so neighbours share
  an edge with no gap or overlap. Phase L1 records how paint snaps today
  and changes it to edge rounding if it differs. A test lays out the same
  tree at scale 1, 1.25, 1.5 and 2 and requires identical logical boxes.
- **Text-driven sizes.** Sizes that come from text come from measurement
  (section 3.1), through the bundled engine and F's substitution table;
  `effective_text_scale` changes are `invalidation::text_content`, which
  re-measures. Layout lengths in `em` are not admitted in L (question 14):
  a source's `em` lengths are resolved to px when imported.

## 5. Bounded effort for large item counts

- Every solver pass is linear in the number of items, plus one stable sort
  when any item has a nonzero `order`, plus the flex freeze rounds (at most
  one per item on a line) and grid's fixed repeat. No step is quadratic in
  the item count except grid spanning-item distribution, which is bounded
  by items times their span.
- Storage: the container owns a workspace (item records, line records,
  track records) with explicit capacity and live length, reserved when the
  child count or track count changes and reused across passes. Nothing
  allocates per item in the measure or arrange loops (house style §3).
  `FlowLayoutPanel`'s per-pass line vectors go when it is rebased.
- Limits, checked before storage is touched, each a count: at most 65,536
  children per container; at most 10,000 explicit or `repeat()`-generated
  tracks per axis; auto-placement occupancy at most 1,048,576 cells (rows
  times columns), refused with a diagnostic above it.
- With the measure cache, a clean item costs no measure callback. The
  phase gates count, at 10,000 children: changing one item's text causes
  exactly one measure callback and no allocation in the arrange loop;
  resizing the container causes zero measure callbacks for items whose
  constraint did not change.
- Virtualization (question 12): containers lay out every realized child.
  For data collections of 10^5 items and more, realization belongs to item
  systems (ObjectView, `realized_items()`), which call the solvers on
  records for the visible window only. L adds no virtualizing container.

## 6. The oracle

### 6.1 What `tools/` gains

Owner decision 1 moves the oracle into this repository. Its source is not
here, so `tools/layout_oracle/` is written new, in Python with the standard
library only, following the approach the README records:

- `devtools.py`: a small Chrome DevTools Protocol client over a local
  WebSocket.
- `capture.py`: launches the pinned Chromium headless shell isolated (a
  fresh profile, `--disable-gpu`, `--hide-scrollbars`,
  `--force-color-profile=srgb`, no network or component updates,
  `--font-render-hinting=none`); sets the viewport
  and device scale through `Emulation.setDeviceMetricsOverride`; reads
  `getBoundingClientRect()` (Chromium's 1/64-px `LayoutUnit` precision),
  the first baseline of text leaves and the used font for every element
  with a `data-gf-id`. Raster samples are left to H.
- `compare.py`: per-node comparison into separate classes (structure,
  geometry, typography), each with its tolerance, a per-node report and
  totals. A node over tolerance is listed; a difference recorded in the
  fixture is listed as recorded, with its reason, and still counted.
- `chromium_pin.json` and `fetch_chromium.py`: the pinned source URL, byte
  size and SHA-256; the script refuses to run without the hash and an
  explicit `--approved` flag, and verifies before unpacking into a new
  empty directory.
- `test_layout_oracle.py`: the comparison's own tests on synthetic
  snapshots, run in CI with the other tool tests.

The Web.Forms compiler and its HTML profile stay behind (owner
decision 1).

### 6.2 Getting Chromium (needs the owner's approval)

- **Source:** Google's Chrome for Testing builds, the `chrome-headless-shell`
  package (the same Blink layout as Chrome, packaged for automation).
- **Version:** `155.0.8059.39`, published 2026-10-06 (at least two weeks
  old on 2026-10-25; the current Stable, `156.0.8078.47` of 2026-10-19,
  is not).
- **CI (Linux x64):**
  `https://storage.googleapis.com/chrome-for-testing-public/155.0.8059.39/linux64/chrome-headless-shell-linux64.zip`,
  124,203,329 bytes (118.4 MiB) compressed; storage MD5
  `BCeU3QHA1ApziO+HAcq6Qw==`.
- **Optional, local on Shadow Windows (question 10):**
  `https://storage.googleapis.com/chrome-for-testing-public/155.0.8059.39/win64/chrome-headless-shell-win64.zip`,
  124,737,213 bytes (119.0 MiB); storage MD5 `WE4q4OzDOhF+ww0U5f6BYA==`.
- Sizes and hashes were read from the storage server's response headers
  only; nothing was downloaded. The unpacked size is not measured. The
  SHA-256 is recorded in `chromium_pin.json` at the first approved
  download. A later pin change is an owner-approved event that regenerates
  every reference and reports the differences.

### 6.3 Where it runs (question 11)

- References are checked in: one JSON per fixture and profile under
  `tests/layout_corpus/references/`, written by `capture.py`.
- `gui_forms_layout_corpus_tests` (ctest, every CI host, no Chromium)
  builds each fixture's tree, lays it out at each profile and compares
  against the checked-in reference with the tolerances below. This is the
  test that gates phases.
- A `layout-oracle` CI job on Linux x64 downloads the pinned shell (cached
  by version and SHA-256), regenerates every reference and fails if any
  differs from the checked-in one by more than 1/64 px. It runs when the
  corpus, the oracle or the pin changes. It is proposed as not required;
  if the owner wants it required, the coordinator adds the check name
  before the PR merges.

### 6.4 Fixture corpus format (question 9)

- One fixture is one well-formed HTML fragment that is also valid XML, so
  the same bytes feed Chromium and GUI.Forms' first-party XML reader:

  ```html
  <div data-gf-id="root" data-gf-kind="flex-layout-panel"
       style="flex-direction: row; flex-wrap: wrap; gap: 8px; width: 100%; height: 100%">
    <div data-gf-id="a" data-gf-kind="box" style="flex: 1 1 120px; height: 40px"></div>
    <div data-gf-id="b" data-gf-kind="label" style="flex: 0 1 auto"
         data-gf-text="Settings and services"></div>
  </div>
  ```

- Element names are `div` and `span` only; `data-gf-kind` names the
  GUI.Forms control (kebab-case schema names, T1 decision 8): a container,
  `box` (a fixed-size test leaf with no content) or a text control.
  `style` holds only admitted layout declarations, parsed on our side by
  the layout value parser (phase L2), which FD and H reuse.
- Text leaves name a bundled face; the page template loads the same font
  files through `@font-face` from `assets/fonts`, so both sides shape with
  the same file. A shared template supplies the reset (`box-sizing:
  border-box`, zero margins and borders, the root font).
- Profiles per fixture: viewports 320 x 240, 800 x 600, 1280 x 800 and
  1440 x 900; device scale 1 and 2; text scale 100 % and 150 % for text
  fixtures; `ltr` and `rtl` for the fixtures marked
  bidirectional.
- `data-gf-expected-difference="<reason>"` on a node records a known
  difference; the report still lists it.
- When FD lands, fixtures can be converted to form documents mechanically;
  both forms carry the same declarations.

### 6.5 Tolerance (question 9)

- Box fixtures (no text): every edge within 0.05 logical px of Chromium.
  Chromium rounds to 1/64 px per operation and GUI.Forms computes in
  doubles, so a nested `fr` split can differ by a few 1/64 steps; 0.05 is
  about three of them.
- Text fixtures: every edge within 0.5 logical px (half a logical pixel,
  so a box is never a whole pixel off) and the first baseline within 1.0 px,
  reported in the typography class. Differences from shaping or line
  breaking stay in that class, separate from geometry.
- The report records, per class, the count over tolerance and the largest
  difference, per node and per profile.

## 7. Form documents (FD) and the interface builder (D)

- FD writes layout in the CSS family it shares with T3: the container is
  the node kind, its own layout declarations sit in its block, and the
  child's layout declarations sit in the child's block:

  ```css
  flex-layout-panel shell.toolbar {
      flex-direction: row; gap: 8px; align-items: center;
      button shell.toolbar.open { text: "Open"; flex: none; }
      label shell.toolbar.status { text: "Ready"; flex: 1 1 0; min-width: 0; }
  }
  grid-layout-panel settings.form {
      grid-template-columns: max-content minmax(0, 1fr);
      gap: 6px 12px;
      label settings.name.caption { text: "Name"; grid-column: 1; }
      text-box settings.name { grid-column: 2; }
  }
  ```

- Child declarations are attached properties: stored by the parent, legal
  only under a container that declares them. The schema gains a list of
  attached properties per container (with T1's manifest); FD refuses
  `flex:` under a `grid-layout-panel` with an exact diagnostic before
  building anything (FD "Validation before mutation").
- The values go through one layout value parser in the core, built in L2
  over T3's tokenizer; FD, H and the fixture reader all call it. Themes
  never reach it (T3 refuses layout properties).
- There is no `display` property in FD when containers are separate types
  (question 1); `display: flex` in imported HTML selects the container
  type in H.
- D edits the same declarations through its inspector, draws flex lines
  and grid tracks from `FlexLayoutSnapshot` and `GridLayoutSnapshot`, and
  turns drops into declarations (`order`, `grid-column`, `grid-row`), so a
  builder edit changes only those bytes (D gate).
- Generated C++ calls the same setters (`set_flex(child, FlexItemStyle)`,
  `set_grid_template_columns(...)`), in the generated-code profile of house
  style §10.

## 8. Phases, gates and the stage gate

Each phase is one to four PRs on main, each with its own small gate;
stacks stay at most two deep. Every container phase runs the W checklist.

**L0. The oracle and the corpus harness.** `tools/layout_oracle/`
(section 6.1), the page template, the fixture reader in C++ (XML reader
plus a minimal `style` reader for the declarations that already exist:
widths, heights, margins, padding), `gui_forms_layout_corpus_tests`, and
a first corpus that describes today's `FlowLayoutPanel` and
`TableLayoutPanel` behaviour. Gate: the comparison's own tests pass; with
the owner's approval of the download, the `layout-oracle` job writes the
first references and the corpus test reports them, honestly, on four
hosts. 2 PRs.

**L1. The contract.** `LayoutConstraint` and `measure_intrinsic`, text
controls' min- and max-content, the measure cache and its counters, layout
boundaries, `LayoutDirection` on `Control`, the wrapper types (section
10), and the snapping record of section 4. Gate: headless tests count
measure callbacks and cache hits; identical logical boxes at four scales;
every existing layout test (`gui_forms_layout_panel_tests`,
`gui_forms_scrollable_control_tests`, `gui_forms_dock_layout_tests` and
the core suite) passes unchanged under the sanitizer. 3 PRs.

**L2. Layout value parser.** The admitted declarations of sections 2.2 to
2.5 over T3 P1's tokens, into typed style records, with exact
diagnostics for refused forms. Depends on T3 P1. Gate: one conformance
fixture per property and per refused form. 1 to 2 PRs.

**L3. Flex.** `flex_solver` and `FlexLayoutPanel`. Gate: the flex corpus
(about 40 fixtures) within tolerance at every profile; the 10,000-child
counts of section 5. 3 PRs.

**L4. Grid.** `grid_solver` and `GridLayoutPanel`, including
auto-placement, intrinsic tracks and areas. Gate: the grid corpus (about
40 fixtures, including every grid form the atlas uses) within tolerance;
the placement and track limits refuse with diagnostics. 3 to 4 PRs.

**L5. Block and inline flow, and positioning.** `block_flow_solver`,
`BlockFlowPanel`, absolute and relative positioning in all three
containers. Gate: the block corpus (about 20 fixtures) within tolerance.
2 to 3 PRs.

**L6. The Windows Forms containers.** Rebase `FlowLayoutPanel` and
`TableLayoutPanel` on the solvers where question 2 allows, and
`LayoutDirection` in the existing containers and in Dock and Anchor
(question 7). Gate: their existing tests unchanged; new right-to-left
tests; their L0 fixtures still within tolerance. 2 PRs.

**L7. The stage gate.** Run the whole corpus at every profile, record the
report, rewrite `docs/user/layout.md` (its "Not provided" section is no
longer true), and update the stage index and board. 1 PR.

**The stage gate** (README; the measurement to record): a corpus of at
least 100 fixtures (about 40 flex, 40 grid, 20 block and inline, and
nested mixtures taken from the atlas's patterns) at the four viewports,
both scales, both text scales for text fixtures and both directions for
bidirectional fixtures, with zero nodes over tolerance other than recorded
differences, each with its reason; plus the counted bounds of section 5.
Recorded under `results/<date>-l-layout-gate/`: the corpus listing, the
profiles, the tolerances, the Chromium pin and hash, the per-node report
and the counters. Any pattern of recorded differences goes to the owner.

## 9. Open questions for the owner

Each has a recommendation; the coordinator turns them into an interview
page.

1. **Container model.** (a) Three typed containers, `FlexLayoutPanel`,
   `GridLayoutPanel` and `BlockFlowPanel`, over one shared solver library;
   (b) one `LayoutPanel` with a `display` mode; (c) extend
   `FlowLayoutPanel` and `TableLayoutPanel` in place. **Recommend (a):**
   each mode has its own child properties and typed API, and FD's node kind
   names the mode.
2. **Windows Forms containers.** (a) Keep their behaviour and rebase their
   internals on the solvers wherever their existing tests pass unchanged;
   (b) leave them untouched; (c) deprecate them in favour of the new
   containers. **Recommend (a).**
3. **Flex automatic minimum size.** (a) As CSS: an item's minimum is its
   content minimum unless `min-width`/`min-height` says otherwise; (b) a
   GUI.Forms rule of minimum zero. **Recommend (a):** sources and the
   atlas are written for it (`minmax(0, 1fr)`, `min-width: 0`).
4. **Block margin collapsing.** (a) CSS 2.1 §8.3.1 without floats
   (siblings, parent with first and last child, empty blocks); (b)
   siblings only; (c) none, with the importer folding margins.
   **Recommend (a)** for H fidelity.
5. **Floats.** (a) Left out of L; H reports them as unsupported;
   (b) included in L5. **Recommend (a).**
6. **Containing block of an absolute child.** (a) The direct parent
   container, with the difference recorded where a source relies on a
   farther positioned ancestor; (b) the nearest positioned ancestor, as
   CSS. **Recommend (a):** a parent owns its children's slots; (b) would
   let a control place a grandchild.
7. **Right-to-left.** (a) An inherited `LayoutDirection` on `Control`,
   used by the new containers, then by the existing containers and
   Dock/Anchor in L6; (b) a per-container property only.
   **Recommend (a).**
8. **Scale and snapping.** (a) Layout independent of display scale;
   snapping at paint by edge rounding; (b) snap arranged bounds to device
   pixels in layout, so layout differs per scale. **Recommend (a).**
9. **Fixture format and tolerance.** (a) Well-formed HTML fragments read
   by both Chromium and the core XML reader; 0.05 px per edge for box
   fixtures, 0.5 px per edge and 1.0 px baseline for text fixtures;
   (b) the same format with one tolerance of 0.5 px for everything;
   (c) wait for FD and write fixtures as form documents.
   **Recommend (a).**
10. **The Chromium download.** (a) Approve
    `chrome-headless-shell-linux64.zip` 155.0.8059.39 (124,203,329 bytes)
    for CI only; (b) also approve the win64 package (124,737,213 bytes) for
    local reference runs on this machine; (c) neither yet: L0 lands the
    tools and the corpus without references. **Recommend (a).**
11. **Where references come from.** (a) Checked-in references; every host
    compares against them without Chromium; a Linux `layout-oracle` job
    regenerates and checks them, not required; (b) Chromium on every CI
    host on every run; (c) as (a) with the job required.
    **Recommend (a).**
12. **Virtualization.** (a) The solvers lay out records; item systems
    realize only visible items; L adds no virtualizing container; (b) L
    adds a virtualizing repeater container. **Recommend (a).**
13. **DK's split solver.** (a) Adopt it as it stands; (b) replace it with
    a two-item flex line. **Recommend (a):** a stored ratio with
    proportional fallback is not flex.
14. **Units and functions in layout values.** (a) `px`, `%`, `fr`, the
    content keywords and linear `calc()`; `em` resolved to px by the
    importer; `min()`, `max()`, `clamp()` later; (b) also `em` relative to
    the box's text-role size, so layouts scale with text; (c) also
    `min()`, `max()`, `clamp()` in L. **Recommend (a).**
15. **Grid naming.** (a) `grid-template-areas` and `grid-area` in L4,
    named lines left out; (b) neither; (c) both. **Recommend (a).**

## 10. House-style points

- **Distinct types** (§1): `LogicalLength` for layout lengths and
  `DevicePixels` only at snapping; `FlexFactor` and `FractionFactor` for
  grow, shrink and `fr`, never added to lengths; `ItemIndex`, `LineIndex`,
  `TrackIndex` and `GridLine` (one-based, possibly negative) distinct from
  each other and from runtime ids. The public control API keeps `Rect` and
  `Size` in logical units, as the surrounding code does.
- **Records with capacity and live length** (§1, §5): item, line and track
  workspaces state both; indexing never grows them.
- **Caller-owned output** (§4): solvers fill the caller's `Rect` span and
  resolution record; the span's length is checked against the item count
  before the first write.
- **No per-item allocation** (§3): workspaces are reserved when counts
  change, before the loops.
- **Choose before the loop** (§3): direction, wrap and axis mapping are
  resolved once into a named axis record; the inner loop works in main and
  cross (or inline and block) coordinates and mirrors once at output.
- **Named predicates** (§2, §7): the order sort uses a named comparator;
  no lambdas.
- **Failure** (§9): a refused declaration leaves the previous style in
  place; limits are checked before storage is touched; a measurement that
  returns a non-finite size is clamped to zero as today, and counted.
- **Effort, not time**: every bound is a count (items, tracks, cells,
  freeze rounds, passes); no timeout anywhere in layout.
- **Spelling** (§10): no `auto`, `->`, structured bindings or `= default`
  comparisons; `(*pointer).member`.

## 11. Size

Eight phases, L0 to L7, of about 17 to 20 PRs: L0 2, L1 3, L2 1 to 2,
L3 3, L4 3 to 4, L5 2 to 3, L6 2, L7 1. L2 waits for T3 P1; L0 and L1 can
start as soon as the owner answers. The corpus grows with each phase to at
least 100 fixtures. No timing is claimed.
