#include "support.hpp"

namespace gallery_pages_tests {

void test_collections_and_popups(gui_forms::Window& window) {
    select_page(window, "collections-and-popups");
    std::shared_ptr<gui_forms::ListBox> multi = std::dynamic_pointer_cast<ListBox>(
        window.find("gallery.collections.multi.list"));
    require(multi && (*multi).selected_indices().size() == 4U,
            "gallery extended ListBox must retain its initial selection range");
    std::shared_ptr<gui_forms::Label> status = std::dynamic_pointer_cast<Label>(
        window.find("gallery.collections.status"));
    std::shared_ptr<gui_forms::ComboBox> combo = std::dynamic_pointer_cast<ComboBox>(
        window.find("gallery.collections.combo.density"));
    click(window, combo);
    require((*combo).dropped_down() && window.focus_scope_depth() == 1U &&
                window.find("gallery.collections.combo.density.popup.list"),
            "gallery ComboBox must open through the public popup controller");
    require(status && (*status).text().find("Density popup: OPEN") == 0U,
            "the page owner must observe drop_down_changed(true) by named handler");
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
    require((*status).text().find("Density popup: CLOSED") == 0U,
            "the page owner must observe drop_down_changed(false) after commit");
    // A programmatic selection reaches the owner through the state event only;
    // item_activated stays reserved for user input.
    std::shared_ptr<gui_forms::ListBox> single = std::dynamic_pointer_cast<ListBox>(
        window.find("gallery.collections.single.list"));
    require(single != nullptr, "gallery single-selection ListBox must resolve");
    (*single).select_index(5U);
    require((*status).text().find("Location 6 selected") == 0U,
            "ListBox selection_changed must drive the owner's status handler");

    // The searchable station field narrows its list as the user types.
    std::shared_ptr<gui_forms::ComboBox> search = std::dynamic_pointer_cast<ComboBox>(
        window.find("gallery.collections.combo.search"));
    require(search && (*search).editor() && window.request_focus((*search).editor()) &&
                window.dispatch_text({"hb"}) && (*search).dropped_down() &&
                (*search).drop_down_items().size() == 4U &&
                (*status).text() == "Stations matching \"hb\": 4",
            "typing in the station field filters its list by substring");
    require(window.dispatch_key({KeyAction::down, PhysicalKey::down}) &&
                window.dispatch_key({KeyAction::down, PhysicalKey::enter}) &&
                !(*search).dropped_down() && (*search).selected_index() == 5U &&
                (*search).text() == "Frankfurt (Main) Hbf",
            "the first filtered row is chosen by its item index");
}

} // namespace gallery_pages_tests
