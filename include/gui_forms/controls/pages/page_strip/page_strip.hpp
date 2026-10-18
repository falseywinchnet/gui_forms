#pragma once

#include "gui_forms/components/context_menu/context_menu.hpp"
#include "gui_forms/control.hpp"
#include "gui_forms/event.hpp"
#include "gui_forms/image_list.hpp"
#include "gui_forms/pages.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace gui_forms {

// The edge of its content a strip sits on. Side strips (left and right, with
// turned labels) are phase P5's.
enum class PageStripPlacement : std::uint8_t { top, bottom };

// How wide each tab is. `content`: its label (and icon) plus padding, within
// the minimum and maximum. `fixed`: the fixed width, whatever the label.
// `fill`: the strip's width shared equally, within the minimum and maximum.
enum class PageItemSizing : std::uint8_t { content, fixed, fill };

// Whether moving the keyboard between tabs selects (automatic, the default:
// owner decision DK-37) or only moves the focus, Space or Enter then
// selecting (manual, for pages whose content is slow to show).
enum class PageStripActivation : std::uint8_t { automatic, manual };

// Tab widths in logical units at text scale 1. 0 < minimum <= maximum <=
// `maximum_width`, and 0 < fixed <= `maximum_width`.
struct PageItemWidths final {
    static constexpr double maximum_width = 1024.0;

    double minimum{40.0};
    double maximum{240.0};
    double fixed{120.0};
};

// A tab was clicked, chosen with the keyboard, by its mnemonic, from the page
// list or through accessibility, and its page is now selected (it may have
// been selected already). Raised after the selection and its
// `selected_page_changed`; never for a programmatic selection and never when
// a `selecting` handler cancelled. Whoever decides activation (a docking
// workspace) listens to it; the strip attaches no meaning (owner decision
// DK-37).
struct PageInvocation final {
    PageId page{};
    PageSelectionCause cause{PageSelectionCause::pointer};
};

// Why a page's close was asked for. Phase P2 raises only `semantic` (the
// accessibility close action); phase P4's close glyph, middle click and
// Ctrl+F4 raise `pointer` and `keyboard` through the same event.
enum class PageCloseCause : std::uint8_t { pointer, keyboard, semantic };

// A request to close a closable page. The strip never removes a page itself:
// whoever owns the set decides (owner decision DK-38). A handler that acts on
// the request sets `handled`.
struct PageCloseRequest final {
    PageId page{};
    PageCloseCause cause{PageCloseCause::semantic};
    bool handled{};
};

// The headers of one PageSet in a single row (owner decisions DK-13, DK-17,
// DK-37): a public control used by PageView, by docking's auto-hide strips and
// by any application strip. Its tabs are the set's pages, in the set's order,
// laid out as the `page` item system and painted through the by-role paint
// path with the `menu_item` family until stage T4 draws the tab stack (owner
// decision DK-40). The strip shows and changes the set's selection; it keeps
// no page state of its own beyond hover, press, keyboard position and scroll.
//
// The set is borrowed. Its owner keeps it alive while the strip is bound to
// it, and calls `release_page_set` before destroying it if the strip may
// outlive it; a released strip shows no tabs and refuses nothing (every
// operation on pages is then a no-op returning false or none). PageView owns
// the set its strip borrows and releases the strip when it is destroyed.
//
// Overflow is the strip policy (DK-37, interview P11 and P13): when the tabs
// do not fit, scroll buttons and a list button appear at the strip's end; the
// list shows every page, and a page chosen from it is selected and scrolled
// into view. Layout keeps the selected page visible: a selection, an edit, a
// resize or a setting scrolls it into view. Only the scroll buttons and the
// wheel move it out of view, until the next of those. The list popup is the
// existing ContextMenu.
//
// Event order for a press on an unselected tab: the set's `selecting`, its
// `selected_page_changed`, then `page_invoked`; then the strip takes focus.
// A cancelled selection raises nothing more. The strip handles the set's
// `pages_changed` and `selected_page_changed` only to relayout and repaint.
class PageStrip : public Control {
public:
    // Borrows `pages` (see the lifetime rule above).
    PageStrip(StableId stable_id, PageSet& pages);
    ~PageStrip() override;

