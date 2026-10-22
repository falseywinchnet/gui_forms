// The internal interpolation seam (src/core/interpolation): binary64, the
// engine's result clamped to the caller's range, every refusal before any
// write, and nothing else added to the engine (stx_conv). The engine itself
// is tested in stx_conv/tests.
#include "../../src/core/interpolation/interpolation.hpp"

#include <stx_conv/conv.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <span>
#include <stdexcept>
#include <vector>

namespace {
namespace interpolation = gui_forms::interpolation;
using interpolation::Status;

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

void require_status(Status actual, Status expected, const char* message) {
    if (actual != expected) {
        std::cerr << "status " << static_cast<int>(actual) << ", expected " << static_cast<int>(expected) << '\n';
        throw std::runtime_error(message);
    }
}

bool same_bytes(const std::vector<double>& left, const std::vector<double>& right) {
    const bool equal = left.size() == right.size()
        && std::memcmp(left.data(), right.data(), left.size() * sizeof(double)) == 0;
    return equal;
}

// Fixed pseudo-random values in [0, 1] (xorshift32).
struct Values final {
    std::uint32_t state = 20261022U;
    double next() {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        const double value = static_cast<double>(state >> 8) / 16777215.0;
        return value;
    }
};

constexpr interpolation::ValueRange unit{0.0, 1.0};
constexpr double sentinel = -7.0;

void line_values() {
    const std::vector<double> source{0.2, 0.9, 0.4, 0.6};
    std::vector<double> target(9, sentinel);
    const Status status = interpolation::resample_line({source, 4, 1, 1}, {target, 9, 1},
                                                       interpolation::Sampling::nodal, unit);
    require_status(status, Status::ok, "nodal line resample");
    require(std::abs(target[0] - 0.2) <= 1.0e-12 && std::abs(target[8] - 0.6) <= 1.0e-12,
            "nodal enlargement keeps the end values");
    const std::vector<double> constant(5, 0.375);
    std::vector<double> reduced(3, sentinel);
    const Status reduce = interpolation::resample_line({constant, 5, 1, 1}, {reduced, 3, 1},
                                                       interpolation::Sampling::pixel_area, unit);
    require_status(reduce, Status::ok, "pixel-area line reduction");
    for (const double value : reduced) require(std::abs(value - 0.375) <= 1.0e-12, "a constant stays constant");
    const std::vector<double> positions{0.0, 1.0, 2.0, 3.0, 1.5};
    std::vector<double> at(5, sentinel);
    const Status sampled = interpolation::interpolate_line_at({source, 4, 1, 1}, positions, {at, 5, 1}, unit);
    require_status(sampled, Status::ok, "line at positions");
    for (std::size_t node = 0; node < 4; ++node) {
        require(std::abs(at[node] - source[node]) <= 1.0e-12, "an integer position is its node");
    }
    require(at[4] >= 0.4 && at[4] <= 0.9, "a midpoint lies between its neighbours here");
}

// The seam's result is the engine's, clamped: bit for bit, for lines,
// positions and images, with padding in every stride.
void seam_is_engine_clamped() {
    Values values{};
    const std::size_t lanes = 3;
    const std::size_t length = 11;
    const std::size_t stride = lanes + 2;
    std::vector<double> source(length * stride, sentinel);
    for (std::size_t node = 0; node < length; ++node) {
        for (std::size_t lane = 0; lane < lanes; ++lane) source[node * stride + lane] = values.next();
    }
    const std::size_t targets[] = {1, 4, 11, 29};
    for (const std::size_t target_length : targets) {
        const std::size_t out_stride = lanes + 1;
        std::vector<double> seam(target_length * out_stride, sentinel);
        std::vector<double> engine(target_length * out_stride, sentinel);
        const Status status = interpolation::resample_line({source, length, lanes, stride},
                                                           {seam, target_length, out_stride},
                                                           interpolation::Sampling::nodal, unit);
        require_status(status, Status::ok, "line through the seam");
        stx_conv::Profile64 profile(length, lanes);
        stx_conv::LineWorkspace64 work(length, lanes);
        const stx_conv::LinePlan64 plan(length, target_length, stx_conv::Sampling::nodal);
        require(stx_conv::compile({source, stride}, profile, work) == stx_conv::Status::ok, "engine compile");
        require(stx_conv::evaluate(profile, plan, {engine, out_stride}) == stx_conv::Status::ok, "engine evaluate");
        for (std::size_t node = 0; node < target_length; ++node) {
            for (std::size_t lane = 0; lane < lanes; ++lane) {
                double& value = engine[node * out_stride + lane];
                value = std::clamp(value, 0.0, 1.0);
            }
        }
        require(same_bytes(seam, engine), "line: the seam is the engine, clamped, and padding is untouched");
    }
    const std::size_t width = 7;
    const std::size_t height = 5;
    const std::size_t channels = 4;
    const std::size_t row_stride = width * channels + 3;
    std::vector<double> image(height * row_stride, sentinel);
    for (std::size_t y = 0; y < height; ++y) {
        for (std::size_t x = 0; x < width * channels; ++x) image[y * row_stride + x] = values.next();
    }
    struct Size final {
        std::size_t width = 0;
        std::size_t height = 0;
    };
    const Size sizes[] = {{7, 5}, {13, 5}, {7, 2}, {16, 11}, {3, 2}, {1, 1}};
    const interpolation::Sampling samplings[] = {interpolation::Sampling::nodal, interpolation::Sampling::pixel_area};
    for (const Size size : sizes) {
        for (const interpolation::Sampling sampling : samplings) {
            const std::size_t out_row = size.width * channels + 1;
            std::vector<double> seam(size.height * out_row, sentinel);
            std::vector<double> engine(size.height * out_row, sentinel);
            const Status status = interpolation::resample_image({image, width, height, channels, row_stride},
                                                                {seam, size.width, size.height, out_row}, sampling, unit);
            require_status(status, Status::ok, "image through the seam");
            const stx_conv::Sampling engine_sampling = sampling == interpolation::Sampling::nodal
                ? stx_conv::Sampling::nodal
                : stx_conv::Sampling::pixel_area;
            const stx_conv::ImagePlan64 plan(width, height, size.width, size.height, engine_sampling);
            stx_conv::ImageWorkspace64 work(plan, channels);
            require(stx_conv::resize(plan, {image, row_stride}, {engine, out_row}, work) == stx_conv::Status::ok,
                    "engine resize");
            for (std::size_t y = 0; y < size.height; ++y) {
                for (std::size_t x = 0; x < size.width * channels; ++x) {
                    double& value = engine[y * out_row + x];
                    value = std::clamp(value, 0.0, 1.0);
                }
            }
            require(same_bytes(seam, engine), "image: the seam is the engine, clamped, and padding is untouched");
        }
    }
}

// Two-node pulses overshoot in the engine (by about 0.16 here); the seam's output
// stays in range.
void clamp_is_active() {
    const std::vector<double> source{0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0};
    const std::size_t target_length = 29;
    stx_conv::Profile64 profile(source.size(), 1);
    stx_conv::LineWorkspace64 work(source.size(), 1);
    const stx_conv::LinePlan64 plan(source.size(), target_length, stx_conv::Sampling::pixel_area);
    std::vector<double> engine(target_length, sentinel);
    require(stx_conv::compile({source, 1}, profile, work) == stx_conv::Status::ok, "engine compile");
    require(stx_conv::evaluate(profile, plan, {engine, 1}) == stx_conv::Status::ok, "engine evaluate");
    const double engine_low = *std::min_element(engine.begin(), engine.end());
    const double engine_high = *std::max_element(engine.begin(), engine.end());
    require(engine_low < 0.0 || engine_high > 1.0, "the engine overshoots this signal (else this test proves nothing)");
    std::vector<double> seam(target_length, sentinel);
    const Status status = interpolation::resample_line({source, source.size(), 1, 1}, {seam, target_length, 1},
                                                       interpolation::Sampling::pixel_area, unit);
    require_status(status, Status::ok, "clamped resample");
    for (const double value : seam) require(value >= 0.0 && value <= 1.0, "the seam clamps to the range");
    const interpolation::ValueRange narrow{0.25, 0.75};
    const std::vector<double> inside{0.25, 0.75, 0.25, 0.75, 0.25};
    std::vector<double> narrowed(17, sentinel);
    const Status narrow_status = interpolation::resample_line({inside, 5, 1, 1}, {narrowed, 17, 1},
                                                              interpolation::Sampling::nodal, narrow);
    require_status(narrow_status, Status::ok, "narrow range");
    for (const double value : narrowed) require(value >= 0.25 && value <= 0.75, "the seam clamps to any range");
}

// A line's source is copied before the target is written, so they may share
// storage; the result matches the out-of-place one.
void lines_may_overlap() {
    std::vector<double> buffer{0.1, 0.8, 0.3, 0.7, 0.2, 0.9};
    const std::vector<double> copy = buffer;
    std::vector<double> separate(4, sentinel);
    const Status apart = interpolation::resample_line({copy, 6, 1, 1}, {separate, 4, 1},
                                                      interpolation::Sampling::pixel_area, unit);
    require_status(apart, Status::ok, "out of place");
    const Status in_place = interpolation::resample_line({buffer, 6, 1, 1}, {buffer, 4, 1},
                                                         interpolation::Sampling::pixel_area, unit);
    require_status(in_place, Status::ok, "in place");
    for (std::size_t node = 0; node < 4; ++node) require(buffer[node] == separate[node], "in place equals out of place");
    require(buffer[4] == 0.2 && buffer[5] == 0.9, "the target's extent only");
}

// Every refusal: its status, and the target's bytes unchanged.
void line_refused(const interpolation::SourceLine& source, std::size_t target_length, std::size_t target_stride,
                  interpolation::ValueRange range, Status expected, const char* message) {
    std::vector<double> target(32, sentinel);
    const std::vector<double> before = target;
    const std::size_t extent = std::min(target.size(), target_length * target_stride);
    const Status status = interpolation::resample_line(source, {std::span<double>(target).first(extent), target_length,
                                                                target_stride},
                                                       interpolation::Sampling::nodal, range);
    require_status(status, expected, message);
    require(same_bytes(target, before), "a refused line leaves its target");
}

void refusals() {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double infinity = std::numeric_limits<double>::infinity();
    const std::vector<double> source{0.1, 0.5, 0.9, 0.4, 0.2, 0.6};
    line_refused({source, 0, 1, 1}, 4, 1, unit, Status::invalid_shape, "empty source");
    line_refused({source, 3, 0, 1}, 4, 1, unit, Status::invalid_shape, "no lanes");
    line_refused({source, 3, 2, 1}, 4, 2, unit, Status::invalid_shape, "source stride below lanes");
    line_refused({source, 3, 2, 2}, 4, 1, unit, Status::invalid_shape, "target stride below lanes");
    line_refused({source, 3, 1, 1}, 0, 1, unit, Status::invalid_shape, "empty target");
    line_refused({source, 3, 1, std::numeric_limits<std::size_t>::max()}, 4, 1, unit, Status::invalid_shape,
                 "a stride whose extent overflows");
    line_refused({source, 6, 1, 1}, 4, 1, {nan, 1.0}, Status::invalid_range, "NaN bound");
    line_refused({source, 6, 1, 1}, 4, 1, {0.0, infinity}, Status::invalid_range, "infinite bound");
    line_refused({source, 6, 1, 1}, 4, 1, {1.0, 0.0}, Status::invalid_range, "reversed bounds");
    line_refused({source, 7, 1, 1}, 4, 1, unit, Status::insufficient_storage, "short source");
    line_refused({source, 3, 1, 3}, 4, 1, unit, Status::insufficient_storage, "short strided source");
    line_refused({source, 6, 1, 1}, 40, 1, unit, Status::insufficient_storage, "short target");
    line_refused({source, 6, 1, 1}, 4, 1, {0.2, 1.0}, Status::sample_out_of_range, "a sample below the range");
    line_refused({source, 6, 1, 1}, 4, 1, {0.0, 0.8}, Status::sample_out_of_range, "a sample above the range");
    const std::vector<double> holed{0.1, nan, 0.9};
    line_refused({holed, 3, 1, 1}, 4, 1, unit, Status::sample_out_of_range, "a NaN sample");
    // Padding is not read: a NaN between nodes is admitted.
    const std::vector<double> padded{0.1, nan, 0.9, nan, 0.4};
    std::vector<double> padded_target(4, sentinel);
    require_status(interpolation::resample_line({padded, 3, 1, 2}, {padded_target, 4, 1},
                                                interpolation::Sampling::nodal, unit),
                   Status::ok, "padding is not read");
    // A range past the engine's sample limit (1e100): the engine refuses.
    const std::vector<double> huge{0.0, 1.0e200, 0.0};
    line_refused({huge, 3, 1, 1}, 4, 1, {-1.0e300, 1.0e300}, Status::sample_out_of_range,
                 "a sample past the engine's limit");

    std::vector<double> at(3, sentinel);
    const std::vector<double> before_at = at;
    const std::vector<double> mismatched{0.0, 1.0};
    require_status(interpolation::interpolate_line_at({source, 6, 1, 1}, mismatched, {at, 3, 1}, unit),
                   Status::invalid_shape, "positions and target length differ");
    const double outside[] = {-0.5, 5.0 + 1.0e-12, nan, infinity};
    for (const double position : outside) {
        const std::vector<double> positions{0.0, position, 1.0};
        require_status(interpolation::interpolate_line_at({source, 6, 1, 1}, positions, {at, 3, 1}, unit),
                       Status::position_out_of_range, "a position outside the source");
    }
    require(same_bytes(at, before_at), "refused positions leave the target");
    const std::vector<double> ends{0.0, 5.0, 2.5};
    require_status(interpolation::interpolate_line_at({source, 6, 1, 1}, ends, {at, 3, 1}, unit), Status::ok,
                   "both ends are in the domain");

    std::vector<double> image(4 * 3 * 2, 0.5);
    std::vector<double> target(5 * 4 * 2, sentinel);
    const std::vector<double> before = target;
    struct Case final {
        interpolation::SourceImage source{};
        interpolation::TargetImage target{};
        interpolation::ValueRange range{};
        Status expected = Status::ok;
        const char* message = "";
    };
    const std::size_t huge_width = std::numeric_limits<std::size_t>::max() / 2;
    const Case cases[] = {
        {{image, 0, 3, 2, 8}, {target, 5, 4, 10}, unit, Status::invalid_shape, "zero source width"},
        {{image, 4, 0, 2, 8}, {target, 5, 4, 10}, unit, Status::invalid_shape, "zero source height"},
        {{image, 4, 3, 0, 8}, {target, 5, 4, 10}, unit, Status::invalid_shape, "zero channels"},
        {{image, 4, 3, 2, 8}, {target, 0, 4, 10}, unit, Status::invalid_shape, "zero target width"},
        {{image, 4, 3, 2, 8}, {target, 5, 0, 10}, unit, Status::invalid_shape, "zero target height"},
        {{image, 4, 3, 2, 7}, {target, 5, 4, 10}, unit, Status::invalid_shape, "source row stride short"},
        {{image, 4, 3, 2, 8}, {target, 5, 4, 9}, unit, Status::invalid_shape, "target row stride short"},
        {{image, huge_width, 3, 4, 8}, {target, 5, 4, 10}, unit, Status::invalid_shape, "a row that overflows"},
        {{image, 4, 3, 2, 8}, {target, 5, 4, 10}, {0.0, nan}, Status::invalid_range, "NaN bound"},
        {{image, 4, 4, 2, 8}, {target, 5, 4, 10}, unit, Status::insufficient_storage, "short source"},
        {{image, 4, 3, 2, 8}, {target, 5, 5, 10}, unit, Status::insufficient_storage, "short target"},
        {{image, 4, 3, 2, 8}, {target, 5, 4, 10}, {0.0, 0.4}, Status::sample_out_of_range, "samples above"},
    };
    for (const Case& item : cases) {
        require_status(interpolation::resample_image(item.source, item.target, interpolation::Sampling::pixel_area,
                                                     item.range),
                       item.expected, item.message);
        require(same_bytes(target, before), "a refused image leaves its target");
    }
    // Overlap: the target shares the source's storage.
    std::vector<double> shared(64, 0.5);
    const std::vector<double> shared_before = shared;
    const std::span<double> whole(shared);
    require_status(interpolation::resample_image({whole.first(24), 4, 3, 2, 8}, {whole.subspan(16, 40), 5, 4, 10},
                                                 interpolation::Sampling::pixel_area, unit),
                   Status::overlap, "overlapping image spans");
    require(same_bytes(shared, shared_before), "a refused overlap leaves the storage");
    // Adjacent spans do not overlap.
    require_status(interpolation::resample_image({whole.first(24), 4, 3, 2, 8}, {whole.subspan(24, 40), 5, 4, 10},
                                                  interpolation::Sampling::pixel_area, unit),
                   Status::ok, "adjacent image spans");
}

} // namespace

int main() {
    try {
        line_values();
        seam_is_engine_clamped();
        clamp_is_active();
        lines_may_overlap();
        refusals();
    } catch (const std::exception& error) {
        std::cerr << "interpolation_tests: " << error.what() << '\n';
        return 1;
    }
    std::cout << "PASS: interpolation seam\n";
    return 0;
}
