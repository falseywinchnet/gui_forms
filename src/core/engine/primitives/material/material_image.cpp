#include "material_image.hpp"

#include "../../../image/resource_pixels.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <optional>
#include <span>

// Identical bytes on every host: no fused multiply-add (see engine_math.cpp).
#if defined(__clang__)
#pragma clang fp contract(off)
#endif

namespace gui_forms::engine_primitives {
namespace {

// Mitchell–Netravali's cubic, B = C = 1/3 (provisional; see
// material_image.hpp).
double mitchell_netravali(double x) noexcept {
    const double a = std::abs(x);
    if (a < 1.0) {
        const double inner = ((7.0 * a - 12.0) * a * a + 16.0 / 3.0) / 6.0;
        return inner;
    }
    if (a < 2.0) {
        const double outer = (((-7.0 / 3.0 * a + 12.0) * a - 20.0) * a + 32.0 / 3.0) / 6.0;
        return outer;
    }
    return 0.0;
}

// The image's view when it is in the registry with the fill's declared
// pixel size.
std::optional<ImageResourceView> matching_view(const ImageRegistry& registry, const MaterialFillLayer& fill) noexcept {
    const std::optional<ImageResourceView> view = registry.find(fill.image);
    if (!view) return std::nullopt;
    const double width = static_cast<double>((*view).metadata.width);
    const double height = static_cast<double>((*view).metadata.height);
    if (width != fill.image_pixel_size.width || height != fill.image_pixel_size.height) return std::nullopt;
    if ((*view).metadata.width == 0U || (*view).metadata.height == 0U) return std::nullopt;
    return view;
}

// One segment of an axis: output centres in [destination_start,
// destination_end) map onto source pixels [source_start, source_end).
struct AxisSegment final {
    double destination_start{};
    double destination_end{};
    double source_start{};
    double source_end{};
};

// The shrink factor of a segment: source pixels per output pixel, at least 1.
double shrink_factor(double source_length, double destination_length) noexcept {
    if (!(destination_length > 0.0)) return 1.0;
    const double factor = std::max(1.0, source_length / destination_length);
    return factor;
}

// Appends the taps of one output pixel whose centre maps to source
// coordinate `u` (in source pixels), the kernel widened by `factor`, and
// normalizes their weights. Taps fall in [low, high) of the source; `wrap`
// wraps them over [0, high) instead of clamping.
void append_taps(double u, double factor, std::uint32_t low, std::uint32_t high, bool wrap, ResampleAxis& axis) {
    const std::size_t begin = axis.weight.size();
    const MaterialResamplingFilter filter = material_resampling_filter;
    const double reach = resampling_filter_reach(filter) * factor;
    const std::int64_t first = static_cast<std::int64_t>(std::floor(u - reach - 0.5));
    const std::int64_t last = static_cast<std::int64_t>(std::ceil(u + reach - 0.5));
    const std::int64_t length = static_cast<std::int64_t>(high);
    double sum = 0.0;
    for (std::int64_t position = first; position <= last; ++position) {
        const double weight = resampling_filter_weight(filter, (static_cast<double>(position) + 0.5 - u) / factor);
        if (weight == 0.0) continue;
        std::int64_t index = position;
        if (wrap) {
            index = index % length;
            if (index < 0) index = index + length;
        } else {
            index = std::clamp<std::int64_t>(index, static_cast<std::int64_t>(low), length - 1);
        }
        axis.source.push_back(static_cast<std::uint32_t>(index));
        axis.weight.push_back(weight);
        sum = sum + weight;
    }
    // The kernel's weights at unit spacing sum to about 1 for any u, never 0.
    if (!(sum > 0.0)) return;
    for (std::size_t tap = begin; tap < axis.weight.size(); ++tap) axis.weight[tap] = axis.weight[tap] / sum;
}

double unit_channel(std::uint8_t value) noexcept {
    const double channel = static_cast<double>(value) / 255.0;
    return channel;
}

// A resampled colour made valid: alpha in [0, 1], each colour channel in
// [0, alpha].
Unit valid_premultiplied(Unit colour) noexcept {
    Unit valid{};
    valid.alpha = std::clamp(colour.alpha, 0.0, 1.0);
    valid.red = std::clamp(colour.red, 0.0, valid.alpha);
    valid.green = std::clamp(colour.green, 0.0, valid.alpha);
    valid.blue = std::clamp(colour.blue, 0.0, valid.alpha);
    return valid;
}

ImageAxisPlacement placement_along(const MaterialFillLayer& fill, double box_start, double box_length, double scale,
                                   double source_length, double slice_start, double slice_end,
                                   double declared_length) noexcept {
    ImageAxisPlacement placement{};
    placement.mode = fill.image_mode;
    placement.start = box_start;
    placement.length = box_length;
    placement.source_length = source_length;
    // Source pixels per logical unit: the fill's image scale.
    const double device_per_source = scale / fill.image_scale;
    placement.tile_length = declared_length * device_per_source;
    placement.slice_start = slice_start;
    placement.slice_end = slice_end;
    double near = slice_start * device_per_source;
    double far = slice_end * device_per_source;
    const double fixed = near + far;
    if (fixed > box_length && fixed > 0.0) {
        const double factor = box_length / fixed;
        near = near * factor;
        far = far * factor;
    }
    placement.destination_start = near;
    placement.destination_end = far;
    return placement;
}

} // namespace

bool material_image_available(const ImageRegistry& registry, const MaterialFillLayer& fill) noexcept {
    const bool available = matching_view(registry, fill).has_value();
    return available;
}

bool read_material_image(const ImageRegistry& registry, const MaterialFillLayer& fill, MaterialImagePixels& pixels) {
    pixels.bytes.clear();
    pixels.width = 0U;
    pixels.height = 0U;
    const std::optional<ImageResourceView> found = matching_view(registry, fill);
    if (!found) return false;
    const ImageResourceView& view = *found;
    const std::uint32_t width = view.metadata.width;
    const std::uint32_t height = view.metadata.height;
    const std::size_t row_bytes = static_cast<std::size_t>(width) * 4U;
    // The registry bounds an image's sides (4096 by default) and pixels, so
    // the product fits; checked anyway, as the decoder's storage is sized
    // from it.
    if (row_bytes / 4U != width || row_bytes > std::numeric_limits<std::size_t>::max() / height) return false;
    std::vector<std::uint8_t> bytes(row_bytes * height, 0U);
    if (view.encoding != ImageResourceEncoding::bgra32_premultiplied) {
        image::ResourcePixelDecoder decoder{};
        const image::PixelTarget target{image::PixelOrder::rgba, image::PixelAlpha::premultiplied};
        const std::span<std::byte> destination = std::as_writable_bytes(std::span<std::uint8_t>(bytes));
        const image::ResourcePixelResult decoded = decoder.decode(view, target, destination, row_bytes);
        if (decoded.error != image::ResourcePixelError::none || decoded.width != width || decoded.height != height) {
            return false;
        }
    } else {
        // Premultiplied BGRA rows of view.row_bytes each: swap to RGBA.
        if (view.row_bytes < row_bytes || view.encoded.size() < view.row_bytes * (height - 1U) + row_bytes) {
            return false;
        }
        for (std::uint32_t row = 0U; row < height; ++row) {
            const std::size_t from = static_cast<std::size_t>(view.row_bytes) * row;
            const std::size_t to = row_bytes * row;
            for (std::uint32_t column = 0U; column < width; ++column) {
                const std::size_t source = from + static_cast<std::size_t>(column) * 4U;
                const std::size_t target = to + static_cast<std::size_t>(column) * 4U;
                bytes[target] = static_cast<std::uint8_t>(view.encoded[source + 2U]);
                bytes[target + 1U] = static_cast<std::uint8_t>(view.encoded[source + 1U]);
                bytes[target + 2U] = static_cast<std::uint8_t>(view.encoded[source]);
                bytes[target + 3U] = static_cast<std::uint8_t>(view.encoded[source + 3U]);
            }
        }
    }
    pixels.bytes = std::move(bytes);
    pixels.width = width;
    pixels.height = height;
    return true;
}

ImageAxisPlacement image_placement_across(const MaterialFillLayer& fill, double box_start, double box_length,
                                          double scale, double source_width) noexcept {
    const ImageAxisPlacement placement =
        placement_along(fill, box_start, box_length, scale, source_width, fill.image_slice.left,
                        fill.image_slice.right, fill.image_pixel_size.width);
    return placement;
}

ImageAxisPlacement image_placement_down(const MaterialFillLayer& fill, double box_start, double box_length,
                                        double scale, double source_height) noexcept {
    const ImageAxisPlacement placement =
        placement_along(fill, box_start, box_length, scale, source_height, fill.image_slice.top,
                        fill.image_slice.bottom, fill.image_pixel_size.height);
    return placement;
}

double resampling_filter_reach(MaterialResamplingFilter filter) noexcept {
    switch (filter) {
    case MaterialResamplingFilter::mitchell_netravali:
        return 2.0;
    }
    return 2.0;
}

double resampling_filter_weight(MaterialResamplingFilter filter, double x) noexcept {
    switch (filter) {
    case MaterialResamplingFilter::mitchell_netravali:
        return mitchell_netravali(x);
    }
    return 0.0;
}

double image_axis_taps(const ImageAxisPlacement& placement) noexcept {
    double factor = 1.0;
    switch (placement.mode) {
    case MaterialImageMode::stretch:
        factor = shrink_factor(placement.source_length, placement.length);
        break;
    case MaterialImageMode::tile:
        factor = shrink_factor(placement.source_length, placement.tile_length);
        break;
    case MaterialImageMode::nine_patch: {
        const double middle_source = placement.source_length - placement.slice_start - placement.slice_end;
        const double middle_destination =
            placement.length - placement.destination_start - placement.destination_end;
        factor = std::max(shrink_factor(placement.slice_start, placement.destination_start),
                          shrink_factor(placement.slice_end, placement.destination_end));
        factor = std::max(factor, shrink_factor(middle_source, middle_destination));
        break;
    }
    }
    const double reach = resampling_filter_reach(material_resampling_filter);
    const double taps = 2.0 * reach * factor + 2.0;
    return taps;
}

void plan_resample_axis(const ImageAxisPlacement& placement, std::uint32_t outputs, ResampleAxis& axis) {
    axis.first.assign(static_cast<std::size_t>(outputs) + 1U, 0U);
    axis.source.clear();
    axis.weight.clear();
    const std::uint32_t source_pixels = static_cast<std::uint32_t>(placement.source_length);
    if (placement.mode == MaterialImageMode::tile) {
        // Each output centre's place within its tile; taps wrap, so tiles
        // join without a seam.
        const double tile = placement.tile_length;
        const double factor = shrink_factor(placement.source_length, tile);
        for (std::uint32_t output = 0U; output < outputs; ++output) {
            const double along = static_cast<double>(output) + 0.5 - placement.start;
            const double within = along - tile * std::floor(along / tile);
            const double u = within * placement.source_length / tile;
            append_taps(u, factor, 0U, source_pixels, true, axis);
            axis.first[output + 1U] = static_cast<std::uint32_t>(axis.source.size());
        }
        return;
    }
    // The segments, in order along the axis; a stretch has one.
    std::array<AxisSegment, 3> segments{};
    std::size_t segment_count = 1U;
    segments[0] = AxisSegment{placement.start, placement.start + placement.length, 0.0, placement.source_length};
    if (placement.mode == MaterialImageMode::nine_patch) {
        const double near_end = placement.start + placement.destination_start;
        const double far_start = placement.start + placement.length - placement.destination_end;
        const double middle_source_end = placement.source_length - placement.slice_end;
        segments[0] = AxisSegment{placement.start, near_end, 0.0, placement.slice_start};
        segments[1] = AxisSegment{near_end, far_start, placement.slice_start, middle_source_end};
        segments[2] = AxisSegment{far_start, placement.start + placement.length, middle_source_end,
                                  placement.source_length};
        segment_count = 3U;
    }
    for (std::uint32_t output = 0U; output < outputs; ++output) {
        const double centre = static_cast<double>(output) + 0.5;
        // The last segment that starts at or before the centre; centres
        // before the first belong to the first. An empty segment is passed
        // over, since the next one starts where it does.
        std::size_t chosen = 0U;
        for (std::size_t index = 1U; index < segment_count; ++index) {
            if (centre >= segments[index].destination_start) chosen = index;
        }
        const AxisSegment& segment = segments[chosen];
        const double destination_length = segment.destination_end - segment.destination_start;
        const double source_length = segment.source_end - segment.source_start;
        if (destination_length > 0.0 && source_length >= 1.0) {
            const double u =
                segment.source_start + (centre - segment.destination_start) * source_length / destination_length;
            const double factor = shrink_factor(source_length, destination_length);
            const std::uint32_t low = static_cast<std::uint32_t>(segment.source_start);
            const std::uint32_t high = static_cast<std::uint32_t>(segment.source_end);
            append_taps(u, factor, low, high, false, axis);
        }
        axis.first[output + 1U] = static_cast<std::uint32_t>(axis.source.size());
    }
}

void resample_material_image(const MaterialImagePixels& pixels, const ResampleAxis& across, const ResampleAxis& down,
                             std::vector<Unit>& scratch, std::vector<Unit>& resampled) {
    const std::size_t columns = across.first.size() - 1U;
    const std::size_t rows = down.first.size() - 1U;
    const std::size_t source_rows = pixels.height;
    const std::size_t source_row_bytes = static_cast<std::size_t>(pixels.width) * 4U;
    scratch.assign(source_rows * columns, Unit{});
    resampled.assign(rows * columns, Unit{});
    // Across: every source row to the output columns.
    for (std::size_t source_row = 0U; source_row < source_rows; ++source_row) {
        const std::size_t row_start = source_row * source_row_bytes;
        for (std::size_t column = 0U; column < columns; ++column) {
            Unit sum{};
            for (std::uint32_t tap = across.first[column]; tap < across.first[column + 1U]; ++tap) {
                const std::size_t offset = row_start + static_cast<std::size_t>(across.source[tap]) * 4U;
                const double weight = across.weight[tap];
                sum.red = sum.red + unit_channel(pixels.bytes[offset]) * weight;
                sum.green = sum.green + unit_channel(pixels.bytes[offset + 1U]) * weight;
                sum.blue = sum.blue + unit_channel(pixels.bytes[offset + 2U]) * weight;
                sum.alpha = sum.alpha + unit_channel(pixels.bytes[offset + 3U]) * weight;
            }
            scratch[source_row * columns + column] = sum;
        }
    }
    // Down: the output rows from the resampled source rows.
    for (std::size_t row = 0U; row < rows; ++row) {
        for (std::size_t column = 0U; column < columns; ++column) {
            Unit sum{};
            for (std::uint32_t tap = down.first[row]; tap < down.first[row + 1U]; ++tap) {
                const Unit& part = scratch[static_cast<std::size_t>(down.source[tap]) * columns + column];
                const double weight = down.weight[tap];
                sum.red = sum.red + part.red * weight;
                sum.green = sum.green + part.green * weight;
                sum.blue = sum.blue + part.blue * weight;
                sum.alpha = sum.alpha + part.alpha * weight;
            }
            resampled[row * columns + column] = valid_premultiplied(sum);
        }
    }
}

} // namespace gui_forms::engine_primitives
