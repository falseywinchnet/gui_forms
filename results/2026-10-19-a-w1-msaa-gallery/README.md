# Stage A, step A-W1: the MSAA client test over the whole Gallery

Date: 2026-10-19. Stage: A, accessibility verified on every host
(`planning/stages/A-accessibility/README.md`). This records the Windows MSAA
half of the gate; UI Automation (A-W2, A-W3), macOS and Linux are not
measured here, so the stage gate is still open.

## What the test does

`gui_forms_windows_accessibility_msaa_gallery_tests`
(`tests/windows_accessibility_msaa_gallery_tests.cpp`) runs the Gallery in a
real Win32 window through `host::run_windows` and reads it as an MSAA client:
`AccessibleObjectFromWindow(OBJID_CLIENT)`, then `get_accChild` down the tree.

1. On each of the 29 Gallery pages, and in a second window holding the
   registry controls the Gallery does not attach to its tree (ButtonBase,
   RangeControl, ScrollBar, ScaledPanel, SplitterPanel, ToolStripPanel,
   PropertyGrid, TaskDialog, FontDialog), it compares every accessible
   object with the semantic node it publishes: role, name, value,
   description, state bits, default-action verb, screen location, child
   count and the parent link. The expected MSAA vocabulary (role table,
   state bits, the Win32 proxies' verbs) is written out in the test,
   independently of the provider.
2. For every pair of control type and semantic role that has a default
   action (or, with focus as its only action, `accSelect(SELFLAG_TAKEFOCUS)`)
   it performs that action through MSAA and observes the change in the
   semantic tree: checked, selected, expanded or focused; a click event; a
   popup or focus scope opening; or, for commands without a state of their
   own, a new semantic generation. Check boxes and expansions are performed
   again and must return to their first state. Actions run in a fresh window
   per page; one whose change is not seen there runs again alone in a fresh
   window, and that result stands. No action runs on the
   dialogs-and-host-services page (a modal dialog would block the client;
   the clipboard is the machine's); its controls are compared like all
   others.
3. It checks the registry: every visual control in `manifests/controls.json`
   publishes a node of its own, except the eight the test lists as
   presentational.
4. An object kept from the first window answers `CO_E_OBJNOTCONNECTED` after
   the window is gone, from `get_accName` and `accDoDefaultAction`.

## Result

| Measure | Result |
|---|---|
| Accessible objects compared | 2,792 on 30 pages (29 Gallery pages and the off-tree window) |
| Mismatches | 0 (2,318 before the fixes below, on the first run against the old provider) |
| Visual registry controls with a node of their own | 77 of 85; the other 8 are presentational (19 nonvisual entries excluded) |
| Default actions performed | 96 control-type and role pairs, 96 with an observed change |
| Stale object after the window closes | `CO_E_OBJNOTCONNECTED` |

The per-control table, every action and its evidence are in
[msaa-run.txt](msaa-run.txt).

## Mismatches found, and how each was resolved

On the first run, against the provider as it was on main (2,318 field
mismatches; a node can count in more than one row):

| Mismatch | Nodes | Resolution |
|---|---|---|
| Virtual items published with runtime ID 0 (Ribbon, BackstageView) resolved to the window's client object: wrong role, name, location, and the client's 82 children | 36 | The window now gives a virtual child published without a runtime ID the one its stable ID gives (`assign_missing_virtual_runtime_ids`, `src/core/window/window.cpp`) |
| Virtual items of different controls share stable IDs (TreeView, TreeListView, BreadcrumbTrail, CorrespondenceView and DataGridView all publish "root", "src", "button.cpp"; the two ribbon galleries both publish "style.Title"), so the provider resolved another control's item, and actions reached the first control in tree order | 13 | The Windows provider names a node by its scope (nearest control-backed node) and its own runtime ID (`windows_semantic_source.hpp`); actions go through the new `Window::perform_scoped_semantic_action` |
| Default-action verbs were internal names ("press", "select", "show_menu") rather than the Win32 proxies' "Press", "Select", "Open", "Expand" | 1,350 | Provider fixed |
| The verb did not follow the role: "press" where a client expects "Check" or "Uncheck" (check boxes), "Jump" (links) or "Open" (menu buttons, menu items with a popup) | 121 | Provider fixed |
| Focus was published as a default action; MSAA reaches focus through `accSelect` | 131 | Provider fixed: such nodes have no default action |
| An expanded node had no default action to collapse it (property groups and rows) | 7 | Provider fixed: collapse when expanded |
| `STATE_SYSTEM_SELECTABLE` (211 nodes), `STATE_SYSTEM_COLLAPSED` (50) and `STATE_SYSTEM_LINKED` (3) were never published; a toggle button reported `CHECKED` rather than `PRESSED` (1) | 265 | Provider fixed |
| Roles fell back to `ROLE_SYSTEM_GROUPING`: date cell (61), date picker (9), property row (8), split pane (5), calendar (3) | 86 | Provider maps them to `CELL`, `DROPLIST`, `ROW`, `PANE`, `TABLE` |
| A disconnected object's `accDoDefaultAction` returned `S_FALSE` (a success code) | 1 check | Provider returns `CO_E_OBJNOTCONNECTED`; `DISP_E_MEMBERNOTFOUND` without a default action; `E_FAIL` when the control refuses |
| A DataGridView row's select, in cell-select mode, selected one cell, so the row never reported selected | 1 action | `DataGridView::on_semantic_child_action` selects the whole row |
| A colour picker's press commits the same colour again and changes no state | 1 action | Not a defect; the test observes its command's `invoked` event |

## Workload and environment

- Machine: the owner's Windows 11 desktop, x64, shared with other build
  workers at the time.
- Toolchain: the project's pinned MSYS2 CLANG64 tree, clang 22.1.8, at
  `D:\a\_temp\msys64`.
- Configuration: worker workspace `D:\gui_forms_w3` (`build.sh`: Ninja,
  Release, `cmake/llvm22.cmake`, Gallery, tests, HarfBuzz text, Windows host,
  Skia off).
- Source: branch `claude/a-w1-msaa-client-test` on main `13d1c7e0`.

## Commands

From PowerShell:

```text
$env:MSYSTEM='CLANG64'; $env:MSYS2_PATH_TYPE='inherit'
& D:\a\_temp\setup-msys2\msys2.cmd -c "sh /d/gui_forms_w3/build.sh all"
& D:\a\_temp\setup-msys2\msys2.cmd -c "sh /d/gui_forms_workers/sanitize.sh /d/gui_forms_w3 'accessib|data_grid|semantic_tests'"
```

and the test alone, which wrote `msaa-run.txt` in 9.4 s wall time:

```text
.build/native-windows-x64/gui-forms/gui_forms_windows_accessibility_msaa_gallery_tests.exe
```
