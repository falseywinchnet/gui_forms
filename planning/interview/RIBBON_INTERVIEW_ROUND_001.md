# Ribbon framework interview, round 001

Date: 2026-10-14

Status: **owner verdicts recorded for parts 1 and 2 (KeyTips declined after
explanation); part 3 pending**.

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

## Part 3. Pending

Vocabulary (G1 to G10) and the remaining taste questions (T2 to T8).
