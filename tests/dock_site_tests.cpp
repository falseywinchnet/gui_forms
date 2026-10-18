// DockSite, DockWorkspace and DockNavigator (stage W, docking phase D2): the
// docked layout in one window. Part geometry; seams moved with the keyboard
// and the pointer within the minimums; collapsing tool stacks by priority
// when the window is too small, and their return; the maximized document
// area; lazy creation (factories run only for panels shown selected); the
// activation tracks and command routing (Ctrl+S from a focused tool control
// reaches the active document); the navigator's order and keys; F6 order;
// Escape and Shift+Escape; Ctrl+F6; the position menu; semantic snapshots;
// refusals.

#include "gui_forms/gui_forms.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {
using namespace gui_forms;

void require(bool condition, const char* message) {
    if (condition) return;
    std::cerr << message << '\n';
    std::exit(1);
}

bool near(double left, double right) {
    const bool close = std::abs(left - right) < 1e-6;
    return close;
}

Point centre(Rect bounds) {
    const Point point{bounds.x + bounds.width * 0.5, bounds.y + bounds.height * 0.5};
    return point;
}

const SemanticNode* find_node(const std::vector<SemanticNode>& nodes, std::string_view stable_id) {
    for (const SemanticNode& node : nodes) {
        if (node.stable_id == stable_id) return &node;
        if (const SemanticNode* nested = find_node(node.children, stable_id)) return nested;
    }
    return nullptr;
}

const SemanticNode* find_named(const std::vector<SemanticNode>& nodes, SemanticRole role, std::string_view name) {
    for (const SemanticNode& node : nodes) {
        if (node.role == role && node.name == name) return &node;
        if (const SemanticNode* nested = find_named(node.children, role, name)) return nested;
    }
    return nullptr;
}

bool key(Window& window, std::uint32_t physical_key, Modifier modifiers = Modifier::none) {
    KeyEvent event{KeyAction::down, physical_key};
    event.modifiers = modifiers;
    const bool handled = window.dispatch_key(event);
    return handled;
}

bool key_up(Window& window, std::uint32_t physical_key, Modifier modifiers = Modifier::none) {
    KeyEvent event{KeyAction::up, physical_key};
    event.modifiers = modifiers;
    const bool handled = window.dispatch_key(event);
    return handled;
}

void click(Window& window, Point point) {
    static_cast<void>(window.dispatch_pointer({PointerAction::down, PointerButton::primary, point}));
    static_cast<void>(window.dispatch_pointer({PointerAction::up, PointerButton::primary, point}));
}

// Records each Save that reaches a document.
struct SaveJournal final {
    std::vector<std::string> saved;
};

class SaveHandler final {
public:
    SaveHandler(SaveJournal& journal, std::string document) : journal_(&journal), document_(std::move(document)) {}
    void run(const CommandInvocation&) { (*journal_).saved.push_back(document_); }

private:
    SaveJournal* journal_{};
    std::string document_;
};

// Builds each panel's content: a panel holding a text box and a button
// (`content.<id>`, `content.<id>.edit` and `content.<id>.action`); a
// document's content carries a command target with a Save handler.
class PanelFactory final {
public:
    explicit PanelFactory(SaveJournal& journal) : journal_(&journal) {}

    void create(DockPanelCreation& creation) {
        made.push_back(creation.panel.value);
        const std::string id = "content." + creation.panel.value;
        std::shared_ptr<Panel> panel = make_control<Panel>(StableId(id));
        std::shared_ptr<TextBox> edit = make_control<TextBox>(StableId(id + ".edit"));
        (*edit).set_accessible_name(creation.panel.value);
        (*edit).set_requested_bounds({6.0, 6.0, 80.0, 22.0});
        (*panel).add_child(edit);
        std::shared_ptr<Button> action = make_control<Button>(StableId(id + ".action"), "Act");
        (*action).set_requested_bounds({6.0, 32.0, 60.0, 22.0});
        (*panel).add_child(action);
        if (creation.panel.value.rfind("doc", 0) == 0) {
            handlers_.push_back(std::make_unique<SaveHandler>(*journal_, creation.panel.value));
            std::shared_ptr<CommandTarget> target = std::make_shared<CommandTarget>(creation.panel.value);
            (*target).set_handler("save", Delegate<const CommandInvocation&>::bind<SaveHandler, &SaveHandler::run>(
                                              *handlers_.back()));
            (*panel).set_command_target(target);
        }
        creation.content = panel;
    }

    std::vector<std::string> made;

private:
    SaveJournal* journal_{};
    std::vector<std::unique_ptr<SaveHandler>> handlers_;
};

DockPanelRegistration tool(PanelFactory& factory, std::string id, std::string title, DockSide side,
                           std::int32_t priority, Size minimum) {
    DockPanelRegistration registration{};
    registration.descriptor.id = DockPanelId(std::move(id));
    registration.descriptor.kind = DockPanelKind::tool;
    registration.descriptor.collapse_priority = priority;
    registration.descriptor.minimum_size = minimum;
    registration.descriptor.default_placement.side = side;
    registration.descriptor.default_placement.ratio = 0.25;
    registration.title = std::move(title);
    registration.factory = Delegate<DockPanelCreation&>::bind<PanelFactory, &PanelFactory::create>(factory);
    return registration;
}

DockPanelRegistration document(PanelFactory& factory, std::string id, std::string title) {
    DockPanelRegistration registration{};
    registration.descriptor.id = DockPanelId(std::move(id));
    registration.descriptor.kind = DockPanelKind::document;
    registration.descriptor.minimum_size = {120.0, 80.0};
    registration.title = std::move(title);
    registration.factory = Delegate<DockPanelCreation&>::bind<PanelFactory, &PanelFactory::create>(factory);
    return registration;
}

void apply(DockWorkspace& workspace, DockEdit edit) {
    DockTransaction transaction{};
    transaction.edits.push_back(std::move(edit));
    require(workspace.apply(transaction).applied, "a fixture edit applies");
}

// A workspace with Solution and Classes tabbed on the left (Classes
// selected), Properties on the right, Output at the bottom and two documents
// (Second selected), realized by a site filling an 800 by 500 window.
struct Fixture final {
    SaveJournal journal;
    PanelFactory factory{journal};
    std::unique_ptr<DockWorkspace> workspace;
    std::shared_ptr<DockSite> site;
    std::unique_ptr<Window> window;
};

