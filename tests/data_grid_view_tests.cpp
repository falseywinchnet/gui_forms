// DataGridView (stage W): columns and unbound rows, layout and published
// parts, selection and keyboard navigation, text editing in the hosted
// editor, check box, button and combo box cells, virtual rows on stable row
// IDs with a provider asked by ID, row identity kept across inserts, removals
// and sorts in every row source, sorting, header clicks and column resizing,
// BindingSource rows, scrolling and semantics.

#include "gui_forms/gui_forms.hpp"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace {
using namespace gui_forms;

void require(bool condition, const char* message) {
    if (condition) return;
    std::cerr << message << '\n';
    std::exit(1);
}

// Positions and addresses, spelled briefly in the tests.
DataGridViewRowPosition row_at(std::size_t position) {
    const DataGridViewRowPosition row{position};
    return row;
}

DataGridViewCellAddress cell_at(std::size_t row, std::size_t column) {
    const DataGridViewCellAddress cell{DataGridViewRowPosition{row}, column};
    return cell;
}

DataGridViewRowId id(std::string_view text) {
    const DataGridViewRowId identity{std::string(text)};
    return identity;
}

// Counts drawn text; paint_part draws every label and content through it.
class CountingPainter final : public Painter {
public:
    void save() override {}
    void restore() override {}
    void translate(Point) override {}
    void clip_rect(Rect) override {}
    void fill_rect(Rect, Color) override {}
    void stroke_rect(Rect, Color, double) override {}
    void draw_line(Point, Point, Color, double) override {}
    void draw_text_utf8(Point, std::string_view text, FontSpec, Color) override { texts.emplace_back(text); }
    void draw_image(ImageId, Rect, double) override {}

    std::vector<std::string> texts;
};

struct Counter final {
    int* count{};
    void operator()() const { ++*count; }
};

struct RecordCurrent final {
    std::vector<std::optional<DataGridViewCellAddress>>* seen{};
    void operator()(std::optional<DataGridViewCellAddress> cell) const { (*seen).push_back(cell); }
};

struct RecordCell final {
    std::vector<DataGridViewCellEvent>* seen{};
    void operator()(const DataGridViewCellEvent& event) const { (*seen).push_back(event); }
};

struct RecordIndex final {
    std::vector<std::size_t>* seen{};
    void operator()(std::size_t index) const { (*seen).push_back(index); }
};

struct CancelBeginEdit final {
    bool* cancel{};
    void operator()(DataGridViewCellCancelEvent& event) const { event.cancel = *cancel; }
};

// Refuses any text that is "bad".
struct RefuseBad final {
    int* calls{};
    void operator()(DataGridViewCellValidatingEvent& event) const {
        ++*calls;
        if (event.formatted_value == "bad") event.cancel = true;
    }
};

// Rows whose values are computed from their IDs; records every ID the grid
// asks for. Column 0 is the ID with a prefix, column 1 whether the ID has an
// even length.
class CountingProvider final : public DataGridViewCellValueProvider {
public:
    [[nodiscard]] BindingValue cell_value(const DataGridViewRowId& row, std::size_t column) const override {
        ++asked;
        asked_ids.insert(row.value);
        if (column == 1U) return BindingValue{(row.value.size() % 2U) == 0U};
        return BindingValue{"label " + row.value};
    }
    bool push_cell_value(const DataGridViewRowId& row, std::size_t column, const BindingValue& value) override {
        ++pushed;
        if (refuse) return false;
        last_row = row;
        last_column = column;
        last_value = value;
        return true;
    }

    mutable std::size_t asked{};
    mutable std::unordered_set<std::string> asked_ids{};
    std::size_t pushed{};
    bool refuse{};
    DataGridViewRowId last_row{};
    std::size_t last_column{};
    BindingValue last_value{};
};

// IDs "v<first>" to "v<first + count - 1>", in that order.
std::vector<DataGridViewRowId> virtual_order(std::size_t count, std::size_t first = 0U) {
    std::vector<DataGridViewRowId> order;
    order.reserve(count);
    for (std::size_t index = 0; index < count; ++index) {
        order.push_back(id("v" + std::to_string(first + index)));
    }
    return order;
}

std::vector<DataGridViewColumn> sample_columns() {
    std::vector<DataGridViewColumn> columns;
    columns.push_back({.name = "name", .header_text = "Name", .width = 100.0});
    columns.push_back({.name = "active", .header_text = "On", .kind = DataGridViewColumnKind::check_box,
                       .width = 50.0});
    columns.push_back({.name = "mode", .header_text = "Mode", .kind = DataGridViewColumnKind::combo_box,
                       .width = 90.0, .items = {"AM", "FM", "USB"}});
    columns.push_back({.name = "tune", .header_text = "", .kind = DataGridViewColumnKind::button,
                       .width = 60.0, .button_text = "Tune"});
    return columns;
}

std::vector<DataGridViewColumn> virtual_columns() {
    std::vector<DataGridViewColumn> columns;
    columns.push_back({.name = "label", .header_text = "Label", .width = 120.0});
    columns.push_back({.name = "even", .header_text = "Even", .kind = DataGridViewColumnKind::check_box,
                       .width = 50.0});
    return columns;
}

std::shared_ptr<DataGridView> make_grid(const char* identity) {
    std::shared_ptr<DataGridView> grid = make_control<DataGridView>(StableId(identity));
    (*grid).set_columns(sample_columns());
    (*grid).add_row({.stable_id = {"alpha"}, .cells = {BindingValue{std::string("Alpha")}, BindingValue{true},
                                                      BindingValue{std::string("FM")}}});
    (*grid).add_row({.stable_id = {"bravo"}, .cells = {BindingValue{std::string("Bravo")}, BindingValue{false},
                                                      BindingValue{std::string("AM")}}});
    (*grid).add_row({.stable_id = {"charlie"}, .cells = {BindingValue{std::string("Charlie")}}});
    return grid;
}

Point centre(const DataGridView& grid, Rect local) {
    const Rect absolute = grid.absolute_bounds();
    const Point point{absolute.x + local.x + local.width * 0.5, absolute.y + local.y + local.height * 0.5};
    return point;
}

// The returned pointer borrows from `cells`, so the vector must outlive it:
// realized_cells() returns a new vector, and a pointer into that temporary
// dangles at the end of the statement. The deleted overload refuses it.
const DataGridView::Cell* find_cell(const std::vector<DataGridView::Cell>& cells, std::size_t row,
                                    std::size_t column) {
    for (const DataGridView::Cell& cell : cells) {
        if (cell.address.row.value == row && cell.address.column == column) return &cell;
    }
    return nullptr;
}
const DataGridView::Cell* find_cell(std::vector<DataGridView::Cell>&& cells, std::size_t row,
                                    std::size_t column) = delete;

