# Stage A: accessibility verified on every host

Release: 0.2 Verified. Depends on: nothing. Runs beside every other stage.
Status 2026-10-20: started. Windows: the MSAA client test enumerates the
whole Gallery and performs every control's default action (A-W1,
`results/2026-10-19-a-w1-msaa-gallery`); the UI Automation provider is in
place (A-W2) and its client test does the same through IUIAutomation, each
default action through its pattern (A-W3,
`results/2026-10-20-a-w3-uia-gallery`). Linux: AT-SPI client tests of a
fixture (A-L1) and of the whole Gallery (A-L2,
`results/2026-10-20-a-l2-linux-gallery`). macOS:
`gui_forms_macos_accessibility_tests` (in process, runs on CI) and
`gui_forms_macos_accessibility_client_tests` (AXUIElement client; skipped as
unsupported without the Accessibility permission) registered; gate run
recorded in `results/2026-10-20-a-macos`. Editable text ranges (item 5) are
in place on all three hosts (ATK Text, UIA TextPattern, AppKit text
attributes, each with client tests), and every native host advertises
`HostCapability::accessibility` (item 6, 2026-10-22). Authority: `planning/FORWARD_PROGRAMME.md`
§3 A; this brief adds context and does not change the gate.

Read [the shared brief](../README.md) first.

## Why

The owner, 2026-10-12: accessibility tools and native, highly accessible
behaviour by design are "a core focus for me — I want blind people to have no
trouble with this. I can't test with software they use, but having
addressable controls, selectable controls, and self-describing controls as
one would expect normally is desirable." Not the largest priority, but "an
early one to get right", and the owner did not know whether it was done.

After hearing the state below, the owner settled the scope: "we expose
capability and make sure it works right. We don't bother with integration."
So the stage verifies GUI.Forms through each platform's own accessibility
API. Driving VoiceOver, NVDA, JAWS or Narrator (for example with Guidepup) is
out of scope.

Accessibility also constrains every theming stage: a theme or the engine
never changes semantics, roles, names, actions or the accessibility tree
(interview GF018–GF019; design language §2.9 rules 1–3). Native publication
is never turned off (accessibility round A3).

## What exists today

- **Semantic tree.** `include/gui_forms/semantics/types/semantic_types.hpp`
  defines `SemanticRole` (about 35 roles: button, check box, slider, tab,
  date cell, property row, menu item …), `SemanticState` flags,
  `SemanticAction` (focus, press, select, increment, decrement, expand,
  collapse, show menu, set value) and `SemanticDescriptor`. Snapshots live in
  `src/core/semantics/snapshot/`. Stock controls supply default adapters;
  authored help metadata is optional and never required to create a control
  (interview GF012–GF013).
- **macOS.** NSAccessibility publication inside
  `src/host/macos/application/macos_host.mm` (`native_accessibility_role` and
  the element classes near it). Tested in process by
  `gui_forms_macos_accessibility_tests` and through AXUIElement by
  `gui_forms_macos_accessibility_client_tests` (and its Gallery variant),
  against the contract in `tests/support/macos_accessibility_contract.cpp`;
  the stock-control fixture is `tests/support/stock_control_fixture.cpp`.
  A control's own text box publishes AppKit's text attributes on the A-T1
  core (AXNumberOfCharacters, AXSelectedTextRange, which a client may set,
  AXSelectedText, AXVisibleCharacterRange, AXInsertionPointLineNumber) and
  the parameterized ones (AXStringForRange, AXAttributedStringForRange,
  AXBoundsForRange, AXRangeForLine, AXLineForIndex, AXRangeForPosition,
  AXRangeForIndex, AXStyleRangeForIndex), in UTF-16 code units, and posts
  AXValueChanged and AXSelectedTextChanged; both tests check them.
- **Windows.** MSAA (`IAccessible`) in
  `src/host/windows/accessibility/windows_accessibility.cpp`. Test
  `tests/windows_accessibility_tests.cpp`, registered in `CMakeLists.txt` as
  `gui_forms_windows_accessibility_tests`; since A-W1 also the Gallery-wide
  client test `tests/windows_accessibility_msaa_gallery_tests.cpp`. Since
  A-W2, UI Automation providers in
  `src/host/windows/accessibility/windows_automation.cpp`: a fragment root
  per window served for `UiaRootObjectId`, a fragment per semantic node,
  AutomationId from stable IDs, and the Invoke, Toggle, Value, RangeValue,
  ExpandCollapse, SelectionItem and Selection patterns from the semantic
  actions and states. Every provider uses COM threading, so calls arrive on
  the UI thread; Invoke posts through `Window::begin_invoke`. A stale element
  answers `UIA_E_ELEMENTNOTAVAILABLE`. Tests
  `tests/windows_accessibility_uia_provider_tests.cpp` and, over the whole
  Gallery, `tests/windows_accessibility_uia_gallery_tests.cpp`.
  TextPattern and TextPattern2 for a control's own text box
  (`windows_text_automation.cpp`, on the A-T1 core): UTF-8 ranges,
  grapheme characters, the control's words, visual lines and paragraphs,
  Select through `Window::set_semantic_text_selection`, and the
  TextSelectionChanged and TextChanged events; tests
  `tests/windows_accessibility_uia_text_tests.cpp`, and the Gallery client
  compares each text box's text and selection offsets.
- **Linux.** ATK/AT-SPI in `src/host/linux/accessibility/`. A fixture
  (`tests/linux_accessibility_fixture.cpp`, built when ATK and X11 are found)
  and a Python AT-SPI client (`tests/linux_accessibility_client.py`) exist
  but are not registered with `ctest`.
