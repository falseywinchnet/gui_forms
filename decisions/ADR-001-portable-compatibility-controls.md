# ADR-001: Port handle-dependent compatibility controls to portable behaviour; no general Win32 emulation

Status: accepted (recorded retroactively)

Date: 2026-10-11 (decision in force since the compatibility programme began;
first written down on this date)

## Context

GUI.Forms carries a compatibility programme for an existing Forms-shaped
application corpus (the retired compatibility specimen and its admitted
plugins). Several of that corpus's custom controls depend on Windows window
handles and messages: they subclass `WndProc`, send or intercept `WM_*`
messages, or draw through GDI on a handle they obtained from the framework.

GUI.Forms is retained, portable and CPU-rendered. Its core and host protocol
target Windows, macOS and Linux equally, and no portable semantic may encode
Win32 assumptions. A general HWND-and-message emulation layer would make the
Windows message model the hidden reference semantics of a toolkit whose other
hosts have no such model.

This decision was cited as ADR-001 by `planning/MASTER_IMPLEMENTATION_PLAN.md`
(compatibility constraints) and `docs/LIBRARY_AND_ASSEMBLY_GUIDE.md` (section
8, compatibility promise) before this file existed.

## Decision

Compatibility stops at calls into the Forms-shaped facade.

- Corpus custom controls that depend on native handles or messages are
  **ported** to GUI.Forms drawing and input while preserving their admitted
  public behaviour. The port is a GUI.Forms control, tested with the same
  normative traces as a stock control.
- The base facade is **not** enlarged with general HWND or message emulation.
  `WndProc` overrides, raw message dispatch and undocumented message-order
  accidents are excluded from the compatibility promise.
- ActiveX, browser hosting, printing, MDI and obsolete control families are
  not admitted by the compatibility rule; they are excluded or packaged
  separately with an explicit migration path.
- The only native-handle concession is the bounded compatibility handle lease
  defined by ADR-014: an invisible, offscreen native surface so unchanged code
  can call native or GDI APIs safely. It is never the visual or input
  authority for the retained tree.

## Consequences

- Every ported control receives a GUI.Forms behavioural trace; a deviation
  from the original names the blocker and the chosen invariant rather than
  reproducing the Windows accident.
- The portable core, controls, DML and C ABI contain no Win32 type; a boundary
  test enforces this on every platform.
- Consumers that need raw native interop use the handle lease and accept its
  limits (not parented, shown, focused or composited as a native child).
- Compatibility coverage is measured against the captured call catalogue
  (ADR-002), not against the ability to host arbitrary native code.

## Rejected options

- **Emulate HWNDs and the message loop portably.** Rejected: it fixes the
  Windows model as the semantics of every host and multiplies the conformance
  surface without bounding it.
- **Admit native child windows on Windows only.** Rejected: it creates a
  Windows-only retained tree with different composition, hit-testing and
  accessibility, contradicting the identical-behaviour goal.
