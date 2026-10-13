#pragma once

#include "gui_forms/anatomy.hpp"
#include "gui_forms/controls/scrollable_control/scrollable_control.hpp"

#include <array>
#include <cstddef>

namespace gui_forms::scrollable_detail {

// A thumb is pressed and captures the pointer while dragged.
inline constexpr std::array<AnatomyState, 2> thumb_part_states{{
    AnatomyState::pressed, AnatomyState::captured}};

// Parts every ScrollableControl publishes: its region, the viewport granted
// to children, and the two scroll axes it paints itself. The axes reuse the
// scroll bar's roles, so a cue on the thumb role reaches these thumbs too.
inline constexpr std::array<PartField<ScrollableControl::Parts>, 12> part_fields{{
    {.declaration = {.name = "region", .role = PartRole::region, .composed_from = {}},
     .member = &ScrollableControl::Parts::region},
    {.declaration = {.name = "viewport", .role = PartRole::region, .composed_from = {}},
     .member = &ScrollableControl::Parts::viewport},
    {.declaration = {.name = "horizontal-decrement-arrow", .role = PartRole::actuator, .composed_from = "button"},
     .member = &ScrollableControl::Parts::horizontal_decrement_arrow},
    {.declaration = {.name = "horizontal-track", .role = PartRole::track, .composed_from = {}},
     .member = &ScrollableControl::Parts::horizontal_track},
    {.declaration = {.name = "horizontal-thumb", .role = PartRole::thumb, .composed_from = {},
                     .states = thumb_part_states},
     .member = &ScrollableControl::Parts::horizontal_thumb},
    {.declaration = {.name = "horizontal-increment-arrow", .role = PartRole::actuator, .composed_from = "button"},
     .member = &ScrollableControl::Parts::horizontal_increment_arrow},
    {.declaration = {.name = "vertical-decrement-arrow", .role = PartRole::actuator, .composed_from = "button"},
     .member = &ScrollableControl::Parts::vertical_decrement_arrow},
    {.declaration = {.name = "vertical-track", .role = PartRole::track, .composed_from = {}},
     .member = &ScrollableControl::Parts::vertical_track},
    {.declaration = {.name = "vertical-thumb", .role = PartRole::thumb, .composed_from = {},
                     .states = thumb_part_states},
     .member = &ScrollableControl::Parts::vertical_thumb},
    {.declaration = {.name = "vertical-increment-arrow", .role = PartRole::actuator, .composed_from = "button"},
     .member = &ScrollableControl::Parts::vertical_increment_arrow},
    {.declaration = {.name = "scroll-corner", .role = PartRole::face, .composed_from = {}},
     .member = &ScrollableControl::Parts::scroll_corner},
    {.declaration = {.name = "focus", .role = PartRole::focus, .composed_from = {}},
     .member = &ScrollableControl::Parts::focus},
}};

inline constexpr std::array<StateField<ScrollableControl::States>, 7> state_fields{{
    {.state = AnatomyState::normal, .member = &ScrollableControl::States::normal},
    {.state = AnatomyState::pressed, .member = &ScrollableControl::States::pressed},
    {.state = AnatomyState::captured, .member = &ScrollableControl::States::captured},
    {.state = AnatomyState::enabled, .member = &ScrollableControl::States::enabled},
    {.state = AnatomyState::disabled, .member = &ScrollableControl::States::disabled},
    {.state = AnatomyState::focused, .member = &ScrollableControl::States::focused},
    {.state = AnatomyState::focus_visible, .member = &ScrollableControl::States::focus_visible},
}};

inline constexpr std::size_t part_count = 12;
inline constexpr std::array<PartDeclaration, part_count> parts =
    part_declarations(part_fields);
inline constexpr std::array<AnatomyState, 7> states =
    state_declarations(state_fields);

} // namespace gui_forms::scrollable_detail