// The semantic row node that publishes focused, or null. It borrows from
// `nodes`, as find_cell borrows from its cells; temporaries are refused.
const SemanticNode* focused_row(const std::vector<SemanticNode>& nodes) {
    for (const SemanticNode& node : nodes) {
        if (node.role == SemanticRole::list_item && has_semantic_state(node.states, SemanticState::focused)) {
            return &node;
        }
    }
    return nullptr;
}
const SemanticNode* focused_row(std::vector<SemanticNode>&& nodes) = delete;

void click(Window& window, Point point, Modifier modifiers = Modifier::none, std::uint32_t clicks = 1U) {
    PointerEvent down{PointerAction::down, PointerButton::primary, point};
    down.modifiers = modifiers;
    down.click_count = clicks;
    static_cast<void>(window.dispatch_pointer(down));
    PointerEvent up{PointerAction::up, PointerButton::primary, point};
    up.modifiers = modifiers;
    static_cast<void>(window.dispatch_pointer(up));
}

bool key(Window& window, std::uint32_t physical, Modifier modifiers = Modifier::none) {
    KeyEvent event{KeyAction::down, physical};
    event.modifiers = modifiers;
    const bool handled = window.dispatch_key(event);
    return handled;
}

template <typename Exception, typename Action>
bool throws(Action action) {
    try {
        action();
    } catch (const Exception&) {
        return true;
    }
    return false;
}

struct AddDottedColumn final {
    DataGridView* grid{};
    void operator()() const { (*grid).add_column({.name = "a.b"}); }
};
struct AddDuplicateRow final {
    DataGridView* grid{};
    void operator()() const { static_cast<void>((*grid).add_row({.stable_id = {"alpha"}})); }
};
struct AddItemsToText final {
    DataGridView* grid{};
    void operator()() const { (*grid).add_column({.name = "extra", .items = {"x"}}); }
};
struct NarrowColumn final {
    DataGridView* grid{};
    void operator()() const { (*grid).set_column_width(0U, 5.0); }
};
struct SetOrder final {
    DataGridView* grid{};
    std::vector<DataGridViewRowId> order{};
    void operator()() const { (*grid).set_virtual_row_order(order); }
};
struct SetCount final {
    DataGridView* grid{};
    std::size_t count{};
    void operator()() const { (*grid).set_row_count(count); }
};
struct SetVirtualRows final {
    DataGridView* grid{};
    std::shared_ptr<DataGridViewCellValueProvider> provider{};
    std::vector<DataGridViewRowId> order{};
    void operator()() const { (*grid).set_virtual_rows(provider, order); }
};

std::shared_ptr<Control> as_control(const std::shared_ptr<TextBox>& editor) {
    const std::shared_ptr<Control> control = editor;
    return control;
}

void columns_and_rows() {
    std::shared_ptr<DataGridView> grid = make_grid("grid.model");
    const ControlAnatomy& anatomy = (*grid).anatomy();
    require(validate_control_anatomy(anatomy).defect == AnatomyDefect::none &&
                anatomy.control_id == "data_grid_view" && anatomy.item_systems.size() == 3U,
            "DataGridView publishes a valid anatomy with row, cell and column item systems");
    require((*grid).columns().size() == 4U && (*grid).row_count() == 3U, "columns and rows are kept");
    require((*grid).cell_text(row_at(0U), 0U) == "Alpha" && (*grid).cell_value(row_at(0U), 1U) == BindingValue{true},
            "cells keep their binding values");
    require((*grid).cell_text(row_at(2U), 3U) == "Tune" && (*grid).cell_value(row_at(2U), 2U) == BindingValue{},
            "short rows are padded, and an empty button cell shows its column's text");
    require((*grid).row_position(id("bravo")) == row_at(1U) && (*grid).row_id(row_at(2U)) == id("charlie"),
            "rows keep their stable IDs");
    const DataGridViewRowPosition generated = (*grid).add_row({});
    require((*grid).row_id(generated).value.rfind("row.", 0U) == 0U, "an empty row ID is generated");
    (*grid).insert_row(row_at(0U), {.stable_id = {"zulu"}});
    require((*grid).row_position(id("alpha")) == row_at(1U), "inserting moves later rows");
    (*grid).remove_row_at(row_at(0U));
    require((*grid).row_position(id("alpha")) == row_at(0U) && (*grid).row_count() == 4U,
            "removing moves them back");
    require(!(*grid).row_position(id("zulu")), "a removed row's ID is gone");
    (*grid).set_row_count(6U);
    require((*grid).row_count() == 6U, "an unbound row count grows with generated rows");
    (*grid).set_row_count(3U);
    require((*grid).row_count() == 3U && (*grid).row_position(id("charlie")) == row_at(2U),
            "and is cut from the end");
    std::shared_ptr<DataGridView> fresh = make_control<DataGridView>(StableId("grid.model.fresh"));
    (*fresh).set_columns(sample_columns());
    (*fresh).set_rows({{.stable_id = {"row.1"}}, {}});
    require((*fresh).row_id(row_at(1U)).value != "row.1" && !(*fresh).row_id(row_at(1U)).value.empty(),
            "a generated row ID skips the IDs the rows already use");
    require(throws<std::invalid_argument>(AddDottedColumn{grid.get()}), "a column name with '.' is refused");
    require(throws<std::invalid_argument>(AddDuplicateRow{grid.get()}), "a duplicate row ID is refused");
    require(throws<std::invalid_argument>(AddItemsToText{grid.get()}), "items on a text column are refused");
    require(throws<std::invalid_argument>(NarrowColumn{grid.get()}), "a width below the minimum is refused");
    bool past_end = false;
    try {
        static_cast<void>((*grid).cell_value(row_at(9U), 0U));
    } catch (const std::out_of_range&) {
        past_end = true;
    }
    require(past_end, "a cell past the rows is refused");
    bool id_past_end = false;
    try {
        static_cast<void>((*grid).row_id(row_at(3U)));
    } catch (const std::out_of_range&) {
        id_past_end = true;
    }
    require(id_past_end, "a row ID past the rows is refused");
}

