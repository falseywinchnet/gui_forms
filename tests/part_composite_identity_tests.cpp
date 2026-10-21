#include "gui_forms/gui_forms.hpp"
#include "skia_raster.hpp"
#include "part_composite_scene.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

// The theme engine's composite through the Skia raster (T2 plan, E2b;
// GUI_FORMS_PART_COMPOSITE_PRINT=1 prints the observed hashes instead of
// checking them). The Windows DIB painter checks the same table in
// windows_part_composite_tests.
namespace {
using namespace gui_forms;
namespace scene = gui_forms::part_composite_identity;

void require(const bool condition, const char* message) {
    if (condition) return;
    std::cerr << message << '\n';
    std::exit(1);
}

std::uint64_t draw_case(const std::size_t kind_index, const std::size_t scale_index) {
    const double scale = scene::scales[scale_index];
    render::SkiaRaster raster{};
    require(raster.resize(scene::scene_size, scale), "raster allocated");
    DamageRegion damage{};
    damage.add({0.0, 0.0, scene::scene_size.width, scene::scene_size.height});
    raster.begin_frame(damage);
    scene::paint_scene(raster, scene::cases[kind_index], scale);
    raster.end_frame();
    const std::uint8_t* const bytes = static_cast<const std::uint8_t*>(raster.pixels());
    require(bytes != nullptr, "raster readable");
    const std::uint64_t hash =
        text_identity::pixel_hash(bytes, raster.pixel_width(), raster.pixel_height(), raster.row_bytes());
    return hash;
}

} // namespace

int main() {
    const char* const print_value = std::getenv("GUI_FORMS_PART_COMPOSITE_PRINT");
    const bool printing = print_value != nullptr && std::strcmp(print_value, "1") == 0;
    bool mismatch = false;
    for (std::size_t kind = 0U; kind < scene::cases.size(); ++kind) {
        for (std::size_t scale = 0U; scale < scene::scales.size(); ++scale) {
            const std::uint64_t observed = draw_case(kind, scale);
            std::printf("part-composite %s %.1f 0x%016llxULL\n", scene::case_names[kind], scene::scales[scale],
                        static_cast<unsigned long long>(observed));
            if (observed != scene::expected[kind][scale]) mismatch = true;
        }
    }
    if (printing) return 0;
    require(!mismatch, "the Skia composite differs from the cross-platform expectation");
    std::cout << "part composite identity tests passed\n";
    return 0;
}
