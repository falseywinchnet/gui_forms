#include "gui_forms/controls/button_base/link_label/link_label.hpp"
#include "../../basic/basic_control_rendering.hpp"
#include "gui_forms/text.hpp"
#include "gui_forms/window.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <vector>

namespace gui_forms {

LinkLabel::LinkLabel(StableId stable_id, std::string text)
    : ButtonBase(std::move(stable_id), std::move(text)) {}

void LinkLabel::set_visited(bool visited_value) {
    require_mutable();
    if (visited_ == visited_value) {
        return;
    }
    visited_ = visited_value;
    invalidate(Dirty::paint | Dirty::semantics);
}

namespace {

constexpr std::array<PartDeclaration, 4> link_label_parts{{
    {.name = "region", .role = PartRole::region, .composed_from = {}},
    {.name = "label", .role = PartRole::label, .composed_from = {}},
    {.name = "underline", .role = PartRole::glyph, .composed_from = {}},
    {.name = "focus", .role = PartRole::focus, .composed_from = {}},
}};

constexpr std::array<AnatomyState, 7> link_label_states{
    AnatomyState::normal,
    AnatomyState::hot,
    AnatomyState::pressed,
    AnatomyState::enabled,
    AnatomyState::disabled,
    AnatomyState::focused,
    AnatomyState::focus_visible,
};

// "visited" has no counterpart in the design language's state set, so the
// link declares it as its own finite state.
constexpr std::array<CustomStateDeclaration, 1> link_label_custom_states{{
    {.name = "visited"},
}};

constexpr CustomStateIndex visited_state{0U};

constexpr ControlAnatomy link_label_anatomy{
    .control_id = "link-label",
    .parts = link_label_parts,
    .states = link_label_states,
    .custom_states = link_label_custom_states,
};

} // namespace

const ControlAnatomy& LinkLabel::anatomy() const noexcept {
    return link_label_anatomy;
}

double LinkLabel::text_baseline() const noexcept {
    const Rect bounds = local_bounds();
    const double baseline = std::max(font().size,
        (bounds.height + font().size) * 0.5 - 1.0);
    return baseline;
}

double LinkLabel::underline_width() const {
    const Rect bounds = local_bounds();
    const double width = std::min(bounds.width - 4.0,
                                  estimated_text_width(display_text(), font()));
    return std::max(0.0, width);
}

void LinkLabel::publish_part_geometry(std::span<PartGeometry> geometry) const {
    const ControlAnatomy& declared = anatomy();
    const Rect bounds = local_bounds();
    const double baseline = text_baseline();
    const double width = underline_width();
    // The label spans one font size above the baseline; the underline is the
    // one-unit stroke two units below it.
    const Rect label{2.0, baseline - font().size, width, font().size};
    const Rect underline{2.0, baseline + 1.5, width, 1.0};
    set_part_geometry(declared, geometry, "region", bounds);
    set_part_geometry(declared, geometry, "label", label);
    set_part_geometry(declared, geometry, "underline", underline);
    set_part_geometry(declared, geometry, "focus", bounds);
}

AnatomyStateSet LinkLabel::anatomy_states() const {
    AnatomyStateSet states = button_anatomy_states();
    if (visited_) {
        states.insert(visited_state);
    }
    return states;
}

void LinkLabel::on_paint(Painter& painter, Rect) {
    const Rect bounds = local_bounds();
    const BasicControlStyle& colors = style();
    const Color foreground = enabled()
        ? (visited_ ? colors.visited_link : colors.link) : colors.disabled_text;
    const double baseline = text_baseline();
    const double width = underline_width();
    painter.draw_text_utf8({2.0, baseline}, display_text(), font(), foreground);
    painter.draw_line({2.0, baseline + 2.0}, {2.0 + width, baseline + 2.0},
                      foreground, 1.0);
    if (focus_cue_visible()) {
        paint_focus(painter, bounds, foreground);
    }
}

void LinkLabel::on_activate() {
    set_visited(true);
    if (is_alive()) {
        ButtonBase::on_activate();
    }
}

SemanticDescriptor LinkLabel::semantic_descriptor() const {
    SemanticDescriptor descriptor = ButtonBase::semantic_descriptor();
    descriptor.role = SemanticRole::link;
    descriptor.value = visited_ ? "visited" : "unvisited";
    return descriptor;
}

} // namespace gui_forms
