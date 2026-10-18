# Docking and PageView interview, round 001

Date: 2026-10-17

Status: **in progress.** Round 1 asks scope and structure: docking A1 to A4
and B1 to B7 (with B5a), and PageView P1 to P6.

The questions are section 8 of `planning/stages/DK-docking/DOCKING_STUDY.md`
(A to K, T1 to T8) and section 7 of
`planning/stages/DK-docking/PAGEVIEW_STUDY.md` (P1 to P35, PT1 to PT8), cited
by their IDs. Each answer names the option number of that question. This
record is not rewritten: later answers and corrections are added as dated
notes.

## Part 1. B5 in detail, 2026-10-17

The owner asked to discuss B5 (floating windows on macOS) in more detail
before answering. It was split into four questions:

- **B5-i, hiding when the application is inactive** (the Mac palette
  convention; possible on every host, since each reports application
  activation): (1) floating panels stay visible everywhere, with a workspace
  option, off by default, to hide floating tool panels while the application
  is inactive, on every host, torn-out documents never hiding (recommended);
  (2) the same with the option on by default on macOS only; (3) always hide
  on macOS, never elsewhere (today's behaviour, an AppKit assumption).
- **B5-ii, moving with the main window** (macOS child windows): (1) never
  (recommended; multiple monitors); (2) only while touching the main
  window's frame; (3) always.
- **B5-iii, macOS full screen:** (1) floating panels join their main window's
  full-screen space (recommended); (2) they stay behind.
- **B5-iv, staying on top:** (1) above their own main window only, never
  above other applications' windows (recommended); (2) above everything when
  they also hide on deactivation.

The owner, verbatim:

> must be portable. lets follow the mac convention. dragging- drags with. full screen- joins main window. on top- only own main window.

Recorded as:

- **GIVEN B5 (portable).** Every rule below is one portable rule on every
  host, not macOS adapter behaviour.
- **GIVEN B5-i (the Mac convention, everywhere).** Floating tool panels hide
  while the application is inactive and return when it is activated again,
  on Windows, macOS and X11 alike. Recorded reading: this applies to
  floating tool panels (the palette convention); a document torn out into
  its own floating window is a document window and stays visible.
- **GIVEN B5-ii-3.** Floating panels move with their main window, keeping
  their offset, on every host (child-window behaviour, done by the framework
  on Windows and X11).
- **GIVEN B5-iii-1.** In macOS full screen, floating panels join their main
  window's full-screen space. Hosts without such spaces need nothing.
- **GIVEN B5-iv-1.** Floating panels stay above their own main window only,
  never above other applications' windows.

### Note, 2026-10-17: two confirmations

Asked: (1) that the hiding rule covers floating tool panels only, a
document torn out into its own window staying visible; (2) that a panel
moved with its main window stops at the edge of the screen it is on rather
than going partly off-screen (offered as the default unless the owner
objected). The owner, verbatim:

> ok. next questions.

- **GIVEN:** torn-out documents stay visible when the application is
  inactive; only floating tool panels hide.
- **GIVEN:** a floating panel carried by its main window is clamped to the
  work area of the screen it is on.
