#include "pages.hpp"

#include <array>

namespace gui_forms::gallery::pages {

// Owner of the CSS Layout Panels page: the containers of stage L that lay
// their children out by CSS rules, each as one group of specimens: the
// FlexLayoutPanel group (phase L3), the GridLayoutPanel group (L4) and the
// BlockFlowPanel group (L5).
class CssLayoutPanelsPage final : public gui_forms::Component {
public:
    CssLayoutPanelsPage(const std::shared_ptr<Surface>& page,
                        const std::shared_ptr<PageContext>& context) {
        static_cast<void>(context);
        (*page).add_at(label("gallery.css-layout.heading", "CSS LAYOUT PANELS",
                             22.0, 700, shell_blue_dark),
                       {24.0, 18.0, 620.0, 34.0});
        (*page).add_at(label("gallery.css-layout.lead",
                             "Containers that lay retained children out by CSS rules, checked against Chromium's boxes by the layout corpus.",
                             12.0, 400, muted),
                       {26.0, 52.0, 920.0, 24.0});
        build_flex(page);
        build_grid(page);
        build_block(page);
    }

private:
    // A wrapping row whose items grow by one, one and two shares, with the
    // last pushed to the end by its automatic start margin; and a column
    // centred on its main axis with its items at their content widths.
    void build_flex(const std::shared_ptr<Surface>& page) {
        std::shared_ptr<LayoutGroup> flex_group = group(
            "gallery.css-layout.flex.group",
            "FlexLayoutPanel · grow shares · wrap · gaps · auto margin · centred column",
            {930.0, 210.0});
        (*page).add_at(flex_group, {24.0, 92.0, 930.0, 210.0});

        std::shared_ptr<FlexLayoutPanel> row = make_control<FlexLayoutPanel>(
            StableId("gallery.css-layout.flex.row"));
        (*row).set_accessible_name("Growing flex row");
        (*row).set_flex_wrap(FlexWrap::wrap);
        (*row).set_align_items(LayoutAlignment::center);
        (*row).set_column_gap(LengthPercentage{LogicalLength{8.0}, 0.0});
        (*row).set_row_gap(LengthPercentage{LogicalLength{6.0}, 0.0});
        (*row).set_padding({6.0, 6.0, 6.0, 6.0});
        (*flex_group).add_at(row, {18.0, 38.0, 560.0, 152.0});
        constexpr std::array<double, 3> shares{1.0, 1.0, 2.0};
        for (std::size_t index = 0U; index < shares.size(); ++index) {
            std::shared_ptr<Button> item = make_control<Button>(
                StableId("gallery.css-layout.flex.row.item." + std::to_string(index)),
                "Grow " + std::to_string(static_cast<int>(shares[index])));
            (*item).set_visual_style(ButtonVisualStyle::command);
            (*item).set_requested_bounds({0.0, 0.0, 140.0, 28.0});
            (*item).set_margin({0.0, 0.0, 0.0, 0.0});
            (*row).add_child(item);
            FlexItemLayout layout{};
            layout.flex.grow = FlexFactor{shares[index]};
            (*row).set_item_layout(*item, layout);
        }
        std::shared_ptr<Button> pushed = make_control<Button>(
            StableId("gallery.css-layout.flex.row.pushed"), "Auto margin");
        (*pushed).set_visual_style(ButtonVisualStyle::accent);
        (*pushed).set_requested_bounds({0.0, 0.0, 120.0, 28.0});
        (*pushed).set_margin({0.0, 0.0, 0.0, 0.0});
        (*row).add_child(pushed);
        FlexItemLayout pushed_layout{};
        pushed_layout.auto_margin.left = true;
        (*row).set_item_layout(*pushed, pushed_layout);

        std::shared_ptr<FlexLayoutPanel> column = make_control<FlexLayoutPanel>(
            StableId("gallery.css-layout.flex.column"));
        (*column).set_accessible_name("Centred flex column");
        (*column).set_flex_direction(FlexDirection::column);
        (*column).set_justify_content(LayoutAlignment::center);
        (*column).set_align_items(LayoutAlignment::center);
        (*column).set_row_gap(LengthPercentage{LogicalLength{6.0}, 0.0});
        (*flex_group).add_at(column, {596.0, 38.0, 316.0, 152.0});
        for (std::size_t index = 0U; index < 3U; ++index) {
            std::shared_ptr<Button> item = make_control<Button>(
                StableId("gallery.css-layout.flex.column.item." + std::to_string(index)),
                std::string(index + 1U, 'W'));
            (*item).set_visual_style(ButtonVisualStyle::standard);
            (*item).set_requested_bounds({0.0, 0.0, 60.0 + static_cast<double>(index) * 40.0, 26.0});
            (*item).set_margin({0.0, 0.0, 0.0, 0.0});
            (*column).add_child(item);
        }
    }

