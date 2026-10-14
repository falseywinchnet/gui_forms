# WinForms and drawing API oversight catalogue

Status: **OBSERVED upstream inventory; conservative implementation ledger**.

Forms rows come from the official shipped API lists of `dotnet/winforms` (`PublicAPI.Shipped.txt` of `System.Windows.Forms` and `System.Windows.Forms.Primitives`), one row per public API line, with the versions that list it. `[Obsolete]` markers come from the `Microsoft.WindowsDesktop.App.Ref` reference assemblies of the same versions (`tools/winforms_catalogue/read_obsolete.py`). Sources:

| .NET | dotnet/winforms branch | Commit | Reference pack |
|---|---|---|---|
| 8.0 | `release/8.0` | `edebee640e06f291a2a2e770cfb86d6bd42563f9` | `8.0.32` |
| 9.0 | `release/9.0` | `3e991f572c195fadf3b06e9e06c7c56d8ac4cd48` | `9.0.21` |
| 10.0 | `release/10.0` | `f3adc279ed152f9b88ec678218011168e90b0bdb` | `10.0.13` |

Drawing rows are carried unchanged from the earlier catalogue (the pinned .NET 10.0.3 `System.Drawing` reference XML). The generated TSV is the member-level authority; this summary is navigation.

**Scope rule (owner, 2026-10-13):** a Forms row is in scope when some version of .NET 8.0 or later lists it without `[Obsolete]`, and it is not otherwise excluded below. Rows obsolete in every version that lists them are excluded with their diagnostic.

A `partial_native` row means only that a corresponding retained GUI.Forms/GUI.Drawing type exists. `measured_partial` identifies an exact behavioral center with named evidence while retaining its stated gaps. Neither status claims that every overload, exception, event order, accessibility projection, or raster result is complete. `missing` is the default for admitted calls without direct evidence.

## Totals

| Surface | Rows | Admitted | Excluded |
|---|---:|---:|---:|
| `System.Drawing` | 3522 | 3050 | 472 |
| `System.Windows.Forms` | 14762 | 12934 | 1828 |

The previous catalogue, generated from the LibreWinForms fork's API list, had 14575 Forms rows, 9850 of them `missing`. Against the official surface: 14762 rows, 9469 `missing`.

## Forms rows by first listing version

| First listed in | Rows |
|---|---:|
| .NET 8.0 | 13701 |
| .NET 9.0 | 195 |
| .NET 10.0 | 866 |

8 admitted rows are marked `[Obsolete]` in a later version but listed without it in an earlier one; the `obsolete` column names them.

## Admitted Forms types still `missing`

598 top-level `System.Windows.Forms` types are admitted and `missing` (nested types are counted with their owner). Controls, components, event arguments, delegates and enumerations are listed together; the number is the type's own `missing` member rows. `missing` means no row is mapped to GUI.Forms evidence yet: some of these have GUI.Forms equivalents that are not mapped (the common dialogs are host services, `Keys` is GUI.Forms' key code, `PropertyGrid` exists without a managed facade), and the rest have no counterpart.

