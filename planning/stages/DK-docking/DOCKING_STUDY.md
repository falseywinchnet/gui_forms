# An in-house docking system for GUI.Forms: study

Date: 2026-10-14. Status: research study for stage DK
(`planning/stages/DK-docking/README.md`), not a decision record. Nothing here
is decided until the owner answers section 8.

Reading key. **Known** means read in a cited source (a vendor page, published
source code, a file in the repository, an interview record). **Inferred**
means reasoned from known material but not stated by any source.
**Unverified** means remembered or reported second hand and not checked
against a primary source in this study. Repository paths are relative to
the repository root unless another root is named; branch-only code
names its branch. Interview and verdict IDs are the owner's.

Contents

1. What a docking system is
2. The windowing foundation underneath
3. The owner's requirements, one by one
4. Proposed model in GUI.Forms terms
5. The hard algorithms
6. Fit with the stages and with File Manager
7. Phased build plan
8. Open owner decisions, as interview questions

Sources used, by short name:

| Name | Source |
|---|---|
| DPS | DockPanel Suite (MIT), https://github.com/dockpanelsuite/dockpanelsuite, source read at `master` under `WinFormsUI/Docking/` (files named per fact); docs https://docs.dockpanelsuite.com/ |
| AD | AvalonDock, https://github.com/Dirkster99/AvalonDock, read at tag `v4.72.1` and `master`; `DropTargetType` from the Xceed documentation URL in 1.3 |
| TEL | Telerik RadDock for WinForms, https://www.telerik.com/products/winforms/documentation/controls/dock/ (architecture pages, API reference for `RadDock`, `RadDockTransaction`, `DockTransactionType`, `DockState`); RadDocking for WPF, https://www.telerik.com/products/wpf/documentation/controls/raddocking/ |
| DX | DevExpress docking, https://docs.devexpress.com/WindowsForms/5952/controls-and-libraries/docking-library  |
| IG | Infragistics XamDockManager and UltraDockManager, https://www.infragistics.com/help/wpf/ and https://www.infragistics.com/help/winforms/ |
| VS | Visual Studio window layouts and shortcuts, https://learn.microsoft.com/en-us/visualstudio/ide/customizing-window-layouts-in-visual-studio, https://learn.microsoft.com/en-us/visualstudio/ide/default-keyboard-shortcuts-in-visual-studio  |
| QADS | Qt Advanced Docking System, https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System, `doc/user-guide.md`  |
| QDW | QDockWidget and QMainWindow, https://doc.qt.io/qt-6/qdockwidget.html, https://doc.qt.io/qt-6/qmainwindow.html  |
| GIMP | https://docs.gimp.org/2.10/en/gimp-concepts-docks.html  |
| JB | https://www.jetbrains.com/help/idea/tool-windows.html  |
| ECL | Eclipse workbench, https://help.eclipse.org/  |
| W32-OWN | Microsoft, Window Features, "Owned Windows", https://learn.microsoft.com/en-us/windows/win32/winmsg/window-features (Known, read 2026-10-14) |
| NS-CHILD | Apple, `addChildWindow(_:ordered:)`, https://developer.apple.com/documentation/appkit/nswindow/addchildwindow(_:ordered:) (Known) |
| NS-HIDE | Apple, `hidesOnDeactivate`, https://developer.apple.com/documentation/appkit/nswindow/hidesondeactivate (Known) |
| EWMH | freedesktop Extended Window Manager Hints, https://specifications.freedesktop.org/wm/latest/ar01s05.html (Known) |
| SPEC | `planning/DESIGN_LANGUAGE_SPECIFICATION.md` |
| LEDGER | `planning/interview/GUI_FORMS_INTERVIEW_LEDGER.md` (GF006, GF008, GF009–GF011, GF012–GF013, GF017) |
| A11Y-001 | `planning/interview/GUI_FORMS_ACCESSIBILITY_AND_TEXT_ROUND_001.md` (A1A–A6A, §3.4, §3.5) |
| RES | `planning/interview/GUI_FORMS_RESOURCES_AND_CONFIGURATION.md` (§2.4, §6, §7, RC005, RC006) |
| RB | `planning/stages/RB-ribbon/README.md` decisions (RB-n) and `planning/interview/RIBBON_INTERVIEW_ROUND_001.md` |
| VISUAL | owner's verdicts in the parent File Manager repository's `frontend/planning/visual/`: `DESIGN_DNA_VERDICTS_007.md` (DDV-007-n), `DESIGN_DNA_006.md` (DNA-xnn), `VERDICTS_001.md`, `VISUAL_CUE_INVENTORY.md`, `MATERIAL_VERDICTS_001.md`, `CEREMONIAL_VERDICTS_001.md`, `STYLE_FAMILY_ATLAS_004.md` |
| HOSTS | `docs/user/hosts-and-capabilities.md` |

---

## 1. What a docking system is

**Status of this section.** Every system was read in its published source
or documentation for this study (October 2026); facts are Known with the
file or page named, and the few that were not confirmed are marked
Unverified. Statements about GUI.Forms are Known with paths.

### 1.1 The canonical concepts

| Concept | What it is | Where it appears |
|---|---|---|
| Tool window (pane, anchorable, view, dockable) | A panel that supports a task: visibility and placement matter; usually one instance; hiding keeps its state | DockPanel Suite `DockContent` with a tool `DockState`; AvalonDock `LayoutAnchorable`; VS tool windows; Qt ADS `CDockWidget`; Eclipse views; JetBrains tool windows; GIMP dockable dialogs |
| Document (editor) | A panel that shows a work object: activation, dirty state, close prompts, MRU navigation; many instances | DockPanel Suite documents (`DockState.Document`); AvalonDock `LayoutDocument`; VS document windows; Eclipse editors; DevExpress `DocumentManager` |
| Dock site | The area of a window that hosts the layout | `DockPanel` (DockPanel Suite), `DockingManager` (AvalonDock), `RadDock`, `CDockManager`, `DockLayoutManager` |
| Split container | Divides space between children with a draggable seam | nested panes (DockPanel Suite), `LayoutPanel` (AvalonDock), `CDockSplitter` (Qt ADS), Eclipse sash containers |
| Tab stack (pane, area, part stack, dockbook) | Several panels sharing one rectangle, one visible | `DockPane`, `LayoutAnchorablePane`/`LayoutDocumentPane`, `CDockAreaWidget`, part stacks, GIMP dockbooks |
| Document well | The central area reserved for documents | VS document well, AvalonDock document pane group, Eclipse shared editor area, DevExpress `DocumentManager` |
| Floating window | A native top-level window holding a sub-layout, usually owned by the main window | `FloatWindow`, `LayoutFloatingWindow`, `CFloatingDockContainer`, VS floating frames |
| Auto-hide | A panel collapsed to a tab on a window edge that slides out on hover or click | DockPanel Suite `DockLeftAutoHide` etc., AvalonDock `LayoutAnchorSide`, VS, Qt ADS 4.x auto-hide, JetBrains "Dock Unpinned", Eclipse minimize-to-trim |
| Drop guides | Indicators shown during a drag, one set over the pane under the pointer (nested split or tab) and one at the site edges (global split) | DockPanel Suite pane and panel indicators, AvalonDock overlay window, VS guides ("diamond"), Telerik `DockingGuides`, Qt ADS overlay crosses, Telerik WPF compass and root compass |
| Preview | The rectangle the panel would occupy, drawn before the drop | outline (DockPanel Suite), translucent fill (VS, AvalonDock), live content (Qt ADS dynamic preview option) |
| Restrictions | Which placements a panel allows | `DockAreas` (DockPanel Suite), `AllowedDockState` and `AllowedDockManagerEdges` (Telerik), `CanFloat`/`CanAutoHide`/`CanClose`/`CanHide` (AvalonDock), `AllowDockLeft` … `AllowDockFill` (DevExpress), `AllowDocking` per edge (Infragistics), feature flags and allowed areas (Qt ADS, `QDockWidget::allowedAreas`) |
| Persistence | Saving the layout and restoring panels by identity | DockPanel Suite XML with a `DeserializeDockContent` callback by persist string; AvalonDock `XmlLayoutSerializer` with a callback by `ContentId`; Qt `saveState`/`restoreState` with a version and `objectName`; DevExpress `SaveLayoutToXml` with a layout version and upgrade event; Eclipse `workbench.xmi` |
| Keyboard navigation | Switching and placing panels without a mouse | VS Ctrl+Tab navigator, Alt+F7, Ctrl+F6, Alt+Hyphen dock menu, Shift+Escape; JetBrains Alt+number, Escape, Shift+Escape, Ctrl+Shift+F12; Eclipse Ctrl+F6, Ctrl+F7, Ctrl+M, Ctrl+3, Alt+Hyphen size menu |

### 1.2 DockPanel Suite (WinForms, MIT)

Known, read in the source at
https://raw.githubusercontent.com/dockpanelsuite/dockpanelsuite/master/WinFormsUI/Docking/
(file named per fact) unless marked.

- **Model.** `DockState` is `Unknown, Float, DockTopAutoHide,
  DockLeftAutoHide, DockBottomAutoHide, DockRightAutoHide, Document, DockTop,
  DockLeft, DockBottom, DockRight, Hidden`; `[Flags] DockAreas` is `Float,
  DockLeft, DockRight, DockTop, DockBottom, Document` (`Enums.cs`). A
  `DockPanel` owns one `DockWindow` per dock state, floating windows and
  panes; a `DockPane` is a tab stack of `DockContent` forms. Splits are not a
  tree: each pane records a previous pane, an alignment and a proportion
  (default 0.5) and is carved out of its predecessor
  (`NestedDockingStatus.cs`, `VisibleNestedPaneCollection.cs`). Edge sizes
  (`DockLeftPortion` and so on, default 0.25) are fractions below 1 and
  pixels at 1 or more (`DockPanel.cs`).
- **Documents and tools.** "Document" is a dock state, not a type; any
  content can be either unless `DockAreas` forbids it (`DockPane.cs`).
  Restrictions: `DockHelper.IsDockStateValid(state, DockAreas)`, a pane being
  valid only if every content in it accepts the state; invalid states throw
  (`Helpers/DockHelper.cs`, `DockContentHandler.cs`). Each content remembers
  both its docked pane and its floating pane, so toggling float returns it to
  its previous place. Switches: `AllowEndUserDocking`,
  `AllowEndUserNestedDocking`, `HideOnClose`, `ShowAutoHideContentOnHover`.
- **Drag.** A tab drag starts outside `SystemInformation.DragSize` around the
  press (`DockPaneStripBase.cs`); the drag handler calls Win32 `DragDetect`,
  captures the pointer, subclasses the form and installs a message filter.
  Mouse move re-tests the drop; button up commits; **Escape,
  `WM_CAPTURECHANGED` and `WM_CANCELMODE` abort, and an abort leaves the
  layout untouched**; Ctrl and Shift changes re-test live
  (`DockPanel.DragHandler.cs`, `DockPanel.DockDragHandler.cs`). **Ctrl
  suppresses every docking target** (float only); **Shift** moves the edge
  indicators from the document area's edges to the whole dock area's edges.
  The layout does not change during a drag: only an outline form moves
  (preview mode). There is no tab reordering within a strip; middle click
  closes a tab; double-click toggles float.
