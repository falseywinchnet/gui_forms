#pragma once

#include "gui_forms/controls/scrollable_control/container_control/container_control.hpp"
#include "gui_forms/controls/scrollable_control/container_control/tab_control/tab_page/tab_page.hpp"
#include "gui_forms/pages/page_set/page_set.hpp"

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

class ImageList;
class PageStrip;
struct ImageListChange;
struct PageInvocation;

enum class TabAlignment : std::uint8_t {
    top,
    bottom,
    left,
    right,
};

// How the tabs are drawn (until stage T4 draws Plan Paint's tab stack, each
// is a recipe family of the by-role paint path): `normal`, flat at rest with
// a plate when hot or selected (the menu-item family); `buttons`, a button's
// plate on every tab; `flat_buttons`, a command button's lighter plate.
enum class TabAppearance : std::uint8_t {
    normal,
    buttons,
    flat_buttons,
};

// How wide each tab is (Windows Forms TabSizeMode). `normal`: its caption
// plus padding. `fill_to_right`: the row shared equally among the tabs.
// `fixed`: `item_size` along the row, whatever the caption.
enum class TabSizeMode : std::uint8_t {
    normal,
    fill_to_right,
    fixed,
};

struct TabSelectionChange final {
    std::optional<std::size_t> old_index;
    std::optional<std::size_t> new_index;
};

// A set of TabPages shown one at a time, chosen from a row of tabs (the
// Windows Forms surface). The tabs are a PageStrip (owner decision DK-14,
// phase P8): the control keeps one record per page in a PageSet the strip
// shows, so the strip's geometry, painting by role, overflow (scroll and
// list buttons) and keys serve the tabs. The strip is an internal child: it
// takes no focus (the control keeps it, as one tab stop) and publishes no
// semantics of its own (the control publishes its tabs as before).
class TabControl final : public ContainerControl {
public:
    static constexpr bool initialize_tree_after_construction = true;
    // The most pages a tab control holds: its strip's set's capacity.
    static constexpr std::size_t maximum_pages = PageSetLimits::maximum_capacity;

    explicit TabControl(StableId stable_id);
    ~TabControl() override;
    // Creates the tab strip. `make_control` calls it; do not call it yourself.
    void initialize_control_tree();

    void add_page(std::shared_ptr<TabPage> page);
    [[nodiscard]] std::shared_ptr<TabPage> remove_page(const TabPage& page);
    [[nodiscard]] std::vector<std::shared_ptr<TabPage>> pages() const;
    [[nodiscard]] std::size_t page_count() const;
    [[nodiscard]] std::shared_ptr<TabPage> page_at(std::size_t index) const;

    [[nodiscard]] std::optional<std::size_t> selected_index() const;
    [[nodiscard]] std::shared_ptr<TabPage> selected_tab() const noexcept {
        return selected_page_.lock();
    }
    void set_selected_index(std::size_t index);
    void set_selected_tab(const std::shared_ptr<TabPage>& page);
    [[nodiscard]] TabAlignment alignment() const noexcept { return alignment_; }
    void set_alignment(TabAlignment alignment);
    [[nodiscard]] TabAppearance appearance() const noexcept { return appearance_; }
    void set_appearance(TabAppearance appearance);
    [[nodiscard]] TabSizeMode size_mode() const noexcept { return size_mode_; }
    void set_size_mode(TabSizeMode mode);
    [[nodiscard]] Size item_size() const noexcept { return item_size_; }
    void set_item_size(Size size);
    [[nodiscard]] const BasicControlStyle& style() const noexcept { return style_; }
    void set_style(BasicControlStyle style);
    // The images the pages' image_key and image_index name (Windows Forms'
    // ImageList); a tab shows its image before its text. Throws
    // std::invalid_argument for a disposed list or one belonging to another
    // window, and keeps the old list.
    [[nodiscard]] std::shared_ptr<ImageList> image_list() const noexcept { return image_list_; }
    void set_image_list(std::shared_ptr<ImageList> image_list);
    [[nodiscard]] Rect tab_bounds(std::size_t index) const;
    [[nodiscard]] Rect display_bounds() const noexcept;
    // Whether the tabs do not all fit, so the strip shows its scroll and list
    // buttons.
    [[nodiscard]] bool overflowing() const;
    [[nodiscard]] Event<const TabSelectionChange&>& selected_index_changed() noexcept {
        return selected_index_changed_;
    }

