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

    std::shared_ptr<gui_forms::PageStrip> strip = std::dynamic_pointer_cast<PageStrip>(
        window.find("gallery.tabs.strip"));
    std::shared_ptr<gui_forms::Label> strip_status = std::dynamic_pointer_cast<Label>(
        window.find("gallery.tabs.strip.status"));
    require(strip && strip_status && (*strip).page_set() != nullptr &&
                (*(*strip).page_set()).count() == 14U && (*strip).overflowing() &&
                (*strip).parts().list_button.present,
            "the gallery's PageStrip overflows into scroll and list buttons");
    require(window.perform_semantic_action("gallery.tabs.strip.tab.strip.13", SemanticAction::select) &&
                (*strip_status).text().find("About selected") != std::string::npos &&
                (*strip).visible_pages().back().value == "strip.13",
            "selecting the gallery strip's last page scrolls it into view and reports it");

    std::shared_ptr<gui_forms::PageView> view = std::dynamic_pointer_cast<PageView>(
        window.find("gallery.tabs.view"));
    std::shared_ptr<gui_forms::Label> view_status = std::dynamic_pointer_cast<Label>(
        window.find("gallery.tabs.view.status"));
    require(view && view_status && (*view).pages().count() == 4U &&
                (*view).page_content(PageId{"view.summary"}) &&
                !(*view).page_content(PageId{"view.report"}),
            "the gallery's PageView starts with two pages whose content comes late");
    require(window.perform_semantic_action("gallery.tabs.view.strip.tab.view.report", SemanticAction::select) &&
                (*view).page_content(PageId{"view.report"}) &&
                (*(*view).page_content(PageId{"view.report"})).effectively_visible() &&
                (*view_status).text().find("Created view.report") != std::string::npos,
            "selecting a late page in the gallery's PageView creates and shows its content");

    std::shared_ptr<gui_forms::NavigationPane> navigation = std::dynamic_pointer_cast<NavigationPane>(
        window.find("gallery.tabs.navigation"));
    std::shared_ptr<gui_forms::Label> navigation_status = std::dynamic_pointer_cast<Label>(
        window.find("gallery.tabs.navigation.status"));
    require(navigation && navigation_status && (*navigation).pages().count() == 6U &&
                (*navigation).page_hidden(PageId{"notes"}) && (*navigation).visible_button_count() == 4U,
            "the gallery's NavigationPane shows four large buttons and hides Notes");
    require(window.perform_semantic_action("gallery.tabs.navigation.tab.contacts", SemanticAction::select) &&
                (*(*navigation).pages().selected_page()).value == "contacts" &&
                (*window.find("gallery.tabs.navigation.content.contacts")).effectively_visible(),
            "selecting a navigation page shows its control");
    require(window.perform_semantic_action("gallery.tabs.navigation.collapse", SemanticAction::press) &&
                (*navigation).collapsed() &&
                (*navigation_status).text().find("Collapsed") != std::string::npos,
            "collapsing the gallery's NavigationPane reports its state");
    require(window.perform_semantic_action("gallery.tabs.navigation.collapse", SemanticAction::press) &&
                !(*navigation).collapsed(),
            "and it expands again");
}

} // namespace gallery_pages_tests
