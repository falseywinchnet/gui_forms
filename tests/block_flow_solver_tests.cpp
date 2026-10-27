// Stage L, phase L5: the block flow solver headless. Margin collapsing
// (siblings, negative margins, through a container's top, empty blocks and
// chains of them), horizontal margins, line breaking and text-align, the
// strut and vertical-align, right-to-left, intrinsic widths and the
// refusal. Expected values are Chromium's, from the calibration probes
// recorded in results/2026-10-25-l5-block/README.md.

#include "gui_forms/layout/block_flow_solver/block_flow_solver.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using namespace gui_forms;

void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

bool near(double left, double right) {
    return std::abs(left - right) <= 1.0e-9;
}

BlockFlowItemInput block(double height, double margin_top = 0.0, double margin_bottom = 0.0) {
    BlockFlowItemInput item{};
    item.level = BlockFlowLevel::block;
    item.width = 100.0;
    item.height = height;
    item.margin.top = margin_top;
    item.margin.bottom = margin_bottom;
    item.collapses_through = height == 0.0;
    return item;
}

BlockFlowItemInput inline_box(double width, double height,
                              InlineVerticalAlignment alignment = InlineVerticalAlignment::baseline) {
    BlockFlowItemInput item{};
    item.level = BlockFlowLevel::inline_level;
    item.width = width;
    item.height = height;
    item.vertical_align = alignment;
    return item;
}

BlockFlowContainerInput container(double width) {
    BlockFlowContainerInput input{};
    input.width = width;
    return input;
}

double top_of(const BlockFlowLayoutWorkspace& workspace, std::size_t index) {
    return workspace.items()[index].bounds.y;
}

void test_sibling_margins_collapse() {
    std::vector<BlockFlowItemInput> items{};
    items.push_back(block(20.0, 0.0, 10.0));
    items.push_back(block(20.0, 30.0));
    items.push_back(block(20.0, 8.0, -10.0));
    items.push_back(block(20.0, 20.0, -10.0));
    items.push_back(block(20.0, -5.0));
    BlockFlowLayoutWorkspace workspace{};
    require(solve_block_flow(container(784.0), items, workspace) == BlockFlowSolveOutcome::solved, "solved");
    require(near(top_of(workspace, 0U), 0.0), "the first block at the top");
    require(near(top_of(workspace, 1U), 50.0), "10 and 30 collapse to 30");
    require(near(top_of(workspace, 2U), 78.0), "0 and 8 collapse to 8");
    require(near(top_of(workspace, 3U), 108.0), "-10 and 20 collapse to 10");
    require(near(top_of(workspace, 4U), 118.0), "-10 and -5 collapse to -10");
    require(near(workspace.content_height(), 138.0), "the content ends at the last block");
    require(workspace.trailing_margin() == CollapsedMargin{}, "no trailing margin");
    require(!workspace.leading_open(), "a root never opens its leading margins");
}

void test_empty_blocks_collapse_through() {
    // Chromium: d (bottom at 0), e empty (20 / 5), f (8): f at 20, e at 20.
    std::vector<BlockFlowItemInput> items{};
    items.push_back(block(20.0));
    items.push_back(block(0.0, 20.0, 5.0));
    items.push_back(block(20.0, 8.0));
    BlockFlowLayoutWorkspace workspace{};
    require(solve_block_flow(container(784.0), items, workspace) == BlockFlowSolveOutcome::solved, "solved");
    require(near(top_of(workspace, 1U), 40.0), "the empty block sits after its own top margin");
    require(near(top_of(workspace, 2U), 40.0), "20, 5 and 8 collapse to 20");

    // A nested empty container whose empty child's margins join its own:
    // its top part is 4 with 7 and 9, so it sits 9 down.
    std::vector<BlockFlowItemInput> chain{};
    chain.push_back(block(20.0));
    BlockFlowItemInput nested = block(0.0, 4.0, 6.0);
    nested.leading_inner.adjoin(7.0);
    nested.leading_inner.adjoin(9.0);
    chain.push_back(nested);
    chain.push_back(block(20.0, 3.0));
    require(solve_block_flow(container(784.0), chain, workspace) == BlockFlowSolveOutcome::solved, "solved");
    require(near(top_of(workspace, 1U), 29.0), "the nested empty block takes its content's margins");
    require(near(top_of(workspace, 2U), 29.0), "and the next block collapses with all of them");
}

