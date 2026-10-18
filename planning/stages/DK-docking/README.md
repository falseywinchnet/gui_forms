# Stage DK: docking and windowing (study, interview, decisions, plan)

Release: not yet placed on the public roadmap; the plan this stage produces
proposes where. Depends on: nothing to start. Status 2026-10-17: complete; the plan
(`PLAN.md`) is accepted and building has started (W0, V0, P1, D1). Authority: this brief, until the owner's decisions are recorded.

Read [the shared brief](../README.md) first.

## Why

The owner, 2026-10-14, verbatim:

> https://github.com/dockpanelsuite/dockpanelsuite telerik rad dock, etc.gimp docking,  i will want to have dockable controls and visible hints so that docking can proceed dynamically for enabled control groups, as many programs use.  we will need to interview on what that will entail to have our own in house responsive docking system programs can use. ocking is one part of a larger windowing system. You'd want the underlying framework to make sizing, focus, input, painting, and state behave consistently—then the fancy panels can sit on top.
>
> Tool windows and document tabs as distinct concepts. Documents tend to need activation, dirty state, close prompts, and navigation history; tool windows usually need visibility and placement.
> Docking, floating, tabbing, splitting, and auto-hide, with restrictions on where individual panels may go.
> Drag previews and drop targets, plus sensible behavior when the user cancels a drag or drops somewhere invalid.(nested as well as global splitting depending on where a dock is dragged to)
> Focus and activation across floating windows. Clicking a floating tool window should preserve the application's command context where appropriate.
> Stable panel identifiers and lazy creation. A saved workspace should restore a panel by identity without requiring every panel to exist at startup.(mostly on the client apps)
> Layout transactions. A move that changes several containers should produce one coherent layout update, with no intermediate flicker or event chaos<we've managed most of this)
> Resilient restore. Missing monitors, renamed panels, removed plugins, and changed framework versions should produce a usable workspace(this is mostly on the client apps to facilitate)
> Keyboard alternatives to dragging. Users should be able to move, dock, activate, and close panels without a mouse.

**Admission.** `planning/CONTROL_COMPLETENESS_MATRIX.md` row 43 listed a
"GUI.Forms-native docking family" as an unadmitted candidate requiring "a
separate parent-product admission record before implementation". The
owner's message above is that admission (2026-10-14): GUI.Forms gets its own
in-house docking system that programs can use. Third-party docking libraries
are study material, not dependencies or facades. ADR-013's reservation of
persistent side panels to File Manager was already retired on 2026-10-13.

The owner's framing sets the order of work: docking is one part of a larger
windowing system, so the framework must first make sizing, focus, input,
painting and state behave consistently across windows; the panels sit on
top. Several requirements are marked by the owner as mostly the client
application's (lazy creation, resilient restore); the framework facilitates
them.

## Sequence

1. **Study** (written 2026-10-14: `DOCKING_STUDY.md`, with `PAGEVIEW_STUDY.md`). The canonical docking systems
   (DockPanel Suite, Telerik RadDock, DevExpress, Infragistics, AvalonDock,
   the Visual Studio shell, Qt Advanced Docking System, GIMP's dockables,
   JetBrains tool windows, Eclipse part stacks); the windowing foundation
   underneath and what GUI.Forms has per host; each of the owner's
   requirements; a proposed layout model and anatomy with every vocabulary
   gap marked; the hard algorithms (drag state machine, drop targets with
   nested and global splitting, layout transactions, floating windows,
   auto-hide, focus and command context across windows, the keyboard model,
   persistence and resilient restore); a phased plan; interview questions.
2. **Interview.** Recorded verbatim in
   `planning/interview/DOCKING_INTERVIEW_ROUND_001.md`, as the ribbon round
   was; mockups where they answer better than words.
3. **Decisions**, numbered in this brief, including vocabulary additions.
4. **Plan**, accepted by the owner before anything is built.

## PageView inside this stage

