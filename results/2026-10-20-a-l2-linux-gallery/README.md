# A-L2: the whole Gallery through AT-SPI on Linux

Date: 2026-10-20. Stage: A (accessibility verified on every host), Linux
step 2. Pull request #197 (branch `claude/a-l2-linux-gallery-atspi`),
stacked on #200 (the core routing API). Status: the test passes on CI on
linux-x64 and linux-arm64; the defects listed below are open.

## What is checked

`gui_forms_linux_accessibility_gallery_tests` runs
`tests/linux_accessibility_gallery_client.py`, a Python AT-SPI client
(`gi.repository.Atspi`), against `tests/linux_accessibility_gallery_fixture.cpp`,
which publishes the Gallery and a supplement window (ButtonBase, ScrollBar,
RangeControl) through the ATK adapter in `src/host/linux/accessibility/`.
The client runs in its own `dbus-run-session` with the AT-SPI bus launcher,
under Xvfb. It reads the published tree over D-Bus and compares it with the
semantic tree the fixture reports as JSON:

1. every node of every Gallery page and of the supplement window: child
   order, accessible id (the stable ID), role, name, description, states,
   interfaces, actions in published order, numeric value and range, value
   text, text of a text box, position in set and set size, and the
   tab-to-panel relation;
2. every type in `manifests/controls.json` accounted for: compared on a
   page, opened by a default action, nonvisual, or not exposed by the
   semantic tree;
3. for one instance of each type with an action, the default action (AT-SPI
   action 0) performed through the `Action` interface, the whole published
   tree compared again, and a change observed (a click, the node's own state
   or value, another node, an opened popup or dialog, or a retained
   mutation);
4. generational identity: a Button is disposed and replaced by one with the
   same stable ID; the old object must not reach the new button, and the new
   object must answer its own action.

AT-SPI `Text` is published read-only (`get_text`); editable text ranges
are the next step and are not checked here.

## Host and commands

- CI, workflow "GUI.Forms programs and release" (`.github/workflows/release.yml`),
  jobs `programs (ubuntu-24.04, linux-x64, LINUX, bash)` and
  `programs (ubuntu-24.04-arm, linux-arm64, LINUX, bash)`; runner image
  ubuntu-24.04 version 20261018.339.1; at-spi2-core 2.52.0, python3-gi,
  gir1.2-atspi-2.0. The Gallery test is configured only with
  `GUI_FORMS_BUILD_GALLERY`, which this workflow sets, so it does not run in
  native.yml or the sanitizer job.
- The workflow runs
  `xvfb-run -a -s "-screen 0 1280x1024x24 -noreset" ctest --test-dir "$BUILD_DIR" --output-on-failure --timeout 120`;
  the test's own `TIMEOUT` is 900 s.
- No Linux machine was used; every run below is CI.

## Result

Green run: head `95e1aa1f`, programs run 39462104430.

```text
linux-x64:   175/175 Test #175: gui_forms_linux_accessibility_gallery_tests ......   Passed    5.97 sec
             100% tests passed, 0 tests failed out of 175
linux-arm64: 175/175 Test #175: gui_forms_linux_accessibility_gallery_tests ......   Passed    6.85 sec
             100% tests passed, 0 tests failed out of 175
```

ctest prints a test's output only when it fails, so the per-type table
below is the client's own output from the run before (head `fe636c05`,
programs run 39458834852, linux-x64), kept beside this file as
`client-output-fe636c05-linux-x64.txt`. That run differed from the green
one only in how the client judged the six entries under "Mismatches" (the
fixture, adapter and Gallery were the same).

| Measure | Count |
|---|---|
| Registry types | 104 |
| Compared over AT-SPI (at least one published instance matched) | 78 |
| Nonvisual components (no node of their own) | 19 |
| Not exposed by the semantic tree | 7 (6 in run fe636c05, plus `visual_inspector_overlay` after the fix below) |
| Gallery pages walked | 29, plus the supplement window |
| Nodes in the first walk of the 29 pages | 2,743 (the same count a headless walk of the model gives on Windows) |
| Node comparisons in all (first walk plus a full re-walk after each action) | 8,514 |
| Node-level mismatches (role, name, states, actions, values, children, ids, relations) | 0 |
| Default actions performed | 49 (one per type with an enabled instance that publishes an action, three of them openers of a dialog or popup) |
| Default actions with an observed change | 42 |
| Default actions with no change, listed as known | 7 (below) |
| Types compared that publish no action on any enabled instance | 29 (labels, panels, pictures, progress bars and similar) |
| Generational identity | the old object was refused and is defunct; the replacement was pressed once |
| AT-SPI round trip (200 name reads of the frame) | 0.040 ms mean |
| Test time | 4.77 s (fe636c05, x64); 5.97 s x64 and 6.85 s arm64 (95e1aa1f) |