void test_shared_leading_and_trailing_margins() {
    std::vector<BlockFlowItemInput> items{};
    items.push_back(block(0.0, 4.0, 15.0));
    items.push_back(block(20.0, 25.0, 15.0));
    BlockFlowContainerInput shared = container(784.0);
    shared.shares_leading_margin = true;
    BlockFlowLayoutWorkspace workspace{};
    require(solve_block_flow(shared, items, workspace) == BlockFlowSolveOutcome::solved, "solved");
    require(near(top_of(workspace, 0U), 0.0) && near(top_of(workspace, 1U), 0.0),
            "margins before the first nonempty block leave the container");
    require(workspace.leading_margin() == CollapsedMargin{25.0, 0.0}, "the leading margins collapse to 25");
    require(workspace.trailing_margin() == CollapsedMargin{15.0, 0.0}, "the last block's margin trails");
    require(near(workspace.content_height(), 20.0), "the content ends at the block's border");

    std::vector<BlockFlowItemInput> empty{};
    empty.push_back(block(0.0, 4.0, 15.0));
    require(solve_block_flow(shared, empty, workspace) == BlockFlowSolveOutcome::solved, "solved");
    require(workspace.leading_open(), "only empty blocks leave the leading margins open");
    require(workspace.leading_margin() == CollapsedMargin{15.0, 0.0}, "all of them in the leading margin");
    require(workspace.trailing_margin() == CollapsedMargin{}, "and none trailing");

    std::vector<BlockFlowItemInput> line{};
    line.push_back(inline_box(10.0, 10.0));
    require(solve_block_flow(shared, line, workspace) == BlockFlowSolveOutcome::solved, "solved");
    require(!workspace.leading_open(), "a line box closes the leading margins");
}

void test_horizontal_margins() {
    std::vector<BlockFlowItemInput> items{};
    BlockFlowItemInput centred = block(10.0);
    centred.auto_margin_left = true;
    centred.auto_margin_right = true;
    items.push_back(centred);
    BlockFlowItemInput over = block(10.0);
    over.margin.left = 10.0;
    over.margin.right = 50.0;
    items.push_back(over);
    BlockFlowItemInput pushed = block(10.0);
    pushed.auto_margin_left = true;
    pushed.margin.right = 40.0;
    items.push_back(pushed);
    BlockFlowItemInput wide = block(10.0);
    wide.width = 2000.0;
    wide.auto_margin_left = true;
    wide.auto_margin_right = true;
    items.push_back(wide);
    BlockFlowLayoutWorkspace workspace{};
    require(solve_block_flow(container(784.0), items, workspace) == BlockFlowSolveOutcome::solved, "solved");
    require(near(workspace.items()[0].bounds.x, 342.0), "auto margins centre");
    require(near(workspace.items()[1].bounds.x, 10.0), "an over-constrained item keeps its left margin");
    require(near(workspace.items()[2].bounds.x, 644.0), "an auto left margin takes what is left");
    require(near(workspace.items()[3].bounds.x, 0.0), "auto margins are zero when nothing is left");

    BlockFlowContainerInput mirrored = container(784.0);
    mirrored.layout_direction = LayoutDirection::right_to_left;
    require(solve_block_flow(mirrored, items, workspace) == BlockFlowSolveOutcome::solved, "solved");
    require(near(workspace.items()[1].bounds.x, 634.0), "right to left keeps the right margin");
    require(near(workspace.items()[3].bounds.x, -1216.0), "and overflows to the left");
}

