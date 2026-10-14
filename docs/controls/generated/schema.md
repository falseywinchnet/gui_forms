# Control schema

Generated from `manifests/control-schema.json` by `python3 tools/control_schema.py render`;
do not edit. The manifest is written by `gui_forms_gallery_pages_tests` from the run-time
registry (stage T1), so this page lists what the library publishes.

Every name here is a registered name, stored in snake_case (owner decision 8). Theme
and form documents, which are CSS-family, write the same words in kebab-case
(`scroll_thumb` is `::part(scroll-thumb)`, `auto_size_mode` is `auto-size-mode`);
PropertyGrid and C# show them in PascalCase (`AutoSizeMode`). C++ reads the anatomy as
typed records (`(*bar).parts().thumb`) and registers handlers through each type's events
(`(*button).clicked()`). An interaction's payload is the handler's argument types; *by
reference* marks a payload the handler receives by mutable reference.

Every control also has these properties: `accessible_description`, `accessible_name`, `allow_drop`, `anchor`, `auto_scroll_offset`, `auto_size`, `auto_size_mode`, `bounds`, `causes_validation`, `dock`, `enabled`, `hit_test_transparent`, `margin`, `maximum_size`, `minimum_size`, `name`, `padding`, `tab_index`, `tab_stop`, `visible`.

Every control also raises these interactions: `name_changed`, `visible_changed`, `enabled_changed`, `auto_size_changed`, `causes_validation_changed`, `validating`, `validated`, `initialization_completed`, `pointer_observed`, `pointer_preview_observed`, `focus_observed`, `arranged_bounds_changed`, `help_requested`.

## `button_base` (ButtonBase, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `icon` | icon |  |  |
| `label` | label |  |  |
| `focus` | focus |  |  |

States: `normal`, `hot`, `pressed`, `enabled`, `disabled`, `focused`, `focus_visible`, `open`, `closed`.

Properties beyond every control's: `content_padding` (insets), `font` (font), `image` (image), `text` (text).

| Interaction | Payload |
|---|---|
| `expanded_changed` | `std::optional<bool>` |
| `clicked` | `ButtonBase`, by reference |
| `text_changed` | `std::string` |

## `button` (Button, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `icon` | icon |  |  |
| `label` | label |  |  |
| `preselect` | preselect |  |  |
| `focus` | focus |  |  |

States: `normal`, `hot`, `pressed`, `preselect`, `selected`, `enabled`, `disabled`, `focused`, `focus_visible`, `open`, `closed`.

Properties beyond every control's: `content_padding` (insets), `font` (font), `image` (image), `text` (text).

| Interaction | Payload |
|---|---|
| `expanded_changed` | `std::optional<bool>` |
| `clicked` | `ButtonBase`, by reference |
| `text_changed` | `std::string` |
| `dialog_result_changed` | `DialogResult` |
| `selected_changed` | `bool` |

## `drop_down_button` (DropDownButton, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `icon` | icon |  |  |
| `label` | label |  |  |
| `drop_down` | actuator |  | pressed |
| `disclosure` | disclosure |  |  |
| `preselect` | preselect |  |  |
| `focus` | focus |  |  |

States: `normal`, `hot`, `pressed`, `preselect`, `selected`, `enabled`, `disabled`, `focused`, `focus_visible`, `open`, `closed`.

Properties beyond every control's: `content_padding` (insets), `font` (font), `image` (image), `text` (text).

| Interaction | Payload |
|---|---|
| `expanded_changed` | `std::optional<bool>` |
| `clicked` | `ButtonBase`, by reference |
| `text_changed` | `std::string` |
| `dialog_result_changed` | `DialogResult` |
| `selected_changed` | `bool` |
| `drop_down_requested` | `DropDownButton`, by reference |
| `drop_down_close_requested` | `DropDownButton`, by reference |

## `link_label` (LinkLabel, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `label` | label |  |  |
| `underline` | glyph |  |  |
| `focus` | focus |  |  |

States: `normal`, `hot`, `pressed`, `enabled`, `disabled`, `focused`, `focus_visible`, `open`, `closed`. Application-defined: `visited`.

