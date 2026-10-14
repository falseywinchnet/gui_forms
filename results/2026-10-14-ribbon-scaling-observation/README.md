# Ribbon scaling observation: WordPad and Word 2010

Date: 2026-10-14. Stage RB, phase 2a, the observed part (the documents part is
`RIBBON_SCALING_STUDY.md`, §6 there is the protocol this follows). Run by
worker 3 under RB decision 33 (the owner allowed this one session on WordPad
and Word 2010). Nothing was run before the owner's answer.

## Machine and programs

| | |
|---|---|
| Machine | the owner's Windows machine (Shadow), one display, 2816 × 1762 physical pixels |
| Windows | Windows 11 Home 22H2, build 22621.4317 |
| Display scale | 200 % (192 dpi, read from `GetDpiForWindow` on each program's window) |
| Text size | 100 % (no `TextScaleFactor` set) |
| WordPad | `C:\Program Files\Windows NT\Accessories\wordpad.exe` 10.0.22621.1; ribbon from `UIRibbon.dll` 10.0.22621.1 (Windows Ribbon Framework) |
| Word | Microsoft Word 2010, `WINWORD.EXE` 14.0.4762.1000 (the owner's 30-day trial), started as `WINWORD.EXE /w /q /a` (new blank document, no splash, no add-ins, settings files locked) |

All sizes in the data are **physical pixels**; at 200 % the logical
(96-dpi) size is half. Tables below give both.

## What was done

`scripts/observe.ps1` (PowerShell 5.1, built-in assemblies only; helpers in
`scripts/common.ps1` and `scripts/Win32Window.cs`) starts one program and acts
only on that process's windows:

- sets the main window's **client** size along a ladder (`SetWindowPos`, the
  frame measured, the script per-monitor DPI aware);
- reads the ribbon through **MSAA** (the NetUI ribbon publishes its tabs,
  groups and controls as `IAccessible`; UI Automation's managed client sees
  none of them): for each group, whether it is collapsed to its button and the
  size of each visible control;
- reduces that to a state signature; whenever the state changes between two
  ladder steps it **bisects to the exact pixel**, each probe approached from
  the side the sweep comes from (shrinking: from the wider step; growing: from
  the narrower);
- captures the top of the program's own window (`PrintWindow`, the window
  only, cropped to the title bar and ribbon) once per new state;
- selects a tab through its MSAA default action and selects Home again at the
  end; never touches the Quick Access Toolbar or the minimized state;
- restores the window placement and closes the program (`WM_CLOSE`); no
  document was edited and nothing was saved.

