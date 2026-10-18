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

### Note, 2026-10-17: the postage stamp corrected

While a mockup was being drawn, the owner corrected the reading of D1,
verbatim:

> by th epostage stamp i do NOT mean the panel resizes, i mean it dissapears and an interpolated miniature is shown

- **GIVEN D1, corrected.** At the start of a drag the panel disappears from
  its place, and an interpolated miniature of it is shown instead: an
  image of the panel animated from the panel's own rectangle down to the
  small stamp that follows the pointer. The panel itself is never
  resized. This replaces the D1 entry's "Recorded reading" above where
  the two differ. Open, for the mockup: whether the vacated place closes
  up during the drag or stays as an empty slot until the drop, and how the
  miniature grows back into the target at the drop (and into its old place
  on Escape).

### Note, 2026-10-17: the mockup's three questions

A mockup showed the corrected stamp (the panel leaving a dashed empty
slot, a miniature animated from the panel's rectangle to a 100 px stamp
in 220 ms, growing into the target at the drop or back into its place on
Escape) and position-sensitive targets with a highlighted result area.
Asked: (1) whether the vacated place stays empty until the drop or closes
up at once; (2) the stamp size and animation length; (3) whether the
hover targeting matched what the owner had seen. The owner, verbatim:

> close up. makes sense. marginally, but what i saw had visible indicators that appear over that show with blue on black the shape essentially- if it was on top, on side, etc.

- **GIVEN:** the vacated place closes up as soon as the drag starts; the
  neighbouring panes take its space (one layout transaction).
- **GIVEN:** the mockup's stamp (about 100 logical units wide) and its
  animation (about 220 ms) make sense as starting values; the motion
  belongs to the engine (T2) when it exists.
- **GIVEN D2, refined.** The targeting matched only marginally. What the
  owner saw shows visible indicators over the pane under the pointer that
  depict, in blue on black, the shape the result would take: on top, on a
  side, and so on. Recorded reading, to be confirmed by a second mockup:
  position still selects the target, and indicator tiles appear over the
  hovered pane, each a dark tile with the resulting region drawn in blue.

### Note, 2026-10-17: the shape indicators

A third mockup closed up the vacated place and drew blue-on-black tiles
over the hovered pane, either as a cross of five (top, left, tab into,
right, bottom) or as one tile for the current result, with an optional
area highlight; window edges showed one tile at the edge's middle. Asked
which was closer to what the owner had seen. The owner, verbatim:

> essentially, ya, but the split controls only appeared when you brought the new control over the existing control- for each. so you could kinda see how it would work.

- **GIVEN D2, settled.** The split indicators appear only while the dragged
  panel is over an existing pane, and only for that pane; each pane shows
  its own as the panel comes over it, so the user sees how the drop would
  work there. Recorded reading: the set of split choices for that pane
  (the cross: top, left, tab into, right, bottom), each tile drawing the
  resulting shape in blue on black, the one the pointer's position selects
  lit. The area highlight stays as a working assumption until the taste
  mockups (T2, T3).

## Part 6. Auto-hide, 2026-10-17

Asked as E1 to E4, each with its recommendation marked (E1-1 with E1-2 as
a workspace setting, E2-1, E3-1, E4-1). The owner, verbatim:

> accepted. pin buttons can live on panels or groups. dragging a panel onto another panel naturally makes a group, and the group inherits a fresh non-autohide pin. so only root-level panels or groups get a pin.

- **GIVEN E1-1.** A side tab's flyout opens on hover after a delay and on
  click; click-only is a workspace setting.
- **GIVEN E2-1.** A click outside an open flyout closes it and passes
  through to what was clicked.