    // Named areas with a fixed side column and a flexible main area; and
    // cards repeated as often as minmax(86px, 1fr) columns fit, one of them
    // spanning two columns.
    void build_grid(const std::shared_ptr<Surface>& page) {
        std::shared_ptr<LayoutGroup> grid_group = group(
            "gallery.css-layout.grid.group",
            "GridLayoutPanel · named areas · fr tracks · auto-fill minmax cards · span",
            {930.0, 210.0});
        (*page).add_at(grid_group, {24.0, 312.0, 930.0, 210.0});

        std::shared_ptr<GridLayoutPanel> areas = make_control<GridLayoutPanel>(
            StableId("gallery.css-layout.grid.areas"));
        (*areas).set_accessible_name("Named grid areas");
        GridTrackList columns{};
        columns.tracks = {fixed_track(90.0), fraction_track(1.0)};
        (*areas).set_template_columns(columns);
        GridTrackList rows{};
        rows.tracks = {fixed_track(28.0), fraction_track(1.0), fixed_track(24.0)};
        (*areas).set_template_rows(rows);
        GridTemplateAreas names{};
        names.rows = 3U;
        names.columns = 2U;
        names.areas = {GridNamedArea{"head", 0U, 1U, 0U, 2U}, GridNamedArea{"nav", 1U, 2U, 0U, 1U},
                       GridNamedArea{"main", 1U, 2U, 1U, 2U}, GridNamedArea{"foot", 2U, 3U, 0U, 2U}};
        (*areas).set_template_areas(names);
        (*areas).set_column_gap(LengthPercentage{LogicalLength{6.0}, 0.0});
        (*areas).set_row_gap(LengthPercentage{LogicalLength{6.0}, 0.0});
        (*grid_group).add_at(areas, {18.0, 38.0, 400.0, 152.0});
        constexpr std::array<const char*, 4> area_names{"head", "nav", "main", "foot"};
        for (const char* name : area_names) {
            std::shared_ptr<Button> item = make_control<Button>(
                StableId(std::string("gallery.css-layout.grid.areas.") + name), name);
            (*item).set_visual_style(ButtonVisualStyle::command);
            (*item).set_margin({0.0, 0.0, 0.0, 0.0});
            (*areas).add_child(item);
            GridLine line{};
            line.kind = GridLineKind::area;
            line.area = name;
            GridItemLayout layout{};
            layout.placement = GridItemStyle{line, line, line, line};
            (*areas).set_item_layout(*item, layout);
        }

        std::shared_ptr<GridLayoutPanel> cards = make_control<GridLayoutPanel>(
            StableId("gallery.css-layout.grid.cards"));
        (*cards).set_accessible_name("Auto-fill grid cards");
        GridTrackSize card{};
        card.minimum = TrackBreadth{TrackBreadthKind::length_percentage, LengthPercentage{LogicalLength{86.0}, 0.0},
                                    FractionFactor{}};
        card.maximum = TrackBreadth{TrackBreadthKind::fraction, LengthPercentage{}, FractionFactor{1.0}};
        GridTrackList card_columns{};
        card_columns.auto_repeat = AutoRepeatKind::auto_fill;
        card_columns.auto_repeat_tracks = {card};
        (*cards).set_template_columns(card_columns);
        const std::array<GridTrackSize, 1> card_rows{fixed_track(28.0)};
        (*cards).set_auto_rows(card_rows);
        (*cards).set_column_gap(LengthPercentage{LogicalLength{6.0}, 0.0});
        (*cards).set_row_gap(LengthPercentage{LogicalLength{6.0}, 0.0});
        (*cards).set_align_content(LayoutAlignment::start);
        (*grid_group).add_at(cards, {436.0, 38.0, 476.0, 152.0});
        for (std::size_t index = 0U; index < 8U; ++index) {
            const bool spans = index == 1U;
            const std::string caption = spans ? std::string("Span 2") : "Card " + std::to_string(index + 1U);
            std::shared_ptr<Button> item = make_control<Button>(
                StableId("gallery.css-layout.grid.cards.item." + std::to_string(index)), caption);
            (*item).set_visual_style(spans ? ButtonVisualStyle::accent : ButtonVisualStyle::standard);
            (*item).set_margin({0.0, 0.0, 0.0, 0.0});
            (*cards).add_child(item);
            if (spans) {
                GridItemLayout spanning{};
                spanning.placement.column_start.kind = GridLineKind::span;
                spanning.placement.column_start.number = 2;
                (*cards).set_item_layout(*item, spanning);
            }
        }
    }

