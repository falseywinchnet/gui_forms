# Ribbon scaling: cross-ribbon study and the phase 2b algorithm

Date: 2026-10-14. Status: research study for stage RB phase 2a (documents
part), written by worker 3 for the coordinator. It is not a decision record.
Nothing here is decided until the owner answers section 7. The observed part
of phase 2a (section 6) has not been run.

Inputs read first: `planning/stages/RB-ribbon/README.md` (decisions 1 to 27,
especially 4, 5, 6, 7, 8, 10, 11, 19, 21 and 27), `PLAN.md` (phase 2) and
`RIBBON_STUDY.md` §1.2 to §1.4 and §5.1, all on origin branch
`claude/rb-ribbon-ministage` at `9aedb620`.

**Reading key**, as in the first study. **Known** means read in a cited
primary source during this study (a Microsoft page, a Microsoft archived
blog by the Office UI team, or a source file of an open implementation, read
on the web on 2026-10-14). **Inferred** means reasoned from known material but
stated by no source. **Unverified** means remembered or reported second hand,
not checked against a primary source here. Quotations are exact.

Contents

1. Findings in one page
2. Each ribbon: sizes, order, galleries, collapse, labels, hiding, scrolling, scale
3. Side by side
4. Labels: where canonical ribbons drop them, and the label-keeping step
5. The GUI.Forms algorithm for phase 2b
6. Observation protocol for the owner's machine
7. Questions for the owner

Sources, by short name:

| Name | Source |
|---|---|
| WRF-SIZE | Customizing a Ribbon Through Size Definitions and Scaling Policies, https://learn.microsoft.com/en-us/windows/win32/windowsribbon/windowsribbon-templates |
| WRF-POLICY | ScalingPolicy element, https://learn.microsoft.com/en-us/windows/win32/windowsribbon/windowsribbon-element-scalingpolicy |
| WRF-SCALE | Scale element, https://learn.microsoft.com/en-us/windows/win32/windowsribbon/windowsribbon-element-scale |
| WRF-GSD | GroupSizeDefinition element, https://learn.microsoft.com/en-us/windows/win32/windowsribbon/windowsribbon-element-groupsizedefinition |
| WRF-CSD | ControlSizeDefinition element, https://learn.microsoft.com/en-us/windows/win32/windowsribbon/windowsribbon-element-controlsizedefinition |
| WRF-IRG | InRibbonGallery element, https://learn.microsoft.com/en-us/windows/win32/windowsribbon/windowsribbon-element-inribbongallery |
| WRF-IMG | Specifying Ribbon Image Resources, https://learn.microsoft.com/en-us/windows/win32/windowsribbon/windowsribbon-imageformats |
| WRF-STATE | Persisting Ribbon State, https://learn.microsoft.com/en-us/windows/win32/windowsribbon/ribbon-statepersistence |
| WRF-SHOW | Displaying the Ribbon, https://learn.microsoft.com/en-us/windows/win32/windowsribbon/ribbon-visibility |
| UX7 | Windows 7 Ribbons (UX guidelines), https://learn.microsoft.com/en-us/windows/win32/uxguide/cmd-ribbons |
| OFF-2010 | Ribbon Extensibility in Office 2010: Tab Activation and Auto-Scaling, https://learn.microsoft.com/en-us/previous-versions/office/developer/office-2010/ee691834(v=office.14) |
| JH-SCALE | Jensen Harris, "Scaling Up, Scaling Down", 2005-10-18, https://learn.microsoft.com/en-us/archive/blogs/jensenh/scaling-up-scaling-down |
| JH-HIDE | Jensen Harris, "A Disappearing Act", 2005-10-19, https://learn.microsoft.com/en-us/archive/blogs/jensenh/a-disappearing-act |
| MFC-CAT | CMFCRibbonCategory, https://learn.microsoft.com/en-us/cpp/mfc/reference/cmfcribboncategory-class |
| MFC-PANEL | CMFCRibbonPanel, https://learn.microsoft.com/en-us/cpp/mfc/reference/cmfcribbonpanel-class |
| MFC-ELEM | CMFCRibbonBaseElement, https://learn.microsoft.com/en-us/cpp/mfc/reference/cmfcribbonbaseelement-class |
| WPF-LAYOUT | Ribbon Layout and Resizing (Microsoft Ribbon for WPF), https://learn.microsoft.com/en-us/previous-versions/ff701790(v=vs.110) |
| WPF-GSRO | RibbonTab.GroupSizeReductionOrder, https://learn.microsoft.com/en-us/dotnet/api/system.windows.controls.ribbon.ribbontab.groupsizereductionorder |
| WPF-GSD | RibbonGroup.GroupSizeDefinitions, https://learn.microsoft.com/en-us/dotnet/api/system.windows.controls.ribbon.ribbongroup.groupsizedefinitions |
| WPF-COLL | Ribbon.IsCollapsed, https://learn.microsoft.com/en-us/dotnet/api/system.windows.controls.ribbon.ribbon.iscollapsed |
| WPF-SRC | dotnet/wpf source, branch `main`, `src/Microsoft.DotNet.Wpf/src/System.Windows.Controls.Ribbon/Microsoft/Windows/Controls/Ribbon/`: `Ribbon.cs`, `RibbonTab.cs`, `RibbonGroup.cs`, `Primitives/RibbonGroupsPanel.cs` |
| FR-SRC | Fluent.Ribbon source, branch `develop`: `Fluent.Ribbon/Controls/RibbonGroupsContainer.cs`, `RibbonGroupBox.cs`, `InRibbonGallery.cs`, `Ribbon.cs`, `Fluent.Ribbon/Data/RibbonGroupBoxStateDefinition.cs`, `Fluent.Ribbon/AttachedProperties/RibbonProperties.cs` |
| FR-DOC | Fluent.Ribbon sizing documentation, https://fluentribbon.github.io/documentation/concepts/sizing |
| WIN-DEP | Resources for deprecated features in the Windows client, https://learn.microsoft.com/en-us/windows/whats-new/deprecated-features-resources |

Source code was read through GitHub's raw file server; nothing was downloaded
to disk or built. MFC's own source ships only with Visual Studio
(`VC\atlmfc\src\mfc`, MFC-PANEL), which this study did not have, so MFC's
internal algorithm is documented here only as far as its reference pages go.

