#include "interpolation.hpp"

#include <stx_conv/conv.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <new>
#include <optional>
#include <stdexcept>

namespace gui_forms::interpolation {
namespace {

// The elements a strided span needs for `count` nodes or rows of `width`
// elements each: (count - 1) * stride + width, or none when that overflows.
// Callers have checked count >= 1 and width <= stride.
std::optional<std::size_t> required_elements(std::size_t count, std::size_t stride, std::size_t width) noexcept {
    const std::size_t steps = count - 1;
    const std::size_t room = std::numeric_limits<std::size_t>::max() - width;
    if (steps != 0 && stride > room / steps) return std::nullopt;
    const std::size_t required = steps * stride + width;
    return required;
}

bool range_valid(ValueRange range) noexcept {
    const bool valid = std::isfinite(range.low) && std::isfinite(range.high) && range.low <= range.high;
    return valid;
}

// True when every live sample is in the range; NaN fails both comparisons.
bool samples_in_range(std::span<const double> samples, std::size_t count, std::size_t stride, std::size_t width,
                      ValueRange range) noexcept {
    for (std::size_t node = 0; node < count; ++node) {
        const std::size_t first = node * stride;
        for (std::size_t index = first; index < first + width; ++index) {
            const double value = samples[index];
            if (!(value >= range.low && value <= range.high)) return false;
        }
    }
    return true;
}

void clamp_written(std::span<double> samples, std::size_t count, std::size_t stride, std::size_t width,
                   ValueRange range) noexcept {
    for (std::size_t node = 0; node < count; ++node) {
        const std::size_t first = node * stride;
        for (std::size_t index = first; index < first + width; ++index) {
            samples[index] = std::clamp(samples[index], range.low, range.high);
        }
    }
}

stx_conv::Sampling engine_sampling(Sampling sampling) noexcept {
    if (sampling == Sampling::pixel_area) return stx_conv::Sampling::pixel_area;
    return stx_conv::Sampling::nodal;
}

// The engine's refusals after this seam's own validation. Sample refusals
// remain possible when the range reaches past the engine's sample limit
// (1e100). An unprepared profile cannot follow a successful compile; it is
// grouped with the tolerance fallback.
Status from_engine(stx_conv::Status status) noexcept {
    switch (status) {
    case stx_conv::Status::ok:
        return Status::ok;
    case stx_conv::Status::invalid_shape:
        return Status::invalid_shape;
    case stx_conv::Status::insufficient_storage:
        return Status::insufficient_storage;
    case stx_conv::Status::overlap:
        return Status::overlap;
    case stx_conv::Status::nonfinite_input:
    case stx_conv::Status::input_out_of_range:
        return Status::sample_out_of_range;
    case stx_conv::Status::unprepared_profile:
    case stx_conv::Status::numerical_failure:
        return Status::numerical_failure;
    }
    return Status::numerical_failure;
}

// The validated extents of one line call.
struct LineExtents final {
    std::span<const double> source{};
    std::span<double> target{};
};

// Checks shape, range, storage and samples in that order, before anything is
// allocated or written, and records the spans the engine may touch.
Status check_line(const SourceLine& source, const TargetLine& target, ValueRange range, LineExtents& extents) noexcept {
    if (source.length == 0 || target.length == 0 || source.lanes == 0) return Status::invalid_shape;
    if (source.node_stride < source.lanes || target.node_stride < source.lanes) return Status::invalid_shape;
    if (!range_valid(range)) return Status::invalid_range;
    const std::optional<std::size_t> source_needed = required_elements(source.length, source.node_stride, source.lanes);
    const std::optional<std::size_t> target_needed = required_elements(target.length, target.node_stride, source.lanes);
    if (!source_needed.has_value() || !target_needed.has_value()) return Status::invalid_shape;
    if (source.samples.size() < *source_needed || target.samples.size() < *target_needed) {
        return Status::insufficient_storage;
    }
    extents.source = source.samples.first(*source_needed);
    extents.target = target.samples.first(*target_needed);
    if (!samples_in_range(extents.source, source.length, source.node_stride, source.lanes, range)) {
        return Status::sample_out_of_range;
    }
    return Status::ok;
}

// Compiles the source and evaluates one plan into the target. Allocates the
// profile and workspace, so it may throw bad_alloc; callers catch.
Status evaluate_line(const SourceLine& source, const LineExtents& extents, const stx_conv::LinePlan64& plan,
                     std::size_t target_node_stride) {
    stx_conv::Profile64 profile(source.length, source.lanes);
    stx_conv::LineWorkspace64 workspace(source.length, source.lanes);
    const stx_conv::LineInput64 input{extents.source, source.node_stride};
    const stx_conv::Status compiled = stx_conv::compile(input, profile, workspace);
    if (compiled != stx_conv::Status::ok) return from_engine(compiled);
    const stx_conv::LineOutput64 output{extents.target, target_node_stride};
    const stx_conv::Status evaluated = stx_conv::evaluate(profile, plan, output);
    return from_engine(evaluated);
}

} // namespace

// The engine's constructors throw invalid_argument and length_error (for
// lengths past its limit, 4e8 nodes) and bad_alloc; this boundary turns them
// into statuses, so nothing above it sees an exception.
Status resample_line(const SourceLine& source, const TargetLine& target, Sampling sampling, ValueRange range) noexcept {
    LineExtents extents{};
    const Status checked = check_line(source, target, range, extents);
    if (checked != Status::ok) return checked;
    try {
        const stx_conv::LinePlan64 plan(source.length, target.length, engine_sampling(sampling));
        const Status result = evaluate_line(source, extents, plan, target.node_stride);
        if (result == Status::ok) clamp_written(extents.target, target.length, target.node_stride, source.lanes, range);
        return result;
    } catch (const std::bad_alloc&) {
        return Status::out_of_memory;
    } catch (const std::length_error&) {
        return Status::invalid_shape;
    } catch (const std::invalid_argument&) {
        return Status::invalid_shape;
    }
}

Status interpolate_line_at(const SourceLine& source, std::span<const double> positions, const TargetLine& target,
                           ValueRange range) noexcept {
    if (positions.size() != target.length) return Status::invalid_shape;
    LineExtents extents{};
    const Status checked = check_line(source, target, range, extents);
    if (checked != Status::ok) return checked;
    const double last_node = static_cast<double>(source.length - 1);
    for (const double position : positions) {
        if (!(position >= 0.0 && position <= last_node)) return Status::position_out_of_range;
    }
    try {
        const stx_conv::LinePlan64 plan = stx_conv::LinePlan64::points(source.length, positions);
        const Status result = evaluate_line(source, extents, plan, target.node_stride);
        if (result == Status::ok) clamp_written(extents.target, target.length, target.node_stride, source.lanes, range);
        return result;
    } catch (const std::bad_alloc&) {
        return Status::out_of_memory;
    } catch (const std::length_error&) {
        return Status::invalid_shape;
    } catch (const std::invalid_argument&) {
        return Status::invalid_shape;
    }
}

Status resample_image(const SourceImage& source, const TargetImage& target, Sampling sampling,
                      ValueRange range) noexcept {
    if (source.width == 0 || source.height == 0 || source.channels == 0 || target.width == 0 || target.height == 0) {
        return Status::invalid_shape;
    }
    // A row is width x channels elements, overflow-checked.
    const std::optional<std::size_t> source_row = required_elements(source.width, source.channels, source.channels);
    const std::optional<std::size_t> target_row = required_elements(target.width, source.channels, source.channels);
    if (!source_row.has_value() || !target_row.has_value()) return Status::invalid_shape;
    if (source.row_stride < *source_row || target.row_stride < *target_row) return Status::invalid_shape;
    if (!range_valid(range)) return Status::invalid_range;
    const std::optional<std::size_t> source_needed = required_elements(source.height, source.row_stride, *source_row);
    const std::optional<std::size_t> target_needed = required_elements(target.height, target.row_stride, *target_row);
    if (!source_needed.has_value() || !target_needed.has_value()) return Status::invalid_shape;
    if (source.samples.size() < *source_needed || target.samples.size() < *target_needed) {
        return Status::insufficient_storage;
    }
    const std::span<const double> source_samples = source.samples.first(*source_needed);
    const std::span<double> target_samples = target.samples.first(*target_needed);
    if (!samples_in_range(source_samples, source.height, source.row_stride, *source_row, range)) {
        return Status::sample_out_of_range;
    }
    try {
        const stx_conv::ImagePlan64 plan(source.width, source.height, target.width, target.height,
                                         engine_sampling(sampling));
        stx_conv::ImageWorkspace64 workspace(plan, source.channels);
        const stx_conv::ImageInput64 input{source_samples, source.row_stride};
        const stx_conv::ImageOutput64 output{target_samples, target.row_stride};
        const stx_conv::Status resized = stx_conv::resize(plan, input, output, workspace);
        const Status result = from_engine(resized);
        if (result == Status::ok) clamp_written(target_samples, target.height, target.row_stride, *target_row, range);
        return result;
    } catch (const std::bad_alloc&) {
        return Status::out_of_memory;
    } catch (const std::length_error&) {
        return Status::invalid_shape;
    } catch (const std::invalid_argument&) {
        return Status::invalid_shape;
    }
}

} // namespace gui_forms::interpolation