// Registers the fixture's six panels.
void register_panels(DockWorkspace& workspace, PanelFactory& factory) {
    require(workspace.register_panel(tool(factory, "solution", "Solution", DockSide::left, 2, {120.0, 100.0})) &&
                workspace.register_panel(tool(factory, "classes", "Classes", DockSide::left, 2, {120.0, 100.0})),
            "the left panels register");
    DockPanelRegistration properties = tool(factory, "properties", "Properties", DockSide::right, 1, {140.0, 100.0});
    properties.descriptor.dock_as_document = true;
    require(workspace.register_panel(properties), "Properties registers");
    require(workspace.register_panel(tool(factory, "output", "Output", DockSide::bottom, 0, {100.0, 60.0})),
            "Output registers");
    require(workspace.register_panel(document(factory, "doc.first", "First.txt")) &&
                workspace.register_panel(document(factory, "doc.second", "Second.txt")),
            "the documents register");
    require(!workspace.register_panel(document(factory, "doc.first", "Again")), "a repeated identity is refused");
}

std::unique_ptr<Fixture> make_fixture(Size client = {800.0, 500.0}) {
    std::unique_ptr<Fixture> fixture = std::make_unique<Fixture>();
    (*fixture).workspace = std::make_unique<DockWorkspace>("workspace");
    DockWorkspace& workspace = *(*fixture).workspace;
    PanelFactory& factory = (*fixture).factory;
    register_panels(workspace, factory);
    apply(workspace, DockEdit::show(DockPanelId("doc.first")));
    apply(workspace, DockEdit::show(DockPanelId("doc.second")));
    apply(workspace, DockEdit::show(DockPanelId("solution")));
    const DockNodeId left = workspace.layout().locate(DockPanelId("solution")).stack;
    apply(workspace, DockEdit::dock_into(DockSubject::of_panel(DockPanelId("classes")), left));
    apply(workspace, DockEdit::show(DockPanelId("properties")));
    apply(workspace, DockEdit::show(DockPanelId("output")));
    require(factory.made.empty(), "registering and placing panels creates nothing");
    (*fixture).site = make_control<DockSite>(StableId("site"), workspace);
    (*fixture).window = std::make_unique<Window>((*fixture).site, client);
    (*(*fixture).window).perform_layout();
    return fixture;
}

DockNodeId stack_of(const DockWorkspace& workspace, std::string_view panel) {
    const DockNodeId stack = workspace.layout().locate(DockPanelId(std::string(panel))).stack;
    return stack;
}

const DockSite::Stack* find_stack(const std::vector<DockSite::Stack>& stacks, DockNodeId stack) {
    for (const DockSite::Stack& record : stacks) {
        if (record.stack == stack) return &record;
    }
    return nullptr;
}

Control::Ptr content_edit(Window& window, std::string_view panel) {
    const Control::Ptr edit = window.find("content." + std::string(panel) + ".edit");
    return edit;
}

bool active_is(const DockWorkspace& workspace, std::string_view panel, std::string_view document) {
    const std::optional<DockPanelId>& active = workspace.active_panel();
    const std::optional<DockPanelId>& active_document = workspace.active_document();
    const bool panel_matches = active && (*active).value == panel;
    const bool document_matches = document.empty() ? !active_document
                                                   : (active_document && (*active_document).value == document);
    const bool result = panel_matches && document_matches;
    return result;
}

// -- anatomy and part geometry ------------------------------------------------

void anatomy_and_parts() {
    std::unique_ptr<Fixture> fixture = make_fixture();
    DockSite& site = *(*fixture).site;
    DockWorkspace& workspace = *(*fixture).workspace;
    const ControlAnatomy& anatomy = site.anatomy();
    require(validate_control_anatomy(anatomy).defect == AnatomyDefect::none && anatomy.control_id == "dock_site" &&
                anatomy.item_systems.size() == 2U && anatomy.item_systems[0].name == "seam" &&
                anatomy.item_systems[1].name == "stack",
            "DockSite publishes a valid anatomy with its seam and stack item systems");
    const DockNavigator navigator(StableId("lone"));
    require(validate_control_anatomy(navigator.anatomy()).defect == AnatomyDefect::none &&
                navigator.anatomy().control_id == "dock_navigator",
            "DockNavigator publishes a valid anatomy");

    // Four stacks in tree order: left, the documents, right, bottom.
    const std::vector<DockNodeId> stacks = site.realized_stacks();
    require(stacks.size() == 4U && stacks[0] == stack_of(workspace, "classes") &&
                stacks[1] == stack_of(workspace, "doc.first") && stacks[2] == stack_of(workspace, "properties") &&
                stacks[3] == stack_of(workspace, "output"),
            "the site realizes one view per stack in tree order");
    const std::vector<DockSite::Seam> seams = site.realized_seams();
    require(seams.size() == 3U, "three splits give three seams");
    for (const DockSite::Seam& seam : seams) {
        const Rect bar = seam.parts.seam_bar.bounds;
        const double thickness = seam.axis == DockAxis::horizontal ? bar.width : bar.height;
        require(near(thickness, 3.0) && seam.parts.seam_grip.present,
                "a seam is a 3-unit bar with a grip of dots");
    }
    const std::vector<DockSite::Stack> items = site.realized_stack_items();
    const DockSite::Stack* left = find_stack(items, stacks[0]);
    const DockSite::Stack* documents = find_stack(items, stacks[1]);
    require(left != nullptr && documents != nullptr && (*left).panel.value == "classes" &&
                (*documents).panel.value == "doc.second",
            "each stack reports its selected panel");
    const DockSite::StackParts& caption = (*left).parts;
    require(caption.caption.present && near(caption.caption.bounds.height, 21.0) &&
                near(caption.caption_key.bounds.width, 7.0) && near(caption.caption_menu.bounds.width, 16.0) &&
                caption.caption_pin.present && caption.caption_close.present &&
                caption.caption_pin.states.contains(AnatomyState::checked) &&
                caption.caption_pin.states.contains(AnatomyState::disabled) &&
                caption.caption_pin_glyph.states.contains(AnatomyState::checked),
            "a tool stack's caption carries the key, the label and three buttons, the pin checked and disabled");
    require(caption.caption_menu.bounds.x < caption.caption_pin.bounds.x &&
                caption.caption_pin.bounds.x < caption.caption_close.bounds.x &&
                near(caption.caption_close.bounds.x + 16.0 + 3.0,
                     caption.caption.bounds.x + caption.caption.bounds.width) &&
                caption.caption_label.bounds.x > caption.caption_key.bounds.x,
            "the buttons run menu, pin, close to the caption's end");
    require(!(*documents).parts.caption.present && !(*documents).parts.caption_pin.present &&
                (*documents).parts.stack_area.present,
            "a document stack has no caption");
    require(site.parts().document_area.present, "the document area is published");

    // Tool tabs at the bottom only with two or more panels.
    const std::shared_ptr<PageView> left_view = site.stack_view(stacks[0]);
    const std::shared_ptr<PageView> right_view = site.stack_view(stacks[2]);
    const std::shared_ptr<PageView> document_view = site.stack_view(stacks[1]);
    require(left_view && right_view && document_view && (*(*left_view).strip()).visible() &&
                (*(*left_view).strip()).placement() == PageStripPlacement::bottom &&
                !(*(*right_view).strip()).visible() && (*(*document_view).strip()).visible() &&
                (*(*document_view).strip()).placement() == PageStripPlacement::top,
            "tool tabs show at the bottom of a stack of two; a lone tool panel has none; documents show tabs on top");
    require(near((*right_view).parts().content.bounds.height, (*right_view).client_rectangle().height),
            "a stack without tabs gives its whole height to the content");
    const Rect left_bounds = (*left_view).absolute_bounds();
    require(near(left_bounds.y, caption.caption.bounds.y + 21.0), "a tool stack's view sits below its caption");

    // The caption of the stack holding the active panel is current.
    require(workspace.activate_panel(DockPanelId("properties")), "Properties activates");
    (*(*fixture).window).perform_layout();
    const std::vector<DockSite::Stack> after = site.realized_stack_items();
    const DockSite::Stack* right = find_stack(after, stacks[2]);
    const DockSite::Stack* left_after = find_stack(after, stacks[0]);
    require(right != nullptr && (*right).states.current && (*right).parts.caption_accent.present &&
                left_after != nullptr && !(*left_after).states.current && !(*left_after).parts.caption_accent.present,
            "only the active stack's caption is current and carries the accent line");
    require(site.realized_item_count() == 7U && site.realized_item_system(0U).value == 0U &&
                site.realized_item_system(3U).value == 1U,
            "the seams then the stacks are the realized items");
}