void layout_and_parts() {
    std::shared_ptr<DataGridView> grid = make_grid("grid.parts");
    Window window(grid, {360.0, 160.0});
    window.perform_layout();
    const DataGridView::Parts parts = (*grid).parts();
    require(parts.column_header_band.present && parts.row_header_band.present && parts.top_left_header.present,
            "both header bands and the corner header are present");
    require(!parts.vertical_scroll_bar.present && !parts.horizontal_scroll_bar.present,
            "content that fits shows no scroll bars");
    require(!parts.editor.present, "the editor is absent outside an edit");
    const std::vector<DataGridView::Row> rows = (*grid).realized_rows();
    const std::vector<DataGridView::Cell> cells = (*grid).realized_cells();
    const std::vector<DataGridView::Column> columns = (*grid).realized_columns();
    require(rows.size() == 3U && cells.size() == 12U && columns.size() == 4U,
            "every visible row, cell and column header is realized");
    require(rows[1].position == row_at(1U) && rows[1].stable_id == "bravo",
            "a realized row carries its position and its row ID");
    require((*grid).realized_item_count() == 19U, "the generic path counts rows, cells and headers");
    require((*grid).realized_item_system(0U).value == 0U && (*grid).realized_item_system(3U).value == 1U &&
                (*grid).realized_item_system(18U).value == 2U && (*grid).realized_item_stable_id(18U) == "grid.parts.column.tune",
            "realized items are rows, then cells, then column headers");
    const DataGridView::Cell* text = find_cell(cells, 0U, 0U);
    const DataGridView::Cell* check = find_cell(cells, 0U, 1U);
    const DataGridView::Cell* combo = find_cell(cells, 0U, 2U);
    const DataGridView::Cell* button = find_cell(cells, 0U, 3U);
    require(text != nullptr && (*text).parts.cell_content.present && !(*text).parts.cell_check_well.present &&
                (*text).stable_id == "alpha.name",
            "a text cell shows content and is named row.column");
    require(check != nullptr && (*check).parts.cell_check_well.present && (*check).states.checked &&
                !(*check).parts.cell_content.present,
            "a check box cell shows a well and publishes checked");
    require(combo != nullptr && (*combo).parts.cell_drop_down.present && (*combo).parts.cell_drop_down_glyph.present &&
                (*combo).parts.cell_content.present,
            "a combo box cell shows its value beside a drop-down actuator");
    require(button != nullptr && (*button).parts.cell_button_face.present && (*button).parts.cell_button_label.present,
            "a button cell shows a face and a label");
    require((*text).parts.cell_vertical_rule.present && (*text).parts.cell_horizontal_rule.present,
            "cells publish their grid lines");
    require(columns[0].parts.column_label.present && columns[0].parts.column_divider.present &&
                columns[0].parts.column_grip.present && !columns[0].parts.sort_glyph.present,
            "a column header publishes its label, divider and grip, and no sort glyph unsorted");
    require(rows[0].parts.row_header.present && !rows[0].parts.row_glyph.present,
            "a row header has no glyph until its row is current");

    CountingPainter painter;
    (*grid).on_paint(painter, (*grid).client_rectangle());
    bool painted_alpha = false;
    bool painted_tune = false;
    bool painted_header = false;
    for (const std::string& drawn : painter.texts) {
        if (drawn == "Alpha") painted_alpha = true;
        if (drawn == "Tune") painted_tune = true;
        if (drawn == "Mode") painted_header = true;
    }
    require(painted_alpha && painted_tune && painted_header,
            "cell content, button labels and header labels are painted by role");

    (*grid).set_column_headers_visible(false);
    (*grid).set_row_headers_visible(false);
    window.perform_layout();
    require(!(*grid).parts().column_header_band.present && !(*grid).parts().row_header_band.present &&
                (*grid).realized_columns().empty(),
            "hidden headers publish no bands and no column items");
}

void selection_and_navigation() {
    std::shared_ptr<DataGridView> grid = make_grid("grid.select");
    Window window(grid, {360.0, 160.0});
    window.perform_layout();
    int selection_changes = 0;
    std::vector<std::optional<DataGridViewCellAddress>> currents;
    SubscriptionToken selection_token = (*grid).selection_changed().subscribe(Counter{&selection_changes});
    SubscriptionToken current_token = (*grid).current_cell_changed().subscribe(RecordCurrent{&currents});
    require(window.request_focus(grid), "the grid takes focus");
    require((*grid).current_cell() == cell_at(0U, 0U) && (*grid).is_cell_selected(row_at(0U), 0U),
            "focus makes the first cell current and selected");
    require((*grid).current_row_id() == id("alpha"), "the current cell names its row's ID");
    require(currents.size() == 1U && selection_changes == 1, "the first current cell is reported once");
    require(key(window, PhysicalKey::right) && (*grid).current_cell() == cell_at(0U, 1U),
            "Right moves to the next column");
    require(key(window, PhysicalKey::down, Modifier::shift) &&
                (*grid).current_cell() == cell_at(1U, 1U) && (*grid).selected_cells().size() == 2U,
            "Shift+Down extends the selection from the anchor");
    require(key(window, PhysicalKey::end, Modifier::control) &&
                (*grid).current_cell() == cell_at(2U, 3U) && (*grid).selected_cells().size() == 1U,
            "Ctrl+End moves to the last cell and selects it alone");
    require(key(window, PhysicalKey::home) && (*grid).current_cell() == cell_at(2U, 0U),
            "Home moves to the row's first cell");
    require(key(window, PhysicalKey::tab, Modifier::shift) && (*grid).current_cell() == cell_at(1U, 3U),
            "Shift+Tab from a row's first cell moves to the previous row's last cell");
    require(key(window, PhysicalKey::tab) && (*grid).current_cell() == cell_at(2U, 0U),
            "Tab wraps to the next row");
    require(key(window, PhysicalKey::page_up) && (*grid).current_cell() == cell_at(0U, 0U),
            "Page Up moves up by a page");
    const std::vector<DataGridView::Row> rows = (*grid).realized_rows();
    require(rows[0].states.current && rows[0].parts.row_glyph.present, "the current row shows its glyph");

    const std::vector<DataGridView::Cell> cells = (*grid).realized_cells();
    click(window, centre(*grid, (*find_cell(cells, 2U, 0U)).parts.cell.bounds), Modifier::control);
    require((*grid).is_cell_selected(row_at(0U), 0U) && (*grid).is_cell_selected(row_at(2U), 0U),
            "Ctrl+click adds a cell to the selection");
    click(window, centre(*grid, (*find_cell(cells, 2U, 0U)).parts.cell.bounds), Modifier::control);
    require(!(*grid).is_cell_selected(row_at(2U), 0U), "Ctrl+click again removes it");

    (*grid).set_selection_mode(DataGridViewSelectionMode::full_row_select);
    click(window, centre(*grid, (*find_cell(cells, 1U, 0U)).parts.cell.bounds));
    require((*grid).selected_rows().size() == 1U && (*grid).selected_rows()[0] == row_at(1U) &&
                (*grid).is_cell_selected(row_at(1U), 3U) && (*grid).realized_rows()[1].states.selected,
            "full-row selection selects the clicked cell's row");
    require(key(window, PhysicalKey::down, Modifier::shift) && (*grid).selected_rows().size() == 2U,
            "Shift+Down extends a row selection");
    (*grid).set_multi_select(false);
    require((*grid).selected_rows().size() == 1U, "turning multi-select off keeps the current row alone");
    (*grid).select_all();
    require((*grid).selected_rows().size() == 1U, "select_all needs multi-select");
    (*grid).set_multi_select(true);
    (*grid).select_all();
    require((*grid).selected_rows().size() == 3U, "select_all selects every row");
    const int before = selection_changes;
    (*grid).clear_selection();
    require(selection_changes == before + 1 && (*grid).selected_cells().empty(),
            "clear_selection raises selection_changed once");
    require(!(*grid).is_row_selected(row_at(7U)) && !(*grid).is_cell_selected(row_at(7U), 0U),
            "a position past the rows is not selected");
}

