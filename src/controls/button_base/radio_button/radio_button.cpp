#include "gui_forms/controls/button_base/radio_button/radio_button.hpp"
#include "gui_forms/detail/property_binding_adapters.hpp"
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

RadioButton::RadioButton(StableId stable_id, std::string text)
    : ButtonBase(std::move(stable_id), std::move(text)) {
    set_text_alignment(ContentAlignment::middle_left);
    set_image_alignment(ContentAlignment::middle_left);
    define_bindable_property({
        {"Checked", BindingValueKind::boolean, "Behavior",
         "Whether the radio button is selected within its group.",
         BindingValue{false}, Dirty::paint | Dirty::semantics},
        detail::BindingMemberGetter<RadioButton, bool>(
            *this, &RadioButton::checked_),
        detail::ConvertedPropertySetter<RadioButton, bool>(
            *this, &RadioButton::set_checked, BindingValueKind::boolean,
            "RadioButton.Checked binding requires Boolean"),
        detail::EventChangeConnector<bool>(checked_changed_), {}, {}});
}

void RadioButton::set_checked_without_exclusion(bool checked_value) {
    require_mutable();
    if (checked_ == checked_value) {
        return;
    }
    checked_ = checked_value;
    invalidate(Dirty::paint | Dirty::semantics);
    publish_change(checked_changed_, checked_);
}

void RadioButton::set_checked(bool checked_value) {
    require_mutable();
    if (checked_ == checked_value) {
        return;
    }
    if (checked_value) {
        if (const Control::Ptr owner = parent()) {
            for (const Control::Ptr& sibling : (*owner).children()) {
                std::shared_ptr<gui_forms::RadioButton> peer = std::dynamic_pointer_cast<RadioButton>(sibling);
                if (peer && peer.get() != this && (*peer).group_name_ == group_name_ &&
                    (*peer).checked_) {
                    (*peer).set_checked_without_exclusion(false);
                    if (!is_alive()) {
                        return;
                    }
                }
            }
        }
    }
    set_checked_without_exclusion(checked_value);
}

void RadioButton::set_group_name(std::string name) {
    require_mutable();
    if (group_name_ == name) {
        return;
    }
    group_name_ = std::move(name);
    invalidate(Dirty::semantics);
    if (checked_) {
        if (const Control::Ptr owner = parent()) {
            for (const Control::Ptr& sibling : (*owner).children()) {
                std::shared_ptr<gui_forms::RadioButton> peer = std::dynamic_pointer_cast<RadioButton>(sibling);
                if (peer && peer.get() != this && (*peer).group_name_ == group_name_ &&
                    (*peer).checked_) {
                    (*peer).set_checked_without_exclusion(false);
                    if (!is_alive()) {
                        return;
                    }
                }
            }
        }
    }
}

void RadioButton::set_auto_check(bool enabled_value) {
    require_mutable();
    if (auto_check_ == enabled_value) {
        return;
    }
    auto_check_ = enabled_value;
    invalidate(Dirty::semantics);
}

void RadioButton::set_indicator_style(ChoiceIndicatorStyle style_value) {
    require_mutable();
    if (indicator_style_ == style_value) {
        return;
    }
    indicator_style_ = style_value;
    invalidate(Dirty::paint | Dirty::semantics);
}

