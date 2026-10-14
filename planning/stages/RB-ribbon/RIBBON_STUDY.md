# A proper ribbon for GUI.Forms: study

Date: 2026-10-14. Status: research study, not a decision record. Nothing here
is decided until the owner answers section 8.

Reading key. **Known** means read in a cited source (a Microsoft page, a file
in the repository, an interview record). **Inferred** means reasoned from
known material but not stated by any source. **Unverified** means remembered
or reported second hand and not checked against a primary source in this
study. Repository paths are relative to `C:\Users\Shadow\gui_forms` unless
another root is named. Interview and verdict IDs are the owner's.

Contents

1. What a ribbon is
2. The ribbon as Windows Forms pieces
3. What GUI.Forms has today, and the gaps
4. Proposed anatomy in GUI.Forms terms
5. The hard algorithms
6. Command and data model, and fit with the stages
7. Phased build plan
8. Open owner decisions

Sources used, by short name:

| Name | Source |
|---|---|
| UIR-CTL | Windows Ribbon Framework control library, https://learn.microsoft.com/en-us/windows/win32/windowsribbon/windowsribbon-controls-entry |
| UIR-SIZE | Customizing a Ribbon Through Size Definitions and Scaling Policies, https://learn.microsoft.com/en-us/windows/win32/windowsribbon/windowsribbon-templates |
| UIR-SCALE | ScalingPolicy element, https://learn.microsoft.com/en-us/windows/win32/windowsribbon/windowsribbon-element-scalingpolicy |
| UIR-KEYTIP | Command.Keytip property, https://learn.microsoft.com/en-us/windows/win32/windowsribbon/windowsribbon-element-command-keytip |
| UIR-TABGROUP | Tab Group (contextual tabs), https://learn.microsoft.com/en-us/windows/win32/windowsribbon/windowsribbon-controls-tabgroup |
| UIR-QAT | Quick Access Toolbar, https://learn.microsoft.com/en-us/windows/win32/windowsribbon/windowsribbon-controls-quickaccesstoolbar |
| UIR-STATE | Persisting Ribbon State, https://learn.microsoft.com/en-us/windows/win32/windowsribbon/ribbon-statepersistence |
| UIR-VERB | UI_EXECUTIONVERB, https://learn.microsoft.com/en-us/windows/win32/api/uiribbon/ne-uiribbon-ui_executionverb |
| UX-RIBBON | Windows 7 Ribbons (UX guidelines), https://learn.microsoft.com/en-us/windows/win32/uxguide/cmd-ribbons |
| OFF-KEYS | Use the keyboard to work with the ribbon, https://support.microsoft.com/en-us/office/use-the-keyboard-to-work-with-the-ribbon-954cd3f7-2f77-4983-978d-c09b20e31f0e |
| OFF-SHOW | Show or hide the ribbon in Office, https://support.microsoft.com/office/show-or-hide-the-ribbon-in-office-d946b26e-0c8c-402d-a0f7-c6efa296b527 |
| OFF-2010 | Ribbon Extensibility in Office 2010: Tab Activation and Auto-Scaling, https://learn.microsoft.com/en-us/previous-versions/office/developer/office-2010/ee691834(v=office.14) |
| OFF-BS | Introduction to the Office 2010 Backstage View for Developers, https://learn.microsoft.com/en-us/previous-versions/office/developer/office-2010/ee691833(v=office.14) |
| OFF-SIMPLE | Using the simplified ribbon (cited in the owner's style atlas), https://support.microsoft.com/en-us/word/using-the-simplified-ribbon-in-word-for-the-web |
| CUI2 | MS-CUSTOMUI2 CT_Group (`autoScale`), https://learn.microsoft.com/en-us/openspecs/office_standards/ms-customui2/c048c71f-0202-454b-b061-8dddea648e6b |
| MFC-BAR | CMFCRibbonBar, https://learn.microsoft.com/en-us/cpp/mfc/reference/cmfcribbonbar-class |
| MFC-CAT | CMFCRibbonCategory, https://learn.microsoft.com/en-us/cpp/mfc/reference/cmfcribboncategory-class |
| MFC-ELEM | CMFCRibbonBaseElement, https://learn.microsoft.com/en-us/cpp/mfc/reference/cmfcribbonbaseelement-class |
| WPF-NS | System.Windows.Controls.Ribbon namespace, https://learn.microsoft.com/en-us/dotnet/api/system.windows.controls.ribbon |
| WPF-GSRO | RibbonTab.GroupSizeReductionOrder, https://learn.microsoft.com/en-us/dotnet/api/system.windows.controls.ribbon.ribbontab.groupsizereductionorder |
| UIA-PROP | UI Automation element property identifiers, https://learn.microsoft.com/en-us/windows/win32/winauto/uiauto-automation-element-propids |
| FR | Fluent.Ribbon (MIT, WPF), https://github.com/fluentribbon/Fluent.Ribbon; documentation https://fluentribbon.github.io/documentation/ (sizing: `/concepts/sizing`, contextual tabs: `/concepts/contextual-tabs`, KeyTip: `/controls/keytip`, Backstage: `/controls/backstage`, start screen: `/controls/start-screen`) |
| FR-SRC | Fluent.Ribbon source, branch `develop`, read on the web on 2026-10-14: `Fluent.Ribbon/Controls/Ribbon.cs`, `RibbonGroupBox.cs`, `RibbonGroupsContainer.cs`, `InRibbonGallery.cs`, `GalleryItem.cs`, `ScreenTip.cs`, `RibbonToolBar.cs`, `Fluent.Ribbon/Enumerations/RibbonGroupBoxState.cs` (raw files under https://raw.githubusercontent.com/fluentribbon/Fluent.Ribbon/develop/) |
| SPEC | `planning/DESIGN_LANGUAGE_SPECIFICATION.md` |
| RENDER | `planning/stages/T4-default-theme/RENDERING_REFERENCE.md` |
| A11Y-001 | `planning/interview/GUI_FORMS_ACCESSIBILITY_AND_TEXT_ROUND_001.md` |
| LEDGER | `planning/interview/GUI_FORMS_INTERVIEW_LEDGER.md` |
| RES | `planning/interview/GUI_FORMS_RESOURCES_AND_CONFIGURATION.md` |
| VISUAL | owner's verdicts in `C:\Users\Shadow\file_manager\frontend\planning\visual\` |
| PLAN-PAINT | `C:\Users\Shadow\plan-paint` at `a959257` (the briefs cite `6b901fc`; the ribbon code read here is the clone's current head) |

---

## 1. What a ribbon is

### 1.1 The canonical anatomy

All four canonical families, and Fluent.Ribbon (1.4), share one anatomy
(Known: UX-RIBBON, UIR-CTL, MFC-BAR, WPF-NS, FR):

| Element | What it is |
|---|---|
| Application button or File tab | Opens the file-level commands: a menu (Office 2007, the Windows Ribbon Framework, MFC, WPF) or the full-window Backstage view (Office 2010 and later). UX-RIBBON requires one even for programs without files. |
| Quick Access Toolbar (QAT) | A small customizable toolbar of frequent commands, in the title bar by default or below the ribbon (UIR-QAT). UX-RIBBON requires one even with a single tab. |
| Tab row | Core tabs (always shown) and contextual tab sets (shown while an object type is selected). Each tab selects a command set, not a content location (DNA-C01). |
| Contextual tab set | A labelled, coloured header above one or more contextual tabs; several sets may show at once for an object with several types (UX-RIBBON). |
| Command area | The selected tab's groups laid out left to right. |
| Group | A captioned cluster of commands; caption beneath; optional dialog box launcher at the caption's end; collapses to a single popup-group button when space runs out. |
| Commands | Button, toggle button, split button, drop-down button, check box, combo box, spinner, in-ribbon gallery, drop-down gallery, split-button gallery, font control, colour picker, control groups (joined segments). |
| Gallery | A grid of graphical choices, in the ribbon (in-ribbon) or in a drop-down, often with live preview on hover (UX-RIBBON). |
| Enhanced tooltip | Title, shortcut, description, optional image and help reference (UX-RIBBON). |
| KeyTips | Letter badges shown on Alt or F10, used to reach every tab and command from the keyboard in a multi-level sequence (UX-RIBBON, UIR-KEYTIP, MFC-BAR). |
| Minimize / collapse control | Hides the command area, leaving the tab row; a tab click then shows the command area as a temporary popup (UIR-STATE, OFF-SHOW). |
| Help button | A help glyph at the tab row's end (UIR-CTL HelpButton). |
| Context popup | A mini toolbar and context menu shown at the pointer, with ribbon commands (UIR-CTL ContextPopup). |

### 1.2 Canonical behaviour

**Tabs and contextual sets.** Known (UX-RIBBON): use at most seven core
tabs; label the first Home; select a contextual tab automatically when the
user inserts an object or double-clicks one (select the first tab of the
set), and return to the previously chosen contextual tab when the user clicks
off and back onto an object of the same type; when the active contextual tab
is removed, make Home or the first tab active; give a newly shown set a
colour different from the sets already shown. A *modal tab* replaces the core
tabs during a temporary mode. In the Windows Ribbon Framework a set's
availability is one of not available, available or active
(`UI_PKEY_ContextAvailable`, UIR-TABGROUP), and *application modes* (a
bitmask the application sets) choose which core tabs and groups exist (Known:
UIR-SCALE example uses `ApplicationModes="1"`; UIR-TABGROUP says a Tab Group
does not support modes but its tabs may).

**Adaptive scaling.** Known (UX-RIBBON): "There is no default ribbon size";
designers specify up to three layouts per group and the combinations that can
be used together; "the ribbon will show the largest valid combination that
fits the current window size"; "the smallest size is a single pop-up group
icon"; a group with a single button is not scaled to a popup icon; the most
used 32×32 commands are the last to become 16×16. Windows Ribbon Framework
groups have Large, Medium, Small and Popup sizes; a tab's `ScalingPolicy`
lists ideal sizes and then an ordered list of `Scale` steps, which "must be in
descending order of valid sizes" per group (UIR-SCALE). Office custom groups
scale only with `autoScale="true"` (Office 2010 and later), "always from
right-to-left", normal to medium to small to a single button whose image is
the group's own (OFF-2010, CUI2). WPF lists groups in
`RibbonTab.GroupSizeReductionOrder`, a group name repeated once per step
(WPF-GSRO). MFC has an internal order, overridable by
`CMFCRibbonCategory::SetCollapseOrder`, which falls back to the internal
algorithm when the list is exhausted (MFC-CAT). When even popup groups do not
fit, the Windows Ribbon Framework introduces scrolling (UIR-SIZE: "before
automatically introducing a scrolling mechanism"); MFC has category scroll
buttons (`HitTestScrollButtons`, MFC-CAT). When the window is too small the
Windows Ribbon Framework hides the ribbon entirely (the *collapsed* state) and
restores it when the window grows (UIR-STATE). Microsoft publishes no pixel
threshold; an Office team blog anecdote places it near 250 px (Unverified).
Fluent.Ribbon hides below 300 px wide or 250 px high (FR-SRC `Ribbon.cs`).

**Minimized ribbon.** Minimized is not collapsed: a minimized ribbon shows
only the tab row and is a user choice; a collapsed ribbon is hidden by the
framework (UIR-STATE). Ctrl+F1 minimizes and restores (OFF-KEYS). Office 365
offers three display states: full-screen mode (ribbon hidden; shown by the
ellipsis or Alt), show tabs only, and always show (OFF-SHOW). UX-RIBBON:
start expanded the first time; persist the minimized state; do not persist
the last selected tab.

**KeyTips and keyboard.** Known (UX-RIBBON): Alt enters keytip mode; a
character chooses a tab, the application button or a QAT command; the next
level shows the chosen tab's command keytips. Keytips may be two letters; the
application button gets F; QAT and recent-file lists are numbered; tabs take
H for Home and otherwise first letters; J, Y and Z are reserved by
convention for contextual tabs, unassigned keytips and popup groups;
popup groups take Z plus a letter. The Windows Ribbon Framework assigns F to
the application menu, Y to commands without a keytip and Z to groups (shown
only for a group whose policy includes Popup), and resolves duplicate tab
keytips by appending 1, 2, 3 in declaration order; a refactored keytip is
drawn in a slightly different text colour (UIR-KEYTIP). MFC shows keytips on
Alt or F10, with a 200 ms delay on Alt so Alt chords still work (MFC-BAR).
Office keyboard rules (OFF-KEYS): Alt shows keytips and focuses the selected
tab; Left/Right switch tabs; Tab and Shift+Tab move between commands; arrows
also move; Ctrl+Left/Right jumps between groups; Esc steps back one keytip
level, then leaves the ribbon; Enter or Space runs a command; Alt+Down opens a
split button's menu; Shift+F10 opens a command's context menu; F6 cycles
ribbon, document, task pane and status bar.

**QAT.** Known (UIR-QAT): up to 20 commands; an application-defaults list
whose members appear in the QAT drop-down with check marks; "More Commands"
launches an application-provided customization dialog; every ribbon command
except those in context popups has an "Add to Quick Access Toolbar" context
menu; a QAT command must exist in the ribbon or the defaults list (no new
commands at run time); QAT items, QAT position (above or below the ribbon)
and the minimized state persist through `SaveSettingsToStream` (UIR-STATE).

**Galleries and live preview.** Known (UX-RIBBON): use an in-ribbon gallery
for frequently used choices that fit the ribbon's height; show at least three
choices; all items one thumbnail size; in-ribbon thumbnails 16×16, 48×48 or
64×48; drop-down thumbnails 16×16 up to 128×128; clicking the document to
dismiss a drop-down gallery dismisses it without selecting anything; a
preview must apply and restore within 500 ms. The Windows Ribbon Framework
delivers hover as `UI_EXECUTIONVERB_PREVIEW`, leaving as
`UI_EXECUTIONVERB_CANCELPREVIEW` and a click as `UI_EXECUTIONVERB_EXECUTE`
(UIR-VERB). In-ribbon galleries declare column limits per group size
(`MaxColumns`, `MaxColumnsMedium`, `MinColumnsLarge`, `MinColumnsMedium`) and
a drop-down layout (`VerticalMenuLayout Rows`, `Gripper`) with menu groups
beneath the items (UIR-SIZE example).

**Application menu and Backstage.** The application menu (Office 2007,
Windows Ribbon Framework, MFC, WPF) is a two-pane menu: commands with large
icons on the left, a recent-items list or submenu on the right (Known for
the controls' existence: UIR-CTL `ApplicationMenu`, `RecentItems`; WPF-NS
`RibbonApplicationMenu`, `RibbonApplicationSplitMenuItem`; layout details
Unverified). Backstage (Office 2010 and later) is "a full-screen user
interface model that exposes file-level functionality"; Office 2010 replaced
the Office button with a File tab; Backstage has fast commands in a left
navigation bar, tabs with one or two columns of groups, and commands marked
definitive close Backstage after running; Alt+F opens it (OFF-BS). Microsoft's
distinction: the ribbon helps you work *in* documents, Backstage *with* them
(OFF-BS).

### 1.3 How the four families differ

| Aspect | Office Fluent (Word, Excel, Outlook) | Windows Ribbon Framework (UIRibbon; Paint, WordPad on Windows 7 to 10) | MFC `CMFCRibbonBar` | WPF `System.Windows.Controls.Ribbon` |
|---|---|---|---|---|
| Authoring | Built-in UI plus customUI XML for add-ins with callbacks (`getEnabled`, `getLabel`, invalidation) | Compiled XML markup (`Application.Commands`, `Application.Views`) plus `IUICommandHandler` code; markup compiler rejects a group that does not match its template (UIR-SIZE) | C++ object construction (`AddCategory`, `AddPanel`, element classes) or a resource loaded by `LoadFromResource` (MFC-BAR) | XAML object tree with data binding and `ICommand` |
| Separation | Product-internal | Strict: markup describes view, commands carry state; properties pulled lazily only when needed (UIR-TABGROUP) | Elements own state; command routing through MFC message maps and `OnUpdateCmdUI` (MFC-CAT) | Controls own state; `ICommand.CanExecute` |
| File area | Office 2007 menu; 2010+ Backstage | Application menu with `RecentItems` | `CMFCRibbonApplicationButton` and main panel (Unverified detail) | `RibbonApplicationMenu` (WPF-NS) |
| Scaling | Product-tuned; add-ins: `autoScale`, right to left (OFF-2010) | Templates or custom `SizeDefinition` per group; tab `ScalingPolicy`; Popup; scrolling last | Internal algorithm; `SetCollapseOrder`; panels collapse to a button; scroll buttons (MFC-CAT) | `RibbonGroupSizeDefinition` per group, `GroupSizeReductionOrder` per tab (WPF-GSRO) |
| Contextual | Tab sets, colour per set | `TabGroup` with availability tri-state; application modes | `AddContextCategory`, `ShowContextCategories`, `ActivateContextCategory`, seven tab colours (MFC-BAR, MFC-CAT) | `RibbonContextualTabGroup` (WPF-NS) |
| QAT | Title bar or below; full customization dialog | Defaults list; customization dialog left to the application (UIR-QAT) | `SetQuickAccessCommands`, `SetQuickAccessToolbarOnTop` (MFC-BAR) | `RibbonQuickAccessToolBar`, clone event for added items (WPF-NS) |
| KeyTips | Alt, multi-level, J/Y/Z conventions | Alt; automatic F, Y, Z; conflict numbering (UIR-KEYTIP) | Alt or F10, 200 ms delay; `SetKeys(keys, menuKeys)` (MFC-BAR, MFC-ELEM) | `KeyTip` property, 1 to 3 uppercase characters; `KeyTipService` (search result, RibbonGroup.KeyTip page; Unverified beyond that) |
| Live preview | Yes, product-wide | Preview and cancel-preview verbs (UIR-VERB) | Not documented here (Unverified) | Not built in; applications implement it (Inferred) |
| Persistence | Product | `SaveSettingsToStream`: QAT items, QAT dock, minimized (UIR-STATE) | `SaveToXMLFile`/`SaveToXMLBuffer` (MFC-BAR) | Application's job (Inferred) |
| Window frame | Draws QAT and contextual headers in the title bar | Same, with DWM (UIR-QAT) | `IsReplaceFrameCaption`, `DWMCompositionChanged` (MFC-BAR) | `RibbonWindow` draws QAT and contextual headers in its border (WPF-NS) |
| Later forms | Simplified one-line ribbon (OFF-SIMPLE); display options (OFF-SHOW); search box | none | none | none |

Two consequences for GUI.Forms. First, the Windows Ribbon Framework is the
cleanest *model*: a view description separate from a command layer, typed
properties, an explicit scaling policy and lazy updates. Second, every family
reaches into the window frame for the QAT and contextual headers, which
GUI.Forms treats as host chrome outside its scope (SPEC §1 "out of scope: host
window chrome").

### 1.4 Fluent.Ribbon, a fifth class model

Fluent.Ribbon is an MIT-licensed WPF library that "implements an Office-like
user interface" (FR). It is not Microsoft's WPF Ribbon. The owner's note: no
Windows Forms equivalent exists, and its look is not GUI.Forms' style; its
class model is what is studied here. Everything in this subsection is Known
from FR and FR-SRC unless marked.

**Structure.** `Ribbon` holds `Tabs` (`RibbonTabItem`), `ContextualGroups`
(`RibbonContextualTabGroup`), `QuickAccessItems`, `ToolBarItems` (items at
the tab row's end), `Menu` (the File area: a `Backstage` or an
`ApplicationMenu`) and `StartScreen`. A `RibbonTabItem` holds
`RibbonGroupBox`es; a group box is a `HeaderedItemsControl` whose items are
ordinary Fluent controls (`Button`, `ToggleButton`, `SplitButton`,
`DropDownButton`, `CheckBox`, `RadioButton`, `ComboBox`, `TextBox`,
`Spinner`, `Gallery`, `InRibbonGallery`, `ColorGallery`, `MenuItem`) or a
`RibbonToolBar` that arranges small controls in rows. `RibbonWindow` draws
the title bar, QAT and contextual headers. The source tree has 63 control
files (FR-SRC `Controls/` listing), including `StatusBar`, `TwoLineLabel` and
`TransitioningControl`.

**Group states and control sizes.** `RibbonGroupBoxState` is `Large`,
`Middle`, `Small`, `Collapsed` ("Group will collapse its content in a single
button") and `QuickAccess` ("a single button in quick access toolbar"). Each
control's `SizeDefinition` maps the three group states to its own size, in
state order: `"Middle, Middle, Small"`, or one value for all (FR sizing
page). A group's `StateDefinition` is its state sequence;
`SimplifiedStateDefinition` (default `"Large,Middle,Collapsed"`) is used in
simplified mode. In `Collapsed` and `QuickAccess` the group is a button;
Space, Alt+Down or its keytip open the popup; focus moves to the first item;
Escape closes it and focus returns to the group (FR-SRC `RibbonGroupBox.cs`).
The group box carries the launcher (`IsLauncherVisible`, `LauncherCommand`,
`LauncherKeys`, `LauncherToolTip`, `IsLauncherEnabled`), an `Icon` used by
the collapsed button and the QAT copy, `CanAddToQuickAccessToolBar`, and
`IsSeparatorVisible`.

**Adaptive resizing.** `RibbonTabItem.ReduceOrder` is a comma or space
separated list of group names. A plain name steps that group's state; a name
in parentheses, `(Name)`, instead steps the group's *scale*, which calls
`Reduce()` or `Enlarge()` once on every visible child implementing
`IScalableRibbonControl`. `InRibbonGallery` is such a child: each reduce step
removes one item column down to `MinItemsInRow`, then collapses the gallery
to a button when `CanCollapseToButton` allows. The panel walks the list in
`MeasureOverride` (enlarge while it fits, reduce while it does not) and
caches the result; when the list is exhausted the groups scroll horizontally.
With an empty `ReduceOrder` (the default) there is no reduction at all, only
scrolling (FR-SRC `RibbonGroupsContainer.cs`). The class's documentation says
entries are listed from first to reduce to last; the code reduces the last
entry first. The two disagree (FR-SRC; reported, not tested).

**Other behaviour.**

| Feature | Fluent.Ribbon |
|---|---|
| Minimize | `IsMinimized`, `CanMinimize`; Ctrl+F1 and the context menu toggle it |
| Automatic collapse | `IsAutomaticCollapseEnabled` (default true) hides the ribbon when the window is narrower than `MinimalVisibleWidth = 300` or lower than `MinimalVisibleHeight = 250` |
| Simplified ribbon | `IsSimplified`, `CanUseSimplified` (default false), Ctrl+F2; separate simplified state and layout definitions |
| QAT | `ShowQuickAccessToolBarAboveRibbon` (title bar) or below; add and remove through a built-in context menu (`AddToQuickAccessCommand`, `RemoveFromQuickAccessCommand`); whole groups, menus and galleries can be added; `CanCustomizeQuickAccessToolBar` raises an event for the application's dialog |
| Persistence | `AutomaticStateManagement` (default true) loads on start and saves on unload through a replaceable `IRibbonStateStorage` |
| KeyTips | Attached `KeyTip.Keys`; groups need keytips to open while collapsed; QAT items get keytips automatically; activation keys default to Alt and F10 and are configurable (`Ribbon.KeyTipKeys`); placement automatic or explicit (`KeyTip.AutoPlacement`); Shift+F10 is left to the context menu (FR KeyTip page) |
| ScreenTip | `Title`, `Text`, `Image`, `DisableReason`, `HelpTopic` (F1 on an open tip raises `HelpPressed`), `IsRibbonAligned` (positioned below the ribbon) |
| Galleries | `Gallery`, `InRibbonGallery` (`MinItemsInRow`, `MaxItemsInRow`, drop-down row limits, `GroupBy`, `Filters`, a `Menu` beneath, `ResizeMode`); `GalleryItem` has `PreviewCommand` on pointer enter and `CancelPreviewCommand` on pointer leave, and `IsDefinitive` |
| Contextual tabs | `RibbonContextualTabGroup` with header and authored brushes; a tab joins through `RibbonTabItem.Group`; the group's `Visibility` shows or hides it; selection rules on show and hide are not documented |
| Backstage, start screen | `Backstage` holds a `BackstageTabControl` of `BackstageTabItem`s and buttons; `StartScreen` shows a `StartScreenTabControl` with left and right content when the window opens, closed by Escape or a definitive button |
| Small-control rows | `RibbonToolBar` with `LayoutDefinitions` per size (rows, control and control-group definitions, row count per column, separators at column breaks) |

**What it adds that the other four lack.**

1. Intermediate scale steps, `(Name)`, between state steps, so a gallery
   loses one column at a time instead of jumping between fixed sizes.
2. A `QuickAccess` group state: a whole group can live in the QAT as one
   button that opens the group.
3. Control-centric size mapping: each control declares its size per group
   state, rather than the group declaring a template for all its controls.
4. Published automatic-collapse thresholds (300 × 250); the others do not
   document numbers.
5. Automatic state persistence behind a replaceable storage interface.
6. A start screen beside Backstage.
7. Configurable keytip activation keys.
8. ScreenTip fields for the disable reason and an F1 help topic.
9. Gallery filters and grouping in the in-ribbon gallery.

**What it leaves out or leaves to the application.**

1. No reduction by default: without an authored `ReduceOrder` the ribbon only
   scrolls; there is no default policy like Office's right-to-left or MFC's
   internal algorithm.
2. No size-definition templates or validation of a group against one; nothing
   rejects a reduce step that does not shrink.
3. No automatic keytip assignment for commands (only QAT items) and no
   documented conflict resolution.
4. No contextual-tab colour assignment or activation rules; colours are
   authored brushes.
5. No application modes.
6. No hover dwell or keyboard trigger documented for live preview; preview
   follows pointer enter and leave.
7. Accessibility publication was not studied here (Unverified).

For GUI.Forms the useful parts are 1, 2, 3 and 8 of the first list, the
explicit `Collapsed`/`QuickAccess` button states, and the group popup's
keyboard contract; the gaps in the second list are the same places where
this study proposes defaults (5.1, 5.3, 5.4).

---

## 2. The ribbon as Windows Forms pieces

Windows Forms on .NET 8+ has no ribbon control (Known: the W-stage catalogue
built from the official surface, `planning/WINFORMS_API_CATALOGUE.md`; no
ribbon type is listed). Commercial Forms ribbons exist (DevExpress,
Infragistics, Telerik, ComponentOne) and a Forms wrapper of the Windows Ribbon
Framework (Unverified; named only to show that Forms programs had to look
elsewhere). The table assumes a ribbon assembled from stock Forms pieces.

| Ribbon element | Nearest Windows Forms piece | What is missing |
|---|---|---|
| Ribbon container | `ToolStripContainer` top panel, or a docked `Panel` | No tab row, no command area, no scaling, no minimize |
| Tab row and pages | `TabControl` with `TabPage` | Tabs select command sets, not content; clicking the selected tab or double-clicking a tab minimizes; contextual set headers and colours; File tab; QAT and help in the row; keytips |
| Group | `GroupBox` (caption on top) or `FlowLayoutPanel` plus `Label` | Caption beneath; dialog launcher; several size layouts; collapse to a popup group |
| Large button | `Button` with `TextImageRelation.ImageAboveText`, or `ToolStripButton` with the same relation | Two-line label with the drop arrow on the second line; large/small image swap by size |
| Small button, toggle | `ToolStripButton` (`CheckOnClick`), `CheckBox` with `Appearance.Button` | Fits |
| Split and drop-down buttons | `ToolStripSplitButton`, `ToolStripDropDownButton` | Vertical split for large buttons (arrow below); keytip on each half |
| Combo, spinner, check box, text box | `ToolStripComboBox`, `ToolStripTextBox`, `ToolStripControlHost` hosting `NumericUpDown` or `CheckBox` | Leading label and width per scale size |
| Control group (joined segments) | Several `ToolStripButton`s with separators | Joined segment geometry (leading, middle, trailing roles) |
| In-ribbon gallery | Hosted `ListView` in `LargeIcon` or `Tile` view | Row scroll buttons and a "more" button; per-size column limits; live preview; drop-down expansion with categories and menu items beneath |
| Drop-down and split-button gallery | `ToolStripDropDown` hosting a `ListView` | Same as above |
| Font control | Two `ToolStripComboBox`es and toggle buttons; `FontDialog` | Composite with font preview in the list |
| Colour picker | `ColorDialog`; a custom `ToolStripDropDown` | Theme and standard colour grids, recent colours, preview |
| Application menu | `ToolStripDropDownMenu` | Two panes, recent items, large items with descriptions |
| Backstage | A full-client `Panel` with a left-aligned `TabControl` | Full-window view, fast commands, definitive commands, Esc returns |
| QAT | A small `ToolStrip`; `AllowItemReorder`; `ToolStripManager.SaveSettings` | Add from any command's context menu; customize drop-down; position switch; persistence of contents |
| KeyTips | Mnemonics (`&`) with Alt keyboard cues; `MenuStrip` menu mode on Alt or F10 | Badges, multi-level, multi-letter, automatic assignment and conflict resolution |
| Enhanced tooltip | `ToolTip` (`ToolTipTitle`, `ToolTipIcon`); `ToolStripItem.ToolTipText` | Title plus description plus shortcut plus image plus help line |
| Minimize and auto-hide | none (hide a panel) | Popup command area over the document |
| Contextual tabs | Adding and removing `TabPage`s | Set headers, colours, activation rules, J keytips |
| Adaptive scaling | `ToolStrip` overflow (`ToolStripItemOverflow.AsNeeded`, overflow button) | Multi-size group layouts, ordered reduction, popup groups |
| High DPI and image pairs | `ImageList` (one `ImageSize` per list), `ToolStrip.ImageScalingSize`, `Form.AutoScaleMode` | A per-command pair of large and small images, each with DPI variants |
| Context popup | `ContextMenuStrip` | Mini toolbar that fades with pointer distance |
| Accessibility | `AccessibleRole`, `ToolStripItem.AccessibleObject` | Group structure, keytips as access keys, contextual set names |

What no Windows Forms piece has, in summary: adaptive group scaling with
popup groups; galleries with live preview; KeyTips; contextual tab sets with
activation rules; the application menu or Backstage; the QAT with its
customization and persistence; the minimized ribbon's popup behaviour; the
ribbon keyboard model (Alt and F10, F6 regions, group jumps); a ribbon
accessibility structure; and per-command image pairs at several densities.

---

## 3. What GUI.Forms has today, and the gaps

### 3.1 Existing pieces

| Piece | Where | What it gives a ribbon | Gap for a ribbon |
|---|---|---|---|
| `Command`, `CommandState` | `include/gui_forms/commands/command/command.hpp`, `include/gui_forms/commands/types/command_types.hpp` | One authority for text, description, `icon_id`, shortcut, mnemonic, `key_tip`, availability reason, enabled, visible, checked, checkable, default, destructive; `state_changed` and `invoked` events | `key_tip` is stored and never read (its only use is the setter, `src/controls/commands/command/command.cpp:65`); `icon_id` is never resolved by any control; no large/small image pair; no value channel (spinner, combo, gallery selection, colour); no preview verbs |
| `CommandBar` | `include/gui_forms/controls/collections/command_bar.hpp` | Retained child `Button`s per item, shortcuts installed | Borrowed `Command*`; one size; no groups |
| `CommandOverflowPanel` | `include/gui_forms/controls/scrollable_control/container_control/command_overflow_panel/command_overflow_panel.hpp`; `experiments/M12P35_COMMAND_OVERFLOW_PROJECTION.md` | Whole-group collapse by unique priority, never by coordinates; focus protection; validation before mutation; snapshot with stable IDs | By its own record "priority collapse, not a general ribbon reflow"; groups vanish into a consumer-built menu rather than shrinking through sizes or becoming popup groups |
| `ToolStrip`, `StatusStrip` (branch `claude/w-tool-strip`, commit `b71f3b4`) | `include/gui_forms/controls/tool_strip/tool_strip.hpp`, `src/controls/tool_strip/tool_strip.cpp` | Items as records in an `item` item system; button, label, separator, drop-down and split button; shared `Command`; check-on-click; overflow menu; every part through `paint_part`; semantics with one virtual child per item | Horizontal only; no hosted controls; no item tool tips (commit message); overflow glyph borrows `PartMark::arrow_down`; split button's semantic role is plain `button` |
| `DropDownButton` | `include/gui_forms/controls/button_base/button/drop_down_button/drop_down_button.hpp` | Menu and split modes; disclosure on the right or bottom edge ("as on a ribbon button", its docstring) | A child control per command; popup content is the consumer's |
| `ContextMenu`, `MenuItemSpec` | `include/gui_forms/components/context_menu/context_menu.hpp` | Popup menus bound to `Command`s, check and radio rows, submenus (depth 8, 1024 items); focus restoration | No large rows with descriptions, no galleries inside a menu |
| `MenuStrip` | `include/gui_forms/controls/menu_strip/menu_strip.hpp` | Top-level menu; mnemonics with duplicate cycling | Not a ribbon; UX-RIBBON says not to combine a ribbon with a menu bar |
| `AnchoredPopupLayer` | `include/gui_forms/controls/panel/anchored_popup_layer/anchored_popup_layer.hpp` | Popup overlay anchored to a control, edge avoidance, click-away and Escape *reported* to the owner | Usable as is for popup groups, the minimized command area, drop-down galleries |
| `TabControl` | `include/gui_forms/controls/scrollable_control/container_control/tab_control/tab_control.hpp` | Tab item system: `tab` (face), `tab_label`, `tab_focus`, `tab_seam`, state `selected` | Tabs own content pages; no contextual sets, no click-to-minimize, no File tab; overflow and images are W P3 work |
| `ImageList` | `include/gui_forms/image_list/image_list/image_list.hpp` | Keyed images, PNG only, density and state variants | One `image_size` per list, so a large/small pair needs two lists or a keyed convention |
| Accelerators and mnemonics | `include/gui_forms/window/window.hpp` (`KeyGesture`, `register_accelerator`); `src/core/window/window.cpp:776` (`dispatch_dialog_key`); `experiments/M12P7_DIALOG_KEYS_AND_MNEMONICS.md` | Alt+letter reaches `dispatch_mnemonic` after the focused route and accelerators | No Alt-alone "menu mode", no F10 entry, no keyboard-cue state, no multi-key sequences; M12P7 lists "ToolStrip collision arbitration" as open |
| `ToolTip` | `include/gui_forms/components/tool_tip/tool_tip.hpp` | Hover and focus tool tips per control | One text string per control; no item-level tips; no title, description, shortcut, image |
| Semantics | `include/gui_forms/semantics/types/semantic_types.hpp`; host maps in `src/host/windows/accessibility/windows_accessibility.cpp`, `src/host/macos/application/macos_host.mm`, `src/host/linux/accessibility/linux_accessibility.cpp` | Roles include `group`, `tab_group`, `tab`, `toolbar`, `menu`, `menu_item`, `list`, `list_item`, `combo_box`, `numeric_field`, `check_box`, `button`, `tool_tip` | No split-button, menu-button or toggle-button role; `SemanticNode` has no access-key or shortcut field, no position-in-set; Windows publishes MSAA only (`get_accKeyboardShortcut` returns `S_FALSE`); stage A brief: "There is no UI Automation provider" |
| By-role paint path | `include/gui_forms/theme/part_painting/part_painting.hpp` | `paint_part` from `PartRole`, `AnatomyStateSet`, recipe family, text, image or `PartMark` | `ControlVisualRole` has `command_button` but no tab, command-plane or key-tip family; until T4 the path draws from `windows-professional`, so a ribbon built now will not show the cobalt plane or the bulge |
| Atom vocabulary | `include/gui_forms/anatomy/types/anatomy_types.hpp` | 31 part roles, 34 standard states, custom states, item systems | `maximum_item_systems = 4` and `maximum_control_parts = 32` bound how much one control can declare (see 4.1) |

### 3.2 Plan Paint, one realization

Read in `C:\Users\Shadow\plan-paint\src\forms\ribbon.cpp`, `ribbon.hpp`,
`editor.cpp` and `assets\screenshots\macos-home-1.1.jpg`. Plan Paint is the
reference for *look* (SPEC §11; RENDER), not the definition of a ribbon.

| Ribbon element | Plan Paint | Compared with the canonical ribbon |
|---|---|---|
| Composition | One `Ribbon` control; every command a child `Button` or `DropDownButton` at an absolute rectangle on a 1200 (Home) or 1280 (other pages) unit grid, multiplied by `width / 1200` (`Ribbon::arrange`) | No adaptive scaling: everything shrinks proportionally, fonts shrink to fit down to 8–9 units, icon size is picked from the scaled width (32, 24, 16, 12) |
| Tabs | Buttons with a tab theme (`RibbonTabButton`, `make_tab_theme`); selected tab 29 units high, others 23 | The owner's tab stack (RENDER §4) |
| File | A `MenuStrip` "File" menu drawn as the solid blue File tab (`Editor::rebuild_file_menu`) | An application menu, not Backstage |
| Contextual tabs | The tool tab's label follows the current tool ("Pencil", "Brush"); Gradients appears only while a gradient fill is chosen; Materials is disabled without ink (`Ribbon::show_page`) | Contextual behaviour without set headers or colours |
| Groups | Captions and etched dividers painted at fixed x positions in `on_paint`, `paint_separator` | No group objects, no launchers, no popup groups |
| Gallery | Shapes: 21 buttons in a 7×3 grid in a white well, plus an 18-unit "shapes-menu" button | No scrolling; the well is painted by the ribbon |
| Popups | `AnchoredPopupLayer` with a panel of buttons, opened from `DropDownButton`s (`Ribbon::dropdown`) | Drop-down galleries in all but name |
| Minimize | Clicking the selected tab toggles `collapsed_`; height 143 to 27 units; another tab click expands and stays expanded | Minimize without the temporary popup |
| QAT | None; Save, Undo, Redo sit in an uncaptioned first group | Canonical ribbons put these in the QAT |
| KeyTips | None (`set_use_mnemonic(false)`) | |
| Help | "?" at the tab row's right end; "Help! English!" in other languages | Matches the Help button |
| Icon-only commands | Crop, resize, cut, copy, undo, redo and the tools are icon-only with tool tips | See decision 8.B2 (labels) |
| Tab order | Child order (`set_tab_index` per child) | A task order, not a keyboard model |

What to learn from it: the look (tab stack, plane, dividers, captions, gold
active tool, wells), the popup composition, the contextual tool tab, and that
the owner's own program reached for absolute layout because nothing adaptive
existed.

### 3.3 Gaps by element

| Element | Status in GUI.Forms |
|---|---|
| Ribbon control with tab row and command area | Missing |
| Groups with captions, dividers, launchers | Missing (Plan Paint paints them by hand) |
| Button family at large/medium/small sizes | Partial: `ToolStrip` items and `DropDownButton` exist at one size |
| Adaptive scaling, popup groups | Missing; `CommandOverflowPanel` is whole-group priority collapse only |
| Hosted controls in groups | Missing in `ToolStrip`; Plan Paint hosts `ComboBox`, `NumericUpDown`, `CheckBox` as plain children |
| In-ribbon and drop-down galleries | Missing; no `ListView` yet (W P3) |
| Live preview | Missing |
| KeyTips and Alt/F10 mode | Missing; `key_tip` field unused |
| Contextual tab sets, application modes | Missing |
| Application menu, Backstage | Missing (Plan Paint uses a `MenuStrip`) |
| QAT, customization, persistence | Missing; runtime configuration contract is a candidate only (RES §7) |
| Minimize, popup command area, auto-collapse | Missing |
| Enhanced tooltips | Missing |
| Ribbon accessibility structure | Partial: roles for groups, tabs, toolbars exist; split/menu/toggle roles, access keys, UIA missing |
| Image pairs and densities | Partial: `ImageList` density variants, one size per list |
| Font control, colour picker | Missing as ribbon controls (`color_value_editor` exists under `src/controls/panel/`, not a drop-down colour grid) |
| Context popup (mini toolbar) | Missing |

---

## 4. Proposed anatomy in GUI.Forms terms

Every vocabulary gap below is marked **[vocabulary decision]**. None is a
done deal: under SPEC §12.7 adding an atom is an owner decision that changes
the specification, `anatomy_types.hpp` and the schema together. Where a gap
exists, the column "nearest existing atom" says what a builder would use in
the meantime, as the W brief requires. The gaps are collected as interview
questions in 8.G.

### 4.1 Control types and item systems

The T1 limits shape the split: one control may declare at most four item
systems and 32 own parts (`anatomy_types.hpp`). A ribbon needs at least seven
repeated kinds (tabs, tab sets, groups, commands, gallery items, QAT items,
keytip badges), so it cannot be one control. Proposed types:

| Type | Kind | Item systems | Notes |
|---|---|---|---|
| `Ribbon` | Control | `tab`, `tab_set`, `group`, `command` | Owns the tab row, the application button, the minimize actuator, the help button and the command area of the selected tab. Button-family commands are records, as in `ToolStrip` (T1 decision 5: repeated items are item systems). |
| hosted controls | real child controls | (their own) | Combo box, spinner (`NumericUpDown`), check box, text box, font control, colour picker. T1 decision 5: "A row that hosts interactive controls keeps them as real child controls." |
| `RibbonGallery` | Control | `gallery_item`, `gallery_category` | In-ribbon form, and the content of drop-down and split-button galleries. |
| `QuickAccessToolbar` | Control (derived from `ToolStrip` or composing it) | `item` | Plus customize and overflow actuators. |
| `KeyTipLayer` | Control in a popup layer | `key_tip` | One per window while keytip mode is active. |
| `RibbonApplicationMenu` | Component like `ContextMenu` | (menu rows) | Two-pane menu; or `BackstageView` per decision 8.A3. |
| `RibbonToolTip` | Component | none | Enhanced tooltip, per command or item. |

Whether button-family commands are records or child controls is decision
8.A2.

### 4.2 `Ribbon` own parts

| Part name | PartRole | States | Nearest existing atom / gap |
|---|---|---|---|
| `region` | region | | |
| `tab_rail` | face | | |
| `command_plane` | face | `open`, `closed` (minimized) | The plane's cobalt gradient is a theme recipe; `ControlVisualRole` has no command-plane family yet (T4) |
| `plane_keyline` | keyline | | White top line and shade line (SPEC §11 item 4) |
| `application_button` | face, composed from `button` | hot, pressed, open, focus_visible | The solid blue File tab (SPEC §11 item 5) |
| `application_label` | label | | |
| `minimize_button` | actuator, composed from `button` | hot, pressed | |
| `minimize_glyph` | glyph | | **[vocabulary decision]** no `PartMark::chevron_up` (only `chevron_down`, `chevron_right`); nearest `arrow_up` |
| `help_button` | actuator, composed from `button` | hot, pressed | |
| `help_glyph` | glyph or label | | **[vocabulary decision]** no help mark; nearest a `label` with "?" as Plan Paint does |
| `scroll_left`, `scroll_right` | actuator | hot, pressed | Last-resort scrolling (5.1); marks `arrow_left`, `arrow_right` exist |
| `focus` | focus | | |

Control-level states: `focused`, `focus_visible`, `disabled`, `deactivated`,
`key` and the participation states as for every control. Ribbon-wide modes
that are not in the standard set: minimized (proposed: `closed` on
`command_plane`), "popup open while minimized" (`open` on `command_plane`),
keytip mode active (**[vocabulary decision]**: a custom state such as
`key_tips_shown`, or nothing, since the badges live in their own layer).

### 4.3 Item systems of `Ribbon`

**`tab`** (one per visible tab, core and contextual):

| Part | PartRole | Notes |
|---|---|---|
| `tab` | face | As `TabControl`'s `tab`; RENDER §4 stack: abutting, overlapping, selected slightly larger, open bottom edge |
| `tab_label` | label | |
| `tab_seam` | seam | |
| `tab_focus` | focus | |

States: `hot`, `selected`, `disabled`, `focus_visible`, `open` (selected while
the minimized popup shows). Contextual membership: **[vocabulary decision]**
either a custom item state `contextual` or a value (the tab-set index).

**`tab_set`** (one per shown contextual set):

| Part | PartRole | Notes |
|---|---|---|
| `tab_set_band` | keyline or face | The coloured header strip over its tabs. **[vocabulary decision]** the set's colour is data the theme must read: no value type or engine input names "accent index of this item" today |
| `tab_set_caption` | caption | "Picture Tools" style label |

**`group`** (one per group of the selected tab, at its current size):

| Part | PartRole | Notes |
|---|---|---|
| `group_region` | region | |
| `group_divider` | divider | Etched pair fading at both ends (SPEC §7.2 rule 3; Plan Paint `paint_separator`) |
| `group_caption` | caption | Beneath the group (SPEC §8.1 "divider, caption") |
| `group_launcher` | actuator, composed from `button` | Dialog box launcher |
| `group_launcher_glyph` | glyph | **[vocabulary decision]** no launcher mark (a small corner arrow); nearest `arrow_down` or `chevron_right` |
| `group_popup_face` | face, composed from `button` | The collapsed popup-group button; present only at Popup size |
| `group_popup_icon` | icon | UX-RIBBON: the 16×16 icon of the group's main command inside a 32×32 container |
| `group_popup_label` | label | |
| `group_popup_glyph` | glyph | `chevron_down` |
| `group_focus` | focus | Focus on the popup button |

States: `hot`, `pressed`, `open` (popup shown), `disabled`, `focus_visible`.
The group's current size (large, medium, small, popup) is a layout fact.
**[vocabulary decision]** whether themes may select it (a custom state per
size, or a published value) or only see which parts are present.

**`command`** (one per button-family command visible in the command area):

| Part | PartRole | Notes |
|---|---|---|
| `command_face` | face | Family `command_button`. In the default theme this is the dithered bulge (SPEC §3.5, §12.6; RENDER §3). An edge fails it |
| `command_icon` | icon | Large (32) or small (16) image by current size; density variant by display scale |
| `command_label` | label | One or two lines; may be absent at small size (decision 8.B2) |
| `command_arrow` | actuator | The split half or drop-down zone; bottom half for a large split button, right part for a small one (as `DropDownButtonEdge`) |
| `command_arrow_glyph` | glyph | `arrow_down` |
| `command_segment_seam` | seam | Joins in a control group (leading, middle, trailing roles from parent order, SPEC §8.2 rule 3) |
| `command_focus` | focus | |

States: `hot`, `pressed` (per half for split buttons, as `ToolStrip` already
does), `checked` (toggle), `active_tool` (Plan Paint's gold current tool;
RENDER §2 gold bulge), `open`, `disabled`, `focus_visible`. Leading, middle,
trailing and sole positions inside a control group come from order, not
coordinates.

### 4.4 `RibbonGallery`

| Part | PartRole | Notes |
|---|---|---|
| `well` | well | Plan Paint gallery well `#F8FBFF` to `#FFFFFF`, border `#94ACC8` (SPEC §11 item 4) |
| `scroll_up`, `scroll_down` | actuator | Marks `arrow_up`, `arrow_down` |
| `more_button` | actuator | Opens the drop-down form |
| `more_glyph` | glyph | **[vocabulary decision]** no "more" mark (a down arrow under a bar); nearest `chevron_down` |
| `gripper` | grip | Resize gripper in the drop-down form; `grip_dots` exists |
| `focus` | focus | |

Item system `gallery_item`: `item_face` (face), `item_selection`
(selection), `item_icon` (icon), `item_label` (label), `item_focus` (focus);
states `hot`, `selected`, `disabled`, `focus_visible`. Item system
`gallery_category`: `category_caption` (caption), `category_divider`
(divider). Whether gallery items take the bulge is taste question 8.T2. Live
preview needs no new state: it is behaviour driven by `hot` and focus.

### 4.5 `QuickAccessToolbar`, `KeyTipLayer`, menus, tooltips

| Control | Parts | Gap |
|---|---|---|
| `QuickAccessToolbar` | `ToolStrip`'s parts and item parts; `customize_button` (actuator) and `customize_glyph` (glyph); `overflow_button` (actuator) and `overflow_glyph` (glyph) | **[vocabulary decision]** no customize mark (bar over a down arrow) and no overflow mark (double chevron); `ToolStrip` already borrows `arrow_down` for its overflow |
| `KeyTipLayer` item system `key_tip` | `tip_face`, `tip_label` (label) | **[vocabulary decision]** no key-tip role; nearest `chip` ("small flat tinted status cell"). States: `disabled` (tip of an unavailable command, drawn dimmed in Office; Unverified), and **[vocabulary decision]** a "refactored" or "prefix matched" state (UIR-KEYTIP draws refactored tips in a different colour) |
| `RibbonApplicationMenu` | Rows: face, icon (large), label, content (description line), disclosure; recent-items pane: caption, rows with numbered keytips | No new roles needed (Inferred) |
| `BackstageView` (if chosen) | Navigation item system (face, icon, label, selection), page face, fast-command rows | No new roles needed (Inferred) |
| `RibbonToolTip` | `face`, `shadow`, title (label), description (content), shortcut (label), image (icon), help line (content) | `ToolTip` is one string today; needs a component or an extension |

### 4.6 Semantic roles each host needs

The core vocabulary today (`semantic_types.hpp`) lacks three roles and two
node fields. Host mappings below are proposals; entries marked (I) are
Inferred, not checked against Office's own publication.

| Ribbon element | GUI.Forms role | Windows UIA control type | Windows MSAA (current host) | macOS NSAccessibility | Linux AT-SPI |
|---|---|---|---|---|---|
| Ribbon | `group` (or `toolbar`) | Pane or ToolBar (I) | `ROLE_SYSTEM_GROUPING` | `AXGroup` | `ROLE_PANEL` (I) |
| Tab row | `tab_group` | Tab | `ROLE_SYSTEM_PAGETABLIST` | `AXTabGroup` | `ROLE_PAGE_TAB_LIST` |
| Tab | `tab` | TabItem | `ROLE_SYSTEM_PAGETAB` | `AXRadioButton` (as AppKit tab views publish tabs; I) | `ROLE_PAGE_TAB` |
| Command area | `group` | Group or Pane (I) | `ROLE_SYSTEM_PROPERTYPAGE` or grouping (I) | `AXGroup` | `ROLE_PANEL` |
| Group | `group` named by caption | Group | `ROLE_SYSTEM_GROUPING` | `AXGroup` | `ROLE_PANEL` or `ROLE_GROUPING` (I) |
| Popup group button | **[decision]** `menu_button` | Button with ExpandCollapse | `ROLE_SYSTEM_BUTTONMENU` | `AXMenuButton` | `ROLE_PUSH_BUTTON_MENU` (I) |
| Button | `button` | Button | `ROLE_SYSTEM_PUSHBUTTON` | `AXButton` | `ROLE_PUSH_BUTTON` |
| Toggle button | **[decision]** `toggle_button`, or `button` plus `checked` | Button with Toggle pattern | push button with `STATE_SYSTEM_PRESSED` (I) | `AXCheckBox` or `AXButton` with a value (I) | `ROLE_TOGGLE_BUTTON` |
| Split button | **[decision]** `split_button` | SplitButton | `ROLE_SYSTEM_SPLITBUTTON` | no split role; a group of `AXButton` and `AXMenuButton` (I) | no split role; a panel of push button and push-button-menu (I) |
| Drop-down button | **[decision]** `menu_button` | Button with ExpandCollapse | `ROLE_SYSTEM_BUTTONDROPDOWN` (I) | `AXMenuButton` | `ROLE_PUSH_BUTTON_MENU` (I) |
| Gallery | `list` (or a grid role, **[decision]**) | List with ListItems (I) | `ROLE_SYSTEM_LIST` | `AXList` | `ROLE_LIST` |
| Gallery item | `list_item` | ListItem with SelectionItem and Invoke (I) | `ROLE_SYSTEM_LISTITEM` | `AXStaticText`/cell in a list (I) | `ROLE_LIST_ITEM` |
| Combo, spinner, check box | existing | ComboBox, Spinner, CheckBox | existing | existing | existing |
| QAT | `toolbar` | ToolBar | `ROLE_SYSTEM_TOOLBAR` | `AXToolbar` | `ROLE_TOOL_BAR` |
| Application menu | `menu` | Menu | `ROLE_SYSTEM_MENUPOPUP` | `AXMenu` | `ROLE_MENU` |
| Enhanced tooltip | `tool_tip` | ToolTip | `ROLE_SYSTEM_TOOLTIP` | `AXHelpTag` (I) | `ROLE_TOOL_TIP` |

Node fields:

- **Access key / keytip path [vocabulary decision].** UIA has `AccessKey`
  and `AcceleratorKey` string properties (UIA-PROP); MSAA has
  `get_accKeyboardShortcut` (GUI.Forms returns `S_FALSE` today); AT-SPI's
  action key binding is a string carrying mnemonic, sequence and accelerator
  (Unverified format). macOS has no standard keytip attribute (Inferred).
  `SemanticNode` has neither field.
- **Position in set and set size** for tabs, commands and gallery items
  (UIA `PositionInSet`, `SizeOfSet`). Missing.
- **Has popup** for drop-down, split and popup-group buttons. The `expanded`
  state exists; a "has popup" fact does not.

The Windows host has no UIA provider (stage A brief), so on Windows today only
the MSAA column applies; UIA is stage A's deliverable 4.

---

## 5. The hard algorithms

### 5.1 Adaptive group scaling

**Inputs.**

- For each group, an ordered list of *size definitions*, largest first:
  `large`, optionally `medium`, optionally `small`, optionally `popup`. Each
  size definition places every command of the group with an image size
  (`large`, `small`), a label flag, and a row/column arrangement (one column
  of large items; up to three rows of small items; column breaks with or
  without dividers; control groups that stay together). This is the Windows
  Ribbon Framework model (`SizeDefinition`, `GroupSizeDefinition`,
  `ControlSizeDefinition`, `Row`, `ColumnBreak`, `ControlGroup`; UIR-SIZE)
  and the WPF model (`RibbonGroupSizeDefinition`, `RibbonControlSizeDefinition`;
  WPF-NS).
- For each tab, a *scaling policy*: the ideal size of each group and an
  ordered list of reduction steps, each naming one group and the size it moves
  to (UIR-SCALE). A step must move its group to a strictly smaller size than
  the group's current one; a policy that violates this is rejected when the
  ribbon definition is validated, before anything is built (house style §9).
- Measured widths: each command's width at each image size and label state,
  from the text engine at the current font, text scale and display scale.

**Intermediate steps.** Fluent.Ribbon adds a second kind of step between
size changes: a scale step that asks scalable children (a gallery) to give
up one column (1.4). The same fits here as a step kind `scale(group)`, valid
only while some child of the group can still shrink; it keeps `width[]`
non-increasing, so the algorithm below is unchanged. Whether to admit it is
question 8.B4.

**Default policy.** When a tab gives no policy, generate one deterministically:
all groups at their largest size; then, repeatedly from the rightmost group
that can still shrink to the leftmost, move one group one size down (Office:
"always from right-to-left", OFF-2010); popup last for every group except a
group of one button (UX-RIBBON "Don't scale groups containing a single
button to a pop-up group icon"). MFC behaves the same way in spirit:
explicit order first, internal algorithm when the list runs out (MFC-CAT).

**Algorithm.**

```text
state[0] = ideal sizes
for k in 1..steps:  state[k] = state[k-1] with step k applied
width[k] = sum over groups of group_width(group, state[k][group])
         + gaps + tab-level fixed parts
(width is non-increasing in k because every step shrinks one group)

arrange(available):
    if width[0] <= available: use state[0]
    else: k = smallest index with width[k] <= available (binary search)
    if none: use state[last]; enable scroll_left/scroll_right; scroll so the
             focused command, else the first group, is visible
    if the window is below the hide threshold (decision 8.B3): command area
             hidden ("collapsed" in UIR-STATE terms), tab row kept
```

- **Cost.** `width[]` is computed once per (tab content generation, font,
  text scale, display scale, theme metrics generation) and cached; a resize
  is a binary search plus one placement, with no allocation per step (house
  style §3). Command state changes that alter width (label text, visibility)
  bump the tab content generation (GF007: declared invalidation). Changes
  that do not (enabled, checked) invalidate paint only.
- **Layout timing.** The ribbon's measure and arrange run inside the existing
  bounded layout pass and its read barrier (LEDGER GF006, L4); the ribbon is a
  control-specific layout family (GF008).
- **Determinism.** Results depend only on declared order and measured
  widths, never on coordinates or incidental child order (as
  `CommandOverflowPanel`'s contract, M12P35).
- **Hidden versus disabled.** A command hidden by visibility leaves its
  layout; UX-RIBBON prefers disabling to hiding because hiding makes the
  ribbon "unstable". Visibility changes therefore re-run scaling; enabled
  changes do not.
- **Focus.** If the focused command's group becomes a popup group, focus
  moves to that group's popup button (Inferred: keeps the keyboard position
  without opening anything). If the popup is open when its group grows back,
  the popup closes and focus moves to the same command in the ribbon.
- **Popup group behaviour.** The popup-group button opens an
  `AnchoredPopupLayer` below it containing the group at its ideal size
  (Inferred from the canonical screenshots; Office shows the full group).
  Escape or click-away closes it and returns focus to the button; running a
  command closes it; a drop-down inside it opens a nested popup. KeyTip:
  `Z` plus a letter (UX-RIBBON).
- **Large text.** Text scale participates in measurement; when text grows,
  the same steps apply, then groups collapse by an accessibility-specific
  priority order where one is declared (A11Y-001 T5A). Essential names never
  clip silently; a popup group keeps its full name.
- **Labels at small size.** The canonical Small size hides labels
  (`IsLabelVisible="false"`, UIR-SIZE). The design language says commands
  stay labelled (SPEC §6.3: V203, V201, V209, M083; M064 R). Decision 8.B2.

**Tests that would prove it.** Golden layouts (group sizes and part
rectangles) for a fixture tab at a ladder of widths; `width[]`
non-increasing; a policy with a non-shrinking step rejected with a
diagnostic and the old ribbon kept; identical results across hosts; focus
transfer to the popup button; text scale 1.0, 1.5, 2.25 (A11Y-001 §3.5 gate
7 uses 225 %).

### 5.2 Gallery layout and scrolling

**In-ribbon form.**

```text
inputs: item size (w, h), same for all items (UX-RIBBON)
        rows: floor(content_height / h), at least 1
        per group size: min_columns, max_columns (UIR-SIZE attributes)
columns = clamp(floor((available_width - scroll_column) / w), min_columns, max_columns)
first_visible_row: kept so the selected item's row is visible after
                   a selection change or reopening; otherwise user-scrolled
scroll_up enabled  when first_visible_row > 0
scroll_down enabled when first_visible_row + rows < total_rows
more_button always present; opens the drop-down form
```

The gallery's width enters the group's size definitions like any command;
at popup size the gallery becomes a drop-down gallery button (UIR-SIZE
`InRibbonGalleryAndButtons-GalleryScalesFirst`).

**Drop-down form.** Items flow in rows of a declared column count, grouped by
category with caption and divider; a declared number of rows before the list
scrolls; menu groups (ordinary menu rows) beneath the items; optional resize
gripper (UIR-SIZE `VerticalMenuLayout Rows`, `Gripper`). Only visible rows
are realized as items (T1 decision 5); the semantic tree still exposes the
whole logical collection with stable item IDs (A11Y-001 A4A).

**Keyboard.** Arrows move by item in the grid (Up/Down by row, scrolling the
in-ribbon window by one row when needed); Home/End; Page Up/Down in the
drop-down form; Enter or Space executes; Escape closes the drop-down and
returns focus to the in-ribbon gallery or its button.

**Live preview protocol.**

```text
hover dwell on item i (delay: decision 8.E1)   -> preview(i)
pointer moves to item j                          -> preview(j)  (implies cancel of i)
pointer leaves the gallery, popup closes, Escape -> cancel_preview()
click or Enter on i                              -> cancel_preview() is not sent;
                                                    execute(i) commits
any other input to the document or window loss  -> cancel_preview()
```

Rules: a preview is never an undo entry; preview and cancel are delivered on
the UI thread in that order; at most one preview is outstanding; UX-RIBBON's
"apply and restore within 500 milliseconds" is the application's budget, and
GUI.Forms records the measured round trip in the window counters so it can be
checked. Whether keyboard focus movement in a gallery also previews is
Unverified for Office; proposed: yes, after the same dwell, so keyboard users
get the same evidence.

**Tests.** Column clamp at each width; scroll enable states; selected item
kept visible; preview/cancel/execute event order for scripted pointer and
keyboard sequences; no preview after the popup closes; virtualization: a
10,000-item gallery realizes only visible rows.

### 5.3 KeyTips and the Alt/F10 keyboard model

**State machine** (per window; one ribbon per window):

```text
Idle
  Alt pressed and released with no other key between      -> Level1
  F10                                                     -> Level1
  Alt held + key k (Alt+H)                                -> Level1, then feed k
Level1 (tips on application button, QAT items, tabs, contextual tabs, help)
  typed character c        -> prefix += c; hide tips not starting with prefix
  prefix equals one tip    -> act(tip)
  prefix matches none      -> ignore c (Inferred; Office beeps — Unverified)
  Escape                   -> Idle, tips hidden, focus stays on the tab row
                              (OFF-KEYS: first Esc hides, a further Esc
                              returns focus to the document)
  Left/Right/Tab/arrows    -> leave tip mode, keyboard navigation (5.7)
  Alt or F10 again         -> Idle
  pointer press anywhere, window deactivation, focus change -> Idle
act(tip):
  tab              -> select it; Level2 for that tab's commands and Z tips
  application menu -> open it; its rows' tips shown
  QAT item         -> execute (or open, for a drop-down); Idle
Level2
  command          -> execute; Idle
  drop-down, split arrow, gallery -> open; tips of its menu rows shown
  popup group (Z?) -> open popup; tips of its commands shown
  Escape           -> back to Level1
```

MFC delays the display of tips by 200 ms after Alt is pressed so that Alt
chords are not disturbed (MFC-BAR); the state machine above only *shows* the
layer on release, which has the same effect (Inferred).

**Where it sits in the input order.** The interview's keyboard order is:
physical key preview, then "mnemonic/command/default-button resolution", then
key-down, text, key-up (LEDGER, "Proposed corrected event ordering"). The
ribbon's keytip mode belongs in step 2, at window level, before control
mnemonics, while a ribbon is present (Inferred). Today
`Window::dispatch_dialog_key` sends Alt+letter straight to
`dispatch_mnemonic` (`src/core/window/window.cpp:776`); a ribbon needs a
window-level hook that can claim Alt release, F10 and Alt+letter. A popup
focus scope keeps first refusal, as `AcceleratorOptions` already states for
accelerators.

**Assignment.**

1. Author-given keytips win. Scopes: Level1 = application button, QAT, tabs,
   help; each tab's Level2 = its commands, launchers and popup groups; each
   menu = its rows. Validation rejects duplicates within a scope when the
   author asked for strictness; otherwise conflicts are resolved by step 3.
2. Unassigned tips: tabs from the first letter of the label, then a
   distinctive consonant; commands from the first letter of the first or
   second word, then two letters; menus one letter (UX-RIBBON). Popup groups
   `Z` + letter; contextual tabs `J` + letter; QAT `1`–`9`, then
   two-character numbers (UX-RIBBON; the exact Office sequence for the
   tenth QAT item onward is Unverified). The application button `F`.
3. Conflicts: append `1`, `2`, `3` in declaration order to every member of a
   colliding set (UIR-KEYTIP). The result is a pure function of declarations
   and labels: same input, same tips, on every host.
4. Tips come from the language assembly, which carries "mnemonic/access-key
   metadata" per string (RES §2.2), so each language has its own set;
   assignment re-runs on a language change (RC003: live replacement).

**Host differences.** Office for Mac has keytips off by default (OFF-KEYS).
The Option key on macOS composes characters, so Alt-release as an entry is a
Windows and Linux convention (Inferred). The core must not encode one host's
convention (AGENTS "GIVEN"); this is decision 8.C1.

**Tests.** Scripted key sequences produce the expected executions and focus
positions; Escape stack; deterministic assignment and conflict numbering for
fixtures; a ribbon in a popup focus scope does not steal Alt; localization
switch re-assigns; the keytip layer's realized tips match the scope.

### 5.4 Contextual tabs

**Model.** A tab set has a stable ID, caption, tabs and an availability of
`not_available`, `available` or `active` (UIR-TABGROUP). The application sets
availability from selection state; the ribbon never infers it. Application
modes are a bit set; each core tab and group declares the modes it belongs
to; changing modes replaces the visible core tabs atomically (Windows Ribbon
Framework application modes; Inferred mechanics).

**Placement and colour.** Sets follow the core tabs in declaration order;
each set's band spans its tabs; a set is given a colour index different from
every other displayed set and keeps its previous index when possible
(UX-RIBBON). The colours come from the theme as an ordered accent ramp
(decision 8.F1; vocabulary gap in 4.3).

**Activation.**

```text
available -> active (application asks):      select first tab of the set
user double-clicks or inserts an object:     application sets active
set leaves while one of its tabs is selected: select the previously selected
                                              core tab if remembered, else
                                              Home (first core tab) (UX-RIBBON)
set returns for the same object type soon after: reselect the tab last used in
                                              that set (UX-RIBBON)
```

"Soon after" is not defined by any source read here (Unverified); proposed:
remembered until a different set or a core tab is chosen.

**Accessibility.** A contextual tab's accessible name includes its set
caption, and its appearance and removal are announced as structure changes
(Inferred). KeyTips use `J` (5.3).

**Tests.** Availability transitions; tab selection on removal; colour
assignment uniqueness; mode switches leave no stale tab; semantic tree
changes in order.

### 5.5 QAT placement and overflow

- **Placement.** Canonical ribbons draw the QAT in the title bar (UIR-QAT).
  GUI.Forms does not theme host window chrome (SPEC §1) and the compositor
  stage only reports capability (T5). So the QAT goes inside the client area:
  on the tab row beside the application button ("above") or as a row beneath
  the command plane ("below"), user-switchable (decision 8.D1).
- **Content.** Commands only; each must exist in the ribbon or the defaults
  list (UIR-QAT). At most a declared maximum (20 in UIR-QAT). Galleries and
  combo boxes appear as drop-down buttons in the QAT (Inferred).
- **Overflow.** Items that do not fit move to an overflow popup in strip
  order, as `ToolStrip` does today; the customize button stays visible.
- **Customization.** The customize drop-down lists the defaults with check
  marks, then "More Commands…" (an application handler), "Show below the
  ribbon", "Minimize the ribbon" (UIR-QAT, UIR-STATE). Every ribbon command's
  context menu (Shift+F10 or secondary click) offers "Add to Quick Access
  Toolbar"; QAT items offer "Remove". Commands in context popups do not
  (UIR-QAT).
- **Persistence.** QAT command IDs in order, QAT position and the minimized
  state are session/layout state (RES §2.4), written atomically; unknown
  command IDs are preserved but inactive (RES §7 "preserve but do not
  activate unknown keys"). The last selected tab is not persisted
  (UX-RIBBON).
- **Tests.** Add, remove, reorder; overflow at widths; persistence round
  trip including an unknown ID; position switch; keytips renumber.

### 5.6 Minimized ribbon popup behaviour

```text
Expanded --(double-click a tab | Ctrl+F1 | minimize button | context menu)--> Minimized
Minimized:
  layout height = tab row only; reported once, on the transition (Plan Paint's
  editor already re-bases its canvas on a ribbon height change)
  tab click            -> Popup(tab): command area shown in an AnchoredPopupLayer
                          over the content, below the tab row, full ribbon width;
                          tab state selected + open
Popup(tab):
  click same tab       -> Minimized (popup closed)
  click another tab    -> Popup(other)
  command executed     -> Minimized (Inferred: spinners and text fields keep it
                          open while editing)
  Escape               -> Minimized, focus returns to the tab (Inferred)
  click outside        -> Minimized; whether the click also reaches the document
                          is decision 8.D3 (Office behaviour Unverified)
  double-click a tab | Ctrl+F1 -> Expanded
  Alt                  -> Level1 tips; choosing a tab opens Popup(tab) with Level2
Collapsed (automatic, window too small): ribbon hidden; returns when the window
  grows (UIR-STATE)
```

The popup is an E4 overlay (SPEC §3.4) and the command area inside it is laid
out by the same scaling algorithm at the window's width. While minimized, the
command area is absent from the semantic tree; in Popup it is present as the
popup's content (A11Y-001 §3.3: "A collapsed pane leaves the accessible tree
when it ceases to participate").

**Tests.** Every transition above; layout height reported only on
Expanded/Minimized changes; popup content equals the expanded command area
at the same width; semantic tree presence.

### 5.7 Focus and accessibility model

- **Entry and exit.** Alt or F10 puts focus on the selected tab with tips
  shown; Escape from the tab row returns focus to the control that had it
  before entry (the window remembers it, as popup focus scopes already
  restore the invoker's focus). F6 cycles regions (ribbon, work pane, side
  panels, status) (OFF-KEYS); GUI.Forms has no region-cycling concept today:
  it needs a window-level ordered list of landmark regions (Inferred gap).
- **Within the ribbon.** Left/Right on the tab row change tabs (selection
  follows focus, as Office does; OFF-KEYS). Tab and Shift+Tab move through
  the command area in *task order* (A11Y-001 A5A: "semantic order is an
  explicit task-order graph... This handles ribbon"), by default group by
  group, column by column, top to bottom. Arrows move within a group along
  the same order, with a geometric fallback across rows. Ctrl+Left/Right jump
  to the previous or next group. Down or Alt+Down opens a drop-down or split
  menu. Enter or Space activates. Shift+F10 opens the command's context menu.
- **Focus cue.** The ring is drawn only after keyboard use for every ribbon
  part (SPEC §12.3, §7.3); a hosted text field draws it whenever focused.
- **One command path.** Pointer, keyboard, keytip and assistive-technology
  activation all reach `Command::execute` with one invocation record
  (A11Y-001 §3.5 gate 2; FM3 row of `planning/CONTROL_COMPLETENESS_MATRIX.md`).
- **Semantic tree.** Ribbon → tab row (tabs, selected one marked) and command
  area (groups named by caption → commands, galleries with full logical item
  sets, hosted controls) → QAT → application button. Popup groups are buttons
  with `expanded`; their content appears as their children while open.
  KeyTips are published as access keys (4.6 gap). Every stock ribbon part has
  a default semantic adapter (A11Y-001 A1A); publication is always on (A3A);
  host high contrast, text scale and reduced motion apply (A6A).
- **Tests.** Headless semantic snapshots keyed by stable IDs for expanded,
  minimized, popup and keytip states; scripted keyboard tours; the
  one-command-path trace for pointer, keytip and semantic invoke.

---

## 6. Command and data model, and fit with the stages

### 6.1 What a ribbon needs from commands

`CommandState` already carries most of what the Windows Ribbon Framework's
property keys carry: label (`text`), tooltip description (`description`),
keytip (`key_tip`), enabled, checked, visibility, availability reason (which
UX-RIBBON recommends showing in the enhanced tooltip of a disabled command).
Missing:

| Need | Canonical source | Proposal |
|---|---|---|
| Large and small images, per density | `Command.LargeImages`/`SmallImages` (UIR-SIZE example); 32 and 16 px (UX-RIBBON) | Resolve `icon_id` through an image catalogue keyed by (id, nominal size, density, visual state); SPEC §6.3 says icons are redrawn at 16, 24, 32, 48, never scaled from one master |
| Tooltip title distinct from label | `TooltipTitle` (UIR-TABGROUP property list) | A `tool_tip_title` field, or reuse `text` (decision within 8.E2) |
| Command kind | Command types (`UI_COMMANDTYPE`) | Declared by the ribbon item, not the command (Inferred: one command may be a button here and a menu row there) |
| Values | Spinner value, combo selection, colour, font | Typed value commands (decision 8.E2) |
| Collections | `IUICollection` item sources for galleries, combo boxes, QAT, recent items (UIR-CTL) | A gallery item record list owned by the gallery's value command, with stable item IDs |
| Preview verbs | `PREVIEW`, `CANCELPREVIEW` (UIR-VERB) | Named events `preview_requested`, `preview_cancelled` beside `invoked` (decision 8.E1) |
| Context availability | `UI_PKEY_ContextAvailable` | On the tab-set definition, not on commands |

House style shapes the API: callbacks are named delegates with explicit
context (`Delegate<...>::bind`, as `ToolStrip` uses), not a universal callable
framework (house style §7, §8); a command shared by many presentations is
held by `std::shared_ptr` with one owner (GF017), as `ToolStrip` and
`MenuItemSpec` do; `CommandBar`'s borrowed `Command*` is the older pattern.

### 6.2 Authoring

A ribbon definition is static structure, so it belongs in a form document
(stage FD: "objects with stable IDs, ... properties, ... references to named
handlers"; GF014: the document is authoritative and round-trippable,
generated code disposable). Commands are declared once (label, keytip,
images, tooltip resources from the language assembly) and referenced by stable
ID from the tab, group, QAT-defaults and application-menu structure, as
`Application.Commands` and `Application.Views` are separated in the Windows
Ribbon Framework (Inferred correspondence). Dynamic content (gallery items,
recent files, user QAT items, contextual availability) is runtime state, not
document content. An illustrative shape in the provisional CSS-family block
syntax of `demo/gallery.dml` (not a proposal of syntax, which FD decides):

```text
ribbon editor.ribbon {
  application-menu: editor.file-menu;
  quick-access-defaults: cmd.save cmd.undo cmd.redo;
  tab editor.home { command: cmd.tab-home;
    scaling: ideal(clipboard large, shapes large)
             step(shapes medium) step(clipboard small) step(shapes popup);
    group editor.clipboard { command: cmd.group-clipboard; launcher: cmd.paste-special;
      size large  { column { item cmd.paste large labelled; }
                    column { item cmd.cut small labelled; item cmd.copy small labelled; } }
      size small  { row { item cmd.paste small labelled; item cmd.cut small; item cmd.copy small; } }
    }
  }
  tab-set editor.picture-tools { caption: "Picture Tools"; tab editor.picture-format { … } }
}
```

Validation happens before anything is built: unknown commands, a step that
does not shrink, a size definition that omits a command, duplicate strict
keytips are diagnostics with file, line and node (FD "Validation before
mutation").

### 6.3 How state flows

```text
Command state change (enabled, checked, text, visible)
   -> Ribbon's per-command subscription (as ToolStrip's command_tokens_)
   -> classify: paint-only (enabled, checked) | layout (text, visible)
   -> paint-only: damage only that item's rectangles (DP)
   -> layout: bump tab content generation; recompute width[] lazily when the
      tab is next arranged (a hidden tab costs nothing until shown, as the
      Windows Ribbon Framework queries properties "until the property is
      required", UIR-TABGROUP)
Gallery items change -> gallery realizes visible rows only; semantic children
   updated with stable IDs
Hover/focus on gallery item -> preview_requested(item) ... (5.2)
Contextual availability change -> tab row re-laid out; selection rules (5.4)
```

### 6.4 Fit with the stages

| Stage | What the ribbon takes from it or gives it |
|---|---|
| T1 (sealed) | Anatomy declarations, item systems, schema export; the item-system and part limits force the split in 4.1 |
| T2 | Hit regions from part geometry (T1 decision 6 hand-off); the bulge and dithered bulge as engine primitives (T2 brief: "implement them as primitives with reference images") |
| T3/T4 | Default theme: tab stack, command plane, File tab, gold active tool, wells, group dividers and captions, contextual band colours, keytip badges; until T4 the ribbon paints through `paint_part` with `windows-professional` recipes (SPEC §12.7) |
| L | The ribbon is a control-specific layout (GF008) inside the bounded pass; it does not need flex or grid |
| FD | Ribbon definitions as documents; generated C++ and C# |
| D | The interface builder should preview the scaling ladder at several widths and the keytip assignment; a ribbon designer is a natural D feature |
| W | Windows Forms has no ribbon, so a ribbon is outside W's .NET 8+ scope; it reuses W's `ToolStrip` family (records, overflow, split items) and needs W's `ListView`-like virtualization for galleries |
| A | UIA provider (keytips as `AccessKey`), the three missing roles, region cycling, the gates of A11Y-001 §3.5 |
| C | Item records and generational handles across the C ABI (GF017) |
| DP | Damage per item and per gallery row |
| F | KeyTip labels in Portsmouth Rapids with bundled fallback for translated tips |
| S | Not needed for Forms swap-in (no Forms ribbon), so it does not gate S |

---

## 7. Phased build plan

Each phase ends with tests in `tests/` run by CI on every platform (shared
brief: "A behaviour is done when a test in `tests/` exercises it").

**Phase 0. Decisions.** Owner answers section 8, at least groups A, B and G
(vocabulary). Deliverable: SPEC, `anatomy_types.hpp`, `semantic_types.hpp`
and the schema updated together for whatever atoms are admitted.

**Phase 1. Static ribbon.** `Ribbon` with core tabs, a command area with
groups at their largest size, captions, dividers, launchers; button,
toggle, split and drop-down commands as records bound to shared `Command`s;
hosted combo, spinner and check box as child controls; application button
opening a `ContextMenu`; help button; anatomy, schema, semantics; every part
through `paint_part`. Tests: anatomy and part geometry; one command path for
pointer, keyboard (Tab order) and semantic invoke; command state changes
repaint only the item; schema check passes.

**Phase 2. Adaptive scaling.** Size definitions, scaling policies, default
policy, popup groups, scroll fallback, hide threshold, cached widths. Tests:
5.1's list, including golden layouts at a width ladder and text scales.

**Phase 3. Keyboard and KeyTips.** Window-level menu-mode hook (Alt release,
F10, Alt+key), `KeyTipLayer`, assignment and conflict rules, Level1/Level2,
group jumps, Shift+F10. Tests: 5.3's list; no regression in the M12P7
mnemonic tests.

**Phase 4. Galleries and preview.** `RibbonGallery` in-ribbon and drop-down,
split-button gallery, categories, virtualization, preview protocol. Tests:
5.2's list.

**Phase 5. Minimize, QAT, enhanced tooltips.** Minimized and popup states,
Ctrl+F1, QAT with overflow, customization menu, add/remove from context
menus, persistence through the runtime configuration; `RibbonToolTip`.
Tests: 5.5 and 5.6 lists; tooltip content and keyboard-initiated display.

**Phase 6. Contextual tabs and modes.** Tab sets, availability, colours,
activation rules, application modes. Tests: 5.4's list.

**Phase 7. Specialized controls and file area.** Font control, drop-down
colour picker, context popup (mini toolbar and context menu), the
application menu's recent-items pane or Backstage per decision 8.A3.

**Phase 8. Integration and evidence.** FD documents for a Gallery ribbon
page; host accessibility verification with stage A (UIA, AX, AT-SPI
enumeration of a ribbon fixture); T4 rendering with the dithered bulge and
reference images; a differential run against the Windows Ribbon Framework
on real Windows (scaling ladder and keytip assignment of an equivalent
markup fixture), recorded under `results/` with machine and commands
("Measure against the reference"; the reference is a test oracle, never a
dependency). Plan Paint's ribbon can then be ported as the consumer test,
which is the owner's call.

---

## 8. Open owner decisions, as interview questions

For the planning ministage's interview. Each question is answered by number
(for example `A1-1, B2-2`); the recommendation is a working hypothesis, not
an implied answer. Nothing in sections 4 to 7 is final until these are
recorded. Sections refer back to these IDs (for example "decision 8.B2").

### A. Scope and structure

**A1. Which stage owns the ribbon?**
1. A new stage (for example "RB"), after T1 (sealed) and DP, built under the
   by-role paint path before T4; T4 supplies its look.
2. Inside W, beside `ToolStrip`.
3. After T4, so it is born themed.

Recommendation: 1. Windows Forms on .NET 8+ has no ribbon, so it is outside
W's scope; SPEC §12.7 already allows building before T4.

**A2. Ribbon commands: records or child controls?**
1. Records in a `command` item system, as `ToolStrip` does; hosted editors and
   galleries are real child controls.
2. Every command a child `Button` or `DropDownButton`, as Plan Paint and
   `CommandBar` do.

Recommendation: 1 (T1 decision 5; no control per item).

**A3. What does the File tab open?**
1. An application menu (two panes with recent items).
2. A Backstage full-window view.
3. Plan Paint's plain File drop-down menu.
4. 1 now, 2 later (with Fluent.Ribbon's start screen as a possible third).

Recommendation: 4. Before 2 is built, see taste question T3.

**A4. May a stage use a Windows Ribbon Framework fixture on Shadow Windows as
the differential reference for scaling and keytips?**
1. Yes, asked again before using the machine.
2. No; the canonical documents are the only reference.

Recommendation: 1 ("Measure against the reference").

### B. Layout and scaling

**B1. Which scaling model?**
1. Windows Ribbon Framework: named templates plus custom size definitions,
   tab scaling policy with ideal sizes and steps.
2. WPF/MFC: per-group size definitions plus a tab reduction order, with a
   deterministic right-to-left default when the order runs out.
3. Fluent.Ribbon: group states plus a per-control size per state, and a tab
   reduce order.
4. Automatic only (Office `autoScale`).

Recommendation: 2, with 1's ideal sizes, 3's per-control sizes as a
shorthand, and no fixed template catalogue.

**B2. May commands lose their labels when groups shrink?**
1. Yes, where a size definition says so (canonical Small size).
2. Never: shrink only by image size, rows and popup groups.
3. Only for commands with a material icon and an enhanced tooltip, and only
   at the last step before popup.

Recommendation: 2 as the default, 1 as a per-ribbon opt-in. SPEC §6.3
(V201, V203, V209, M083; M064 R) favours labels; Plan Paint has icon-only
tools. See also taste question T1.

**B3. When is the ribbon hidden in a small window?**
1. Fixed thresholds (Fluent.Ribbon: 300 × 250 px).
2. Derived from content: when the command area would take more than a
   declared fraction of the client height, or still needs scrolling after
   every group is a popup group below a declared width.
3. Never; rely on the 150 × 150 window minimum (DDV-007-02) and scrolling.

Recommendation: 2.

**B4. Admit intermediate scale steps (a gallery gives up one column at a
time, as in Fluent.Ribbon)?**
1. Yes, as a step kind in the scaling policy.
2. No; galleries change only with their group's size.

Recommendation: 1; it costs nothing in the algorithm (5.1).

**B5. Offer the simplified one-line ribbon?**
1. No.
2. Yes, as a display mode with its own size definitions (Fluent.Ribbon,
   Office 365).

Recommendation: 1 for now; the owner's style atlas keeps the full labelled
shelf (`STYLE_FAMILY_ATLAS_004.md`).

### C. Keyboard

**C1. KeyTips across hosts?**
1. One Alt/F10 model, on by default on every host.
2. On by default on Windows and Linux; off by default on macOS and
   application-enabled, reported as a host capability default.
3. Off everywhere unless the application enables it.

Recommendation: 2 (as Office for Mac; one model, only the default differs).

**C2. May applications change the keytip activation keys (Fluent.Ribbon's
`KeyTipKeys`)?**
1. Yes.
2. No; Alt and F10 only.

Recommendation: 2 until a consumer needs it.

**C3. F6 region cycling (ribbon, work pane, side panels, status)?**
1. Add a window-level ordered list of landmark regions, used by F6.
2. Leave region cycling to applications.

Recommendation: 1, with stage A (it also serves side panels and File
Manager).

**C4. Automatic keytip assignment?**
1. Assign every unassigned tip by the rules of 5.3, conflicts numbered in
   declaration order.
2. Assign only QAT and popup-group tips (as Fluent.Ribbon); every other
   command must have an authored tip, checked at validation.

Recommendation: 1, with a strict validation mode for applications that want
2.

### D. Quick Access Toolbar, minimize and persistence

**D1. Where does the QAT go?**
1. On the tab row beside the File tab, inside the client area, with "show
   below the ribbon" as the user's alternative.
2. Below the ribbon only.
3. In the host title bar through a new host capability.
4. No QAT (as Plan Paint).

Recommendation: 1; 3 is host chrome, out of scope by SPEC §1.

**D2. May a whole group be added to the QAT as one button (Fluent.Ribbon's
`QuickAccess` group state)?**
1. Yes.
2. Commands only (Windows Ribbon Framework).

Recommendation: 2 first; 1 is a later addition with no model change.

**D3. Click outside the minimized ribbon's popup?**
1. Consume the click (it only closes the popup).
2. Close the popup and pass the click through.

Recommendation: 1, as GUI.Forms menus do. Office's behaviour was not
verified.

**D4. Who persists ribbon state (QAT items, QAT position, minimized)?**
1. The ribbon, automatically, through the runtime configuration's
   session/layout store (RES §2.4), as Fluent.Ribbon's automatic state
   management does.
2. The application, through explicit save and load calls (Windows Ribbon
   Framework).

Recommendation: 1, with explicit save and load also available.

### E. Galleries and commands

**E1. Live preview shape and trigger?**
1. Preview and cancel-preview events on gallery value commands only.
2. Preview events on every `Command`.
3. Per-item preview and cancel commands (Fluent.Ribbon `GalleryItem`).

And: a hover dwell before preview (proposed 300 ms; Office's value
Unverified) and keyboard focus also previews (proposed yes).

Recommendation: 1, with the dwell and keyboard preview.

**E2. Value-bearing commands (spinner, combo, gallery, colour, font)?**
1. Extend `Command` with a typed value.
2. Separate command types sharing identity, enabled and visibility with
   `Command`.

Recommendation: 2.

**E3. Enhanced tooltip fields?**
1. Title, description, shortcut, image, availability reason.
2. 1 plus a help topic opened by F1 (Fluent.Ribbon `ScreenTip.HelpTopic`).

Recommendation: 2; `CommandState` already has `availability_reason`.

### F. Contextual tabs

**F1. Contextual tab set colours?**
1. The theme supplies an ordered accent ramp; the ribbon assigns indices.
2. The application authors each set's colours (MFC, Fluent.Ribbon).

Recommendation: 1 (GF018–GF019 keep raw colours out of ordinary controls).

**F2. Application modes (a mode bit set choosing core tabs and groups)?**
1. Yes, as in the Windows Ribbon Framework.
2. No; applications swap tabs themselves.

Recommendation: 1; Plan Paint's page switching is this need in another form.

**F3. Contextual selection rules (select the first tab when a set appears on
insert or double-click; return to the last tab of a returning set; fall back
to Home)?**
1. Built in, driven by the application's availability calls.
2. Left to the application (Fluent.Ribbon).

Recommendation: 1.

### G. Vocabulary (each a yes or no; SPEC §12.7)

1. `PartMark::chevron_up` for the minimize actuator. Recommend yes.
2. A launcher mark for the dialog box launcher. Recommend yes.
3. A "more" mark (gallery expand) and an overflow mark (double chevron), which
   `ToolStrip` would adopt. Recommend yes.
4. A customize mark for the QAT. Recommend yes.
5. A `key_tip` part role, or `chip` for keytip badges. Recommend `chip`.
6. Group size selectable by themes (custom states `large`, `medium`,
   `small`, `popup`), or part presence only. Recommend presence only.
7. A per-item accent value for contextual tab sets. Recommend yes.
8. Semantic roles `split_button`, `menu_button`, `toggle_button`. Recommend
   the first two; toggle as `button` plus `checked`.
9. `SemanticNode` fields: access-key path, position in set, set size, has
   popup. Recommend yes, with stage A.
10. Keytip-mode and "refactored tip" states. Recommend no state for the mode,
    and a custom item state for refactored tips only if themes should draw
    them differently.

### Questions for the owner's taste

These have no technical answer; the study can only show the options.

**T1. Which canonical ribbon to follow where they differ.** Office 2007
(application button, QAT in the title bar), Office 2010+ (File tab,
Backstage), the Windows 7 ribbon of Paint and WordPad (application menu,
popup groups, Windows Ribbon Framework scaling), or Plan Paint's own
(icon-only small tools, no QAT, minimize by clicking the selected tab)?
Recommendation to look at first: the Windows 7 Paint ribbon, since Plan
Paint descends from MS Paint and the owner's verdicts accept the "dynamic,
spatially stable shelf" idea (V089 listed; V181 A).

**T2. Which parts take the dithered bulge?** Command faces only (with split
halves and popup-group buttons), or also gallery items, QAT items, the
launcher and the minimize actuator? Tabs never ("A tab is not a button").
The study's lean: command faces only.

**T3. The File area's look.** If Backstage comes, may it have a left
navigation column of topic tabs? The owner rejected horizontal topic tabs on
temporary start and setup surfaces (C141 R) and web-page start surfaces
(C123, C154).

**T4. Contextual tab sets.** A coloured band over the tabs with a caption
(Office), a tint on the tabs themselves, or both? How loud may the colour be
beside the tab stack of RENDER §4?

**T5. KeyTip badges.** Small tinted chips, plates with the house pill, or
plain labels on the plane? Dim or hide tips of unavailable commands?

**T6. Popup group buttons.** The canonical form is the group's main icon in a
32 × 32 container with the caption and a chevron beneath. Keep that, or show
the group caption only?

**T7. The minimized ribbon.** Tab row only, as Plan Paint and Office; and
does the temporary popup cast the E4 overlay shadow (SPEC §3.4) or read as
the plane sliding down?

**T8. QAT appearance.** Flat members on the tab row like the help button, or
small bulges like ribbon commands?

---

### Interview and verdict citations used

GF006, GF007, GF008, GF009–GF011, GF012–GF013, GF014, GF017, GF018–GF019 and
the proposed keyboard order (LEDGER); A1A, A3A, A4A, A5A, A6A, T5A and §3.3,
§3.5 (A11Y-001); RC003, §2.2, §2.4, §7 (RES); DNA-C01 to DNA-C12
(`DESIGN_DNA_006.md`); DDV-007-02, DDV-007-03, DDV-007-14
(`DESIGN_DNA_VERDICTS_007.md`); V201 A, V203 A, V209 A (`VERDICTS_001.md`);
V181 accepted, and V089, V090, V091, V100 listed in `VISUAL_CUE_INVENTORY.md`
with no recorded verdict; M064 R, M072 R, M076 A, M083 A
(`MATERIAL_VERDICTS_001.md`); C123, C141 R, C152, C154 (ceremonial verdicts);
SPEC §3.4, §3.5, §6.3, §7.2–§7.4, §8, §11, §12.3–§12.7; RENDER §2–§5.
