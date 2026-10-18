#pragma once

#include "gui_forms/component/component/component.hpp"
#include "gui_forms/control.hpp"
#include "gui_forms/docking.hpp"
#include "gui_forms/event.hpp"
#include "gui_forms/event/delegate/delegate.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace gui_forms {

class DockSite;

// A factory's request: build the content of `panel` and store it in
// `content`. Left null, the panel shows an empty area and the factory is
// asked again the next time the panel is shown selected.
struct DockPanelCreation final {
    DockPanelId panel{};
    Control::Ptr content{};
};

// What the program registers for one panel: the layout's descriptor, the
// title its caption and tab show, and the factory that builds its content.
// The factory is a named delegate: its object must outlive the workspace's
// use of it (remove the panel or destroy the workspace first).
struct DockPanelRegistration final {
    DockPanelDescriptor descriptor{};
    std::string title{};
    Delegate<DockPanelCreation&> factory{};
};

// Why a panel became active.
enum class DockActivationCause : std::uint8_t {
    programmatic,
    pointer,
    keyboard,
    // Focus entered the panel (a click in its content, Tab, F6).
    focus,
    navigator,
    // The active panel or document left the layout and another took its
    // place.
    fallback,
};

// The two activation tracks (owner decision DK-29; docking study 5.6): the
// active panel is the panel that has, or last had, keyboard focus, tool or
// document; the active document is the document panel last active. A tool
// panel's activation changes only the first.
enum class DockActivationTrack : std::uint8_t { panel, document };

struct DockActivationChange final {
    DockActivationTrack track{DockActivationTrack::panel};
    std::optional<DockPanelId> old_panel{};
    std::optional<DockPanelId> new_panel{};
    DockActivationCause cause{DockActivationCause::programmatic};
};

// What opens an auto-hidden panel's flyout from its side tab (owner decision
// DK-27, E1-1): resting the pointer on the tab for DockSite's hover delay or
// clicking it (the default), or only clicking it.
enum class DockFlyoutOpening : std::uint8_t { hover_or_click, click };

// The docking workspace (owner decisions DK-3, DK-29, DK-34): a component
// owning the DockLayout, the panel registrations with their factories, the
// content each created panel holds, and the activation tracks with their
// most-recently-active histories. A DockSite realizes the layout in a
// window; the workspace names, on that window's command routing chain,
// the command targets of the active panel and the active document (the
// targets their content controls carry, Control::set_command_target), so a
// document command such as Save reaches the active document from a focused
// tool panel.
//
// Lazy creation: a panel's factory runs the first time the panel is shown
// selected in a realized stack or in a flyout, never for a panel that stays
// behind another tab, hidden, collapsed or behind a side tab without its
// flyout opening, or in a floating site (not realized until D4). The
// workspace keeps each created control until the
// panel is closed (close_panel, or a document leaving the layout); hiding
// keeps it.
//
// Edits are model transactions (apply, or the named operations below); the
// layout publishes one change per transaction, after which the workspace
// prunes its histories and moves an active panel or document that left the
// layout to the next in its history (cause fallback), and then the site
// realizes the change. One DockSite may be bound at a time.
//
// Execution: one UI thread, the site's window's; nothing is synchronized.
class DockWorkspace final : public Component {
public:
    explicit DockWorkspace(std::string stable_id);
    ~DockWorkspace() override;

    [[nodiscard]] const std::string& stable_id() const noexcept { return stable_id_; }
    // The layout. Restrictions, groups, saving and restoring go through it
    // directly; register panels through register_panel, so that their
    // titles and factories are known.
    [[nodiscard]] DockLayout& layout() noexcept { return layout_; }
    [[nodiscard]] const DockLayout& layout() const noexcept { return layout_; }

    // Registers a panel with the layout and keeps its title and factory.
    // Registering places nothing (show_panel does). False, changing
    // nothing, when the layout refuses the descriptor (an empty or repeated
    // identity).
    bool register_panel(DockPanelRegistration registration);
    [[nodiscard]] const DockPanelRegistration* find_registration(const DockPanelId& panel) const noexcept;
    // Retitles a registered panel's caption, tab and navigator row. False
    // for an unregistered panel.
    bool set_panel_title(const DockPanelId& panel, std::string title);
    // The registered title, or the identity for a panel without one.
    [[nodiscard]] std::string panel_title(const DockPanelId& panel) const;