namespace {

constexpr std::array<PartField<RadioButton::Parts>, 6> radio_button_part_fields{{
    {.declaration = {.name = "region", .role = PartRole::region, .composed_from = {}},
     .member = &RadioButton::Parts::region},
    {.declaration = {.name = "well", .role = PartRole::well, .composed_from = {}},
     .member = &RadioButton::Parts::well},
    {.declaration = {.name = "indicator", .role = PartRole::indicator, .composed_from = {}},
     .member = &RadioButton::Parts::indicator},
    {.declaration = {.name = "icon", .role = PartRole::icon, .composed_from = {}},
     .member = &RadioButton::Parts::icon},
    {.declaration = {.name = "label", .role = PartRole::label, .composed_from = {}},
     .member = &RadioButton::Parts::label},
    {.declaration = {.name = "focus", .role = PartRole::focus, .composed_from = {}},
     .member = &RadioButton::Parts::focus},
}};

constexpr std::array<StateField<RadioButton::States>, 10> radio_button_state_fields{{
    {.state = AnatomyState::normal, .member = &RadioButton::States::normal},
    {.state = AnatomyState::hot, .member = &RadioButton::States::hot},
    {.state = AnatomyState::pressed, .member = &RadioButton::States::pressed},
    {.state = AnatomyState::checked, .member = &RadioButton::States::checked},
    {.state = AnatomyState::enabled, .member = &RadioButton::States::enabled},
    {.state = AnatomyState::disabled, .member = &RadioButton::States::disabled},
    {.state = AnatomyState::focused, .member = &RadioButton::States::focused},
    {.state = AnatomyState::focus_visible, .member = &RadioButton::States::focus_visible},
    {.state = AnatomyState::open, .member = &RadioButton::States::open},
    {.state = AnatomyState::closed, .member = &RadioButton::States::closed},
}};

constexpr std::array<PartDeclaration, 6> radio_button_parts =
    part_declarations(radio_button_part_fields);
constexpr std::array<AnatomyState, 10> radio_button_states =
    state_declarations(radio_button_state_fields);

constexpr ControlAnatomy radio_button_anatomy{
    .control_id = "radio-button",
    .parts = radio_button_parts,
    .states = radio_button_states,
    .custom_states = {},
};

} // namespace

const ControlAnatomy& RadioButton::anatomy() const noexcept {
    return radio_button_anatomy;
}

void RadioButton::publish_part_geometry(std::span<PartGeometry> geometry) const {
    const Rect bounds = local_bounds();
    const double indicator_width =
        indicator_style_ == ChoiceIndicatorStyle::toggle ? 30.0 : 15.0;
    const double top = std::max(1.0, (bounds.height - 15.0) * 0.5);
    const Rect well{1.0, top, indicator_width, 15.0};
    // The toggle's knob, or the round dot drawn inside the 15-unit disc.
    Rect mark{};
    if (indicator_style_ == ChoiceIndicatorStyle::toggle) {
        const double knob_x = checked_ ? well.x + well.width - 13.0 : well.x + 2.0;
        mark = {knob_x, well.y + 2.0, 11.0, 11.0};
    } else if (checked_) {
        mark = {well.x + 4.0, well.y + 4.0, 7.0, 7.0};
    }
    const Rect content_face{well.x + well.width + 1.0, 0.0,
                            std::max(0.0, bounds.width - well.x - well.width - 1.0),
                            bounds.height};
    const ContentLayout layout =
        published_content_layout(content_face, checked_, false);
    Parts record;
    record.region = PartGeometry::from_bounds(bounds);
    record.well = PartGeometry::from_bounds(well);
    record.indicator = PartGeometry::from_bounds(mark);
    if (layout.has_image) {
        record.icon = PartGeometry::from_bounds(layout.image);
    }
    if (layout.has_text) {
        record.label = PartGeometry::from_bounds(layout.text);
    }
    record.focus = PartGeometry::from_bounds(bounds);
    write_parts(anatomy(), radio_button_part_fields, record, geometry);
}

AnatomyStateSet RadioButton::anatomy_states() const {
    AnatomyStateSet states = button_anatomy_states();
    if (checked_) {
        states.insert(AnatomyState::checked);
    }
    return states;
}

RadioButton::Parts RadioButton::parts() const {
    const PartGeometryBuffer published = published_part_geometry();
    Parts record;
    read_parts(anatomy(), radio_button_part_fields, published.parts(), record);
    return record;
}

RadioButton::States RadioButton::states() const {
    States record;
    read_states(radio_button_state_fields, anatomy_states(), record);
    return record;
}

