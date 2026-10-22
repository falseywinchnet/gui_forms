#pragma once

#include "gui_forms/control.hpp"

#include <algorithm>
#include <cmath>

namespace gui_forms::container_layout_detail {

[[nodiscard]] inline double horizontal_extent(Insets insets) noexcept {
    return insets.left + insets.right;
}

[[nodiscard]] inline double vertical_extent(Insets insets) noexcept {
    return insets.top + insets.bottom;
}

[[nodiscard]] inline Size preferred_child_size(
    const Control::Ptr& child, Size available) {
    const Rect requested = (*child).requested_bounds();
    const Size measure_available{
        std::max(std::max(0.0, available.width), requested.width),
        std::max(std::max(0.0, available.height), requested.height)};
    Size result = (*child).measure(measure_available);
    if (!std::isfinite(result.width) || result.width < 0.0) result.width = 0.0;
    if (!std::isfinite(result.height) || result.height < 0.0) result.height = 0.0;
    return result;
}

// Where an undocked child `width` by `height` sits in `area` by its anchors:
// both opposite anchors stretch it, one keeps it to that side, and none
// centres it.
[[nodiscard]] inline Rect place_by_anchor(AnchorStyles anchor, Rect area, double width,
                                          double height) noexcept {
    const bool left = has_anchor(anchor, AnchorStyles::left);
    const bool right = has_anchor(anchor, AnchorStyles::right);
    const bool top = has_anchor(anchor, AnchorStyles::top);
    const bool bottom = has_anchor(anchor, AnchorStyles::bottom);
    Rect placed{area.x, area.y, width, height};
    if (left && right) placed.width = area.width;
    else if (right) placed.x += area.width - width;
    else if (!left) placed.x += (area.width - width) * 0.5;
    if (top && bottom) placed.height = area.height;
    else if (bottom) placed.y += area.height - height;
    else if (!top) placed.y += (area.height - height) * 0.5;
    return placed;
}

// Where a child of `desired` size sits in `area`, its cell less its margins:
// docked, it spans the area along its side (fill takes all of it);
// undocked, its anchors place it.
[[nodiscard]] inline Rect place_in_cell(DockStyle dock, AnchorStyles anchor, Rect area,
                                        Size desired) noexcept {
    const double width = std::min(desired.width, area.width);
    const double height = std::min(desired.height, area.height);
    switch (dock) {
    case DockStyle::fill:
        return area;
    case DockStyle::top: {
        const Rect placed{area.x, area.y, area.width, height};
        return placed;
    }
    case DockStyle::bottom: {
        const Rect placed{area.x, area.y + area.height - height, area.width, height};
        return placed;
    }
    case DockStyle::left: {
        const Rect placed{area.x, area.y, width, area.height};
        return placed;
    }
    case DockStyle::right: {
        const Rect placed{area.x + area.width - width, area.y, width, area.height};
        return placed;
    }
    case DockStyle::none:
        return place_by_anchor(anchor, area, width, height);
    }
    // A style outside the enumeration keeps the child at its desired size
    // in the area's corner.
    const Rect placed{area.x, area.y, width, height};
    return placed;
}

} // namespace gui_forms::container_layout_detail
