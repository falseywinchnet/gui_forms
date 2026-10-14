#include "gui_forms/controls/data_grid_view/data_grid_view.hpp"

#include "../scrollable_control/scrolling_utilities.hpp"

#include "gui_forms/binding/binding_source/binding_source.hpp"
#include "gui_forms/delegate.hpp"
#include "gui_forms/text.hpp"
#include "gui_forms/window.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <stdexcept>
#include <system_error>
#include <unordered_set>
#include <utility>

namespace gui_forms {
namespace {

// Selected cells are kept sorted by row, then column.
struct CellAddressOrder final {
    bool operator()(const DataGridViewCellAddress& left,
                    const DataGridViewCellAddress& right) const noexcept {
        if (left.row != right.row) return left.row < right.row;
        return left.column < right.column;
    }
};

// The rank of a value's kind in sort order: empty values first, then
// booleans, numbers, text, and every other kind by its text.
int value_rank(const BindingValue& value) noexcept {
    const BindingValueKind kind = binding_value_kind(value);
    if (kind == BindingValueKind::null) return 0;
    if (kind == BindingValueKind::boolean) return 1;
    if (kind == BindingValueKind::signed_integer || kind == BindingValueKind::unsigned_integer ||
        kind == BindingValueKind::number) {
        return 2;
    }
    if (kind == BindingValueKind::text) return 3;
    return 4;
}

// Orders two cell values: by rank, then booleans false first, numbers as
// doubles, text and other kinds by their text in byte order.
bool value_less(const BindingValue& left, const BindingValue& right) {
    const int left_rank = value_rank(left);
    const int right_rank = value_rank(right);
    if (left_rank != right_rank) return left_rank < right_rank;
    if (left_rank == 0) return false;
    if (left_rank == 1) {
        const bool left_value = std::get<bool>(left);
        const bool right_value = std::get<bool>(right);
        return !left_value && right_value;
    }
    if (left_rank == 2) {
        const double left_value = binding_value_to_number(left).value_or(0.0);
        const double right_value = binding_value_to_number(right).value_or(0.0);
        return left_value < right_value;
    }
    const std::string left_text = binding_value_to_string(left);
    const std::string right_text = binding_value_to_string(right);
    return left_text < right_text;
}

// Orders unbound row positions by one column's values for a stable sort.
struct RowValueOrder final {
    const std::vector<DataGridViewRow>* rows{};
    std::size_t column{};
    bool descending{};

