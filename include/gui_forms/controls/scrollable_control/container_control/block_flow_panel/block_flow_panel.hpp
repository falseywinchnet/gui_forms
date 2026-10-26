#pragma once

#include "gui_forms/controls/scrollable_control/container_control/container_control.hpp"
#include "gui_forms/layout/block_flow_solver/block_flow_solver.hpp"
#include "gui_forms/layout/style/layout_style.hpp"

#include <cstdint>
#include <span>
#include <unordered_map>
#include <vector>

namespace gui_forms {

// One child's block flow properties (CSS's names; PLAN section 2.4). The
// panel keeps one per child, keyed by its runtime id and dropped when the
// child leaves. Margins are the child's own `margin`; `auto_margin` marks
// the sides that are `auto` instead, which matters only for a block item's
// left and right (CSS 2.1 10.3.3; any other `auto` margin is zero). The
// child's minimum_size and maximum_size bound its used size as its own min
// and max sizes do.
struct BlockFlowItemLayout final {
    // `display: block` (the default) or an atomic inline box, CSS's
    // `inline-block`.
    BlockFlowLevel level{BlockFlowLevel::block};
    // CSS width: `auto` fills the panel less the margins for a block item
    // and shrinks to fit the content for an inline one; a length or a
    // percentage of the panel's content width is definite.
    SizeValue width{};
    // CSS height: `auto` is the content's height at the used width; a
    // length, or a percentage of a definite panel height (otherwise `auto`).
    SizeValue height{};
    SizeValue min_width{};
    SizeValue min_height{};
    SizeValue max_width{SizeKind::none, {}};
    SizeValue max_height{SizeKind::none, {}};
    BoxSides<bool> auto_margin{};
    // An inline item's alignment on its line.
    InlineVerticalAlignment vertical_align{InlineVerticalAlignment::baseline};

    friend bool operator==(const BlockFlowItemLayout& left, const BlockFlowItemLayout& right) noexcept {
        return left.level == right.level && left.width == right.width && left.height == right.height &&
               left.min_width == right.min_width && left.min_height == right.min_height &&
               left.max_width == right.max_width && left.max_height == right.max_height &&
               left.auto_margin == right.auto_margin && left.vertical_align == right.vertical_align;
    }
};

// The margins a nested BlockFlowPanel's content shares with the block flow
// that lays it out (CSS 2.1 8.3.1), its own margins excluded.
struct BlockFlowSharedMargins final {
    // Collapsing through its top edge (empty with top padding).
    CollapsedMargin leading{};
    // Collapsing through its bottom edge (empty with bottom padding, or
    // when its own minimum or maximum changed its content's height).
    CollapsedMargin trailing{};
    // No line box or nonempty block closed the leading margins: at a zero
    // height the panel's own top and bottom margins adjoin.
    bool open{};

    friend bool operator==(const BlockFlowSharedMargins& left, const BlockFlowSharedMargins& right) noexcept {
        return left.leading == right.leading && left.trailing == right.trailing && left.open == right.open;
    }
};

// A container that lays its children out as a CSS block formatting context
// (stage L, phase L5): block items stacked with collapsing margins, and runs
// of inline items broken into line boxes, through the core block flow
// solver. A BlockFlowPanel laid out as a block item of another, without
// auto_scroll, is not a formatting-context root: its first and last
// children's margins collapse with its own where no padding separates them,
// as nested CSS blocks do. Every other BlockFlowPanel is a root.
class BlockFlowPanel final : public ContainerControl {
public:
    explicit BlockFlowPanel(StableId stable_id);

    // The line boxes' inline alignment; `start` by default. `justify` is not
    // admitted.
    [[nodiscard]] TextAlignment text_align() const noexcept { return text_align_; }
    void set_text_align(TextAlignment alignment);
    // The strut every line box starts with, from the font and line height
    // the panel's inline content is set in (a form importer resolves a
    // source's). Zero by default: a point on the baseline, so a line is as
    // tall as its items. Its values must be finite and nonnegative.
    [[nodiscard]] LineStrut line_strut() const noexcept { return line_strut_; }
    void set_line_strut(LineStrut strut);

