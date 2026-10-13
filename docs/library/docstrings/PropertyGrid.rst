An inspector that lists and edits the registered properties of the selected controls.

A property grid reads the property descriptors of its `selected_object`, or
of every control in `selected_objects`, and shows them as rows grouped by
category or in one alphabetical list. Each writable property gets an editor
chosen from its kind: a check box, a text box, a drop-down of choices, a
`NumericUpDown`, a `FlagsValueEditor` or a `ColorValueEditor`. Compound
values such as bounds and padding, objects and collections expand into rows
for their parts. Edits are converted, validated and applied through the
control's own setters, and a row refreshes when the control reports a
change. Use a `PropertyList` instead to show settings that are not
properties of a control.

Parameters
----------
stable_id
    Identity of the control, unique within its window. Logs, automation and
    semantic actions address the control by this ID. The rows are shown by
    a `PropertyList` named ``<stable_id>.list``.

Properties
----------
selected_object
    The first selected control, or null when nothing is selected. Setting it
    is the same as setting `selected_objects` to that one control, or to
    none when it is null.
selected_objects
    The controls being inspected. With more than one, only the properties
    that every control has, with the same schema and value shape, are
    shown; a property is writable only if it is writable on all of them,
    and the rows show the first control's values. The grid does not keep
    the controls alive: when one of them is destroyed or disposed,
    `selected_objects` is empty and edits are refused until the selection
    is set again. Setting it throws `std::invalid_argument` for a null
    pointer, a disposed control or a control listed twice. Setting the same
    selection again only refreshes the rows.
property_sort
    `PropertySort::categorized`, the default, groups the rows by category
    with the categories and the names within them in alphabetical order; a
    property without a category goes under "Misc".
    `PropertySort::alphabetical` shows every row in one group titled
    "PROPERTIES". Another value throws `std::invalid_argument`. Changing it
    rebuilds the rows.
property_list
    The `PropertyList` that shows the rows.
converter_registry
    The converters that turn values into text and typed text into values.
    The grid starts with `PropertyValueConverterRegistry::create_default`.
    Setting it to null throws `std::invalid_argument`; setting another
    registry rebuilds the rows.
editor_registry
    The factories that create editors for particular kinds of value. The
    default from `PropertyEditorRegistry::create_default` creates a
    `NumericUpDown` for numbers, a `FlagsValueEditor` for flags enumerations
    and a `ColorValueEditor` for colours. Setting it to null throws
    `std::invalid_argument`; setting another registry rebuilds the rows.
last_error
    The most recent rejected edit, or no value. A successful edit and a
    rebuild clear it.

Events
------
selected_object_changed
    Raised after the selection changes and the rows are rebuilt. It carries
    the first selected control, or null.
property_value_changed
    Raised after an edit made through the grid changes a value, or after a
    reset. It is also raised when the first selected control reports a
    change to a property that publishes change notifications. It carries a
    `PropertyGridValueChange` with the property path as edited, such as
    ``Text`` or ``Bounds.X``, the previous and current values, the value's
    origin and whether it was a reset.
edit_failed
    Raised when an edit is rejected: the property does not exist or is
    read-only, the text does not convert, the control's setter throws, or an
    editor factory fails. It carries a `PropertyGridEditError` with the
    property path, the attempted value and a message. The grid also shows
    the message under the property's row and keeps it in `last_error`.

Methods
-------
refresh_properties
    Rebuilds every row from the selection's current descriptors and values.
editor
    The editor of a property, or null when the property has none. Names are
    matched without regard to case. A part of a compound value or a member
    of an object is named with a dot, as in ``Bounds.X``, and an item of a
    collection with its index, as in ``Items[0]``; the other methods that
    take a property name accept the same paths.
reset_button
    The Reset button of a resettable property, or null. It is enabled while
    the control reports the property as changed from its default.
selected_descriptor
    The descriptor the grid uses for a property or property part, or no
    value when it is not shown.
selected_origin
    Where the first selected control's value for a property comes from, such
    as a local assignment, a default or inheritance, or no value when it is
    not shown.
set_property_expanded
    Shows or hides the rows for the parts of a compound property. Returns
    false when the property is not shown or has no parts. The state is kept
    when the rows are rebuilt.
property_expanded
    Whether the parts of a compound property are shown, or no value when the
    property is not shown or has no parts.
try_set_property_value
    Converts a value to the property's declared kind and applies it to every
    selected control. Returns true on success. On failure it returns false,
    raises `edit_failed` and leaves every control as it was; with nothing
    selected it returns false without raising anything.
