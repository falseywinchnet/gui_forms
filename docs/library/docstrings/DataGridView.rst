A grid of typed columns over rows the grid owns, virtual rows or a binding source.

A data grid view shows rows of cells under a row of column headers, with an
optional column of row headers at the left. Each `DataGridViewColumn` is a
text, check box, combo box or button column with a name, header text, width,
read-only flag and alignment. The rows come from one of three sources: unbound
rows the grid owns (`DataGridViewRow` records with stable IDs and
`BindingValue` cells), virtual rows whose values an application
`DataGridViewCellValueProvider` supplies as they are shown, or the records of
a `BindingSource`. Use it for tabular data the user reads, selects and edits
in place; use an `ObjectView` for a list of objects with icons.

Parameters
----------
stable_id
    Identity of the grid, unique within its window. The editor is created as
    ``<stable_id>.editor``, the scroll bars as ``<stable_id>.vertical_scroll``
    and ``<stable_id>.horizontal_scroll``, and combo box choices are shown by
    a `ContextMenu` named ``<stable_id>.choices``.

Properties
----------
columns
    The columns, in display order. `set_columns` replaces them, `add_column`
    appends one and `remove_column_at` removes one. A column name must be
    unique, not empty and without ``.``; its width finite and at least its
    `minimum_width`; and only a combo box column may have `items`. Anything
    else throws `std::invalid_argument`. Replacing or removing columns ends
    any edit and clears the selection and current cell.
row_source : default unbound
    Where the rows come from: `DataGridViewRowSource::unbound`,
    `virtual_rows` or `binding_source`.
row_count
    The number of rows. `set_row_count` grows unbound rows with generated
    rows or cuts them from the end, and sets the count of virtual rows.
data_source
    The `BindingSource` whose records are the rows, or none.
    `set_data_source` shows a source's records as rows, or returns to empty
    unbound rows for none.
column_headers_visible : default ``true``
    Whether the column header row shows.
row_headers_visible : default ``true``
    Whether the row header column shows.
column_headers_height : default ``23``
    The height of the column header row, from 4 to 256.
row_headers_width : default ``41``
    The width of the row header column, from 4 to 1024.
row_height : default ``22``
    The height of every row, from 4 to 256.
font
    The font of cell and header text.
read_only : default ``false``
    Whether no cell of the grid edits, toggles or chooses.
allow_user_to_resize_columns : default ``true``
    Whether dragging a column header's divider resizes the column.
first_displayed_row
    The first row shown under the column headers. `set_first_displayed_row`
    scrolls to a row; layout keeps the last rows filling the grid.
horizontal_scroll_offset
    How far the cells are scrolled left, in logical units at text scale 1.
displayed_row_end
    One past the last row shown, partly shown rows included.
vertical_scroll_bar
    The child `VScrollBar` that scrolls by rows.
horizontal_scroll_bar
    The child `HScrollBar` that scrolls by logical units.
selection_mode : default cell_select
    `DataGridViewSelectionMode::cell_select` selects cells,
    `full_row_select` whole rows. Changing it selects the current cell (or
    row) alone.
multi_select : default ``true``
    Whether Ctrl and Shift add to the selection.
current_cell
    The cell keyboard input acts on, or none. `set_current_cell` moves it,
    committing any edit first, and selects it alone; it returns false and
    changes nothing when the edit does not commit.
selected_cells
    The selected cells by row, then column; in full-row mode every cell of
    every selected row.
selected_rows
    The selected rows: in full-row mode the selected rows, in cell mode the
    rows whose every cell is selected.
is_current_cell_in_edit_mode
    Whether a text cell is in edit or a combo box cell's choices are open.
editing_control
    The `TextBox` a text cell edits in. It is laid out over the current cell
    only during an edit.
drop_down_open
    Whether a combo box cell's choices are open.

Events
------
selection_changed
    Raised once after an operation changes which cells or rows are selected.
current_cell_changed
    Raised after the current cell moves, with the new cell or none.
cell_begin_edit
    Raised before an edit begins, by F2, typing, a double click, a combo box
    cell opening, or `begin_edit`. Setting ``cancel`` keeps the cell out of
    edit.
