#pragma once

#include "gui_forms/engine/primitive_registry/primitive_registry.hpp"
#include "gui_forms/surface_material/types/surface_material_types.hpp"

#include <span>

namespace gui_forms::engine_primitives {

// The material primitive (T2 plan, phase E8; owner decision E8-1 (a)): a
// deterministic engine kernel that rasterizes the theme's SurfaceMaterial
// for one of its recipe roles. Its `recipe` choice names the role
// (ControlVisualRole, in enum order: window, panel, card, button,
// accent_button, command_button, choice, editor, menu_item, selection,
// progress, tool_tip); the engine resolves that role's recipe for the
// part's state and lends the recipe's material in the target. The kernel
// draws the material's layers in the order paint_surface_material does:
//
// 1. outer shadows, under the box;
// 2. fills, clipped to the box (rounded by the material's corner radius, or
//    the part's own when it sets one): solid; linear (endpoints, or a CSS
//    angle through engine_sine_cosine), padded, repeated or reflected;
//    radial, padded; and images (see material_image.hpp), at their opacity;
// 3. inset shadows, inside the box;
// 4. the border, a ring inside the box's edge;
// 5. the border's single edges;
// 6. keylines, clipped to the box when it is rounded.
//
// Every layer is anti-aliased by coverage in device pixels and blended
// premultiplied in double; each pixel is quantized once. Gradient colours
// are interpolated in straight colour, then premultiplied. A shadow of blur
// b at scale s is a Gaussian of standard deviation σ = b / 2 × s device
// pixels, widened by the pixel's own box filter, σ' = sqrt(σ² + 1/12); its
// coverage is the smaller of the Gaussian integral over the shadow's
// rectangle and the integral across its rounded edge, Φ(−d / σ'), where d is
// the pixel centre's signed distance to the rounded shadow shape and Φ the
// polynomial normal integral of gaussian_integral (error at most 4e-4).
//
// Outsets. The primitive draws up to 8 logical units outside the part's box
// on every side (its descriptor's outsets): the destination covers the box
// grown by them, and the box starts 8 × s device pixels in. An outer shadow
// that reaches further is clipped there: the rest of it is not drawn.
//
// The hit region is the part's box, not the rounded shape: the corner
// radius is the theme material's, which the primitive's parameters do not
// carry. No text slot either (content_insets is none).
//
// Refusals (accepts). The primitive draws nothing, and the engine draws the
// part with the built-in recipe and counts the refusal, when the target
// lends no material, the material is not valid, an image fill's image is
// not in the window's resources with its declared pixel size, or the
// material's estimated bake exceeds the bound of material_bake_policy.hpp.
// An image fill whose pixels cannot be decoded or stored at bake time is
// drawn as nothing; the other layers are drawn.
class MaterialPrimitive final : public EnginePrimitive {
public:
    [[nodiscard]] const PrimitiveDescriptor& descriptor() const noexcept override;
    [[nodiscard]] std::optional<ControlVisualRole> theme_material(
        const PrimitiveParameterBlock& parameters) const noexcept override;
    [[nodiscard]] bool accepts(const PrimitiveParameterBlock& parameters,
                               const PrimitiveTarget& target) const noexcept override;

private:
    void derive(const PrimitiveParameterBlock& parameters, const PrimitiveTarget& target,
                std::span<double> values) const noexcept override;
    void fill(const PrimitiveParameterBlock& parameters, const PrimitiveTarget& target,
              std::span<const double> prepared, const PartRasterView& destination) const noexcept override;
};

// The recipe roles' names, the `recipe` choices, in ControlVisualRole order.
[[nodiscard]] std::span<const std::string_view> material_recipe_names() noexcept;

// The estimated bake of `material` for `target`, in nanoseconds on the
// reference machine (material_bake_policy.hpp), into a destination of the
// box grown by the outsets.
[[nodiscard]] double material_bake_estimate(const SurfaceMaterial& material, const PrimitiveTarget& target) noexcept;

// The kernel alone: draws `material` for `target` into every pixel of
// `destination`, without accepts()'s checks or bound. The caller has
// validated the destination (validate_raster_view), the target and the
// material (valid_surface_material); target.material is not read. The bake
// measurement uses it to time materials past the bound.
void bake_material(const SurfaceMaterial& material, const PrimitiveTarget& target,
                   const PartRasterView& destination) noexcept;

// Φ, the standard normal integral, by a polynomial: deterministic, absolute
// error at most 4e-4, exactly 0 and 1 from ±3.5 on.
[[nodiscard]] double gaussian_integral(double u) noexcept;

} // namespace gui_forms::engine_primitives