    [[nodiscard]] Size measure(Size available) override;
    void arrange(Rect final_bounds) override;
    void on_paint(Painter& painter, Rect local_damage) override;
    void on_key_preview(KeyEvent& event) override;
    void on_key(KeyEvent& event) override;
    void on_focus_changed(bool focused) override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;
    [[nodiscard]] std::vector<SemanticNode> semantic_virtual_children() const override;
    bool on_semantic_child_action(std::string_view stable_id,
                                  SemanticAction action,
                                  std::string_view value) override;
    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;
    [[nodiscard]] std::size_t realized_item_count() const override;
    [[nodiscard]] std::string realized_item_stable_id(std::size_t item) const override;
    [[nodiscard]] AnatomyStateSet realized_item_anatomy_states(
        std::size_t item) const override;
    // Stage T1 anatomy, as dot members.
    struct Parts final {
        ScrollableControl::Parts scrollable;
        PartGeometry face;
        PartGeometry display;
    };
    [[nodiscard]] Parts parts() const;
    // A tab's parts, in the control's coordinates: the strip's tab, label,
    // focus, seam and icon parts of the same names (the strip paints them).
    struct ItemParts final {
        PartGeometry tab;
        PartGeometry tab_label;
        PartGeometry tab_focus;
        PartGeometry tab_seam;
        PartGeometry tab_icon;
    };
    struct ItemStates final {
        bool selected{};
    };
    struct Item final {
        std::size_t index{};
        std::string stable_id;
        ItemParts parts;
        ItemStates states;
    };
    // The items currently realized, in paint order (owner decision 5).
    [[nodiscard]] std::vector<Item> realized_items() const;

protected:
    void append_interactions(std::vector<InteractionDeclaration>& interactions) const override;
    void publish_part_geometry(std::span<PartGeometry> geometry) const override;
    void publish_realized_item_geometry(std::size_t item,
                                        std::span<PartGeometry> geometry) const override;
    void on_dispose() noexcept override;

private:
    using RememberedFocusMap =
        std::unordered_map<std::uint64_t, Control::WeakPtr>;
    [[nodiscard]] std::optional<std::size_t> index_of(
        const std::shared_ptr<TabPage>& page) const;
    void select_relative(int delta);
    void show_selected(std::size_t index, const std::vector<std::shared_ptr<TabPage>>& live);
    void remember_page_focus(const std::shared_ptr<TabPage>& page);
    void restore_page_focus(const std::shared_ptr<TabPage>& page,
                            bool selection_owned_focus);
    void reconcile_pages();
    // -- the strip's set --
    void synchronize_tab_set();
    void select_in_tab_set(const std::shared_ptr<TabPage>& page);
    void configure_strip();
    [[nodiscard]] Rect header_bounds() const noexcept;
    [[nodiscard]] ItemParts tab_parts(const TabPage& page) const;
    // The key of the page's image in the image list: its image_key, or the
    // key at its image_index; empty when it names none the list has.
    [[nodiscard]] std::string tab_image_key(const TabPage& page) const;
    void on_image_list_changed(const ImageListChange& change);
    [[nodiscard]] bool nested_tab_control_has_focus() const;
    void on_set_selecting(PageSelectingEvent& event);
    void on_set_selection_changed(const PageSelectionChange& change);
    void on_strip_invoked(const PageInvocation& invocation);

    std::vector<std::weak_ptr<TabPage>> pages_;
    std::weak_ptr<TabPage> selected_page_;
    RememberedFocusMap remembered_focus_;
    BasicControlStyle style_;
    // Shared with the strip, which paints the images.
    std::shared_ptr<ImageList> image_list_{};
    Size item_size_{120.0, 30.0};
    TabAlignment alignment_{TabAlignment::top};
    TabAppearance appearance_{TabAppearance::normal};
    TabSizeMode size_mode_{TabSizeMode::normal};
    // The strip's pages: one record per live page in tab order, the page's
    // stable ID its PageId and its text its title; the set's selection is
    // the control's. The strip borrows the set and is released before the
    // set is destroyed.
    PageSet tab_set_;
    std::shared_ptr<PageStrip> strip_;
    // Set while the control edits `tab_set_` itself, so that its handlers
    // ignore the set's events about those edits.
    bool editing_tab_set_{};
    SubscriptionToken selecting_token_;
    SubscriptionToken selection_token_;
    SubscriptionToken invoked_token_;
    // The list's changes move the keys image_index names.
    SubscriptionToken image_list_token_;
    Event<const TabSelectionChange&> selected_index_changed_;
};

} // namespace gui_forms