void text_editing() {
    std::shared_ptr<DataGridView> grid = make_grid("grid.edit");
    Window window(grid, {360.0, 160.0});
    window.perform_layout();
    std::vector<DataGridViewCellEvent> ended;
    std::vector<DataGridViewCellEvent> changed;
    SubscriptionToken end_token = (*grid).cell_end_edit().subscribe(RecordCell{&ended});
    SubscriptionToken change_token = (*grid).cell_value_changed().subscribe(RecordCell{&changed});
    require(window.request_focus(grid), "the grid takes focus");

    require(key(window, PhysicalKey::f2) && (*grid).is_current_cell_in_edit_mode(), "F2 begins an edit");
    window.perform_layout();
    require((*grid).parts().editor.present && window.focused_control() == as_control((*grid).editing_control()),
            "the editor covers the cell and takes focus");
    require((*(*grid).editing_control()).text() == "Alpha", "the editor starts with the cell's text");
    require(window.dispatch_text({"Delta"}) && (*(*grid).editing_control()).text() == "Delta",
            "typing replaces the selected text");
    require(key(window, PhysicalKey::enter), "Enter commits");
    require(!(*grid).is_current_cell_in_edit_mode() && (*grid).cell_text(row_at(0U), 0U) == "Delta",
            "the committed text is the cell's value");
    require(changed.size() == 1U && ended.size() == 1U && ended[0].row == row_at(0U) &&
                ended[0].row_id == id("alpha"),
            "a commit raises cell_value_changed then cell_end_edit, with the row's ID");
    const std::shared_ptr<Control> grid_control = grid;
    require((*grid).current_cell() == cell_at(1U, 0U) && window.focused_control() == grid_control,
            "Enter moves to the cell below and focus returns to the grid");

    require(window.dispatch_text({"X"}) && (*grid).is_current_cell_in_edit_mode() &&
                (*(*grid).editing_control()).text() == "X",
            "typing on a text cell begins an edit with the typed text");
    require(key(window, PhysicalKey::escape) && !(*grid).is_current_cell_in_edit_mode() &&
                (*grid).cell_text(row_at(1U), 0U) == "Bravo" && ended.size() == 2U && changed.size() == 1U,
            "Escape discards the edit");

    bool cancel = true;
    SubscriptionToken begin_token = (*grid).cell_begin_edit().subscribe(CancelBeginEdit{&cancel});
    require(!(*grid).begin_edit() && !(*grid).is_current_cell_in_edit_mode(),
            "a cancelled cell_begin_edit keeps the cell out of edit");
    cancel = false;

    int validations = 0;
    SubscriptionToken validate_token = (*grid).cell_validating().subscribe(RefuseBad{&validations});
    require((*grid).begin_edit(true), "an edit begins");
    require(window.dispatch_text({"bad"}), "the editor takes typing");
    require(key(window, PhysicalKey::tab) && (*grid).is_current_cell_in_edit_mode() && validations == 1,
            "a refused value keeps the cell in edit");
    const std::vector<DataGridView::Cell> refused_cells = (*grid).realized_cells();
    const DataGridView::Cell* refused = find_cell(refused_cells, 1U, 0U);
    require(refused != nullptr && (*refused).states.invalid, "the refused cell publishes invalid");
    (*(*grid).editing_control()).set_text("Echo");
    require(key(window, PhysicalKey::tab) && !(*grid).is_current_cell_in_edit_mode() &&
                (*grid).cell_text(row_at(1U), 0U) == "Echo" && (*grid).current_cell() == cell_at(1U, 1U),
            "Tab commits and moves to the next cell");

    // A number column converts the typed text and refuses what is not a number.
    (*grid).add_column({.name = "frequency", .header_text = "MHz", .width = 60.0,
                        .value_kind = BindingValueKind::number});
    require((*grid).set_current_cell(cell_at(0U, 4U)) && (*grid).begin_edit(), "edit a number");
    (*(*grid).editing_control()).set_text("abc");
    require(!(*grid).end_edit() && (*grid).is_current_cell_in_edit_mode(), "text that is not a number is refused");
    (*(*grid).editing_control()).set_text("7.25");
    require((*grid).end_edit() && (*grid).cell_value(row_at(0U), 4U) == BindingValue{7.25} &&
                (*grid).cell_text(row_at(0U), 4U) == "7.25",
            "a number commits as a number and shows its shortest text");

    // Read-only cells do not edit; a double click edits a text cell.
    (*grid).set_column_read_only(0U, true);
    require((*grid).set_current_cell(cell_at(2U, 0U)) && !(*grid).begin_edit(),
            "a read-only column does not edit");
    (*grid).set_column_read_only(0U, false);
    window.perform_layout();
    const std::vector<DataGridView::Cell> target_cells = (*grid).realized_cells();
    const DataGridView::Cell* target = find_cell(target_cells, 2U, 0U);
    click(window, centre(*grid, (*target).parts.cell.bounds), Modifier::none, 2U);
    require((*grid).is_current_cell_in_edit_mode(), "a double click begins an edit");
    (*grid).cancel_edit();
}

