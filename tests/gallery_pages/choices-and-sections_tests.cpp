#include "page_tests.hpp"

namespace gallery_pages_tests {

void test_choices_and_sections(gui_forms::Window& window) {
    select_page(window, "choices-and-sections");
    require(window.find("gallery.choices-and-sections.heading") != nullptr,
            "choices-and-sections page shows its heading");
}

}  // namespace gallery_pages_tests
