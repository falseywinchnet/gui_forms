#pragma once

#include "gui_forms/engine/primitive_registry/primitive_registry.hpp"

#include <span>

namespace gui_forms::engine_primitives {

// The overlay shadow (T2 plan, phase E9; GIVEN E1-1): an engine primitive
// with the first-party blur, identical on every host. It draws the theme's
// overlay shadow (ThemeDefinition::overlay_shadow, or its high-contrast
// layers) for a `shadow` part, around the casting surface's box, with the
// material kernel (material_primitive.hpp): the engine lends the layers as
// a material of one transparent fill and those outer shadows, at the
// part's corner radius (its own, else its family recipe's), so that the
// shadow is drawn as an outer shadow of the material primitive is, under
// and around the box. A theme that casts none draws nothing. A built-in
// rule selects it for every shadow part, replacing Painter::draw_box_shadow;
// the damage outsets controls report (overlay_shadow_outsets) are
// unchanged, and reach at least as far as the shadow draws.
//
// Outsets. It draws up to overlay_shadow_outset logical units outside the
// box on every side.
//
// Refusals (accepts). The engine draws the part with the built-in recipe
// (draw_box_shadow) and counts the refusal when the target lends no valid
// material, a shadow is inset, a shadow reaches past the outset (its
// spread, offset and 3.5 σ' of blur, rounded up by a device pixel), or the
// bake effort of its reference (it slices like any material) exceeds the
// budget of material_bake_policy.hpp.
inline constexpr double overlay_shadow_outset = 32.0;

class OverlayShadowPrimitive final : public EnginePrimitive {
public:
    [[nodiscard]] const PrimitiveDescriptor& descriptor() const noexcept override;
    [[nodiscard]] bool lends_overlay_shadow() const noexcept override;
    [[nodiscard]] bool accepts(const PrimitiveParameterBlock& parameters,
                               const PrimitiveTarget& target) const noexcept override;
    [[nodiscard]] PrimitiveSlicePlan slice_plan(const PrimitiveParameterBlock& parameters,
                                                const PrimitiveTarget& target,
                                                DeviceExtent extent) const noexcept override;

private:
    void derive(const PrimitiveParameterBlock& parameters, const PrimitiveTarget& target,
                std::span<double> values) const noexcept override;
    void fill(const PrimitiveParameterBlock& parameters, const PrimitiveTarget& target,
              std::span<const double> prepared, const PartRasterView& destination) const noexcept override;
};

} // namespace gui_forms::engine_primitives
