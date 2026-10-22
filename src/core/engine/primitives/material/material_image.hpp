#pragma once

#include "../kernel/pixel_kernel.hpp"

#include "gui_forms/resources/image_registry/image_registry.hpp"
#include "gui_forms/surface_material/material_fill_layer/material_fill_layer.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

// A material's image fill (owner, 2026-10-22: supported, never used by a
// stock theme): the image read from the window's resources and resampled to
// the part's device pixels with one named, deterministic filter.
//
// The filter is Mitchell–Netravali's cubic with B = C = 1/3, applied
// separably to premultiplied colour:
//
//   k(x) = (7|x|³ − 12|x|² + 16/3) / 6                    for |x| < 1
//   k(x) = (−(7/3)|x|³ + 12|x|² − 20|x| + 32/3) / 6       for 1 ≤ |x| < 2
//   k(x) = 0                                               otherwise
//
// Why this one. An icon composited into a face is mostly hard edges on flat
// ground. Bilinear blurs when it enlarges and aliases when it shrinks; a box
// filter is blocky; Lanczos rings, leaving light and dark halos beside each
// edge. Mitchell and Netravali chose B = C = 1/3 in their 1988 study as the
// balance of blur, ringing and anisotropy that viewers judged best, and it
// is a fixed piecewise polynomial: no transcendental functions, so the same
// inputs give the same bytes on every host. When the image shrinks by a
// factor f > 1, the kernel is widened by f (its taps at (i + 0.5 − u) / f),
// so every source pixel contributes and nothing aliases. The weights of each
// output pixel are normalized to sum to 1, and the result is clamped to a
// valid premultiplied colour (each channel within [0, alpha]), since the
// kernel's small negative lobes can overshoot.

namespace gui_forms::engine_primitives {

// An image's pixels as premultiplied RGBA8, rows packed: `width` × `height`
// × 4 live bytes.
struct MaterialImagePixels final {
    std::vector<std::uint8_t> bytes;
    std::uint32_t width{};
    std::uint32_t height{};
};

// Whether `fill`'s image is in `registry` with the pixel size the fill
// declares. No decoding.
[[nodiscard]] bool material_image_available(const ImageRegistry& registry, const MaterialFillLayer& fill) noexcept;

// Reads `fill`'s image into `pixels` (reset-size), decoding a PNG with the
// shared core decoder. False, with `pixels` empty, when the image is not
// available or does not decode.
[[nodiscard]] bool read_material_image(const ImageRegistry& registry, const MaterialFillLayer& fill,
                                       MaterialImagePixels& pixels);

// Where an image lies along one axis, in the destination's device pixels.
// `start` and `length` are the part's box; `source_length` the image's
// pixels along the axis. A nine-patch's `slice_start` and `slice_end` are
// the source pixels of its two fixed segments and `destination_start` and
// `destination_end` their device lengths; a tile's `tile_length` is one
// tile's device length.
struct ImageAxisPlacement final {
    MaterialImageMode mode{MaterialImageMode::stretch};
    double start{};
    double length{};
    double source_length{};
    double slice_start{};
    double slice_end{};
    double destination_start{};
    double destination_end{};
    double tile_length{};
};

// The placement of `fill`'s image across and down a box (device pixels) at
// `scale`, for an image of `source_width` × `source_height` pixels. A
// nine-patch's fixed segments shrink in proportion when they do not fit.
[[nodiscard]] ImageAxisPlacement image_placement_across(const MaterialFillLayer& fill, double box_start,
                                                        double box_length, double scale,
                                                        double source_width) noexcept;
[[nodiscard]] ImageAxisPlacement image_placement_down(const MaterialFillLayer& fill, double box_start,
                                                      double box_length, double scale,
                                                      double source_height) noexcept;

// The most taps any output pixel takes along an axis: 4 × f + 2, where f is
// the largest shrink factor of its segments (at least 1).
[[nodiscard]] double image_axis_taps(const ImageAxisPlacement& placement) noexcept;

// One axis's resampling: the taps of output pixel o are
// [first[o], first[o + 1]) of `source` and `weight`.
struct ResampleAxis final {
    std::vector<std::uint32_t> first;
    std::vector<std::uint32_t> source;
    std::vector<double> weight;
};

// Fills `axis` (reset-size) for `outputs` output pixels along the
// placement: each output pixel's centre mapped into its segment's source
// pixels, its taps clamped to the segment (wrapped, for a tile), and its
// weights normalized. An output in an empty segment has no taps.
void plan_resample_axis(const ImageAxisPlacement& placement, std::uint32_t outputs, ResampleAxis& axis);

// Resamples `pixels` into `resampled` (reset-size to the two axes' output
// counts, row-major, premultiplied, in [0, 1]), through `scratch`
// (reset-size to the image's rows by the output columns). The three
// buffers are distinct.
void resample_material_image(const MaterialImagePixels& pixels, const ResampleAxis& across,
                             const ResampleAxis& down, std::vector<Unit>& scratch, std::vector<Unit>& resampled);

} // namespace gui_forms::engine_primitives
