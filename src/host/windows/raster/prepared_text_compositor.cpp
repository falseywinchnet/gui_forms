#include "prepared_text_compositor.hpp"

#include "../../../core/text/composite/text_composite.hpp"

#include <algorithm>
#include <cmath>

namespace gui_forms::host::detail {
using gui_forms::detail::composite_text_coverage;
using gui_forms::detail::TextCompositeClip;
using gui_forms::detail::TextCompositeStatus;
using gui_forms::detail::TextCompositeTarget;
using gui_forms::detail::TextCoverage;
using gui_forms::detail::TextPixelOrder;

namespace {

bool valid_coordinate(const double value) noexcept {
    const bool valid = std::isfinite(value) && std::abs(value) <= 100'000'000.0;
    return valid;
}

bool valid_rect(const Rect rect) noexcept {
    const bool valid = valid_coordinate(rect.x) && valid_coordinate(rect.y) &&
        valid_coordinate(rect.width) && valid_coordinate(rect.height) && rect.width >= 0 && rect.height >= 0;
    return valid;
}

} // namespace

PreparedTextStatus composite_prepared_mask(const GrayTextMask& mask, const PreparedCompositeTarget target,
    const PreparedCompositeClip clip, const Point baseline, const double scale, const Color color) noexcept {
    if (mask.empty() || target.width == 0 || target.height == 0 || target.stride < target.width ||
        target.height > target.pixels.size() / target.stride || !valid_rect(clip.rect) ||
        !valid_coordinate(baseline.x) || !valid_coordinate(baseline.y) || !std::isfinite(scale) ||
        scale < 0.5 || scale > 4 || scale != mask.metrics().device_scale) return PreparedTextStatus::invalid_geometry;
    if (clip.rounded && (!valid_rect(clip.rounded_rect) || !std::isfinite(clip.radius) || clip.radius < 0 ||
        clip.radius > std::min(clip.rounded_rect.width, clip.rounded_rect.height) * 0.5)) {
        return PreparedTextStatus::invalid_geometry;
    }
    if (mask.width() == 0 || mask.height() == 0 || color.alpha == 0 || clip.rect.empty()) return PreparedTextStatus::success;
    const std::span<const std::uint8_t> source = mask.pixels();
    if (mask.height() > source.size() / mask.width()) return PreparedTextStatus::invalid_geometry;
    // ADR-004: clipping and blending use the shared text compositor, so this
    // host produces the same bytes as every other one. Clip edges and rounded
    // corners are anti-aliased in device pixels.
    const TextCoverage coverage{.pixels = source, .width = mask.width(), .height = mask.height(),
                                .left = std::llround(baseline.x * scale) + mask.left(),
                                .top = std::llround(baseline.y * scale) + mask.top()};
    const TextCompositeTarget shared_target{.pixels = target.pixels, .width = target.width, .height = target.height,
                                            .stride = target.stride, .order = TextPixelOrder::bgra, .opaque = true};
    TextCompositeClip device_clip{};
    device_clip.rect = {clip.rect.x * scale, clip.rect.y * scale, clip.rect.width * scale, clip.rect.height * scale};
    if (clip.rounded) {
        device_clip.rounded = true;
        device_clip.rounded_rect = {clip.rounded_rect.x * scale, clip.rounded_rect.y * scale,
                                    clip.rounded_rect.width * scale, clip.rounded_rect.height * scale};
        device_clip.radius = clip.radius * scale;
    }
    const TextCompositeStatus composited = composite_text_coverage(coverage, shared_target, device_clip, color);
    if (composited != TextCompositeStatus::success) return PreparedTextStatus::invalid_geometry;
    return PreparedTextStatus::success;
}
} // namespace gui_forms::host::detail