Properties beyond every control's: `content_padding` (insets), `font` (font), `image` (image), `text` (text).

| Interaction | Payload |
|---|---|
| `expanded_changed` | `std::optional<bool>` |
| `clicked` | `ButtonBase`, by reference |
| `text_changed` | `std::string` |

## `command_bar` (CommandBar, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `invoked` | `CommandItem` |

## `text_box` (TextBox, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `well` | well |  |  |
| `content` | content |  |  |
| `caret` | caret |  |  |
| `selection` | selection |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`, `read_only`, `invalid`, `pending`.

Properties beyond every control's: `text` (text).

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `text_changed` | `std::string` |
| `selection_changed` | `TextSelection` |
| `committed` | `std::string` |
| `cancelled` | none |

## `label` (Label, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `label` | label |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: `font` (font), `fore_color` (color), `maximum_lines` (unsigned_integer), `text` (text).

| Interaction | Payload |
|---|---|
| `text_changed` | `std::string` |

## `picture_box` (PictureBox, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `image` | icon |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: `image` (image), `image_opacity` (number), `size_mode` (enumeration).

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `image_changed` | `ImageId` |

## `progress_bar` (ProgressBar, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `track` | track |  |  |
| `fill` | fill |  |  |
| `activity` | activity |  |  |

States: `normal`, `indeterminate`, `enabled`, `disabled`, `reduced_motion`.

Properties beyond every control's: `value` (number).

| Interaction | Payload |
|---|---|
| `range_changed` | `double, double` |
| `value_changed` | `double` |
| `scroll` | `RangeScrollEvent` |

## `review_card` (ReviewCard, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `hot`, `pressed`, `captured`, `selected`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `activated` | `Card`, by reference |
| `selected_changed` | `bool` |
| `record_changed` | `ReviewRecord` |

## `list_box` (ListBox, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `well` | well |  |  |
| `content` | content |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Item system `item`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `row` | region |  |  |
| `label` | label |  |  |
| `row_focus` | focus |  |  |

Item states: `selected`, `hot`, `current`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `selection_changed` | `ListSelectionChange` |
| `item_activated` | `std::size_t` |

## `checked_list_box` (CheckedListBox, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `well` | well |  |  |
| `content` | content |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Item system `item`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `row` | region |  |  |
| `label` | label |  |  |
| `row_focus` | focus |  |  |
| `check_well` | well |  |  |
| `check` | indicator |  |  |

Item states: `selected`, `hot`, `current`, `checked`, `indeterminate`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `selection_changed` | `ListSelectionChange` |
| `item_activated` | `std::size_t` |
| `item_checking` | `ItemCheckEvent`, by reference |
| `item_check_state_changed` | `std::size_t, CheckState` |

## `combo_box` (ComboBox, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `well` | well |  |  |
| `content` | content |  |  |
| `divider` | divider |  |  |
| `drop_down` | actuator | `button` |  |
| `disclosure` | disclosure |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`, `open`, `closed`.

Properties beyond every control's: `items` (collection), `selected_index` (signed_integer), `text` (text).

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `selected_index_changed` | `std::optional<std::size_t>` |
| `drop_down_changed` | `bool` |
| `items_changed` | none |

## `tree_view` (TreeView, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `content` | content |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Item system `item`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `row` | region |  |  |
| `expander` | disclosure |  |  |
| `icon` | icon |  |  |
| `label` | label |  |  |
| `row_focus` | focus |  |  |

Item states: `selected`, `hot`, `current`, `open`, `closed`, `disabled`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `selection_changed` | `TreeSelectionChange` |
| `expansion_changed` | `TreeExpansionChange` |
| `item_activated` | `std::string` |

## `object_view` (ObjectView, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `header` | face |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Item system `item`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `cell` | region |  |  |
| `glyph` | icon |  |  |
| `name` | label |  |  |
| `secondary` | content |  |  |
| `cell_focus` | focus |  |  |

Item states: `selected`, `hot`, `current`, `disabled`.

Item system `column`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `column_header` | region |  |  |
| `column_label` | label |  |  |
| `sort_glyph` | glyph |  |  |
| `column_divider` | divider |  |  |
| `column_grip` | grip |  |  |
| `column_focus` | focus |  |  |