void check_button_and_combo_cells() {
    std::shared_ptr<DataGridView> grid = make_grid("grid.kinds");
    Window window(grid, {360.0, 160.0});
    window.perform_layout();
    std::vector<DataGridViewCellEvent> changed;
    std::vector<DataGridViewCellEvent> clicked;
    SubscriptionToken change_token = (*grid).cell_value_changed().subscribe(RecordCell{&changed});
    SubscriptionToken click_token = (*grid).cell_content_click().subscribe(RecordCell{&clicked});
    require(window.request_focus(grid), "the grid takes focus");

    require((*grid).set_current_cell(cell_at(0U, 1U)), "a check box cell becomes current");
    require(key(window, PhysicalKey::space) && (*grid).cell_value(row_at(0U), 1U) == BindingValue{false} &&
                changed.size() == 1U && clicked.size() == 1U,
            "Space toggles a check box cell");
    const std::vector<DataGridView::Cell> check_cells = (*grid).realized_cells();
    const DataGridView::Cell* check = find_cell(check_cells, 1U, 1U);
    click(window, centre(*grid, (*check).parts.cell_check_well.bounds));
    const std::vector<DataGridView::Cell> checked_cells = (*grid).realized_cells();
    require((*grid).cell_value(row_at(1U), 1U) == BindingValue{true} &&
                (*find_cell(checked_cells, 1U, 1U)).states.checked,
            "clicking the check well toggles and publishes checked");
    (*grid).set_read_only(true);
    require(key(window, PhysicalKey::space) && (*grid).cell_value(row_at(1U), 1U) == BindingValue{true},
            "a read-only grid does not toggle");
    (*grid).set_read_only(false);

    const std::vector<DataGridView::Cell> button_cells = (*grid).realized_cells();
    const DataGridView::Cell* button = find_cell(button_cells, 2U, 3U);
    const Point face = centre(*grid, (*button).parts.cell_button_face.bounds);
    require(window.dispatch_pointer({PointerAction::down, PointerButton::primary, face}),
            "the button cell takes the press");
    const std::vector<DataGridView::Cell> pressed_cells = (*grid).realized_cells();
    require((*find_cell(pressed_cells, 2U, 3U)).states.pressed, "a held button cell publishes pressed");
    static_cast<void>(window.dispatch_pointer({PointerAction::up, PointerButton::primary, face}));
    require(clicked.size() == 3U && clicked.back().row == row_at(2U) && clicked.back().column == 3U &&
                clicked.back().row_id == id("charlie"),
            "releasing on the button cell raises cell_content_click");

    require((*grid).set_current_cell(cell_at(0U, 2U)), "a combo box cell becomes current");
    require(key(window, PhysicalKey::f4) && (*grid).drop_down_open() && (*grid).is_current_cell_in_edit_mode(),
            "F4 opens the combo box cell's choices");
    const std::vector<DataGridView::Cell> open_cells = (*grid).realized_cells();
    require((*find_cell(open_cells, 0U, 2U)).states.open, "the open combo box cell publishes open");
    const std::size_t before = changed.size();
    require(key(window, PhysicalKey::down) && key(window, PhysicalKey::enter), "a choice is made by keyboard");
    require(!(*grid).drop_down_open() && !(*grid).is_current_cell_in_edit_mode() &&
                (*grid).cell_text(row_at(0U), 2U) == "FM" && changed.size() == before,
            "choosing the current value changes nothing");
    require(key(window, PhysicalKey::down, Modifier::alt) && (*grid).drop_down_open(), "Alt+Down opens the choices");
    require(key(window, PhysicalKey::end) && key(window, PhysicalKey::enter) &&
                (*grid).cell_text(row_at(0U), 2U) == "USB" && changed.size() == before + 1U,
            "a different choice becomes the cell's value");
    const std::vector<DataGridView::Cell> combo_cells = (*grid).realized_cells();
    const DataGridView::Cell* combo = find_cell(combo_cells, 1U, 2U);
    click(window, centre(*grid, (*combo).parts.cell_drop_down.bounds));
    require((*grid).drop_down_open() && (*grid).current_cell() == cell_at(1U, 2U),
            "clicking a drop-down actuator opens that cell's choices");
    (*grid).close_drop_down();
    require(!(*grid).is_current_cell_in_edit_mode(), "closing the choices ends the edit");
}

void virtual_rows() {
    std::shared_ptr<CountingProvider> provider = std::make_shared<CountingProvider>();
    std::shared_ptr<DataGridView> grid = make_control<DataGridView>(StableId("grid.virtual"));
    (*grid).set_columns(virtual_columns());
    const std::vector<DataGridViewRowId> order = virtual_order(1000000U);
    (*grid).set_virtual_rows(provider, order);
    Window window(grid, {300.0, 200.0});
    window.perform_layout();
    require((*grid).row_source() == DataGridViewRowSource::virtual_rows && (*grid).row_count() == 1000000U,
            "virtual rows are counted by their order");
    require((*grid).parts().vertical_scroll_bar.present, "a million rows show a vertical scroll bar");
    const std::size_t shown = (*grid).displayed_row_end().value - (*grid).first_displayed_row().value;
    require(shown > 0U && shown < 20U, "only a window of rows is displayed");

    // Realization is bounded by the viewport: rows, their cells and headers.
    const std::vector<DataGridView::Row> realized = (*grid).realized_rows();
    require(realized.size() == shown && (*grid).realized_cells().size() == shown * 2U &&
                (*grid).realized_item_count() == shown + shown * 2U + 2U,
            "only the displayed rows and cells are realized");
    require(realized[0].stable_id == "v0" && realized[shown - 1U].stable_id == order[shown - 1U].value,
            "realized rows carry the application's row IDs in order");

    // The provider is asked by ID, and only for displayed rows.
    (*provider).asked = 0U;
    (*provider).asked_ids.clear();
    CountingPainter painter;
    (*grid).on_paint(painter, (*grid).client_rectangle());
    std::unordered_set<std::string> displayed;
    for (std::size_t position = 0; position < shown; ++position) displayed.insert(order[position].value);
    bool only_displayed = !(*provider).asked_ids.empty();
    for (const std::string& asked : (*provider).asked_ids) {
        if (displayed.count(asked) == 0U) only_displayed = false;
    }
    require(only_displayed && (*provider).asked <= shown * 2U * 3U,
            "paint asks only for the displayed rows' values, by ID");
    require((*grid).cell_text(row_at(5U), 0U) == "label v5" && (*grid).row_id(row_at(5U)) == id("v5"),
            "virtual rows take their values from the provider by ID");

    (*grid).set_first_displayed_row(row_at(500000U));
    window.perform_layout();
    (*provider).asked_ids.clear();
    CountingPainter later;
    (*grid).on_paint(later, (*grid).client_rectangle());
    bool scrolled_only = !(*provider).asked_ids.empty();
    for (const std::string& asked : (*provider).asked_ids) {
        const std::optional<DataGridViewRowPosition> position = (*grid).row_position(id(asked));
        if (!position || (*position).value < 500000U || (*position).value >= 500000U + shown) scrolled_only = false;
    }
    require(scrolled_only, "scrolled rows are asked for where they are, and no others");
    require((*grid).realized_rows()[0].stable_id == "v500000", "the realized window follows the scroll");

    require(window.request_focus(grid), "the grid takes focus");
    require((*grid).current_cell() == cell_at(500000U, 0U) && (*grid).current_row_id() == id("v500000"),
            "the first displayed cell becomes current");
    require(window.dispatch_text({"new"}) && key(window, PhysicalKey::enter), "an edit commits through the provider");
    require((*provider).pushed == 1U && (*provider).last_row == id("v500000") &&
                (*provider).last_value == BindingValue{std::string("new")},
            "the provider receives the committed value with the row's ID");
    (*provider).refuse = true;
    require((*grid).set_current_cell(cell_at(500000U, 1U)) && key(window, PhysicalKey::space) &&
                (*provider).pushed == 2U,
            "a toggle pushes to the provider");
    require(!(*grid).set_cell_value(row_at(3U), 0U, BindingValue{std::string("x")}), "a refused value reports false");
    require(key(window, PhysicalKey::end, Modifier::control) && (*grid).current_cell() == cell_at(999999U, 1U),
            "Ctrl+End moves to the last virtual row");
    window.perform_layout();
    require((*grid).displayed_row_end() == row_at(1000000U), "and scrolls it into view");
}