void test_line_breaking_and_text_align() {
    std::vector<BlockFlowItemInput> items{};
    for (int index = 0; index < 3; ++index) items.push_back(inline_box(40.0, 10.0));
    BlockFlowContainerInput input = container(100.0);
    BlockFlowLayoutWorkspace workspace{};
    require(solve_block_flow(input, items, workspace) == BlockFlowSolveOutcome::solved, "solved");
    require(workspace.lines().size() == 2U, "two lines");
    require(near(workspace.items()[1].bounds.x, 40.0) && near(workspace.items()[2].bounds.y, 10.0),
            "the third item wraps");
    require(near(workspace.content_height(), 20.0), "two lines of ten");

    input.text_align = TextAlignment::center;
    require(solve_block_flow(input, items, workspace) == BlockFlowSolveOutcome::solved, "solved");
    require(near(workspace.items()[0].bounds.x, 10.0) && near(workspace.items()[2].bounds.x, 30.0), "centred");
    input.text_align = TextAlignment::right;
    require(solve_block_flow(input, items, workspace) == BlockFlowSolveOutcome::solved, "solved");
    require(near(workspace.items()[0].bounds.x, 20.0) && near(workspace.items()[2].bounds.x, 60.0), "right");

    std::vector<BlockFlowItemInput> wide{};
    wide.push_back(inline_box(140.0, 10.0));
    input.text_align = TextAlignment::end;
    require(solve_block_flow(input, wide, workspace) == BlockFlowSolveOutcome::solved, "solved");
    require(near(workspace.items()[0].bounds.x, 0.0), "an overflowing line is start-aligned");

    input.text_align = TextAlignment::start;
    input.layout_direction = LayoutDirection::right_to_left;
    require(solve_block_flow(input, items, workspace) == BlockFlowSolveOutcome::solved, "solved");
    require(near(workspace.items()[0].bounds.x, 60.0) && near(workspace.items()[1].bounds.x, 20.0) &&
                near(workspace.items()[2].bounds.x, 60.0),
            "right to left starts at the right");

    // Three thirds of the line fit, whatever the rounding.
    std::vector<BlockFlowItemInput> thirds{};
    for (int index = 0; index < 3; ++index) thirds.push_back(inline_box(100.0 / 3.0, 10.0));
    input.layout_direction = LayoutDirection::left_to_right;
    require(solve_block_flow(input, thirds, workspace) == BlockFlowSolveOutcome::solved, "solved");
    require(workspace.lines().size() == 1U, "thirds share one line");
}

void test_blocks_between_inline_runs() {
    // Chromium: an inline box, a block with a 7 top margin, an inline box
    // with a 5 top margin.
    std::vector<BlockFlowItemInput> items{};
    items.push_back(inline_box(40.0, 10.0));
    items.push_back(block(10.0, 7.0));
    BlockFlowItemInput last = inline_box(40.0, 10.0);
    last.margin.top = 5.0;
    items.push_back(last);
    BlockFlowLayoutWorkspace workspace{};
    require(solve_block_flow(container(784.0), items, workspace) == BlockFlowSolveOutcome::solved, "solved");
    require(near(top_of(workspace, 1U), 17.0), "the block after the first line");
    require(near(top_of(workspace, 2U), 32.0), "the second line after the block");
    require(near(workspace.content_height(), 42.0), "the second line is its margin box tall");
    require(workspace.items()[1].line == no_block_flow_line, "a block has no line");
}

