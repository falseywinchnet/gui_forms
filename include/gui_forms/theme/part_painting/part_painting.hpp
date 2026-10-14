#pragma once
#include "gui_forms/anatomy/types/anatomy_types.hpp"
#include "gui_forms/theme/types/theme_types.hpp"
#include "gui_forms/types/paint_types/paint_types.hpp"

#include <cstdint>
#include <optional>
#include <string_view>

namespace gui_forms {

class Control;
class Painter;

// The by-role paint path (owner ruling, 2026-10-14: the atom vocabulary is the
// canonical source). A control describes each part it paints with words from
// the vocabulary (its PartRole, the AnatomyStates it is in) plus the content
// it shows, and paint_part draws it. A control never draws a part itself, so
// when the engine (T2) and the default theme (T4) arrive, they replace this
// path's body and no control changes.
//
// Until then the path draws from today's theme records: `family` names the
// ControlVisualRole whose recipe the part uses, and the part's states select
// the recipe's surface state, focus and default cues.

// The drawn mark of a glyph or indicator part. Marks are engine vocabulary:
// a control names the mark, never strokes it.
enum class PartMark : std::uint8_t {
    none,
    check,
    indeterminate,
    radio_dot,
    toggle_knob,
    arrow_up,
    arrow_down,
    arrow_left,
    arrow_right,
    chevron_down,
    chevron_right,
    close,
    grip_dots,
    count,
};

enum class PartTextAlignment : std::uint8_t { start, center, end };

// One part to paint, in the control's local coordinates. `text`, `image` and
// `mark` are read only by roles that show them (label, caption and content;
// icon; glyph and indicator). `text` is borrowed for the call. `font` is an
// effective font (Control::effective_font). `corner_radius`, when set, gives
// a surface part its shape; the theme (T4) will own shapes, so a control sets
// it only where today's recipe has no shape for the part.
struct PartPaint final {
    PartRole role{PartRole::region};
    Rect bounds{};
    AnatomyStateSet states{};
    ControlVisualRole family{ControlVisualRole::panel};
    PartMark mark{PartMark::none};
    std::string_view text{};
    FontSpec font{};
    PartTextAlignment alignment{PartTextAlignment::start};
    ImageId image{};
    std::optional<double> corner_radius{};
};

// The theme context a part's states select: hot, pressed or captured,
// selection (selected, checked, current, open, active_tool), the visible focus
// cue and preselect. The control supplies what the part does not carry: its
// enabled, invalid and pending status, window activity and high contrast. A
// part that is itself disabled or unavailable is drawn disabled.
[[nodiscard]] ControlVisualContext part_visual_context(
    const Control& control, AnatomyStateSet states) noexcept;

// Paints one part of `control` by its role (see PartPaint). Roles that draw
// nothing of their own (region, shadow, edge, perimeter, sheen) are no-ops:
// the material of the surface they belong to draws them. An empty or
// non-finite `bounds` paints nothing.
void paint_part(Painter& painter, const Control& control, const PartPaint& part);

// paint_part's drawing, with the recipe and context already resolved. For
// helpers that resolve a recipe themselves (the existing controls' focus and
// default cues), so that every part is drawn by one implementation.
void paint_part_from_recipe(Painter& painter, const PartPaint& part,
                            const ControlVisualRecipe& recipe, ControlVisualContext context);

} // namespace gui_forms