void RadioButton::on_paint(Painter& painter, Rect) {
    const Rect bounds = local_bounds();
    if (!has_style_override()) {
        const double indicator_width =
            indicator_style_ == ChoiceIndicatorStyle::toggle ? 30.0 : 15.0;
        const double top = std::max(1.0, (bounds.height - 15.0) * 0.5);
        const Rect indicator_bounds{1.0, top, indicator_width, 15.0};
        const ControlVisualContext context = visual_context(
            hovered_visual(), pressed_visual(), checked_, focus_cue_visible());
        const ControlVisualRecipe& recipe = effective_theme().resolve(
            ControlVisualRole::choice, context);
        SurfaceMaterial indicator = recipe.material;
        indicator.corner_radius = 7.5;
        paint_surface_material(painter, indicator_bounds, indicator);
        if (indicator_style_ == ChoiceIndicatorStyle::toggle) {
            const double knob_x = checked_ ? indicator_bounds.x +
                    indicator_bounds.width - 13.0 : indicator_bounds.x + 2.0;
            painter.fill_rounded_rect(
                {knob_x, indicator_bounds.y + 2.0, 11.0, 11.0}, 5.5,
                recipe.text);
        } else if (checked_) {
            painter.fill_rounded_rect(
                {indicator_bounds.x + 4.0, indicator_bounds.y + 4.0,
                 7.0, 7.0}, 3.5, recipe.glyph);
        }
        paint_button_content(
            painter,
            {indicator_bounds.x + indicator_bounds.width + 1.0, 0.0,
             std::max(0.0, bounds.width - indicator_bounds.x -
                                   indicator_bounds.width - 1.0),
             bounds.height},
            display_text(), recipe.text, {}, checked_);
        paint_theme_cues(painter, bounds, recipe, context);
        return;
    }
    const BasicControlStyle& colors = style();
    const double indicator_width =
        indicator_style_ == ChoiceIndicatorStyle::toggle ? 30.0 : 15.0;
    const double top = std::max(1.0, (bounds.height - 15.0) * 0.5);
    if (indicator_style_ == ChoiceIndicatorStyle::toggle) {
        const Rect track{1.0, top, indicator_width, 15.0};
        painter.fill_rect(track, checked_ ? colors.accent : colors.border);
        const double knob_x = checked_ ? track.x + track.width - 13.0
                                       : track.x + 2.0;
        painter.fill_rect({knob_x, track.y + 2.0, 11.0, 11.0}, colors.paper);
        paint_button_content(
            painter,
            {track.x + track.width + 1.0, 0.0,
             std::max(0.0, bounds.width - track.x - track.width - 1.0),
             bounds.height},
            display_text(), effectively_enabled() ? colors.text : colors.disabled_text, {}, checked_);
        return;
    }
    const Color ring = indicator_style_ == ChoiceIndicatorStyle::modern && checked_
        ? colors.accent : colors.border;
    fill_radio_disc(painter, 1.0, top, ring, false);
    fill_radio_face(painter, 1.0, top, colors.paper);
    if (checked_) {
        fill_radio_disc(painter, 1.0, top,
                        indicator_style_ == ChoiceIndicatorStyle::modern
                            ? colors.accent : colors.dark_border,
                        true);
    }
    paint_button_content(
        painter, {17.0, 0.0, std::max(0.0, bounds.width - 17.0), bounds.height},
        display_text(), effectively_enabled() ? colors.text : colors.disabled_text, {}, checked_);
}

Insets RadioButton::visual_outsets() const noexcept {
    if (has_style_override()) return {};
    const ControlVisualContext context = visual_context(
        hovered_visual(), pressed_visual(), checked_, focus_cue_visible());
    return surface_material_visual_outsets(
        effective_theme().resolve(ControlVisualRole::choice, context).material);
}

void RadioButton::on_activate() {
    if (auto_check_ && !checked_) {
        set_checked(true);
        if (!is_alive()) {
            return;
        }
    }
    ButtonBase::on_activate();
}

SemanticDescriptor RadioButton::semantic_descriptor() const {
    SemanticDescriptor descriptor = ButtonBase::semantic_descriptor();
    descriptor.role = SemanticRole::radio_button;
    descriptor.value = checked_ ? "selected" : "not selected";
    if (checked_) {
        descriptor.states |= SemanticState::checked;
        descriptor.states |= SemanticState::selected;
    }
    return descriptor;
}

namespace {

// The events RadioButton adds to those of ButtonBase, in declaration order.
constexpr std::array<InteractionDeclaration, 1> radio_button_interactions{{
    {.name = "checked-changed", .payload = "bool", .mutable_payload = false},
}};

} // namespace

void RadioButton::append_interactions(std::vector<InteractionDeclaration>& interactions) const {
    ButtonBase::append_interactions(interactions);
    interactions.insert(interactions.end(), radio_button_interactions.begin(), radio_button_interactions.end());
}

} // namespace gui_forms
