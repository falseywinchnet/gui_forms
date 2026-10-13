# Control schema

Generated from `manifests/control-schema.json` by `python3 tools/control_schema.py render`;
do not edit. The manifest is written by `gui_forms_gallery_pages_tests` from the run-time
registry (stage T1), so this page lists what the library publishes.

Part, state, item-system and interaction names are kebab-case identities that theme and
form documents address (`::part(thumb):state(pressed)`). C++ reads the same anatomy as
typed records (`(*bar).parts().thumb`) and registers handlers through each type's events
(`(*button).clicked()`). An interaction's payload is the handler's argument types; *by
reference* marks a payload the handler receives by mutable reference.

Every control also has these properties: `AccessibleDescription`, `AccessibleName`, `AllowDrop`, `Anchor`, `AutoScrollOffset`, `AutoSize`, `AutoSizeMode`, `Bounds`, `CausesValidation`, `Dock`, `Enabled`, `HitTestTransparent`, `Margin`, `MaximumSize`, `MinimumSize`, `Name`, `Padding`, `TabIndex`, `TabStop`, `Visible`.

Every control also raises these interactions: `name-changed`, `visible-changed`, `enabled-changed`, `auto-size-changed`, `causes-validation-changed`, `validating`, `validated`, `initialization-completed`, `pointer-observed`, `pointer-preview-observed`, `focus-observed`, `arranged-bounds-changed`, `help-requested`.

## `button-base` (ButtonBase, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `icon` | icon |  |  |
| `label` | label |  |  |
| `focus` | focus |  |  |

States: `normal`, `hot`, `pressed`, `enabled`, `disabled`, `focused`, `focus-visible`, `open`, `closed`.

Properties beyond every control's: `ContentPadding` (insets), `Font` (font), `Image` (image), `Text` (text).

| Interaction | Payload |
|---|---|
| `expanded-changed` | `std::optional<bool>` |
| `clicked` | `ButtonBase`, by reference |
| `text-changed` | `std::string` |

## `button` (Button, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `icon` | icon |  |  |
| `label` | label |  |  |
| `preselect` | preselect |  |  |
| `focus` | focus |  |  |

States: `normal`, `hot`, `pressed`, `preselect`, `selected`, `enabled`, `disabled`, `focused`, `focus-visible`, `open`, `closed`.

Properties beyond every control's: `ContentPadding` (insets), `Font` (font), `Image` (image), `Text` (text).

| Interaction | Payload |
|---|---|
| `expanded-changed` | `std::optional<bool>` |
| `clicked` | `ButtonBase`, by reference |
| `text-changed` | `std::string` |
| `dialog-result-changed` | `DialogResult` |
| `selected-changed` | `bool` |

## `drop-down-button` (DropDownButton, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `icon` | icon |  |  |
| `label` | label |  |  |
| `drop-down` | actuator |  | pressed |
| `disclosure` | disclosure |  |  |
| `preselect` | preselect |  |  |
| `focus` | focus |  |  |

States: `normal`, `hot`, `pressed`, `preselect`, `selected`, `enabled`, `disabled`, `focused`, `focus-visible`, `open`, `closed`.

Properties beyond every control's: `ContentPadding` (insets), `Font` (font), `Image` (image), `Text` (text).

| Interaction | Payload |
|---|---|
| `expanded-changed` | `std::optional<bool>` |
| `clicked` | `ButtonBase`, by reference |
| `text-changed` | `std::string` |
| `dialog-result-changed` | `DialogResult` |
| `selected-changed` | `bool` |
| `drop-down-requested` | `DropDownButton`, by reference |
| `drop-down-close-requested` | `DropDownButton`, by reference |

## `link-label` (LinkLabel, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `label` | label |  |  |
| `underline` | glyph |  |  |
| `focus` | focus |  |  |

States: `normal`, `hot`, `pressed`, `enabled`, `disabled`, `focused`, `focus-visible`, `open`, `closed`. Application-defined: `visited`.

Properties beyond every control's: `ContentPadding` (insets), `Font` (font), `Image` (image), `Text` (text).

| Interaction | Payload |
|---|---|
| `expanded-changed` | `std::optional<bool>` |
| `clicked` | `ButtonBase`, by reference |
| `text-changed` | `std::string` |

## `command-bar` (CommandBar, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `invoked` | `CommandItem` |