The types not exposed by the semantic tree are `range_control` (the
supplement's bare RangeControl), `property_grid` (its descriptor is not
exposed and includes its descendants; its PropertyList publishes the
rows, and `property_list` is compared), `scaled_panel` (62 instances),
`splitter_panel` (4), `scrollable_control` (1), `tool_strip_panel` (4)
and `visual_inspector_overlay` (a diagnostic overlay, hidden until
enabled, whose descriptor is not exposed).

## Default actions without an observed change (listed in the client)

A listed entry that starts to show a change fails the test until it leaves
the list.

| Target | Action | Why |
|---|---|---|
| TokenBox `gallery.text.tokens.recipients` | focus | refused by the window (below) |
| MaskedTextBox `gallery.text.masked.phone` | focus | refused by the window |
| NumericUpDown `gallery.values.numeric.0` | focus | refused by the window |
| DomainUpDown `gallery.values.domain` | focus | refused by the window |
| SplitContainer `gallery.containers.split` | focus | refused by the window |
| rack module `gallery.value-editors.rack.lamp` | decrement | the module emits `move_requested` (move earlier); it is the first module and the rack keeps the order |
| RibbonColorPicker `gallery.commands.ribbon-file-area.colour` | press | applies the current colour (Automatic) again |

The five refusals are one defect, outside the ATK adapter. Each of these
controls publishes the focus action without the focusable state, and its
own `on_semantic_action` moves focus to its inner editor (for
SplitContainer, to its splitter). `Window::perform_semantic_action`
handles focus itself: it asks `request_focus` for the control, which is not
focusable, and refuses, so the control's own handler never runs. A headless
probe on Windows (this branch) gives the same refusal for all five. The fix
belongs in the core routing (#200's file); it is not made here.

## Iterations and how each was resolved

| Head | Programs run | Linux result | Resolution |
|---|---|---|---|
| `21a35704` (before this record) | 39379788795 | compile error: `reference to 'Window' is ambiguous` in the fixture (Xlib's global `Window` typedef against `gui_forms::Window` under `using namespace gui_forms`) | `2c817d9c` qualifies every `gui_forms` name in the fixture |
| `2c817d9c` | 39443190133 | test timed out at 900 s on both architectures with no output | the client wrote only at the end, block-buffered; `87bd2be1` adds progress lines flushed as it goes and a 780 s budget |
| `87bd2be1` | 39448674161 | timed out again with no output, not even the first progress line | `e9e5ed08` adds a faulthandler watchdog at 840 s and checks the budget inside the tree walk |
| `e9e5ed08` | 39452743009 | the watchdog showed the client stuck for 14 minutes in its first name read of the fixture's application (`find_frames`) | the client had set libatspi's cache mask to none; A-L1's client sets none and reads the same property at once. `fe636c05` sets no cache mask (without one and without libatspi's event loop, every read but an object's interfaces is a D-Bus call) and sets libatspi's call timeout. Why the mask stopped the read was not investigated further. The same head merges #200 and uses its `is_virtual_semantic_runtime_id` |
| `fe636c05` | 39458834852 | completed in 4.8 s: 0 node mismatches; 6 mismatches (the five refusals reported as succeeded; `visual_inspector_overlay` not found) | at-spi2-atk answered `DoAction` with true for actions the window refused, so the answer says nothing: `95e1aa1f` judges every action by its observed effect and keeps a false answer as a mismatch. The fixture now reports whether each control's descriptor is exposed, and a control that is not is accounted as not exposed, visible or not |
| `95e1aa1f` | 39462104430 | passed on linux-x64 and linux-arm64 | |

## Not covered

- Editable text ranges (AT-SPI `Text` beyond the whole text, and
  `EditableText` beyond setting the whole contents): the next step of
  stage A.
- Keyboard reach is not driven through AT-SPI here; tab order and
  mnemonics are covered by the core suites.
- `HostCapability::accessibility` is still not advertised (stage A
  deliverable 6).
- Screen readers (Orca) are out of scope by the owner's ruling.
