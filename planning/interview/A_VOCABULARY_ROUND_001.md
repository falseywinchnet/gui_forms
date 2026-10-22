# Stage A accessibility vocabulary interview, round 001

Date: 2026-10-21

Status: **answered.** The owner accepted every recommended option.

## Background
Stage A's core gate holds on all three hosts. Every stock control is
checked through MSAA, UI Automation, AT-SPI and macOS AX, with zero
mismatches, as the records under `results/` show. The workers found the
gaps that remained: semantic roles, fields and text support that no atom
expresses yet. Adding an atom is the owner's decision (design language
§12.7), so these questions went to the owner.

## The interview
The owner answered on a local HTML page,
`planning/stages/A-accessibility/vocabulary_round_001_source.html`. It holds
the page's source with the embedded fonts removed. Each question explained
its options.

The owner's answer, verbatim:

> i have accepted all recommended options for this stage in stage A.

A-M1 (who finishes the macOS PRs) had already been answered in the
conversation: a worker here finished them through CI, and the M4 ran only
the final gate.

The answers are transcribed below, each with its option number. This
record is not rewritten: later answers and corrections are added as dated
notes.

## A. Roles
- **A-R1.** 1: `tree` and `tree_item` roles. Items gain a `level` field
  beside `position_in_set` and `set_size`, and keep `expanded`.
- **A-R2.** 1: `table`, `row`, `cell`, `column_header` and `row_header`
  roles, with row and column index and count fields.
- **A-R3.** 1: a `tree_grid` role: table semantics with `tree_item` rows
  (TreeListView).
- **A-R4.** 1: a `dialog` role with a `modal` flag.
- **A-R5.** 1: a `status_bar` role.
- **A-R6.** 1: no new roles for DockNavigator or TokenBox. Their
  compositions stay, with clear names.

## B. Fields and states
- **A-F1.** 1: a `checkable` state.
- **A-F2.** 1: an `access_key` field, filled from the control's mnemonic.
- **A-F3.** 1: `small_step` and `large_step` fields on range nodes.
- **A-F4.** 1: a `multiselectable` state on lists, trees and grids.
- **A-F5.** 1: the docking pin stays a check box, announced exactly through
  `checkable`.

## C. Editable text
- **A-T1.** 1: two node fields plus two on-demand window queries.
  - The `text_selection` field holds anchor and caret as UTF-8 offsets into
    the value, on grapheme boundaries. It is absent for password boxes.
  - The `multiline` field.
  - A window query returns the text layout (visual lines, grapheme
    positions, word starts), built only when a host asks.
  - A window query sets the selection. It refuses stale identities,
    positions off a boundary, and targets outside the focus scope.
  - No new action is added.
- **A-T2.** 1: the defaults.
  - Offsets are UTF-8 in the core, converted per host.
  - Words follow the control's own Ctrl+Arrow rule, and lines are visual
    lines.
  - Sentences are reported unsupported.
  - A caret placed inside a combining sequence is refused.
  - Password boxes publish no text or layout.
  - This pass covers TextBox (single-line and multiline), MaskedTextBox and
    TokenBox's editor. The hidden editors of ComboBox, DomainUpDown and
    NumericUpDown follow later.

## M. macOS
- **A-M1.** 1: a worker here finished #194 and #195 through CI, and the M4
  ran the final gate (#221, `results/2026-10-20-a-macos`).

## Already decided
- **DK-51:** a live-region role for the docking move mode's announcements.
  It is built in the same vocabulary pass.

## What follows
1. **The vocabulary pass:** the atoms in the core, the schema and the docs.
   Then the controls publish them: trees, grids, the tree-grid, dialogs,
   the status strip, `checkable`, access keys, steps, `multiselectable`
   and the live region. Each is mapped on MSAA, UI Automation, AT-SPI and
   macOS AX, and the Gallery client tests check it on all three hosts.
2. **Editable text** on the A-T1 core: AT-SPI Text, UI Automation
   TextPattern and the macOS text attributes.
3. **The capability:** each host then advertises
   `HostCapability::accessibility`.
