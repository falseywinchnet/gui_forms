#include "gui_forms/controls/docking/dock_site/dock_site.hpp"

#include "gui_forms/delegate.hpp"
#include "gui_forms/text.hpp"
#include "gui_forms/theme/part_painting/part_painting.hpp"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <utility>

namespace gui_forms {
namespace {

// Geometry in logical units. Seams are 3 units whatever the text scale
// (owner decision DK-41, T7-1); the caption scales with the text.
constexpr double seam_bar_thickness = 3.0;
// The seam's hit band reaches this far past the bar on each side.
constexpr double seam_hit_extension = 3.0;
// The grip's length along the bar.
constexpr double seam_grip_length = 24.0;
// The caption strip (DK-41's look studies: 21 units, a 7-unit identity key
// 7 units in, 16-unit buttons 1 unit apart, 3 units from the end).
constexpr double caption_strip_height = 21.0;
constexpr double key_side = 7.0;
constexpr double key_inset = 7.0;
constexpr double key_gap = 6.0;
constexpr double button_side = 16.0;
constexpr double button_gap = 1.0;
constexpr double button_end_inset = 3.0;
constexpr double glyph_side = 9.0;
constexpr double label_gap = 4.0;
// The active caption's accent line (DK-41, T4-3).
constexpr double accent_height = 2.0;
// The PageStrip's preferred height at text scale 1.
constexpr double strip_preferred_height = 26.0;
constexpr double extent_tolerance = 1e-9;

constexpr std::string_view document_pane_name = "Documents";

constexpr std::array<std::string_view, 6> position_names{
    "float", "dock", "dock_as_document", "auto_hide", "hide", "close",
};
constexpr std::array<std::string_view, 6> position_texts{
    "Float", "Dock", "Dock as Tabbed Document", "Auto Hide", "Hide", "Close",
};

// Virtual semantic children carry a runtime ID derived from their stable ID,
// in the high half so it never meets a control's own runtime ID.
std::uint64_t node_runtime_id(std::string_view id) noexcept {
    std::uint64_t hash = 1469598103934665603ULL;
    for (const unsigned char byte : id) {
        hash ^= byte;
        hash *= 1099511628211ULL;
    }
    const std::uint64_t result = hash | (1ULL << 63U);
    return result;
}

Rect to_window(Rect absolute, Rect local) noexcept {
    const Rect result{absolute.x + local.x, absolute.y + local.y, local.width, local.height};
    return result;
}

Rect centred(Rect outer, double width, double height) noexcept {
    const Rect inner{outer.x + (outer.width - width) * 0.5, outer.y + (outer.height - height) * 0.5, width, height};
    return inner;
}

bool contains_node(std::span<const DockNodeId> nodes, DockNodeId node) noexcept {
    for (const DockNodeId held : nodes) {
        if (held == node) return true;
    }
    return false;
}

std::size_t command_index(DockPositionCommand command) noexcept {
    const std::size_t index = static_cast<std::size_t>(command);
    return index;
}

// The order tool stacks collapse in: lower collapse priority first; on equal
// priority the stack later in tree order first.
struct CollapseCandidate final {
    DockNodeId stack{};
    std::int32_t priority{};
    std::size_t order{};
};

struct CollapsesEarlier final {
    [[nodiscard]] bool operator()(const CollapseCandidate& left, const CollapseCandidate& right) const noexcept {
        if (left.priority != right.priority) return left.priority < right.priority;
        const bool later = left.order > right.order;
        return later;
    }
};

// Whether `control` can take focus now.
bool focus_candidate(const Control& control) noexcept {
    const bool candidate = control.focusable() && control.eligible_for_input();
    return candidate;
}

// The first control in `root`'s subtree, in child order, that can take
// focus, or null.
Control::Ptr first_focusable(const Control::Ptr& root) {
    if (!root) return {};
    const std::span<const Control::Ptr> children = (*root).children();
    for (const Control::Ptr& child : children) {
        if (!child) continue;
        if (focus_candidate(*child)) return child;
        Control::Ptr nested = first_focusable(child);
        if (nested) return nested;
    }
    return {};
}

} // namespace

// -- the stack view ------------------------------------------------------------

// A stack's PageView: a `pane` region named by its selected panel ("Documents"
// for a document stack), whose caption buttons (painted by the site) are its
// semantic children.
class DockSite::StackView final : public PageView {
public:
    StackView(StableId stable_id, DockSite& site, DockNodeKind kind)
        : PageView(std::move(stable_id), options_for(kind)), site_(&site), kind_(kind) {}

    void release_site() noexcept { site_ = nullptr; }

    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override {
        SemanticDescriptor descriptor{};
        descriptor.role = SemanticRole::pane;
        if (kind_ == DockNodeKind::document_stack) {
            descriptor.name = std::string(document_pane_name);
        } else if (site_ != nullptr) {
            const std::size_t index = (*site_).slot_index_of(*this);
            if (index < (*site_).stacks_.size()) descriptor.name = (*site_).stacks_[index].title;
        }
        descriptor.exposed = true;
        descriptor.include_descendants = true;
        return descriptor;
    }

    [[nodiscard]] std::vector<SemanticNode> semantic_virtual_children() const override {
        std::vector<SemanticNode> nodes{};
        if (site_ == nullptr) return nodes;
        const std::size_t index = (*site_).slot_index_of(*this);
        if (index >= (*site_).stacks_.size()) return nodes;
        nodes = (*site_).caption_nodes((*site_).stacks_[index], index);
        return nodes;
    }

    bool on_semantic_child_action(std::string_view stable_id, SemanticAction action,
                                  std::string_view value) override {
        if (site_ != nullptr) {
            const std::size_t index = (*site_).slot_index_of(*this);
            if (index < (*site_).stacks_.size() && (*site_).caption_action(index, stable_id, action)) return true;
        }
        const bool handled = PageView::on_semantic_child_action(stable_id, action, value);
        return handled;
    }

private:
    [[nodiscard]] static PageSetOptions options_for(DockNodeKind kind) noexcept {
        PageSetOptions options{};
        options.fallback = kind == DockNodeKind::document_stack ? PageSelectionFallback::most_recent
                                                                : PageSelectionFallback::neighbour;
        return options;
    }

