A list of records with a current position, to which controls are bound.

A binding source holds the data that a form edits: a list of `BindingRecord`
values, each with a stable ID, named fields, an editable flag and optional
error messages, and a current position. Controls bound to it with `Binding`
show the fields of the current record and write their changes back; moving
the position moves every bound control to another record at once. The source
offers navigation, insertion and removal, edit transactions with
cancellation, and the events a list view or navigator needs. Use a `Value`
instead to share a single scalar between controls.

Parameters
----------
window
    The window whose UI thread the source belongs to. Construct the source on
    that thread, with `std::make_shared`, since bindings hold it by
    `std::shared_ptr`.

Properties
----------
records
    The records, in order. `set_records(records, metadata_changed = false)`
    replaces them all. Every record needs a nonempty stable ID, unique in the
    list, and its field and error names must not collide after trimming and
    ASCII case folding; otherwise `std::invalid_argument` is thrown and
    nothing changes. The record with the previous current stable ID stays
    current; otherwise the first record becomes current. It raises
    `data_source_changed`, then `list_changed` with `reset`, then the
    position events if the current record changed.
count
    The number of records.
position : default ``-1``
    The index of the current record, or -1 when there is none. Setting it
    first ends any edit in progress, then raises `list_changed` with
    `BindingListChangeKind::position_changed`, `position_changed`,
    `current_changed` and `current_item_changed`, in that order, after the
    bound controls have been updated. It returns false, raising nothing,
    when the position is unchanged, and throws `std::out_of_range` outside
    -1 to `count` - 1.
current
    A pointer to the current record, or null when there is none. It is
    invalidated by any change to the list.
bindings
    The bindings currently connected to the source.
allow_edit : default ``true``
    Whether fields may be changed. While false, `set_current_field` and
    `begin_edit` return false, and turning it off ends any edit in progress.
allow_new : default ``true``
    Whether records may be added; while false, `add` and `insert` throw
    `std::logic_error`.
allow_remove : default ``true``
    Whether records may be removed; while false, `remove_at`,
    `remove_current` and `clear` on a nonempty list throw `std::logic_error`.
binding_suspended : default ``false``
    Whether changes are being withheld from bound controls; see
    `suspend_binding`.
raise_list_changed_events : default ``true``
    Whether `list_changed` is raised. Bound controls are updated either way.
data_member : default ``""``
    A name stored for consumers of the source. Changing it raises
    `data_member_changed` and refreshes the bindings; the source itself
    does not use it to look up fields.
currency_manager
    The `CurrencyManager` of the source: the same records and position
    behind the `BindingManagerBase` interface, as returned by
    `BindingContext::manager`.
snapshot
    A `BindingSourceSnapshot` with the count, position and current stable ID,
    a revision counter, counts of resets, field changes, position changes and
    suspended mutations, and whether binding is suspended, an edit is in
    progress and `list_changed` is raised.

Events
------
list_changed
    Raised after every change to the list, with a `BindingListChange` giving
    its `kind`: `reset` after `set_records`, `clear`, `resume_binding` or a
    reset request, `item_added` and `item_removed` with the index and stable ID,
    `item_changed` with the index, stable ID and, for a field change, the
    field name, and `position_changed`. It is raised after the bound
    controls have been updated, and is not raised while
    `raise_list_changed_events` is false.
position_changed
    Raised with the current position whenever the position or the current
    record changes: when `position` is set or moved, and when `set_records`,
    `insert`, `remove_at` or `clear` shift or replace the current record.
    The index may be unchanged, when removing the current record makes the
    next one current.
current_changed
    Raised just after each `position_changed`.
current_item_changed
    Raised just after each `current_changed`, and also when a field of the
    current record changes, when an edit of it is cancelled and when it is
    reset with `reset_item`.
data_error
    Raised with the error text when a binding fails to transfer a value.
data_source_changed
    Raised by `set_records`, before its `list_changed`.
data_member_changed
    Raised after `data_member` changes, with the new value.
binding_complete
    Raised after a bound transfer completes, for bindings whose options have
    `formatting_enabled`, just after the binding's own `binding_complete`.
disposed_event
    Raised when the source is disposed, before its events are disconnected.
    Bindings and binding contexts use it to let go of the source.

Methods
-------
move_first
    Makes the first record current. Returns false when the list is empty or
    the first record is already current.
move_last
    Makes the last record current. Returns false when the list is empty or
    the last record is already current.
move_next
    Makes the next record current. Returns false at the end of the list.
move_previous
    Makes the previous record current. Returns false at the start of the
    list.
current_field
    `current_field(name)` returns the value of a field of the current record,
    or an empty optional when there is no current record or no such field.
current_error
    `current_error(name)` returns the current record's error message for a
    field, or for the record as a whole when the name is empty, or an empty
    string.
