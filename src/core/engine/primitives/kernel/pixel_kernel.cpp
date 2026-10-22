#include "pixel_kernel.hpp"

#include <algorithm>
#include <cmath>

// Identical bytes on every host: no fused multiply-add (see engine_math.cpp).
#if defined(__clang__)
#pragma clang fp contract(off)
#endif

namespace gui_forms::engine_primitives {

Unit premultiply(Color colour, double coverage) noexcept {
    const double alpha = static_cast<double>(colour.alpha) / 255.0 * coverage;
    const Unit unit{static_cast<double>(colour.red) / 255.0 * alpha, static_cast<double>(colour.green) / 255.0 * alpha,
                    static_cast<double>(colour.blue) / 255.0 * alpha, alpha};
    return unit;
}

void blend_over(Unit& target, Unit source) noexcept {
    const double keep = 1.0 - source.alpha;
    target.red = source.red + target.red * keep;
    target.green = source.green + target.green * keep;
    target.blue = source.blue + target.blue * keep;
    target.alpha = source.alpha + target.alpha * keep;
}

std::uint8_t to_byte(double value) noexcept {
    const double clamped = std::clamp(value, 0.0, 1.0);
    const double scaled = std::floor(clamped * 255.0 + 0.5);
    const std::uint8_t byte = static_cast<std::uint8_t>(scaled);
    return byte;
}

void store(const PartRasterView& destination, std::size_t offset, const Unit& unit) noexcept {
    destination.pixels[offset] = to_byte(unit.red);
    destination.pixels[offset + 1U] = to_byte(unit.green);
    destination.pixels[offset + 2U] = to_byte(unit.blue);
    destination.pixels[offset + 3U] = to_byte(unit.alpha);
}

double keyline_width(double scale) noexcept {
    const double rounded = std::floor(std::min(scale, 2.0) + 0.5);
    const double width = std::max(1.0, rounded);
    return width;
}

double interval_overlap(double pixel, double start, double end) noexcept {
    const double low = std::max(pixel, start);
    const double high = std::min(pixel + 1.0, end);
    const double overlap = std::max(0.0, high - low);
    return overlap;
}

} // namespace gui_forms::engine_primitives
