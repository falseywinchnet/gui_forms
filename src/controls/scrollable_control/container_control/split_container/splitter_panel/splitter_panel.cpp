#include "gui_forms/controls/scrollable_control/container_control/split_container/splitter_panel/splitter_panel.hpp"
#include "../../../scrollable_anatomy.hpp"

#include <array>
#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>

namespace gui_forms {

SplitterPanel::SplitterPanel(StableId stable_id)
    : ContainerControl(std::move(stable_id)) {}

void SplitterPanel::set_background(Color color) {
    require_mutable();
    if (background_ == color) return;
    background_ = color;
    invalidate(invalidation::style_only);
}

void SplitterPanel::on_paint(Painter& painter, Rect) {
    if (background_.alpha == 0U) return;
    const Rect bounds = committed_arranged_bounds();
    painter.fill_rect({0.0, 0.0, bounds.width, bounds.height}, background_);
}


namespace {

constexpr std::array<PartDeclaration, 1> splitter_panel_extra{{
    {.name = "face", .role = PartRole::face, .composed_from = {}},
}};
constexpr std::array<PartDeclaration, scrollable_detail::part_count + 1>
    splitter_panel_parts = scrollable_detail::with_parts(splitter_panel_extra);

constexpr ControlAnatomy splitter_panel_anatomy{
    .control_id = "splitter-panel",
    .parts = splitter_panel_parts,
    .states = scrollable_detail::states,
    .custom_states = {},
};

} // namespace

const ControlAnatomy& SplitterPanel::anatomy() const noexcept {
    return splitter_panel_anatomy;
}

void SplitterPanel::publish_part_geometry(std::span<PartGeometry> geometry) const {
    ContainerControl::publish_part_geometry(geometry);
    // The face exists only while the panel has an opaque background.
    if (background_.alpha != 0U) {
        set_part_geometry(anatomy(), geometry, "face", client_rectangle());
    }
}

} // namespace gui_forms