// -- lazy creation ------------------------------------------------------------

void lazy_creation() {
    std::unique_ptr<Fixture> fixture = make_fixture();
    const std::vector<std::string>& made = (*fixture).factory.made;
    const std::vector<std::string> expected{"classes", "doc.second", "properties", "output"};
    require(made == expected, "the factories ran only for the selected panel of each shown stack");
    DockWorkspace& workspace = *(*fixture).workspace;
    require(!workspace.panel_content(DockPanelId("solution")) && !workspace.panel_content(DockPanelId("doc.first")),
            "panels behind another tab have no content");
    const std::shared_ptr<PageView> left = (*(*fixture).site).stack_view(stack_of(workspace, "solution"));
    require((*(*fixture).window).perform_semantic_action(
                std::string((*(*left).strip()).stable_id().value()) + ".tab.solution", SemanticAction::select),
            "Solution's tab is chosen");
    require(made.size() == 5U && made.back() == "solution" && workspace.panel_content(DockPanelId("solution")) &&
                (*workspace.panel_content(DockPanelId("solution"))).effectively_visible(),
            "choosing a tab creates its panel's content and shows it");
    require((*workspace.layout().find_node(stack_of(workspace, "solution"))).selected == 0U,
            "the layout records the chosen tab");
    // Hiding keeps the content; showing again reuses it.
    require(workspace.hide_panel(DockPanelId("output")).applied && workspace.show_panel(DockPanelId("output")).applied,
            "Output hides and shows");
    (*(*fixture).window).perform_layout();
    require(made.size() == 5U, "a hidden panel shown again keeps its content");
    require(workspace.created_panel_count() == 5U, "the workspace counts five creations");

    // A restored layout creates only what it shows (decision DK-34).
    const DockLayoutWriteResult saved = workspace.layout().write_xml();
    require(saved.error == XmlError::none, "the layout saves");
    SaveJournal journal;
    PanelFactory restored_factory(journal);
    DockWorkspace restored("restored");
    register_panels(restored, restored_factory);
    const DockRestoreResult result = restored.layout().restore_xml(saved.text, DockRestoreOptions{});
    require(result.status == DockRestoreStatus::restored, "the saved layout restores");
    std::shared_ptr<DockSite> site = make_control<DockSite>(StableId("restored.site"), restored);
    Window window(site, {800.0, 500.0});
    window.perform_layout();
    const std::vector<std::string> shown{"solution", "doc.second", "properties", "output"};
    require(restored_factory.made == shown && (*site).realized_stacks().size() == 4U,
            "after a restore the factories run only for the selected panel of each shown stack");
}

// -- seams --------------------------------------------------------------------