    DockSite* site_{};
    DockNodeKind kind_{DockNodeKind::tool_stack};
};

// -- construction --------------------------------------------------------------

DockSite::DockSite(StableId stable_id, DockWorkspace& workspace)
    : Control(std::move(stable_id)), workspace_(&workspace) {
    set_cursor(CursorKind::arrow);
    set_paint_plane(PaintPlane::control);
    const std::string prefix((*this).stable_id().value());
    menu_ = std::make_unique<ContextMenu>(prefix + ".position_menu");
    menu_invoked_ = (*menu_).item_invoked().subscribe(
        *this, Delegate<const MenuItemInvocation&>::bind<DockSite, &DockSite::on_menu_item_invoked>(*this));
    menu_changed_ = (*menu_).open_changed().subscribe(
        *this, Delegate<bool>::bind<DockSite, &DockSite::on_menu_open_changed>(*this));
    for (std::size_t index = 0; index < menu_commands_.size(); ++index) {
        menu_commands_[index] = std::make_shared<Command>(prefix + ".position." + std::string(position_names[index]),
                                                          std::string(position_texts[index]));
    }
    (*workspace_).attach_site(*this);
    layout_token_ = (*workspace_).layout().changed().subscribe(
        *this, Delegate<const DockLayoutChange&>::bind<DockSite, &DockSite::on_layout_changed>(*this));
    activation_token_ = (*workspace_).activation_changed().subscribe(
        *this, Delegate<const DockActivationChange&>::bind<DockSite, &DockSite::on_activation_changed>(*this));
}

void DockSite::initialize_control_tree() {
    realize();
}

DockSite::~DockSite() {
    focus_token_.disconnect();
    window_pointer_.disconnect();
    navigator_revoked_.disconnect();
    navigator_token_.disconnect();
    if (menu_) (*menu_).close();
    layout_token_.disconnect();
    activation_token_.disconnect();
    for (StackSlot& slot : stacks_) {
        if (slot.view) (*slot.view).release_site();
    }
    if (workspace_ != nullptr) (*workspace_).detach_site(*this);
    workspace_ = nullptr;
}

void DockSite::on_dispose() noexcept {
    close_navigator();
    close_position_menu();
    if (menu_) (*menu_).dispose();
    focus_token_.disconnect();
    layout_token_.disconnect();
    activation_token_.disconnect();
    if (workspace_ != nullptr) (*workspace_).detach_site(*this);
    workspace_ = nullptr;
    for (StackSlot& slot : stacks_) {
        if (slot.view) (*slot.view).release_site();
    }
    Control::on_dispose();
}

void DockSite::release_workspace() noexcept {
    close_navigator();
    close_position_menu();
    layout_token_.disconnect();
    activation_token_.disconnect();
    workspace_ = nullptr;
    try {
        for (StackSlot& slot : stacks_) remove_slot(slot);
        stacks_.clear();
        seams_.clear();
        collapsed_.clear();
        if (is_alive()) {
            set_focusable(false);
            invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
        }
    } catch (...) {
    }
}

void DockSite::set_keyboard_increment(double increment) {
    require_mutable();
    if (!std::isfinite(increment) || increment < 1.0 || increment > 256.0) {
        throw std::out_of_range("DockSite keyboard increment must lie in 1 to 256");
    }
    keyboard_increment_ = increment;
}

void DockSite::set_document_area_maximized(bool maximized) {
    require_mutable();
    if (maximized_ == maximized) return;
    maximized_ = maximized;
    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

// -- realization ---------------------------------------------------------------

DockSite::StackSlot* DockSite::slot_for(DockNodeId stack) noexcept {
    for (StackSlot& slot : stacks_) {
        if (slot.stack == stack) return &slot;
    }
    return nullptr;
}

const DockSite::StackSlot* DockSite::slot_for(DockNodeId stack) const noexcept {
    for (const StackSlot& slot : stacks_) {
        if (slot.stack == stack) return &slot;
    }
    return nullptr;
}

std::size_t DockSite::slot_index_of(const Control& control) const {
    for (std::size_t index = 0; index < stacks_.size(); ++index) {
        const std::shared_ptr<StackView>& view = stacks_[index].view;
        if (!view) continue;
        if ((*view).runtime_id() == control.runtime_id() || (*view).contains(control)) return index;
    }
    return stacks_.size();
}

std::optional<DockPanelId> DockSite::selected_panel(const StackSlot& slot) const {
    if (!slot.view) return std::nullopt;
    const std::optional<PageId>& selected = (*slot.view).pages().selected_page();
    if (!selected) return std::nullopt;
    const DockPanelId panel((*selected).value);
    return panel;
}

std::shared_ptr<PageView> DockSite::stack_view(DockNodeId stack) const {
    const StackSlot* const slot = slot_for(stack);
    if (slot == nullptr) return {};
    const std::shared_ptr<PageView> view = (*slot).view;
    return view;
}

std::vector<DockNodeId> DockSite::realized_stacks() const {
    std::vector<DockNodeId> order{};
    order.reserve(stacks_.size());
    for (const StackSlot& slot : stacks_) order.push_back(slot.stack);
    return order;
}

std::vector<DockNodeId> DockSite::collapsed_stacks() const {
    const std::vector<DockNodeId> collapsed = collapsed_;
    return collapsed;
}

// The stacks of the main site's tree in tree order: a split's first child
// before its second, the document area's content in place.
namespace {

void collect_main_stacks(const DockLayout& layout, DockNodeId node_id, std::vector<DockNodeId>& out) {
    const DockNode* const node = layout.find_node(node_id);
    if (node == nullptr) return;
    if ((*node).kind == DockNodeKind::split) {
        collect_main_stacks(layout, (*node).first, out);
        collect_main_stacks(layout, (*node).second, out);
        return;
    }
    if ((*node).kind == DockNodeKind::document_area) {
        if (!((*node).first == DockNodeId{})) collect_main_stacks(layout, (*node).first, out);
        return;
    }
    out.push_back(node_id);
}

} // namespace

void DockSite::remove_slot(StackSlot& slot) noexcept {
    if (!slot.view) return;
    (*slot.view).release_site();
    try {
        static_cast<void>(remove_child((*slot.view).runtime_id()));
    } catch (...) {
    }
    slot.view.reset();
}

// Brings the views in line with the main site's tree: one per stack in tree
// order, reused by node identity, each holding its stack's panels.
void DockSite::realize() {
    if (workspace_ == nullptr || !is_alive()) return;
    const DockLayout& layout = (*workspace_).layout();
    std::vector<DockNodeId> order{};
    collect_main_stacks(layout, layout.state().main_root, order);
    std::vector<StackSlot> next{};
    next.reserve(order.size());
    for (const DockNodeId stack : order) {
        StackSlot* const existing = slot_for(stack);
        if (existing != nullptr) {
            next.push_back(std::move(*existing));
            (*existing).view.reset();
            continue;
        }
        const DockNode& node = *layout.find_node(stack);
        StackSlot slot{};
        slot.stack = stack;
        slot.kind = node.kind;
        slot.view = make_control<StackView>(StableId(stack_identity(stack)), *this, node.kind);
        PageView& view = *slot.view;
        PageStrip& strip = *view.strip();
        strip.set_mnemonics_enabled(false);
        if (node.kind == DockNodeKind::tool_stack) strip.set_placement(PageStripPlacement::bottom);
        // The workspace's navigator takes Ctrl+Tab (owner decision DK-30).
        view.set_ctrl_tab(PageViewCtrlTab::pass);
        on(view.pages().selected_page_changed(), *this, &DockSite::on_view_selection, stack);
        on(view.content_requested(), *this, &DockSite::on_view_content, stack);
        on(strip.page_close_requested(), *this, &DockSite::on_view_close, stack);
        add_child(slot.view);
        next.push_back(std::move(slot));
    }
    for (StackSlot& slot : stacks_) remove_slot(slot);
    stacks_ = std::move(next);
    for (std::size_t index = 0; index < stacks_.size(); ++index) {
        StackSlot& slot = stacks_[index];
        static_cast<void>(set_child_index((*slot.view).runtime_id(), index));
        sync_pages(slot, *layout.find_node(slot.stack));
    }
    update_focus_regions();
    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

void DockSite::sync_pages(StackSlot& slot, const DockNode& node) {
    PageView& view = *slot.view;
    PageSet& pages = view.pages();
    syncing_ = true;
    {
        PageSetBatch batch(pages);
        // Pages whose panels left the stack go first, from the end.
        std::size_t position = pages.count();
        while (position > 0U) {
            --position;
            const PageId page = pages.record_at(PagePosition{position}).id;
            const DockPanelId panel(page.value);
            if (std::find(node.panels.begin(), node.panels.end(), panel) == node.panels.end()) {
                static_cast<void>(pages.remove(page));
            }
        }
        for (std::size_t index = 0; index < node.panels.size(); ++index) {
            const DockPanelId& panel = node.panels[index];
            const PageId page{panel.value};
            const std::string title = (*workspace_).panel_title(panel);
            const std::optional<PagePosition> found = pages.find(page);
            if (!found) {
                PageRecord record{};
                record.id = page;
                record.title = title;
                record.closable = node.kind == DockNodeKind::document_stack;
                static_cast<void>(
                    view.add_page(record, (*workspace_).panel_content(panel), PagePlacement::at(PagePosition{index})));
                continue;
            }
            if ((*found).value != index) static_cast<void>(pages.move(page, PagePosition{index}));
            if (pages.record_at(PagePosition{index}).title != title) static_cast<void>(pages.set_title(page, title));
        }
        if (!node.panels.empty() && node.selected < node.panels.size()) {
            const PageId selected{node.panels[node.selected].value};
            static_cast<void>(pages.select(selected, PageSelectionCause::programmatic));
        }
        batch.close();
    }
    syncing_ = false;
    const std::optional<DockPanelId> panel = selected_panel(slot);
    slot.title = panel ? (*workspace_).panel_title(*panel) : std::string{};
    // A tool stack shows its tabs only with two or more panels.
    const bool strip_shown = node.kind == DockNodeKind::document_stack || node.panels.size() >= 2U;
    PageStrip& strip = *view.strip();
    if (strip.visible() != strip_shown) {
        strip.set_visible(strip_shown);
        view.invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test);
    }
}

// The selected panel of a stack being shown gets its content, created by
// its factory the first time (lazy creation).
void DockSite::supply_content(StackSlot& slot) {
    if (workspace_ == nullptr || !slot.view || slot.collapsed) return;
    const std::optional<DockPanelId> panel = selected_panel(slot);
    if (!panel) return;
    const PageId page{(*panel).value};
    if ((*slot.view).page_content(page)) return;
    const Control::Ptr content = (*workspace_).create_panel_content(*panel);
    if (!content) return;
    static_cast<void>((*slot.view).set_page_content(page, content));
}

// The stacks are the window's F6 regions in tree order, where the site's
// regions were before (or at the end); the program's other regions keep
// their places.
void DockSite::update_focus_regions() {
    Window* const host = attached_window();
    if (host == nullptr) return;
    std::vector<Control::Ptr> current = (*host).focus_regions();
    std::vector<Control::Ptr> regions{};
    regions.reserve(current.size() + stacks_.size());
    std::optional<std::size_t> insert_at{};
    for (const Control::Ptr& region : current) {
        bool ours = false;
        for (const Control::WeakPtr& registered : registered_regions_) {
            if (registered.lock() == region) ours = true;
        }
        if (ours) {
            if (!insert_at) insert_at = regions.size();
            continue;
        }
        regions.push_back(region);
    }
    const std::size_t at = insert_at.value_or(regions.size());
    std::vector<Control::Ptr> ours{};
    ours.reserve(stacks_.size());
    for (const StackSlot& slot : stacks_) {
        if (slot.view) ours.push_back(slot.view);
    }
    regions.insert(regions.begin() + static_cast<std::ptrdiff_t>(at), ours.begin(), ours.end());
    try {
        (*host).set_focus_regions(std::move(regions));
    } catch (const std::invalid_argument&) {
        // The program made the site (or an ancestor) a region itself; its
        // stacks then stay out of the list.
        return;
    }
    registered_regions_.clear();
    for (const Control::Ptr& view : ours) registered_regions_.push_back(view);
}

namespace {

bool focus_within(const Control& site, Window* host) {
    if (host == nullptr) return false;
    const Control::Ptr focused = (*host).focused_control();
    if (!focused) return false;
    const bool inside = (*focused).runtime_id() == site.runtime_id() || site.contains(*focused);
    return inside;
}

} // namespace

void DockSite::on_layout_changed(const DockLayoutChange&) {
    const bool focus_was_inside = focus_within(*this, attached_window());
    realize();
    refocus_if_lost(focus_was_inside);
}

// Focus was in the site and left it with a stack (a panel hidden, closed,
// moved or collapsed): it goes to the active panel, else the active
// document, else the first stack shown.
void DockSite::refocus_if_lost(bool focus_was_inside) {
    if (!focus_was_inside || workspace_ == nullptr) return;
    Window* const host = attached_window();
    if (host == nullptr || focus_within(*this, host)) return;
    const std::array<std::optional<DockPanelId>, 2> candidates{(*workspace_).active_panel(),
                                                               (*workspace_).active_document()};
    for (const std::optional<DockPanelId>& candidate : candidates) {
        if (!candidate) continue;
        const DockPanelLocation location = (*workspace_).layout().locate(*candidate);
        const StackSlot* const slot = slot_for(location.stack);
        if (slot == nullptr || (*slot).collapsed) continue;
        static_cast<void>((*workspace_).activate_panel(*candidate, DockActivationCause::fallback));
        return;
    }
    for (const StackSlot& slot : stacks_) {
        if (slot.collapsed) continue;
        const std::optional<DockPanelId> panel = selected_panel(slot);
        if (!panel) continue;
        static_cast<void>((*workspace_).activate_panel(*panel, DockActivationCause::fallback));
        return;
    }
}

void DockSite::on_view_selection(DockNodeId stack, const PageSelectionChange& change) {
    if (syncing_ || workspace_ == nullptr || !change.new_page) return;
    if (change.cause == PageSelectionCause::programmatic || change.cause == PageSelectionCause::first_page ||
        change.cause == PageSelectionCause::fallback) {
        return;
    }
    // The user chose a tab: the layout records the selection.
    const DockPanelId panel((*change.new_page).value);
    DockTransaction transaction{};
    transaction.origin = DockEditOrigin::user;
    transaction.edits.push_back(DockEdit::select(panel));
    static_cast<void>((*workspace_).apply(transaction));
    // A stack that held the active panel now shows another: it becomes
    // active (focus, if it moved, says the same).
    const std::optional<DockPanelId>& active = (*workspace_).active_panel();
    if (change.old_page && active && (*active).value == (*change.old_page).value) {
        (*workspace_).note_active(panel, DockActivationCause::keyboard);
    }
    StackSlot* const slot = slot_for(stack);
    if (slot != nullptr) {
        (*slot).title = (*workspace_).panel_title(panel);
        invalidate(Dirty::paint | Dirty::semantics);
    }
}

void DockSite::on_view_content(DockNodeId stack, const PageContentRequest&) {
    if (syncing_) return;
    StackSlot* const slot = slot_for(stack);
    if (slot != nullptr) supply_content(*slot);
}

// A document tab's close (the accessibility action until the document
// preset's glyph arrives): the workspace closes the document.
void DockSite::on_view_close(DockNodeId, PageCloseRequest& request) {
    if (workspace_ == nullptr || request.handled) return;
    const DockPanelId panel(request.page.value);
    if (!position_command_enabled(panel, DockPositionCommand::close)) return;
    request.handled = true;
    static_cast<void>(run_position_command(panel, DockPositionCommand::close));
}

void DockSite::on_activation_changed(const DockActivationChange&) {
    invalidate(Dirty::paint | Dirty::semantics);
}

// Focus entered a stack: its selected panel is the active panel, and a
// control in the panel's content is remembered for its next activation.
void DockSite::on_focus_moved(const FocusedControlChange&) {
    if (workspace_ == nullptr) return;
    Window* const host = attached_window();
    if (host == nullptr) return;
    const Control::Ptr focused = (*host).focused_control();
    if (!focused) return;
    const std::size_t index = slot_index_of(*focused);
    if (index >= stacks_.size()) return;
    const std::optional<DockPanelId> panel = selected_panel(stacks_[index]);
    if (!panel) return;
    (*workspace_).note_active(*panel, DockActivationCause::focus);
    // Only a control in the panel's content is remembered (not the stack's
    // tabs), so activation returns into the panel itself.
    const Control::Ptr content = (*stacks_[index].view).page_content(PageId{(*panel).value});
    const bool in_content =
        content && ((*content).runtime_id() == (*focused).runtime_id() || (*content).contains(*focused));
    if (!in_content) return;
    bool remembered = false;
    for (RememberedFocus& entry : remembered_focus_) {
        if (!(entry.panel == *panel)) continue;
        entry.control = focused;
        remembered = true;
    }
    if (!remembered) {
        RememberedFocus entry{};
        entry.panel = *panel;
        entry.control = focused;
        remembered_focus_.push_back(std::move(entry));
    }
}

// Moves focus into a panel's content: the control that last had focus
// there, else its first focusable control, else the stack's tabs.
void DockSite::focus_panel(const DockPanelId& panel) {
    Window* const host = attached_window();
    if (host == nullptr || workspace_ == nullptr) return;
    const DockPanelLocation location = (*workspace_).layout().locate(panel);
    StackSlot* const slot = slot_for(location.stack);
    if (slot == nullptr || (*slot).collapsed) return;
    supply_content(*slot);
    const PageId page{panel.value};
    for (const RememberedFocus& entry : remembered_focus_) {
        if (!(entry.panel == panel)) continue;
        const Control::Ptr control = entry.control.lock();
        if (control && focus_candidate(*control) && (*(*slot).view).contains(*control) &&
            (*host).request_focus(control)) {
            return;
        }
    }
    const Control::Ptr content = (*(*slot).view).page_content(page);
    if (content) {
        if (focus_candidate(*content) && (*host).request_focus(content)) return;
        const Control::Ptr inner = first_focusable(content);
        if (inner && (*host).request_focus(inner)) return;
    }
    const std::shared_ptr<PageStrip>& strip = (*(*slot).view).strip();
    if (strip && focus_candidate(*strip)) static_cast<void>((*host).request_focus(strip));
}

std::optional<DockPanelId> DockSite::focused_panel() const {
    const Window* const host = attached_window();
    if (host == nullptr) return std::nullopt;
    const Control::Ptr focused = (*host).focused_control();
    if (!focused) return std::nullopt;
    const std::size_t index = slot_index_of(*focused);
    if (index >= stacks_.size()) return std::nullopt;
    const std::optional<DockPanelId> panel = selected_panel(stacks_[index]);
    return panel;
}

bool DockSite::focus_in_tool_stack() const {
    const Window* const host = attached_window();
    if (host == nullptr) return false;
    const Control::Ptr focused = (*host).focused_control();
    if (!focused) return false;
    const std::size_t index = slot_index_of(*focused);
    const bool tool = index < stacks_.size() && stacks_[index].kind == DockNodeKind::tool_stack;
    return tool;
}

// -- attachment ----------------------------------------------------------------

void DockSite::on_attachment_committed() noexcept {
    Control::on_attachment_committed();
    Window* const host = attached_window();
    if (host == nullptr) return;
    try {
        if (followed_window_ != host) {
            focus_token_.disconnect();
            focus_token_ = (*host).focused_control_changed().subscribe(
                *this, Delegate<const FocusedControlChange&>::bind<DockSite, &DockSite::on_focus_moved>(*this));
            followed_window_ = host;
        }
        update_focus_regions();
        if (workspace_ != nullptr) (*workspace_).publish_command_targets();
    } catch (...) {
    }
}

void DockSite::on_detaching_from_window(Window& former_window) noexcept {
    close_navigator();
    close_position_menu();
    focus_token_.disconnect();
    followed_window_ = nullptr;
    try {
        std::vector<Control::Ptr> regions = former_window.focus_regions();
        std::vector<Control::Ptr> kept{};
        kept.reserve(regions.size());
        for (const Control::Ptr& region : regions) {
            bool ours = false;
            for (const Control::WeakPtr& registered : registered_regions_) {
                if (registered.lock() == region) ours = true;
            }
            if (!ours) kept.push_back(region);
        }
        former_window.set_focus_regions(std::move(kept));
    } catch (...) {
    }
    registered_regions_.clear();
    // The workspace's targets leave the window with the site.
    const std::array<CommandRouteStage, 2> stages{CommandRouteStage::active_panel, CommandRouteStage::active_document};
    for (const CommandRouteStage stage : stages) {
        try {
            former_window.set_command_target(stage, {});
        } catch (...) {
        }
    }
    Control::on_detaching_from_window(former_window);
}

// -- layout --------------------------------------------------------------------

double DockSite::seam_thickness() const {
    return seam_bar_thickness;
}

double DockSite::caption_height() const {
    const double height = caption_strip_height * effective_text_scale();
    return height;
}

namespace {

// The PageStrip's height for the default font, as PageStrip::measure gives
// it.
double strip_extent(const Control& control) {
    const double scale = control.effective_text_scale();
    const ResolvedTextLayout metrics = control.resolve_text_layout_utf8("Mg", control.effective_font(FontSpec{}));
    const double height = std::max(strip_preferred_height * scale, metrics.logical_size.height + 10.0 * scale);
    return height;
}

} // namespace

Size DockSite::stack_minimum(const DockNode& node) const {
    double width = 0.0;
    double height = 0.0;
    for (const DockPanelId& panel : node.panels) {
        const DockPanelDescriptor* const descriptor = (*workspace_).layout().find_descriptor(panel);
        if (descriptor == nullptr) continue;
        width = std::max(width, (*descriptor).minimum_size.width);
        height = std::max(height, (*descriptor).minimum_size.height);
    }
    if (node.kind == DockNodeKind::tool_stack) {
        height += caption_height();
        if (node.panels.size() >= 2U) height += strip_extent(*this);
    } else {
        height += strip_extent(*this);
    }
    const Size minimum{width, height};
    return minimum;
}

DockSite::Extent DockSite::extent_of(DockNodeId node_id, std::span<const DockNodeId> collapsed) const {
    Extent extent{};
    const DockNode* const node = (*workspace_).layout().find_node(node_id);
    if (node == nullptr) return extent;
    if ((*node).kind == DockNodeKind::tool_stack) {
        if (contains_node(collapsed, node_id)) return extent;
        extent.present = true;
        extent.minimum = stack_minimum(*node);
        return extent;
    }
    if ((*node).kind == DockNodeKind::document_stack) {
        extent.present = true;
        extent.minimum = stack_minimum(*node);
        return extent;
    }
    if ((*node).kind == DockNodeKind::document_area) {
        // The document area is always there, empty or not (decision DK-19).
        extent.present = true;
        if (!((*node).first == DockNodeId{})) {
            const Extent content = extent_of((*node).first, collapsed);
            if (content.present) extent.minimum = content.minimum;
        }
        return extent;
    }
    const Extent first = extent_of((*node).first, collapsed);
    const Extent second = extent_of((*node).second, collapsed);
    if (!first.present) return second;
    if (!second.present) return first;
    extent.present = true;
    if ((*node).axis == DockAxis::horizontal) {
        extent.minimum.width = first.minimum.width + seam_thickness() + second.minimum.width;
        extent.minimum.height = std::max(first.minimum.height, second.minimum.height);
    } else {
        extent.minimum.width = std::max(first.minimum.width, second.minimum.width);
        extent.minimum.height = first.minimum.height + seam_thickness() + second.minimum.height;
    }
    return extent;
}

Size DockSite::minimum_size() const {
    if (workspace_ == nullptr) return Size{};
    const std::vector<DockNodeId> none{};
    const Extent extent = extent_of((*workspace_).layout().state().main_root, none);
    return extent.minimum;
}

// The tool stacks to collapse at `available` (decision DK-23): every tool
// stack while the document area is maximized; else, while the tree does not
// fit, the next stack in collapse order whose collapse reduces the shortfall.
std::vector<DockNodeId> DockSite::plan_collapse(Size available) const {
    std::vector<DockNodeId> collapsed{};
    std::vector<CollapseCandidate> candidates{};
    candidates.reserve(stacks_.size());
    for (std::size_t order = 0; order < stacks_.size(); ++order) {
        const StackSlot& slot = stacks_[order];
        if (slot.kind != DockNodeKind::tool_stack) continue;
        const DockNode* const node = (*workspace_).layout().find_node(slot.stack);
        if (node == nullptr) continue;
        CollapseCandidate candidate{};
        candidate.stack = slot.stack;
        candidate.order = order;
        candidate.priority = std::numeric_limits<std::int32_t>::min();
        for (const DockPanelId& panel : (*node).panels) {
            const DockPanelDescriptor* const descriptor = (*workspace_).layout().find_descriptor(panel);
            const std::int32_t priority = descriptor == nullptr ? 0 : (*descriptor).collapse_priority;
            candidate.priority = std::max(candidate.priority, priority);
        }
        candidates.push_back(candidate);
    }
    if (maximized_) {
        for (const StackSlot& slot : stacks_) {
            if (slot.kind == DockNodeKind::tool_stack) collapsed.push_back(slot.stack);
        }
        return collapsed;
    }
    const DockNodeId root = (*workspace_).layout().state().main_root;
    Extent extent = extent_of(root, collapsed);
    std::sort(candidates.begin(), candidates.end(), CollapsesEarlier{});
    for (const CollapseCandidate& candidate : candidates) {
        const bool wide = extent.minimum.width > available.width + extent_tolerance;
        const bool tall = extent.minimum.height > available.height + extent_tolerance;
        if (!wide && !tall) break;
        collapsed.push_back(candidate.stack);
        const Extent trial = extent_of(root, collapsed);
        const bool narrower = wide && trial.minimum.width < extent.minimum.width - extent_tolerance;
        const bool shorter = tall && trial.minimum.height < extent.minimum.height - extent_tolerance;
        if (!narrower && !shorter) {
            collapsed.pop_back();
            continue;
        }
        extent = trial;
    }
    return collapsed;
}

void DockSite::place(DockNodeId node_id, Rect bounds, std::span<const DockNodeId> collapsed) {
    const DockNode* const node = (*workspace_).layout().find_node(node_id);
    if (node == nullptr) return;
    if ((*node).kind == DockNodeKind::tool_stack || (*node).kind == DockNodeKind::document_stack) {
        StackSlot* const slot = slot_for(node_id);
        if (slot == nullptr) return;
        (*slot).bounds = bounds;
        if ((*node).kind == DockNodeKind::tool_stack) {
            (*slot).caption = {bounds.x, bounds.y, bounds.width, std::min(bounds.height, caption_height())};
        }
        return;
    }
    if ((*node).kind == DockNodeKind::document_area) {
        document_area_ = bounds;
        if (!((*node).first == DockNodeId{})) place((*node).first, bounds, collapsed);
        return;
    }
    const Extent first = extent_of((*node).first, collapsed);
    const Extent second = extent_of((*node).second, collapsed);
    if (!first.present) {
        place((*node).second, bounds, collapsed);
        return;
    }
    if (!second.present) {
        place((*node).first, bounds, collapsed);
        return;
    }
    const bool horizontal = (*node).axis == DockAxis::horizontal;
    const double extent = horizontal ? bounds.width : bounds.height;
    const double seam = seam_thickness();
    const double span = std::max(0.0, extent - seam);
    const double first_minimum = horizontal ? first.minimum.width : first.minimum.height;
    const double second_minimum = horizontal ? second.minimum.width : second.minimum.height;
    const DockSplitSizes sizes = solve_dock_split(span, (*node).ratio, first_minimum, second_minimum);
    SeamSlot record{};
    record.split = node_id;
    record.axis = (*node).axis;
    record.span = span;
    record.first = sizes.first;
    record.first_minimum = first_minimum;
    record.second_minimum = second_minimum;
    Rect first_bounds{};
    Rect second_bounds{};
    if (horizontal) {
        record.span_start = bounds.x;
        first_bounds = {bounds.x, bounds.y, sizes.first, bounds.height};
        record.bar = {bounds.x + sizes.first, bounds.y, seam, bounds.height};
        second_bounds = {bounds.x + sizes.first + seam, bounds.y, sizes.second, bounds.height};
        record.hit = {record.bar.x - seam_hit_extension, bounds.y, seam + seam_hit_extension * 2.0, bounds.height};
    } else {
        record.span_start = bounds.y;
        first_bounds = {bounds.x, bounds.y, bounds.width, sizes.first};
        record.bar = {bounds.x, bounds.y + sizes.first, bounds.width, seam};
        second_bounds = {bounds.x, bounds.y + sizes.first + seam, bounds.width, sizes.second};
        record.hit = {bounds.x, record.bar.y - seam_hit_extension, bounds.width, seam + seam_hit_extension * 2.0};
    }
    seams_.push_back(record);
    place((*node).first, first_bounds, collapsed);
    place((*node).second, second_bounds, collapsed);
}

Size DockSite::measure(Size available) {
    return available;
}

// Whether this pass's seams differ from the last pass's.
bool DockSite::seams_moved() const noexcept {
    if (previous_seams_.size() != seams_.size()) return true;
    for (std::size_t index = 0; index < seams_.size(); ++index) {
        const bool same = previous_seams_[index].split == seams_[index].split &&
                          previous_seams_[index].bar == seams_[index].bar;
        if (!same) return true;
    }
    return false;
}

void DockSite::arrange(Rect final_bounds) {
    arrange_self(final_bounds);
    // The last pass's seams, collapse and document area, to tell whether
    // the site's own parts moved (the views report their own changes).
    previous_seams_.swap(seams_);
    seams_.clear();
    previous_collapsed_.swap(collapsed_);
    const Rect previous_document_area = document_area_;
    document_area_ = {};
    for (StackSlot& slot : stacks_) {
        slot.bounds = {};
        slot.caption = {};
    }
    if (workspace_ == nullptr) {
        collapsed_.clear();
        return;
    }
    const bool focus_was_inside = focus_within(*this, attached_window());
    const Size surface{final_bounds.width, final_bounds.height};
    collapsed_ = plan_collapse(surface);
    const Rect whole{0.0, 0.0, surface.width, surface.height};
    place((*workspace_).layout().state().main_root, whole, collapsed_);
    for (StackSlot& slot : stacks_) {
        slot.collapsed = contains_node(collapsed_, slot.stack);
        set_child_layout_collapsed(slot.view, slot.collapsed);
        if (slot.collapsed) continue;
        Rect content = slot.bounds;
        if (slot.kind == DockNodeKind::tool_stack) {
            content.y += slot.caption.height;
            content.height = std::max(0.0, content.height - slot.caption.height);
        }
        set_child_layout(slot.view, content);
    }
    if (keyboard_seam_ >= seams_.size()) keyboard_seam_ = 0U;
    const bool moved = seams_moved() || previous_collapsed_ != collapsed_ || !(previous_document_area == document_area_);
    if (moved) invalidate(Dirty::paint | Dirty::hit_test | Dirty::semantics);
    // The site takes focus (for its seams) only while it has some.
    set_focusable(!seams_.empty());
    for (StackSlot& slot : stacks_) supply_content(slot);
    refocus_if_lost(focus_was_inside);
}

DockSite::CaptionGeometry DockSite::caption_geometry(Rect caption, bool pin) const {
    const double scale = effective_text_scale();
    CaptionGeometry placed;
    placed.caption = caption;
    placed.accent = {caption.x, caption.y, caption.width, std::min(caption.height, accent_height * scale)};
    const double key = key_side * scale;
    placed.key = {caption.x + key_inset * scale, caption.y + (caption.height - key) * 0.5, key, key};
    const double button = button_side * scale;
    const double gap = button_gap * scale;
    const double top = caption.y + (caption.height - button) * 0.5;
    const double end = caption.x + caption.width - button_end_inset * scale;
    placed.close = {end - button, top, button, button};
    double next = placed.close.x - gap;
    if (pin) {
        placed.pin = {next - button, top, button, button};
        next = placed.pin.x - gap;
    }
    placed.menu = {next - button, top, button, button};
    const double glyph = glyph_side * scale;
    placed.close_glyph = centred(placed.close, glyph, glyph);
    if (pin) placed.pin_glyph = centred(placed.pin, glyph, glyph);
    placed.menu_glyph = centred(placed.menu, glyph, glyph);
    const double label_start = placed.key.x + key + key_gap * scale;
    const double label_end = placed.menu.x - label_gap * scale;
    placed.label = {label_start, caption.y, std::max(0.0, label_end - label_start), caption.height};
    return placed;
}

// -- states --------------------------------------------------------------------

bool DockSite::focus_cue() const noexcept {
    const Window* const host = attached_window();
    const bool cue = focused_ && host != nullptr && (*host).focus_cue_visible();
    return cue;
}

bool DockSite::stack_current(const StackSlot& slot) const {
    if (workspace_ == nullptr) return false;
    const std::optional<DockPanelId>& active = (*workspace_).active_panel();
    if (!active || !slot.view) return false;
    const bool current = (*slot.view).pages().find(PageId{(*active).value}).has_value();
    return current;
}

bool DockSite::pin_shown(const StackSlot& slot) const {
    if (workspace_ == nullptr || slot.kind != DockNodeKind::tool_stack) return false;
    const std::optional<DockPanelId> panel = selected_panel(slot);
    if (!panel) return false;
    const std::optional<DockNodeId> owner = (*workspace_).layout().pin_owner(*panel);
    const bool shown = owner.has_value() && *owner == slot.stack;
    return shown;
}

AnatomyStateSet DockSite::seam_states(std::size_t seam) const {
    AnatomyStateSet states;
    if (hot_.zone == Zone::seam && hot_.index == seam) states.insert(AnatomyState::hot);
    if (seam_captured_ && drag_seam_ == seam) states.insert(AnatomyState::captured);
    if (focus_cue() && keyboard_seam_ == seam) states.insert(AnatomyState::focus_visible);
    if (!effectively_enabled()) states.insert(AnatomyState::disabled);
    return states;
}

AnatomyStateSet DockSite::stack_states(const StackSlot& slot) const {
    AnatomyStateSet states;
    if (stack_current(slot)) states.insert(AnatomyState::current);
    if (!effectively_enabled()) states.insert(AnatomyState::disabled);
    return states;
}

AnatomyStateSet DockSite::button_states(std::size_t stack, Zone zone) const {
    AnatomyStateSet states;
    if (hot_.zone == zone && hot_.index == stack) states.insert(AnatomyState::hot);
    if (pressed_.zone == zone && pressed_.index == stack) states.insert(AnatomyState::pressed);
    bool available = effectively_enabled();
    if (zone == Zone::menu && menu_open_ && menu_stack_ == stack) states.insert(AnatomyState::open);
    if (zone == Zone::pin) {
        // Docked is pinned; unpinning is auto-hide, which waits for D5.
        states.insert(AnatomyState::checked);
        available = false;
    }
    if (zone == Zone::close && stack < stacks_.size()) {
        const std::optional<DockPanelId> panel = selected_panel(stacks_[stack]);
        const bool closable = panel && (position_command_enabled(*panel, DockPositionCommand::hide) ||
                                        position_command_enabled(*panel, DockPositionCommand::close));
        available = available && closable;
    }
    if (!available) states.insert(AnatomyState::disabled);
    return states;
}

FontSpec DockSite::caption_font() const {
    const FontSpec font{FontRole::control, 10.5, 400, false, 0.12};
    return effective_font(font);
}

// -- paint ---------------------------------------------------------------------

// Paints from the last layout pass and the slots' cached titles: no
// allocation per seam or caption.
void DockSite::on_paint(Painter& painter, Rect) {
    PartPaint part;
    part.font = caption_font();
    part.family = ControlVisualRole::panel;
    part.role = PartRole::well;
    part.bounds = document_area_;
    paint_part(painter, *this, part);
    for (std::size_t seam = 0; seam < seams_.size(); ++seam) {
        const SeamParts parts = seam_parts(seam);
        part.family = ControlVisualRole::panel;
        part.states = seam_states(seam);
        part.role = PartRole::seam;
        part.bounds = parts.seam_bar.bounds;
        paint_part(painter, *this, part);
        part.role = PartRole::grip;
        part.mark = PartMark::grip_dots;
        part.bounds = parts.seam_grip.bounds;
        paint_part(painter, *this, part);
        part.mark = PartMark::none;
    }
    for (std::size_t index = 0; index < stacks_.size(); ++index) {
        const StackSlot& slot = stacks_[index];
        if (slot.collapsed || slot.kind != DockNodeKind::tool_stack || slot.caption.empty()) continue;
        const CaptionGeometry placed = caption_geometry(slot.caption, pin_shown(slot));
        const AnatomyStateSet states = stack_states(slot);
        // The caption strip and, while current, its accent line: the pale
        // tint and the line are the default theme's (T4, decision DK-41);
        // today's panel recipe draws its selected face.
        part.family = ControlVisualRole::panel;
        part.states = states;
        part.role = PartRole::face;
        part.bounds = placed.caption;
        paint_part(painter, *this, part);
        if (states.contains(AnatomyState::current)) {
            part.role = PartRole::keyline;
            part.bounds = placed.accent;
            part.accent = AccentIndex{0U};
            paint_part(painter, *this, part);
            part.accent.reset();
        }
        part.role = PartRole::chip;
        part.bounds = placed.key;
        paint_part(painter, *this, part);
        part.role = PartRole::caption;
        part.bounds = placed.label;
        part.text = slot.title;
        paint_part(painter, *this, part);
        part.text = {};
        // The buttons: flat at rest with a plate when hot (the menu-item
        // family, as the tool strip's).
        const std::array<Zone, 3> zones{Zone::menu, Zone::pin, Zone::close};
        const std::array<Rect, 3> buttons{placed.menu, placed.pin, placed.close};
        const std::array<Rect, 3> glyphs{placed.menu_glyph, placed.pin_glyph, placed.close_glyph};
        const std::array<PartMark, 3> marks{PartMark::chevron_down, PartMark::pin, PartMark::close};
        part.family = ControlVisualRole::menu_item;
        for (std::size_t button = 0; button < zones.size(); ++button) {
            if (buttons[button].empty()) continue;
            part.states = button_states(index, zones[button]);
            part.role = PartRole::actuator;
            part.bounds = buttons[button];
            paint_part(painter, *this, part);
            part.role = PartRole::glyph;
            part.bounds = glyphs[button];
            part.mark = marks[button];
            paint_part(painter, *this, part);
            part.mark = PartMark::none;
        }
    }
}

// -- input ---------------------------------------------------------------------

DockSite::Hit DockSite::hit_at(Point local) const {
    Hit hit;
    for (std::size_t seam = 0; seam < seams_.size(); ++seam) {
        if (!seams_[seam].hit.contains(local)) continue;
        hit.zone = Zone::seam;
        hit.index = seam;
        return hit;
    }
    for (std::size_t index = 0; index < stacks_.size(); ++index) {
        const StackSlot& slot = stacks_[index];
        if (slot.collapsed || slot.kind != DockNodeKind::tool_stack || !slot.caption.contains(local)) continue;
        const CaptionGeometry placed = caption_geometry(slot.caption, pin_shown(slot));
        hit.index = index;
        hit.zone = Zone::caption;
        if (placed.menu.contains(local)) hit.zone = Zone::menu;
        if (placed.pin.contains(local)) hit.zone = Zone::pin;
        if (placed.close.contains(local)) hit.zone = Zone::close;
        return hit;
    }
    return hit;
}

void DockSite::set_hot(Hit hit) {
    if (hot_ == hit) return;
    hot_ = hit;
    CursorKind cursor = CursorKind::arrow;
    if (hit.zone == Zone::seam && hit.index < seams_.size()) {
        cursor = seams_[hit.index].axis == DockAxis::horizontal ? CursorKind::resize_horizontal
                                                                : CursorKind::resize_vertical;
    }
    set_cursor(cursor);
    invalidate(Dirty::paint);
}

void DockSite::begin_seam_drag(std::size_t seam, Point local) {
    const SeamSlot& slot = seams_[seam];
    const double along = slot.axis == DockAxis::horizontal ? local.x : local.y;
    drag_offset_ = along - (slot.span_start + slot.first);
    drag_seam_ = seam;
    seam_captured_ = true;
    keyboard_seam_ = seam;
    invalidate(Dirty::paint);
}

void DockSite::continue_seam_drag(Point local) {
    if (drag_seam_ >= seams_.size()) return;
    const SeamSlot& slot = seams_[drag_seam_];
    const double along = slot.axis == DockAxis::horizontal ? local.x : local.y;
    const double first = along - drag_offset_ - slot.span_start;
    static_cast<void>(set_seam_first(drag_seam_, first));
}

// Sets the first child's extent of a seam's split, clamped to both
// minimums, as one user transaction recording the ratio.
bool DockSite::set_seam_first(std::size_t seam, double first) {
    if (workspace_ == nullptr || seam >= seams_.size() || !std::isfinite(first)) return false;
    SeamSlot& slot = seams_[seam];
    if (slot.span <= 0.0) return false;
    const DockSplitSizes sizes =
        solve_dock_split(slot.span, first / slot.span, slot.first_minimum, slot.second_minimum);
    const double clamped = sizes.first;
    if (std::abs(clamped - slot.first) < 1e-6) return false;
    // The ratio lies strictly inside (0, 1); a child at zero keeps a
    // vanishing share.
    const double smallest = 1e-6;
    const double ratio = std::clamp(clamped / slot.span, smallest, 1.0 - smallest);
    DockTransaction transaction{};
    transaction.origin = DockEditOrigin::user;
    transaction.edits.push_back(DockEdit::set_ratio(slot.split, ratio));
    const DockTransactionResult result = (*workspace_).apply(transaction);
    if (!result) return false;
    slot.first = clamped;
    return true;
}

bool DockSite::move_seam(std::size_t seam, double delta) {
    if (seam >= seams_.size()) return false;
    const double first = seams_[seam].first + delta;
    const bool moved = set_seam_first(seam, first);
    return moved;
}

bool DockSite::focus_seam(std::size_t seam) {
    require_mutable();
    Window* const host = attached_window();
    if (host == nullptr || seam >= seams_.size()) return false;
    keyboard_seam_ = seam;
    invalidate(Dirty::paint | Dirty::semantics);
    const bool focused = (*host).request_focus(shared_from_this());
    return focused;
}

std::optional<std::size_t> DockSite::focused_seam() const noexcept {
    if (!focused_ || seams_.empty()) return std::nullopt;
    const std::optional<std::size_t> seam = keyboard_seam_;
    return seam;
}

// A press in a seam's hit band, which reaches past the bar into the stacks
// on both sides, starts dragging the seam (the site captures the pointer).
// A press anywhere else in a stack's content activates its panel before the
// pressed control takes focus, so a click on an unfocusable part of a panel
// still activates it. Presses on a stack's tabs are left to the strip.
void DockSite::on_pointer_preview(PointerEvent& event) {
    if (workspace_ == nullptr || event.action != PointerAction::down || event.button != PointerButton::primary) {
        return;
    }
    const Rect absolute = absolute_bounds();
    const Point local{event.position.x - absolute.x, event.position.y - absolute.y};
    const Hit hit = hit_at(local);
    Window* const host = attached_window();
    if (hit.zone == Zone::seam && host != nullptr) {
        const std::uint64_t pointer = event.pointer_id == 0U ? 1U : event.pointer_id;
        (*host).capture_pointer(shared_from_this(), pointer);
        begin_seam_drag(hit.index, local);
        event.handled = true;
        return;
    }
    for (const StackSlot& slot : stacks_) {
        if (slot.collapsed || !slot.view) continue;
        if (!(*slot.view).absolute_bounds().contains(event.position)) continue;
        const std::shared_ptr<PageStrip>& strip = (*slot.view).strip();
        if (strip && (*strip).visible() && (*strip).absolute_bounds().contains(event.position)) return;
        const std::optional<DockPanelId> panel = selected_panel(slot);
        const std::optional<DockPanelId>& active = (*workspace_).active_panel();
        if (panel && !(active && *active == *panel)) {
            static_cast<void>((*workspace_).activate_panel(*panel, DockActivationCause::pointer));
        }
        return;
    }
}

void DockSite::on_pointer(PointerEvent& event) {
    if (event.action == PointerAction::leave) {
        if (!seam_captured_) set_hot({});
        return;
    }
    const Rect absolute = absolute_bounds();
    const Point local{event.position.x - absolute.x, event.position.y - absolute.y};
    if (event.action == PointerAction::move || event.action == PointerAction::enter) {
        if (seam_captured_) {
            continue_seam_drag(local);
            event.handled = true;
            return;
        }
        set_hot(hit_at(local));
        return;
    }
    if (event.button != PointerButton::primary || workspace_ == nullptr) return;
    if (event.action == PointerAction::up) {
        if (seam_captured_) {
            seam_captured_ = false;
            invalidate(Dirty::paint);
            event.handled = true;
            return;
        }
        const Hit released = hit_at(local);
        const Hit pressed = pressed_;
        pressed_ = {};
        invalidate(Dirty::paint);
        if (pressed.zone == Zone::none) return;
        event.handled = true;
        if (pressed != released || pressed.zone != Zone::close || pressed.index >= stacks_.size()) return;
        const std::optional<DockPanelId> panel = selected_panel(stacks_[pressed.index]);
        if (!panel) return;
        // A caption's close hides a tool panel (its placeholder stays), or
        // closes one that may not be hidden.
        const DockPositionCommand command = position_command_enabled(*panel, DockPositionCommand::hide)
                                                ? DockPositionCommand::hide
                                                : DockPositionCommand::close;
        static_cast<void>(run_position_command(*panel, command));
        return;
    }
    if (event.action != PointerAction::down) return;
    const Hit hit = hit_at(local);
    if (hit.zone == Zone::none) return;
    const Control::Ptr keep = shared_from_this();
    event.handled = true;
    if (hit.zone == Zone::seam) {
        begin_seam_drag(hit.index, local);
        return;
    }
    const std::optional<DockPanelId> panel = selected_panel(stacks_[hit.index]);
    if (!panel) return;
    if (hit.zone == Zone::caption) {
        static_cast<void>((*workspace_).activate_panel(*panel, DockActivationCause::pointer));
        return;
    }
    if (hit.zone == Zone::menu) {
        if (menu_open_ && menu_stack_ == hit.index) {
            close_position_menu();
            return;
        }
        static_cast<void>((*workspace_).activate_panel(*panel, DockActivationCause::pointer));
        static_cast<void>(show_position_menu(*panel, MenuOpenMode::pointer));
        return;
    }
    // An unavailable button (the pin until D5) takes no press.
    if (button_states(hit.index, hit.zone).contains(AnatomyState::disabled)) return;
    pressed_ = hit;
    invalidate(Dirty::paint);
}

void DockSite::on_focus_changed(bool focused) {
    focused_ = focused;
    invalidate(Dirty::paint | Dirty::semantics);
}

// While the site itself has focus, the keyboard is on a seam: Tab and
// Shift+Tab step between seams (leaving the site past either end), the
// arrows along the split's axis move the seam.
bool DockSite::handle_seam_key(KeyEvent& event) {
    if (!focused_ || seams_.empty() || event.action != KeyAction::down) return false;
    if (keyboard_seam_ >= seams_.size()) keyboard_seam_ = 0U;
    const bool plain = event.modifiers == Modifier::none;
    if (event.physical_key == PhysicalKey::tab && (plain || event.modifiers == Modifier::shift)) {
        if (plain && keyboard_seam_ + 1U < seams_.size()) {
            ++keyboard_seam_;
        } else if (!plain && keyboard_seam_ > 0U) {
            --keyboard_seam_;
        } else {
            return false;
        }
        invalidate(Dirty::paint | Dirty::semantics);
        event.handled = true;
        return true;
    }
    if (!plain) return false;
    const bool horizontal = seams_[keyboard_seam_].axis == DockAxis::horizontal;
    double delta = 0.0;
    if (horizontal && event.physical_key == PhysicalKey::left) delta = -keyboard_increment_;
    if (horizontal && event.physical_key == PhysicalKey::right) delta = keyboard_increment_;
    if (!horizontal && event.physical_key == PhysicalKey::up) delta = -keyboard_increment_;
    if (!horizontal && event.physical_key == PhysicalKey::down) delta = keyboard_increment_;
    if (delta == 0.0) return false;
    static_cast<void>(move_seam(keyboard_seam_, delta));
    event.handled = true;
    return true;
}

// The docking keys (owner decision DK-30), wherever focus is in the site.
bool DockSite::handle_docking_key(KeyEvent& event) {
    if (event.handled || event.action != KeyAction::down || workspace_ == nullptr) return false;
    const std::uint32_t key = event.physical_key;
    const Modifier modifiers = event.modifiers;
    const bool shift = has_modifier(modifiers, Modifier::shift);
    const Modifier without_shift = shift ? static_cast<Modifier>(static_cast<std::uint8_t>(modifiers) &
                                                                 ~static_cast<std::uint8_t>(Modifier::shift))
                                         : modifiers;
    const int direction = shift ? -1 : 1;
    bool handled = false;
    if (key == PhysicalKey::tab && without_shift == Modifier::control) {
        handled = open_navigator(DockNavigatorList::documents, direction, Modifier::control);
    } else if (key == PhysicalKey::f7 && without_shift == Modifier::alt) {
        handled = open_navigator(DockNavigatorList::tools, direction, Modifier::alt);
    } else if (key == PhysicalKey::f6 && without_shift == Modifier::control) {
        handled = activate_relative_document(direction);
    } else if (key == PhysicalKey::f12 && modifiers == (Modifier::control | Modifier::shift)) {
        set_document_area_maximized(!maximized_);
        handled = true;
    } else if (key == PhysicalKey::escape && modifiers == Modifier::none) {
        const std::optional<DockPanelId>& document = (*workspace_).active_document();
        if (focus_in_tool_stack() && document) {
            handled = (*workspace_).activate_panel(*document, DockActivationCause::keyboard);
        }
    } else if (key == PhysicalKey::escape && modifiers == Modifier::shift) {
        const std::optional<DockPanelId> panel = focused_panel();
        if (focus_in_tool_stack() && panel) handled = run_position_command(*panel, DockPositionCommand::hide);
    } else {
        const bool menu_key = (key == PhysicalKey::minus && modifiers == Modifier::alt) ||
                              (key == PhysicalKey::f10 && modifiers == Modifier::shift) ||
                              (key == PhysicalKey::application && modifiers == Modifier::none);
        if (menu_key) {
            std::optional<DockPanelId> panel = focused_panel();
            if (!panel) panel = (*workspace_).active_panel();
            if (panel) handled = show_position_menu(*panel, MenuOpenMode::keyboard);
        }
    }
    if (handled) event.handled = true;
    return handled;
}

void DockSite::on_key(KeyEvent& event) {
    if (handle_seam_key(event)) return;
    static_cast<void>(handle_docking_key(event));
}

void DockSite::on_key_bubble(KeyEvent& event) {
    static_cast<void>(handle_docking_key(event));
}

bool DockSite::activate_relative_document(int direction) {
    if (workspace_ == nullptr) return false;
    std::vector<DockPanelId> documents{};
    for (const StackSlot& slot : stacks_) {
        if (slot.kind != DockNodeKind::document_stack || slot.collapsed) continue;
        const DockNode* const node = (*workspace_).layout().find_node(slot.stack);
        if (node == nullptr) continue;
        documents.insert(documents.end(), (*node).panels.begin(), (*node).panels.end());
    }
    if (documents.empty()) return false;
    const std::size_t count = documents.size();
    std::size_t next = direction >= 0 ? 0U : count - 1U;
    const std::optional<DockPanelId>& active = (*workspace_).active_document();
    if (active) {
        const std::vector<DockPanelId>::const_iterator found = std::find(documents.begin(), documents.end(), *active);
        if (found != documents.end()) {
            const std::size_t at = static_cast<std::size_t>(found - documents.begin());
            next = direction >= 0 ? (at + 1U) % count : (at + count - 1U) % count;
        }
    }
    const bool activated = (*workspace_).activate_panel(documents[next], DockActivationCause::keyboard);
    return activated;
}

// -- the position menu ---------------------------------------------------------

bool DockSite::position_command_enabled(const DockPanelId& panel, DockPositionCommand command) const {
    if (workspace_ == nullptr) return false;
    const DockLayout& layout = (*workspace_).layout();
    const DockPanelDescriptor* const descriptor = layout.find_descriptor(panel);
    const DockPanelLocation location = layout.locate(panel);
    if (descriptor == nullptr || location.placement != DockPlacementKind::docked) return false;
    const DockNode* const stack = layout.find_node(location.stack);
    const bool in_documents = stack != nullptr && (*stack).kind == DockNodeKind::document_stack;
    const bool tool = (*descriptor).kind == DockPanelKind::tool;
    bool enabled = false;
    switch (command) {
    case DockPositionCommand::float_panel:
    case DockPositionCommand::auto_hide:
        // Floating windows are phase D4 and auto-hide phase D5.
        enabled = false;
        break;
    case DockPositionCommand::dock:
        enabled = tool && in_documents && (*descriptor).allowed.docked;
        break;
    case DockPositionCommand::dock_as_document:
        enabled = tool && !in_documents && (*descriptor).dock_as_document && (*descriptor).allowed.document_area;
        break;
    case DockPositionCommand::hide:
        enabled = tool && (*descriptor).allowed.hidden;
        break;
    case DockPositionCommand::close:
        enabled = (*descriptor).allowed.closed;
        break;
    }
    return enabled;
}

bool DockSite::run_position_command(const DockPanelId& panel, DockPositionCommand command) {
    if (!position_command_enabled(panel, command)) return false;
    const DockPanelDescriptor& descriptor = *(*workspace_).layout().find_descriptor(panel);
    DockTransactionResult result{};
    if (command == DockPositionCommand::hide) {
        result = (*workspace_).hide_panel(panel, DockEditOrigin::user);
    } else if (command == DockPositionCommand::close) {
        result = (*workspace_).close_panel(panel, DockEditOrigin::user);
    } else {
        DockTransaction transaction{};
        transaction.origin = DockEditOrigin::user;
        const DockSubject subject = DockSubject::of_panel(panel);
        if (command == DockPositionCommand::dock) {
            const DockDefaultPlacement& placement = descriptor.default_placement;
            transaction.edits.push_back(DockEdit::dock_at_edge(subject, placement.side, placement.ratio));
        } else {
            transaction.edits.push_back(DockEdit::dock_into(subject, (*workspace_).layout().document_area()));
        }
        result = (*workspace_).apply(transaction);
        // The panel moved: it stays the active panel in its new place.
        if (result) static_cast<void>((*workspace_).activate_panel(panel, DockActivationCause::keyboard));
    }
    const bool applied = result.applied;
    return applied;
}

bool DockSite::show_position_menu(const DockPanelId& panel, MenuOpenMode mode) {
    require_mutable();
    if (mode != MenuOpenMode::keyboard && mode != MenuOpenMode::pointer) {
        throw std::invalid_argument("DockSite menu opening mode is invalid");
    }
    if (workspace_ == nullptr || attached_window() == nullptr) return false;
    const DockPanelLocation location = (*workspace_).layout().locate(panel);
    if (location.placement != DockPlacementKind::docked) return false;
    std::size_t index = stacks_.size();
    for (std::size_t candidate = 0; candidate < stacks_.size(); ++candidate) {
        if (stacks_[candidate].stack == location.stack) index = candidate;
    }
    if (index == stacks_.size() || stacks_[index].collapsed) return false;
    const StackSlot& slot = stacks_[index];
    const bool tool_panel = (*(*workspace_).layout().find_descriptor(panel)).kind == DockPanelKind::tool;
    for (std::size_t command = 0; command < menu_commands_.size(); ++command) {
        const bool enabled = position_command_enabled(panel, static_cast<DockPositionCommand>(command));
        (*menu_commands_[command]).set_enabled(enabled);
    }
    // Dock is checked while the panel stands in a tool stack.
    (*menu_commands_[command_index(DockPositionCommand::dock)]).set_checked(slot.kind == DockNodeKind::tool_stack);
    const std::string prefix(stable_id().value());
    std::vector<MenuItemSpec> items{};
    const std::array<DockPositionCommand, 4> placing{DockPositionCommand::float_panel, DockPositionCommand::dock,
                                                     DockPositionCommand::dock_as_document,
                                                     DockPositionCommand::auto_hide};
    for (const DockPositionCommand command : placing) {
        if (!tool_panel && command != DockPositionCommand::float_panel) continue;
        const std::size_t at = command_index(command);
        const MenuItemKind kind = command == DockPositionCommand::dock ? MenuItemKind::check : MenuItemKind::command;
        items.emplace_back(prefix + ".position." + std::string(position_names[at]), kind, menu_commands_[at],
                           std::string(position_texts[at]));
    }
    items.emplace_back(prefix + ".position.separator", MenuItemKind::separator);
    if (tool_panel) {
        const std::size_t hide = command_index(DockPositionCommand::hide);
        items.emplace_back(prefix + ".position.hide", MenuItemKind::command, menu_commands_[hide],
                           std::string(position_texts[hide]));
    }
    const std::size_t close = command_index(DockPositionCommand::close);
    items.emplace_back(prefix + ".position.close", MenuItemKind::command, menu_commands_[close],
                       std::string(position_texts[close]));
    (*menu_).set_items(std::move(items));
    const Rect absolute = absolute_bounds();
    Point anchor{absolute.x + slot.bounds.x, absolute.y + slot.bounds.y};
    if (slot.kind == DockNodeKind::tool_stack) {
        const CaptionGeometry placed = caption_geometry(slot.caption, pin_shown(slot));
        anchor = {absolute.x + placed.menu.x, absolute.y + placed.menu.y + placed.menu.height};
    } else {
        anchor.y += strip_extent(*this);
    }
    menu_panel_ = panel;
    menu_stack_ = index;
    (*menu_).show(shared_from_this(), anchor, mode);
    invalidate(Dirty::paint | Dirty::semantics);
    return true;
}

void DockSite::close_position_menu() noexcept {
    if (menu_) (*menu_).close();
}

bool DockSite::position_menu_open() const noexcept {
    const bool open = menu_ && (*menu_).is_open();
    return open;
}

void DockSite::on_menu_open_changed(bool open) {
    menu_open_ = open;
    invalidate(Dirty::paint | Dirty::semantics);
}

void DockSite::on_menu_item_invoked(const MenuItemInvocation& invocation) {
    const std::string prefix = std::string(stable_id().value()) + ".position.";
    for (std::size_t command = 0; command < position_names.size(); ++command) {
        if (invocation.item_id != prefix + std::string(position_names[command])) continue;
        const DockPanelId panel = menu_panel_;
        static_cast<void>(run_position_command(panel, static_cast<DockPositionCommand>(command)));
        return;
    }
}

// -- the navigator -------------------------------------------------------------

std::shared_ptr<DockNavigator> DockSite::navigator() const noexcept {
    if (!navigator_open_) return {};
    return navigator_;
}

bool DockSite::open_navigator(DockNavigatorList list, int direction, Modifier release) {
    require_mutable();
    Window* const host = attached_window();
    if (host == nullptr || workspace_ == nullptr) return false;
    if (navigator_open_) return true;
    close_position_menu();
    // Each list: the panels in most-recently-active order, then the others
    // shown in tree order; panels in collapsed stacks are left out.
    std::vector<DockNavigatorEntry> tools{};
    std::vector<DockNavigatorEntry> documents{};
    std::vector<DockPanelId> shown{};
    for (const StackSlot& slot : stacks_) {
        if (slot.collapsed) continue;
        const DockNode* const node = (*workspace_).layout().find_node(slot.stack);
        if (node != nullptr) shown.insert(shown.end(), (*node).panels.begin(), (*node).panels.end());
    }
    const std::array<std::span<const DockPanelId>, 2> histories{(*workspace_).tool_history(),
                                                                (*workspace_).document_history()};
    for (std::size_t kind = 0; kind < histories.size(); ++kind) {
        std::vector<DockNavigatorEntry>& entries = kind == 0U ? tools : documents;
        const DockPanelKind wanted = kind == 0U ? DockPanelKind::tool : DockPanelKind::document;
        for (const DockPanelId& panel : histories[kind]) {
            if (std::find(shown.begin(), shown.end(), panel) == shown.end()) continue;
            entries.push_back(DockNavigatorEntry{panel, (*workspace_).panel_title(panel)});
        }
        for (const DockPanelId& panel : shown) {
            const DockPanelDescriptor* const descriptor = (*workspace_).layout().find_descriptor(panel);
            if (descriptor == nullptr || (*descriptor).kind != wanted) continue;
            bool listed = false;
            for (const DockNavigatorEntry& entry : entries) {
                if (entry.panel == panel) listed = true;
            }
            if (!listed) entries.push_back(DockNavigatorEntry{panel, (*workspace_).panel_title(panel)});
        }
    }
    if (tools.empty() && documents.empty()) return false;
    const std::string identity = std::string(stable_id().value()) + ".navigator";
    navigator_ = make_control<DockNavigator>(StableId(identity));
    (*navigator_).set_entries(std::move(tools), std::move(documents), list);
    // The row after the active one (the second most recent), or the last.
    const DockNavigatorList shown_list = (*navigator_).current_list();
    const std::size_t count = (*navigator_).entries(shown_list).size();
    const std::size_t row = direction >= 0 ? std::min<std::size_t>(1U, count - 1U) : count - 1U;
    static_cast<void>((*navigator_).select(shown_list, row));
    (*navigator_).set_release_modifier(release);
    const Size wanted = (*navigator_).measure(Size{absolute_bounds().width, absolute_bounds().height});
    const Rect absolute = absolute_bounds();
    const Rect placed = centred(absolute, wanted.width, wanted.height);
    (*navigator_).set_requested_bounds(placed);
    navigator_chosen_ = (*navigator_).chosen().subscribe(
        *this, Delegate<const DockNavigatorChoice&>::bind<DockSite, &DockSite::on_navigator_chosen>(*this));
    navigator_cancelled_ =
        (*navigator_).cancelled().subscribe(*this, Delegate<>::bind<DockSite, &DockSite::on_navigator_cancelled>(*this));
    navigator_token_ = (*host).open_popup(shared_from_this(), navigator_);
    if (Event<>* const closed = navigator_token_.closed_event()) {
        navigator_revoked_ =
            (*closed).subscribe(*this, Delegate<>::bind<DockSite, &DockSite::on_navigator_revoked>(*this));
    }
    if (const Control::Ptr root = (*host).root()) {
        window_pointer_ = (*root).pointer_preview_observed().subscribe(
            *this, Delegate<const PointerEvent&>::bind<DockSite, &DockSite::on_window_pointer>(*this));
    }
    FocusScopeOptions options{};
    options.contain_focus = true;
    navigator_scope_ = (*host).begin_focus_scope(navigator_, navigator_, options);
    static_cast<void>((*host).request_focus(navigator_));
    navigator_open_ = true;
    return true;
}

// Closing order: focus returns (the scope restores it), the window drops the
// popup, then the site forgets it.
void DockSite::close_navigator() noexcept {
    if (!navigator_open_ || navigator_closing_) return;
    navigator_closing_ = true;
    Window* const host = attached_window();
    if (host != nullptr && navigator_scope_) {
        try {
            static_cast<void>((*host).end_focus_scope(navigator_scope_));
        } catch (...) {
        }
    }
    navigator_scope_ = {};
    window_pointer_.disconnect();
    navigator_revoked_.disconnect();
    navigator_chosen_.disconnect();
    navigator_cancelled_.disconnect();
    navigator_token_.disconnect();
    navigator_.reset();
    navigator_open_ = false;
    navigator_closing_ = false;
}

void DockSite::on_navigator_revoked() {
    if (!navigator_open_ || navigator_closing_) return;
    Window* const host = attached_window();
    if (host != nullptr && navigator_scope_) {
        try {
            static_cast<void>((*host).end_focus_scope(navigator_scope_, FocusScopeCloseReason::owner_unavailable));
        } catch (...) {
        }
    }
    navigator_scope_ = {};
    close_navigator();
}

void DockSite::on_navigator_chosen(const DockNavigatorChoice& choice) {
    const DockPanelId panel = choice.panel;
    close_navigator();
    if (workspace_ != nullptr) static_cast<void>((*workspace_).activate_panel(panel, DockActivationCause::navigator));
}

void DockSite::on_navigator_cancelled() {
    close_navigator();
}

// A press outside the navigator closes it and passes through.
void DockSite::on_window_pointer(const PointerEvent& event) {
    if (!navigator_open_ || event.action != PointerAction::down) return;
    close_navigator();
}

// -- semantics -----------------------------------------------------------------

std::string DockSite::seam_identity(DockNodeId split) const {
    const std::string identity = std::string(stable_id().value()) + ".seam." + std::to_string(split.value);
    return identity;
}

std::string DockSite::stack_identity(DockNodeId stack) const {
    const std::string identity = std::string(stable_id().value()) + ".stack." + std::to_string(stack.value);
    return identity;
}

SemanticDescriptor DockSite::semantic_descriptor() const {
    SemanticDescriptor descriptor{};
    descriptor.role = SemanticRole::group;
    descriptor.name = accessible_name().empty() ? "Docking workspace" : accessible_name();
    descriptor.description = accessible_description();
    descriptor.actions = {SemanticAction::focus};
    descriptor.exposed = true;
    return descriptor;
}

// Each seam is a split pane whose value is the first side's extent, between
// the two sides' minimums.
std::vector<SemanticNode> DockSite::semantic_virtual_children() const {
    std::vector<SemanticNode> nodes{};
    nodes.reserve(seams_.size());
    const Rect absolute = absolute_bounds();
    const bool enabled = effectively_enabled();
    for (std::size_t seam = 0; seam < seams_.size(); ++seam) {
        const SeamSlot& slot = seams_[seam];
        SemanticNode node{};
        node.stable_id = seam_identity(slot.split);
        node.runtime_id = node_runtime_id(node.stable_id);
        node.role = SemanticRole::split_pane;
        node.name = slot.axis == DockAxis::horizontal ? "Vertical seam" : "Horizontal seam";
        node.numeric_value = slot.first;
        node.minimum_value = slot.first_minimum;
        node.maximum_value = std::max(slot.first_minimum, slot.span - slot.second_minimum);
        node.value = std::to_string(static_cast<long long>(std::lround(slot.first)));
        node.bounds = to_window(absolute, slot.bar);
        node.states = SemanticState::visible | SemanticState::focusable;
        if (enabled) node.states |= SemanticState::enabled;
        if (focused_ && keyboard_seam_ == seam) node.states |= SemanticState::focused;
        node.actions = {SemanticAction::focus, SemanticAction::increment, SemanticAction::decrement,
                        SemanticAction::set_value};
        nodes.push_back(std::move(node));
    }
    return nodes;
}

bool DockSite::on_semantic_child_action(std::string_view stable_id, SemanticAction action, std::string_view value) {
    for (std::size_t seam = 0; seam < seams_.size(); ++seam) {
        if (seam_identity(seams_[seam].split) != stable_id) continue;
        if (action == SemanticAction::focus) return focus_seam(seam);
        if (action == SemanticAction::increment) return move_seam(seam, keyboard_increment_);
        if (action == SemanticAction::decrement) return move_seam(seam, -keyboard_increment_);
        if (action != SemanticAction::set_value) return false;
        double requested = 0.0;
        const char* const begin = value.data();
        const char* const end = value.data() + value.size();
        const std::from_chars_result parsed = std::from_chars(begin, end, requested);
        if (parsed.ec != std::errc{} || parsed.ptr != end) return false;
        const bool set = set_seam_first(seam, requested);
        return set;
    }
    return false;
}

// A tool stack's caption buttons, as children of its pane: the position
// menu, the pin (checked while docked; unavailable until D5) and close.
std::vector<SemanticNode> DockSite::caption_nodes(const StackSlot& slot, std::size_t index) const {
    std::vector<SemanticNode> nodes{};
    if (slot.collapsed || slot.kind != DockNodeKind::tool_stack || slot.caption.empty() || !slot.view) return nodes;
    const CaptionGeometry placed = caption_geometry(slot.caption, pin_shown(slot));
    const Rect absolute = absolute_bounds();
    const std::string prefix((*slot.view).stable_id().value());
    nodes.reserve(3U);
    SemanticNode menu{};
    menu.stable_id = prefix + ".menu";
    menu.runtime_id = node_runtime_id(menu.stable_id);
    menu.role = SemanticRole::menu_button;
    menu.name = "Window position";
    menu.has_popup = true;
    menu.bounds = to_window(absolute, placed.menu);
    menu.states = SemanticState::visible;
    if (effectively_enabled()) menu.states |= SemanticState::enabled;
    if (menu_open_ && menu_stack_ == index) menu.states |= SemanticState::expanded;
    menu.actions = {SemanticAction::press, SemanticAction::show_menu};
    nodes.push_back(std::move(menu));
    if (!placed.pin.empty()) {
        SemanticNode pin{};
        pin.stable_id = prefix + ".pin";
        pin.runtime_id = node_runtime_id(pin.stable_id);
        pin.role = SemanticRole::check_box;
        pin.name = "Pinned";
        pin.bounds = to_window(absolute, placed.pin);
        pin.states = SemanticState::visible | SemanticState::checked;
        nodes.push_back(std::move(pin));
    }
    SemanticNode close{};
    close.stable_id = prefix + ".close";
    close.runtime_id = node_runtime_id(close.stable_id);
    close.role = SemanticRole::button;
    close.name = "Close";
    close.bounds = to_window(absolute, placed.close);
    close.states = SemanticState::visible;
    if (!button_states(index, Zone::close).contains(AnatomyState::disabled)) close.states |= SemanticState::enabled;
    close.actions = {SemanticAction::press};
    nodes.push_back(std::move(close));
    return nodes;
}

bool DockSite::caption_action(std::size_t index, std::string_view stable_id, SemanticAction action) {
    if (index >= stacks_.size() || !stacks_[index].view) return false;
    const StackSlot& slot = stacks_[index];
    const std::string prefix((*slot.view).stable_id().value());
    const std::optional<DockPanelId> panel = selected_panel(slot);
    if (!panel) return false;
    if (stable_id == prefix + ".menu" && (action == SemanticAction::press || action == SemanticAction::show_menu)) {
        const bool shown = show_position_menu(*panel, MenuOpenMode::keyboard);
        return shown;
    }
    if (stable_id == prefix + ".close" && action == SemanticAction::press) {
        const DockPositionCommand command = position_command_enabled(*panel, DockPositionCommand::hide)
                                                ? DockPositionCommand::hide
                                                : DockPositionCommand::close;
        const bool ran = run_position_command(*panel, command);
        return ran;
    }
    return false;
}

// -- anatomy -------------------------------------------------------------------

namespace {

constexpr std::array<PartField<DockSite::Parts>, 3> site_part_fields{{
    {.declaration = {.name = "region", .role = PartRole::region, .composed_from = {}},
     .member = &DockSite::Parts::region},
    {.declaration = {.name = "document_area", .role = PartRole::well, .composed_from = {}},
     .member = &DockSite::Parts::document_area},
    {.declaration = {.name = "focus", .role = PartRole::focus, .composed_from = {}},
     .member = &DockSite::Parts::focus},
}};
constexpr std::array<PartDeclaration, 3> site_parts = part_declarations(site_part_fields);
constexpr std::array<AnatomyState, 5> site_states{
    AnatomyState::normal, AnatomyState::enabled, AnatomyState::disabled,
    AnatomyState::focused, AnatomyState::focus_visible,
};

constexpr std::array<PartField<DockSite::SeamParts>, 2> seam_part_fields{{
    {.declaration = {.name = "seam_bar", .role = PartRole::seam, .composed_from = {}},
     .member = &DockSite::SeamParts::seam_bar},
    {.declaration = {.name = "seam_grip", .role = PartRole::grip, .composed_from = {}},
     .member = &DockSite::SeamParts::seam_grip},
}};
constexpr std::array<PartDeclaration, 2> seam_parts_declared = part_declarations(seam_part_fields);
constexpr std::array<StateField<DockSite::SeamStates>, 4> seam_state_fields{{
    {AnatomyState::hot, &DockSite::SeamStates::hot},
    {AnatomyState::captured, &DockSite::SeamStates::captured},
    {AnatomyState::focus_visible, &DockSite::SeamStates::focus_visible},
    {AnatomyState::disabled, &DockSite::SeamStates::disabled},
}};
constexpr std::array<AnatomyState, 4> seam_states_declared = state_declarations(seam_state_fields);

// The caption buttons' own states, and their glyphs'.
constexpr std::array<AnatomyState, 4> menu_button_states{AnatomyState::hot, AnatomyState::pressed,
                                                         AnatomyState::open, AnatomyState::disabled};
constexpr std::array<AnatomyState, 4> pin_button_states{AnatomyState::hot, AnatomyState::pressed,
                                                        AnatomyState::checked, AnatomyState::disabled};
constexpr std::array<AnatomyState, 3> close_button_states{AnatomyState::hot, AnatomyState::pressed,
                                                          AnatomyState::disabled};
constexpr std::array<AnatomyState, 2> pin_glyph_states{AnatomyState::checked, AnatomyState::disabled};
constexpr std::array<AnatomyState, 1> glyph_states{AnatomyState::disabled};

constexpr std::array<PartField<DockSite::StackParts>, 11> stack_part_fields{{
    {.declaration = {.name = "stack_area", .role = PartRole::region, .composed_from = "page_view"},
     .member = &DockSite::StackParts::stack_area},
    {.declaration = {.name = "caption", .role = PartRole::face, .composed_from = {}},
     .member = &DockSite::StackParts::caption},
    // The active caption's accent line (DK-41): no accent-line role exists;
    // keyline is the nearest atom.
    {.declaration = {.name = "caption_accent", .role = PartRole::keyline, .composed_from = {}},
     .member = &DockSite::StackParts::caption_accent},
    // The identity key: chip is the nearest atom for a small coloured mark.
    {.declaration = {.name = "caption_key", .role = PartRole::chip, .composed_from = {}},
     .member = &DockSite::StackParts::caption_key},
    {.declaration = {.name = "caption_label", .role = PartRole::caption, .composed_from = {}},
     .member = &DockSite::StackParts::caption_label},
    {.declaration = {.name = "caption_menu", .role = PartRole::actuator, .composed_from = "button",
                     .states = menu_button_states},
     .member = &DockSite::StackParts::caption_menu},
    {.declaration = {.name = "caption_menu_glyph", .role = PartRole::glyph, .composed_from = {},
                     .states = glyph_states},
     .member = &DockSite::StackParts::caption_menu_glyph},
    {.declaration = {.name = "caption_pin", .role = PartRole::actuator, .composed_from = "button",
                     .states = pin_button_states},
     .member = &DockSite::StackParts::caption_pin},
    {.declaration = {.name = "caption_pin_glyph", .role = PartRole::glyph, .composed_from = {},
                     .states = pin_glyph_states},
     .member = &DockSite::StackParts::caption_pin_glyph},
    {.declaration = {.name = "caption_close", .role = PartRole::actuator, .composed_from = "button",
                     .states = close_button_states},
     .member = &DockSite::StackParts::caption_close},
    {.declaration = {.name = "caption_close_glyph", .role = PartRole::glyph, .composed_from = {},
                     .states = glyph_states},
     .member = &DockSite::StackParts::caption_close_glyph},
}};
constexpr std::array<PartDeclaration, 11> stack_parts_declared = part_declarations(stack_part_fields);
constexpr std::array<StateField<DockSite::StackStates>, 2> stack_state_fields{{
    {AnatomyState::current, &DockSite::StackStates::current},
    {AnatomyState::disabled, &DockSite::StackStates::disabled},
}};
constexpr std::array<AnatomyState, 2> stack_states_declared = state_declarations(stack_state_fields);

constexpr std::array<ItemSystemDeclaration, 2> site_item_systems{{
    {.name = "seam", .parts = seam_parts_declared, .states = seam_states_declared, .custom_states = {}},
    {.name = "stack", .parts = stack_parts_declared, .states = stack_states_declared, .custom_states = {}},
}};

constexpr ControlAnatomy site_anatomy{
    .control_id = "dock_site",
    .parts = site_parts,
    .states = site_states,
    .custom_states = {},
    .item_systems = site_item_systems,
};

// Only the states a part declares are published with it.
AnatomyStateSet declared_only(AnatomyStateSet states, std::span<const AnatomyState> declared) noexcept {
    AnatomyStateSet kept;
    for (const AnatomyState state : declared) {
        if (states.contains(state)) kept.insert(state);
    }
    return kept;
}

PartGeometry with_states(Rect bounds, AnatomyStateSet states, std::span<const AnatomyState> declared) noexcept {
    PartGeometry geometry = PartGeometry::from_bounds(bounds);
    geometry.states = declared_only(states, declared);
    return geometry;
}

} // namespace

const ControlAnatomy& DockSite::anatomy() const noexcept {
    return site_anatomy;
}

void DockSite::publish_part_geometry(std::span<PartGeometry> geometry) const {
    const Rect surface = client_rectangle();
    Parts record;
    record.region = PartGeometry::from_bounds(surface);
    record.document_area = PartGeometry::from_bounds(document_area_);
    record.focus = PartGeometry::from_bounds(surface);
    write_parts(anatomy(), site_part_fields, record, geometry);
}

DockSite::Parts DockSite::parts() const {
    const PartGeometryBuffer published = published_part_geometry();
    Parts record;
    read_parts(anatomy(), site_part_fields, published.parts(), record);
    return record;
}

DockSite::SeamParts DockSite::seam_parts(std::size_t seam) const {
    const SeamSlot& slot = seams_[seam];
    SeamParts record;
    record.seam_bar = PartGeometry::from_bounds(slot.bar);
    const bool horizontal = slot.axis == DockAxis::horizontal;
    const double length = std::min(seam_grip_length, horizontal ? slot.bar.height : slot.bar.width);
    const Rect grip = horizontal ? centred(slot.bar, slot.bar.width, length) : centred(slot.bar, length, slot.bar.height);
    record.seam_grip = PartGeometry::from_bounds(grip);
    return record;
}

DockSite::StackParts DockSite::stack_parts(const StackSlot& slot, std::size_t index) const {
    StackParts record;
    record.stack_area = PartGeometry::from_bounds(slot.bounds);
    if (slot.kind != DockNodeKind::tool_stack || slot.caption.empty()) return record;
    const CaptionGeometry placed = caption_geometry(slot.caption, pin_shown(slot));
    record.caption = PartGeometry::from_bounds(placed.caption);
    if (stack_current(slot)) record.caption_accent = PartGeometry::from_bounds(placed.accent);
    record.caption_key = PartGeometry::from_bounds(placed.key);
    record.caption_label = PartGeometry::from_bounds(placed.label);
    const AnatomyStateSet menu = button_states(index, Zone::menu);
    const AnatomyStateSet pin = button_states(index, Zone::pin);
    const AnatomyStateSet close = button_states(index, Zone::close);
    record.caption_menu = with_states(placed.menu, menu, menu_button_states);
    record.caption_menu_glyph = with_states(placed.menu_glyph, menu, glyph_states);
    record.caption_pin = with_states(placed.pin, pin, pin_button_states);
    record.caption_pin_glyph = with_states(placed.pin_glyph, pin, pin_glyph_states);
    record.caption_close = with_states(placed.close, close, close_button_states);
    record.caption_close_glyph = with_states(placed.close_glyph, close, glyph_states);
    return record;
}

std::size_t DockSite::visible_stack_count() const {
    std::size_t count = 0U;
    for (const StackSlot& slot : stacks_) {
        if (!slot.collapsed) ++count;
    }
    return count;
}

// The index in stacks_ of the ordinal-th stack that is not collapsed.
std::size_t DockSite::visible_stack_at(std::size_t ordinal) const {
    std::size_t seen = 0U;
    for (std::size_t index = 0; index < stacks_.size(); ++index) {
        if (stacks_[index].collapsed) continue;
        if (seen == ordinal) return index;
        ++seen;
    }
    return stacks_.size();
}

std::size_t DockSite::realized_item_count() const {
    const std::size_t count = seams_.size() + visible_stack_count();
    return count;
}

ItemSystemIndex DockSite::publish_realized_item_system(std::size_t item) const {
    const ItemSystemIndex system{static_cast<std::uint8_t>(item < seams_.size() ? 0U : 1U)};
    return system;
}

std::string DockSite::realized_item_stable_id(std::size_t item) const {
    if (item >= realized_item_count()) throw std::out_of_range("DockSite item is out of range");
    if (item < seams_.size()) return seam_identity(seams_[item].split);
    const std::size_t index = visible_stack_at(item - seams_.size());
    const std::string identity = stack_identity(stacks_[index].stack);
    return identity;
}

AnatomyStateSet DockSite::realized_item_anatomy_states(std::size_t item) const {
    if (item >= realized_item_count()) throw std::out_of_range("DockSite item is out of range");
    if (item < seams_.size()) return seam_states(item);
    const std::size_t index = visible_stack_at(item - seams_.size());
    const AnatomyStateSet states = stack_states(stacks_[index]);
    return states;
}

void DockSite::publish_realized_item_geometry(std::size_t item, std::span<PartGeometry> geometry) const {
    if (item < seams_.size()) {
        const SeamParts record = seam_parts(item);
        write_declared_parts(seam_parts_declared, seam_part_fields, record, geometry);
        return;
    }
    const std::size_t index = visible_stack_at(item - seams_.size());
    const StackParts record = stack_parts(stacks_[index], index);
    write_declared_parts(stack_parts_declared, stack_part_fields, record, geometry);
}

std::vector<DockSite::Seam> DockSite::realized_seams() const {
    std::vector<Seam> seams{};
    seams.reserve(seams_.size());
    for (std::size_t seam = 0; seam < seams_.size(); ++seam) {
        Seam record;
        record.split = seams_[seam].split;
        record.axis = seams_[seam].axis;
        record.stable_id = seam_identity(record.split);
        const PartGeometryBuffer published = published_realized_item_geometry(seam);
        read_declared_parts(seam_parts_declared, seam_part_fields, published.parts(), record.parts);
        read_states(seam_state_fields, seam_states(seam), record.states);
        seams.push_back(std::move(record));
    }
    return seams;
}

std::vector<DockSite::Stack> DockSite::realized_stack_items() const {
    std::vector<Stack> stacks{};
    const std::size_t count = visible_stack_count();
    stacks.reserve(count);
    for (std::size_t ordinal = 0; ordinal < count; ++ordinal) {
        const std::size_t index = visible_stack_at(ordinal);
        const StackSlot& slot = stacks_[index];
        Stack record;
        record.stack = slot.stack;
        record.kind = slot.kind;
        record.panel = selected_panel(slot).value_or(DockPanelId{});
        record.stable_id = stack_identity(slot.stack);
        const PartGeometryBuffer published = published_realized_item_geometry(seams_.size() + ordinal);
        read_declared_parts(stack_parts_declared, stack_part_fields, published.parts(), record.parts);
        read_states(stack_state_fields, stack_states(slot), record.states);
        stacks.push_back(std::move(record));
    }
    return stacks;
}

} // namespace gui_forms
