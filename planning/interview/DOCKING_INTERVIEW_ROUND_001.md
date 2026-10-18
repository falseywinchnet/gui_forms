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

## Part 2. Docking scope and the window layer, 2026-10-17

Asked as A1 to A4 and the remaining B questions (B1 to B4, B5a, B6, B7),
each with its recommendation marked: A1 a DK build stage for the window
layer (phase 0) and W for the docking controls; A2-1; A3-1; A4-1; B1-1;
B2-1; B3-1; B4-1; B5a-1; B6-1; B7-1. The owner, verbatim:

> accepted on these.

- **GIVEN A1.** The window layer underneath (study phase 0, host protocol
  work) is built in a DK build stage of its own; the docking controls are
  built in W, with DK as their planning ministage, as the ribbon (RB-1).
- **GIVEN A2-1.** A renderer-free `DockLayout` model, a `DockWorkspace`
  component that owns it, and controls that realize it.
- **GIVEN A3-1.** Tool panels and documents are built together from the
  start.
- **GIVEN A4-1.** DK writes a small n-ary weighted split solver for the
  dock site; stage L may adopt or replace it.
- **GIVEN B1-1.** Owned windows can be opened and destroyed during a run,
  bounded at 64 windows in all (today's `maximum_windows`) and 32 floating
  sites.
- **GIVEN B2-1.** Window frame get and set in screen logical units, the
  pointer's screen position and the window under a screen point, each a
  capability reporting `unsupported` where a host cannot.
- **GIVEN B3-1.** The image registry and the theme become application
  scope, shared by every window of a run.
- **GIVEN B4-1.** A workspace transaction opens update scopes on every
  affected window and orders window creation and destruction (study 5.3).
- **GIVEN B5a-1.** Floating windows are owned by the main window: above it,
  minimized with it, no taskbar button.
- **GIVEN B6-1.** On X11, tool and floating windows are
  `_NET_WM_WINDOW_TYPE_UTILITY` with skip-taskbar.
- **GIVEN B7-1.** A floating window is moved by the framework's own capture
  drag on its caption, setting the native frame per pointer move; phase 0
  measures smoothness and a host that cannot keep up falls back to the
  native move loop with progress reports (B7-2).

## Part 3. PageView shape, 2026-10-17

Asked as P1 to P6, each with its recommendation marked (P1-1, P2-1, P3-1,
P4-1 unless the owner meant Outlook 2013's bottom bar, P5-1, P6-1). The
owner, verbatim:

> accepted on these. for P4; i dont remember the context.

- **GIVEN P1-1.** One renderer-free `PageSet` and separate presenters:
  `PageStrip` (headers only), `PageView` (strip plus content, with presets)
  and `NavigationPane`. Docking's tab stacks and auto-hide strips are built
  from them and lose their own tab item systems; the docking study's
  section 4 is read with that change.
- **GIVEN P2-1.** `TabControl` keeps its Windows Forms surface; its layout
  and painting are later re-based on `PageStrip` and `paint_part`.
- **GIVEN P3-1.** Strip, document and Outlook-style presentations now; the
  navigation strip next; the stack (list bar) and explorer bar are not
  admitted until a consumer asks. Backstage stays the ribbon's (RB-26).
- **P4 deferred.** The phrase "navigation strips" came from the list of
  advanced controls the owner pasted on 2026-10-14 ("multiple tab
  presentation modes, including document tabs, navigation strips, and
  Outlook-style navigation"); the owner does not recall which control it
  meant. Since P3-1 builds the navigation strip after the first three
  presentations, P4 is asked again when that phase is planned, with
  mockups of both readings. Working assumption until then: P4-1, a left
  column of icon-and-label items that collapses to icons when narrow.
- **GIVEN P5-1.** A page may start without content and receive it through
  `content_requested` on first selection (docking's lazy creation).
- **GIVEN P6-1.** `PageStrip` is a public control of its own.

## Part 4. Layout model and restrictions, 2026-10-17

Asked as C1 to C6, each with its recommendation marked (C1-1 n-ary
weighted splits, C2-1, C3-2, C4-1, C5-1, C6-1). The owner, verbatim:

> two per split. otherwise accepted.

- **GIVEN C1-2.** Split nodes are binary: two children per split, nested to
  make more (DockPanel Suite's form). This narrows A4: the split solver DK
  writes divides one span between two children by a ratio, within both
  children's minimums, applied recursively.
- **GIVEN C2-1.** One document area per main window, always present even
  when empty; documents may also float.
- **GIVEN C3-2.** A tool panel and documents share a stack only when the
  panel's descriptor allows "dock as document".
- **GIVEN C4-1.** "Enabled control groups" are dock groups: a panel joins a
  stack or site only when its group admits it, with a default group so
  applications that ignore groups are unaffected.
- **GIVEN C5-1.** Hidden panels keep placeholders for their last docked,
  floating and auto-hide placements.
- **GIVEN C6-1.** When the window shrinks below the panels' minimums, tool
  panels collapse to auto-hide by declared priority, then the window
  minimum grows (DDV-007-02).

## Part 5. Dragging and drop targets, 2026-10-17

Asked as D1 to D7, each with its recommendation marked (D1-1 preview
first, then D1-3; D1b-1; D2-1 guide buttons; D2a-1; D3-1; D4-1; D5-1;
D6-1; D7-1). The owner, verbatim:

> something more mature than either. it tears off, and is then shrunk into a postage stamp version of itself so its easier to handle.  for d2: ive seen some show something more mature than guide buttons, depending on *where* you hover. otherwise- all accepted.

- **GIVEN D1 (new option, the postage stamp).** A dragged panel tears off
  at once and is shrunk into a small "postage stamp" version of itself
  that follows the pointer, so it is easy to handle; nothing in the layout
  moves until the drop. Recorded reading: the stamp is a scaled image of
  the panel (caption and content) drawn as an overlay; it is the drag
  image, not a live floating window, so it needs no runtime window to
  start (phase 0 only for dropping outside the window). Its size, its
  look and how it grows back at the drop are settled by mockup.
- **GIVEN D1b-1.** Escape puts the panel back where it was.
- **GIVEN D2 (new option, position-sensitive targets).** Instead of guide
  buttons, the target follows *where* the pointer hovers: each pane is
  divided into zones (its edge bands split it on that side; its centre
  tabs the panel into it), and the area the panel would occupy is
  highlighted as the pointer moves. Recorded reading, to be confirmed by
  mockup: the kind of targeting used by Visual Studio Code's editor
  groups and similar editors; the zone proportions and whether a small
  cue marks the zones are settled by mockup.
- **GIVEN D2a-1.** No dropping straight into auto-hide; auto-hide is chosen
  with the pin or the menu.
- **GIVEN D3-1.** Global targets along the main window's outer edges are
  always offered during a drag (with D2, as edge zones of the window).
- **GIVEN D4-1.** A drop where nothing is accepted floats the panel if it
  may float, else snaps it back, with a rejected cue under the pointer.
- **GIVEN D5-1.** Drag threshold 4 logical units, overridable by a host
  metric.
- **GIVEN D6-1.** Ctrl held suppresses docking during a drag.
- **GIVEN D7-1.** A caption drag moves the whole stack; a tab drag moves
  one panel.