void seam_keyboard_and_pointer() {
    std::unique_ptr<Fixture> fixture = make_fixture();
    DockSite& site = *(*fixture).site;
    DockWorkspace& workspace = *(*fixture).workspace;
    Window& window = *(*fixture).window;
    // The first seam is the bottom split's (Output below everything else);
    // find the left stack's seam: the split whose first child holds Classes.
    std::vector<DockSite::Seam> seams = site.realized_seams();
    std::size_t left_seam = seams.size();
    for (std::size_t index = 0; index < seams.size(); ++index) {
        const DockNode& split = *workspace.layout().find_node(seams[index].split);
        if (split.first == stack_of(workspace, "classes")) left_seam = index;
    }
    require(left_seam < seams.size() && seams[left_seam].axis == DockAxis::horizontal,
            "the left stack's seam is found");
    const std::shared_ptr<PageView> left = site.stack_view(stack_of(workspace, "classes"));
    const double width = (*left).client_rectangle().width;
    require(site.focus_seam(left_seam) && site.focused_seam() == left_seam, "the seam takes the keyboard");
    require(key(window, PhysicalKey::right), "Right moves the seam");
    window.perform_layout();
    require(near((*left).client_rectangle().width, width + 8.0), "the seam moved 8 units right");
    require(key(window, PhysicalKey::left) && key(window, PhysicalKey::left), "Left moves it back");
    window.perform_layout();
    require(near((*left).client_rectangle().width, width - 8.0), "the seam moved left");
    for (int step = 0; step < 40; ++step) {
        static_cast<void>(key(window, PhysicalKey::left));
        window.perform_layout();
    }
    require(near((*left).client_rectangle().width, 120.0), "the seam stops at the left panels' minimum width");
    require(!site.move_seam(left_seam, -8.0), "past the minimum the seam does not move");
    require(!key(window, PhysicalKey::up), "Up does not move a vertical bar");
    // Tab steps to the next seam, and leaves the site after the last.
    seams = site.realized_seams();
    std::size_t expected = left_seam;
    while (expected + 1U < seams.size()) {
        require(key(window, PhysicalKey::tab) && site.focused_seam() == expected + 1U, "Tab steps to the next seam");
        ++expected;
    }
    static_cast<void>(key(window, PhysicalKey::tab));
    require(!site.focused_seam().has_value(), "Tab past the last seam leaves the site's seams");

    // The pointer drags the seam; the solver keeps the right side's minimum.
    window.perform_layout();
    seams = site.realized_seams();
    const Rect bar = seams[left_seam].parts.seam_bar.bounds;
    const Point start = centre(bar);
    static_cast<void>(window.dispatch_pointer({PointerAction::move, PointerButton::none, start}));
    require(site.realized_seams()[left_seam].states.hot,
            "the seam is hot under the pointer");
    static_cast<void>(window.dispatch_pointer({PointerAction::down, PointerButton::primary, start}));
    require(site.realized_seams()[left_seam].states.captured, "the pressed seam is captured");
    static_cast<void>(
        window.dispatch_pointer({PointerAction::move, PointerButton::primary, {start.x + 50.0, start.y}}));
    window.perform_layout();
    require(near((*left).client_rectangle().width, 170.0), "the seam follows the pointer");
    static_cast<void>(
        window.dispatch_pointer({PointerAction::move, PointerButton::primary, {start.x + 2000.0, start.y}}));
    window.perform_layout();
    const Rect documents = site.parts().document_area.bounds;
    require(documents.width >= 120.0 - 1e-6, "dragging far keeps the other side's minimum");
    static_cast<void>(window.dispatch_pointer({PointerAction::up, PointerButton::primary, {start.x + 2000.0, start.y}}));
    require(!site.realized_seams()[left_seam].states.captured, "the release ends the drag");
    // The hit band reaches 3 units past the bar, over the stacks beside it.
    window.perform_layout();
    const Rect band = site.realized_seams()[left_seam].parts.seam_bar.bounds;
    const double before = (*left).client_rectangle().width;
    const Point inside{band.x - 2.0, band.y + 40.0};
    require(window.hit_test(inside) != site.shared_from_this(), "the point beside the bar is over a stack");
    static_cast<void>(window.dispatch_pointer({PointerAction::down, PointerButton::primary, inside}));
    static_cast<void>(
        window.dispatch_pointer({PointerAction::move, PointerButton::primary, {inside.x - 30.0, inside.y}}));
    static_cast<void>(window.dispatch_pointer({PointerAction::up, PointerButton::primary, {inside.x - 30.0, inside.y}}));
    window.perform_layout();
    require(near((*left).client_rectangle().width, before - 30.0), "a press in the hit band beside the bar drags it");
    // The semantic seam: a split pane with its value and actions.
    const SemanticSnapshot snapshot = window.semantic_snapshot();
    const SemanticNode* node = find_node(snapshot.roots, seams[left_seam].stable_id);
    require(node != nullptr && (*node).role == SemanticRole::split_pane && (*node).numeric_value &&
                near(*(*node).minimum_value, 120.0),
            "the seam publishes a split pane with its value and minimum");
    require(window.perform_semantic_action(seams[left_seam].stable_id, SemanticAction::set_value, "200"),
            "set_value moves the seam");
    window.perform_layout();
    require(near((*left).client_rectangle().width, 200.0), "the seam stands at the value set");
    bool threw = false;
    try {
        site.set_keyboard_increment(0.5);
    } catch (const std::out_of_range&) {
        threw = true;
    }
    require(threw && near(site.keyboard_increment(), 8.0), "a keyboard increment below 1 is refused");
}

// -- minimums, collapse and the maximized document area ------------------------

void collapse_by_priority() {
    std::unique_ptr<Fixture> fixture = make_fixture();
    DockSite& site = *(*fixture).site;
    DockWorkspace& workspace = *(*fixture).workspace;
    Window& window = *(*fixture).window;
    const Size minimum = site.minimum_size();
    // Widths: left 120 + 3 + documents 120 + 3 + right 140.
    require(near(minimum.width, 386.0), "the minimum width adds the panels' minimums and the seams");
    require(site.collapsed_stacks().empty(), "nothing collapses at full size");
    const std::uint64_t generation = workspace.layout().generation();
    // Too narrow for all three columns: Properties (priority 1) collapses
    // before the left panels (2); Output (0) is below and does not help.
    window.resize({340.0, 500.0});
    window.perform_layout();
    std::vector<DockNodeId> collapsed = site.collapsed_stacks();
    require(collapsed.size() == 1U && collapsed[0] == stack_of(workspace, "properties"),
            "the lowest priority stack whose collapse helps goes first");
    require(!(*site.stack_view(stack_of(workspace, "properties"))).effectively_visible() &&
                (*site.stack_view(stack_of(workspace, "output"))).effectively_visible(),
            "the collapsed stack is hidden and the others stay");
    require(workspace.layout().generation() == generation &&
                workspace.layout().locate(DockPanelId("properties")).placement == DockPlacementKind::docked,
            "collapsing leaves the layout model unchanged");
    // Too short as well: Output (priority 0) goes first, then Properties.
    window.resize({340.0, 200.0});
    window.perform_layout();
    collapsed = site.collapsed_stacks();
    require(collapsed.size() == 2U && collapsed[0] == stack_of(workspace, "output") &&
                collapsed[1] == stack_of(workspace, "properties"),
            "too short and too narrow, Output collapses before Properties");
    window.resize({200.0, 200.0});
    window.perform_layout();
    collapsed = site.collapsed_stacks();
    require(collapsed.size() == 3U, "at the smallest only the documents remain");
    require(site.parts().document_area.bounds.width > 199.0, "the documents take the whole width");
    window.resize({800.0, 500.0});
    window.perform_layout();
    require(site.collapsed_stacks().empty() &&
                (*site.stack_view(stack_of(workspace, "properties"))).effectively_visible(),
            "with room again every stack returns");
    require((*fixture).factory.made.size() == 4U, "collapsing and returning creates nothing new");

    // Ctrl+Shift+F12 hides every tool stack and restores them.
    require(workspace.activate_panel(DockPanelId("properties")) &&
                (*window.focused_control()).stable_id().value() == "content.properties.edit",
            "focus is in Properties");
    require(key(window, PhysicalKey::f12, Modifier::control | Modifier::shift) && site.document_area_maximized(),
            "Ctrl+Shift+F12 maximizes the document area");
    window.perform_layout();
    require(site.collapsed_stacks().size() == 3U && site.parts().document_area.bounds.width > 799.0,
            "every tool stack is hidden and the documents fill the site");
    require(window.focused_control() &&
                (*window.focused_control()).stable_id().value() == "content.doc.second.edit" &&
                active_is(workspace, "doc.second", "doc.second"),
            "focus moved from the hidden tool panel to the active document");
    require(key(window, PhysicalKey::f12, Modifier::control | Modifier::shift) && !site.document_area_maximized(),
            "Ctrl+Shift+F12 again restores");
    window.perform_layout();
    require(site.collapsed_stacks().empty(), "the tool stacks return");
}