Item states: `pressed`, `captured`, `current`. Application-defined: `sorted_ascending`, `sorted_descending`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `sort_requested` | `ObjectDetailsSort` |
| `selection_changed` | `ObjectSelectionChange` |
| `item_activated` | `std::string` |
| `context_requested` | `ObjectContextRequest` |

## `correspondence_view` (CorrespondenceView, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Item system `item`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `card` | face |  |  |
| `status_rail` | keyline |  |  |
| `glyph` | icon |  |  |
| `title` | label |  |  |
| `secondary` | content |  |  |
| `metric` | content |  |  |
| `evidence` | well |  |  |
| `card_focus` | focus |  |  |

Item states: `selected`, `hot`, `current`, `open`, `unavailable`, `disabled`. Application-defined: `pinned`, `stale`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `selection_changed` | `CorrespondenceSelectionChange` |
| `pin_changed` | `CorrespondencePinChange` |
| `expansion_changed` | `CorrespondenceExpansionChange` |
| `item_activated` | `std::string` |
| `context_requested` | `ObjectContextRequest` |

## `choice_group` (ChoiceGroup, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `changed` | `int` |
| `activated` | `int` |

## `breadcrumb_trail` (BreadcrumbTrail, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Item system `item`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `segment` | face | `button` |  |
| `segment_label` | label |  |  |
| `segment_cap` | cap |  |  |
| `segment_focus` | focus |  |  |

Item states: `hot`, `current`, `disabled`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `segment_activated` | `std::string` |
| `overflow_activated` | none |
| `edit_committed` | `std::string` |
| `edit_started` | `std::string` |
| `edit_cancelled` | none |
| `edit_completion_requested` | none |

## `check_box` (CheckBox, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `well` | well |  |  |
| `indicator` | indicator |  |  |
| `icon` | icon |  |  |
| `label` | label |  |  |
| `focus` | focus |  |  |

States: `normal`, `hot`, `pressed`, `checked`, `indeterminate`, `enabled`, `disabled`, `focused`, `focus_visible`, `open`, `closed`.

Properties beyond every control's: `checked` (boolean), `content_padding` (insets), `font` (font), `image` (image), `text` (text).

| Interaction | Payload |
|---|---|
| `expanded_changed` | `std::optional<bool>` |
| `clicked` | `ButtonBase`, by reference |
| `text_changed` | `std::string` |
| `check_state_changed` | `CheckState` |
| `checked_changed` | `bool` |

## `radio_button` (RadioButton, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `well` | well |  |  |
| `indicator` | indicator |  |  |
| `icon` | icon |  |  |
| `label` | label |  |  |
| `focus` | focus |  |  |

States: `normal`, `hot`, `pressed`, `checked`, `enabled`, `disabled`, `focused`, `focus_visible`, `open`, `closed`.

Properties beyond every control's: `checked` (boolean), `content_padding` (insets), `font` (font), `image` (image), `text` (text).

| Interaction | Payload |
|---|---|
| `expanded_changed` | `std::optional<bool>` |
| `clicked` | `ButtonBase`, by reference |
| `text_changed` | `std::string` |
| `checked_changed` | `bool` |

## `range_control` (RangeControl, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `focus` | focus |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: `value` (number).

| Interaction | Payload |
|---|---|
| `range_changed` | `double, double` |
| `value_changed` | `double` |
| `scroll` | `RangeScrollEvent` |

## `track_bar` (TrackBar, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `track` | track |  |  |
| `fill` | fill |  |  |
| `ticks` | glyph |  |  |
| `thumb` | thumb |  | pressed, captured |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: `value` (number).

| Interaction | Payload |
|---|---|
| `range_changed` | `double, double` |
| `value_changed` | `double` |
| `scroll` | `RangeScrollEvent` |

