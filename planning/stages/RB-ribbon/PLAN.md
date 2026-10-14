# Ribbon framework: plan

Date: 2026-10-14. Status: **accepted by the owner on 2026-10-14** ("i
accept"), closing stage RB's gate; building proceeds in stage W, phase 0
first. Sources: the decisions in `README.md` (from interview
round 001, `planning/interview/RIBBON_INTERVIEW_ROUND_001.md`) and the study,
`RIBBON_STUDY.md`. Where this plan and a recorded decision disagree, the
decision governs; fix the plan.

The ribbon is built in stage W (decision 1), under the atom vocabulary ruling
(design language §12.7): every part is painted through `paint_part`, the
controls use only admitted atoms, and its look comes from the theme (T4) and
its motion from the engine (T2).

## What the framework is

The Office 2007 to 2010 ribbon (decision 10), as GUI.Forms controls:

| Type | Kind | What it holds |
|---|---|---|
| `Ribbon` | control; item systems `tab`, `tab_set`, `group`, `command` | The tab row (always visible), the File tab, the command plane of the selected tab, groups with captions and dialog launchers, commands as records (decision 2): button, toggle, split, drop-down; contextual tab sets as a tint (decision 25); the minimized state (decisions 19, 24). |
| hosted controls | real child controls | Combo box, numeric up-down (spinner), check box, text box; later the font control and colour picker. Placed by the ribbon's layout, as `ToolStrip` places its hosted items. |
| `RibbonGallery` | control; item systems `gallery_item`, `gallery_category` | In-ribbon and drop-down galleries, live preview (decision 13). |
| `QuickAccessToolbar` | control, from `ToolStrip` | Flat buttons on the tab row beside the File tab, or below the ribbon by the user's choice; commands only; customize and overflow buttons (decisions 12, 24). |
| `RibbonApplicationMenu` | component, as `ContextMenu` | The File tab's two-pane menu (commands, recent items), a popup over the ribbon (decisions 3, 23). |
| `BackstageView` | control | Later: the File tab's full page, a dark left column of commands and topics beside the topic's page (decisions 3, 26). |
| `RibbonToolTip` | component | Enhanced tooltip: title, description, shortcut, image, availability reason, F1 help topic (decision 13). |
| value commands | types sharing identity with `Command` | Spinner, combo, gallery, colour and font values (decision 13). |

Keyboard (decisions 16, 17): shortcuts, mnemonics, Tab and arrow navigation,
and F6 between window regions. No KeyTips. Menus opened from the ribbon are
popups over it, never laid out inside it (decision 23).

## Phases

Each phase ends with tests in `tests/` that CI runs on every platform, and
its own pull request (or stack of them).

**Phase 0. Vocabulary and shared seams.** Admit, together in the design
language (§8, §12), `anatomy_types.hpp`, `semantic_types.hpp` and the schema:
the dialog-launcher, "more", overflow (double chevron) and customize marks
(decision 20); a per-item accent value (decision 22); the semantic roles
`split_button` and `menu_button` and the node fields position-in-set,
set-size and has-popup (decision 22), each mapped on the three hosts (MSAA on
Windows, AX on macOS, ATK on Linux) with stage A. `ToolStrip`'s overflow
button adopts the overflow mark. Remove `Command::key_tip` (decision 18). Add
the window-level ordered list of regions that F6 cycles (decision 17), used
first by side panels. Tests: anatomy and schema validation of the new atoms;
F6 order across a window with three regions; semantic snapshots carrying the
new roles and fields.

**Phase 1. Static ribbon.** `Ribbon` with core tabs; clicking the selected
tab minimizes or opens the command plane, tabs always visible (decision 19);
groups at their largest size with captions, dividers and dialog launchers;
button, toggle, split and drop-down commands as records bound to shared
`Command`s; labels always shown (decision 6); hosted combo box, spinner, check
box and text box; the File tab opening `RibbonApplicationMenu` over the
ribbon; mnemonics on tab and command labels; Tab and arrow navigation;
anatomy, schema and semantics; every part through `paint_part`. Tests: part
geometry; one command path for pointer, keyboard and semantic invoke; command
state changes repaint only their item; minimizing by tab click; schema check.

**Phase 2. Adaptive scaling.** Two steps.
- *2a. The cross-ribbon scaling study* (decisions 4, 11): how Office 2007 and
  2010, the Windows Ribbon Framework (WordPad on the owner's machine), MFC,
  WPF and Fluent.Ribbon shrink groups, from their documents and, asking
  before each use, from ribbons running on the owner's Windows machine (the
  owner offered to install Office). Recorded under `results/` with the width
  ladders observed and the machine and builds used. It settles the default
  reduction order and any gap in the model below before 2b is built.
- *2b. Scaling* in the WPF/MFC model (decision 5): per-group size
  definitions and a tab reduction order, with a deterministic default when
  the order runs out; groups shrink by image size, rows and finally a
  collapsed-group button (icon, caption and chevron, decision 27) that opens
  the group in a popup; galleries give up one column at a time (decision 8);
  labels never drop (decision 6); a content-derived rule hides the command
  plane in a small window (decision 7); widths cached per step. Tests: golden
  layouts at a width ladder and at text scales 1 and 1.5; the reduction order
  followed; the hide rule at its threshold; comparison with 2a's recorded
  ladders where the models agree.

**Phase 3. Galleries and preview.** `RibbonGallery` in the ribbon and in
drop-down and split-button popups, categories, virtualization; value commands
(decision 13); preview and cancel-preview after a 300 ms hover and on keyboard
focus. Tests: layout and column steps; preview event order on hover, keyboard,
commit and cancel; virtualization bound.

**Phase 4. Quick Access Toolbar, minimized ribbon, tooltips, state.** The
QAT on the tab row or below the ribbon, flat buttons, commands only, its
customize menu and overflow (decisions 12, 24); the minimized ribbon's
temporary popup with the overlay shadow, a click outside it consumed
(decisions 12, 24); `RibbonToolTip` with F1 help; the ribbon saving QAT
items and position and the minimized state through the runtime layout store,
with explicit save and load (decision 12). Tests: QAT placement and overflow;
the popup's open, close and focus order; persistence round trip.

**Phase 5. Contextual tabs and modes.** Tab sets tinted from the theme's
accent ramp (decisions 14, 22, 25); application modes; the built-in selection
rules (a set's first tab selected when it appears, return to it, fall back to
Home). Tests: availability changes and the selection rules; mode switches
showing the right core tabs.

**Phase 6. File area and specialized controls.** The application menu's
recent-items pane; `BackstageView` with its left column of commands and topics
(decisions 3, 26); the font control and drop-down colour picker as hosted
controls with value commands. Tests: Backstage navigation, Escape and File
returning to the document; the specialized controls' value commands.

**Phase 7. Integration and evidence.** A Gallery ribbon page; the ribbon
described in a form document once stage FD exists; host accessibility
verification of a ribbon fixture with stage A; the look from T4: the dithered
button on every large element that conventionally shows a hover square,
sunk into the plane at rest and rising under the pointer, the rise animated
by a T2 settling primitive (decisions 15, 23), tabs never bulged; reference
images. Porting Plan Paint's ribbon as the consumer test is the owner's call.

## Dependencies

- W's strip work: `ToolStrip`, `StatusStrip`, hosted strip items and
  `ToolStripContainer` (PRs #86 and #93) supply the record layout, hosted
  controls and overflow the ribbon reuses.
- The by-role paint path (#84). The look waits for T2 and T4; until then the
  ribbon draws from today's theme recipes like every W control.
- Stage A for the host mappings of the new semantic roles and fields.
- Stage FD for declarative ribbon documents (phase 7); until then ribbons are
  described in C++ records.

## Gate

Each phase's tests pass on every CI platform; phase 2a's study is recorded
under `results/`; the owner accepts the Gallery ribbon page (phase 7).