- **Indicators.** A transparent overlay form whose region is the union of the
  visible indicators. The pane diamond is centred on the client rectangle of
  the pane under the pointer, found through the native window under the
  point (so the operating system's z-order decides across floats). Five panel
  indicators (left, right, top, bottom, fill) sit 10 px inside the edges of
  the document area (the whole dock area with Shift); **an edge indicator
  appears only while that edge's dock window is empty**. Hit priority: panel
  indicators, then the diamond, then a pane caption (tab into) or a tab
  (insert at that index), then a float window's caption, then float. With no
  target and floating allowed, the outline becomes a float rectangle and
  release floats there; with floating forbidden the cursor shows "no" and
  nothing happens (Unverified that release is a no-op). The outline repaints
  only when its target changes (`DockOutlineBase.cs`).
- **Floating windows.** `FloatWindow : Form`, `SizableToolWindow`, not in the
  taskbar, owned by the main form, not topmost. Only vertical clamping
  against the virtual screen; no per-monitor work-area logic; none on load.
  Dragging a float uses the library's own drag (outline), the float made
  click-through while dragging; caption double-click restores its panes to
  their docked places; the window disposes itself when empty. Open issues
  include floats listed in Alt+Tab (#578) and saving a minimized float (#547)
  (https://github.com/dockpanelsuite/dockpanelsuite/issues).
- **Auto-hide.** Click activates (focus moves in); hover opens without focus
  after the system hover time (`AutoHideStripBase.cs`). The flyout is a child
  panel over the document area with its own splitter; a polled timer at twice
  the hover time closes it when the pointer is outside both flyout and strip,
  but **not while the flyout has focus**; a 100 ms slide
  (`DockPanel.AutoHideWindow.cs`).
- **Activation.** A focus hook finds the pane holding focus and sets, in
  order, `ActivePane`, `ActiveContent`, `ActiveDocumentPane` (kept when a tool
  pane takes focus) and `ActiveDocument`, raising each change event once after
  all are set. A most-recently-active list gives the successor on close or
  hide (`GiveUpFocus`); tracking can be suspended and resumed
  (`DockPanel.FocusManager.cs`).
- **Keyboard.** No navigator, no Ctrl+F4 and no keyboard docking in
  `DockPanel.cs` (Known); none found elsewhere in the library (Unverified).
- **Persistence.** XML with `FormatVersion="1.0"`; any other version throws.
  Elements: `DockPanel` (portions, active panes), `Contents/Content` (`ID`,
  `PersistString`, `AutoHidePortion`, `IsHidden`, `IsFloat`), `Panes/Pane`,
  `DockWindows/DockWindow/NestedPanes/Pane` (`PrevPane`, `Alignment`,
  `Proportion`), `FloatWindows/FloatWindow` (`Bounds`); references are
  ordinal indices. The whole file is parsed before mutation; the
  `DeserializeDockContent` callback receives each persist string, and **a
  null answer creates a placeholder (`DummyContent`) that keeps indices valid
  and is closed at the end**, so unknown panels are dropped and the rest
  loads. Every content is created during load. An exception after mutation
  starts leaves a partly built panel (Unverified; no rollback seen)
  (`DockPanel.Persistor.cs`;
  https://docs.dockpanelsuite.com/tutorials/customizing-persistence.html).
- **Themes.** VS2003, VS2005, VS2012, VS2013 and VS2015 (the last three in
  blue, light and dark), each supplying indicators, outline and strips
  (https://docs.dockpanelsuite.com/themes/existing-themes.html).

### 1.3 AvalonDock (WPF)

Known, read in https://github.com/Dirkster99/AvalonDock at tag `v4.72.1` and
on `master` (now version 5, with a JSON serializer), unless marked.

- **Model.** A layout tree separate from the controls: `LayoutRoot` with
  `RootPanel` (an oriented `LayoutPanel`), four `LayoutAnchorSide`s of
  `LayoutAnchorGroup`s (auto-hide), `FloatingWindows` and `Hidden`; split
  groups (`LayoutPanel`, `LayoutAnchorablePaneGroup`,
  `LayoutDocumentPaneGroup`); panes (`LayoutAnchorablePane`,
  `LayoutDocumentPane`); leaves `LayoutAnchorable` and `LayoutDocument`.
  Sizes are star or pixel lengths with minimums (default 25). A fixpoint
  `CollectGarbage()` removes empty panes and groups and flattens single-child
  groups after edits, but always keeps one document pane
  (`Layout/LayoutRoot.cs`).
- **Documents and tools** are distinct types. Content flags `CanClose`,
  `CanFloat`, `CanHide`, `CanAutoHide`, `CanDockAsTabbedDocument`, `CanMove`;
  an anchorable hides rather than closes by default. Hidden content remembers
  its previous container and index and returns there on `Show()`.
  `ILayoutUpdateStrategy` lets the application place inserted content.
  Closing a document raises a cancellable `DocumentClosing`, then chooses the
  next document to activate (`DockingManager.cs`).
- **Drag** always tears content into a **real floating window** at once
  (immediate mode): a tab drag reorders live inside its strip and tears off
  when the pointer leaves the strip; the floating window is then moved by the
  **native move loop** (a caption `WM_NCLBUTTONDOWN`), with a `DragService`
  driven by `WM_MOVING` and the drop made on `WM_EXITSIZEMOVE`. With no
  target the window stays floating; there is no explicit Escape handling
  (`Controls/LayoutFloatingWindowControl.cs`, `Controls/DragService.cs`).
  Hosts under the pointer are found by walking the native z-order.
- **Targets.** `DropTargetType` has 19 kinds: four docking-manager edges
  (global), five document-pane targets, one document-group inside, five
  anchorable-pane targets, and four "dock as anchorable beside the document
  pane" targets (https://xceed.com/documentation/xceed-toolkit-plus-for-wpf/Xceed.Wpf.AvalonDock~Xceed.Wpf.AvalonDock.Controls.DropTargetType.html).
  A reused transparent, non-activating overlay window shows per-area button
  sets and a preview path; per-tab insertion targets exist; **the first
  target in enumeration order whose rectangle contains the pointer wins**
  (`Controls/OverlayWindow.cs`). Previews are the matching half of the target
  for side drops and a tab shape for inside drops.
- **Floating windows**: not in the taskbar, owned by the main window when
  `OwnedByDockingManagerWindow`, not topmost. Restore clamps a float that
  misses the primary or virtual screen, but despite its name not to the
  nearest monitor, with a coordinate error when the virtual screen's origin
  is not zero, and without per-monitor DPI
  (`Layout/ILayoutElementForFloatingWindowExtension.cs`).
- **Auto-hide**: click opens and activates; hover opens after 400 ms; a close
  timer (`AutoHideDelay`, 500 ms) keeps it open while the pointer is over it
  or its tab or while it is active; no animation; the flyout is a native
  child window (`Controls/LayoutAnchorControl.cs`,
  `Controls/AutoHideWindowManager.cs`).
- **Activation**: one `ActiveContent` and a `LastFocusedDocument`, with
  `LastActivationTimeStamp` for most-recently-used order. Restoring focus
  when a floating window is activated is not wired (the subscription is
  commented out in `Controls/FocusElementManager.cs`).
- **Keyboard**: Ctrl+Tab opens a `NavigatorWindow` of anchorables and
  documents in MRU order (Left and Right switch lists, Up and Down move,
  releasing Ctrl activates); no keyboard docking.
- **Persistence**: `XmlLayoutSerializer` over `LayoutRoot`, with **no version
  attribute or check**. Load builds a new root and swaps it in, so a failure
  leaves the old layout. Content is matched by `ContentId`; a
  `LayoutSerializationCallback` may supply content or cancel; without one,
  unmatched anchorables are hidden and unmatched documents closed. An
  unresolved pane reference throws
  (`Layout/Serialization/LayoutSerializer.cs`).
- **Weaknesses** visible in its issues: duplicate or ghost floating windows
  (#426, #456, #587), re-dock failures (#551), the wrong document activated
  on close (#434) (https://github.com/Dirkster99/AvalonDock/issues).

### 1.4 Telerik RadDock (WinForms) and RadDocking (WPF)

Known from Telerik's documentation (the old `docs.telerik.com/devtools/winforms`
paths now redirect to `https://www.telerik.com/products/winforms/documentation/controls/dock/…`;
pages named per fact), unless marked.

- **Model.** `RadDock` derives from `RadSplitContainer`, which holds a list
  of panels (n-ary) separated by splitters. Exactly one `DocumentContainer`
  per `RadDock`, which cannot be deleted and stays visible with no documents
  (`architecture-and-features/understanding-raddock`). `DocumentWindow`s move
  only inside the document container; `ToolWindow`s float, dock, auto-hide
  and may enter the document area. `DockState` is `Docked`,
  `TabbedDocument`, `Hidden`, `AutoHide`, `Floating`; a tool window's menu
  offers "Floating", "Dockable", "Tabbed Document", "Auto Hide", "Hide".
  `Name` is the identity used for saving and restoring.
- **Restrictions.** `AllowedDockState` flags also remove the matching
  caption buttons and menu entries (Snippet-level source). The drag service
  exposes `PreviewDropTarget`, `PreviewDockPosition` (allowed positions per
  target), `PreviewHitTest`, and `AllowedDockManagerEdges` for the outer
  (global) edges (`using-the-dragdropservice`).
- **Guides.** `DockingGuidesTemplate` presets VS2008, VS2010 (default),
  ControlDefault and Office2010; a custom template is images (left, top,
  right, bottom, fill, each with a hot image) placed on a centre background,
  and **hit areas are derived from the image positions**; the same side
  images serve the outer guides; the drop hint has a back colour, border
  colour and width (`dockingguidestemplate`).
- **Drag modes.** `DragDropMode.Immediate` (the default) floats the window at
  drag start; `DragDropMode.Preview` leaves it docked, shows a translucent
  rectangle and is "fully cancelable" (the designer uses it). Escape cancels;
  `Starting` and `Stopping` events can veto or change the commit
  (`using-the-dragdropservice`).
- **Transactions.** Every structural change is a `RadDockTransaction`
  (target state, associated windows, added windows, removed windows, a
  restore-first flag), with subclasses for auto-hide, close, float, dock and
  redock, and `DockTransactionType` values DragDrop, Redock, DockWindow,
  AutoHide, Close, Float. `BeginTransactionBlock` and `EndTransactionBlock`
  nest by a counter, and "when the counter reaches zero, all pending
  transactions are executed"; `FlushTransactions` commits without leaving the
  block; `TransactionCommitting` is cancellable and `TransactionCommitted`
  follows; `BeginUpdate`/`EndUpdate` (layout suspension) are tied to the
  block (RadDock API reference). No rollback or undo was seen. This is the
  only canonical system that makes the owner's "layout transactions" a public
  concept.
- **Activation and keys.** `ActiveWindow` holds keyboard focus;
  `DocumentManager.ActiveDocument` stays the document while a tool window
  has focus (`document-manager`). Predefined rebindable commands:
  Ctrl+Tab quick navigator, Ctrl+F4 close active document, Ctrl+F6 and
  Ctrl+Shift+F6 next and previous document (`using-the-commandmanager`).
  `SingleScreen` confines behaviour to the primary screen.
- **Persistence.** `SaveToXml`/`LoadFromXml`, hidden and floating windows
  included; `DockWindowSerializing` can exclude windows; no layout
  versioning documented (`loading-and-saving-layouts`).
- **WPF RadDocking.** Panes always live in pane groups inside split
  containers; a **Compass** (five indicators) over the container under the
  pointer and a **RootCompass** (four, no centre) at the root's edges, both
  filterable in `PreviewShowCompass`; dropping outside any indicator floats
  the pane; a `DockingNavigator` on Ctrl+Tab or Alt+F7 with panes, documents
  and a preview; layouts saved by `SerializationTag`, with `ElementLoading`
  able to supply an existing instance (the restore-by-identity hook) and
  untagged elements removed on load; no versioning documented
  (https://www.telerik.com/products/wpf/documentation/controls/raddocking/features/compass,
  `…/navigator`, `…/save-load-layout/overview`).

### 1.5 DevExpress, Infragistics, Visual Studio

**DevExpress** (Known, https://docs.devexpress.com/, pages named per fact).
WinForms `DockingStyle`: `Float`, `Top`, `Bottom`, `Left`, `Right`, `Fill`.
Per-panel options separate the **form edges** (`AllowDockLeft` … , global)
from `AllowDockFill` ("docked to another dock panel", nested) and
`AllowDockAsTabbedDocument`; manager options include `FloatPanelsAlwaysOnTop`,
`CursorFloatCanceled` and `AutoHiddenPanelShowMode` (hover or click).
Layouts carry `OptionsLayout.LayoutVersion`; **`BeforeLoadLayout` can veto a
restore and `LayoutUpgrade` is raised after a restore whose version differs**
(common-features "saving and restoring layouts"). `DocumentManager`'s
TabbedView keeps at least one document group; floating documents can host
their own nested manager; a Document Selector on Ctrl+Tab lists documents and
panels in MRU order with arrows switching columns; `QueryControl` supplies a
document's content when first shown (lazy creation; Snippet-level). WPF
`DockLayoutManager`: `LayoutPanel`, `LayoutGroup`, `TabbedGroup`,
`FloatGroup`, `AutoHideGroup`, `DocumentGroup`, a `ClosedPanels` list; its
edge guides have both a side hint and an **auto-hide hint** (dock straight to
auto-hide); the VS2010 style confines document panels to document groups;
`FloatingMode` keeps floats inside the host window or lets them roam the
desktop (owned, minimized with the owner); restore options decide whether
panels missing from the file go to `ClosedPanels` and whether saved panels
missing from the application are kept.

**Infragistics** (Known, https://www.infragistics.com/help/wpf/ and its
Korean mirror). `XamDockManager`: `ContentPane`, `SplitPane`, `TabGroupPane`,
`DocumentContentHost`; locations docked, floating (dockable), floating only,
document, unpinned. `AllowDocking` with per-edge overrides that "refer to
edges of the dockmanager" (global edges). A pane navigator on Alt+F7 (panes)
and Ctrl+Tab (documents) with previews. XML layouts record closed panes and
relative sizes; **`InitializePaneContent` fires for every saved pane with no
live match, and the handler supplies content by `SerializationId` or the pane
is dropped** (restore by identity and lazy creation in one hook). A property
named `AllowedDockLocations` could not be confirmed (Unverified).

**Visual Studio** (Known,
https://learn.microsoft.com/en-us/visualstudio/ide/customizing-window-layouts-in-visual-studio,
https://learn.microsoft.com/en-us/visualstudio/ide/default-keyboard-shortcuts-in-visual-studio).
Tool windows drag by the title bar, documents by the tab; document windows
"can be docked only within the editing frame", so their guide diamond differs.
Over a guide arrow "a shaded area appears that shows you where the window
will dock". Ctrl while dragging moves "without snapping"; Ctrl+double-click
toggles between floating and the last docked place. Floating-window
ownership is a user option with three values: none (independent, in the
taskbar, visible when the IDE is minimized), tool windows owned (the
default: they minimize with the IDE, stay above it while it is active, have no
taskbar button), or documents and tool windows owned. Auto Hide slides a
window away and leaves an edge tab, which is **selected** to slide it back;
an option makes the auto-hide button act on one window or the whole group.
Up to ten saved window layouts on Ctrl+Alt+1 … 0, Reset Window Layout; since
17.9 the document layout is stored as JSON under `.vs`. Tabs: preview tab,
pinned tabs, dirty indicator, Ctrl+K Ctrl+Z restores a closed tab. Keys:
Esc `Window.ActivateDocumentWindow`; Ctrl+F4 close document; Shift+Esc close
tool window; Ctrl+F6 next document; Ctrl+Tab and Ctrl+Shift+Tab the IDE
Navigator; Alt+F6 next pane; **F6 `Window.NextSplitPane`**; Alt+F7 and
Shift+Alt+F7 tool-window navigator; Alt+Hyphen `Window.ShowDockMenu`;
Shift+Alt+Enter full screen. Keyboard moving after Alt+Hyphen is not
documented (Unverified). VS SDK: `ProvideToolWindow` registers a default
style, a window to dock with and an orientation, used only the first time;
tool windows are created on demand (`FindToolWindow(type, id, create: true)`);
multi-instance windows probe instance IDs
(https://learn.microsoft.com/en-us/visualstudio/extensibility/adding-a-tool-window).

### 1.6 Qt Advanced Docking System and QDockWidget

**Qt ADS** (Known,
https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System, `README.md`,
`doc/user-guide.md`, `src/FloatingDragPreview.cpp`, `src/DockManager.cpp`).
`CDockManager`, `CDockContainerWidget`, `CDockAreaWidget` (tab stack),
`CDockWidget`, `CFloatingDockContainer` (each floating window is a full
container). Optional non-movable central widget. Per-widget features
(closable, movable, floatable, `CustomCloseHandling` that emits
`closeRequested()` instead of closing, `DeleteContentOnClose` that rebuilds
content from a factory on show). Two overlays during a drag: an **area
overlay** (five-way cross) over the hovered dock area and a **container
overlay** (outer edges; centre only when one area exists); the source area is
excluded; over an area's title bar the container drop wins. Docking happens
only on release; **Escape and loss of application activation cancel**; with
no target the widget floats if floatable, else nothing happens. Auto-hide
(4.0): hover or click, closes on an outside click by default, works in
floating containers too, and a drag hovering a collapsed tab opens it.
Persistence: `saveState(version)` writes a library `Version` and a
`UserVersion`; `restoreState` rejects a newer library version or a
mismatched user version and **runs a full dry-run pass before mutating**;
widgets missing from the state are closed, unknown saved names skipped;
named perspectives. Under native Wayland floating containers must use native
decorations and "the compositor decides where restored windows appear"; no
keyboard switcher or keyboard docking.

**QDockWidget in QMainWindow** (Known, https://doc.qt.io/qt-6/qmainwindow.html,
https://doc.qt.io/qt-6/qdockwidget.html): four dock areas around a central
widget; `AllowNestedDocks`, `AllowTabbedDocks`, `GroupedDragging`;
`allowedAreas` and features per dock; `saveState(version)` identifies docks
by unique `objectName`, and `restoreState` returns false and changes nothing
on a version mismatch; `restoreDockWidget` places a dock created after the
restore (lazy creation).

### 1.7 GIMP, JetBrains and Eclipse

**GIMP** (Known, https://docs.gimp.org/2.10/en/gimp-concepts-docks.html and
the 3.0 page). Docks hold persistent dialogs as tabs; a tab dropped on a
dock's tab bar joins the group; dropped on a **blue docking bar** that appears
at a dock border it starts a new column or row; in 3.0 a drop where nothing
docks detaches the tab into a new dock window. Tab menu: Detach Tab, Lock
Tab to Dock, Move to Screen. Single-window and multi-window modes; Tab hides
docks. Session state in `sessionrc` (Snippet-level).

**JetBrains** (Known, https://www.jetbrains.com/help/idea/tool-windows.html,
`viewing-modes.html`, `manipulating-the-tool-windows.html`). Tool windows on
bars at the sides and bottom; view modes Dock Pinned, Dock Unpinned (shown
only while active), Undock, Float (above the main window), Window. No drop
guides: tool windows move by dragging their bar buttons or by menu. Keys: Esc
returns focus to the editor; Shift+Esc hides the active tool window;
Ctrl+Shift+F12 hides all; F12 returns to the last tool window; Alt+1 … Alt+0
open specific windows; **Ctrl+Alt+Shift+arrows** resize; Ctrl+Tab Switcher.
Window | Layouts saves and applies named layouts. Editor tabs split right or
down, detach by dragging outside, have a preview tab and pinned tabs.

**Eclipse** (Known, https://help.eclipse.org/latest/, the pages on
perspectives, editors, views, moving views, detaching, minimize and maximize,
accessibility navigation). Perspectives are named view layouts; all
perspectives of a window share the editors. A view is dragged by its tab to
a border of another part, with **cursor and outline feedback and no compass**
(a vertical bar for a tab insertion point, a rectangle for an area or a
detached window, a prohibition cursor for invalid). Dragging outside detaches
the part into a window "always shown in front of the Workbench window".
Ctrl+M maximizes a part by minimizing the other stacks to trim; maximizing an
editor maximizes the whole editor area. Keys: Ctrl+F6, Ctrl+F7, Ctrl+F8
cycle editors, views and perspectives; Ctrl+E editor list; Ctrl+3 Quick
Access; **Alt+Hyphen opens a "Size" menu whose sides are moved with the arrow
keys** (keyboard resizing). The e4 model persists between sessions;
`-clearPersistedState` discards it; model fragments declare whether they
apply always, only on a fresh state, or only when the element does not exist
(https://help.eclipse.org/latest/topic/org.eclipse.platform.doc.isv/reference/extension-points/org_eclipse_e4_workbench_model.html).

### 1.8 Where the systems differ

| Question | DockPanel Suite | AvalonDock | Telerik WinForms | VS | Qt ADS | JetBrains | Eclipse | GIMP |
|---|---|---|---|---|---|---|---|---|
| Model separate from controls | no | yes | partly (transactions over controls) | yes (Unverified) | partly | Unverified | yes (e4 model) | Unverified |
| Split nodes | "split previous pane" lists | n-ary tree | n-ary split containers | n-ary (Unverified) | n-ary splitters | fixed sides | sashes (Unverified) | docks and dockbooks |
| Distinct document area | a dock state | yes, one pane always kept | one document container, never removed | the editing frame; documents dock only there | central widget optional | editor area | shared editor area | image window |
| Nested targets | pane diamond | per-area button sets | compass | diamond | area cross | none | outline | tab bar |
| Global targets | edge indicators, only for empty edges; Shift for whole area | four manager edges | outer guides; `AllowedDockManagerEdges` | edge arrows | container cross (outer edges) | none | outline | blue border bars |
| Drag mode | preview outline | immediate float, native move loop | immediate (default) or preview | immediate float, shaded preview | preview, static or dynamic | menu and bar drag | outline | highlight |
| Cancel | Escape, capture loss, `WM_CANCELMODE`: no change | no explicit Escape | Escape | Escape (Unverified) | Escape, deactivation | n/a | Unverified | Unverified |
| Invalid drop | float if allowed, else nothing | stays floating | float (WPF) | float | float if allowed, else nothing | n/a | prohibition cursor | detach to a new dock (3.0) |
| Suppress docking | Ctrl | none seen | none seen | Ctrl | none seen | n/a | n/a | n/a |
| Auto-hide opening | hover or click | 400 ms hover or click | Unverified | select the tab | hover or click (option) | Dock Unpinned | minimize to trim | none |
| Transactions as a public concept | no | no | **yes**, nested blocks, cancellable commit | no | no | no | no | no |
| Restore by identity | persist-string callback; null gives a placeholder | `ContentId` callback; unmatched hidden or closed | `Name` | GUID and instance ID | `objectName`; unknown skipped | ID | model element IDs | sessionrc |
| Layout versioning | `FormatVersion` "1.0", others rejected | none | none documented | per release; JSON since 17.9 | library and user versions; dry run first | Unverified | fragment apply modes | none seen |
| Keyboard switcher | none in `DockPanel.cs` | Ctrl+Tab navigator | Ctrl+Tab navigator | Ctrl+Tab navigator, Alt+F7 | none | Ctrl+Tab Switcher | Ctrl+F6, Ctrl+F7, Ctrl+F8 | none |
| Keyboard placement | none | none | menu | Alt+Hyphen dock menu | none | view-mode menu, resize keys | Alt+Hyphen size menu with arrows | tab menu |

The study's reading (Inferred): every system with guides separates targets
around a pane (nested) from targets at the site's outer edges (global); most
keep a distinct document area; keyboard placement exists only as menus (VS's
dock menu, Eclipse's size menu), and none read here has a keyboard move mode
stepping through the drag targets, which is why 5.7.1 proposes one; only
Telerik exposes transactions as a public concept, and only Qt ADS validates a
whole restore in a dry run before mutating.

---

## 2. The windowing foundation underneath

The owner: "docking is one part of a larger windowing system. You'd want the
underlying framework to make sizing, focus, input, painting, and state behave
consistently—then the fancy panels can sit on top." This section lists what
the panels need from the framework, what GUI.Forms has, and what is missing,
per concern and per host.

### 2.1 What GUI.Forms has (Known, read in the repository)

| Piece | Where | What it gives docking |
|---|---|---|
| Multi-window run | `include/gui_forms/application/application.hpp` | `Application::run(std::vector<ApplicationWindow>)`: up to 64 windows (`maximum_windows`), each with its own `Window` model, `stable_id`, optional `owner_id` and a `tool_window` flag. Exactly one primary window (no owner, not a tool window); ownership cycles are rejected (`src/host/windows/application/windows_host.cpp:4090-4190`). |
| Window handle | same file, `ApplicationWindowHandle` | Weak, thread-affine: `show`, `hide`, `toggle_full_screen`, `set_title`, `request_client_size`, `request_close`. `request_client_size` is a request; the model follows only on the host's resize event (`docs/user/hosts-and-capabilities.md`, "Multiple windows"). |
| Window options | same file, `ApplicationWindowOptions` | Initial and minimum size (default minimum 150 × 150, DDV-007-02), `initially_visible`, `hide_on_close`, `minimizable`, `closing` (cancellable `HostCloseRequest`), `closed`. |
| Host services | `include/gui_forms/host/services/host_services.hpp` | `query_monitors`, cursors (including custom images), `set_pointer_capture`, clipboard, dialogs, sound cues, modal depth (`maximum_nested_modal_depth = 8`). |
| Monitors | `include/gui_forms/host/types/host_types.hpp` | `HostMonitor {id, frame, work_area, scale, primary}`; `HostDisplayEvent` carries the new list on display change. `monitor_geometry` is advertised on macOS, Windows and X11 (`docs/user/hosts-and-capabilities.md`). |
| Window chrome drag regions | `host_types.hpp`, `WindowChromeHitRole`, `resolve_window_chrome_hit` | An authored area of the client can start the native window move, while interactive descendants keep their input. |
| Activation | `HostActivationEvent`; `Window::set_active`, `Window::active_changed` (`include/gui_forms/window/window.hpp:290`) | Each window learns when it becomes active or inactive (`src/core/host/session/host_session.cpp:211`). |
| Participation states | `AnatomyState::key`, `secondary_participating`, `drag_proximate`, `deactivated` (`include/gui_forms/anatomy/types/anatomy_types.hpp`) | The vocabulary already names DDV-007-01's three participation states and drag proximity. |
| Focus | `Window::request_focus`, `move_focus`, focus scopes (`begin_focus_scope`, `FocusScopeOptions{contain_focus, restore_focus, focus_first}`, depth 32), focus-visible modality (`window.hpp:440-480`) | Focus inside one window, popups and dialogs that contain and restore focus. |
| F6 regions | branch `claude/w-ribbon-phase-0`, `Window::set_focus_regions`, `move_focus_region`; branch `claude/w-ribbon-phase-1` adds `add_focus_region` | Owner decision RB-17. Regions must be attached, distinct and **not nested in one another**; entering a region restores the control that last had focus inside it. |
| Accelerators | `Window::register_accelerator` (`window.hpp:502`) | Per-window shortcuts, tried after the focused control's route. |
| Update scopes | `Window::begin_update`, `UpdateScope` (`include/gui_forms/window/update_scope/update_scope.hpp`); `Control::suspend_layout` (`experiments/M12P11_LAYOUT_TRANSACTIONS.md`, `M12P12_MUTATION_SAFE_LAYOUT.md`) | Nested, coalescing, read-barrier layout (interview GF006 L4). `UpdateScope` is a move-only token for **one** `Window`. |
| Cross-window invalidation | `Window::set_paint_wake_handler` (`window.hpp:400`) | "Retained invalidation may originate from another Window's callback": one window's handler may change another window's model, which presents on its next wake. |
| Damage-preserving paint | stage DP (`planning/stages/DP-damage-preserving-painting/`) | Rectangle-set damage per window on every host; an idle window schedules no frames. |
| Splits | `SplitContainer` (`include/gui_forms/controls/scrollable_control/container_control/split_container/split_container.hpp`) | Two panels and one seam; seam geometry with hit extension (`SplitSeamGeometry`), seam states, keyboard and collapse (`SplitChangeReason`, `SplitCollapseOrigin`). |
| Tabs | `TabControl` (`…/tab_control/tab_control.hpp`) | Tab pages, alignment, per-page focus memory; Ctrl+Tab (or Meta+Tab) in key preview selects the next page (`src/controls/scrollable_control/container_control/tab_control/tab_control.cpp:409-421`). No closable tabs, no tab dragging, no overflow (W P3). |
| Strip rafting | branch `claude/w-tool-strip-family`, `ToolStripPanel`, `ToolStripContainer` | Strips joined to four edge panels; the header says "Dragging strips between panels is not supported yet." |
| Popups | `Window::open_popup` with `PopupToken`; `AnchoredPopupLayer` (`include/gui_forms/controls/panel/anchored_popup_layer/`) | In-window overlays anchored to a control, with click-away and Escape reported to the owner. |
| Commands | `include/gui_forms/commands/` | `Command` with state and `invoked`; no routing to a "current target" (no command context). |
| Semantics | `include/gui_forms/semantics/types/semantic_types.hpp` | Roles include `group`, `tab_group`, `tab`, `split_pane`, `toolbar`, `menu`; actions `focus`, `press`, `select`, `expand`, `collapse`, `show_menu`, `set_value`. |
| Panels in a consumer | `file_manager_demoboard/src/product_window.cpp:938-960` | Two nested `SplitContainer`s (`fm.workspace`, `fm.workspace.content_selection`); a controller tool window owned by the product window (`file_manager_demoboard/src/windows_main.cpp:41-47` and the macOS and Linux mains). |

### 2.2 What each host does with an owned tool window today (Known)

| Behaviour | Windows | macOS | X11 |
|---|---|---|---|
| Native object | `CreateWindowExW` with the owner's `HWND` as owner; `WS_EX_TOOLWINDOW` when `tool_window` (`windows_host.cpp:4158-4170`) | `NSPanel` with `NSWindowStyleMaskUtilityWindow`, added with `addChildWindow:ordered:NSWindowAbove` (`src/host/macos/application/macos_host.mm:3253-3318`) | `XSetTransientForHint` to the owner (`src/host/linux/application/linux_host.cpp:580`, `1321`) |
| Stacking | An owned window "is always above its owner in the z-order", is destroyed with its owner and is hidden when its owner is minimized; ownership cannot be transferred after creation (Microsoft, *Window Features*, "Owned Windows", https://learn.microsoft.com/en-us/windows/win32/winmsg/window-features) | A child window keeps its relative ordering; "moving the window will cause childWin to move" (Apple, `addChildWindow(_:ordered:)`, https://developer.apple.com/documentation/appkit/nswindow/addchildwindow(_:ordered:)) | Depends on the window manager; the transient hint names the main window (freedesktop EWMH, https://specifications.freedesktop.org/wm/latest/ar01s05.html) |
| Shown on start | `SW_SHOWNOACTIVATE` (does not take activation) (`windows_host.cpp:4204`) | ordered out if not initially visible | mapped |
| Placement | `CW_USEDEFAULT` | primary centred; owned windows placed to the owner's right and clamped into the screen's visible frame (`macos_host.mm:3320-3330`) | window manager |
| Hides when the application deactivates | no (Inferred: plain owned window) | **yes by default**: `NSPanel`'s `hidesOnDeactivate` defaults to true (Apple, `hidesOnDeactivate`, https://developer.apple.com/documentation/appkit/nswindow/hidesondeactivate) and the host never sets it (grep of `macos_host.mm`, Known) | no (Inferred) |
| Window type for the window manager | `WS_EX_TOOLWINDOW` keeps it off the taskbar (Unverified wording; Microsoft's extended-style page) | utility panel | none: no `_NET_WM_WINDOW_TYPE_UTILITY` and no `_NET_WM_STATE_SKIP_TASKBAR` (grep, Known), so a floating panel may appear in taskbars (Inferred) |

### 2.3 Gaps, by the owner's five concerns

**Sizing.**

- G-SIZE-1 (Known gap). No portable window position. `ApplicationWindowHandle`
  can request a client size but cannot read or set a window's position, and
  "coordinate conversion ends at window client coordinates; a desktop-screen
  origin and multi-monitor transform are host facts you query through
  monitor records" (`docs/user/hosts-and-capabilities.md`). A floating panel
  must be placed where it was dropped and restored where it was saved, so the
  host protocol needs a window frame (position and size) request and a
  frame-changed event, in screen logical coordinates tied to a monitor.
- G-SIZE-2 (Known gap). `PointerEvent` carries only a client `position`
  (`include/gui_forms/events/input_events/input_events.hpp:62`). Docking needs
  the pointer in screen coordinates to find which window and which dock site
  is under it during a drag that started in another window.
- G-SIZE-3 (Inferred). Minimum sizes: each docked panel needs a minimum its
  content declares (`measure`), the split solver must honour the sum of
  minimums, and the window minimum (DDV-007-02) must grow to fit them or the
  layout must collapse by declared priority (SPEC §7.4 rule 3; DNA-C12).
  GUI.Forms has the pieces (`measure`, priority collapse in
  `ResponsiveTrackPanel` and `CommandOverflowPanel`) but no n-way split with
  minimums.
- G-SIZE-4 (Known). Per-monitor scale: `scale_notifications` exists on macOS
  and Windows, not X11 (`hosts-and-capabilities.md`). A floating window moved
  to another monitor changes scale on two hosts and not on the third; restore
  must convert saved logical frames per monitor scale.

**Focus and activation.**

- G-FOCUS-1 (Known gap). Focus, focus scopes, F6 regions and accelerators are
  per `Window`. Nothing chains a floating tool window to its main window, so a
  shortcut such as Ctrl+S pressed while a floating tool window is key reaches
  only the floating window's accelerators. Docking needs an owned window to
  fall back to its owner's accelerators and command context (section 5.6).
- G-FOCUS-2 (Known gap). F6 cycles regions of one window; a floating panel is
  a different window. Docking needs F6 (or a navigator) to reach panels in
  every window of the workspace.
- G-FOCUS-3 (Known constraint). F6 regions may not nest
  (`window.hpp` on `claude/w-ribbon-phase-0`). A dock site therefore registers
  its leaf stacks (each tool stack and the document well) as regions, never
  itself (Inferred).
- G-FOCUS-4 (Known gap). No "active panel" or "active document" concept
  above controls; no command routing to a current target (`commands/`).
- G-FOCUS-5 (Known). On macOS today, a tool window hides whenever the
  application deactivates (NSPanel default). Whether floating dock windows
  should do that is an owner question (B5).

**Input.**

- G-INPUT-1 (Known, mostly present). Pointer capture keeps a drag's events in
  the window where it began: `SetCapture` on Windows
  (`windows_host.cpp:3494-3498`), the native implicit drag sequence on macOS,
  an implicit grab on X11 (Inferred for X11). Capture loss is reported
  (`WM_CAPTURECHANGED` releases the pointer, `windows_host.cpp:2507`), which a
  dock drag must treat as cancel.
- G-INPUT-2 (Known gap). The chrome drag region hands the move to the
  platform's own move loop. During a native move the application does not get
  ordinary pointer events (Inferred from how Win32 `HTCAPTION`, AppKit window
  dragging and EWMH `_NET_WM_MOVERESIZE` work; Unverified per host), so drop
  guides cannot follow the pointer unless the host reports move progress. The
  study's recommendation (5.4) is to move floating dock windows by the
  framework's own capture-driven drag instead, which needs G-SIZE-1.
- G-INPUT-3 (Known). Operating-system drag and drop is not needed: every
  docking system read here drags with its own capture (section 1). GUI.Forms'
  `typed_drag_source` is unsupported everywhere and `typed_drag_destination`
  is missing on Windows; neither blocks docking (Inferred).
- G-INPUT-4 (Known). `TabControl` consumes Ctrl+Tab in key preview. A
  workspace navigator on Ctrl+Tab collides with any `TabControl` inside a
  panel; the arbitration is an owner question (G1).
- G-INPUT-5 (Inferred). No portable drag threshold. Windows has a system
  metric for it (`SM_CXDRAG`, Unverified default 4 px); macOS and X11 have no
  single equivalent read by GUI.Forms. A portable threshold in logical units,
  host-overridable, is needed (5.1).

**Painting.**

- G-PAINT-1 (Inferred). Guides and previews can be drawn inside the windows
  GUI.Forms owns, in an overlay layer above the control plane, with ordinary
  rectangle damage (DP). No native overlay window is needed while the pointer
  is over one of the application's windows. Over the desktop (a float
  outside every window) a preview needs either a native non-activating
  overlay window or no preview at all (option D1). The Windows and macOS hosts
  already create such a window privately for tool tips (`windows_host.cpp:2785`
  `WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE`; `macos_host.mm:1604`
  an `NSPanel` child), so a host precedent exists; it is not public.
- G-PAINT-2 (Known). Every window has its own theme (`Window::set_theme`) and
  its own image registry (`Window::load_png`, `image_resources`); an
  `ImageList` belongs to one window and a control refuses another window's
  list ("Button must reject cross-Window ImageList assignment",
  `tests/basic_controls_tests.cpp:1694-1705`). Moving a panel's controls to a
  floating window therefore breaks their images unless images and theme
  become application-scope resources or the move re-registers them. This is
  the largest hidden foundation gap the study found.

**State.**

- G-STATE-1 (Known gap). No runtime window creation or destruction:
  `Application::run` takes the full window list before the loop starts. A
  floating panel is created when the user tears it off and destroyed when it
  is redocked. The host protocol needs `open_window` and `destroy_window`
  requests during a run (with an owner, tool-window style, initial frame and
  visibility) and must keep the 64-window bound or revise it.
- G-STATE-2 (Known gap). `UpdateScope` covers one `Window`. A move from a
  floating window into the main window changes two windows; the coherent
  update the owner asks for needs a workspace transaction that opens a scope
  on every affected window, creates new windows hidden, and shows and
  destroys windows in a fixed order after the layout commits (5.3).
- G-STATE-3 (Known gap). No runtime layout store. The resources interview
  separates session and layout state from configuration (RES §2.4) and
  proposes a transactional store for high-churn session data (RES §7); ribbon
  decision 12 also saves through "the layout store". Nothing in `include/` or
  `src/` implements it (grep for a layout store finds nothing). Docking and
  the ribbon share this prerequisite.
- G-STATE-4 (Known). Z-order: no request to bring a window to the front or
  to order one window above another, and no query of which window lies under
  a screen point. A cross-window drag must know which window the pointer is
  over when windows overlap.

### 2.4 The foundation, summarized per host

| Need | Windows | macOS | X11 | Portable seam today |
|---|---|---|---|---|
| Create and destroy an owned window during a run | `CreateWindowExW` with owner (Known: done at start only) | `NSPanel` + child window (start only) | transient window (start only) | none (G-STATE-1) |
| Window frame get/set in screen coordinates | `SetWindowPos`, `GetWindowRect` (Unverified API names) | `setFrame:` (Unverified) | `XMoveResizeWindow`, already used for full screen (`linux_host.cpp:767`) | client size request only (G-SIZE-1) |
| Pointer in screen coordinates | (Inferred: available natively) | (Inferred) | (Inferred) | none (G-SIZE-2) |
| Window under a screen point, z-order, bring to front | (Inferred: available natively) | (Inferred) | partial; window-manager dependent (Inferred) | none (G-STATE-4) |
| Floating window that does not hide on deactivation | default | needs `hidesOnDeactivate = NO` (Known default YES) | default | none |
| Floating window that does not move with the main window | default | not with `addChildWindow` (Known) | default | none |
| Utility window type, off the taskbar | `WS_EX_TOOLWINDOW` (done) | utility panel (done) | `_NET_WM_WINDOW_TYPE_UTILITY` (missing) | `tool_window` flag |
| Non-activating overlay for an off-window preview | private tooltip window | private tooltip panel | none found | none |
| Scale change when a window crosses monitors | yes | yes | no | `scale_notifications` |
| Capture across the screen during a drag | yes | yes | yes (Inferred) | `set_pointer_capture` |

Two notes. First, the hosts' GIVEN rule (`AGENTS.md`) applies: a floating
panel's behaviour must be one portable rule, with honest differences reported
as capabilities (for example, a window manager that ignores a position
request). Second, on Linux the supported path is X11, including under
XWayland; Qt Advanced Docking System's documented difficulties with floating
windows concern native Wayland, which GUI.Forms does not target (see 1.6;
`planning/stages/README.md`, "a Wayland-native adapter is not planned").
Whether XWayland honours client window positions under every compositor is
Unverified and must be measured (phase 0 tests).

---

## 3. The owner's requirements, one by one

For each: what the canonical systems do (section 1),
what GUI.Forms needs, and the split between framework and
client application. The owner marked lazy creation and resilient restore
"mostly on the client apps".

| Requirement | Canonical practice | GUI.Forms needs | Framework | Client application |
|---|---|---|---|---|
| Tool windows and documents distinct | Separate types or states everywhere (1.1) | `PanelDescriptor::kind`, document area, two activation tracks (4.2, 4.3, 5.6) | kinds, document area, dirty display, closing event, MRU history | what dirty means, prompt wording and result, in-document back and forward |
| Dock, float, tab, split, auto-hide, with restrictions | All but JetBrains and GIMP offer the full set; restrictions per panel (1.1) | layout tree, restrictions in three layers (4.4), floating windows (5.4), auto-hide (5.5); foundation: runtime windows, frames, screen pointer (2.3) | all mechanisms and checks | which panels allow what; dock groups |
| Drag previews, drop targets, cancel and invalid drop, nested and global splits | Pane guides plus site-edge guides; Escape cancels; no target floats or snaps back (1.8) | state machine (5.1), targets and priorities (5.2), overlay and preview parts (4.5) | all | none |
| Focus and activation across floating windows; command context | Active content versus active document; owned floating windows (1.2, 1.3, 1.5) | activation tracks, command routing chain, owner accelerator fallback, participation states (5.6; G-FOCUS-1, G-FOCUS-4) | routing and fallback | which commands target documents and which target tools |
| Stable identities and lazy creation | Callbacks by persist string or content ID; VS creates tool windows on demand (1.2, 1.3, 1.5) | `PanelId`, factories, placeholders with cached titles (4.3, 5.8 step 6) | calls factories by ID at the right moment | the factories, the IDs |
| Layout transactions | Telerik's transactions; others batch internally (1.4) | workspace transaction across windows (5.3; G-STATE-2) | all; the owner notes per-window coalescing already exists (`UpdateScope`) | none |
| Resilient restore | Callbacks may skip unknown content; DevExpress layout upgrade; Qt version argument (1.8) | 5.8 steps 1 to 8; monitors (G-SIZE-1, G-SIZE-4) | validation, format migration, monitor mapping, placeholders, pruning | application version hook, alias table for renamed panels, default layout |
| Keyboard alternatives | VS and Eclipse menus and switchers; no keyboard move mode found (1.8) | 5.7 bindings, navigator, position menu, keyboard move mode (5.7.1) | all bindings as rebindable commands | per-panel show shortcuts |

Each row's open points are questions in section 8: A and B (foundation),
C (model and restrictions), D (drag), E (auto-hide), F (focus), G
(keyboard), H (documents), I (persistence).


---

## 4. Proposed model in GUI.Forms terms

Everything in this section is a proposal for the interview. Every
vocabulary gap is marked **[vocabulary decision]**; under SPEC §12.7 adding an
atom changes the specification, `anatomy_types.hpp` and the schema together
and is the owner's decision. Where a gap exists, the "nearest existing atom"
column says what a builder would use in the meantime, as the W brief requires.
The gaps are collected as questions in 8.J.

### 4.1 Two layers: a layout model and the controls that show it

The canonical systems that age well separate a data model of the layout from
the controls that draw it: AvalonDock's `Layout*` classes beside its
controls, Qt ADS's state beside its widgets, Eclipse's e4 application model
beside its renderers (section 1). DockPanel Suite mixes them, which is why
its restore needs every content object to exist before layout is loaded
(1.2). The study proposes the separated form (Inferred, from those sources):

1. **`DockLayout`** (data, renderer-free, in `GUIForms::Controls` with no
   host): the tree of section 4.2 as records with stable identities. It is
   validated, transformed by transactions (5.3), saved and restored (5.8), and
   tested headless. It knows nothing about controls.
2. **`DockWorkspace`** (a component, as `ContextMenu` is a component): one per
   application or per main window. It owns the `DockLayout`, the panel
   registry (4.3), the drag session (5.1), the transaction engine (5.3), the
   floating windows (5.4) and the activation history (5.6). It realizes the
   layout as controls in each window, reusing them where it can.
3. **Controls** (4.5): the dock site, stacks, captions, tabs, seams, the
   auto-hide strip and flyout, the guide overlay, the navigator. They paint
   through `paint_part` and publish anatomy and semantics like every W
   control.

A consequence worth stating: a saved layout can be read, checked, migrated
and pruned without creating a single panel. That is what lazy creation and
resilient restore need (3.5, 3.7).

### 4.2 The layout tree

| Node | Children | Records |
|---|---|---|
| `workspace` | one `main_site` per main window, any number of `float_site`s, four `auto_hide_edge`s per main site, one `hidden_set` | format version, application layout version |
| `main_site` | one root `split`, `stack` or `document_area` | owning window's stable ID |
| `float_site` | one root `split` or `stack` | frame in screen logical units, monitor ID, normal or maximized, owner window ID |
| `split` | two or more `split`, `stack` or `document_area` nodes | orientation; one weight per child (proportional) and an optional fixed extent for one child; minimum extents derived from children |
| `document_area` | one `split` or `stack` of documents (the "document well"); may be empty | at most one per main site (option C2) |
| `stack` | one or more panel references, ordered | kind (tool or document), selected panel, stack ID |
| `auto_hide_edge` | groups of panel references | edge (left, top, right, bottom); per group the flyout extent |
| `hidden_set` | placeholders | per hidden or closed panel: its last placement (5.8) |
| panel reference (leaf) | none | `PanelId`; for a not-yet-created panel, cached title and icon key |

Rules (Inferred; each one an interview point where marked):

1. A `stack` holds only tool panels or only document panels (C3); a tool
   stack has its tab strip at the bottom and a caption at the top, a document
   stack its tabs at the top (VS convention; T4 taste question).
2. A `split` with one child is replaced by that child; an empty `stack` or
   `split` is removed. Pruning runs inside every transaction, so the tree is
   always minimal (DockPanel Suite and AvalonDock prune the same way; 1.2,
   1.3).
3. Splits are n-ary (AvalonDock's `LayoutPanel`, Qt's splitters) rather than
   nested binary pairs (DockPanel Suite's `NestedDockingStatus`), so that
   docking a third panel beside two others gives three equal columns instead
   of a half and two quarters (option C1).
4. The `document_area` is never removed from a main site even when empty, so
   documents always have a home (VS, AvalonDock, Eclipse's shared editor
   area; option C2).
5. Identities: `PanelId` is the application's stable string for a panel (for
   example `tools.properties`, `doc:file:///…`); `StackId` and `SiteId` are
   workspace-assigned stable strings saved in the layout. House style §1
   requires distinct wrapper types for these and for tree indices.

### 4.3 Panels: documents and tool windows

The owner: "Tool windows and document tabs as distinct concepts. Documents
tend to need activation, dirty state, close prompts, and navigation history;
tool windows usually need visibility and placement." Proposed records:

```text
PanelDescriptor            (registered by the application, before or after restore)
  id             PanelId
  kind           tool | document
  title, tooltip, icon     (Command-like display state; title may change)
  allowed        PlacementSet: dock_left, dock_top, dock_right, dock_bottom,
                 document_area, float, auto_hide, tab_with_others, close, hide
  group          DockGroupId (option C4: which panels may share a stack)
  default        a placement used when no saved layout places it
  minimum_size, preferred_size (logical units)
  creation       factory key: how to create the content on first show (5.8)
  multi_instance pattern, when one descriptor makes many panels (documents)

ToolPanelState             (tool windows)
  visible, placement (docked, floating, auto-hidden, hidden), last placement

DocumentPanelState         (documents)
  dirty (shown on the tab), close policy (prompt through an event, 5.7),
  read-only, preview (option H3), pinned (option H3)
```

Framework responsibilities (Inferred): show and hide; placement; activation
history in most-recently-used order; dirty display; the closing event and its
order; restoring by identity. Application responsibilities (owner's notes):
what dirty means, the prompt's wording and outcome, back and forward
navigation inside documents, creating content on demand, deciding what an
unknown saved panel becomes.

### 4.4 Restrictions

The owner: "restrictions on where individual panels may go" and "docking can
proceed dynamically for enabled control groups". Proposed three layers, all
checked in one function used by pointer drags, keyboard docking and
programmatic moves (Inferred):

1. **Per panel**, `PanelDescriptor::allowed` (DockPanel Suite's `DockAreas`,
   Telerik's `AllowedDockState`, Qt ADS's feature flags and allowed areas,
   AvalonDock's `CanFloat`, `CanAutoHide`, `CanClose`, `CanHide`; section 1).
2. **Per group**, `DockGroupId`: a panel may join a stack only when every
   panel in that stack has the same group, or the group admits the other
   (option C4). This is the study's reading of "enabled control groups"; the
   owner may mean something else (question C4).
3. **Per site**: a site or window may refuse kinds (for example a float site
   that accepts tools only) and a workspace-wide lock turns user docking off
   (DockPanel Suite's `AllowEndUserDocking`, Telerik's lock, Qt ADS's
   `lockDockWidgetFeaturesGlobally`; section 1).

A target that fails any layer is not offered as a guide at all (rather than
offered and refused), except the "drop here does nothing" case of 5.2, which
shows the `drop_rejected` state over the hovered area (option D4).

### 4.5 Control types, parts and item systems

The T1 limits shape the split, as for the ribbon (RB study §4.1): one control
declares at most four item systems and 32 own parts
(`anatomy_types.hpp:113-115`). Docking has more repeated kinds than that
(tabs of tool stacks, tabs of document stacks, seams, auto-hide tabs, guide
buttons, navigator entries), so it is several controls.

| Type | Kind | Item systems | Notes |
|---|---|---|---|
| `DockSite` | control, one per window root | `seam` | Lays out the realized tree: splits (as seams between children; n-ary), stacks, document area, the four auto-hide strips. Seams are records, not `SplitContainer` instances, because splits are n-ary (C1) and a seam's states are already defined by `SplitSeamState`. |
| `ToolStack` | control | `tab` | Caption with title, the position-menu, auto-hide (pin) and close actuators; content host; bottom tab strip when it holds two or more panels. |
| `DocumentStack` | control | `tab` | Top tab strip with per-tab close actuator and dirty mark; overflow actuator and active-files menu; content host. |
| `AutoHideStrip` | control (one per edge in use) | `side_tab` | Tabs along a window edge for auto-hidden panels. |
| `AutoHideFlyout` | control in the window's popup layer | none | Slides out over the site with one panel's caption and content (5.5). |
| `DockGuideOverlay` | control in the window's overlay layer | `guide` | Drop guides and the preview area during a drag or keyboard move (5.2). |
| `DockNavigator` | control in a popup | `entry` | Ctrl+Tab switcher: tool panels and documents in MRU order (5.7). |
| floating window | an `ApplicationWindow` | none | Holds one `DockSite` for its float site; chrome per option T6. |

#### `ToolStack` parts

| Part | PartRole | States | Nearest existing atom / gap |
|---|---|---|---|
| `region` | region | | |
| `caption` | face | `current` (the stack holds the active panel), window participation | DNA-P04 pane caption: "Studio 2003 warm-neutral factual strip with a tiny identity key" |
| `caption_label` | label | | |
| `caption_icon` | icon | | |
| `position_button` | actuator, composed from `button` | hot, pressed, open | mark `chevron_down` exists |
| `pin_button` | actuator, composed from `button` | hot, pressed, `checked` (pinned) | **[vocabulary decision]** no pin mark; nearest `arrow_down`/`arrow_left` (VS draws a pushpin) |
| `close_button` | actuator, composed from `button` | hot, pressed | mark `close` exists |
| `content` | well or region | | Studio 2003 panel interior; panels are flat, never raised (SPEC §3.2; T4 brief) |
| `focus` | focus | | |

Item system `tab` (bottom tabs): `tab` (face; selected, hot, pressed,
captured while dragged), `tab_label` (label), `tab_icon` (icon), `tab_focus`
(focus), as `TabControl` already declares (`tab`, `tab_label`, `tab_focus`,
`tab_seam`).

#### `DocumentStack` parts

| Part | PartRole | States | Nearest existing atom / gap |
|---|---|---|---|
| `region` | region | | |
| `tab_rail` | face | | |
| `overflow_button` | actuator | hot, pressed, open | mark `overflow` (admitted for the ribbon, RB-20, branch `claude/w-ribbon-phase-1`) or `chevron_down` |
| `content` | well | | the work pane: recessed (SPEC §3.2 "Work pane deepest") |
| `focus` | focus | | |

Item system `tab`: `tab` (face; selected, `current` when the stack is
active, hot, pressed, captured), `tab_label`, `tab_icon`, `tab_close`
(actuator, mark `close`; visible on selected or hot tabs, T4 taste),
`tab_modified` (glyph) **[vocabulary decision]**: a dirty mark, or a custom
item state `modified` that changes the label (VS appends `*`), `tab_focus`.

#### `DockSite` parts

| Part | PartRole | States | Nearest existing atom / gap |
|---|---|---|---|
| `region` | region | | |
| `chassis` | face | | the graphite chassis between panes (SPEC §4.7 House Composite) |
| item `seam` | seam | hot, captured, focused, disabled; drag-proximate | `SplitSeamState` already has idle, near, hot, dragging, focused, disabled, collapsed |
| item `seam_grip` | grip | as DDV-007-04 | mark `grip_dots` exists |

#### `DockGuideOverlay` parts

| Part | PartRole | States | Nearest existing atom / gap |
|---|---|---|---|
| `preview` | drop | `drop_accepted` | the translucent area the panel would occupy; the role `drop` exists |
| `rejected_area` | drop | `drop_rejected` | option D4 |
| item `guide` | face | hot (pointer over it), `drop_accepted` (it is the current target), disabled (not allowed, if shown at all) | |
| item `guide_mark` | glyph | | arrows: `arrow_up`, `arrow_down`, `arrow_left`, `arrow_right` exist; **[vocabulary decision]** a "tab into" mark for the centre guide, and an "outer edge" variant (VS draws a different glyph for outer guides) |
| item `guide_backplate` | shadow or face | | the diamond or cross behind the five buttons; taste question T2 |

#### `AutoHideStrip`, `AutoHideFlyout`, `DockNavigator`

`AutoHideStrip` item `side_tab`: face, label, icon; states hot, `open`
(its flyout is out). **[vocabulary decision]** labels along a left or right
edge read vertically in VS, JetBrains and DockPanel Suite (section 1);
`PartPaint` has no text orientation, so either the paint path gains a
rotated-label parameter or side tabs are icon-and-tooltip only (question
J4). `AutoHideFlyout`: `shadow` (E4 overlay shadow or E3, taste T5), `face`,
the stack's caption parts, a `grip` on the inner edge for resizing.
`DockNavigator` item `entry`: face, icon, label, `selected`; own parts
`face`, `caption` ("Active Tool Windows", "Active Files" in VS), `preview`
area (option G1).

#### Window-level states

Floating windows and the main window take the participation states
`key`, `secondary_participating`, `drag_proximate` and `deactivated`
(SPEC §8.3; DDV-007-01). The main window stays `secondary_participating`
while a floating tool window owned by it is key, never deactivated-looking
(DDV-007-01: "Never make a secondary participating window appear disabled").
`drag_proximate` is how a window under a dock drag regains destination
emphasis without taking focus (DDV-007-01; DNA-F03). These atoms exist.

### 4.6 Semantic roles and actions each host needs

| Element | Today's nearest role | Gap |
|---|---|---|
| dock site | `group` | none needed (Inferred) |
| tool stack | `tab_group` when tabbed, else `group` | **[vocabulary decision]** a `pane` role (MSAA `ROLE_SYSTEM_PANE`, UIA Pane, ATK panel; Unverified mapping names) so a tool window reads as a named region and F6 targets are landmarks |
| document stack | `tab_group` | |
| tab | `tab` | position in set and set size (already planned for the ribbon, RB-22) |
| seam | `split_pane` | none: `SplitContainer` publishes `split_pane` with the distance as a numeric value, minimum and maximum, and `increment`, `decrement`, `set_value` (`src/controls/scrollable_control/container_control/split_container/split_container.cpp:510-525`, Known); an n-ary site would publish one such virtual child per seam (Inferred) |
| floating window | the native window root | none (Inferred) |
| guide overlay | not exposed | the keyboard move mode announces the current target as a live text change (Inferred) |
| navigator | `list` with `list_item` | |

Actions: **[vocabulary decision]** `SemanticAction` has no `close`. A
document tab's close, a tool window's hide and "dock as" placements are
actions an assistive technology should reach (a11y round §3.5 gate 2: "The
same action invoked by keyboard, pointer, and assistive technology reaches
one command path"). Proposed: `close` as a standard action; placement moves
as the position menu's items (reachable as menu items, so no new action).

### 4.7 What is not proposed

- No MDI. MDI is excluded by `decisions/ADR-001-portable-compatibility-controls.md`
  and matrix row 46. Documents live in the document area, not in child
  windows (DockPanel Suite's `DocumentStyle.SystemMdi` has no counterpart).
- No third-party code. Matrix row 43 and the DK brief: the canonical
  libraries are study material, not dependencies or facades.
- No requirement that applications use docking. Owned dialogs remain
  first-class capabilities (`AGENTS.md`), as ADR-013's second point said of
  secondary tools; docking is offered, not imposed.

---

## 5. The hard algorithms

Each algorithm is specified so that a builder could write the tests first.
Numbers are proposals unless a source is cited. Pointer, keyboard and
programmatic paths share one target set and one commit (a11y round §3.5
gate 2).

### 5.1 The drag state machine

States: `idle`, `armed`, `dragging`, `committing`, `cancelled`.

| From | Event | To | Action |
|---|---|---|---|
| idle | primary press on a tab, a stack caption, or a floating window's caption | armed | Record the source (one panel from a tab; the whole stack from a caption, option D8), the press point in screen units, the source window. Capture the pointer. Nothing changes on screen. |
| armed | pointer moves less than the threshold | armed | Nothing. |
| armed | pointer moves at least the threshold from the press point (proposed 4 logical units each way, overridable per host; G-INPUT-5) | dragging | Open the drag session: snapshot the layout (generation number), compute the candidate targets of every site (5.2), show the guide overlay in the site under the pointer, set the source tab or caption `captured`. In "preview" mode (option D1) the layout is not changed. In "immediate float" mode the source is torn off into a floating window now (one transaction, 5.3) and the window follows the pointer. |
| armed | release, or Escape | idle | Release capture. A release on a tab without movement is an ordinary click (select the tab). |
| armed or dragging | capture lost, the source window closes or is deactivated by the system, a modal dialog opens, the source panel is closed by code | cancelled | As Escape. |
| dragging | pointer moves | dragging | Find the window and site under the pointer (screen point; G-SIZE-2, G-STATE-4). Resolve the target (5.2). Update guides and preview with damage limited to their old and new rectangles. Set `drag_proximate` on that window when it is not the source window (DDV-007-01). |
| dragging | modifier held (proposed Ctrl, option D6) | dragging | Suppress docking targets: the drop would float. |
| dragging | Escape | cancelled | Remove guides and preview. Preview mode: the layout was never changed. Immediate mode: roll the tear-off back (the saved snapshot is restored in one transaction), unless the owner prefers to leave it floating where it is (option D1b). |
| dragging | release over a valid target | committing | Build the move as one transaction (5.3) and commit it; activate the moved panel (5.6). |
| dragging | release with no target | committing or cancelled | If the panel may float: float it with its frame at the pointer (VS; DockPanel Suite; section 1). If not: cancel, the panel stays where it was (option D4). A drop never loses a panel. |
| committing | transaction done | idle | Release capture; clear `captured`, `drag_proximate`; close the session. |

Invariants (Inferred, each a test):

1. Exactly one drag session exists per workspace; a second press while one is
   live is ignored.
2. The layout generation recorded at drag start must equal the generation at
   commit; if code changed the layout during the drag (a panel closed by the
   application), the drag is cancelled, never applied to a stale tree.
3. Cancel leaves the layout byte-identical to the snapshot (compare
   serialized forms).
4. No intermediate layout is published while dragging in preview mode: the
   layout-changed event count is zero until commit.

### 5.2 Drop target computation: nested and global splitting

The owner: "nested as well as global splitting depending on where a dock is
dragged to". Every canonical system read implements this distinction with
two sets of targets (section 1): targets around a pane split that pane
(nested), and targets at the site's outer edges split the whole site
(global). Proposed target kinds, highest hit priority first:

| Priority | Kind | Where it is | Result |
|---|---|---|---|
| 1 | Tab insertion | Pointer over a stack's tab strip | Insert the panel into that stack at the gap nearest the pointer (also reorders within a stack). Preview: an insertion bar between tabs plus the stack's content area. |
| 2 | Guide button | Pointer inside one guide's bounds | The guide's own result (below). |
| 3 | Edge zone (option D2) | Pointer inside the outer band of a pane, without a guide | Same result as that pane's side guide (Inferred; no system read here was confirmed to use button-less edge zones). |
| 4 | Float | Anywhere else, panel may float | Float at the pointer. |
| 5 | Nothing | Panel may not float | No target; `drop_rejected` cue at the pointer's site (option D4). |

Guides shown during a drag (VS and AvalonDock layout; section 1):

1. **Pane compass** (nested): centred on the stack under the pointer. Five
   buttons: left, top, right, bottom split that stack's parent at the
   stack's position (creating a new split if the parent's orientation does
   not match), centre tabs the panel into the stack.
2. **Document-area compass**: when the pointer is over the document area,
   centred on the document stack under the pointer. Its inner five buttons
   split or tab the document stack (document panels; tool panels only if
   their `allowed` contains `document_area`). Its outer four buttons (VS
   draws them as arms beyond the inner ring) dock a tool panel beside the
   whole document area (a split whose sibling is the `document_area` node).
3. **Site edge guides** (global): four buttons centred on the main site's
   edges, inset a fixed distance. Dropping on one creates a split at the
   root of the site, so the panel spans the full height or width of the
   window (DockPanel Suite's panel indicators; AvalonDock's
   `DockingManagerDropTarget`; Qt ADS's container overlay; section 1).
4. **Float site guides**: a floating window shows its own pane compasses for
   its stacks; it has no document-area compass unless the owner admits
   documents in float sites with splits (option C2).

Geometry (proposed, in logical units; taste question T2 for the look):

```text
compass button        32 × 32, gap 4; compass footprint 112 × 112
edge guide button     32 × 32, centred on the edge midpoint, inset 8
preview for a side    the target pane's rectangle cut to the share the panel
                      would get: the panel's saved extent along the split
                      axis, else half the pane, clamped to [minimum,
                      pane extent − sibling minimum]
preview for global    the site rectangle cut along the edge to the panel's
                      saved extent, else one third of the site, clamped
preview for centre    the target stack's whole rectangle (tab)
preview for float     the panel's last floating frame placed so the pointer
                      keeps its offset within the grabbed tab or caption
```

Rules (Inferred):

1. Only the compass of the pane under the pointer is shown, plus the site's
   edge guides, plus the document-area compass's outer arms when the pointer
   is over the document area. This matches what VS shows (section 1) and
   keeps the overlay's damage to a few small rectangles.
2. A guide whose result the restrictions refuse (4.4) is not drawn.
3. A compass is not shown over the source stack when the source is that
   stack's only panel (dropping a stack onto itself changes nothing), but its
   tab strip still accepts reordering.
4. When a compass would overlap a site edge guide (a small pane near an
   edge), the edge guide keeps its place and the compass shifts inward until
   they do not overlap; if the pane is smaller than the compass footprint,
   only the centre button is shown (Inferred; DockPanel Suite and AvalonDock
   behaviour here is Unverified).
5. The target is recomputed from scratch on each pointer move from the
   layout snapshot; no state carries over between moves except the hovered
   guide (for hysteresis of 2 units at guide edges, proposed).
6. Hit-testing order is the table's priority order, with the guide overlay
   above all controls. The overlay never takes focus and never receives
   ordinary input (it is hit-transparent except during a drag).

Nested versus global, in one sentence for the interview: dropping on a
pane's own compass splits that pane where it stands; dropping on the window
edge guides splits the whole window, so the panel runs along the entire edge.

### 5.3 Layout transactions

The owner: "A move that changes several containers should produce one
coherent layout update, with no intermediate flicker or event chaos (we've
managed most of this)". What GUI.Forms has managed is per window (2.1:
`UpdateScope`, read barriers, bounded flushes, DP damage). What docking adds
is that one user action touches the tree, several controls and possibly
several native windows.

Proposed transaction (Inferred; Telerik RadDock's documented transaction
concept is compared in section 1):

1. **Plan.** The action is translated into a list of primitive edits on the
   `DockLayout` model: `remove(panel)`, `insert_tab(stack, index, panel)`,
   `split(node, side, extent, panel)`, `float(panel, frame)`,
   `auto_hide(panel, edge)`, `restore(panel)`, `set_weights(split, …)`.
2. **Validate.** Apply the edits to a copy of the model; check restrictions
   (4.4), minimum sizes, tree invariants (4.2 rules), window limits. Any
   failure rejects the whole transaction with a reason; the model and
   screen are untouched (house style §9: validate before mutating).
3. **Commit the model.** Swap the copy in; prune; bump the layout generation.
4. **Reconcile windows.** Before any control moves: create the floating
   windows the new model needs, hidden (G-STATE-1). Open an `UpdateScope` on
   every window whose site changes (G-STATE-2).
5. **Reconcile controls.** Re-parent existing panel content and stacks to
   their new places (no content is destroyed and re-created by a move),
   create stack and seam controls the new model needs, retire ones it does
   not. Focus is held: the focused control is recorded before and restored
   after (focus scopes already restore focus, `window.hpp:188-196`).
6. **Close scopes.** Each window lays out once and paints its damage once.
7. **Show and destroy windows.** Show new floating windows; then hide and
   destroy floating windows left empty. Order: show before destroy, so a
   panel moving between two floating windows is never invisible.
8. **Publish events,** after everything above, in a fixed order:
   per-panel `placement_changed` (in tree order), then one
   `layout_changed{generation, reason}` for the workspace, then
   `active_panel_changed` if activation moved. Handlers that mutate the
   layout start a new transaction; they do not re-enter this one (the
   phase guard of GF006 L4).

Bound: one transaction may create at most the windows the model asks for
(proposal: a float limit of 32, below `Application::maximum_windows = 64`,
option B1). Test: a drag from one floating window into another closes the
first, and the trace shows exactly one layout pass and one paint per
affected window and one `layout_changed`.

### 5.4 Floating windows: creation and redocking

Creation (tear-off or float command, Inferred unless cited):

1. The float frame: the panel's last floating frame if it has one and it
   lies on a connected monitor (5.8 rule 4); else its current on-screen
   rectangle offset by the drop point; else a default size centred on the
   pointer.
2. The window: owned by the main window of the workspace, `tool_window`
   style, not minimizable, not in the taskbar (2.2; option B6 for X11), not
   hidden on application deactivation (option B5 for macOS), not moving with
   the main window (option B5).
3. Content: the site holds the panel's stack. The panel's controls move from
   the main window's model to the floating window's model; their images and
   theme must move or be application-scope (G-PAINT-2, option B3).
4. Chrome (taste T6): a floating window with one tool stack shows that
   stack's caption as its title bar; with several stacks, a thin frame and
   per-stack captions (VS; DockPanel Suite; section 1).

Moving and redocking: a floating window is dragged by its stack caption with
the framework's own capture-driven drag (5.1), which moves the native window
each pointer move through a frame request (G-SIZE-1), so guides in other
windows can follow the pointer. Using the native move loop instead
(the existing `WindowChromeHitRole::drag_region`) would need the hosts to
report move progress and the pointer's screen position during the native
loop, which they do not today (G-INPUT-2; option B7). AvalonDock uses the
native loop and pays for it: no explicit Escape, and an invalid drop leaves
the window floating (1.3). Double-clicking a floating window's caption
redocks its panes to their last docked places, and double-clicking a docked
tab floats it (DockPanel Suite; VS uses Ctrl+double-click for both; 1.2,
1.5).

### 5.5 Auto-hide flyouts

Auto-hide removes a panel (or a whole stack, option E4) from the tree and
puts a side tab on the window edge nearest its last docked place. Proposed
behaviour (option E1 to E3; sources in section 1):

| Event | Behaviour |
|---|---|
| Pointer rests on a side tab for the hover delay (proposed 400 ms, AvalonDock's value) or the tab is clicked (VS: "select the tab") | Open the flyout from that edge over the site, at the panel's flyout extent. The flyout is an in-window overlay (`open_popup`), like the ribbon's popups (RB-23: "Menus opened from the ribbon are popups over it"). |
| Flyout opened by click or keyboard | Focus moves into it. |
| Flyout opened by hover | Focus stays where it was until the user clicks inside. |
| Pointer leaves the flyout and its tab, flyout not focused | Close after the leave delay (proposed 500 ms, AvalonDock's `AutoHideDelay`; DockPanel Suite polls at twice the hover time). A focused flyout stays open (DockPanel Suite, AvalonDock). |
| Focus leaves the flyout (click elsewhere, F6, Ctrl+Tab), or Escape inside it | Close; Escape returns focus to the previously active panel. |
| Another side tab activated | Swap flyouts without animation of the closing one. |
| Pin button in the flyout's caption | Dock the panel back to its last docked place (transaction 5.3). |
| Inner edge grip dragged | Resize the flyout; the extent is saved. |

Motion: open and close over the standard duration (150–180 ms) with
smoothstep easing (SPEC §9.2), immediate under reduced motion (SPEC §9.5;
DNA-A06). Shadow: E4 overlay or E3 floating (SPEC §3.4; taste T5). A click
outside the flyout closes it and is passed through (option E2; the ribbon's
minimized popup consumes the click instead, RB-12, and the owner may want
the same rule here).

### 5.6 Focus, activation and command context across floating windows

The owner: "Clicking a floating tool window should preserve the
application's command context where appropriate." Proposed model
(Inferred, from the systems in section 1):

1. **Two activation tracks.** The workspace keeps an `active_panel` (the
   panel that has, or last had, keyboard focus) and an `active_document` (the
   document panel last active). Activating a tool panel changes
   `active_panel` only; `active_document` changes only when a document is
   activated or the active document closes. This is how VS, DockPanel Suite
   (`ActiveContent` versus `ActiveDocument`) and AvalonDock (`ActiveContent`
   versus the last focused document) behave (section 1).
2. **Command context.** A command's target is resolved through a context
   chain: the focused control's route in the key window; then the active
   panel; then the active document; then the main window. Commands that act
   on documents (Save, Undo in an editor, Find) target `active_document`
   even while a floating tool window is key. Commands bound to a tool panel
   target it only while it is the active panel. This needs a command routing
   seam GUI.Forms does not have (G-FOCUS-4; option F2).
3. **Accelerators across windows.** A key unhandled by a floating window's
   own route and accelerators is offered to its owner window's accelerators,
   then to the workspace's command bindings (G-FOCUS-1). The ribbon's and
   menus' shortcuts therefore work from a floating tool window.
4. **Activation on click.** Clicking in a floating tool window activates it
   (it becomes key, receives keyboard focus). The main window shows
   `secondary_participating` (SPEC §8.3). The alternative, a non-activating
   palette window that never takes key status (macOS `becomesKeyOnlyIfNeeded`
   style), keeps the main window key but makes keyboard entry in the panel
   impossible without a click in a text field (option F5).
5. **Return.** Escape in a tool panel (not consumed by the content) returns
   focus to the active document (JetBrains; section 1); Shift+Escape hides
   the tool panel and returns focus likewise (VS, JetBrains).
6. **Window participation.** When the application deactivates, all its
   windows show `secondary_participating` (or the host's inactive look);
   floating windows do not hide on macOS unless option B5 says so.

### 5.7 The keyboard model

The owner: "Users should be able to move, dock, activate, and close panels
without a mouse." Proposed default bindings, each a command that applications
may rebind; defaults per host where a host convention differs, reported as a
capability default rather than core policy (`AGENTS.md` GIVEN rule;
option G6):

| Task | Proposed key | Source |
|---|---|---|
| Switch panels and documents (navigator) | Ctrl+Tab, Ctrl+Shift+Tab; release Ctrl to activate; arrows move between the tool list and the document list | VS IDE Navigator, DockPanel-based apps, DevExpress document selector, Infragistics PaneNavigator (section 1) |
| Next or previous document | Ctrl+F6, Ctrl+Shift+F6 | VS (`Window.NextDocumentWindow`), Eclipse Ctrl+F6 editors (section 1) |
| Next or previous tool panel | Alt+F7, Alt+Shift+F7 (navigator opened on the tool list) | VS; Eclipse Ctrl+F7 views (section 1) |
| Cycle window regions | F6, Shift+F6 | RB-17; each visible tool stack and the document area is a region (G-FOCUS-3). VS binds F6 to `Window.NextSplitPane` and Alt+F6 to the next pane (1.5); RB-17 already gives F6 to regions |
| Open the panel's position menu (Float, Dock, Dock as Document, Auto Hide, Hide, Close, Move) | Alt+Hyphen (VS `Window.ShowDockMenu`), and the context-menu key or Shift+F10 on a tab or caption | VS, Eclipse (1.5, 1.7) |
| Move or dock by keyboard | "Move" in the position menu, or Ctrl+Alt+Shift+M (proposed) | 5.7.1 below |
| Return to the active document | Escape in a tool panel | JetBrains (section 1) |
| Hide the active tool panel | Shift+Escape | VS, JetBrains (section 1) |
| Close the active document | Ctrl+F4 (Windows, X11), Cmd+W (macOS) | host conventions (Unverified for X11) |
| Resize the active panel | Focus its seam with F6 or Tab and use arrows (as `SplitContainer` today); or Ctrl+Alt+Shift+arrows (JetBrains); Eclipse's Alt+Hyphen size menu moves a chosen side with arrows | `split_container.cpp:490-507`; 1.7 |
| Show a specific panel | Application commands (View menu), e.g. Alt+number | JetBrains; application side |
| Maximize the active document area (hide tool panels) | Shift+Alt+Enter is VS's full screen; Ctrl+M Eclipse; option G7 | section 1 |

#### 5.7.1 Keyboard move mode

1. The user opens "Move" on the active panel. The guide overlay appears as in
   a pointer drag, with the current placement highlighted and announced
   (semantic live text: "Docked left of Editor").
2. Arrow keys step through the targets that the pointer would offer, in a
   spatial order: Left and Right move among horizontal targets of the
   current compass, Up and Down among vertical ones; Tab and Shift+Tab move
   to the next pane's compass (in F6 region order), then the site edge
   guides, then "Float". Each step updates the preview exactly as a pointer
   hover would.
3. Enter commits through the same transaction as a pointer drop (5.3);
   Escape cancels with no change.
4. When the target is "Float", arrow keys move the preview frame by 16
   units (Shift: 1 unit), Enter commits.

This gives keyboard users every placement the pointer has, through the same
target list, which is the a11y round's one-command-path gate.

### 5.8 Persistence, versioning and resilient restore

What a saved workspace holds (proposed):

```text
workspace layout
  format_version         framework's layout format (integer, starts at 1)
  application_version    the application's own layout version (integer)
  saved_by               framework version string (diagnostic only)
  monitors               id, frame, work area, scale of every monitor at save
  main windows           stable ID, frame, normal or maximized, monitor ID
  sites                  the tree of 4.2: splits with weights, stacks with
                         panel IDs and selection, document area, auto-hide
                         edges with extents
  floating windows       site tree, frame, monitor ID
  hidden placeholders    panel ID, kind, last placement (site, path, extent)
  panel entries          panel ID, kind, cached title and icon key, and an
                         optional application state string per panel
                         (bounded; DockPanel Suite's persist-string idea)
```

Where it is saved: the runtime layout store (RES §2.4, G-STATE-3), the same
store the ribbon uses (RB-12), with explicit save and load also available
(option I2). Syntax: the store's syntax is not yet chosen (RES §7: "The
syntax needs a separate decision"); the layout should use whatever that
decision gives, not a docking-only format (option I1).

Restore, in this order (each step deterministic; Inferred unless cited):

1. **Read and validate the whole record** before changing anything (RC005:
   validate the whole file, apply atomically, keep the running state on an
   invalid edit), as Qt ADS's dry-run pass does (1.6); DockPanel Suite
   parses fully but can leave a partly built panel on a late failure (1.2).
   An unreadable record leaves the application's default layout and a
   diagnostic.
2. **Framework version.** An older `format_version` is upgraded step by step
   by the framework's own migrations. A newer one is read best-effort:
   unknown fields are preserved but not activated (RES §7), unknown node
   kinds are dropped with their panels moved to the hidden set, and a
   diagnostic names them.
3. **Application version.** A different `application_version` calls the
   application's upgrade hook with the parsed model before it is applied
   (DevExpress offers a veto before load, `BeforeLoadLayout`, and a fix-up
   after it, `LayoutUpgrade`; Qt rejects a mismatch outright; 1.5, 1.6). The hook may
   rename panel IDs (renamed panels), move panels, or reject the layout
   (then the default layout is used).
4. **Monitors.** Each saved window frame is placed on the monitor with the
   same ID if it is connected; otherwise on the monitor whose work area
   overlaps the saved frame most; otherwise on the primary monitor, at the
   same position relative to the work area's origin. The frame is then
   scaled by the monitor scale ratio and clamped so that at least the
   caption's full height and a 64-unit width of it lie inside a work area
   (proposed numbers). A maximized main window is restored maximized on the
   chosen monitor.
5. **Panel identities.** Each panel ID in the record is looked up in the
   registry. Known: placed. Renamed: the application's alias table maps the
   old ID (step 3). Unknown (a removed plugin): kept in the record as an
   inactive placeholder so that the plugin's return restores it, and not
   shown; empty stacks and splits are pruned from the visible tree (option
   I3). DockPanel Suite's placeholder content and Infragistics'
   `InitializePaneContent` are the canonical forms of this (1.2, 1.5).
6. **Lazy creation.** Only panels that are visible after restore and
   selected in their stack are created (through their factory, 4.3);
   unselected tabs show their cached title and icon and are created when
   first selected; hidden and auto-hidden panels are created on first show.
   The owner marks this "mostly on the client apps": the framework calls the
   factory by ID and the application decides what to build (option I7).
7. **Missing panels at runtime.** A panel registered after restore (a plugin
   loaded later) is placed where its placeholder says, if one exists, else
   at its descriptor's default.
8. **Apply** as one transaction (5.3): every window and panel appears once,
   in its final place. A floating window whose site ends up empty is not
   created.

The owner marks resilient restore "mostly on the client apps to
facilitate". Framework parts: steps 1, 2, 4, 5 (mechanism), 6 (mechanism),
8. Application parts: step 3's hook, step 5's alias table, factories, and
the choice of default layout.

---

## 6. Fit with the stages and with File Manager

| Stage | What docking needs from it, or gives it |
|---|---|
| W (controls) | Docking's controls are new controls under W's rules (articulated from birth, only vocabulary atoms, `paint_part`, semantics, tests on every platform), as the ribbon is built in W (RB-1). W's P3 `TabControl` work (overflow, images) and the tool-strip family's overflow mark are reused by document stacks. `ToolStripContainer`'s missing strip dragging ("not supported yet") is a small docking problem; it could use the same drag session and target computation later (option K2). |
| RB (ribbon) | Shares four prerequisites: F6 regions (RB-17, branch `claude/w-ribbon-phase-0`), the runtime layout store (RB-12), the position-in-set semantic fields (RB-22), and popups over a control rather than inside it (RB-23), which the auto-hide flyout follows. |
| L (layout) | The dock site is the "shell layer" of interview GF008 ("application/window composition uses a GUI.Forms-owned flex algorithm"); its n-ary weighted splits with minimums are a flex row or column. Whether the split solver is written in DK or taken from L's flex containers is a sequencing question (option A4). |
| DP (painting) | Guides, previews and seam drags must produce rectangle damage only; a drag over a window repaints the guide and preview rectangles, not the window. Idle after a drop: zero frames. |
| T2 (engine) | Auto-hide slide, guide fade-in and the floating preview are presentational motion; until T2 they snap (no animation), which reduced motion requires anyway. |
| T4 (default theme) | The look of captions, tool tabs, document tabs, guides, previews, side tabs and floating chrome (taste questions T1 to T8). Panels are flat, never raised (T4 brief; SPEC §3.2); document tabs follow the Plan Paint tab stack only if the owner wants document tabs to look like the ribbon's tabs (T4). |
| A (accessibility) | The `pane` role, the `close` action, region landmarks for F6, live announcements in keyboard move mode, and their MSAA, AX and ATK mappings (4.6; option J). |
| FD (form documents) | The application's default layout (which panels, where) is authored data and belongs in a form document; the user's saved layout is session state in the layout store (RES §2.4). |
| D (interface builder) | The builder is the first large docking consumer: toolbox, canvas, inspector, document tabs (D brief, decision 1: "persistent panels (toolbox, canvas, inspector)"). Building D's shell on DK is a dogfood test. |
| V (theme designer) | Same as D (V brief, decision 1). |
| C (C ABI), S (C# sibling) | Docking's public surface becomes ABI later. Windows Forms has no docking; the matrix says "third-party docking internals do not enlarge the facade" (row 43), so the C# sibling exposes GUI.Forms' docking under its own names, not DockPanel Suite's (Inferred). |
| R (SDRSharp reflection) | The T4 brief lists docking among SDRSharp's visual demands, and RES §6 records that the specimen keeps its dock layout in a separate `Layout.xml`. Which docking library SDRSharp uses is Unverified here; if it matters for R, stage R should record it. |

### File Manager's panels

File Manager's topology is fixed by its verdicts: one location per window, no
tabs or dual content panes (DNA-F01; STYLE_FAMILY_ATLAS_004: "No candidate
introduces tabs, a dual pane"), tree and selection panes collapsible around a
single content surface (DNA-P01), seams 3 px or thinner with a grip
(DNA-P02, DNA-P03, DDV-007-04), pane captions as a Studio 2003 strip
(DNA-P04), drag and drop conventional with "no novel house overlay"
(DDV-007-06, DNA-O10). The demoboard builds this with two `SplitContainer`s
(2.1). Docking would add user rearrangement File Manager's verdicts do not
ask for. Recommendation (option K1): File Manager does not adopt docking;
its panes stay a fixed split composition. Docking's seams, captions and
collapse grip should still share parts and looks with File Manager's, so the
two read as one design language.

DDV-007-06 and DNA-O10 concern file drag and drop, not panel docking, but
they show the owner's caution about drag overlays ("no theatrical house
overlay"). Docking guides are by nature an overlay; how quiet they must be is
taste question T2.

---

## 7. Phased build plan

Each phase ends with tests in `tests/` that CI runs on every platform, and
its own pull request (or stack). The owner's framing sets the order: the
windowing foundation first, the panels on top. Phases 1 and 2 can overlap
phase 0 because the layout model is renderer-free.

**Phase 0. Windowing foundation (host protocol, every host).**
Runtime creation and destruction of owned windows (G-STATE-1); window frame
get and set in screen logical units with a frame-changed event (G-SIZE-1);
the pointer's screen position and the window under a screen point
(G-SIZE-2, G-STATE-4); bring-to-front; the X11 utility window type and
skip-taskbar hint; the macOS floating-window policy (option B5); application-
scope images and theme, or a defined move of a control subtree between
windows that carries them (G-PAINT-2, option B3); owner fallback for
accelerators (G-FOCUS-1); a workspace-level transaction that opens update
scopes on several windows (G-STATE-2); the runtime layout store's first
form (G-STATE-3, shared with the ribbon). Each new request is a capability
bit with `unsupported` where a host cannot do it. Tests: headless host traces
for window creation, frame requests and ordering; per-host capability
records; moving a subtree with images between two windows keeps its images;
an accelerator on the main window fires from an owned window; one paint per
window for a two-window transaction; XWayland position honouring measured
and recorded (not assumed).

**Phase 1. The layout model (renderer-free).** `DockLayout` records, panel
descriptors, restrictions, primitive edits, validation, pruning,
transactions on the model, serialization with format and application
versions, restore steps 1 to 5 of 5.8 (versions, monitors, unknown and
renamed IDs). Tests: tree invariants after every edit (property-style test
over random edit sequences); restrictions refuse with reasons and leave the
model unchanged; round trip save and restore is byte-stable; restore against
fixtures: a missing monitor, a removed plugin's panel, a renamed panel via an
alias, an older format version, a newer format version with unknown fields,
a corrupt record (default layout and a diagnostic).

**Phase 2. Docked layout in one window.** `DockSite` with n-ary seams and
minimums, `ToolStack`, `DocumentStack`, the document area, show, hide and
close, tab selection, the position menu (Float disabled until phase 4), F6
regions for stacks, `DockNavigator` on Ctrl+Tab, Escape and Shift+Escape,
activation tracks (`active_panel`, `active_document`), anatomy, schema and
semantics, every part through `paint_part`, lazy creation by factory. Tests:
part geometry; seam keyboard resizing; minimums honoured at small window
sizes and the 150 × 150 minimum; activation tracks across tool and document
activation; navigator order is MRU; semantic snapshots; lazy creation calls
factories only for visible selected panels.

**Phase 3. Dragging and drop targets in one window.** The drag state
machine (5.1), target computation with nested and global targets (5.2), the
guide overlay and preview, tab reordering, cancel and invalid drop, keyboard
move mode (5.7.1). Tests: state-machine traces for threshold, Escape, capture
loss, invalid drop; target resolution tables for pointer positions over a
fixture layout (compass, document-area arms, site edges, tab gaps); cancel
leaves the serialized layout identical; one transaction and one layout pass
per drop; keyboard move reaches every target the pointer reaches (compare
the two target lists).

**Phase 4. Floating windows.** Tear-off, float command, redock by drag and
by double-click, drags across windows with guides in the window under the
pointer, `drag_proximate` on the target window, command context and
accelerators from floating windows (5.6), monitors and scale changes,
the float limit. Tests: headless multi-window traces; a drag from a floating
window into the main window closes the floating window after the panel
appears (show before destroy); Ctrl+S from a floating tool window reaches the
active document's command; floating windows restored onto a different
monitor arrangement land inside a work area.

**Phase 5. Auto-hide.** Side strips and flyouts (5.5), pinning and unpinning,
hover and click opening, closing rules, flyout resizing, reduced motion.
Tests: open and close event order for hover, click, focus loss and Escape;
focus restored after Escape; the flyout never changes the layout tree.

**Phase 6. Documents.** Dirty mark, closing with an asynchronous prompt
(the application answers save, discard or cancel; cancel keeps the
document and the layout unchanged), close-all and close-others with one
prompt sequence, MRU activation history after closing, document overflow
menu, and the preview and pinned tabs if admitted (option H3). Tests:
closing order and cancellation; activation after close follows MRU; no
layout change when a prompt is cancelled.

**Phase 7. Persistence integration.** Save and load through the layout
store with explicit calls also available; named layouts if admitted (VS's
saved window layouts; option I6); restore steps 6 to 8 of 5.8 on live
windows. Tests: a full application restore creates only visible panels; a
plugin registered after restore appears at its placeholder.

**Phase 8. Integration and evidence.** A Gallery docking page; the default
layout written in a form document once FD exists; host accessibility
verification of a docking fixture with stage A; the look from T4 and its
motion from T2; reference images. Building D's and V's shells on docking is
the consumer test.

### Dependencies

- Phase 0 is host work in `src/host/` on all three hosts and the headless
  host; it should be accepted before phase 4.
- The layout store is shared with ribbon phase 4.
- Stage A for the new role and action mappings.
- T4 and T2 for the look and motion; until then the controls draw from
  today's recipes, as every W control does.

### Gate (proposed)

Each phase's tests pass on every CI platform; the phase 0 measurements of
window positioning per host (including X11 under XWayland) are recorded under
`results/`; the owner accepts the Gallery docking page.

---

## 8. Open owner decisions, as interview questions

For the docking interview (`planning/interview/DOCKING_INTERVIEW_ROUND_001.md`).
Answer by number (for example `A1-1, D2-3`). Each recommendation is a working
hypothesis, not an implied answer. Nothing in sections 4 to 7 is final until
these are recorded.

### A. Scope and structure

**A1. Where are the docking controls built?**
1. In W, beside the ribbon, with DK as the planning ministage (as RB-1).
2. In a DK build stage of their own, after its phase 0 foundation.
3. After T4, so they are born themed.

Recommendation: 2 for phase 0 (host protocol work is not W's) and 1 for the
controls.

**A2. Layout model separate from controls?**
1. Yes: a renderer-free `DockLayout` model, a `DockWorkspace` component, and
   controls that realize it (4.1).
2. No: the controls are the layout (DockPanel Suite's form).

Recommendation: 1.

**A3. First scope.**
1. Tool panels and documents together.
2. Tool panels first (docked, floating, auto-hide), documents next.
3. Documents first (a tabbed document area), tool panels next.

Recommendation: 1; the model (phase 1) and the docked controls (phase 2)
cover both kinds from the start, since the distinction shapes the tree.

**A4. The n-ary weighted split solver.**
1. Written in DK for the dock site.
2. Taken from stage L's flex containers (GF008's shell layer); DK waits for L.
3. Written in DK and later replaced by L's.

Recommendation: 1, kept small, with L adopting or replacing it.

### B. Windowing foundation

**B1. Runtime windows.** Add open and destroy requests for owned windows
during a run, keeping a bound?
1. Yes, bound 64 windows in all (today's `maximum_windows`), floating sites
   at most 32.
2. Yes, no fixed bound.
3. No floating windows; docking stays inside each window.

Recommendation: 1.

**B2. Screen coordinates.** Add window frame get and set in screen logical
units, the pointer's screen position, and the window under a screen point,
each a capability with `unsupported` where a host cannot?
1. Yes.
2. Frame set only; no screen pointer (drags across windows not supported).

Recommendation: 1.

**B3. Images and theme across windows** (G-PAINT-2).
1. Make the image registry and theme application-scope, shared by all
   windows of a run.
2. Keep them per window; moving a control subtree re-registers its images
   in the destination window and refuses when it cannot.
3. Keep them per window; floating windows get copies of everything at
   creation.

Recommendation: 1 (one theme and one image set per application is what
users see anyway); 2 if per-window themes must stay possible.

**B4. Cross-window transaction.** A workspace transaction opens update
scopes on every affected window and orders window creation and destruction
(5.3).
1. Yes, as specified.
2. Each window updates independently; accept a one-frame gap.

Recommendation: 1.

**B5. Floating windows on macOS.** Today a tool window is an `NSPanel` child
window: it hides when the application deactivates and moves with its owner
(2.2).
1. Floating dock windows neither hide on deactivation nor move with the main
   window (as on Windows and X11); the existing `tool_window` keeps today's
   behaviour.
2. Keep both macOS behaviours for floating dock windows (platform
   convention).
3. Hide on deactivation (palette convention) but do not move with the main
   window.

Recommendation: 1; one portable rule, as the GIVEN rule asks.

**B5a. Floating window ownership** (VS makes it a user option, 1.5).
1. Owned by the main window: above it, minimized with it, no taskbar button
   (VS's default; DockPanel Suite; AvalonDock).
2. Independent windows with taskbar buttons.
3. 1 by default, 2 as a workspace option.

Recommendation: 1; 3 later if a consumer asks.

**B6. X11 window type.** Mark tool and floating windows
`_NET_WM_WINDOW_TYPE_UTILITY` with skip-taskbar?
1. Yes.
2. No.

Recommendation: 1.

**B7. How a floating window is moved.**
1. By the framework's own capture drag on its caption, setting the native
   frame per pointer move (guides can follow).
2. By the native move loop (the chrome drag region), with hosts reporting
   move progress and the screen pointer.
3. Native move loop with no redocking by dragging (redock by double-click or
   menu only).

Recommendation: 1; measure smoothness in phase 0, fall back to 2 on a host
that cannot keep up.

### C. Layout model and restrictions

**C1. Split nodes.**
1. N-ary with weights (AvalonDock, Qt).
2. Binary nested (DockPanel Suite).

Recommendation: 1.

**C2. The document area.**
1. One per main window, always present even when empty; documents may also
   float.
2. Documents may go anywhere tool panels may (no distinct area).
3. One per main window; documents never float.

Recommendation: 1 (VS, AvalonDock, Eclipse's shared editor area).

**C3. Mixed stacks.** May a tool panel and a document share one tab stack?
1. Never.
2. Only when the tool panel's descriptor allows "dock as document" (VS).

Recommendation: 2.

**C4. "Enabled control groups."** The study reads the owner's phrase as
groups of panels that may dock together.
1. Dock groups: a panel joins a stack or site only when its group admits it
   (4.4 layer 2).
2. Per-panel allowed placements only; no groups.
3. The phrase means something else (please describe).

Recommendation: 1, with a default group so applications that ignore groups
are unaffected.

**C5. Hidden panels remember their place.**
1. Yes: placeholders keep last docked, floating and auto-hide placements
   (VS, AvalonDock, Qt ADS).
2. No: a re-shown panel goes to its default placement.

Recommendation: 1.

**C6. When the window shrinks below the panels' minimums.**
1. Collapse tool panels to auto-hide by declared priority, then let the
   window minimum grow.
2. Raise the window minimum to the sum of minimums.
3. Clip.

Recommendation: 1 (DDV-007-02: "regions collapse by declared priority").

### D. Dragging and drop targets

**D1. Drag mode.**
1. Preview: nothing moves until the drop; a preview shows where the panel
   would go (DockPanel Suite, Telerik preview mode).
2. Immediate float: the panel tears off into a floating window at the start
   of the drag and follows the pointer (VS (Unverified since which version), Telerik immediate
   mode).
3. 1 inside the window, 2 once the pointer leaves the window.

Recommendation: 1 first (no foundation needed beyond one window); 3 once
phase 4 lands.

**D1b. Escape after an immediate tear-off:**
1. Puts the panel back where it was.
2. Leaves it floating where it is.

Recommendation: 1.

**D2. Targets.**
1. Guide buttons only (VS, DockPanel Suite, Telerik, Qt ADS's crosses).
2. Outline and cursor feedback with no buttons (Eclipse).
3. Both: guide buttons, plus edge zones of each pane as a larger fallback.

Recommendation: 1; 3 if the owner finds the buttons too small to hit.

**D2a. Dock straight to auto-hide from a guide** (DevExpress WPF puts an
auto-hide hint beside each edge guide, 1.5)?
1. No; auto-hide is chosen with the pin or the menu.
2. Yes.

Recommendation: 1.

**D3. Global (outer edge) targets.**
1. Always shown on the main window's edges during a drag (DockPanel Suite
   shows an edge indicator only while that edge is empty, 1.2).
2. Shown only while a modifier is held.
3. Not offered (nested only).

Recommendation: 1.

**D4. Dropping where nothing is accepted.**
1. Float if the panel may float, else snap back, with a rejected cue under
   the pointer.
2. Always snap back; floating needs the float guide or a menu command.

Recommendation: 1.

**D5. Drag threshold.** 
1. A portable 4 logical units, overridable by a host metric where one exists.
2. The host's own metric only.

Recommendation: 1.

**D6. Suppress docking while dragging** (to place a float exactly over a
docked area).
1. Ctrl held (DockPanel Suite and VS, 1.2 and 1.5).
2. No modifier.

Recommendation: 1.

**D7. What a caption drag moves.**
1. The whole stack (all its tabs); a tab drag moves one panel.
2. Only the selected panel in both cases.

Recommendation: 1 (VS, DockPanel Suite).

### E. Auto-hide

**E1. Opening a side tab's flyout.**
1. On hover after a delay, and on click (DockPanel Suite, AvalonDock).
2. On click only (VS: "select the tab"; Qt ADS by default).

Recommendation: 1; offer 2 as a workspace setting (DevExpress's
`AutoHiddenPanelShowMode`, Qt ADS's `AutoHideShowOnMouseOver`).

**E2. A click outside an open flyout:**
1. Closes it and is passed through to what was clicked.
2. Closes it and is consumed (as RB-12 for the minimized ribbon).

Recommendation: 1; a flyout covers working content the user is returning to
(Qt ADS closes on an outside click by default; whether the click passes
through was not read).

**E3. The flyout is an in-window overlay** (like the ribbon's popups,
RB-23), not a native window.
1. Yes.
2. No, a native child window.

Recommendation: 1.

**E4. Auto-hide unit.**
1. The pin auto-hides the whole stack (VS).
2. Only the selected panel.

Recommendation: 1.

### F. Focus, activation and command context

**F1. Two activation tracks** (`active_panel` and `active_document`, 5.6).
1. Yes.
2. One active panel only.

Recommendation: 1.

**F2. Command context.** Add a command routing chain (focused route, active
panel, active document, main window) so that document commands work from a
floating tool window?
1. Yes, in the commands seam, used by menus, ribbon, shortcuts and docking.
2. Accelerators only fall back to the owner window; no routing to a target.
3. Left to applications.

Recommendation: 1.

**F3. F6 across windows.**
1. F6 cycles regions of the key window only; Alt+F7 or Ctrl+Tab reach other
   windows' panels.
2. F6 cycles regions across all windows of the workspace.

Recommendation: 1 (F6 stays a window-level list as RB-17 decided).

**F4. Main window look while a floating tool window is key.**
1. `secondary_participating` (DDV-007-01).
2. The host's ordinary inactive look.

Recommendation: 1.

**F5. Clicking a floating tool window:**
1. Activates it (it becomes key).
2. Leaves the main window key unless an editable control is clicked
   (palette behaviour).

Recommendation: 1.

### G. Keyboard

**G1. Ctrl+Tab.** `TabControl` already uses Ctrl+Tab in key preview.
1. The workspace navigator owns Ctrl+Tab; a `TabControl` inside a panel uses
   Ctrl+Page Down and Ctrl+Page Up instead.
2. A `TabControl` with focus inside it keeps Ctrl+Tab; the navigator gets
   it otherwise.
3. The navigator uses another key.

Recommendation: 2 (most local wins, as the key preview order already gives).

**G2. Adopt the VS keys** of 5.7 (Ctrl+F6, Alt+F7, Alt+Hyphen, Shift+Escape,
Escape to the document) as defaults?
1. Yes, all.
2. Yes, but no Alt+Hyphen (use the context-menu key and Shift+F10 only).
3. Choose individually.

Recommendation: 1.

**G3. Keyboard move mode** (5.7.1), with arrows stepping through the same
targets as the pointer.
1. Yes.
2. Menu commands only (Float, Dock left, …), no move mode.

Recommendation: 1; it is the only way a keyboard user gets nested and
global splits.

**G4. Navigator shape.**
1. Two lists (tool panels, documents) as VS.
2. One MRU list.
3. Two lists plus a live preview of the selected panel.

Recommendation: 1.

**G5. Keyboard defaults per host.** On macOS, Cmd+W closes a document; other
keys keep Ctrl. Reported as host keymap defaults, not core policy.
1. Yes.
2. Identical keys on every host.

Recommendation: 1.

**G6. Rebinding.** May applications rebind docking keys?
1. Yes, they are commands like any other.
2. No.

Recommendation: 1.

**G7. A "maximize the document area" toggle** (hide all tool panels).
1. Yes, as a command with a default key (proposed Ctrl+Shift+F12, as
   JetBrains' "hide all tool windows"; VS's Shift+Alt+Enter is full screen,
   a different thing).
2. No.

Recommendation: 1.

### H. Documents

**H1. Closing.** The framework raises a cancellable, possibly asynchronous
`closing` event per document; close-all asks in sequence and stops at the
first cancel.
1. Yes.
2. One combined prompt listing every dirty document (VS's save-changes
   dialog).
3. Applications handle prompts entirely.

Recommendation: 1 with 2 as an option the application can choose.

**H2. Navigation history.**
1. The framework keeps MRU activation history (for the navigator and for
   activation after close); back and forward inside documents is the
   application's.
2. The framework also keeps a back and forward history of document
   activations.

Recommendation: 1.

**H3. Preview and pinned tabs** (VS's preview tab in a single-click open;
pinned tabs at the start).
1. Neither now.
2. Pinned tabs only.
3. Both.

Recommendation: 1.

**H4. Document tab overflow.**
1. Tabs that do not fit go to an overflow menu listing every document
   (VS).
2. The tab strip scrolls.
3. Tabs shrink, then overflow.

Recommendation: 3 then 1.

### I. Persistence and restore

**I1. Syntax.** The layout uses the runtime layout store's syntax (RES §7,
not yet chosen).
1. Yes; the store's syntax is decided once for configuration, ribbon and
   docking.
2. A docking-only format now (for example XML as DockPanel Suite).

Recommendation: 1.

**I2. Who saves.**
1. The workspace saves automatically through the layout store, with
   explicit save and load also available (as RB-12).
2. The application calls save and load.

Recommendation: 1.

**I3. Unknown panel IDs** (removed plugins).
1. Kept as inactive placeholders and restored if the panel returns.
2. Dropped.

Recommendation: 1.

**I4. Versions.**
1. Framework format version with built-in migrations, plus an application
   layout version with an upgrade hook (5.8 steps 2 and 3).
2. Framework version only; a changed application rejects old layouts.

Recommendation: 1.

**I5. Missing monitors** (5.8 step 4: same ID, else most overlap, else
primary; clamp so the caption is reachable).
1. As proposed.
2. Always reset floating windows to the main window's monitor.

Recommendation: 1.

**I6. Named layouts** (save and apply several, VS's Window Layouts,
JetBrains' layouts, Eclipse perspectives).
1. Yes, in the workspace API (applications offer them to users if they
   want).
2. No; one saved layout.

Recommendation: 1, in phase 7.

**I7. Lazy creation.**
1. Factories by panel ID; only visible selected panels are created on
   restore; others on first show.
2. Every registered panel is created at startup and docked by ID
   (DockPanel Suite's callback creates them all during load).

Recommendation: 1.

### J. Vocabulary (each a yes or no; SPEC §12.7)

1. A pin mark (`PartMark::pin`, drawn unpinned or pinned by the `checked`
   state) for the auto-hide button. Recommend yes.
2. A "tab into" mark for the centre guide, and whether outer-edge guides use
   the existing arrow marks or a separate mark. Recommend a centre mark;
   arrows for edges.
3. A dirty-document mark, or a custom item state `modified` that the theme
   shows. Recommend the custom item state (no new mark).
4. A text-orientation parameter in `PartPaint` for vertical side-tab labels,
   or icon-only side tabs. Recommend the parameter (VS, JetBrains and
   DockPanel Suite all show vertical text; section 1).
5. Semantic role `pane` for tool panels and F6 regions. Recommend yes, with
   stage A.
6. Semantic action `close`. Recommend yes.
7. Marks for float, dock and maximize on floating chrome (if T6 chooses
   GUI.Forms-drawn chrome). Recommend deciding with T6.
8. No new part roles: guides use `face`, `glyph` and `drop`; previews use
   `drop` with `drop_accepted` and `drop_rejected`. Recommend confirming
   this.

### K. Fit

**K1. File Manager adopts docking?**
1. No; its panes stay a fixed split composition (DNA-F01, DNA-P01), sharing
   parts and looks with docking.
2. Yes, for user-movable tree and selection panes.

Recommendation: 1.

**K2. `ToolStripContainer` strip dragging through the docking drag session
and targets later?**
1. Yes, after phase 3.
2. No, separate code.

Recommendation: 1.

### Questions for the owner's taste

These have no technical answer; the study can only show the options.

**T1. Which canonical docking look to start from where they differ.** VS
2005's diamond guides and tool-window tabs (V176 and V092 are listed in the
cue inventory; V092 marked "reject/reference", no verdict recorded),
VS 2010 to 2012's guides with blue translucent previews, DockPanel Suite's
VS2005 and VS2012 themes, Qt ADS's crosses, JetBrains' stripes with no
guides, GIMP's highlight bars. The ribbon round chose "Office 2010 and
earlier" (RB-10); the study's lean is the VS 2005 to 2010 period for the
same reason.

**T2. Guides.** A five-button diamond on a backplate (VS 2005), separate
square buttons (VS 2010), or crosses of arrows (Qt ADS)? How quiet: the
owner rejected theatrical drag overlays for file drag and drop (DDV-007-06,
DNA-O10).

**T3. Preview.** A translucent tinted area (VS, AvalonDock), a hatched or
dotted outline (DockPanel Suite's outline, Classic Windows drag rectangle),
or the panel's live content (Qt ADS dynamic preview)?

**T4. Tool tabs and document tabs.** Should document tabs look like the
ribbon's tab stack (Plan Paint's tabs; "A tab is not a button") or a quieter
Studio 2003 strip, and tool tabs at the bottom of a stack plainer still?
Caption: DNA-P04's Studio 2003 strip with a tiny identity key; how does the
active panel's caption differ from an inactive one (VS uses a coloured
caption for the active tool window)?

**T5. Auto-hide.** Side tabs with vertical text (VS) or icons; the flyout
casting the E4 overlay shadow or E3; the slide motion or a plain appear.

**T6. Floating window chrome.** The host's title bar (native, smallest
utility style) or a GUI.Forms-drawn caption that is the stack's caption (VS
draws its own floating chrome); window keyline per DNA-F04 either way.

**T7. Seams between docked panels.** File Manager's graphite seam
(DNA-P02, 3 px or thinner, grip DNA-P03) everywhere, or Studio 2003's flat
gap?

**T8. Drag proximity.** How a window under a drag shows `drag_proximate`
(DDV-007-01: "Drag proximity may temporarily restore destination emphasis
without transferring keyboard focus").

### Questions a mockup answers better than words

- **T1, T2, T3:** one static mockup per guide family over the same
  three-panel layout, with a nested and a global preview, in the house look.
- **T4:** a tool stack and a document stack side by side, active and
  inactive, in the house look.
- **T5:** the auto-hide strip and an open flyout (a short animated mockup if
  the slide matters).
- **T6:** a floating window with one stack and with two stacks, native and
  drawn chrome.
- **D1:** a short animated mockup of preview mode against immediate float
  is the clearest way to choose.
- **G3:** a sequence of frames of keyboard move mode stepping through
  targets.

---

### Interview and verdict citations used

GF006, GF008, GF009–GF011, GF012–GF013, GF017 (LEDGER); A1A–A6A, §3.4,
§3.5 (A11Y-001); RES §2.4, §6, §7, RC005, RC006; RB-1, RB-10, RB-12, RB-17,
RB-20, RB-22, RB-23; DDV-007-01, DDV-007-02, DDV-007-04, DDV-007-06;
DNA-F01, DNA-F03, DNA-F04, DNA-P01 to DNA-P04, DNA-O10, DNA-A06, DNA-C12;
V092 and V176 (listed in `VISUAL_CUE_INVENTORY.md`, no recorded verdict),
V016 R, V125 REJECT; M051, M052; C141 R; SPEC §3.2, §3.4, §4.7, §7.2–§7.4,
§8.1–§8.4, §9.2, §9.5, §12.3–§12.7; ADR-001, ADR-013 (retired); matrix rows
43 and 46.
