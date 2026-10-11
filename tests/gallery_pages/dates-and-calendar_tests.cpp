#include "support.hpp"

namespace gallery_pages_tests {

void test_dates_and_calendar(gui_forms::Window& window) {
    select_page(window, "dates-and-calendar");

    const std::shared_ptr<gui_forms::DateTimePicker> long_date = std::dynamic_pointer_cast<DateTimePicker>(
        window.find("gallery.date.format.0"));
    const std::shared_ptr<gui_forms::DateTimePicker> short_date = std::dynamic_pointer_cast<DateTimePicker>(
        window.find("gallery.date.format.1"));
    const std::shared_ptr<gui_forms::DateTimePicker> time = std::dynamic_pointer_cast<DateTimePicker>(
        window.find("gallery.date.format.2"));
    const std::shared_ptr<gui_forms::DateTimePicker> custom = std::dynamic_pointer_cast<DateTimePicker>(
        window.find("gallery.date.format.3"));
    const std::shared_ptr<gui_forms::DateTimePicker> optional = std::dynamic_pointer_cast<DateTimePicker>(
        window.find("gallery.date.optional"));
    const std::shared_ptr<gui_forms::DateTimePicker> spinner = std::dynamic_pointer_cast<DateTimePicker>(
        window.find("gallery.date.spinner"));
    const std::shared_ptr<gui_forms::DateTimePicker> provider = std::dynamic_pointer_cast<DateTimePicker>(
        window.find("gallery.date.provider"));
    const std::shared_ptr<gui_forms::DateTimePicker> disabled = std::dynamic_pointer_cast<DateTimePicker>(
        window.find("gallery.date.disabled"));
    const std::shared_ptr<gui_forms::DateTimePicker> primary = std::dynamic_pointer_cast<DateTimePicker>(
        window.find("gallery.date.primary"));
    require(long_date && short_date && time && custom && optional && spinner &&
                provider && disabled && primary,
            "date board must use public DateTimePicker instances for every variant");
    require((*long_date).formatted_value() == "Wednesday, August 5, 2026" &&
                (*short_date).formatted_value() == "8/5/2026" &&
                (*time).formatted_value() == "2:07 PM" &&
                (*custom).formatted_value() == "2026-08-05 · 14:07" &&
                (*provider).formatted_value() == "mercredi 5 août 2026",
            "date board must visibly distinguish all format and provider paths");
    require((*optional).show_check_box() && !(*optional).checked() &&
                (*spinner).show_up_down() && !(*disabled).enabled(),
            "date board must retain nullable, spinner, and disabled states");

    const Rect optional_bounds = (*optional).absolute_bounds();
    const Point optional_check{optional_bounds.x + 12.0,
                               optional_bounds.y + optional_bounds.height * 0.5};
    require(window.dispatch_pointer({PointerAction::down, PointerButton::primary,
                                     optional_check}) &&
                window.dispatch_pointer({PointerAction::up, PointerButton::primary,
                                         optional_check}) && (*optional).checked(),
            "optional date checkbox must toggle through its public hit region");
    require(window.perform_semantic_action(
                "gallery.date.optional", SemanticAction::press) &&
                !(*optional).checked() &&
                window.perform_semantic_action(
                    "gallery.date.optional", SemanticAction::set_value,
                    "2026-08-09") && (*optional).checked() &&
                (*optional).value().day == 9U,
            "optional date semantics must toggle and activate a supplied value");
    require(window.request_focus(spinner) &&
                window.dispatch_key({KeyAction::down, PhysicalKey::up}) &&
                (*spinner).value().day == 6U,
            "date spinner must step its civil value through keyboard input");

    click(window, window.find("gallery.date.open"));
    require((*primary).dropped_down() && window.focus_scope_depth() == 1U &&
                window.find("gallery.date.primary.popup.calendar") &&
                window.semantic_snapshot().to_json().find(
                    "gallery.date.primary.popup.calendar.day.2026-08-12") !=
                    std::string::npos,
            "date board must open one retained semantic calendar focus scope");
    require(window.perform_semantic_action(
                "gallery.date.primary.popup.calendar.day.2026-08-12",
                SemanticAction::press) && (*primary).value().day == 12U &&
                !(*primary).dropped_down() && window.focus_scope_depth() == 0U,
            "semantic date-cell commit must share pointer commit and restore focus");

    click(window, window.find("gallery.date.open"));
    require(window.dispatch_key({KeyAction::down, PhysicalKey::left}) &&
                window.dispatch_key({KeyAction::down, PhysicalKey::escape}) &&
                (*primary).value().day == 12U && !(*primary).dropped_down(),
            "calendar Escape must discard popup-local navigation without mutation");

    click(window, window.find("gallery.date.open"));
    (*window.find("gallery.surface.dates-and-calendar")).set_visible(false);
    require(!(*primary).dropped_down() && window.focus_scope_depth() == 0U &&
                !window.find("gallery.date.primary.popup.calendar"),
            "hiding a page must revoke its open calendar and contained focus");
}

} // namespace gallery_pages_tests
