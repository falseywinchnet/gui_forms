#include "gui_forms/theme/part_painting/part_painting.hpp"

#include "gui_forms/control/control/control.hpp"
#include "gui_forms/surface_material/painting/surface_material_painting.hpp"
#include "gui_forms/theme/theme/theme.hpp"
#include "gui_forms/types/painter/painter.hpp"

#include <algorithm>
#include <cmath>

namespace gui_forms {
namespace {

// Bounds whose arithmetic is meaningful: finite and not empty.
bool paintable(Rect bounds) noexcept {
    const bool result = bounds.finite() && !bounds.empty();
    return result;
}

// Roles painted as a surface of the recipe's material: the ones an accent
// tints.
bool surface_role(PartRole role) noexcept {
    switch (role) {
    case PartRole::face:
    case PartRole::well:
    case PartRole::track:
    case PartRole::fill:
    case PartRole::thumb:
    case PartRole::actuator:
    case PartRole::chip:
    case PartRole::cap:
    case PartRole::activity:
    case PartRole::selection:
        return true;
    default:
        return false;
    }
}

bool any_state(AnatomyStateSet states, AnatomyState first, AnatomyState second) noexcept {
    const bool result = states.contains(first) || states.contains(second);
    return result;
}

// The surface of a part whose role is a plate, well or mark ground.
void paint_surface(Painter& painter, const PartPaint& part, const ControlVisualRecipe& recipe) {
    if (!part.corner_radius) {
        paint_surface_material(painter, part.bounds, recipe.material);
        return;
    }
    SurfaceMaterial shaped = recipe.material;
    shaped.corner_radius = std::max(0.0, *part.corner_radius);
    paint_surface_material(painter, part.bounds, shaped);
}

// A divider or seam: one line across the long axis of `bounds`, centred.
void paint_rule(Painter& painter, Rect bounds, const ControlVisualRecipe& recipe) {
    const Color color = recipe.material.border ? (*recipe.material.border).color : recipe.muted_text;
    if (bounds.width >= bounds.height) {
        const double y = bounds.y + bounds.height * 0.5;
        painter.draw_line({bounds.x, y}, {bounds.x + bounds.width, y}, color, 1.0);
    } else {
        const double x = bounds.x + bounds.width * 0.5;
        painter.draw_line({x, bounds.y}, {x, bounds.y + bounds.height}, color, 1.0);
    }
}

// The preselect (default) ring, inside the part's bounds.
void paint_preselect(Painter& painter, Rect bounds, const ControlVisualRecipe& recipe) {
    const double width = recipe.default_width;
    if (width <= 0.0 || bounds.width <= width * 2.0 || bounds.height <= width * 2.0) return;
    const double inset = width * 0.5;
    const Rect ring{bounds.x + inset, bounds.y + inset,
                    bounds.width - inset * 2.0, bounds.height - inset * 2.0};
    painter.stroke_rounded_rect(ring, std::max(0.0, recipe.material.corner_radius - inset),
                                recipe.default_ring, width);
}

// The focus ring: the recipe's authored outline outside the bounds, or the
// house ring inset inside them.
void paint_focus_ring(Painter& painter, Rect bounds, const ControlVisualRecipe& recipe) {
    if (recipe.focus_width <= 0.0 || bounds.width <= 8.0 || bounds.height <= 8.0) return;
    if (recipe.authored_focus_outline) {
        const double outset = recipe.focus_offset + recipe.focus_width * 0.5;
        const Rect outline{bounds.x - outset, bounds.y - outset,
                           bounds.width + outset * 2.0, bounds.height + outset * 2.0};
        if (outline.empty()) return;
        painter.stroke_rounded_rect(outline, std::max(0.0, recipe.material.corner_radius + outset),
                                    recipe.focus_ring, recipe.focus_width);
        return;
    }
    const double inset = std::max(3.0, recipe.default_width + 1.0);
    const Rect ring{bounds.x + inset, bounds.y + inset,
                    bounds.width - inset * 2.0, bounds.height - inset * 2.0};
    if (ring.empty()) return;
    painter.stroke_rounded_rect(ring, std::max(0.0, recipe.material.corner_radius - inset),
                                recipe.focus_ring, recipe.focus_width);
}

// A solid triangle pointing `direction`, filled one device-independent row
// at a time so every host rasterizes the same spans.
void fill_arrow(Painter& painter, Rect bounds, PartMark direction, Color color) {
    const double side = std::min(bounds.width, bounds.height);
    const double half = std::floor(side * 0.5);
    if (half < 1.0) return;
    const double centre_x = std::floor(bounds.x + bounds.width * 0.5);
    const double centre_y = std::floor(bounds.y + bounds.height * 0.5);
    // `half` rows (or columns), each one unit longer than the last from the tip.
    const int rows = static_cast<int>(half);
    for (int row = 0; row < rows; ++row) {
        const double extent = static_cast<double>(row);
        const double offset = static_cast<double>(row) - std::floor(half * 0.5);
        if (direction == PartMark::arrow_down) {
            const double y = centre_y - offset + 0.5;
            painter.fill_rect({centre_x - extent, y - 0.5, extent * 2.0 + 1.0, 1.0}, color);
        } else if (direction == PartMark::arrow_up) {
            const double y = centre_y + offset - 0.5;
            painter.fill_rect({centre_x - extent, y - 0.5, extent * 2.0 + 1.0, 1.0}, color);
        } else if (direction == PartMark::arrow_right) {
            const double x = centre_x - offset + 0.5;
            painter.fill_rect({x - 0.5, centre_y - extent, 1.0, extent * 2.0 + 1.0}, color);
        } else {
            const double x = centre_x + offset - 0.5;
            painter.fill_rect({x - 0.5, centre_y - extent, 1.0, extent * 2.0 + 1.0}, color);
        }
    }
}

// The largest square that fits `box`, centred, on whole units.
Rect square_in(Rect box) noexcept {
    const double side = std::floor(std::min(box.width, box.height));
    const Rect square{std::floor(box.x + (box.width - side) * 0.5),
                      std::floor(box.y + (box.height - side) * 0.5), side, side};
    return square;
}

// A ribbon group's dialog launcher: the top-left corner of a small window
// and an arrow leaving it towards the bottom right, in one-unit strokes.
void paint_dialog_launcher(Painter& painter, Rect box, Color ink) {
    const Rect square = square_in(box);
    const double side = square.width;
    if (side < 4.0) return;
    const double corner = side * 0.6;
    const Point origin{square.x + 0.5, square.y + 0.5};
    painter.draw_line(origin, {origin.x + corner, origin.y}, ink, 1.0);
    painter.draw_line(origin, {origin.x, origin.y + corner}, ink, 1.0);
    const Point tip{square.x + side - 0.5, square.y + side - 0.5};
    const double head = side * 0.4;
    painter.draw_line({origin.x + side * 0.3, origin.y + side * 0.3}, tip, ink, 1.0);
    painter.draw_line({tip.x - head, tip.y}, tip, ink, 1.0);
    painter.draw_line({tip.x, tip.y - head}, tip, ink, 1.0);
}

// A gallery's "more" button: a bar across the top third and a solid arrow
// pointing down beneath it.
void paint_more(Painter& painter, Rect box, Color ink) {
    const Rect square = square_in(box);
    const double side = square.width;
    if (side < 4.0) return;
    const double bar = std::max(1.0, std::floor(side / 8.0));
    const double bar_top = std::floor(side * 0.2);
    painter.fill_rect({square.x, square.y + bar_top, side, bar}, ink);
    const double top = bar_top + bar + 1.0;
    fill_arrow(painter, {square.x, square.y + top, side, side - top}, PartMark::arrow_down, ink);
}

// Overflow: a double chevron pointing to where the hidden items went, the
// two chevrons a third of the square apart.
void paint_overflow(Painter& painter, Rect box, Color ink) {
    const Rect square = square_in(box);
    const double side = square.width;
    if (side < 4.0) return;
    const double half = side * 0.25;
    const double middle = square.y + side * 0.5;
    const double gap = side / 3.0;
    const double first_tip = square.x + side * 0.5 - gap * 0.5 + half * 0.5;
    for (int chevron = 0; chevron < 2; ++chevron) {
        const double tip = first_tip + gap * static_cast<double>(chevron);
        painter.draw_line({tip - half, middle - half}, {tip, middle}, ink, 1.5);
        painter.draw_line({tip, middle}, {tip - half, middle + half}, ink, 1.5);
    }
}

// A Quick Access Toolbar's customize button: a short bar over an open
// chevron pointing down. It differs from "more" by the open chevron and the
// shorter bar.
void paint_customize(Painter& painter, Rect box, Color ink) {
    const Rect square = square_in(box);
    const double side = square.width;
    if (side < 4.0) return;
    const double bar = std::max(1.0, std::floor(side / 8.0));
    const double inset = std::floor(side * 0.2);
    painter.fill_rect({square.x + inset, square.y + std::floor(side * 0.25), side - inset * 2.0, bar}, ink);
    const double half = side * 0.25;
    const Point middle{square.x + side * 0.5, square.y + side * 0.75};
    painter.draw_line({middle.x - half, middle.y - half * 0.75}, middle, ink, 1.5);
    painter.draw_line(middle, {middle.x + half, middle.y - half * 0.75}, ink, 1.5);
}

void paint_mark(Painter& painter, const PartPaint& part, const ControlVisualRecipe& recipe) {
    const Rect box = part.bounds;
    const Color ink = recipe.glyph;
    switch (part.mark) {
    case PartMark::none:
    case PartMark::count:
        return;
    case PartMark::check: {
        // Two strokes through fixed fractions of the mark's box: the house
        // check is 12 by 9 units with strokes (1, 5), (4, 8), (11, 1).
        const Point start{box.x + box.width * (1.0 / 12.0), box.y + box.height * (5.0 / 9.0)};
        const Point corner{box.x + box.width * (4.0 / 12.0), box.y + box.height * (8.0 / 9.0)};
        const Point end{box.x + box.width * (11.0 / 12.0), box.y + box.height * (1.0 / 9.0)};
        painter.draw_line(start, corner, ink, 2.0);
        painter.draw_line(corner, end, ink, 2.0);
        return;
    }
    case PartMark::indeterminate:
        painter.fill_rect(box, ink);
        return;
    case PartMark::radio_dot:
        painter.fill_rounded_rect(box, std::min(box.width, box.height) * 0.5, ink);
        return;
    case PartMark::toggle_knob:
        painter.fill_rounded_rect(box, std::min(box.width, box.height) * 0.5, recipe.text);
        return;
    case PartMark::arrow_up:
    case PartMark::arrow_down:
    case PartMark::arrow_left:
    case PartMark::arrow_right:
        fill_arrow(painter, box, part.mark, ink);
        return;
    case PartMark::chevron_down: {
        const double side = std::min(box.width, box.height) * 0.5;
        const Point middle{box.x + box.width * 0.5, box.y + box.height * 0.5 + side * 0.25};
        painter.draw_line({middle.x - side * 0.5, middle.y - side * 0.5}, middle, ink, 1.5);
        painter.draw_line(middle, {middle.x + side * 0.5, middle.y - side * 0.5}, ink, 1.5);
        return;
    }
    case PartMark::chevron_right: {
        const double side = std::min(box.width, box.height) * 0.5;
        const Point middle{box.x + box.width * 0.5 + side * 0.25, box.y + box.height * 0.5};
        painter.draw_line({middle.x - side * 0.5, middle.y - side * 0.5}, middle, ink, 1.5);
        painter.draw_line(middle, {middle.x - side * 0.5, middle.y + side * 0.5}, ink, 1.5);
        return;
    }
    case PartMark::close: {
        const double side = std::min(box.width, box.height) * 0.6;
        const Point centre{box.x + box.width * 0.5, box.y + box.height * 0.5};
        const double half = side * 0.5;
        painter.draw_line({centre.x - half, centre.y - half}, {centre.x + half, centre.y + half}, ink, 1.5);
        painter.draw_line({centre.x - half, centre.y + half}, {centre.x + half, centre.y - half}, ink, 1.5);
        return;
    }
    case PartMark::grip_dots: {
        // Two-unit dots on a four-unit pitch, filling the box.
        for (double y = box.y + 1.0; y + 2.0 <= box.y + box.height; y += 4.0) {
            for (double x = box.x + 1.0; x + 2.0 <= box.x + box.width; x += 4.0) {
                painter.fill_rect({x, y, 2.0, 2.0}, recipe.muted_text);
            }
        }
        return;
    }
    case PartMark::dialog_launcher:
        paint_dialog_launcher(painter, box, ink);
        return;
    case PartMark::more:
        paint_more(painter, box, ink);
        return;
    case PartMark::overflow:
        paint_overflow(painter, box, ink);
        return;
    case PartMark::customize:
        paint_customize(painter, box, ink);
        return;
    }
}

// Text centred vertically in the part's bounds, aligned along it, clipped to
// it. The origin is the baseline, as Painter::draw_text_utf8 takes it.
void paint_text(Painter& painter, const PartPaint& part, Color color) {
    if (part.text.empty()) return;
    const Size measured = painter.measure_text_utf8(part.text, part.font);
    const double line_height = std::max(measured.height, part.font.size * 1.2);
    double x = part.bounds.x;
    if (part.alignment == PartTextAlignment::center) {
        x += (part.bounds.width - measured.width) * 0.5;
    } else if (part.alignment == PartTextAlignment::end) {
        x += part.bounds.width - measured.width;
    }
    const double top = part.bounds.y + (part.bounds.height - line_height) * 0.5;
    const double baseline = top + std::max(part.font.size, line_height * 0.82);
    painter.save();
    painter.clip_rect(part.bounds);
    painter.draw_text_utf8({x, baseline}, part.text, part.font, color);
    painter.restore();
}

} // namespace

ControlVisualContext part_visual_context(const Control& control, AnatomyStateSet states) noexcept {
    const bool hovered = states.contains(AnatomyState::hot);
    const bool pressed = any_state(states, AnatomyState::pressed, AnatomyState::captured);
    const bool selected = any_state(states, AnatomyState::selected, AnatomyState::checked) ||
                          any_state(states, AnatomyState::current, AnatomyState::open) ||
                          states.contains(AnatomyState::active_tool);
    const bool focused = states.contains(AnatomyState::focus_visible);
    const bool defaulted = states.contains(AnatomyState::preselect);
    ControlVisualContext context = control.visual_context(hovered, pressed, selected, focused, defaulted);
    if (any_state(states, AnatomyState::disabled, AnatomyState::unavailable)) {
        context.surface = ControlSurfaceState::disabled;
    }
    return context;
}

std::string_view part_mark_name(PartMark mark) noexcept {
    switch (mark) {
    case PartMark::none: return "none";
    case PartMark::check: return "check";
    case PartMark::indeterminate: return "indeterminate";
    case PartMark::radio_dot: return "radio_dot";
    case PartMark::toggle_knob: return "toggle_knob";
    case PartMark::arrow_up: return "arrow_up";
    case PartMark::arrow_down: return "arrow_down";
    case PartMark::arrow_left: return "arrow_left";
    case PartMark::arrow_right: return "arrow_right";
    case PartMark::chevron_down: return "chevron_down";
    case PartMark::chevron_right: return "chevron_right";
    case PartMark::close: return "close";
    case PartMark::grip_dots: return "grip_dots";
    case PartMark::dialog_launcher: return "dialog_launcher";
    case PartMark::more: return "more";
    case PartMark::overflow: return "overflow";
    case PartMark::customize: return "customize";
    case PartMark::count: return {};
    }
    return {};
}

void paint_part(Painter& painter, const Control& control, const PartPaint& part) {
    if (!paintable(part.bounds)) return;
    const ControlVisualContext context = part_visual_context(control, part.states);
    const Theme& theme = control.effective_theme();
    const ControlVisualRecipe& recipe = theme.resolve(part.family, context);
    paint_part_from_recipe(painter, part, recipe, context);
    if (!part.accent || !surface_role(part.role)) return;
    // Provisional accent (design language 8.5): the ramp colour, which
    // carries its own translucency, laid over the part's material in the
    // part's shape. T4 replaces this with the engine's tint.
    const Color tint = theme.accent(*part.accent);
    const double radius = part.corner_radius ? std::max(0.0, *part.corner_radius) : recipe.material.corner_radius;
    painter.fill_rounded_rect(part.bounds, radius, tint);
}

void paint_part_from_recipe(Painter& painter, const PartPaint& part,
                            const ControlVisualRecipe& recipe, ControlVisualContext context) {
    if (!paintable(part.bounds)) return;
    const bool disabled = context.surface == ControlSurfaceState::disabled;
    switch (part.role) {
    case PartRole::region:
    case PartRole::shadow:
    case PartRole::edge:
    case PartRole::perimeter:
    case PartRole::sheen:
    case PartRole::count:
        return;
    case PartRole::face:
    case PartRole::well:
    case PartRole::track:
    case PartRole::fill:
    case PartRole::thumb:
    case PartRole::actuator:
    case PartRole::chip:
    case PartRole::cap:
    case PartRole::activity:
    case PartRole::selection:
    case PartRole::drop:
    case PartRole::validation:
        paint_surface(painter, part, recipe);
        return;
    case PartRole::indicator:
    case PartRole::glyph:
        if (part.mark == PartMark::none) {
            paint_surface(painter, part, recipe);
        } else {
            paint_mark(painter, part, recipe);
        }
        return;
    case PartRole::keyline:
    case PartRole::divider:
    case PartRole::seam:
        paint_rule(painter, part.bounds, recipe);
        return;
    case PartRole::grip: {
        PartPaint dots = part;
        dots.mark = PartMark::grip_dots;
        paint_mark(painter, dots, recipe);
        return;
    }
    case PartRole::caption:
    case PartRole::label:
    case PartRole::content:
        paint_text(painter, part, disabled ? recipe.muted_text : recipe.text);
        return;
    case PartRole::icon:
        if (part.image.value != 0U) painter.draw_image(part.image, part.bounds, disabled ? 0.45 : 1.0);
        return;
    case PartRole::caret:
        painter.fill_rect({part.bounds.x, part.bounds.y, std::max(1.0, part.bounds.width),
                           part.bounds.height}, recipe.text);
        return;
    case PartRole::disclosure:
        paint_mark(painter, part, recipe);
        return;
    case PartRole::focus:
        if (context.focused) paint_focus_ring(painter, part.bounds, recipe);
        return;
    case PartRole::preselect:
        if (context.defaulted) paint_preselect(painter, part.bounds, recipe);
        return;
    }
}

} // namespace gui_forms
