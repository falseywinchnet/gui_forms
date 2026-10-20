#include "gui_forms/engine/types/engine_types.hpp"

#include <limits>

namespace gui_forms {

RasterViewDefect validate_raster_view(const PartRasterView& view) noexcept {
    const DeviceExtent extent = view.extent;
    if (extent.width > maximum_part_raster_side || extent.height > maximum_part_raster_side) {
        return RasterViewDefect::extent_too_large;
    }
    if (extent.width == 0U || extent.height == 0U) return RasterViewDefect::none;
    // Both sides are at most 16384, so these products stay below 2^40.
    const std::size_t row_pixel_bytes = static_cast<std::size_t>(extent.width) * part_raster_bytes_per_pixel;
    if (view.row_bytes < row_pixel_bytes) return RasterViewDefect::row_bytes_too_small;
    if (view.row_bytes % part_raster_bytes_per_pixel != 0U) return RasterViewDefect::row_bytes_not_whole_pixels;
    const std::size_t last_row = static_cast<std::size_t>(extent.height) - 1U;
    // No buffer can hold a stride whose span overflows std::size_t.
    const std::size_t largest = std::numeric_limits<std::size_t>::max();
    if (last_row != 0U && view.row_bytes > (largest - row_pixel_bytes) / last_row) {
        return RasterViewDefect::buffer_too_small;
    }
    const std::size_t required = last_row * view.row_bytes + row_pixel_bytes;
    if (view.pixels.size() < required) return RasterViewDefect::buffer_too_small;
    return RasterViewDefect::none;
}

const char* raster_view_defect_name(RasterViewDefect defect) noexcept {
    switch (defect) {
    case RasterViewDefect::none: return "none";
    case RasterViewDefect::extent_too_large: return "extent_too_large";
    case RasterViewDefect::row_bytes_too_small: return "row_bytes_too_small";
    case RasterViewDefect::row_bytes_not_whole_pixels: return "row_bytes_not_whole_pixels";
    case RasterViewDefect::buffer_too_small: return "buffer_too_small";
    }
    return "unknown";
}

} // namespace gui_forms