cell_end_edit
    Raised when an edit ends, committed or discarded.
cell_validating
    Raised before an edited text cell commits, with the text as typed.
    Setting ``cancel`` refuses it: the cell stays in edit and publishes the
    invalid state.
cell_value_changed
    Raised after a cell's value changes: a committed edit, a toggled check
    box, a chosen item or `set_cell_value`.
cell_content_click
    Raised when the user clicks a button cell or toggles a check box cell.
column_header_click
    Raised when a column header is clicked, with the column's index.
column_width_changed
    Raised after a column's width changes, with the column's index.

Methods
-------
column_index
    The position of the column with a name, or none.
set_column_width
    Sets a column's width; an unknown column throws `std::out_of_range` and a
    width below the column's minimum `std::invalid_argument`.
set_column_sort_glyph
    Shows an ascending or descending sort glyph on a column header, or none.
set_column_read_only
    Makes a column's cells read-only, ending an edit in that column.
add_row
    Appends an unbound row and returns its index. An empty stable ID is
    replaced by a generated one (``row.1``, ``row.2``, ...); a repeated ID,
    invalid UTF-8 or more values than columns throws `std::invalid_argument`.
    Virtual and bound rows refuse it with `std::logic_error`, as they do
    `insert_row`, `set_rows` and `remove_row_at`.
insert_row
    Inserts an unbound row before an index. The current cell and the
    selection follow their rows.
set_rows
    Replaces the unbound rows, ending any edit and clearing the selection.
remove_row_at
    Removes an unbound row. When it held the current cell, the row that takes
    its place becomes current.
clear_rows
    Removes every row of any source and returns to unbound rows.
row_stable_id
    A row's stable ID: an unbound row's own, the provider's for a virtual row,
    the record's for a bound row.
row_index
    The row with a stable ID, or none. For virtual rows only the displayed
    rows and the current row are searched.
cell_value
    A cell's value. A cell past the rows or columns throws
    `std::out_of_range`.
cell_text
    The text a cell shows: numbers in their shortest exact form, and a button
    column's `button_text` for an empty button cell.
set_cell_value
    Stores a value from the program, read-only or not, and raises
    `cell_value_changed` when it differs. Returns false when a provider or
    binding source refuses it.
sort
    Orders unbound rows stably by a column: empty values first, then
    booleans, numbers and text. The column shows the sort glyph and the
    others none; the current cell and the selection follow their rows.
    Returns false for virtual or bound rows, or when an edit in progress does
    not commit.
set_virtual_rows
    Switches to virtual rows asked of a provider, discarding unbound rows. A
    null provider throws `std::invalid_argument`.
refresh_rows
    Repaints after the provider's values changed outside the grid.
is_cell_selected
    Whether a cell is selected; in full-row mode, whether its row is.
is_row_selected
    Whether a row is selected; in cell mode, whether all its cells are.
set_cell_selected
    Selects or deselects a cell (its row in full-row mode). Without
    `multi_select`, selecting clears the rest.
set_row_selected
    Selects or deselects a row, or all its cells in cell mode.
clear_selection
    Deselects everything.
select_all
    Selects every cell, or every row; nothing without `multi_select`.
begin_edit
    Puts the current text cell in edit, its text selected or the caret at
    its end, or opens the current combo box cell's choices. Returns false
    for no current cell, a read-only cell, a check box or button cell, or a
    cancelled `cell_begin_edit`.
end_edit
    Commits the edit in progress: `cell_validating`, conversion to the
    column's `value_kind`, then the value. Returns false, leaving the cell in
    edit and invalid, when any step refuses.
cancel_edit
    Discards the edit in progress.
close_drop_down
    Closes a combo box cell's choices, ending its edit.
initialize_control_tree
    Creates the editor and the scroll bars. `make_control` calls it; do not
    call it yourself.

See Also
--------
ObjectView : A virtual list of objects with icon and details views.
BindingSource : Records with a current position, shared by bound controls.
TextBox : The editor text cells edit in.
ContextMenu : The popup that shows a combo box cell's choices.

