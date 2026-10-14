# Stage RB: ribbon framework (study, interview, decisions, plan)

Release: not yet placed on the public roadmap; the plan this stage produces
proposes where. Depends on: nothing to start; the controls it plans build on
W's strip and gallery work and on the by-role paint path. Status 2026-10-14:
study delivered (`RIBBON_STUDY.md`); interview round 001 part 1 answered
(decisions 1 to 10 below) and part 2 except keyboard (decisions 11 to 15;
C1 to C4 asked again after an explanation of KeyTips); part 3 pending. The controls are built in
stage W (decision 1). Authority: this brief and
`planning/interview/RIBBON_INTERVIEW_ROUND_001.md`.

Read [the shared brief](../README.md) first.

## Why

The owner, 2026-10-14: "we will want to do our own interview and
decision/planning ministage then, to perfectly elaborate the canon controls
that make a ribbon framework with all controls and structure and behavior
needed."

A ribbon is not one control. It is a framework of controls and behaviour
(tabs and contextual tab sets, groups that rescale as the window narrows,
galleries, an application menu or Backstage view, a Quick Access Toolbar,
KeyTips, a minimized mode) that Windows Forms never had. The canonical
ribbons are Microsoft's: Office's Fluent ribbon, the Windows Ribbon Framework
that Paint and WordPad use, MFC's `CMFCRibbonBar` and the WPF Ribbon. The
owner also named Fluent.Ribbon (https://github.com/fluentribbon/fluent.ribbon),
an open-source WPF implementation: "one exists for wpf, but not for winforms,
and it is also not in our canon style. nevertheless, it is another
meaningful study component."

The owner's own ribbons (Plan Paint's, the design language's command plane,
tab rail and ribbon-button bulge) are one realization to learn from, not the
definition: the study starts from the canonical ribbons and decomposes them
into Windows Forms-style pieces.

## Sequence

1. **Study** (delivered 2026-10-14: `RIBBON_STUDY.md` in this folder). What a ribbon is in each canonical
   implementation; the ribbon as Windows Forms pieces (what maps onto tab
   strips, `ToolStrip` items, drop-downs, list views, and what no Forms piece
   covers); what GUI.Forms has; a proposed anatomy in vocabulary terms with
   every gap marked; the hard algorithms (adaptive group scaling, galleries,
   KeyTips, contextual tabs, the Quick Access Toolbar, the minimized ribbon,
   focus and accessibility); the command and data model; a phased build plan;
   and the open questions written as interview questions. Sources: Office,
   the Windows Ribbon Framework, MFC, WPF, Fluent.Ribbon, Plan Paint
   (`C:\Users\Shadow\plan-paint`, `src/forms/ribbon.cpp`), the design
   language, the interview ledger and the owner's visual verdicts.
2. **Interview.** The owner answers the study's questions, grouped by topic,
   as short numbered options with a recommendation each, plus the questions
   that need taste rather than a technical answer (look, feel, which
   canonical ribbon to follow where they differ). Recorded verbatim in
   `planning/interview/RIBBON_INTERVIEW_ROUND_001.md`, in the style of the
   accessibility and text round: answers by ID, never rewritten, amended by
   dated notes.
3. **Decisions.** The answers become numbered owner decisions in this brief,
   including any vocabulary additions (part roles, states, marks, semantic
   roles) the ribbon needs; vocabulary changes also go to the design
   language (§8, §12) and `anatomy_types.hpp` as the ruling of 2026-10-14
   requires.
4. **Plan.** The controls, structure and behaviour of the framework, each with
   its scope, dependencies and the tests that prove it; where it sits among
   the stages (W controls, FD form documents, D interface builder, T2/T4
   theming, A accessibility); and its gate. Nothing is built before the plan
   is accepted.

## What exists today

- Commands: `Command` with text, description, `icon_id`, shortcut, mnemonic,
  `key_tip`, enabled, visible and checked state
  (`include/gui_forms/commands/`).
- Strips and menus: `ToolStrip` and `StatusStrip` (stage W, 2026-10-14),
  `MenuStrip`, `ContextMenu`, `CommandBar`, `DropDownButton`,
  `CommandOverflowPanel` (priority-ordered collapse of command groups,
  `experiments/M12P35_COMMAND_OVERFLOW_PROJECTION.md`).
- Tabs: `TabControl`. Popups: `AnchoredPopupLayer`. Images: `ImageList`.
- The design language's ribbon looks (§3.5 bulge and dithered bulge, the
  tab rail, the command plane, §12.4 to §12.6 decisions), the T4 rendering
  reference for the ribbon button, and Plan Paint's ribbon.

## Owner decisions

From interview round 001, part 1 (2026-10-14); the record is
`planning/interview/RIBBON_INTERVIEW_ROUND_001.md`, the questions are
`RIBBON_STUDY.md` §8.

1. **Stage W builds the ribbon (A1-2)**, beside `ToolStrip`. RB stays the
   planning ministage.
2. **Commands are records (A2-1)** in a command item system, as `ToolStrip`'s
   items; hosted editors and galleries are child controls.
3. **The File tab opens an application menu now and Backstage later (A3-4).**
4. **Scaling is studied across several ribbons (A4-3, the owner's own
   option)**: the reference for group scaling is a comparative study of the
   canonical ribbons, not one fixture. Whether it may observe ribbons running
   on the owner's machine is question A4a.
5. **WPF/MFC scaling model (B1-2)**: per-group size definitions plus a tab
   reduction order, with a deterministic default when the order runs out.
6. **Labels are never dropped (B2-2)**; groups shrink by image size, rows and
   popup groups only.
7. **The ribbon hides by a content-derived rule (B3-2)**, not fixed sizes.
8. **Galleries may give up one column at a time (B4-1).**
9. **No simplified one-line ribbon (B5-1).**
10. **Follow Office 2010 and earlier (T1)** where the canonical ribbons
    differ.

From part 2 (2026-10-14):

11. **The scaling study may observe ribbons running on the owner's Windows
    machine (A4a-2)**, asking before each use; the owner offered to install
    Office for it.
12. **Quick Access Toolbar, minimize and state (D1-1, D2-2, D3-1, D4-1):** on
    the tab row beside the File tab (or below the ribbon, the user's choice);
    commands only; a click outside the minimized popup is consumed; the
    ribbon saves its own state through the layout store, with explicit save
    and load available.
13. **Galleries and commands (E1-1, E2-2, E3-2):** preview events on gallery
    value commands only (300 ms hover, and keyboard focus); value-bearing
    commands are separate types sharing identity and state with `Command`;
    enhanced tooltips with an F1 help topic.
14. **Contextual tabs (F1-1, F2-1, F3-1):** theme accent ramp, application
    modes, built-in selection rules.
15. **Styling note for T2 and T4:** ribbon buttons are the dithered 3D button;
    at rest the dithering sinks into the flat command plane, and under the
    pointer it rises to form the button, so the user sees the control can be
    used; animated later (a T2 settling primitive). The states it needs
    already exist; "flat at rest, raised when hot" is theme data.

## Gate

The interview round is recorded, the decisions are in this brief, and the
owner accepts the plan.

## Read first

- `RIBBON_STUDY.md` (this folder), especially §8, the interview questions.
- `planning/DESIGN_LANGUAGE_SPECIFICATION.md` §3.5, §8, §12.
- `planning/stages/T4-default-theme/RENDERING_REFERENCE.md`.
- `planning/stages/W-winforms-controls/README.md`.
