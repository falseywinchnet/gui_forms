# Ribbon framework interview, round 001

Date: 2026-10-14

Status: **complete. Owner verdicts recorded for every question of the round
(parts 1 to 3 and the notes below).**

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

## Part 2. Owner verdicts, 2026-10-14

The owner's message, verbatim:

> round 2:
> A4a: 2.
> I could install office.
> styling note for later:
> note that from our interviews in the last couple of days i indicated a specific styling for buttons that isnt a crisp outline, but rathere a dithered 3d button, and that we can animate later - we can make it so the dithering sinks into the flat support, then you move the mouse over and it "raises" to form the button so you know the control can be interacted with.  modern office styling may give specific behavioral cues i dont approve of, but if that doesnt impact implementation then we're fine for now.
> C: i dont know what a keytip is. explain.
>
> D:
> follows all recommend, same thing for E,F

A4a asked what the cross-ribbon scaling study may use: (1) documents only, or
(2) documents plus observing ribbons running on the owner's Windows machine
(WordPad uses the Windows Ribbon Framework there), asking before each use;
recommendation 2.

Recorded as:

- **GIVEN A4a-2.** The scaling study may observe ribbons running on the owner's
  Windows machine, asking before each use. The owner offered to install
  Office for it ("I could install office"); installing it is the owner's
  action, and the study asks before relying on it.
- **Styling note, recorded for later (stages T2 and T4, and taste question
  T2).** Ribbon buttons are not a crisp outline but the dithered 3D button of
  the earlier interviews (design language §3.5, dithered bulge; §12.4 to
  §12.6). At rest the dithering sinks into the flat support (the command
  plane); when the pointer moves over a button it rises to form the button,
  so the user knows the control can be interacted with; the rise can be
  animated later. Modern Office styling may give behavioural cues the owner
  does not approve of; that does not change the implementation now. Reading
  for the implementation: the states the engine needs (rest, hot, pressed)
  already exist; "flat at rest, raised when hot" is theme data, as the
  `menu_item` family already makes strip items flat at rest; the animation
  of the rise is a T2 settling primitive.
- **C1 to C4 not answered:** the owner asked what a KeyTip is. Explained in
  the conversation and asked again.
- **GIVEN D1-1, D2-2, D3-1, D4-1** ("follows all recommend"): the Quick
  Access Toolbar sits on the tab row beside the File tab, with "show below the
  ribbon" as the user's alternative; commands only, not whole groups, for now;
  a click outside the minimized ribbon's popup is consumed and only closes
  it; the ribbon saves its state (Quick Access Toolbar items and position,
  minimized) automatically through the runtime configuration's layout store,
  with explicit save and load also available.
- **GIVEN E1-1, E2-2, E3-2** ("same thing for E,F"): live preview and
  cancel-preview events on gallery value commands only, previewing after a
  300 ms hover and on keyboard focus; value-bearing commands (spinner, combo,
  gallery, colour, font) are separate command types sharing identity,
  enabled and visibility with `Command`; enhanced tooltips carry title,
  description, shortcut, image, availability reason and a help topic opened
  by F1.
- **GIVEN F1-1, F2-1, F3-1:** contextual tab set colours come from an
  ordered accent ramp the theme supplies; application modes are admitted (a
  mode set choosing core tabs and groups, as in the Windows Ribbon
  Framework); contextual tab selection rules are built in, driven by the
  application's availability calls.

### Note, 2026-10-14: KeyTips