Notes
-----
Cells are records, not child controls. Only the rows and columns in view are
realized: the grid publishes them as three item systems, ``row`` (the row
header with ``row_header``, ``row_header_label``, ``row_glyph`` and
``row_rule``), ``cell`` (``cell``, ``cell_selection``, ``cell_content``,
``cell_check_well``, ``cell_check_indicator``, ``cell_button_face``,
``cell_button_label``, ``cell_drop_down``, ``cell_drop_down_glyph``,
``cell_vertical_rule``, ``cell_horizontal_rule`` and ``cell_focus``) and
``column`` (``column_header``, ``column_label``, ``sort_glyph``,
``column_divider`` and ``column_grip``), and paints every part through the
by-role paint path (`paint_part`). Cells take the states selected, current,
read_only, invalid, checked, pressed, open and focus_visible; rows selected,
current and read_only; column headers hot, pressed and captured, and the
application-defined ``sorted_ascending`` and ``sorted_descending``. The
column header row and the row header column stay in place while the cells
scroll beneath them.

A virtual grid asks its provider only for the cells it shows, the current
cell, and cells the program or accessibility reads; a million rows cost no
more to show than ten. A bound grid shows the field each column's
`data_property_name` names. Its current row follows the source's position
and moves it, and a committed edit moves the position to its row and sets
the field; a record the source does not allow to be edited is read-only.

Keys: the arrows move the current cell, Shift extends the selection from the
anchor and Ctrl jumps to the edge; Tab and Shift+Tab move across and wrap to
the next or previous row, leaving the grid at either end; Home and End go to
the row's first and last cell (the first and last row in full-row mode),
Ctrl+Home and Ctrl+End to the grid's; Page Up and Page Down move by the rows
in view; Enter moves down; Ctrl+A selects all. F2, typing or a double click
edits a text cell; Enter, Tab, an arrow up or down, a click elsewhere or
leaving the grid commits, and Escape discards. When focus leaves for another
control and the text does not commit, the edit is discarded. Space toggles a
check box cell and clicks a button cell; F4 or Alt+Down opens a combo box
cell's choices. `cell_validating` is raised only for an edit being committed.

The vocabulary has no grid, table or cell role, so the grid publishes the
accessibility role `list`, with the visible column headers as `button`
children and each visible row as a `list_item` holding its cells: a
`text_box`, `check_box`, `combo_box` or `button` by column kind. A cell's
stable ID is its row's ID, ``.`` and its column's name. Cells take
`SemanticAction::set_value` (text through the same validation and conversion
as typing), `press` (toggle or click) and `expand` (open the choices).

Not supported yet: cell styles and their inheritance, image and link
columns, frozen rows and columns beyond the headers, clipboard copy, sort
modes (the application sorts, or calls `sort` for unbound rows), automatic
column and row sizing, row height resizing, the new-row and row-deletion
editing interface, error icons and cell tool tips.

Examples
--------
A grid of stations with a check box and a combo box column, sorted by name:

.. code-block:: cpp

   std::shared_ptr<gf::DataGridView> grid =
       gf::make_control<gf::DataGridView>(gf::StableId("radio.stations"));
   (*grid).set_columns({
       {.name = "name", .header_text = "Station", .width = 160.0},
       {.name = "favourite", .header_text = "Favourite", .kind = gf::DataGridViewColumnKind::check_box,
        .width = 70.0},
       {.name = "mode", .header_text = "Mode", .kind = gf::DataGridViewColumnKind::combo_box,
        .width = 90.0, .items = {"AM", "FM", "USB"}},
   });
   (*grid).add_row({.stable_id = "wqxr", .cells = {gf::BindingValue{std::string("WQXR")},
                                                   gf::BindingValue{true},
                                                   gf::BindingValue{std::string("FM")}}});
   (*grid).add_row({.stable_id = "bbc4", .cells = {gf::BindingValue{std::string("BBC Radio 4")},
                                                   gf::BindingValue{false},
                                                   gf::BindingValue{std::string("AM")}}});
   (*grid).sort(0U, gf::DataGridViewSortDirection::ascending);   // BBC Radio 4 first
