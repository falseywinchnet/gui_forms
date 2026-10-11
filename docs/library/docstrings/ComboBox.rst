A field that shows one selected item and drops down a list of the others.

A combo box takes the space of one line and opens a pop-up list when the
user clicks it or presses F4, Alt+Down or Space. Choosing a row in the
list selects it and closes the list; Up, Down, Home and End change the
selection without opening it. The field cannot be typed into: it always
shows an item from `items`, or `placeholder_text` when nothing is
selected. Use a `ListBox` to show all choices at once or to allow several,
and a `ChoiceGroup` for a few choices that should all stay visible.

Parameters
----------
stable_id
    Identity of the control, unique within its window. Logs, automation and
    semantic actions address the control by this ID. The pop-up list is
    named after it, ``<stable_id>.popup.list``, and its rows
    ``<stable_id>.popup.list.item.<index>``.

Properties
----------
items
    The choices, in display order. Empty by default. Replacing them with
    `set_items` raises `items_changed`, unless the new items are equal to
    the current ones. A selected index that no longer exists is cleared
    first, raising `selected_index_changed`; a selected index that still
    exists is kept, even if a different item now has it. Every item must
    be valid UTF-8, or `std::invalid_argument` is thrown. The same
    collection is also available to bindings as the ``Items`` property.
selected_index : default ``std::nullopt``
    The index of the selected item, or no value when nothing is selected.
    Setting an index outside the items throws `std::out_of_range`. Also
    available to bindings as ``SelectedIndex``, with -1 for no selection.
selected_text
    The text of the selected item, or an empty view when nothing is
    selected. Also available to bindings as ``Text``; setting ``Text``
    selects the first item with that text, or clears the selection.
placeholder_text : default ``""``
    Text shown in a muted colour when nothing is selected. It is also the
    accessible description when the control has none of its own. It must
    be valid UTF-8, or `std::invalid_argument` is thrown.
dropped_down : default ``false``
    Whether the pop-up list is open. Opening has no effect, and leaves the
    property false, while the control is not attached to a window or has
    no items.
maximum_drop_down_items : default ``8``
    The most rows the open list shows before it scrolls and shows a scroll
    bar; a short window can make the list shorter still. Values outside 1
    to 64 throw `std::invalid_argument`.
    Changing it while the list is open closes and reopens the list.
drop_down_width : default ``0.0``
    Width of the pop-up list in logical pixels. Zero makes the list as wide
    as the field. The list never exceeds the window width. Values outside 0
    to 4096, or not finite, throw `std::out_of_range`.
font : default ``FontSpec{FontRole::content, 12.0, 400, false}``
    The font of the field and of the pop-up rows. An invalid specification
    throws `std::invalid_argument`.

Events
------
selected_index_changed
    Raised after `selected_index` changes, by input, by the setter, or by
    `set_items` clearing an index that no longer exists. It carries the new
    index, or no value. Setting the current index again raises nothing.
drop_down_changed
    Raised with `true` after the pop-up list opens and with `false` after
    it closes, whichever way it closed.
items_changed
    Raised after `set_items` or `add_item` changes the items, after any
    `selected_index_changed` the change caused.

Methods
-------
add_item
    Appends one choice, raising `items_changed`. The item must be valid
    UTF-8, or `std::invalid_argument` is thrown.
on_pointer
    Opens the pop-up list on a primary press. The window calls it.
on_key
    Handles the keyboard input described under Notes. The window calls it.
semantic_descriptor
    Publishes the role `combo_box`, with the selected text as value, the
    `expanded` state while the list is open, and the action `expand` or
    `collapse`.
on_semantic_action
    Opens the list on `expand` and closes it on `collapse`; `focus` focuses
    the field. Other actions are not handled.

See Also
--------
ListBox : All choices visible at once, with optional multiple selection.
ChoiceGroup : A row of exclusive choices bound to a `Value`.

Notes
-----
A primary press on the field opens the list. With the field focused, F4,
Alt+Down and Space open the list, and Up, Down, Home and End select the
previous, next, first or last item directly without opening it. When
nothing is selected Up and Down count from the first item, so Down selects
the second. There is no type-ahead search.

The open list is a `ListBox` in a pop-up layer over the window, and it
takes the keyboard focus. Moving through it with the arrow keys selects
each row as it is reached, raising `selected_index_changed` each time.
Clicking a row, or pressing Enter, selects it and closes the list; a
semantic press on a row does the same. Escape or a press outside the list
closes it without restoring the earlier selection. Closing returns the
focus to the field. The list also closes when the control is hidden,
disabled or removed from the window.

Opening the list and clicking a row therefore raises `drop_down_changed`
with `true`, then `selected_index_changed` if the row was not already
selected, then `drop_down_changed` with `false`.

The control publishes the accessibility role `combo_box`; the open list
publishes the role `list` with `list_item` rows.

Examples
--------
Offer three choices with a placeholder, and select one:

.. code-block:: cpp

   std::shared_ptr<gf::ComboBox> density =
       gf::make_control<gf::ComboBox>(gf::StableId("view.density"));
   (*density).set_items({"Compact", "Comfortable", "Spacious"});
   (*density).set_placeholder_text("Choose a density");
   (*density).set_selected_index(1U);
   // (*density).selected_text() is now "Comfortable"

React to the selection with an owned handler. Shrinking the items clears a
selection that no longer exists, and the handler sees that too:

.. code-block:: cpp

   struct DensityField final : gf::Component {
       std::shared_ptr<gf::ComboBox> combo =
           gf::make_control<gf::ComboBox>(gf::StableId("view.density"));
       std::string density;

       DensityField() {
           (*combo).set_items({"Compact", "Comfortable", "Spacious"});
           gf::on((*combo).selected_index_changed(), *this, &DensityField::on_density);
       }
       void on_density(std::optional<std::size_t> index) {
           density = index ? std::string((*combo).selected_text()) : std::string();
       }
   };

   void example() {
       DensityField field;
       (*field.combo).set_selected_index(2U);                 // field.density is "Spacious"
       (*field.combo).set_items({"Compact", "Comfortable"});  // and now ""
   }
