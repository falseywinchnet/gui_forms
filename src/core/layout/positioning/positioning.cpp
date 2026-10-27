// Positioning (stage L, phase L5): the arithmetic of CSS 2.1's
// absolutely positioned boxes, one axis at a time, and of relative offsets.

#include "gui_forms/layout/positioning/positioning.hpp"

#include <algorithm>

namespace gui_forms {

namespace {

double resolve_inset(const AutoLengthPercentage& inset, double basis) noexcept {
    return inset.value.length.value() + basis * inset.value.percent / 100.0;
}

} // namespace

Point relative_position_offset(const ItemPositioning& positioning, Size containing_block,
                               LayoutDirection direction) noexcept {
    Point offset{};
    if (positioning.position != PositionKind::relative) return offset;
    const BoxSides<AutoLengthPercentage>& inset = positioning.inset;
    const bool left = !inset.left.automatic;
    const bool right = !inset.right.automatic;
    const bool right_wins = right && (!left || direction == LayoutDirection::right_to_left);
    if (right_wins) {
        offset.x = -resolve_inset(inset.right, containing_block.width);
    } else if (left) {
        offset.x = resolve_inset(inset.left, containing_block.width);
    }
    if (!inset.top.automatic) {
        offset.y = resolve_inset(inset.top, containing_block.height);
    } else if (!inset.bottom.automatic) {
        offset.y = -resolve_inset(inset.bottom, containing_block.height);
    }
    return offset;
}

bool absolute_axis_reads_content(const AbsoluteAxisInput& input) noexcept {
    return !input.definite_size && (input.start_inset.automatic || input.end_inset.automatic);
}

AbsoluteAxisResult resolve_absolute_axis(const AbsoluteAxisInput& input) noexcept {
    const double extent = input.containing_extent;
    const bool start_set = !input.start_inset.automatic;
    const bool end_set = !input.end_inset.automatic;
    const double start = start_set ? resolve_inset(input.start_inset, extent) : 0.0;
    const double end = end_set ? resolve_inset(input.end_inset, extent) : 0.0;
    const double margin_start = input.auto_margin_start ? 0.0 : input.margin_start;
    const double margin_end = input.auto_margin_end ? 0.0 : input.margin_end;

    // The size, before the minimum and maximum: given; stretched between
    // both insets; or the content's (shrink-to-fit for a width).
    double size = 0.0;
    if (input.definite_size) {
        size = input.size;
    } else if (start_set && end_set) {
        size = extent - start - end - margin_start - margin_end;
    } else {
        const double available = extent - start - end - margin_start - margin_end;
        size = std::min(std::max(input.content_minimum, available), input.content_maximum);
    }
    size = std::max(input.minimum, std::min(size, input.maximum));
    size = std::max(0.0, size);

    AbsoluteAxisResult result{};
    result.size = size;
    if (!start_set && !end_set) {
        result.position = input.static_from_end ? input.static_position - margin_end - size
                                                : input.static_position + margin_start;
        return result;
    }
    if (start_set && end_set) {
        // Both insets and the size are fixed: auto margins share what is
        // left (CSS 2.1 10.3.7 and 10.6.4); with none, the box is
        // over-constrained and keeps its start inset (end_wins: its end).
        const double free = extent - start - end - size - margin_start - margin_end;
        if (input.auto_margin_start && input.auto_margin_end) {
            if (input.vertical || free >= 0.0) {
                result.position = start + free / 2.0;
            } else {
                result.position = input.end_wins ? start + free : start;
            }
        } else if (input.auto_margin_start) {
            result.position = start + free;
        } else if (input.auto_margin_end) {
            result.position = start + margin_start;
        } else {
            result.position = input.end_wins ? extent - end - margin_end - size : start + margin_start;
        }
        return result;
    }
    result.position = start_set ? start + margin_start : extent - end - margin_end - size;
    return result;
}

} // namespace gui_forms
