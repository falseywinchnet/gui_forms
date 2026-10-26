# Rich text: the RTF parser library and RichTextBox's run model

Date: 2026-10-25. Status: **brief; owner decision 1 of 2026-10-25 (W-Q1) in
force; nothing built**. Read with the [W brief](README.md),
[AGENTS.md](../../../AGENTS.md), the complete
[house style](../../PROGRAMMING_HOUSE_STYLE.md) and the
[zlib stage](../Z-zlib/README.md), whose library layout this follows.

## Decision

`RichTextBox` is a rich text field over an inline markdown-like run model,
not Windows Forms' RTF-native design (owner, 2026-10-25, W-Q1). RTF is
read and written by a standalone first-party library inside `gui_forms/`.
SwiftEdit consumes the library and dogfoods it; a SwiftEdit worker builds
it under house style. W does not build the library. W builds the run model
and the control once the library's seam (below) exists.

## Part 1: the parser library

### Name

**`stx_rtf2md`** (owner, RT-Q1, 2026-10-26), beside `stx_zlib`, `stx_png`
and `stx_vorbis` (`gui_forms/stx_rtf2md/`, target `stx_rtf2md::stx_rtf2md`).
The library is the RT stage; its plan,
`planning/stages/RT-rich-text/PLAN.md`, is authoritative where it and this
brief differ.

### Shape

A C++20 house-style library with no GUI.Forms dependency, its own
`CMakeLists.txt`, `README.md`, `PROVENANCE.md` and licence, as `stx_zlib`
has. It owns one document model and four conversions:

- `parse_rtf`: RTF bytes to a `Document`;
- `write_rtf`: a `Document` to RTF bytes;
- `parse_markdown`: dialect text (UTF-8) to a `Document`;
- `write_markdown`: a `Document` to dialect text.

RTF to markdown is `parse_rtf` then `write_markdown`; one model behind both
formats means one markdown reader in the program, shared by SwiftEdit and
GUI.Forms, instead of a second one inside `RichTextBox`.

### The document model (the public seam)

- `Document`: an ordered list of `Block`s and a `LinkTable` (targets,
  referenced by index).
- `Block`: a kind (`paragraph`, `heading` with level 1 to 6, `list_item`
  with ordered or bulleted and a nesting level, `quote`, `code_block`), an
  alignment (left, centre, right) and its `Run`s.
- `Run`: UTF-8 text (no paragraph breaks; a line break is a run of its own
  kind) and a `RunStyle`: bold, italic, underline, strikethrough, inline
  code, a link index or none, a colour or none, a size step or none.
- Positions are named types: byte offsets into a run's UTF-8 and scalar
  positions are not interchangeable (house style 1).

Every function fills caller-owned storage (house style 4) and returns an
explicit result: `ok`, or a named error (`malformed`, `limit_exceeded`,
`unsupported_encoding`, `effort_exceeded`) with the byte offset where it
stopped. A failed call leaves the output cleared, never partial.

### RTF subset read

Admitted (RTF 1.9.1 spelling):

- the header: `\rtf1`, `\ansi`, `\ansicpg`, `\deff`, `\uc`, `\u` (with its
  fallback characters skipped per `\uc`), `\'hh` in code page 1252 (other
  code pages are `unsupported_encoding` until their tables are admitted);
- the font table (`\fonttbl`, `\f`): recorded only as monospace or not, so
  a monospace run becomes inline code;
- the colour table (`\colortbl`, `\cf`, `\red\green\blue`);
- character formatting: `\b`, `\i`, `\ul`, `\ulnone`, `\strike`, `\fs`
  (mapped to a size step), `\plain`;
