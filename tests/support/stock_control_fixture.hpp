#pragma once

#include "gui_forms/gui_forms.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace gui_forms::test_support {

// One window holding one instance of every stock control type that takes a
// place in the control tree, for host accessibility tests (stage A). Each
// placed control's stable id is its id in manifests/controls.json, so a
// semantic node is attributed to the stock control whose subtree it belongs
// to. The fixture uses portable controls only; a host test supplies the
// native window.

// One placed stock control.
struct StockControlEntry final {
    std::string manifest_id;
    Control::Ptr control;
};

// A manifest record the fixture does not place in the tree, and why: a
// component without a place in the control tree, or a base type covered by
// its placed subclasses.
struct StockControlOmission final {
    std::string manifest_id;
    std::string reason;
};

// Counts the fixture's effects that publish no semantic state, so a test
// sees what a default action did: clicks of every ButtonBase in the tree (also
// per stable id), command invocations, drop-down requests, task dialog
// closes and column-header clicks.
class StockEffectCounter final {
public:
    void clicked(ButtonBase& sender);
    void invoked(const CommandInvocation& invocation);
    void drop_down_requested(DropDownButton& sender);
    void dialog_closed(const TaskDialogResult& result);
    void list_column_clicked(const ListViewColumnClick& click);
    void grid_column_clicked(std::size_t column);
    [[nodiscard]] std::uint64_t total() const noexcept { return total_; }
    [[nodiscard]] std::uint64_t presses(std::string_view stable_id) const;

private:
    struct Count final {
        std::string stable_id;
        std::uint64_t presses{};
    };
    std::vector<Count> counts_;
    std::uint64_t total_{};
};

// Builds the content of the fixture's docked panels when the workspace first
// shows them. It must outlive the workspace (DockPanelRegistration).
class StockDockContent final {
public:
    void make_panel(DockPanelCreation& creation);
};

// Members are declared in dependency order: everything a placed control
// borrows (the effect counter, commands, the page set, the dock workspace
// and its content factory) comes before the root that owns the controls,
// and the subscriptions that report into the counter come last, so each
// borrower is destroyed before what it borrows.
struct StockControlFixture final {
    std::shared_ptr<StockEffectCounter> effects;
    std::vector<std::shared_ptr<Command>> commands;
    std::shared_ptr<ColorCommand> color_command;
    std::unique_ptr<PageSet> page_strip_pages;
    std::unique_ptr<StockDockContent> dock_content;
    std::unique_ptr<DockWorkspace> dock_workspace;
    std::shared_ptr<Panel> root;
    std::vector<StockControlEntry> controls;
    std::vector<StockControlOmission> omissions;
    std::vector<SubscriptionToken> subscriptions;
};

// The window size the fixture's layout is designed for, in logical units.
inline constexpr Size stock_control_fixture_size{1900.0, 1200.0};

// Builds the fixture. Throws only where a control's own setup throws.
[[nodiscard]] StockControlFixture make_stock_control_fixture();

// The entry whose control has this manifest id, or nullptr. The result
// borrows from `fixture` and is invalidated when an entry is added; a
// temporary fixture is refused.
[[nodiscard]] const StockControlEntry* find_stock_control(
    const StockControlFixture& fixture, std::string_view manifest_id);
const StockControlEntry* find_stock_control(
    StockControlFixture&& fixture, std::string_view manifest_id) = delete;

} // namespace gui_forms::test_support