    // The bound set, or null after `release_page_set`.
    [[nodiscard]] PageSet* page_set() const noexcept { return pages_; }
    // Unbinds the set: the strip forgets it, closes the page list and shows
    // no tabs. Rebinding is not offered; a released strip stays empty.
    void release_page_set() noexcept;

    [[nodiscard]] PageStripPlacement placement() const noexcept { return placement_; }
    void set_placement(PageStripPlacement placement);
    [[nodiscard]] PageItemSizing item_sizing() const noexcept { return item_sizing_; }
    void set_item_sizing(PageItemSizing sizing);
    [[nodiscard]] const PageItemWidths& item_widths() const noexcept { return item_widths_; }
    // Throws std::invalid_argument for widths outside the bounds stated on
    // PageItemWidths, leaving the old widths.
    void set_item_widths(PageItemWidths widths);
    [[nodiscard]] PageStripActivation activation() const noexcept { return activation_; }
    void set_activation(PageStripActivation activation);
    // Whether `&` in a title marks a mnemonic that selects the page (on for
    // strips; the document preset turns it off, DK-38). When off, a title is
    // shown as written.
    [[nodiscard]] bool mnemonics_enabled() const noexcept { return mnemonics_enabled_; }
    void set_mnemonics_enabled(bool enabled);
    [[nodiscard]] FontSpec font() const noexcept { return font_; }
    void set_font(FontSpec font);
    // The images a page's `image_key` names. Throws std::invalid_argument for
    // a disposed list or one belonging to another window.
    [[nodiscard]] std::shared_ptr<ImageList> image_list() const noexcept { return image_list_; }
    void set_image_list(std::shared_ptr<ImageList> image_list);

    // Scrolls so the page's tab is visible. False for a page not in the set.
    bool ensure_visible(const PageId& page);
    // Scrolls the row by one tab towards the start or the end. False when it
    // cannot scroll that way.
    bool scroll_backward();
    bool scroll_forward();
    // Whether the tabs do not all fit, so the scroll and list buttons show.
    [[nodiscard]] bool overflowing() const;
    // The pages whose tabs are laid out, in order.
    [[nodiscard]] std::vector<PageId> visible_pages() const;
    // The page whose tab is under a point in window coordinates, or none.
    [[nodiscard]] std::optional<PageId> page_at(Point window_point) const;
    // The page the keyboard is on while the strip has focus, or none.
    [[nodiscard]] std::optional<PageId> focused_page() const;

    // Opens the list of every page below (or, on a bottom strip, above) the
    // list button. False when the strip is not overflowing or not attached.
    bool show_page_list(MenuOpenMode mode = MenuOpenMode::keyboard);
    void close_page_list() noexcept;
    [[nodiscard]] bool page_list_open() const noexcept;

    // Selects the next (+1) or previous (-1) page, wrapping, with `cause`,
    // and raises `page_invoked` unless a `selecting` handler cancelled.
    // Ctrl+PageDown and Ctrl+PageUp, and PageView's Ctrl+Tab, come here.
    bool select_relative(int direction, PageSelectionCause cause);

    [[nodiscard]] Event<const PageInvocation&>& page_invoked() noexcept { return page_invoked_; }
    [[nodiscard]] Event<PageCloseRequest&>& page_close_requested() noexcept {
        return page_close_requested_;
    }

