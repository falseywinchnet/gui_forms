#pragma once

#include "gui_forms/basic_controls.hpp"

namespace gui_forms {

// The overlay of one plain tooltip: the `tool_tip` family's face (owner
// decision RB-38) casting the theme's overlay shadow (RB-37), with a
// wrapping label child. Its bounds are the face; the shadow falls outside
// them, within its visual outsets.
class ToolTipBubble final : public Panel {
public:
    static constexpr bool initialize_tree_after_construction = true;
    ToolTipBubble(StableId stable_id, std::string text);

    void initialize_control_tree();
    void set_content_size(Size size);
    // Gives the label the `tool_tip` family's text colour in the bubble's
    // current theme and presentation. Call once the bubble is attached to
    // its window; a bubble lives for one showing, so it is not repeated
    // when the theme changes while it shows.
    void apply_family_text_colour();
    void on_paint(Painter& painter, Rect local_damage) override;
    [[nodiscard]] Insets visual_outsets() const noexcept override;
    [[nodiscard]] bool hit_test_local(Point point) const override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;

private:
    std::string text_;
    std::shared_ptr<Label> label_;
};

} // namespace gui_forms
