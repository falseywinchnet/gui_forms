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

// What the strip does when its tabs do not fit (owner decisions DK-31 and
// DK-37). `scroll`, the strip policy: scroll buttons and a list button at the
// strip's end; the tabs keep their widths. `shrink_then_list`, the document
// policy: ordinary tabs shrink towards the minimum width, the widest first;
// when even that does not fit, the tabs from the end of the order that do
// not fit are hidden behind a list button showing their count. Pinned tabs
// never shrink and are never hidden under `shrink_then_list`.
enum class PageStripOverflow : std::uint8_t { scroll, shrink_then_list };

// Which closable tabs show their close button (owner decision DK-38). Space
// for it is reserved on every closable tab whatever the rule, so a tab's
// width does not change when the button appears. `none` reserves nothing.
enum class PageCloseButtons : std::uint8_t { none, selected_and_hot, all };

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

// Why a page's close was asked for: the close button (`pointer`), a middle
// click released over the tab it pressed (`middle_button`), Ctrl+F4
// (`keyboard`), or the accessibility close action (`semantic`).
enum class PageCloseCause : std::uint8_t { pointer, middle_button, keyboard, semantic };

// A request to close a closable page. The strip never removes a page itself:
// whoever owns the set decides (owner decision DK-38). A handler that acts on
// the request, or refuses it (a document that asks to save and is
// cancelled), sets `handled`. A PageView whose close policy is
// `remove_when_unhandled` removes the page after the handlers when none set
// it; a docking owner only requests.
struct PageCloseRequest final {
    PageId page{};
    PageCloseCause cause{PageCloseCause::semantic};
    bool handled{};
};

// A request to pin or unpin a page: a click on a pinned tab's pin glyph asks
// to unpin it. When no handler sets `handled`, the strip applies it to the
// set (PageSet::set_pinned).
struct PagePinRequest final {
    PageId page{};
    bool pinned{};
    bool handled{};
};

