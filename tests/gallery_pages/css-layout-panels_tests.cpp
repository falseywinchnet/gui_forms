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

    // The named areas: the head and foot span both columns, the main area
    // follows the fixed 90 px side column and its gap.
    const std::shared_ptr<GridLayoutPanel> areas =
        std::dynamic_pointer_cast<GridLayoutPanel>(window.find("gallery.css-layout.grid.areas"));
    const Control::Ptr head = window.find("gallery.css-layout.grid.areas.head");
    const Control::Ptr main = window.find("gallery.css-layout.grid.areas.main");
    const Control::Ptr foot = window.find("gallery.css-layout.grid.areas.foot");
    require(areas && head && main && foot, "the page must retain its named-area GridLayoutPanel");
    const Rect area_bounds = (*areas).arranged_bounds();
    const Rect foot_bounds = (*foot).arranged_bounds();
    require(std::abs((*head).arranged_bounds().width - area_bounds.width) < 1.0e-6 &&
                std::abs(foot_bounds.width - area_bounds.width) < 1.0e-6 &&
                std::abs((*main).arranged_bounds().x - 96.0) < 1.0e-6 &&
                std::abs(foot_bounds.y + foot_bounds.height - area_bounds.height) < 1.0e-6,
            "named areas must span their columns and the fraction row must fill the height");

    // The cards: five minmax(86px, 1fr) columns fit 476 px; the spanning
    // card covers two of them and the gap between.
    const std::shared_ptr<GridLayoutPanel> cards =
        std::dynamic_pointer_cast<GridLayoutPanel>(window.find("gallery.css-layout.grid.cards"));
    const Control::Ptr first_card = window.find("gallery.css-layout.grid.cards.item.0");
    const Control::Ptr spanning = window.find("gallery.css-layout.grid.cards.item.1");
    require(cards && first_card && spanning && (*cards).layout_snapshot().columns().size() == 5U,
            "five auto-fill card columns must fit the specimen");
    const double card_width = (*first_card).arranged_bounds().width;
    require(card_width >= 86.0 && std::abs((*spanning).arranged_bounds().width - (2.0 * card_width + 6.0)) < 1.0e-6,
            "the spanning card must cover two columns and their gap");

    const Control::Ptr grid_group = window.find("gallery.css-layout.grid.group");
    require(grid_group && (*grid_group).arranged_bounds().y + (*grid_group).arranged_bounds().height <= 660.0,
            "the grid group must fit the page body");

    const std::string semantics = window.semantic_snapshot().to_json();
    require(semantics.find("Growing flex row") != std::string::npos &&
                semantics.find("Centred flex column") != std::string::npos &&
                semantics.find("Named grid areas") != std::string::npos &&
                semantics.find("Auto-fill grid cards") != std::string::npos,
            "the flex and grid specimens must publish their named group semantics");
}

} // namespace gallery_pages_tests
