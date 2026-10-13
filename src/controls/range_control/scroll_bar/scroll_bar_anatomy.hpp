#pragma once

#include "gui_forms/anatomy.hpp"

#include <array>

namespace gui_forms::scroll_bar_detail {

// Parts and states ScrollBar, HScrollBar and VScrollBar share. Each type
// publishes them under its own registry id; ScrollBar::publish_part_geometry
// fills them by index, so the order here is part of that contract.
inline constexpr std::array<PartDeclaration, 6> parts{{
    {.name = "region", .role = PartRole::region, .composed_from = {}},
    {.name = "decrement-arrow", .role = PartRole::actuator, .composed_from = "button"},
    {.name = "increment-arrow", .role = PartRole::actuator, .composed_from = "button"},
    {.name = "track", .role = PartRole::track, .composed_from = {}},
    {.name = "thumb", .role = PartRole::thumb, .composed_from = {}},
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

} // namespace gui_forms::scroll_bar_detail
