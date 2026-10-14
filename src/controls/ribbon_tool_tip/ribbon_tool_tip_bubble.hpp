#pragma once

#include "gui_forms/control.hpp"

#include <string>
#include <vector>

namespace gui_forms {

// What one enhanced tooltip shows, already resolved from its command.
// `disabled_reason` is empty unless the target is disabled, and `help_line`
// empty unless the mapping has a help topic.
struct RibbonToolTipBubbleContent final {
    std::string title;
    std::string shortcut;
    std::string description;
    std::string disabled_reason;
    std::string help_line;
    ImageId image{};
};

// One wrapped line of the description or the disabled reason.
struct RibbonToolTipLine final {
    Rect bounds{};
    std::string text;
};

// Where every part of the bubble sits, in the bubble's local logical units,
// from one pass: the overlay is sized from it and the bubble paints from it.
// The face leaves `shadow_offset` units right and below for the shadow.
struct RibbonToolTipLayout final {
    Size size{};
    Rect face{};
    Rect shadow{};
    Rect title{};
    Rect shortcut{};
    Rect image{};
    std::vector<RibbonToolTipLine> description;
    std::vector<RibbonToolTipLine> disabled_reason;
    Rect help_rule{};
    Rect help_line{};
    FontSpec title_font{};
    FontSpec body_font{};
};

// Lays out `content` with `measurer`'s effective fonts and text provider,
// wrapping the description and reason within `maximum_width` logical units.
[[nodiscard]] RibbonToolTipLayout layout_ribbon_tool_tip(const Control& measurer,
                                                         const RibbonToolTipBubbleContent& content,
                                                         double maximum_width);

// The overlay of one enhanced tooltip: input-transparent, unfocusable, and
// painted by role from its layout.
class RibbonToolTipBubble final : public Control {
public:
    RibbonToolTipBubble(StableId stable_id, RibbonToolTipBubbleContent content, RibbonToolTipLayout layout);

    void on_paint(Painter& painter, Rect local_damage) override;
    [[nodiscard]] bool hit_test_local(Point point) const override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;
    [[nodiscard]] const RibbonToolTipLayout& layout() const noexcept { return layout_; }

private:
    RibbonToolTipBubbleContent content_;
    RibbonToolTipLayout layout_;
};

} // namespace gui_forms