Commands (from the script folder; `-Out` is this folder's subfolder):

```text
observe.ps1 -App wordpad -Tabs Home,View -MaxWidth 2760 -MinWidth 300 -Step 40 -LadderHeight 1200
            -HeightLadderWidths 1600,800,600 -MaxHeight 1400 -MinHeight 160 -HeightStep 20
            -RestoreRect 98,98,2112,1212                                  -> wordpad/
observe.ps1 -App wordpad -Tabs Home -SkipWidth -HeightLadderWidths 1600,800,600
            -MaxHeight 1400 -MinHeight 120 -HeightStep 20 -RestoreRect 98,98,2112,1212  -> wordpad_height/
observe.ps1 -App word -Tabs Home -MaxWidth 2760 -MinWidth 300 -Step 40 -LadderHeight 1200
            -HeightLadderWidths 1600,800 -MaxHeight 1400 -MinHeight 120 -HeightStep 20  -> word2010/
summarize.py <folder> 192                                                  -> transitions.md
```

Files per folder: `run.log`, `probes.csv` (every probe: requested and actual
client size, state), `transitions.csv` and `transitions.md` (every boundary,
with what changed), `states.json` (each state's groups and control sizes),
`Home_Snn.png` / `View_Snn.png` (one capture per state; near-duplicate Word
gallery steps S02 to S05 and S08 dropped).

Ladders: width from 2760 down to 300 and back up in steps of 40 at client
height 1200 (logical 1380 to 150 at 600); height from 1400 down to 120 and
back at client widths 1600, 800 and 600 (WordPad) or 1600 and 800 (Word).
The 150 % display-scale and text-size runs of the protocol were not done
(they are the owner's to set, later).

Two measurement notes:

1. When the framework hides the ribbon, MSAA keeps reporting the last layout
   as visible. The first WordPad run therefore saw no hiding; a direct check
   (`wordpad_height/hide_check.txt`) showed the ribbon's dock window becoming
   invisible instead, and the height ladders were rerun with the dock window's
   visibility as the hidden signal (`wordpad_height/`, and Word's run). The
   committed `observe.ps1` is that second version.
2. Word changes its frame when the ribbon hides (its client area grows by 58
   px), so a requested client height is reached only on one side of the
   threshold; the threshold is read from the shrinking sweep, and the growing
   probes show the ribbon returning at the same outer window height.

## WordPad, Home tab (width at client height 600 logical)

| Shrinking: last px of old state | logical | What changed |
|---|---|---|
| 1343 → 1342 | 671.5 | Clipboard: Cut and Copy lose their labels (83 and 98 → 44 px wide, icon only) |
| 1289 → 1288 | 644.5 | Insert collapses to its group button |
| 1093 → 1092 | 546.5 | Editing collapses |
| 1026 → 1025 | 513 | Paragraph collapses |
| 874 → 873 | 437 | Clipboard collapses |
| 717 → 716 | 358.5 | Font collapses (every group is now a button) |
| 480 → 479 | 240 | scroll buttons appear |

Growing reverses every step at the same pixel, except the scroll buttons,
which disappear only at 499 (249.5 logical): **18 px (9 logical) of
hysteresis**, the only one observed. Insert (three large buttons) and Editing
(three small labelled buttons) go straight to their buttons: WordPad's policy
has no intermediate size for them. No image changes from large to small on
this tab.

## WordPad, View tab

| Shrinking | logical | What changed |
|---|---|---|
| 711 → 710 | 355.5 | Zoom: Zoom in, Zoom out, 100 % go from large buttons to small labelled rows |
| 601 → 600 | 300.5 | Zoom collapses |
| 525 → 524 | 262.5 | Show or hide collapses |
| 449 → 448 | 224.5 | Settings collapses |

No hysteresis.

## Word 2010, Home tab

| Shrinking | logical | What changed |
|---|---|---|
| 2669 → 2668 | 1334.5 | Styles: Quick Styles gallery 1396 → 1260 px (one column, 136 px) |
| 2533 → 2532 | 1266.5 | Quick Styles −1 column (1124) |
| 2397 → 2396 | 1198.5 | −1 column (988) |
| 2261 → 2260 | 1130.5 | −1 column (852) |
| 2125 → 2124 | 1062.5 | −1 column (716) |
| 1988 → 1987 | 994 | Clipboard: Cut, Copy, Format Painter lose their labels (icon only) |
| 1848 → 1847 | 924 | Quick Styles −1 column (580) |
| 1713 → 1712 | 856.5 | −1 column (444) |
| 1576 → 1575 | 788 | Editing collapses to its group button |
| 1424 → 1423 | 712 | Font: rearranged into its smaller layout (font name box 165 → 237 px wide) |
| 1302 → 1301 | 651 | Quick Styles gallery becomes one large button (beside Change Styles) |
| 932 → 931 | 466 | Styles collapses |
| 849 → 848 | 424.5 | Paragraph collapses |
| 691 → 690 | 345.5 | Font collapses |
| 453 → 452 | 226.5 | scroll buttons appear |

Growing reverses each step at the same pixel except the scroll buttons, which
go at 461 (8 px, 4 logical, of hysteresis). Clipboard never collapses (Paste
stays large) down to 150 logical. Captures: `word2010/Home_S01.png` (widest)
to `Home_S16.png` (scrolling), `Home_S17.png` (ribbon hidden). No licence,
activation, privacy or first-run prompt appeared on any of the three starts.

## Hide thresholds

| Program | Height | Width |
|---|---|---|
| WordPad | hidden at client height ≤ 237 px, shown at ≥ 238 (119 logical), at widths 800, 400 and 300 logical alike; no hysteresis | never hidden down to 300 px (150 logical), the narrowest probed; it scrolls instead |
| Word 2010 | hidden when the client height (with the ribbon shown) would be ≤ 237 px, shown at ≥ 238 (119 logical), at widths 800 and 400 logical; returns at the same outer window height | never hidden down to 150 logical; scrolls |

The ribbon's dock (tab row and command plane) is 242 px (121 logical) tall in
WordPad; both programs hide the whole ribbon, tab row included, once the
client is shorter than about that height.

## Comparison with the study

Expectations of `RIBBON_SCALING_STUDY.md` §1 to §4:

- **Confirmed.** Groups shrink by layout steps and end as group buttons
  showing icon, caption and chevron (decision 27); popups follow a designed
  order, not right to left (WordPad: Insert, Editing, Paragraph, Clipboard,
  Font; Word: Editing, Styles, Paragraph, Font); both drop labels before any
  group collapses (WordPad Cut/Copy at 671.5, Word Clipboard at 994);
  scrolling is the last resort; the hidden state hides the tab row too.
- **Galleries** (Word) give up exactly one column per step, interleaved with
  other groups' steps, then turn into one large button before the group
  collapses: Fluent's model and decision 8, as expected.
- **Hiding is by height only** in both programs, at about the ribbon's own
  height. The study's sources suggested a width threshold as well (Office 2007
  "below about 250 pixels wide"; WPF and Fluent.Ribbon 300 × 250); neither
  program hid by width down to 150 logical.
- **Hysteresis** exists only for the scroll buttons (9 logical in WordPad, 4 in
  Word); the study did not expect any.

Phase 2b's predictions for the WordPad-Home fixture (§5.9) against WordPad:

| | Phase 2b (default order, labels kept) | WordPad (authored policy) |
|---|---|---|
| Order of steps | WPF cycle from the right: Editing collapses, Insert to small labelled rows, Paragraph collapses, Font collapses, Clipboard's Paste to small, Insert collapses, Clipboard collapses | Clipboard labels drop, then Insert, Editing, Paragraph, Clipboard, Font collapse |
| Label drop | none (decision 6) | first step (Cut, Copy) |
| Intermediate layouts | Insert gets a small-row layout from the default generator | Insert and Editing have none: large straight to button |
| Width hide (H2) | below one collapsed group plus two scroll buttons | none down to 150 logical; scrolling |
| Height hide (H1, ratio 1.0) | when the workspace below would be shorter than the plane: client < tab row + 2 × plane, about 218 logical with WordPad's metrics | client < 119 logical, about the ribbon's own height (ratio about 0) |
| Hysteresis | 8 units on the hide rule only | none on hiding; 9 logical on the scroll buttons |

So with an authored order copied from WordPad (Insert, Editing, Paragraph,
Clipboard, Font to their buttons) the phase 2b model reproduces every WordPad
transition except the label drop, which decision 6 excludes; the default
order differs from WordPad's, as expected (WordPad's is authored). The hide
rule as decided (decision 31) hides the plane much earlier in height than
WordPad and Word hide their ribbons, and hides by width where they scroll;
that is a design choice to confirm, not a measurement error. No observed
program shows a gallery giving all its columns before the rest of its group
shrinks, which §5.5 assumes for the default cycle; Word's order interleaves
them because it is authored.

## Machine state afterwards

- Both programs were closed by the scripts; no WordPad or Word process remains.
- WordPad: an exploratory run before the measured runs (a DPI-unaware probe
  that restored a virtualized placement) left WordPad's saved window rectangle
  at 14,14 to 278,166; the measured run then put the window back at the
  rectangle seen at the first start (physical 98,98, 2112 × 1212) before
  closing, so WordPad saved `FrameRect` 98,98 to 2210,1310 again
  (`Applets\Wordpad\Options`, read back after the run). The Quick Access
  Toolbar and minimized state were never touched.
- Word: started with `/a` (settings files locked); after the run it was
  started once more to confirm it still opens maximized as before
  (`scripts/word_restore_check.ps1`: "at start: showCmd=3"), then closed.
- No system setting, display scale or text size was changed; no other window
  was touched.