---

## 1. Findings in one page

1. **Every canonical ribbon scales in the same three moves**, in this order of
   preference: shrink command images (large to small) and pack small commands
   into rows of up to three; then turn a whole group into one button that opens
   the group at its largest layout in a popup; then, when even that does not
   fit, scroll the groups or hide the command area. (Known: WRF-SIZE, WRF-SCALE,
   WPF-LAYOUT, MFC-PANEL, JH-SCALE, FR-SRC.)
2. **Order.** The Windows Ribbon Framework (WRF) and Office's own tabs take the
   order from the author, step by step. WPF and MFC take an author order and,
   when it runs out, fall back to a built-in order. WPF's fallback is published
   and read in its source: every group, **right to left, one step at a time,
   cyclically** (WPF-LAYOUT, WPF-SRC `RibbonTab.cs`). MFC's fallback is
   "the internal algorithm" and is not published (MFC-CAT). Office 2010
   add-in groups with `autoScale` scale "always from right-to-left" (OFF-2010).
   Fluent.Ribbon has **no** fallback: without `ReduceOrder` it does not reduce
   at all and scrolls (FR-SRC).
3. **Office's designers report that the order hardly matters to users but
   predictability does**: "Other rules we thought would be important (such as
   always collapsing from right to left) turned out to not matter at all", and
   their rules include "No commands ever appear or disappear between chunk
   sizes" and "Popup chunks show the same layout people would have seen in the
   Ribbon itself" (JH-SCALE). This supports a deterministic default with an
   authored override, which is decision 5.
4. **Galleries absorb width between group sizes.** WRF galleries have a column
   range per group size (`MinColumnsLarge` to `MaxColumns`, `MinColumnsMedium`
   to `MaxColumnsMedium`, WRF-IRG); Fluent.Ribbon reduces one column per
   `(Name)` step down to `MinItemsInRow`, then collapses the gallery to a
   button (FR-SRC `InRibbonGallery.cs`). Decision 8 (one column at a time)
   matches Fluent's model and WRF's ranges.
5. **Labels.** WRF (custom Small, `IsLabelVisible="false"`), WPF (its Small
   control size and its default size definitions), MFC (compact mode, "a small
   image only") and Office's product tabs ("The smaller chunks sometimes have
   no labels at all") all drop labels at their smallest non-popup size. Under
   decision 6 the smallest non-popup size is the small image *with* its label,
   so GUI.Forms groups reach the popup step at wider widths than canonical
   ribbons do (Inferred). Section 4 lists each drop and its equivalent.
6. **Hiding is content-derived in WRF and WPF's documentation, fixed in WPF's
   and Fluent's code.** WRF hides the ribbon "if the application window is
   reduced in size, either horizontally or vertically, to the point that the
   ribbon obscures the application workspace" (WRF-STATE) and "when all
   potential control layouts have been exhausted and the ribbon cannot be
   rendered with a usable application workspace" (WRF-IMG). WPF's documentation
   says the same in words (WPF-COLL), but its code collapses below a window of
   300 × 250 (WPF-SRC `Ribbon.cs`, `CollapseWidth = 300.0`,
   `CollapseHeight = 250.0`), the same numbers as Fluent.Ribbon (FR-SRC). Office
   2007 hid "below about 250 pixels wide" (JH-HIDE). Office, WRF and WPF hide
   the whole ribbon, tab row included (Known from JH-HIDE, the WRF-IMG
   screenshot and WPF-COLL; Fluent Inferred; MFC not documented); GUI.Forms
   keeps its tab row
   (decision 19 and PLAN phase 2b), which is a difference to confirm (question
   7.1).
7. **Scrolling** is the last resort in WRF ("before automatically introducing a
   scrolling mechanism", WRF-SIZE; "render without scroll bars when resized to a
   width of 300 pixels at 96 dots per inch", WRF-POLICY), in MFC (category scroll
   buttons, MFC-CAT) and in Fluent.Ribbon (`IScrollInfo`, 48 units per line,
   FR-SRC). Office's designers ruled out scroll arrows as the primary mechanism
   (JH-SCALE). WPF's panel leaves a comment `//// Scroll if not enough space`
   with no code after it (WPF-SRC `RibbonGroupsPanel.cs`): content is clipped
   (Inferred).
8. **DPI** scales images linearly (16/32 px at 96 dpi to 32/64 px at 192 dpi,
   WRF-IMG); layout in logical units is therefore DPI-independent up to
   rounding (Inferred). **Text scale** is not addressed by any source read;
   larger text widens labels and so moves every transition to a wider window
   (Inferred).

What 2b must settle that no source settles: the default order's treatment of
gallery steps, the hide rule's numbers when derived from content, what a
non-shrinking step does, and how a group's caption bounds its width. Section 5
proposes each.

---

## 2. Each ribbon

### 2.1 Office 2007 and 2010 (the product, and add-in groups)

- **Hand-designed layouts and order.** "Each "chunk" in the Ribbon can have
  multiple layouts ... The larger chunks attempt to label all commands and
  present bigger buttons for more important commands. The smaller chunks
  sometimes have no labels at all and require tooltips to see what commands
  do" (JH-SCALE, Known). "Each of the chunk sizes is designed by hand, and the
  order in which the chunks collapse into different versions is also designed
  by us and not by the computer. There's no attempt to "auto-scale" the UI"
  (JH-SCALE, Known).
- **Popup chunks.** "Chunks can also turn into "popup" chunks, in which the
  entire chunk turns into a singe button. When the button is clicked, a
  full-size version of the chunk contents pop down, like a menu. This is the
  most compact version of any chunk and it only used at small widths (because
  it adds extra clicks.)" (JH-SCALE, Known.)
- **The four rules** (JH-SCALE, Known): no commands appear or disappear between
  chunk sizes; multiple commands cannot be rolled into a menu at smaller sizes;
  "Keep labels on commands as long as possible unless the icons themselves are
  well-known"; popup chunks show the same layout as in the ribbon.
- **Order did not matter in the lab**: "always collapsing from right to left ...
  turned out to not matter at all" (JH-SCALE, Known). A reader comment on the
  same post observes the demo tab collapsing right to left (Unverified for the
  shipped product).
- **Add-in groups (Office 2010 customUI).** `autoScale="true"` per group; "the
  ribbon is resized horizontally (always from right-to-left), the controls in
  that group also resize automatically from the normal size ... to a medium
  size ... to a smaller size ... successively", ending as "a single button"
  whose icon is "an image to the group itself" (OFF-2010, Known). Whether one
  autoScale group reaches its single button before its left neighbour starts to
  shrink (whole-group-first) or the groups alternate (cyclic) is not stated
  (Unverified; observable, section 6).
- **Hiding.** "Below about 250 pixels wide ... the whole Ribbon disappears,
  leaving you with just your document ... When you size the window back over
  the threshold, the Ribbon reappears" (JH-HIDE, Known for Office 2007 beta;
  the shipped threshold and any height rule are Unverified).
