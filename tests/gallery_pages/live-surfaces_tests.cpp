#include "page_tests.hpp"

namespace gallery_pages_tests {

void test_live_surfaces(gui_forms::Window& window) {
    select_page(window, "live-surfaces");
    require(window.find("gallery.live-surfaces.heading") != nullptr,
            "live-surfaces page shows its heading");
}

}  // namespace gallery_pages_tests