    // Applies a transaction to the layout (DockLayout::apply).
    DockTransactionResult apply(const DockTransaction& transaction);
    // Shows a hidden or new panel where it was last (DockEdit::show) and
    // activates it. A placed panel is only activated.
    DockTransactionResult show_panel(const DockPanelId& panel, DockEditOrigin origin = DockEditOrigin::program);
    // Hides a panel, keeping its placeholder (DockEdit::hide).
    DockTransactionResult hide_panel(const DockPanelId& panel, DockEditOrigin origin = DockEditOrigin::user);
    // Closes a panel (DockEdit::close) and releases its content; a closed
    // tool panel keeps its placeholder, a closed document leaves.
    DockTransactionResult close_panel(const DockPanelId& panel, DockEditOrigin origin = DockEditOrigin::user);

    // The panel's content, or null before its factory ran (or after it was
    // released).
    [[nodiscard]] Control::Ptr panel_content(const DockPanelId& panel) const;
    // Runs the panel's factory if the panel has no content yet, and returns
    // the content (null when there is no factory or it built nothing).
    Control::Ptr create_panel_content(const DockPanelId& panel);
    // The number of factory runs that produced content, for diagnostics
    // and tests.
    [[nodiscard]] std::size_t created_panel_count() const noexcept { return created_count_; }

    [[nodiscard]] const std::optional<DockPanelId>& active_panel() const noexcept { return active_panel_; }
    [[nodiscard]] const std::optional<DockPanelId>& active_document() const noexcept { return active_document_; }
    // Most recently active first; only placed panels. Valid until the next
    // activation or layout change.
    [[nodiscard]] std::span<const DockPanelId> tool_history() const noexcept {
        const std::span<const DockPanelId> history(tool_history_);
        return history;
    }
    [[nodiscard]] std::span<const DockPanelId> document_history() const noexcept {
        const std::span<const DockPanelId> history(document_history_);
        return history;
    }
    // Makes a panel docked in the main site active: selects it in its stack,
    // creates its content, moves focus into it (through the bound site) and
    // updates the tracks. A panel behind a side tab (auto-hidden, or in a
    // stack collapsed by shrinking) opens in its flyout with focus inside;
    // an auto-hidden stack's selection is not edited. False, changing
    // nothing, for a panel that is neither, or not registered.
    bool activate_panel(const DockPanelId& panel, DockActivationCause cause = DockActivationCause::programmatic);

    // The workspace setting for opening flyouts; hover_or_click by default.
    [[nodiscard]] DockFlyoutOpening flyout_opening() const noexcept { return flyout_opening_; }
    void set_flyout_opening(DockFlyoutOpening opening) noexcept { flyout_opening_ = opening; }

    // Raised after a track changed: the panel track first, then the document
    // track when both change. Handlers may activate other panels.
    [[nodiscard]] Event<const DockActivationChange&>& activation_changed() noexcept { return activation_changed_; }

    // The bound site, or null.
    [[nodiscard]] DockSite* site() const noexcept { return site_; }

protected:
    void append_interactions(std::vector<InteractionDeclaration>& interactions) const override;
    void on_dispose() noexcept override;

private:
    friend class DockSite;

    struct PanelEntry final {
        DockPanelRegistration registration{};
        Control::Ptr content{};
    };

    [[nodiscard]] PanelEntry* find_entry(const DockPanelId& panel) noexcept;
    [[nodiscard]] const PanelEntry* find_entry(const DockPanelId& panel) const noexcept;
    [[nodiscard]] bool docked_in_main_site(const DockPanelId& panel) const;
    [[nodiscard]] bool placed(const DockPanelId& panel) const;
    [[nodiscard]] DockPanelKind panel_kind(const DockPanelId& panel) const;
    // The site binds itself on construction and unbinds on destruction.
    void attach_site(DockSite& site) noexcept;
    void detach_site(DockSite& site) noexcept;
    // Records `panel` as active without moving focus (the site calls it when
    // focus enters a panel).
    void note_active(const DockPanelId& panel, DockActivationCause cause);
    void set_track(DockActivationTrack track, const std::optional<DockPanelId>& panel, DockActivationCause cause);
    // Names the active panel's and document's command targets on the site's
    // window.
    void publish_command_targets();
    void on_layout_changed(const DockLayoutChange& change);

    std::string stable_id_;
    DockLayout layout_{};
    // One per registered panel, in registration order.
    std::vector<PanelEntry> panels_{};
    std::size_t created_count_{};
    std::optional<DockPanelId> active_panel_{};
    std::optional<DockPanelId> active_document_{};
    std::vector<DockPanelId> tool_history_{};
    std::vector<DockPanelId> document_history_{};
    // Observes the bound site; the site clears it when it is destroyed.
    DockSite* site_{};
    SubscriptionToken layout_token_{};
    DockFlyoutOpening flyout_opening_{DockFlyoutOpening::hover_or_click};
    Event<const DockActivationChange&> activation_changed_{};
};

} // namespace gui_forms