// -- activation and commands ---------------------------------------------------

void activation_and_commands() {
    std::unique_ptr<Fixture> fixture = make_fixture();
    DockWorkspace& workspace = *(*fixture).workspace;
    Window& window = *(*fixture).window;
    std::vector<DockActivationChange> changes;
    struct Record final {
        std::vector<DockActivationChange>* seen{};
        void operator()(const DockActivationChange& change) const { (*seen).push_back(change); }
    };
    const SubscriptionToken token = workspace.activation_changed().subscribe(Record{&changes});
    require(!workspace.active_panel() && !workspace.active_document(), "nothing is active at first");
    // Focus entering a document's control activates it on both tracks.
    require(window.request_focus(content_edit(window, "doc.second")), "the document's editor takes focus");
    require(active_is(workspace, "doc.second", "doc.second") && changes.size() == 2U &&
                changes[0].track == DockActivationTrack::panel && changes[1].track == DockActivationTrack::document &&
                changes[0].cause == DockActivationCause::focus,
            "a document becomes the active panel and the active document, the panel track first");
    // A tool panel changes only the active panel.
    require(window.request_focus(content_edit(window, "properties")), "the tool editor takes focus");
    require(active_is(workspace, "properties", "doc.second") && changes.size() == 3U,
            "a tool panel changes the active panel only");
    require(window.command_target(CommandRouteStage::active_document) &&
                (*window.command_target(CommandRouteStage::active_document)).name() == "doc.second",
            "the active document's target is named on the window");
    // Ctrl+S from the focused tool control reaches the active document.
    const std::shared_ptr<Command> save = std::make_shared<Command>("save", "Save");
    const AcceleratorToken shortcut = (*save).bind_shortcut(window, {PhysicalKey::s, Modifier::control});
    require(key(window, PhysicalKey::s, Modifier::control), "Ctrl+S is handled");
    require((*fixture).journal.saved.size() == 1U && (*fixture).journal.saved[0] == "doc.second",
            "Ctrl+S from a focused tool control reaches the active document's Save");
    // Activating the other document (a tab, through the workspace) moves the
    // document track and the command target.
    require(workspace.activate_panel(DockPanelId("doc.first"), DockActivationCause::keyboard) &&
                active_is(workspace, "doc.first", "doc.first") &&
                (*window.focused_control()).stable_id().value() == "content.doc.first.edit",
            "activating a document focuses its content");
    require(window.request_focus(content_edit(window, "output")) && key(window, PhysicalKey::s, Modifier::control) &&
                (*fixture).journal.saved.back() == "doc.first",
            "Save now reaches the newly active document");
    const std::span<const DockPanelId> history = workspace.tool_history();
    const std::vector<DockPanelId> tools(history.begin(), history.end());
    require(tools.size() == 2U && tools[0].value == "output" && tools[1].value == "properties",
            "the tool history is most recent first");
    // A press on a caption activates its panel.
    DockSite& site = *(*fixture).site;
    const std::vector<DockSite::Stack> stacks = site.realized_stack_items();
    const DockSite::Stack* right = find_stack(stacks, stack_of(workspace, "properties"));
    const Rect absolute = site.absolute_bounds();
    const Rect label = (*right).parts.caption_label.bounds;
    click(window, {absolute.x + label.x + 4.0, absolute.y + label.y + 4.0});
    require(active_is(workspace, "properties", "doc.first") &&
                (*window.focused_control()).stable_id().value() == "content.properties.edit",
            "a press on a caption activates its panel and returns to its remembered control");
    // Closing the active document falls back to the next in the history.
    require(workspace.close_panel(DockPanelId("doc.first")).applied, "the first document closes");
    require(workspace.active_document() && (*workspace.active_document()).value == "doc.second" &&
                !workspace.panel_content(DockPanelId("doc.first")),
            "the active document falls back to the next most recent and the closed one's content is released");
    require(!workspace.activate_panel(DockPanelId("doc.first")) && !workspace.activate_panel(DockPanelId("missing")),
            "a closed or unknown panel cannot be activated");
}

// -- the navigator -------------------------------------------------------------