## `text-box` (TextBox, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `well` | well |  |  |
| `content` | content |  |  |
| `caret` | caret |  |  |
| `selection` | selection |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`, `read-only`, `invalid`, `pending`.

Properties beyond every control's: `Text` (text).

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `text-changed` | `std::string` |
| `selection-changed` | `TextSelection` |
| `committed` | `std::string` |
| `cancelled` | none |

## `label` (Label, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `label` | label |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: `Font` (font), `ForeColor` (color), `MaximumLines` (unsigned-integer), `Text` (text).

| Interaction | Payload |
|---|---|
| `text-changed` | `std::string` |

## `picture-box` (PictureBox, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `image` | icon |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: `Image` (image), `ImageOpacity` (number), `SizeMode` (enumeration).

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `image-changed` | `ImageId` |

## `progress-bar` (ProgressBar, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `track` | track |  |  |
| `fill` | fill |  |  |
| `activity` | activity |  |  |

States: `normal`, `indeterminate`, `enabled`, `disabled`, `reduced-motion`.

Properties beyond every control's: `Value` (number).

| Interaction | Payload |
|---|---|
| `range-changed` | `double, double` |
| `value-changed` | `double` |
| `scroll` | `RangeScrollEvent` |

## `review-card` (ReviewCard, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `hot`, `pressed`, `captured`, `selected`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `activated` | `Card`, by reference |
| `selected-changed` | `bool` |
| `record-changed` | `ReviewRecord` |

## `list-box` (ListBox, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `well` | well |  |  |
| `content` | content |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Item system `item`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `row` | region |  |  |
| `label` | label |  |  |
| `row-focus` | focus |  |  |

Item states: `selected`, `hot`, `current`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `selection-changed` | `ListSelectionChange` |
| `item-activated` | `std::size_t` |

## `checked-list-box` (CheckedListBox, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `well` | well |  |  |
| `content` | content |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Item system `item`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `row` | region |  |  |
| `label` | label |  |  |
| `row-focus` | focus |  |  |
| `check-well` | well |  |  |
| `check` | indicator |  |  |

Item states: `selected`, `hot`, `current`, `checked`, `indeterminate`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `selection-changed` | `ListSelectionChange` |
| `item-activated` | `std::size_t` |
| `item-checking` | `ItemCheckEvent`, by reference |
| `item-check-state-changed` | `std::size_t, CheckState` |

## `combo-box` (ComboBox, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `well` | well |  |  |
| `content` | content |  |  |
| `divider` | divider |  |  |
| `drop-down` | actuator | `button` |  |
| `disclosure` | disclosure |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`, `open`, `closed`.

Properties beyond every control's: `Items` (collection), `SelectedIndex` (signed-integer), `Text` (text).

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `selected-index-changed` | `std::optional<std::size_t>` |
| `drop-down-changed` | `bool` |
| `items-changed` | none |

## `tree-view` (TreeView, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `content` | content |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Item system `item`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `row` | region |  |  |
| `expander` | disclosure |  |  |
| `icon` | icon |  |  |
| `label` | label |  |  |
| `row-focus` | focus |  |  |

Item states: `selected`, `hot`, `current`, `open`, `closed`, `disabled`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `selection-changed` | `TreeSelectionChange` |
| `expansion-changed` | `TreeExpansionChange` |
| `item-activated` | `std::string` |

## `object-view` (ObjectView, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `header` | face |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Item system `item`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `cell` | region |  |  |
| `glyph` | icon |  |  |
| `name` | label |  |  |
| `secondary` | content |  |  |
| `cell-focus` | focus |  |  |

Item states: `selected`, `hot`, `current`, `disabled`.

Item system `column`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `column-header` | region |  |  |
| `column-label` | label |  |  |
| `sort-glyph` | glyph |  |  |
| `column-divider` | divider |  |  |
| `column-grip` | grip |  |  |
| `column-focus` | focus |  |  |

Item states: `pressed`, `captured`, `current`. Application-defined: `sorted-ascending`, `sorted-descending`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `sort-requested` | `ObjectDetailsSort` |
| `selection-changed` | `ObjectSelectionChange` |
| `item-activated` | `std::string` |
| `context-requested` | `ObjectContextRequest` |

## `correspondence-view` (CorrespondenceView, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Item system `item`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `card` | face |  |  |
| `status-rail` | keyline |  |  |
| `glyph` | icon |  |  |
| `title` | label |  |  |
| `secondary` | content |  |  |
| `metric` | content |  |  |
| `evidence` | well |  |  |
| `card-focus` | focus |  |  |

Item states: `selected`, `hot`, `current`, `open`, `unavailable`, `disabled`. Application-defined: `pinned`, `stale`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `selection-changed` | `CorrespondenceSelectionChange` |
| `pin-changed` | `CorrespondencePinChange` |
| `expansion-changed` | `CorrespondenceExpansionChange` |
| `item-activated` | `std::string` |
| `context-requested` | `ObjectContextRequest` |