## `numeric_up_down` (NumericUpDown, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `field` | well | `text_box` |  |
| `increment_arrow` | actuator | `button` | hot, pressed |
| `decrement_arrow` | actuator | `button` | hot, pressed |
| `increment_glyph` | glyph |  | hot, pressed |
| `decrement_glyph` | glyph |  | hot, pressed |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`, `invalid`, `pending`.

Properties beyond every control's: `value` (number).

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `value_changed` | `double` |

## `domain_up_down` (DomainUpDown, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `field` | well | `text_box` |  |
| `up_arrow` | actuator | `button` | hot, pressed |
| `down_arrow` | actuator | `button` | hot, pressed |
| `up_glyph` | glyph |  | hot, pressed |
| `down_glyph` | glyph |  | hot, pressed |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`, `read_only`, `invalid`, `pending`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `selected_index_changed` | `std::optional<std::size_t>` |

## `color_value_editor` (ColorValueEditor, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `swatch` | chip |  |  |
| `field` | well | `text_box` |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`, `invalid`, `pending`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `value_changed` | `Color` |
| `edit_failed` | `PropertyEditorInputError` |

## `flags_value_editor` (FlagsValueEditor, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `content` | content |  |  |
| `divider` | divider |  |  |
| `drop_down` | actuator | `button` |  |
| `disclosure` | disclosure |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`, `open`, `closed`, `invalid`, `pending`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `value_changed` | `PropertyEnumValue` |
| `drop_down_changed` | `bool` |

## `property_grid` (PropertyGrid, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `selected_object_changed` | `Control::Ptr` |
| `property_value_changed` | `PropertyGridValueChange` |
| `edit_failed` | `PropertyGridEditError` |

## `property_list` (PropertyList, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Item system `item`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `group_header` | face |  |  |
| `group_expander` | disclosure |  |  |
| `group_title` | caption |  |  |
| `property_row` | region |  |  |
| `property_name` | label |  |  |
| `property_value` | content |  |  |
| `row_expander` | disclosure |  |  |
| `validation` | validation |  |  |
| `row_divider` | divider |  |  |

Item states: `open`, `closed`, `invalid`, `read_only`, `disabled`. Application-defined: `group_header`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `value_changed` | `PropertyValueChange` |
| `value_committed` | `PropertyValueChange` |
| `group_changed` | `PropertyGroupChange` |
| `row_expansion_changed` | `PropertyRowExpansionChange` |
| `reset_requested` | `PropertyResetRequest` |

## `instrument_rack` (InstrumentRack, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `field_changed` | `InstrumentFieldChange` |
| `field_committed` | `InstrumentFieldChange` |
| `module_toggled` | `InstrumentModuleToggle` |
| `remove_requested` | `InstrumentModuleRequest` |
| `move_requested` | `InstrumentModuleMoveRequest` |

## `date_time_picker` (DateTimePicker, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `content` | content |  |  |
| `drop_down` | actuator | `button` |  |
| `disclosure` | disclosure |  |  |
| `increment_arrow` | actuator | `button` | pressed |
| `decrement_arrow` | actuator | `button` | pressed |
| `well` | well |  | pressed |
| `indicator` | indicator |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`, `open`, `closed`, `checked`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `value_changed` | `DateTimeValue` |
| `checked_changed` | `bool` |
| `drop_down_changed` | `bool` |

## `panel` (Panel, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |

## `group_box` (GroupBox, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `caption` | caption |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |

## `scaled_group_box` (ScaledGroupBox, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `caption` | caption |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |

## `material_panel` (MaterialPanel, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `material_changed` | `SurfaceMaterial` |

## `card` (Card, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `hot`, `pressed`, `captured`, `selected`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `activated` | `Card`, by reference |
| `selected_changed` | `bool` |

## `expandable_sections` (ExpandableSections, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `toggled` | `int, bool` |

## `container_control` (ContainerControl, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `auto_validate_changed` | `AutoValidate` |

## `user_control` (UserControl, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `auto_validate_changed` | `AutoValidate` |
| `loaded` | none |

## `tab_control` (TabControl, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `display` | face |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Item system `item`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `tab` | face |  |  |
| `tab_label` | label |  |  |
| `tab_focus` | focus |  |  |
| `tab_seam` | seam |  |  |

Item states: `selected`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `auto_validate_changed` | `AutoValidate` |
| `selected_index_changed` | `TabSelectionChange` |

## `tab_page` (TabPage, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |

## `flow_layout_panel` (FlowLayoutPanel, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `auto_validate_changed` | `AutoValidate` |

## `table_layout_panel` (TableLayoutPanel, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `cell_grid` | divider |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `auto_validate_changed` | `AutoValidate` |

## `responsive_track_panel` (ResponsiveTrackPanel, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `auto_validate_changed` | `AutoValidate` |

## `scaled_panel` (ScaledPanel, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |

## `split_container` (SplitContainer, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `seam` | seam |  | hot, pressed, captured |
| `grip` | grip |  | open, closed |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`, `hot`, `open`, `closed`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `auto_validate_changed` | `AutoValidate` |
| `splitter_changed` | `SplitChangeEvent` |

