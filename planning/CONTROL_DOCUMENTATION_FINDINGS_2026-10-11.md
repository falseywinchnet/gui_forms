# Behaviour found while documenting the controls (2026-10-11)

Writing the API reference docstrings in `docs/library/docstrings/` meant
reading every control's header, implementation and tests. These are the
behaviours found that look like defects or surprise a Windows Forms user. None
is fixed here; the docstrings describe the code as it behaves today. File and
line references are as of commit 44e3959 plus the documentation branch.

## Likely defects

- `UpdateScope` default constructor opens no scope but `close()` and the
  destructor still close one (`window/update_scope.cpp:30-35`): with none open
  it throws "update scope underflow" from a destructor and terminates; with an
  outer scope open it closes that one early.
- `~UpdateScope` is implicitly noexcept but calls `close()`, which can throw
  (wrong thread, or user layout code throwing at the outermost close).
- `Window` constructor does not reject a root already owned by another window
  (`window_lifecycle.cpp:35-45`; `window.cpp:1692-1700`).
- Destroying a `Window` off its UI thread terminates (`~Window` ->
  `detach_subtree`, `window_lifecycle.cpp:67`, `window.cpp:1773`).
- `Window::find` has no UI-thread check (`window.cpp:446-450`).
- `InstrumentRack::on_dispose` never calls `Control::on_dispose()`
  (`instrument_rack.cpp:1065-1069`): a disposed rack stays in its parent and
  skips base cleanup.
- `SemanticFeedback` holds a raw `Window*` with no lifetime tracking
  (`semantic_feedback.hpp:45`); `emit` after the window dies is a
  use-after-free. Its constructor skips the thread check.
- `Binding::update_source` counts a refused source write as a success and
  returns false with no `binding_complete` or `data_error` (`binding.cpp:257-260`).
- `MasterDetailView::configure_split` resets the splitter distance on every
  measure and arrange (`master_detail_view.cpp:104`, called at 237 and 278),
  so a user resize does not stick.
- Turning on `auto_scroll` (or setting `auto_scroll_min_size`) hides the
  children of Card, ExpandableSections, ScaledGroupBox and TabControl: their
  own `arrange` never sets the scrolling viewport (`scrollable_control.cpp:101`
  with `card.cpp:189`, `expandable_sections.cpp:208`, `scaled_group_box.cpp:61`,
  `tab_control.cpp:299`).
- ExpandableSections gives tab indexes only to headers (`expandable_sections.cpp:163`),
  so Tab visits every open section's content before the remaining headers.
- With nothing selected, Down/Right skips the first item in ListBox
  (`list_box.cpp:390-392`), ComboBox (`combo_box.cpp:485-487`) and ChoiceGroup
  (`choice_group.cpp:245-249`).
- Keyboard navigation stops on a disabled item in TreeView (`tree_view.cpp:220`),
  ObjectView (`object_view.cpp:815,825`) and CorrespondenceView
  (`correspondence_view.cpp:433`).
- CorrespondenceView activates disabled records on double-click (`:730`) and
  semantic press (`:945`) but not on Enter (`:754`); a press selects and pins
  them (`:716`, `:733`). Its semantic descriptor advertises increment and
  decrement that nothing handles (`:820-821`).
- `CheckedListBox` semantic press parses only index-derived IDs, so it fails
  after `set_item_stable_ids` (`checked_list_box.cpp:224-231`); `remove_item`
  erases check state before the disposed-control throw (`:51-56`).
- `BreadcrumbTrail::set_active_id` before the first layout is lost
  (`breadcrumb_trail.cpp:293-297`).
- `TreeView::set_items` clears a vanished selection without raising
  `selection_changed` (`tree_view.cpp:40-44`).
- DateTimePicker's `parse_iso_date` does not check that each field was fully
  consumed (`date_time_utilities.cpp:194-202`): "2026-1x-05" parses.
- DropDownButton split mode skips `Button::on_activate`, so `dialog_result` is
  never applied; menu mode never raises `clicked`, so a bound Command never runs
  (`drop_down_button.cpp:176`).
- ButtonBase accessibility expand/collapse calls `on_activate()` directly,
  skipping the enabled and validation checks (`button_base.cpp:870-874`).
