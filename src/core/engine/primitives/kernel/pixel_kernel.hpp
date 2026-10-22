#pragma once

#include "gui_forms/engine/types/engine_types.hpp"
#include "gui_forms/types/paint_types/paint_types.hpp"

#include <cstddef>
#include <cstdint>

// The pixel arithmetic the engine's primitive kernels share (T2 plan,
// "Numerics and identical pixels"): premultiplied colour in double,
// source-over, one quantization to 8 bits, and pixel-interval coverage.
// Defined in pixel_kernel.cpp, compiled without fused multiply-add, so the
// same inputs give the same bytes on every host.

namespace gui_forms::engine_primitives {

// A premultiplied colour in [0, 1] per channel.
struct Unit final {
    double red{};
    double green{};
    double blue{};
    double alpha{};
};

// `colour` (straight alpha) at `coverage`, premultiplied.
[[nodiscard]] Unit premultiply(Color colour, double coverage) noexcept;

// Source-over of `source` onto `target`, both premultiplied.
void blend_over(Unit& target, Unit source) noexcept;

// One channel quantized once: floor(clamp(v, 0, 1) × 255 + 0.5).
[[nodiscard]] std::uint8_t to_byte(double value) noexcept;

// Writes `unit` as RGBA8 at byte `offset` of `destination`, which the
// caller has validated.
void store(const PartRasterView& destination, std::size_t offset, const Unit& unit) noexcept;

// The width of a line in device pixels at `scale` (design language §3.6,
// proposed): max(1, round(min(scale, 2))).
[[nodiscard]] double keyline_width(double scale) noexcept;

// How much of the pixel interval [pixel, pixel + 1) lies in [start, end).
[[nodiscard]] double interval_overlap(double pixel, double start, double end) noexcept;

} // namespace gui_forms::engine_primitives
