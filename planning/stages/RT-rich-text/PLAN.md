# Stage RT plan: `stx_rtf2md`, the RTF and rich-markdown library

Status 2026-10-26: **accepted.** The owner answered section 12 on
2026-10-26: every recommendation was accepted, and the library is named
`stx_rtf2md`. Revised on 2026-10-25 after W's review of section 11 (items 6
and 8).
Written by the SwiftEdit worker, which builds the library (owner decision W-Q1,
2026-10-25). Authority: the W stage README, "Owner decisions, 2026-10-25",
item 1, and W's companion brief
[`RICH_TEXT_BRIEF.md`](../W-winforms-controls/RICH_TEXT_BRIEF.md). This plan
takes that brief as its starting point. Section 11 lists where the plan
differs from it. Where this plan and a recorded owner decision disagree, the
decision governs.

Decisions this plan builds on:

- W-Q1, the owner's words: "lets not do it how they did it. lets do it
  something like an inline markdown system. its a rich text field, but it
  relies on a parser that converts rtf to markdown that can be shared with
  swiftedit and done by a swiftedit worker and dogfed on that program." On
  where the library lives, the owner accepted "in gui_forms, as a
  standalone library".
- Standing rules: effort is bounded in counts, never in wall-clock time. Our
  engine is the reference, so nothing is aligned to Microsoft's RTF control.
  House style is mandatory. Memory errors in code or tests are defects, and
  there is no accepted flake.
- AGENTS.md: the library includes, links and reads nothing outside
  `gui_forms/`. It has no third-party dependency at all.

## Contents

