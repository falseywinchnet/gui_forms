# Ribbon framework interview, round 001

Date: 2026-10-14

Status: **owner verdicts recorded for part 1 (scope, structure, layout and
scaling, and taste question T1); parts 2 and 3 pending**.

The questions are section 8 of `planning/stages/RB-ribbon/RIBBON_STUDY.md`,
cited by their IDs (A1, B2 and so on). Each answer names the option number of
that question. This record is not rewritten: later answers and corrections
are added as dated notes.

## Part 1. Owner verdicts, 2026-10-14

The owner's message, verbatim:

> A1: 2.
> A2: 1.
> A3: 4.
> A4. 3,  we should study scaling across multiple ribbons
> B1: 2.
> B2: 2
> B3: 2
> b4: 1
> B5: 1
> T1: office 2010 and earlier.

Recorded as:

- **GIVEN A1-2.** The ribbon belongs to stage W, beside `ToolStrip`, not to a
  stage of its own (the study recommended a new stage). Stage RB remains the
  planning ministage (study, interview, decisions, plan); the controls it
  plans are built in W under the by-role paint path.
- **GIVEN A2-1.** Ribbon commands are records laid out by the ribbon, in a
  command item system as `ToolStrip`'s items are; hosted editors and
  galleries are real child controls.
- **GIVEN A3-4.** The File tab opens an application menu now (two panes with
  recent items) and a Backstage view later.
- **GIVEN A4-3, an option the owner added: "we should study scaling across
  multiple ribbons".** Recorded reading: the reference for group scaling is a
  comparative study of how several canonical ribbons scale (Office 2007 and
  2010, the Windows Ribbon Framework, MFC, WPF, Fluent.Ribbon), not one
  Windows Ribbon Framework fixture; neither option 1 nor option 2 as written.
  Whether the study may also observe ribbons running on the owner's Windows
  machine is asked in part 2 (A4a). If this reading is wrong, this one parse
  must be corrected by the owner.
- **GIVEN B1-2.** Scaling model: WPF/MFC, per-group size definitions plus a
  tab reduction order, with a deterministic default when the order runs out.
  (The study's recommendation added the Windows Ribbon Framework's ideal sizes
  and Fluent.Ribbon's per-control sizes as a shorthand; the owner answered 2
  without qualification, so those additions are not given and come back only
  if a later decision asks for them.)
- **GIVEN B2-2.** Commands never lose their labels when groups shrink: groups
  shrink only by image size, rows and popup groups. The study's
  recommendation also proposed option 1 as a per-ribbon opt-in; the owner
  answered 2 alone, so there is no opt-in.
- **GIVEN B3-2.** The ribbon hides in a small window by a rule derived from its
  content (the command area would take more than a declared fraction of the
  client height, or still needs scrolling after every group is a popup group
  below a declared width), not at fixed sizes.
- **GIVEN B4-1.** Intermediate scale steps are admitted: a gallery may give up
  one column at a time, as a step kind in the scaling order.
- **GIVEN B5-1.** No simplified one-line ribbon.
- **GIVEN T1: "office 2010 and earlier".** Where the canonical ribbons
  differ, follow the Office ribbons up to and including Office 2010 (Office
  2007 and 2010), not later Office, the Windows 7 Paint ribbon or Plan
  Paint's. Where 2007 and 2010 differ, A3-4 already orders them for the File
  area: 2007's application menu first, 2010's Backstage later.

## Part 2. Pending

Keyboard (C1 to C4), the Quick Access Toolbar, minimizing and persistence
(D1 to D4), galleries and commands (E1 to E3), contextual tabs (F1 to F3),
and the A4 follow-up (A4a).

## Part 3. Pending

Vocabulary (G1 to G10) and the remaining taste questions (T2 to T8).
