// The theme engine's composite through the Windows DIB painter (T2 plan, E2b;
// GIVEN A2-2): the same scene as part_composite_identity_tests, checked
// against the same table, so Windows writes the bytes Skia writes on macOS
// and Linux. GUI_FORMS_PART_COMPOSITE_PRINT=1 prints the observed hashes
// instead of checking them.
#include "gui_forms/window.hpp"
#include "../src/host/windows/application/windows_host.cpp"
#include "part_composite_scene.hpp"

#include <cstdio>
#include <cstring>
#include <iostream>

using namespace gui_forms;
namespace scene = gui_forms::part_composite_identity;

int main() {
    const char* const print_value = std::getenv("GUI_FORMS_PART_COMPOSITE_PRINT");
    const bool printing = print_value != nullptr && std::strcmp(print_value, "1") == 0;
    bool mismatch = false;
    for (std::size_t kind = 0U; kind < scene::cases.size(); ++kind) {
        for (std::size_t scale = 0U; scale < scene::scales.size(); ++scale) {
            host::DibPainter painter;
            const std::unique_ptr<PaintFramebuffer> framebuffer =
                painter.create_framebuffer(scene::scene_size, scene::scales[scale]);
            if (framebuffer == nullptr) {
                std::cerr << "the identity framebuffer is allocated\n";
                return 1;
            }
            scene::paint_scene((*framebuffer).painter(), scene::cases[kind], scene::scales[scale]);
            (*framebuffer).end();
            const std::uint8_t* const bytes = reinterpret_cast<const std::uint8_t*>((*framebuffer).pixels().data());
            const std::uint64_t observed =
                text_identity::pixel_hash(bytes, (*framebuffer).width(), (*framebuffer).height(),
                                          (*framebuffer).row_bytes(), text_identity::Order::bgra);
            std::printf("part-composite %s %.1f 0x%016llxULL\n", scene::case_names[kind], scene::scales[scale],
                        static_cast<unsigned long long>(observed));
            if (observed != scene::expected[kind][scale]) mismatch = true;
        }
    }
    if (printing) return 0;
    if (mismatch) {
        std::cerr << "the Windows composite differs from the cross-platform expectation\n";
        return 1;
    }
    std::cout << "windows part composite tests passed\n";
    return 0;
}
