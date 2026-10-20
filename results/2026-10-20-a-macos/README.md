# Stage A, macOS: accessibility verified through AX

Date: 2026-10-20 (final runs 05:26 UTC). Stage: A, accessibility verified
on every host (`planning/stages/A-accessibility/README.md`), macOS part.
Windows and Linux are recorded separately.

## The gate (macOS)

> An automated test enumerates the Gallery through the native API and finds
> every stock control with the role, name, state and actions the semantic
> tree reports, and performs each control's default action through that API.

| Condition | Result |
|---|---|
| Enumerate the Gallery through AX | `gui_forms_macos_accessibility_client_gallery_tests` launches the Gallery, presses each of its 29 navigation rows through `AXUIElementPerformAction`, and walks every page from outside the process through `AXUIElementCreateApplication`: 81 control types, 956 control instances, **2,795 of 2,795** node readings match the semantic tree. |
| Every stock control: role, name, state, value, actions | The contract below, checked per node. In process (`gui_forms_macos_accessibility_tests`): 80 placed stock controls, **580 of 580** readings over two passes. Through AX (`gui_forms_macos_accessibility_client_tests`, same fixture): 83 control types, 162 instances, **585 of 585**. |
| Each control's default action through AX | In the stock-control fixture, every control with an enabled action other than focus had it performed and its effect observed: 54 controls in process, through the NSAccessibility element; 51 through AX (`AXPress`, `AXIncrement`, `AXShowMenu`, or setting `AXValue` or `AXSelected`), all observed. The client acts on the first control on each path from the window, so it reaches TabPage through the tab control, and ToolStripPanel and PropertyGrid (which publish no node of their own) through their strips and editors. In the Gallery the default action performed is each page's navigation press (29). See "Not covered". |
| Stale element (interview GF017) | The client holds the element of a control, has the app dispose of it, then reads and presses the held element: both return `kAXErrorInvalidUIElement` (-25202) and the app keeps running. In process, a press on the removed control's element is refused. |
| CI without the Accessibility permission | The AX client checks `AXIsProcessTrusted()` first; when false it prints `unsupported: …` and exits 77, which ctest reports as **Skipped**, never as passed. The in-process test needs no permission and runs on CI. |

Per-control tables: [per-control-fixture.md](per-control-fixture.md) (the
stock-control fixture, in process and through AX) and
[per-control-gallery.md](per-control-gallery.md) (the Gallery, by type). Raw
output: [in-process.txt](in-process.txt), [ax-client-fixture.txt](ax-client-fixture.txt),
[ax-client-gallery.txt](ax-client-gallery.txt).

## Environment

- Machine: the owner's Mac mini, Apple M4, 1920 × 1080 display, macOS 26.5
  (25F71), Xcode 26.5 (17F42), MacOSX SDK 26.5.
- Compiler: Homebrew clang 22.1.8 (`/opt/homebrew/opt/llvm@22`), the pinned
  libc++ 22.1.8 runtime for macOS 14, `cmake/llvm22.cmake`.
- Layout: the CI layout of `docs/building/fast-builds.md`, workspace
  `~/Developer/CodexBuilds/gui_forms-escape` (`gui_forms/`, `.build/`,
  `.ccache/`), with ccache.
- Permission: `AXIsProcessTrusted()` was already true for processes started
  over ssh on the M4 (checked with `osascript` and again inside the client).
  No system setting was changed.
- Source: `claude/a-macos-ax-client`, stacked on `claude/a-macos-in-process`,
  on `main` at 13d1c7e0.

## Commands

From the workspace, with the environment of `docs/building/fast-builds.md`:

```text
python3 -B gui_forms/tools/ci_build.py --gallery --jobs 8
#   186/186 tests passed (the three accessibility tests among them), installed-SDK examples 11/11

B=.build/native-macos-arm64/gui-forms
$B/gui_forms_macos_accessibility_tests.app/Contents/MacOS/gui_forms_macos_accessibility_tests
$B/gui_forms_macos_accessibility_client_tests --app $B/gui_forms_macos_accessibility_fixture_app.app --dump /tmp/ax-fixture.json
$B/gui_forms_macos_accessibility_client_tests --gallery --app $B/gui_forms_macos_accessibility_gallery_app.app --dump /tmp/ax-gallery.json

python3 -B gui_forms/tools/ci_build.py --jobs 8 --sanitize --tests accessib
#   2/2 passed under ASan, UBSan and LSan (the in-process test and the AX client)

python3 -B tools/check_house_style.py include src demo tests examples tools file_manager_demoboard stx_vorbis    # 0 findings
python3 -B tools/check_lifetime_rules.py include src demo tests examples tools file_manager_demoboard stx_vorbis # 0 findings
```

The sanitized client launches its app through LaunchServices, which does not
pass the sanitizer options on, so the app runs with AddressSanitizer
instrumented but without leak checking. Each AX run was repeated three times
with the same result.

## The contract

Written in `tests/support/macos_accessibility_contract.cpp`, apart from the
host, and checked by both tests:

