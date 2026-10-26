#pragma once

#include "gui_forms/control/control/control.hpp"
#include "gui_forms/event.hpp"

#include <optional>
#include <span>

namespace gui_forms {

// A move of a Splitter by the user. `pointer` is the pointer in the parent's
// coordinates. In splitter_moving, `split_position` is the target's proposed
// extent, which a handler may change (it is clamped again to the limits); in
// splitter_moved it is the extent the target took.
struct SplitterMoveEvent final {
    Point pointer{};
    double split_position{};
};

// A bar docked between two docked siblings that resizes the one before it
// (Windows Forms' Splitter, kept as a migration facade over the dock layout;
// owner decision 2 of 2026-10-25). New code uses SplitContainer.
//
// The splitter docks at an edge (left by default) and its target is the
// visible sibling docked at the same edge whose far side touches the
// splitter's near side, as in Windows Forms: for a left splitter, the
// left-docked sibling whose right edge is the splitter's left edge. Dragging
// the bar sets the target's requested width (left and right) or height (top
// and bottom), live, between min_size and the extent that leaves min_extra
// for the rest of the parent's dock area. A splitter docked none or fill, or
// one with no target, does nothing. The bar is painted by role (a face
// plate) and takes no focus, as in Windows Forms.
class Splitter final : public Control {
public:
    explicit Splitter(StableId stable_id);

    // The least extent the target keeps (Windows Forms' MinSize, default 25).
    // Throws std::invalid_argument for a negative or non-finite value.
    [[nodiscard]] double min_size() const noexcept { return min_size_; }
    void set_min_size(double extent);
    // The least extent left for the rest of the dock area (Windows Forms'
    // MinExtra, default 25). Refused as min_size is.
    [[nodiscard]] double min_extra() const noexcept { return min_extra_; }
    void set_min_extra(double extent);
    // The target's extent along the splitter's axis, or none without a
    // target (Windows Forms' SplitPosition, -1 there).
    [[nodiscard]] std::optional<double> split_position() const;
    // Resizes the target to `extent`, clamped to the limits; does nothing
    // without a target. Throws std::invalid_argument for a non-finite value.
    void set_split_position(double extent);
    // The sibling the splitter resizes, or null.
    [[nodiscard]] Control::Ptr target() const;

    [[nodiscard]] Event<SplitterMoveEvent&>& splitter_moving() noexcept { return splitter_moving_; }
    [[nodiscard]] Event<const SplitterMoveEvent&>& splitter_moved() noexcept { return splitter_moved_; }

    [[nodiscard]] Size measure(Size available) override;
    void on_pointer(PointerEvent& event) override;
    void on_paint(Painter& painter, Rect local_damage) override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;
    bool on_semantic_action(SemanticAction action, std::string_view value) override;
    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;
    [[nodiscard]] AnatomyStateSet anatomy_states() const override;

    // Stage T1 anatomy, as dot members.
    struct Parts final {
        PartGeometry region;
        PartGeometry face;
    };
    struct States final {
        bool normal{};
        bool hot{};
        bool pressed{};
        bool captured{};
        bool enabled{};
        bool disabled{};
    };
    [[nodiscard]] Parts parts() const;
    [[nodiscard]] States states() const;

protected:
    void append_interactions(std::vector<InteractionDeclaration>& interactions) const override;
    void publish_part_geometry(std::span<PartGeometry> geometry) const override;

private:
    // The limits a split position is clamped to, for the current target.
    struct SplitLimits final {
        double minimum{};
        double maximum{};
    };

    [[nodiscard]] bool horizontal_axis() const noexcept;
    [[nodiscard]] double axis_of(Point point) const noexcept;
    [[nodiscard]] double extent_of(const Control& control) const noexcept;
    [[nodiscard]] SplitLimits limits(const Control& target) const;
    [[nodiscard]] double clamped(double extent, const Control& target) const;
    void apply(Control& target, double extent);
    void end_drag();

    double min_size_{25.0};
    double min_extra_{25.0};
    bool hot_{};
    bool dragging_{};
    // The drag's start: the pointer along the axis, in window coordinates,
    // and the target's extent then.
    double drag_start_axis_{};
    double drag_start_extent_{};
    Control::WeakPtr drag_target_{};
    Event<SplitterMoveEvent&> splitter_moving_{};
    Event<const SplitterMoveEvent&> splitter_moved_{};
};

} // namespace gui_forms
