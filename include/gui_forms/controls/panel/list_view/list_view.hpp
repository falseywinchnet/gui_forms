#pragma once

#include "gui_forms/control.hpp"
#include "gui_forms/event.hpp"
#include "gui_forms/image_list.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace gui_forms {

class HScrollBar;
class VScrollBar;

// How a ListView arranges its items (Windows Forms' View; Tile is not
// supported yet).
//
// - details: one row per item, with the columns' headers above and each
//   column's text (the item's text, then its sub-items) in the row;
// - list: small-icon rows filled top to bottom, then in further columns to
//   the right; the view scrolls sideways;
// - small_icon: small-icon cells filled left to right, then in further rows;
// - large_icon: large-icon cells with the text under the icon, filled left to
//   right, then in further rows.
enum class ListViewView : std::uint8_t { details, list, small_icon, large_icon };

// How a column's text sits in its cells. The first column's text always
// starts at the left, after the check box and icon, as in Windows Forms.
enum class ListViewAlignment : std::uint8_t { left, center, right };

// The order a sort glyph shows, or none.
enum class ListViewSortOrder : std::uint8_t { none, ascending, descending };

// One details-view column. `stable_id` is unique within the view and names
// the column in the published anatomy and semantics. `width` is in logical
// units at text scale 1.
struct ListViewColumn final {
    std::string stable_id{};
    std::string text{};
    double width{120.0};
    ListViewAlignment alignment{ListViewAlignment::left};
};

// One item. `text` is shown in the first column (and as the label in the
// other views); `sub_items[n]` in column n + 1 of the details view. Sub-items
// past the last column are kept but not shown. `image_key` names an image
// in the small image list (details, list and small-icon views) or the large
// one (large-icon view); an empty or unknown key shows no image. `checked`
// is shown while the view has check boxes.
struct ListViewItem final {
    std::string stable_id{};
    std::string text{};
    std::vector<std::string> sub_items{};
    std::string image_key{};
    bool checked{};
};

// The items of a virtual ListView (Windows Forms' VirtualMode with
// RetrieveVirtualItem). The application owns the data; the view asks for
// the count when the source is set or refresh_virtual_items() is called, and
// for single items only when it paints, lays out or describes the visible
// ones. Both functions are called on the view's UI thread from inside the
// view's own (const) work and must not change the view.
class ListViewItemSource {
public:
    virtual ~ListViewItemSource();
    [[nodiscard]] virtual std::size_t item_count() const = 0;
    // Fills `item` for `index` (below the last count the view read). `item`
    // is reused between calls: overwrite every field. An empty stable ID is
    // replaced by "#" and the index; text that is not valid UTF-8 is shown
    // empty.
    virtual void retrieve_item(std::size_t index, ListViewItem& item) const = 0;
};

// A change of the selected items, after it happened. The selected indices
// are read from the view (selected_indices()); a virtual view with a large
// selection does not copy them into every event.
struct ListViewSelectionChange final {
    std::string list_id{};
    std::size_t selected_count{};
    std::optional<std::size_t> focused_index{};
};

// An item activated by a double click, Enter, or the semantic press action
// on a view without check boxes.
struct ListViewItemActivation final {
    std::string list_id{};
    std::size_t index{};
    std::string item_id{};
};

// A check box toggled by the user. item_checking is raised first and may
// cancel the change or alter `new_value`; item_checked follows once the
// change is made. In a virtual view the application owns the checked state:
// it stores `new_value` when handling item_checking, and the view shows
// what the source then retrieves.
struct ListViewItemCheck final {
    std::string list_id{};
    std::size_t index{};
    std::string item_id{};
    bool current_value{};
    bool new_value{};
    bool cancel{};
};

// A click on a column header. The view does not sort: the application
// reorders its items (sort_items() is a helper for that) and shows the order
// with set_sort_indicator().
struct ListViewColumnClick final {
    std::string list_id{};
    std::size_t column{};
    std::string column_id{};
};

// A column's new width in logical units, after a divider drag ends or
// set_column_width() changes it.
struct ListViewColumnWidthChange final {
    std::string list_id{};
    std::size_t column{};
    double width{};
};

