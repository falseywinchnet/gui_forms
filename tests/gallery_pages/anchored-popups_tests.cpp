#include "page_tests.hpp"

namespace gallery_pages_tests {

void test_anchored_popups(gui_forms::Window& window) {
    select_page(window, "anchored-popups");
    require(window.find("gallery.anchored-popups.heading") != nullptr,
            "anchored-popups page shows its heading");
}

}  // namespace gallery_pages_tests
