#include "support.hpp"

namespace gallery_pages_tests {

void test_tabs_and_pages(gui_forms::Window& window) {
    select_page(window, "tabs-and-pages");
    std::shared_ptr<gui_forms::TabControl> tabs = std::dynamic_pointer_cast<TabControl>(
        window.find("gallery.tabs.primary"));
    std::shared_ptr<gui_forms::Label> status = std::dynamic_pointer_cast<Label>(
        window.find("gallery.tabs.status"));
    std::shared_ptr<gui_forms::CheckBox> toggle = std::dynamic_pointer_cast<CheckBox>(
        window.find("gallery.tabs.primary.activity.toggle"));
    require(tabs && status && toggle && (*tabs).page_count() == 4U &&
                (*tabs).selected_index() == 1U && (*toggle).checked(),
            "gallery tab page must retain a real public TabControl page model");
    // The Activity toggle binds a Value<bool>; the page observes the value.
    click(window, toggle);
    require(!(*toggle).checked() &&
                (*status).text().find("keep visible off") != std::string::npos,
            "a bound Value<bool> must carry the toggle's user change to the page owner");
    const Rect security = (*tabs).tab_bounds(2U);
    const Rect tabs_absolute = (*tabs).absolute_bounds();
    const Point security_click{tabs_absolute.x + security.x + security.width * 0.5,
                               tabs_absolute.y + security.y + security.height * 0.5};
    require(window.dispatch_pointer({PointerAction::down, PointerButton::primary,
                                     security_click}) &&
                window.dispatch_pointer({PointerAction::up, PointerButton::primary,
                                         security_click}) &&
                (*tabs).selected_index() == 2U &&
                (*window.find("gallery.tabs.primary.security.radio")).effectively_visible() &&
                !(*toggle).effectively_visible() &&
                (*status).text().find("Security selected") != std::string::npos,
            "gallery tabs must select pages through public pointer behavior");
    require(window.perform_semantic_action(
                "gallery.tabs.primary.diagnostics.tab", SemanticAction::select) &&
                (*tabs).selected_index() == 3U &&
                (*status).text().find("Diagnostics selected") != std::string::npos,
            "gallery tabs must share semantic and pointer selection behavior");
    (*tabs).set_selected_index(0U);
    require((*tabs).selected_index() == 0U &&
                (*status).text().find("Overview selected") != std::string::npos,
            "programmatic selection must reach the same state event as user input");
}

} // namespace gallery_pages_tests
