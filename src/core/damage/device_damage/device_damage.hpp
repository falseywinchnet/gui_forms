#pragma once

#include "gui_forms/types.hpp"

#include <cmath>
#include <cstdint>

namespace gui_forms::detail {

// Owner decision L-Q8: layout publishes unsnapped logical boxes, and painting
// rounds each edge of a box (not its extent) to the nearest device-pixel
// boundary, halves upward. Two boxes that share a logical edge then share a
// device edge, with no seam and no overlap, at any scale. `device` is the
// edge in device pixels; a box narrower than a device pixel may round to
// nothing.
[[nodiscard]] inline double snap_device_edge(const double device) noexcept {
    const double snapped = std::floor(device + 0.5);
    return snapped;
}

// Internal host/renderer contract: repaint whole device pixels even when a
// retained control reports fractional logical bounds.
[[nodiscard]] inline Rect align_damage_outward(Rect rect, double scale) noexcept {
    if (rect.empty() || !std::isfinite(scale) || scale <= 0.0) {
        return {};
    }
    const double left = std::floor(rect.x * scale) / scale;
    const double top = std::floor(rect.y * scale) / scale;
    const double right = std::ceil((rect.x + rect.width) * scale) / scale;
    const double bottom = std::ceil((rect.y + rect.height) * scale) / scale;
    return {left, top, right - left, bottom - top};
}

// A presentation region in surface pixels, as window points. It widens by one
// surface pixel so a filtered, scaled draw also refreshes neighbours that
// sample changed pixels, then rounds outward to whole device pixels.
[[nodiscard]] inline Rect live_damage_in_window(
    const Rect damage, const Rect destination, const std::uint32_t width,
    const std::uint32_t height, const double scale) noexcept {
    if (damage.empty() || width == 0U || height == 0U || destination.empty() ||
        !destination.finite()) {
        return {};
    }
    const Rect surface{0.0, 0.0, static_cast<double>(width), static_cast<double>(height)};
    const Rect widened = Rect::intersection(
        {damage.x - 1.0, damage.y - 1.0, damage.width + 2.0, damage.height + 2.0}, surface);
    const double horizontal = destination.width / static_cast<double>(width);
    const double vertical = destination.height / static_cast<double>(height);
    const Rect mapped{destination.x + widened.x * horizontal, destination.y + widened.y * vertical,
                      widened.width * horizontal, widened.height * vertical};
    const Rect result = align_damage_outward(mapped, scale);
    return result;
}

} // namespace gui_forms::detail
