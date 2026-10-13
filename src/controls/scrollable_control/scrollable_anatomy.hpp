#pragma once

#include "gui_forms/anatomy.hpp"

#include <array>
#include <cstddef>

namespace gui_forms::scrollable_detail {

// Parts every ScrollableControl publishes: its region, the viewport granted
// to children, and the two scroll axes it paints itself. The axes reuse the
// scroll bar's roles, so a cue on the thumb role reaches these thumbs too.
inline constexpr std::size_t part_count = 12;
inline constexpr std::array<PartDeclaration, part_count> parts{{
    {.name = "region", .role = PartRole::region, .composed_from = {}},
    {.name = "viewport", .role = PartRole::region, .composed_from = {}},
    {.name = "horizontal-decrement-arrow", .role = PartRole::actuator, .composed_from = "button"},
    {.name = "horizontal-track", .role = PartRole::track, .composed_from = {}},
    {.name = "horizontal-thumb", .role = PartRole::thumb, .composed_from = {}},
    {.name = "horizontal-increment-arrow", .role = PartRole::actuator, .composed_from = "button"},
    {.name = "vertical-decrement-arrow", .role = PartRole::actuator, .composed_from = "button"},
    {.name = "vertical-track", .role = PartRole::track, .composed_from = {}},
    {.name = "vertical-thumb", .role = PartRole::thumb, .composed_from = {}},
    {.name = "vertical-increment-arrow", .role = PartRole::actuator, .composed_from = "button"},
    {.name = "scroll-corner", .role = PartRole::face, .composed_from = {}},
    {.name = "focus", .role = PartRole::focus, .composed_from = {}},
}};

inline constexpr std::array<AnatomyState, 7> states{
    AnatomyState::normal,
    AnatomyState::pressed,
    AnatomyState::captured,
    AnatomyState::enabled,
    AnatomyState::disabled,
    AnatomyState::focused,
    AnatomyState::focus_visible,
};

// The scrollable parts followed by a subclass's own parts, in that order.
template <std::size_t Extra>
constexpr std::array<PartDeclaration, part_count + Extra> with_parts(
    const std::array<PartDeclaration, Extra>& extra) {
    std::array<PartDeclaration, part_count + Extra> combined{};
    for (std::size_t index = 0; index < part_count; ++index) {
        combined[index] = parts[index];
    }
    for (std::size_t index = 0; index < Extra; ++index) {
        combined[part_count + index] = extra[index];
    }
    return combined;
}

// Panel paints a face and, when it has a border style, an edge.
inline constexpr std::array<PartDeclaration, 2> panel_extra{{
    {.name = "face", .role = PartRole::face, .composed_from = {}},
    {.name = "edge", .role = PartRole::edge, .composed_from = {}},
}};
inline constexpr std::array<PartDeclaration, part_count + 2> panel_parts =
    with_parts(panel_extra);

} // namespace gui_forms::scrollable_detail