| Type | Missing members |
|---|---:|
| `IPropertyValueUIService` | 5 |
| `PaintValueEventArgs` | 5 |
| `PropertyValueUIHandler` | 1 |
| `PropertyValueUIItem` | 5 |
| `PropertyValueUIItemInvokeHandler` | 1 |
| `UITypeEditor` | 10 |
| `UITypeEditorEditStyle` | 3 |
| `Converter` | 5 |
| `AccessibleEvents` | 42 |
| `AccessibleNavigation` | 8 |
| `AccessibleObject` | 26 |
| `AccessibleRole` | 66 |
| `AccessibleSelection` | 6 |
| `AccessibleStates` | 34 |
| `AmbientProperties` | 9 |
| `AnchorStyles` | 5 |
| `Appearance` | 2 |
| `ArrangeDirection` | 4 |
| `ArrangeStartingPosition` | 5 |
| `ArrowDirection` | 4 |
| `AutoCompleteMode` | 4 |
| `AutoCompleteSource` | 9 |
| `AutoCompleteStringCollection` | 20 |
| `AutoScaleMode` | 4 |
| `BaseCollection` | 8 |
| `BatteryChargeStatus` | 6 |
| `BindableComponent` | 6 |
| `BindingCompleteEventHandler` | 1 |
| `BindingManagerDataErrorEventArgs` | 2 |
| `BindingManagerDataErrorEventHandler` | 1 |
| `BindingMemberInfo` | 10 |
| `BindingNavigator` | 32 |
| `BindingsCollection` | 4 |
| `BootMode` | 3 |
| `Border3DSide` | 6 |
| `Border3DStyle` | 10 |
| `BorderStyle` | 3 |
| `ButtonBorderStyle` | 6 |
| `ButtonRenderer` | 11 |
| `ButtonState` | 6 |
| `CacheVirtualItemsEventArgs` | 3 |
| `CacheVirtualItemsEventHandler` | 1 |
| `CaptionButton` | 5 |
| `CharacterCasing` | 3 |
| `CheckBoxRenderer` | 10 |
| `CheckState` | 3 |
| `Clipboard` | 28 |
| `CloseReason` | 7 |
| `ColorDepth` | 5 |
| `ColorDialog` | 20 |
| `ColumnClickEventArgs` | 2 |
| `ColumnClickEventHandler` | 1 |
| `ColumnHeader` | 26 |
| `ColumnHeaderAutoResizeStyle` | 3 |
| `ColumnHeaderConverter` | 3 |
| `ColumnHeaderStyle` | 3 |
| `ColumnReorderedEventArgs` | 4 |
| `ColumnReorderedEventHandler` | 1 |
| `ColumnStyle` | 5 |
| `ColumnWidthChangedEventArgs` | 2 |
| `ColumnWidthChangedEventHandler` | 1 |
| `ColumnWidthChangingEventArgs` | 5 |
| `ColumnWidthChangingEventHandler` | 1 |
| `ComboBoxRenderer` | 7 |
| `ComboBoxStyle` | 3 |
| `CommonDialog` | 11 |
| `ContentsResizedEventArgs` | 2 |
| `ContentsResizedEventHandler` | 1 |
| `ControlEventArgs` | 2 |
| `ControlEventHandler` | 1 |
| `ControlPaint` | 52 |
| `ControlStyles` | 18 |
| `ConvertEventArgs` | 4 |
| `ConvertEventHandler` | 1 |
| `Cursor` | 26 |
| `CursorConverter` | 7 |
| `Cursors` | 28 |
| `DataFormats` | 23 |
| `DataGridView` | 512 |
| `DataGridViewAdvancedBorderStyle` | 14 |
| `DataGridViewAdvancedCellBorderStyle` | 8 |
| `DataGridViewAutoSizeColumnMode` | 8 |
| `DataGridViewAutoSizeColumnModeEventArgs` | 3 |
| `DataGridViewAutoSizeColumnModeEventHandler` | 1 |
| `DataGridViewAutoSizeColumnsMode` | 7 |
| `DataGridViewAutoSizeColumnsModeEventArgs` | 2 |
| `DataGridViewAutoSizeColumnsModeEventHandler` | 1 |
| `DataGridViewAutoSizeModeEventArgs` | 2 |
| `DataGridViewAutoSizeModeEventHandler` | 1 |
| `DataGridViewAutoSizeRowMode` | 3 |
| `DataGridViewAutoSizeRowsMode` | 7 |
| `DataGridViewBand` | 25 |
| `DataGridViewBindingCompleteEventArgs` | 2 |
| `DataGridViewBindingCompleteEventHandler` | 1 |
| `DataGridViewButtonCell` | 29 |
| `DataGridViewButtonColumn` | 13 |
| `DataGridViewCell` | 110 |
| `DataGridViewCellBorderStyle` | 11 |
| `DataGridViewCellCancelEventArgs` | 3 |
| `DataGridViewCellCancelEventHandler` | 1 |
| `DataGridViewCellCollection` | 17 |
| `DataGridViewCellContextMenuStripNeededEventArgs` | 3 |
| `DataGridViewCellContextMenuStripNeededEventHandler` | 1 |
| `DataGridViewCellErrorTextNeededEventArgs` | 2 |
| `DataGridViewCellErrorTextNeededEventHandler` | 1 |
| `DataGridViewCellEventArgs` | 3 |
| `DataGridViewCellEventHandler` | 1 |
| `DataGridViewCellFormattingEventArgs` | 7 |
| `DataGridViewCellFormattingEventHandler` | 1 |
| `DataGridViewCellMouseEventArgs` | 3 |
| `DataGridViewCellMouseEventHandler` | 1 |
| `DataGridViewCellPaintingEventArgs` | 16 |
| `DataGridViewCellPaintingEventHandler` | 1 |
| `DataGridViewCellParsingEventArgs` | 7 |
| `DataGridViewCellParsingEventHandler` | 1 |
| `DataGridViewCellStateChangedEventArgs` | 3 |
| `DataGridViewCellStateChangedEventHandler` | 1 |
| `DataGridViewCellStyle` | 36 |
| `DataGridViewCellStyleContentChangedEventArgs` | 2 |
| `DataGridViewCellStyleContentChangedEventHandler` | 1 |
| `DataGridViewCellStyleConverter` | 3 |
| `DataGridViewCellStyleScopes` | 9 |
| `DataGridViewCellToolTipTextNeededEventArgs` | 2 |
| `DataGridViewCellToolTipTextNeededEventHandler` | 1 |
| `DataGridViewCellValidatingEventArgs` | 3 |
| `DataGridViewCellValidatingEventHandler` | 1 |
| `DataGridViewCellValueEventArgs` | 5 |
| `DataGridViewCellValueEventHandler` | 1 |
| `DataGridViewCheckBoxCell` | 48 |
| `DataGridViewCheckBoxColumn` | 17 |
| `DataGridViewClipboardCopyMode` | 4 |
| `DataGridViewColumn` | 53 |
| `DataGridViewColumnCollection` | 26 |
| `DataGridViewColumnDesignTimeVisibleAttribute` | 9 |
| `DataGridViewColumnDividerDoubleClickEventArgs` | 2 |
| `DataGridViewColumnDividerDoubleClickEventHandler` | 1 |
| `DataGridViewColumnEventArgs` | 2 |
| `DataGridViewColumnEventHandler` | 1 |
| `DataGridViewColumnHeaderCell` | 14 |
| `DataGridViewColumnHeadersHeightSizeMode` | 3 |
| `DataGridViewColumnSortMode` | 3 |
| `DataGridViewColumnStateChangedEventArgs` | 3 |
| `DataGridViewColumnStateChangedEventHandler` | 1 |
| `DataGridViewComboBoxCell` | 44 |
| `DataGridViewComboBoxColumn` | 26 |
| `DataGridViewComboBoxDisplayStyle` | 3 |
| `DataGridViewComboBoxEditingControl` | 18 |
| `DataGridViewContentAlignment` | 10 |
| `DataGridViewDataErrorContexts` | 11 |
| `DataGridViewDataErrorEventArgs` | 5 |
| `DataGridViewDataErrorEventHandler` | 1 |
| `DataGridViewEditMode` | 5 |
| `DataGridViewEditingControlShowingEventArgs` | 4 |
| `DataGridViewEditingControlShowingEventHandler` | 1 |
| `DataGridViewElement` | 10 |
| `DataGridViewElementStates` | 8 |
| `DataGridViewHeaderBorderStyle` | 5 |
| `DataGridViewHeaderCell` | 29 |
| `DataGridViewHitTestType` | 7 |
| `DataGridViewImageCell` | 22 |
| `DataGridViewImageCellLayout` | 4 |
| `DataGridViewImageColumn` | 18 |
| `DataGridViewLinkCell` | 36 |
| `DataGridViewLinkColumn` | 19 |
| `DataGridViewPaintParts` | 9 |
| `DataGridViewRow` | 48 |
| `DataGridViewRowCancelEventArgs` | 3 |
| `DataGridViewRowCancelEventHandler` | 1 |
| `DataGridViewRowCollection` | 37 |
| `DataGridViewRowContextMenuStripNeededEventArgs` | 4 |
| `DataGridViewRowContextMenuStripNeededEventHandler` | 1 |
| `DataGridViewRowDividerDoubleClickEventArgs` | 2 |
| `DataGridViewRowDividerDoubleClickEventHandler` | 1 |
| `DataGridViewRowErrorTextNeededEventArgs` | 3 |
| `DataGridViewRowErrorTextNeededEventHandler` | 1 |
| `DataGridViewRowEventArgs` | 2 |
| `DataGridViewRowEventHandler` | 1 |
| `DataGridViewRowHeaderCell` | 14 |
| `DataGridViewRowHeadersWidthSizeMode` | 5 |
| `DataGridViewRowHeightInfoNeededEventArgs` | 5 |
| `DataGridViewRowHeightInfoNeededEventHandler` | 1 |
| `DataGridViewRowHeightInfoPushedEventArgs` | 3 |
| `DataGridViewRowHeightInfoPushedEventHandler` | 1 |
| `DataGridViewRowPostPaintEventArgs` | 17 |
| `DataGridViewRowPostPaintEventHandler` | 1 |
| `DataGridViewRowPrePaintEventArgs` | 19 |
| `DataGridViewRowPrePaintEventHandler` | 1 |
| `DataGridViewRowStateChangedEventArgs` | 3 |
| `DataGridViewRowStateChangedEventHandler` | 1 |
| `DataGridViewRowsAddedEventArgs` | 3 |
| `DataGridViewRowsAddedEventHandler` | 1 |
| `DataGridViewRowsRemovedEventArgs` | 3 |
| `DataGridViewRowsRemovedEventHandler` | 1 |
| `DataGridViewSelectedCellCollection` | 5 |
| `DataGridViewSelectedColumnCollection` | 5 |
| `DataGridViewSelectedRowCollection` | 5 |
| `DataGridViewSelectionMode` | 5 |
| `DataGridViewSortCompareEventArgs` | 8 |
| `DataGridViewSortCompareEventHandler` | 1 |
| `DataGridViewTextBoxCell` | 19 |
| `DataGridViewTextBoxColumn` | 8 |
| `DataGridViewTextBoxEditingControl` | 20 |
| `DataGridViewTopLeftHeaderCell` | 8 |
| `DataGridViewTriState` | 3 |
| `DataObject` | 38 |
| `DataObjectExtensions` | 4 |
| `DateBoldEventArgs` | 4 |
| `DateBoldEventHandler` | 1 |
| `DateRangeEventArgs` | 3 |
| `DateRangeEventHandler` | 1 |
| `DateTimePickerFormat` | 4 |
| `Day` | 8 |
| `DockStyle` | 6 |
| `DockingAttribute` | 7 |
| `DockingBehavior` | 3 |
| `DomainUpDown` | 21 |
| `DpiChangedEventArgs` | 4 |
| `DpiChangedEventHandler` | 1 |
| `DragAction` | 3 |
| `DragDropEffects` | 6 |
| `DragEventArgs` | 15 |
| `DragEventHandler` | 1 |
| `DrawItemEventArgs` | 13 |
| `DrawItemEventHandler` | 1 |
| `DrawItemState` | 12 |
| `DrawListViewColumnHeaderEventArgs` | 14 |
| `DrawListViewColumnHeaderEventHandler` | 1 |
| `DrawListViewItemEventArgs` | 12 |
| `DrawListViewItemEventHandler` | 1 |
| `DrawListViewSubItemEventArgs` | 15 |
| `DrawListViewSubItemEventHandler` | 1 |
| `DrawMode` | 3 |
| `DrawToolTipEventArgs` | 11 |
| `DrawToolTipEventHandler` | 1 |
| `DrawTreeNodeEventArgs` | 7 |
| `DrawTreeNodeEventHandler` | 1 |
| `DropImageType` | 8 |
| `FeatureSupport` | 7 |
| `FileDialog` | 45 |
| `FileDialogCustomPlace` | 7 |
| `FileDialogCustomPlacesCollection` | 3 |
| `FixedPanel` | 3 |
| `FlatButtonAppearance` | 10 |
| `FlatStyle` | 4 |
| `FlowDirection` | 4 |
| `FlowLayoutSettings` | 7 |
| `FolderBrowserDialog` | 30 |
| `FontDialog` | 39 |
| `FormBorderStyle` | 7 |
| `FormClosedEventArgs` | 2 |
| `FormClosedEventHandler` | 1 |
| `FormClosingEventArgs` | 2 |
| `FormClosingEventHandler` | 1 |
| `FormCollection` | 3 |
| `FormCornerPreference` | 4 |
| `FormStartPosition` | 5 |
| `FormWindowState` | 3 |
| `FrameStyle` | 2 |
| `GiveFeedbackEventArgs` | 11 |
| `GiveFeedbackEventHandler` | 1 |
| `GridItem` | 13 |
| `GridItemCollection` | 5 |
| `GridItemType` | 4 |
| `GroupBoxRenderer` | 9 |
| `HandledMouseEventArgs` | 4 |
| `Help` | 6 |
| `HighDpiMode` | 5 |
| `HorizontalAlignment` | 3 |
| `IAsyncDropTarget` | 1 |
| `IBindableComponent` | 3 |
| `ICommandExecutor` | 1 |
| `IComponentEditorPageSite` | 2 |
| `IContainerControl` | 3 |
| `ICurrencyManagerProvider` | 2 |
| `IDataGridColumnStyleEditingNotificationService` | 1 |
| `IDataGridViewEditingCell` | 6 |
| `IDataGridViewEditingControl` | 14 |
| `IDataObject` | 12 |
| `IDropTarget` | 4 |
| `IFeatureSupport` | 3 |
| `IFileReaderService` | 1 |
| `IMessageFilter` | 1 |
| `ITypedDataObject` | 4 |
| `IWindowTarget` | 2 |
| `ImageIndexConverter` | 7 |
| `ImageKeyConverter` | 8 |
| `ImageLayout` | 5 |
| `ImageListStreamer` | 2 |
| `ImeContext` | 6 |
| `ImeMode` | 14 |
| `ImeModeConversion` | 3 |
| `InputLanguage` | 10 |
| `InputLanguageChangedEventArgs` | 5 |
| `InputLanguageChangedEventHandler` | 1 |
| `InputLanguageChangingEventArgs` | 5 |
| `InputLanguageChangingEventHandler` | 1 |
| `InputLanguageCollection` | 4 |
| `InsertKeyMode` | 3 |
| `ItemActivation` | 3 |
| `ItemBoundsPortion` | 4 |
| `ItemChangedEventArgs` | 1 |
| `ItemChangedEventHandler` | 1 |
| `ItemCheckEventArgs` | 5 |
| `ItemCheckEventHandler` | 1 |
| `ItemCheckedEventArgs` | 2 |
| `ItemCheckedEventHandler` | 1 |
| `ItemDragEventArgs` | 4 |
| `ItemDragEventHandler` | 1 |
| `KeyEventArgs` | 12 |
| `KeyEventHandler` | 1 |
| `KeyPressEventArgs` | 5 |
| `KeyPressEventHandler` | 1 |
| `Keys` | 194 |
| `KeysConverter` | 9 |
| `LabelEditEventArgs` | 6 |
| `LabelEditEventHandler` | 1 |
| `LayoutSettings` | 2 |
| `LeftRightAlignment` | 2 |
| `LinkArea` | 13 |
| `LinkBehavior` | 4 |
| `LinkClickedEventArgs` | 5 |
| `LinkClickedEventHandler` | 1 |
| `LinkConverter` | 5 |
| `LinkLabelLinkClickedEventArgs` | 4 |
| `LinkLabelLinkClickedEventHandler` | 1 |
| `LinkState` | 4 |
| `ListBindingConverter` | 5 |
| `ListBindingHelper` | 8 |
| `ListControl` | 45 |
| `ListControlConvertEventArgs` | 2 |
| `ListControlConvertEventHandler` | 1 |
| `ListView` | 180 |
| `ListViewAlignment` | 4 |
| `ListViewGroup` | 29 |
| `ListViewGroupCollapsedState` | 3 |
| `ListViewGroupCollection` | 18 |
| `ListViewGroupEventArgs` | 2 |
| `ListViewHitTestInfo` | 4 |
| `ListViewHitTestLocations` | 8 |
| `ListViewInsertionMark` | 8 |
| `ListViewItem` | 72 |
| `ListViewItemConverter` | 3 |
| `ListViewItemMouseHoverEventArgs` | 2 |
| `ListViewItemMouseHoverEventHandler` | 1 |
| `ListViewItemSelectionChangedEventArgs` | 4 |
| `ListViewItemSelectionChangedEventHandler` | 1 |
| `ListViewItemStateImageIndexConverter` | 3 |
| `ListViewItemStates` | 9 |
| `ListViewVirtualItemsSelectionRangeChangedEventArgs` | 4 |
| `ListViewVirtualItemsSelectionRangeChangedEventHandler` | 1 |
| `MaskFormat` | 4 |
| `MaskInputRejectedEventArgs` | 3 |
| `MaskInputRejectedEventHandler` | 1 |
| `MaskedTextBox` | 101 |
| `MeasureItemEventArgs` | 8 |
| `MeasureItemEventHandler` | 1 |
| `MenuGlyph` | 5 |
| `MergeAction` | 5 |
| `MessageBox` | 21 |
| `MessageBoxButtons` | 7 |
| `MessageBoxDefaultButton` | 4 |
| `MessageBoxIcon` | 9 |
| `MessageBoxOptions` | 4 |
| `MethodInvoker` | 1 |
| `MonthCalendar` | 111 |
| `MouseButtons` | 6 |
| `MouseEventArgs` | 7 |
| `MouseEventHandler` | 1 |
| `NavigateEventArgs` | 2 |
| `NavigateEventHandler` | 1 |
| `NodeLabelEditEventArgs` | 6 |
| `NodeLabelEditEventHandler` | 1 |
| `NotifyIcon` | 30 |
| `NumericUpDownAcceleration` | 5 |
| `NumericUpDownAccelerationCollection` | 10 |
| `OSFeature` | 6 |
| `OpacityConverter` | 4 |
| `OpenFileDialog` | 17 |
| `Orientation` | 2 |
| `OwnerDrawPropertyBag` | 9 |
| `Padding` | 27 |
| `PaddingConverter` | 9 |
| `PaintEventArgs` | 6 |
| `PaintEventHandler` | 1 |
| `PictureBoxSizeMode` | 5 |
| `PopupEventArgs` | 6 |
| `PopupEventHandler` | 1 |
| `PowerLineStatus` | 3 |
| `PowerState` | 2 |
| `PowerStatus` | 5 |
| `PreProcessControlState` | 3 |
| `PreviewKeyDownEventArgs` | 10 |
| `PreviewKeyDownEventHandler` | 1 |
| `ProfessionalColorTable` | 60 |
| `ProfessionalColors` | 57 |
| `ProgressBarRenderer` | 7 |
| `ProgressBarStyle` | 3 |
| `PropertyGrid` | 140 |
| `PropertyManager` | 15 |
| `PropertySort` | 5 |
| `PropertyTabChangedEventArgs` | 3 |
| `PropertyTabChangedEventHandler` | 1 |
| `PropertyValueChangedEventArgs` | 3 |
| `PropertyValueChangedEventHandler` | 1 |
| `QueryAccessibilityHelpEventArgs` | 8 |
| `QueryAccessibilityHelpEventHandler` | 1 |
| `QueryContinueDragEventArgs` | 5 |
| `QueryContinueDragEventHandler` | 1 |
| `QuestionEventArgs` | 4 |
| `QuestionEventHandler` | 1 |
| `RadioButtonRenderer` | 10 |
| `RelatedImageListAttribute` | 2 |
| `RetrieveVirtualItemEventArgs` | 4 |
| `RetrieveVirtualItemEventHandler` | 1 |
| `RichTextBox` | 127 |
| `RichTextBoxFinds` | 5 |
| `RichTextBoxLanguageOptions` | 7 |
| `RichTextBoxScrollBars` | 7 |
| `RichTextBoxSelectionAttribute` | 3 |
| `RichTextBoxSelectionTypes` | 5 |
| `RichTextBoxStreamType` | 5 |
| `RichTextBoxWordPunctuations` | 4 |
| `RightToLeft` | 3 |
| `RowStyle` | 5 |
| `SaveFileDialog` | 11 |
| `Screen` | 20 |
| `ScreenCaptureMode` | 3 |
| `ScreenOrientation` | 4 |
| `ScrollBarRenderer` | 13 |
| `ScrollBars` | 4 |
| `ScrollButton` | 6 |
| `SearchDirectionHint` | 4 |
| `SearchForVirtualItemEventArgs` | 10 |
| `SearchForVirtualItemEventHandler` | 1 |
| `SecurityIDType` | 9 |
| `SelectedGridItemChangedEventArgs` | 3 |
| `SelectedGridItemChangedEventHandler` | 1 |
| `SelectionMode` | 4 |
| `SelectionRange` | 8 |
| `SelectionRangeConverter` | 9 |
| `SendKeys` | 3 |
| `Shortcut` | 154 |
| `SizeGripStyle` | 3 |
| `SizeType` | 3 |
| `SortOrder` | 3 |
| `SplitterCancelEventArgs` | 7 |
| `SplitterCancelEventHandler` | 1 |
| `SplitterEventArgs` | 7 |
| `SplitterEventHandler` | 1 |
| `StatusStrip` | 31 |
| `StructFormat` | 3 |
| `SystemColorMode` | 3 |
| `SystemInformation` | 110 |
| `SystemParameter` | 11 |
| `TabAlignment` | 4 |
| `TabAppearance` | 3 |
| `TabControlAction` | 4 |
| `TabControlCancelEventArgs` | 4 |
| `TabControlCancelEventHandler` | 1 |
| `TabControlEventArgs` | 4 |
| `TabControlEventHandler` | 1 |
| `TabDrawMode` | 2 |
| `TabRenderer` | 10 |
| `TabSizeMode` | 3 |
| `TableLayoutCellPaintEventArgs` | 4 |
| `TableLayoutCellPaintEventHandler` | 1 |
| `TableLayoutColumnStyleCollection` | 7 |
| `TableLayoutControlCollection` | 3 |
| `TableLayoutPanelCellBorderStyle` | 7 |
| `TableLayoutPanelCellPosition` | 12 |
| `TableLayoutPanelGrowStyle` | 3 |
| `TableLayoutRowStyleCollection` | 7 |
| `TableLayoutSettings` | 19 |
| `TableLayoutStyle` | 3 |
| `TableLayoutStyleCollection` | 6 |
| `TaskDialog` | 8 |
| `TaskDialogButton` | 30 |
| `TaskDialogButtonCollection` | 6 |
| `TaskDialogCommandLinkButton` | 4 |
| `TaskDialogControl` | 3 |
| `TaskDialogExpander` | 14 |
| `TaskDialogExpanderPosition` | 2 |
| `TaskDialogFootnote` | 8 |
| `TaskDialogIcon` | 15 |
| `TaskDialogLinkClickedEventArgs` | 2 |
| `TaskDialogPage` | 43 |
| `TaskDialogProgressBar` | 12 |
| `TaskDialogProgressBarState` | 6 |
| `TaskDialogRadioButton` | 10 |
| `TaskDialogRadioButtonCollection` | 6 |
| `TaskDialogStartupLocation` | 2 |
| `TaskDialogVerificationCheckBox` | 9 |
| `TextBoxRenderer` | 6 |
| `TextDataFormat` | 5 |
| `TextFormatFlags` | 28 |
| `TextImageRelation` | 5 |
| `TextRenderer` | 28 |
| `ThreadExceptionDialog` | 4 |
| `TickStyle` | 4 |
| `ToolStrip` | 154 |
| `ToolStripArrowRenderEventArgs` | 9 |
| `ToolStripArrowRenderEventHandler` | 1 |
| `ToolStripButton` | 24 |
| `ToolStripComboBox` | 70 |
| `ToolStripContainer` | 47 |
| `ToolStripContentPanel` | 47 |
| `ToolStripContentPanelRenderEventArgs` | 5 |
| `ToolStripContentPanelRenderEventHandler` | 1 |
| `ToolStripControlHost` | 87 |
| `ToolStripDropDown` | 115 |
| `ToolStripDropDownButton` | 19 |
| `ToolStripDropDownCloseReason` | 5 |
| `ToolStripDropDownClosedEventArgs` | 2 |
| `ToolStripDropDownClosedEventHandler` | 1 |
| `ToolStripDropDownClosingEventArgs` | 2 |
| `ToolStripDropDownClosingEventHandler` | 1 |
| `ToolStripDropDownDirection` | 7 |
| `ToolStripDropDownItem` | 32 |
| `ToolStripDropDownItemAccessibleObject` | 5 |
| `ToolStripDropDownMenu` | 17 |
| `ToolStripGripDisplayStyle` | 2 |
| `ToolStripGripRenderEventArgs` | 4 |
| `ToolStripGripRenderEventHandler` | 1 |
| `ToolStripGripStyle` | 2 |
| `ToolStripItem` | 211 |
| `ToolStripItemAlignment` | 2 |
| `ToolStripItemClickedEventArgs` | 2 |
| `ToolStripItemClickedEventHandler` | 1 |
| `ToolStripItemCollection` | 23 |
| `ToolStripItemDisplayStyle` | 4 |
| `ToolStripItemEventArgs` | 2 |
| `ToolStripItemEventHandler` | 1 |
| `ToolStripItemImageRenderEventArgs` | 4 |
| `ToolStripItemImageRenderEventHandler` | 1 |
| `ToolStripItemImageScaling` | 2 |
| `ToolStripItemOverflow` | 3 |
| `ToolStripItemPlacement` | 3 |
| `ToolStripItemRenderEventArgs` | 4 |
| `ToolStripItemRenderEventHandler` | 1 |
| `ToolStripItemTextRenderEventArgs` | 14 |
| `ToolStripItemTextRenderEventHandler` | 1 |
| `ToolStripLabel` | 26 |
| `ToolStripLayoutStyle` | 5 |
| `ToolStripManager` | 19 |
| `ToolStripManagerRenderMode` | 3 |
| `ToolStripOverflow` | 8 |
| `ToolStripOverflowButton` | 4 |
| `ToolStripPanel` | 56 |
| `ToolStripPanelRenderEventArgs` | 5 |
| `ToolStripPanelRenderEventHandler` | 1 |
| `ToolStripPanelRow` | 20 |
| `ToolStripProfessionalRenderer` | 23 |
| `ToolStripProgressBar` | 39 |
| `ToolStripRenderEventArgs` | 7 |
| `ToolStripRenderEventHandler` | 1 |
| `ToolStripRenderMode` | 4 |
| `ToolStripRenderer` | 70 |
| `ToolStripSeparator` | 50 |
| `ToolStripSeparatorRenderEventArgs` | 2 |
| `ToolStripSeparatorRenderEventHandler` | 1 |
| `ToolStripSplitButton` | 40 |
| `ToolStripStatusLabel` | 21 |
| `ToolStripStatusLabelBorderSides` | 6 |
| `ToolStripSystemRenderer` | 14 |
| `ToolStripTextBox` | 83 |
| `ToolStripTextDirection` | 4 |
| `ToolTipIcon` | 4 |
| `TrackBarRenderer` | 15 |
| `TreeNode` | 69 |
| `TreeNodeCollection` | 32 |
| `TreeNodeConverter` | 3 |
| `TreeNodeMouseClickEventArgs` | 2 |
| `TreeNodeMouseClickEventHandler` | 1 |
| `TreeNodeMouseHoverEventArgs` | 2 |
| `TreeNodeMouseHoverEventHandler` | 1 |
| `TreeNodeStates` | 9 |
| `TreeViewAction` | 5 |
| `TreeViewCancelEventArgs` | 3 |
| `TreeViewCancelEventHandler` | 1 |
| `TreeViewDrawMode` | 3 |
| `TreeViewEventArgs` | 4 |
| `TreeViewEventHandler` | 1 |
| `TreeViewHitTestInfo` | 3 |
| `TreeViewHitTestLocations` | 11 |
| `TreeViewImageIndexConverter` | 5 |
| `TreeViewImageKeyConverter` | 2 |
| `TypeValidationEventArgs` | 7 |
| `TypeValidationEventHandler` | 1 |
| `UICues` | 7 |
| `UICuesEventArgs` | 6 |
| `UICuesEventHandler` | 1 |
| `UnhandledExceptionMode` | 3 |
| `UpDownBase` | 73 |
| `UpDownEventArgs` | 2 |
| `UpDownEventHandler` | 1 |
| `View` | 5 |
| `WindowsFormsSection` | 3 |
| `WindowsFormsSynchronizationContext` | 8 |

