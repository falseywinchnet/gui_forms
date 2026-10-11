#include "page_tests.hpp"

namespace gallery_pages_tests {

void test_tree_and_object_views(gui_forms::Window& window) {
    select_page(window, "tree-and-object-views");
    require(window.find("gallery.tree-and-object-views.heading") != nullptr,
            "tree-and-object-views page shows its heading");
}

}  // namespace gallery_pages_tests
