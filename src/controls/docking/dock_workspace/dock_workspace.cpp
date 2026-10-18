#include "gui_forms/controls/docking/dock_workspace/dock_workspace.hpp"

#include "gui_forms/commands/command_route/command_route.hpp"
#include "gui_forms/controls/docking/dock_site/dock_site.hpp"
#include "gui_forms/window.hpp"

#include <algorithm>
#include <array>
#include <utility>

namespace gui_forms {
namespace {

// Moves `panel` to the front of a most-recently-active history.
void bring_to_front(std::vector<DockPanelId>& history, const DockPanelId& panel) {
    const std::vector<DockPanelId>::iterator found = std::find(history.begin(), history.end(), panel);
    if (found != history.end()) history.erase(found);
    history.insert(history.begin(), panel);
}

// The events DockWorkspace adds to those of Component, in declaration order.
constexpr std::array<InteractionDeclaration, 1> workspace_interactions{{
    {.name = "activation_changed", .payload = "DockActivationChange", .mutable_payload = false},
}};

} // namespace

DockWorkspace::DockWorkspace(std::string stable_id) : stable_id_(std::move(stable_id)) {
    // Subscribed before any site, so the tracks are pruned before a site
    // realizes a change.
    layout_token_ = layout_.changed().subscribe(
        *this, Delegate<const DockLayoutChange&>::bind<DockWorkspace, &DockWorkspace::on_layout_changed>(*this));
}

DockWorkspace::~DockWorkspace() {
    layout_token_.disconnect();
    if (site_ != nullptr) (*site_).release_workspace();
    site_ = nullptr;
}

void DockWorkspace::on_dispose() noexcept {
    layout_token_.disconnect();
    if (site_ != nullptr) (*site_).release_workspace();
    site_ = nullptr;
    Component::on_dispose();
}

// -- registration --------------------------------------------------------------

DockWorkspace::PanelEntry* DockWorkspace::find_entry(const DockPanelId& panel) noexcept {
    for (PanelEntry& entry : panels_) {
        if (entry.registration.descriptor.id == panel) return &entry;
    }
    return nullptr;
}

const DockWorkspace::PanelEntry* DockWorkspace::find_entry(const DockPanelId& panel) const noexcept {
    for (const PanelEntry& entry : panels_) {
        if (entry.registration.descriptor.id == panel) return &entry;
    }
    return nullptr;
}

bool DockWorkspace::register_panel(DockPanelRegistration registration) {
    if (!layout_.register_panel(registration.descriptor)) return false;
    PanelEntry entry{};
    entry.registration = std::move(registration);
    panels_.push_back(std::move(entry));
    return true;
}

const DockPanelRegistration* DockWorkspace::find_registration(const DockPanelId& panel) const noexcept {
    const PanelEntry* const entry = find_entry(panel);
    if (entry == nullptr) return nullptr;
    return &(*entry).registration;
}

bool DockWorkspace::set_panel_title(const DockPanelId& panel, std::string title) {
    PanelEntry* const entry = find_entry(panel);
    if (entry == nullptr) return false;
    if ((*entry).registration.title == title) return true;
    (*entry).registration.title = std::move(title);
    if (site_ != nullptr) (*site_).realize();
    return true;
}

std::string DockWorkspace::panel_title(const DockPanelId& panel) const {
    const PanelEntry* const entry = find_entry(panel);
    if (entry == nullptr || (*entry).registration.title.empty()) return panel.value;
    return (*entry).registration.title;
}

// -- placement -----------------------------------------------------------------

bool DockWorkspace::placed(const DockPanelId& panel) const {
    const DockPlacementKind placement = layout_.locate(panel).placement;
    const bool result = placement == DockPlacementKind::docked || placement == DockPlacementKind::floating ||
                        placement == DockPlacementKind::auto_hidden;
    return result;
}

bool DockWorkspace::docked_in_main_site(const DockPanelId& panel) const {
    const DockPanelLocation location = layout_.locate(panel);
    const bool docked = location.placement == DockPlacementKind::docked;
    return docked;
}

DockPanelKind DockWorkspace::panel_kind(const DockPanelId& panel) const {
    const DockPanelDescriptor* const descriptor = layout_.find_descriptor(panel);
    if (descriptor == nullptr) return DockPanelKind::tool;
    return (*descriptor).kind;
}

DockTransactionResult DockWorkspace::apply(const DockTransaction& transaction) {
    const DockTransactionResult result = layout_.apply(transaction);
    return result;
}

DockTransactionResult DockWorkspace::show_panel(const DockPanelId& panel, DockEditOrigin origin) {
    DockTransactionResult result{};
    if (!placed(panel)) {
        DockTransaction transaction{};
        transaction.origin = origin;
        transaction.edits.push_back(DockEdit::show(panel));
        result = layout_.apply(transaction);
        if (!result) return result;
    } else {
        result.applied = true;
        result.generation = layout_.generation();
    }
    const DockActivationCause cause =
        origin == DockEditOrigin::user ? DockActivationCause::keyboard : DockActivationCause::programmatic;
    static_cast<void>(activate_panel(panel, cause));
    return result;
}

DockTransactionResult DockWorkspace::hide_panel(const DockPanelId& panel, DockEditOrigin origin) {
    DockTransaction transaction{};
    transaction.origin = origin;
    transaction.edits.push_back(DockEdit::hide(panel));
    const DockTransactionResult result = layout_.apply(transaction);
    return result;
}

DockTransactionResult DockWorkspace::close_panel(const DockPanelId& panel, DockEditOrigin origin) {
    DockTransaction transaction{};
    transaction.origin = origin;
    transaction.edits.push_back(DockEdit::close(panel));
    const DockTransactionResult result = layout_.apply(transaction);
    if (!result) return result;
    // The layout change already released a closed document's content; a
    // closed tool panel's goes here (its placeholder stays).
    PanelEntry* const entry = find_entry(panel);
    if (entry != nullptr) (*entry).content.reset();
    return result;
}

// -- content -------------------------------------------------------------------

Control::Ptr DockWorkspace::panel_content(const DockPanelId& panel) const {
    const PanelEntry* const entry = find_entry(panel);
    if (entry == nullptr) return {};
    return (*entry).content;
}

Control::Ptr DockWorkspace::create_panel_content(const DockPanelId& panel) {
    PanelEntry* entry = find_entry(panel);
    if (entry == nullptr) return {};
    if ((*entry).content) return (*entry).content;
    if (!(*entry).registration.factory) return {};
    DockPanelCreation creation{};
    creation.panel = panel;
    // Copied: the factory may register panels, which moves the entries.
    const Delegate<DockPanelCreation&> factory = (*entry).registration.factory;
    factory(creation);
    if (!creation.content) return {};
    entry = find_entry(panel);
    if (entry == nullptr) return {};
    (*entry).content = std::move(creation.content);
    ++created_count_;
    // The new content may carry the active panel's or document's targets.
    publish_command_targets();
    return (*entry).content;
}

// -- activation ----------------------------------------------------------------

bool DockWorkspace::activate_panel(const DockPanelId& panel, DockActivationCause cause) {
    if (find_entry(panel) == nullptr || !docked_in_main_site(panel)) return false;
    const DockPanelLocation location = layout_.locate(panel);
    const DockNode* const stack = layout_.find_node(location.stack);
    if (stack != nullptr && (*stack).selected != location.index) {
        DockTransaction transaction{};
        transaction.origin = cause == DockActivationCause::programmatic ? DockEditOrigin::program : DockEditOrigin::user;
        transaction.edits.push_back(DockEdit::select(panel));
        if (!layout_.apply(transaction)) return false;
    }
    note_active(panel, cause);
    if (site_ != nullptr) (*site_).focus_panel(panel);
    return true;
}

void DockWorkspace::set_track(DockActivationTrack track, const std::optional<DockPanelId>& panel,
                              DockActivationCause cause) {
    std::optional<DockPanelId>& held = track == DockActivationTrack::panel ? active_panel_ : active_document_;
    if (held == panel) return;
    DockActivationChange change{};
    change.track = track;
    change.old_panel = held;
    change.new_panel = panel;
    change.cause = cause;
    held = panel;
    activation_changed_.emit(change);
}

void DockWorkspace::note_active(const DockPanelId& panel, DockActivationCause cause) {
    if (find_entry(panel) == nullptr) return;
    const bool document = panel_kind(panel) == DockPanelKind::document;
    if (document) {
        bring_to_front(document_history_, panel);
    } else {
        bring_to_front(tool_history_, panel);
    }
    const std::optional<DockPanelId> chosen = panel;
    set_track(DockActivationTrack::panel, chosen, cause);
    if (document) set_track(DockActivationTrack::document, chosen, cause);
    publish_command_targets();
}

void DockWorkspace::publish_command_targets() {
    if (site_ == nullptr) return;
    Window* const host = (*site_).attached_window();
    if (host == nullptr) return;
    std::shared_ptr<CommandTarget> panel_target{};
    if (active_panel_) {
        const Control::Ptr content = panel_content(*active_panel_);
        if (content) panel_target = (*content).command_target();
    }
    std::shared_ptr<CommandTarget> document_target{};
    if (active_document_) {
        const Control::Ptr content = panel_content(*active_document_);
        if (content) document_target = (*content).command_target();
    }
    (*host).set_command_target(CommandRouteStage::active_panel, panel_target);
    (*host).set_command_target(CommandRouteStage::active_document, document_target);
}

// The layout committed a transaction: panels that left it leave the
// histories, a document that left releases its content, and an active panel
// or document that left is replaced by the next in its history (the active
// panel by the active document). The bound site realizes the change after
// this.
void DockWorkspace::on_layout_changed(const DockLayoutChange&) {
    const std::array<std::vector<DockPanelId>*, 2> histories{&tool_history_, &document_history_};
    for (std::vector<DockPanelId>* const history : histories) {
        std::size_t index = 0U;
        while (index < (*history).size()) {
            if (placed((*history)[index])) {
                ++index;
                continue;
            }
            (*history).erase((*history).begin() + static_cast<std::ptrdiff_t>(index));
        }
    }
    for (PanelEntry& entry : panels_) {
        const bool document = entry.registration.descriptor.kind == DockPanelKind::document;
        if (document && entry.content && layout_.locate(entry.registration.descriptor.id).placement ==
                                             DockPlacementKind::absent) {
            entry.content.reset();
        }
    }
    if (active_document_ && !placed(*active_document_)) {
        std::optional<DockPanelId> next{};
        if (!document_history_.empty()) next = document_history_.front();
        set_track(DockActivationTrack::document, next, DockActivationCause::fallback);
    }
    if (active_panel_ && !placed(*active_panel_)) {
        set_track(DockActivationTrack::panel, active_document_, DockActivationCause::fallback);
    }
    publish_command_targets();
}

void DockWorkspace::attach_site(DockSite& site) noexcept {
    site_ = &site;
}

void DockWorkspace::detach_site(DockSite& site) noexcept {
    if (site_ == &site) site_ = nullptr;
}

void DockWorkspace::append_interactions(std::vector<InteractionDeclaration>& interactions) const {
    Component::append_interactions(interactions);
    interactions.insert(interactions.end(), workspace_interactions.begin(), workspace_interactions.end());
}

} // namespace gui_forms
