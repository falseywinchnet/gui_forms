#include "glass_primitive.hpp"

#include "../kernel/box_blur.hpp"
#include "../kernel/pixel_kernel.hpp"

#include "gui_forms/engine/part_raster/part_raster.hpp"
#include "gui_forms/engine/part_shape/part_shape.hpp"
#include "gui_forms/surface_material/types/surface_material_types.hpp"
#include "gui_forms/theme/types/theme_types.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <optional>
#include <span>
#include <string_view>
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
constexpr std::size_t glass_recipe = 3U;

// `none`, then the theme's recipe roles in enum order (E9.6): the recipe
// whose material's colour filter glass applies to its backdrop.
constexpr std::array<std::string_view, 15> glass_recipe_names{
    "none", "window", "panel", "card", "button", "accent_button", "command_button",
    "choice", "editor", "menu_item", "selection", "progress", "tool_tip",
    "link", "visited_link"};
static_assert(glass_recipe_names.size() == static_cast<std::size_t>(ControlVisualRole::count) + 1U,
              "none, then one choice per recipe role");

const PrimitiveParameterDeclaration glass_parameters[] = {
    {"blur", PrimitiveParameterKind::number, 0.0, 8.0, 6.0, {}},
    {"tint", PrimitiveParameterKind::colour, 0.0, 0.0, 0.0, {255, 255, 255, 64}},
    {"corner", PrimitiveParameterKind::number, 0.0, 64.0, 0.0, {}},
    {"recipe", PrimitiveParameterKind::choice, 0.0, 0.0, 0.0, {}, false, glass_recipe_names},
};

const PrimitiveDescriptor glass_descriptor{
    "glass", EngineVersion{0, 2}, glass_roles, {}, glass_parameters, PrimitiveCost{48U, 64U, 0U}, Insets{}, 0U};

constexpr std::size_t lanes = box_blur_lanes;

// The blurred backplane under the destination: its pixels from the start
// of `pixels` (the work's first image), rows `stride` elements apart.
struct Blurred final {
    std::span<const std::uint32_t> pixels{};
    std::size_t stride{};
};

// Blurs `backplane` (the destination grown by 3 × half on every side)
// through `work` (prepared for the backplane's pixels, four lanes), and
// says where the result lies.
Blurred blur_backplane(const PartRaster& backplane, std::uint32_t half, BoxBlurWork& work) noexcept {
    const std::span<const std::uint8_t> bytes = backplane.pixels();
    const std::size_t columns = backplane.extent().width;
    const std::size_t rows = backplane.extent().height;
    const std::size_t stride = columns * lanes;
    for (std::size_t row = 0U; row < rows; ++row) {
        const std::size_t read = row * backplane.row_bytes();
        const std::size_t write = row * stride;
        for (std::size_t element = 0U; element < stride; ++element) {
            work.first[write + element] = bytes[read + element];
        }
    }
    blur_three_boxes(work, columns, rows, lanes, half);
    return Blurred{work.first, stride};
}

double unit_lane(std::uint32_t value) noexcept {
    const double unit = static_cast<double>(value) / 255.0;
    return unit;
}

} // namespace

std::uint32_t glass_box_half(double blur, double scale) noexcept {
    const std::uint32_t half = box_blur_half(blur, scale);
    return half;
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

std::optional<ControlVisualRole> GlassPrimitive::theme_material(
    const PrimitiveParameterBlock& parameters) const noexcept {
    const double position = parameters.values[glass_recipe].number;
    if (!(position >= 1.0)) return std::nullopt;
    const ControlVisualRole role = static_cast<ControlVisualRole>(static_cast<std::uint8_t>(position - 1.0));
    return role;
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
    BoxBlurWork work{};
    Blurred blurred{};
    bool sampled = false;
    if (target.backplane != nullptr && target.backplane_margin == margin) {
        const PartRaster& backplane = *target.backplane;
        const bool fits = backplane.extent().width == width + margin * 2U &&
                          backplane.extent().height == height + margin * 2U;
        if (fits) {
            try {
                prepare_box_blur(work, backplane.extent().width, backplane.extent().height, lanes);
                blurred = blur_backplane(backplane, half, work);
                sampled = true;
            } catch (const std::exception&) {
                // std::bad_alloc or std::length_error: no backdrop is sampled.
                sampled = false;
            }
        }
    }

    // The backdrop's colour filter, from the recipe's material (E9.6).
    const SurfaceMaterial* const lent = target.material;
    const bool filtered = lent != nullptr && (*lent).colour_filter.has_value();
    const MaterialColourMatrix filter = filtered ? *(*lent).colour_filter : MaterialColourMatrix{};
    const Unit over = premultiply(tint, 1.0);
    for (std::size_t row = 0U; row < height; ++row) {
        const std::size_t start = row * destination.row_bytes;
        for (std::size_t column = 0U; column < width; ++column) {
            Unit pixel{};
            if (sampled) {
                const std::size_t at = row * blurred.stride + column * lanes;
                pixel = Unit{unit_lane(blurred.pixels[at]), unit_lane(blurred.pixels[at + 1U]),
                             unit_lane(blurred.pixels[at + 2U]), unit_lane(blurred.pixels[at + 3U])};
                if (filtered) pixel = colour_filtered(filter, pixel);
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
