#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

namespace gui_forms::interpolation {

// General-use interpolation of sampled values: lines (curves, tables, one
// image row) and interleaved images, in binary64 (owner decision 2(a),
// 2026-10-22). An internal seam: no public header names it, and the engine
// behind it (the owner's CONV resampler, stx_conv) never appears here.
//
// The engine's output is not bounded by its input: it can overshoot near
// sign transitions. Every call therefore takes the range its values live in,
// refuses a source sample outside it, and clamps every written value to it
// (owner decision 5(a): the clamp belongs to this seam, not to the engine).
// Channels are clamped independently; a premultiplied consumer that needs
// colour <= alpha imposes that itself.
//
// Each call validates everything before it allocates or writes, allocates
// its plan and working storage once, and releases them before returning; it
// never throws. A refusal leaves the target's bytes unchanged. Only
// numerical_failure (an internal tolerance fallback that no admitted input
// is known to reach; stx_conv/docs/HARDENING.md) leaves the target
// unspecified.

enum class Sampling : std::uint8_t {
    // Endpoint-aligned nodes: on enlargement the first and last target values
    // are the first and last source values; on reduction each target node
    // integrates its share of the source. For curves and tables.
    nodal,
    // Pixel footprints: each target pixel integrates the source area it
    // covers. For images.
    pixel_area
};

enum class Status : std::uint8_t {
    ok,
    // A zero or oversized length, zero channels or lanes, or a stride shorter
    // than one node or row.
    invalid_shape,
    // A span shorter than its shape and stride require.
    insufficient_storage,
    // A range bound that is not finite, or low > high.
    invalid_range,
    // A source sample that is NaN or outside the range.
    sample_out_of_range,
    // A position that is NaN or outside [0, source length - 1].
    position_out_of_range,
    // Image source and target storage overlap (lines may overlap: the source
    // is copied before the target is written).
    overlap,
    // The plan or working storage could not be allocated.
    out_of_memory,
    // The engine's tolerance fallback; the target is unspecified.
    numerical_failure
};

// The closed interval every source sample lies in and every result is
// clamped to.
struct ValueRange final {
    double low = 0.0;
    double high = 1.0;
};

// Node-major samples: node i, lane j is samples[i * node_stride + j].
// Padding between nodes is neither read nor written.
struct SourceLine final {
    std::span<const double> samples{};
    std::size_t length = 0;
    std::size_t lanes = 1;
    std::size_t node_stride = 1;
};
// The target has the source's lanes.
struct TargetLine final {
    std::span<double> samples{};
    std::size_t length = 0;
    std::size_t node_stride = 1;
};

// Row-major interleaved pixels: pixel (x, y), channel c is
// samples[y * row_stride + x * channels + c]. Strides are in elements.
struct SourceImage final {
    std::span<const double> samples{};
    std::size_t width = 0;
    std::size_t height = 0;
    std::size_t channels = 0;
    std::size_t row_stride = 0;
};
// The target has the source's channels.
struct TargetImage final {
    std::span<double> samples{};
    std::size_t width = 0;
    std::size_t height = 0;
    std::size_t row_stride = 0;
};

// Resamples a line to the target's length.
[[nodiscard]] Status resample_line(const SourceLine& source, const TargetLine& target, Sampling sampling,
                                   ValueRange range) noexcept;

// Evaluates a line at arbitrary positions, measured in source nodes:
// position p lies between nodes floor(p) and floor(p) + 1, and an integer
// position is that node. One target node per position.
[[nodiscard]] Status interpolate_line_at(const SourceLine& source, std::span<const double> positions,
                                         const TargetLine& target, ValueRange range) noexcept;

// Resamples an image to the target's dimensions. An unchanged axis is copied.
[[nodiscard]] Status resample_image(const SourceImage& source, const TargetImage& target, Sampling sampling,
                                    ValueRange range) noexcept;

} // namespace gui_forms::interpolation
