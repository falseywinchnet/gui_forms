// Vertical ToolStrips, ToolStripPanel and ToolStripContainer (stage W):
// vertical item layout and keys, panel stacking and orientation, and the
// container's four panels around its content.

#include "gui_forms/gui_forms.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using namespace gui_forms;

void require(bool condition, const char* message) {
    if (condition) return;
    std::cerr << message << '\n';
    std::exit(1);
}

struct RecordClick final {
    std::vector<std::string>* seen{};
    void operator()(const ToolStripItemClick& click) const { (*seen).push_back(click.item_id); }
};

const ToolStrip::Item* find_item(const std::vector<ToolStrip::Item>& items, std::string_view id) {
    for (const ToolStrip::Item& item : items) {
        if (item.stable_id == id) return &item;
    }
    return nullptr;
}
const ToolStrip::Item* find_item(std::vector<ToolStrip::Item>&& items, std::string_view id) = delete;

std::vector<ToolStripItemSpec> tools() {
    const std::shared_ptr<Command> choice = std::make_shared<Command>("choice", "Choice");
    std::vector<ToolStripItemSpec> items;
    items.push_back({.stable_id = "pen", .text = "Pen"});
    items.push_back({.stable_id = "rule", .kind = ToolStripItemKind::separator});
    items.push_back({.stable_id = "brush", .text = "Brush"});
    items.push_back({.stable_id = "shapes", .kind = ToolStripItemKind::drop_down_button, .text = "Shapes",
                     .drop_down = {MenuItemSpec("shapes.square", MenuItemKind::command, choice, "Square")}});
    return items;
}

std::shared_ptr<ToolStrip> strip_named(std::string_view id) {
    std::shared_ptr<ToolStrip> strip = make_control<ToolStrip>(StableId(std::string(id)));
    (*strip).set_items(tools());
    return strip;
}

void vertical_strip() {
    std::shared_ptr<ToolStrip> strip = strip_named("tools");
    (*strip).set_orientation(Orientation::vertical);
    require((*strip).orientation() == Orientation::vertical, "a strip turns vertical");
    const Size measured = (*strip).measure({400.0, 300.0});
    require(measured.width >= 25.0 && measured.width < 400.0 && measured.height == 300.0,
            "a vertical strip is as wide as its widest item and takes the height offered");
    Window window(strip, {measured.width, 300.0});
    window.perform_layout();
    const std::vector<ToolStrip::Item> items = (*strip).realized_items();
    const ToolStrip::Item* pen = find_item(items, "pen");
    const ToolStrip::Item* brush = find_item(items, "brush");
    const ToolStrip::Item* rule = find_item(items, "rule");
    require(pen != nullptr && brush != nullptr && rule != nullptr &&
                (*brush).parts.item_face.bounds.y > (*pen).parts.item_face.bounds.y &&
                (*brush).parts.item_face.bounds.x == (*pen).parts.item_face.bounds.x,
            "a vertical strip stacks its items top to bottom");
    require((*rule).parts.item_separator.bounds.width > (*rule).parts.item_separator.bounds.height,
            "a vertical strip's separator is a level rule");
    require((*strip).parts().grip.bounds.width > (*strip).parts().grip.bounds.height,
            "a vertical strip's grip lies across its top");

    std::vector<std::string> clicks;
    SubscriptionToken token = (*strip).item_clicked().subscribe(RecordClick{&clicks});
    require(window.request_focus(strip), "the strip takes focus");
    KeyEvent down{KeyAction::down, PhysicalKey::down};
    KeyEvent enter{KeyAction::down, PhysicalKey::enter};
    require(window.dispatch_key(down) && window.dispatch_key(enter) && clicks.size() == 1U && clicks[0] == "brush",
            "Down moves to the next item of a vertical strip");
    require(window.dispatch_key(down), "Down reaches the drop-down button");
    KeyEvent right{KeyAction::down, PhysicalKey::right};
    require(window.dispatch_key(right) && (*strip).drop_down_open(), "Right opens a vertical strip's menu");
    (*strip).close_drop_down();

    std::shared_ptr<StatusStrip> status = make_control<StatusStrip>(StableId("status"));
    bool threw = false;
    try {
        (*status).set_orientation(Orientation::vertical);
    } catch (const std::logic_error&) {
        threw = true;
    }
    require(threw && (*status).orientation() == Orientation::horizontal, "a StatusStrip stays horizontal");
}