// A retained item list with details, list, small-icon and large-icon views
// (Windows Forms' ListView). Items are records, not child controls; only the
// visible ones are laid out, painted and described, so a virtual view of
// millions of items realizes a screenful. Every part is painted through the
// by-role paint path. Two child scroll bars scroll the view when its items
// or columns do not fit.
//
// Selection, focus and check boxes are kept by index. Replacing the items
// of an ordinary view carries the selection, the focused item and the anchor
// over by stable ID; a virtual view keeps them by index, clamped to the new
// count. Columns are resized by dragging a header's divider. Groups, the
// tile view, label editing, the insertion mark, owner drawing, tooltips and
// hot-tracking activation are not supported yet.
class ListView final : public Control {
public:
    static constexpr bool initialize_tree_after_construction = true;
    // Development bounds, not measured capacity.
    static constexpr std::size_t maximum_items = 10'000'000U;
    static constexpr std::size_t maximum_columns = 64U;
    static constexpr double minimum_column_width = 20.0;
    static constexpr double maximum_column_width = 4096.0;

    explicit ListView(StableId stable_id);
    ~ListView() override;
    void initialize_control_tree();

    [[nodiscard]] ListViewView view() const noexcept { return view_; }
    void set_view(ListViewView view);

    [[nodiscard]] std::span<const ListViewColumn> columns() const noexcept { return columns_; }
    // Replaces the columns. Throws std::invalid_argument for an empty or
    // duplicate ID, invalid UTF-8, a width outside the minimum and maximum
    // column widths, or more than maximum_columns columns. Clears the sort
    // indicator when its column is gone.
    void set_columns(std::vector<ListViewColumn> columns);
    // Throws std::out_of_range for a column past the end and
    // std::invalid_argument for a width outside the bounds.
    void set_column_width(std::size_t column, double width);

    // The items of an ordinary view; empty in virtual mode.
    [[nodiscard]] std::span<const ListViewItem> items() const noexcept { return items_; }
    // Replaces the items. Throws std::invalid_argument for an empty or
    // duplicate ID or invalid UTF-8, and std::logic_error in virtual mode.
    void set_items(std::vector<ListViewItem> items);
    // The number of items, from the source in virtual mode.
    [[nodiscard]] std::size_t item_count() const noexcept;
    // The index of the item with this ID in an ordinary view; none in
    // virtual mode or when no item has it.
    [[nodiscard]] std::optional<std::size_t> index_of(std::string_view item_id) const;

    // Virtual mode. Setting a source drops the ordinary items and reads the
    // source's count; null returns to an ordinary, empty view. Throws
    // std::invalid_argument when the count exceeds maximum_items.
    [[nodiscard]] bool virtual_mode() const noexcept { return source_ != nullptr; }
    [[nodiscard]] const std::shared_ptr<ListViewItemSource>& virtual_source() const noexcept {
        return source_;
    }
    void set_virtual_source(std::shared_ptr<ListViewItemSource> source);
    // Re-reads the source's count after the application changed its data,
    // keeps the selection, focus and anchor below the new count, and
    // repaints. Throws std::logic_error outside virtual mode.
    void refresh_virtual_items();

    [[nodiscard]] bool check_boxes() const noexcept { return check_boxes_; }
    void set_check_boxes(bool shown);
    // Throws std::out_of_range for an index past the end.
    [[nodiscard]] bool item_checked(std::size_t index) const;
    // Sets an ordinary item's checked state and raises item_checked when it
    // changes. Throws std::out_of_range for an index past the end and
    // std::logic_error in virtual mode, where the source owns it.
    void set_item_checked(std::size_t index, bool checked);

    [[nodiscard]] bool multi_select() const noexcept { return multi_select_; }
    // Turning multiple selection off keeps only the focused item selected
    // (or the first selected one).
    void set_multi_select(bool multiple);
    // Whether the selection highlight spans the whole details row rather
    // than the first column's text.
    [[nodiscard]] bool full_row_select() const noexcept { return full_row_select_; }
    void set_full_row_select(bool full_row);

    [[nodiscard]] std::size_t selected_count() const noexcept { return selected_count_; }
    // The selected indices in ascending order.
    [[nodiscard]] std::vector<std::size_t> selected_indices() const;
    // Throws std::out_of_range for an index past the end.
    [[nodiscard]] bool is_selected(std::size_t index) const;
    // Selects or deselects one item, keeping the others (in single-selection
    // mode selecting an item deselects the rest). Throws std::out_of_range.
    void set_selected(std::size_t index, bool selected);
    void select_all();
    void clear_selection();
    // The item with the focus cue and the keyboard's position, if any.
    [[nodiscard]] std::optional<std::size_t> focused_index() const noexcept { return focused_index_; }
    // Throws std::out_of_range for an index past the end.
    void set_focused_index(std::optional<std::size_t> index);