    // A live direct child's record; throws std::invalid_argument for any
    // other child or for a value the panel does not admit (content keywords,
    // a negative or non-finite length, `none` outside a maximum, `auto` as a
    // maximum).
    void set_item_layout(const Control& child, const BlockFlowItemLayout& layout);
    [[nodiscard]] BlockFlowItemLayout item_layout(const Control& child) const;

    // The last arrange's resolution: one record per laid-out child, in
    // laid_out_children's order, and the line boxes. Both borrow this
    // panel's storage until its next arrange.
    [[nodiscard]] const BlockFlowLayoutWorkspace& layout_snapshot() const& noexcept { return arranged_; }
    const BlockFlowLayoutWorkspace& layout_snapshot() const&& = delete;
    [[nodiscard]] std::span<const RuntimeId> laid_out_children() const& noexcept { return arranged_ids_; }
    std::span<const RuntimeId> laid_out_children() const&& = delete;
    [[nodiscard]] BlockFlowSolveOutcome layout_outcome() const noexcept { return arranged_outcome_; }

    // The margins this panel's content shares with its parent block flow
    // when laid out at the border-box `width`, for that parent to collapse
    // with its own; all empty when it shares none. It answers from the
    // panel's last measure at that width while nothing changed since, and
    // otherwise lays the content out again (children's measures answer
    // from their caches).
    [[nodiscard]] BlockFlowSharedMargins shared_margins(double width);

    [[nodiscard]] bool auto_size() const noexcept override { return Control::auto_size(); }
    void set_auto_size(bool auto_size) override;
    [[nodiscard]] Size measure(Size available) override;
    void arrange(Rect final_bounds) override;
    [[nodiscard]] bool measures_children_in_arrange() const noexcept override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;
    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;

protected:
    [[nodiscard]] Size measure_intrinsic(const LayoutConstraint& constraint) override;

private:
    struct ItemRecord final {
        BlockFlowItemLayout layout{};
        std::uint64_t seen_pass{};
    };
    using ItemMap = std::unordered_map<std::uint64_t, ItemRecord>;

    struct SolveRequest final {
        // The content box's width.
        double width{};
        // The content box's height, read by percentage heights when
        // height_definite.
        double height{};
        bool height_definite{};
        bool assign{};
    };

    struct SolveResult final {
        // The content's height before the panel's own minimum and maximum:
        // with the trailing margins when it does not share them.
        double content_height{};
        BlockFlowSharedMargins shared{};
        BlockFlowSolveOutcome outcome{BlockFlowSolveOutcome::solved};
    };

    // One answer of shared_margins, valid while the panel's measure
    // generation is unchanged.
    struct SharedMarginsEntry final {
        double width{};
        std::uint64_t generation{};
        BlockFlowSharedMargins margins{};
        bool valid{};
    };

    SolveResult solve(const SolveRequest& request, BlockFlowLayoutWorkspace& workspace);
    double intrinsic_content_width(BlockFlowIntrinsicKind kind);
    void gather_children();
    void resolve_item_inputs(const SolveRequest& request);
    void publish_slots(const BlockFlowLayoutWorkspace& workspace);
    [[nodiscard]] bool shares_parent_flow() const noexcept;
    [[nodiscard]] bool percentage_heights_definite() const noexcept;
    [[nodiscard]] const BlockFlowItemLayout& record_for(const Control& child) const noexcept;
    void forget_departed_children();

    ItemMap items_{};
    TextAlignment text_align_{TextAlignment::start};
    LineStrut line_strut_{};

    std::uint64_t pass_{};
    std::vector<Control::Ptr> children_{};
    std::vector<BlockFlowItemInput> inputs_{};
    BlockFlowLayoutWorkspace workspace_{};
    BlockFlowLayoutWorkspace arranged_{};
    std::vector<RuntimeId> arranged_ids_{};
    BlockFlowSolveOutcome arranged_outcome_{BlockFlowSolveOutcome::solved};
    SharedMarginsEntry shared_entry_{};
};

} // namespace gui_forms