try_set_property_text
    Like `try_set_property_value`, but parses text with the converter
    registry first.
activate_property_editor
    Presses a property's editor, as a click would; for example, it toggles a
    check box. Returns false when there is no enabled editor or it does not
    respond to a press.
reset_property
    Resets a top-level property on every selected control through the
    control's own reset. Returns false, raising `edit_failed`, when the
    property is not resettable.
insert_collection_item
    Inserts an item into a collection property at an index and applies the
    new collection. Returns false, raising `edit_failed`, for a property that
    is not a collection, an index past the end, or a full collection.
remove_collection_item
    Removes the item at an index from a collection property. Returns false,
    raising `edit_failed`, for an invalid index.
move_collection_item
    Moves an item of a collection property from one index to another.
    Returns false, raising `edit_failed`, for an invalid index.
initialize_control_tree
    Creates the property list. `make_control` calls it; calling it again
    does nothing.

See Also
--------
PropertyList : A settings sheet whose rows the application describes.
PropertyEditorRegistry : Chooses the editor control for each kind of value.
PropertyValueConverterRegistry : Converts property values to and from text.
NumericUpDown : The default editor for numeric properties.
FlagsValueEditor : The default editor for flags properties.
ColorValueEditor : The default editor for colour properties.

Notes
-----
Only properties whose descriptors are readable and browsable are shown. Each
row's description adds the property's origin, for example "Origin: local",
and "Reset available" when the property can be reset.

Editors follow the property's kind. A writable Boolean gets a check box, and
text and integers get a text box. A non-flags enumeration, or a property
with an exclusive list of standard values, gets a drop-down. The editor
registry then supplies editors for numbers, flags and colours. Points,
sizes, rectangles, insets, colours and fonts expand into a row for each
part, such as ``Bounds.X`` or ``Padding.Left``, each with its own editor;
the property's own row shows a summary, or a `ColorValueEditor` for a
colour. Objects and collections expand into rows for their members and
items. Read-only properties are shown as text.

A text box commits when the user presses Enter, and a check box or a
drop-down commits as soon as it changes. A `NumericUpDown` commits each
complete number as the user types it, and `FlagsValueEditor` and
`ColorValueEditor` commit their own edits. A committed value is converted
to the property's kind, checked against every selected control, and only
then applied; if a setter throws, the controls already changed are restored
and `edit_failed` is raised. After a successful edit the row's validation
message is cleared.

The grid itself is not published to accessibility; its property list is,
with the role `property_grid` and the name "<control id> properties".

Examples
--------
Inspect a check box and change two of its properties, one from text and one
from a typed value:

.. code-block:: cpp

   std::shared_ptr<gf::CheckBox> wrap =
       gf::make_control<gf::CheckBox>(gf::StableId("options.wrap"), "Wrap lines");
   std::shared_ptr<gf::PropertyGrid> inspector =
       gf::make_control<gf::PropertyGrid>(gf::StableId("options.inspector"));
   (*inspector).set_selected_object(wrap);
   (*inspector).try_set_property_text("text", "Wrap long lines");
   (*inspector).try_set_property_value("checked", gf::BindingValue{true});
   // the check box now reads "Wrap long lines" and is checked

Follow successful edits and rejected ones from an owner:

.. code-block:: cpp

   struct Inspector final : gf::Component {
       std::shared_ptr<gf::CheckBox> target =
           gf::make_control<gf::CheckBox>(gf::StableId("demo.target"), "Inspectable");
       std::shared_ptr<gf::PropertyGrid> grid =
           gf::make_control<gf::PropertyGrid>(gf::StableId("demo.grid"));
       std::string changed;
       std::string rejected;

       Inspector() {
           (*grid).set_selected_object(target);
           gf::on((*grid).property_value_changed(), *this, &Inspector::on_changed);
           gf::on((*grid).edit_failed(), *this, &Inspector::on_failed);
       }
       void on_changed(const gf::PropertyGridValueChange& change) {
           changed = change.property_name;
       }
       void on_failed(const gf::PropertyGridEditError& error) {
           rejected = error.property_name;
       }
   };

   void example() {
       Inspector inspector;
       (*inspector.grid).try_set_property_value("checked", gf::BindingValue{true});
       // changed is "checked"
       (*inspector.grid).try_set_property_value(
           "TabIndex", gf::BindingValue{std::string("-1")});
       // rejected is "TabIndex"; the tab index is unchanged
   }
