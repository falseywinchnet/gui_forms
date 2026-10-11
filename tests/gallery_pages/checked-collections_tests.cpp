#include "support.hpp"

namespace gallery_pages_tests {

void test_checked_collections(gui_forms::Window& window) {
    select_page(window, "checked-collections");
    std::shared_ptr<gui_forms::CheckedListBox> checked = std::dynamic_pointer_cast<CheckedListBox>(
        window.find("gallery.checked.immediate.list"));
    require(checked && (*checked).check_on_click() &&
                (*checked).item_check_state(3U) == CheckState::indeterminate,
            "gallery checked page must retain a public check-state collection");
    const Rect checked_bounds = (*checked).absolute_bounds();
    const Point last_row{checked_bounds.x + 14.0,
                         checked_bounds.y + 2.0 + 5.5 * (*checked).item_height()};
    require(window.dispatch_pointer({PointerAction::down, PointerButton::primary,
                                     last_row}) &&
                window.dispatch_pointer({PointerAction::up, PointerButton::primary,
                                         last_row}) &&
                (*checked).item_checked(5U),
            "gallery CheckOnClick row must select and toggle immediately");
    const std::string checked_semantics = window.semantic_snapshot().to_json();
    require(checked_semantics.find("\"role\":\"check_list_item\"") !=
                std::string::npos,
            "gallery checked rows must publish checkable virtual semantics");
}

} // namespace gallery_pages_tests
