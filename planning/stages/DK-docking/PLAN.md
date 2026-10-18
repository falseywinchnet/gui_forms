# Docking and PageView: plan

Date: 2026-10-17. Status: **accepted by the owner on 2026-10-17** ("yes.
lets start building and work."), closing stage DK's gate. Sources: decisions 1 to 43 in `README.md`
(from interview round 001, `planning/interview/DOCKING_INTERVIEW_ROUND_001.md`),
the studies `DOCKING_STUDY.md` and `PAGEVIEW_STUDY.md`, and the look studies
(`docking_look_studies_source.html`). Where this plan and a recorded decision
disagree, the decision governs; fix the plan.

Where it is built (decision 2): the window layer underneath is a DK build
stage of its own (phase W0 below); the PageView family and the docking
controls are built in stage W, as the ribbon is. Every part is painted
through `paint_part` under the atom vocabulary ruling (design language
§12.7); the look recorded in decisions 41 and 42 is the T4 theme's to draw,
and the motion (stamp, tile fade, flyout slide) is T2's. Until T4 and T2 the
controls draw from today's recipes and animate with plain interpolation, as
every W control does.

## What the framework is

| Type | Kind | What it holds |
|---|---|---|
| `PageSet` | renderer-free model | Pages with stable `PageId`s, order (pinned group first), selection, activation history, batched edits, take and put of a page with its content (decisions 13, 16, 37). |
| `PageStrip` | control; item system `page` | The headers only, top, bottom, left or right: content-sized items, scroll and list overflow (strip) or shrink then list (documents), close and pin glyphs, live reorder; public (decision 17). |
| `PageView` | control | A `PageStrip` plus content, with strip, document and (later) navigation presets; late content through `content_requested` (decisions 13 to 16). |
| `NavigationPane` | control; item systems `nav_page`, `nav_overflow` | The Outlook-style pane: header, content, grip, large buttons, overflow icons, configure menu, collapsed bar with a popup (decision 39). |
| `DockLayout` | renderer-free model | Binary splits with ratios, tool stacks, one document area per main window, floating sites, auto-hide edges, placeholders, dock groups and restrictions (decisions 3, 18 to 23). |
| `DockWorkspace` | component | Owns the layout, panel descriptors and factories, the transaction, activation tracks, the command context, save and restore on request (decisions 3, 29, 31 to 34). |
| `DockSite` | control | Realizes one window's part of the layout: stacks built from `PageView`, graphite seams with grips, the drag layer (stamp, quiet tiles, tint). |
| `DockNavigator` | control | The two-list Ctrl+Tab switcher (decision 30). |
| floating windows | owned host windows | One drawn caption for a single stack, the host's title bar for more (decision 41); the Mac convention on every host (decision 1). |

## Phases

Each phase ends with tests in `tests/` that CI runs on every platform, and
its own pull request (or stack). W0 and P1 to P4 can run in parallel: the
models are renderer-free and PageView needs nothing new from the hosts.

### Window layer (DK build stage)

**W0. Windowing foundation, on every host and the headless host.**
- Opening and destroying owned windows during a run, at most 64 windows and
  32 floating sites (decision 6).
- Window frame get and set in screen logical units with a frame-changed
  event; the pointer's screen position; the window under a screen point;
  each a capability reporting `unsupported` where a host cannot (decision
  7). XWayland's handling of positions is measured, not assumed.
- Floating-window policy, one rule everywhere (decision 1): tool windows
  hide while the application is inactive (documents do not); they move with
  their main window keeping their offset, clamped to the screen's work
  area; they join the main window's macOS full-screen space; they stay
  above their own main window only. Owned, no taskbar button, minimized
  with the main window (decision 10); X11 utility type with skip-taskbar
  (decision 11).
- Application-scope image registry and theme, shared by every window
  (decision 8). This also removes today's refusal of another window's
  `ImageList` (`tests/basic_controls_tests.cpp:1694-1705`).
- A workspace transaction that opens update scopes on several windows and
  orders creation (hidden) before destruction (decision 9).
- The framework's own capture drag moving a native frame per pointer move,
  with smoothness measured per host and a fallback to the native move loop
  where a host cannot keep up (decision 12).

Tests: headless host traces for window creation, frame requests, ordering
and the hide, follow and clamp rules; per-host capability records; a
control subtree with images moved between two windows keeps its images; one
paint per window for a two-window transaction. Measurements recorded under
`results/`.

### Shared seams (W)

**V0. Vocabulary and the command chain.** Admit together, in the design
language, the enums and the schema (decisions 35, 40): the `pin` mark; one
dock-shape mark with the region as a parameter; the custom item states
`modified` and `pinned`; a text orientation in `PartPaint`; the semantic
roles `pane` and `tab_panel`, the action `close`, a tab-to-panel relation
and a tab-list orientation field, each mapped on MSAA, AX and ATK with stage
A; tabs stop publishing `press`. Add the command routing chain to the
commands seam (focused route, active panel, active document, main window;
decision 29), used by menus, the ribbon, shortcuts and docking. Tests: anatomy
and schema validation of the new atoms; semantic snapshots; Ctrl+S from a
focused tool control reaches the active document's command.

### PageView (W)

**P1. `PageSet`.** Records, `PageId`, the pinned group, cancellable
`selecting` then `selected_page_changed`, history, batches, take and put.
Tests: invariants after random edit sequences; refusals leave the set
unchanged; a batch publishes one change; selection after a close (next,
else previous, for strips; most recent for documents).

**P2. `PageStrip`, top and bottom.** Content-sized items with minimum and
maximum, fixed and fill; scroll and list buttons for strips; arrows with
automatic selection (manual per view), Home, End, Ctrl+PageDown and
Ctrl+PageUp; mnemonics except in documents; the wheel scrolls; semantics
without `press`. Tests: geometry tables at several widths; the selected tab
always visible; keyboard traces; no allocation per tab during paint.

**P3. `PageView`, strip preset.** Content, focus memory, late content,
Ctrl+Tab policy (kept by a focused view, passed to the navigator inside
docking), F6 registration; a Gallery specimen beside `TabControl`. Tests:
event order; cancel keeps everything; focus restored on return.

**P4. The document preset.** Close glyph on selected and hot tabs with
reserved space and frozen widths during successive closes; middle-click close
on release; close requests decided by the owner (`PageView` alone removes,
docking decides); `modified` as a dot in the close slot; opt-in pinned tabs,
icon-only and expanding on hover, the pin replacing close; shrink then the
count-and-arrow list, a page chosen from it taking the last visible position;
Ctrl+F4 (Cmd+W) closes; Ctrl+Shift+PageDown and PageUp move a tab within its
group. Tests: one close event per cause; cancel leaves the set unchanged;
the pinned boundary holds in every move path; shrink to the minimum, then
overflow; the overflow choice keeps the front order.

**P5. Side strips.** Left and right placement with rotated labels reading
bottom to top (decision 42). Tests: geometry per edge; hit-testing on rotated
labels.

**P6. Drag hooks.** Live-sliding reorder; `page_drag_started` that docking
may claim; insertion position at a point; context menu and double-click
events. Tests: a fake docking session claims drags; claimed drags never
reorder; Escape restores the order.

**P7. `NavigationPane`.** Rows in Plan Paint's tab material, the grip, the
overflow strip, the configure menu with `options`, the collapsed bar with a
vertical caption and its popup (an outside click closes and passes
through); its state exposed for the program to save. Tests: grip steps;
hidden pages only in the menu; popup open and close order; state round trip.

**P8. The navigation preset and `TabControl`.** The navigation preset, once
its meaning is settled (P4 is asked again then); `TabControl` re-based on
`PageStrip` and `paint_part`, keeping its Windows Forms surface. Tests:
`TabControl`'s existing tests unchanged; its anatomy names kept.

### Docking (W)

**D1. The layout model.** `DockLayout` with binary splits, panel descriptors
(tool or document, "dock as document", dock group, allowed placements,
collapse priority), restrictions in three layers, primitive edits,
validation, pruning, placeholders, transactions on the model. The XML layout
form (decision 32) with a framework format version and migrations, an
application layout version with an upgrade hook, unknown IDs kept as
placeholders, monitor fallback with clamping (decision 34). Tests: tree
invariants after random edit sequences; restrictions refuse with reasons and
leave the model unchanged; a save and restore round trip is byte-stable;
restore fixtures (a missing monitor, a removed plugin's panel, an older and a
newer format version, a corrupt file giving the default layout and a
diagnostic).

**D2. Docked layout in one window.** `DockSite` with binary seams and
minimums (graphite seam, grip), tool stacks and the document area built from
`PageView`, captions with the identity key, the pale tinted active caption
and Plan Paint tool tabs below the stack, pins on root-level items only,
show, hide and close, the position menu (Float disabled until D4), F6
regions per stack, the `DockNavigator`, Escape and Shift+Escape, activation
tracks, Ctrl+Shift+F12, shrinking by declared priority, lazy creation by
factory. Tests: part geometry; seam keyboard resizing; minimums at small
sizes; activation tracks; navigator order; semantic snapshots; factories
called only for visible selected panels.

**D3. Dragging in one window.** The postage stamp (the panel leaves, its
place closes up in one transaction, the miniature animates to about 100
units in about 220 ms and follows the pointer, grows into the target or back
on Escape); position-sensitive targets with quiet tiles over the hovered
pane only, the lit tile, the translucent tint, global edge targets; Ctrl
suppresses docking; invalid drops snap back until D4 adds floating; the
keyboard move mode over the same targets. Tests: state-machine traces for
threshold, Escape, capture loss and invalid drop; target tables for pointer
positions over a fixture layout; Escape leaves the saved layout identical;
one transaction and one layout pass per drop; the keyboard reaches every
target the pointer reaches.

**D4. Floating windows.** Tear-off by dropping outside, the Float command,
redock by drag and double-click, drags across windows with tiles in the
window under the pointer and only the hovered pane lit (decision 41), the
drawn one-stack caption or the host bar for more, command context from
floating windows, monitors and scale changes, the float limit. Tests:
multi-window headless traces; a drag from a floating window into the main
window shows the panel before the floating window closes; Ctrl+S from a
floating tool window reaches the active document; restored floating windows
land inside a work area.

**D5. Auto-hide.** Side strips built from `PageStrip` (P5), flyouts as
in-window overlays sliding over about 180 ms with the overlay shadow, hover
after a delay or click (click-only as a setting), an outside click closing
and passing through, the pin auto-hiding the whole group. Tests: open and
close order for hover, click, focus loss and Escape; focus restored after
Escape; a flyout never changes the layout tree.

**D6. Documents.** The cancellable, possibly asynchronous `closing` per
document; close-all and close-others asking in sequence (a combined prompt
as an application option); activation history after a close. Tests: closing
order and cancellation; activation after close; no layout change when a
prompt is cancelled.

**D7. Persistence integration and named layouts.** Save and load on the
program's request through the XML form (no automatic saving, decision 33);
named layouts; restore on live windows. `ToolStripContainer` strip dragging
moves onto the docking drag session (decision 36). Tests: a full restore
creates only visible panels; a plugin registered after restore appears at
its placeholder; toolbar drags use the same targets.

**D8. Integration and evidence.** A Gallery docking page; host
accessibility verification of a docking fixture with stage A; the look from
T4 (decisions 41, 42) and its motion from T2; reference images. Building
stage D's and V's shells on docking is the consumer test. File Manager keeps
its fixed splits and shares the parts (decision 36).

## Order and dependencies

- W0, V0 and P1 to P4 first, in parallel. D1 can start with them.
- P2 before D5 (auto-hide strips); P4 before D6 (documents); P6 before D3
  (dragging); W0 before D4 (floating).
- Stage A for the new role, action and field mappings (V0, D8).
- T4 and T2 for the look and motion (D8, and the look of every phase).
- Stage L may adopt or replace the binary split solver (decision 5).

## Open items

- **The XML reader and writer** (decided, decision 43): first-party, for
  the layout's own element set, UTF-8 only, no DTDs or external entities,
  bounded depth and size, with a fuzz corpus; built in D1.
- **P4, what "navigation strip" means,** is asked again before P8, with
  mockups of both readings.

## Gate

Each phase's tests pass on every CI platform; W0's per-host measurements
(including X11 under XWayland) are recorded under `results/`; the owner
accepts the Gallery specimens of `PageView` and `NavigationPane` and the
Gallery docking page.