- **Capability.** `HostCapability::accessibility` exists in
  `include/gui_forms/host/types/host_capability/host_capability.hpp`; the
  macOS, Windows and Linux hosts advertise it (the headless host does not),
  and each host's accessibility or host test checks that.
- **Keyboard reach.** Tab order, focus scopes, mnemonics and accept/cancel
  targets are implemented and tested.
- **Text.** Editable text ranges are not exposed on any host, so a client
  cannot read by character or word inside a text box.

## What to deliver

1. Registered tests on each host that query the host's own accessibility API
   from a client's point of view: AX (`AXUIElement`) on macOS; MSAA and UI
   Automation (`IUIAutomation`) on Windows; AT-SPI over D-Bus on Linux.
2. For every stock control in the Gallery (`manifests/controls.json` lists
   them): role, name, state, value, supported actions and keyboard reach
   match what the semantic tree reports.
3. Each control's default action performed through the native API, with the
   resulting state change observed.
4. A Windows UI Automation provider (Narrator and modern tools expect UIA;
   MSAA alone covers only basics).
5. Editable text ranges for single and multiline text boxes: macOS text
   attributes, UIA `TextPattern`, AT-SPI `Text`.
6. Then, and only then, each host advertises `HostCapability::accessibility`.

## How

- Start with the cheapest proof: register the existing Linux fixture and
  client under `ctest` (CI needs a D-Bus session bus and the AT-SPI registry:
  run under `dbus-run-session` with Xvfb), and extend the Windows test to
  enumerate the Gallery. Then write the macOS client test.
- **macOS risk to settle first:** an AX client normally needs the
  Accessibility (TCC) permission. Find out early whether the CI runner and the
  M4 allow it. If only in-process NSAccessibility queries are possible on CI,
  that is weaker evidence; raise it with the owner as an option list rather
  than quietly downgrading the gate.
- **Threading (house style §8):** UI Automation can call a provider on a
  thread other than the UI thread unless the provider opts into COM
  threading (`ProviderOptions_UseComThreading`). Decide deliberately and
  write down which execution context touches the semantic tree; the tree has
  one owner, the UI thread. AT-SPI and AX calls arrive on the main loop.
- **Foreign interfaces (house style §9):** every provider states the types,
  counts, ownership and error results it returns, and how a stale element is
  rejected after its control is disposed (generational identity, interview
  GF017). A client holding an element for a destroyed control gets an error,
  never a crash.
- Win32, COM and UIA types stay inside `src/host/windows/`; AppKit types
  inside `src/host/macos/`; ATK inside `src/host/linux/`.
- Stable IDs may be exposed as the platform automation identifier
  (interview GF012–GF013).
- Focus visibility is a separate design-language rule (§7.3): text fields
  draw focus whenever focused, other controls after keyboard use, and host
  high contrast or focus-visibility settings force it on. Accessibility
  clients follow focus through the API, not the drawn ring.

## Gate

On each host, an automated test enumerates the Gallery through the native API
and finds every stock control with the role, name, state and actions the
semantic tree reports, and performs each control's default action through
that API. Record the run (host, runner or machine, commands, per-control
results) under `results/` with a dated name, and update the stage index.

## Owner decisions, vocabulary round 001 (2026-10-21)

Recorded in `planning/interview/A_VOCABULARY_ROUND_001.md`. The owner accepted
every recommended option.

### Roles
- **GIVEN A-R1.** `tree` and `tree_item`, with a `level` field.
- **GIVEN A-R2.** `table`, `row`, `cell`, `column_header` and `row_header`,
  with row and column index and count fields.
- **GIVEN A-R3.** `tree_grid`.
- **GIVEN A-R4.** `dialog`, with a `modal` flag.
- **GIVEN A-R5.** `status_bar`.
- **GIVEN A-R6.** No new roles for DockNavigator or TokenBox.

### Fields and states
- **GIVEN A-F1.** A `checkable` state.
- **GIVEN A-F2.** An `access_key` field.
- **GIVEN A-F3.** `small_step` and `large_step` fields.
- **GIVEN A-F4.** A `multiselectable` state.
- **GIVEN A-F5.** The docking pin stays a check box.

### Editable text
- **GIVEN A-T1.** A `text_selection` field (UTF-8 anchor and caret, absent
  for password boxes) and a `multiline` field. Two on-demand window
  queries: the text layout, and setting the selection. No new action.
- **GIVEN A-T2.** The text defaults:
  - UTF-8 in the core;
  - the control's own word rule;
  - visual lines;
  - sentences unsupported;
  - a caret inside a combining sequence refused;
  - no text published for password boxes;
  - this pass covers TextBox, MaskedTextBox and TokenBox's editor.

### Already decided
- **DK-51.** The live-region role is built in the same pass.

## Open decisions (ask, do not decide)

None recorded. If the macOS permission issue limits what CI can prove, that
becomes a decision for the owner.

## Read first

- `planning/interview/GUI_FORMS_ACCESSIBILITY_AND_TEXT_ROUND_001.md`
  (A-numbered and T-numbered answers; LCD text admission there is superseded
  by design language §12.3)
- `planning/interview/GUI_FORMS_INTERVIEW_LEDGER.md` GF012–GF013, GF017,
  GF018–GF019
- `planning/DESIGN_LANGUAGE_SPECIFICATION.md` §2.9, §5.9, §7.3, §8.4
- `docs/user/hosts-and-capabilities.md`
