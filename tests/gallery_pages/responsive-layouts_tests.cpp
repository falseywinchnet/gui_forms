#include "page_tests.hpp"

namespace gallery_pages_tests {

void test_responsive_layouts(gui_forms::Window& window) {
    select_page(window, "responsive-layouts");
    require(window.find("gallery.responsive-layouts.heading") != nullptr,
            "responsive-layouts page shows its heading");
}

}  // namespace gallery_pages_tests
