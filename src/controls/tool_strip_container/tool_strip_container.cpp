#include "gui_forms/controls/tool_strip_container/tool_strip_container.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <utility>

namespace gui_forms {
namespace {

// A measured thickness, with non-finite or negative answers as zero.
double usable(double thickness) noexcept {
    if (!std::isfinite(thickness) || thickness < 0.0) return 0.0;
    return thickness;
}

} // namespace

ToolStripContainer::ToolStripContainer(StableId stable_id) : Control(std::move(stable_id)) {
    set_paint_plane(PaintPlane::control);
}

void ToolStripContainer::initialize_control_tree() {
    const std::string prefix(stable_id().value());
    top_ = make_control<ToolStripPanel>(StableId(prefix + ".top"), ToolStripPanelEdge::top);
    bottom_ = make_control<ToolStripPanel>(StableId(prefix + ".bottom"), ToolStripPanelEdge::bottom);
    left_ = make_control<ToolStripPanel>(StableId(prefix + ".left"), ToolStripPanelEdge::left);
    right_ = make_control<ToolStripPanel>(StableId(prefix + ".right"), ToolStripPanelEdge::right);
    content_ = make_control<Panel>(StableId(prefix + ".content"));
    // The content panel comes first, so Tab reaches the content before the
    // strips.
    add_child(content_);
    add_child(top_);
    add_child(left_);
    add_child(right_);
    add_child(bottom_);
}

void ToolStripContainer::set_panel_visible(bool& flag, bool visible) {
    require_mutable();
    if (flag == visible) return;
    flag = visible;
    invalidate(Dirty::arrange | Dirty::paint | Dirty::hit_test | Dirty::semantics);
}

void ToolStripContainer::set_top_panel_visible(bool visible) {
    set_panel_visible(top_visible_, visible);
}

void ToolStripContainer::set_bottom_panel_visible(bool visible) {
    set_panel_visible(bottom_visible_, visible);
}

void ToolStripContainer::set_left_panel_visible(bool visible) {
    set_panel_visible(left_visible_, visible);
}

void ToolStripContainer::set_right_panel_visible(bool visible) {
    set_panel_visible(right_visible_, visible);
}

// The top and bottom panels take the height their strips need across the
// whole width; the left and right panels take the width theirs need over the
// height between; the content panel takes what is left.
ToolStripContainer::Layout ToolStripContainer::layout_for(Size client) {
    Layout placed;
    const double width = std::max(0.0, client.width);
    const double height = std::max(0.0, client.height);
    double top = 0.0;
    double bottom = 0.0;
    if (top_ && top_visible_) top = std::min(height, usable((*top_).measure({width, height}).height));
    if (bottom_ && bottom_visible_) {
        bottom = std::min(height - top, usable((*bottom_).measure({width, height - top}).height));
    }
    const double middle = std::max(0.0, height - top - bottom);
    double left = 0.0;
    double right = 0.0;
    if (left_ && left_visible_) left = std::min(width, usable((*left_).measure({width, middle}).width));
    if (right_ && right_visible_) {
        right = std::min(width - left, usable((*right_).measure({width - left, middle}).width));
    }
    placed.top = {0.0, 0.0, width, top};
    placed.bottom = {0.0, height - bottom, width, bottom};
    placed.left = {0.0, top, left, middle};
    placed.right = {width - right, top, right, middle};
    placed.content = {left, top, std::max(0.0, width - left - right), middle};
    return placed;
}

void ToolStripContainer::arrange(Rect final_bounds) {
    arrange_self(final_bounds);
    const Layout placed = layout_for({final_bounds.width, final_bounds.height});
    if (!is_alive()) return;
    const std::array<std::pair<Control::Ptr, Rect>, 4> panels{{
        {top_, placed.top},
        {bottom_, placed.bottom},
        {left_, placed.left},
        {right_, placed.right},
    }};
    const std::array<bool, 4> shown{{top_visible_, bottom_visible_, left_visible_, right_visible_}};
    for (std::size_t index = 0; index < panels.size(); ++index) {
        const Control::Ptr& panel = panels[index].first;
        if (!panel || !is_current_layout_child(panel)) continue;
        set_child_layout_collapsed(panel, !shown[index]);
        if (shown[index]) set_child_layout(panel, panels[index].second);
    }
    if (content_ && is_current_layout_child(content_)) set_child_layout(content_, placed.content);
}

SemanticDescriptor ToolStripContainer::semantic_descriptor() const {
    // A layout frame: the strips and the content describe themselves.
    SemanticDescriptor descriptor = Control::semantic_descriptor();
    descriptor.role = SemanticRole::group;
    descriptor.exposed = !accessible_name().empty();
    return descriptor;
}

namespace {

constexpr std::array<PartField<ToolStripContainer::Parts>, 6> tool_strip_container_part_fields{{
    {.declaration = {.name = "region", .role = PartRole::region, .composed_from = {}},
     .member = &ToolStripContainer::Parts::region},
    {.declaration = {.name = "top_panel", .role = PartRole::face, .composed_from = "tool_strip_panel"},
     .member = &ToolStripContainer::Parts::top_panel},
    {.declaration = {.name = "bottom_panel", .role = PartRole::face, .composed_from = "tool_strip_panel"},
     .member = &ToolStripContainer::Parts::bottom_panel},
    {.declaration = {.name = "left_panel", .role = PartRole::face, .composed_from = "tool_strip_panel"},
     .member = &ToolStripContainer::Parts::left_panel},
    {.declaration = {.name = "right_panel", .role = PartRole::face, .composed_from = "tool_strip_panel"},
     .member = &ToolStripContainer::Parts::right_panel},
    {.declaration = {.name = "content_panel", .role = PartRole::region, .composed_from = "panel"},
     .member = &ToolStripContainer::Parts::content_panel},
}};

constexpr std::array<PartDeclaration, 6> tool_strip_container_parts =
    part_declarations(tool_strip_container_part_fields);
constexpr std::array<AnatomyState, 5> tool_strip_container_states{
    AnatomyState::normal, AnatomyState::enabled, AnatomyState::disabled,
    AnatomyState::focused, AnatomyState::focus_visible,
};

constexpr ControlAnatomy tool_strip_container_anatomy{
    .control_id = "tool_strip_container",
    .parts = tool_strip_container_parts,
    .states = tool_strip_container_states,
    .custom_states = {},
    .item_systems = {},
};

// A panel's published geometry: where it was placed, while it is shown.
PartGeometry panel_geometry(const Control::Ptr& panel, bool shown) {
    if (!panel || !shown) return PartGeometry{};
    return PartGeometry::from_bounds((*panel).committed_arranged_bounds());
}

} // namespace

const ControlAnatomy& ToolStripContainer::anatomy() const noexcept {
    return tool_strip_container_anatomy;
}

void ToolStripContainer::publish_part_geometry(std::span<PartGeometry> geometry) const {
    Parts record;
    record.region = PartGeometry::from_bounds(client_rectangle());
    record.top_panel = panel_geometry(top_, top_visible_);
    record.bottom_panel = panel_geometry(bottom_, bottom_visible_);
    record.left_panel = panel_geometry(left_, left_visible_);
    record.right_panel = panel_geometry(right_, right_visible_);
    record.content_panel = panel_geometry(content_, true);
    write_parts(anatomy(), tool_strip_container_part_fields, record, geometry);
}

ToolStripContainer::Parts ToolStripContainer::parts() const {
    const PartGeometryBuffer published = published_part_geometry();
    Parts record;
    read_parts(anatomy(), tool_strip_container_part_fields, published.parts(), record);
    return record;
}

} // namespace gui_forms
