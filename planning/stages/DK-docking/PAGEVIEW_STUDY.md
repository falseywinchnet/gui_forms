# A page view family for GUI.Forms: study

Date: 2026-10-14. Status: research study for stage DK
(`planning/stages/DK-docking/README.md`, section "PageView inside this
stage"), not a decision record. It is the tab section of the docking study
(`DOCKING_STUDY.md`, written beside it) and its questions join the docking
interview. Nothing here is decided until the owner answers section 7.

Reading key. **Known** means read in a cited source (a vendor page,
published source code, a file in the repository, an interview record).
**Inferred** means reasoned from known material but not stated by any
source. **Unverified** means remembered or reported second hand and not
checked against a primary source in this study. Repository files were read
in the checkout `D:\gui_forms_main\gui_forms` at commit `4a371ed9` (branch
`claude/advanced-candidates`, which carries the ribbon line); paths below are
relative to the repository root. Interview and verdict IDs are the owner's.
Docking questions are cited as "DK H4", "DK J3" and so on.

Contents

1. What a page view is: modes, shared concepts, comparison
2. What GUI.Forms has today, and the gaps
3. Proposed model and anatomy
4. What docking needs from it, stated as interfaces
5. Interaction details
6. Phased build plan
7. Open owner decisions, as interview questions

Sources used, by short name:

| Name | Source |
|---|---|
| TPV | Telerik RadPageView for WinForms: overview https://www.telerik.com/products/winforms/documentation/controls/pageview/overview ; architecture `/pageview/architecture`; strip buttons `/pageview/stripview/scrolling-and-overflow--strip-buttons-`; strip element properties `/pageview/stripview/strip-element-properties`; fitting items `/pageview/stripview/fitting-items`; stack `/pageview/stackview/overview`; Outlook `/pageview/outlookview/overview` and `/pageview/outlookview/using-the-overflow-grip`; explorer bar `/pageview/explorerbarview/overview` (all under https://www.telerik.com/products/winforms/documentation/controls) |
| TPV-API | `RadPageView` API reference, https://www.telerik.com/products/winforms/documentation/api/telerik.wincontrols.ui.radpageview |
| TDOCK | RadDock tab strips: https://docs.telerik.com/devtools/winforms/api/telerik.wincontrols.ui.docking.docktabstripneededeventargs ; `DocumentStripButtons` https://www.telerik.com/products/winforms/documentation/api/telerik.wincontrols.ui.docking.documentstripbuttons ; feedback item 1368933 https://feedback.telerik.com/winforms/1368933-add-raddock-add-support-for-pinning-a-documentwindow |
| DXTAB | DevExpress `XtraTabControl` https://docs.devexpress.com/WindowsForms/DevExpress.XtraTab.XtraTabControl ; `ClosePageButtonShowMode` `/WindowsForms/DevExpress.XtraTab.ClosePageButtonShowMode`; `PinPageButtonShowMode` `/WindowsForms/DevExpress.XtraTab.PinPageButtonShowMode`; `TabMiddleClickFiringMode` `/WindowsForms/DevExpress.XtraTab.TabMiddleClickFiringMode` |
| DXDOC | DevExpress `TabbedView` https://docs.devexpress.com/WindowsForms/DevExpress.XtraBars.Docking2010.Views.Tabbed.TabbedView and `IDocumentGroupProperties` properties `/WindowsForms/DevExpress.XtraBars.Docking2010.Views.Tabbed.IDocumentGroupProperties._properties` |
| DXNAV | DevExpress `NavigationPane` https://docs.devexpress.com/WindowsForms/DevExpress.XtraBars.Navigation.NavigationPane ; NavBarControl `OptionsNavPane` properties https://docs.devexpress.com/WindowsForms/DevExpress.XtraNavBar.OptionsNavPane._properties |
| IGTAB | Infragistics `TabLayoutStyle` https://www.infragistics.co.kr/help/winforms/Infragistics.Win~Infragistics.Win.UltraWinTabs.TabLayoutStyle.html |
| IGEXP | Infragistics `UltraExplorerBar.Style` https://www.infragistics.com/help/winforms/infragistics.win.ultrawinexplorerbar~infragistics.win.ultrawinexplorerbar.ultraexplorerbar~style and `NavigationMaxGroupHeaders` https://infragistics.co.kr/help/winforms/Infragistics.Win.UltraWinExplorerBar~Infragistics.Win.UltraWinExplorerBar.UltraExplorerBar~NavigationMaxGroupHeaders |
| VSTAB | Visual Studio, "Customize and save layouts of windows and tabs", https://learn.microsoft.com/en-us/visualstudio/ide/customizing-window-layouts-in-visual-studio (updated 2026-10-13); blog posts https://devblogs.microsoft.com/visualstudio/doc-management-personalization/ and https://devblogs.microsoft.com/visualstudio/too-many-tabs-open-no-problem/ |
| OL | Outlook 2007 Navigation Pane, https://www.dotnetcurry.com/ShowArticle.aspx?ID=315 (no primary Microsoft page for Outlook 2010's pane was found; see 1.2) |
| AD | AvalonDock `DocumentPaneTabPanel.cs`, https://raw.githubusercontent.com/Dirkster99/AvalonDock/master/source/Components/AvalonDock/Controls/DocumentPaneTabPanel.cs ; other AvalonDock facts through the docking study §1.3 |
| WF | .NET `TabControl`: `Alignment` remarks https://learn.microsoft.com/en-us/dotnet/api/system.windows.forms.tabcontrol.alignment?view=windowsdesktop-8.0 ; `TabSizeMode` `/dotnet/api/system.windows.forms.tabsizemode`; `Selecting` event order `/dotnet/api/system.windows.forms.tabcontrol.selecting`; source https://github.com/dotnet/winforms/blob/main/src/System.Windows.Forms/System/Windows/Forms/Controls/TabControl/TabControl.cs |
| W32TAB | Win32 tab control styles, https://learn.microsoft.com/en-us/windows/win32/controls/tab-control-styles |
| APG | WAI-ARIA Authoring Practices, Tabs pattern, https://www.w3.org/WAI/ARIA/apg/patterns/tabs/ |
| UIA-TAB | UI Automation TabItem control type, https://learn.microsoft.com/en-us/windows/win32/winauto/uiauto-supporttabitemcontroltype |
| DOCK | `D:\gui_forms_workers\DOCKING_STUDY.md` (sections and questions cited by number) |
| SPEC | `planning/DESIGN_LANGUAGE_SPECIFICATION.md` (§8, §12.7, §12.8) |
| RB | `planning/stages/RB-ribbon/README.md` decisions (RB-n) |
| MATRIX | `planning/CONTROL_COMPLETENESS_MATRIX.md` rows 43 and 51 |

---

## 1. What a page view is

**Status of this section.** Vendor facts are Known from the pages in the
source table, read for this study (October 2026), unless marked otherwise.

### 1.1 The owner's admission and what it covers

The owner (2026-10-14): a `PageView` family, the equivalent of Telerik's
RadPageView, "with multiple tab presentation modes, including document
tabs, navigation strips, and Outlook-style navigation. It's a useful
foundation for several other controls." MATRIX row 51 records it as "one
page collection and selection model presented as strip tabs, document tabs
(close, pin, modified mark, overflow), a navigation strip and an
Outlook-style navigation pane", and sets two limits (Known): it is "not a
replacement for `TabControl`, which keeps its Windows Forms surface", and
"Backstage stays the ribbon's (RB decision 26)". MATRIX row 43 names
`PageView` as what docking uses "for tab stacks, the document area and
auto-hide strips".

### 1.2 The canonical systems

**Telerik RadPageView (WinForms).** One control with a `ViewMode`
(`PageViewMode`) and seven modes (TPV): Strip (the successor of RadTabStrip),
Stack (RadPanelBar's list bar), Outlook (RadPanelBar's Outlook navigation
pane), ExplorerBar ("multiple visible pages at once"), Backstage (Office
2010), Navigation ("changes strip width based on available space") and
OfficeNavigationBar (Outlook 2013 to 2019). The architecture page says the
control "consists of pages, each with associated items", arranged by a
per-mode layout strategy. Events (TPV-API): `SelectedPageChanging`
(cancelable), `SelectedPageChanged`, `PageAdding`/`PageAdded`,
`PageRemoving`/`PageRemoved`, `PageIndexChanging`/`PageIndexChanged`,
`PagesClearing`/`PagesCleared`, `NewPageRequested`, `ItemCreating`,
`ItemDropping`/`ItemDropped`, `ItemListMenuDisplaying` (cancelable) and
`ItemListMenuDisplayed`, `PeekPopupOpening`/`PeekPopupClosed`,
`ViewModeChanging`/`ViewModeChanged`, and for ExplorerBar
`PageExpanding`/`PageExpanded`/`PageCollapsing`/`PageCollapsed`.

Strip mode (TPV): `StripAlignment` (Top, Bottom, Left, Right);
`ItemAlignment` (Near, Center, Far); `StripButtons` of type
`StripViewButtons` with `LeftScroll`, `RightScroll`, `Scroll`, `Close`,
`ItemList` (a drop-down listing the items), `VS2005Style` (scroll and
close), `VS2008Style` (item list and close), `All` and `None`;
`ShowItemCloseButton`, `ShowItemPinButton`, `PreviewItem` ("the item opened
for preview, placed after the regular items"), `NewItemVisibility`,
`AnimatedStripScrolling`; `ItemFitMode` of type `StripViewItemFitMode`:
`None`, `Shrink`, `Fill`, `ShrinkAndFill`, `FillHeight`, `MultiLine`, with
`MultiLineItemFitMode` `None` (rows set per item) or `Reflow`.

Stack mode: items stacked beside or below the content, four positions,
three selection types (two named: standard and `ContentWithSelected`).
Outlook mode "extends Stack mode with an overflow grip and an overflow
panel": dragging the grip moves items between the stack and a strip of
buttons below it; API `DragGripDown()`, `DragGripUp()`, `HideItems(int)`,
`ShowItems(int)`; events `ItemCollapsed`, `ItemShown`,
`ItemAssociatedButtonClicked`; an overflow menu where "users choose which
items are permanently hidden". ExplorerBar: several pages expanded at once,
three content sizing types (fixed length, auto size to best fit, equal
length), stacked at the top or left.

**Telerik RadDock document tabs.** RadDock's `DocumentTabStrip` and
`ToolTabStrip` expose `ShowItemCloseButton` and `ShowItemPinButton`, the
same members as `RadPageViewStripElement` (TDOCK; pin only on document
strips); `DocumentStripButtons` has close, active-window-list and
system-menu flags. That RadDock's strips are built on RadPageView's strip
element is Inferred from the shared member names and the feedback item's
use of `RadPageViewStripElement`; it is the reuse the owner asks for here.

**DevExpress.** `XtraTabControl` (DXTAB): `HeaderLocation`,
`HeaderOrientation`, `MultiLine`, `HeaderButtons` (Close, Next, Prev),
`HeaderButtonsShowMode`; cancel and change argument types
(`TabPageCancelEventArgs`, `TabPageChangingEventArgs`). `ClosePageButtonShowMode`
has seven values: `Default`, `InTabControlHeader`, `InAllTabPageHeaders`,
`InActiveTabPageHeader`, `InAllTabPagesAndTabControlHeader`,
`InActiveTabPageAndTabControlHeader`, `InActiveTabPageHeaderAndOnMouseHover`
(the `TabbedView` default). `PinPageButtonShowMode`: `Default` (as the hover
mode), `InAllTabPageHeaders`, `InActiveTabPageHeader`,
`InActiveTabPageHeaderAndOnMouseHover`. `TabMiddleClickFiringMode`:
`Default`, `MouseDown`, `MouseUp`, `None`. The document manager's
`TabbedView` (DXDOC) splits the area into document groups; group properties
include `ClosePageButtonShowMode`, `CloseTabOnMiddleClick`,
`CustomHeaderButtons`, `HeaderAutoFill`, `HeaderButtons`,
`HeaderButtonsShowMode`, `HeaderLocation`, `HeaderOrientation`,
`MaxDocuments`, `MultiLine`, `PageImagePosition`, `PinPageButtonShowMode`,
`ShowDocumentSelectorButton`, `ShowTabHeader`. `NavigationPane` (DXNAV):
"vertically arranged built-in flat tabs" whose content area can be resized,
maximized and collapsed; `ItemOrientation` for headers; `PageProperties.ShowMode`
for image, text or both; `RegularSize`, `AllowResize`, `State`. The
NavBarControl's Outlook-style paint style (`OptionsNavPane`, DXNAV):
`NavPaneState`, `ExpandedWidth`, `CollapsedWidth`, `ShowExpandButton`,
`ExpandButtonMode`, `ShowSplitter`, `ShowOverflowPanel`,
`ShowOverflowButton`, `AllowOptionsMenuItem` ("the Navigation Pane Options
dialog"), `GroupImageShowMode`, `ShowHeaderText`, `MaxPopupFormWidth` and
`PopupFormSize` (a popup shown when the active group is clicked in the
collapsed control), `CollapsedNavPaneContentControl`.

**Infragistics.** `UltraTabControl` layout styles (IGTAB):
`SingleRowSizeToFit` (longest tabs shrink first, spare space shared),
`MultiRowAutoSize` (rows added; past `MaxVisibleTabRows` scroll buttons
scroll rows), `MultiRowSizeToFit`, `MultiRowFixed` (`TabSize`),
`SingleRowTabsPerRow` (`TabsPerRow`), `SingleRowJustified`; properties
`CloseButtonLocation` and `ShowTabListButton` exist (behaviour not read,
Unverified). `UltraExplorerBar.Style` (IGEXP) has five styles, among them
`Listbar`, `Toolbox`, `OutlookNavigationPane` and a Visual Studio 2005
toolbox; in the Outlook style one group shows its items at a time, groups
can be hidden or collapsed to an icon, `NavigationMaxGroupHeaders` (−1:
as many as fit) and `NavigationCurrentGroupAreaMinHeight` decide how many
headers show, and the user may drag the splitter for more.

**Visual Studio document well** (VSTAB, Known). Documents are dragged by
their tab; Ctrl+double-click toggles float. A **preview tab** shows a file
without opening it, "in a tab on the right side of the document tab well";
it opens for editing when modified or opened explicitly. Documents can be
"pin[ned] to their current position in the tab order". **Multiple rows**
("Show tabs in multiple rows"): "Pinned tabs show up in a separate row from
unpinned tabs." **Tab layout** Top, Left or Right (vertical tabs, a list
beside the editor, with horizontal text). Tabs colourized by project, file
extension or regular expression, or by the user ("Set Tab Color"). Options
for bold active tab, tab width, an extra close button. **Dirty indicator**:
a larger dot or the asterisk. The overflow drop-down can show "invisible
tabs in italic". Restore Closed Tab: Ctrl+K, Ctrl+Z. From VS 2026 18.11:
Ctrl+NumPad1..9 activates the nth visible tab ("Preview tabs don't consume a
shortcut slot"); Ctrl+Shift+PgUp/PgDn moves the active tab, "Movement
preserves the boundary between pinned and unpinned tabs", "Preview tabs
can't be moved". The chevron at the right of the well lists every open
document (blog). Whether a document chosen from that list moves to the
front of the strip is Unverified.

**AvalonDock** (AD, Known from source). `DocumentPaneTabPanel` lays tabs out
left to right and hides every tab from the first one that does not fit
(`skipAllOthers`), even if later ones would fit; if the selected tab is
hidden and not first, it **moves the document to index 0** in the model
and re-arranges. From the docking study §1.3: a tab drag reorders live
inside its strip and tears off when the pointer leaves it.

**DockPanel Suite** (DOCK §1.2): no tab reordering within a strip; middle
click closes a tab; double-click toggles float.

**Outlook navigation pane** (OL, Known for Outlook 2007). Large view
buttons below the folder area; dragging the splitter collapses buttons into
icons at the bottom; a small drop-down ("Configure Buttons") with Add or
Remove Buttons, Show More Buttons, Show Fewer Buttons and Navigation Pane
Options (a dialog to hide and reorder buttons with Move Up, Move Down and
Reset); the pane's width is dragged at its right border; it can be
minimized, and the minimized state persists. Outlook 2010's minimized pane
shows the current folder's name as vertical text and opens the folder pane
as a temporary flyout when clicked (Unverified; consistent with DXNAV's
collapsed popup, Known). Outlook keyboard shortcuts (Ctrl+1 and so on) were
not read (Unverified).

**Windows Forms `TabControl`** (.NET 8, WF; Known). `Alignment` (Top,
Bottom, Left, Right); setting Left or Right forces `Multiline` true;
`FlatButtons` "only appears as such" with Top alignment, and Buttons needs
Top to display correctly. `SizeMode`: `Normal` (sized to content, rows not
stretched), `FillToRight` (rows fill the width, only with more than one
row), `Fixed` (equal width). Events on a change of tab, in order:
`Deselecting`, `Deselected`, `Selecting`, `Selected` (the -ing events
cancellable). Source: `OnKeyDown` handles Ctrl+Tab, Ctrl+Shift+Tab,
Ctrl+PageDown and Ctrl+PageUp; `PrefixAmpersands` doubles every `&` in tab
text, because comctl32 would underline the next character without making it
work, so Windows Forms tab text has **no mnemonics**. `ShowToolTips` shows
`TabPage.ToolTipText`. `DrawMode.OwnerDrawFixed` and `HotTrack` exist
(catalogue rows in `planning/generated/WINFORMS_API_CATALOGUE.tsv`).
Win32 underneath (W32TAB): single-line tabs scroll; `TCS_MULTILINE` shows
rows "so all tabs are visible at once"; `TCS_SCROLLOPPOSITE` sends unneeded
tabs to the opposite side on selection; `TCS_VERTICAL` rotates text and is
"not supported" with ComCtl32 6; `TCS_FOCUSNEVER` and
`TCS_FOCUSONBUTTONDOWN` choose whether a click focuses the control. The
multi-row "row rotation" (the selected tab's row moves next to the page) is
widely reported but was not read in a primary source (Unverified).

**Accessibility patterns.** APG (Known): `tablist`, `tab`, `tabpanel`;
`aria-selected`, `aria-controls` (tab to panel), `aria-labelledby` (panel to
tab), `aria-orientation` for vertical lists. Keys: Tab enters at the
active tab and leaves to the panel; arrows move (wrapping); Home and End;
Space or Enter activates under manual activation; Shift+F10 opens a tab's
menu; Delete, optional, closes a tab and focuses the following tab, or the
preceding one. Automatic activation (select on focus) is recommended only
"when panels show without noticeable delay". UIA-TAB (Known): a TabItem's
control view holds Image, Text and a Pane with the page's controls;
`ControllerFor` may point to the pane when it cannot be a child;
`SelectionItem` is required and **Invoke is "Never"** supported.

### 1.3 Shared concepts

Every mode decomposes into the same concepts (Inferred from 1.2):

1. **Page model.** An ordered collection of pages, each with a stable
   identity, text, tooltip, image, enabled and visible flags and a content
   control (created eagerly or on first selection). Document pages add
   closable, modified, pinned and preview flags.
2. **Header item.** The thing the user clicks: a tab, a stack button, an
   overflow icon, a side tab. One page may have several items at once (an
   Outlook page is a large button or an overflow icon, and an entry in the
   configure menu).
3. **Header layout.** Where items go (edge, row or column, order with
   pinned first and preview last), how they are sized (content, fixed,
   fill, shrink) and what happens when they do not fit.
4. **Content area.** One selected page shown (strip, stack, Outlook), or
   several (explorer bar), or none (an auto-hide strip whose content is a
   flyout owned by someone else).
5. **Overflow.** Scroll, a list drop-down, shrink, multiple rows, or (Outlook)
   demotion to a smaller presentation with a grip deciding how many.
6. **Affordances.** Close, pin, modified mark, new page, strip-level
   buttons; each with a visibility rule (always, selected, hot).
7. **Selection versus activation.** Selection is which page is shown in this
   view; activation is which page the application considers current (the
   docking workspace's active panel, DOCK §5.6). A click selects; whether it
   also activates, and where focus goes, is a policy.
8. **Keyboard model.** Arrows and Home/End inside the strip; Ctrl+Tab or
   Ctrl+PageDown from anywhere inside the view; close and move keys;
   mnemonics.
9. **Drag hooks.** Reorder inside the strip; hand-off to docking (tear-off,
   insertion into another strip); a drop insertion mark.

### 1.4 Comparison

| Concept | WinForms `TabControl` | Telerik page view | DevExpress | Infragistics | VS document well | AvalonDock | Outlook pane |
|---|---|---|---|---|---|---|---|
| Modes | tabs, buttons, flat buttons | 7 modes in one control | separate controls (tab control, tabbed view, navigation pane, nav bar) | tab control; explorer bar with 5 styles | document tabs only | document and tool panes | one |
| Selection events | 4, two cancellable | changing (cancel), changed | changing, changed (argument types) | Unverified | n/a | Unverified | n/a |
| Close | none | per item and strip button | 7 visibility modes | close button location | per tab, extra close option | per tab | n/a |
| Pin | none | per item (`ShowItemPinButton`) | 4 visibility modes, `Document.Pinned` | Unverified | pinned position, own row when multi-row | Unverified | n/a |
| Modified | none | none read | none read | none read | dot or asterisk | Unverified | n/a |
| Preview | none | `PreviewItem` after the others | none read | none read | right side, promoted on edit | Unverified | n/a |
| Overflow | scroll (single row) or rows | scroll, item list, shrink, fill, multi-line | prev/next, multi-line, document selector | size-to-fit, rows, tab list | list drop-down (chevron), rows | hide the rest, selected moves to first | grip demotes to icon strip |
| Side placement | left/right force multi-line | strip alignment 4 sides | header location and orientation | Unverified | Top, Left, Right lists | Unverified | vertical caption when minimized (Unverified) |
| Middle click | none | Unverified | event, fired on down or up | Unverified | Unverified | Unverified | n/a |
| Reorder | none | `ItemDropping`/`ItemDropped` | drag in tabbed view | Unverified | drag; Ctrl+Shift+PgUp/PgDn | live drag | options dialog Move Up/Down |
| Keyboard | Ctrl+Tab, Ctrl+PgDn | Unverified | Ctrl+Tab document selector | Unverified | Ctrl+NumPad1..9, navigator | Ctrl+Tab navigator | Unverified |
| Mnemonics | none (`&` doubled) | Unverified | Unverified | Unverified | n/a | n/a | n/a |

---

## 2. What GUI.Forms has today, and the gaps

### 2.1 Existing pieces (Known, read in the repository)

| Piece | Where | What it gives a page view |
|---|---|---|
| `TabControl`, `TabPage` | `include/gui_forms/controls/scrollable_control/container_control/tab_control/tab_control.hpp`, `.../tab_page/tab_page.hpp`; source `src/controls/scrollable_control/container_control/tab_control/tab_control.cpp` | Pages as child `TabPage` panels held by weak pointers; selection by index or page; `TabAlignment` (top, bottom, left, right); `TabAppearance` (normal, buttons, flat buttons); one fixed `item_size` (default 120 × 30, minimum 24 × 18); per-page focus memory (`remember_page_focus`, `restore_page_focus`, lines 131–160); Ctrl+Tab or Meta+Tab in key preview (409–422); arrows, Home, End when focused (423–447); one event `selected_index_changed` with optional old and new indices; semantics `tab_group` with `tab` virtual children (456–508); anatomy parts `face`, `display`, item system `item` with `tab`, `tab_label`, `tab_focus`, `tab_seam` and state `selected` (513–555). |
| Ribbon tab row | `src/controls/ribbon/ribbon.cpp:810–824`, anatomy 1477–1575 | Tabs painted through `paint_part` with the `menu_item` family, "flat at rest, a plate when hot or selected … until T4 draws the tab stack"; labels through `parse_mnemonic_text`; item system `tab` with `tab`, `tab_label`, `tab_focus` and states hot, selected, focus_visible. |
| Strip overflow | `src/controls/tool_strip/tool_strip.cpp:482–498, 697–701` | Overflow from the last item back with a reserved button painted with `PartMark::overflow` (admitted RB-20). |
| Mnemonics | `include/gui_forms/control/control/control.hpp:227–238, 649, 777–779` | `MnemonicText`, `parse_mnemonic_text`, `process_mnemonic`, `mnemonic_matches`, `process_mnemonic_self`. |
| F6 regions | `include/gui_forms/window/window.hpp:474–487` | `set_focus_regions`, `add_focus_region`, `move_focus_region` (RB-17). |
| Paint path and atoms | `include/gui_forms/theme/part_painting/part_painting.hpp`, `include/gui_forms/anatomy/types/anatomy_types.hpp` | `paint_part` with role, bounds, states, family, mark, text, image, accent; 31 part roles; 31 states; at most 32 parts, 4 item systems and 16 custom states per control (`anatomy_types.hpp:113–115`); item part names unique across the control (`anatomy_types.hpp:230–234`). |
| Marks | `part_painting.hpp:33–52`; SPEC §8.5 | `arrow_*`, `chevron_down`, `chevron_right`, `close`, `grip_dots`, `overflow`, `customize`, `more`, `dialog_launcher`, `radio_dot` and the indicators. No pin, no plus, no `chevron_left`/`chevron_up` (RB-20 declined `chevron_up`). |
| Semantics | `include/gui_forms/semantics/types/semantic_types.hpp:17–25, 61–64, 104` | Roles include `tab_group`, `tab`, `group`, `list`, `list_item`, `split_pane`, `menu_button`; no tab panel, tree or pane role. Actions `focus`, `press`, `select`, `expand`, `collapse`, `show_menu`, `set_value`; no `close`. Nodes carry position in set, set size, has-popup (RB-22) and nested `children`. |
| Cancellable events | e.g. `src/controls/data_grid_view/data_grid_view_parts.cpp:936`; `include/gui_forms/control/control/control.hpp:214` | Mutable payloads with a `cancel` field are an established pattern. |

### 2.2 Gaps in `TabControl` (Known from the source, with line numbers)

1. **Painting bypasses the by-role path.** `on_paint` calls `fill_rect`,
   `stroke_rect` and `draw_text_utf8` with `BasicControlStyle` colours
   (lines 313–373). Under SPEC §12.7 a new control may not do this; an old one
   is not yet moved.
2. **Label width is estimated**, `text().size() * font.size * 0.55`
   (lines 359–360 and 609), so labels are centred and published by byte
   count, not measured text. Non-Latin and proportional text misplace.
3. **One fixed item size** for every tab (`tab_bounds`, 231–247); no
   content sizing, no `SizeMode`, no rows, no overflow: tabs past the edge
   are cut off. W's backlog lists "`TabControl` overflow, images and owner
   draw" (`planning/stages/W-winforms-controls/README.md:67`, P3).
4. **Repeated allocation per tab.** `tab_bounds` calls `page_count()`, which
   rebuilds the live page vector (`pages()`, 36–46); `on_paint` and the
   item publishers call `tab_bounds` once per tab, so painting n tabs builds
   n + 1 vectors (house style §3: no per-element allocation in a repeated
   kernel).
5. **No hot state, images, tooltips, disabled pages or hidden pages**; the
   item system declares only `selected` (531–533) though
   `TabAppearance::flat_buttons` implies a hover look.
6. **No cancellable selection.** `set_selected_index` changes and then
   publishes (162–188).
7. **Identity by index.** `TabSelectionChange` carries indices; a page's
   identity is its `TabPage` stable ID plus ".tab" (475, 591).
8. **Ctrl+Tab always consumed** in key preview, including Meta+Tab, and no
   Ctrl+PageDown/PageUp (409–422); the docking study's G-INPUT-4 and DK G1.
9. **No mnemonics** (no `process_mnemonic_self` override); Windows Forms
   has none either (WF), but the ribbon parses them (RB-16 keyboard model:
   "shortcuts, mnemonics, and tab, arrow navigation").
10. **Semantics.** Tabs publish `press` (485–486), which UIA-TAB says a tab
    item never supports; the page publishes `group`
    (`tab_page.cpp:33`), not a tab panel; no tab-to-panel relation; no
    position in set (admitted RB-22, not yet used here).
11. **Nothing for documents or docking**: close, pin, modified, preview,
    drag, insertion mark, context menu, double-click, lazy content.

### 2.3 Gaps for the family as a whole

| Need | Exists | Gap |
|---|---|---|
| Stable page identity separate from index | no | `PageId` wrapper (house style §1) |
| Shared page model usable by several presenters | no | `PageSet` (3.2) |
| Headers without a content area (auto-hide) | no | `PageStrip` (3.3) |
| Vertical text for side strips and the collapsed Outlook caption | no; `PartPaint` has no orientation | vocabulary decision (DK J4; P35.3) |
| Pin mark | no | vocabulary decision (DK J1; P35.1) |
| Modified display | no | custom item state or vocabulary (DK J3; P35.2) |
| Tab panel role, tab-to-panel relation, close action | no | vocabulary decisions with stage A (P35.4–6; DK J6) |
| Outline (tree) role, if an explorer-bar mode is admitted | no; `TreeView` publishes `list`/`list_item` (`src/controls/panel/tree_view/tree_view.cpp:444, 464`) | vocabulary decision (P35.7) |
| A tab paint family | ribbon uses `menu_item` | owner decision until T4 (P35.9) |

---

## 3. Proposed model and anatomy

Everything here is a proposal for the interview. Every vocabulary gap is
marked **[vocabulary decision]** with the nearest existing atom a builder
would use meanwhile (the W brief's rule).

### 3.1 Why separate controls rather than one control with a mode

Telerik puts seven modes in one control (TPV). In GUI.Forms a control type
has one static anatomy (`ControlAnatomy` is a `constexpr` table per type,
`anatomy_types.hpp:246–257`), limited to 32 parts and 4 item systems. The
union of the strip's, the Outlook pane's and the explorer bar's parts and
item kinds exceeds four item systems (tab; Outlook large button; Outlook
overflow icon; explorer group header; explorer group item), and a theme
rule written for `page_view::part(tab)` would silently mean different
things per mode (Inferred). DevExpress ships separate controls for the same
reason of shape (DXTAB, DXDOC, DXNAV). The proposal: **one model, several
presenter types**, with "modes" inside one presenter only where the anatomy
is the same (strip and document tabs).

### 3.2 `PageSet`: the page collection and selection model

A renderer-free record set (in `GUIForms::Controls`, no host), owned by its
presenter or handed between presenters by explicit transfer. It is tested
headless.

```text
PageId              wrapper over the application's stable string
                    (docking: the PanelId, DOCK §4.2 rule 5)
PageRecord
  id                PageId
  text              mnemonic text (& marks the mnemonic, && a literal &)
  tooltip, image    ImageId; tooltip defaults to the full text
  enabled, visible  a hidden page keeps its place and selection history
  closable          shows the close affordance and accepts close requests
  modified          document dirty state, set by the application
  pinned            kept in the pinned group at the start
  preview           at most one per set, kept after the others
  accent            optional AccentIndex (VS colour-coded tabs; RB-22 value)
  content           the page's control, or none until first selected
PageSet
  order             pinned pages, then ordinary pages, then the preview page
  selected          optional PageId
  history           selection history, most recent first (for close fallback)
  edits             insert, remove, move, take (transfer out with content),
                    put (transfer in), set flags, select, batch
```

Rules (Inferred; each a test):

1. Identity, not index, is the key. Indices are positions in the current
   order and appear only as `std::size_t` positions in payloads named as
   such.
2. The order invariant holds after every edit: pinned, ordinary, preview.
   Pinning moves a page to the end of the pinned group; unpinning to the
   start of the ordinary group (VS keeps the boundary, VSTAB).
3. Selecting a hidden or disabled page is refused (validate before mutate).
4. Removing or taking the selected page selects by the fallback rule (P10)
   in the same edit; one change event results.
5. A batch applies several edits and publishes one `pages_changed` and at
   most one `selected_page_changed` at its end (DOCK §5.3 step 8: one
   coherent update).
6. `take` returns the record and the owning pointer to its content without
   destroying it; `put` accepts both. This is how docking moves a document
   between stacks (DOCK §5.3 step 5: "no content is destroyed and
   re-created by a move").

### 3.3 Presenter types

| Type | Control id | Holds | Content area | Used by |
|---|---|---|---|---|
| `PageStrip` | `page_strip` | headers of one `PageSet` (borrowed from its owner) | none | `PageView`; docking's auto-hide strips; any application strip |
| `PageView` | `page_view` | owns a `PageSet`, a child `PageStrip` and the selected page's content | one page | standalone tabs; document tabs (preset); navigation strip (preset, P4); docking tool and document stacks |
| `NavigationPane` | `navigation_pane` | owns a `PageSet` | one page | Outlook-style navigation |
| (not proposed now) explorer bar, stack (list bar) | | | several or one | P3 |

`PageView` presets (configuration, same anatomy; Inferred from TPV strip
properties and DXDOC group properties):

| Preset | Strip | Close | Overflow | Mnemonics | Selection after close |
|---|---|---|---|---|---|
| `strip` (default) | top, content-sized items | off | scroll buttons plus list button | on | neighbour |
| `document` | top, shrink then list | selected and hot tabs | shrink, then list drop-down | off (titles are data) | history |
| `navigation` | left column, icon and label, collapsible to icons | off | scroll | on | neighbour |

### 3.4 `PageStrip` anatomy

Own parts (13 of 32):

| Part | PartRole | Part states | Nearest atom / gap |
|---|---|---|---|
| `region` | region | | |
| `rail` | face | | the band behind the tabs (DOCK's `tab_rail`) |
| `pinned_rail` | face | | present only when pinned pages have their own row (P19) |
| `scroll_back` | actuator, composed from `button` | hot, pressed, disabled | |
| `scroll_back_glyph` | glyph | | `arrow_left`, or `arrow_up` on a side strip |
| `scroll_forward` | actuator, composed from `button` | hot, pressed, disabled | |
| `scroll_forward_glyph` | glyph | | `arrow_right` or `arrow_down` |
| `list_button` | actuator, composed from `button` | hot, pressed, open | |
| `list_glyph` | glyph | | `overflow` (RB-20) or `chevron_down`; taste PT6 |
| `strip_close` | actuator, composed from `button` | hot, pressed | the strip-end close (Telerik `VS2005Style`, DevExpress `InTabControlHeader`) |
| `strip_close_glyph` | glyph | | `close` |
| `insertion_mark` | drop | drop_accepted | the gap bar during reorder and docking's tab insertion target (DOCK §5.2 priority 1) |
| `focus` | focus | | |

Item system `tab` (10 parts, names do not repeat the control's):

| Item part | PartRole | Notes |
|---|---|---|
| `tab` | face | the item's states below |
| `tab_icon` | icon | |
| `tab_label` | label | measured text; ellipsized when shrunk (RB-30 admits ellipsized tab labels for the ribbon) |
| `tab_modified` | glyph | present when modified; **[vocabulary decision]** mark (P21, DK J3); nearest `radio_dot` (a dot, as VS's large dot) |
| `tab_pin` | actuator, composed from `button` | part states hot, pressed, checked (pinned) |
| `tab_pin_glyph` | glyph | **[vocabulary decision]** pin mark (DK J1); nearest `arrow_down` pinned, `arrow_left` unpinned, as DOCK §4.5 |
| `tab_close` | actuator, composed from `button` | part states hot, pressed |
| `tab_close_glyph` | glyph | `close` |
| `tab_seam` | seam | the selected tab's join with the content, as `TabControl` |
| `tab_focus` | focus | |

Item states: standard `hot`, `pressed`, `captured` (being dragged),
`selected`, `current` (the strip's view holds the application's active
page; DOCK §4.5), `open` (a side tab whose flyout is out; DOCK §4.5),
`disabled`, `focus_visible`; custom `modified`, `pinned`, `preview` (3 of
16). Custom states need no vocabulary change (SPEC §8.4 rule 3 and §12.6)
but the owner may prefer standard states for `modified` (P35.2).

Paint family: `menu_item` until T4, as the ribbon's tabs (2.1);
**[vocabulary decision]** whether T4 adds a tab family (P35.9).

Text orientation: a strip on the left or right edge either lays labels
horizontally in a column (VS vertical tabs, VSTAB; DevExpress
`ItemOrientation`) or rotates them (Win32 `TCS_VERTICAL`; VS and DockPanel
Suite auto-hide tabs, DOCK §4.5). Rotation needs a text orientation in
`PartPaint` **[vocabulary decision]** (DK J4; P35.3); until then a side
strip uses a horizontal column and an auto-hide side tab shows icon and
tooltip.

### 3.5 `PageView` anatomy

Own parts (5): `region` (region); `strip` (region, composed from
`page_strip`, the child strip's bounds); `content` (well; "the work pane"
for documents, SPEC §3.2; `TabControl` calls it `display`); `empty_label`
(label, present when the set is empty and an empty text is set, for an
empty document area); `focus` (focus). No item systems of its own: tabs
belong to the child strip. Control states: the standard availability and
focus states plus `current` (the view holds the active page).

Behaviour carried over from `TabControl` (Known, 2.1): per-page focus
memory; selection swaps child visibility; Ctrl+Tab handling (now under a
policy, P23).

### 3.6 `NavigationPane` anatomy (Outlook style)

Own parts (17 of 32):

| Part | PartRole | Part states | Nearest atom / gap |
|---|---|---|---|
| `region` | region | | |
| `header` | face | | the current page's caption band |
| `header_icon` | icon | | |
| `header_label` | caption | | |
| `collapse_button` | actuator, composed from `button` | hot, pressed | |
| `collapse_glyph` | glyph | | `arrow_left`; `arrow_right` when collapsed (no `chevron_left`; no new mark proposed, P35.11) |
| `content` | well | | the selected page |
| `grip` | grip | hot, captured, focus_visible | the splitter between content and buttons; mark `grip_dots` |
| `button_area` | face | | behind the large buttons |
| `overflow_area` | face | | the strip of small icons |
| `configure_button` | actuator, composed from `button` | hot, pressed, open | "Configure Buttons" (OL) |
| `configure_glyph` | glyph | | `customize` (RB-20: "a short bar over an open down chevron") |
| `collapsed_bar` | face | | present only when collapsed |
| `collapsed_label` | label | | the current page's name; vertical **[vocabulary decision]** (P35.3); nearest: icon only |
| `width_grip` | grip | hot, captured | the pane's resizing edge (OL: drag the right border) |
| `popup_shadow` | shadow | | the collapsed pane's flyout (DXNAV popup; E4 overlay shadow as RB-24) |
| `focus` | focus | | |

Item system `nav_button` (large buttons): `nav_button` (face; hot,
pressed, selected, disabled, focus_visible), `nav_button_icon`,
`nav_button_label`, `nav_button_focus`.

Item system `overflow_item` (small icons in the overflow area, and in the
collapsed bar): `overflow_item` (face; hot, pressed, selected, disabled,
focus_visible), `overflow_item_icon`, `overflow_item_focus`. Its name comes
from the tooltip.

Behaviour (Inferred from OL, TPV Outlook, DXNAV, IGEXP; P31–P34):

1. The grip moves in whole-button steps; dragging it down demotes the last
   large button to the overflow area, up promotes the first overflow icon
   (TPV `DragGripDown`/`DragGripUp`). Keyboard: the grip is a semantic
   `split_pane` whose value is the large-button count, with increment and
   decrement (existing role and actions).
2. Pages that fit neither area are listed in the configure menu, which also
   offers Show More Buttons, Show Fewer Buttons and an "options" command
   that raises an event (P31).
3. Collapsed: the pane shrinks to a bar with the vertical caption and the
   overflow icons; clicking the caption opens the selected page's content in
   a popup over the window (an `open_popup` overlay, RB-23's rule for
   popups); Escape or an outside click closes it.
4. The state (order, large-button count, hidden pages, collapsed, width) is
   readable and settable as a record for the runtime layout store (as
   RB-12).

### 3.7 Semantics

| Element | Role | Fields and actions | Gap |
|---|---|---|---|
| `PageStrip` | `tab_group` | name; orientation not expressible | APG `aria-orientation` has no field (minor; P35.5) |
| tab | `tab` | selected; position in set and set size (RB-22); focus, select; **not** `press` (UIA-TAB: Invoke "Never") | `close` action (DK J6); description carries "modified" and "pinned" until a state exists |
| close and pin buttons | `button`, nested `children` of the tab node | press | none (nested children exist, `semantic_types.hpp:104`) |
| list button | `menu_button`, has-popup | show_menu | none |
| page content | `group` today (`tab_page.cpp:33`) | named by the tab's text | **[vocabulary decision]** `tab_panel` role and a tab-to-panel relation (APG `aria-controls`, UIA `ControllerFor`) |
| `NavigationPane` | `group` named by the pane | | |
| nav buttons and overflow icons | `tab_group` with `tab` (they select a page) | as tabs | none |
| grip | `split_pane` | value = large-button count; increment, decrement, set_value | none |
| collapse button | `button` | expand, collapse; `expanded` state | none |
| explorer bar groups (if admitted) | outline | expand, collapse | **[vocabulary decision]** tree and tree item roles (MSAA outline, UIA Tree, ATK tree; mapping names Unverified) |

---

## 4. What docking needs from it

The docking study's controls (DOCK §4.5) each grow a tab item system
today. Under this proposal they delegate: `ToolStack` = caption parts +
a `PageView` (strip at the bottom, shown only with two or more pages);
`DocumentStack` = a `PageView` with the `document` preset; `AutoHideStrip`
= a `PageStrip` on one edge bound to that edge's `PageSet`. Their own
`tab` item systems in DOCK §4.5 are then removed (Inferred; a change to the
docking study to record if the owner agrees, P1).

Each interface below is stated as what docking calls or hears, what the
page view guarantees, and a test.

| # | Docking needs (source) | Interface (proposed names) | Guarantee | Test |
|---|---|---|---|---|
| I1 | Panels by identity (DOCK §4.2 rule 5) | `PageId` equals `PanelId`; `find(PageId)`; positions only in payloads | no API takes a bare index for identity | lookups survive reorder |
| I2 | Moves without re-creation (DOCK §5.3 step 5) | `take(PageId) -> TakenPage{record, content}`; `put(TakenPage, position)` | content keeps its control, focus memory and state; ownership moves explicitly | a text box's text and caret survive a move between two views |
| I3 | One coherent update (DOCK §5.3 steps 4–8) | `PageSet::begin_batch()` / `end_batch()`; `PageView` honours the window's `UpdateScope` | one `pages_changed` and at most one `selected_page_changed` per batch, published after the edits; handlers that edit start a new batch (no re-entry) | event trace for a three-edit batch |
| I4 | Selection distinct from activation (DOCK §5.6, F1) | `page_invoked{page, cause}` on click, Enter, mnemonic or semantic select; `set_current(bool)` from outside; focus policy `select_focuses: strip \| content \| none` | the view never decides activation; it reports invocation and shows `current` when told | clicking a tab in an inactive stack raises `page_invoked` and selects; `current` only after docking sets it |
| I5 | Dirty display (DOCK §4.3) | `set_modified(PageId, bool)` | display only; the view attaches no meaning | item state and semantic description follow the flag |
| I6 | Close with prompts (DOCK H1) | `page_close_requested{page, cause}` (mutable `handled`); `ClosePolicy::request_only` | under `request_only` the view never removes a page itself; docking removes it after the application's answer | cancel leaves set and selection unchanged |
| I7 | Pinned and preview tabs (DOCK H3) | flags on the record; `page_pin_requested`; `promote_preview(PageId)` | order invariant 3.2 rule 2 | pinning keeps the boundary; preview stays last |
| I8 | Overflow with an active-files list (DOCK §4.5, H4) | overflow strategy setting; `overflow_menu_opening` (mutable: entries may be appended); `ensure_visible(PageId)` | the selected page is always visible after layout | with forty pages the selected one is visible at every width ≥ one tab |
| I9 | Drag hand-off (DOCK §5.1 idle → armed → dragging) | the strip arms on press and, past the threshold, raises `page_drag_started{page, press_point_screen, pointer_screen}` (mutable `claimed`); if claimed, the strip releases capture to the docking session and does nothing more | one drag owner at a time; an unclaimed drag reorders inside the strip (P28) | claimed drag: no reorder, `captured` cleared on release |
| I10 | Tab insertion target (DOCK §5.2 priority 1) | `insertion_position_at(Point screen) -> std::optional<std::size_t>`; `show_insertion_mark(position)`, `clear_insertion_mark()` | positions count gaps in the current order and respect the pinned boundary | table of pointer positions to gaps over a fixture strip |
| I11 | Position menu (DOCK §5.7: context-menu key, Shift+F10) | `page_context_menu_requested{page, anchor, cause}` | raised for right click, menu key and Shift+F10; the view shows no menu of its own when the event is handled | three causes, one event each |
| I12 | Float on double-click (DOCK §5.4) | `page_double_clicked{page}` | raised on the second click of a double click on a tab, not on its buttons | no double event from the close button |
| I13 | Auto-hide hover (DOCK §5.5) | `hot_page_changed{old, new}`; `set_page_open(PageId, bool)` for the `open` state | the strip reports hover; docking owns the delays and the flyout | hover enter and leave sequence |
| I14 | Lazy creation (DOCK I7) | record without content; `content_requested{page}` before first display | raised once per page, before `selected_page_changed` | factories called only for selected pages |
| I15 | Ctrl+Tab belongs to the navigator (DOCK G1) | `ctrl_tab_policy: select_next \| pass` | with `pass`, Ctrl+Tab is not handled; Ctrl+PageDown/PageUp still are | key routing table |
| I16 | Close keys and semantic close (DOCK §5.7, J6) | Ctrl+F4 (Cmd+W on macOS, a host keymap default, DK G5) raises `page_close_requested` with cause keyboard; the semantic `close` action does the same | one command path for pointer, key and assistive technology (a11y round §3.5 gate 2, via DOCK) | the three causes reach one handler |
| I17 | Persistence (DOCK §5.8) | `PageSet::snapshot()` returning order, selected, pinned and history | docking stores it in its layout; the view stores nothing itself | snapshot round trip |
| I18 | F6 regions (RB-17, DOCK G-FOCUS-3) | a `PageView` can be registered as a region | as any control | none new |

---

## 5. Interaction details

### 5.1 Overflow strategies

| Strategy | How it behaves | Seen in | Cost |
|---|---|---|---|
| Scroll | buttons (and wheel) shift the row; selection scrolls into view | Win32 single line (W32TAB), Telerik `Scroll`, DevExpress Prev/Next | tabs hide at both ends; position is lost on resize |
| List drop-down | a button lists every page (or only hidden ones); choosing one selects it | VS chevron, Telerik `ItemList`, DevExpress document selector, AvalonDock | a hidden selected page must be brought into the row: AvalonDock moves it to the first position (AD, Known) |
| Shrink | items narrow to a minimum, labels ellipsize | Telerik `Shrink`, Infragistics `SingleRowSizeToFit` | unreadable when very narrow; needs a minimum, then another strategy |
| Multi-row | rows added so all show | Win32 `TCS_MULTILINE`, WinForms `Multiline`, VS multi-row, DevExpress `MultiLine`, Telerik `MultiLine` | the content moves when rows change; row rotation confuses (Unverified) |
| Demotion (Outlook) | large items become icons by a grip | OL, TPV Outlook, IGEXP | Outlook only |

Proposed rules (Inferred):

1. Strategies compose in a fixed order per preset: shrink to the minimum
   item width, then the configured fallback (scroll or list). Multi-row
   replaces both when chosen.
2. Pinned pages never shrink below their icon-only width and never scroll
   or overflow (VS keeps pinned tabs in their own row, VSTAB; that they never
   overflow in a single row is Unverified).
3. The selected page is always visible after layout (I8). Under the list
   strategy, a page chosen from the list moves to the first ordinary
   position (AvalonDock) or swaps with the last visible one (P13).
4. Under multi-row, rows keep their order; selection never rotates rows
   (P14).
5. Layout is one pass over measured widths into caller-supplied storage,
   with the overflow decision made before the placement loop (house style
   §3); measuring uses real text widths, not the estimate of 2.2 item 2.

### 5.2 Close behaviour

1. Visibility of `tab_close`: one rule per view, from DevExpress's set
   (DXTAB): all tabs, selected only, selected and hot (the `TabbedView`
   default), strip end only. Space for the close button is reserved on
   every closable tab so widths do not jump on hover (Inferred).
2. A close request is raised; under `remove_when_unhandled` (standalone
   default) the view removes the page if no handler sets `handled`; under
   `request_only` (docking) it never removes (I6).
3. After the selected page closes: neighbour (the next page, else the
   previous; APG Delete rule, and `TabControl::remove_page` today,
   lines 113–120) or history (the most recent other page; DK H2's MRU) (P10).
4. Middle click: a release over the same closable tab that received the
   press closes it (DevExpress `MouseUp`; DockPanel Suite closes on middle
   click, DOCK §1.2); press and release on different tabs do nothing (P17).
5. Successive closes by the pointer: keeping item widths frozen while the
   pointer stays in the strip, so the next close button arrives under the
   pointer, is a known browser behaviour (Unverified); offered as part of
   P16 rather than assumed.

### 5.3 Keyboard

| Key | Effect | Source |
|---|---|---|
| Tab | enters the strip at the selected tab; again, into the content | APG |
| Left/Right (Up/Down on a side strip) | previous or next enabled page, wrapping; selects (automatic) or moves focus only (manual), P9 | APG; `TabControl` today |
| Home, End | first, last | APG; `TabControl` today |
| Space, Enter | selects the focused tab under manual activation | APG |
| Ctrl+PageDown, Ctrl+PageUp | next, previous page from anywhere inside the view | WF source |
| Ctrl+Tab, Ctrl+Shift+Tab | the same, unless `ctrl_tab_policy` is `pass` (I15, P23) | WF source; DK G1 |
| Ctrl+Shift+PageDown/PageUp | move the selected page right or left, within its pinned or ordinary group | VSTAB (18.11) |
| Ctrl+F4 (macOS Cmd+W) | close the selected closable page | DOCK §5.7, DK G5 |
| Delete on a focused tab | close it, focus the following tab or the preceding (optional) | APG |
| Shift+F10, menu key | the page's context menu (I11) | APG; DOCK §5.7 |
| Alt+mnemonic | select the page and focus its tab (strip and navigation presets) | RB-16; `parse_mnemonic_text` |

Nested views (a `PageView` inside a docked panel): the innermost view with
focus inside it handles Ctrl+PageDown first; Ctrl+Tab follows each view's
policy, and docking stacks set `pass` so the navigator receives it (DK G1
option 2 made explicit; Inferred).

Mnemonics: Windows Forms deliberately has none on tabs (WF source). The
house keyboard model includes mnemonics (RB-16), so strip pages may have
them; document titles are data (file names may contain `&`), so the
document preset turns them off (P25).

### 5.4 Selection, activation and events

Order for a pointer press on an unselected, enabled tab (proposed; WF's
four events collapse into one cancellable event, as Telerik's
`SelectedPageChanging`):

1. Pointer down: `pressed` on the tab; focus goes to the strip unless the
   focus policy says otherwise (Win32 offers both, `TCS_FOCUSNEVER`,
   `TCS_FOCUSONBUTTONDOWN`).
2. `selecting{old, new, cause}`, mutable `cancel`. Handlers may not edit
   the set (refused, as the phase guard of GF006 L4 cited in DOCK §5.3).
3. If not cancelled: remember the old page's focus; `content_requested` if
   the new page has no content; swap visibility; restore the new page's
   focus if the old page held it (`TabControl` today).
4. `selected_page_changed{old, new, cause}`.
5. `page_invoked{page, cause}` (I4).
6. Pointer up: `pressed` cleared. A release on another tab does nothing.

Programmatic selection raises 2 to 4 with cause `programmatic`, never 5.

### 5.5 Drag

Without docking: an unclaimed drag past the threshold reorders inside the
strip, with the dragged tab `captured`, and either live sliding or the
`insertion_mark` (P28); it never crosses the pinned boundary (VSTAB);
Escape restores the order; release commits one `page_moved`. With docking:
the strip raises `page_drag_started` and docking claims it (I9); docking
then uses I10 to show the insertion mark in whichever strip is under the
pointer. Tear-off is docking's (P29). The threshold is docking's portable 4
logical units (DK D5).

### 5.6 Accessibility

Covered by 3.7. Two points for stage A: the tab must not offer `press`
(UIA-TAB), and the modified and pinned flags need an accessible form;
until a state exists, the tab's description reads "modified" and "pinned"
(Inferred).

---

## 6. Phased build plan

Each phase is one pull request with tests in `tests/` run by CI on every
platform, under stage W's checklist (anatomy, schema, registry, Gallery
specimen, docstring). Docking phases are cited from DOCK §7.

**Phase 1. `PageSet` (headless).** Records, `PageId`, order invariant,
selection, history, cancellable selection hook, batches, take and put.
Tests: invariants after random edit sequences; refusals leave the set
unchanged; batch publishes one change; take and put keep the content
pointer; fallback selection after removal under both rules.

**Phase 2. `PageStrip`, top and bottom, single row.** Measured item
layout (content, fixed, fill), scroll and list overflow, `ensure_visible`,
item system and states, keyboard (arrows, Home, End, Ctrl+PageDown),
mnemonics, semantics without `press`, everything through `paint_part`.
Tests: part geometry tables at several widths; the selected tab visible at
every width; keyboard traces; mnemonic selection; semantic snapshot with
position in set; no allocation per tab during paint (an allocation counter
if the test harness has one, else a review item).

**Phase 3. `PageView` (`strip` preset).** Child strip, content, focus
memory, `selecting` and event order of 5.4, lazy content, Ctrl+Tab policy,
F6 registration, Gallery specimen beside `TabControl`. Tests: event order
traces; cancel keeps everything; focus restored on return; nested views
and key routing.

**Phase 4. Document affordances (`document` preset).** Close rules,
middle click, close policies, modified, pinned group, preview (if
admitted), shrink then list, overflow menu hook, Ctrl+F4 (Delete only if P26-1),
Ctrl+Shift+PageDown/PageUp. Tests: close causes reach one event; cancel
leaves the set unchanged; history fallback; pinned boundary in every move
path; shrink minimum then overflow.

**Phase 5. Side strips and rows.** Left and right placement with a
horizontal column; rotated text after the owner's J4/P35.3 answer;
multi-row if admitted (P14). Tests: geometry per edge; row order stable on
selection.

**Phase 6. Drag and docking hooks.** Reorder, `page_drag_started` with
claim, `insertion_position_at`, insertion mark, context menu and
double-click events. Tests: a fake docking session claims drags; insertion
tables; Escape restores; claimed drags never reorder. This phase precedes
DOCK phase 3.

**Phase 7. `NavigationPane`.** Header, content, grip, both item systems,
configure menu, collapsed bar and popup, state record. Tests: grip steps
and their semantic `split_pane` value; hidden pages appear only in the
menu; collapse and popup open and close order; state round trip.

**Phase 8. Navigation preset and `TabControl` alignment.** The navigation
preset (P4); re-basing `TabControl`'s painting and layout on `PageStrip`'s
geometry and `paint_part` while keeping its Windows Forms surface (P2).
Tests: `TabControl`'s existing tests unchanged; its anatomy names kept.

Dependencies: phases 1 to 4 need nothing new from the hosts. Phase 5's
rotated text needs J4/P35.3. Phase 6 must land before DOCK phase 3 (drag),
phase 4 before DOCK phase 6 (documents), phase 2 before DOCK phase 5
(auto-hide strips). T4 supplies the look; T2 the scroll animation (until
then scrolling snaps, as reduced motion requires anyway).

Gate (proposed): each phase's tests pass on every CI platform; the owner
accepts the Gallery specimens of `PageView` (strip and document) and
`NavigationPane`.

---

## 7. Open owner decisions, as interview questions

For the docking interview (`planning/interview/DOCKING_INTERVIEW_ROUND_001.md`)
as its tab section. Answer by number (for example `P1-1, P11-2`). Where a
question overlaps a docking question, one answer serves both and the
overlap is named. Each recommendation is a working hypothesis.

### A. Scope and structure

**P1. Shape of the family.**
1. One renderer-free `PageSet`, and separate presenter types: `PageStrip`
   (headers only), `PageView` (strip plus content, with presets) and
   `NavigationPane`; docking's stacks and auto-hide strips are built from
   them and lose their own tab item systems (3.1, 4).
2. One `PageView` control with a view mode, as Telerik.
3. Extend `TabControl` with the document features.

Recommendation: 1; 2 cannot fit one static anatomy within four item
systems (3.1), and 3 is excluded by MATRIX row 51.

**P2. `TabControl`'s future.**
1. Keep its Windows Forms surface and re-base its layout and painting on
   `PageStrip` and `paint_part` later (phase 8).
2. Leave it as it is.
3. Keep the surface and make it a thin wrapper over `PageView`.

Recommendation: 1; it also fixes the gaps of 2.2 items 1 to 4 once.

**P3. Which modes now.**
1. Strip, document and Outlook-style now; the navigation strip next;
   stack (list bar) and explorer bar not admitted until a consumer asks.
2. Every Telerik mode except Backstage (which stays the ribbon's, RB-26).
3. Strip and document only; Outlook-style later.

Recommendation: 1.

**P4. What "navigation strip" means.**
1. A left column of icon-and-label page items that collapses to icons when
   narrow (Telerik Navigation mode, DevExpress `NavigationPane`), as a
   `PageView` preset.
2. Outlook 2013's bottom bar of text items with a "…" overflow and peek
   popups (Telerik OfficeNavigationBar), as its own control.
3. Both.

Recommendation: 1, unless the owner meant 2.

**P5. Content ownership.**
1. The view owns page content as child controls; a page may start without
   content and receive it through `content_requested` on first selection.
2. Content is always supplied when the page is added.

Recommendation: 1 (docking's lazy creation, DK I7).

**P6. Headers-only strip.**
1. `PageStrip` is a public control of its own (auto-hide strips,
   application strips with external content).
2. Internal to `PageView`; auto-hide strips are views with no content area.

Recommendation: 1.

### B. Selection, activation and events

**P7. Selection events.**
1. One cancellable `selecting`, then `selected_page_changed`, keyed by
   `PageId` (Telerik's pair).
2. Windows Forms' four events (deselecting, deselected, selecting,
   selected).
3. No cancellation.

Recommendation: 1; `TabControl` keeps its own surface (P2).

**P8. Activation.**
1. The view reports `page_invoked` and shows `current` when told; who is
   active is decided outside (docking's two tracks, DK F1).
2. The view decides activation itself (focus into content on every click).

Recommendation: 1.

**P9. Arrow keys on tabs.**
1. Select as they move (automatic activation, as `TabControl` today).
2. Move focus only; Space or Enter selects (manual).
3. Automatic by default; manual per view when content is slow.

Recommendation: 3 (APG's guidance).

**P10. Selection after the selected page closes.**
1. Strip preset: the next page, else the previous (APG; `TabControl`
   today); document preset: the most recent other page (DK H2).
2. Always the neighbour.
3. Always the most recent.

Recommendation: 1.

### C. Overflow and layout (refines DK H4)

**P11. Overflow strategy.**
1. Strip preset: scroll buttons plus a list button; document preset:
   shrink to a minimum, then a list drop-down (DK H4 recommendation 3
   then 1); multi-row as an option on either (P14).
2. One strategy for both: scroll.
3. One strategy for both: list drop-down.

Recommendation: 1.

**P12. Item sizing.**
1. Content-sized with a minimum and maximum, optional fill; a fixed size as
   the alternative (Windows Forms `Normal` and `Fixed`; Telerik `Fill`).
2. Fixed size only (as `TabControl` today).

Recommendation: 1.

**P13. A hidden page chosen from the list.**
1. Moves to the first ordinary position (AvalonDock, Known).
2. Swaps with the last visible ordinary page; order otherwise kept.
3. The row scrolls to it (only with the scroll strategy).

Recommendation: 1 for the document preset (it puts the page where the
eye starts and matches a shipped system); 3 for the strip preset.

**P14. Multiple rows.**
1. Not offered.
2. Offered, rows in a stable order (no rotation), pinned pages in a row of
   their own (VS).
3. Offered with Windows Forms row rotation.

Recommendation: 2 in phase 5.

**P15. Mouse wheel over a strip.**
1. Scrolls the strip (scroll strategy only).
2. Selects the next or previous page.
3. Nothing.

Recommendation: 1.

### D. Document affordances (refines DK H3)

**P16. Close button visibility.**
1. Selected and hot tabs, with the space reserved on every closable tab
   (DevExpress `TabbedView` default).
2. All tabs.
3. Selected tab only.
4. One close button at the strip end (VS 2005 style).

Recommendation: 1; whether widths freeze during successive closes (5.2
item 5) is a sub-question: recommend yes.

**P17. Middle click.**
1. Closes a closable tab on release over the tab that was pressed.
2. Closes on press.
3. Nothing.

Recommendation: 1.

**P18. Close policy.**
1. A close request event; standalone views remove the page when no
   handler takes the request; docking's views only request (I6).
2. The view always removes, after a cancellable `closing` event.
3. The application always removes.

Recommendation: 1.

**P19. Pinned pages.**
1. A capability of the page model and strip (pinned group first, pin
   button per tab, separate row under multi-row); docking decides whether
   documents use it (DK H3).
2. Not built now.

Recommendation: 1; with DK H3's recommendation ("neither now") the
capability waits unused until a consumer turns it on.

**P20. Preview page.**
1. A capability: one preview page kept last, custom state `preview`,
   promoted by `promote_preview` (VS).
2. Not now.

Recommendation: 2, consistent with DK H3-1.

**P21. Modified display (same as DK J3).**
1. Custom item state `modified` plus a `tab_modified` glyph part whose mark
   the theme chooses; the label text is unchanged.
2. The label gains a `*` (VS's older indicator).
3. A standard `modified` state in the vocabulary (all controls may use it,
   for example a property grid row).

Recommendation: 1 now, with 3 if stage A or another control needs the
state (VS offers both a dot and an asterisk; the look is taste PT4).

**P22. A new-page ("+") button.**
1. Not offered.
2. Offered as a strip part with a new `add` mark (Telerik `NewItem`).

Recommendation: 1.

### E. Keyboard and mnemonics (refines DK G1)

**P23. Ctrl+Tab and Ctrl+PageDown.**
1. Every view handles Ctrl+PageDown and Ctrl+PageUp when focus is inside
   it; Ctrl+Tab per view policy, on for standalone views and off (`pass`)
   for docking stacks, so the navigator gets it.
2. Views handle only Ctrl+PageDown and Ctrl+PageUp; Ctrl+Tab is never a
   view's.
3. Both always, innermost first (DK G1 option 2).

Recommendation: 1.

**P24. Moving a tab by keyboard.**
1. Ctrl+Shift+PageDown and Ctrl+Shift+PageUp, within the pinned or
   ordinary group (VS 18.11).
2. No keyboard move; drag and the context menu only.

Recommendation: 1.

**P25. Mnemonics in page text.**
1. On in the strip and navigation presets (Alt+letter selects the page),
   off in the document preset.
2. Never (Windows Forms doubles every `&`).
3. Always.

Recommendation: 1.

**P26. Close keys.**
1. Ctrl+F4 (Cmd+W on macOS) closes the selected closable page; Delete on
   a focused tab closes it (APG's optional key).
2. Ctrl+F4 only.
3. Neither; docking's commands only.

Recommendation: 2; Delete is too easy to press while the strip has focus
by accident (Inferred), and APG makes it optional.

### F. Dragging

**P27. Who may reorder.** A per-view flag, off by default for the strip
preset and on for the document preset?
1. Yes.
2. On everywhere.
3. Off everywhere unless docking is present.

Recommendation: 1.

**P28. Reorder feedback.**
1. Insertion mark, then drop (the same feedback as docking's tab
   insertion in preview mode, DK D1-1).
2. Live sliding (AvalonDock, VS).

Recommendation: 1 for one behaviour inside and outside docking; 2 if
docking adopts immediate mode (DK D1-3).

**P29. Tear-off.**
1. Only through the drag hand-off (I9); a view alone never creates
   windows.
2. Views can tear pages off into floating windows without docking.

Recommendation: 1.

### G. Outlook-style navigation pane

**P30. Scope of the pane.**
1. Header, content, grip with large buttons and an overflow icon strip,
   the configure menu (Show More, Show Fewer, hidden pages) and an
   "options" event; no built-in options dialog.
2. As 1 plus a built-in options dialog (reorder, hide, reset; OL).
3. Large buttons and grip only.

Recommendation: 1.

**P31. Collapsed pane.**
1. A narrow bar with the vertical caption and the overflow icons; the
   caption opens the content in a popup over the window (DXNAV popup).
2. Icons only, no caption, no popup; expanding is the only way back.
3. No collapse.

Recommendation: 1 (needs P35.3 for the vertical caption; icon-only until
then).

**P32. A click outside the collapsed pane's popup.**
1. Closes it and passes through (DK E2-1, the auto-hide rule).
2. Closes it and is consumed (RB-12, the minimized ribbon).

Recommendation: the same answer as DK E2.

**P33. Pane state.**
1. Exposed as a record saved by the runtime layout store, with explicit
   save and load (as RB-12).
2. The application saves it.

Recommendation: 1.

**P34. Semantics of the navigation buttons.**
1. Tabs in a tab list (they select a page); the pane a named group.
2. Buttons in a toolbar.
3. An outline (tree), only if an explorer-bar mode is admitted.

Recommendation: 1.

### H. Vocabulary (each a yes or no; SPEC §12.7)

**P35.**
1. A pin mark, drawn pinned or unpinned by `checked` (same as DK J1).
   Recommend yes.
2. `modified`: custom item state now (P21-1), or a standard state (P21-3).
   Recommend custom.
3. A text orientation in `PartPaint` for side tabs and the collapsed
   pane's caption (same as DK J4). Recommend yes.
4. A `tab_panel` semantic role for page content (APG `tabpanel`; UIA's
   Pane under a TabItem). Recommend yes, with stage A.
5. A tab-to-panel relation field on semantic nodes (APG `aria-controls`,
   UIA `ControllerFor`), and an orientation field for tab lists. Recommend
   the relation yes, orientation yes, with stage A.
6. A `close` semantic action (same as DK J6), and removing `press` from
   tabs (UIA-TAB). Recommend yes to both.
7. Tree and tree item roles (outline). Recommend deferring to stage A
   unless an explorer-bar mode is admitted (P3); `TreeView` needs them too.
8. Custom item states `pinned` and `preview` (no vocabulary change).
   Recommend confirming.
9. A tab paint family: `menu_item` until T4 (as the ribbon), with T4
   adding the tab stack's drawing. Recommend yes.
10. An `add` mark only if P22-2.
11. No new marks for the navigation pane: `arrow_left`/`arrow_right` for
    collapse and expand, `customize` for the configure button, `grip_dots`
    for the grip. Recommend confirming.

### Questions for the owner's taste

Numbered PT to keep them apart from the docking study's T1 to T8.

**PT1. Tab shape.** Strip tabs as the ribbon's tab stack (Plan Paint; "A
tab is not a button", cited for DK T4), a quieter Studio 2003 strip, or
classic raised tabs joined to the page (today's `TabControl` seam)? Are
document tabs and strip tabs the same shape?

**PT2. The selected tab's join.** An open seam into the page (classic), an
accent underline, or a plate?

**PT3. Close and pin glyphs.** Inside the tab at the trailing end, with a
hover plate or bare? Pin beside close, or pin replacing close on pinned
tabs?

**PT4. Modified.** VS's large dot, an asterisk, italic label, or a tint;
does the dot replace the close glyph until hover?

**PT5. Pinned tabs.** Full width with a pin glyph, or compressed to icon
only?

**PT6. Overflow button.** The `overflow` double chevron (as `ToolStrip`),
a down arrow, or a count ("12 ▾")? Hidden pages in italic in the list
(VS)?

**PT7. Outlook pane.** Office 2007/2010 large buttons with their banded
material, or flat; the grip's look; the collapsed bar with vertical text.

**PT8. Side tabs.** On the left edge, does rotated text read bottom to top
or top to bottom; horizontal-text columns instead?

### Questions a mockup answers better than words

- **PT1, PT2, PT3:** one strip of five tabs (selected, hot, pressed,
  disabled, with icon) in two or three candidate shapes, top and bottom.
- **PT4, PT5, PT6:** a document strip of twelve tabs at a narrow width:
  pinned, modified, preview, shrink then overflow, the list open.
- **PT7:** the navigation pane expanded with four large buttons and three
  overflow icons, the configure menu open, and the collapsed bar with its
  popup.
- **PT8 and P31:** a left auto-hide strip and the collapsed pane, rotated
  text against icon-only.
- **P13 and P28:** two short frame sequences: a page chosen from the list
  under each rule, and reorder by insertion mark against live sliding.

---

### Interview and verdict citations used

RB-12, RB-16, RB-17, RB-20, RB-22, RB-23, RB-24, RB-26, RB-30; SPEC §3.2,
§8.1, §8.4, §8.5, §12.6, §12.7, §12.8; MATRIX rows 43 and 51; DOCK §1.2,
§1.3, §4.2, §4.3, §4.5, §5.1–§5.8, §7 and questions D1, D5, E2, F1, G1,
G5, H1–H4, I7, J1, J3, J4, J6; GF006 L4 (through DOCK §5.3); a11y round
§3.5 gate 2 (through DOCK §5.7).