## `choice-group` (ChoiceGroup, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `changed` | `int` |
| `activated` | `int` |

## `breadcrumb-trail` (BreadcrumbTrail, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Item system `item`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `segment` | face | `button` |  |
| `segment-label` | label |  |  |
| `segment-cap` | cap |  |  |
| `segment-focus` | focus |  |  |

Item states: `hot`, `current`, `disabled`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `segment-activated` | `std::string` |
| `overflow-activated` | none |
| `edit-committed` | `std::string` |
| `edit-started` | `std::string` |
| `edit-cancelled` | none |
| `edit-completion-requested` | none |

## `check-box` (CheckBox, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `well` | well |  |  |
| `indicator` | indicator |  |  |
| `icon` | icon |  |  |
| `label` | label |  |  |
| `focus` | focus |  |  |

States: `normal`, `hot`, `pressed`, `checked`, `indeterminate`, `enabled`, `disabled`, `focused`, `focus-visible`, `open`, `closed`.

Properties beyond every control's: `Checked` (boolean), `ContentPadding` (insets), `Font` (font), `Image` (image), `Text` (text).

| Interaction | Payload |
|---|---|
| `expanded-changed` | `std::optional<bool>` |
| `clicked` | `ButtonBase`, by reference |
| `text-changed` | `std::string` |
| `check-state-changed` | `CheckState` |
| `checked-changed` | `bool` |

## `radio-button` (RadioButton, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `well` | well |  |  |
| `indicator` | indicator |  |  |
| `icon` | icon |  |  |
| `label` | label |  |  |
| `focus` | focus |  |  |

States: `normal`, `hot`, `pressed`, `checked`, `enabled`, `disabled`, `focused`, `focus-visible`, `open`, `closed`.

Properties beyond every control's: `Checked` (boolean), `ContentPadding` (insets), `Font` (font), `Image` (image), `Text` (text).

| Interaction | Payload |
|---|---|
| `expanded-changed` | `std::optional<bool>` |
| `clicked` | `ButtonBase`, by reference |
| `text-changed` | `std::string` |
| `checked-changed` | `bool` |

## `range-control` (RangeControl, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `focus` | focus |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: `Value` (number).

| Interaction | Payload |
|---|---|
| `range-changed` | `double, double` |
| `value-changed` | `double` |
| `scroll` | `RangeScrollEvent` |

## `track-bar` (TrackBar, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `track` | track |  |  |
| `fill` | fill |  |  |
| `ticks` | glyph |  |  |
| `thumb` | thumb |  | pressed, captured |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: `Value` (number).

| Interaction | Payload |
|---|---|
| `range-changed` | `double, double` |
| `value-changed` | `double` |
| `scroll` | `RangeScrollEvent` |

## `numeric-up-down` (NumericUpDown, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `field` | well | `text-box` |  |
| `increment-arrow` | actuator | `button` |  |
| `decrement-arrow` | actuator | `button` |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`, `invalid`, `pending`.

Properties beyond every control's: `Value` (number).

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `value-changed` | `double` |

## `color-value-editor` (ColorValueEditor, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `swatch` | chip |  |  |
| `field` | well | `text-box` |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`, `invalid`, `pending`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `value-changed` | `Color` |
| `edit-failed` | `PropertyEditorInputError` |

## `flags-value-editor` (FlagsValueEditor, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `content` | content |  |  |
| `divider` | divider |  |  |
| `drop-down` | actuator | `button` |  |
| `disclosure` | disclosure |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`, `open`, `closed`, `invalid`, `pending`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `value-changed` | `PropertyEnumValue` |
| `drop-down-changed` | `bool` |

## `property-grid` (PropertyGrid, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `selected-object-changed` | `Control::Ptr` |
| `property-value-changed` | `PropertyGridValueChange` |
| `edit-failed` | `PropertyGridEditError` |

## `property-list` (PropertyList, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Item system `item`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `group-header` | face |  |  |
| `group-expander` | disclosure |  |  |
| `group-title` | caption |  |  |
| `property-row` | region |  |  |
| `property-name` | label |  |  |
| `property-value` | content |  |  |
| `row-expander` | disclosure |  |  |
| `validation` | validation |  |  |
| `row-divider` | divider |  |  |

Item states: `open`, `closed`, `invalid`, `read-only`, `disabled`. Application-defined: `group-header`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `value-changed` | `PropertyValueChange` |
| `value-committed` | `PropertyValueChange` |
| `group-changed` | `PropertyGroupChange` |
| `row-expansion-changed` | `PropertyRowExpansionChange` |
| `reset-requested` | `PropertyResetRequest` |

## `instrument-rack` (InstrumentRack, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `field-changed` | `InstrumentFieldChange` |
| `field-committed` | `InstrumentFieldChange` |
| `module-toggled` | `InstrumentModuleToggle` |
| `remove-requested` | `InstrumentModuleRequest` |
| `move-requested` | `InstrumentModuleMoveRequest` |

## `date-time-picker` (DateTimePicker, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `content` | content |  |  |
| `drop-down` | actuator | `button` |  |
| `disclosure` | disclosure |  |  |
| `increment-arrow` | actuator | `button` | pressed |
| `decrement-arrow` | actuator | `button` | pressed |
| `well` | well |  | pressed |
| `indicator` | indicator |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`, `open`, `closed`, `checked`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `value-changed` | `DateTimeValue` |
| `checked-changed` | `bool` |
| `drop-down-changed` | `bool` |