void navigator_order_and_keys() {
    std::unique_ptr<Fixture> fixture = make_fixture();
    DockWorkspace& workspace = *(*fixture).workspace;
    DockSite& site = *(*fixture).site;
    Window& window = *(*fixture).window;
    require(workspace.activate_panel(DockPanelId("doc.first")) && workspace.activate_panel(DockPanelId("doc.second")) &&
                workspace.activate_panel(DockPanelId("output")) && workspace.activate_panel(DockPanelId("properties")),
            "four panels activate in turn");
    // Ctrl+Tab from a tool panel: the documents, most recent first, the
    // second current; releasing Ctrl activates it.
    require(key(window, PhysicalKey::tab, Modifier::control) && site.navigator_open(), "Ctrl+Tab opens the navigator");
    std::shared_ptr<DockNavigator> navigator = site.navigator();
    require(navigator && (*navigator).current_list() == DockNavigatorList::documents &&
                (*navigator).entries(DockNavigatorList::documents).size() == 2U &&
                (*navigator).entries(DockNavigatorList::documents)[0].panel.value == "doc.second" &&
                (*navigator).entries(DockNavigatorList::documents)[1].panel.value == "doc.first" &&
                (*navigator).current_panel() && (*(*navigator).current_panel()).value == "doc.first",
            "the documents are listed most recent first with the second current");
    const std::span<const DockNavigatorEntry> tools = (*navigator).entries(DockNavigatorList::tools);
    require(tools.size() == 4U && tools[0].panel.value == "properties" && tools[1].panel.value == "output" &&
                tools[2].panel.value == "solution" && tools[3].panel.value == "classes",
            "the tool list is most recent first, then the other shown tool panels in tree order");
    require(window.focused_control() == navigator, "the navigator has focus");
    require(key(window, PhysicalKey::tab, Modifier::control) && (*(*navigator).current_panel()).value == "doc.second",
            "Tab with Ctrl held steps and wraps");
    require(!key_up(window, PhysicalKey::tab, Modifier::control) || site.navigator_open(),
            "a key-up with Ctrl still held keeps the navigator open");
    require(site.navigator_open(), "the navigator is still open");
    require(key(window, PhysicalKey::left, Modifier::control) &&
                (*navigator).current_list() == DockNavigatorList::tools,
            "Left switches to the tool list");
    require((*(*navigator).current_panel()).value == "properties" && key(window, PhysicalKey::down, Modifier::control) &&
                (*(*navigator).current_panel()).value == "output",
            "the row index carries over, and Down steps in the tool list");
    require(key_up(window, PhysicalKey::left_control, Modifier::none) && !site.navigator_open(),
            "releasing Ctrl closes the navigator");
    require(active_is(workspace, "output", "doc.second") &&
                (*window.focused_control()).stable_id().value() == "content.output.edit",
            "and activates the chosen panel");
    // Ctrl+Shift+Tab starts at the last row; Escape cancels.
    require(key(window, PhysicalKey::tab, Modifier::control | Modifier::shift) && site.navigator_open() &&
                (*(*site.navigator()).current_panel()).value == "doc.first",
            "Ctrl+Shift+Tab starts at the last document");
    require(key(window, PhysicalKey::escape, Modifier::control) && !site.navigator_open() &&
                active_is(workspace, "output", "doc.second") &&
                (*window.focused_control()).stable_id().value() == "content.output.edit",
            "Escape cancels and focus returns");
    // Alt+F7 opens on the tool list.
    require(key(window, PhysicalKey::f7, Modifier::alt) && site.navigator_open() &&
                (*site.navigator()).current_list() == DockNavigatorList::tools &&
                (*(*site.navigator()).current_panel()).value == "properties",
            "Alt+F7 opens on the tool panels with the previous one current");
    require(key(window, PhysicalKey::enter, Modifier::alt) && !site.navigator_open() &&
                active_is(workspace, "properties", "doc.second"),
            "Enter chooses");
    // Semantics: two lists of rows; press chooses.
    require(key(window, PhysicalKey::tab, Modifier::control) && site.navigator_open(), "the navigator opens again");
    const SemanticSnapshot snapshot = window.semantic_snapshot();
    const SemanticNode* list = find_named(snapshot.roots, SemanticRole::list, "Active Files");
    require(list != nullptr && (*list).children.size() == 2U && (*list).children[1].name == "First.txt" &&
                has_semantic_state((*list).children[1].states, SemanticState::selected),
            "the navigator publishes its lists with the current row selected");
    require(window.perform_semantic_action((*list).children[0].stable_id, SemanticAction::press) &&
                !site.navigator_open() && active_is(workspace, "doc.second", "doc.second"),
            "a row's press chooses it");
    // An outside press closes it.
    require(key(window, PhysicalKey::tab, Modifier::control) && site.navigator_open(), "and opens once more");
    click(window, {4.0, 4.0});
    require(!site.navigator_open(), "a press outside the navigator closes it");
}

// -- F6, Escape, Shift+Escape, Ctrl+F6 -----------------------------------------

void regions_and_escape_keys() {
    std::unique_ptr<Fixture> fixture = make_fixture();
    DockWorkspace& workspace = *(*fixture).workspace;
    DockSite& site = *(*fixture).site;
    Window& window = *(*fixture).window;
    const std::vector<Control::Ptr> regions = window.focus_regions();
    const std::vector<DockNodeId> stacks = site.realized_stacks();
    require(regions.size() == 4U, "every stack is a focus region");
    for (std::size_t index = 0; index < stacks.size(); ++index) {
        require(regions[index] == site.stack_view(stacks[index]), "the regions follow the tree order");
    }
    require(window.request_focus(content_edit(window, "classes")), "focus starts in Classes");
    require(key(window, PhysicalKey::f6), "F6 moves to the next region");
    require(active_is(workspace, "doc.second", "doc.second") &&
                (*site.stack_view(stacks[1])).contains(*window.focused_control()),
            "F6 enters the document area (its first Tab stop) and activates its document");
    require(key(window, PhysicalKey::f6) && active_is(workspace, "properties", "doc.second"), "F6 to Properties");
    require(key(window, PhysicalKey::f6) && active_is(workspace, "output", "doc.second"), "F6 to Output");
    require(key(window, PhysicalKey::f6) && active_is(workspace, "classes", "doc.second"), "F6 wraps to the left");
    require(key(window, PhysicalKey::f6, Modifier::shift) && active_is(workspace, "output", "doc.second"),
            "Shift+F6 goes back");

    // Escape in a tool panel returns to the active document, unless the
    // content uses it (a text box does).
    require(!key(window, PhysicalKey::escape) || active_is(workspace, "output", "doc.second"),
            "a text box keeps its Escape");
    require(active_is(workspace, "output", "doc.second"), "the text box's Escape leaves Output active");
    require(window.request_focus(window.find("content.output.action")), "Output's button takes focus");
    require(key(window, PhysicalKey::escape) && active_is(workspace, "doc.second", "doc.second") &&
                (*window.focused_control()).stable_id().value() == "content.doc.second.edit",
            "Escape in a tool panel returns to the active document");
    require(window.request_focus(window.find("content.doc.second.action")) && !key(window, PhysicalKey::escape) &&
                active_is(workspace, "doc.second", "doc.second"),
            "Escape in a document is left alone");
    // Shift+Escape hides the active tool panel and returns to the document.
    require(window.request_focus(window.find("content.output.action")) &&
                key(window, PhysicalKey::escape, Modifier::shift),
            "Shift+Escape is handled in Output");
    window.perform_layout();
    require(workspace.layout().locate(DockPanelId("output")).placement == DockPlacementKind::hidden &&
                active_is(workspace, "doc.second", "doc.second") &&
                (*window.focused_control()).stable_id().value() == "content.doc.second.action",
            "Shift+Escape hides the tool panel and focus returns to the active document's last control");
    require(window.focus_regions().size() == 3U, "the hidden panel's region leaves the list");

    // Ctrl+F6 moves through the documents in tab order, wrapping.
    require(key(window, PhysicalKey::f6, Modifier::control) && active_is(workspace, "doc.first", "doc.first"),
            "Ctrl+F6 activates the next document");
    require(key(window, PhysicalKey::f6, Modifier::control) && active_is(workspace, "doc.second", "doc.second"),
            "and wraps");
    require(key(window, PhysicalKey::f6, Modifier::control | Modifier::shift) &&
                active_is(workspace, "doc.first", "doc.first"),
            "Ctrl+Shift+F6 goes back");
}

