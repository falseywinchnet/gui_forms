# Stage A: accessibility verified on every host

Release: 0.2 Verified. Depends on: nothing. Runs beside every other stage.
Status 2026-10-20: started. macOS: `gui_forms_macos_accessibility_tests`
(in process, runs on CI) and `gui_forms_macos_accessibility_client_tests`
(AXUIElement client; skipped as unsupported without the Accessibility
permission) registered; gate run recorded in `results/2026-10-20-a-macos`. Authority: `planning/FORWARD_PROGRAMME.md`
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
- **Windows.** MSAA (`IAccessible`) in
  `src/host/windows/accessibility/windows_accessibility.cpp`. Test
  `tests/windows_accessibility_tests.cpp`, registered in `CMakeLists.txt` as
  `gui_forms_windows_accessibility_tests`. There is no UI Automation provider.
- **Linux.** ATK/AT-SPI in `src/host/linux/accessibility/`. A fixture
  (`tests/linux_accessibility_fixture.cpp`, built when ATK and X11 are found)
  and a Python AT-SPI client (`tests/linux_accessibility_client.py`) exist
  but are not registered with `ctest`.
- **Capability.** `HostCapability::accessibility` exists in
  `include/gui_forms/host/types/host_capability/host_capability.hpp`; no host
  advertises it.
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