void test_strut_and_vertical_align() {
    BlockFlowLayoutWorkspace workspace{};
    // A line height of 20 at a zero font size: 10 above and below.
    BlockFlowContainerInput leaded = container(784.0);
    leaded.strut = LineStrut{10.0, 10.0, 0.0};
    std::vector<BlockFlowItemInput> small{};
    small.push_back(inline_box(10.0, 10.0));
    require(solve_block_flow(leaded, small, workspace) == BlockFlowSolveOutcome::solved, "solved");
    require(near(workspace.content_height(), 20.0) && near(top_of(workspace, 0U), 0.0), "the strut sets the line");
    small[0].height = 30.0;
    require(solve_block_flow(leaded, small, workspace) == BlockFlowSolveOutcome::solved, "solved");
    require(near(workspace.content_height(), 40.0), "the strut's descent stays under a tall box");

    BlockFlowContainerInput zero = container(784.0);
    std::vector<BlockFlowItemInput> middle{};
    middle.push_back(inline_box(10.0, 10.0, InlineVerticalAlignment::middle));
    middle.push_back(inline_box(10.0, 30.0));
    require(solve_block_flow(zero, middle, workspace) == BlockFlowSolveOutcome::solved, "solved");
    require(near(workspace.content_height(), 35.0) && near(top_of(workspace, 0U), 25.0), "middle on the baseline");

    // The tallest top- or bottom-aligned box decides (Chromium's probes).
    struct Case final {
        double baseline_height;
        double first_height;
        InlineVerticalAlignment first;
        double second_height;
        InlineVerticalAlignment second;
        double expected_baseline_top;
    };
    const InlineVerticalAlignment top = InlineVerticalAlignment::top;
    const InlineVerticalAlignment bottom = InlineVerticalAlignment::bottom;
    const Case cases[] = {
        {20.0, 40.0, bottom, 30.0, top, 20.0}, {20.0, 40.0, top, 30.0, bottom, 0.0},
        {20.0, 30.0, bottom, 40.0, top, 0.0},  {20.0, 30.0, top, 40.0, bottom, 20.0},
    };
    for (const Case& entry : cases) {
        std::vector<BlockFlowItemInput> line{};
        line.push_back(inline_box(10.0, entry.baseline_height));
        line.push_back(inline_box(10.0, entry.first_height, entry.first));
        line.push_back(inline_box(10.0, entry.second_height, entry.second));
        require(solve_block_flow(zero, line, workspace) == BlockFlowSolveOutcome::solved, "solved");
        require(near(workspace.content_height(), 40.0), "the line is as tall as the tallest box");
        require(near(top_of(workspace, 0U), entry.expected_baseline_top), "the tallest aligned box decides");
    }

    // Margins are part of the box on the line.
    std::vector<BlockFlowItemInput> margins{};
    BlockFlowItemInput first = inline_box(10.0, 10.0);
    first.margin = BoxSides<double>{3.0, 2.0, 5.0, 4.0};
    margins.push_back(first);
    BlockFlowItemInput second = inline_box(10.0, 10.0, InlineVerticalAlignment::middle);
    second.margin.top = 6.0;
    margins.push_back(second);
    require(solve_block_flow(zero, margins, workspace) == BlockFlowSolveOutcome::solved, "solved");
    require(near(workspace.content_height(), 26.0), "margin boxes on the line");
    require(near(workspace.items()[0].bounds.x, 4.0) && near(top_of(workspace, 0U), 3.0), "first item");
    require(near(workspace.items()[1].bounds.x, 16.0) && near(top_of(workspace, 1U), 16.0), "second item");
}

void test_intrinsic_widths() {
    std::vector<BlockFlowItemInput> items{};
    items.push_back(inline_box(40.0, 10.0));
    items.push_back(inline_box(30.0, 10.0));
    BlockFlowItemInput wide = block(10.0);
    wide.width = 50.0;
    wide.margin.left = 5.0;
    items.push_back(wide);
    items.push_back(inline_box(20.0, 10.0));
    require(near(block_flow_intrinsic_width(items, BlockFlowIntrinsicKind::max_content), 70.0),
            "max-content: the longest run");
    require(near(block_flow_intrinsic_width(items, BlockFlowIntrinsicKind::min_content), 55.0),
            "min-content: the widest item");
}

void test_refusal() {
    std::vector<BlockFlowItemInput> items(maximum_block_flow_items + 1U, BlockFlowItemInput{});
    BlockFlowLayoutWorkspace workspace{};
    require(solve_block_flow(container(100.0), items, workspace) == BlockFlowSolveOutcome::refused_too_many_items,
            "too many items are refused");
    require(workspace.items().empty() && workspace.lines().empty(), "and leave the workspace empty");
}

} // namespace

int main() {
    try {
        test_sibling_margins_collapse();
        test_empty_blocks_collapse_through();
        test_shared_leading_and_trailing_margins();
        test_horizontal_margins();
        test_line_breaking_and_text_align();
        test_blocks_between_inline_runs();
        test_strut_and_vertical_align();
        test_intrinsic_widths();
        test_refusal();
        std::cout << "block-flow-solver-tests: pass\n";
        return EXIT_SUCCESS;
    } catch (const std::exception& error) {
        std::cerr << "block-flow-solver-tests: fail: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
