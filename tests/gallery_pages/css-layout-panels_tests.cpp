#include "support.hpp"

namespace gallery_pages_tests {

void test_css_layout_panels(gui_forms::Window& window) {
    select_page(window, "css-layout-panels");
    require(window.find("gallery.css-layout.heading") != nullptr, "css-layout-panels page shows its heading");

    // The flex row: equal bases, so the two-share item grows twice as much;
    // the auto-margin item ends flush with the content box's right edge.
    const std::shared_ptr<FlexLayoutPanel> flex_row =
        std::dynamic_pointer_cast<FlexLayoutPanel>(window.find("gallery.css-layout.flex.row"));
    const Control::Ptr one = window.find("gallery.css-layout.flex.row.item.0");
    const Control::Ptr two = window.find("gallery.css-layout.flex.row.item.2");
    const Control::Ptr pushed = window.find("gallery.css-layout.flex.row.pushed");
    const std::shared_ptr<FlexLayoutPanel> flex_column =
        std::dynamic_pointer_cast<FlexLayoutPanel>(window.find("gallery.css-layout.flex.column"));
    const Control::Ptr narrow = window.find("gallery.css-layout.flex.column.item.0");
    const Control::Ptr wide = window.find("gallery.css-layout.flex.column.item.2");
    window.perform_layout();
    require(flex_row && one && two && pushed && flex_column && narrow && wide,
            "the page must retain its two FlexLayoutPanel specimens");
    const double one_growth = (*one).arranged_bounds().width - (*one).requested_bounds().width;
    const double two_growth = (*two).arranged_bounds().width - (*two).requested_bounds().width;
    const Rect row_bounds = (*flex_row).arranged_bounds();
    const Rect pushed_bounds = (*pushed).arranged_bounds();
    require(one_growth > 0.0 && std::abs(two_growth - 2.0 * one_growth) < 1.0e-6,
            "a two-share flex item must grow twice as much as a one-share item");
    require(std::abs(pushed_bounds.x + pushed_bounds.width - (row_bounds.width - 6.0)) < 1.0e-6 &&
                pushed_bounds.y > (*one).arranged_bounds().y,
            "the auto-margin item must wrap to its own line and end at the content edge");
    const Rect narrow_bounds = (*narrow).arranged_bounds();
    const Rect wide_bounds = (*wide).arranged_bounds();
    require(std::abs((narrow_bounds.x + narrow_bounds.width / 2.0) - (wide_bounds.x + wide_bounds.width / 2.0)) <
                    1.0e-6 &&
                narrow_bounds.width < wide_bounds.width,
            "the centred column must centre items of different content widths");

    // Every specimen lies within the page body (660 px high), so none is
    // clipped.
    const Control::Ptr flex_group = window.find("gallery.css-layout.flex.group");
    require(flex_group && (*flex_group).arranged_bounds().y + (*flex_group).arranged_bounds().height <= 660.0,
            "the flex group must fit the page body");

    const std::string semantics = window.semantic_snapshot().to_json();
    require(semantics.find("Growing flex row") != std::string::npos &&
                semantics.find("Centred flex column") != std::string::npos,
            "the flex specimens must publish their named group semantics");
}

} // namespace gallery_pages_tests