- paragraphs: `\par`, `\pard`, `\line`, `\tab`, `\ql`, `\qc`, `\qr`,
  `\li` and `\fi` (mapped to list or quote nesting where a list marker or
  Word's `\listtext`/`\pntext` is present, otherwise ignored);
- headings as written by Word, WordPad and TextEdit (an outline level
  `\outlinelevel` or a recognised heading style);
- hyperlinks: `\field` with `\*\fldinst HYPERLINK "target"` and
  `\fldrslt` text.

Skipped as whole groups: `\*\` destinations the library does not know,
`\pict`, `\object`, `\stylesheet`, `\info`, headers and footers. Embedded
objects and pictures are not supported (no embedded objects, as the owner's
choice 1 said). Tables are flattened to paragraphs, one per cell, in the
first version.

### Markdown dialect written and read

CommonMark's block and inline syntax for what CommonMark has (headings,
lists, quotes, fenced code, `*`/`**` emphasis, `` ` `` code, `[text](url)`
links, hard line breaks), plus GitHub's `~~strike~~`. What CommonMark lacks
is written as an inline attribute span: `[text]{u}` for underline,
`[text]{color=#RRGGBB}` and `[text]{size=+1}`, combinable in one span. Raw
HTML is neither written nor interpreted. The dialect's grammar is written
down in the library's README before the reader is built, and the owner
reviews the extension spelling (open question 2).

### Round-trip rules

1. **Markdown is canonical.** `write_markdown(parse_markdown(m))` equals
   the canonical form of `m`: byte identity for text the writer produced.
2. **RTF round trips by meaning, not bytes.** `parse_rtf(write_rtf(d))`
   equals `d` for every `Document` the dialect can express.
3. **RTF read loses only what the subset lacks**, and says so: the result
   counts the skipped destinations and dropped control words by kind, so
   SwiftEdit can tell the user what an import dropped.
4. Adjacent runs with equal styles are merged on every parse, so equal
   documents have one representation.

### Bounds

Bounded in effort units, never wall-clock (the zlib stage's rule):

- input size, group nesting depth, control-word length (32 letters, per the
  specification), numeric parameter range, colour and font table entries,
  block count, run count and link count each have a stated limit in a
  `Limits` record with conservative defaults;
- each byte read and each run written is charged before its loop; work is
  linear in the input, and an exceeded allowance stops with
  `effort_exceeded` and cleared output;
- no recursion follows RTF group nesting: groups use an explicit,
  capacity-checked state stack.

### Fuzz target and tests

- `fuzz/rtf_parse_fuzz.cpp` (libFuzzer, built with
  `GUI_FORMS_BUILD_FUZZERS`, and as a standalone test with
  `GUI_FORMS_FUZZ_STANDALONE=1`, as `fuzz/deflate_fuzz.cpp` is): any bytes to
  `parse_rtf`, then rule 2 on every document it accepts;
- `fuzz/markdown_round_trip_fuzz.cpp`: any UTF-8 to `parse_markdown`, then
  rule 1;
- a corpus of RTF saved by WordPad, Word, TextEdit and Windows Forms'
  RichTextBox, with provenance recorded per file; a malformed sweep in the
  manner of `tests/compression/malformed_sweep_tests.cpp`;
- the sanitizer over every test; no flake is accepted.

## Part 2: RichTextBox's run model (W, after the seam exists)

W's to build once `stx_rtf2md`'s `Document` seam has landed and the
coordinator confirms:

- **The run model** (`RichTextModel`, private to the control or a public
  `gui_forms/text/rich_text` type if a second control needs it): paragraphs
  of styled runs, seeded from and saved to `stx_rtf2md::Document`, with run
  splitting and merging on edits, selection by scalar position, and style
  queries and changes over a selection (Windows Forms' `SelectionFont`,
  `SelectionColor`, `SelectionBullet`, `SelectionAlignment` and the rest,
  mapped onto run styles).
- **The control**: `RichTextBox` on the TextBox engine's editing,
  caret, selection, undo and IME paths, its runs shaped through the text
  engine's font runs (HarfBuzz, bundled faces), painted by role; `rtf()` /
  `set_rtf()` and `markdown()` / `set_markdown()` through the library;
  `text()` the plain text. Links are the link role of decision 4.
- Out of scope: embedded objects, OLE, zoom factors beyond the size steps,
  and Windows Forms' RTF-specific members that have no meaning over the run
  model (they are classified in the catalogue as declined, with the reason).

## Owner answers, 2026-10-26

1. The library's name is `stx_rtf2md` (RT-Q1).
2. The extension spelling is the attribute span, `[text]{u}`, as
   recommended; the RT plan's span also carries superscript, subscript,
   colour, background colour and size, all accepted.
3. The first version reads code page 1252 only, as recommended.
