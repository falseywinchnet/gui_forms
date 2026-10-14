#include "gui_forms/controls/panel/list_view/list_view.hpp"

#include "../collection_control_utilities.hpp"
#include "../details_columns.hpp"

#include "gui_forms/controls/range_control/scroll_bar/h_scroll_bar/h_scroll_bar.hpp"
#include "gui_forms/controls/range_control/scroll_bar/v_scroll_bar/v_scroll_bar.hpp"
#include "gui_forms/delegate.hpp"
#include "gui_forms/text.hpp"
#include "gui_forms/theme/part_painting/part_painting.hpp"
#include "gui_forms/window.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <numeric>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>

namespace gui_forms {
namespace {

// View geometry in logical units at text scale 1. The scroll bar extent is
// not scaled: it matches ScrollBar's own arrow extent.
constexpr double edge_inset = 2.0;
constexpr double cell_padding = 4.0;
constexpr double check_size = 13.0;
constexpr double check_mark_inset = 3.0;
constexpr double minimum_row_height = 18.0;
constexpr double row_text_padding = 6.0;
constexpr double sub_item_padding = 6.0;
constexpr double list_cell_width = 160.0;
constexpr double large_cell_width = 80.0;
constexpr double default_small_icon = 16.0;
constexpr double default_large_icon = 32.0;
constexpr double divider_reach = 4.0;
constexpr double horizontal_key_step = 16.0;
constexpr double scroll_bar_extent = 17.0;
constexpr std::ptrdiff_t wheel_lines = 3;

std::size_t ceiling_quotient(std::size_t count, std::size_t divisor) noexcept {
    if (divisor == 0U) return 0U;
    const std::size_t result = count / divisor + (count % divisor != 0U ? 1U : 0U);
    return result;
}

// Whole lines of `extent` that fit in `available`, at least one.
std::size_t fitting_lines(double available, double extent) noexcept {
    if (extent <= 0.0 || available <= 0.0) return 1U;
    const double lines = std::floor(available / extent);
    const std::size_t result = std::max<std::size_t>(1U, static_cast<std::size_t>(lines));
    return result;
}

// Lines of `extent` at least partly visible in `available`.
std::size_t touched_lines(double available, double extent) noexcept {
    if (extent <= 0.0 || available <= 0.0) return 0U;
    const double lines = std::ceil(available / extent);
    const std::size_t result = static_cast<std::size_t>(lines);
    return result;
}

PartTextAlignment text_alignment(ListViewAlignment alignment) noexcept {
    if (alignment == ListViewAlignment::center) return PartTextAlignment::center;
    if (alignment == ListViewAlignment::right) return PartTextAlignment::end;
    return PartTextAlignment::start;
}

bool valid_alignment(ListViewAlignment alignment) noexcept {
    const bool result = alignment == ListViewAlignment::left ||
                        alignment == ListViewAlignment::center ||
                        alignment == ListViewAlignment::right;
    return result;
}

bool valid_view(ListViewView view) noexcept {
    const bool result = view == ListViewView::details || view == ListViewView::list ||
                        view == ListViewView::small_icon || view == ListViewView::large_icon;
    return result;
}

bool valid_item_text(const ListViewItem& item) {
    if (!validate_utf8(item.text).valid() || !validate_utf8(item.image_key).valid()) return false;
    for (const std::string& sub_item : item.sub_items) {
        if (!validate_utf8(sub_item).valid()) return false;
    }
    return true;
}

// The text an item shows in one column: its text in the first, then its
// sub-items; empty past the last sub-item.
std::string_view column_text(const ListViewItem& item, std::size_t column) noexcept {
    if (column == 0U) return item.text;
    if (column - 1U < item.sub_items.size()) return item.sub_items[column - 1U];
    return {};
}

unsigned char folded(unsigned char byte) noexcept {
    const bool upper = byte >= 'A' && byte <= 'Z';
    const unsigned char result = upper ? static_cast<unsigned char>(byte - 'A' + 'a') : byte;
    return result;
}

// ASCII case folded, then byte order, so "apple" and "Apple" keep a fixed
// order.
bool folded_less(std::string_view left, std::string_view right) noexcept {
    const std::size_t shared = std::min(left.size(), right.size());
    for (std::size_t index = 0; index < shared; ++index) {
        const unsigned char a = folded(static_cast<unsigned char>(left[index]));
        const unsigned char b = folded(static_cast<unsigned char>(right[index]));
        if (a != b) return a < b;
    }
    if (left.size() != right.size()) return left.size() < right.size();
    return left < right;
}

// The order sort_items applies to item positions: by one column's text,
// ascending or descending. Equal texts keep their order (stable sort).
struct ColumnTextOrder final {
    const std::vector<ListViewItem>* items{};
    std::size_t column{};
    bool descending{};