- **Scrolling.** Ruled out as the scaling mechanism: "this ruled out just
  adding scroll arrows to either side" (JH-SCALE, Known). Whether Office 2007 or
  2010 scrolls between the last layout and the hide threshold is Unverified.
- **Galleries.** "Ribbon tabs that include in-ribbon galleries scale up very
  well, because they can contain any number of discrete items" (JH-SCALE,
  Known); the step size of gallery shrinking in Office is Unverified.
- **Text and DPI.** The blog names "a maximized 2048x1600 200 DPI screen" as a
  target (JH-SCALE, Known); no rule for text size is given.

### 2.2 Windows Ribbon Framework (Paint and WordPad on Windows 7 to 11 22H2)

- **Group sizes.** A group's `SizeDefinition` (a named template or a custom
  one) has up to three `GroupSizeDefinition`s, `Large` (the default), `Medium`
  and `Small` (WRF-GSD, Known). `Popup` is not definable: it is "Identical
  control layout to `Large` but hosted in a pop-up or a drop-down pane"
  (WRF-SCALE, Known).
- **Control sizes.** `ControlSizeDefinition` has `ImageSize` (`Large` or
  `Small`, default `Small`), `IsLabelVisible` (default `true`),
  `IsImageVisible` (default `true`) and `IsPopup` (WRF-CSD, Known). Layout
  elements are `Row` (a row of small controls), `ControlGroup` (controls kept
  together), and `ColumnBreak ShowSeparator` (WRF-SIZE, Known).
- **Templates.** Twenty named templates fix the control family, count and
  order (`OneButton` to `ElevenButtons`, `ButtonGroups`, the gallery and font
  templates); "If the controls declared in markup do not map exactly to control
  type, order, and quantity defined in the associated template, a validation
  error is logged ... and compilation is terminated" (WRF-SIZE, Known). Some
  templates support only some sizes: `OneButton` only Large; `TwoButtons`,
  `ThreeButtons` Large and Medium; `OneInRibbonGallery` Large and Small
  (WRF-SIZE, Known).
- **Order.** A tab's `ScalingPolicy` lists `IdealSizes` (the starting size of
  each group) and then `Scale` steps; "The list of Scale declarations must be
  in descending order of valid sizes (Large, Medium, Small, Popup) for the
  SizeDefinition associated with the Group element" and "Each (Group, Size)
  attribute pair must be unique" (WRF-POLICY, WRF-SCALE, Known). The documented
  example interleaves groups: Clipboard Small, Clipboard Popup, Font Medium,
  Font Popup, Paragraph Medium, Paragraph Popup (WRF-SIZE, Known). The policy
  is applied in list order (Inferred from "manifest of size preferences ... used
  as a guide").
- **Default.** "The Ribbon framework provides default layout behaviors based on
  a set of built-in heuristics ... However, this capability is intended for
  prototyping purposes only" (WRF-SIZE, Known). The heuristics are not
  published.
- **Galleries.** Column ranges per group size: `MaxColumns`,
  `MaxColumnsMedium` ("before switching to Large layout"), `MinColumnsLarge`
  ("before switching to Medium"), `MinColumnsMedium` ("before switching to
  Small"), `MaxRows` (default 1) (WRF-IRG, Known). So within one group size the
  gallery gives up columns one at a time as space shrinks, and the group changes
  size only when the gallery reaches its minimum for that size (Inferred from
  the attribute descriptions). `InRibbonGalleryAndButtons-GalleryScalesFirst`
  turns the gallery into a popup at Medium and Small (WRF-SIZE, Known).
- **Single-button groups.** "Don't scale groups containing a single button to a
  pop-up group icon" (UX7, Known); the `OneButton` template supports only Large
  (WRF-SIZE, Known).
- **Popup group icon.** "Consider creating pop-up group icons by placing the
  16x16 pixel icon of the most prominent command in the group inside a 32x32
  pixel visual container" (UX7, Known).
- **Scrolling.** Recommended policies let the ribbon reach Popup for most
  groups "before automatically introducing a scrolling mechanism" (WRF-SIZE,
  Known), and "render without scroll bars when resized to a width of 300 pixels
  at 96 dots per inch (dpi)" (WRF-POLICY, Known).
- **Hiding.** "The framework invokes this state automatically if the application
  window is reduced in size, either horizontally or vertically, to the point
  that the ribbon obscures the application workspace. The framework restores
  the ribbon when the size of the application window is increased" (WRF-STATE,
  Known). "The ribbon is hidden when all potential control layouts have been
  exhausted and the ribbon cannot be rendered with a usable application
  workspace" (WRF-IMG, Known). The tab row is hidden too in that state (Known
  from the WRF-IMG screenshot caption "the ribbon in hidden state"; the exact
  test is not published).
- **Tab labels.** "Avoid tab names with the same initial letters ... When the
  ribbon is scaled down, these tab names will have the same truncated text"
  (UX7, Known): WRF truncates tab labels at narrow widths.
- **DPI.** Images 16/32 px at 96 dpi, 20/40 at 120, 24/48 at 144, 32/64 at
  192, "increase in a linear fashion relative to dpi" (WRF-IMG, Known).
- **Most-used last.** "Use 32x32 pixel icons for the most frequently used and
  important labeled commands. When scaling a group down, make these commands
  the last to convert to 16x16 pixel icons" (UX7, Known).
- **No default size.** "There is no default ribbon size ... The ribbon will show
  the largest valid combination that fits the current window size" (UX7, Known).

### 2.3 MFC `CMFCRibbonBar`

