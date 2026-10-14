// DataGridView (stage W): columns and unbound rows, layout and published
// parts, selection and keyboard navigation, text editing in the hosted
// editor, check box, button and combo box cells, virtual rows with a provider,
// sorting, header clicks and column resizing, BindingSource rows, scrolling and
// semantics.

#include "gui_forms/gui_forms.hpp"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {
using namespace gui_forms;

void require(bool condition, const char* message) {
    if (condition) return;
    std::cerr << message << '\n';
    std::exit(1);
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

// Rows whose values are computed; counts what the grid asks.
class CountingProvider final : public DataGridViewCellValueProvider {
public:
    [[nodiscard]] BindingValue cell_value(std::size_t row, std::size_t column) const override {
        ++asked;
        if (row > highest) highest = row;
        if (column == 1U) return BindingValue{(row % 2U) == 0U};
        return BindingValue{"r" + std::to_string(row) + "c" + std::to_string(column)};
    }
    bool push_cell_value(std::size_t row, std::size_t column, const BindingValue& value) override {
        ++pushed;
        if (refuse) return false;
        last_row = row;
        last_column = column;
        last_value = value;
        return true;
    }

    mutable std::size_t asked{};
    mutable std::size_t highest{};
    std::size_t pushed{};
    bool refuse{};
    std::size_t last_row{};
    std::size_t last_column{};
    BindingValue last_value{};
};

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

std::shared_ptr<DataGridView> make_grid(const char* id) {
    std::shared_ptr<DataGridView> grid = make_control<DataGridView>(StableId(id));
    (*grid).set_columns(sample_columns());
    (*grid).add_row({.stable_id = "alpha", .cells = {BindingValue{std::string("Alpha")}, BindingValue{true},
                                                    BindingValue{std::string("FM")}}});
    (*grid).add_row({.stable_id = "bravo", .cells = {BindingValue{std::string("Bravo")}, BindingValue{false},
                                                    BindingValue{std::string("AM")}}});
    (*grid).add_row({.stable_id = "charlie", .cells = {BindingValue{std::string("Charlie")}}});
    return grid;
}

Point centre(const DataGridView& grid, Rect local) {
    const Rect absolute = grid.absolute_bounds();
    const Point point{absolute.x + local.x + local.width * 0.5, absolute.y + local.y + local.height * 0.5};
    return point;
}

const DataGridView::Cell* find_cell(const std::vector<DataGridView::Cell>& cells, std::size_t row,
                                    std::size_t column) {
    for (const DataGridView::Cell& cell : cells) {
        if (cell.address.row == row && cell.address.column == column) return &cell;
    }
    return nullptr;
}

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

template <typename Action>
bool throws_invalid(Action action) {
    try {
        action();
    } catch (const std::invalid_argument&) {
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
    void operator()() const { static_cast<void>((*grid).add_row({.stable_id = "alpha"})); }
};
struct AddItemsToText final {
    DataGridView* grid{};
    void operator()() const { (*grid).add_column({.name = "extra", .items = {"x"}}); }
};
struct NarrowColumn final {
    DataGridView* grid{};
    void operator()() const { (*grid).set_column_width(0U, 5.0); }
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
    require((*grid).cell_text(0U, 0U) == "Alpha" && (*grid).cell_value(0U, 1U) == BindingValue{true},
            "cells keep their binding values");
    require((*grid).cell_text(2U, 3U) == "Tune" && (*grid).cell_value(2U, 2U) == BindingValue{},
            "short rows are padded, and an empty button cell shows its column's text");
    require((*grid).row_index("bravo") == 1U && (*grid).row_stable_id(2U) == "charlie",
            "rows keep their stable IDs");
    const std::size_t generated = (*grid).add_row({});
    require((*grid).row_stable_id(generated).rfind("row.", 0U) == 0U, "an empty row ID is generated");
    (*grid).insert_row(0U, {.stable_id = "zulu"});
    require((*grid).row_index("alpha") == 1U, "inserting moves later rows");
    (*grid).remove_row_at(0U);
    require((*grid).row_index("alpha") == 0U && (*grid).row_count() == 4U, "removing moves them back");
    (*grid).set_row_count(6U);
    require((*grid).row_count() == 6U, "an unbound row count grows with generated rows");
    (*grid).set_row_count(3U);
    require((*grid).row_count() == 3U && (*grid).row_index("charlie") == 2U, "and is cut from the end");
    require(throws_invalid(AddDottedColumn{grid.get()}), "a column name with '.' is refused");
    require(throws_invalid(AddDuplicateRow{grid.get()}), "a duplicate row ID is refused");
    require(throws_invalid(AddItemsToText{grid.get()}), "items on a text column are refused");
    require(throws_invalid(NarrowColumn{grid.get()}), "a width below the minimum is refused");
    bool past_end = false;
    try {
        static_cast<void>((*grid).cell_value(9U, 0U));
    } catch (const std::out_of_range&) {
        past_end = true;
    }
    require(past_end, "a cell past the rows is refused");
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
    require((*grid).current_cell() == DataGridViewCellAddress{0U, 0U} && (*grid).is_cell_selected(0U, 0U),
            "focus makes the first cell current and selected");
    require(currents.size() == 1U && selection_changes == 1, "the first current cell is reported once");
    require(key(window, PhysicalKey::right) && (*grid).current_cell() == DataGridViewCellAddress{0U, 1U},
            "Right moves to the next column");
    require(key(window, PhysicalKey::down, Modifier::shift) &&
                (*grid).current_cell() == DataGridViewCellAddress{1U, 1U} && (*grid).selected_cells().size() == 2U,
            "Shift+Down extends the selection from the anchor");
    require(key(window, PhysicalKey::end, Modifier::control) &&
                (*grid).current_cell() == DataGridViewCellAddress{2U, 3U} && (*grid).selected_cells().size() == 1U,
            "Ctrl+End moves to the last cell and selects it alone");
    require(key(window, PhysicalKey::home) && (*grid).current_cell() == DataGridViewCellAddress{2U, 0U},
            "Home moves to the row's first cell");
    require(key(window, PhysicalKey::tab, Modifier::shift) &&
                (*grid).current_cell() == DataGridViewCellAddress{1U, 3U},
            "Shift+Tab from a row's first cell moves to the previous row's last cell");
    require(key(window, PhysicalKey::tab) && (*grid).current_cell() == DataGridViewCellAddress{2U, 0U},
            "Tab wraps to the next row");
    require(key(window, PhysicalKey::page_up) && (*grid).current_cell() == DataGridViewCellAddress{0U, 0U},
            "Page Up moves up by a page");
    const std::vector<DataGridView::Row> rows = (*grid).realized_rows();
    require(rows[0].states.current && rows[0].parts.row_glyph.present, "the current row shows its glyph");

    const std::vector<DataGridView::Cell> cells = (*grid).realized_cells();
    click(window, centre(*grid, (*find_cell(cells, 2U, 0U)).parts.cell.bounds), Modifier::control);
    require((*grid).is_cell_selected(0U, 0U) && (*grid).is_cell_selected(2U, 0U),
            "Ctrl+click adds a cell to the selection");
    click(window, centre(*grid, (*find_cell(cells, 2U, 0U)).parts.cell.bounds), Modifier::control);
    require(!(*grid).is_cell_selected(2U, 0U), "Ctrl+click again removes it");

    (*grid).set_selection_mode(DataGridViewSelectionMode::full_row_select);
    click(window, centre(*grid, (*find_cell(cells, 1U, 0U)).parts.cell.bounds));
    require((*grid).selected_rows().size() == 1U && (*grid).selected_rows()[0] == 1U &&
                (*grid).is_cell_selected(1U, 3U) && (*grid).realized_rows()[1].states.selected,
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
    require(!(*grid).is_current_cell_in_edit_mode() && (*grid).cell_text(0U, 0U) == "Delta",
            "the committed text is the cell's value");
    require(changed.size() == 1U && ended.size() == 1U && ended[0].row == 0U,
            "a commit raises cell_value_changed then cell_end_edit");
    const std::shared_ptr<Control> grid_control = grid;
    require((*grid).current_cell() == DataGridViewCellAddress{1U, 0U} && window.focused_control() == grid_control,
            "Enter moves to the cell below and focus returns to the grid");

    require(window.dispatch_text({"X"}) && (*grid).is_current_cell_in_edit_mode() &&
                (*(*grid).editing_control()).text() == "X",
            "typing on a text cell begins an edit with the typed text");
    require(key(window, PhysicalKey::escape) && !(*grid).is_current_cell_in_edit_mode() &&
                (*grid).cell_text(1U, 0U) == "Bravo" && ended.size() == 2U && changed.size() == 1U,
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
    const DataGridView::Cell* refused = find_cell((*grid).realized_cells(), 1U, 0U);
    require(refused != nullptr && (*refused).states.invalid, "the refused cell publishes invalid");
    (*(*grid).editing_control()).set_text("Echo");
    require(key(window, PhysicalKey::tab) && !(*grid).is_current_cell_in_edit_mode() &&
                (*grid).cell_text(1U, 0U) == "Echo" && (*grid).current_cell() == DataGridViewCellAddress{1U, 1U},
            "Tab commits and moves to the next cell");

    // A number column converts the typed text and refuses what is not a number.
    (*grid).add_column({.name = "frequency", .header_text = "MHz", .width = 60.0,
                        .value_kind = BindingValueKind::number});
    require((*grid).set_current_cell(DataGridViewCellAddress{0U, 4U}) && (*grid).begin_edit(), "edit a number");
    (*(*grid).editing_control()).set_text("abc");
    require(!(*grid).end_edit() && (*grid).is_current_cell_in_edit_mode(), "text that is not a number is refused");
    (*(*grid).editing_control()).set_text("7.25");
    require((*grid).end_edit() && (*grid).cell_value(0U, 4U) == BindingValue{7.25} &&
                (*grid).cell_text(0U, 4U) == "7.25",
            "a number commits as a number and shows its shortest text");

    // Read-only cells do not edit; a double click edits a text cell.
    (*grid).set_column_read_only(0U, true);
    require((*grid).set_current_cell(DataGridViewCellAddress{2U, 0U}) && !(*grid).begin_edit(),
            "a read-only column does not edit");
    (*grid).set_column_read_only(0U, false);
    window.perform_layout();
    const DataGridView::Cell* target = find_cell((*grid).realized_cells(), 2U, 0U);
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

    require((*grid).set_current_cell(DataGridViewCellAddress{0U, 1U}), "a check box cell becomes current");
    require(key(window, PhysicalKey::space) && (*grid).cell_value(0U, 1U) == BindingValue{false} &&
                changed.size() == 1U && clicked.size() == 1U,
            "Space toggles a check box cell");
    const DataGridView::Cell* check = find_cell((*grid).realized_cells(), 1U, 1U);
    click(window, centre(*grid, (*check).parts.cell_check_well.bounds));
    require((*grid).cell_value(1U, 1U) == BindingValue{true} &&
                (*find_cell((*grid).realized_cells(), 1U, 1U)).states.checked,
            "clicking the check well toggles and publishes checked");
    (*grid).set_read_only(true);
    require(key(window, PhysicalKey::space) && (*grid).cell_value(1U, 1U) == BindingValue{true},
            "a read-only grid does not toggle");
    (*grid).set_read_only(false);

    const DataGridView::Cell* button = find_cell((*grid).realized_cells(), 2U, 3U);
    const Point face = centre(*grid, (*button).parts.cell_button_face.bounds);
    require(window.dispatch_pointer({PointerAction::down, PointerButton::primary, face}) &&
                (*find_cell((*grid).realized_cells(), 2U, 3U)).states.pressed,
            "a held button cell publishes pressed");
    static_cast<void>(window.dispatch_pointer({PointerAction::up, PointerButton::primary, face}));
    require(clicked.size() == 3U && clicked.back().row == 2U && clicked.back().column == 3U,
            "releasing on the button cell raises cell_content_click");

    require((*grid).set_current_cell(DataGridViewCellAddress{0U, 2U}), "a combo box cell becomes current");
    require(key(window, PhysicalKey::f4) && (*grid).drop_down_open() && (*grid).is_current_cell_in_edit_mode() &&
                (*find_cell((*grid).realized_cells(), 0U, 2U)).states.open,
            "F4 opens the combo box cell's choices");
    const std::size_t before = changed.size();
    require(key(window, PhysicalKey::down) && key(window, PhysicalKey::enter), "a choice is made by keyboard");
    require(!(*grid).drop_down_open() && !(*grid).is_current_cell_in_edit_mode() &&
                (*grid).cell_text(0U, 2U) == "FM" && changed.size() == before,
            "choosing the current value changes nothing");
    require(key(window, PhysicalKey::down, Modifier::alt) && (*grid).drop_down_open(), "Alt+Down opens the choices");
    require(key(window, PhysicalKey::end) && key(window, PhysicalKey::enter) && (*grid).cell_text(0U, 2U) == "USB" &&
                changed.size() == before + 1U,
            "a different choice becomes the cell's value");
    const DataGridView::Cell* combo = find_cell((*grid).realized_cells(), 1U, 2U);
    click(window, centre(*grid, (*combo).parts.cell_drop_down.bounds));
    require((*grid).drop_down_open() && (*grid).current_cell() == DataGridViewCellAddress{1U, 2U},
            "clicking a drop-down actuator opens that cell's choices");
    (*grid).close_drop_down();
    require(!(*grid).is_current_cell_in_edit_mode(), "closing the choices ends the edit");
}

void virtual_rows() {
    std::shared_ptr<CountingProvider> provider = std::make_shared<CountingProvider>();
    std::shared_ptr<DataGridView> grid = make_control<DataGridView>(StableId("grid.virtual"));
    (*grid).set_columns({{.name = "label", .header_text = "Label", .width = 120.0},
                         {.name = "even", .header_text = "Even", .kind = DataGridViewColumnKind::check_box,
                          .width = 50.0}});
    (*grid).set_virtual_rows(provider, 1000000U);
    Window window(grid, {300.0, 200.0});
    window.perform_layout();
    require((*grid).row_source() == DataGridViewRowSource::virtual_rows && (*grid).row_count() == 1000000U,
            "virtual rows are counted by the grid");
    require((*grid).parts().vertical_scroll_bar.present, "a million rows show a vertical scroll bar");
    const std::size_t shown = (*grid).displayed_row_end() - (*grid).first_displayed_row();
    require(shown > 0U && shown < 20U, "only a window of rows is displayed");
    (*provider).asked = 0U;
    CountingPainter painter;
    (*grid).on_paint(painter, (*grid).client_rectangle());
    require((*provider).asked > 0U && (*provider).asked <= shown * 2U * 3U && (*provider).highest < shown,
            "paint asks only for the displayed rows' values");
    require((*grid).realized_cells().size() == shown * 2U, "only displayed cells are realized");
    require((*grid).realized_rows()[0].stable_id == "row.0" && (*grid).cell_text(5U, 0U) == "r5c0",
            "virtual rows take their IDs and values from the provider");

    (*grid).set_first_displayed_row(500000U);
    window.perform_layout();
    (*provider).asked = 0U;
    (*provider).highest = 0U;
    CountingPainter later;
    (*grid).on_paint(later, (*grid).client_rectangle());
    require((*provider).highest >= 500000U && (*provider).highest < 500000U + shown,
            "scrolled rows are asked for where they are");

    require(window.request_focus(grid), "the grid takes focus");
    require((*grid).current_cell() == DataGridViewCellAddress{500000U, 0U},
            "the first displayed cell becomes current");
    require(window.dispatch_text({"new"}) && key(window, PhysicalKey::enter), "an edit commits through the provider");
    require((*provider).pushed == 1U && (*provider).last_row == 500000U &&
                (*provider).last_value == BindingValue{std::string("new")},
            "the provider receives the committed value");
    (*provider).refuse = true;
    require((*grid).set_current_cell(DataGridViewCellAddress{500000U, 1U}) && key(window, PhysicalKey::space) &&
                (*provider).pushed == 2U,
            "a toggle pushes to the provider");
    require(!(*grid).set_cell_value(3U, 0U, BindingValue{std::string("x")}), "a refused value reports false");
    require(key(window, PhysicalKey::end, Modifier::control) &&
                (*grid).current_cell() == DataGridViewCellAddress{999999U, 1U},
            "Ctrl+End moves to the last virtual row");
    window.perform_layout();
    require((*grid).displayed_row_end() == 1000000U, "and scrolls it into view");
}

void sorting_headers_and_resizing() {
    std::shared_ptr<DataGridView> grid = make_grid("grid.sort");
    static_cast<void>((*grid).set_cell_value(2U, 0U, BindingValue{std::string("Able")}));
    Window window(grid, {360.0, 160.0});
    window.perform_layout();
    std::vector<std::size_t> header_clicks;
    std::vector<std::size_t> widths;
    SubscriptionToken header_token = (*grid).column_header_click().subscribe(RecordIndex{&header_clicks});
    SubscriptionToken width_token = (*grid).column_width_changed().subscribe(RecordIndex{&widths});
    require((*grid).set_current_cell(DataGridViewCellAddress{0U, 0U}), "Alpha is current");

    std::vector<DataGridView::Column> columns = (*grid).realized_columns();
    click(window, centre(*grid, columns[0].parts.column_label.bounds));
    require(header_clicks.size() == 1U && header_clicks[0] == 0U, "clicking a header raises column_header_click");
    require((*grid).sort(0U, DataGridViewSortDirection::ascending), "unbound rows sort");
    require((*grid).cell_text(0U, 0U) == "Able" && (*grid).cell_text(1U, 0U) == "Alpha" &&
                (*grid).cell_text(2U, 0U) == "Bravo",
            "rows are ordered by the column's text");
    require((*grid).current_cell() == DataGridViewCellAddress{1U, 0U} && (*grid).is_cell_selected(1U, 0U),
            "the current cell and selection follow their row");
    window.perform_layout();
    columns = (*grid).realized_columns();
    require(columns[0].states.sorted_ascending && columns[0].parts.sort_glyph.present &&
                (*grid).columns()[0].sort_glyph == DataGridViewSortDirection::ascending,
            "the sorted column shows its glyph and publishes sorted_ascending");
    require((*grid).sort(1U, DataGridViewSortDirection::descending) &&
                (*grid).cell_value(0U, 1U) == BindingValue{true} &&
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
    (*virtual_grid).set_virtual_rows(std::make_shared<CountingProvider>(), 10U);
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
                (*grid).cell_text(1U, 0U) == "Grace" && (*grid).row_stable_id(1U) == "person.2",
            "bound rows are the source's records");
    require((*source).set_position(1) && (*grid).current_cell() == DataGridViewCellAddress{1U, 0U},
            "the current row follows the source's position");
    require((*grid).set_current_cell(DataGridViewCellAddress{0U, 1U}) && (*source).position() == 0,
            "moving the current row moves the position");
    require((*grid).begin_edit(), "a bound cell edits");
    (*(*grid).editing_control()).set_text("Paris");
    require((*grid).end_edit() && (*source).current_field("City") == BindingValue{std::string("Paris")} &&
                (*grid).cell_text(0U, 1U) == "Paris",
            "a committed edit sets the source's field");
    static_cast<void>((*source).add(BindingRecord("person.3", first)));
    require((*grid).row_count() == 3U, "an added record is a new row");
    require((*source).remove_at(0U) && (*grid).row_count() == 2U && (*grid).row_index("person.1") == std::nullopt,
            "a removed record leaves the grid");
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
    require((*grid).current_cell() == DataGridViewCellAddress{10U, 0U} && (*grid).first_displayed_row() > 0U &&
                (*grid).displayed_row_end() > 10U,
            "moving below the displayed rows scrolls them");
    require(static_cast<std::size_t>((*(*grid).vertical_scroll_bar()).value()) == (*grid).first_displayed_row(),
            "the vertical bar follows the first displayed row");
    require(key(window, PhysicalKey::end) && (*grid).horizontal_scroll_offset() > 0.0,
            "moving to the last column scrolls it into view");
    window.perform_layout();
    PointerEvent wheel{PointerAction::wheel, PointerButton::none, centre(*grid, (*grid).parts().well.bounds)};
    wheel.wheel_delta = {0.0, 1.0};
    const std::size_t before = (*grid).first_displayed_row();
    require(window.dispatch_pointer(wheel) && (*grid).first_displayed_row() + 3U == before,
            "a wheel notch scrolls three rows");
    window.perform_layout();
    (*(*grid).vertical_scroll_bar()).set_value(20.0);
    require((*grid).first_displayed_row() == 20U, "moving the bar scrolls the rows");
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
                (*grid).cell_value(0U, 1U) == BindingValue{false},
            "press toggles a check box cell");
    require((*grid).on_semantic_child_action("bravo.name", SemanticAction::set_value, "Foxtrot") &&
                (*grid).cell_text(1U, 0U) == "Foxtrot" && !(*grid).is_current_cell_in_edit_mode(),
            "set_value commits a text cell");
    require((*grid).on_semantic_child_action("charlie.mode", SemanticAction::set_value, "USB") &&
                (*grid).cell_text(2U, 2U) == "USB",
            "set_value chooses a combo box item");
    require(!(*grid).on_semantic_child_action("charlie.mode", SemanticAction::set_value, "SSB"),
            "a value that is no item is refused");
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
    sorting_headers_and_resizing();
    bound_rows();
    scrolling();
    semantics();
    std::cout << "DataGridView: columns, rows, parts, selection, keys, editing, cell kinds, virtual rows, "
                 "sorting, resizing, binding, scrolling and semantics passed\n";
    return 0;
}
