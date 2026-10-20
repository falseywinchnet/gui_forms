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

// One side panel's placement for arrange: the panel, its rectangle and
// whether it is shown.
struct PanelPlacement final {
    Control::Ptr panel{};
    Rect bounds{};
    bool shown{};
};

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
    // Each panel is offered what the panels before it left, and takes at
    // most that.
    double top = 0.0;
    if (top_ && top_visible_) {
        const Size wanted = (*top_).measure({width, height});
        top = std::min(height, usable(wanted.height));
    }
    double bottom = 0.0;
    if (bottom_ && bottom_visible_) {
        const Size wanted = (*bottom_).measure({width, height - top});
        bottom = std::min(height - top, usable(wanted.height));
    }
    const double middle = std::max(0.0, height - top - bottom);
    double left = 0.0;
    if (left_ && left_visible_) {
        const Size wanted = (*left_).measure({width, middle});
        left = std::min(width, usable(wanted.width));
    }
    double right = 0.0;
    if (right_ && right_visible_) {
        const Size wanted = (*right_).measure({width - left, middle});
        right = std::min(width - left, usable(wanted.width));
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
    // Measuring the panels measures their strips, whose program code may
    // dispose this container or take a panel out of it.
    if (!is_alive()) return;
    const std::array<PanelPlacement, 4> panels{{
        {.panel = top_, .bounds = placed.top, .shown = top_visible_},
        {.panel = bottom_, .bounds = placed.bottom, .shown = bottom_visible_},
        {.panel = left_, .bounds = placed.left, .shown = left_visible_},
        {.panel = right_, .bounds = placed.right, .shown = right_visible_},
    }};
    for (const PanelPlacement& placement : panels) {
        if (!placement.panel || !is_current_layout_child(placement.panel)) continue;
        set_child_layout_collapsed(placement.panel, !placement.shown);
        if (placement.shown) set_child_layout(placement.panel, placement.bounds);
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
