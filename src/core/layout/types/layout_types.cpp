#include "gui_forms/layout/types/layout_types.hpp"

#include <cmath>

namespace gui_forms {
namespace {

[[nodiscard]] bool valid_available_extent(const AvailableExtent& axis) noexcept {
    const double extent = axis.extent.value();
    switch (axis.kind) {
    case AvailableExtentKind::definite:
    case AvailableExtentKind::at_most: return std::isfinite(extent) && extent >= 0.0;
    case AvailableExtentKind::min_content:
    case AvailableExtentKind::max_content: return extent == 0.0;
    }
    // A kind outside the enumeration is no constraint GUI.Forms can answer.
    return false;
}

[[nodiscard]] double measure_available_extent(const AvailableExtent& axis) noexcept {
    switch (axis.kind) {
    case AvailableExtentKind::definite:
    case AvailableExtentKind::at_most: return axis.extent.value();
    case AvailableExtentKind::min_content:
    case AvailableExtentKind::max_content: return practical_unbounded_extent;
    }
    return practical_unbounded_extent;
}

} // namespace

bool valid_layout_constraint(const LayoutConstraint& constraint) noexcept {
    const bool valid = valid_available_extent(constraint.width) &&
                       valid_available_extent(constraint.height);
    return valid;
}

Size measure_available_size(const LayoutConstraint& constraint) noexcept {
    const Size available{measure_available_extent(constraint.width),
                         measure_available_extent(constraint.height)};
    return available;
}

} // namespace gui_forms