- EasingPreview reduced motion is not centred, against the comment at
  `animation_timeline.hpp:75` (`easing_preview.cpp:163,173`).
- MetricsView's second line starts with a stray byte of the " · " separator
  (`metrics_view.cpp:71`, `substr(split + 3U)` on a 4-byte separator).
- Setters that accept out-of-range enum values silently while their siblings
  throw: `PictureBox::set_size_mode` (`picture_box.cpp:119`),
  `ProgressBar::set_visual_style`/`set_overlay_style` (`progress_bar.cpp:20,31`),
  `TabControl::set_alignment`/`set_appearance`.
- `MaterialPanel::set_material` lacks `require_mutable()` (`material_panel.cpp:15`);
  `BindingSource::set_raise_list_changed_events` lacks a thread check.
- `ObjectContextRequest::screen_position` holds window-client coordinates
  (`object_view.hpp:97`; `object_view.cpp:1090,1101`).
- The Windows host advertises `text_composition` but has no IME code.

## Behaviour a Windows Forms user will not expect

- A focused single-line TextBox consumes Enter and Escape, so the window's
  accept and cancel buttons never respond while it has focus
  (`text_box.cpp:1005-1020`).
- TextBox `maximum_length` applies to the result of an edit, so text longer
  than the limit assigned by `set_text` refuses every edit, deletions included
  (`text_box.cpp:309-313`).
- NumericUpDown commits on every keystroke; typing 150 into a 0-100 field
  commits 1, then 15, then refuses 150 (`numeric_up_down.cpp:32-38,149-168`).
- `ListBox::item_activated` fires on every single click release, not on
  double-click.
- `ColorValueEditor` and `FlagsValueEditor` raise `value_changed` only on a
  user commit, never from `set_value`.
- `set_dropped_down(true)` throws on a hidden or disabled control in a window
  but does nothing outside a window (FlagsValueEditor, DateTimePicker).
- SplitContainer `SplitFixedPanel::none` behaves like `first`; collapsing a
  pane sets the SplitterPanel's own `visible` to false; a set accessible
  description is replaced when `collapse_panel` is set (`split_container.cpp:256,291,372-381,526`).
- CommandOverflowPanel throws from layout with more than 32 groups and never
  grows a group past `preferred` (`command_overflow_panel.cpp:181,189`).
- TableLayoutPanel `percent` sizes are relative weights; content never widens
  a percent column.
- Activating a check or radio menu item does not toggle the Command's
  `checked` state.
- `Button` `flat` looks like `standard` under theme painting; `flat` and
  `flat_border_width` take effect only after `set_style`.
- TabControl paints with a default `BasicControlStyle`, colours tab text by
  `enabled()` rather than `effectively_enabled()`, and a tab click does not
  remember the old page's focused control (Ctrl+Tab does).
- LinkLabel colours by `enabled()` and paints with `font()`, ignoring the text
  scale (`link_label.cpp:30-36`).
- ScrollableControl's built-in bars do not auto-repeat; `ScrollBar` arrow
  auto-repeat continues after the pointer leaves the arrow.
- Several controls ignore `accessible_name`: VisualInspectorView,
  TypographySpecimen, MetricsView; ChoiceGroup items keep the `&` marker in
  their names (`choice_group.cpp:189`); `RasterCanvas` reads its zoom as
  "400.000000%" (`raster_canvas.cpp:344`).
- `Application::run` rejects `initially_visible = false` and `hide_on_close`
  for the primary window (`src/core/application/application.cpp:91`).

## Documentation inputs that were wrong

- `manual.json` described phantom methods `PropertyRegistration::descriptor`
  and `BindingRecord::stable_id` (data members the old parser misread); both
  entries are removed.
- `manual.json` setter texts claimed clamping where the setters throw
  (`ListBox::set_top_index`, `TreeView`/`ObjectView::set_top_row`) and claimed
  `set_selected_id` scrolls the item into view; pages for documented types no
  longer use manual.json member texts.
- ADR-004 says GUI.Forms never consults a host font catalogue; its own phase 7
  added a CoreText lookup to the GUI.Drawing C API.
