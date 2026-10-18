# Stage DK: docking and windowing (study, interview, decisions, plan)

Release: not yet placed on the public roadmap; the plan this stage produces
proposes where. Depends on: nothing to start. Status 2026-10-17: studies written;
interview round 001 in progress. Authority: this brief, until the owner's decisions are recorded.

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
   main window only, never above other applications' windows.

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