- **GIVEN E3-1.** The flyout is an in-window overlay, not a native window.
- **GIVEN E4-1.** The pin auto-hides the whole group.
- **GIVEN, pins.** Pin buttons live on root-level dock items only: a panel
  standing alone, or a group. Dropping a panel onto another panel (tab
  into) makes a group, and the new group gets a fresh pin in the
  not-auto-hidden state; the panels inside a group carry no pin of their
  own. Recorded reading: "root-level" means a top-level item of a dock
  site (a split's leaf), not a panel nested inside a group.

## Part 7. Focus, activation and command context, 2026-10-17

Asked as F1 to F5, each with its recommendation marked (F1-1, F2-1, F3-1,
F4-1, F5-1; F5-2 named as the Mac palette behaviour that fits B5). The
owner, verbatim:

> accepted.

- **GIVEN F1-1.** Two activation tracks: `active_panel` and
  `active_document`.
- **GIVEN F2-1.** A command routing chain (focused route, active panel,
  active document, main window) in the commands seam, used by menus, the
  ribbon, shortcuts and docking, so document commands work from a
  floating tool window.
- **GIVEN F3-1.** F6 cycles the regions of the key window only; other
  windows' panels are reached with the panel switcher (Ctrl+Tab) or
  Alt+F7.
- **GIVEN F4-1.** While a floating tool window is key, the main window shows
  `secondary_participating` (DDV-007-01).
- **GIVEN F5-1.** Clicking a floating tool window activates it (it becomes
  key); F2's routing keeps document commands working.

## Part 8. Keyboard, 2026-10-17

Asked as G1 to G7, each with its recommendation marked (G1-2, G2-1, G3-1
described over D2's split tiles, G4-1, G5-1, G6-1, G7-1). The owner,
verbatim:

> accepted.

- **GIVEN G1-2.** A `TabControl` with focus inside it keeps Ctrl+Tab; the
  workspace navigator gets it otherwise (most local wins).
- **GIVEN G2-1.** Visual Studio's keys are the defaults: Ctrl+F6 (next
  document), Alt+F7 (next tool panel), Alt+Hyphen and the context-menu key
  or Shift+F10 (the position menu), Escape in a tool panel (to the active
  document), Shift+Escape (hide the active tool panel).
- **GIVEN G3-1.** A keyboard move mode steps through the same targets as a
  pointer drag: the hovered pane's split tiles (D2) by arrows, the next
  pane, the window edges and Float by Tab, with the same preview and live
  announcements; Enter commits through the same transaction, Escape
  cancels with no change.
- **GIVEN G4-1.** The navigator shows two lists, tool panels and documents.
- **GIVEN G5-1.** Keyboard defaults per host (Cmd+W on macOS) are host
  keymap defaults, not core policy.
- **GIVEN G6-1.** Applications may rebind docking keys; they are commands.
- **GIVEN G7-1.** A "maximize the document area" command hides and restores
  all tool panels, default Ctrl+Shift+F12.

## Part 9. Documents, 2026-10-17

Asked as H1 to H4, each with its recommendation marked (H1-1 with H1-2 as
an application option; H2-1; H3-2, which also answers PageView P19 and
differs from the docking study's own H3 recommendation; H4-3, which also
answers PageView P11). The owner, verbatim:

> accepted.

- **GIVEN H1-1.** A cancellable, possibly asynchronous `closing` event per
  document; close-all asks in sequence and stops at the first cancel. An
  application may choose one combined prompt listing every dirty document
  instead (H1-2).
- **GIVEN H2-1.** The framework keeps most-recently-activated history (for
  the navigator and for activation after a close); back and forward inside
  documents is the application's.
- **GIVEN H3-2 (and P19).** Pinned tabs are a `PageStrip` capability an
  application switches on; no preview tab.
- **GIVEN H4-3 (and P11).** Document tabs shrink first, then overflow into
  a menu listing every document.

## Part 10. Persistence and restore, 2026-10-17

Asked as I1 to I7, each with its recommendation marked (I1-1 the runtime
layout store's syntax, not yet chosen; I2-1 automatic saving with explicit
save and load, as RB-12; I3-1; I4-1; I5-1; I6-1; I7-1). The owner,
verbatim:

> xml. such as sdrsharp uses. the program is responsible for saving its own setup, but that's only for configurables. for ribbon type layout- that's baked into the program, so its a settings permutation, not a layout. otherwise accepted.

- **GIVEN I1 (XML).** A saved docking layout is XML, as SDRSharp keeps its
  dock layout in a separate `Layout.xml` (RES §6). Recorded reading: this
  settles the docking layout's format; it does not choose the syntax of
  the general settings store (RES §7), which stays open.
- **GIVEN I2-2.** The program is responsible for saving its own setup: the
  framework writes and reads a layout when the program asks and never
  saves on its own. Only configurable things are saved; what the program
  bakes in is not.
- **GIVEN, the ribbon is not a layout.** A ribbon's structure is baked into
  the program; what a user changes on it (Quick Access Toolbar items and
  position, minimized) is a settings permutation, saved with the
  program's settings, not a layout. This revises RB-12's "through the
  layout store" (RB decision 44).
- **GIVEN I3-1.** Unknown panel IDs are kept as inactive placeholders and
  restored if the panel returns.
- **GIVEN I4-1.** A framework format version with built-in migrations, and
  an application layout version with an upgrade hook.
- **GIVEN I5-1.** Missing monitors: the same ID, else the most overlap,
  else the primary; clamped so the caption is reachable.
- **GIVEN I6-1.** Named layouts in the workspace API, in phase 7.
- **GIVEN I7-1.** Factories by panel ID; on restore only visible selected
  panels are created, others on first show.
