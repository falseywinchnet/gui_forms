#include "page_tests.hpp"

namespace gallery_pages_tests {

void test_typography_and_inspection(gui_forms::Window& window) {
    select_page(window, "typography-and-inspection");
    require(window.find("gallery.typography-and-inspection.heading") != nullptr,
            "typography-and-inspection page shows its heading");
}

}  // namespace gallery_pages_tests
