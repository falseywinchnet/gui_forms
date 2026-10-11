# Controls by function

Generated from `manifests/controls.json` by `python3 tools/control_registry.py render`. Do not edit.

| Function | Control | WinForms counterpart | Description |
|---|---|---|---|
| Commands | `Button` | Button | Push button with accept/cancel dialog results, default-button state, and four visual policies. |
|  | `ButtonBase` | ButtonBase | Shared retained command state machine for pointer, keyboard, mnemonic, semantic, and dialog activation that Button, CheckBox, RadioButton, and LinkLabel specialize. |
|  | `CommandBar` | ToolStrip | Horizontal row of retained item buttons built from a replaceable CommandItem list that raises one group notification per replacement. |
|  | `DropDownButton` | ToolStripSplitButton | Command button with an explicit disclosure actuator that either routes the whole button to an owned popup menu or keeps a primary click region beside a trailing split region. |
|  | `LinkLabel` | LinkLabel | Command presented as underlined link text with retained visited state and link semantics. |
| Text editing | `TextBox` | TextBox | Single-line Unicode editor with directional selection, clipboard and history commands, password projection, bounded input length, and ordered text/selection events. |
| Information display | `Label` | Label | Noninteractive text primitive with inherited typography roles, alignment, word wrapping, a bounded line count, and mnemonics. |
|  | `PictureBox` | PictureBox | Noninteractive image presenter over Window-owned generational image resources with the five PictureBoxSizeMode policies and bounded opacity. |
|  | `ProgressBar` | ProgressBar | Noninteractive range visualization with continuous, segmented, and marquee modes whose motion follows the Window frame scheduler and reduced-motion policy. |
|  | `ReviewCard` | GUI.Forms-only | Typed review projection over Card that owns title, body, and verdict labels and projects pending/rejected state through ordinary Card laws. |
| Selection from a list | `BreadcrumbTrail` | GUI.Forms-only | Location instrument whose logical path segments are virtual semantic children and whose exact-path TextBox occupies the row only while editing. |
|  | `CheckedListBox` | CheckedListBox | ListBox with an independently retained three-state check model, cancellable pre-change events, and optional check-on-click. |
|  | `ChoiceGroup` | GUI.Forms-only | Row of retained item buttons that selects exactly one ChoiceItem and exposes the selection as a bindable Value. |
|  | `ComboBox` | ComboBox | Noneditable choice field with placeholder projection, keyboard cycling, and a source-private transient ListBox popup. |
|  | `CorrespondenceView` | GUI.Forms-only | Variable-height virtual list with independent keyboard focus, delayed pointer inspection, and one persistent pinned item. |
|  | `ListBox` | ListBox | Virtual-row selector with single, multi-simple, and multi-extended selection policies, stable item identities, and scroll-window projection. |
|  | `ObjectView` | ListView | Stable-ID virtual object collection with icon and details projections that owns no per-item controls. |
|  | `TreeView` | TreeView | Flat-model retained tree that paints and publishes only visible rows while item identity and expansion state stay stable for unrealized descendants. |
| Value setting | `CheckBox` | CheckBox | Two- or three-state selection control with optional automatic cycling, three indicator families, and ordered state events. |
|  | `ColorValueEditor` | GUI.Forms-only | Compound editor that delegates editing to a real TextBox and adds a checker-backed swatch with canonical #RRGGBBAA conversion. |
|  | `FlagsValueEditor` | GUI.Forms-only | Compact editor for independent-bit enumeration values that opens a real CheckedListBox popup under Window popup and focus-scope leases. |
|  | `InstrumentRack` | GUI.Forms-only | Bounded retained rack of compact editing modules that reconciles stable module and field identities across model replacement. |
|  | `NumericUpDown` | NumericUpDown | Composite numeric editor over an owned TextBox and source-private spin buttons with ordered ranges, bounded step and precision, and decimal or hexadecimal formatting. |
|  | `PropertyGrid` | PropertyGrid | Metadata-driven inspector and editor over one or more selected Controls that projects descriptors into a PropertyList. |
|  | `PropertyList` | GUI.Forms-only | Settings surface over caller-authored stable groups and rows that owns stock or replacement child editors, reset buttons, and one vertical scroll plane. |
|  | `RadioButton` | RadioButton | Retained checked state with container-scoped named exclusion groups, optional automatic checking, and radio semantics. |
|  | `RangeControl` | GUI.Forms-only | Nonvisual retained authority for ordered bounds, a constrained value, small/large changes, and orientation that TrackBar, ProgressBar, and ScrollBar specialize. |
|  | `TrackBar` | TrackBar | Slider over RangeControl with bounded tick frequency, absolute pointer positioning, keyboard movement, and slider semantics. |
| Date setting | `DateTimePicker` | DateTimePicker | Civil date/time field with validated bounds, four formatting policies, optional check state, and calendar or spin presentation. |
| Grouping | `Card` | GUI.Forms-only | Header/body/footer composition that owns each installed section as an ordinary visual child and returns the detached predecessor on replacement. |
|  | `ContainerControl` | ContainerControl | ScrollableControl with descendant-scoped active-control authority and inherited validation policy. |
|  | `ExpandableSections` | GUI.Forms-only | Stack of captioned sections whose content controls expand and collapse individually or in single-open mode. |
|  | `GroupBox` | GroupBox | Titled Panel with mnemonic focus routing, caption-aware border painting, and group semantics. |
|  | `MaterialPanel` | GUI.Forms-only | Content-agnostic container painted with a SurfaceMaterial. |
|  | `Panel` | Panel | Scroll-capable surface primitive with theme-role rendering, explicit background and border overrides, and optional named-group semantics. |
|  | `ScaledGroupBox` | GUI.Forms-only | GroupBox whose children occupy stable design-space slots arranged proportionally to the current content bounds. |
|  | `TabControl` | TabControl | Owner of TabPage lifetime and selection with per-page focus restoration, four header alignments, three appearances, and virtual tab semantics. |
|  | `TabPage` | TabPage | Panel surface owned by a TabControl with bounded caption text and page semantics. |
|  | `UserControl` | UserControl | Authored-container base that distinguishes current attachment from first committed load and counts attachment epochs. |
| Layout | `CommandOverflowPanel` | GUI.Forms-only | Horizontal command-group container that owns only layout collapse and the overflow actuator's availability while command authority stays with the consumer. |
|  | `FlowLayoutPanel` | FlowLayoutPanel | Directional packing container with four flow directions, optional wrapping, bounded spacing, per-child margins, and stable flow breaks. |
|  | `MasterDetailView` | GUI.Forms-only | Content-agnostic master/detail shell with responsive presentation, a draggable and keyboard-operable splitter, and focus transfer on collapse. |
|  | `ResponsiveTrackPanel` | GUI.Forms-only | Track-based container that places each child in one authored track and collapses tracks as an effective-layout state without changing author visibility. |
|  | `ScaledPanel` | GUI.Forms-only | Panel that maps caller-authored design-space child rectangles into the current content bounds while retaining slot identity independently of child order. |
|  | `SplitContainer` | SplitContainer | Two-pane allocation container with bounded splitter distance, fixed-pane resize preservation, pointer and keyboard seam input, and collapse. |
|  | `SplitterPanel` | SplitterPanel | Allocated pane surface owned by SplitContainer that paints its own background from committed bounds. |
|  | `TableLayoutPanel` | TableLayoutPanel | Grid container with bounded rows and columns, absolute/percent/auto tracks, explicit or automatic placement, and row/column spans. |
| Scrolling | `HScrollBar` | HScrollBar | Explicit horizontal ScrollBar leaf type. |
|  | `ScrollableControl` | ScrollableControl | Retained scrolling state machine with overflow discovery, viewport rectangles, two axis policies, wheel/track/thumb interaction, and virtual scrollbar semantics. |
|  | `ScrollBar` | ScrollBar | RangeControl with bounded button and thumb geometry, proportional thumb allocation, drag and track interaction, and retained auto-repeat. |
|  | `VScrollBar` | VScrollBar | Explicit vertical ScrollBar leaf type. |
| Menus and commands | `Command` | GUI.Forms-only | Shared retained authority for menu, shortcut, context, semantic, and test surfaces with validated UTF-8 state and enabled-only invocation. |
|  | `ContextMenu` | ContextMenuStrip | Popup-menu controller whose rows are temporary popup presentations over stable MenuItemSpec identities and bound Commands. |
|  | `MenuStrip` | MenuStrip | Top-level application menu bar whose active menu is presented by ContextMenu as a separate popup root bound to shared Command instances. |
| Dialogs and popups | `AnchoredPopupLayer` | GUI.Forms-only | Full-client overlay that owns one retained popup content subtree, re-resolves anchored bounds on resize, and reports click-away and Escape requests. |
|  | `ToolTip` | ToolTip | Nonvisual provider that maps Controls to tooltip text and presents a retained, input-transparent overlay owned by each target. |
| Drawing | `DrawingSurface` | GUI.Forms-only | Owner-render surface configured through the renderer-neutral Painter vocabulary without a Control subclass. |
|  | `LiveSurface` | GUI.Forms-only | Latest-frame surface where a producer publishes complete frame candidates and painters acquire only the newest immutable one. |
|  | `RasterCanvas` | GUI.Forms-only | Viewport over an owned GUI.Drawing bitmap that handles presentation, coordinate transforms, resource synchronization, and local invalidation. |
| Diagnostics | `EasingPreview` | GUI.Forms-only | Animation and easing conformance surface over an owned AnimationTimeline with configurable tracks and motion policy. |
|  | `MetricsView` | GUI.Forms-only | View over the structured Window metrics snapshot. |
|  | `TypographySpecimen` | GUI.Forms-only | Typography review control that draws one committed text run with its logical line box, resolved baseline, and provider status. |
|  | `VisualInspectorOverlay` | GUI.Forms-only | Transparent companion overlay that draws a target's arranged, visual, and effective clipped bounds without altering hit testing. |
|  | `VisualInspectorView` | GUI.Forms-only | Presentation of the Window visual-inspection snapshot for demoboards and local diagnostics. |
| Components | `Binding` | Binding | Connects one Control property to one BindingSource field with validated conversion, update timing, and error routing. |
|  | `BindingContext` | BindingContext | Window-scoped weak membership and disposal subscriptions for BindingSources. |
|  | `BindingSource` | BindingSource | Owns stable rows, currency, edit snapshots, list and error events, and the active Bindings for one Window. |
|  | `ErrorProvider` | ErrorProvider | Validation adornment provider that projects error text onto the target semantic node and gives each visible target an independent overlay glyph. |
|  | `HelpProvider` | HelpProvider | Policy-free help provider that retains authored metadata and routes F1 and pointer help requests to the consumer. |
|  | `ImageList` | ImageList | Ordered, keyed collection of PNG-sourced Window image resources with density and state variants and atomic mutation. |
|  | `SemanticFeedback` | GUI.Forms-only | Turns one-shot application transitions into bounded, traceable host cues. |
|  | `Timer` | Timer | Timer whose callbacks are serialized with input, layout, and paint on the owning Window's UI thread and whose late ticks are coalesced. |