## Honest current states

| Surface | State | Rows |
|---|---|---:|
| `System.Drawing` | `excluded` | 472 |
| `System.Drawing` | `measured_partial` | 1 |
| `System.Drawing` | `missing` | 1875 |
| `System.Drawing` | `package_review` | 19 |
| `System.Drawing` | `partial_native` | 1138 |
| `System.Drawing` | `platform_extension_partial` | 17 |
| `System.Windows.Forms` | `excluded` | 1828 |
| `System.Windows.Forms` | `measured_partial` | 494 |
| `System.Windows.Forms` | `missing` | 9469 |
| `System.Windows.Forms` | `partial_native` | 2909 |
| `System.Windows.Forms` | `platform_extension_review` | 62 |

## Families

| Surface | Family | Rows |
|---|---|---:|
| `System.Drawing` | `drawing_misc` | 581 |
| `System.Drawing` | `fonts_text_glyphs` | 198 |
| `System.Drawing` | `graphics_state_commands_targets` | 305 |
| `System.Drawing` | `images_pixels_color_adjustment` | 1036 |
| `System.Drawing` | `paint_gradients_textures` | 480 |
| `System.Drawing` | `paths_transforms_regions_strokes` | 386 |
| `System.Drawing` | `values_geometry_color` | 536 |
| `System.Windows.Forms` | `accessibility_semantics` | 435 |
| `System.Windows.Forms` | `application_window_control_kernel` | 819 |
| `System.Windows.Forms` | `collections_binding_virtualization` | 3739 |
| `System.Windows.Forms` | `commands_popups_guidance` | 1700 |
| `System.Windows.Forms` | `controls_components_misc` | 5113 |
| `System.Windows.Forms` | `dialogs_host_services` | 607 |
| `System.Windows.Forms` | `input_transfer` | 479 |
| `System.Windows.Forms` | `layout_scrolling` | 780 |
| `System.Windows.Forms` | `style_resources_owner_draw` | 327 |
| `System.Windows.Forms` | `text_editing` | 763 |

## Policy boundary

The base excludes ActiveX/browser hosting, printing, MDI, design-time services, classic `DataGrid`, obsolete menu/toolbar/status families, metafile playback, and arbitrary desktop capture. Raw HWND/HDC members are platform-extension work behind opaque leases, never portable public types. PNG is the renderer-core codec; other codec/metadata families remain package review.

Everything else is an admitted oversight row, not an implementation promise. Promotion to `native` requires a named test or runtime trace for the exact behavioral center; generated identity resolution alone is facade evidence.

## Regeneration

See `tools/winforms_catalogue/README.md`. Regeneration is deterministic for the pinned inputs and writes `planning/generated/WINFORMS_API_CATALOGUE.tsv` plus this summary.
