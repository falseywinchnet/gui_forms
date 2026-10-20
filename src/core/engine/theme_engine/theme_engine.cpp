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
    inputs[input_slot(EngineInput::time_in_state)] = in_state;
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

} // namespace

ThemeEngine::ThemeEngine() : registry_(&builtin_primitive_registry()) {}

ThemeEngine::ThemeEngine(const ThemeRuleSheet& sheet)
    : registry_(&builtin_primitive_registry()), rules_(CompiledThemeRules::compile(sheet, *registry_)) {}

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

void ThemeEngine::paint(Painter& painter, const Control& control, const PartPaint& part) const {
    Window* const window = control.attached_window();
    std::optional<PartRecordHandle> record{};
    if (part.key && part_bounds_paintable(part.bounds) && window != nullptr) {
        record = (*window).note_engine_part(control, *part.key, part.states);
    }
    const PartPlan decided = plan(control, part);
    const bool primitive_draws = decided.drawing == PartDrawing::primitive && window != nullptr;
    // Where the part lands, and what it reads: the inputs are delivered and
    // damaged only for a part with a record (E6).
    Window::EnginePaintPlace place{};
    if (record || primitive_draws) place = (*window).engine_paint_place(control, painter);
    if (record) {
        Insets outsets{};
        std::uint64_t inputs_read = 0U;
        if (primitive_draws) {
            outsets = (*decided.resolution.primitive).descriptor().outsets;
            inputs_read = decided.resolution.inputs_read;
        }
        const Rect extent = part_extent(part.bounds, place.origin, outsets);
        (*window).note_engine_paint(*record, inputs_read, *place.painted_by, extent);
    }
    if (!primitive_draws) {
        paint_part_with_built_in_recipe(painter, control, part);
        return;
    }
    PartRasterRequest request{};
    request.primitive = decided.resolution.primitive;
    request.parameters = decided.resolution.parameters;
    request.target.logical_size = Size{part.bounds.width, part.bounds.height};
    request.target.scale = decided.scale;
    request.target.states = part.states;
    request.extent = decided.extent;
    // The inputs, then the expressions that read them.
    std::array<double, maximum_expression_inputs> inputs{};
    PartInputSources sources{};
    sources.window = window;
    sources.part_origin = Point{place.window_origin.x + part.bounds.x, place.window_origin.y + part.bounds.y};
    sources.states = part.states;
    sources.pointer = (*window).engine_pointer();
    if (record) sources.record = (*window).engine_parts().record(*record);
    gather_inputs(rules_, decided.resolution.inputs_read, sources, inputs);
    for (std::size_t index = 0; index < engine_input_count; ++index) request.target.inputs[index] = inputs[index];
    evaluate_expressions(decided.resolution, inputs, part.states, request.parameters);
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
        paint_part_with_built_in_recipe(painter, control, part);
        return;
    }
    painter.draw_part_raster(raster, Point{part.bounds.x, part.bounds.y}, 1.0);
}

} // namespace gui_forms
