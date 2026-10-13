#include "gui_forms/controls/button_base/button/button.hpp"
#include "../../basic/basic_control_rendering.hpp"
#include "gui_forms/text.hpp"
#include "gui_forms/window.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <vector>

namespace gui_forms {

Button::Button(StableId stable_id, std::string text)
    : ButtonBase(std::move(stable_id), std::move(text)) {}

void Button::set_default_button(bool is_default) {
    require_mutable();
    if (default_button_ == is_default) {
        return;
    }
    default_button_ = is_default;
    invalidate(Dirty::paint | Dirty::semantics);
}

void Button::notify_default(bool value) { set_default_button(value); }

void Button::set_dialog_result(DialogResult result) {
    require_mutable();
    switch (result) {
    case DialogResult::none:
    case DialogResult::ok:
    case DialogResult::cancel:
    case DialogResult::abort:
    case DialogResult::retry:
    case DialogResult::ignore:
    case DialogResult::yes:
    case DialogResult::no:
    case DialogResult::try_again:
    case DialogResult::continue_: break;
    default:
        throw std::invalid_argument("Button DialogResult is not a defined value");
    }
    if (dialog_result_ == result) return;
    dialog_result_ = result;
    invalidate(Dirty::semantics);
    publish_change(dialog_result_changed_, dialog_result_);
}

void Button::assign_cancel_dialog_result() {
    if (dialog_result_ == DialogResult::none) {
        set_dialog_result(DialogResult::cancel);
    }
}

void Button::on_activate() {
    const Control::Ptr retained = weak_from_this().lock();
    Window* owner = attached_window();
    ButtonBase::on_activate();
    // Read the value after Click: a handler may deliberately replace or clear
    // DialogResult before the Form/Window observes it.
    if (dialog_result_ != DialogResult::none && is_alive() &&
        attached_window() == owner && owner != nullptr) {
        (*owner).set_dialog_result(dialog_result_);
    }
}

void Button::set_visual_style(ButtonVisualStyle style) {
    require_mutable();
    if (visual_style_ == style) {
        return;
    }
    visual_style_ = style;
    invalidate(Dirty::paint | Dirty::semantics);
}

void Button::set_selected(bool selected) {
    require_mutable();
    if (selected_ == selected) return;
    selected_ = selected;
    invalidate(Dirty::style | Dirty::paint | Dirty::semantics);
    publish_change(selected_changed_, selected);
}

void Button::set_flat_border_width(double width) {
    require_mutable();
    if (!std::isfinite(width) || width < 0.0) {
        throw std::invalid_argument("button flat border width must be finite and nonnegative");
    }
    if (flat_border_width_ == width) return;
    flat_border_width_ = width;
    invalidate(Dirty::paint | Dirty::semantics);
}

namespace {

constexpr std::array<PartField<Button::Parts>, 6> button_part_fields{{
    {.declaration = {.name = "region", .role = PartRole::region, .composed_from = {}},
     .member = &Button::Parts::region},
    {.declaration = {.name = "face", .role = PartRole::face, .composed_from = {}},
     .member = &Button::Parts::face},
    {.declaration = {.name = "icon", .role = PartRole::icon, .composed_from = {}},
     .member = &Button::Parts::icon},
    {.declaration = {.name = "label", .role = PartRole::label, .composed_from = {}},
     .member = &Button::Parts::label},
    {.declaration = {.name = "preselect", .role = PartRole::preselect, .composed_from = {}},
     .member = &Button::Parts::preselect},
    {.declaration = {.name = "focus", .role = PartRole::focus, .composed_from = {}},
     .member = &Button::Parts::focus},
}};

constexpr std::array<StateField<Button::States>, 11> button_state_fields{{
    {.state = AnatomyState::normal, .member = &Button::States::normal},
    {.state = AnatomyState::hot, .member = &Button::States::hot},
    {.state = AnatomyState::pressed, .member = &Button::States::pressed},
    {.state = AnatomyState::preselect, .member = &Button::States::preselect},
    {.state = AnatomyState::selected, .member = &Button::States::selected},
    {.state = AnatomyState::enabled, .member = &Button::States::enabled},
    {.state = AnatomyState::disabled, .member = &Button::States::disabled},
    {.state = AnatomyState::focused, .member = &Button::States::focused},
    {.state = AnatomyState::focus_visible, .member = &Button::States::focus_visible},
    {.state = AnatomyState::open, .member = &Button::States::open},
    {.state = AnatomyState::closed, .member = &Button::States::closed},
}};

constexpr std::array<PartDeclaration, 6> button_parts =
    part_declarations(button_part_fields);
constexpr std::array<AnatomyState, 11> button_states =
    state_declarations(button_state_fields);

constexpr ControlAnatomy button_anatomy{
    .control_id = "button",
    .parts = button_parts,
    .states = button_states,
    .custom_states = {},
};

} // namespace

const ControlAnatomy& Button::anatomy() const noexcept {
    return button_anatomy;
}

Button::Parts Button::button_parts() const {
    // Paint passes the selection to the image lookup only on the themed path.
    const bool layout_selected = !has_style_override() && selected_;
    const bool command_alignment = visual_style_ == ButtonVisualStyle::command;
    const ButtonBase::Parts base = push_button_parts(layout_selected, command_alignment);
    Parts record;
    record.region = base.region;
    record.face = base.face;
    record.icon = base.icon;
    record.label = base.label;
    record.preselect = base.face;
    record.focus = base.focus;
    return record;
}

void Button::publish_part_geometry(std::span<PartGeometry> geometry) const {
    const Parts record = button_parts();
    write_parts(anatomy(), button_part_fields, record, geometry);
}

AnatomyStateSet Button::anatomy_states() const {
    AnatomyStateSet states = button_anatomy_states();
    if (default_button_) {
        states.insert(AnatomyState::preselect);
    }
    if (selected_) {
        states.insert(AnatomyState::selected);
    }
    return states;
}

Button::Parts Button::parts() const {
    const PartGeometryBuffer published = published_part_geometry();
    Parts record;
    read_parts(anatomy(), button_part_fields, published.parts(), record);
    return record;
}

Button::States Button::states() const {
    States record;
    read_states(button_state_fields, anatomy_states(), record);
    return record;
}

void Button::on_paint(Painter& painter, Rect) {
    const Rect bounds = local_bounds();
    const std::string display = display_text();
    if (!has_style_override()) {
        ControlVisualRole role = ControlVisualRole::button;
        if (visual_style_ == ButtonVisualStyle::accent) {
            role = ControlVisualRole::accent_button;
        } else if (visual_style_ == ButtonVisualStyle::command) {
            role = ControlVisualRole::command_button;
        }
        paint_themed_button(painter, bounds, role, default_button_,
                            visual_style_ == ButtonVisualStyle::command,
                            selected_);
        return;
    }
    switch (visual_style_) {
    case ButtonVisualStyle::standard:
        paint_button_frame(painter, bounds, default_button_);
        paint_button_text(painter, bounds, display);
        break;
    case ButtonVisualStyle::flat:
        painter.fill_rect(bounds, pressed_visual() ? style().accent_light
                                                   : style().face_light);
        if (flat_border_width_ > 0.0) {
            const double inset = flat_border_width_ * 0.5;
            painter.stroke_rect(
                {inset, inset,
                 std::max(0.0, bounds.width - flat_border_width_),
                 std::max(0.0, bounds.height - flat_border_width_)},
                default_button_ ? style().accent : style().border,
                flat_border_width_);
        }
        paint_button_text(painter, bounds, display);
        break;
    case ButtonVisualStyle::accent: {
        const Color fill = pressed_visual() ? style().link : style().accent;
        painter.fill_rect(bounds, fill);
        painter.draw_line({0.0, 0.0}, {bounds.width - 1.0, 0.0},
                          style().accent_light, 1.0);
        painter.stroke_rect({0.5, 0.5, std::max(0.0, bounds.width - 1.0),
                             std::max(0.0, bounds.height - 1.0)},
                            style().dark_border, 1.0);
        const double offset = pressed_visual() ? 1.0 : 0.0;
        paint_button_content(painter, bounds, display,
                             effectively_enabled() ? style().paper : style().disabled_text,
                             {offset, offset});
        break;
    }
    case ButtonVisualStyle::command:
        painter.fill_rect(bounds, pressed_visual() ? style().accent_light
                                                   : style().face);
        painter.fill_rect({0.0, 0.0, 4.0, bounds.height}, style().accent);
        painter.stroke_rect({0.5, 0.5, std::max(0.0, bounds.width - 1.0),
                             std::max(0.0, bounds.height - 1.0)},
                            style().border, 1.0);
        paint_button_content(painter, bounds, display,
                             effectively_enabled() ? style().text : style().disabled_text,
                             {}, false, true);
        break;
    }
}

SemanticDescriptor Button::semantic_descriptor() const {
    SemanticDescriptor descriptor = ButtonBase::semantic_descriptor();
    if (selected_) descriptor.states |= SemanticState::selected;
    return descriptor;
}

Insets Button::visual_outsets() const noexcept {
    if (has_style_override()) return {};
    ControlVisualRole role = ControlVisualRole::button;
    if (visual_style_ == ButtonVisualStyle::accent) {
        role = ControlVisualRole::accent_button;
    } else if (visual_style_ == ButtonVisualStyle::command) {
        role = ControlVisualRole::command_button;
    }
    const ControlVisualContext context = visual_context(
        hovered_visual(), pressed_visual(), selected_, focus_cue_visible(),
        default_button_);
    return resolved_visual_outsets(role, context);
}

namespace {

// The events Button adds to those of ButtonBase, in declaration order.
constexpr std::array<InteractionDeclaration, 2> button_interactions{{
    {.name = "dialog-result-changed", .payload = "DialogResult", .mutable_payload = false},
    {.name = "selected-changed", .payload = "bool", .mutable_payload = false},
}};

} // namespace

void Button::append_interactions(std::vector<InteractionDeclaration>& interactions) const {
    ButtonBase::append_interactions(interactions);
    interactions.insert(interactions.end(), button_interactions.begin(), button_interactions.end());
}

} // namespace gui_forms
