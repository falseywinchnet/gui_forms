#include "gui_forms/gui_forms.hpp"
#include "skia_raster.hpp"

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

// ADR-004 identity: text drawn through the raster must produce the same bytes
// on every platform. Each case hashes the whole raster after drawing; the
// expected values were produced on macOS arm64 and Alpine aarch64 and are
// checked by every CI platform. GUI_FORMS_TEXT_IDENTITY_PRINT=1 prints the
// observed hashes instead of checking them.
namespace {
using namespace gui_forms;

void require(const bool condition, const char* message) {
    if (condition) return;
    std::cerr << message << '\n';
    std::exit(1);
}

std::uint64_t raster_hash(const render::SkiaRaster& raster) {
    const std::uint8_t* const bytes = static_cast<const std::uint8_t*>(raster.pixels());
    require(bytes != nullptr, "raster readable");
    std::uint64_t value = 1469598103934665603ULL;
    const std::size_t width_bytes = static_cast<std::size_t>(raster.pixel_width()) * 4U;
    for (std::uint32_t row = 0; row < raster.pixel_height(); ++row) {
        const std::uint8_t* const line = bytes + static_cast<std::size_t>(row) * raster.row_bytes();
        for (std::size_t column = 0; column < width_bytes; ++column) {
            value ^= line[column];
            value *= 1099511628211ULL;
        }
    }
    return value;
}

enum class Case : std::uint8_t { plain, translucent, fractional_clip, rounded_clip, damage_region };

constexpr std::array<Case, 5> cases{Case::plain, Case::translucent, Case::fractional_clip, Case::rounded_clip,
                                    Case::damage_region};
constexpr std::array<double, 3> scales{1.0, 1.5, 2.0};
constexpr std::array<const char*, 5> case_names{"plain", "translucent", "fractional_clip", "rounded_clip", "damage_region"};

// Expected hashes, [case][scale]: identical on macOS arm64 (Apple SDK) and
// Alpine aarch64 musl (Clang 22.1.3), 2026-10-11.
constexpr std::array<std::array<std::uint64_t, 3>, 5> expected{{
    {0x8d50f64b74fcf8b8ULL, 0x471b6a11e111ec7dULL, 0xab5fc975326f3f28ULL},
    {0x83f2cc636fa71c45ULL, 0x6c602d56fa0ed64eULL, 0x63a26ab8baf146e2ULL},
    {0x9f2321491ff1b2c3ULL, 0x9f4f28a24374934eULL, 0x2822334624cfadd3ULL},
    {0x5746df86d820334eULL, 0x24e0cb400ebdd4adULL, 0x1733822b6219aa3dULL},
    {0x2a513350b2c7890fULL, 0x65c047c6320db78fULL, 0x06b94ee7cb3d0779ULL},
}};

std::uint64_t draw_case(const Case kind, const double scale) {
    const Size size{160.0, 48.0};
    render::SkiaRaster raster{};
    require(raster.resize(size, scale), "raster allocated");
    require(raster.register_typeface_file(FontRole::control, 400, false, GUI_FORMS_TEST_CONTROL_FONT), "font registered");
    DamageRegion damage{};
    if (kind == Case::damage_region) {
        damage.add({0.0, 0.0, 60.0, 48.0});
        damage.add({90.0, 0.0, 70.0, 48.0});
    } else {
        damage.add({0.0, 0.0, size.width, size.height});
    }
    raster.begin_frame(damage);
    raster.fill_rect({0.0, 0.0, size.width, size.height}, Color::rgba(250, 248, 240, 255));
    raster.fill_rect({0.0, 24.0, size.width, 24.0}, Color::rgba(40, 70, 110, 255));
    const FontSpec font{.role = FontRole::control, .size = 15.0};
    Color colour = Color::rgba(20, 20, 30, 255);
    if (kind == Case::translucent) colour = Color::rgba(200, 60, 40, 170);
    raster.save();
    if (kind == Case::fractional_clip) raster.clip_rect({10.3, 0.0, 100.4, 48.0});
    if (kind == Case::rounded_clip) raster.clip_rounded_rect({4.0, 6.0, 120.0, 34.0}, 12.0);
    raster.translate({2.0, 0.0});
    raster.draw_text_utf8({3.25, 20.5}, "Hamburgefonstiv 0123 Help", font, colour);
    raster.draw_text_utf8({3.25, 41.0}, "quick brown fox jumps", font, Color::rgba(245, 245, 250, 255));
    raster.restore();
    raster.end_frame();
    return raster_hash(raster);
}

} // namespace

int main() {
    const char* const print = std::getenv("GUI_FORMS_TEXT_IDENTITY_PRINT");
    const bool printing = print != nullptr && std::strcmp(print, "1") == 0;
    bool mismatch = false;
    for (std::size_t kind = 0; kind < cases.size(); ++kind) {
        for (std::size_t scale = 0; scale < scales.size(); ++scale) {
            const std::uint64_t observed = draw_case(cases[kind], scales[scale]);
            std::printf("text-identity %s %.1f 0x%016llxULL\n", case_names[kind], scales[scale],
                        static_cast<unsigned long long>(observed));
            if (!printing && observed != expected[kind][scale]) mismatch = true;
        }
    }
    require(printing || !mismatch, "text output differs from the cross-platform expectation");
    return 0;
}
