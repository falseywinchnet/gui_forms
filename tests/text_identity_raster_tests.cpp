#include "gui_forms/gui_forms.hpp"
#include "skia_raster.hpp"
#include "text_identity_scene.hpp"

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

std::uint64_t raster_hash(const render::SkiaRaster& raster, const std::size_t kind, const std::size_t scale) {
    const std::uint8_t* const bytes = static_cast<const std::uint8_t*>(raster.pixels());
    require(bytes != nullptr, "raster readable");
    text_identity::dump_pixels(bytes, raster.pixel_width(), raster.pixel_height(), raster.row_bytes(), kind, scale);
    return text_identity::pixel_hash(bytes, raster.pixel_width(), raster.pixel_height(), raster.row_bytes());
}

std::uint64_t draw_case(const std::size_t kind_index, const std::size_t scale_index) {
    const text_identity::Case kind = text_identity::cases[kind_index];
    const double scale = text_identity::scales[scale_index];
    render::SkiaRaster raster{};
    require(raster.resize(text_identity::scene_size, scale), "raster allocated");
    require(raster.register_typeface_file(FontRole::content, 400, false, GUI_FORMS_TEST_CONTROL_FONT), "font registered");
    DamageRegion damage{};
    if (kind == text_identity::Case::damage_region) {
        damage.add({0.0, 0.0, 60.0, 48.0});
        damage.add({90.0, 0.0, 70.0, 48.0});
    } else {
        damage.add({0.0, 0.0, text_identity::scene_size.width, text_identity::scene_size.height});
    }
    raster.begin_frame(damage);
    text_identity::paint_scene(raster, kind);
    raster.end_frame();
    return raster_hash(raster, kind_index, scale_index);
}

} // namespace

int main() {
    const char* const print = std::getenv("GUI_FORMS_TEXT_IDENTITY_PRINT");
    const bool printing = print != nullptr && std::strcmp(print, "1") == 0;
    bool mismatch = false;
    for (std::size_t kind = 0; kind < text_identity::cases.size(); ++kind) {
        for (std::size_t scale = 0; scale < text_identity::scales.size(); ++scale) {
            const std::uint64_t observed = draw_case(kind, scale);
            std::printf("text-identity %s %.1f 0x%016llxULL\n", text_identity::case_names[kind],
                        text_identity::scales[scale], static_cast<unsigned long long>(observed));
            if (!printing && observed != text_identity::expected[kind][scale]) mismatch = true;
        }
    }
    require(printing || !mismatch, "text output differs from the cross-platform expectation");
    return 0;
}