After the explanation (Alt or F10 shows letter badges over tabs and commands;
typing a badge's letters opens a tab or runs a command), the owner, verbatim:

> we use shortcuts, memnonics, and tab,arrow navigation. we wont implement these keytips.

Recorded as:

- **GIVEN: no KeyTips.** The ribbon's keyboard model is GUI.Forms' existing
  one: shortcuts (key gestures bound to commands), mnemonics (underlined
  letters in labels), and Tab and arrow navigation. C1, C2 and C4 fall away,
  with the study's §5.3 KeyTip state machine and the KeyTip parts and states
  of §4 (G5, G10, taste question T5).
- **C3 (F6 cycling between window regions) is not answered by this note**;
  asked in part 3. So is what becomes of `Command::key_tip`, which nothing
  reads and now has no planned reader.

## Part 3. Owner verdicts, 2026-10-14

Asked: C3 (F6 cycling between window regions), C5 (remove the unused
`Command::key_tip`), G1 to G4 and G6 to G9 (G5 and G10 fell away with
KeyTips), and taste questions T2, T3, T4, T6, T7 and T8 (T5 fell away). The
owner's message, verbatim:

> accepted recommendations. for G:
> we dont have a minimize ribbon button. we follow plan paint- clicking the tab minimizes or opens the ribbon.tabs always visible.
> arrow mark- okay. more: okay. customize: okay.
> no. g7: yes. G8: yes. G9: yes.
>
> taste: anything large that normally draws a square under it when you mouse over. so yes.
> T3: note i just thought of: plan paint puts menus over ribbon, not inside ribbon.
> otherwise inconsistency cross platforms.
> I have never seen this topic tabs behavior.
> explain more.
> T4:
> unsure, show me.
> T6:
> T7: tab row only. cast shadow.
> T8: 1

Recorded as:

- **GIVEN C3-1 and C5-1** ("accepted recommendations", read as answering the
  two non-G questions asked first): F6 cycles focus between a window's
  regions (ribbon, work pane, side panels, status) through a window-level
  ordered list of regions, shared with side panels and File Manager; the
  unused `Command::key_tip` is removed.
- **G1: no.** There is no minimize-ribbon button. As in Plan Paint, clicking
  the tab minimizes or opens the ribbon, and the tabs are always visible.
  Recorded reading: clicking the selected tab collapses the ribbon to its
  tab row; with the ribbon collapsed, clicking a tab opens that tab's
  commands (as the temporary popup of D3 and T7); the up-chevron mark is not
  needed. If clicking the selected tab of a collapsed ribbon should pin it
  open again (Plan Paint's behaviour to be checked in its source), the plan
  follows Plan Paint.
- **GIVEN G2, G3, G4** ("arrow mark- okay. more: okay. customize: okay."):
  a dialog-launcher mark; a "more" mark and an overflow (double chevron)
  mark, which `ToolStrip`'s overflow button adopts; a customize mark for the
  Quick Access Toolbar.
- **GIVEN G6: no.** Themes do not select a group by its size; size shows
  only through which parts are present.
- **GIVEN G7, G8, G9: yes.** A per-item accent value for contextual tab sets
  (from the theme's accent ramp, F1-1); semantic roles `split_button` and
  `menu_button` (a toggle stays `button` plus checked); semantic node fields
  for position in set, set size and has-popup (the access-key field fell
  away with KeyTips).
- **T2: "anything large that normally draws a square under it when you
  mouse over. so yes."** Recorded rule: every large ribbon element that
  conventionally shows a hover square (command buttons and their split
  halves, collapsed-group buttons, gallery items, the dialog launcher and
  similar) takes the dithered button that sinks into the plane at rest and
  rises under the pointer. Small tab-row members do not (T8-1). Tabs never.
- **T3 not answered; the owner asked for more and added a note:** "plan
  paint puts menus over ribbon, not inside ribbon. otherwise inconsistency
  cross platforms." Recorded: menus opened from the ribbon (drop-downs, the
  application menu of A3-4) are popups over the ribbon, never laid out
  inside it, so they behave the same on every host. The owner has not seen
  Backstage's topic tabs; explained in the conversation and asked again.
- **T4 not answered:** "unsure, show me". Shown as mockups in the
  conversation and asked again.
- **T6 not answered** (left blank); asked again.
- **GIVEN T7:** the minimized ribbon shows its tab row only, and its
  temporary popup casts the overlay shadow (design language §3.4, E4).
- **GIVEN T8-1:** Quick Access Toolbar buttons are flat, like the other
  tab-row members.

### Note, 2026-10-14: T4 after the mockups

Shown three mockups of a "Picture tools" contextual set holding a "Format"
tab: (1) a captioned band above the tabs, as Office 2007 and 2010; (2) a
tint on the tab (and its command plane) only, no band; (3) both. The owner,
verbatim:

> 2. tint only.

- **GIVEN T4-2.** A contextual tab set is marked by a tint on its tabs and
  their command plane, from the theme's accent ramp (F1-1, G7); no captioned
  band above the tabs. How loud the tint may be is left to T4 (the default
  theme), beside the tab stack of the rendering reference.

### Note, 2026-10-14: T3 and T6 after the mockups

Shown three mockups: the application menu (Office 2007) dropping over the
ribbon; Backstage (Office 2010) replacing the window below the tab row, with a
dark left column of commands and topics (Save, Open, Close, Info, Recent, New,
Print, Options) and the chosen topic's page beside it; and a group collapsed
to one button (icon, caption and chevron; or caption only). T3 was asked as:
(1) Backstage later, with the left column of topics as Office 2010; (2)
Backstage later without a topic column, each application-menu item opening
its own page with a Back button; (3) no Backstage, keep the application menu
(revising A3-4). The owner, verbatim:

> on your three depictions:  backstage is good.  the color contrast also helps here.
> so option 1.
> T6: 1.

- **GIVEN T3-1.** Backstage, when it comes (A3-4), has the left column of
  commands and topics as Office 2010 does; the owner notes that the colour
  contrast between that column and the page helps.
- **GIVEN T6-1.** A collapsed group's button shows the group's icon, its
  caption and a chevron, as Office 2010 does.