    // The first item laid out in the visible area.
    [[nodiscard]] std::size_t top_index() const;
    // Scrolls so the item is fully visible. Throws std::out_of_range.
    void ensure_visible(std::size_t index);
    // The details view's sideways scroll, in logical units.
    [[nodiscard]] double horizontal_offset() const noexcept { return horizontal_offset_; }
    void set_horizontal_offset(double offset);

    // The sort glyph shown on one column header. The view never reorders
    // items for it. Throws std::out_of_range for a column past the end.
    [[nodiscard]] std::optional<std::size_t> sort_column() const noexcept { return sort_column_; }
    [[nodiscard]] ListViewSortOrder sort_order() const noexcept { return sort_order_; }
    void set_sort_indicator(std::optional<std::size_t> column, ListViewSortOrder order);
    // A helper the application may call from column_click: stable-sorts an
    // ordinary view's items by one column's text (ASCII case folded, then
    // byte order) and shows the sort glyph. The selection and focus follow
    // their items. Throws std::out_of_range for a column past the end and
    // std::logic_error in virtual mode.
    void sort_items(std::size_t column, ListViewSortOrder order);

    [[nodiscard]] std::shared_ptr<ImageList> small_image_list() const noexcept { return small_images_; }
    [[nodiscard]] std::shared_ptr<ImageList> large_image_list() const noexcept { return large_images_; }
    // Throws std::invalid_argument for a list bound to another window.
    void set_small_image_list(std::shared_ptr<ImageList> images);
    void set_large_image_list(std::shared_ptr<ImageList> images);

    [[nodiscard]] FontSpec font() const noexcept { return font_; }
    void set_font(FontSpec font);

    // The item under a point in window coordinates, if any.
    [[nodiscard]] std::optional<std::size_t> item_index_at(Point window_point) const;

    [[nodiscard]] Event<const ListViewSelectionChange&>& selection_changed() noexcept {
        return selection_changed_;
    }
    [[nodiscard]] Event<const ListViewItemActivation&>& item_activated() noexcept {
        return item_activated_;
    }
    [[nodiscard]] Event<ListViewItemCheck&>& item_checking() noexcept { return item_checking_; }
    [[nodiscard]] Event<const ListViewItemCheck&>& item_checked() noexcept { return item_checked_; }
    [[nodiscard]] Event<const ListViewColumnClick&>& column_click() noexcept { return column_click_; }
    [[nodiscard]] Event<const ListViewColumnWidthChange&>& column_width_changed() noexcept {
        return column_width_changed_;
    }

    void arrange(Rect final_bounds) override;
    void on_paint(Painter& painter, Rect local_damage) override;
    void on_pointer(PointerEvent& event) override;
    void on_key(KeyEvent& event) override;
    void on_text_input(TextInputEvent& event) override;
    void on_focus_changed(bool focused) override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;
    [[nodiscard]] std::vector<SemanticNode> semantic_virtual_children() const override;
    bool on_semantic_child_action(std::string_view stable_id, SemanticAction action,
                                  std::string_view value) override;
    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;
    [[nodiscard]] std::size_t realized_item_count() const override;
    [[nodiscard]] std::string realized_item_stable_id(std::size_t item) const override;
    [[nodiscard]] AnatomyStateSet realized_item_anatomy_states(std::size_t item) const override;

    // Stage T1 anatomy, as dot members; its states are Control's.
    struct Parts final {
        PartGeometry region;
        PartGeometry well;
        PartGeometry header;
        PartGeometry vertical_scroll_bar;
        PartGeometry horizontal_scroll_bar;
        PartGeometry scroll_corner;
        PartGeometry focus;
    };
    [[nodiscard]] Parts parts() const;

    // The "item" item system: one realized item (a details row, or a cell of
    // the other views).
    struct ItemParts final {
        PartGeometry cell;
        PartGeometry item_face;
        PartGeometry check_well;
        PartGeometry check;
        PartGeometry icon;
        PartGeometry text;
        PartGeometry cell_focus;
    };
    struct ItemStates final {
        bool selected{};
        bool hot{};
        bool current{};
        bool checked{};
        bool disabled{};
        bool focus_visible{};
    };
    struct Item final {
        // The item's position in the model.
        std::size_t index{};
        std::string stable_id;
        ItemParts parts;
        ItemStates states;
    };
    // The realized items in paint order.
    [[nodiscard]] std::vector<Item> realized_items() const;

