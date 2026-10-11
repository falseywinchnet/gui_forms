#include "page_tests.hpp"

namespace gallery_pages_tests {

void test_commands_and_menus(gui_forms::Window& window) {
    select_page(window, "commands-and-menus");
    require(window.find("gallery.commands-and-menus.heading") != nullptr,
            "commands-and-menus page shows its heading");
}

}  // namespace gallery_pages_tests