1. [What exists today](#1-what-exists-today)
2. [What RT builds](#2-what-rt-builds)
3. [The document model](#3-the-document-model)
4. [The markdown dialect](#4-the-markdown-dialect)
5. [The RTF subset read](#5-the-rtf-subset-read)
6. [RTF written](#6-rtf-written)
7. [Round trips, losses and failures](#7-round-trips-losses-and-failures)
8. [Bounds](#8-bounds)
9. [Tests, fuzz targets and CI](#9-tests-fuzz-targets-and-ci)
10. [Phases and gates](#10-phases-and-gates)
11. [Differences from the W brief](#11-differences-from-the-w-brief)
12. [Owner decisions, 2026-10-26](#12-owner-decisions-2026-10-26)
13. [House-style points](#13-house-style-points)

## 1. What exists today

Read from main at `8d1f2be0`.

- GUI.Forms has no RTF code (`git grep -w -i rtf -- src include` finds
  nothing). `RichTextBox` is W's priority 4 and is not started.
- `stx_zlib/` is the layout to copy. It has its own `CMakeLists.txt`
  (standalone project or subdirectory, `STX_ZLIB_TESTS`, warnings with
  `-Wconversion`), `include/stx_zlib/`, `src/`, `tests/`, `README.md`,
  `PROVENANCE.md` and `LICENSE`, plus an installed package found with
  `find_package`. Its fuzz entry points live in `fuzz/` and also build as
  standalone tests with `GUI_FORMS_FUZZ_STANDALONE=1`
  (`gui_forms_deflate_fuzz_tests`).
- SwiftEdit, the first consumer, renders Markdown with MD4C, using
  `MD_FLAG_TABLES | MD_FLAG_TASKLISTS | MD_FLAG_STRIKETHROUGH | MD_FLAG_NOHTML`
  (SwiftEdit's `src/markdown.cpp`). Its objectives (owner interview
  2026-09-30) exclude "RTF/Office/PDF editor" from its scope. Section 10,
  phase RT4, and decision 5 cover how RTF import fits that exclusion.

## 2. What RT builds

The library is `gui_forms/stx_rtf2md/` with target `stx_rtf2md::stx_rtf2md`. The name
is the owner's (decision 1). It is a static C++20 library with no
GUI.Forms dependency. It builds as its own project or as a subdirectory of
GUI.Forms, installs with its own CMake package, and is listed in the
`stx-packages` workflow. It holds one document model and four conversions:

| Conversion | From | To |
|---|---|---|
| `MarkdownReader::read` | dialect text, UTF-8 | `Document` |
| `write_markdown` | `Document` | dialect text, UTF-8 |
| `RtfReader::read` | RTF bytes | `Document` |
| `write_rtf` | `Document` | RTF bytes, 7-bit ASCII |

RTF to markdown is `RtfReader::read` followed by `write_markdown`. Because
both formats go through one model, the program has one markdown reader.
SwiftEdit, `RichTextBox` and its clipboard all use it.

```text
stx_rtf2md/
├── CMakeLists.txt          standalone or subdirectory; STX_RTF_TESTS
├── README.md               the dialect grammar and RTF subset (normative)
├── PROVENANCE.md           first-party; the RTF 1.9.1 specification consulted
├── LICENSE
├── include/stx_rtf2md/
│   ├── document.hpp        Document, Block, Run, RunStyle, typed offsets
│   ├── limits.hpp          Limits, Result, Status, LossReport
│   ├── markdown.hpp        MarkdownReader, write_markdown
│   └── rtf.hpp             RtfReader, write_rtf
├── src/                    one source per conversion, plus normal_form.cpp
└── tests/                  core, markdown, rtf, malformed sweep, corpus/
```

## 3. The document model

The model is the public seam, shared by `RichTextBox` (W) and SwiftEdit. A
`Document` is a value that owns all of its storage. Its text lives in one
contiguous UTF-8 string, and runs refer to ranges of it, so no allocation is
made per run.

```cpp
namespace stx_rtf2md {

struct TextOffset final { std::uint32_t value = 0; };   // byte offset into Document::text
struct TextLength final { std::uint32_t value = 0; };   // UTF-8 byte count
struct SourceOffset final { std::uint64_t value = 0; }; // byte offset into a reader's input
struct LinkIndex final { std::uint32_t value = 0; };    // index into Document::links

struct Rgb final { std::uint8_t red = 0; std::uint8_t green = 0; std::uint8_t blue = 0; };

enum class BlockKind : std::uint8_t { paragraph, heading, list_item, code_block };
enum class ListMarker : std::uint8_t { bullet, ordered };
enum class Alignment : std::uint8_t { left, center, right };
enum class RunKind : std::uint8_t { text, line_break };

struct RunStyle final {
    bool bold = false;
    bool italic = false;
    bool underline = false;
    bool strikethrough = false;
    bool code = false;                     // inline code; combines with every other attribute
    bool superscript = false;              // exclusive with subscript
    bool subscript = false;
    std::optional<LinkIndex> link;
    std::optional<Rgb> colour;
    std::optional<Rgb> background;
    std::int8_t size_step = 0;             // 0 is body size; -2 to +4
};

struct Run final {
    RunKind kind = RunKind::text;
    TextOffset start;                      // line_break runs have length 0
    TextLength length;
    RunStyle style;
};

struct Block final {
    BlockKind kind = BlockKind::paragraph;
    std::uint8_t level = 0;                // heading 1-6; list nesting 0-7; else 0
    std::uint8_t quote_depth = 0;          // 0-7, any kind may be quoted
    ListMarker marker = ListMarker::bullet;
    std::uint32_t ordinal = 0;             // the number written on an ordered item; 0 otherwise
    Alignment alignment = Alignment::left;
    std::uint32_t first_run = 0;           // index into Document::runs
    std::uint32_t run_count = 0;
};

struct Link final { TextOffset target_start; TextLength target_length; };

class Document final {
public:
    std::string text;                      // run text and link targets, UTF-8
    std::vector<Block> blocks;
    std::vector<Run> runs;
    std::vector<Link> links;
    void clear();                          // ends every element; keeps capacity
};

}
```

**Normal form.** Every reader produces a document in normal form, and every
writer refuses a document that is not in it (`invalid_document`, with nothing
written). This makes equal content have one representation, so documents
can be compared field by field:

- runs are contiguous and in block order. Every block has at least one text
  run, and no run is empty except a line break;
- adjacent text runs with equal styles are merged. A line break may open a
  block or follow another (markdown writes each as a backslash line), but
  never ends a block, and headings have none;
- a list item at nesting level L > 0 directly follows a list item at the
  same quote depth whose level is at least L - 1, since markdown writes
  nesting under a parent;
- text is valid UTF-8 without paragraph or line separators, without C0
  controls other than tab, and without U+FEFF;
- blanks (spaces and tabs) are kept wherever they appear, including at the
  start and end of a block. The markdown writer escapes the ones CommonMark
  would strip (section 4), so nothing the user can see is lost;
- `code` is an attribute like the others: inline code may be bold,
  coloured, spanned or linked. A code block is one unstyled text run whose
  text holds its lines joined by line feeds, so blank lines inside code are
  kept; no other text contains a line feed;
- an ordered list item's `ordinal` is the number written on its marker.
  Readers keep the number they read, so renumbering is the consumer's
  presentation choice; bullets and other blocks have ordinal 0;
- `size_step` is 0 in headings, whose level stands for their size.

`Document::clear` keeps capacity, so one `Document` can be reused across
many conversions without allocating again.

## 4. The markdown dialect

The dialect is CommonMark 0.31 for what CommonMark has, plus GitHub's
`~~strikethrough~~`, plus two attribute forms for what CommonMark lacks. The
grammar is written in full in `stx_rtf2md/README.md` before the reader is
built. This section fixes its shape.

**Canonical form (what `write_markdown` writes):**

| Model | Written |
|---|---|
| paragraph | its text; blocks are separated by one blank line |
| heading level n | `#` × n, a space, then the text |
| list item | `-` or the ordinal and `.`, then a space; nesting indents by the parent marker's width plus one space (2 for `-`, 3 for `1.`, 4 for `10.`); continuation lines are indented to the item's content |
| quote depth n | `> ` × n before every line of the block; the blank line between two quoted blocks is quoted to their shared depth, so the quote continues |
| code block | a backtick fence one longer than the longest run of backticks inside, at least three; each line of the run's text on its own line |
| bold, italic | `**text**`, `*text*` |
| strikethrough | `~~text~~` |
| inline code | a backtick string one longer than the longest run of backticks inside, padded with one space on each side when the text starts or ends with a backtick or a space and is not all spaces (CommonMark strips exactly that padding) |
| link | `[text](target)`, or `[text](<target>)` when the target holds a space or parenthesis |
| line break | `\` at the end of the line |
| underline, superscript, subscript, colour, background, size | an attribute span: `[text]{u}`, `{sup}`, `{sub}`, `{color=#rrggbb}`, `{bg=#rrggbb}`, `{size=+1}`. One span may combine attributes, `[text]{u color=#c00000 size=-1}`, written in that fixed key order |
| alignment other than left | a block attribute line, `{align=center}` or `{align=right}`, on the line directly before the block (inside the quote markers when quoted) |

When a run has both CommonMark emphasis and span attributes, the emphasis
goes inside the span: `[**text**]{u}`. A link is written outside the spans
it contains. CommonMark's flanking rules cannot place `**`, `*` and `~~` at
every run boundary (bold "foo." before plain "bar" cannot be `**foo.**bar`),
so the writer reads each block back and, when the runs differ, writes that
block's bold, italic and strikethrough as the span items `b`, `i` and `s`. Within the text, the writer escapes
`\`, `` ` ``, `*`, `_`, `[`, `]`, `~`, `<`, `{` and `&` with a backslash
wherever they appear. At the start of a line it also escapes `#`, `>`, `-`,
`+`, `=`, `|`, and a digit run followed by `.` or `)`; every `#` in a
heading; and a `!` ending a run that another text run follows, where it
could meet a link's or span's `[`. Blanks that
CommonMark would strip, those at the start of a line and at the end of a
block, are written as numeric character references, `&#32;` for a space and
`&#9;` for a tab. Numeric character references are CommonMark syntax, not
raw HTML, and escaping every `&` keeps literal text from reading as one.
Nothing else is escaped. The writer emits LF line endings and ends with one
LF.

**What the reader accepts.** The reader accepts the canonical form exactly,
plus the CommonMark spellings people type: `__bold__` and `_italic_`, `*`
and `+` bullets, `1)` ordered markers, setext headings, indented code
blocks, `<https://...>` autolinks, hard breaks made of two trailing spaces,
CommonMark's delimiter-run rules for emphasis, and numeric character
references (decimal and hexadecimal) for any Unicode scalar the normal form
admits. Constructs outside the dialect are kept as literal text and counted
in the loss report: raw HTML, images, reference links and their
definitions, tables, task-list markers, footnotes, named entity references,
and numeric references to a character the normal form forbids. Nothing is
discarded silently, and nothing outside the dialect is interpreted.

**Degrading to plain markdown.** In a CommonMark or GFM renderer that does
not know the extensions, including SwiftEdit's MD4C configuration, an
attribute span shows as `[text]{u}` and a block attribute line shows as a
paragraph reading `{align=center}`. Nothing is hidden or reinterpreted. The
span cannot become a link, because the dialect never writes link reference
definitions.

## 5. The RTF subset read

The reader follows the RTF 1.9.1 specification's grammar: groups, control
words of at most 32 letters with an optional signed numeric parameter,
control symbols, `\'hh` bytes, `\binN`, and `\*` destinations. Groups are
kept on an explicit stack whose capacity is checked, never by recursion.
Every group pushes the character state and the destination, and every close
pops them.

| Area | Read | Mapped to |
|---|---|---|
| Header | `\rtf1`; `\ansi`, `\ansicpgN`, `\deffN`, `\ucN`, `\uN` | Text must start `{\rtf`; otherwise `malformed` at offset 0. After `\uN`, its `\ucN` fallback characters are skipped |
| Bytes | `\'hh`, and literal bytes 0x20-0x7E | Code page 1252 only. Another declared code page is accepted until it decodes a byte above 0x7F; that byte stops the read with `unsupported_encoding` (decision 3) |
| Fonts | `\fonttbl`, `\fN`, family words, font names | A font is monospace when its family is `\fmodern` or its name is in a fixed list (Courier New, Courier, Consolas, Lucida Console, Menlo, Monaco, Cascadia Mono, Cascadia Code, DejaVu Sans Mono, Liberation Mono). Runs in a monospace font other than the document's default font (`\deff`) become inline code, so a document typed entirely in Courier is not one code span (coordinator, 2026-10-26). Other font identity is dropped |
| Colours | `\colortbl`, `\redN\greenN\blueN`, `\cfN`, `\cbN`, `\highlightN` | `colour` and `background`. Index 0 (auto) means none |
| Characters | `\b`, `\i`, `\ul` and its style variants, `\ulnone`, `\strike`, `\striked1`, `\super`, `\sub`, `\nosupersub`, `\fsN`, `\plain`, each with a `0` parameter to turn off | Run style. Underline styles collapse to one underline (counted). `\fsN` maps to a size step (below) |
| Paragraphs | `\par`, `\pard`, `\line`, `\tab`, `\page`, `\sect`, `\ql`, `\qc`, `\qr`, `\qj`, `\liN`, `\fiN`, `\sbN`, `\saN` | Blocks and line breaks. `\qj` reads as left (counted). Indents and spacing are dropped (counted), except as list and quote evidence |
| Styles | `\stylesheet` names only, and `\sN` references | A style named `heading 1` to `heading 6` (in any case), `Quote`, `Intense Quote`, `Code`, `HTML Preformatted` or `Plain Text` sets the block kind, the heading level or the quote depth. Style formatting is not applied |
| Headings | `\outlinelevelN` (0 to 5) | Heading levels 1 to 6. This takes precedence over the style |
| Lists | `\listtable`, `\listoverridetable`, `\lsN`, `\ilvlN`, `\levelnfcN`, `\levelstartatN`; and the older `\pn...` and `{\pntext ...}` / `{\listtext ...}` | List items with marker, nesting level and ordered start. Marker text in `\listtext` and `\pntext` is skipped, because the writer generates markers |
| Links | `\field{\*\fldinst HYPERLINK "target"}{\fldrslt text}` | A `Link` and the link index on the result runs. Other field types keep their result text (counted) |
| Specials | `\emdash`, `\endash`, `\bullet`, `\lquote`, `\rquote`, `\ldblquote`, `\rdblquote`, `\~`, `\-`, `\_`, `\{`, `\}`, `\\` | The corresponding characters; the optional hyphen `\-` is dropped |
| Tables | `\trowd`, `\cell`, `\row` | Flattened to one paragraph per cell (counted) |

**Skipped, as whole groups, and counted by kind:** any `\*` destination the
reader does not know, `\pict`, `\object`, `\shp`, `\info`, `\header...`,
`\footer...`, `\footnote`, `\annotation`, `\bkmkstart`, `\bkmkend` and the
document-variable destinations. A control word the reader does not know,
outside such a group, is ignored and counted. Nothing in a skipped group
reaches the document.

**Size steps.** Size steps are relative to the document's body size: step
0 is body text. A consumer maps step 0 to its own body font, as
`RichTextBox` maps it to the control's `Font`, and scales the other steps
with it. The reader finds the body size as the first of: Word's
`{\*\defchp \fsN}`, the Normal style's `\fs` (style 0), and the size
covering the most characters; `RtfReader::last_body()` reports which rule
chose it (coordinator, 2026-10-26). Each absolute size `a` is scaled to a
12-point body, `h = round(a * 24 / body)`, so Windows Forms' 8.5-point
RichTextBox text and Word's 11-point text both read as step 0. The scaled
value `h` maps as follows: h ≤ 17 is -2; 18-21 is -1; 22-25 is 0;
26-31 is +1; 32-39 is +2; 40-55 is +3; h ≥ 56 is +4. A size that is not one
of the canonical values written in section 6 is counted as quantised.

**Counting.** An empty paragraph ended by `\par` or `\cell` is spacing the
model cannot hold, counted as `spacing_dropped`, as are non-zero
`\sb`/`\sa`. Producer words that change nothing the model holds (`\rsid`,
`\lang`, page geometry, Cocoa and Word document settings) are not counted;
unknown words and words that change appearance the model cannot hold
(`\caps`, `\outl` and the like) are counted as `control_word_ignored`.
Effort is charged per input byte, per control-word letter, per decoded
character and per emitted run and block.

## 6. RTF written

`write_rtf` writes one canonical RTF, so its output is deterministic for a
given document:

- header `{\rtf1\ansi\ansicpg1252\deff0\uc1`, a font table holding body font
  0 and monospace font 1 (names from `RtfWriteOptions`, defaulting to
  `Calibri` and `Consolas`), a colour table of the document's colours in
  first-use order, and a stylesheet holding `heading 1` to `heading 6`,
  `Quote` and `Code`;
- each block is `\pard`, then its style, alignment and list properties,
  then its runs, then `\par`. Quote depth n is `\s` Quote with
  `\li` = 720 × n. A list item writes `\ls`/`\ilvl` and a `{\listtext ...}`
  marker, so readers without list tables still show a marker;
- size steps -2 to +4 are written against a 12-point body, as `\fs` 16,
  20, 24, 28, 36, 48 and 64;
- text is written in ASCII. Characters above 0x7E are `\uN?` (a signed
  16-bit value, with surrogate pairs for characters beyond the BMP), and
  `\`, `{` and `}` are escaped;
- a link is a `HYPERLINK` field.

## 7. Round trips, losses and failures

Round-trip rules, each checked by tests and a fuzz target (section 9):

1. **Markdown is canonical.** `write_markdown(read_markdown(m))` is the
   canonical form of `m`. It is byte-identical to `m` when `m` was written
   by `write_markdown`.
2. **RTF round-trips by meaning.** `read_rtf(write_rtf(d))` equals `d` for
   every document `d` in normal form. Every model feature has a written RTF
   form that the reader maps back to it, which is why the reader reads
   style names (section 5).
3. **Reading RTF loses only what the subset lacks, and reports it.**
4. **Normal form** (section 3) makes "equals" mean field-by-field equality.

**The loss report.** `LossReport` is a fixed array with one count, and the
source offset of the first occurrence, for each `LossKind`:
`destination_skipped`, `picture_skipped`, `object_skipped`,
`control_word_ignored`, `field_result_kept`, `table_flattened`,
`justify_read_as_left`, `underline_style_collapsed`, `font_dropped`,
`size_quantised`, `indent_dropped`, `spacing_dropped`,
`control_character_dropped`, and for markdown input `html_kept_as_text`,
`image_kept_as_text`, `reference_link_kept_as_text`, `table_kept_as_text`,
`task_marker_kept_as_text`, `footnote_kept_as_text`,
`entity_kept_as_text` (named references, and numeric references to
forbidden characters). The report needs no allocation, so SwiftEdit can
tell the user what an import dropped without inspecting the document.

**Results.** Every call returns a `Result`. It holds a `Status`, the
`SourceOffset` where reading stopped, and, for writers, the bytes produced
and the bytes required. The statuses are `ok`, `malformed`,
`limit_exceeded`, `unsupported_encoding`, `effort_exceeded`,
`output_too_small` and `invalid_document`. On any status other than `ok`,
the output `Document` is cleared and the output span's contents are
unspecified with zero bytes produced. Nothing partial is ever presented as
a result. `output_too_small` reports the exact size required, so the caller
can resize its buffer and call again; the writers are deterministic, so the
second call succeeds. Malformed RTF that leaves groups open at the end of
input is `malformed` at the end offset. Repairing a damaged file is not the
reader's job.

## 8. Bounds

`Limits` is a plain record with conservative defaults. Every limit is a
count, checked before the work it bounds:

| Limit | Default | Exceeded gives |
|---|---|---|
| `input_bytes` | 16 MiB (SwiftEdit's read-only threshold) | `limit_exceeded` at 0 |
| `group_depth` | 256 | `limit_exceeded` |
| control-word letters | 32 (specification; not adjustable) | `malformed` |
| numeric parameter | signed 32-bit | `malformed` |
| `font_entries`, `colour_entries`, `style_entries`, `list_entries` | 4,096 each | `limit_exceeded` |
| `blocks`, `runs`, `links` | 1,048,576; 4,194,304; 65,536 | `limit_exceeded` |
| `work_units` | 32 × `input_bytes` | `effort_exceeded` |

Work units: one unit is charged for each input byte read, each output byte
written, each run or block created, and each step of the emphasis delimiter
algorithm. The markdown reader uses CommonMark's openers-bottom table, so
its delimiter processing stays linear. Work is therefore linear in the
input, and the default allowance is a stated multiple of it. Each kernel
charges its whole extent before its loop starts, not per byte. A reader
reserves its group stack and its tables once, at construction, from
`Limits`, and reuses them across calls. `\binN` must not claim more bytes
than remain in the input.

## 9. Tests, fuzz targets and CI

- `stx_rtf2md/tests/`: model and normal-form tests; the dialect's full grammar
  table (every row of section 4, read and written); every row of section 5;
  `write_rtf` output for each feature; every `Status` and every `LossKind`;
  `Limits` enforcement at each limit exactly and one past it.
- **Malformed sweep**, in the manner of
  `tests/compression/malformed_sweep_tests.cpp`: every prefix of every
  corpus file, and single-byte changes at every offset, must return a
  status without fault, and an `ok` result must be in normal form.
- **Fuzz targets** (libFuzzer with `GUI_FORMS_BUILD_FUZZERS`; also built as
  standalone tests with `GUI_FORMS_FUZZ_STANDALONE=1`, and added to
  `fuzz-smoke.yml` for a light smoke run on each PR):
  - `fuzz/rtf_read_fuzz.cpp`: any bytes to the RTF reader; an `ok` document
    must be in normal form and satisfy rules 1 and 2;
  - `fuzz/markdown_round_trip_fuzz.cpp`: any bytes to the markdown reader;
    an `ok` document must satisfy rules 1 and 2. A second write must equal
    the first.
- **Corpus**, `stx_rtf2md/tests/corpus/`: files authored first-party for this
  repository, reproducing the control-word patterns of Word, WordPad,
  TextEdit and Windows Forms' RichTextBox as described by the specification.
  Each file records its provenance. Files saved by those programs are added
  only as the owner decides (decision 4). Nothing is downloaded.
- Sanitizers: ASan and UBSan over every test locally, through the shared
  sanitizer tree; the Linux sanitizer job in CI.
- CI lists: `stx_rtf2md` joins the `house-style` and `check_lifetime_rules.py`
  directory lists in `native.yml`, the `stx-packages` find-package check,
  and the paths that trigger `fuzz-smoke.yml`. Its licence and provenance
  are installed beside the other `stx_*` ones.

## 10. Phases and gates

Each phase lands as its own pull request (or a stack of at most two) with
its tests, sanitizer run and CI.

- **RT0, this plan.** Gate: the owner answers section 12, and the plan is
  revised to the answers. Met 2026-10-26.
- **RT1, the model and markdown.** The library skeleton, `Document` and
  normal form, `write_markdown`, `MarkdownReader`, the markdown fuzz target
  (rule 1 only, until `write_rtf` exists), and the CI lists. Gate: rule 1
  over the grammar tests and the fuzz smoke; sanitizer clean. **W can start
  `RichTextBox`'s run model once RT1 has landed.**
- **RT2, reading RTF.** `RtfReader`, the loss report, the RTF fuzz target,
  the malformed sweep and the corpus. Gate: every row of section 5 tested;
  the sweep clean; sanitizer clean.
- **RT3, writing RTF.** `write_rtf`, and rule 2 in both fuzz targets. Gate:
  rule 2 over the corpus and the fuzz smoke.
- **RT4, SwiftEdit dogfood** (in SwiftEdit's repository, against a pinned
  GUI.Forms with `stx_rtf2md` installed). Opening a `.rtf` file reads it with
  `RtfReader` and opens the `write_markdown` text as a new untitled Markdown
  document, never as an editable RTF file. A summary of the loss report is
  shown. The command session gains `rtf-to-markdown`. SwiftEdit's Markdown
  view renders attribute spans and block attribute lines instead of showing
  them literally. SwiftEdit never writes RTF in this phase. Gate: SwiftEdit's
  native CI on three platforms, with its own recorded evidence.

Later, and not planned here: code pages beyond 1252, tables written as GFM
tables, and pictures.

## 11. Differences from the W brief

For the coordinator to route to W. None changes the brief's direction.
W's review (2026-10-25, through the coordinator): items 1, 4, 5, 7 and 9
accepted; items 2 and 3 are with the owner; items 6 and 8 revised as the
coordinator decided and recorded below.

1. **Quote is a depth on any block, not a block kind.** This lets a quoted
   list item or heading be represented. `BlockKind` keeps paragraph,
   heading, list item and code block.
2. **Three run attributes are added: superscript, subscript and background
   colour.** RTF documents use them often (footnote marks, formulae,
   highlighting), and dropping them would be the largest everyday loss.
   They are written as `{sup}`, `{sub}` and `{bg=#rrggbb}`. The owner
   admitted them (decision 6).
3. **Alignment needs a markdown spelling**, which the brief does not give:
   the block attribute line `{align=center}`.
4. **The reader reads style names from the stylesheet**, but not style
   formatting. Without this, Word's headings and quotes, and rule 2 for
   quote and code blocks, cannot be recovered. The brief skips
   `\stylesheet` entirely.
5. **Another declared code page is refused only when it is used.** The read
   stops with `unsupported_encoding` when a byte above 0x7F would need
   another code page, not when `\ansicpgN` is merely declared. Word and
   TextEdit declare their system code page and then write `\u` anyway.
6. **Normal form sets size steps to 0 inside headings, and keeps every
   blank.** The first draft trimmed leading and trailing blanks. The
   coordinator decided against that: trimming drops text the user can see,
   and `RichTextBox.text()` must keep the document's spaces. Instead, the
   writer escapes blanks that CommonMark would strip as numeric character
   references (`&#32;`, `&#9;`), and the reader decodes them, so the normal
   form is lossless.
7. **The corpus starts first-party.** Saved files from WordPad, Word,
   TextEdit and Windows Forms depend on applications on the owner's
   machines. WordPad is no longer installed by default on Windows 11 24H2
   and later. Real saves are added only as the owner decides.
8. **Size steps have a stated table** (sections 5 and 6), so that sizes
   round-trip. The steps are relative to the document's body size, so
   `RichTextBox` can map body text to the control's `Font` and scale the
   steps with it. Only RTF's absolute sizes are converted against a
   12-point body.
9. **Phase order puts markdown first**, so W can build the run model before
   RTF reading exists.

Refinements found while building RT1, accepted by the coordinator on
2026-10-26 and sent to W (who replies only to object):

10. **A code block is one text run** holding its lines joined by line
    feeds, so blank lines inside code survive.
11. **List items carry `ordinal`**, the number written on the marker,
    instead of `ordered_start`. Readers keep what they read; adjacent lists
    with different delimiters would otherwise not round-trip.
12. **The emphasis fallback**: the span items `b`, `i` and `s`, written only
    for a block whose delimiters would not read back as the same runs.
    They fall within the owner's attribute-span spelling (decision 2).
13. **Inline code combines with other attributes** (bold code, coloured
    code), where the first draft made it exclusive and lost them.
14. **Line breaks** may open a block or follow one another; none ends a
    block or sits in a heading. **A nested list item follows its parent**,
    so nesting can always be written as indentation.

## 12. Owner decisions, 2026-10-26

The owner answered the questions this section asked, through the
coordinator. Every recommendation was accepted, and question 1 was
answered with a different name.

1. **Name: `stx_rtf2md`.** The owner chose option 1 and wrote `stx_rtf2md`
   in the note. It names the directory, the targets
   (`stx_rtf2md::stx_rtf2md`), the namespace and the documents.
2. **Extension spelling: attribute spans and block attribute lines**, as
   section 4 gives them: `[text]{u color=#c00000}` and `{align=center}`.
3. **Code pages: 1252 only.** Another declared code page is refused only
   when a byte needs it (section 5).
4. **Corpus: first-party authored files now.** Real saves from Word or
   TextEdit are added later, at the owner's request.
5. **SwiftEdit: RTF import only.** An RTF file opens as a new untitled
   Markdown document, and SwiftEdit never writes RTF. This keeps SwiftEdit's
   2026-09-30 exclusion of an RTF editor.
6. **Superscript, subscript and background colour are admitted**, written
   as `{sup}`, `{sub}` and `{bg=#rrggbb}`.

With these answers RT0's gate is met, and RT1 proceeds.

## 13. House-style points

- Typed offsets (`TextOffset`, `SourceOffset`, `TextLength`) are never
  mixed. A conversion between them is a named function.
- No recursion follows input nesting. Group and list nesting use explicit
  stacks reserved from `Limits`.
- Readers are objects that own their reusable workspaces, allocated once at
  construction. Writers fill caller-supplied spans. Documents keep their
  capacity across `clear`.
- Validate, then mutate: a failed call clears its output and leaves no
  partial result.
- Each stage of a conversion has a named intermediate form: tokens, then
  destination events, then runs, then a normalised document, then the
  writer's output.
- No lambdas, no `auto`, `(*pointer).member`, explicit results with
  exceptions only for programmer errors, and effort in counts, never in
  time.
