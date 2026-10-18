#pragma once

#include "gui_forms/control.hpp"
#include "gui_forms/controls/pages/page_strip/page_strip.hpp"
#include "gui_forms/event.hpp"
#include "gui_forms/pages.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <vector>

namespace gui_forms {

// The content of one page of a PageView: the application's control, shown
// in the view's content area while its page is selected. The set owns this
// record and the record holds the control; the view makes the control a
// child of the page's panel while the page is in the view's set, so `take`
// and `put` move a page's control between views with its state, and
// removing the page releases it.
class PageControlContent final : public PageContent {
public:
    // Throws std::invalid_argument for a null control.
    explicit PageControlContent(Control::Ptr control);
    ~PageControlContent() override;

    [[nodiscard]] const Control::Ptr& control() const noexcept { return control_; }

private:
    Control::Ptr control_;
};

// What Ctrl+Tab and Ctrl+Shift+Tab do while focus is inside the view (owner
// decisions DK-30 and DK-38): select the next or previous page (a view on
// its own), or pass the key on (a view inside docking, whose navigator
// takes it). Ctrl+PageDown and Ctrl+PageUp select in every view.
enum class PageViewCtrlTab : std::uint8_t { select_next, pass };

// The presentation a view starts from (owner decision DK-15). `strip`:
// content-sized tabs, scroll and list overflow, mnemonics, no close buttons,
// selection after a close by neighbour, no live reorder. `document` (DK-31,
// DK-38, DK-39): close buttons on the selected and hot tabs, shrink then
// list overflow, no mnemonics (titles are data), live reorder by dragging,
// selection after a close by most recent activation. The navigation preset is phase P8's.
enum class PageViewPreset : std::uint8_t { strip, document };

// What a view does with a close request no handler took (owner decision
// DK-38). `remove_when_unhandled`, the default: the view removes the page.
// `request_only`, for an owner that decides (a docking workspace): the view
// never removes a page itself.
enum class PageViewClosePolicy : std::uint8_t { remove_when_unhandled, request_only };

struct PageViewOptions final {
    PageViewPreset preset{PageViewPreset::strip};
    // Pinned tabs (owner decision DK-31) are opt-in.
    bool pinning_enabled{false};
    // As PageSetOptions::capacity.
    std::size_t capacity{PageSetLimits::default_capacity};
};

// The set options a view's options give: pinning and capacity as stated, and
// the preset's selection after a close (strip: neighbour; document: most
// recent, owner decision DK-37).
[[nodiscard]] PageSetOptions page_set_options(const PageViewOptions& options) noexcept;

// A selected page has no content yet (owner decision DK-16). A handler
// supplies it with `set_page_content` (or the set's `set_content`); a page
// left without content shows an empty content area and is not asked again.
struct PageContentRequest final {
    PageId page{};
};

// A PageStrip over its own PageSet, and a content area showing the selected
// page's control (owner decisions DK-13 to DK-16), in the strip or document
// preset (see PageViewPreset). The preset sets the strip's settings once,
// when the view is made; they may be changed on the strip afterwards.
//
// Closing (owner decision DK-38): the strip raises `page_close_requested`
// for the close button, a middle click, Ctrl+F4 and the semantic close
// action; Ctrl+F4 is handled anywhere inside the view, as Ctrl+PageDown is.
// When no handler sets `handled`, the close policy decides. Removing the
// selected page selects another by the set's fallback.
//
// The view owns its set and the strip borrows it; the view releases the
// strip when it is destroyed. Each page with content is shown in a panel the
// view owns (`<view id>.panel.<page id>`), published to accessibility as the
// page's tab panel, named by its tab and linked from it; only the selected
// page's panel is visible.
//
// Event order when the selection changes (the set's events, then the
// view's work inside the first `selected_page_changed` handler, which the
// view registers before anyone else can): the set's `selecting` (a handler
// may cancel; then nothing else happens); the view remembers the control
// that had focus in the old page; `content_requested` if the new page has
// no content and was never asked; the new page's panel is shown and the old
// one hidden; focus moves into the new page (its remembered control, else
// the strip) if it was inside the old page and the selection did not come
// from a click on a tab (the strip then takes focus itself); then the other
// `selected_page_changed` handlers; then the strip's `page_invoked` when a
// tab was chosen.
class PageView : public Control {
public:
    static constexpr bool initialize_tree_after_construction = true;
    // The strip preset over a set made with `options`.
    explicit PageView(StableId stable_id, PageSetOptions options = {});
    // The preset, over a set made with the options `page_set_options` gives.
    // Throws std::invalid_argument for an invalid preset.
    PageView(StableId stable_id, PageViewOptions options);
    ~PageView() override;
    // Creates the strip. `make_control` calls it; do not call it yourself.
    void initialize_control_tree();