// The headers of one PageSet in a single row (owner decisions DK-13, DK-17,
// DK-37, DK-38): a public control used by PageView, by docking's auto-hide
// strips and by any application strip. Its tabs are the set's pages, in the
// set's order, laid out as the `page` item system and painted through the
// by-role paint path with the `menu_item` family until stage T4 draws the
// tab stack and the notched document tab (owner decisions DK-40, DK-42). The
// strip shows and changes the set's selection; it keeps no page state of its
// own beyond hover, press, keyboard position, scroll and frozen widths.
//
// The set is borrowed. Its owner keeps it alive while the strip is bound to
// it, and calls `release_page_set` before destroying it if the strip may
// outlive it; a released strip shows no tabs and refuses nothing (every
// operation on pages is then a no-op returning false or none). PageView owns
// the set its strip borrows and releases the strip when it is destroyed.
//
// Overflow (see PageStripOverflow): the list shows every page, those whose
// tabs are not laid out in italic. Under `scroll` a page chosen from it is
// selected and scrolled into view. Under `shrink_then_list` a hidden page
// chosen from it takes the last visible position, the pages before it keeping
// their places (interview P13): the strip selects it and moves it there in
// one PageSet batch, so the pinned boundary holds as for any move. A hidden
// page selected another way (a key, a program) is shown in the last visible
// place without being moved. Layout keeps the selected page visible: a
// selection, an edit, a resize or a setting brings it into view. Only the
// scroll buttons and the wheel move it out of view, until the next of those.
// The list popup is the existing ContextMenu.
//
// Document features (owner decision DK-38, interview P16 to P26): a close
// button on closable tabs (see PageCloseButtons) with a plate while hot; a
// middle click released over the tab it pressed closes it; Ctrl+F4 closes the
// selected closable page (Delete does not); every close is a
// `page_close_requested`. A tab whose page is `modified` shows a dot in the
// close button's place, replaced by the close button while the tab is hot.
// After a close by the pointer the tabs keep their widths until the pointer
// leaves the strip, so the next tab's close button arrives under the
// pointer. Pinned pages (PageSet pinning) show only their icon until hot,
// then their full width; their pin glyph replaces the close button and
// asks to unpin. Ctrl+Shift+PageDown and Ctrl+Shift+PageUp move the
// selected page within its group.
//
// Event order for a press on an unselected tab: the set's `selecting`, its
// `selected_page_changed`, then `page_invoked`; then the strip takes focus.
// A cancelled selection raises nothing more. A press on a close button or a
// pin glyph selects nothing; the request is raised on the release over the
// same button. The strip handles the set's `pages_changed` and
// `selected_page_changed` only to relayout and repaint.
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
    [[nodiscard]] PageStripOverflow overflow() const noexcept { return overflow_; }
    void set_overflow(PageStripOverflow overflow);
    [[nodiscard]] PageCloseButtons close_buttons() const noexcept { return close_buttons_; }
    void set_close_buttons(PageCloseButtons buttons);
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
    // cannot scroll that way (always under `shrink_then_list`).
    bool scroll_backward();
    bool scroll_forward();
    // Whether the tabs do not all fit, so the scroll and list buttons (or,
    // under `shrink_then_list`, the list button) show.
    [[nodiscard]] bool overflowing() const;
    // The pages whose tabs are laid out, in the order they are shown.
    [[nodiscard]] std::vector<PageId> visible_pages() const;
    // How many pages have no tab laid out (scrolled out of view or hidden
    // behind the list button).
    [[nodiscard]] std::size_t hidden_count() const;
    // The page whose tab is under a point in window coordinates, or none.
    [[nodiscard]] std::optional<PageId> page_at(Point window_point) const;
    // The page the keyboard is on while the strip has focus, or none.
    [[nodiscard]] std::optional<PageId> focused_page() const;
    // Whether tab widths are frozen after a close by the pointer (they thaw
    // when the pointer leaves the strip, or the strip's size or settings
    // change).
    [[nodiscard]] bool widths_frozen() const noexcept { return widths_frozen_; }

    // Opens the list of every page below (or, on a bottom strip, above) the
    // list button. False when the strip is not overflowing or not attached.
    bool show_page_list(MenuOpenMode mode = MenuOpenMode::keyboard);
    void close_page_list() noexcept;
    [[nodiscard]] bool page_list_open() const noexcept;
    // The list's rows while it is open (one per page, in order; hidden
    // pages' rows are italic), or empty.
    [[nodiscard]] std::vector<MenuItemSpec> page_list_items() const;

    // Selects the next (+1) or previous (-1) page, wrapping, with `cause`,
    // and raises `page_invoked` unless a `selecting` handler cancelled.
    // Ctrl+PageDown and Ctrl+PageUp, and PageView's Ctrl+Tab, come here.
    bool select_relative(int direction, PageSelectionCause cause);
    // Moves the selected page one place later (+1) or earlier (-1) within its
    // group, the pinned group or the ordinary pages: Ctrl+Shift+PageDown and
    // Ctrl+Shift+PageUp. False at its group's end, with nothing selected, or
    // when the set refuses.
    bool move_selected(int direction);
    // Raises `page_close_requested` for a closable page in the set, then,
    // when no handler set `handled`, gives the owner its fallback (PageView's
    // close policy). True when the request was handled (by a handler or the
    // fallback); false for a page that is not in the set or not closable,
    // which raises nothing, and when nobody handled it.
    bool request_close(const PageId& page, PageCloseCause cause);

    [[nodiscard]] Event<const PageInvocation&>& page_invoked() noexcept { return page_invoked_; }
    [[nodiscard]] Event<PageCloseRequest&>& page_close_requested() noexcept {
        return page_close_requested_;
    }
    [[nodiscard]] Event<PagePinRequest&>& page_pin_requested() noexcept { return page_pin_requested_; }

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
        // The count of hidden pages on the list button (`shrink_then_list`).
        PartGeometry list_count;
        PartGeometry list_glyph;
        PartGeometry focus;
    };
    [[nodiscard]] Parts parts() const;
    // Item part names start with tab: an item part may not repeat one of the
    // strip's own part names. The close button, the pin and the modified dot
    // share one slot after the label; at most one of them is present.
    struct ItemParts final {
        PartGeometry tab;
        PartGeometry tab_icon;
        PartGeometry tab_label;
        // The dot of a modified page, while its close button is not shown.
        PartGeometry tab_modified;
        PartGeometry tab_close;
        PartGeometry tab_close_glyph;
        // A pinned tab's pin, shown in place of the close button while the
        // tab is expanded.
        PartGeometry tab_pin;
        PartGeometry tab_pin_glyph;
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
        // The admitted custom states (DK-35, DK-40): the page is in the
        // pinned group; its content has unsaved changes.
        bool pinned{};
        bool modified{};
    };
    struct Item final {
        // The page's position in the set.
        PagePosition position{};
        PageId page{};
        std::string stable_id;
        ItemParts parts;
        ItemStates states;
    };
    // The tabs laid out (not those scrolled out of view or hidden), in the
    // order they are shown.
    [[nodiscard]] std::vector<Item> realized_items() const;

    // The semantic identity of a page's tab: `<strip id>.tab.<page id>`.
    [[nodiscard]] std::string tab_identity(const PageId& page) const;