// Virtual rows: the application inserts, removes and sorts rows by publishing
// a new order; the grid keeps the selection, the current cell, focus and an
// edit on their row IDs.
void virtual_row_identity() {
    std::shared_ptr<CountingProvider> provider = std::make_shared<CountingProvider>();
    std::shared_ptr<DataGridView> grid = make_control<DataGridView>(StableId("grid.identity"));
    (*grid).set_columns(virtual_columns());
    (*grid).set_virtual_rows(provider, virtual_order(20U));
    Window window(grid, {300.0, 200.0});
    window.perform_layout();
    require(window.request_focus(grid), "the grid takes focus");
    require((*grid).set_current_cell(cell_at(3U, 0U)), "v3 becomes current");
    (*grid).set_cell_selected(row_at(5U), 0U, true);
    require((*grid).selected_cells().size() == 2U && (*grid).current_row_id() == id("v3"),
            "v3 and v5 are selected, v3 current");
    int selection_changes = 0;
    std::vector<std::optional<DataGridViewCellAddress>> currents;
    SubscriptionToken selection_token = (*grid).selection_changed().subscribe(Counter{&selection_changes});
    SubscriptionToken current_token = (*grid).current_cell_changed().subscribe(RecordCurrent{&currents});

    // Insert two rows above.
    std::vector<DataGridViewRowId> inserted{id("n1"), id("n2")};
    const std::vector<DataGridViewRowId> base = virtual_order(20U);
    inserted.insert(inserted.end(), base.begin(), base.end());
    (*grid).set_virtual_row_order(inserted);
    window.perform_layout();
    require((*grid).row_count() == 22U && (*grid).current_cell() == cell_at(5U, 0U) &&
                (*grid).current_row_id() == id("v3"),
            "an insert above moves the current cell with its row");
    require((*grid).is_cell_selected(row_at(5U), 0U) && (*grid).is_cell_selected(row_at(7U), 0U) &&
                !(*grid).is_cell_selected(row_at(3U), 0U) && (*grid).selected_cells().size() == 2U,
            "the selection stays on its rows");
    require(currents.empty() && selection_changes == 0,
            "a moved row is not a new current cell or selection");
    const std::vector<SemanticNode> nodes = (*grid).semantic_virtual_children();
    const SemanticNode* focused = focused_row(nodes);
    require(focused != nullptr && (*focused).stable_id == "v3", "focus stays on the current row's ID");
    const std::vector<DataGridView::Row> realized = (*grid).realized_rows();
    std::size_t current_rows = 0U;
    for (const DataGridView::Row& row : realized) {
        if (!row.states.current) continue;
        ++current_rows;
        require(row.stable_id == "v3" && row.position == row_at(5U), "the realized current row is v3 at its new position");
    }
    require(current_rows == 1U && (*grid).first_displayed_row() == row_at(2U),
            "one realized row is current, and the first displayed row stayed on v0");

    // Remove a selected row and a row above the current one.
    std::vector<DataGridViewRowId> removed{};
    for (const DataGridViewRowId& row : inserted) {
        if (row == id("n1") || row == id("v5")) continue;
        removed.push_back(row);
    }
    (*grid).set_virtual_row_order(removed);
    require((*grid).row_count() == 20U && (*grid).current_cell() == cell_at(4U, 0U) &&
                (*grid).selected_cells().size() == 1U && selection_changes == 1 && currents.empty(),
            "a removed selected row leaves the selection once; the current cell stays on v3");

    // Sort: the application reverses its order.
    const std::vector<DataGridViewRowId> reversed(removed.rbegin(), removed.rend());
    (*grid).set_virtual_row_order(reversed);
    const std::optional<DataGridViewRowPosition> v3 = (*grid).row_position(id("v3"));
    require(v3 && (*grid).current_cell() == DataGridViewCellAddress{*v3, 0U} && (*grid).is_cell_selected(*v3, 0U) &&
                (*grid).selected_cells().size() == 1U && currents.empty(),
            "a sort keeps the current cell and the selection on their IDs");

    // Full-row selection is kept by ID too.
    (*grid).set_selection_mode(DataGridViewSelectionMode::full_row_select);
    (*grid).set_row_selected(*(*grid).row_position(id("v10")), true);
    (*grid).set_virtual_row_order(removed);
    require((*grid).is_row_selected(*(*grid).row_position(id("v10"))) &&
                (*grid).is_row_selected(*(*grid).row_position(id("v3"))) && (*grid).selected_rows().size() == 2U,
            "selected rows stay selected through a sort");
    (*grid).set_selection_mode(DataGridViewSelectionMode::cell_select);

    // An edit in progress stays on its row through a reorder and commits to it.
    require((*grid).set_current_cell(DataGridViewCellAddress{*(*grid).row_position(id("v3")), 0U}) &&
                (*grid).begin_edit(),
            "an edit begins on v3");
    (*(*grid).editing_control()).set_text("edited");
    std::vector<DataGridViewRowId> moved{};
    for (const DataGridViewRowId& row : removed) {
        if (!(row == id("v3"))) moved.push_back(row);
    }
    moved.push_back(id("v3"));
    (*grid).set_virtual_row_order(moved);
    window.perform_layout();
    require((*grid).is_current_cell_in_edit_mode() && (*grid).current_cell() == cell_at(19U, 0U),
            "the edit follows its row to the end");
    require((*grid).end_edit() && (*provider).last_row == id("v3") &&
                (*provider).last_value == BindingValue{std::string("edited")},
            "the edit commits to its own row's ID");

    // Removing the current row discards its edit; the row now at its position
    // becomes current.
    std::vector<DataGridViewCellEvent> ended;
    SubscriptionToken end_token = (*grid).cell_end_edit().subscribe(RecordCell{&ended});
    require((*grid).set_current_cell(cell_at(2U, 0U)) && (*grid).begin_edit(), "an edit begins at position 2");
    const DataGridViewRowId edited = (*grid).row_id(row_at(2U));
    const DataGridViewRowId following = (*grid).row_id(row_at(3U));
    std::vector<DataGridViewRowId> without{};
    for (const DataGridViewRowId& row : moved) {
        if (!(row == edited)) without.push_back(row);
    }
    (*grid).set_virtual_row_order(without);
    require(!(*grid).is_current_cell_in_edit_mode() && ended.size() == 1U && ended[0].row_id == edited &&
                ended[0].row == row_at(2U),
            "the removed row's edit is discarded and reported with its last position");
    require((*grid).current_row_id() == following && (*grid).current_cell() == cell_at(2U, 0U),
            "the row that takes the removed row's position becomes current");

    // The first displayed row stays on its ID, as ObjectView's top row does.
    (*grid).set_first_displayed_row(row_at(4U));
    const DataGridViewRowId top = (*grid).row_id(row_at(4U));
    std::vector<DataGridViewRowId> above{id("n7"), id("n8"), id("n9")};
    above.insert(above.end(), without.begin(), without.end());
    (*grid).set_virtual_row_order(above);
    require((*grid).first_displayed_row() == *(*grid).row_position(top), "the first displayed row follows its ID");

    // Refusals leave the grid unchanged.
    const std::size_t count = (*grid).row_count();
    const std::optional<DataGridViewRowId> current = (*grid).current_row_id();
    std::vector<DataGridViewRowId> duplicate = above;
    duplicate.push_back(id("v0"));
    require(throws<std::invalid_argument>(SetOrder{grid.get(), duplicate}), "a repeated ID is refused");
    require(throws<std::invalid_argument>(SetOrder{grid.get(), {id("v0"), id("")}}), "an empty ID is refused");
    require((*grid).row_count() == count && (*grid).current_row_id() == current,
            "a refused order changes nothing");
    require(throws<std::logic_error>(SetCount{grid.get(), 5U}), "virtual rows are not counted by set_row_count");
    require(throws<std::invalid_argument>(SetVirtualRows{grid.get(), nullptr, virtual_order(2U)}),
            "virtual rows need a provider");
    require(throws<std::invalid_argument>(SetVirtualRows{grid.get(), provider, {id("a"), id("a")}}) &&
                (*grid).row_count() == count,
            "set_virtual_rows refuses a repeated ID and changes nothing");
    std::shared_ptr<DataGridView> unbound = make_grid("grid.identity.unbound");
    require(throws<std::logic_error>(SetOrder{unbound.get(), virtual_order(2U)}),
            "only virtual rows take an order");
}

