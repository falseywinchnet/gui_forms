#include "../src/core/text/composite/text_composite.hpp"

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

// ADR-004 shared text blending: one integer arithmetic, anti-aliased clips.
namespace {
using namespace gui_forms;
using detail::TextCompositeClip;
using detail::TextCompositeStatus;
using detail::TextCompositeTarget;
using detail::TextCoverage;
using detail::TextPixelOrder;

void require(const bool condition, const char* message) {
    if (condition) return;
    std::cerr << message << '\n';
    std::exit(1);
}

std::uint32_t composite_one(const std::uint8_t coverage_value, const Color color, const std::uint32_t destination,
                            const TextPixelOrder order, const bool opaque) {
    std::array<std::uint32_t, 1> pixels{destination};
    const std::array<std::uint8_t, 1> coverage{coverage_value};
    const TextCompositeStatus status = detail::composite_text_coverage(
        {.pixels = coverage, .width = 1, .height = 1, .left = 0, .top = 0},
        {.pixels = pixels, .width = 1, .height = 1, .stride = 1, .order = order, .opaque = opaque},
        {.rect = {0.0, 0.0, 1.0, 1.0}}, color);
    require(status == TextCompositeStatus::success, "single pixel composite");
    return pixels[0];
}

// The unclipped alpha must equal the pre-ADR-004 Windows rule exactly, so
// existing Windows output is unchanged wherever clips are whole pixels.
void unclipped_alpha_matches_previous_rule() {
    for (unsigned coverage = 0; coverage < 256U; ++coverage) {
        for (unsigned alpha = 0; alpha < 256U; ++alpha) {
            const unsigned previous = (coverage * alpha + 127U) / 255U;
            const unsigned shared = static_cast<unsigned>((static_cast<std::uint64_t>(coverage) * alpha * 256U + 32640U) / 65280U);
            require(previous == shared, "unclipped alpha equals the previous Windows rule");
        }
    }
    // Black at alpha 128 over white: (255 * 127 + 127) / 255 = 127 per channel.
    const std::uint32_t grey = composite_one(255U, Color::rgba(0, 0, 0, 128), 0xffffffffU, TextPixelOrder::bgra, true);
    require(grey == 0xff7f7f7fU, "previous Windows alpha/blend contract");
}

void pixel_orders_agree() {
    const Color colour = Color::rgba(200, 40, 90, 230);
    for (unsigned coverage = 0; coverage < 256U; coverage += 15U) {
        const std::uint32_t bgra_destination = (12U << 16U) | (34U << 8U) | 56U | (200U << 24U);
        const std::uint32_t rgba_destination = 12U | (34U << 8U) | (56U << 16U) | (200U << 24U);
        const std::uint32_t bgra = composite_one(static_cast<std::uint8_t>(coverage), colour, bgra_destination, TextPixelOrder::bgra, false);
        const std::uint32_t rgba = composite_one(static_cast<std::uint8_t>(coverage), colour, rgba_destination, TextPixelOrder::rgba, false);
        const std::uint32_t swapped = (rgba & 0xff00ff00U) | ((rgba & 0xffU) << 16U) | ((rgba >> 16U) & 0xffU);
        require(bgra == swapped, "BGRA and RGBA give the same channels");
    }
}

void alpha_channel_modes() {
    const std::uint32_t transparent = 0U;
    const std::uint32_t premultiplied = composite_one(255U, Color::rgba(255, 255, 255, 128), transparent, TextPixelOrder::rgba, false);
    require((premultiplied >> 24U) == 128U, "premultiplied destination gains the source alpha");
    const std::uint32_t opaque = composite_one(255U, Color::rgba(255, 255, 255, 128), transparent, TextPixelOrder::bgra, true);
    require((opaque >> 24U) == 255U, "opaque destinations stay opaque");
}

void fractional_rect_edges_are_area_coverage() {
    // A clip starting a quarter into pixel 0 covers three quarters of it.
    const TextCompositeClip quarter{.rect = {0.25, 0.0, 10.0, 10.0}};
    require(detail::text_clip_coverage(quarter, 0, 0) == 192U, "three-quarter edge coverage");
    require(detail::text_clip_coverage(quarter, 1, 0) == 256U, "interior full coverage");
    require(detail::text_clip_coverage(quarter, 11, 0) == 0U, "exterior zero coverage");
    const TextCompositeClip corner{.rect = {0.5, 0.5, 10.0, 10.0}};
    require(detail::text_clip_coverage(corner, 0, 0) == 64U, "corner pixel is the product of both edges");
    const TextCompositeClip whole{.rect = {2.0, 2.0, 4.0, 4.0}};
    require(detail::text_clip_coverage(whole, 1, 3) == 0U && detail::text_clip_coverage(whole, 2, 3) == 256U &&
            detail::text_clip_coverage(whole, 5, 3) == 256U && detail::text_clip_coverage(whole, 6, 3) == 0U,
            "whole-pixel edges stay hard");
}

void rounded_corners_are_antialiased() {
    const TextCompositeClip round{.rect = {0.0, 0.0, 40.0, 40.0}, .rounded = true,
                                  .rounded_rect = {0.0, 0.0, 40.0, 40.0}, .radius = 16.0};
    require(detail::text_clip_coverage(round, 0, 0) == 0U, "outer corner pixel is outside");
    require(detail::text_clip_coverage(round, 20, 20) == 256U, "centre is fully inside");
    require(detail::text_clip_coverage(round, 0, 20) == 256U, "straight edge is unaffected");
    // Along the diagonal coverage rises monotonically through partial values.
    std::uint32_t previous = 0U;
    bool partial = false;
    for (std::int64_t step = 0; step < 16; ++step) {
        const std::uint32_t value = detail::text_clip_coverage(round, step, step);
        require(value >= previous, "corner coverage is monotonic inward");
        if (value > 0U && value < 256U) partial = true;
        previous = value;
    }
    require(partial, "corner arc has anti-aliased pixels");
    // All four corners agree by symmetry.
    for (std::int64_t offset = 0; offset < 16; ++offset) {
        for (std::int64_t other = 0; other < 16; ++other) {
            const std::uint32_t top_left = detail::text_clip_coverage(round, offset, other);
            require(top_left == detail::text_clip_coverage(round, 39 - offset, other) &&
                    top_left == detail::text_clip_coverage(round, offset, 39 - other) &&
                    top_left == detail::text_clip_coverage(round, 39 - offset, 39 - other),
                    "rounded corners are symmetric");
        }
    }
}

void invalid_geometry_writes_nothing() {
    std::vector<std::uint32_t> pixels(16, 0x11223344U);
    const std::vector<std::uint32_t> before = pixels;
    const std::array<std::uint8_t, 4> coverage{255U, 255U, 255U, 255U};
    const TextCompositeStatus stride = detail::composite_text_coverage(
        {.pixels = coverage, .width = 2, .height = 2},
        {.pixels = pixels, .width = 4, .height = 4, .stride = 3}, {.rect = {0.0, 0.0, 4.0, 4.0}}, Color::rgba(0, 0, 0, 255));
    const TextCompositeStatus clip = detail::composite_text_coverage(
        {.pixels = coverage, .width = 2, .height = 2},
        {.pixels = pixels, .width = 4, .height = 4, .stride = 4}, {.rect = {0.0, 0.0, -1.0, 4.0}}, Color::rgba(0, 0, 0, 255));
    require(stride == TextCompositeStatus::invalid_geometry && clip == TextCompositeStatus::invalid_geometry && pixels == before,
            "invalid geometry leaves the target untouched");
}

void clipped_composite_blends_partially() {
    std::vector<std::uint32_t> pixels(4, 0xffffffffU);
    const std::array<std::uint8_t, 4> coverage{255U, 255U, 255U, 255U};
    const TextCompositeStatus status = detail::composite_text_coverage(
        {.pixels = coverage, .width = 4, .height = 1},
        {.pixels = pixels, .width = 4, .height = 1, .stride = 4, .order = TextPixelOrder::bgra, .opaque = true},
        {.rect = {0.5, 0.0, 3.0, 1.0}}, Color::rgba(0, 0, 0, 255));
    require(status == TextCompositeStatus::success, "clipped composite succeeds");
    // Half-covered edge pixels blend half way; covered pixels are black.
    require(pixels[0] == 0xff7f7f7fU && pixels[1] == 0xff000000U && pixels[2] == 0xff000000U && pixels[3] == 0xff7f7f7fU,
            "partially clipped pixels blend by clip area");
}

} // namespace

