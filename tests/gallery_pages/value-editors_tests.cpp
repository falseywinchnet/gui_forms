#include "page_tests.hpp"

namespace gallery_pages_tests {

void test_value_editors(gui_forms::Window& window) {
    select_page(window, "value-editors");
    require(window.find("gallery.value-editors.heading") != nullptr,
            "value-editors page shows its heading");
}

}  // namespace gallery_pages_tests