    bool operator()(std::size_t left, std::size_t right) const {
        const BindingValue& left_value = (*rows)[left].cells[column];
        const BindingValue& right_value = (*rows)[right].cells[column];
        if (descending) return value_less(right_value, left_value);
        return value_less(left_value, right_value);
    }
};

bool contains_dot(std::string_view text) noexcept {
    const bool result = text.find('.') != std::string_view::npos;
    return result;
}

} // namespace

DataGridViewCellValueProvider::~DataGridViewCellValueProvider() = default;

std::string DataGridViewCellValueProvider::row_stable_id(std::size_t row) const {
    std::string identity = "row." + std::to_string(row);
    return identity;
}

DataGridView::DataGridView(StableId stable_id) : Control(std::move(stable_id)) {
    set_focusable(true);
    set_cursor(CursorKind::arrow);
    set_paint_plane(PaintPlane::control);
    choices_ = std::make_unique<ContextMenu>(std::string((*this).stable_id().value()) + ".choices");
    choice_invoked_ = (*choices_).item_invoked().subscribe(
        *this, Delegate<const MenuItemInvocation&>::bind<DataGridView, &DataGridView::on_choice_invoked>(*this));
    choices_changed_ = (*choices_).open_changed().subscribe(
        *this, Delegate<bool>::bind<DataGridView, &DataGridView::on_choices_open_changed>(*this));
}

DataGridView::~DataGridView() {
    close_drop_down();
}

void DataGridView::initialize_control_tree() {
    const std::string prefix(stable_id().value());
    editor_ = make_control<TextBox>(StableId(prefix + ".editor"));
    (*editor_).set_border_style(BorderStyle::none);
    (*editor_).set_font(font_);
    vertical_bar_ = make_control<VScrollBar>(StableId(prefix + ".vertical_scroll"));
    horizontal_bar_ = make_control<HScrollBar>(StableId(prefix + ".horizontal_scroll"));
    // The bars scroll the grid; keyboard focus stays on the grid.
    (*vertical_bar_).set_focusable(false);
    (*horizontal_bar_).set_focusable(false);
    add_child(editor_);
    add_child(vertical_bar_);
    add_child(horizontal_bar_);
    set_child_layout_collapsed(editor_, true);
    set_child_layout_collapsed(vertical_bar_, true);
    set_child_layout_collapsed(horizontal_bar_, true);
    editor_focus_ = (*editor_).focus_observed().subscribe(
        *this, Delegate<bool>::bind<DataGridView, &DataGridView::on_editor_focus>(*this));
    vertical_scrolled_ = (*vertical_bar_).value_changed().subscribe(
        *this, Delegate<double>::bind<DataGridView, &DataGridView::on_vertical_scroll>(*this));
    horizontal_scrolled_ = (*horizontal_bar_).value_changed().subscribe(
        *this, Delegate<double>::bind<DataGridView, &DataGridView::on_horizontal_scroll>(*this));
}

void DataGridView::on_dispose() noexcept {
    close_drop_down();
    if (choices_) (*choices_).dispose();
    detach_data_source();
    Control::on_dispose();
}

// -- model -------------------------------------------------------------------

std::size_t DataGridView::row_count() const noexcept {
    if (row_source_ == DataGridViewRowSource::virtual_rows) return virtual_row_count_;
    if (row_source_ == DataGridViewRowSource::binding_source) {
        if (!data_source_ || !(*data_source_).is_alive()) return 0U;
        return (*data_source_).count();
    }
    return rows_.size();
}

void DataGridView::require_cell(std::size_t row, std::size_t column) const {
    if (row >= row_count() || column >= columns_.size()) {
        throw std::out_of_range("DataGridView cell is past the rows or columns");
    }
}

BindingValue DataGridView::stored_value(std::size_t row, std::size_t column) const {
    if (row_source_ == DataGridViewRowSource::virtual_rows) {
        if (!provider_) return BindingValue{};
        return (*provider_).cell_value(row, column);
    }
    if (row_source_ == DataGridViewRowSource::binding_source) {
        const std::string& field = columns_[column].data_property_name;
        if (field.empty()) return BindingValue{};
        const BindingRecord& record = (*data_source_).records()[row];
        const std::string canonical = canonical_binding_name(field);
        const BindingRecord::FieldMap::const_iterator found = record.fields.find(canonical);
        if (found == record.fields.end()) return BindingValue{};
        return (*found).second;
    }
    return rows_[row].cells[column];
}

bool DataGridView::store_value(std::size_t row, std::size_t column, const BindingValue& value) {
    if (row_source_ == DataGridViewRowSource::virtual_rows) {
        if (!provider_) return false;
        const bool accepted = (*provider_).push_cell_value(row, column, value);
        return accepted;
    }
    if (row_source_ == DataGridViewRowSource::binding_source) {
        const std::string& field = columns_[column].data_property_name;
        if (field.empty()) return false;
        BindingSource& source = *data_source_;
        const std::ptrdiff_t position = static_cast<std::ptrdiff_t>(row);
        if (source.position() != position) {
            synchronizing_position_ = true;
            static_cast<void>(source.set_position(position));
            synchronizing_position_ = false;
        }
        const bool accepted = source.set_current_field(field, value);
        return accepted;
    }
    rows_[row].cells[column] = value;
    return true;
}

bool DataGridView::row_read_only(std::size_t row) const {
    if (row_source_ == DataGridViewRowSource::virtual_rows) return false;
    if (row_source_ == DataGridViewRowSource::binding_source) {
        const BindingRecord& record = (*data_source_).records()[row];
        const bool result = !record.editable || !(*data_source_).allow_edit();
        return result;
    }
    return rows_[row].read_only;
}

bool DataGridView::cell_read_only(std::size_t row, std::size_t column) const {
    if (read_only_ || columns_[column].read_only || row_read_only(row)) return true;
    const bool unbound_field = row_source_ == DataGridViewRowSource::binding_source &&
                               columns_[column].data_property_name.empty();
    return unbound_field;
}

std::string DataGridView::row_stable_id(std::size_t row) const {
    if (row >= row_count()) throw std::out_of_range("DataGridView row is past the end");
    if (row_source_ == DataGridViewRowSource::virtual_rows) {
        if (!provider_) return "row." + std::to_string(row);
        return (*provider_).row_stable_id(row);
    }
    if (row_source_ == DataGridViewRowSource::binding_source) {
        return (*data_source_).records()[row].stable_id;
    }
    return rows_[row].stable_id;
}

std::optional<std::size_t> DataGridView::row_index(std::string_view stable_id) const {
    const std::size_t count = row_count();
    if (row_source_ == DataGridViewRowSource::virtual_rows) {
        // Virtual rows are searched only where they are realized: the
        // displayed rows and the current row.
        const std::size_t end = std::min(count, displayed_row_end());
        for (std::size_t row = std::min(first_row_, end); row < end; ++row) {
            if (row_stable_id(row) == stable_id) return row;
        }
        if (current_ && (*current_).row < count && row_stable_id((*current_).row) == stable_id) {
            return (*current_).row;
        }
        return std::nullopt;
    }
    for (std::size_t row = 0; row < count; ++row) {
        if (row_stable_id(row) == stable_id) return row;
    }
    return std::nullopt;
}

BindingValue DataGridView::cell_value(std::size_t row, std::size_t column) const {
    require_cell(row, column);
    return stored_value(row, column);
}

namespace {

// The text a value shows in a cell. Numbers use the shortest text that reads
// back as the same value, not binding_value_to_string's seventeen digits.
std::string display_text(const BindingValue& value) {
    const BindingValueKind kind = binding_value_kind(value);
    if (kind == BindingValueKind::number) {
        const double number = std::get<double>(value);
        std::array<char, 32> buffer{};
        const std::to_chars_result written =
            std::to_chars(buffer.data(), buffer.data() + buffer.size(), number);
        if (written.ec == std::errc{}) {
            const std::string text(buffer.data(), written.ptr);
            return text;
        }
    }
    return binding_value_to_string(value);
}

} // namespace

std::string DataGridView::cell_text(std::size_t row, std::size_t column) const {
    require_cell(row, column);
    const BindingValue value = stored_value(row, column);
    std::string text = display_text(value);
    if (text.empty() && columns_[column].kind == DataGridViewColumnKind::button) {
        text = columns_[column].button_text;
    }
    return text;
}

bool DataGridView::set_cell_value(std::size_t row, std::size_t column, BindingValue value) {
    require_mutable();
    require_cell(row, column);
    const BindingValue previous = stored_value(row, column);
    if (previous == value) return true;
    const bool accepted = store_value(row, column, value);
    if (!accepted) return false;
    invalidate(Dirty::paint | Dirty::semantics);
    const DataGridViewCellEvent changed{row, column};
    cell_value_changed_.emit(changed);
    return true;
}

void DataGridView::structure_changed() {
    invalidate(Dirty::measure | Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

void DataGridView::reset_row_state() {
    cancel_edit();
    close_drop_down();
    const std::uint64_t before = selection_revision_;
    clear_selection_state();
    anchor_.reset();
    pressed_button_.reset();
    first_row_ = 0U;
    apply_current(std::nullopt);
    publish_selection(before);
}

// -- columns -----------------------------------------------------------------

void DataGridView::validate_column(const DataGridViewColumn& column) const {
    if (column.name.empty() || contains_dot(column.name) || !validate_utf8(column.name).valid() ||
        !validate_utf8(column.header_text).valid() || !validate_utf8(column.button_text).valid() ||
        !validate_utf8(column.data_property_name).valid()) {
        throw std::invalid_argument("DataGridView columns require a name without '.' and valid UTF-8");
    }
    if (!std::isfinite(column.width) || !std::isfinite(column.minimum_width) ||
        column.minimum_width < 1.0 || column.width < column.minimum_width) {
        throw std::invalid_argument("DataGridView column widths must be finite and at least the minimum");
    }
    if (column.kind != DataGridViewColumnKind::combo_box && !column.items.empty()) {
        throw std::invalid_argument("DataGridView column items belong to combo box columns only");
    }
    for (const std::string& item : column.items) {
        if (!validate_utf8(item).valid()) {
            throw std::invalid_argument("DataGridView combo box items require valid UTF-8");
        }
    }
}

void DataGridView::set_columns(std::vector<DataGridViewColumn> columns) {
    require_mutable();
    std::unordered_set<std::string> names{};
    for (const DataGridViewColumn& column : columns) {
        validate_column(column);
        if (!names.insert(column.name).second) {
            throw std::invalid_argument("DataGridView column names must be unique");
        }
    }
    reset_row_state();
    columns_ = std::move(columns);
    for (DataGridViewRow& row : rows_) row.cells.resize(columns_.size());
    horizontal_offset_ = 0.0;
    hot_column_.reset();
    pressed_column_.reset();
    resizing_column_.reset();
    structure_changed();
}

void DataGridView::add_column(DataGridViewColumn column) {
    require_mutable();
    validate_column(column);
    if (column_index(column.name)) {
        throw std::invalid_argument("DataGridView column names must be unique");
    }
    columns_.push_back(std::move(column));
    for (DataGridViewRow& row : rows_) row.cells.resize(columns_.size());
    structure_changed();
}

void DataGridView::remove_column_at(std::size_t column) {
    require_mutable();
    if (column >= columns_.size()) throw std::out_of_range("DataGridView column is past the end");
    reset_row_state();
    const std::ptrdiff_t offset = static_cast<std::ptrdiff_t>(column);
    columns_.erase(columns_.begin() + offset);
    for (DataGridViewRow& row : rows_) row.cells.erase(row.cells.begin() + offset);
    hot_column_.reset();
    pressed_column_.reset();
    resizing_column_.reset();
    structure_changed();
}

std::optional<std::size_t> DataGridView::column_index(std::string_view name) const noexcept {
    for (std::size_t index = 0; index < columns_.size(); ++index) {
        if (columns_[index].name == name) return index;
    }
    return std::nullopt;
}

void DataGridView::set_column_width(std::size_t column, double width) {
    require_mutable();
    if (column >= columns_.size()) throw std::out_of_range("DataGridView column is past the end");
    if (!std::isfinite(width) || width < columns_[column].minimum_width) {
        throw std::invalid_argument("DataGridView column width must be finite and at least the minimum");
    }
    if (columns_[column].width == width) return;
    columns_[column].width = width;
    structure_changed();
    column_width_changed_.emit(column);
}

void DataGridView::set_column_sort_glyph(std::size_t column, DataGridViewSortDirection direction) {
    require_mutable();
    if (column >= columns_.size()) throw std::out_of_range("DataGridView column is past the end");
    if (columns_[column].sort_glyph == direction) return;
    columns_[column].sort_glyph = direction;
    invalidate(Dirty::paint | Dirty::semantics);
}

void DataGridView::set_column_read_only(std::size_t column, bool read_only) {
    require_mutable();
    if (column >= columns_.size()) throw std::out_of_range("DataGridView column is past the end");
    if (columns_[column].read_only == read_only) return;
    if (read_only && editing_ && current_ && (*current_).column == column) cancel_edit();
    columns_[column].read_only = read_only;
    invalidate(Dirty::paint | Dirty::semantics);
}

// -- rows --------------------------------------------------------------------

std::string DataGridView::generated_row_id() {
    for (;;) {
        std::string candidate = "row." + std::to_string(next_row_number_);
        ++next_row_number_;
        bool used = false;
        for (const DataGridViewRow& row : rows_) {
            if (row.stable_id == candidate) {
                used = true;
                break;
            }
        }
        if (!used) return candidate;
    }
}

void DataGridView::validate_row(const DataGridViewRow& row, std::size_t replacing) const {
    if (!validate_utf8(row.stable_id).valid() || !validate_utf8(row.header_text).valid()) {
        throw std::invalid_argument("DataGridView rows require valid UTF-8");
    }
    if (row.cells.size() > columns_.size()) {
        throw std::invalid_argument("DataGridView row has more values than columns");
    }
    if (row.stable_id.empty()) return;
    for (std::size_t index = 0; index < rows_.size(); ++index) {
        if (index != replacing && rows_[index].stable_id == row.stable_id) {
            throw std::invalid_argument("DataGridView row stable IDs must be unique");
        }
    }
}

void DataGridView::prepare_row(DataGridViewRow& row) {
    if (row.stable_id.empty()) row.stable_id = generated_row_id();
    row.cells.resize(columns_.size());
}

std::size_t DataGridView::add_row(DataGridViewRow row) {
    const std::size_t index = rows_.size();
    insert_row(index, std::move(row));
    return index;
}

void DataGridView::insert_row(std::size_t index, DataGridViewRow row) {
    require_mutable();
    if (row_source_ != DataGridViewRowSource::unbound) {
        throw std::logic_error("DataGridView rows are added only to unbound rows");
    }
    if (index > rows_.size()) throw std::out_of_range("DataGridView row insertion index is past the end");
    validate_row(row, rows_.size());
    prepare_row(row);
    rows_.insert(rows_.begin() + static_cast<std::ptrdiff_t>(index), std::move(row));
    shift_rows_after_insert(index);
    structure_changed();
}

void DataGridView::set_rows(std::vector<DataGridViewRow> rows) {
    require_mutable();
    if (row_source_ != DataGridViewRowSource::unbound) {
        throw std::logic_error("DataGridView rows are set only on unbound rows");
    }
    std::unordered_set<std::string> identities{};
    for (const DataGridViewRow& row : rows) {
        if (!validate_utf8(row.stable_id).valid() || !validate_utf8(row.header_text).valid() ||
            row.cells.size() > columns_.size()) {
            throw std::invalid_argument("DataGridView rows require valid UTF-8 and no more values than columns");
        }
        if (!row.stable_id.empty() && !identities.insert(row.stable_id).second) {
            throw std::invalid_argument("DataGridView row stable IDs must be unique");
        }
    }
    reset_row_state();
    rows_ = std::move(rows);
    // Generated IDs skip every ID the new rows already use.
    for (DataGridViewRow& row : rows_) {
        if (row.stable_id.empty()) row.stable_id = generated_row_id();
        row.cells.resize(columns_.size());
    }
    structure_changed();
}

void DataGridView::remove_row_at(std::size_t row) {
    require_mutable();
    if (row_source_ != DataGridViewRowSource::unbound) {
        throw std::logic_error("DataGridView rows are removed only from unbound rows");
    }
    if (row >= rows_.size()) throw std::out_of_range("DataGridView row is past the end");
    if (editing_ && current_ && (*current_).row == row) cancel_edit();
    rows_.erase(rows_.begin() + static_cast<std::ptrdiff_t>(row));
    shift_rows_after_remove(row);
    structure_changed();
}

void DataGridView::clear_rows() {
    require_mutable();
    reset_row_state();
    detach_data_source();
    provider_.reset();
    virtual_row_count_ = 0U;
    rows_.clear();
    row_source_ = DataGridViewRowSource::unbound;
    structure_changed();
}

void DataGridView::set_row_count(std::size_t count) {
    require_mutable();
    if (row_source_ == DataGridViewRowSource::binding_source) {
        throw std::logic_error("DataGridView bound rows are counted by their source");
    }
    const std::size_t previous = row_count();
    if (count == previous) return;
    if (count < previous) {
        if (editing_ && current_ && (*current_).row >= count) cancel_edit();
        trim_rows(count);
    }
    if (row_source_ == DataGridViewRowSource::virtual_rows) {
        virtual_row_count_ = count;
    } else if (count < previous) {
        rows_.resize(count);
    } else {
        rows_.reserve(count);
        while (rows_.size() < count) {
            DataGridViewRow row;
            prepare_row(row);
            rows_.push_back(std::move(row));
        }
    }
    structure_changed();
}

void DataGridView::set_virtual_rows(std::shared_ptr<DataGridViewCellValueProvider> provider,
                                    std::size_t count) {
    require_mutable();
    if (!provider) throw std::invalid_argument("DataGridView virtual rows require a provider");
    reset_row_state();
    detach_data_source();
    rows_.clear();
    provider_ = std::move(provider);
    virtual_row_count_ = count;
    row_source_ = DataGridViewRowSource::virtual_rows;
    structure_changed();
}

void DataGridView::refresh_rows() {
    require_mutable();
    invalidate(Dirty::paint | Dirty::semantics);
}

void DataGridView::detach_data_source() noexcept {
    source_list_changed_.disconnect();
    source_position_changed_.disconnect();
    data_source_.reset();
}

void DataGridView::set_data_source(std::shared_ptr<BindingSource> source) {
    require_mutable();
    reset_row_state();
    detach_data_source();
    provider_.reset();
    virtual_row_count_ = 0U;
    rows_.clear();
    if (!source) {
        row_source_ = DataGridViewRowSource::unbound;
        structure_changed();
        return;
    }
    data_source_ = std::move(source);
    row_source_ = DataGridViewRowSource::binding_source;
    source_list_changed_ = (*data_source_).list_changed().subscribe(
        *this, Delegate<const BindingListChange&>::bind<DataGridView, &DataGridView::on_source_list_changed>(*this));
    source_position_changed_ = (*data_source_).position_changed().subscribe(
        *this, Delegate<std::ptrdiff_t>::bind<DataGridView, &DataGridView::on_source_position_changed>(*this));
    structure_changed();
    on_source_position_changed((*data_source_).position());
}

void DataGridView::on_source_list_changed(const BindingListChange& change) {
    if (row_source_ != DataGridViewRowSource::binding_source) return;
    const std::size_t count = row_count();
    if (change.kind == BindingListChangeKind::item_added && change.index >= 0) {
        shift_rows_after_insert(static_cast<std::size_t>(change.index));
    } else if (change.kind == BindingListChangeKind::item_removed && change.index >= 0) {
        const std::size_t removed = static_cast<std::size_t>(change.index);
        if (editing_ && current_ && (*current_).row == removed) cancel_edit();
        shift_rows_after_remove(removed);
    } else if (change.kind == BindingListChangeKind::reset) {
        if (editing_) cancel_edit();
        trim_rows(count);
    }
    structure_changed();
}

void DataGridView::on_source_position_changed(std::ptrdiff_t position) {
    if (synchronizing_position_ || row_source_ != DataGridViewRowSource::binding_source) return;
    if (position < 0 || columns_.empty()) return;
    const std::size_t row = static_cast<std::size_t>(position);
    if (row >= row_count()) return;
    const std::size_t column = current_ ? (*current_).column : 0U;
    synchronizing_position_ = true;
    static_cast<void>(move_current({row, column}, SelectionGesture::replace));
    synchronizing_position_ = false;
}

bool DataGridView::sort(std::size_t column, DataGridViewSortDirection direction) {
    require_mutable();
    if (column >= columns_.size()) throw std::out_of_range("DataGridView column is past the end");
    if (row_source_ != DataGridViewRowSource::unbound || direction == DataGridViewSortDirection::none) {
        return false;
    }
    if (!end_edit()) return false;
    const std::size_t count = rows_.size();
    // order[new position] = old position; inverse[old position] = new position.
    std::vector<std::size_t> order(count, 0U);
    for (std::size_t index = 0; index < count; ++index) order[index] = index;
    const RowValueOrder compare{&rows_, column, direction == DataGridViewSortDirection::descending};
    std::stable_sort(order.begin(), order.end(), compare);
    std::vector<std::size_t> inverse(count, 0U);
    std::vector<DataGridViewRow> sorted{};
    sorted.reserve(count);
    for (std::size_t index = 0; index < count; ++index) {
        inverse[order[index]] = index;
        sorted.push_back(std::move(rows_[order[index]]));
    }
    rows_ = std::move(sorted);
    for (DataGridViewCellAddress& cell : selected_cells_) cell.row = inverse[cell.row];
    std::sort(selected_cells_.begin(), selected_cells_.end(), CellAddressOrder{});
    for (std::size_t& row : selected_rows_) row = inverse[row];
    std::sort(selected_rows_.begin(), selected_rows_.end());
    if (current_) (*current_).row = inverse[(*current_).row];
    if (anchor_) (*anchor_).row = inverse[(*anchor_).row];
    pressed_button_.reset();
    for (std::size_t index = 0; index < columns_.size(); ++index) {
        columns_[index].sort_glyph = index == column ? direction : DataGridViewSortDirection::none;
    }
    structure_changed();
    return true;
}

// -- appearance and scrolling --------------------------------------------------

void DataGridView::set_column_headers_visible(bool visible) {
    require_mutable();
    if (column_headers_visible_ == visible) return;
    column_headers_visible_ = visible;
    structure_changed();
}

void DataGridView::set_row_headers_visible(bool visible) {
    require_mutable();
    if (row_headers_visible_ == visible) return;
    row_headers_visible_ = visible;
    structure_changed();
}

void DataGridView::set_column_headers_height(double height) {
    require_mutable();
    if (!std::isfinite(height) || height < 4.0 || height > 256.0) {
        throw std::out_of_range("DataGridView column header height must be between 4 and 256");
    }
    if (column_headers_height_ == height) return;
    column_headers_height_ = height;
    structure_changed();
}

void DataGridView::set_row_headers_width(double width) {
    require_mutable();
    if (!std::isfinite(width) || width < 4.0 || width > 1024.0) {
        throw std::out_of_range("DataGridView row header width must be between 4 and 1024");
    }
    if (row_headers_width_ == width) return;
    row_headers_width_ = width;
    structure_changed();
}

void DataGridView::set_row_height(double height) {
    require_mutable();
    if (!std::isfinite(height) || height < 4.0 || height > 256.0) {
        throw std::out_of_range("DataGridView row height must be between 4 and 256");
    }
    if (row_height_ == height) return;
    row_height_ = height;
    structure_changed();
}

void DataGridView::set_font(FontSpec font) {
    require_mutable();
    if (!valid_font_spec(font)) throw std::invalid_argument("DataGridView font specification is invalid");
    if (font_ == font) return;
    font_ = font;
    if (editor_) (*editor_).set_font(font_);
    structure_changed();
}

void DataGridView::set_read_only(bool read_only) {
    require_mutable();
    if (read_only_ == read_only) return;
    if (read_only) cancel_edit();
    read_only_ = read_only;
    invalidate(Dirty::paint | Dirty::semantics);
}

void DataGridView::set_allow_user_to_resize_columns(bool allow) {
    require_mutable();
    if (allow_column_resize_ == allow) return;
    allow_column_resize_ = allow;
    invalidate(Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

void DataGridView::set_first_displayed_row(std::size_t row) {
    require_mutable();
    const std::size_t count = row_count();
    const std::size_t last = count == 0U ? 0U : count - 1U;
    const std::size_t next = std::min(row, last);
    if (first_row_ == next) return;
    first_row_ = next;
    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

void DataGridView::set_horizontal_scroll_offset(double offset) {
    require_mutable();
    if (!std::isfinite(offset) || offset < 0.0) {
        throw std::invalid_argument("DataGridView horizontal offset must be finite and not negative");
    }
    if (horizontal_offset_ == offset) return;
    horizontal_offset_ = offset;
    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

void DataGridView::on_vertical_scroll(double value) {
    if (synchronizing_scroll_) return;
    const double rounded = std::max(0.0, std::round(value));
    first_row_ = static_cast<std::size_t>(rounded);
    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

void DataGridView::on_horizontal_scroll(double value) {
    if (synchronizing_scroll_) return;
    horizontal_offset_ = std::max(0.0, value);
    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

void DataGridView::scroll_into_view(DataGridViewCellAddress cell) {
    const GridLayout layout = grid_layout();
    const std::size_t fully = std::max<std::size_t>(1U, fully_visible_rows(layout));
    if (cell.row < first_row_) {
        first_row_ = cell.row;
    } else if (cell.row >= first_row_ + fully) {
        first_row_ = cell.row + 1U - fully;
    }
    // Columns in logical units at text scale 1.
    double left = 0.0;
    for (std::size_t index = 0; index < cell.column && index < columns_.size(); ++index) {
        left += columns_[index].width;
    }
    const double right = left + (cell.column < columns_.size() ? columns_[cell.column].width : 0.0);
    const double viewport = layout.data.width / layout.scale;
    if (left < horizontal_offset_) {
        horizontal_offset_ = left;
    } else if (right > horizontal_offset_ + viewport) {
        horizontal_offset_ = std::min(left, right - viewport);
    }
    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

void DataGridView::arrange(Rect final_bounds) {
    arrange_self(final_bounds);
    const GridLayout layout = grid_layout();
    first_row_ = std::min(first_row_, maximum_first_row(layout));
    horizontal_offset_ = std::clamp(horizontal_offset_, 0.0, maximum_horizontal_offset(layout));
    synchronize_scroll_bars(layout);
    if (vertical_bar_) {
        set_child_layout_collapsed(vertical_bar_, layout.vertical_bar.empty());
        set_child_layout(vertical_bar_, layout.vertical_bar);
    }
    if (horizontal_bar_) {
        set_child_layout_collapsed(horizontal_bar_, layout.horizontal_bar.empty());
        set_child_layout(horizontal_bar_, layout.horizontal_bar);
    }
    if (editor_) {
        const Parts placed = grid_parts(layout);
        // A text edit scrolled out of view keeps its editor, and focus, at
        // no size until the cell is shown again.
        set_child_layout(editor_, placed.editor.bounds);
    }
}

void DataGridView::synchronize_scroll_bars(const GridLayout& layout) {
    synchronizing_scroll_ = true;
    const std::size_t last_first = maximum_first_row(layout);
    if (vertical_bar_ && last_first > 0U) {
        VScrollBar& bar = *vertical_bar_;
        bar.set_range(0.0, static_cast<double>(last_first));
        bar.set_small_change(1.0);
        bar.set_large_change(static_cast<double>(std::max<std::size_t>(1U, fully_visible_rows(layout))));
        bar.set_value(static_cast<double>(first_row_));
    }
    const double last_offset = maximum_horizontal_offset(layout);
    if (horizontal_bar_ && last_offset > 0.0) {
        HScrollBar& bar = *horizontal_bar_;
        bar.set_range(0.0, last_offset);
        bar.set_small_change(16.0);
        bar.set_large_change(std::max(1.0, layout.data.width / layout.scale));
        bar.set_value(horizontal_offset_);
    }
    synchronizing_scroll_ = false;
}

// -- selection -----------------------------------------------------------------

bool DataGridView::cell_in_selection(std::size_t row, std::size_t column) const noexcept {
    const DataGridViewCellAddress cell{row, column};
    const bool result = std::binary_search(selected_cells_.begin(), selected_cells_.end(), cell,
                                           CellAddressOrder{});
    return result;
}

bool DataGridView::row_in_selection(std::size_t row) const noexcept {
    const bool result = std::binary_search(selected_rows_.begin(), selected_rows_.end(), row);
    return result;
}

void DataGridView::insert_selected_cell(DataGridViewCellAddress cell) {
    const std::vector<DataGridViewCellAddress>::iterator place =
        std::lower_bound(selected_cells_.begin(), selected_cells_.end(), cell, CellAddressOrder{});
    if (place != selected_cells_.end() && *place == cell) return;
    selected_cells_.insert(place, cell);
    ++selection_revision_;
}

void DataGridView::erase_selected_cell(DataGridViewCellAddress cell) {
    const std::vector<DataGridViewCellAddress>::iterator place =
        std::lower_bound(selected_cells_.begin(), selected_cells_.end(), cell, CellAddressOrder{});
    if (place == selected_cells_.end() || !(*place == cell)) return;
    selected_cells_.erase(place);
    ++selection_revision_;
}

void DataGridView::insert_selected_row(std::size_t row) {
    const std::vector<std::size_t>::iterator place =
        std::lower_bound(selected_rows_.begin(), selected_rows_.end(), row);
    if (place != selected_rows_.end() && *place == row) return;
    selected_rows_.insert(place, row);
    ++selection_revision_;
}

void DataGridView::erase_selected_row(std::size_t row) {
    const std::vector<std::size_t>::iterator place =
        std::lower_bound(selected_rows_.begin(), selected_rows_.end(), row);
    if (place == selected_rows_.end() || *place != row) return;
    selected_rows_.erase(place);
    ++selection_revision_;
}

void DataGridView::clear_selection_state() noexcept {
    if (selected_cells_.empty() && selected_rows_.empty()) return;
    selected_cells_.clear();
    selected_rows_.clear();
    ++selection_revision_;
}

void DataGridView::publish_selection(std::uint64_t before) {
    if (selection_revision_ == before) return;
    invalidate(Dirty::paint | Dirty::semantics);
    selection_changed_.emit();
}

// Selects the cells (or rows) of the rectangle the two cells span.
void DataGridView::select_range(DataGridViewCellAddress from, DataGridViewCellAddress to) {
    const std::size_t first_row = std::min(from.row, to.row);
    const std::size_t last_row = std::max(from.row, to.row);
    if (selection_mode_ == DataGridViewSelectionMode::full_row_select) {
        for (std::size_t row = first_row; row <= last_row; ++row) insert_selected_row(row);
        return;
    }
    const std::size_t first_column = std::min(from.column, to.column);
    const std::size_t last_column = std::max(from.column, to.column);
    for (std::size_t row = first_row; row <= last_row; ++row) {
        for (std::size_t column = first_column; column <= last_column; ++column) {
            insert_selected_cell({row, column});
        }
    }
}

void DataGridView::select_current_alone() {
    clear_selection_state();
    if (!current_) return;
    if (selection_mode_ == DataGridViewSelectionMode::full_row_select) {
        insert_selected_row((*current_).row);
    } else {
        insert_selected_cell(*current_);
    }
}

void DataGridView::apply_current(std::optional<DataGridViewCellAddress> cell) {
    if (current_ == cell) return;
    current_ = cell;
    invalidate(Dirty::arrange | Dirty::paint | Dirty::semantics);
    if (cell && row_source_ == DataGridViewRowSource::binding_source && data_source_ &&
        !synchronizing_position_) {
        synchronizing_position_ = true;
        static_cast<void>((*data_source_).set_position(static_cast<std::ptrdiff_t>((*cell).row)));
        synchronizing_position_ = false;
    }
    publish_change(current_cell_changed_, current_);
}

bool DataGridView::move_current(DataGridViewCellAddress target, SelectionGesture gesture) {
    if (target.row >= row_count() || target.column >= columns_.size()) return false;
    if (editing_ && !(current_ && *current_ == target) && !end_edit()) return false;
    const std::uint64_t before = selection_revision_;
    const bool full_row = selection_mode_ == DataGridViewSelectionMode::full_row_select;
    if (gesture == SelectionGesture::extend && multi_select_) {
        const DataGridViewCellAddress from = anchor_ ? *anchor_ : (current_ ? *current_ : target);
        clear_selection_state();
        select_range(from, target);
        anchor_ = from;
    } else if (gesture == SelectionGesture::toggle && multi_select_) {
        if (full_row) {
            if (row_in_selection(target.row)) erase_selected_row(target.row);
            else insert_selected_row(target.row);
        } else {
            if (cell_in_selection(target.row, target.column)) erase_selected_cell(target);
            else insert_selected_cell(target);
        }
        anchor_ = target;
    } else {
        clear_selection_state();
        if (full_row) insert_selected_row(target.row);
        else insert_selected_cell(target);
        anchor_ = target;
    }
    apply_current(target);
    scroll_into_view(target);
    publish_selection(before);
    return true;
}

void DataGridView::shift_rows_after_insert(std::size_t row) {
    for (DataGridViewCellAddress& cell : selected_cells_) {
        if (cell.row >= row) ++cell.row;
    }
    for (std::size_t& selected : selected_rows_) {
        if (selected >= row) ++selected;
    }
    if (current_ && (*current_).row >= row) ++(*current_).row;
    if (anchor_ && (*anchor_).row >= row) ++(*anchor_).row;
    pressed_button_.reset();
}

void DataGridView::shift_rows_after_remove(std::size_t row) {
    const std::uint64_t before = selection_revision_;
    std::vector<DataGridViewCellAddress> cells{};
    cells.reserve(selected_cells_.size());
    for (const DataGridViewCellAddress& cell : selected_cells_) {
        if (cell.row == row) continue;
        const std::size_t moved = cell.row > row ? cell.row - 1U : cell.row;
        cells.push_back({moved, cell.column});
    }
    if (cells.size() != selected_cells_.size()) ++selection_revision_;
    selected_cells_ = std::move(cells);
    std::vector<std::size_t> rows{};
    rows.reserve(selected_rows_.size());
    for (const std::size_t selected : selected_rows_) {
        if (selected == row) continue;
        rows.push_back(selected > row ? selected - 1U : selected);
    }
    if (rows.size() != selected_rows_.size()) ++selection_revision_;
    selected_rows_ = std::move(rows);
    if (anchor_ && (*anchor_).row == row) anchor_.reset();
    else if (anchor_ && (*anchor_).row > row) --(*anchor_).row;
    pressed_button_.reset();
    if (current_ && (*current_).row > row) {
        --(*current_).row;
    } else if (current_ && (*current_).row == row) {
        // The current row is gone: the row that took its place, or the new
        // last row, becomes current.
        const std::size_t count = row_count();
        std::optional<DataGridViewCellAddress> next{};
        if (count > 0U) next = DataGridViewCellAddress{std::min(row, count - 1U), (*current_).column};
        current_.reset();
        apply_current(next);
    }
    publish_selection(before);
}

void DataGridView::trim_rows(std::size_t count) {
    const std::uint64_t before = selection_revision_;
    while (!selected_cells_.empty() && selected_cells_.back().row >= count) {
        selected_cells_.pop_back();
        ++selection_revision_;
    }
    while (!selected_rows_.empty() && selected_rows_.back() >= count) {
        selected_rows_.pop_back();
        ++selection_revision_;
    }
    if (anchor_ && (*anchor_).row >= count) anchor_.reset();
    pressed_button_.reset();
    if (current_ && (*current_).row >= count) apply_current(std::nullopt);
    if (first_row_ >= count) first_row_ = 0U;
    publish_selection(before);
}

void DataGridView::set_selection_mode(DataGridViewSelectionMode mode) {
    require_mutable();
    if (selection_mode_ == mode) return;
    const std::uint64_t before = selection_revision_;
    selection_mode_ = mode;
    select_current_alone();
    ++selection_revision_;
    publish_selection(before);
}

void DataGridView::set_multi_select(bool multi_select) {
    require_mutable();
    if (multi_select_ == multi_select) return;
    multi_select_ = multi_select;
    if (multi_select_) return;
    const std::uint64_t before = selection_revision_;
    const std::size_t selected = selected_cells_.size() + selected_rows_.size();
    if (selected > 1U) select_current_alone();
    publish_selection(before);
}

bool DataGridView::set_current_cell(std::optional<DataGridViewCellAddress> cell) {
    require_mutable();
    if (cell) require_cell((*cell).row, (*cell).column);
    if (!cell) {
        if (editing_ && !end_edit()) return false;
        anchor_.reset();
        apply_current(std::nullopt);
        return true;
    }
    const bool moved = move_current(*cell, SelectionGesture::replace);
    return moved;
}

bool DataGridView::is_cell_selected(std::size_t row, std::size_t column) const noexcept {
    if (selection_mode_ == DataGridViewSelectionMode::full_row_select) return row_in_selection(row);
    return cell_in_selection(row, column);
}

bool DataGridView::is_row_selected(std::size_t row) const noexcept {
    if (selection_mode_ == DataGridViewSelectionMode::full_row_select) return row_in_selection(row);
    if (columns_.empty()) return false;
    for (std::size_t column = 0; column < columns_.size(); ++column) {
        if (!cell_in_selection(row, column)) return false;
    }
    return true;
}

void DataGridView::set_cell_selected(std::size_t row, std::size_t column, bool selected) {
    require_mutable();
    require_cell(row, column);
    if (selection_mode_ == DataGridViewSelectionMode::full_row_select) {
        set_row_selected(row, selected);
        return;
    }
    const std::uint64_t before = selection_revision_;
    if (selected && !multi_select_) clear_selection_state();
    if (selected) insert_selected_cell({row, column});
    else erase_selected_cell({row, column});
    publish_selection(before);
}

void DataGridView::set_row_selected(std::size_t row, bool selected) {
    require_mutable();
    if (row >= row_count()) throw std::out_of_range("DataGridView row is past the end");
    const std::uint64_t before = selection_revision_;
    if (selected && !multi_select_) clear_selection_state();
    if (selection_mode_ == DataGridViewSelectionMode::full_row_select) {
        if (selected) insert_selected_row(row);
        else erase_selected_row(row);
    } else {
        for (std::size_t column = 0; column < columns_.size(); ++column) {
            if (selected) insert_selected_cell({row, column});
            else erase_selected_cell({row, column});
        }
    }
    publish_selection(before);
}

std::vector<DataGridViewCellAddress> DataGridView::selected_cells() const {
    if (selection_mode_ == DataGridViewSelectionMode::cell_select) return selected_cells_;
    std::vector<DataGridViewCellAddress> cells{};
    cells.reserve(selected_rows_.size() * columns_.size());
    for (const std::size_t row : selected_rows_) {
        for (std::size_t column = 0; column < columns_.size(); ++column) cells.push_back({row, column});
    }
    return cells;
}

std::vector<std::size_t> DataGridView::selected_rows() const {
    if (selection_mode_ == DataGridViewSelectionMode::full_row_select) return selected_rows_;
    std::vector<std::size_t> rows{};
    std::size_t index = 0U;
    while (index < selected_cells_.size()) {
        const std::size_t row = selected_cells_[index].row;
        std::size_t run = 0U;
        while (index < selected_cells_.size() && selected_cells_[index].row == row) {
            ++run;
            ++index;
        }
        if (!columns_.empty() && run == columns_.size()) rows.push_back(row);
    }
    return rows;
}

void DataGridView::clear_selection() {
    require_mutable();
    const std::uint64_t before = selection_revision_;
    clear_selection_state();
    publish_selection(before);
}

void DataGridView::select_all() {
    require_mutable();
    if (!multi_select_) return;
    const std::uint64_t before = selection_revision_;
    const std::size_t count = row_count();
    if (count == 0U || columns_.empty()) return;
    clear_selection_state();
    select_range({0U, 0U}, {count - 1U, columns_.size() - 1U});
    publish_selection(before);
}

// -- editing -------------------------------------------------------------------

bool DataGridView::begin_edit(bool select_all) {
    require_mutable();
    if (editing_) return true;
    if (!current_) return false;
    const DataGridViewColumnKind kind = columns_[(*current_).column].kind;
    if (kind == DataGridViewColumnKind::combo_box) return open_choices();
    if (kind != DataGridViewColumnKind::text) return false;
    const bool begun = begin_text_edit(cell_text((*current_).row, (*current_).column), select_all);
    return begun;
}

bool DataGridView::begin_text_edit(std::string initial_text, bool select_all) {
    if (!current_ || !editor_) return false;
    const DataGridViewCellAddress cell = *current_;
    if (cell_read_only(cell.row, cell.column)) return false;
    DataGridViewCellCancelEvent request{cell.row, cell.column, false};
    cell_begin_edit_.emit(request);
    // A handler may cancel, or move the current cell or end the grid.
    if (request.cancel || !is_alive() || !current_ || !(*current_ == cell)) return false;
    scroll_into_view(cell);
    editing_ = true;
    edit_kind_ = EditKind::text;
    edit_invalid_ = false;
    TextBox& editor = *editor_;
    editor.set_text(std::move(initial_text));
    set_child_layout_collapsed(editor_, false);
    set_child_layout(editor_, grid_parts(grid_layout()).editor.bounds);
    if (window() != nullptr) static_cast<void>((*window()).request_focus(editor_));
    if (select_all) {
        editor.select_all();
    } else {
        const Utf8Offset end(editor.text().size());
        editor.select(end, end);
    }
    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
    return true;
}

bool DataGridView::end_edit() {
    require_mutable();
    if (!editing_ || !current_) return true;
    if (edit_kind_ == EditKind::choice) {
        close_drop_down();
        return true;
    }
    const DataGridViewCellAddress cell = *current_;
    const DataGridViewColumn& column = columns_[cell.column];
    const std::string text((*editor_).text());
    DataGridViewCellValidatingEvent validating{cell.row, cell.column, text, false};
    cell_validating_.emit(validating);
    if (!is_alive() || !editing_) return !editing_;
    if (validating.cancel) {
        edit_invalid_ = true;
        invalidate(Dirty::paint | Dirty::semantics);
        return false;
    }
    // The typed text converts to the column's value kind; empty text is an
    // empty value except in a text column.
    BindingValue value{};
    if (column.value_kind == BindingValueKind::text) {
        value = text;
    } else if (!text.empty()) {
        const std::optional<BindingValue> converted =
            convert_binding_value(BindingValue{text}, column.value_kind);
        if (!converted) {
            edit_invalid_ = true;
            invalidate(Dirty::paint | Dirty::semantics);
            return false;
        }
        value = *converted;
    }
    const BindingValue previous = stored_value(cell.row, cell.column);
    const bool changed = !(previous == value);
    if (changed && !store_value(cell.row, cell.column, value)) {
        edit_invalid_ = true;
        invalidate(Dirty::paint | Dirty::semantics);
        return false;
    }
    finish_edit();
    const DataGridViewCellEvent ended{cell.row, cell.column};
    if (changed) cell_value_changed_.emit(ended);
    cell_end_edit_.emit(ended);
    return true;
}

void DataGridView::cancel_edit() {
    if (!editing_) return;
    if (edit_kind_ == EditKind::choice) {
        close_drop_down();
        return;
    }
    const DataGridViewCellAddress cell = current_ ? *current_ : DataGridViewCellAddress{};
    finish_edit();
    const DataGridViewCellEvent ended{cell.row, cell.column};
    cell_end_edit_.emit(ended);
}

// Leaves edit mode: the editor hides, and keyboard focus returns to the grid
// when the editor held it.
void DataGridView::finish_edit() {
    ending_edit_ = true;
    const bool editor_focused = window() != nullptr && editor_ &&
                                (*window()).focused_control() == editor_;
    editing_ = false;
    edit_kind_ = EditKind::none;
    edit_invalid_ = false;
    if (editor_focused) static_cast<void>((*window()).request_focus(shared_from_this()));
    if (editor_) set_child_layout_collapsed(editor_, true);
    ending_edit_ = false;
    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

void DataGridView::on_editor_focus(bool focused) {
    if (focused || !editing_ || ending_edit_ || edit_kind_ != EditKind::text) return;
    // Focus left the editor for another control: commit, or discard an edit
    // that does not commit, since the user has moved on.
    if (!end_edit()) cancel_edit();
}

bool DataGridView::open_choices() {
    if (!current_ || !choices_ || window() == nullptr) return false;
    const DataGridViewCellAddress cell = *current_;
    const DataGridViewColumn& column = columns_[cell.column];
    if (column.kind != DataGridViewColumnKind::combo_box || column.items.empty() ||
        cell_read_only(cell.row, cell.column)) {
        return false;
    }
    DataGridViewCellCancelEvent request{cell.row, cell.column, false};
    cell_begin_edit_.emit(request);
    if (request.cancel || !is_alive() || !current_ || !(*current_ == cell)) return false;
    scroll_into_view(cell);
    const std::string prefix = std::string(stable_id().value()) + ".choice.";
    const std::string current_text = cell_text(cell.row, cell.column);
    const std::size_t count = columns_[cell.column].items.size();
    choice_commands_.reserve(count);
    std::vector<MenuItemSpec> menu{};
    menu.reserve(count);
    for (std::size_t index = 0; index < count; ++index) {
        const std::string& item = columns_[cell.column].items[index];
        std::string identity = prefix + std::to_string(index);
        if (index == choice_commands_.size()) {
            choice_commands_.push_back(std::make_shared<Command>(identity, item));
        }
        Command& command = *choice_commands_[index];
        command.set_text(item);
        command.set_checked(item == current_text);
        menu.emplace_back(std::move(identity), MenuItemKind::radio, choice_commands_[index], item);
    }
    (*choices_).set_items(std::move(menu));
    const GridLayout layout = grid_layout();
    const Rect bounds = cell_bounds(cell.row, cell.column, layout);
    const Rect absolute = absolute_bounds();
    editing_ = true;
    edit_kind_ = EditKind::choice;
    edit_invalid_ = false;
    invalidate(Dirty::paint | Dirty::semantics);
    (*choices_).set_preferred_width(std::max(bounds.width, 120.0));
    (*choices_).show(shared_from_this(), {absolute.x + bounds.x, absolute.y + bounds.y + bounds.height},
                     MenuOpenMode::keyboard);
    if (!(*choices_).is_open()) {
        editing_ = false;
        edit_kind_ = EditKind::none;
        const DataGridViewCellEvent ended{cell.row, cell.column};
        cell_end_edit_.emit(ended);
        return false;
    }
    return true;
}

void DataGridView::on_choice_invoked(const MenuItemInvocation& invocation) {
    if (edit_kind_ != EditKind::choice || !current_) return;
    const DataGridViewCellAddress cell = *current_;
    const std::string prefix = std::string(stable_id().value()) + ".choice.";
    if (invocation.item_id.compare(0U, prefix.size(), prefix) != 0) return;
    const std::vector<std::string>& items = columns_[cell.column].items;
    std::optional<std::size_t> chosen{};
    for (std::size_t index = 0; index < items.size(); ++index) {
        if (invocation.item_id == prefix + std::to_string(index)) {
            chosen = index;
            break;
        }
    }
    if (!chosen) return;
    const BindingValue value{items[*chosen]};
    const BindingValue previous = stored_value(cell.row, cell.column);
    if (previous == value || !store_value(cell.row, cell.column, value)) return;
    invalidate(Dirty::paint | Dirty::semantics);
    const DataGridViewCellEvent changed{cell.row, cell.column};
    cell_value_changed_.emit(changed);
}

void DataGridView::on_choices_open_changed(bool open) {
    if (open || edit_kind_ != EditKind::choice) return;
    const DataGridViewCellAddress cell = current_ ? *current_ : DataGridViewCellAddress{};
    editing_ = false;
    edit_kind_ = EditKind::none;
    invalidate(Dirty::paint | Dirty::semantics);
    const DataGridViewCellEvent ended{cell.row, cell.column};
    cell_end_edit_.emit(ended);
}

bool DataGridView::drop_down_open() const noexcept {
    const bool result = choices_ && (*choices_).is_open();
    return result;
}

void DataGridView::close_drop_down() noexcept {
    if (choices_) (*choices_).close();
}

bool DataGridView::toggle_check(DataGridViewCellAddress cell) {
    if (columns_[cell.column].kind != DataGridViewColumnKind::check_box ||
        cell_read_only(cell.row, cell.column)) {
        return false;
    }
    const bool checked = binding_value_to_bool(stored_value(cell.row, cell.column)).value_or(false);
    const BindingValue value{!checked};
    if (!store_value(cell.row, cell.column, value)) return false;
    invalidate(Dirty::paint | Dirty::semantics);
    const DataGridViewCellEvent changed{cell.row, cell.column};
    cell_value_changed_.emit(changed);
    cell_content_click_.emit(changed);
    return true;
}

void DataGridView::click_button(DataGridViewCellAddress cell) {
    if (columns_[cell.column].kind != DataGridViewColumnKind::button) return;
    const DataGridViewCellEvent clicked{cell.row, cell.column};
    cell_content_click_.emit(clicked);
}

// The current cell's own action: a check box toggles, a button clicks, a
// combo box opens its choices and a text cell begins an edit.
bool DataGridView::activate_current() {
    if (!current_) return false;
    const DataGridViewCellAddress cell = *current_;
    const DataGridViewColumnKind kind = columns_[cell.column].kind;
    if (kind == DataGridViewColumnKind::check_box) return toggle_check(cell);
    if (kind == DataGridViewColumnKind::button) {
        click_button(cell);
        return true;
    }
    const bool begun = begin_edit(true);
    return begun;
}

// -- input ---------------------------------------------------------------------

void DataGridView::on_pointer(PointerEvent& event) {
    const Control::Ptr lifetime = shared_from_this();
    const Point local = point_from_window(event.position);
    const GridLayout layout = grid_layout();
    if (resizing_column_ && !has_pointer_capture()) resizing_column_.reset();
    if (resizing_column_) {
        if (event.action == PointerAction::move) {
            const DataGridViewColumn& column = columns_[*resizing_column_];
            const double delta = (local.x - resize_start_x_) / layout.scale;
            const double width = std::max(column.minimum_width, resize_start_width_ + delta);
            set_column_width(*resizing_column_, width);
        } else if (event.action == PointerAction::up) {
            resizing_column_.reset();
            if (window() != nullptr && has_pointer_capture()) (*window()).release_pointer();
            invalidate(Dirty::paint | Dirty::semantics);
        }
        event.handled = true;
        return;
    }
    if (event.action == PointerAction::leave) {
        if (hot_column_) {
            hot_column_.reset();
            invalidate(Dirty::paint);
        }
        return;
    }
    const HitResult hit = hit_test(local);
    if (event.action == PointerAction::move || event.action == PointerAction::enter) {
        std::optional<std::size_t> hot{};
        if (hit.zone == HitZone::column_header || hit.zone == HitZone::column_divider) hot = hit.column;
        if (hot_column_ != hot) {
            hot_column_ = hot;
            invalidate(Dirty::paint);
        }
        return;
    }
    if (event.action == PointerAction::wheel) {
        const bool sideways = event.wheel_delta.x != 0.0 || has_modifier(event.modifiers, Modifier::shift);
        const double delta = event.wheel_delta.x != 0.0 ? event.wheel_delta.x : event.wheel_delta.y;
        if (delta == 0.0) return;
        // Small deltas are notches (three rows, or 48 units, each); larger
        // ones are already logical units.
        const bool notches = std::abs(delta) <= 8.0;
        if (sideways) {
            const double units = notches ? delta * 48.0 : delta;
            const double next = std::clamp(horizontal_offset_ - units, 0.0, maximum_horizontal_offset(layout));
            if (next != horizontal_offset_) set_horizontal_scroll_offset(next);
        } else {
            const double rows = notches ? delta * 3.0 : delta / layout.row_height;
            const double moved = std::round(static_cast<double>(first_row_) - rows);
            const double last = static_cast<double>(maximum_first_row(layout));
            const std::size_t next = static_cast<std::size_t>(std::clamp(moved, 0.0, last));
            if (next != first_row_) set_first_displayed_row(next);
        }
        event.handled = true;
        return;
    }
    if (event.button != PointerButton::primary) return;
    if (event.action == PointerAction::down) {
        event.handled = true;
        if (editing_ && !end_edit()) {
            if (edit_kind_ == EditKind::text && window() != nullptr) {
                static_cast<void>((*window()).request_focus(editor_));
            }
            return;
        }
        if (window() != nullptr) static_cast<void>((*window()).request_focus(lifetime));
        const bool shift = has_modifier(event.modifiers, Modifier::shift);
        const bool control = has_modifier(event.modifiers, Modifier::control);
        SelectionGesture gesture = SelectionGesture::replace;
        if (shift) gesture = SelectionGesture::extend;
        else if (control) gesture = SelectionGesture::toggle;
        if (hit.zone == HitZone::column_divider && allow_column_resize_) {
            resizing_column_ = hit.column;
            resize_start_x_ = local.x;
            resize_start_width_ = columns_[hit.column].width;
            if (window() != nullptr) (*window()).capture_pointer(lifetime, event.pointer_id);
            invalidate(Dirty::paint | Dirty::semantics);
            return;
        }
        if (hit.zone == HitZone::column_header || hit.zone == HitZone::column_divider) {
            pressed_column_ = hit.column;
            invalidate(Dirty::paint);
            return;
        }
        if (hit.zone == HitZone::top_left) {
            select_all();
            return;
        }
        if (hit.zone == HitZone::row_header) {
            const std::size_t column = current_ ? (*current_).column : 0U;
            if (!move_current({hit.row, column}, gesture)) return;
            if (selection_mode_ == DataGridViewSelectionMode::cell_select && gesture == SelectionGesture::replace) {
                const std::uint64_t before = selection_revision_;
                select_range({hit.row, 0U}, {hit.row, columns_.size() - 1U});
                publish_selection(before);
            }
            return;
        }
        if (hit.zone != HitZone::cell) return;
        const DataGridViewCellAddress cell{hit.row, hit.column};
        if (!move_current(cell, gesture) || gesture != SelectionGesture::replace) return;
        const CellParts placed = cell_parts(cell.row, cell.column, grid_layout());
        const DataGridViewColumnKind kind = columns_[cell.column].kind;
        if (kind == DataGridViewColumnKind::check_box && placed.cell_check_well.bounds.contains(local)) {
            static_cast<void>(toggle_check(cell));
        } else if (kind == DataGridViewColumnKind::button && placed.cell_button_face.bounds.contains(local)) {
            pressed_button_ = cell;
            invalidate(Dirty::paint | Dirty::semantics);
        } else if (kind == DataGridViewColumnKind::combo_box &&
                   (placed.cell_drop_down.bounds.contains(local) || event.click_count >= 2U)) {
            static_cast<void>(open_choices());
        } else if (kind == DataGridViewColumnKind::text && event.click_count >= 2U) {
            static_cast<void>(begin_edit(true));
        }
        return;
    }
    if (event.action == PointerAction::up) {
        if (pressed_column_) {
            const std::size_t pressed = *pressed_column_;
            pressed_column_.reset();
            invalidate(Dirty::paint);
            const bool on_header = hit.zone == HitZone::column_header || hit.zone == HitZone::column_divider;
            if (on_header && hit.column == pressed) column_header_click_.emit(pressed);
            event.handled = true;
            return;
        }
        if (pressed_button_) {
            const DataGridViewCellAddress pressed = *pressed_button_;
            pressed_button_.reset();
            invalidate(Dirty::paint | Dirty::semantics);
            if (hit.zone == HitZone::cell && hit.row == pressed.row && hit.column == pressed.column) {
                click_button(pressed);
            }
            event.handled = true;
        }
    }
}

void DataGridView::on_key_preview(KeyEvent& event) {
    // Keys for a text edit in progress, before the editor sees them.
    if (!editing_ || edit_kind_ != EditKind::text || event.action != KeyAction::down) return;
    const std::uint32_t key = event.physical_key;
    if (key == PhysicalKey::escape) {
        cancel_edit();
        event.handled = true;
        return;
    }
    if (key == PhysicalKey::enter) {
        event.handled = true;
        if (!end_edit() || !current_) return;
        const std::size_t next = (*current_).row + 1U;
        if (next < row_count()) static_cast<void>(move_current({next, (*current_).column}, SelectionGesture::replace));
        return;
    }
    if (key == PhysicalKey::tab) {
        event.handled = true;
        if (!end_edit()) return;
        static_cast<void>(tab_key(has_modifier(event.modifiers, Modifier::shift)));
        return;
    }
    const bool vertical = key == PhysicalKey::up || key == PhysicalKey::down ||
                          key == PhysicalKey::page_up || key == PhysicalKey::page_down;
    if (!vertical) return;
    event.handled = true;
    if (!end_edit()) return;
    static_cast<void>(navigation_key(event));
}

void DataGridView::on_key(KeyEvent& event) {
    if (event.action != KeyAction::down || editing_ || !focused_) return;
    const std::uint32_t key = event.physical_key;
    const bool alt = has_modifier(event.modifiers, Modifier::alt);
    const bool control = has_modifier(event.modifiers, Modifier::control);
    if (current_ && (key == PhysicalKey::f4 || (alt && key == PhysicalKey::down)) &&
        columns_[(*current_).column].kind == DataGridViewColumnKind::combo_box) {
        static_cast<void>(open_choices());
        event.handled = true;
        return;
    }
    if (key == PhysicalKey::f2) {
        static_cast<void>(begin_edit(true));
        event.handled = true;
        return;
    }
    if (key == PhysicalKey::space && current_) {
        const DataGridViewColumnKind kind = columns_[(*current_).column].kind;
        if (kind == DataGridViewColumnKind::check_box || kind == DataGridViewColumnKind::button) {
            static_cast<void>(activate_current());
            event.handled = true;
        }
        return;
    }
    if (control && key == PhysicalKey::a) {
        select_all();
        event.handled = true;
        return;
    }
    if (key == PhysicalKey::enter) {
        // Enter moves to the cell below, as in Windows Forms.
        if (current_ && (*current_).row + 1U < row_count()) {
            static_cast<void>(move_current({(*current_).row + 1U, (*current_).column}, SelectionGesture::replace));
        }
        event.handled = true;
        return;
    }
    if (key == PhysicalKey::tab) {
        if (tab_key(has_modifier(event.modifiers, Modifier::shift))) event.handled = true;
        return;
    }
    if (navigation_key(event)) event.handled = true;
}

bool DataGridView::tab_key(bool backward) {
    const std::size_t count = row_count();
    const std::size_t columns = columns_.size();
    if (count == 0U || columns == 0U) return false;
    if (!current_) {
        const bool moved = move_current({0U, 0U}, SelectionGesture::replace);
        return moved;
    }
    DataGridViewCellAddress next = *current_;
    if (backward) {
        if (next.column > 0U) {
            --next.column;
        } else if (next.row > 0U) {
            --next.row;
            next.column = columns - 1U;
        } else {
            // Shift+Tab from the first cell leaves the grid.
            return false;
        }
    } else {
        if (next.column + 1U < columns) {
            ++next.column;
        } else if (next.row + 1U < count) {
            ++next.row;
            next.column = 0U;
        } else {
            // Tab from the last cell leaves the grid.
            return false;
        }
    }
    static_cast<void>(move_current(next, SelectionGesture::replace));
    return true;
}

bool DataGridView::navigation_key(KeyEvent& event) {
    const std::size_t count = row_count();
    const std::size_t columns = columns_.size();
    if (count == 0U || columns == 0U) return false;
    const std::uint32_t key = event.physical_key;
    const bool control = has_modifier(event.modifiers, Modifier::control);
    const bool shift = has_modifier(event.modifiers, Modifier::shift);
    const bool full_row = selection_mode_ == DataGridViewSelectionMode::full_row_select;
    const DataGridViewCellAddress from = current_ ? *current_ : DataGridViewCellAddress{first_row_, 0U};
    const std::size_t last_row = count - 1U;
    const std::size_t last_column = columns - 1U;
    const std::size_t page = std::max<std::size_t>(1U, fully_visible_rows(grid_layout()));
    DataGridViewCellAddress target = from;
    if (key == PhysicalKey::up) {
        target.row = control || from.row == 0U ? 0U : from.row - 1U;
    } else if (key == PhysicalKey::down) {
        target.row = control ? last_row : std::min(last_row, from.row + 1U);
    } else if (key == PhysicalKey::left) {
        target.column = control || from.column == 0U ? 0U : from.column - 1U;
    } else if (key == PhysicalKey::right) {
        target.column = control ? last_column : std::min(last_column, from.column + 1U);
    } else if (key == PhysicalKey::page_up) {
        target.row = from.row > page ? from.row - page : 0U;
    } else if (key == PhysicalKey::page_down) {
        target.row = std::min(last_row, from.row + page);
    } else if (key == PhysicalKey::home) {
        // Ctrl+Home is the first cell; Home is the row's first cell, or the
        // first row when whole rows are selected.
        if (control) {
            target = {0U, 0U};
        } else if (full_row) {
            target.row = 0U;
        } else {
            target.column = 0U;
        }
    } else if (key == PhysicalKey::end) {
        if (control) {
            target = {last_row, last_column};
        } else if (full_row) {
            target.row = last_row;
        } else {
            target.column = last_column;
        }
    } else {
        return false;
    }
    const SelectionGesture gesture = shift ? SelectionGesture::extend : SelectionGesture::replace;
    static_cast<void>(move_current(target, gesture));
    return true;
}

void DataGridView::on_text_input(TextInputEvent& event) {
    if (editing_ || !focused_ || !current_ || event.composing || event.text_utf8.empty()) return;
    if (columns_[(*current_).column].kind != DataGridViewColumnKind::text) return;
    // Typing into a text cell begins an edit that starts with the typed text.
    if (begin_text_edit(event.text_utf8, false)) event.handled = true;
}

void DataGridView::on_focus_changed(bool focused) {
    focused_ = focused;
    if (focused_ && !current_ && row_count() > 0U && !columns_.empty()) {
        static_cast<void>(move_current({first_row_, 0U}, SelectionGesture::replace));
    }
    invalidate(Dirty::paint | Dirty::semantics);
}

// -- semantics -----------------------------------------------------------------

std::string DataGridView::cell_stable_id(std::size_t row, std::size_t column) const {
    std::string identity = row_stable_id(row);
    identity += '.';
    identity += columns_[column].name;
    return identity;
}

std::string DataGridView::column_stable_id(std::size_t column) const {
    std::string identity(stable_id().value());
    identity += ".column.";
    identity += columns_[column].name;
    return identity;
}

std::optional<DataGridViewCellAddress> DataGridView::cell_from_stable_id(std::string_view id) const {
    // Column names hold no '.', so the last '.' separates row and column.
    const std::size_t dot = id.rfind('.');
    if (dot == std::string_view::npos) return std::nullopt;
    const std::optional<std::size_t> column = column_index(id.substr(dot + 1U));
    if (!column) return std::nullopt;
    const std::optional<std::size_t> row = row_index(id.substr(0U, dot));
    if (!row) return std::nullopt;
    const DataGridViewCellAddress cell{*row, *column};
    return cell;
}

SemanticDescriptor DataGridView::semantic_descriptor() const {
    // The vocabulary has no grid or table role: the grid is a list whose
    // items are rows, each holding its cells (vocabulary decision).
    SemanticDescriptor descriptor{};
    descriptor.role = SemanticRole::list;
    descriptor.name = accessible_name().empty() ? "Data grid" : accessible_name();
    descriptor.description = accessible_description();
    if (current_ && (*current_).row < row_count() && (*current_).column < columns_.size()) {
        descriptor.value = cell_text((*current_).row, (*current_).column);
    }
    descriptor.actions = {SemanticAction::focus};
    if (read_only_) descriptor.states |= SemanticState::read_only;
    descriptor.exposed = true;
    return descriptor;
}

std::vector<SemanticNode> DataGridView::semantic_virtual_children() const {
    std::vector<SemanticNode> nodes{};
    const Rect absolute = absolute_bounds();
    const GridLayout layout = grid_layout();
    const bool focus_within = focused_ || editing_;
    const std::size_t first_column = first_visible_column(layout);
    const std::size_t column_end = visible_column_end(layout);
    if (column_headers_visible_) {
        for (std::size_t column = first_column; column < column_end; ++column) {
            const ColumnParts placed = column_parts(column, layout);
            SemanticNode node{};
            node.stable_id = column_stable_id(column);
            node.runtime_id = scrolling_detail::virtual_runtime_id(node.stable_id);
            node.role = SemanticRole::button;
            node.name = columns_[column].header_text.empty() ? columns_[column].name : columns_[column].header_text;
            node.description = "Column header";
            node.bounds = {absolute.x + placed.column_header.bounds.x, absolute.y + placed.column_header.bounds.y,
                           placed.column_header.bounds.width, placed.column_header.bounds.height};
            node.states = SemanticState::visible | SemanticState::enabled;
            node.actions = {SemanticAction::press};
            nodes.push_back(std::move(node));
        }
    }
    const std::size_t row_end = visible_row_end(layout);
    for (std::size_t row = std::min(first_row_, row_end); row < row_end; ++row) {
        SemanticNode row_node{};
        row_node.stable_id = row_stable_id(row);
        row_node.runtime_id = scrolling_detail::virtual_runtime_id(row_node.stable_id);
        row_node.role = SemanticRole::list_item;
        row_node.name = row_source_ == DataGridViewRowSource::unbound && !rows_[row].header_text.empty()
                            ? rows_[row].header_text
                            : "Row " + std::to_string(row + 1U);
        const Rect row_bounds{layout.client.x, layout.data.y + (static_cast<double>(row) -
                              static_cast<double>(first_row_)) * layout.row_height,
                              layout.client.width, layout.row_height};
        row_node.bounds = {absolute.x + row_bounds.x, absolute.y + row_bounds.y, row_bounds.width,
                           row_bounds.height};
        row_node.states = SemanticState::visible | SemanticState::enabled | SemanticState::focusable;
        if (is_row_selected(row)) row_node.states |= SemanticState::selected;
        if (row_read_only(row) || read_only_) row_node.states |= SemanticState::read_only;
        if (focus_within && current_ && (*current_).row == row) row_node.states |= SemanticState::focused;
        row_node.actions = {SemanticAction::focus, SemanticAction::select};
        for (std::size_t column = first_column; column < column_end; ++column) {
            const DataGridViewColumn& definition = columns_[column];
            const CellParts placed = cell_parts(row, column, layout);
            SemanticNode cell{};
            cell.stable_id = cell_stable_id(row, column);
            cell.runtime_id = scrolling_detail::virtual_runtime_id(cell.stable_id);
            cell.name = definition.header_text.empty() ? definition.name : definition.header_text;
            cell.value = cell_text(row, column);
            cell.bounds = {absolute.x + placed.cell.bounds.x, absolute.y + placed.cell.bounds.y,
                           placed.cell.bounds.width, placed.cell.bounds.height};
            cell.states = SemanticState::visible | SemanticState::enabled | SemanticState::focusable;
            if (is_cell_selected(row, column)) cell.states |= SemanticState::selected;
            const bool current = current_ && *current_ == DataGridViewCellAddress{row, column};
            if (focus_within && current) cell.states |= SemanticState::focused;
            const bool read_only = cell_read_only(row, column);
            if (read_only) cell.states |= SemanticState::read_only;
            if (current && edit_invalid_) cell.states |= SemanticState::invalid;
            cell.actions = {SemanticAction::focus, SemanticAction::select};
            if (definition.kind == DataGridViewColumnKind::check_box) {
                cell.role = SemanticRole::check_box;
                const bool checked = binding_value_to_bool(stored_value(row, column)).value_or(false);
                if (checked) cell.states |= SemanticState::checked;
                if (!read_only) cell.actions.push_back(SemanticAction::press);
            } else if (definition.kind == DataGridViewColumnKind::button) {
                cell.role = SemanticRole::button;
                cell.name = cell.value;
                cell.actions.push_back(SemanticAction::press);
            } else if (definition.kind == DataGridViewColumnKind::combo_box) {
                cell.role = SemanticRole::combo_box;
                if (current && edit_kind_ == EditKind::choice) cell.states |= SemanticState::expanded;
                if (!read_only) {
                    cell.actions.push_back(SemanticAction::set_value);
                    cell.actions.push_back(SemanticAction::expand);
                }
            } else {
                cell.role = SemanticRole::text_box;
                if (!read_only) cell.actions.push_back(SemanticAction::set_value);
            }
            row_node.children.push_back(std::move(cell));
        }
        nodes.push_back(std::move(row_node));
    }
    return nodes;
}

bool DataGridView::on_semantic_action(SemanticAction action, std::string_view value) {
    if (action == SemanticAction::focus && window() != nullptr) {
        return (*window()).request_focus(shared_from_this());
    }
    return Control::on_semantic_action(action, value);
}

bool DataGridView::on_semantic_child_action(std::string_view id, SemanticAction action,
                                            std::string_view value) {
    const std::string column_prefix = std::string(stable_id().value()) + ".column.";
    if (id.compare(0U, column_prefix.size(), column_prefix) == 0) {
        const std::optional<std::size_t> column = column_index(id.substr(column_prefix.size()));
        if (!column || action != SemanticAction::press) return false;
        column_header_click_.emit(*column);
        return true;
    }
    const std::optional<DataGridViewCellAddress> cell = cell_from_stable_id(id);
    if (!cell) {
        const std::optional<std::size_t> row = row_index(id);
        if (!row || columns_.empty()) return false;
        if (action != SemanticAction::focus && action != SemanticAction::select) return false;
        if (action == SemanticAction::focus && window() != nullptr) {
            static_cast<void>((*window()).request_focus(shared_from_this()));
        }
        const std::size_t column = current_ ? (*current_).column : 0U;
        const bool moved = move_current({*row, column}, SelectionGesture::replace);
        return moved;
    }
    if (action == SemanticAction::focus && window() != nullptr) {
        static_cast<void>((*window()).request_focus(shared_from_this()));
    }
    if (!move_current(*cell, SelectionGesture::replace)) return false;
    if (action == SemanticAction::focus || action == SemanticAction::select) return true;
    const DataGridViewColumn& column = columns_[(*cell).column];
    if (action == SemanticAction::press) {
        if (column.kind != DataGridViewColumnKind::check_box && column.kind != DataGridViewColumnKind::button) {
            return false;
        }
        const bool activated = activate_current();
        return activated;
    }
    if (action == SemanticAction::expand) {
        const bool opened = open_choices();
        return opened;
    }
    if (action != SemanticAction::set_value || cell_read_only((*cell).row, (*cell).column)) return false;
    if (column.kind == DataGridViewColumnKind::combo_box) {
        const std::vector<std::string>::const_iterator found =
            std::find(column.items.begin(), column.items.end(), value);
        if (found == column.items.end()) return false;
        const bool stored = set_cell_value((*cell).row, (*cell).column, BindingValue{std::string(value)});
        return stored;
    }
    if (column.kind != DataGridViewColumnKind::text) return false;
    // A text value goes through the same path as typing it: begin, validate,
    // convert and commit.
    if (!begin_text_edit(std::string(value), false)) return false;
    if (end_edit()) return true;
    cancel_edit();
    return false;
}

} // namespace gui_forms