Added 2026-10-14 by the owner: a `PageView` family (the equivalent of
Telerik's RadPageView) with several tab presentation modes, including
document tabs, navigation strips and Outlook-style navigation, "a useful
foundation for several other controls" (matrix row 51). Docking's tab
stacks, document area and auto-hide strips reuse it rather than growing
their own tab control, so it is studied here and its questions form the
tab section of this stage's interview. Its study is
`PAGEVIEW_STUDY.md` beside the docking study.

## Decisions

From interview round 001 (`planning/interview/DOCKING_INTERVIEW_ROUND_001.md`):

1. **Floating panels follow the Mac convention, portably (B5).** One rule
   on every host: floating tool panels hide while the application is
   inactive (documents torn out into floating windows stay visible); they
   move with their main window, keeping their offset; in macOS full
   screen they join the main window's space; they stay above their own
   main window only, never above other applications' windows. A panel
   carried by its main window is clamped to its screen's work area.
2. **Where it is built (A1).** The window layer underneath is a DK build
   stage of its own; the docking controls are built in W, as the ribbon.
3. **A separate layout model (A2-1):** renderer-free `DockLayout`, a
   `DockWorkspace` component, and controls that realize it.
4. **Tool panels and documents together from the start (A3-1).**
5. **DK's own small split solver (A4-1),** which stage L may adopt or
   replace; binary after decision 18.
6. **Runtime windows (B1-1):** opened and destroyed during a run, at most
   64 windows and 32 floating sites.
7. **Screen coordinates (B2-1):** window frames, the screen pointer and the
   window under a point, each capability-reported.
8. **Application-scope images and theme (B3-1).**
9. **Cross-window transactions (B4-1),** with ordered window creation and
   destruction.
10. **Floating windows owned by the main window (B5a-1):** no taskbar
    button, minimized with it.
11. **X11 utility windows with skip-taskbar (B6-1).**
12. **The framework's own capture drag moves floating windows (B7-1),**
    falling back to the native move loop where a host cannot keep up.
13. **The PageView family (P1-1):** one `PageSet` with `PageStrip`,
    `PageView` and `NavigationPane`; docking's stacks and auto-hide strips
    are built from them.
14. **`TabControl` keeps its surface (P2-1)** and is later re-based on
    `PageStrip` and `paint_part`.
15. **Presentations (P3-1):** strip, document and Outlook-style now; the
    navigation strip next (what it means, P4, is asked again then);
    stack and explorer bar only when a consumer asks.
16. **Late page content (P5-1)** through `content_requested`.
17. **`PageStrip` is public (P6-1).**
18. **Binary splits (C1-2):** two children per split, nested; one ratio per
    split within both children's minimums.
19. **One document area per main window (C2-1),** always present;
    documents may float.
20. **Mixed stacks only by "dock as document" (C3-2).**
21. **Dock groups (C4-1),** with a default group.
22. **Placeholders remember hidden panels' places (C5-1).**
23. **Shrinking collapses tool panels by declared priority (C6-1),** then
    the window minimum grows.
24. **The postage-stamp drag (D1, as corrected):** at the start of a drag
    the panel disappears from its place and an interpolated miniature of
    it, animated from the panel's rectangle down to a small stamp, follows
    the pointer; the panel itself is never resized; Escape puts it back
    (D1b-1). The vacated place closes up as soon as the drag starts; about
    100 units and 220 ms are the starting stamp size and animation.
25. **Position-sensitive targets (D2):** the target depends on where the
    pointer hovers within a pane (edge bands split, centre tabs into),
    with the resulting area highlighted; a cross of indicator tiles appears
    only over the pane the dragged panel is over, each tile depicting the
    resulting shape in blue on black, the selected one lit; the
    window's outer edges are always global targets (D3-1). No drop
    straight to auto-hide (D2a-1).
27. **Auto-hide (E1-1, E2-1, E3-1, E4-1):** flyouts open on hover after a
    delay and on click (click-only as a setting); an outside click closes
    the flyout and passes through; the flyout is an in-window overlay; the
    pin auto-hides the whole group.
28. **Pins on root-level items only:** a lone panel or a group carries the
    pin; tabbing a panel into another makes a group with a fresh,
    not-auto-hidden pin; panels inside a group have none.
29. **Focus and command context (F1-1 to F5-1):** separate active panel
    and active document; a command routing chain in the commands seam
    (focused route, active panel, active document, main window); F6 per
    window; the main window shows `secondary_participating` while a
    floating tool window is key; clicking a floating tool window makes it
    key.
30. **Keyboard (G1-2 to G7-1):** Ctrl+Tab to a focused `TabControl`, else
    the two-list navigator; Visual Studio's docking keys as defaults; a
    keyboard move mode over the same targets as the pointer; host keymap
    defaults; rebindable commands; Ctrl+Shift+F12 maximizes the document
    area.
31. **Documents (H1-1 to H4-3):** a cancellable, possibly asynchronous
    `closing` per document, sequential close-all (a combined prompt as an
    application option); activation history kept by the framework;
    pinned tabs as an opt-in `PageStrip` capability, no preview tab (also
    P19); tabs shrink, then overflow to a menu (also P11).
32. **Layouts are XML (I1),** as SDRSharp's `Layout.xml`; the general
    settings store's syntax stays open.
33. **The program saves (I2-2):** the framework writes and reads a layout
    on request and never saves on its own; only configurable things are
    saved.
34. **Restore (I3-1 to I7-1):** unknown IDs kept as placeholders; format
    and application versions with migrations and an upgrade hook; monitor
    fallback with clamping; named layouts in phase 7; lazy creation by
    factories.
35. **Vocabulary (J1 to J8):** a pin mark; one dock-shape mark with the
    region as a parameter; a custom item state `modified`; a
    text-orientation parameter in `PartPaint`; the semantic role `pane`
    and action `close` (tabs stop publishing `press`); float, dock and
    maximize marks decided with T6; no new part roles. Admitted to the
    design language with the enum and schema when built (§12.7).
36. **Fit (K1-1, K2-1):** File Manager keeps its fixed splits; toolbar
    dragging joins the docking drag session after phase 3.
37. **PageView selection and layout (P7 to P15):** cancellable selection
    by `PageId`; activation decided outside; automatic arrow selection,
    manual per view; next-else-previous (strip) or most recent (document)
    after a close; strip overflow by scroll and list buttons;
    content-sized items; a page chosen from the overflow list takes the
    last visible position, keeping the user's order at the front;
    multiple rows later; the wheel scrolls.
38. **PageView document features and keys (P16 to P26):** close buttons on
    selected and hot tabs with frozen widths; middle-click close on
    release; close requests decided by the owner; no "+" button;
    Ctrl+PageDown/PageUp in every view, Ctrl+Tab per view policy;
    Ctrl+Shift+PageDown/PageUp moves a tab; mnemonics except in documents;
    Ctrl+F4 closes, Delete does not.
39. **PageView dragging and the navigation pane (P27 to P34):** reorder per
    view (on for documents); live sliding; tear-off only through docking;
    the Outlook-style pane with a configure menu and `options` event, a
    collapsed bar with a vertical caption and popup, tab-list semantics.
40. **PageView vocabulary (P35):** a `tab_panel` role, a tab-to-panel
    relation and a tab-list orientation field (with stage A); tree roles
    deferred; a custom item state `pinned`; tabs paint as `menu_item`
    until T4; no new marks for the navigation pane.
41. **The look (T1 to T8):** the VS 2005 period in house colours; quiet
    tiles; a translucent tint for the area highlight; a pale tinted
    active caption with an accent line and Plan Paint tool tabs below the
    stack; a sliding flyout with the overlay shadow; a drawn caption for
    a one-stack floating window, the host's title bar for more; graphite
    seams with a grip; drag proximity lights only the hovered pane.
42. **The PageView look (PT1 to PT8):** strip tabs in Plan Paint's stack,
    document tabs notched (slanted, VS 2005); an open
    seam; a plated close glyph, the pin replacing close on pinned tabs; a
    dot for `modified`; pinned tabs icon-only, expanding on hover; a count
    on the overflow button; navigation-pane rows in Plan Paint's tab
    material; side tabs read bottom to top. Both readings confirmed.
43. **A first-party XML reader and writer** for layouts: the layout's own
    element set, UTF-8 only, no DTDs or external entities, bounded, fuzzed.
26. **Invalid drops float or snap back (D4-1);** threshold 4 units
    (D5-1); Ctrl suppresses docking (D6-1); caption drags move the stack,
    tab drags one panel (D7-1).

## The look studies

The taste questions (T1 to T8, PT1 to PT8) are answered on a published
look-and-feel page, https://claude.ai/artifact/YYGbZoWDipuGFwHxeU8Dqk
(private to the owner); its source is copied in
`docking_look_studies_source.html`.

## What exists today

- Multi-window applications (`ApplicationWindow` with owner IDs and tool
  windows; `ApplicationWindowHandle`, including `request_client_size`), host
  services (monitors, drag and drop, capture, cursors, owned popups).
- Focus scopes; F6 focus regions (ribbon phase 0); update scopes and bounded
  layout flushes.
- `SplitContainer`, `TabControl`, `ToolStripContainer` and `ToolStripPanel`,
  `AnchoredPopupLayer`; the File Manager demoboard's panels.
- The by-role paint path and the atom vocabulary (design language §12.7).

## Gate

The interview round is recorded, the decisions are in this brief, and the
owner accepts the plan.
