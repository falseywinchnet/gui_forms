#pragma once

#include "../../scrollable_control/scrollable_anatomy.hpp"

#include <array>

namespace gui_forms::group_box_detail {

// GroupBox and ScaledGroupBox: the scrollable parts, the face, the frame
// stroked below the caption, and the caption block.
inline constexpr std::array<PartDeclaration, 3> extra{{
    {.name = "face", .role = PartRole::face, .composed_from = {}},
    {.name = "edge", .role = PartRole::edge, .composed_from = {}},
    {.name = "caption", .role = PartRole::caption, .composed_from = {}},
}};
inline constexpr std::array<PartDeclaration, scrollable_detail::part_count + 3> parts =
    scrollable_detail::with_parts(extra);

} // namespace gui_forms::group_box_detail
