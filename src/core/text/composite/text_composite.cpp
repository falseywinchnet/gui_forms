#include "text_composite.hpp"

#include <algorithm>
#include <cmath>

namespace gui_forms::detail {
namespace {

// One device pixel in clip fixed point.
constexpr std::int64_t unit = 256;
constexpr std::int64_t half_unit = 128;
// Coordinates beyond this are refused rather than risking overflow.
constexpr double coordinate_limit = 100'000'000.0;
// Radii above this many device pixels are clamped; corner distance squares
// then stay well inside 64-bit integers.
constexpr std::int64_t radius_limit = 4096 * unit;

bool valid_coordinate(const double value) noexcept {
    const bool valid = std::isfinite(value) && std::abs(value) <= coordinate_limit;
    return valid;
}

bool valid_rect(const Rect rect) noexcept {
    const bool valid = valid_coordinate(rect.x) && valid_coordinate(rect.y) && valid_coordinate(rect.width) &&
        valid_coordinate(rect.height) && rect.width >= 0.0 && rect.height >= 0.0;
    return valid;
}

// Edges of a device rectangle in fixed point.
struct FixedRect final {
    std::int64_t left{};
    std::int64_t top{};
    std::int64_t right{};
    std::int64_t bottom{};
};

FixedRect fixed_rect(const Rect rect) noexcept {
    FixedRect result{};
    result.left = std::llround(rect.x * static_cast<double>(unit));
    result.top = std::llround(rect.y * static_cast<double>(unit));
    result.right = std::llround((rect.x + rect.width) * static_cast<double>(unit));
    result.bottom = std::llround((rect.y + rect.height) * static_cast<double>(unit));
    return result;
}

// Covered length of the pixel span [start, start + unit) inside [low, high).
std::int64_t span_coverage(const std::int64_t start, const std::int64_t low, const std::int64_t high) noexcept {
    const std::int64_t covered = std::min(high, start + unit) - std::max(low, start);
    return std::clamp<std::int64_t>(covered, 0, unit);
}

// Exact covered area of pixel (x, y) inside a rectangle, 0..unit.
std::int64_t area_coverage(const FixedRect rect, const std::int64_t x, const std::int64_t y) noexcept {
    const std::int64_t horizontal = span_coverage(x * unit, rect.left, rect.right);
    const std::int64_t vertical = span_coverage(y * unit, rect.top, rect.bottom);
    const std::int64_t area = (horizontal * vertical + half_unit) / unit;
    return area;
}

// Floor square root by binary digits: exact and identical on every machine.
std::uint64_t integer_sqrt(std::uint64_t value) noexcept {
    std::uint64_t result = 0;
    std::uint64_t bit = std::uint64_t{1} << 62U;
    while (bit > value) bit >>= 2U;
    while (bit != 0) {
        if (value >= result + bit) {
            value -= result + bit;
            result = (result >> 1U) + bit;
        } else {
            result >>= 1U;
        }
        bit >>= 2U;
    }
    return result;
}

// Coverage from the pixel centre's distance to one corner arc: half a pixel
// inside the arc is full coverage, half a pixel outside is none.
std::int64_t corner_coverage(const std::int64_t centre_x, const std::int64_t centre_y, const std::int64_t corner_x,
                             const std::int64_t corner_y, const std::int64_t radius) noexcept {
    const std::int64_t dx = centre_x - corner_x;
    const std::int64_t dy = centre_y - corner_y;
    const std::uint64_t squared = static_cast<std::uint64_t>(dx * dx) + static_cast<std::uint64_t>(dy * dy);
    const std::int64_t distance = static_cast<std::int64_t>(integer_sqrt(squared));
    const std::int64_t outside = distance - radius;
    return std::clamp<std::int64_t>(half_unit - outside, 0, unit);
}

std::int64_t rounded_coverage(const FixedRect rect, const std::int64_t radius, const std::int64_t x,
                              const std::int64_t y) noexcept {
    const std::int64_t area = area_coverage(rect, x, y);
    if (area == 0 || radius == 0) return area;
    const std::int64_t centre_x = x * unit + half_unit;
    const std::int64_t centre_y = y * unit + half_unit;
    const std::int64_t inner_left = rect.left + radius;
    const std::int64_t inner_top = rect.top + radius;
    const std::int64_t inner_right = rect.right - radius;
    const std::int64_t inner_bottom = rect.bottom - radius;
    std::int64_t corner_x = 0;
    std::int64_t corner_y = 0;
    bool corner = false;
    if (centre_x < inner_left && centre_y < inner_top) { corner_x = inner_left; corner_y = inner_top; corner = true; }
    else if (centre_x > inner_right && centre_y < inner_top) { corner_x = inner_right; corner_y = inner_top; corner = true; }
    else if (centre_x < inner_left && centre_y > inner_bottom) { corner_x = inner_left; corner_y = inner_bottom; corner = true; }
    else if (centre_x > inner_right && centre_y > inner_bottom) { corner_x = inner_right; corner_y = inner_bottom; corner = true; }
    if (!corner) return area;
    const std::int64_t arc = corner_coverage(centre_x, centre_y, corner_x, corner_y, radius);
    return std::min(area, arc);
}

struct PreparedClip final {
    FixedRect rect{};
    bool rounded{};
    FixedRect rounded_rect{};
    std::int64_t radius{};
};

PreparedClip prepare_clip(const TextCompositeClip clip) noexcept {
    PreparedClip result{};
    result.rect = fixed_rect(clip.rect);
    result.rounded = clip.rounded;
    if (clip.rounded) {
        result.rounded_rect = fixed_rect(clip.rounded_rect);
        const std::int64_t width = result.rounded_rect.right - result.rounded_rect.left;
        const std::int64_t height = result.rounded_rect.bottom - result.rounded_rect.top;
        const std::int64_t requested = std::llround(clip.radius * static_cast<double>(unit));
        result.radius = std::clamp<std::int64_t>(requested, 0, std::min({width / 2, height / 2, radius_limit}));
    }
    return result;
}

std::int64_t prepared_clip_coverage(const PreparedClip& clip, const std::int64_t x, const std::int64_t y) noexcept {
    const std::int64_t rectangular = area_coverage(clip.rect, x, y);
    if (!clip.rounded || rectangular == 0) return rectangular;
    const std::int64_t rounded = rounded_coverage(clip.rounded_rect, clip.radius, x, y);
    const std::int64_t combined = (rectangular * rounded + half_unit) / unit;
    return combined;
}

std::int64_t floor_pixel(const std::int64_t fixed) noexcept {
    std::int64_t quotient = fixed / unit;
    if (fixed % unit != 0 && fixed < 0) --quotient;
    return quotient;
}

std::int64_t ceil_pixel(const std::int64_t fixed) noexcept {
    std::int64_t quotient = fixed / unit;
    if (fixed % unit != 0 && fixed > 0) ++quotient;
    return quotient;
}

struct ChannelShifts final {
    unsigned red{};
    unsigned green{};
    unsigned blue{};
    unsigned alpha{};
};

ChannelShifts channel_shifts(const TextPixelOrder order) noexcept {
    ChannelShifts shifts{16U, 8U, 0U, 24U};
    if (order == TextPixelOrder::rgba) shifts = {0U, 8U, 16U, 24U};
    return shifts;
}

unsigned blend_channel(const unsigned source, const unsigned destination, const unsigned alpha) noexcept {
    const unsigned blended = (source * alpha + destination * (255U - alpha) + 127U) / 255U;
    return blended;
}

} // namespace

std::uint32_t text_clip_coverage(const TextCompositeClip clip, const std::int64_t x, const std::int64_t y) noexcept {
    if (!valid_rect(clip.rect) || (clip.rounded && (!valid_rect(clip.rounded_rect) || !valid_coordinate(clip.radius)))) {
        return 0U;
    }
    const PreparedClip prepared = prepare_clip(clip);
    const std::int64_t coverage = prepared_clip_coverage(prepared, x, y);
    return static_cast<std::uint32_t>(coverage);
}

TextCompositeStatus composite_text_coverage(const TextCoverage coverage, const TextCompositeTarget target,
                                            const TextCompositeClip clip, const Color color) noexcept {
    if (target.width == 0 || target.height == 0 || target.stride < target.width ||
        target.height > target.pixels.size() / target.stride || !valid_rect(clip.rect) ||
        (clip.rounded && (!valid_rect(clip.rounded_rect) || !valid_coordinate(clip.radius) || clip.radius < 0.0)) ||
        std::abs(coverage.left) > static_cast<std::int64_t>(coordinate_limit) ||
        std::abs(coverage.top) > static_cast<std::int64_t>(coordinate_limit)) {
        return TextCompositeStatus::invalid_geometry;
    }
    if (coverage.width == 0 || coverage.height == 0 || color.alpha == 0) return TextCompositeStatus::success;
    if (coverage.height > coverage.pixels.size() / coverage.width) return TextCompositeStatus::invalid_geometry;
    const PreparedClip prepared = prepare_clip(clip);
    FixedRect limit = prepared.rect;
    if (prepared.rounded) {
        limit.left = std::max(limit.left, prepared.rounded_rect.left);
        limit.top = std::max(limit.top, prepared.rounded_rect.top);
        limit.right = std::min(limit.right, prepared.rounded_rect.right);
        limit.bottom = std::min(limit.bottom, prepared.rounded_rect.bottom);
    }
    const std::int64_t left = std::max<std::int64_t>({0, floor_pixel(limit.left), coverage.left});
    const std::int64_t top = std::max<std::int64_t>({0, floor_pixel(limit.top), coverage.top});
    const std::int64_t right = std::min<std::int64_t>({static_cast<std::int64_t>(target.width), ceil_pixel(limit.right),
                                                       coverage.left + static_cast<std::int64_t>(coverage.width)});
    const std::int64_t bottom = std::min<std::int64_t>({static_cast<std::int64_t>(target.height), ceil_pixel(limit.bottom),
                                                        coverage.top + static_cast<std::int64_t>(coverage.height)});
    const ChannelShifts shifts = channel_shifts(target.order);
    const std::uint64_t colour_alpha = color.alpha;
    for (std::int64_t y = top; y < bottom; ++y) {
        const std::size_t source_row = static_cast<std::size_t>(y - coverage.top) * coverage.width;
        const std::size_t target_row = static_cast<std::size_t>(y) * target.stride;
        for (std::int64_t x = left; x < right; ++x) {
            const std::uint64_t text = coverage.pixels[source_row + static_cast<std::size_t>(x - coverage.left)];
            if (text == 0) continue;
            const std::uint64_t clipped = static_cast<std::uint64_t>(prepared_clip_coverage(prepared, x, y));
            if (clipped == 0) continue;
            // One rounding for coverage x colour alpha x clip (unit = 256).
            const unsigned alpha = static_cast<unsigned>((text * colour_alpha * clipped + 32640U) / 65280U);
            if (alpha == 0) continue;
            std::uint32_t& pixel = target.pixels[target_row + static_cast<std::size_t>(x)];
            const std::uint32_t previous = pixel;
            const unsigned red = blend_channel(color.red, (previous >> shifts.red) & 255U, alpha);
            const unsigned green = blend_channel(color.green, (previous >> shifts.green) & 255U, alpha);
            const unsigned blue = blend_channel(color.blue, (previous >> shifts.blue) & 255U, alpha);
            unsigned result_alpha = 255U;
            if (!target.opaque) result_alpha = blend_channel(255U, (previous >> shifts.alpha) & 255U, alpha);
            pixel = (red << shifts.red) | (green << shifts.green) | (blue << shifts.blue) | (result_alpha << shifts.alpha);
        }
    }
    return TextCompositeStatus::success;
}

} // namespace gui_forms::detail
