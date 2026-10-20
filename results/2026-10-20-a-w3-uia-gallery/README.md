# Stage A, step A-W3: the UI Automation client test over the whole Gallery

Date: 2026-10-20. Stage: A, accessibility verified on every host
(`planning/stages/A-accessibility/README.md`). This records the Windows UI
Automation half of the gate, read through the provider of A-W2. TextPattern
(stage A item 5) is not covered. macOS and Linux are not measured here.

## What the test does

`gui_forms_windows_accessibility_uia_gallery_tests`
(`tests/windows_accessibility_uia_gallery_tests.cpp`) runs the Gallery in a
real Win32 window through `host::run_windows`. It reads the window through
`IUIAutomation` (`ElementFromHandle`, then the raw view walker) as a separate
client would, from a thread of its own in the multithreaded apartment.

- The UI thread keeps running its message loop. UI Automation marshals every
  provider call to that thread, because the providers ask for COM threading.
- The client reads and changes the model only through `Window::invoke`, which
  runs on the UI thread in posting order.

It mirrors A-W1 (`results/2026-10-19-a-w1-msaa-gallery`) step for step.

1. **Comparison.** It covers each of the 29 Gallery pages, plus a second
   window holding the registry controls the Gallery does not attach to its
   tree. The client compares every element with the semantic node it
   publishes:
   - control type, name, AutomationId (the stable ID), help text and
     FrameworkId;
   - enabled, keyboard-focusable, has-focus and offscreen;
   - bounding rectangle, and position in set and size of set;
   - the seven patterns each node should or should not offer (Invoke,
     Toggle, SelectionItem, Selection, RangeValue, Value, ExpandCollapse),
     and their states: toggle state, expand-collapse state, is-selected,
     value, range value, minimum and maximum;
   - a tab's `ControllerFor` (its panel);
   - child order and the parent link (`CompareElements`);
   - unique runtime IDs per page.

   The expected UI Automation vocabulary is written out in the test,
   independently of the provider. At the window level only GUI.Forms
   children are compared; the host's non-client parts belong to the system.
2. **Actions.** For every pair of control type and semantic role that has a
   default action (A-W1's choice of node), the client performs it through
   the pattern that action belongs to:
   - press: Invoke, or Toggle for a check box or checked item, or
     SelectionItem.Select for a radio button;
   - select: SelectionItem.Select;
   - expand and show menu: ExpandCollapse.Expand;
   - collapse: ExpandCollapse.Collapse;
   - a node whose only action is focus: SetFocus.

   It observes the change in the semantic tree by A-W1's rules. A toggle or
   an expansion is undone through the same pattern and must return to its
   first state. Actions run in a fresh window per page. One whose change is
   not seen there runs again alone in a fresh window, and that result
   stands; the first try is recorded in the evidence. No action runs on the
   dialogs-and-host-services page.
3. **Registry coverage.** Every visual control in `manifests/controls.json`
   publishes a node of its own, except the eight A-W1 lists as
   presentational.
4. **Stale element.** An element of a control on the first page, kept while
   the second page replaces it, answers `UIA_E_ELEMENTNOTAVAILABLE`.

Reading at one instant: A-W1's client runs on the UI thread, so no timer
fires while it compares. This client reads through many UI-thread calls while
the message loop runs. Without a freeze, the timing page's tick label and
progress bar, the easing preview's value and the motion target's position
changed between the expectation and the reads; the first run reported those
4 mismatches. The comparison windows therefore cancel their frame requests
(`Window::cancel_frame_requests`) after showing each page, before the
expectation is taken. The action windows are not frozen.

## Result

| Measure | Result |
|---|---|
| Elements compared | 2,792 on 30 pages (29 Gallery pages and the off-tree window), the same count as A-W1 |
| Mismatches | 0 (15 on the first run; see below) |
| Tab-to-panel relations (`ControllerFor`) checked | 11 |
| Visual registry controls with a node of their own | 77 of 85; the other 8 are presentational (19 nonvisual entries excluded) |
| Default actions performed | 96 control-type and role pairs, 96 with an observed change |
| Pattern calls | Invoke 42, SetFocus 32, SelectionItem.Select 11, Toggle 5, ExpandCollapse.Expand 4, ExpandCollapse.Collapse 2 |
| Actions that needed a fresh window | 7. In the page's window an earlier action had removed the node (a docked panel closed, a token removed, the navigation pane or calendar changed). A-W1 needs a fresh window for the same 7 and one more. |
| Stale element | `UIA_E_ELEMENTNOTAVAILABLE` after its page was replaced |
| Time | 48 to 49 s per run in a release tree (four runs gave the same report); 741 s in the sanitizer tree, with no ASan or UBSan report |

The per-control table and every action with its evidence are in
[uia-run.txt](uia-run.txt).

## Mismatches found on the first run, and how each was resolved

| Mismatch | Elements | Resolution |
|---|---|---|
| Nodes with no area (overflowed page-strip tabs, ribbon gallery items scrolled out) read as the empty rectangle at 0,0, while the client expected the window origin with zero size | 11 | UI Automation reports an empty rectangle for no area. The provider now answers one explicitly (`get_BoundingRectangle`), and the client expects it. |
| The timing page's tick label and progress bar, the easing preview and the motion target changed while the client read them | 4 | The comparison windows are frozen (above); not a provider defect |

The first CI run (GitHub's windows-2022 runner, a different screen size)
compared 2,800 elements and performed 94 actions. It found one more node with
no area, a label with zero width and nonzero height. The client had kept that
node's height, and now expects the whole rectangle to be empty.

## Workload and environment

- Machine: the owner's Windows 11 desktop, x64, shared with other build
  workers at the time.
- Toolchain: the project's pinned MSYS2 CLANG64 tree, clang 22.1.8, at
  `D:\a\_temp\msys64`.
- Configuration: worker workspace `D:\gui_forms_w3` (`build.sh`: Ninja,
  Release, `cmake/llvm22.cmake`, Gallery, tests, HarfBuzz text, Windows host,
  Skia off).
- Source: branch `claude/a-w3-uia-gallery-client`, on A-W2 (#203).

## Commands

From PowerShell:

```text
$env:MSYSTEM='CLANG64'; $env:MSYS2_PATH_TYPE='inherit'
& D:\a\_temp\setup-msys2\msys2.cmd -c "sh /d/gui_forms_w3/build.sh all"
& D:\a\_temp\setup-msys2\msys2.cmd -c "sh /d/gui_forms_workers/sanitize.sh /d/gui_forms_w3 'uia|semantic'"
```

and the test alone, which wrote `uia-run.txt`:

```text
.build/native-windows-x64/gui-forms/gui_forms_windows_accessibility_uia_gallery_tests.exe
```