    [[nodiscard]] Size measure(Size available) override;
    void arrange(Rect final_bounds) override;
    void on_paint(Painter& painter, Rect local_damage) override;
    void on_pointer(PointerEvent& event) override;
    void on_key(KeyEvent& event) override;
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
        // The band behind the tabs.
        PartGeometry rail;
        PartGeometry scroll_back;
        PartGeometry scroll_back_glyph;
        PartGeometry scroll_forward;
        PartGeometry scroll_forward_glyph;
        PartGeometry list_button;
        PartGeometry list_glyph;
        PartGeometry focus;
    };
    [[nodiscard]] Parts parts() const;
    // Item part names start with tab: an item part may not repeat one of the
    // strip's own part names. Phase P4 adds the close and pin actuators and
    // the modified mark in the space after the label.
    struct ItemParts final {
        PartGeometry tab;
        PartGeometry tab_icon;
        PartGeometry tab_label;
        // The selected tab's join with the content, on the content's side.
        PartGeometry tab_seam;
        PartGeometry tab_focus;
    };
    struct ItemStates final {
        bool hot{};
        bool pressed{};
        bool selected{};
        bool disabled{};
        bool focus_visible{};
        // The admitted custom state (DK-40): the page is in the pinned group.
        bool pinned{};
    };
    struct Item final {
        // The page's position in the set.
        PagePosition position{};
        PageId page{};
        std::string stable_id;
        ItemParts parts;
        ItemStates states;
    };
    // The tabs laid out (not those scrolled out of view), in order.
    [[nodiscard]] std::vector<Item> realized_items() const;

    // The semantic identity of a page's tab: `<strip id>.tab.<page id>`.
    [[nodiscard]] std::string tab_identity(const PageId& page) const;

protected:
    // The stable identity of the tab_panel node a page's tab controls, or
    // empty (owner decision DK-40). A strip alone shows no panels; PageView's
    // strip names the panel of a page that has content.
    [[nodiscard]] virtual std::string page_panel_identity(const PageId& page) const;

    void append_interactions(std::vector<InteractionDeclaration>& interactions) const override;
    void on_attached_to_window() override;
    void on_dispose() noexcept override;
    [[nodiscard]] bool mnemonic_matches(char32_t character) const noexcept override;
    bool process_mnemonic_self(char32_t character) override;
    void publish_part_geometry(std::span<PartGeometry> geometry) const override;
    void publish_realized_item_geometry(std::size_t item, std::span<PartGeometry> geometry) const override;

