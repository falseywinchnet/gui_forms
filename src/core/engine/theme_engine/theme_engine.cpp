#include "gui_forms/engine/theme_engine/theme_engine.hpp"

#include "gui_forms/control/control/control.hpp"
#include "gui_forms/engine/part_raster/part_raster.hpp"
#include "gui_forms/theme/part_painting/part_painting.hpp"
#include "gui_forms/window/window.hpp"
#include "../../theme/part_painting/built_in_recipe.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <limits>
#include <memory>
#include <span>

// The values a part is drawn with (expressions, transitions, inputs) must
// have the same bits on every host: no fused multiply-add here either.
#if defined(__clang__)
#pragma clang fp contract(off)
#endif

namespace gui_forms {
namespace {

bool animated(const PrimitiveDescriptor& descriptor) noexcept {
    for (const PrimitiveParameterDeclaration& parameter : descriptor.parameters) {
        if (parameter.animated) return true;
    }
    return false;
}

// One side of a raster: the logical length times the scale, rounded up;
// zero when it does not fit maximum_part_raster_side.
std::uint32_t device_side(double logical, double scale) noexcept {
    const double device = std::ceil(logical * scale);
    if (!(device > 0.0) || device > static_cast<double>(maximum_part_raster_side)) return 0U;
    const std::uint32_t side = static_cast<std::uint32_t>(device);
    return side;
}

// A part's extent where it was recorded: its bounds moved by the painter's
// origin and grown by its primitive's outsets.
Rect part_extent(Rect bounds, Point origin, Insets outsets) noexcept {
    const double width = bounds.width + outsets.left + outsets.right;
    const double height = bounds.height + outsets.top + outsets.bottom;
    const Rect extent{bounds.x + origin.x - outsets.left, bounds.y + origin.y - outsets.top, width, height};
    return extent;
}

// An input that is not published reads as NaN, so an expression reading it
// gives its fallback (E6).
constexpr double not_published = std::numeric_limits<double>::quiet_NaN();

// Where a part's inputs come from (E6).
struct PartInputSources final {
    const Window* window{};
    // The part's record, or null for a part painted without a key, which
    // reads only the two scales: it is drawn statically.
    const EnginePartRecord* record{};
    // The pointer in window coordinates, or none outside the window.
    std::optional<Point> pointer{};
    // The part's top-left corner in window coordinates.
    Point part_origin{};
    AnatomyStateSet states{};
    // time_in_state as an ambient effect gives it (E7), in place of the
    // time since the record's state entry.
    std::optional<double> time_in_state{};
};

std::size_t input_slot(EngineInput input) noexcept {
    const std::size_t slot = engine_input_position(input).value;
    return slot;
}

// Fills `inputs`, by expression input position, with every input published
// for the part, and NaN elsewhere. The published values are looked up only
// where `read` names them. `inputs` holds maximum_expression_inputs values.
void gather_inputs(const CompiledThemeRules& rules, std::uint64_t read, const PartInputSources& sources,
                   std::span<double> inputs) {
    std::fill(inputs.begin(), inputs.end(), not_published);
    const Window& window = *sources.window;
    inputs[input_slot(EngineInput::display_scale)] = window.scale();
    inputs[input_slot(EngineInput::text_scale)] = window.presentation_settings().text_scale;
    if (sources.record == nullptr) return;
    const EnginePartRecord& record = *sources.record;
    const FrameTime now = window.motion_clock().now();
    const double in_state = std::chrono::duration<double>(now - record.state_entered).count();
    inputs[input_slot(EngineInput::time_in_state)] = sources.time_in_state.value_or(in_state);
    // The pointer only while the part is hot (GIVEN B1-1), in part-local
    // logical units (B2-1).
    if (sources.states.contains(AnatomyState::hot) && sources.pointer) {
        const Point pointer = *sources.pointer;
        inputs[input_slot(EngineInput::pointer_x)] = pointer.x - sources.part_origin.x;
        inputs[input_slot(EngineInput::pointer_y)] = pointer.y - sources.part_origin.y;
    }
    const std::vector<std::string>& names = rules.published_values();
    for (std::size_t value = 0; value < names.size(); ++value) {
        const ExpressionInputIndex position = published_value_position(value);
        if ((read & expression_input_bit(position)) == 0U) continue;
        const std::optional<double> published = window.published_value(names[value]);
        if (published) inputs[position.value] = *published;
    }
}

// Gives each parameter whose winning declaration is an expression the
// expression's value, clamped to the parameter's range. Every expression
// reads the cascade's values of the parameters, before any expression.
void evaluate_expressions(const CompiledThemeRules::Resolution& resolution, std::span<const double> inputs,
                          AnatomyStateSet states, PrimitiveParameterBlock& parameters) noexcept {
    bool any = false;
    std::array<double, maximum_primitive_parameters> numbers{};
    for (std::size_t index = 0; index < parameters.count; ++index) {
        numbers[index] = parameters.values[index].number;
        if (resolution.expressions[index] != nullptr) any = true;
    }
    if (!any) return;
    const PrimitiveDescriptor& descriptor = (*resolution.primitive).descriptor();
    ExpressionEnvironment environment{};
    environment.inputs = inputs;
    environment.states = states;
    environment.parameters = std::span<const double>(numbers.data(), parameters.count);
    for (std::size_t index = 0; index < parameters.count; ++index) {
        const EngineExpression* const expression = resolution.expressions[index];
        if (expression == nullptr) continue;
        const ExpressionResult result = (*expression).evaluate(environment);
        const PrimitiveParameterDeclaration& declared = descriptor.parameters[index];
        const double raised = std::max(result.value, declared.minimum);
        parameters.values[index].number = std::min(raised, declared.maximum);
    }
}

// -- transitions (E7) ------------------------------------------------------------

// Where a part's transitions run: its window, its record and the record's
// handle (none for a part painted without a key, which is drawn
// statically), the part's control as the owner of its motion values, and
// its published states.
struct PartTransitionSources final {
    Window* window{};
    EnginePartRecord* record{};
    PartRecordHandle handle{};
    std::shared_ptr<Control> owner{};
    AnatomyStateSet states{};
};

using CompiledTransition = CompiledThemeRules::CompiledTransition;

// The machine state the part's states select: the first whose `when`
// states they all include, or none.
std::optional<std::size_t> machine_state(const CompiledTransition& transition, AnatomyStateSet states) noexcept {
    for (std::size_t index = 0; index < transition.states.size(); ++index) {
        const std::uint64_t needed = transition.states[index].when.standard_bits();
        if ((states.standard_bits() & needed) == needed) return index;
    }
    return std::nullopt;
}

// A property's range: [0, 1] for opacity, the presentational scale's for
// scale, the parameter's declared range otherwise.
struct PropertyRange final {
    double minimum{};
    double maximum{1.0};
};

PropertyRange property_range(const CompiledTransition& transition, const PrimitiveDescriptor* descriptor) noexcept {
    PropertyRange range{};
    if (transition.property == PresentationProperty::scale) {
        range.minimum = minimum_presentational_scale;
        range.maximum = maximum_presentational_scale;
        return range;
    }
    if (transition.property == PresentationProperty::opacity || descriptor == nullptr) return range;
    const PrimitiveParameterDeclaration& declared = (*descriptor).parameters[transition.parameter];
    range.minimum = declared.minimum;
    range.maximum = declared.maximum;
    return range;
}

// The value a property rests at when the part's states match no machine
// state: its cascade value, or 1 for opacity and scale.
double rest_value(const CompiledTransition& transition, const PrimitiveParameterBlock& parameters) noexcept {
    if (transition.property != PresentationProperty::parameter) return 1.0;
    const double value = parameters.values[transition.parameter].number;
    return value;
}

// The engine properties a part is shown with this paint.
struct PresentedProperties final {
    double opacity{1.0};
    double scale{1.0};
};

// Shows a transition's value: the part's opacity or scale, or its
// parameter, clamped to the property's range.
void show_value(const CompiledTransition& transition, double value, PropertyRange range,
                PrimitiveParameterBlock& parameters, PresentedProperties& presented) noexcept {
    const double raised = std::max(value, range.minimum);
    const double shown = std::min(raised, range.maximum);
    if (transition.property == PresentationProperty::opacity) {
        presented.opacity = shown;
        return;
    }
    if (transition.property == PresentationProperty::scale) {
        presented.scale = shown;
        return;
    }
    parameters.values[transition.parameter].number = shown;
}

// The logical extent the settled condition measures a property's value
// against (MotionValue::set_extent): one unit of the value spans 255
// device pixels over the property's range, so that a value settles within
// half an 8-bit step of its range.
double settle_extent(PropertyRange range, double scale) noexcept {
    const double span = range.maximum - range.minimum;
    const double extent = 255.0 / (span * scale);
    return extent;
}

// Runs the part's presentational state machines for one paint and shows
// their values: a parameter's in `parameters`, opacity and scale returned. A
// part with a record moves each property on its motion value, retargeting
// from the value shown; one without stands each property at its target.
// Parameter transitions run only while a primitive draws the part
// (`descriptor` set). A record's values for properties no longer
// transitioned are released.
PresentedProperties run_transitions(const CompiledThemeRules::Resolution& resolution,
                                    const PartTransitionSources& sources, const PrimitiveDescriptor* descriptor,
                                    PrimitiveParameterBlock& parameters) {
    PresentedProperties presented{};
    EnginePartRecord* const record = sources.owner ? sources.record : nullptr;
    PartMotion* motion = nullptr;
    if (record != nullptr && resolution.transition_count > 0U) {
        if (!(*record).motion) (*record).motion = std::make_unique<PartMotion>(*sources.window, sources.handle);
        motion = (*record).motion.get();
    }
    std::array<bool, maximum_part_transitions> used{};
    for (std::size_t index = 0; index < resolution.transition_count; ++index) {
        const CompiledTransition& transition = *resolution.transitions[index];
        if (transition.property == PresentationProperty::parameter && descriptor == nullptr) continue;
        const PropertyRange range = property_range(transition, descriptor);
        const std::optional<std::size_t> state = machine_state(transition, sources.states);
        const double target = state ? transition.states[*state].target : rest_value(transition, parameters);
        if (motion == nullptr) {
            show_value(transition, target, range, parameters, presented);
            continue;
        }
        // The channel already running this property, else the first free one.
        std::size_t chosen = maximum_part_transitions;
        for (std::size_t channel = 0; channel < maximum_part_transitions; ++channel) {
            const PartTransitionChannel& running = (*motion).channels[channel];
            if (!running.value || used[channel]) continue;
            if (running.property == transition.property && running.parameter == transition.parameter) {
                chosen = channel;
            }
        }
        for (std::size_t channel = 0; chosen == maximum_part_transitions && channel < maximum_part_transitions;
             ++channel) {
            if (!(*motion).channels[channel].value && !used[channel]) chosen = channel;
        }
        if (chosen == maximum_part_transitions) continue;
        used[chosen] = true;
        PartTransitionChannel& channel = (*motion).channels[chosen];
        std::optional<double> entry{};
        if (state) entry = transition.states[*state].enter_from;
        if (!channel.value) {
            // The part appears in its state: at the state's entry value if
            // it has one, else at its target.
            channel.property = transition.property;
            channel.parameter = transition.parameter;
            channel.state = state;
            const double initial = entry ? *entry : target;
            channel.value.emplace(*sources.owner, transition.curve, initial);
            MotionValue& made = *channel.value;
            static_cast<void>(made.set_extent(settle_extent(range, (*sources.window).scale())));
            made.set_listener(MotionListener::bind<PartMotion, &PartMotion::on_step>(*motion));
        } else {
            (*channel.value).set_curve(transition.curve);
            if (channel.state != state) {
                channel.state = state;
                if (entry) (*channel.value).jump_to(*entry);
            }
        }
        MotionValue& value = *channel.value;
        static_cast<void>(value.animate_to(target));
        show_value(transition, value.current(), range, parameters, presented);
    }
    if (motion != nullptr) {
        for (std::size_t channel = 0; channel < maximum_part_transitions; ++channel) {
            if (used[channel]) continue;
            (*motion).channels[channel].value.reset();
            (*motion).channels[channel].state.reset();
        }
        (*record).motion_unsettled = (*motion).moving();
    } else if (record != nullptr && (*record).motion) {
        (*record).motion.reset();
        (*record).motion_unsettled = false;
    }
    return presented;
}

// Whether the part's transitions include its scale.
bool scales(const CompiledThemeRules::Resolution& resolution) noexcept {
    for (std::size_t index = 0; index < resolution.transition_count; ++index) {
        if ((*resolution.transitions[index]).property == PresentationProperty::scale) return true;
    }
    return false;
}

// The largest scale at which `bounds`, grown about its centre, stays within
// the overflow its control grants: no more than 1 when none is granted.
double granted_scale(Rect bounds, Insets overflow) noexcept {
    double limit = maximum_presentational_scale;
    if (bounds.width > 0.0) {
        const double across = 1.0 + 2.0 * std::min(overflow.left, overflow.right) / bounds.width;
        limit = std::min(limit, across);
    }
    if (bounds.height > 0.0) {
        const double down = 1.0 + 2.0 * std::min(overflow.top, overflow.bottom) / bounds.height;
        limit = std::min(limit, down);
    }
    const double granted = std::max(1.0, limit);
    return granted;
}

// `bounds` less `insets`, each axis shrinking to the centre line of
// `bounds` where the insets leave no room on it.
Rect slot_within(Rect bounds, Insets insets) noexcept {
    Rect slot{bounds.x + insets.left, bounds.y + insets.top, bounds.width - insets.left - insets.right,
              bounds.height - insets.top - insets.bottom};
    if (!(slot.width > 0.0)) {
        slot.x = bounds.x + bounds.width * 0.5;
        slot.width = 0.0;
    }
    if (!(slot.height > 0.0)) {
        slot.y = bounds.y + bounds.height * 0.5;
        slot.height = 0.0;
    }
    return slot;
}

// `bounds` at `scale` about its centre.
Rect scaled_about_centre(Rect bounds, double scale) noexcept {
    const double width = bounds.width * scale;
    const double height = bounds.height * scale;
    const Rect scaled{bounds.x + (bounds.width - width) * 0.5, bounds.y + (bounds.height - height) * 0.5, width,
                      height};
    return scaled;
}

// The part as shown at its presentational scale: its bounds grown or
// shrunk about their centre, the growth clamped to the granted overflow.
PartPaint scaled_part(const PartPaint& part, double scale) noexcept {
    PartPaint shown = part;
    const double granted = std::min(scale, granted_scale(part.bounds, part.granted_overflow));
    if (granted != 1.0) shown.bounds = scaled_about_centre(part.bounds, granted);
    return shown;
}

// How far past its bounds the built-in recipe may draw a part that is not
// a shadow: its one-pixel contact shadow, default ring and focus outsets.
constexpr double built_in_group_margin = 4.0;

// Paints a part with the built-in recipe at `opacity`: not at all at 0, and
// between 0 and 1 as one paint group, so that the recipe's overlapping
// commands composite as one image (owner, E7-1). The group covers the
// part's bounds and what the recipe draws around them: the overlay
// shadow's outsets for a shadow part, built_in_group_margin otherwise.
void paint_built_in(Painter& painter, const Control& control, const PartPaint& part, double opacity) {
    if (opacity <= 0.0) return;
    if (opacity >= 1.0) {
        paint_part_with_built_in_recipe(painter, control, part);
        return;
    }
    Insets reach{built_in_group_margin, built_in_group_margin, built_in_group_margin, built_in_group_margin};
    if (part.role == PartRole::shadow) reach = overlay_shadow_outsets(control);
    PaintGroup group{};
    group.bounds = Rect{part.bounds.x - reach.left, part.bounds.y - reach.top,
                        part.bounds.width + reach.left + reach.right, part.bounds.height + reach.top + reach.bottom};
    group.opacity = opacity;
    painter.begin_group(group);
    paint_part_with_built_in_recipe(painter, control, part);
    painter.end_group();
}

} // namespace

ThemeEngine::ThemeEngine() : ThemeEngine(ThemeRuleSheet{}) {}

ThemeEngine::ThemeEngine(const ThemeRuleSheet& sheet)
    : registry_(&builtin_primitive_registry()),
      rules_(CompiledThemeRules::compile_with_built_in(sheet, *registry_)) {}

ThemeEngine::PartPlan ThemeEngine::plan(const Control& control, const PartPaint& part) const noexcept {
    PartPlan result{};
    if (rules_.empty()) return result;
    const Window* const window = control.attached_window();
    if (window == nullptr || !part_bounds_paintable(part.bounds)) {
        result.drawing = PartDrawing::not_paintable;
        return result;
    }
    CompiledThemeRules::PartFacts facts{};
    facts.control_type = control.anatomy().control_id;
    facts.stable_id = control.stable_id().value();
    facts.classes = control.classes();
    if (part.key) facts.part = (*part.key).part;
    facts.role = part.role;
    facts.states = part.states;
    result.resolution = rules_.resolve(facts);
    if (result.resolution.outcome == CompiledThemeRules::Outcome::role_mismatch) {
        result.drawing = PartDrawing::role_mismatch;
        return result;
    }
    if (result.resolution.outcome != CompiledThemeRules::Outcome::primitive) return result;
    // The engine's own last pass (CascadeLevel::forced_high_contrast).
    const ControlVisualContext context = part_visual_context(control, part.states);
    if (context.high_contrast) {
        result.drawing = PartDrawing::forced_high_contrast;
        return result;
    }
    const double scale = (*window).scale();
    result.scale = DeviceScale{scale};
    result.extent = DeviceExtent{device_side(part.bounds.width, scale), device_side(part.bounds.height, scale)};
    if (result.extent.width == 0U || result.extent.height == 0U) {
        result.drawing = PartDrawing::not_paintable;
        return result;
    }
    const PrimitiveDescriptor& descriptor = (*result.resolution.primitive).descriptor();
    if (animated(descriptor)) {
        const std::uint64_t pixels =
            static_cast<std::uint64_t>(result.extent.width) * static_cast<std::uint64_t>(result.extent.height);
        const std::uint64_t operations =
            pixels * static_cast<std::uint64_t>(descriptor.cost.per_device_pixel) + descriptor.cost.fixed;
        if (operations > maximum_animated_operations_per_frame) {
            result.drawing = PartDrawing::over_frame_budget;
            return result;
        }
    }
    result.drawing = PartDrawing::primitive;
    return result;
}

PartHitRegion ThemeEngine::hit_region(const Control& control, const PartPaint& part) const noexcept {
    PartHitRegion region{};
    region.bounds = part.bounds;
    const PartPlan decided = plan(control, part);
    if (decided.drawing == PartDrawing::primitive) {
        region.shape = (*decided.resolution.primitive).shape(decided.resolution.parameters);
    }
    return region;
}

Rect ThemeEngine::text_slot(const Control& control, const PartPaint& part) const noexcept {
    const PartPlan decided = plan(control, part);
    if (decided.drawing != PartDrawing::primitive) return part.bounds;
    const Window* const window = control.attached_window();
    const double text_scale = (*window).presentation_settings().text_scale;
    const Size logical{part.bounds.width, part.bounds.height};
    const Insets insets = (*decided.resolution.primitive)
                              .content_insets(decided.resolution.parameters, logical, decided.scale, text_scale);
    const Rect slot = slot_within(part.bounds, insets);
    return slot;
}

void ThemeEngine::paint(Painter& painter, const Control& control, const PartPaint& part) const {
    Window* const window = control.attached_window();
    std::optional<PartRecordHandle> record{};
    if (part.key && part_bounds_paintable(part.bounds) && window != nullptr) {
        record = (*window).note_engine_part(control, *part.key, part.states);
    }
    const PartPlan decided = plan(control, part);
    const CompiledThemeRules::Resolution& resolution = decided.resolution;
    const bool primitive_draws = decided.drawing == PartDrawing::primitive && window != nullptr;
    // Where the part lands, and what it reads: the inputs are delivered and
    // damaged only for a part with a record (E6).
    Window::EnginePaintPlace place{};
    if (record || primitive_draws) place = (*window).engine_paint_place(control, painter);
    Rect extent{};
    if (record) {
        Insets outsets{};
        std::uint64_t inputs_read = 0U;
        if (primitive_draws) {
            outsets = (*resolution.primitive).descriptor().outsets;
            inputs_read = resolution.inputs_read;
        }
        // A part that scales may draw anywhere in its granted overflow.
        if (scales(resolution)) {
            outsets.left = outsets.left + part.granted_overflow.left;
            outsets.top = outsets.top + part.granted_overflow.top;
            outsets.right = outsets.right + part.granted_overflow.right;
            outsets.bottom = outsets.bottom + part.granted_overflow.bottom;
        }
        extent = part_extent(part.bounds, place.origin, outsets);
        (*window).note_engine_paint(*record, inputs_read, *place.painted_by, extent);
    }
    // The part's record, for its transitions and its ambient effect (E7).
    EnginePartRecord* const kept = record ? (*window).engine_part_record(*record) : nullptr;
    PartTransitionSources transitions{};
    transitions.window = window;
    transitions.record = kept;
    if (record) transitions.handle = *record;
    if (kept != nullptr) transitions.owner = (*window).find(control.stable_id().value());
    transitions.states = part.states;
    if (!primitive_draws) {
        if (kept != nullptr) stop_ambient(*window, *kept, *record);
        PrimitiveParameterBlock unused{};
        const PresentedProperties presented = run_transitions(resolution, transitions, nullptr, unused);
        const PartPaint shown = scaled_part(part, presented.scale);
        paint_built_in(painter, control, shown, presented.opacity);
        return;
    }
    PartRasterRequest request{};
    request.primitive = resolution.primitive;
    request.parameters = resolution.parameters;
    request.target.logical_size = Size{part.bounds.width, part.bounds.height};
    request.target.scale = decided.scale;
    request.target.states = part.states;
    request.extent = decided.extent;
    // The ambient effect's time, then the inputs, the expressions that read
    // them, and the transitions over both.
    std::optional<double> ambient_time{};
    if (resolution.ambient != nullptr) {
        const double window_area = (*window).client_size().width * (*window).client_size().height;
        const bool everyday = !control.on_ceremonial_surface();
        const bool area_ok = !everyday || extent.area() <= maximum_ambient_window_fraction * window_area;
        ambient_time = ambient_seconds(*window, kept, record, *resolution.ambient, area_ok);
    } else if (kept != nullptr) {
        stop_ambient(*window, *kept, *record);
    }
    std::array<double, maximum_expression_inputs> inputs{};
    PartInputSources sources{};
    sources.window = window;
    sources.part_origin = Point{place.window_origin.x + part.bounds.x, place.window_origin.y + part.bounds.y};
    sources.states = part.states;
    sources.pointer = (*window).engine_pointer();
    sources.record = kept;
    sources.time_in_state = ambient_time;
    gather_inputs(rules_, resolution.inputs_read, sources, inputs);
    for (std::size_t index = 0; index < engine_input_count; ++index) request.target.inputs[index] = inputs[index];
    evaluate_expressions(resolution, inputs, part.states, request.parameters);
    const PrimitiveDescriptor& descriptor = (*request.primitive).descriptor();
    const PresentedProperties presented = run_transitions(resolution, transitions, &descriptor, request.parameters);
    const double opacity = presented.opacity;
    // A part at opacity 0, or one its primitive draws as nothing (a flat
    // bulge), composites nothing and needs no raster.
    if (opacity <= 0.0 || (*request.primitive).blank(request.parameters)) return;
    // A scaled part is drawn at its scaled size (D5-1), so that its edges
    // stay sharp; one whose scaled raster would not fit, or would break the
    // animated frame budget, is drawn at its own size.
    PartPaint shown = scaled_part(part, presented.scale);
    if (shown.bounds != part.bounds) {
        const double scale = decided.scale.value;
        const DeviceExtent scaled{device_side(shown.bounds.width, scale), device_side(shown.bounds.height, scale)};
        const std::uint64_t pixels =
            static_cast<std::uint64_t>(scaled.width) * static_cast<std::uint64_t>(scaled.height);
        const std::uint64_t operations =
            pixels * static_cast<std::uint64_t>(descriptor.cost.per_device_pixel) + descriptor.cost.fixed;
        const bool within = scaled.width > 0U && scaled.height > 0U &&
                            (!animated(descriptor) || operations <= maximum_animated_operations_per_frame);
        if (within) {
            request.target.logical_size = Size{shown.bounds.width, shown.bounds.height};
            request.extent = scaled;
        } else {
            shown = part;
        }
    }
    std::shared_ptr<const PartRaster> raster{};
    if (record) {
        const PartRasterResult drawn = (*window).engine_part_raster(*record, request);
        raster = drawn.raster;
    } else {
        // A part with no record: a raster of its own, for this paint only.
        std::shared_ptr<PartRaster> own = PartRaster::create(request.extent, request.target.scale);
        PrimitivePreparation preparation{};
        const EnginePrimitive& primitive = *request.primitive;
        bool painted = false;
        if (own && primitive.prepare(request.parameters, request.target, preparation) ==
                       PreparationOutcome::prepared) {
            const RasterOutcome outcome =
                primitive.rasterize(request.parameters, request.target, preparation, (*own).view());
            painted = outcome == RasterOutcome::painted;
        }
        if (painted) raster = std::move(own);
    }
    if (!raster) {
        paint_built_in(painter, control, shown, opacity);
        return;
    }
    painter.draw_part_raster(raster, Point{shown.bounds.x, shown.bounds.y}, opacity);
}

double ThemeEngine::ambient_seconds(Window& window, EnginePartRecord* record,
                                    const std::optional<PartRecordHandle>& handle, const AmbientEffect& ambient,
                                    bool area_ok) {
    const double pose = std::chrono::duration<double>(ambient.pose).count();
    // A part without a record is drawn statically, at its pose.
    if (record == nullptr || !handle) return pose;
    EnginePartRecord& part = *record;
    part.ambient = true;
    if (!area_ok && !part.ambient_refused) window.metrics().record_engine_ambient_refusal();
    part.ambient_refused = !area_ok;
    if (!area_ok || window.motion_clock().ambient_suppressed()) {
        window.stop_engine_ambient(*handle);
        return pose;
    }
    const std::chrono::duration<double> frame(1.0 / ambient.frames_per_second);
    const FrameInterval interval = std::chrono::ceil<FrameInterval>(frame);
    window.run_engine_ambient(*handle, interval);
    // The time in the part's states, wrapped to the period, so the effect
    // cycles; exact in the clock's ticks.
    const FrameInterval elapsed = window.motion_clock().now() - part.state_entered;
    const FrameInterval forward = elapsed < FrameInterval::zero() ? FrameInterval::zero() : elapsed;
    const FrameInterval within = forward % ambient.period;
    const double seconds = std::chrono::duration<double>(within).count();
    return seconds;
}

void ThemeEngine::stop_ambient(Window& window, EnginePartRecord& record, PartRecordHandle handle) noexcept {
    if (!record.ambient) return;
    record.ambient = false;
    record.ambient_refused = false;
    window.stop_engine_ambient(handle);
}

} // namespace gui_forms