    // Stacked blocks whose margins collapse, the middle one narrowed and
    // centred by auto margins; and a centred line of inline buttons that
    // wraps, one of them taller and aligned on the line's middle.
    void build_block(const std::shared_ptr<Surface>& page) {
        std::shared_ptr<LayoutGroup> block_group = group(
            "gallery.css-layout.block.group",
            "BlockFlowPanel · collapsing margins · auto margins · centred inline lines",
            {930.0, 124.0});
        (*page).add_at(block_group, {24.0, 532.0, 930.0, 124.0});

        std::shared_ptr<BlockFlowPanel> stack = make_control<BlockFlowPanel>(
            StableId("gallery.css-layout.block.stack"));
        (*stack).set_accessible_name("Stacked blocks");
        (*block_group).add_at(stack, {18.0, 36.0, 400.0, 76.0});
        constexpr std::array<const char*, 3> captions{"Block", "Auto margins", "Block"};
        for (std::size_t index = 0U; index < captions.size(); ++index) {
            std::shared_ptr<Button> item = make_control<Button>(
                StableId("gallery.css-layout.block.stack.item." + std::to_string(index)), captions[index]);
            (*item).set_visual_style(index == 1U ? ButtonVisualStyle::accent : ButtonVisualStyle::command);
            (*item).set_requested_bounds({0.0, 0.0, 0.0, 20.0});
            // 4 above and below: neighbours' margins collapse to 4.
            (*item).set_margin({0.0, 4.0, 0.0, 4.0});
            (*stack).add_child(item);
            if (index == 1U) {
                BlockFlowItemLayout centred{};
                centred.max_width = SizeValue{SizeKind::length_percentage, LengthPercentage{LogicalLength{200.0}, 0.0}};
                centred.auto_margin.left = true;
                centred.auto_margin.right = true;
                (*stack).set_item_layout(*item, centred);
            }
        }

        std::shared_ptr<BlockFlowPanel> line = make_control<BlockFlowPanel>(
            StableId("gallery.css-layout.block.line"));
        (*line).set_accessible_name("Centred inline line");
        (*line).set_text_align(TextAlignment::center);
        (*block_group).add_at(line, {436.0, 36.0, 476.0, 76.0});
        for (std::size_t index = 0U; index < 6U; ++index) {
            const bool tall = index == 2U;
            std::shared_ptr<Button> item = make_control<Button>(
                StableId("gallery.css-layout.block.line.item." + std::to_string(index)),
                tall ? std::string("Middle") : "Inline " + std::to_string(index + 1U));
            (*item).set_visual_style(tall ? ButtonVisualStyle::accent : ButtonVisualStyle::standard);
            (*item).set_margin({0.0, 0.0, 6.0, 0.0});
            (*line).add_child(item);
            BlockFlowItemLayout atomic{};
            atomic.level = BlockFlowLevel::inline_level;
            atomic.width = SizeValue{SizeKind::length_percentage, LengthPercentage{LogicalLength{90.0}, 0.0}};
            atomic.height = SizeValue{SizeKind::length_percentage,
                                      LengthPercentage{LogicalLength{tall ? 30.0 : 22.0}, 0.0}};
            if (tall) atomic.vertical_align = InlineVerticalAlignment::middle;
            (*line).set_item_layout(*item, atomic);
        }
    }

    static GridTrackSize fixed_track(double length) {
        GridTrackSize size{};
        size.minimum = TrackBreadth{TrackBreadthKind::length_percentage, LengthPercentage{LogicalLength{length}, 0.0},
                                    FractionFactor{}};
        size.maximum = size.minimum;
        return size;
    }

    static GridTrackSize fraction_track(double fraction) {
        GridTrackSize size{};
        size.maximum = TrackBreadth{TrackBreadthKind::fraction, LengthPercentage{}, FractionFactor{fraction}};
        return size;
    }
};

void add_css_layout_panels(const std::shared_ptr<Surface>& page,
                           const std::shared_ptr<PageContext>& context) {
    (*context).components.add(std::make_shared<CssLayoutPanelsPage>(page, context));
}

} // namespace gui_forms::gallery::pages
