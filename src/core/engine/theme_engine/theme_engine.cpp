#include "gui_forms/engine/theme_engine/theme_engine.hpp"

#include "gui_forms/control/control/control.hpp"
#include "gui_forms/engine/part_raster/part_raster.hpp"
#include "gui_forms/theme/part_painting/part_painting.hpp"
#include "gui_forms/window/window.hpp"
#include "../../theme/part_painting/built_in_recipe.hpp"

#include <cmath>
#include <memory>

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
    if (decided.drawing != PartDrawing::primitive || window == nullptr) {
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