- **Element modes.** An element has up to three modes: large, intermediate ("a
  small image and text on the right of the image", 16 × 16) and compact ("an
  element displays a small image only") (MFC-ELEM, Known). `SetCompactMode`
  steps one mode at a time: large to intermediate, intermediate to compact
  (MFC-ELEM, Known).
- **Panels.** "As the size of the ribbon changes, the panel layout automatically
  adjusts to display as many elements as possible"; `RecalcWidths` "Recalculates
  the width of each display layout configuration for the ribbon panel"
  (MFC-PANEL, Known): a panel precomputes several layout widths.
- **Collapse.** "When a ribbon panel is collapsed, it only displays its default
  button, its name, and a drop-down arrow"; the default button's icon is the
  panel icon given to `AddPanel`; `ShowPopup` is "only available when the
  display of the ribbon panel is collapsed" (MFC-PANEL, MFC-CAT, Known). This
  is decision 27's icon, caption and chevron.
- **Order.** `SetCollapseOrder(indexes)`: "When the category detects that it has
  to collapse a ribbon panel, it looks for the next element in the specified
  list. If the list is empty, or you have not specified enough elements, then
  the category uses the internal algorithm" (MFC-CAT, Known). An index may
  repeat ("0, 0, 2, 2 ... The panel that has the index of 1 remains
  uncollapsed", MFC-CAT, Known). The internal algorithm is not published
  (Unverified).
- **Scrolling.** `HitTestScrollButtons`, `OnScrollHorz` (MFC-CAT, Known).
- **Hiding.** Not documented on the pages read (Unverified).
- **DPI.** `GetImageSize` "includes the global image scale factor" (MFC-CAT,
  Known).

### 2.4 WPF `System.Windows.Controls.Ribbon`

- **Control sizes.** Large: "a large image and up to two lines for a label";
  Medium: "a small image and one line for a label"; Small: "just a small image
  and no label" (WPF-LAYOUT, Known). `RibbonControlSizeDefinition` also has
  `Width`, `MinWidth`, `MaxWidth` for container controls and `IsCollapsed`
  (WPF API page, Known).
- **Group size definitions.** An ordered collection, largest first, each a
  list of control size definitions "applied to controls in the group in
  sequential order", the last usually `IsCollapsed="True"`; "every group should
  specify at least two predefined size variants: a large variant and a
  collapsed variant"; "When a group is collapsed, the largest specified variant
  is shown when the drop-down in opened" (WPF-LAYOUT, WPF-GSD, Known). A
  `RibbonControlGroup` takes one size definition for all its controls
  (WPF-LAYOUT, Known).
- **Default size definitions** when none are given (WPF-SRC `RibbonGroup.cs`,
  Known from source): the large definition from each control's own size; then,
  for more than three controls, runs of three like-sized controls shrink from
  the end: "L L L L -> L M M M -> L S S S -> Collapsed" (source comment); a
  group of two or three large labelled controls gains one medium definition; a
  single control has only Large and Collapsed; the last definition is always
  collapsed.
- **Tab order.** `GroupSizeReductionOrder`, a list of group names, one per step,
  repeats allowed (WPF-GSRO, Known). Rules (WPF-LAYOUT, Known): an unknown name
  "is ignored"; a name listed more times than the group has variants: "the
  group resizes to its smallest variant and the additional times it appears in
  the collection are ignored"; groups not listed "are reduced in size
  right-to-left, one-by-one, in cyclical order, after all of the user-specified
  reductions"; with no list at all, every group is reduced that way; the order
  is mirrored for right-to-left cultures.
- **The loop** (WPF-SRC, Known from source). `RibbonTab.DecreaseNextGroupSize`
  walks the author list, then `DefaultCyclicalReduceGroup` from the rightmost
  group leftwards, wrapping; a pass that tries every group without success
  ends reduction. Increases undo reductions in reverse order (two stacks: the
  author steps and the cyclic steps). `RibbonGroupsPanel.MeasureOverride`
  shrinks while the remaining space is negative and grows while it is positive,
  caching the width at which the last failed growth happened
  (`_nextGroupIncreaseWidth`) so that it does not retry a growth that cannot fit,
  and giving leftover space to star-sized controls (`StarMeasure`).
- **When nothing more can shrink**: the panel returns its desired size and the
  comment `//// Scroll if not enough space` has no code after it (WPF-SRC); the
  content is clipped (Inferred).
- **Hiding.** "The ribbon collapses automatically when the application window
  is resized and does not have enough space to display the ribbon in a
  meaningful way. When the ribbon is collapsed, all ribbon elements are hidden
  from view" (WPF-COLL, Known). In code: the window's `ActualWidth` below 300 or
  `ActualHeight` below 250 (WPF-SRC `Ribbon.cs`, Known).

### 2.5 Fluent.Ribbon

- **Group states.** `Large`, `Middle`, `Small`, `Collapsed` (and `QuickAccess`);
  the default `StateDefinition` is all four in that order (FR-SRC
  `RibbonGroupBoxStateDefinition.cs`, Known); a control's `SizeDefinition` maps
  group state to control size, default `Large, Middle, Small` (FR-SRC
  `RibbonProperties.cs`, Known). Whether `Small` hides the label is set in the
  control templates, which were not read (Unverified; Fluent's screenshots show
  icon-only small buttons, Unverified).
- **Order.** `RibbonTabItem.ReduceOrder`, a list of group names split on commas
  and spaces; a plain name steps the group's state, `(Name)` steps its scale
  (gallery columns) (FR-SRC `RibbonGroupsContainer.cs`, Known). **The code
  reduces the last entry first** (it starts at `reduceOrderIndex = Length - 1`
  and moves to 0), although the comment says the list runs "from the first to
  reduce" (FR-SRC, Known; the first study reported the same disagreement).
- **No default.** With an empty `ReduceOrder` (the default) the panel returns
  the sum of its children's widths and scrolls; nothing reduces (FR-SRC, Known).
- **Galleries.** `InRibbonGallery.Reduce()` lowers the panel's `MaxItemsInRow` by
  one while above `MinItemsInRow` (defaults 8 and 1), then collapses the gallery
  to a button when `CanCollapseToButton` (default true) (FR-SRC
  `InRibbonGallery.cs`, Known).
- **Caching.** The container caches the last (available, desired) pair and
  returns early when measured with the same size; it keeps `reduceOrderIndex`
  between passes; a child's desired-size change resets every group and the
  index (FR-SRC, Known).
- **Scrolling.** `IScrollInfo`, horizontal only, 48 units per line or wheel
  step (FR-SRC, Known).
- **Hiding.** Below an owner window of 300 wide or 250 high when
  `IsAutomaticCollapseEnabled` (default true) (FR-SRC `Ribbon.cs`, Known).

---

## 3. Side by side

### 3.1 Size vocabulary

| | Group sizes | Command sizes | Popup group |
|---|---|---|---|
| Office (product) | hand-designed "chunks", several per group | large with label; small with label; small without label | "full-size version" pops down |
| WRF | Large, Medium, Small (+ Popup) | `ImageSize` Large/Small × `IsLabelVisible` × `IsImageVisible`; rows; control groups | Large layout in a pop-up |
| MFC | panel layout configurations (count not published) | large; intermediate (small image + text); compact (small image) | default button, name, arrow; popup |
| WPF | ordered `RibbonGroupSizeDefinition`s, last collapsed | Large (2-line label), Medium (1-line), Small (no label) | largest variant in a drop-down |
| Fluent | Large, Middle, Small, Collapsed | per control per state | button; popup |

### 3.2 Order and defaults

| | Authored order | When it runs out or is absent |
|---|---|---|
| Office product | designed per tab | n/a |
| Office add-ins | none (per-group `autoScale`) | right to left; group sequence normal, medium, small, button |
| WRF | `ScalingPolicy` steps, descending per group | heuristics "for prototyping purposes only"; then scrolling |
| MFC | `SetCollapseOrder` panel indexes | "internal algorithm" (not published) |
| WPF | `GroupSizeReductionOrder` names | right to left, one step per group, cyclic; mirrored in RTL |
| Fluent | `ReduceOrder`, last entry first | none: scrolling only |

### 3.3 Galleries

| | How a gallery shrinks |
|---|---|
| WRF | column range per group size; gallery may become a popup in Medium/Small (template) |
| Fluent | one column per `(Name)` step to `MinItemsInRow`, then a button |
| WPF | star sizing of container controls (`Width`, `MinWidth`, `MaxWidth`) with leftover space (WPF-SRC `StarMeasure`); in-ribbon gallery details not studied |
| MFC | not documented on the pages read |
| Office | not documented (Unverified) |

### 3.4 Hide rule

| | Rule | Hides |
|---|---|---|
| Office 2007 | window "below about 250 pixels wide" | whole ribbon |
| WRF | content: ribbon would obscure the workspace, horizontally or vertically, after all layouts are exhausted | whole ribbon |
| WPF | documented as content; coded as window width < 300 or height < 250 | whole ribbon |
| Fluent | window width < 300 or height < 250 | whole ribbon |
| MFC | not documented | — |
| GUI.Forms (decisions 7, 19) | content-derived | command plane only; tab row stays |

### 3.5 Scrolling

WRF and MFC scroll buttons, Fluent scroll offset, WPF clips (Inferred). Office
rejected scrolling as the mechanism; whether it scrolls before hiding is
Unverified.

### 3.6 DPI and text scale

DPI: WRF images scale linearly (WRF-IMG); MFC sizes include the image scale
(MFC-CAT); WPF and Fluent work in device-independent units (WPF platform,
Known in general; Inferred for the ribbon). In logical units the ladder of
layouts and its transition widths do not change with DPI except for rounding
(Inferred). Text scale: no source addresses it. Windows' "Make text bigger"
setting enlarges system fonts, which WRF uses for labels (Unverified;
observable, section 6).

---

## 4. Labels: where canonical ribbons drop them, and the label-keeping step

Decision 6: labels never drop; groups shrink by image size, rows and popup
groups only. The table records each place a canonical ribbon drops labels and
the step GUI.Forms takes instead.

| Ribbon | Where labels drop | Label-keeping equivalent |
|---|---|---|
| Office product | "smaller chunks sometimes have no labels at all" (JH-SCALE); toolbar-style groups such as Font are icon-only at every size (UX7 "toolbar-style groups ... that don't need labels") | the small labelled layout is the floor; the next step is the popup group |
| WRF | custom `Small` with `IsLabelVisible="false"` (WRF-SIZE example: buttons 3 and 4); templates' Small sizes show icon-only rows (pictures, Known; label state per template Unverified) | `Small` = same rows as `Medium`, labels kept; a template whose Small differs from Medium only by labels has no Small step |
| MFC | compact mode ("a small image only", MFC-ELEM) | intermediate mode (small image, text on the right) is the floor |
| WPF | Small control size ("just a small image and no label"); default definitions `L S S S` (WPF-SRC) | `L M M M` is the floor; the next step is collapsed |
| Fluent | `Small` control size (label hidden by templates, Unverified) | `Middle` is the floor |

Consequences (Inferred):

- A GUI.Forms group has at most three non-popup layouts that differ: commands
  large; some commands moved to small labelled rows; all commands in small
  labelled rows. Canonical ribbons have a fourth (small, unlabelled) that is
  roughly the label widths narrower.
- So, for the same commands, GUI.Forms reaches popup groups at wider windows
  than the canonical ribbons. In the observation (section 6) the transitions
  that GUI.Forms should reproduce are the image-size and row transitions; the
  label-drop transitions have no counterpart and are recorded only to measure
  how much width labels cost.
- Two places are not "dropping" in the canonical sense and need an owner ruling
  (question 7.3): toolbar-style groups whose commands are authored without
  labels at every size (Bold, Italic, alignment), and tab labels, which WRF
  truncates when the tab row is narrow (UX7).

---

## 5. The GUI.Forms algorithm for phase 2b

The WPF/MFC model (decision 5): per-group size definitions plus a tab reduction
order, with a deterministic default when the order runs out. Everything below
is a proposal for the plan's phase 2b, written to be implemented and tested
under the house style. Logical units are device-independent units at the
window's text scale; widths are `double`.

### 5.1 Data model (records the application supplies)

```cpp
// How a command shows its image. Labels always show (decision 6), so there
// is no label flag.
enum class RibbonImageSize : std::uint8_t { large, small };

// One column of a group layout: one large command, or a stack of one to three
// small commands (rows), or one hosted control row stack. A control group
// keeps its commands side by side on one row (toolbar-style group).
enum class RibbonColumnKind : std::uint8_t { large_command, stack };

struct RibbonColumn final {
    RibbonColumnKind kind{RibbonColumnKind::stack};
    // Indexes into the group's commands (or control groups), in row order;
    // one for large_command, one to three for stack.
    std::array<std::uint16_t, 3> rows{};
    std::uint8_t row_count{};
    bool divider_before{};   // ColumnBreak ShowSeparator
};

// One layout of a group, largest first. `collapsed` layouts hold no columns:
// the group shows its button (icon, caption, chevron; decision 27) and opens
// its first (largest) layout in a popup.
struct RibbonGroupLayout final {
    bool collapsed{};
    std::vector<RibbonColumn> columns;
    // Column range of the group's in-ribbon gallery in this layout, when the
    // group has one (WRF MinColumns*/MaxColumns*; decision 8).
    std::uint16_t gallery_minimum_columns{};
    std::uint16_t gallery_maximum_columns{};
};

struct RibbonGroupDefinition final {
    std::string id;                        // unique within the tab
    std::vector<RibbonGroupLayout> layouts; // empty: generate (5.4)
};

// One authored reduction step: shrink `group` by its next layout, or by one
// gallery column (Fluent's "(Name)", decision 8).
enum class RibbonStepKind : std::uint8_t { layout, gallery_column };
struct RibbonReductionStep final {
    std::string group;
    RibbonStepKind kind{RibbonStepKind::layout};
};

struct RibbonTabScaling final {
    std::vector<RibbonReductionStep> order; // may be empty
};
```

Commands carry, besides their shared `Command`, an authored *importance*
(`primary` or `secondary`, default `secondary`) used only by the default layout
generator (UX7: the most used large commands are the last to become small).

### 5.2 Validation (before anything is built; house style §9)

A tab definition is rejected, with a diagnostic and the previous ribbon kept,
when:

1. a group id repeats, or a reduction step names an unknown group (WPF ignores
   unknown names; GUI.Forms rejects them, question 7.2);
2. a layout places a command twice, omits a command, or a stack holds zero or
   more than three rows ("no commands ever appear or disappear between chunk
   sizes", JH-SCALE);
3. a collapsed layout is not last, or a group of one command has a collapsed
   layout (UX7);
4. a gallery range is empty (`minimum > maximum`) or a gallery step names a
   group without a gallery;
5. an authored step would shrink a group past its last layout or its gallery
   below its minimum (WPF silently ignores this; GUI.Forms rejects it).

Measured widths are not validated: whether a step narrows the tab depends on the
font and text scale, so it is handled in the ladder (5.5).

### 5.3 Measuring a group

For a layout L of group g, at the current font, text scale and theme metrics:

```text
large column width  = max(large_image, width of the label in at most two lines)
                      (the two-line break is chosen to minimize the wider line)
stack column width  = max over its rows of
                      small_image + gap + label width (+ arrow for drop-down/split)
                      (a control-group row is the sum of its commands' widths)
content width(L)    = sum of column widths + gaps + dividers + group padding
caption width       = width of the group caption (+ dialog launcher when present)
width(g, L)         = max(content width(L), caption width)
collapsed width     = max(large_image, caption in at most two lines) + chevron
                      + padding (decision 27)
gallery column cost = one item width; a gallery at c columns adds c * item width
```

Labels never truncate; a label wider than the window is left to the popup or to
scrolling. (The canonical sources do not say how a caption limits a group's
width; `max(content, caption)` is Inferred from screenshots and must be
checked in section 6.)

### 5.4 Default layouts when a group gives none

Generated deterministically from the commands, after WPF's default
(WPF-SRC) with labels kept:

1. **Layout 0.** Each command with a large image and importance `primary`, or
   authored as large, is a large column; the others fill stacks of three in
   declaration order.
2. **Following layouts.** Repeatedly take the rightmost run of large columns
   that are not `primary`, up to three, and turn them into one stack (WPF's
   `L L L L -> L M M M`); then the `primary` ones, rightmost first (UX7 "the
   last to convert"). Each conversion is one layout. A conversion that leaves
   the layout unchanged is skipped.
3. **Last layout.** Collapsed, unless the group has a single command.

No layout ever hides a label, so there is no `L S S S` stage (section 4).

### 5.5 The reduction ladder

The ladder is the sequence of tab states from the widest to the narrowest, each
with its measured width. It is built once per cache key (5.7).

```text
state      = { layout index per group (0), gallery columns per group (layout 0's maximum) }
ladder     = [ (state, width(state)) ]
authored   = tab.order, consumed front to back (WPF/WRF order; not Fluent's)
cycle      = groups in visual order from the trailing edge (right to left in LTR,
             left to right in RTL), restarted at the trailing edge after the leading
             one (WPF's cyclical default)

next_step():
    if authored has an entry: return it
    for up to one full turn of `cycle`, starting after the group last reduced by the cycle:
        g = next group in the cycle
        if g has a gallery above its current layout's minimum columns: return gallery_column(g)
        if g's next layout exists (and is not a forbidden collapse): return layout(g)
    return none

apply(step):
    gallery_column: columns(g) -= 1
    layout:         layout(g) += 1; columns(g) = min(columns(g), new layout's maximum)
                    (a gallery entering a layout keeps as many columns as it may)

loop:
    step = next_step(); if none: stop
    apply(step); w = width(state)
    if w < ladder.last.width: append (state, w)
    else: replace ladder.last.state with state (the step is merged into the
          entry above it; counter `ribbon_nonshrinking_steps` += 1)
```

Properties the tests check:

- **Monotone.** `ladder[k].width` strictly decreases with `k`, so the entry for
  an available width is found by binary search.
- **Deterministic.** The ladder depends only on the definition, the authored
  order and measured widths, never on coordinates or the previous width (unlike
  WPF's incremental grow and shrink, whose result can depend on the resize
  history; Inferred from WPF-SRC's cached increase threshold).
- **Gallery first within a group's turn.** A group's gallery gives up its
  columns before the group changes layout, as WRF's per-size ranges imply and
  as `InRibbonGalleryAndButtons-GalleryScalesFirst` does (decision 8).
- **Authored steps first.** As WPF: the authored list, then the cycle over every
  group that can still shrink (groups the list already reduced continue from
  where they are).

### 5.6 Arrange, scrolling and the hide rule

```text
arrange(available_width, client_height):
    k = smallest index with ladder[k].width <= available_width   (binary search)
    if none: k = last; scrolling = true
    plane_height = height of the command plane at this text scale
    hidden = hide_rule(...)
```

**Scrolling.** When even the last entry is wider than the available width and
the plane is not hidden, scroll buttons appear at both ends (MFC, WRF); a click
scrolls by one group; the focused command, else the first group, is kept in
view. No partial group is shown under a scroll button.

*Superseded in part, 2026-10-14:* the owner replaced H1 with the document
area's own minimum and dropped H2 (RB decisions 34 to 36, after the
observation); the hysteresis stays.

**Hide rule (decision 7), content-derived.** The command plane (not the tab
row; decision 19, question 7.1) is hidden when either holds:

- **H1, height**: the workspace below the ribbon would be shorter than the
  plane itself: `client_height - tab_row_height - plane_height < plane_height *
  workspace_ratio`, with `workspace_ratio = 1.0` (question 7.4). This is WRF's
  "the ribbon obscures the application workspace" made measurable.
- **H2, width**: even with scrolling, less than one whole group fits:
  `available_width < widest collapsed-group button + 2 * scroll_button_width`.
  This is WRF's "all potential control layouts have been exhausted and the
  ribbon cannot be rendered" made measurable.

To avoid flicker at the threshold, a hidden plane shows again only when both
quantities clear their thresholds by `hide_hysteresis = 8` logical units
(question 7.4). The rule uses only measured content, so it moves with text
scale and theme metrics and needs no fixed window size; for comparison, WPF and
Fluent use 300 × 250 and Office 2007 about 250 wide (section 3.4).

### 5.7 Caching and invalidation

- **Ladder cache**, one per tab, keyed by: the tab's content generation (bumped
  by command text, visibility and definition changes; enabled and checked do not
  bump it, UX7), the font and text scale, the theme metrics generation, and the
  display scale (for rounding only). Each entry stores the per-group layout
  index and gallery columns (`std::uint8_t` and `std::uint16_t` per group; a tab
  of 12 groups and 60 steps is under 2 KiB) and the width.
- **Measure cache**: command label widths and two-line breaks per command, keyed
  like the ladder, so building the ladder measures each label once.
- **Resize** does no measuring: a binary search over the cached ladder and one
  arrangement. Counters: `ribbon_ladder_builds`, `ribbon_ladder_entries`,
  `ribbon_nonshrinking_steps`, `ribbon_resize_lookups`.
- Visibility changes of commands re-run the ladder (they change widths); UX7
  recommends disabling rather than hiding for this reason.

### 5.8 Focus and popups (from the first study, unchanged)

When the focused command's group becomes collapsed, focus moves to the group
button; when an open group popup's group grows back, the popup closes and focus
moves to the same command in the plane. The popup shows layout 0 (every source:
WRF-SCALE, WPF-LAYOUT, JH-SCALE, MFC-PANEL).

### 5.9 Golden-layout test ladder

Fixtures (C++ records, no host): **WordPad Home** (Clipboard: Paste large
primary, Cut, Copy small; Font: two hosted combos and a control group of eight
toggles; Paragraph: two control groups; Insert: Picture, Paint drawing, Date and
time, Insert object; Editing: Find, Replace, Select all), once with an authored
order copied from the observation (section 6) and once with none; **Gallery
tab** (one group with an in-ribbon gallery of 10 to 3 columns beside two
buttons, and a single-button group); **Long labels** (German-length labels).

Tests (all in `tests/`, on every CI platform):

1. **Ladder shape.** For each fixture and text scale: widths strictly decrease;
   the number of entries and each entry's per-group state equal a checked-in
   golden table.
2. **Boundaries.** For every entry k: at `width[k]` the arrangement is entry k;
   at `width[k] - 1` it is entry k + 1 (or scrolling after the last).
3. **Fixed ladder.** Golden part rectangles (group faces, captions, commands,
   collapsed buttons, scroll buttons) at widths 1600, 1400, 1200, 1000, 900, 800,
   700, 600, 500, 400, 300 and 200, at text scales 1.0 and 1.5 (and 2.25, the
   accessibility round's gate).
4. **Order.** The authored order is followed step by step; after it, the
   cycle; RTL mirrors the cycle; a gallery gives up one column per step before
   its group changes layout.
5. **Rules.** A single-command group never collapses; no label is ever hidden
   (every command's label part is present at every entry); a collapsed group's
   popup shows layout 0.
6. **Validation.** Each defect in 5.2 rejects the definition with its
   diagnostic and keeps the previous ribbon.
7. **Hide rule.** At the H1 and H2 thresholds: hidden at threshold − 1, shown at
   threshold + hysteresis, unchanged in between when coming from either side;
   the tab row present in all cases.
8. **Cache.** A resize sweep of 1,000 widths performs no label measurement and
   one ladder build; changing a command's text rebuilds once; changing enabled
   rebuilds nothing.
9. **Comparison with 2a.** Where the models agree (image-size and row
   transitions, popup order), the fixture's sequence of transitions equals the
   recorded WordPad sequence; label-drop transitions are listed as expected
   differences.

---

## 6. Observation protocol for the owner's machine

Decision 11: observing ribbons on the owner's Windows machine is allowed,
asking before each use. Nothing in this section has been run. Read-only checks
made for this protocol (2026-10-14, this machine, Windows 11 build 22621):
`C:\Program Files\Windows NT\Accessories\wordpad.exe` exists (file version
10.0.22621.1), `C:\Windows\System32\UIRibbon.dll` exists, and no Microsoft
Office folder exists under `Program Files`. WordPad is removed from Windows 11
24H2 and later (WIN-DEP), so this observation is possible only while the
machine stays on 22H2 or 23H2; it should be run before any feature update.

### 6.1 What is observed

1. **WordPad** (Windows Ribbon Framework), tabs Home and View. Required.
2. **An Office 2010-style ribbon**, only if the owner installs Office. Current
   Microsoft 365 builds have a later ribbon (simplified mode, search box);
   decision 10 asks for Office 2010 and earlier, so the version installed must be
   recorded and its differences noted. Optional.
3. Not observed: MFC and WPF ribbons (they would need a sample built or a
   program installed); their behaviour comes from documents and source (section
   2).

### 6.2 Ask before each use

Before each session the worker asks the owner, in chat, naming: the program to
start (WordPad, or Word), the script to run (6.4), the folder it writes to, and
the expected duration. The worker does not change display scale, text size,
theme or any other system setting; where a ladder needs one (6.3, steps 4 and
5), the owner changes it and says so in chat.

### 6.3 Width and height ladders

All sizes are window client sizes in physical pixels, recorded with the display
scale.

1. **Coarse width ladder**, Home tab, client height 600: widths from 1600 down
   to 200 in steps of 20.
2. **Transition search.** For each pair of neighbouring coarse widths whose
   ribbon state differs, bisect to the exact pixel where it changes, in both
   directions (shrinking and growing) to detect hysteresis.
3. **Height ladder** for the hide rule, client width 800: heights from 600 down
   to 120 in steps of 10, then bisect the hide transition; repeat at widths 400
   and 300.
4. **Display scale**, if the owner sets it: repeat 1 and 3 at 150 %.
5. **Text size**, if the owner sets it (Settings, Accessibility, Text size): repeat
   1 at 150 % text size with 100 % display scale.
6. **View tab**: step 1 only.

### 6.4 How it is captured, touching nothing else

One PowerShell 5.1 script (no download, no install), run from the worker's
scratchpad folder, writing only to a new folder
`results/2026-10-xx-ribbon-scaling-observation/` in the worker's own workspace
(not the owner's checkout):

1. Record the machine: `winver` build, `wordpad.exe` version, display scale
   (`GetDpiForWindow`), text scale, theme (light or dark), monitor size.
2. Start WordPad with a new empty document (`Start-Process wordpad.exe`, no
   file argument) and remember its process id; act only on windows of that
   process.
3. Read WordPad's current window placement first (`GetWindowPlacement`) so it can
   be restored before closing; WordPad keeps its window placement and ribbon
   state between runs (Unverified where; the ribbon's minimized state and Quick
   Access Toolbar are persisted through `SaveSettingsToStream` in WRF
   applications, WRF-STATE), so the script never minimizes the ribbon, never
   touches the Quick Access Toolbar, and restores the original placement.
4. For each ladder size: `SetWindowPos` on WordPad's top-level window (sizes via
   `AdjustWindowRectExForDpi` so the *client* size is exact), wait for two idle
   frames (`WaitForInputIdle` and a 200 ms pause), then record:
   - **a UI Automation snapshot** of the ribbon subtree (`UIAutomationClient`
     from the .NET Framework that ships with Windows): every group and control
     with name, control type, bounding rectangle, `IsOffscreen`, and whether a
     group is a single collapsed button (control type and the group's child
     count);
   - **a screenshot of WordPad's window only** (`PrintWindow` into a bitmap,
     saved as PNG), so label visibility and image size can be read;
   - whether the ribbon is hidden (the ribbon's UIA element absent or
     zero-height) and whether scroll buttons are present.
5. Restore the window placement, close WordPad with `WM_CLOSE`, answer "Don't
   Save" if asked (the document was never edited), and verify the process has
   exited.

The script sends no input to any other window, reads no other process, and
moves no other window. Screenshots contain only WordPad's window. If the owner
prefers, each run can be done by the owner with the worker reading the output
afterwards.

### 6.5 What is recorded per size (one row in a CSV, plus the PNG)

`program, version, tab, display_scale, text_scale, client_width, client_height,
direction (shrinking or growing), ribbon_hidden, scroll_buttons, then per group:
group_name, state (large, medium, small, popup: derived from control image
sizes and the collapsed button), group_width, per control: name, image size
(16 or 32 at 100 %), label visible (from the screenshot), row, column`.

From the rows the study derives, for each tab: the ordered list of transitions
(which group changed to which state at which width), the gallery column steps
(if any), the hide thresholds for width and height, whether hysteresis exists,
how the transitions move at 150 % display scale and at 150 % text size, and
which transitions drop labels (section 4).

### 6.6 Report

`results/2026-10-xx-ribbon-scaling-observation/README.md`: the machine, builds
and settings; the commands run; the transition tables; the derived authored
order for the WordPad Home fixture (5.9); differences from the documents in
section 2; and every Unverified item of this study the observation settles.

---

## 7. Questions for the owner

Each with options and a recommendation.

**7.1 When the window is too small, what hides?**
1. The command plane only; the tab row stays (PLAN phase 2b, decision 19).
2. The whole ribbon, tab row included (every canonical ribbon).

Recommendation: 1, as planned; it keeps the tabs usable as the way back.

**7.2 An authored reduction order names an unknown group or over-shrinks one.**
1. Reject the ribbon definition with a diagnostic (house style §9).
2. Ignore the bad entries (WPF).

Recommendation: 1.

**7.3 Decision 6 and authored icon-only commands.** Office's Font group shows
Bold, Italic and alignment as icons only at every size (UX7 "toolbar-style
groups"); WRF truncates tab labels when the tab row is narrow.
1. Decision 6 forbids dropping labels while scaling only; an application may
   still author icon-only commands in a toolbar-style control group, and tab
   labels may truncate.
2. Every command has a visible label at every size, and tab labels never
   truncate (the tab row scrolls instead).
3. Every command has a visible label; tab labels may truncate.

Recommendation: 1; it keeps the familiar Font group possible while scaling never
removes a label.

**7.4 Hide-rule parameters.**
1. H1 with the workspace at least as tall as the plane (`workspace_ratio` 1.0),
   H2 with one collapsed group plus scroll buttons, 8-unit hysteresis.
2. As 1 but `workspace_ratio` 0.5.
3. Only H1 (never hide for width; scroll instead).

Recommendation: 1.

**7.5 The default order's shape.**
1. Cyclic: one step per group per turn, right to left (WPF).
2. Whole group first: the rightmost group goes all the way to its button before
   the next starts (one reading of Office 2010 `autoScale`).

Recommendation: 1; it keeps more groups usable for longer, and the observation
can show which one Office used.

**7.6 Observation.** May the worker run the PowerShell capture script of 6.4
on this machine (asking before each session), and will the owner set display
scale 150 % and text size 150 % for the extra ladders? Should it run before
any Windows feature update, since 24H2 removes WordPad?