// SourceCopy replaces the destination with the premultiplied colour in
// proportion to coverage; it agrees with source-over for opaque colours.
void source_mode_replaces() {
    std::array<std::uint32_t, 1> pixels{0xff00ff00U};
    const std::array<std::uint8_t, 1> full{255U};
    const TextCompositeTarget target{.pixels = pixels, .width = 1, .height = 1, .stride = 1, .order = TextPixelOrder::bgra};
    const TextCompositeClip clip{.rect = {0.0, 0.0, 1.0, 1.0}};
    TextCompositeStatus status = detail::composite_text_coverage({.pixels = full, .width = 1, .height = 1}, target, clip,
                                                                 Color::rgba(255, 0, 0, 128), detail::TextCompositeMode::source);
    // Premultiplied red at alpha 128 replaces opaque green entirely.
    require(status == TextCompositeStatus::success && pixels[0] == 0x80800000U, "full coverage copies the premultiplied colour");
    pixels[0] = 0xff00ff00U;
    const std::array<std::uint8_t, 1> half{128U};
    status = detail::composite_text_coverage({.pixels = half, .width = 1, .height = 1}, target, clip, Color::rgba(0, 0, 0, 0),
                                             detail::TextCompositeMode::source);
    // Transparent colour erases in proportion to coverage: (255 * 127 + 127) / 255.
    require(status == TextCompositeStatus::success && pixels[0] == 0x7f007f00U, "transparent source erases by coverage");
    for (unsigned coverage = 0; coverage < 256U; coverage += 17U) {
        const std::array<std::uint8_t, 1> value{static_cast<std::uint8_t>(coverage)};
        std::array<std::uint32_t, 1> over{0xff336699U};
        std::array<std::uint32_t, 1> copy{0xff336699U};
        const Color colour = Color::rgba(200, 40, 90, 255);
        static_cast<void>(detail::composite_text_coverage({.pixels = value, .width = 1, .height = 1},
                                                          {.pixels = over, .width = 1, .height = 1, .stride = 1}, clip, colour));
        static_cast<void>(detail::composite_text_coverage({.pixels = value, .width = 1, .height = 1},
                                                          {.pixels = copy, .width = 1, .height = 1, .stride = 1}, clip, colour,
                                                          detail::TextCompositeMode::source));
        require(over[0] == copy[0], "opaque source equals source-over");
    }
}

int main() {
    unclipped_alpha_matches_previous_rule();
    pixel_orders_agree();
    alpha_channel_modes();
    fractional_rect_edges_are_area_coverage();
    rounded_corners_are_antialiased();
    invalid_geometry_writes_nothing();
    clipped_composite_blends_partially();
    source_mode_replaces();
    return 0;
}