## `panel` (Panel, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |

## `group-box` (GroupBox, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `caption` | caption |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |

## `scaled-group-box` (ScaledGroupBox, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `caption` | caption |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |

## `material-panel` (MaterialPanel, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `material-changed` | `SurfaceMaterial` |

## `card` (Card, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `hot`, `pressed`, `captured`, `selected`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `activated` | `Card`, by reference |
| `selected-changed` | `bool` |

## `expandable-sections` (ExpandableSections, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `toggled` | `int, bool` |

## `container-control` (ContainerControl, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `auto-validate-changed` | `AutoValidate` |

## `user-control` (UserControl, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `auto-validate-changed` | `AutoValidate` |
| `loaded` | none |

## `tab-control` (TabControl, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `display` | face |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Item system `item`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `tab` | face |  |  |
| `tab-label` | label |  |  |
| `tab-focus` | focus |  |  |
| `tab-seam` | seam |  |  |

Item states: `selected`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `auto-validate-changed` | `AutoValidate` |
| `selected-index-changed` | `TabSelectionChange` |

## `tab-page` (TabPage, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |

## `flow-layout-panel` (FlowLayoutPanel, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `auto-validate-changed` | `AutoValidate` |

## `table-layout-panel` (TableLayoutPanel, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `cell-grid` | divider |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `auto-validate-changed` | `AutoValidate` |

## `responsive-track-panel` (ResponsiveTrackPanel, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `auto-validate-changed` | `AutoValidate` |

## `scaled-panel` (ScaledPanel, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |

## `split-container` (SplitContainer, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `seam` | seam |  | hot, pressed, captured |
| `grip` | grip |  | open, closed |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`, `hot`, `open`, `closed`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `auto-validate-changed` | `AutoValidate` |
| `splitter-changed` | `SplitChangeEvent` |

## `splitter-panel` (SplitterPanel, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `auto-validate-changed` | `AutoValidate` |

## `command-overflow-panel` (CommandOverflowPanel, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `auto-validate-changed` | `AutoValidate` |

## `master-detail-view` (MasterDetailView, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `auto-validate-changed` | `AutoValidate` |
| `presentation-changed` | `MasterDetailPresentationChange` |

## `scrollable-control` (ScrollableControl, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |

## `scroll-bar` (ScrollBar, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `decrement-arrow` | actuator | `button` | pressed |
| `increment-arrow` | actuator | `button` | pressed |
| `track` | track |  | pressed |
| `thumb` | thumb |  | pressed, captured |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: `Value` (number).

| Interaction | Payload |
|---|---|
| `range-changed` | `double, double` |
| `value-changed` | `double` |
| `scroll` | `RangeScrollEvent` |

## `h-scroll-bar` (HScrollBar, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `decrement-arrow` | actuator | `button` | pressed |
| `increment-arrow` | actuator | `button` | pressed |
| `track` | track |  | pressed |
| `thumb` | thumb |  | pressed, captured |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: `Value` (number).

| Interaction | Payload |
|---|---|
| `range-changed` | `double, double` |
| `value-changed` | `double` |
| `scroll` | `RangeScrollEvent` |

## `v-scroll-bar` (VScrollBar, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `decrement-arrow` | actuator | `button` | pressed |
| `increment-arrow` | actuator | `button` | pressed |
| `track` | track |  | pressed |
| `thumb` | thumb |  | pressed, captured |
| `focus` | focus |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: `Value` (number).

| Interaction | Payload |
|---|---|
| `range-changed` | `double, double` |
| `value-changed` | `double` |
| `scroll` | `RangeScrollEvent` |

## `menu-strip` (MenuStrip, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `divider` | divider |  |  |
| `focus` | focus |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus-visible`.