protected:
    // The stable identity of the tab_panel node a page's tab controls, or
    // empty (owner decision DK-40). A strip alone shows no panels; PageView's
    // strip names the panel of a page that has content.
    [[nodiscard]] virtual std::string page_panel_identity(const PageId& page) const;
    // Called by `request_close` after the handlers when none set `handled`.
    // A strip alone does nothing; PageView's strip applies the view's close
    // policy and sets `handled` when it removed the page.
    virtual void close_unhandled(PageCloseRequest& request);

    void append_interactions(std::vector<InteractionDeclaration>& interactions) const override;
    void on_attached_to_window() override;
    void on_dispose() noexcept override;
    [[nodiscard]] bool mnemonic_matches(char32_t character) const noexcept override;
    bool process_mnemonic_self(char32_t character) override;
    void publish_part_geometry(std::span<PartGeometry> geometry) const override;
    void publish_realized_item_geometry(std::size_t item, std::span<PartGeometry> geometry) const override;

private:
    // What a point or a press is on.
    enum class Zone : std::uint8_t { none, tab, tab_close, tab_pin, scroll_back, scroll_forward, list_button };
    struct Hit final {
        Zone zone{Zone::none};
        // The tab's position in the set, for the tab zones.
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
    // What occupies a tab's slot after its label.
    enum class SlotContent : std::uint8_t { none, close, pin, modified };

    // One tab, in set order. `width` is its extent along the row as laid out
    // (after shrinking or freezing); `bounds` is empty when the tab is not
    // laid out. `label` is the title as shown (mnemonic marker removed).
    struct TabSlot final {
        double width{};
        // The width the tab asks for before shrinking.
        double natural{};
        Rect bounds{};
        std::string label{};
        bool has_icon{};
        // The close, pin or modified slot is reserved after the label.
        bool reserves_slot{};
        // A pinned tab shown as its icon alone.
        bool collapsed{};
        // Scratch for the shrink pass: the tab keeps its natural width.
        bool settled{};
        bool pinned{};
    };
    // A tab's width when the strip froze its widths.
    struct FrozenWidth final {
        PageId page{};
        double width{};
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
        // The pinned tab expanded by hover, or `page_count` for none.
        std::size_t expanded{};
        bool bound{};

        friend bool operator==(const LayoutKey& left, const LayoutKey& right) noexcept {
            const bool equal = left.surface == right.surface && left.scale == right.scale &&
                               left.set_revision == right.set_revision &&
                               left.settings_revision == right.settings_revision &&
                               left.scroll_revision == right.scroll_revision &&
                               left.page_count == right.page_count && left.expanded == right.expanded &&
                               left.bound == right.bound;
            return equal;
        }
    };
    // One layout pass. `tabs` has one slot per page and `visible` the set
    // positions laid out, in the order shown (their capacities are kept
    // between passes). Under `scroll` the visible tabs are the run from
    // `first_visible`; under `shrink_then_list` they are the pinned tabs, the
    // ordinary tabs up to `front_end`, and possibly the selected page after
    // them.
    struct Layout final {
        std::vector<TabSlot> tabs{};
        std::vector<std::size_t> visible{};
        Rect rail{};
        Rect scroll_back{};
        Rect scroll_forward{};
        Rect list_button{};
        Rect list_count{};
        Rect list_glyph{};
        // The count of hidden pages as shown on the list button.
        std::string list_count_text{};
        std::size_t first_visible{};
        // The largest `first_visible` that still fills the row to its end.
        std::size_t last_first{};
        // One past the last ordinary tab of the front run
        // (`shrink_then_list`).
        std::size_t front_end{};
        std::size_t hidden{};
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
    void apply_frozen_widths() const;
    [[nodiscard]] double item_extent_scale() const noexcept;
    [[nodiscard]] std::size_t first_showing(std::size_t target, double area) const;
    void place_tabs(double start, double area) const;
    void place_visible(double start) const;
    void layout_scroll(bool scroll_only) const;
    void layout_shrink() const;
    [[nodiscard]] double shrink_cap(std::size_t begin, std::size_t end, double area,
                                    std::optional<std::size_t> extra) const;
    [[nodiscard]] Rect band_rect(double start, double length) const;
    void bump_settings();

    // -- one geometry function per part --
    // The tab's parts with the tab as hot or not (hit-testing asks for the
    // tab under the pointer as hot).
    [[nodiscard]] ItemParts tab_parts_as(std::size_t position, bool hot) const;
    [[nodiscard]] ItemParts tab_parts(std::size_t position) const;
    [[nodiscard]] SlotContent slot_content(std::size_t position, bool hot) const;
    [[nodiscard]] Rect glyph_bounds(Rect button) const;
    [[nodiscard]] Parts strip_parts() const;

    // -- states --
    [[nodiscard]] bool tab_hot(std::size_t position) const noexcept;
    [[nodiscard]] AnatomyStateSet tab_states(std::size_t position) const;
    [[nodiscard]] AnatomyStateSet slot_states(Zone zone, std::size_t position) const;
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
    // The page list's choice: selects the page and, under
    // `shrink_then_list`, moves a hidden page to the last visible position in
    // the same batch.
    bool choose_from_list(std::size_t position);
    void move_keyboard(std::size_t position);
    [[nodiscard]] std::optional<std::size_t> keyboard_position() const;
    [[nodiscard]] std::optional<std::size_t> selected_position() const;
    bool scroll_by(int direction);
    void reveal(std::size_t position);
    void freeze_widths();
    void thaw_widths();
    void request_pin(std::size_t position, bool pinned);
    void on_pointer_release(PointerEvent& event, Hit hit);
    void on_middle_button(PointerEvent& event, Hit hit);
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
    PageStripOverflow overflow_{PageStripOverflow::scroll};
    PageCloseButtons close_buttons_{PageCloseButtons::none};
    bool mnemonics_enabled_{true};
    FontSpec font_{FontRole::control, 10.5, 400, false, 0.12};
    std::shared_ptr<ImageList> image_list_{};

    // The cached layout (see LayoutKey); paint and the published geometry
    // read it, so painting n tabs allocates nothing per tab.
    mutable Layout layout_{};
    // The first tab the row starts at, as requested by scrolling; the layout
    // clamps it and writes the clamped value back.
    mutable std::size_t first_visible_{};
    std::uint64_t settings_revision_{};
    std::uint64_t scroll_revision_{};

    // Widths kept after a close by the pointer, one per page that had a tab
    // then; the layout uses them for those pages while `widths_frozen_`.
    std::vector<FrozenWidth> frozen_widths_{};
    bool widths_frozen_{};

    Hit hot_{};
    Hit pressed_{};
    // The page a middle press went down on, for a close on its release.
    std::optional<PageId> middle_pressed_{};
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
    Event<PagePinRequest&> page_pin_requested_{};
};

} // namespace gui_forms