| Semantic | AX |
|---|---|
| role | static_text AXStaticText; button, split_button, date_cell AXButton; check_box, check_list_item AXCheckBox; radio_button AXRadioButton; tab AXRadioButton with subrole **AXTabButton**; link AXLink; text_box, numeric_field AXTextField; list AXList; list_item AXRow; combo_box, date_picker AXComboBox; slider AXSlider; scroll_bar AXScrollBar; progress_bar AXProgressIndicator; image AXImage; tab_group AXTabGroup; split_pane AXSplitGroup; tool_tip, property_row AXStaticText; menu_bar AXMenuBar; menu_bar_item AXMenuBarItem; menu AXMenu; menu_item AXMenuItem; menu_button AXMenuButton; toolbar AXToolbar; radio_group AXRadioGroup; tab_panel AXGroup with subrole AXTabPanel; generic, group, calendar, property_grid, property_group, separator, pane AXGroup |
| name | AXDescription |
| stable id | AXIdentifier |
| value | check box, check list item, radio button 0, 1 or 2 (mixed); tab 1 when selected; a numeric value as the number; otherwise the value text |
| states | enabled AXEnabled, focused AXFocused, selected AXSelected, expanded AXExpanded |
| actions | AXPress for press, select, expand or collapse; AXShowMenu for show menu or expand; AXIncrement, AXDecrement; settable AXFocused for focus, AXValue for set value, AXSelected for select; a custom action "Close" for close |
| children | AX children in the semantic order, one per semantic child |

## Mismatches found and how they were resolved

| Found | Resolution |
|---|---|
| Tabs published no subrole and an empty value, so a client could not tell a tab from a radio button or read which tab is selected. | Fixed in the host: a tab has AppKit's tab-button subrole (`NSAccessibilityTabButtonSubrole`) and AXValue 1 when selected, as NSTabView's tabs do. |
| `metrics_view` and `visual_inspector_view` values (live paint counters) and the Gallery's animated `easing_preview` value change without a new semantic generation, so a dump and the AX read that follows it could differ. | Not a publication error. The app rewrites its dump whenever the content changes; the client checks a moving value against the values the node took around the read and reports it as a note (`live value gallery.animation.easing`). |
| Test errors during development (an open popup replacing the window's roots; a stale "before" dump; unsorted rows needed to see a sort; values a field rejects). | Corrected in the tests; no product change. |

## Vocabulary gaps (owner decisions, design language §12.7)

Not invented here; each would be an addition to the semantic vocabulary:

1. **No tree role.** TreeView, TreeListView, ObjectView and the property
   rows publish lists and list items (AXList / AXRow) with an expanded
   state; there is no outline role, level or disclosure triangle, so a
   client cannot read nesting depth (AppKit: AXOutline, AXDisclosureLevel,
   AXDisclosing).
2. **No table, column, row-header or cell roles.** ListView details,
   DataGridView and TreeListView publish lists, items and column headers as
   buttons; no row or column index (AppKit: AXTable, AXColumn, AXCell,
   AXRowIndexRange).
3. **text_box does not say single or multiple lines**, so macOS cannot
   publish AXTextArea for a multiline box; editable text ranges are a
   separate stage A item.
4. **States without a macOS projection:** read_only, busy and invalid are
   published by the semantic tree but not through NSAccessibility (AppKit
   has no public attribute for invalid or busy; read-only shows only as an
   AXValue that is not settable when set_value is absent).
5. **set_size and has_popup** have no AppKit attribute and are not published
   (position_in_set is, as AXIndex).

## Other findings for the owner

- **Controls that publish no node of their own:** PropertyGrid publishes its
  rows directly (the `property_grid` role exists but the control's own node
  is not exposed), ToolStripPanel publishes its strips directly, ReviewCard
  publishes nothing even when interactive, and RangeControl,
  ScrollableControl and VisualInspectorOverlay publish nothing.
- **MonthCalendar and DateTimePicker** publish their value in the long form
  ("Tuesday, October 20, 2026") but accept only an ISO date when set, so an
  AX client cannot write back the value it reads.
- **menu_bar → AXMenuBar** for an in-window MenuStrip: AppKit's AXMenuBar is
  the application's system menu bar; VoiceOver may treat an in-window one
  differently.
- **AXTabPanel** is WebKit's subrole for ARIA tab panels; AppKit declares no
  constant for it.
- **CommandOverflowPanel throws during layout when it has no groups**
  ("ResponsiveTrackPanel requires 1 through 64 tracks"); found while
  building the fixture, not an accessibility defect.
- The retained view publishes itself as an AXGroup labelled "GUI.Forms
  retained surface" (English, the same in every window).

## Not covered

- **Gallery default actions** other than page navigation: the Gallery's
  pages include buttons that open native dialogs and file panels, which a
  client would have to dismiss; every stock control type's default action is
  instead performed in the stock-control fixture, through AX. Performing
  every Gallery instance's default action is a possible follow-up.
- Keyboard reach through AX, notifications (focus, value and layout changed)
  and editable text ranges: other stage A items.
- Screen readers (VoiceOver) are out of scope by the owner's decision.
- `HostCapability::accessibility` is not advertised; the brief makes that the
  last step, after text ranges.