// -- the position menu and the caption buttons ---------------------------------

void position_menu() {
    std::unique_ptr<Fixture> fixture = make_fixture();
    DockWorkspace& workspace = *(*fixture).workspace;
    DockSite& site = *(*fixture).site;
    Window& window = *(*fixture).window;
    const DockPanelId properties("properties");
    require(!site.position_command_enabled(properties, DockPositionCommand::float_panel) &&
                !site.position_command_enabled(properties, DockPositionCommand::auto_hide) &&
                !site.position_command_enabled(properties, DockPositionCommand::dock) &&
                site.position_command_enabled(properties, DockPositionCommand::dock_as_document) &&
                site.position_command_enabled(properties, DockPositionCommand::hide) &&
                site.position_command_enabled(properties, DockPositionCommand::close),
            "a docked tool panel may dock as a document, hide or close; Float and Auto Hide wait");
    require(!site.position_command_enabled(DockPanelId("output"), DockPositionCommand::dock_as_document),
            "a panel without dock-as-document may not join the documents");
    // Alt+Hyphen opens the active panel's menu.
    require(window.request_focus(content_edit(window, "properties")) && key(window, PhysicalKey::minus, Modifier::alt) &&
                site.position_menu_open(),
            "Alt+Hyphen opens the position menu");
    SemanticSnapshot snapshot = window.semantic_snapshot();
    const SemanticNode* floating = find_named(snapshot.roots, SemanticRole::menu_item, "Float");
    const SemanticNode* as_document = find_named(snapshot.roots, SemanticRole::menu_item, "Dock as Tabbed Document");
    require(floating != nullptr && !has_semantic_state((*floating).states, SemanticState::enabled) &&
                as_document != nullptr && has_semantic_state((*as_document).states, SemanticState::enabled),
            "the menu shows Float disabled and Dock as Tabbed Document enabled");
    require(window.perform_semantic_action((*as_document).stable_id, SemanticAction::press),
            "Dock as Tabbed Document is chosen");
    window.perform_layout();
    const DockNodeId stack = stack_of(workspace, "properties");
    require((*workspace.layout().find_node(stack)).kind == DockNodeKind::document_stack &&
                active_is(workspace, "properties", "") &&
                (*window.focused_control()).stable_id().value() == "content.properties.edit",
            "Properties stands among the documents and keeps focus");
    require(site.position_command_enabled(properties, DockPositionCommand::dock) &&
                site.run_position_command(properties, DockPositionCommand::dock),
            "Dock returns it to the tool stacks");
    window.perform_layout();
    require((*workspace.layout().find_node(stack_of(workspace, "properties"))).kind == DockNodeKind::tool_stack,
            "Properties is a tool stack again");
    // Shift+F10 opens the menu too; the context-menu key as well.
    site.close_position_menu();
    require(key(window, PhysicalKey::f10, Modifier::shift) && site.position_menu_open(), "Shift+F10 opens it");
    site.close_position_menu();
    require(key(window, PhysicalKey::application) && site.position_menu_open(), "the context-menu key opens it");
    site.close_position_menu();
    // The caption's close button hides the panel.
    window.perform_layout();
    const DockSite::Stack* output = find_stack(site.realized_stack_items(), stack_of(workspace, "output"));
    require(output != nullptr && (*output).parts.caption_close.present, "Output's caption has a close button");
    const Rect absolute = site.absolute_bounds();
    const Rect close = (*output).parts.caption_close.bounds;
    click(window, {absolute.x + close.x + close.width * 0.5, absolute.y + close.y + close.height * 0.5});
    require(workspace.layout().locate(DockPanelId("output")).placement == DockPlacementKind::hidden,
            "the caption's close hides the panel, keeping its placeholder");
    require(!site.show_position_menu(DockPanelId("output")), "a hidden panel has no position menu");
    require(workspace.show_panel(DockPanelId("output")).applied &&
                workspace.layout().locate(DockPanelId("output")).placement == DockPlacementKind::docked,
            "show puts it back");
    // The caption buttons are semantic children of the stack's pane.
    window.perform_layout();
    snapshot = window.semantic_snapshot();
    const std::string pane = site.stack_identity(stack_of(workspace, "output"));
    const SemanticNode* pane_node = find_node(snapshot.roots, pane);
    require(pane_node != nullptr && (*pane_node).role == SemanticRole::pane && (*pane_node).name == "Output" &&
                find_node((*pane_node).children, pane + ".menu") != nullptr &&
                find_node((*pane_node).children, pane + ".pin") != nullptr &&
                find_node((*pane_node).children, pane + ".close") != nullptr,
            "a tool stack is a pane named by its panel with its caption buttons");
    require(window.perform_semantic_action(pane + ".close", SemanticAction::press) &&
                workspace.layout().locate(DockPanelId("output")).placement == DockPlacementKind::hidden,
            "the close button's press hides the panel");
    bool threw = false;
    try {
        static_cast<void>(site.show_position_menu(properties, static_cast<MenuOpenMode>(9)));
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    require(threw && !site.position_menu_open(), "an unknown menu mode is refused");
}

// -- semantics -----------------------------------------------------------------

void semantic_snapshot() {
    std::unique_ptr<Fixture> fixture = make_fixture();
    DockWorkspace& workspace = *(*fixture).workspace;
    Window& window = *(*fixture).window;
    const SemanticSnapshot snapshot = window.semantic_snapshot();
    const SemanticNode* site = find_node(snapshot.roots, "site");
    require(site != nullptr && (*site).role == SemanticRole::group && (*site).name == "Docking workspace",
            "the site is a named group");
    const std::string documents = (*(*fixture).site).stack_identity(stack_of(workspace, "doc.first"));
    const SemanticNode* document_pane = find_node(snapshot.roots, documents);
    require(document_pane != nullptr && (*document_pane).role == SemanticRole::pane &&
                (*document_pane).name == "Documents" &&
                find_named((*document_pane).children, SemanticRole::tab, "Second.txt") != nullptr &&
                find_named((*document_pane).children, SemanticRole::tab_panel, "Second.txt") != nullptr,
            "the document area is a pane holding the documents' tabs and the selected tab panel");
    const SemanticNode* tab = find_named((*document_pane).children, SemanticRole::tab, "First.txt");
    require(tab != nullptr && std::find((*tab).actions.begin(), (*tab).actions.end(), SemanticAction::close) !=
                                  (*tab).actions.end(),
            "a document tab can be closed");
    require(window.perform_semantic_action((*tab).stable_id, SemanticAction::close) &&
                workspace.layout().locate(DockPanelId("doc.first")).placement == DockPlacementKind::absent,
            "a document tab's close closes the document");
    std::size_t seams = 0U;
    for (const SemanticNode& child : (*site).children) {
        if (child.role == SemanticRole::split_pane) ++seams;
    }
    require(seams == 3U, "each seam is a split pane");
}

// -- lifetime and refusals -----------------------------------------------------

void lifetime_and_refusals() {
    SaveJournal journal;
    PanelFactory factory(journal);
    std::unique_ptr<DockWorkspace> workspace = std::make_unique<DockWorkspace>("alone");
    require((*workspace).register_panel(document(factory, "doc.one", "One")) &&
                (*workspace).show_panel(DockPanelId("doc.one")).applied,
            "a document shows without a site");
    require((*workspace).active_panel() && (*(*workspace).active_panel()).value == "doc.one",
            "without a site show still records the activation");
    std::shared_ptr<DockSite> site = make_control<DockSite>(StableId("detached"), *workspace);
    require((*workspace).site() == site.get() && (*site).realized_stacks().size() == 1U,
            "a site binds to its workspace and realizes it");
    require(!(*site).open_navigator(DockNavigatorList::documents, 1, Modifier::control) &&
                !(*site).show_position_menu(DockPanelId("doc.one")),
            "a detached site opens no popup");
    require(factory.made.empty(), "a site never laid out creates nothing");
    // The workspace goes first: the site empties and stays usable.
    workspace.reset();
    require((*site).workspace() == nullptr && (*site).realized_stacks().empty(),
            "destroying the workspace unbinds the site");
    Window window(site, {400.0, 300.0});
    window.perform_layout();
    require((*site).realized_seams().empty() && !(*site).parts().document_area.present,
            "an unbound site lays out nothing");
    // A site destroyed first unbinds itself.
    DockWorkspace second("second");
    {
        std::shared_ptr<DockSite> transient = make_control<DockSite>(StableId("transient"), second);
        require(second.site() == transient.get(), "the second site binds");
    }
    require(second.site() == nullptr, "a destroyed site unbinds itself");
    // The navigator on its own.
    std::shared_ptr<DockNavigator> navigator = make_control<DockNavigator>(StableId("navigator"));
    bool threw = false;
    try {
        (*navigator).set_entries({DockNavigatorEntry{DockPanelId(""), "Nameless"}}, {});
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    require(threw && !(*navigator).current_index(), "an entry without an identity is refused");
    (*navigator).set_entries({}, {}, DockNavigatorList::documents);
    require(!(*navigator).choose() && !(*navigator).step(1) && !(*navigator).switch_list(),
            "an empty navigator chooses nothing");
    threw = false;
    try {
        (*navigator).set_release_modifier(Modifier::shift);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    require(threw, "shift is not a release modifier");
    (*navigator).set_entries({DockNavigatorEntry{DockPanelId("a"), "A"}},
                             {DockNavigatorEntry{DockPanelId("b"), "B"}, DockNavigatorEntry{DockPanelId("c"), "C"}},
                             DockNavigatorList::tools);
    require((*navigator).current_list() == DockNavigatorList::tools && (*navigator).current_index() == 0U &&
                (*navigator).switch_list() && (*navigator).current_list() == DockNavigatorList::documents &&
                !(*navigator).select(DockNavigatorList::documents, 5U),
            "the navigator selects within its lists");
    Window navigator_window(navigator, {460.0, 140.0});
    navigator_window.perform_layout();
    const std::vector<DockNavigator::Item> rows = (*navigator).realized_items();
    require(rows.size() == 3U && rows[1].states.selected && rows[1].panel.value == "b" &&
                rows[0].parts.entry_face.present && near(rows[0].parts.entry_face.bounds.height, 20.0) &&
                (*navigator).parts().tool_heading.present,
            "the navigator publishes its rows and headings");
}

} // namespace

int main() {
    anatomy_and_parts();
    lazy_creation();
    seam_keyboard_and_pointer();
    collapse_by_priority();
    activation_and_commands();
    navigator_order_and_keys();
    regions_and_escape_keys();
    position_menu();
    semantic_snapshot();
    lifetime_and_refusals();
    std::cout << "DockSite, DockWorkspace and DockNavigator: parts, seams, collapse, lazy creation, activation, "
                 "navigator, keys, menu and semantics passed\n";
    return EXIT_SUCCESS;
}