private:
    // What a point or a press is on.
    enum class Zone : std::uint8_t { none, tab, scroll_back, scroll_forward, list_button };
    struct Hit final {
        Zone zone{Zone::none};
        // The tab's position in the set, for Zone::tab.
        std::size_t position{};

        friend bool operator==(const Hit& left, const Hit& right) noexcept {
            const bool equal = left.zone == right.zone && left.position == right.position;
            return equal;
        }
        friend bool operator!=(const Hit& left, const Hit& right) noexcept {
            const bool different = !(left == right);
            return different;
        }
    };

    // One tab, in set order. `bounds` is empty when the tab is scrolled out
    // of view. `label` is the title as shown (mnemonic marker removed).
    struct TabSlot final {
        double width{};
        Rect bounds{};
        std::string label{};
        bool has_icon{};
    };
    // What the cached layout was computed from; a layout is recomputed when
    // any of these differ.
    struct LayoutKey final {
        Size surface{};
        double scale{};
        std::uint64_t set_revision{};
        std::uint64_t settings_revision{};
        std::uint64_t scroll_revision{};
        std::size_t page_count{};
        bool bound{};

        friend bool operator==(const LayoutKey& left, const LayoutKey& right) noexcept {
            const bool equal = left.surface == right.surface && left.scale == right.scale &&
                               left.set_revision == right.set_revision &&
                               left.settings_revision == right.settings_revision &&
                               left.scroll_revision == right.scroll_revision &&
                               left.page_count == right.page_count && left.bound == right.bound;
            return equal;
        }
    };
    // One layout pass. `tabs` has one slot per page (its capacity is kept
    // between passes); `first_visible` and `visible_count` name the run of
    // laid-out tabs.
    struct Layout final {
        std::vector<TabSlot> tabs{};
        Rect rail{};
        Rect scroll_back{};
        Rect scroll_forward{};
        Rect list_button{};
        std::size_t first_visible{};
        std::size_t visible_count{};
        // The largest `first_visible` that still fills the row to its end.
        std::size_t last_first{};
        // The selected page's position when the layout was computed (the
        // set's revision, part of the key, moves with every selection).
        std::optional<std::size_t> selected{};
        bool overflow{};
        bool valid{};
        LayoutKey key{};
    };

    // -- layout --
    [[nodiscard]] LayoutKey current_key() const;
    // The cached layout, recomputed first when its key changed.
    [[nodiscard]] const Layout& layout() const;
    void compute_layout(const LayoutKey& key) const;
    void measure_tabs() const;
    [[nodiscard]] double item_extent_scale() const noexcept;
    [[nodiscard]] std::size_t first_showing(std::size_t target, double area) const;
    void place_tabs(double start, double area) const;
    void bump_settings();

    // -- one geometry function per part --
    [[nodiscard]] ItemParts tab_parts(std::size_t position) const;
    [[nodiscard]] Rect scroll_glyph_bounds(Rect button) const;
    [[nodiscard]] Parts strip_parts() const;

    // -- states --
    [[nodiscard]] AnatomyStateSet tab_states(std::size_t position) const;
    [[nodiscard]] AnatomyStateSet button_states(Zone zone) const;
    [[nodiscard]] bool can_scroll_backward() const;
    [[nodiscard]] bool can_scroll_forward() const;
    [[nodiscard]] bool focus_cue() const noexcept;

    // -- input --
    [[nodiscard]] Hit hit_at(Point window_point) const;
    void set_hot(Hit hit);
    // Selects the page at `position` with `cause` and raises page_invoked
    // unless a selecting handler cancelled. False when cancelled or refused.
    bool choose(std::size_t position, PageSelectionCause cause);
    void move_keyboard(std::size_t position);
    [[nodiscard]] std::optional<std::size_t> keyboard_position() const;
    [[nodiscard]] std::optional<std::size_t> selected_position() const;
    bool scroll_by(int direction);
    void reveal(std::size_t position);
    [[nodiscard]] std::optional<std::size_t> position_of_tab_identity(std::string_view identity) const;

    // -- page list --
    void prepare_list_commands();
    void on_list_item_invoked(const MenuItemInvocation& invocation);
    void on_list_open_changed(bool open);

    // -- set and image list notifications --
    void on_pages_changed(const PageSetChange& change);
    void on_selection_changed(const PageSelectionChange& change);
    void on_image_list_changed(const ImageListChange& change);

    PageSet* pages_{};
    PageStripPlacement placement_{PageStripPlacement::top};
    PageItemSizing item_sizing_{PageItemSizing::content};
    PageItemWidths item_widths_{};
    PageStripActivation activation_{PageStripActivation::automatic};
    bool mnemonics_enabled_{true};
    FontSpec font_{FontRole::control, 10.5, 400, false, 0.12};
    std::shared_ptr<ImageList> image_list_{};

    // The cached layout (see LayoutKey); paint and the published geometry
    // read it, so painting n tabs allocates nothing per tab.
    mutable Layout layout_{};
    // The first tab the row starts at, as requested by scrolling; the layout
    // clamps it and writes the clamped value back.
    mutable std::size_t first_visible_{};
    // Set when anything but scrolling changed: the next layout scrolls the
    // selected page (or, under manual activation, the keyboard's page) into
    // view.
    mutable bool reveal_pending_{true};
    std::uint64_t settings_revision_{};
    std::uint64_t scroll_revision_{};

    Hit hot_{};
    Hit pressed_{};
    bool focused_{};
    // The keyboard's page while focused; under automatic activation it is
    // the selected page.
    std::optional<PageId> keyboard_page_{};

    std::unique_ptr<ContextMenu> list_{};
    // One command per list row, reused between openings: a menu row needs a
    // shared command. Row i lists the page at position i.
    std::vector<std::shared_ptr<Command>> list_commands_{};
    bool list_open_{};

    SubscriptionToken pages_changed_token_{};
    SubscriptionToken selection_token_{};
    SubscriptionToken list_invoked_token_{};
    SubscriptionToken list_changed_token_{};
    SubscriptionToken image_list_token_{};
    Event<const PageInvocation&> page_invoked_{};
    Event<PageCloseRequest&> page_close_requested_{};
};

} // namespace gui_forms
