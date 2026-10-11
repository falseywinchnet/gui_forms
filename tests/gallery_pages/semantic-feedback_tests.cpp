#include "page_tests.hpp"

namespace gallery_pages_tests {

void test_semantic_feedback(gui_forms::Window& window) {
    select_page(window, "semantic-feedback");
    require(window.find("gallery.semantic-feedback.heading") != nullptr,
            "semantic-feedback page shows its heading");
}

}  // namespace gallery_pages_tests