    // The "sub_item" item system: one visible details-view cell past the
    // first column. Its identity is the item's ID, ".sub.", and the column's.
    struct SubItemParts final {
        PartGeometry sub_item_cell;
        PartGeometry sub_item_text;
    };
    struct SubItemStates final {
        bool selected{};
        bool disabled{};
    };
    struct SubItem final {
        std::size_t index{};
        std::size_t column{};
        std::string stable_id;
        SubItemParts parts;
        SubItemStates states;
    };
    [[nodiscard]] std::vector<SubItem> realized_sub_items() const;

    // The "column" item system, named as ObjectView's: one visible column
    // header of the details view. Its identity is the view's stable ID,
    // ".column.", and the column's ID.
    struct ColumnParts final {
        PartGeometry column_header;
        PartGeometry column_label;
        PartGeometry sort_glyph;
        PartGeometry column_divider;
        PartGeometry column_grip;
    };
    struct ColumnStates final {
        bool hot{};
        bool pressed{};
        bool captured{};
        // Application-defined column states.
        bool sorted_ascending{};
        bool sorted_descending{};
    };
    struct Column final {
        std::size_t index{};
        std::string stable_id;
        ColumnParts parts;
        ColumnStates states;
    };
    [[nodiscard]] std::vector<Column> realized_columns() const;

protected:
    void append_interactions(std::vector<InteractionDeclaration>& interactions) const override;
    void on_attached_to_window() override;
    void publish_part_geometry(std::span<PartGeometry> geometry) const override;
    [[nodiscard]] ItemSystemIndex publish_realized_item_system(std::size_t item) const override;
    void publish_realized_item_geometry(std::size_t item, std::span<PartGeometry> geometry) const override;

private:
    // Where everything sits for the current bounds, view and model, from one
    // pass (see layout()). A "line" is the unit the view scrolls by: a row
    // of the details and icon views, a column of items in the list view.
    struct Layout final {
        Rect well{};
        Rect header{};          // the details view's header band; empty otherwise
        Rect viewport{};        // where items are shown
        Rect vertical_bar{};    // empty when not shown
        Rect horizontal_bar{};
        Rect corner{};          // between two shown bars
        double scale{1.0};
        double row_height{};    // details, list and small-icon rows
        Size cell{};            // one item's cell; a details row spans the columns
        double columns_width{}; // the details columns' total scaled width
        double horizontal_offset{}; // clamped, logical units
        std::size_t shown_count{};  // items shown: none in a details view without columns
        std::size_t per_line{1U};   // items in one line
        std::size_t line_count{};
        std::size_t page_lines{1U}; // fully visible lines
        std::size_t visible_lines{}; // lines at least partly visible
        std::size_t top_line{};     // clamped
    };
    // Which part of the view a point is on.
    enum class HitZone : std::uint8_t { none, item, check, header, divider };
    struct Hit final {
        HitZone zone{HitZone::none};
        std::size_t index{};
    };
    using ItemIndices = std::unordered_map<std::string, std::size_t>;

    [[nodiscard]] Layout layout() const;
    [[nodiscard]] Layout layout_for(Rect client) const;
    [[nodiscard]] double text_line_height() const;
    [[nodiscard]] Size icon_size(bool large) const noexcept;
    [[nodiscard]] std::size_t maximum_top_line(const Layout& placed) const noexcept;
    // The realized items: model indices [first, end).
    [[nodiscard]] std::size_t first_realized(const Layout& placed) const noexcept;
    [[nodiscard]] std::size_t end_realized(const Layout& placed) const noexcept;
    [[nodiscard]] Rect item_bounds(const Layout& placed, std::size_t index) const noexcept;
    [[nodiscard]] Rect column_cell(const Layout& placed, std::size_t column) const noexcept;
    [[nodiscard]] bool column_visible(const Layout& placed, std::size_t column) const noexcept;
    [[nodiscard]] std::size_t visible_column_count(const Layout& placed) const noexcept;
    [[nodiscard]] std::size_t visible_column(const Layout& placed, std::size_t position) const noexcept;
    [[nodiscard]] ItemParts item_parts(const Layout& placed, std::size_t index, Rect clip) const;
    [[nodiscard]] SubItemParts sub_item_parts(const Layout& placed, std::size_t index,
                                              std::size_t column, Rect clip) const;
    [[nodiscard]] ColumnParts column_parts(const Layout& placed, std::size_t column, Rect clip) const;
    [[nodiscard]] AnatomyStateSet item_states(std::size_t index, bool checked) const;
    [[nodiscard]] AnatomyStateSet column_states(std::size_t column) const;
    // The item at `index`: a reference into the ordinary items, or `scratch`
    // filled by the source in virtual mode. Valid until `scratch` is reused
    // or the model changes.
    [[nodiscard]] const ListViewItem& item_at(std::size_t index, ListViewItem& scratch) const;
    [[nodiscard]] Hit hit_test(const Layout& placed, Point local) const;
    // The realized row of a realized-item position, and the sub-item and
    // column positions after the rows.
    [[nodiscard]] std::size_t realized_row_count(const Layout& placed) const noexcept;
    [[nodiscard]] std::size_t realized_sub_item_count(const Layout& placed) const noexcept;
    [[nodiscard]] std::size_t realized_sub_item_column(const Layout& placed, std::size_t position) const noexcept;

