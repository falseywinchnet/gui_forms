# Stage W: Windows Forms control coverage

Release: lands across 0.4 Flexible to 0.6 Bindings. Depends on: T4 (new
controls are built articulated and engine-painted from the start). Feeds S,
and the 1.0 manifest condition. Status 2026-10-14: started; coverage
surveyed 2026-10-25 (see "Coverage, 2026-10-25"). The catalogue is
regenerated from the official surface (deliverable 1); the by-role paint path
new controls paint through is in place; decision 1 is revised (decision 3).
Added on the owner's request. Authority: `planning/FORWARD_PROGRAMME.md` §5 W.

Read [the shared brief](../README.md) first.

## Why

The owner, 2026-10-12: "I do want to try to expand on the number of controls
in Forms, but 9850 does include some massive amount of deprecated redundancy
probably from the .NET 2.0 era." On 2026-10-13, asked whether the plan covered
fleshing out the remaining Windows Forms controls (it did not; no stage owned
it), the owner set the scope:

> no dotnet 2.0 items. only items supported by winforms of at least .net 8.0

and added: "that also eliminates .net 4.5".

## The scope rule

A Windows Forms type or member is in scope only if it is **supported by
Windows Forms on .NET 8.0 or later**. Concretely:

- It is present in the official `Microsoft.WindowsDesktop.App.Ref` reference
  assemblies for .NET 8.0 or a later version.
- It is not marked `[Obsolete]` there, and it is not a compatibility stub that
  throws `PlatformNotSupportedException`.
- It is out of scope if it exists only in .NET Framework (2.0 through 4.x,
  4.5 included).
- It is out of scope if it was removed in .NET Core 3.1: `DataGrid`,
  `ToolBar`, `StatusBar`, `MainMenu`, `ContextMenu`, `MenuItem` and their
  helpers. That holds even where later versions carry them as non-functional
  binary-compatibility types.
- The existing GUI.Forms exclusions still apply on top: ActiveX and browser
  hosting, printing, MDI, design-time services, raw HWND and message
  emulation (ADR-001), and the non-UI component tray
  (`planning/CONTROL_COMPLETENESS_MATRIX.md` rows 45–50).

Everything else in that surface is admitted, then ordered by priority.

## What exists today

- `planning/CONTROL_COMPLETENESS_MATRIX.md`: 50 rows with priorities and
  measured states.
