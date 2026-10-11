#include "support.hpp"

namespace gallery_pages_tests {

void test_collections_and_popups(gui_forms::Window& window) {
    select_page(window, "collections-and-popups");
    std::shared_ptr<gui_forms::ListBox> multi = std::dynamic_pointer_cast<ListBox>(
        window.find("gallery.collections.multi.list"));
    require(multi && (*multi).selected_indices().size() == 4U,
            "gallery extended ListBox must retain its initial selection range");
    std::shared_ptr<gui_forms::ComboBox> combo = std::dynamic_pointer_cast<ComboBox>(
        window.find("gallery.collections.combo.density"));
    click(window, combo);
    require((*combo).dropped_down() && window.focus_scope_depth() == 1U &&
                window.find("gallery.collections.combo.density.popup.list"),
            "gallery ComboBox must open through the public popup controller");
    const Control::Ptr popup =
        window.find("gallery.collections.combo.density.popup.list");
    window.perform_layout();
    const Rect bounds = (*popup).absolute_bounds();
    const Point choice{bounds.x + 20.0, bounds.y + 2.0 + 2.0 * 26.0 + 13.0};
    require(window.dispatch_pointer({PointerAction::down, PointerButton::primary, choice}) &&
                window.dispatch_pointer({PointerAction::up, PointerButton::primary, choice}) &&
                (*combo).selected_index() == 2U && !(*combo).dropped_down() &&
                window.focus_scope_depth() == 0U,
            "gallery ComboBox popup must commit and restore focus without residue");
}

} // namespace gallery_pages_tests