// Unbound and bound rows keep the same identity rules.
void unbound_row_identity() {
    std::shared_ptr<DataGridView> grid = make_grid("grid.unbound.identity");
    Window window(grid, {360.0, 160.0});
    window.perform_layout();
    require((*grid).set_current_cell(cell_at(1U, 0U)), "bravo becomes current");
    (*grid).set_cell_selected(row_at(2U), 0U, true);
    (*grid).insert_row(row_at(0U), {.stable_id = {"zulu"}});
    require((*grid).current_cell() == cell_at(2U, 0U) && (*grid).current_row_id() == id("bravo") &&
                (*grid).is_cell_selected(row_at(3U), 0U) && (*grid).selected_cells().size() == 2U,
            "an insert above keeps the current cell and the selection on their rows");
    int selection_changes = 0;
    SubscriptionToken token = (*grid).selection_changed().subscribe(Counter{&selection_changes});
    (*grid).remove_row_at(row_at(3U));
    require((*grid).selected_cells().size() == 1U && selection_changes == 1 && (*grid).current_row_id() == id("bravo"),
            "removing a selected row drops it from the selection");
    (*grid).remove_row_at(row_at(2U));
    require((*grid).current_row_id() == id("alpha") && (*grid).current_cell() == cell_at(1U, 0U),
            "removing the current last row makes the new last row current");
}

void sorting_headers_and_resizing() {
    std::shared_ptr<DataGridView> grid = make_grid("grid.sort");
    static_cast<void>((*grid).set_cell_value(row_at(2U), 0U, BindingValue{std::string("Able")}));
    Window window(grid, {360.0, 160.0});
    window.perform_layout();
    std::vector<std::size_t> header_clicks;
    std::vector<std::size_t> widths;
    SubscriptionToken header_token = (*grid).column_header_click().subscribe(RecordIndex{&header_clicks});
    SubscriptionToken width_token = (*grid).column_width_changed().subscribe(RecordIndex{&widths});
    std::vector<DataGridView::Column> columns = (*grid).realized_columns();
    click(window, centre(*grid, columns[0].parts.column_label.bounds));
    require(header_clicks.size() == 1U && header_clicks[0] == 0U, "clicking a header raises column_header_click");
    require((*grid).set_current_cell(cell_at(0U, 0U)), "Alpha is current");
    (*grid).set_cell_selected(row_at(1U), 0U, true);
    require((*grid).sort(0U, DataGridViewSortDirection::ascending), "unbound rows sort");
    require((*grid).cell_text(row_at(0U), 0U) == "Able" && (*grid).cell_text(row_at(1U), 0U) == "Alpha" &&
                (*grid).cell_text(row_at(2U), 0U) == "Bravo",
            "rows are ordered by the column's text");
    require((*grid).current_cell() == cell_at(1U, 0U) && (*grid).current_row_id() == id("alpha") &&
                (*grid).is_cell_selected(row_at(1U), 0U) && (*grid).is_cell_selected(row_at(2U), 0U) &&
                (*grid).row_position(id("charlie")) == row_at(0U),
            "the current cell and selection stay on their rows");
    window.perform_layout();
    columns = (*grid).realized_columns();
    require(columns[0].states.sorted_ascending && columns[0].parts.sort_glyph.present &&
                (*grid).columns()[0].sort_glyph == DataGridViewSortDirection::ascending,
            "the sorted column shows its glyph and publishes sorted_ascending");
    require((*grid).sort(1U, DataGridViewSortDirection::descending) &&
                (*grid).cell_value(row_at(0U), 1U) == BindingValue{true} &&
                (*grid).columns()[0].sort_glyph == DataGridViewSortDirection::none,
            "sorting another column moves the glyph to it");

    window.perform_layout();
    columns = (*grid).realized_columns();
    const Rect grip = columns[0].parts.column_grip.bounds;
    const Point start = centre(*grid, grip);
    require(window.dispatch_pointer({PointerAction::down, PointerButton::primary, start}),
            "the divider takes the press");
    require((*grid).realized_columns()[0].states.captured, "a resizing column publishes captured");
    static_cast<void>(window.dispatch_pointer({PointerAction::move, PointerButton::none, {start.x + 30.0, start.y}}));
    static_cast<void>(window.dispatch_pointer({PointerAction::up, PointerButton::primary, {start.x + 30.0, start.y}}));
    require((*grid).columns()[0].width == 130.0 && !widths.empty() && widths.back() == 0U,
            "dragging the divider widens the column");
    require(header_clicks.size() == 1U, "a resize is not a header click");

    std::shared_ptr<DataGridView> virtual_grid = make_control<DataGridView>(StableId("grid.sort.virtual"));
    (*virtual_grid).set_columns({{.name = "only"}});
    (*virtual_grid).set_virtual_rows(std::make_shared<CountingProvider>(), virtual_order(10U));
    require(!(*virtual_grid).sort(0U, DataGridViewSortDirection::ascending),
            "virtual rows are not sorted by the grid");
}