- `planning/WINFORMS_API_CATALOGUE.md` and
  `planning/generated/WINFORMS_API_CATALOGUE.tsv`, regenerated on 2026-10-14
  from the official shipped API lists of `dotnet/winforms` (release 8.0, 9.0
  and 10.0) with `[Obsolete]` read from the `Microsoft.WindowsDesktop.App.Ref`
  packs: 14,762 Forms rows, 12,934 in scope, 9,469 of them `missing`. The
  previous catalogue (the LibreWinForms fork's list) had 14,575 rows and
  9,850 `missing`. The summary lists every admitted type still `missing`.
- `planning/WINFORMS_BEHAVIOR_GAP_CATALOGUE.md`, and
  `docs/user/for-windows-forms-developers.md` (the public correspondence
  page).
- 72 GUI.Forms control types (`manifests/controls.json`).

Missing or partial, per the matrix:

| Priority | Controls |
|---|---|
| P2 | `DataGridView` (columns, cells, editing controls); `BindingSource` breadth; `BackgroundWorker` |
| P3 | `ListView` (items, subitems, columns, groups, insertion mark); `ToolStrip`, `StatusStrip` and `ToolStripContainer` completion, with hosted items and overflow; `MaskedTextBox`; `DomainUpDown`; `MonthCalendar`; `NotifyIcon`; `TabControl` overflow, images and owner draw |
| P4 | `RichTextBox` (package) |

## What to deliver

1. **Rebuild the catalogue against the official surface.** Done 2026-10-14
   (see "What exists today"). The member lists come from the official
   `PublicAPI.Shipped.txt` files and `[Obsolete]` from the reference
   assemblies; the binary-compatibility stubs that throw
   `PlatformNotSupportedException` are the `[Obsolete]` WFDEV006 types, which
   the exclusions already remove. Regenerate when a new version ships (ask the
   owner before downloading; `tools/winforms_catalogue/README.md`).
2. **Implement the missing controls in priority order**, with corpus capture
   (stage S) able to raise anything real applications use. Order:
   `DataGridView`, `ListView`, the `ToolStrip` / `StatusStrip` family first,
   since S needs them; then the P3 controls; then member depth on existing
   controls.
3. **Classify every in-scope type and member** as supported, deferred or
   excluded in the machine-readable manifest, and generate the docs from it
   (1.0 condition).

## How

- **Every new control is articulated from birth.** It publishes named parts
  and states (T1) using only atoms of the vocabulary, and paints every part
  through `paint_part` (`gui_forms/theme/part_painting/part_painting.hpp`)
  with geometry from the same functions that publish its anatomy: no
  control-specific drawing. T4 replaces that path with the engine. A part
  that needs a role, state or mark the vocabulary lacks is a vocabulary
  decision for the owner, not a local addition. It has semantic adapters and
  native accessibility (A).
- **Behaviour is GUI.Forms' own.** The aim is broad familiarity, not strict
  compatibility (interview GF009–GF011; ADR-003 ordering; ADR-014
  lifecycle). Incorrect historical ordering is not copied. Large collections
  virtualize (only visible rows are realized), as the existing lists and
  trees do.
- **Done means a test.** A control or member counts as supported only with a
  test in `tests/` that exercises it on every CI platform (house rule; matrix
  "honest states").
- Follow the house style in full. New controls live in their own directories
  under `src/controls/` and `include/gui_forms/controls/`.

## Gate

Accepted by the owner on 2026-10-13: the catalogue is regenerated from the official
.NET 8+ reference surface; every in-scope matrix row is either implemented
and tested on every platform, or excluded or deferred with a written reason;
the manifest records each in-scope type and member, and the docs are generated
from it. Record under `results/`.

## Owner decisions, 2026-10-13

1. **Timing.** Start after T4, so no new control is hand-painted and later
   moved onto the engine. Revised by decision 3.
2. **The gate above is accepted.**
3. **The atom vocabulary is the canonical source** (owner, 2026-10-14; design
   language §12.7). The owner asked where the canonical definition of what
   can be addressed and manipulated lives, since controls and the engine each
   mirror what the other offers. Answer accepted: neither end; both declare
   against the vocabulary of part roles, states, value types and engine
   inputs. So a control built now, from vocabulary atoms only and painted
   through the by-role path, needs no move when the engine arrives, and W may
   start before T4 on those terms.
4. **Grids and tree lists use stable row identity** (owner, 2026-10-14,
   accepting the recommendation). A virtual grid, `TreeListView` and
   `DataGridView`'s virtual mode use `ObjectView`'s stable-ID virtualization
   rather than a second row-ownership model. `DataGridView`'s virtual mode
   today asks its provider for values by row number
   (`DataGridViewCellValueProvider::cell_value(row, column)` in
   `data_grid_view.hpp`); it moves onto stable row IDs, and row numbers
   become presentation positions only. The control is provisional, so the
   change is allowed. Done 2026-10-17 for `DataGridView`: the provider
   answers by `DataGridViewRowId`, the application publishes the row order
   as IDs, and every row source keeps selection, the current cell and edits
   by ID (matrix row 52 records the tests).
5. **An adaptive order model for virtual grids (owner review R9,
   2026-10-19).** The grid holds its order in an edit mode (the program
   reports inserts, removals and moves by ID; a chunked order index makes
   each cost proportional to the change) or a window mode (the grid asks
   for the IDs at the visible positions plus a margin and caches that
   window), switching automatically by row count and change rate and
   reporting each switch; a full republish stays as a reset.
6. **TextBox keys (owner review N1, N2, 2026-10-19).** Escape and Enter
   pass on to the window's cancel and accept buttons unless the box has
   something to cancel or accept; editors that need them opt in with
   `accepts_escape`/`accepts_enter`. An empty `TokenBox` passes Enter on.
7. **Content colour in the paint path (owner, 2026-10-19, C1-1).** A
   content-colour value on `PartPaint` (for a swatch, a colour field, a
   palette or a chart series) so such parts paint by role; the ribbon colour
   picker's direct-paint bridge is replaced by it. Admitted to the design
   language and schema vocabulary with the field.

## Owner decisions, 2026-10-25

The owner answered the W interview (round 1) on 2026-10-25, through the
coordinator.

1. **`RichTextBox` (W-Q1): an inline markdown-like rich text field, not
   Windows Forms' RTF-native design.** Its text is an inline run model
   (paragraphs of styled runs) that reads and writes a markdown-like
   dialect. RTF goes through a standalone first-party parser library inside
   `gui_forms/`, built the way `stx_zlib` is: no GUI.Forms dependency, its own
   tests, fuzz target and provenance. SwiftEdit consumes and dogfoods the
   library, and a SwiftEdit worker builds it under house style. The
   [rich text brief](RICH_TEXT_BRIEF.md) states the library's scope and
   the seam `RichTextBox` needs; W builds the run model and the control once
   that seam exists, not before.
2. **`Splitter` (W-Q2): a thin migration facade.** A `Splitter` docked
   between dock children resizes its neighbour, built on GUI.Forms' dock
   layout. New code uses `SplitContainer`.
3. **`MessageBox.show` (W-Q3): the host's message dialog**, the same rule
   as the file and colour dialogs (`HostMessageDialogRequest` through
   `HostServices`). `TaskDialog` stays the themed, richer in-window dialog.
4. **A link role in the paint vocabulary (W-Q4)**, with normal, hover,
   visited and disabled states. `LinkLabel` moves onto it with its goldens
   unchanged, then `DataGridViewLinkColumn` is built on it. The role is a
   vocabulary addition after the T2 seal, so it lands in its own PR, which
   records the vocabulary's new version and digest and the built-in themes'
   recipes for the role.

## Owner decisions, 2026-10-26

The owner answered W-Q5 and W-Q6 on 2026-10-26, through the coordinator,
both as recommended.

5. **The link role (W-Q5) is two families, `link` and `visited_link`**,
   chosen by the link's visited flag. Hover keeps the normal link colour.
   This refines decision 4's state list. The vocabulary PR comes first, with
   `LinkLabel`'s goldens unchanged, then `DataGridViewLinkColumn` on top of
   it.
6. **Owner drawing (W-Q6) is content-only.** A per-item draw event receives
   GUI.Forms' `Painter` and the item's content rectangle; the plate,
   selection and focus stay painted by role. It lands as one cross-control
   seam PR first, then `ListView` and `TabControl` adopt it, and later the
   `ListBox` family.

The rich text library is named `stx_rtf2md` (RT-Q1); the
[rich text brief](RICH_TEXT_BRIEF.md) records it and the other answers.

## Advanced candidates, 2026-10-14

The owner listed common advanced controls from other frameworks and
admitted them as candidates (matrix rows 51 to 57). Built in W, by
workers, once decision 4's grid work is planned:

- `TreeListView` (row 53): `TreeView`'s flat model with the shared details
  columns (`src/controls/panel/details_columns.hpp`).
- The virtual grid on stable row identity (row 52), and `DataGridView`'s
  virtual mode moved onto it.
- Advanced editors (row 56): Windows Forms `AutoCompleteMode`,
  `AutoCompleteSource` and `AutoCompleteCustomSource` on `TextBox` and
  `ComboBox` (in the .NET 8 catalogue); a searchable, filtering
  `ComboBox`; a token box.

Studied elsewhere first: `PageView` (row 51) in stage DK, because docking
reuses it; the ribbon (row 54) in stage RB; `Diagram` (row 55) and
`DataLayout` (row 57, after stage FD) in studies of their own.

## Coverage, 2026-10-25

A survey of every Windows Forms control and UI component in
`planning/generated/WINFORMS_API_CATALOGUE.tsv` against `manifests/controls.json`,
the public headers and `tests/`, on `main` at `d6111581`. The types are the
catalogue's `Control` and `ToolStripItem` descendants (found by their
`CreateParams`, `DefaultSize`, `OnPaint` and `WndProc` overrides) and its UI
components; event arguments, enumerations and nested records are counted
with their owner.