set_current_field
    `set_current_field(name, value)` changes a field of the current record,
    adding it when it does not exist, and raises `list_changed` with
    `item_changed` and then `current_item_changed`. It begins an edit
    first, so `cancel_edit` can undo it. Returns false, changing nothing,
    when there is no current record, editing is not allowed, the record is
    not editable or the value is unchanged.
add
    Appends a record and returns its index; see `insert`.
insert
    `insert(index, record)` inserts a record and returns its index. When
    there was no current record, the first record becomes current; otherwise
    the current record stays current, and its position moves when the record
    is inserted before it. Throws `std::out_of_range` past the end, and
    `std::invalid_argument` for a duplicate stable ID or an invalid record.
remove_at
    Removes the record at an index and returns true, or returns false past
    the end. When the current record is removed, the record that takes its
    index becomes current, or the new last record.
remove_current
    Removes the current record. Returns false when there is none.
clear
    Removes every record and raises `list_changed` with `reset`. The
    position becomes -1.
find
    `find(name, value)` returns the index of the first record whose field
    equals the value, or an empty optional.
begin_edit
    Takes a snapshot of the current record so that `cancel_edit` can restore
    it. Returns false when there is no current record, editing is not allowed
    or the record is not editable. Calling it again during the same edit
    keeps the first snapshot.
cancel_edit
    Restores the record from the snapshot taken by `begin_edit` and raises
    `item_changed`, and `current_item_changed` when it is the current
    record. Does nothing when no edit is in progress.
end_edit
    Keeps the changes and discards the snapshot.
suspend_binding
    Withholds changes from bound controls. The list still changes, and
    `list_changed` is still raised; the bound controls keep their values.
resume_binding
    Ends suspension; suspension does not nest. When anything changed
    meanwhile, the bound controls are updated once and `list_changed` is
    raised with `reset`.
reset_bindings
    `reset_bindings(metadata_changed = false)` raises a `reset` so that every
    bound control shows its field again.
reset_current_item
    `reset_item` for the current record.
reset_item
    `reset_item(index)` raises `item_changed` for a record, so that controls
    bound to it show it again, and returns false past the end.

See Also
--------
Binding : Connects one control property to one field of a source.
BindingContext : The currency managers of a window's binding sources.
ErrorProvider : Shows the errors that records carry for bound fields.

Notes
-----
Field and error names are trimmed and compared ignoring ASCII case:
"Name", "name" and " NAME " are the same field. An empty field name passed to
`current_field`, `set_current_field` or `find` throws
`std::invalid_argument`.

Edits are snapshots, not transactions with the bound controls: moving to
another record ends the edit and keeps its changes, as does `end_edit`.
Changing `allow_edit`, `allow_new` or `allow_remove` raises a `reset` with
`metadata_changed` set, so that bound views can update their commands.

Use the source only on its window's UI thread. Its mutating members throw
`std::logic_error` from another thread, after disposal and after the window
has shut down. Disposal raises `disposed_event`, deactivates the bindings and
empties the source; destroying an undisposed source disposes it.

Examples
--------
Fill a source and move through it:

.. code-block:: cpp

   std::shared_ptr<gf::Control> root = gf::make_control<gf::Control>(gf::StableId("app"));
   gf::Window window(root, gf::Size{200.0, 100.0});
   std::shared_ptr<gf::BindingSource> cities = std::make_shared<gf::BindingSource>(window);

   gf::BindingRecord::FieldMap oslo;
   oslo["name"] = std::string("Oslo");
   gf::BindingRecord::FieldMap lima;
   lima["name"] = std::string("Lima");
   (*cities).set_records({gf::BindingRecord("city.oslo", oslo),
                          gf::BindingRecord("city.lima", lima)});
   // position() is 0, the first record is current
   static_cast<void>((*cities).move_next());
   std::optional<gf::BindingValue> name = (*cities).current_field("Name");
   // name holds std::string("Lima")

Follow the current position with an owned handler, and undo an edit:

.. code-block:: cpp

   struct Navigator final : gf::Component {
       std::ptrdiff_t shown = -1;
       void on_position(std::ptrdiff_t position) { shown = position; }
   };

   void example() {
       std::shared_ptr<gf::Control> root = gf::make_control<gf::Control>(gf::StableId("app"));
       gf::Window window(root, gf::Size{200.0, 100.0});
       std::shared_ptr<gf::BindingSource> notes = std::make_shared<gf::BindingSource>(window);
       Navigator navigator;
       gf::on((*notes).position_changed(), navigator, &Navigator::on_position);

       gf::BindingRecord::FieldMap fields;
       fields["title"] = std::string("Draft");
       static_cast<void>((*notes).add(gf::BindingRecord("note.1", fields)));
       // navigator.shown is 0: the first record became current
       static_cast<void>((*notes).set_current_field("title", std::string("Final")));
       (*notes).cancel_edit();
       // the title is "Draft" again
   }
