#pragma once

// Positioned children of the CSS containers (stage L, phase L5; PLAN
// sections 2.5 and 3.3, decision L-Q6), shared by FlexPanel,
// GridPanel and BlockPanel. Private to the controls library.

#include "gui_forms/control/control/control.hpp"
#include "gui_forms/layout/positioning/positioning.hpp"
#include "gui_forms/layout/style/layout_style.hpp"

#include <memory>
#include <vector>

namespace gui_forms::out_of_flow {

// Whether a record's positioning can be kept: its insets are finite.
[[nodiscard]] bool admitted_positioning(const ItemPositioning& positioning) noexcept;

// The `position` a control's parent gives it: its item record's when the
// parent is a FlexPanel, GridPanel or BlockPanel, static
// otherwise.
[[nodiscard]] PositionKind position_in_parent(const Control& control) noexcept;

// The containing block of `container`'s absolutely positioned children:
// the padding box of the nearest positioned box from the container up (the
// container itself, a positioned ancestor, or the top of the tree, the
// window's root control, whose box is the client area), held so that it
// outlives the placement's measure callbacks. `box` is in the
// container's own coordinates, those its children's slots use; a
// scrolling box's padding box is at its scroll origin. Reads the committed
// arranged bounds, which an arrange pass has already set above the
// container.
struct ContainingBlock final {
    std::shared_ptr<const Control> ancestor{};
    Rect box{};
    // The ancestor's direction, which decides an over-constrained box.
    LayoutDirection direction{LayoutDirection::left_to_right};
};

[[nodiscard]] ContainingBlock resolve_containing_block(const Control& container);

// The controls whose arrangement can move `block` relative to `container`:
// from the container's parent up to the ancestor, both included; empty
// when the container is its own containing block. Fills `ids` (emptied
// first, its capacity reused).
void fill_dependency_path(const Control& container, const ContainingBlock& block, std::vector<RuntimeId>& ids);

// An absolutely positioned child's record, as its container keeps it.
struct AbsoluteItem final {
    ItemPositioning positioning{};
    SizeValue width{};
    SizeValue height{};
    SizeValue min_width{};
    SizeValue min_height{};
    SizeValue max_width{SizeKind::none, {}};
    SizeValue max_height{SizeKind::none, {}};
    BoxSides<bool> auto_margin{};
};

// An item record's absolute-placement fields; FlexItemLayout,
// GridItemLayout and BlockFlowItemLayout name them alike.
template <typename ItemLayout> [[nodiscard]] AbsoluteItem absolute_item(const ItemLayout& layout) noexcept {
    AbsoluteItem item{};
    item.positioning = layout.positioning;
    item.width = layout.width;
    item.height = layout.height;
    item.min_width = layout.min_width;
    item.min_height = layout.min_height;
    item.max_width = layout.max_width;
    item.max_height = layout.max_height;
    item.auto_margin = layout.auto_margin;
    return item;
}

// The container's side of a placement: its padding, border-box size,
// scroll offset and direction, for the static position (the content box's
// start corner, its end corner's x for right to left; CSS takes the in-flow
// position, a recorded limit).
struct AbsoluteContainer final {
    Insets padding{};
    Size size{};
    Point scroll{};
    LayoutDirection direction{LayoutDirection::left_to_right};
};

// The child's border box (CSS 2.1 10.3.7, then 10.6.4 at the used width) in
// the container's coordinates. The child is measured, through
// measure_for_layout, only on an axis whose size comes from its content;
// a measure callback may change the tree, so the caller checks that the
// container and the child are still current before it assigns the slot.
[[nodiscard]] Rect place_absolute_child(Control& child, const AbsoluteItem& item, const ContainingBlock& block,
                                        const AbsoluteContainer& container);

} // namespace gui_forms::out_of_flow