- **done**: a GUI.Forms type covers the control's core use, has tests in
  `tests/`, and is in the Gallery. Member-level gaps remain, and are the
  catalogue's business.
- **partial**: a counterpart exists but lacks a part of the control's core
  use, named in the row.
- **missing**: no counterpart.

The catalogue's own `status` column has not been remapped since 2026-10-14
(it still reads `missing` for `ListView`, `DataGridView`, `MonthCalendar` and
the strips, all built since); the states below come from the code. Remapping
the member rows is deliverable 3. "Rows" is the type's own admitted rows.

### Commands, text and labels

| Windows Forms | Rows | GUI.Forms | State |
|---|---:|---|---|
| `ButtonBase` | 73 | `ButtonBase` | done |
| `Button` | 22 | `Button` | done |
| `CheckBox` | 33 | `CheckBox` | done |
| `RadioButton` | 30 | `RadioButton` | done |
| `Label` | 74 | `Label` | done |
| `LinkLabel` | 55 | `LinkLabel` | done |
| `TextBoxBase` | 75 | none public | partial: no public base; `TextBox` carries the surface |
| `TextBox` | 42 | `TextBox` | done (single and multiple lines, auto-complete #123) |
| `MaskedTextBox` | 102 | `MaskedTextBox` | done (#89) |
| `RichTextBox` | 128 | none | missing: matrix row 42 places it as a P4 package; its scope is an owner decision |

### Lists, trees and grids

| Windows Forms | Rows | GUI.Forms | State |
|---|---:|---|---|
| `ListControl` | 46 | none public | partial: no public base; the list controls carry the surface |
| `ListBox` | 109 | `ListBox` | done |
| `CheckedListBox` | 52 | `CheckedListBox` | done |
| `ComboBox` | 129 | `ComboBox` | done (auto-complete #123, filtering #126) |
| `ListView` | 181 | `ListView`, `ObjectView` | partial: groups, tile view, label editing, the insertion mark, owner drawing and item tooltips are not supported (#88) |
| `ColumnHeader` | 27 | `ListViewColumn` records | done |
| `TreeView` | 137 | `TreeView` | done |
| `DataGridView` | 513 | `DataGridView` (provisional) | partial: text, check-box, combo-box and button columns; no image or link column (#90, #117, #149) |
| `DataGridViewTextBoxColumn`, `CheckBoxColumn`, `ComboBoxColumn`, `ButtonColumn` | 68 | `DataGridViewColumnKind` | done |
| `DataGridViewImageColumn` | 19 | none | missing |
| `DataGridViewLinkColumn` | 20 | none | missing |
| `DataGridViewTextBoxEditingControl`, `ComboBoxEditingControl` | 40 | the grid's own editors | partial: not public types |
| `PropertyGrid` | 141 | `PropertyGrid` | done |

### Values and ranges

| Windows Forms | Rows | GUI.Forms | State |
|---|---:|---|---|
| `UpDownBase` | 74 | none public | partial: no public base; the spin parts are shared (`spin_buttons.cpp`) |
| `NumericUpDown` | 37 | `NumericUpDown` | done |
| `DomainUpDown` | 22 | `DomainUpDown` | done (#85, #92) |
| `TrackBar` | 73 | `TrackBar` | done |
| `ProgressBar` | 68 | `ProgressBar` | done |
| `ScrollBar`, `HScrollBar`, `VScrollBar` | 75 | the same three | done |
| `DateTimePicker` | 92 | `DateTimePicker` | done |
| `MonthCalendar` | 112 | `MonthCalendar` | done (#91) |

### Containers and windows

| Windows Forms | Rows | GUI.Forms | State |
|---|---:|---|---|
| `Control` | 439 | `Control` | done (the base) |
| `ScrollableControl` | 37 | `ScrollableControl` | done |
| `ContainerControl` | 42 | `ContainerControl` | done |
| `UserControl` | 25 | `UserControl` | done |
| `Form` | 224 | `Window`, `Application` | partial: the top level is `Window`, not a control; MDI is excluded |
| `Panel` | 22 | `Panel` | done |
| `GroupBox` | 41 | `GroupBox` | done |
| `FlowLayoutPanel` | 9 | `FlowLayoutPanel` | done |
| `TableLayoutPanel` | 39 | `TableLayoutPanel` | done |
| `SplitContainer`, `SplitterPanel` | 130 | the same two | done |
| `Splitter` | 57 | none | missing: matrix row 45 holds it to a migration facade over `SplitContainer`; building it is an owner decision |
| `TabControl` | 98 | `TabControl` | partial: tab images (`ImageList`, `ImageIndex`) and owner drawing are not supported (re-based on `PageStrip`, #137) |
| `TabPage` | 55 | `TabPage` | done |

### Strips and menus

| Windows Forms | Rows | GUI.Forms | State |
|---|---:|---|---|
| `ToolStrip` | 156 | `ToolStrip` | done (#86, #93) |
| `MenuStrip` | 24 | `MenuStrip` | done |
| `StatusStrip` | 32 | `StatusStrip` | done (#86) |
| `ContextMenuStrip` | 8 | `ContextMenu` | done |
| `ToolStripContainer` | 48 | `ToolStripContainer` | done (#93); dragging strips between panels is DK phase D7 |
| `ToolStripPanel` | 57 | `ToolStripPanel` | done |
| `ToolStripContentPanel` | 48 | `ToolStripContainer::content_panel()`, a `Panel` | done |
| `ToolStripPanelRow` | 21 | the panel's rows | partial: not a public type |
| `ToolStripItem`, `ToolStripDropDownItem` | 246 | item records (`ToolStripItemKind`) | done |
| `ToolStripButton`, `ToolStripLabel`, `ToolStripSeparator`, `ToolStripDropDownButton`, `ToolStripSplitButton` | 164 | item kinds | done |
| `ToolStripComboBox`, `ToolStripTextBox`, `ToolStripProgressBar` | 195 | hosted item kinds | done (#93) |
| `ToolStripControlHost` | 88 | none | partial: the hosted kinds are fixed to the three above; an arbitrary hosted control is not supported |
| `ToolStripStatusLabel` | 22 | `StatusStrip` labels | done |
| `ToolStripMenuItem` | 49 | menu items | done |
| `ToolStripOverflow` | 9 | the strip's overflow menu | done |
| `ToolStripDropDown`, `ToolStripDropDownMenu` | 135 | `ContextMenu`, `AnchoredPopupLayer` | partial: no public drop-down that hosts arbitrary content |
| `BindingNavigator` | 33 | `BindingNavigator` | done (#93) |

### Components

| Windows Forms | Rows | GUI.Forms | State |
|---|---:|---|---|
| `ToolTip` | 53 | `ToolTip` | done |
| `ErrorProvider` | 35 | `ErrorProvider` | done |
| `HelpProvider` | 17 | `HelpProvider` | done |
| `ImageList` | 21 | `ImageList` | done |
| `Timer` | 15 | `Timer` | done |
| `BindingSource` | 91 | `BindingSource` | done |
| `NotifyIcon` | 31 | `NotifyIcon` (provisional) | done (#97), refused per host where the capability is absent |

`BackgroundWorker` (`System.ComponentModel`, outside the Forms catalogue) is
`BackgroundWorker`.

### Dialogs

| Windows Forms | Rows | GUI.Forms | State |
|---|---:|---|---|
| `CommonDialog` | 12 | none | missing |
| `FileDialog` | 46 | none | missing |
| `OpenFileDialog` | 18 | `HostOpenFileDialogRequest` | partial: a host service (`HostServices::show_dialog`) on every host, no Windows Forms-shaped component |
| `SaveFileDialog` | 12 | `HostSaveFileDialogRequest` | partial: as `OpenFileDialog` |
| `FolderBrowserDialog` | 31 | `HostFolderDialogRequest` | partial: as `OpenFileDialog` |
| `ColorDialog` | 21 | `HostColorDialogRequest` | partial: as `OpenFileDialog` |
| `FontDialog` | 40 | `FontDialog` | done (in the window, bundled faces only) |
| `TaskDialog` | 9 | `TaskDialog` (provisional) | done (in the window) |
| `MessageBox` | 22 | `show_message_box` (the host's message dialog) | done (host-drawn) |
| `ThreadExceptionDialog` | 5 | `ThreadExceptionDialog` (a `TaskDialog` preset) | done (in the window) |

### Excluded

`AxHost`, `WebBrowser`, `WebBrowserBase` (ActiveX and browser hosting),
`MdiClient` (MDI), `PrintPreviewControl`, `PrintPreviewDialog`,
`PrintDialog`, `PageSetupDialog` (printing), `ComponentEditorPage`
(design time); `NativeWindow` is a platform-extension review row under
ADR-001.

### Totals

Of the 93 types in the tables above (rows that name several types count
each): 68 done, 18 partial, 7 missing. The missing ones are `RichTextBox`, `Splitter`, two `DataGridView` column
types, `CommonDialog`, `FileDialog` and `ThreadExceptionDialog`; four of the
partial ones (`OpenFileDialog`, `SaveFileDialog`, `FolderBrowserDialog`,
`ColorDialog`) lack only the component over an existing host service.

### Advanced candidates

`TreeListView` (row 53) is built (#116); `DataGridView` rows are on stable
row identity (decision 4, #117) with the adaptive order model (decision 5,
#149); the advanced editors (row 56) are built: auto-complete (#123), the
filtering `ComboBox` (#126) and `TokenBox` (#128, #152). `PageView` (row
51) and the ribbon (row 54) are below. `Diagram` (row 55) has its study
(stage DG) and waits for the owner's interview; `DataLayout` (row 57) waits
for stage FD.

### Docking (DK) phases

| Phase | State | PRs |
|---|---|---|
| W0 windowing foundation | done | #114 (W0a), #115 (W0b), #119 (W0c), #121 (W0d) |
| V0 vocabulary and command chain | done | #113; #131, #147 (key maps, routed commands) |
| P1 `PageSet` | done | #110 |
| P2 `PageStrip` top and bottom | done | #118 |
| P3 `PageView` strip preset | done | #120 |
| P4 document preset | done | #124 |
| P5 side strips | done | #127 |
| P6 drag hooks | done | #129 |
| P7 `NavigationPane` | done | #122 |
| P8 navigation preset and `TabControl` | half: `TabControl` re-based (#137); the navigation preset waits for the P4 question to be asked again | #137 |
| D1 layout model and XML | done | #111, #112, #146 |
| D2 docked layout in one window | done | #130 |
| D3 dragging in one window | done | #144, #158 |
| D4 floating windows | done | #168 |
| D5 auto-hide | done | #145 |
| D6 documents (cancellable `closing`, close all and close others) | left | |
| D7 persistence integration, named layouts, `ToolStripContainer` drags | left | |
| D8 integration and evidence (Gallery docking page, host accessibility, T4 look) | left | |

Reviews and fixes: #170, #173, #176, #184.

### Ribbon (RB) phases

| Phase | State | PRs |
|---|---|---|
| 0 vocabulary and shared seams | done | #96 |
| 1 static ribbon | done | #99 |
| 2a scaling study | done | #101 (`results/2026-10-14-ribbon-scaling-observation`) |
| 2b adaptive scaling | done | #139 |
| 3 galleries and preview | done | #98 |
| 4 QAT, minimized ribbon, tooltips, state | done | #102 (landed through #108), #107 |
| 5 contextual tabs and modes | done | #143 |
| 6 file area and specialized controls | done | #148 |
| 7 integration and evidence (Gallery ribbon page, form document, host accessibility, T4 look) | left: waits for FD, T4 and stage A's fixture | |

Reviews and fixes: #157, #167, #177.

### Next batch

Ordered by how often Windows Forms applications use them. None needs a
vocabulary atom: the four dialogs paint nothing (each is a component over a
host service that every host already answers or refuses), and the grid's
columns paint with roles that exist.

1. **`OpenFileDialog`**: the Windows Forms filter string, file names, title,
   initial directory, multiple selection and a cancellable `file_ok`.
   `CommonDialog` and `FileDialog` do not become public bases: nothing needs
   them at run time (house style §7), so the two file dialogs share the
   filter parser instead. The filter index waits for the host request to
   carry one.
2. **`SaveFileDialog`**: default extension, add extension, overwrite prompt.
3. **`FolderBrowserDialog`**.
4. **`ColorDialog`**, over the host's colour dialog.
5. **`DataGridViewImageColumn` and `DataGridViewLinkColumn`**: the grid's
   two missing column kinds, painted by role with the `image` and `label`
   roles that exist.

Not in the batch, because each needs an owner decision first: `RichTextBox`
(its scope; matrix row 42), `Splitter` (row 45's facade), a `MessageBox`
facade (`TaskDialog` and the host message dialog both exist; which one
`show` uses), and the `ListView` and `TabControl` gaps above, which are
member depth on existing controls.

## Read first

- `planning/CONTROL_COMPLETENESS_MATRIX.md`, `planning/WINFORMS_API_CATALOGUE.md`,
  `planning/WINFORMS_BEHAVIOR_GAP_CATALOGUE.md`, `tools/winforms_catalogue/README.md`
- `planning/interview/WINFORMS_CONTROL_INVENTORY.md`
- `decisions/ADR-001-portable-compatibility-controls.md`
- [T1](../T1-anatomy/README.md), [T4](../T4-default-theme/README.md),
  [S](../S-csharp-swap-in/README.md) briefs