void bound_rows() {
    std::shared_ptr<DataGridView> grid = make_control<DataGridView>(StableId("grid.bound"));
    (*grid).set_columns({{.name = "name", .header_text = "Name", .data_property_name = "Name"},
                         {.name = "city", .header_text = "City", .data_property_name = "City"}});
    Window window(grid, {360.0, 160.0});
    std::shared_ptr<BindingSource> source = std::make_shared<BindingSource>(window);
    BindingRecord::FieldMap first;
    first["Name"] = std::string("Ada");
    first["City"] = std::string("London");
    BindingRecord::FieldMap second;
    second["Name"] = std::string("Grace");
    second["City"] = std::string("Arlington");
    (*source).set_records({BindingRecord("person.1", first), BindingRecord("person.2", second)});
    (*grid).set_data_source(source);
    window.perform_layout();
    require((*grid).row_source() == DataGridViewRowSource::binding_source && (*grid).row_count() == 2U &&
                (*grid).cell_text(row_at(1U), 0U) == "Grace" && (*grid).row_id(row_at(1U)) == id("person.2"),
            "bound rows are the source's records");
    require((*source).set_position(1) && (*grid).current_cell() == cell_at(1U, 0U),
            "the current row follows the source's position");
    require((*grid).set_current_cell(cell_at(0U, 1U)) && (*source).position() == 0,
            "moving the current row moves the position");
    require((*grid).begin_edit(), "a bound cell edits");
    (*(*grid).editing_control()).set_text("Paris");
    require((*grid).end_edit() && (*source).current_field("City") == BindingValue{std::string("Paris")} &&
                (*grid).cell_text(row_at(0U), 1U) == "Paris",
            "a committed edit sets the source's field");
    static_cast<void>((*source).add(BindingRecord("person.3", first)));
    require((*grid).row_count() == 3U, "an added record is a new row");
    (*grid).set_cell_selected(row_at(2U), 0U, true);
    static_cast<void>((*source).insert(0U, BindingRecord("person.0", second)));
    require((*grid).current_row_id() == id("person.1") && (*grid).current_cell() == cell_at(1U, 1U) &&
                (*grid).is_cell_selected(row_at(3U), 0U) && (*source).position() == 1,
            "an inserted record keeps the current cell and the selection on their records");
    require((*source).remove_at(1U) && (*grid).row_count() == 3U && (*grid).row_position(id("person.1")) == std::nullopt,
            "a removed record leaves the grid");
    // As for unbound rows: the record that took the removed record's place
    // is current (the source's position agrees), and the selection keeps
    // the records that remain.
    require((*grid).current_row_id() == id("person.2") && (*grid).current_cell() == cell_at(1U, 1U) &&
                (*source).position() == 1,
            "the record that takes the removed current record's place becomes current");
    require((*grid).is_cell_selected(row_at(2U), 0U) && (*grid).selected_cells().size() == 1U,
            "the removed record leaves the selection and person.3 stays selected");
    (*grid).set_data_source(nullptr);
    require((*grid).row_source() == DataGridViewRowSource::unbound && (*grid).row_count() == 0U,
            "a null source returns to empty unbound rows");
}

void scrolling() {
    std::shared_ptr<DataGridView> grid = make_grid("grid.scroll");
    (*grid).set_row_count(40U);
    Window window(grid, {200.0, 140.0});
    window.perform_layout();
    const DataGridView::Parts parts = (*grid).parts();
    require(parts.vertical_scroll_bar.present && parts.horizontal_scroll_bar.present && parts.scroll_corner.present,
            "content larger than the grid shows both bars and the corner");
    require(window.request_focus(grid), "the grid takes focus");
    for (int step = 0; step < 10; ++step) static_cast<void>(key(window, PhysicalKey::down));
    window.perform_layout();
    require((*grid).current_cell() == cell_at(10U, 0U) && (*grid).first_displayed_row().value > 0U &&
                (*grid).displayed_row_end().value > 10U,
            "moving below the displayed rows scrolls them");
    require(static_cast<std::size_t>((*(*grid).vertical_scroll_bar()).value()) == (*grid).first_displayed_row().value,
            "the vertical bar follows the first displayed row");
    require(key(window, PhysicalKey::end) && (*grid).horizontal_scroll_offset() > 0.0,
            "moving to the last column scrolls it into view");
    window.perform_layout();
    PointerEvent wheel{PointerAction::wheel, PointerButton::none, centre(*grid, (*grid).parts().well.bounds)};
    wheel.wheel_delta = {0.0, 1.0};
    const std::size_t before = (*grid).first_displayed_row().value;
    require(window.dispatch_pointer(wheel) && (*grid).first_displayed_row().value + 3U == before,
            "a wheel notch scrolls three rows");
    window.perform_layout();
    (*(*grid).vertical_scroll_bar()).set_value(20.0);
    require((*grid).first_displayed_row() == row_at(20U), "moving the bar scrolls the rows");
}

void semantics() {
    std::shared_ptr<DataGridView> grid = make_grid("grid.semantics");
    (*grid).set_accessible_name("Stations");
    Window window(grid, {360.0, 160.0});
    window.perform_layout();
    const SemanticDescriptor descriptor = (*grid).semantic_descriptor();
    require(descriptor.role == SemanticRole::list && descriptor.name == "Stations",
            "the grid is a named list to accessibility");
    const std::vector<SemanticNode> nodes = (*grid).semantic_virtual_children();
    require(nodes.size() == 7U && nodes[0].role == SemanticRole::button &&
                nodes[0].stable_id == "grid.semantics.column.name",
            "column headers are buttons before the rows");
    const SemanticNode& row = nodes[4];
    require(row.role == SemanticRole::list_item && row.stable_id == "alpha" && row.children.size() == 4U,
            "each row is a list item holding its cells");
    require(row.children[0].role == SemanticRole::text_box && row.children[0].value == "Alpha" &&
                row.children[1].role == SemanticRole::check_box &&
                has_semantic_state(row.children[1].states, SemanticState::checked) &&
                row.children[2].role == SemanticRole::combo_box && row.children[3].role == SemanticRole::button,
            "cells take the nearest roles of their kinds");
    require((*grid).on_semantic_child_action("alpha.active", SemanticAction::press, {}) &&
                (*grid).cell_value(row_at(0U), 1U) == BindingValue{false},
            "press toggles a check box cell");
    require((*grid).on_semantic_child_action("bravo.name", SemanticAction::set_value, "Foxtrot") &&
                (*grid).cell_text(row_at(1U), 0U) == "Foxtrot" && !(*grid).is_current_cell_in_edit_mode(),
            "set_value commits a text cell");
    require((*grid).on_semantic_child_action("charlie.mode", SemanticAction::set_value, "USB") &&
                (*grid).cell_text(row_at(2U), 2U) == "USB",
            "set_value chooses a combo box item");
    require(!(*grid).on_semantic_child_action("charlie.mode", SemanticAction::set_value, "SSB"),
            "a value that is no item is refused");
    require((*grid).on_semantic_child_action("bravo", SemanticAction::select, {}) &&
                (*grid).current_row_id() == id("bravo"),
            "selecting a row node by its ID makes its row current");
    std::vector<std::size_t> header_clicks;
    SubscriptionToken token = (*grid).column_header_click().subscribe(RecordIndex{&header_clicks});
    require((*grid).on_semantic_child_action("grid.semantics.column.mode", SemanticAction::press, {}) &&
                header_clicks.size() == 1U && header_clicks[0] == 2U,
            "pressing a header node raises column_header_click");
}

} // namespace

int main() {
    columns_and_rows();
    layout_and_parts();
    selection_and_navigation();
    text_editing();
    check_button_and_combo_cells();
    virtual_rows();
    virtual_row_identity();
    unbound_row_identity();
    sorting_headers_and_resizing();
    bound_rows();
    scrolling();
    semantics();
    std::cout << "DataGridView: columns, rows, parts, selection, keys, editing, cell kinds, virtual rows, "
                 "row identity, sorting, resizing, binding, scrolling and semantics passed\n";
    return 0;
}