## `splitter_panel` (SplitterPanel, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `auto_validate_changed` | `AutoValidate` |

## `command_overflow_panel` (CommandOverflowPanel, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `auto_validate_changed` | `AutoValidate` |

## `master_detail_view` (MasterDetailView, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `auto_validate_changed` | `AutoValidate` |
| `presentation_changed` | `MasterDetailPresentationChange` |

## `scrollable_control` (ScrollableControl, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |

## `scroll_bar` (ScrollBar, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `decrement_arrow` | actuator | `button` | pressed |
| `increment_arrow` | actuator | `button` | pressed |
| `track` | track |  | pressed |
| `thumb` | thumb |  | pressed, captured |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: `value` (number).

| Interaction | Payload |
|---|---|
| `range_changed` | `double, double` |
| `value_changed` | `double` |
| `scroll` | `RangeScrollEvent` |

## `h_scroll_bar` (HScrollBar, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `decrement_arrow` | actuator | `button` | pressed |
| `increment_arrow` | actuator | `button` | pressed |
| `track` | track |  | pressed |
| `thumb` | thumb |  | pressed, captured |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: `value` (number).

| Interaction | Payload |
|---|---|
| `range_changed` | `double, double` |
| `value_changed` | `double` |
| `scroll` | `RangeScrollEvent` |

## `v_scroll_bar` (VScrollBar, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `decrement_arrow` | actuator | `button` | pressed |
| `increment_arrow` | actuator | `button` | pressed |
| `track` | track |  | pressed |
| `thumb` | thumb |  | pressed, captured |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: `value` (number).

| Interaction | Payload |
|---|---|
| `range_changed` | `double, double` |
| `value_changed` | `double` |
| `scroll` | `RangeScrollEvent` |

## `menu_strip` (MenuStrip, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `divider` | divider |  |  |
| `focus` | focus |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus_visible`.

Item system `item`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `menu_item` | face |  |  |
| `menu_label` | label |  |  |
| `menu_focus` | focus |  |  |

Item states: `hot`, `open`, `selected`, `disabled`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `item_invoked` | `MenuStripInvocation` |
| `open_changed` | `std::optional<std::size_t>` |

## `tool_strip` (ToolStrip, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `grip` | grip |  |  |
| `overflow_button` | actuator | `button` | hot, open |
| `overflow_glyph` | glyph |  | hot, open |
| `focus` | focus |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus_visible`.

Item system `item`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `item_face` | face |  |  |
| `item_icon` | icon |  |  |
| `item_label` | label |  |  |
| `item_arrow` | actuator | `button` | hot, pressed |
| `item_arrow_glyph` | glyph |  |  |
| `item_separator` | divider |  |  |
| `item_focus` | focus |  |  |

Item states: `hot`, `pressed`, `checked`, `open`, `disabled`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `item_clicked` | `ToolStripItemClick` |
| `drop_down_item_invoked` | `ToolStripDropDownInvocation` |