Item system `item`:

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `menu-item` | face |  |  |
| `menu-label` | label |  |  |
| `menu-focus` | focus |  |  |

Item states: `hot`, `open`, `selected`, `disabled`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `item-invoked` | `MenuStripInvocation` |
| `open-changed` | `std::optional<std::size_t>` |

## `context-menu` (ContextMenu, component)

| Interaction | Payload |
|---|---|
| `item-invoked` | `MenuItemInvocation` |
| `open-changed` | `bool` |

## `command` (Command, component)

| Interaction | Payload |
|---|---|
| `state-changed` | `CommandState` |
| `invoked` | `CommandInvocation` |

## `anchored-popup-layer` (AnchoredPopupLayer, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `viewport` | region |  |  |
| `horizontal-decrement-arrow` | actuator | `button` |  |
| `horizontal-track` | track |  |  |
| `horizontal-thumb` | thumb |  | pressed, captured |
| `horizontal-increment-arrow` | actuator | `button` |  |
| `vertical-decrement-arrow` | actuator | `button` |  |
| `vertical-track` | track |  |  |
| `vertical-thumb` | thumb |  | pressed, captured |
| `vertical-increment-arrow` | actuator | `button` |  |
| `scroll-corner` | face |  |  |
| `focus` | focus |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |

States: `normal`, `pressed`, `captured`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

| Interaction | Payload |
|---|---|
| `scroll` | `ScrollEvent`, by reference |
| `dismiss-requested` | `PopupDismissReason` |

## `tool-tip` (ToolTip, component)

| Interaction | Payload |
|---|---|
| `visibility-changed` | `ToolTipEvent` |

## `drawing-surface` (DrawingSurface, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `focus` | focus |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

Interactions beyond every control's: none.

## `raster-canvas` (RasterCanvas, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `image` | icon |  |  |
| `focus` | focus |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

Interactions beyond every control's: none.

## `live-surface` (LiveSurface, service)

Interactions: none.

## `easing-preview` (EasingPreview, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `focus` | focus |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

Interactions beyond every control's: none.

## `metrics-view` (MetricsView, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `accent-strip` | keyline |  |  |
| `focus` | focus |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

Interactions beyond every control's: none.

## `typography-specimen` (TypographySpecimen, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `edge` | edge |  |  |
| `focus` | focus |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

Interactions beyond every control's: none.

## `visual-inspector-view` (VisualInspectorView, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `face` | face |  |  |
| `header` | face |  |  |
| `accent-strip` | keyline |  |  |
| `focus` | focus |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

Interactions beyond every control's: none.

## `visual-inspector-overlay` (VisualInspectorOverlay, control)

| Part | Role | Reuses | Own states |
|---|---|---|---|
| `region` | region |  |  |
| `highlight` | selection |  |  |
| `focus` | focus |  |  |

States: `normal`, `enabled`, `disabled`, `focused`, `focus-visible`.

Properties beyond every control's: none.

Interactions beyond every control's: none.

## `timer` (Timer, component)

| Interaction | Payload |
|---|---|
| `tick` | none |

## `image-list` (ImageList, component)

| Interaction | Payload |
|---|---|
| `changed` | `ImageListChange` |

## `error-provider` (ErrorProvider, component)

| Interaction | Payload |
|---|---|
| `right-to-left-changed` | `bool` |
| `data-source-changed` | none |
| `data-member-changed` | `std::string` |
| `error-changed` | `ErrorProviderChange` |

## `help-provider` (HelpProvider, component)

| Interaction | Payload |
|---|---|
| `help-requested` | `HelpRequestEvent`, by reference |

## `semantic-feedback` (SemanticFeedback, component)

| Interaction | Payload |
|---|---|
| `emitted` | `SemanticFeedbackRecord` |

## `binding` (Binding, component)

| Interaction | Payload |
|---|---|
| `format` | `BindingConvertEvent`, by reference |
| `parse` | `BindingConvertEvent`, by reference |
| `binding-complete` | `BindingCompleteEvent`, by reference |

## `binding-source` (BindingSource, component)

| Interaction | Payload |
|---|---|
| `list-changed` | `BindingListChange` |
| `current-changed` | none |
| `current-item-changed` | none |
| `position-changed` | `std::ptrdiff_t` |
| `data-error` | `std::string` |
| `data-source-changed` | none |
| `data-member-changed` | `std::string` |
| `binding-complete` | `BindingCompleteEvent`, by reference |
| `disposed-event` | none |

## `binding-context` (BindingContext, component)

| Interaction | Payload |
|---|---|
| `collection-changed` | `BindingContextChange` |
