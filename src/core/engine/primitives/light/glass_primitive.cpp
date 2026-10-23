#include "glass_primitive.hpp"

#include "../kernel/pixel_kernel.hpp"

#include "gui_forms/engine/part_raster/part_raster.hpp"
#include "gui_forms/engine/part_shape/part_shape.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <new>
#include <optional>
#include <span>
#include <vector>

// Identical bytes on every host: no fused multiply-add (see engine_math.cpp).
#if defined(__clang__)
#pragma clang fp contract(off)
#endif

namespace gui_forms::engine_primitives {
namespace {

const PartRole glass_roles[] = {PartRole::region, PartRole::face, PartRole::well};

constexpr std::size_t glass_blur = 0U;
constexpr std::size_t glass_tint = 1U;
constexpr std::size_t glass_corner = 2U;

const PrimitiveParameterDeclaration glass_parameters[] = {
    {"blur", PrimitiveParameterKind::number, 0.0, 8.0, 6.0, {}},
    {"tint", PrimitiveParameterKind::colour, 0.0, 0.0, 0.0, {255, 255, 255, 64}},
    {"corner", PrimitiveParameterKind::number, 0.0, 64.0, 0.0, {}},
};

const PrimitiveDescriptor glass_descriptor{
    "glass", EngineVersion{0, 2}, glass_roles, {}, glass_parameters, PrimitiveCost{48U, 64U, 0U}, Insets{}, 0U};

// Four lanes, premultiplied RGBA, per pixel.
constexpr std::size_t lanes = 4U;

// One box pass: `lines` lines of `length` pixels, each pixel's lanes
// `node` elements apart and each line `line` elements apart (the same in
// the source and the result), averaged over windows of 2 × half + 1
// pixels into length − 2 × half pixels per line.
struct BoxPass final {
    std::size_t lines{};
    std::size_t length{};
    std::size_t line{};
    std::size_t node{};
};

// Runs `pass` from `from` into `to`, distinct buffers holding every
// element the pass reads or writes. Each average is rounded once.
void box_pass(std::span<const std::uint32_t> from, std::span<std::uint32_t> to, const BoxPass& pass,
              std::uint32_t half) noexcept {
    const std::size_t width = static_cast<std::size_t>(half) * 2U + 1U;
    const std::size_t outputs = pass.length - (width - 1U);
    for (std::size_t line = 0U; line < pass.lines; ++line) {
        const std::size_t first = line * pass.line;
        std::array<std::uint32_t, lanes> sums{};
        for (std::size_t node = 0U; node < width; ++node) {
            const std::size_t at = first + node * pass.node;
            for (std::size_t lane = 0U; lane < lanes; ++lane) sums[lane] = sums[lane] + from[at + lane];
        }
        for (std::size_t output = 0U; output < outputs; ++output) {
            const std::size_t write = first + output * pass.node;
            for (std::size_t lane = 0U; lane < lanes; ++lane) {
                to[write + lane] = (sums[lane] + half) / static_cast<std::uint32_t>(width);
            }
            if (output + 1U == outputs) break;
            // Slide the window one pixel: add the next, drop the first.
            const std::size_t entering = first + (output + width) * pass.node;
            const std::size_t leaving = first + output * pass.node;
            for (std::size_t lane = 0U; lane < lanes; ++lane) {
                sums[lane] = sums[lane] + from[entering + lane] - from[leaving + lane];
            }
        }
    }
}

// The blurred backplane under the destination: its pixels from the start
// of `pixels` (one of the two work buffers), rows `stride` elements apart.
struct Blurred final {
    std::span<const std::uint32_t> pixels{};
    std::size_t stride{};
};

// Blurs `backplane` (the destination, `width` pixels wide, grown by
// 3 × half on every side) through `first` and `second` (each sized to the
// backplane's pixels × 4), and says where the result lies.
Blurred blur_backplane(const PartRaster& backplane, std::uint32_t half, std::size_t width,
                       std::vector<std::uint32_t>& first, std::vector<std::uint32_t>& second) noexcept {
    const std::span<const std::uint8_t> bytes = backplane.pixels();
    const std::size_t columns = backplane.extent().width;
    const std::size_t rows = backplane.extent().height;
    const std::size_t stride = columns * lanes;
    for (std::size_t row = 0U; row < rows; ++row) {
        const std::size_t read = row * backplane.row_bytes();
        const std::size_t write = row * stride;
        for (std::size_t element = 0U; element < stride; ++element) first[write + element] = bytes[read + element];
    }
    if (half == 0U) return Blurred{first, stride};
    const std::size_t step = static_cast<std::size_t>(half) * 2U;
    // Across, three times: first → second → first → second.
    BoxPass across{rows, columns, stride, lanes};
    box_pass(first, second, across, half);
    across.length = across.length - step;
    box_pass(second, first, across, half);
    across.length = across.length - step;
    box_pass(first, second, across, half);
    // Down, three times over the destination's columns: second → first →
    // second → first.
    BoxPass down{width, rows, lanes, stride};
    box_pass(second, first, down, half);
    down.length = down.length - step;
    box_pass(first, second, down, half);
    down.length = down.length - step;
    box_pass(second, first, down, half);
    return Blurred{first, stride};
}

double unit_lane(std::uint32_t value) noexcept {
    const double unit = static_cast<double>(value) / 255.0;
    return unit;
}

} // namespace

std::uint32_t glass_box_half(double blur, double scale) noexcept {
    const double sigma = blur * scale * 0.5;
    const double width = std::sqrt(4.0 * sigma * sigma + 1.0);
    const double half = std::round((width - 1.0) * 0.5);
    if (!(half > 0.0)) return 0U;
    const std::uint32_t pixels = static_cast<std::uint32_t>(std::min(half, 1024.0));
    return pixels;
}

const PrimitiveDescriptor& GlassPrimitive::descriptor() const noexcept {
    return glass_descriptor;
}

PartShape GlassPrimitive::shape(const PrimitiveParameterBlock& parameters) const noexcept {
    PartShape rounded{};
    rounded.kind = PartShapeKind::rounded_rectangle;
    rounded.corner_radius = parameters.values[glass_corner].number;
    return rounded;
}

std::optional<std::uint32_t> GlassPrimitive::backplane_margin(const PrimitiveParameterBlock& parameters,
                                                              DeviceScale scale) const noexcept {
    const std::uint32_t half = glass_box_half(parameters.values[glass_blur].number, scale.value);
    const std::uint32_t margin = half * 3U;
    return margin;
}

void GlassPrimitive::derive(const PrimitiveParameterBlock&, const PrimitiveTarget&, std::span<double>) const noexcept {}

void GlassPrimitive::fill(const PrimitiveParameterBlock& parameters, const PrimitiveTarget& target,
                          std::span<const double>, const PartRasterView& destination) const noexcept {
    const double scale = target.scale.value;
    const std::size_t width = destination.extent.width;
    const std::size_t height = destination.extent.height;
    const Color tint = parameters.values[glass_tint].colour;
    PartShape shape = GlassPrimitive::shape(parameters);
    if (target.corner_radius) shape.corner_radius = std::max(0.0, *target.corner_radius);
    const Rect box{0.0, 0.0, target.logical_size.width, target.logical_size.height};

    // The blurred backplane, when one is lent at the margin this blur reads.
    const std::uint32_t half = glass_box_half(parameters.values[glass_blur].number, scale);
    const std::size_t margin = static_cast<std::size_t>(half) * 3U;
    std::vector<std::uint32_t> first{};
    std::vector<std::uint32_t> second{};
    Blurred blurred{};
    bool sampled = false;
    if (target.backplane != nullptr && target.backplane_margin == margin) {
        const PartRaster& backplane = *target.backplane;
        const bool fits = backplane.extent().width == width + margin * 2U &&
                          backplane.extent().height == height + margin * 2U;
        if (fits) {
            try {
                const std::size_t elements =
                    static_cast<std::size_t>(backplane.extent().width) * backplane.extent().height * lanes;
                first.assign(elements, 0U);
                second.assign(elements, 0U);
                blurred = blur_backplane(backplane, half, width, first, second);
                sampled = true;
            } catch (const std::bad_alloc&) {
                sampled = false;
            }
        }
    }

    const Unit over = premultiply(tint, 1.0);
    for (std::size_t row = 0U; row < height; ++row) {
        const std::size_t start = row * destination.row_bytes;
        for (std::size_t column = 0U; column < width; ++column) {
            Unit pixel{};
            if (sampled) {
                const std::size_t at = row * blurred.stride + column * lanes;
                pixel = Unit{unit_lane(blurred.pixels[at]), unit_lane(blurred.pixels[at + 1U]),
                             unit_lane(blurred.pixels[at + 2U]), unit_lane(blurred.pixels[at + 3U])};
            }
            blend_over(pixel, over);
            const double coverage = shape_coverage(shape, box, scale, static_cast<std::int64_t>(column),
                                                   static_cast<std::int64_t>(row));
            const Unit covered{pixel.red * coverage, pixel.green * coverage, pixel.blue * coverage,
                               pixel.alpha * coverage};
            store(destination, start + column * part_raster_bytes_per_pixel, covered);
        }
    }
}

} // namespace gui_forms::engine_primitives