    bool operator()(std::size_t left, std::size_t right) const noexcept {
        const std::string_view a = column_text((*items)[left], column);
        const std::string_view b = column_text((*items)[right], column);
        if (descending) return folded_less(b, a);
        return folded_less(a, b);
    }
};

ImageVisualState image_state(AnatomyStateSet states) noexcept {
    if (states.contains(AnatomyState::disabled)) return ImageVisualState::disabled;
    if (states.contains(AnatomyState::selected)) return ImageVisualState::selected;
    if (states.contains(AnatomyState::hot)) return ImageVisualState::hot;
    return ImageVisualState::normal;
}

std::string column_identity(std::string_view list_id, std::string_view column_id) {
    std::string identity(list_id);
    identity.append(".column.");
    identity.append(column_id);
    return identity;
}

std::string sub_item_identity(std::string_view item_id, std::string_view column_id) {
    std::string identity(item_id);
    identity.append(".sub.");
    identity.append(column_id);
    return identity;
}

} // namespace

ListViewItemSource::~ListViewItemSource() {}

ListView::ListView(StableId stable_id) : Control(std::move(stable_id)) {
    set_focusable(true);
    set_paint_plane(PaintPlane::control);
}

ListView::~ListView() {}

void ListView::initialize_control_tree() {
    const std::string prefix(stable_id().value());
    vertical_bar_ = make_control<VScrollBar>(StableId(prefix + ".vertical_scroll_bar"));
    horizontal_bar_ = make_control<HScrollBar>(StableId(prefix + ".horizontal_scroll_bar"));
    // The bars scroll with the pointer; the keyboard scrolls the view itself.
    (*vertical_bar_).set_focusable(false);
    (*horizontal_bar_).set_focusable(false);
    add_child(vertical_bar_);
    add_child(horizontal_bar_);
    vertical_scrolled_ = (*vertical_bar_).value_changed().subscribe(
        *this, Delegate<double>::bind<ListView, &ListView::on_vertical_scroll>(*this));
    horizontal_scrolled_ = (*horizontal_bar_).value_changed().subscribe(
        *this, Delegate<double>::bind<ListView, &ListView::on_horizontal_scroll>(*this));
    set_child_layout_collapsed(vertical_bar_, true);
    set_child_layout_collapsed(horizontal_bar_, true);
}

// -- model -------------------------------------------------------------------

void ListView::set_view(ListViewView view) {
    require_mutable();
    if (!valid_view(view)) throw std::invalid_argument("ListView view is invalid");
    if (view_ == view) return;
    view_ = view;
    hot_index_.reset();
    hot_column_.reset();
    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

void ListView::set_columns(std::vector<ListViewColumn> columns) {
    require_mutable();
    if (columns.size() > maximum_columns) {
        throw std::invalid_argument("ListView has more columns than maximum_columns");
    }
    std::unordered_set<std::string> identities{};
    for (const ListViewColumn& column : columns) {
        if (column.stable_id.empty() || !validate_utf8(column.stable_id).valid() ||
            !validate_utf8(column.text).valid() || !identities.insert(column.stable_id).second) {
            throw std::invalid_argument("ListView columns require unique stable IDs and valid UTF-8");
        }
        if (!std::isfinite(column.width) || column.width < minimum_column_width ||
            column.width > maximum_column_width || !valid_alignment(column.alignment)) {
            throw std::invalid_argument("ListView column width or alignment is invalid");
        }
    }
    if (resizing_column_ && has_pointer_capture()) set_pointer_capture(false);
    resizing_column_.reset();
    pressed_column_.reset();
    hot_column_.reset();
    columns_ = std::move(columns);
    if (sort_column_ && *sort_column_ >= columns_.size()) {
        sort_column_.reset();
        sort_order_ = ListViewSortOrder::none;
    }
    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

void ListView::set_column_width(std::size_t column, double width) {
    require_mutable();
    if (column >= columns_.size()) throw std::out_of_range("ListView column index is past the end");
    if (!std::isfinite(width) || width < minimum_column_width || width > maximum_column_width) {
        throw std::invalid_argument("ListView column width is outside the column width bounds");
    }
    if (columns_[column].width == width) return;
    columns_[column].width = width;
    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
    const ListViewColumnWidthChange change{std::string(stable_id().value()), column, width};
    publish_change(column_width_changed_, change);
}

std::size_t ListView::item_count() const noexcept {
    if (source_) return virtual_count_;
    return items_.size();
}

std::optional<std::size_t> ListView::index_of(std::string_view item_id) const {
    if (source_) return std::nullopt;
    const ItemIndices::const_iterator found = item_indices_.find(std::string(item_id));
    if (found == item_indices_.end()) return std::nullopt;
    return (*found).second;
}

void ListView::set_items(std::vector<ListViewItem> items) {
    require_mutable();
    if (source_) throw std::logic_error("ListView items cannot be replaced in virtual mode");
    if (items.size() > maximum_items) throw std::invalid_argument("ListView has more items than maximum_items");
    ItemIndices next_indices{};
    next_indices.reserve(items.size());
    for (std::size_t index = 0; index < items.size(); ++index) {
        const ListViewItem& item = items[index];
        if (item.stable_id.empty() || !validate_utf8(item.stable_id).valid() || !valid_item_text(item)) {
            throw std::invalid_argument("ListView items require stable IDs and valid UTF-8");
        }
        const std::pair<ItemIndices::iterator, bool> inserted = next_indices.emplace(item.stable_id, index);
        if (!inserted.second) throw std::invalid_argument("ListView item IDs must be unique");
    }
    // Carry the selection, focus, anchor and hot item over by stable ID.
    std::vector<std::uint8_t> next_selected(items.size(), 0U);
    std::size_t next_count = 0U;
    bool moved = false;
    for (std::size_t index = 0; index < items_.size(); ++index) {
        if (selected_[index] == 0U) continue;
        const ItemIndices::const_iterator found = next_indices.find(items_[index].stable_id);
        if (found == next_indices.end()) continue;
        next_selected[(*found).second] = 1U;
        ++next_count;
        if ((*found).second != index) moved = true;
    }
    const bool selection_changed = moved || next_count != selected_count_;
    std::optional<std::size_t> next_focus{};
    std::optional<std::size_t> next_anchor{};
    if (focused_index_) {
        const ItemIndices::const_iterator found = next_indices.find(items_[*focused_index_].stable_id);
        if (found != next_indices.end()) next_focus = (*found).second;
    }
    if (anchor_index_) {
        const ItemIndices::const_iterator found = next_indices.find(items_[*anchor_index_].stable_id);
        if (found != next_indices.end()) next_anchor = (*found).second;
    }
    items_.swap(items);
    item_indices_.swap(next_indices);
    selected_.swap(next_selected);
    selected_count_ = next_count;
    focused_index_ = next_focus;
    anchor_index_ = next_anchor;
    hot_index_.reset();
    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
    if (selection_changed) publish_selection();
}

void ListView::reset_after_model_change(std::size_t count) {
    selected_.resize(count, 0U);
    std::size_t next_count = 0U;
    for (const std::uint8_t flag : selected_) {
        if (flag != 0U) ++next_count;
    }
    const bool selection_changed = next_count != selected_count_;
    selected_count_ = next_count;
    if (focused_index_ && *focused_index_ >= count) focused_index_.reset();
    if (anchor_index_ && *anchor_index_ >= count) anchor_index_.reset();
    hot_index_.reset();
    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
    if (selection_changed) publish_selection();
}

void ListView::set_virtual_source(std::shared_ptr<ListViewItemSource> source) {
    require_mutable();
    std::size_t count = 0U;
    if (source) {
        count = (*source).item_count();
        if (count > maximum_items) throw std::invalid_argument("ListView source has more items than maximum_items");
    }
    // A new source is a new model: nothing carries over.
    items_.clear();
    item_indices_.clear();
    source_ = std::move(source);
    virtual_count_ = count;
    std::fill(selected_.begin(), selected_.end(), static_cast<std::uint8_t>(0U));
    focused_index_.reset();
    anchor_index_.reset();
    top_item_ = 0U;
    reset_after_model_change(count);
}

void ListView::refresh_virtual_items() {
    require_mutable();
    if (!source_) throw std::logic_error("ListView is not in virtual mode");
    const std::size_t count = (*source_).item_count();
    if (count > maximum_items) throw std::invalid_argument("ListView source has more items than maximum_items");
    virtual_count_ = count;
    reset_after_model_change(count);
}

const ListViewItem& ListView::item_at(std::size_t index, ListViewItem& scratch) const {
    if (!source_) return items_[index];
    (*source_).retrieve_item(index, scratch);
    if (scratch.stable_id.empty() || !validate_utf8(scratch.stable_id).valid()) {
        scratch.stable_id = "#" + std::to_string(index);
    }
    if (!valid_item_text(scratch)) {
        scratch.text.clear();
        scratch.sub_items.clear();
        scratch.image_key.clear();
    }
    return scratch;
}

// -- checks, selection and focus ----------------------------------------------

void ListView::set_check_boxes(bool shown) {
    require_mutable();
    if (check_boxes_ == shown) return;
    check_boxes_ = shown;
    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

bool ListView::item_checked(std::size_t index) const {
    if (index >= item_count()) throw std::out_of_range("ListView item index is past the end");
    ListViewItem scratch;
    const ListViewItem& item = item_at(index, scratch);
    return item.checked;
}

void ListView::set_item_checked(std::size_t index, bool checked) {
    require_mutable();
    if (source_) throw std::logic_error("A virtual ListView's source owns the checked state");
    if (index >= items_.size()) throw std::out_of_range("ListView item index is past the end");
    ListViewItem& item = items_[index];
    if (item.checked == checked) return;
    const ListViewItemCheck change{std::string(stable_id().value()), index, item.stable_id, item.checked,
                                   checked, false};
    item.checked = checked;
    invalidate(Dirty::paint | Dirty::semantics);
    publish_change(item_checked_, change);
}

void ListView::toggle_check(std::size_t index) {
    if (!check_boxes_ || index >= item_count()) return;
    ListViewItem scratch;
    const ListViewItem& item = item_at(index, scratch);
    ListViewItemCheck check{std::string(stable_id().value()), index, item.stable_id, item.checked,
                            !item.checked, false};
    const Control::Ptr lifetime = shared_from_this();
    item_checking_.emit(check);
    // A handler may cancel, dispose the view or replace its items.
    if (!is_alive() || check.cancel || index >= item_count()) return;
    if (!source_) {
        if (items_[index].stable_id != check.item_id || items_[index].checked == check.new_value) return;
        items_[index].checked = check.new_value;
    }
    invalidate(Dirty::paint | Dirty::semantics);
    item_checked_.emit(check);
}

void ListView::set_multi_select(bool multiple) {
    require_mutable();
    if (multi_select_ == multiple) return;
    multi_select_ = multiple;
    if (multiple || selected_count_ <= 1U) {
        invalidate(Dirty::semantics);
        return;
    }
    std::size_t keep = 0U;
    if (focused_index_ && selected_[*focused_index_] != 0U) {
        keep = *focused_index_;
    } else {
        while (keep < selected_.size() && selected_[keep] == 0U) ++keep;
    }
    static_cast<void>(select_only(keep));
    anchor_index_ = keep;
    publish_selection();
}

void ListView::set_full_row_select(bool full_row) {
    require_mutable();
    if (full_row_select_ == full_row) return;
    full_row_select_ = full_row;
    invalidate(Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

std::vector<std::size_t> ListView::selected_indices() const {
    std::vector<std::size_t> indices;
    indices.reserve(selected_count_);
    for (std::size_t index = 0; index < selected_.size(); ++index) {
        if (selected_[index] != 0U) indices.push_back(index);
    }
    return indices;
}

bool ListView::is_selected(std::size_t index) const {
    if (index >= item_count()) throw std::out_of_range("ListView item index is past the end");
    const bool result = selected_[index] != 0U;
    return result;
}

bool ListView::select_only(std::size_t index) {
    if (selected_count_ == 1U && selected_[index] != 0U) return false;
    if (selected_count_ != 0U) std::fill(selected_.begin(), selected_.end(), static_cast<std::uint8_t>(0U));
    selected_[index] = 1U;
    selected_count_ = 1U;
    return true;
}

bool ListView::select_none() {
    if (selected_count_ == 0U) return false;
    std::fill(selected_.begin(), selected_.end(), static_cast<std::uint8_t>(0U));
    selected_count_ = 0U;
    return true;
}

bool ListView::toggle_selected(std::size_t index) {
    if (selected_[index] != 0U) {
        selected_[index] = 0U;
        --selected_count_;
        return true;
    }
    if (!multi_select_) return select_only(index);
    selected_[index] = 1U;
    ++selected_count_;
    return true;
}

bool ListView::assign_selected(std::size_t index, bool selected) {
    const bool current = selected_[index] != 0U;
    if (current == selected) return false;
    if (selected && !multi_select_) return select_only(index);
    selected_[index] = selected ? 1U : 0U;
    if (selected) {
        ++selected_count_;
    } else {
        --selected_count_;
    }
    return true;
}

bool ListView::select_range(std::size_t anchor, std::size_t index, bool keep_existing) {
    const std::size_t first = std::min(anchor, index);
    const std::size_t last = std::max(anchor, index);
    const std::size_t length = last - first + 1U;
    std::size_t already = 0U;
    for (std::size_t position = first; position <= last; ++position) {
        if (selected_[position] != 0U) ++already;
    }
    const bool changed = keep_existing ? already != length
                                       : (already != length || selected_count_ != length);
    if (!changed) return false;
    if (!keep_existing) {
        std::fill(selected_.begin(), selected_.end(), static_cast<std::uint8_t>(0U));
        selected_count_ = 0U;
        already = 0U;
    }
    std::fill(selected_.begin() + static_cast<std::ptrdiff_t>(first),
              selected_.begin() + static_cast<std::ptrdiff_t>(last) + 1, static_cast<std::uint8_t>(1U));
    selected_count_ += length - already;
    return true;
}

void ListView::publish_selection() {
    invalidate(Dirty::paint | Dirty::semantics);
    const ListViewSelectionChange change{std::string(stable_id().value()), selected_count_, focused_index_};
    publish_change(selection_changed_, change);
}

void ListView::set_selected(std::size_t index, bool selected) {
    require_mutable();
    if (index >= item_count()) throw std::out_of_range("ListView item index is past the end");
    if (!assign_selected(index, selected)) return;
    if (selected) anchor_index_ = index;
    publish_selection();
}

void ListView::select_all() {
    require_mutable();
    const std::size_t count = item_count();
    if (count == 0U) return;
    if (!multi_select_) {
        // One item at most: the focused one, or the first.
        const std::size_t only = focused_index_.value_or(0U);
        if (select_only(only)) publish_selection();
        return;
    }
    if (selected_count_ == count) return;
    std::fill(selected_.begin(), selected_.end(), static_cast<std::uint8_t>(1U));
    selected_count_ = count;
    publish_selection();
}

void ListView::clear_selection() {
    require_mutable();
    if (select_none()) publish_selection();
}

void ListView::set_focused_index(std::optional<std::size_t> index) {
    require_mutable();
    if (index && *index >= item_count()) throw std::out_of_range("ListView item index is past the end");
    if (focused_index_ == index) return;
    focused_index_ = index;
    if (index) ensure_visible(*index);
    invalidate(Dirty::paint | Dirty::semantics);
}

void ListView::move_focus(std::size_t index) {
    focused_index_ = index;
    ensure_visible(index);
    invalidate(Dirty::paint | Dirty::semantics);
}

void ListView::choose(std::size_t index, Modifier modifiers, bool move_only) {
    const bool control = has_modifier(modifiers, Modifier::control) || has_modifier(modifiers, Modifier::meta);
    const bool shift = has_modifier(modifiers, Modifier::shift);
    if (move_only && multi_select_) {
        move_focus(index);
        return;
    }
    bool changed = false;
    if (multi_select_ && shift) {
        const std::size_t anchor = anchor_index_.value_or(focused_index_.value_or(index));
        changed = select_range(anchor, index, control);
        anchor_index_ = anchor;
    } else if (multi_select_ && control) {
        changed = toggle_selected(index);
        anchor_index_ = index;
    } else {
        changed = select_only(index);
        anchor_index_ = index;
    }
    move_focus(index);
    if (changed) publish_selection();
}

void ListView::activate(std::size_t index) {
    if (index >= item_count()) return;
    ListViewItem scratch;
    const ListViewItem& item = item_at(index, scratch);
    const ListViewItemActivation activation{std::string(stable_id().value()), index, item.stable_id};
    item_activated_.emit(activation);
}

// -- scrolling ---------------------------------------------------------------

std::size_t ListView::top_index() const {
    const Layout placed = layout();
    const std::size_t result = std::min(placed.shown_count, placed.top_line * placed.per_line);
    return result;
}

void ListView::ensure_visible(std::size_t index) {
    if (index >= item_count()) throw std::out_of_range("ListView item index is past the end");
    const Layout placed = layout();
    const std::size_t line = index / placed.per_line;
    std::size_t top = placed.top_line;
    if (line < top) {
        top = line;
    } else if (line >= top + placed.page_lines) {
        top = line - placed.page_lines + 1U;
    }
    if (top != placed.top_line) set_top_line(top);
}

void ListView::set_top_line(std::size_t line) {
    const Layout placed = layout();
    const std::size_t clamped = std::min(line, maximum_top_line(placed));
    const std::size_t item = clamped * placed.per_line;
    if (item == top_item_ && clamped == placed.top_line) return;
    top_item_ = item;
    hot_index_.reset();
    invalidate(Dirty::paint | Dirty::hit_test | Dirty::semantics);
    sync_scroll_bars(layout());
}

void ListView::scroll_lines(std::ptrdiff_t lines) {
    const Layout placed = layout();
    const std::ptrdiff_t current = static_cast<std::ptrdiff_t>(placed.top_line);
    const std::ptrdiff_t maximum = static_cast<std::ptrdiff_t>(maximum_top_line(placed));
    const std::ptrdiff_t next = std::clamp<std::ptrdiff_t>(current + lines, 0, maximum);
    set_top_line(static_cast<std::size_t>(next));
}

void ListView::set_horizontal_offset(double offset) {
    require_mutable();
    if (!std::isfinite(offset) || offset < 0.0) throw std::invalid_argument("ListView horizontal offset is invalid");
    horizontal_offset_ = offset;
    const Layout placed = layout();
    horizontal_offset_ = placed.horizontal_offset;
    invalidate(Dirty::paint | Dirty::hit_test | Dirty::semantics);
    sync_scroll_bars(placed);
}

void ListView::sync_scroll_bars(const Layout& placed) {
    if (!vertical_bar_ || !horizontal_bar_) return;
    synchronizing_scroll_ = true;
    const double maximum_line = static_cast<double>(maximum_top_line(placed));
    const double page = static_cast<double>(placed.page_lines);
    const double top = static_cast<double>(placed.top_line);
    if (!placed.vertical_bar.empty() && maximum_line > 0.0) {
        VScrollBar& bar = *vertical_bar_;
        bar.set_range(0.0, maximum_line);
        bar.set_small_change(1.0);
        bar.set_large_change(page);
        bar.set_value(std::clamp(top, 0.0, maximum_line));
    }
    if (!placed.horizontal_bar.empty()) {
        HScrollBar& bar = *horizontal_bar_;
        if (view_ == ListViewView::list && maximum_line > 0.0) {
            bar.set_range(0.0, maximum_line);
            bar.set_small_change(1.0);
            bar.set_large_change(page);
            bar.set_value(std::clamp(top, 0.0, maximum_line));
        } else if (view_ == ListViewView::details) {
            const double visible = placed.viewport.width / placed.scale;
            const double maximum_offset = std::max(0.0, placed.columns_width / placed.scale - visible);
            if (maximum_offset > 0.0) {
                bar.set_range(0.0, maximum_offset);
                bar.set_small_change(horizontal_key_step);
                bar.set_large_change(std::max(1.0, visible));
                bar.set_value(std::clamp(placed.horizontal_offset, 0.0, maximum_offset));
            }
        }
    }
    synchronizing_scroll_ = false;
}

void ListView::on_vertical_scroll(double value) {
    if (synchronizing_scroll_ || !is_alive()) return;
    const double line = std::max(0.0, std::round(value));
    set_top_line(static_cast<std::size_t>(line));
}

void ListView::on_horizontal_scroll(double value) {
    if (synchronizing_scroll_ || !is_alive()) return;
    if (view_ == ListViewView::list) {
        const double line = std::max(0.0, std::round(value));
        set_top_line(static_cast<std::size_t>(line));
        return;
    }
    if (view_ != ListViewView::details || horizontal_offset_ == value) return;
    horizontal_offset_ = std::max(0.0, value);
    invalidate(Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

// -- sorting -----------------------------------------------------------------

void ListView::set_sort_indicator(std::optional<std::size_t> column, ListViewSortOrder order) {
    require_mutable();
    if (column && *column >= columns_.size()) throw std::out_of_range("ListView sort column is past the end");
    if (order != ListViewSortOrder::none && order != ListViewSortOrder::ascending &&
        order != ListViewSortOrder::descending) {
        throw std::invalid_argument("ListView sort order is invalid");
    }
    if (!column || order == ListViewSortOrder::none) {
        column.reset();
        order = ListViewSortOrder::none;
    }
    if (sort_column_ == column && sort_order_ == order) return;
    sort_column_ = column;
    sort_order_ = order;
    invalidate(Dirty::paint | Dirty::semantics);
}

void ListView::sort_items(std::size_t column, ListViewSortOrder order) {
    require_mutable();
    if (source_) throw std::logic_error("A virtual ListView's source owns the item order");
    if (column >= columns_.size()) throw std::out_of_range("ListView sort column is past the end");
    set_sort_indicator(column, order);
    if (order == ListViewSortOrder::none) return;
    // `order_of[new position]` is the item's old position.
    std::vector<std::size_t> order_of(items_.size(), 0U);
    std::iota(order_of.begin(), order_of.end(), std::size_t{0U});
    const ColumnTextOrder by_text{&items_, column, order == ListViewSortOrder::descending};
    std::stable_sort(order_of.begin(), order_of.end(), by_text);
    // `position_of[old position]` is its new position.
    std::vector<std::size_t> position_of(items_.size(), 0U);
    std::vector<ListViewItem> sorted;
    sorted.reserve(items_.size());
    std::vector<std::uint8_t> sorted_flags(items_.size(), 0U);
    bool moved = false;
    for (std::size_t position = 0; position < order_of.size(); ++position) {
        const std::size_t old = order_of[position];
        position_of[old] = position;
        sorted.push_back(std::move(items_[old]));
        sorted_flags[position] = selected_[old];
        if (selected_[old] != 0U && old != position) moved = true;
    }
    items_.swap(sorted);
    selected_.swap(sorted_flags);
    item_indices_.clear();
    for (std::size_t index = 0; index < items_.size(); ++index) {
        item_indices_.emplace(items_[index].stable_id, index);
    }
    if (focused_index_) focused_index_ = position_of[*focused_index_];
    if (anchor_index_) anchor_index_ = position_of[*anchor_index_];
    hot_index_.reset();
    invalidate(Dirty::paint | Dirty::hit_test | Dirty::semantics);
    if (moved) publish_selection();
}

// -- images and font -----------------------------------------------------------

void ListView::set_small_image_list(std::shared_ptr<ImageList> images) {
    require_mutable();
    if (images && (!(*images).is_alive() || (window() && !(*images).belongs_to(*window())))) {
        throw std::invalid_argument("ListView requires a live ImageList of its own Window");
    }
    if (small_images_ == images) return;
    small_images_changed_.disconnect();
    small_images_ = std::move(images);
    if (small_images_) {
        small_images_changed_ = (*small_images_).changed().subscribe(
            *this, Delegate<const ImageListChange&>::bind<ListView, &ListView::image_list_changed>(*this));
    }
    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

void ListView::set_large_image_list(std::shared_ptr<ImageList> images) {
    require_mutable();
    if (images && (!(*images).is_alive() || (window() && !(*images).belongs_to(*window())))) {
        throw std::invalid_argument("ListView requires a live ImageList of its own Window");
    }
    if (large_images_ == images) return;
    large_images_changed_.disconnect();
    large_images_ = std::move(images);
    if (large_images_) {
        large_images_changed_ = (*large_images_).changed().subscribe(
            *this, Delegate<const ImageListChange&>::bind<ListView, &ListView::image_list_changed>(*this));
    }
    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

void ListView::image_list_changed(const ImageListChange&) {
    if (is_alive()) invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test);
}

void ListView::on_attached_to_window() {
    Control::on_attached_to_window();
    if ((small_images_ && !(*small_images_).belongs_to(*window())) ||
        (large_images_ && !(*large_images_).belongs_to(*window()))) {
        throw std::logic_error("ListView cannot attach to a Window other than its ImageLists'");
    }
}

void ListView::set_font(FontSpec font) {
    require_mutable();
    if (!valid_font_spec(font)) throw std::invalid_argument("ListView font specification is invalid");
    if (font_ == font) return;
    font_ = font;
    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

// -- layout ------------------------------------------------------------------

double ListView::text_line_height() const {
    const ResolvedTextLayout metrics = resolve_text_layout_utf8("Mg", effective_font(font_));
    return metrics.logical_size.height;
}

Size ListView::icon_size(bool large) const noexcept {
    const double scale = effective_text_scale();
    const std::shared_ptr<ImageList>& images = large ? large_images_ : small_images_;
    const double fallback = large ? default_large_icon : default_small_icon;
    Size size{fallback, fallback};
    if (images) size = (*images).image_size();
    const Size result{size.width * scale, size.height * scale};
    return result;
}

ListView::Layout ListView::layout() const {
    return layout_for(client_rectangle());
}

ListView::Layout ListView::layout_for(Rect client) const {
    Layout placed;
    const double scale = effective_text_scale();
    placed.scale = scale;
    placed.well = client;
    const double inset = edge_inset * scale;
    const Rect content{client.x + inset, client.y + inset, std::max(0.0, client.width - inset * 2.0),
                       std::max(0.0, client.height - inset * 2.0)};
    const double line = text_line_height();
    double row = std::max(minimum_row_height * scale, line + row_text_padding * scale);
    if (check_boxes_) row = std::max(row, (check_size + cell_padding) * scale);
    if (small_images_) row = std::max(row, icon_size(false).height + cell_padding * scale);
    placed.row_height = row;
    const bool details = view_ == ListViewView::details;
    const bool list = view_ == ListViewView::list;
    double columns_width = 0.0;
    for (const ListViewColumn& column : columns_) columns_width += column.width * scale;
    placed.columns_width = columns_width;
    // The details view shows nothing without columns, as in Windows Forms.
    placed.shown_count = details && columns_.empty() ? 0U : item_count();
    const double header_height = details && !columns_.empty() ? row + 2.0 * scale : 0.0;
    if (details) {
        placed.cell = {columns_width, row};
    } else if (view_ == ListViewView::large_icon) {
        const Size large = icon_size(true);
        placed.cell = {std::max(large_cell_width * scale, large.width + cell_padding * 4.0 * scale),
                       large.height + line + (cell_padding * 3.0 + row_text_padding) * scale};
    } else {
        placed.cell = {list_cell_width * scale, row};
    }
    const double line_extent = list ? placed.cell.width : placed.cell.height;
    // Showing one bar can shrink the area enough to need the other, so
    // settle the bars over at most three passes.
    bool vertical = false;
    bool horizontal = false;
    Rect area{};
    for (int pass = 0; pass < 3; ++pass) {
        const double bar_width = vertical ? scroll_bar_extent : 0.0;
        const double bar_height = horizontal ? scroll_bar_extent : 0.0;
        area = {content.x, content.y + header_height, std::max(0.0, content.width - bar_width),
                std::max(0.0, content.height - header_height - bar_height)};
        if (details) {
            placed.per_line = 1U;
        } else if (list) {
            placed.per_line = fitting_lines(area.height, row);
        } else {
            placed.per_line = fitting_lines(area.width, placed.cell.width);
        }
        placed.line_count = ceiling_quotient(placed.shown_count, placed.per_line);
        placed.page_lines = fitting_lines(list ? area.width : area.height, line_extent);
        const bool overflows = placed.line_count > placed.page_lines;
        const bool next_vertical = !list && overflows;
        const bool next_horizontal = (list && overflows) || (details && columns_width > area.width + 0.5);
        if (next_vertical == vertical && next_horizontal == horizontal) break;
        vertical = next_vertical;
        horizontal = next_horizontal;
    }
    placed.viewport = area;
    if (header_height > 0.0) placed.header = {content.x, content.y, area.width, header_height};
    const double bar_bottom = horizontal ? scroll_bar_extent : 0.0;
    if (vertical) {
        placed.vertical_bar = {content.x + content.width - scroll_bar_extent, content.y, scroll_bar_extent,
                               std::max(0.0, content.height - bar_bottom)};
    }
    if (horizontal) {
        placed.horizontal_bar = {content.x, content.y + content.height - scroll_bar_extent,
                                 std::max(0.0, content.width - (vertical ? scroll_bar_extent : 0.0)),
                                 scroll_bar_extent};
    }
    if (vertical && horizontal) {
        placed.corner = {content.x + content.width - scroll_bar_extent,
                         content.y + content.height - scroll_bar_extent, scroll_bar_extent, scroll_bar_extent};
    }
    placed.visible_lines = touched_lines(list ? area.width : area.height, line_extent);
    const std::size_t top_line = top_item_ / placed.per_line;
    placed.top_line = std::min(top_line, maximum_top_line(placed));
    if (details) {
        const double maximum_offset = std::max(0.0, (columns_width - area.width) / scale);
        placed.horizontal_offset = std::clamp(horizontal_offset_, 0.0, maximum_offset);
    }
    return placed;
}

std::size_t ListView::maximum_top_line(const Layout& placed) const noexcept {
    if (placed.line_count <= placed.page_lines) return 0U;
    const std::size_t result = placed.line_count - placed.page_lines;
    return result;
}

std::size_t ListView::first_realized(const Layout& placed) const noexcept {
    const std::size_t result = std::min(placed.shown_count, placed.top_line * placed.per_line);
    return result;
}

std::size_t ListView::end_realized(const Layout& placed) const noexcept {
    const std::size_t lines = placed.top_line + placed.visible_lines;
    const std::size_t result = std::min(placed.shown_count, lines * placed.per_line);
    return result;
}

Rect ListView::item_bounds(const Layout& placed, std::size_t index) const noexcept {
    const std::size_t line = index / placed.per_line;
    const std::size_t position = index % placed.per_line;
    const double relative = static_cast<double>(line) - static_cast<double>(placed.top_line);
    const double along = static_cast<double>(position);
    const Rect area = placed.viewport;
    if (view_ == ListViewView::details) {
        const Rect row{area.x - placed.horizontal_offset * placed.scale, area.y + relative * placed.row_height,
                       placed.cell.width, placed.cell.height};
        return row;
    }
    if (view_ == ListViewView::list) {
        const Rect cell{area.x + relative * placed.cell.width, area.y + along * placed.cell.height,
                        placed.cell.width, placed.cell.height};
        return cell;
    }
    const Rect cell{area.x + along * placed.cell.width, area.y + relative * placed.cell.height,
                    placed.cell.width, placed.cell.height};
    return cell;
}

Rect ListView::column_cell(const Layout& placed, std::size_t column) const noexcept {
    double x = placed.viewport.x - placed.horizontal_offset * placed.scale;
    for (std::size_t index = 0; index < column; ++index) x += columns_[index].width * placed.scale;
    const Rect cell{x, placed.header.y, columns_[column].width * placed.scale, placed.header.height};
    return cell;
}

bool ListView::column_visible(const Layout& placed, std::size_t column) const noexcept {
    if (placed.header.empty()) return false;
    const Rect cell = column_cell(placed, column);
    const bool result = cell.x < placed.viewport.x + placed.viewport.width && cell.x + cell.width > placed.viewport.x;
    return result;
}

std::size_t ListView::visible_column_count(const Layout& placed) const noexcept {
    std::size_t count = 0U;
    for (std::size_t column = 0; column < columns_.size(); ++column) {
        if (column_visible(placed, column)) ++count;
    }
    return count;
}

std::size_t ListView::visible_column(const Layout& placed, std::size_t position) const noexcept {
    std::size_t seen = 0U;
    for (std::size_t column = 0; column < columns_.size(); ++column) {
        if (!column_visible(placed, column)) continue;
        if (seen == position) return column;
        ++seen;
    }
    return columns_.size();
}

// One item's parts. `clip` bounds every part: the viewport when publishing,
// the item's own cell when painting (the painter clips to the viewport), so
// text scrolled partly out of view keeps its place instead of moving into
// the visible part. In the details view the check box, icon and text also
// stay inside the first column.
ListView::ItemParts ListView::item_parts(const Layout& placed, std::size_t index, Rect clip) const {
    const double scale = placed.scale;
    const Rect cell = item_bounds(placed, index);
    Rect check_well{};
    Rect icon{};
    Rect text{};
    Rect face{};
    Rect inner_clip = clip;
    if (view_ == ListViewView::large_icon) {
        const Size image = icon_size(true);
        const double top = cell.y + cell_padding * scale;
        if (large_images_) {
            icon = {std::floor(cell.x + (cell.width - image.width) * 0.5), top, image.width, image.height};
        }
        if (check_boxes_) check_well = {cell.x + 2.0 * scale, top, check_size * scale, check_size * scale};
        const double text_top = top + image.height + cell_padding * scale;
        text = {cell.x + 2.0 * scale, text_top, std::max(0.0, cell.width - 4.0 * scale),
                std::max(0.0, cell.y + cell.height - 2.0 * scale - text_top)};
        face = {cell.x + scale, cell.y + scale, std::max(0.0, cell.width - 2.0 * scale),
                std::max(0.0, cell.height - 2.0 * scale)};
    } else {
        double first_right = cell.x + cell.width;
        if (view_ == ListViewView::details) {
            first_right = cell.x + columns_.front().width * scale;
            inner_clip = Rect::intersection(clip, {cell.x, cell.y, first_right - cell.x, cell.height});
        }
        const double middle = cell.y + cell.height * 0.5;
        double x = cell.x + cell_padding * scale;
        if (check_boxes_) {
            const double side = check_size * scale;
            check_well = {x, std::floor(middle - side * 0.5), side, side};
            x += side + cell_padding * scale;
        }
        if (small_images_) {
            const Size image = icon_size(false);
            icon = {x, std::floor(middle - image.height * 0.5), image.width, image.height};
            x += image.width + cell_padding * scale;
        }
        text = {x, cell.y, std::max(0.0, first_right - cell_padding * scale - x), cell.height};
        const double face_left = x - 2.0 * scale;
        face = {face_left, cell.y, std::max(0.0, first_right - face_left), cell.height};
        if (view_ == ListViewView::details && full_row_select_) face = cell;
    }
    Rect check{};
    if (!check_well.empty()) {
        const double inset = check_mark_inset * scale;
        check = {check_well.x + inset, check_well.y + inset, std::max(0.0, check_well.width - inset * 2.0),
                 std::max(0.0, check_well.height - inset * 2.0)};
    }
    const Rect face_clip = view_ == ListViewView::details && full_row_select_ ? clip : inner_clip;
    ItemParts parts;
    parts.cell = PartGeometry::from_bounds(Rect::intersection(cell, clip));
    parts.item_face = PartGeometry::from_bounds(Rect::intersection(face, face_clip));
    parts.check_well = PartGeometry::from_bounds(Rect::intersection(check_well, inner_clip));
    parts.check = PartGeometry::from_bounds(Rect::intersection(check, inner_clip));
    parts.icon = PartGeometry::from_bounds(Rect::intersection(icon, inner_clip));
    parts.text = PartGeometry::from_bounds(Rect::intersection(text, inner_clip));
    parts.cell_focus = parts.item_face;
    return parts;
}

// One details-view cell past the first column, clipped as item_parts is.
ListView::SubItemParts ListView::sub_item_parts(const Layout& placed, std::size_t index, std::size_t column,
                                                Rect clip) const {
    const Rect row = item_bounds(placed, index);
    const Rect header = column_cell(placed, column);
    const Rect cell{header.x, row.y, header.width, row.height};
    const double padding = sub_item_padding * placed.scale;
    const Rect text{cell.x + padding, cell.y, std::max(0.0, cell.width - padding * 2.0), cell.height};
    const Rect visible = Rect::intersection(cell, clip);
    SubItemParts parts;
    parts.sub_item_cell = PartGeometry::from_bounds(visible);
    parts.sub_item_text = PartGeometry::from_bounds(Rect::intersection(text, visible));
    return parts;
}

// One column header, from the layout ObjectView's headers share. `clip` is
// the header band when publishing, the column's own cell when painting.
ListView::ColumnParts ListView::column_parts(const Layout& placed, std::size_t column, Rect clip) const {
    const bool sorted = sort_column_ == column && sort_order_ != ListViewSortOrder::none;
    const details_detail::ColumnHeaderGeometry header =
        details_detail::column_header_geometry(column_cell(placed, column), clip, sorted, placed.scale);
    ColumnParts parts;
    parts.column_header = PartGeometry::from_bounds(header.header);
    parts.column_label = PartGeometry::from_bounds(header.label);
    parts.sort_glyph = PartGeometry::from_bounds(header.sort_glyph);
    parts.column_divider = PartGeometry::from_bounds(header.divider);
    parts.column_grip = PartGeometry::from_bounds(header.grip);
    return parts;
}

namespace {

// Application-defined column states, as ObjectView declares them.
constexpr CustomStateIndex sorted_ascending_state{0U};
constexpr CustomStateIndex sorted_descending_state{1U};

} // namespace

AnatomyStateSet ListView::item_states(std::size_t index, bool checked) const {
    AnatomyStateSet states;
    if (selected_[index] != 0U) states.insert(AnatomyState::selected);
    if (hot_index_ == index) states.insert(AnatomyState::hot);
    if (focused_index_ == index) states.insert(AnatomyState::current);
    if (check_boxes_ && checked) states.insert(AnatomyState::checked);
    if (!effectively_enabled()) states.insert(AnatomyState::disabled);
    const Window* const host = attached_window();
    if (focused_ && focused_index_ == index && host != nullptr && (*host).focus_cue_visible()) {
        states.insert(AnatomyState::focus_visible);
    }
    return states;
}

AnatomyStateSet ListView::column_states(std::size_t column) const {
    AnatomyStateSet states;
    if (hot_column_ == column) states.insert(AnatomyState::hot);
    if (pressed_column_ == column) states.insert(AnatomyState::pressed);
    if (resizing_column_ == column) states.insert(AnatomyState::captured);
    if (sort_column_ == column && sort_order_ == ListViewSortOrder::ascending) {
        states.insert(sorted_ascending_state);
    }
    if (sort_column_ == column && sort_order_ == ListViewSortOrder::descending) {
        states.insert(sorted_descending_state);
    }
    return states;
}

void ListView::arrange(Rect final_bounds) {
    arrange_self(final_bounds);
    const Layout placed = layout_for({0.0, 0.0, final_bounds.width, final_bounds.height});
    if (vertical_bar_) {
        const bool shown = !placed.vertical_bar.empty();
        set_child_layout_collapsed(vertical_bar_, !shown);
        if (shown) set_child_layout(vertical_bar_, placed.vertical_bar);
    }
    if (horizontal_bar_) {
        const bool shown = !placed.horizontal_bar.empty();
        set_child_layout_collapsed(horizontal_bar_, !shown);
        if (shown) set_child_layout(horizontal_bar_, placed.horizontal_bar);
    }
    // Keep the stored scroll within the new bounds.
    top_item_ = std::min(top_item_, placed.top_line * placed.per_line);
    horizontal_offset_ = placed.horizontal_offset;
    sync_scroll_bars(placed);
}

// -- paint -------------------------------------------------------------------

void ListView::paint_column(Painter& painter, const Layout& placed, std::size_t column) {
    const Rect cell = column_cell(placed, column);
    const ColumnParts parts = column_parts(placed, column, cell);
    const AnatomyStateSet states = column_states(column);
    PartPaint part;
    part.font = effective_font(font_);
    part.states = states;
    // Header cells are flat at rest and show a plate when hot or pressed:
    // the menu_item family's look.
    part.family = ControlVisualRole::menu_item;
    part.role = PartRole::face;
    part.bounds = parts.column_header.bounds;
    paint_part(painter, *this, part);
    part.role = PartRole::label;
    part.bounds = parts.column_label.bounds;
    part.alignment = text_alignment(columns_[column].alignment);
    details_detail::elide_to_width(painter, columns_[column].text, part.font, part.bounds.width, paint_text_);
    part.text = paint_text_;
    paint_part(painter, *this, part);
    part.text = {};
    part.alignment = PartTextAlignment::start;
    if (parts.sort_glyph.present) {
        part.role = PartRole::glyph;
        part.bounds = parts.sort_glyph.bounds;
        part.mark = sort_order_ == ListViewSortOrder::ascending ? PartMark::arrow_up : PartMark::arrow_down;
        paint_part(painter, *this, part);
        part.mark = PartMark::none;
    }
    part.family = ControlVisualRole::panel;
    part.role = PartRole::divider;
    part.bounds = parts.column_divider.bounds;
    paint_part(painter, *this, part);
    // column_grip is the divider's drag zone. The divider is its visible
    // sign; painting the grip role would draw grip dots at every divider.
}

void ListView::paint_item(Painter& painter, const Layout& placed, std::size_t index) {
    const ListViewItem& item = item_at(index, paint_item_);
    const Rect cell = item_bounds(placed, index);
    const ItemParts parts = item_parts(placed, index, cell);
    const AnatomyStateSet states = item_states(index, item.checked);
    PartPaint part;
    part.font = effective_font(font_);
    part.states = states;
    part.family = ControlVisualRole::selection;
    part.role = PartRole::face;
    part.bounds = parts.item_face.bounds;
    paint_part(painter, *this, part);
    if (parts.check_well.present) {
        part.family = ControlVisualRole::choice;
        part.role = PartRole::well;
        part.bounds = parts.check_well.bounds;
        paint_part(painter, *this, part);
        // An unchecked box shows no mark: an indicator without a mark
        // would paint a plate.
        if (states.contains(AnatomyState::checked)) {
            part.role = PartRole::indicator;
            part.bounds = parts.check.bounds;
            part.mark = PartMark::check;
            paint_part(painter, *this, part);
            part.mark = PartMark::none;
        }
        part.family = ControlVisualRole::selection;
    }
    if (parts.icon.present && !item.image_key.empty()) {
        const std::shared_ptr<ImageList>& images =
            view_ == ListViewView::large_icon ? large_images_ : small_images_;
        const double density = attached_window() != nullptr ? (*attached_window()).scale() : 1.0;
        const ImageListResolution resolved = (*images).resolve(item.image_key, image_state(states), density);
        if (resolved) {
            part.role = PartRole::icon;
            part.bounds = parts.icon.bounds;
            part.image = resolved.image;
            paint_part(painter, *this, part);
            part.image = {};
        }
    }
    part.role = PartRole::content;
    part.bounds = parts.text.bounds;
    part.alignment = view_ == ListViewView::large_icon ? PartTextAlignment::center : PartTextAlignment::start;
    details_detail::elide_to_width(painter, item.text, part.font, part.bounds.width, paint_text_);
    part.text = paint_text_;
    paint_part(painter, *this, part);
    part.text = {};
    if (view_ == ListViewView::details) {
        AnatomyStateSet sub_states;
        if (states.contains(AnatomyState::selected)) sub_states.insert(AnatomyState::selected);
        if (states.contains(AnatomyState::disabled)) sub_states.insert(AnatomyState::disabled);
        part.states = sub_states;
        for (std::size_t column = 1; column < columns_.size(); ++column) {
            if (!column_visible(placed, column)) continue;
            const Rect header = column_cell(placed, column);
            const Rect sub_cell{header.x, cell.y, header.width, cell.height};
            const SubItemParts sub = sub_item_parts(placed, index, column, sub_cell);
            part.bounds = sub.sub_item_text.bounds;
            part.alignment = text_alignment(columns_[column].alignment);
            details_detail::elide_to_width(painter, column_text(item, column), part.font, part.bounds.width,
                                           paint_text_);
            part.text = paint_text_;
            paint_part(painter, *this, part);
            part.text = {};
        }
        part.states = states;
        part.alignment = PartTextAlignment::start;
    }
    part.role = PartRole::focus;
    part.bounds = parts.cell_focus.bounds;
    paint_part(painter, *this, part);
}

void ListView::on_paint(Painter& painter, Rect) {
    const Layout placed = layout();
    AnatomyStateSet control_states;
    const Window* const host = attached_window();
    if (focused_ && host != nullptr && (*host).focus_cue_visible()) {
        control_states.insert(AnatomyState::focus_visible);
    }
    PartPaint well;
    well.family = ControlVisualRole::editor;
    well.role = PartRole::well;
    well.bounds = placed.well;
    paint_part(painter, *this, well);
    if (!placed.header.empty()) {
        PartPaint header;
        header.family = ControlVisualRole::panel;
        header.role = PartRole::face;
        header.bounds = placed.header;
        paint_part(painter, *this, header);
        painter.save();
        painter.clip_rect(placed.header);
        for (std::size_t column = 0; column < columns_.size(); ++column) {
            if (column_visible(placed, column)) paint_column(painter, placed, column);
        }
        painter.restore();
    }
    if (!placed.viewport.empty()) {
        painter.save();
        painter.clip_rect(placed.viewport);
        const std::size_t end = end_realized(placed);
        for (std::size_t index = first_realized(placed); index < end; ++index) {
            paint_item(painter, placed, index);
        }
        painter.restore();
    }
    if (!placed.corner.empty()) {
        PartPaint corner;
        corner.family = ControlVisualRole::panel;
        corner.role = PartRole::face;
        corner.bounds = placed.corner;
        paint_part(painter, *this, corner);
    }
    // With no item to carry the focus cue, the view itself shows it.
    if (placed.shown_count == 0U) {
        PartPaint focus;
        focus.family = ControlVisualRole::editor;
        focus.role = PartRole::focus;
        focus.bounds = placed.well;
        focus.states = control_states;
        paint_part(painter, *this, focus);
    }
}

// -- input -------------------------------------------------------------------

ListView::Hit ListView::hit_test(const Layout& placed, Point local) const {
    Hit hit;
    if (!placed.header.empty() && placed.header.contains(local)) {
        const double reach = divider_reach * placed.scale;
        for (std::size_t column = 0; column < columns_.size(); ++column) {
            if (!column_visible(placed, column)) continue;
            const Rect cell = column_cell(placed, column);
            if (std::abs(local.x - (cell.x + cell.width)) <= reach) {
                hit.zone = HitZone::divider;
                hit.index = column;
                return hit;
            }
        }
        for (std::size_t column = 0; column < columns_.size(); ++column) {
            if (column_cell(placed, column).contains(local)) {
                hit.zone = HitZone::header;
                hit.index = column;
                return hit;
            }
        }
        return hit;
    }
    if (placed.viewport.empty() || !placed.viewport.contains(local)) return hit;
    const double across = local.x - placed.viewport.x;
    const double down = local.y - placed.viewport.y;
    std::size_t line = 0U;
    std::size_t position = 0U;
    if (view_ == ListViewView::details) {
        line = placed.top_line + static_cast<std::size_t>(down / placed.row_height);
    } else if (view_ == ListViewView::list) {
        line = placed.top_line + static_cast<std::size_t>(across / placed.cell.width);
        position = static_cast<std::size_t>(down / placed.cell.height);
    } else {
        line = placed.top_line + static_cast<std::size_t>(down / placed.cell.height);
        position = static_cast<std::size_t>(across / placed.cell.width);
    }
    if (position >= placed.per_line) return hit;
    const std::size_t index = line * placed.per_line + position;
    if (index >= placed.shown_count) return hit;
    if (!item_bounds(placed, index).contains(local)) return hit;
    hit.index = index;
    hit.zone = HitZone::item;
    if (check_boxes_) {
        const ItemParts parts = item_parts(placed, index, placed.viewport);
        if (parts.check_well.present && parts.check_well.bounds.contains(local)) hit.zone = HitZone::check;
    }
    return hit;
}

std::optional<std::size_t> ListView::item_index_at(Point window_point) const {
    const Rect absolute = absolute_bounds();
    const Point local{window_point.x - absolute.x, window_point.y - absolute.y};
    const Hit hit = hit_test(layout(), local);
    if (hit.zone != HitZone::item && hit.zone != HitZone::check) return std::nullopt;
    return hit.index;
}

void ListView::finish_column_resize() {
    if (!resizing_column_) return;
    const std::size_t column = *resizing_column_;
    resizing_column_.reset();
    if (has_pointer_capture()) set_pointer_capture(false);
    invalidate(Dirty::paint | Dirty::semantics);
    if (column >= columns_.size() || columns_[column].width == resize_start_width_) return;
    const ListViewColumnWidthChange change{std::string(stable_id().value()), column, columns_[column].width};
    column_width_changed_.emit(change);
}

void ListView::request_column_click(std::size_t column) {
    if (column >= columns_.size()) return;
    const ListViewColumnClick click{std::string(stable_id().value()), column, columns_[column].stable_id};
    column_click_.emit(click);
}

void ListView::on_pointer(PointerEvent& event) {
    if (!eligible_for_input()) return;
    const Rect absolute = absolute_bounds();
    const Point local{event.position.x - absolute.x, event.position.y - absolute.y};
    const Control::Ptr lifetime = shared_from_this();
    if (resizing_column_) {
        // The window may end capture before it delivers the release, and a
        // host or another control may take it away: either ends the drag.
        if (event.action == PointerAction::up || !has_pointer_capture()) {
            finish_column_resize();
            event.handled = event.action == PointerAction::up;
            if (event.handled) return;
        } else {
            if (event.action == PointerAction::move && *resizing_column_ < columns_.size()) {
                const double delta = (local.x - resize_start_x_) / effective_text_scale();
                const double width =
                    std::clamp(resize_start_width_ + delta, minimum_column_width, maximum_column_width);
                if (columns_[*resizing_column_].width != width) {
                    columns_[*resizing_column_].width = width;
                    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
                }
            }
            event.handled = true;
            return;
        }
    }
    const Layout placed = layout();
    if (event.action == PointerAction::leave) {
        if (hot_index_ || hot_column_) {
            hot_index_.reset();
            hot_column_.reset();
            invalidate(Dirty::paint);
        }
        return;
    }
    const Hit hit = hit_test(placed, local);
    if (event.action == PointerAction::move || event.action == PointerAction::enter) {
        const bool on_item = hit.zone == HitZone::item || hit.zone == HitZone::check;
        const bool on_header = hit.zone == HitZone::header;
        const std::optional<std::size_t> next_hot = on_item ? std::optional<std::size_t>(hit.index) : std::nullopt;
        const std::optional<std::size_t> next_column =
            on_header ? std::optional<std::size_t>(hit.index) : std::nullopt;
        set_cursor(hit.zone == HitZone::divider ? CursorKind::resize_horizontal : CursorKind::arrow);
        if (next_hot != hot_index_ || next_column != hot_column_) {
            hot_index_ = next_hot;
            hot_column_ = next_column;
            invalidate(Dirty::paint);
        }
        return;
    }
    if (event.action == PointerAction::wheel) {
        if (event.wheel_delta.y == 0.0 && event.wheel_delta.x == 0.0) return;
        const bool sideways = event.wheel_delta.x != 0.0 || has_modifier(event.modifiers, Modifier::shift);
        if (sideways && view_ == ListViewView::details) {
            const double delta = event.wheel_delta.x != 0.0 ? event.wheel_delta.x : event.wheel_delta.y;
            set_horizontal_offset(std::max(0.0, horizontal_offset_ - delta * horizontal_key_step * 2.0));
        } else {
            const double delta = event.wheel_delta.y != 0.0 ? event.wheel_delta.y : event.wheel_delta.x;
            scroll_lines(delta > 0.0 ? -wheel_lines : wheel_lines);
        }
        event.handled = true;
        return;
    }
    if (event.action == PointerAction::down) {
        if (event.button != PointerButton::primary && event.button != PointerButton::secondary) return;
        if (window() != nullptr) static_cast<void>((*window()).request_focus(lifetime));
        if (!is_alive()) return;
        event.handled = true;
        if (event.button == PointerButton::secondary) {
            // A secondary press selects the item under it unless it is
            // already selected (context menus are the application's).
            if (hit.zone == HitZone::item && selected_[hit.index] == 0U) choose(hit.index, Modifier::none, false);
            return;
        }
        if (hit.zone == HitZone::divider) {
            resizing_column_ = hit.index;
            resize_start_x_ = local.x;
            resize_start_width_ = columns_[hit.index].width;
            if (window() != nullptr) (*window()).capture_pointer(lifetime, event.pointer_id);
            invalidate(Dirty::paint | Dirty::semantics);
        } else if (hit.zone == HitZone::header) {
            pressed_column_ = hit.index;
            if (window() != nullptr) (*window()).capture_pointer(lifetime, event.pointer_id);
            invalidate(Dirty::paint | Dirty::semantics);
        } else if (hit.zone == HitZone::check) {
            move_focus(hit.index);
            toggle_check(hit.index);
        } else if (hit.zone == HitZone::item) {
            if (event.click_count >= 2U) {
                activate(hit.index);
            } else {
                choose(hit.index, event.modifiers, false);
            }
        } else if (!placed.viewport.empty() && placed.viewport.contains(local) &&
                   event.modifiers == Modifier::none) {
            if (select_none()) publish_selection();
        }
        return;
    }
    if (event.action == PointerAction::up && event.button == PointerButton::primary && pressed_column_) {
        const std::size_t pressed = *pressed_column_;
        pressed_column_.reset();
        if (has_pointer_capture()) set_pointer_capture(false);
        invalidate(Dirty::paint | Dirty::semantics);
        event.handled = true;
        // A click is a press and release on the same header.
        if (hit.zone == HitZone::header && hit.index == pressed) request_column_click(pressed);
    }
}

std::optional<std::size_t> ListView::navigation_target(std::uint32_t key, std::size_t from,
                                                       const Layout& placed) const {
    const std::size_t count = placed.shown_count;
    const std::size_t last = count - 1U;
    const std::size_t per = placed.per_line;
    const std::size_t page = placed.page_lines * per;
    // The step of each arrow: along a line (`per` items apart in the list
    // view's columns and the icon views' rows) or to the next item.
    std::size_t vertical = 1U;
    std::size_t horizontal = 0U;
    if (view_ == ListViewView::list) {
        horizontal = per;
    } else if (view_ != ListViewView::details) {
        vertical = per;
        horizontal = 1U;
    }
    if (key == PhysicalKey::home) return std::size_t{0U};
    if (key == PhysicalKey::end) return last;
    if (key == PhysicalKey::up) return from >= vertical ? from - vertical : from;
    if (key == PhysicalKey::down) return std::min(from + vertical, last);
    if (key == PhysicalKey::page_up) return from >= page ? from - page : std::size_t{0U};
    if (key == PhysicalKey::page_down) return std::min(from + page, last);
    if (horizontal == 0U) return std::nullopt;
    if (key == PhysicalKey::left) return from >= horizontal ? from - horizontal : from;
    if (key == PhysicalKey::right) return std::min(from + horizontal, last);
    return std::nullopt;
}

void ListView::on_key(KeyEvent& event) {
    if (!focused_ || !effectively_enabled() || event.action != KeyAction::down) return;
    if (event.physical_key == PhysicalKey::escape && resizing_column_) {
        columns_[*resizing_column_].width = resize_start_width_;
        finish_column_resize();
        invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
        event.handled = true;
        return;
    }
    const Layout placed = layout();
    if (placed.shown_count == 0U) return;
    const bool control = has_modifier(event.modifiers, Modifier::control) ||
                         has_modifier(event.modifiers, Modifier::meta);
    const bool shift = has_modifier(event.modifiers, Modifier::shift);
    const std::size_t from = std::min(focused_index_.value_or(0U), placed.shown_count - 1U);
    if (event.physical_key == PhysicalKey::a && control && !shift) {
        select_all();
        event.handled = true;
        return;
    }
    if (event.physical_key == PhysicalKey::enter) {
        if (focused_index_) activate(*focused_index_);
        event.handled = true;
        return;
    }
    if (event.physical_key == PhysicalKey::space) {
        if (check_boxes_ && !control) {
            toggle_check(from);
        } else if (control && multi_select_) {
            const bool changed = toggle_selected(from);
            anchor_index_ = from;
            move_focus(from);
            if (changed) publish_selection();
        } else {
            choose(from, Modifier::none, false);
        }
        event.handled = true;
        return;
    }
    const std::optional<std::size_t> target = navigation_target(event.physical_key, from, placed);
    if (!target) {
        // The details view scrolls sideways with Left and Right.
        const bool sideways = event.physical_key == PhysicalKey::left || event.physical_key == PhysicalKey::right;
        if (!sideways || view_ != ListViewView::details) return;
        const double step = event.physical_key == PhysicalKey::left ? -horizontal_key_step : horizontal_key_step;
        set_horizontal_offset(std::max(0.0, placed.horizontal_offset + step));
        event.handled = true;
        return;
    }
    // Control with an arrow moves the focus without selecting; Shift extends
    // the selection from the anchor (Control and Shift adds the range).
    choose(*target, event.modifiers, control && !shift);
    event.handled = true;
}

void ListView::type_select(std::string_view text) {
    const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    if (now - last_type_time_ > collection_detail::type_timeout) type_prefix_.clear();
    last_type_time_ = now;
    type_prefix_ += collection_detail::fold_ascii(text);
    const std::size_t count = items_.size();
    if (count == 0U) return;
    // A repeated first letter steps to the next match; a longer prefix
    // starts its search at the focused item.
    const bool single = type_prefix_.size() == 1U;
    const std::size_t focus = focused_index_.value_or(count - 1U);
    const std::size_t start = single ? (focus + 1U) % count : focus % count;
    for (std::size_t offset = 0; offset < count; ++offset) {
        const std::size_t index = (start + offset) % count;
        if (collection_detail::fold_ascii(items_[index].text).starts_with(type_prefix_)) {
            choose(index, Modifier::none, false);
            return;
        }
    }
}

void ListView::on_text_input(TextInputEvent& event) {
    // Type-ahead searches the view's own items. A virtual view's items are
    // the application's: searching them is not supported yet.
    if (!focused_ || !effectively_enabled() || source_ || event.composing || event.text_utf8.empty() ||
        !validate_utf8(event.text_utf8).valid()) {
        return;
    }
    // Space belongs to on_key (check or select); it never starts a search.
    if (event.text_utf8 == " " && type_prefix_.empty()) return;
    type_select(event.text_utf8);
    event.handled = true;
}

void ListView::on_focus_changed(bool focused) {
    focused_ = focused;
    if (!focused) {
        if (resizing_column_) finish_column_resize();
        if (pressed_column_) {
            pressed_column_.reset();
            if (has_pointer_capture()) set_pointer_capture(false);
        }
    }
    if (focused_ && !focused_index_ && item_count() != 0U) {
        std::size_t first = 0U;
        while (first < selected_.size() && selected_[first] == 0U) ++first;
        focused_index_ = first < selected_.size() ? first : top_index();
    }
    invalidate(Dirty::paint | Dirty::semantics);
}

// -- semantics -----------------------------------------------------------------

SemanticDescriptor ListView::semantic_descriptor() const {
    SemanticDescriptor descriptor{};
    // There is no table or grid role: the details view is a list whose
    // items describe their sub-items, as ObjectView's do.
    descriptor.role = SemanticRole::list;
    descriptor.name = accessible_name();
    descriptor.description = accessible_description();
    if (selected_count_ == 1U && focused_index_ && selected_[*focused_index_] != 0U) {
        ListViewItem scratch;
        descriptor.value = item_at(*focused_index_, scratch).text;
    } else if (selected_count_ != 0U) {
        descriptor.value = std::to_string(selected_count_) + " selected";
    }
    descriptor.actions = {SemanticAction::focus};
    descriptor.exposed = true;
    return descriptor;
}

std::vector<SemanticNode> ListView::semantic_virtual_children() const {
    std::vector<SemanticNode> nodes{};
    const Layout placed = layout();
    const Rect absolute = absolute_bounds();
    const std::string list_id(stable_id().value());
    const bool enabled = effectively_enabled();
    // Column headers first, in reading order. There is no column-header
    // role: a header is a button that raises column_click.
    for (std::size_t column = 0; column < columns_.size(); ++column) {
        if (!column_visible(placed, column)) continue;
        const Rect bounds = Rect::intersection(column_cell(placed, column), placed.header);
        SemanticNode node{};
        node.stable_id = column_identity(list_id, columns_[column].stable_id);
        node.runtime_id = collection_detail::virtual_runtime_id(node.stable_id);
        node.role = SemanticRole::button;
        node.name = columns_[column].text;
        if (sort_column_ == column && sort_order_ == ListViewSortOrder::ascending) node.value = "ascending";
        if (sort_column_ == column && sort_order_ == ListViewSortOrder::descending) node.value = "descending";
        node.bounds = {absolute.x + bounds.x, absolute.y + bounds.y, bounds.width, bounds.height};
        node.states = SemanticState::visible;
        if (enabled) node.states |= SemanticState::enabled;
        node.actions = {SemanticAction::press};
        nodes.push_back(std::move(node));
    }
    const std::size_t first = first_realized(placed);
    const std::size_t end = end_realized(placed);
    ListViewItem scratch;
    for (std::size_t index = first; index < end; ++index) {
        const ListViewItem& item = item_at(index, scratch);
        const Rect bounds = Rect::intersection(item_bounds(placed, index), placed.viewport);
        SemanticNode node{};
        node.stable_id = item.stable_id;
        node.runtime_id = collection_detail::virtual_runtime_id(node.stable_id);
        node.role = check_boxes_ ? SemanticRole::check_list_item : SemanticRole::list_item;
        node.name = item.text;
        if (view_ == ListViewView::details) {
            for (std::size_t column = 1; column < columns_.size(); ++column) {
                if (!node.description.empty()) node.description.append("; ");
                node.description.append(columns_[column].text);
                node.description.append(": ");
                node.description.append(column_text(item, column));
            }
        }
        node.bounds = {absolute.x + bounds.x, absolute.y + bounds.y, bounds.width, bounds.height};
        node.states = SemanticState::visible | SemanticState::focusable;
        if (enabled) node.states |= SemanticState::enabled;
        if (selected_[index] != 0U) node.states |= SemanticState::selected;
        if (focused_ && focused_index_ == index) node.states |= SemanticState::focused;
        if (check_boxes_) {
            node.value = item.checked ? "checked" : "unchecked";
            if (item.checked) node.states |= SemanticState::checked;
        }
        node.actions = {SemanticAction::focus, SemanticAction::select, SemanticAction::press};
        nodes.push_back(std::move(node));
    }
    return nodes;
}

std::optional<std::size_t> ListView::realized_index_of(std::string_view item_id) const {
    const std::optional<std::size_t> known = index_of(item_id);
    if (known) return known;
    const Layout placed = layout();
    const std::size_t end = end_realized(placed);
    ListViewItem scratch;
    for (std::size_t index = first_realized(placed); index < end; ++index) {
        if (item_at(index, scratch).stable_id == item_id) return index;
    }
    return std::nullopt;
}

bool ListView::on_semantic_child_action(std::string_view id, SemanticAction action, std::string_view) {
    if (!effectively_enabled()) return false;
    const std::string list_id(stable_id().value());
    for (std::size_t column = 0; column < columns_.size(); ++column) {
        if (id != column_identity(list_id, columns_[column].stable_id)) continue;
        if (action != SemanticAction::press) return false;
        request_column_click(column);
        return true;
    }
    const std::optional<std::size_t> index = realized_index_of(id);
    if (!index || *index >= item_count()) return false;
    if (action != SemanticAction::focus && action != SemanticAction::select && action != SemanticAction::press) {
        return false;
    }
    const Control::Ptr lifetime = shared_from_this();
    if (window() != nullptr) static_cast<void>((*window()).request_focus(lifetime));
    if (!is_alive()) return true;
    if (action == SemanticAction::focus) {
        move_focus(*index);
    } else if (action == SemanticAction::select) {
        choose(*index, Modifier::none, false);
    } else if (check_boxes_) {
        // Press toggles a check-list item, as CheckedListBox's do.
        move_focus(*index);
        toggle_check(*index);
    } else {
        activate(*index);
    }
    return true;
}

// -- anatomy -----------------------------------------------------------------

namespace {

constexpr std::array<PartField<ListView::Parts>, 7> list_view_part_fields{{
    {.declaration = {.name = "region", .role = PartRole::region, .composed_from = {}},
     .member = &ListView::Parts::region},
    {.declaration = {.name = "well", .role = PartRole::well, .composed_from = {}},
     .member = &ListView::Parts::well},
    {.declaration = {.name = "header", .role = PartRole::face, .composed_from = {}},
     .member = &ListView::Parts::header},
    // The child scroll bars, each a whole scroll bar system. A ranged path
    // is the nearest role to a composed scroll bar.
    {.declaration = {.name = "vertical_scroll_bar", .role = PartRole::track, .composed_from = "v_scroll_bar"},
     .member = &ListView::Parts::vertical_scroll_bar},
    {.declaration = {.name = "horizontal_scroll_bar", .role = PartRole::track,
                     .composed_from = "h_scroll_bar"},
     .member = &ListView::Parts::horizontal_scroll_bar},
    {.declaration = {.name = "scroll_corner", .role = PartRole::face, .composed_from = {}},
     .member = &ListView::Parts::scroll_corner},
    {.declaration = {.name = "focus", .role = PartRole::focus, .composed_from = {}},
     .member = &ListView::Parts::focus},
}};

constexpr std::array<PartField<ListView::ItemParts>, 7> list_view_item_part_fields{{
    {.declaration = {.name = "cell", .role = PartRole::region, .composed_from = {}},
     .member = &ListView::ItemParts::cell},
    {.declaration = {.name = "item_face", .role = PartRole::face, .composed_from = {}},
     .member = &ListView::ItemParts::item_face},
    {.declaration = {.name = "check_well", .role = PartRole::well, .composed_from = {}},
     .member = &ListView::ItemParts::check_well},
    {.declaration = {.name = "check", .role = PartRole::indicator, .composed_from = {}},
     .member = &ListView::ItemParts::check},
    {.declaration = {.name = "icon", .role = PartRole::icon, .composed_from = {}},
     .member = &ListView::ItemParts::icon},
    {.declaration = {.name = "text", .role = PartRole::content, .composed_from = {}},
     .member = &ListView::ItemParts::text},
    {.declaration = {.name = "cell_focus", .role = PartRole::focus, .composed_from = {}},
     .member = &ListView::ItemParts::cell_focus},
}};

constexpr std::array<StateField<ListView::ItemStates>, 6> list_view_item_state_fields{{
    {.state = AnatomyState::selected, .member = &ListView::ItemStates::selected},
    {.state = AnatomyState::hot, .member = &ListView::ItemStates::hot},
    {.state = AnatomyState::current, .member = &ListView::ItemStates::current},
    {.state = AnatomyState::checked, .member = &ListView::ItemStates::checked},
    {.state = AnatomyState::disabled, .member = &ListView::ItemStates::disabled},
    {.state = AnatomyState::focus_visible, .member = &ListView::ItemStates::focus_visible},
}};

constexpr std::array<PartField<ListView::SubItemParts>, 2> list_view_sub_item_part_fields{{
    {.declaration = {.name = "sub_item_cell", .role = PartRole::region, .composed_from = {}},
     .member = &ListView::SubItemParts::sub_item_cell},
    {.declaration = {.name = "sub_item_text", .role = PartRole::content, .composed_from = {}},
     .member = &ListView::SubItemParts::sub_item_text},
}};

constexpr std::array<StateField<ListView::SubItemStates>, 2> list_view_sub_item_state_fields{{
    {.state = AnatomyState::selected, .member = &ListView::SubItemStates::selected},
    {.state = AnatomyState::disabled, .member = &ListView::SubItemStates::disabled},
}};

constexpr std::array<PartField<ListView::ColumnParts>, 5> list_view_column_part_fields{{
    {.declaration = {.name = "column_header", .role = PartRole::face, .composed_from = {}},
     .member = &ListView::ColumnParts::column_header},
    {.declaration = {.name = "column_label", .role = PartRole::label, .composed_from = {}},
     .member = &ListView::ColumnParts::column_label},
    {.declaration = {.name = "sort_glyph", .role = PartRole::glyph, .composed_from = {}},
     .member = &ListView::ColumnParts::sort_glyph},
    {.declaration = {.name = "column_divider", .role = PartRole::divider, .composed_from = {}},
     .member = &ListView::ColumnParts::column_divider},
    {.declaration = {.name = "column_grip", .role = PartRole::grip, .composed_from = {}},
     .member = &ListView::ColumnParts::column_grip},
}};

constexpr std::array<StateField<ListView::ColumnStates>, 3> list_view_column_state_fields{{
    {.state = AnatomyState::hot, .member = &ListView::ColumnStates::hot},
    {.state = AnatomyState::pressed, .member = &ListView::ColumnStates::pressed},
    {.state = AnatomyState::captured, .member = &ListView::ColumnStates::captured},
}};

constexpr std::array<CustomStateDeclaration, 2> list_view_column_custom_states{{
    {.name = "sorted_ascending"},
    {.name = "sorted_descending"},
}};

constexpr std::array<PartDeclaration, 7> list_view_parts = part_declarations(list_view_part_fields);
constexpr std::array<PartDeclaration, 7> list_view_item_parts = part_declarations(list_view_item_part_fields);
constexpr std::array<AnatomyState, 6> list_view_item_states = state_declarations(list_view_item_state_fields);
constexpr std::array<PartDeclaration, 2> list_view_sub_item_parts =
    part_declarations(list_view_sub_item_part_fields);
constexpr std::array<AnatomyState, 2> list_view_sub_item_states =
    state_declarations(list_view_sub_item_state_fields);
constexpr std::array<PartDeclaration, 5> list_view_column_parts = part_declarations(list_view_column_part_fields);
constexpr std::array<AnatomyState, 3> list_view_column_states = state_declarations(list_view_column_state_fields);
constexpr std::array<AnatomyState, 5> list_view_states{
    AnatomyState::normal, AnatomyState::enabled, AnatomyState::disabled,
    AnatomyState::focused, AnatomyState::focus_visible,
};

constexpr ItemSystemIndex item_system{0U};
constexpr ItemSystemIndex sub_item_system{1U};
constexpr ItemSystemIndex column_system{2U};

constexpr std::array<ItemSystemDeclaration, 3> list_view_item_systems{{
    {.name = "item", .parts = list_view_item_parts, .states = list_view_item_states, .custom_states = {}},
    {.name = "sub_item", .parts = list_view_sub_item_parts, .states = list_view_sub_item_states,
     .custom_states = {}},
    {.name = "column", .parts = list_view_column_parts, .states = list_view_column_states,
     .custom_states = list_view_column_custom_states},
}};

constexpr ControlAnatomy list_view_anatomy{
    .control_id = "list_view",
    .parts = list_view_parts,
    .states = list_view_states,
    .custom_states = {},
    .item_systems = list_view_item_systems,
};

} // namespace

const ControlAnatomy& ListView::anatomy() const noexcept {
    return list_view_anatomy;
}

void ListView::publish_part_geometry(std::span<PartGeometry> geometry) const {
    const Layout placed = layout();
    Parts record;
    record.region = PartGeometry::from_bounds(placed.well);
    record.well = PartGeometry::from_bounds(placed.well);
    record.header = PartGeometry::from_bounds(placed.header);
    record.vertical_scroll_bar = PartGeometry::from_bounds(placed.vertical_bar);
    record.horizontal_scroll_bar = PartGeometry::from_bounds(placed.horizontal_bar);
    record.scroll_corner = PartGeometry::from_bounds(placed.corner);
    record.focus = PartGeometry::from_bounds(placed.well);
    write_parts(anatomy(), list_view_part_fields, record, geometry);
}

ListView::Parts ListView::parts() const {
    const PartGeometryBuffer published = published_part_geometry();
    Parts record;
    read_parts(anatomy(), list_view_part_fields, published.parts(), record);
    return record;
}

std::size_t ListView::realized_row_count(const Layout& placed) const noexcept {
    const std::size_t result = end_realized(placed) - first_realized(placed);
    return result;
}

// Sub-items are realized row by row, across the visible columns after the
// first.
std::size_t ListView::realized_sub_item_count(const Layout& placed) const noexcept {
    if (view_ != ListViewView::details || columns_.size() < 2U) return 0U;
    std::size_t columns = visible_column_count(placed);
    if (column_visible(placed, 0U)) --columns;
    const std::size_t result = realized_row_count(placed) * columns;
    return result;
}

std::size_t ListView::realized_sub_item_column(const Layout& placed, std::size_t position) const noexcept {
    const std::size_t skip = column_visible(placed, 0U) ? 1U : 0U;
    const std::size_t result = visible_column(placed, position + skip);
    return result;
}

std::size_t ListView::realized_item_count() const {
    const Layout placed = layout();
    const std::size_t columns = view_ == ListViewView::details ? visible_column_count(placed) : 0U;
    const std::size_t result = realized_row_count(placed) + realized_sub_item_count(placed) + columns;
    return result;
}

// The realized item `item`, in paint order: rows, then sub-items, then
// column headers.
std::string ListView::realized_item_stable_id(std::size_t item) const {
    const Layout placed = layout();
    const std::size_t rows = realized_row_count(placed);
    const std::size_t sub_items = realized_sub_item_count(placed);
    ListViewItem scratch;
    if (item < rows) return item_at(first_realized(placed) + item, scratch).stable_id;
    if (item < rows + sub_items) {
        const std::size_t per_row = sub_items / rows;
        const std::size_t position = item - rows;
        const std::size_t index = first_realized(placed) + position / per_row;
        const std::size_t column = realized_sub_item_column(placed, position % per_row);
        return sub_item_identity(item_at(index, scratch).stable_id, columns_[column].stable_id);
    }
    const std::size_t column = visible_column(placed, item - rows - sub_items);
    return column_identity(stable_id().value(), columns_[column].stable_id);
}

ItemSystemIndex ListView::publish_realized_item_system(std::size_t item) const {
    const Layout placed = layout();
    const std::size_t rows = realized_row_count(placed);
    if (item < rows) return item_system;
    if (item < rows + realized_sub_item_count(placed)) return sub_item_system;
    return column_system;
}

void ListView::publish_realized_item_geometry(std::size_t item, std::span<PartGeometry> geometry) const {
    const Layout placed = layout();
    const std::size_t rows = realized_row_count(placed);
    const std::size_t sub_items = realized_sub_item_count(placed);
    if (item < rows) {
        const ItemParts record = item_parts(placed, first_realized(placed) + item, placed.viewport);
        write_item_parts(anatomy().item_systems[item_system.value], list_view_item_part_fields, record, geometry);
        return;
    }
    if (item < rows + sub_items) {
        const std::size_t per_row = sub_items / rows;
        const std::size_t position = item - rows;
        const std::size_t index = first_realized(placed) + position / per_row;
        const std::size_t column = realized_sub_item_column(placed, position % per_row);
        const SubItemParts record = sub_item_parts(placed, index, column, placed.viewport);
        write_item_parts(anatomy().item_systems[sub_item_system.value], list_view_sub_item_part_fields, record,
                         geometry);
        return;
    }
    const std::size_t column = visible_column(placed, item - rows - sub_items);
    const ColumnParts record = column_parts(placed, column, placed.header);
    write_item_parts(anatomy().item_systems[column_system.value], list_view_column_part_fields, record, geometry);
}

AnatomyStateSet ListView::realized_item_anatomy_states(std::size_t item) const {
    const Layout placed = layout();
    const std::size_t rows = realized_row_count(placed);
    const std::size_t sub_items = realized_sub_item_count(placed);
    ListViewItem scratch;
    if (item < rows) {
        const std::size_t index = first_realized(placed) + item;
        return item_states(index, item_at(index, scratch).checked);
    }
    if (item < rows + sub_items) {
        const std::size_t index = first_realized(placed) + (item - rows) / (sub_items / rows);
        AnatomyStateSet states;
        if (selected_[index] != 0U) states.insert(AnatomyState::selected);
        if (!effectively_enabled()) states.insert(AnatomyState::disabled);
        return states;
    }
    return column_states(visible_column(placed, item - rows - sub_items));
}

std::vector<ListView::Item> ListView::realized_items() const {
    const Layout placed = layout();
    const std::size_t first = first_realized(placed);
    const std::size_t count = realized_row_count(placed);
    std::vector<Item> records;
    records.reserve(count);
    for (std::size_t item = 0; item < count; ++item) {
        const PartGeometryBuffer published = published_realized_item_geometry(item);
        Item record;
        record.index = first + item;
        record.stable_id = realized_item_stable_id(item);
        read_item_parts(anatomy().item_systems[item_system.value], list_view_item_part_fields, published.parts(),
                        record.parts);
        read_states(list_view_item_state_fields, realized_item_anatomy_states(item), record.states);
        records.push_back(record);
    }
    return records;
}

std::vector<ListView::SubItem> ListView::realized_sub_items() const {
    const Layout placed = layout();
    const std::size_t rows = realized_row_count(placed);
    const std::size_t count = realized_sub_item_count(placed);
    std::vector<SubItem> records;
    records.reserve(count);
    for (std::size_t position = 0; position < count; ++position) {
        const std::size_t item = rows + position;
        const PartGeometryBuffer published = published_realized_item_geometry(item);
        SubItem record;
        record.index = first_realized(placed) + position / (count / rows);
        record.column = realized_sub_item_column(placed, position % (count / rows));
        record.stable_id = realized_item_stable_id(item);
        read_item_parts(anatomy().item_systems[sub_item_system.value], list_view_sub_item_part_fields,
                        published.parts(), record.parts);
        read_states(list_view_sub_item_state_fields, realized_item_anatomy_states(item), record.states);
        records.push_back(record);
    }
    return records;
}

std::vector<ListView::Column> ListView::realized_columns() const {
    const Layout placed = layout();
    const std::size_t count = view_ == ListViewView::details ? visible_column_count(placed) : 0U;
    std::vector<Column> records;
    records.reserve(count);
    for (std::size_t position = 0; position < count; ++position) {
        const std::size_t column = visible_column(placed, position);
        Column record;
        record.index = column;
        record.stable_id = column_identity(stable_id().value(), columns_[column].stable_id);
        record.parts = column_parts(placed, column, placed.header);
        const AnatomyStateSet states = column_states(column);
        read_states(list_view_column_state_fields, states, record.states);
        record.states.sorted_ascending = states.contains(sorted_ascending_state);
        record.states.sorted_descending = states.contains(sorted_descending_state);
        records.push_back(record);
    }
    return records;
}

namespace {

// The events ListView adds to those of Control, in declaration order.
constexpr std::array<InteractionDeclaration, 6> list_view_interactions{{
    {.name = "selection_changed", .payload = "ListViewSelectionChange", .mutable_payload = false},
    {.name = "item_activated", .payload = "ListViewItemActivation", .mutable_payload = false},
    {.name = "item_checking", .payload = "ListViewItemCheck", .mutable_payload = true},
    {.name = "item_checked", .payload = "ListViewItemCheck", .mutable_payload = false},
    {.name = "column_click", .payload = "ListViewColumnClick", .mutable_payload = false},
    {.name = "column_width_changed", .payload = "ListViewColumnWidthChange", .mutable_payload = false},
}};

} // namespace

void ListView::append_interactions(std::vector<InteractionDeclaration>& interactions) const {
    Control::append_interactions(interactions);
    interactions.insert(interactions.end(), list_view_interactions.begin(), list_view_interactions.end());
}

} // namespace gui_forms