void panels() {
    std::shared_ptr<ToolStripPanel> panel = make_control<ToolStripPanel>(StableId("panel"));
    require(validate_control_anatomy((*panel).anatomy()).defect == AnatomyDefect::none &&
                (*panel).anatomy().control_id == "tool_strip_panel",
            "ToolStripPanel publishes a valid anatomy");
    require((*panel).measure({500.0, 300.0}).height == 0.0, "an empty panel has no thickness");
    std::shared_ptr<ToolStrip> first = strip_named("first");
    std::shared_ptr<ToolStrip> second = strip_named("second");
    (*panel).join(first);
    (*panel).join(second);
    require((*panel).strips().size() == 2U && (*panel).measure({500.0, 300.0}).height == 50.0,
            "a top panel is as tall as its strips' rows together");
    Window window(panel, {500.0, 50.0});
    window.perform_layout();
    require((*first).committed_arranged_bounds() == Rect{0.0, 0.0, 500.0, 25.0} &&
                (*second).committed_arranged_bounds() == Rect{0.0, 25.0, 500.0, 25.0},
            "each strip takes its own row across the panel");
    bool threw = false;
    try {
        (*panel).join(first);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    require(threw, "a strip that already has a parent is refused");
    (*panel).set_edge(ToolStripPanelEdge::left);
    require((*first).orientation() == Orientation::vertical && (*second).orientation() == Orientation::vertical,
            "moving the panel to the left turns its strips vertical");
    std::shared_ptr<StatusStrip> status = make_control<StatusStrip>(StableId("status"));
    threw = false;
    try {
        (*panel).join(status);
    } catch (const std::logic_error&) {
        threw = true;
    }
    require(threw, "a StatusStrip cannot join a left panel");
    const std::shared_ptr<ToolStrip> removed = (*panel).remove(*second);
    require(removed == second && (*panel).strips().size() == 1U && !(*second).parent(),
            "remove hands a strip back");
    require((*panel).remove(*second) == nullptr, "removing a strip that is not joined returns null");
}

void container() {
    std::shared_ptr<ToolStripContainer> frame = make_control<ToolStripContainer>(StableId("frame"));
    require(validate_control_anatomy((*frame).anatomy()).defect == AnatomyDefect::none &&
                (*frame).anatomy().control_id == "tool_strip_container",
            "ToolStripContainer publishes a valid anatomy");
    require((*(*frame).top_panel()).stable_id().value() == "frame.top" &&
                (*(*frame).left_panel()).edge() == ToolStripPanelEdge::left &&
                (*(*frame).content_panel()).stable_id().value() == "frame.content",
            "the container owns its four panels and its content panel");
    std::shared_ptr<ToolStrip> top = strip_named("top-tools");
    std::shared_ptr<ToolStrip> left = strip_named("left-tools");
    std::shared_ptr<StatusStrip> status = make_control<StatusStrip>(StableId("status"));
    (*status).set_items({{.stable_id = "ready", .kind = ToolStripItemKind::label, .text = "Ready"}});
    (*(*frame).top_panel()).join(top);
    (*(*frame).left_panel()).join(left);
    (*(*frame).bottom_panel()).join(status);
    std::shared_ptr<Button> body = make_control<Button>(StableId("body"), "Body");
    (*body).set_dock(DockStyle::fill);
    (*(*frame).content_panel()).add_child(body);
    Window window(frame, {600.0, 400.0});
    window.perform_layout();
    ToolStripContainer::Parts parts = (*frame).parts();
    require(parts.top_panel.present && parts.top_panel.bounds == Rect{0.0, 0.0, 600.0, 25.0},
            "the top panel spans the width, as tall as its strip");
    require(parts.bottom_panel.present && parts.bottom_panel.bounds.y + parts.bottom_panel.bounds.height == 400.0,
            "the bottom panel sits on the bottom edge");
    require(parts.left_panel.present && parts.left_panel.bounds.x == 0.0 && parts.left_panel.bounds.y == 25.0 &&
                parts.left_panel.bounds.width >= 25.0 && (*left).orientation() == Orientation::vertical,
            "the left panel holds a vertical strip between the top and bottom panels");
    require(!parts.right_panel.present, "an empty right panel takes no room");
    require(parts.content_panel.present &&
                parts.content_panel.bounds.x == parts.left_panel.bounds.width &&
                parts.content_panel.bounds.y == 25.0 &&
                parts.content_panel.bounds.width == 600.0 - parts.left_panel.bounds.width,
            "the content panel takes what the panels leave");
    (*frame).set_top_panel_visible(false);
    window.perform_layout();
    parts = (*frame).parts();
    require(!parts.top_panel.present && parts.content_panel.bounds.y == 0.0 && !(*top).effectively_visible(),
            "a hidden panel takes no room and hides its strips");
    require((*frame).semantic_descriptor().role == SemanticRole::group, "the container is a group");
}

} // namespace

int main() {
    vertical_strip();
    panels();
    container();
    std::cout << "Vertical ToolStrip, ToolStripPanel and ToolStripContainer: layout, keys, orientation, "
                 "panels and content passed\n";
    return 0;
}
