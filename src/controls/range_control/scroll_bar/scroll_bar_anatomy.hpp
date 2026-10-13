#pragma once

#include "gui_forms/anatomy.hpp"
#include "gui_forms/controls/range_control/scroll_bar/scroll_bar.hpp"

#include <array>

namespace gui_forms::scroll_bar_detail {

// Parts and states ScrollBar, HScrollBar and VScrollBar share. Each type
// publishes them under its own registry id.
inline constexpr std::array<PartField<ScrollBar::Parts>, 6> part_fields{{
    {.declaration = {.name = "region", .role = PartRole::region, .composed_from = {}},
     .member = &ScrollBar::Parts::region},
    {.declaration = {.name = "decrement-arrow", .role = PartRole::actuator, .composed_from = "button"},
     .member = &ScrollBar::Parts::decrement_arrow},
    {.declaration = {.name = "increment-arrow", .role = PartRole::actuator, .composed_from = "button"},
     .member = &ScrollBar::Parts::increment_arrow},
    {.declaration = {.name = "track", .role = PartRole::track, .composed_from = {}},
     .member = &ScrollBar::Parts::track},
    {.declaration = {.name = "thumb", .role = PartRole::thumb, .composed_from = {}},
     .member = &ScrollBar::Parts::thumb},
    {.declaration = {.name = "focus", .role = PartRole::focus, .composed_from = {}},
     .member = &ScrollBar::Parts::focus},
}};

inline constexpr std::array<StateField<ScrollBar::States>, 7> state_fields{{
    {.state = AnatomyState::normal, .member = &ScrollBar::States::normal},
    {.state = AnatomyState::pressed, .member = &ScrollBar::States::pressed},
    {.state = AnatomyState::captured, .member = &ScrollBar::States::captured},
    {.state = AnatomyState::enabled, .member = &ScrollBar::States::enabled},
    {.state = AnatomyState::disabled, .member = &ScrollBar::States::disabled},
    {.state = AnatomyState::focused, .member = &ScrollBar::States::focused},
    {.state = AnatomyState::focus_visible, .member = &ScrollBar::States::focus_visible},
}};

inline constexpr std::array<PartDeclaration, 6> parts =
    part_declarations(part_fields);
inline constexpr std::array<AnatomyState, 7> states =
    state_declarations(state_fields);

} // namespace gui_forms::scroll_bar_detail