    void paint_item(Painter& painter, const Layout& placed, std::size_t index);
    void paint_column(Painter& painter, const Layout& placed, std::size_t column);

    void set_top_line(std::size_t line);
    void scroll_lines(std::ptrdiff_t lines);
    void sync_scroll_bars(const Layout& placed);
    void on_vertical_scroll(double value);
    void on_horizontal_scroll(double value);
    void image_list_changed(const ImageListChange& change);

    // Selection operations. Each returns whether the selected set changed.
    [[nodiscard]] bool select_only(std::size_t index);
    [[nodiscard]] bool select_none();
    [[nodiscard]] bool toggle_selected(std::size_t index);
    [[nodiscard]] bool select_range(std::size_t anchor, std::size_t index, bool keep_existing);
    [[nodiscard]] bool assign_selected(std::size_t index, bool selected);
    void publish_selection();
    // A pointer press or a navigation key on `index`, with its modifiers.
    void choose(std::size_t index, Modifier modifiers, bool move_only);
    void move_focus(std::size_t index);
    void toggle_check(std::size_t index);
    void activate(std::size_t index);
    void request_column_click(std::size_t column);
    void finish_column_resize();
    [[nodiscard]] std::optional<std::size_t> navigation_target(std::uint32_t key, std::size_t from,
                                                               const Layout& placed) const;
    void type_select(std::string_view text);
    [[nodiscard]] std::optional<std::size_t> realized_index_of(std::string_view item_id) const;
    void reset_after_model_change(std::size_t count);

    std::vector<ListViewColumn> columns_{};
    std::vector<ListViewItem> items_{};
    ItemIndices item_indices_{};
    std::shared_ptr<ListViewItemSource> source_{};
    std::size_t virtual_count_{};
    // One flag per item: nonzero when selected.
    std::vector<std::uint8_t> selected_{};
    std::size_t selected_count_{};
    std::optional<std::size_t> focused_index_{};
    std::optional<std::size_t> anchor_index_{};
    std::optional<std::size_t> hot_index_{};
    std::optional<std::size_t> hot_column_{};
    std::optional<std::size_t> pressed_column_{};
    std::optional<std::size_t> resizing_column_{};
    double resize_start_x_{};
    double resize_start_width_{};
    // The first visible item; the layout shows the line holding it.
    std::size_t top_item_{};
    double horizontal_offset_{};
    ListViewView view_{ListViewView::details};
    std::optional<std::size_t> sort_column_{};
    ListViewSortOrder sort_order_{ListViewSortOrder::none};
    bool check_boxes_{};
    bool multi_select_{true};
    bool full_row_select_{};
    bool focused_{};
    bool synchronizing_scroll_{};
    FontSpec font_{FontRole::content, 10.5, 400, false};
    std::shared_ptr<ImageList> small_images_{};
    std::shared_ptr<ImageList> large_images_{};
    SubscriptionToken small_images_changed_{};
    SubscriptionToken large_images_changed_{};
    std::shared_ptr<VScrollBar> vertical_bar_{};
    std::shared_ptr<HScrollBar> horizontal_bar_{};
    SubscriptionToken vertical_scrolled_{};
    SubscriptionToken horizontal_scrolled_{};
    // Paint scratch, reused for every item of a paint pass.
    ListViewItem paint_item_{};
    std::string paint_text_{};
    // Type-ahead.
    std::string type_prefix_{};
    std::chrono::steady_clock::time_point last_type_time_{};
    Event<const ListViewSelectionChange&> selection_changed_{};
    Event<const ListViewItemActivation&> item_activated_{};
    Event<ListViewItemCheck&> item_checking_{};
    Event<const ListViewItemCheck&> item_checked_{};
    Event<const ListViewColumnClick&> column_click_{};
    Event<const ListViewColumnWidthChange&> column_width_changed_{};
};

} // namespace gui_forms