    [[nodiscard]] PageSet& pages() noexcept { return pages_; }
    [[nodiscard]] const PageSet& pages() const noexcept { return pages_; }
    [[nodiscard]] const std::shared_ptr<PageStrip>& strip() const noexcept { return strip_; }

    // Adds a page with `content` (or none, for late content) at `placement`,
    // as PageSet::put. The set's refusals are returned; on a refusal the
    // view is unchanged.
    [[nodiscard]] PageEditStatus add_page(const PageRecord& record, Control::Ptr content,
                                          PagePlacement placement = PagePlacement::end_of_group());
    // Attaches late content, as PageSet::set_content. Refuses a null control
    // with `unchanged`, as the set does for null content.
    [[nodiscard]] PageEditStatus set_page_content(const PageId& page, Control::Ptr content);
    // The page's control, or null when it has none or is not in the set.
    [[nodiscard]] Control::Ptr page_content(const PageId& page) const;
    // The stable identity of the page's tab panel, or empty when the page has
    // no content shown by this view.
    [[nodiscard]] std::string panel_identity(const PageId& page) const;

    [[nodiscard]] PageViewPreset preset() const noexcept { return preset_; }
    [[nodiscard]] PageViewClosePolicy close_policy() const noexcept { return close_policy_; }
    void set_close_policy(PageViewClosePolicy policy);

    [[nodiscard]] PageViewCtrlTab ctrl_tab() const noexcept { return ctrl_tab_; }
    void set_ctrl_tab(PageViewCtrlTab policy);
    // Whether the view registers itself as an F6 region of its window
    // (owner decision RB-17), off by default. A view nested in another region
    // is not registered.
    [[nodiscard]] bool focus_region() const noexcept { return focus_region_; }
    void set_focus_region(bool region);

    [[nodiscard]] Event<const PageContentRequest&>& content_requested() noexcept {
        return content_requested_;
    }

    [[nodiscard]] Size measure(Size available) override;
    void arrange(Rect final_bounds) override;
    void on_paint(Painter& painter, Rect local_damage) override;
    void on_key_bubble(KeyEvent& event) override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;
    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;

    // Stage T1 anatomy, as dot members; its states are Control's.
    struct Parts final {
        PartGeometry region;
        // The child strip's bounds.
        PartGeometry strip;
        // The selected page's content area.
        PartGeometry content;
        PartGeometry focus;
    };
    [[nodiscard]] Parts parts() const;

protected:
    void append_interactions(std::vector<InteractionDeclaration>& interactions) const override;
    void on_attachment_committed() noexcept override;
    void on_dispose() noexcept override;
    void publish_part_geometry(std::span<PartGeometry> geometry) const override;

private:
    // One page of the set, as the view shows it. `panel` exists while the
    // page has a PageControlContent; `content` is that control, a child of
    // the panel. `remembered_focus` observes the control that last had focus
    // in the page when another page was selected.
    struct PageSlot final {
        PageId page{};
        Control::Ptr panel{};
        Control::Ptr content{};
        Control::WeakPtr remembered_focus{};
        bool content_requested{};
    };
    struct Areas final {
        Rect strip{};
        Rect content{};
    };

    [[nodiscard]] Areas areas_for(Size surface) const;
    [[nodiscard]] Rect panel_bounds(Rect content) const;
    [[nodiscard]] std::size_t slot_index(const PageId& page) const;
    void reconcile_panels();
    void detach_panel(PageSlot& slot);
    void attach_panel(PageSlot& slot, const Control::Ptr& content);
    void restore_focus(const PageId& page);
    void on_pages_changed(const PageSetChange& change);
    void on_selection_changed(const PageSelectionChange& change);

    PageSet pages_;
    PageViewPreset preset_{PageViewPreset::strip};
    PageViewClosePolicy close_policy_{PageViewClosePolicy::remove_when_unhandled};
    std::shared_ptr<PageStrip> strip_{};
    // One per page of the set (reconciled on every change), in no order.
    std::vector<PageSlot> slots_{};
    PageViewCtrlTab ctrl_tab_{PageViewCtrlTab::select_next};
    bool focus_region_{};
    SubscriptionToken pages_changed_token_{};
    SubscriptionToken selection_token_{};
    Event<const PageContentRequest&> content_requested_{};
};

} // namespace gui_forms
