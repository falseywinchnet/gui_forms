# Stage W: Windows Forms control coverage

Release: lands across 0.4 Flexible to 0.6 Bindings. Depends on: T4 (new
controls are built articulated and engine-painted from the start). Feeds S,
and the 1.0 manifest condition. Status 2026-10-14: started. The catalogue is
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

## Read first

- `planning/CONTROL_COMPLETENESS_MATRIX.md`, `planning/WINFORMS_API_CATALOGUE.md`,
  `planning/WINFORMS_BEHAVIOR_GAP_CATALOGUE.md`, `tools/winforms_catalogue/README.md`
- `planning/interview/WINFORMS_CONTROL_INVENTORY.md`
- `decisions/ADR-001-portable-compatibility-controls.md`
- [T1](../T1-anatomy/README.md), [T4](../T4-default-theme/README.md),
  [S](../S-csharp-swap-in/README.md) briefs