## `status_strip` (StatusStrip, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `sizing_grip` | grip |  |  |
| `focus` | focus |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus_visible`.

Item system `item`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `item_face` | face |  |  |
| `item_icon` | icon |  |  |
| `item_label` | label |  |  |
| `item_arrow` | actuator | `button` | hot, pressed |
| `item_arrow_glyph` | glyph |  |  |
| `item_separator` | divider |  |  |
| `item_focus` | focus |  |  |

Item states: `hot`, `pressed`, `checked`, `open`, `disabled`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `item_clicked` | `ToolStripItemClick` |
| `drop_down_item_invoked` | `ToolStripDropDownInvocation` |

## `context_menu` (ContextMenu, component)

| Interaction | Payload |
|---|---|
| `item_invoked` | `MenuItemInvocation` |
| `open_changed` | `bool` |

## `command` (Command, component)

| Interaction | Payload |
|---|---|
| `state_changed` | `CommandState` |
| `invoked` | `CommandInvocation` |

## `anchored_popup_layer` (AnchoredPopupLayer, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal_decrement_arrow` | actuator | `button` |  |
| `horizontal_track` | track |  |  |
| `horizontal_thumb` | thumb |  | pressed, captured |
| `horizontal_increment_arrow` | actuator | `button` |  |
| `vertical_decrement_arrow` | actuator | `button` |  |
| `vertical_track` | track |  |  |
| `vertical_thumb` | thumb |  | pressed, captured |
| `vertical_increment_arrow` | actuator | `button` |  |
| `scroll_corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `dismiss_requested` | `PopupDismissReason` |

## `tool_tip` (ToolTip, component)

| Interaction | Payload |
|---|---|
| `visibility_changed` | `ToolTipEvent` |

## `drawing_surface` (DrawingSurface, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `focus` | focus |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

Interactions beyond every control's: none.

## `raster_canvas` (RasterCanvas, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `image` | icon |  |  |
| `focus` | focus |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

Interactions beyond every control's: none.

## `live_surface` (LiveSurface, service)

Interactions: none.

## `easing_preview` (EasingPreview, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `focus` | focus |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

Interactions beyond every control's: none.

## `metrics_view` (MetricsView, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `accent_strip` | keyline |  |  |
| `focus` | focus |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

Interactions beyond every control's: none.

## `typography_specimen` (TypographySpecimen, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `focus` | focus |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

Interactions beyond every control's: none.

## `visual_inspector_view` (VisualInspectorView, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `header` | face |  |  |
| `accent_strip` | keyline |  |  |
| `focus` | focus |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

Interactions beyond every control's: none.

## `visual_inspector_overlay` (VisualInspectorOverlay, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `highlight` | selection |  |  |
| `focus` | focus |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus_visible`.

Properties beyond every control's: none.

Interactions beyond every control's: none.

## `timer` (Timer, component)

| Interaction | Payload |
|---|---|
| `tick` | none |

## `notify_icon` (NotifyIcon, component)

| Interaction | Payload |
|---|---|
| `click` | `NotifyIconClick` |
| `double_click` | none |
| `balloon_tip_clicked` | none |
| `balloon_tip_closed` | none |
| `menu_item_invoked` | `MenuItemInvocation` |

## `image_list` (ImageList, component)

| Interaction | Payload |
|---|---|
| `changed` | `ImageListChange` |

## `error_provider` (ErrorProvider, component)

| Interaction | Payload |
|---|---|
| `right_to_left_changed` | `bool` |
| `data_source_changed` | none |
| `data_member_changed` | `std::string` |
| `error_changed` | `ErrorProviderChange` |

## `help_provider` (HelpProvider, component)

| Interaction | Payload |
|---|---|
| `help_requested` | `HelpRequestEvent`, by reference |

## `semantic_feedback` (SemanticFeedback, component)

| Interaction | Payload |
|---|---|
| `emitted` | `SemanticFeedbackRecord` |

## `binding` (Binding, component)

| Interaction | Payload |
|---|---|
| `format` | `BindingConvertEvent`, by reference |
| `parse` | `BindingConvertEvent`, by reference |
| `binding_complete` | `BindingCompleteEvent`, by reference |

## `binding_source` (BindingSource, component)

| Interaction | Payload |
|---|---|
| `list_changed` | `BindingListChange` |
| `current_changed` | none |
| `current_item_changed` | none |
| `position_changed` | `std::ptrdiff_t` |
| `data_error` | `std::string` |
| `data_source_changed` | none |
| `data_member_changed` | `std::string` |
| `binding_complete` | `BindingCompleteEvent`, by reference |
| `disposed_event` | none |

## `binding_context` (BindingContext, component)

| Interaction | Payload |
|---|---|
| `collection_changed` | `BindingContextChange` |
