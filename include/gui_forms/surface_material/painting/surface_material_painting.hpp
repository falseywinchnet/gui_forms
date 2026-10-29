#pragma once

#include "gui_forms/surface_material/types/surface_material_types.hpp"

#include <span>

namespace gui_forms {

[[nodiscard]] bool valid_surface_material(
    const SurfaceMaterial& material) noexcept;
void paint_surface_material(Painter& painter, Rect bounds,
                            const SurfaceMaterial& material);
[[nodiscard]] Insets surface_material_visual_outsets(
    const SurfaceMaterial& material) noexcept;
// Whether paint_surface_material leaves the whole of its bounds opaque,
// whatever lay beneath: a valid, unfinished material with square corners, no
// visual outsets, filter or mask, and an opaque solid fill drawn normally,
// over which everything else it draws composites.
[[nodiscard]] bool surface_material_opaque(
    const SurfaceMaterial& material) noexcept;
// How far the outer (not inset) shadows among `shadows` reach outside the
// rect that casts them: three blur radii plus any positive spread, shifted
// by each offset.
[[nodiscard]] Insets box_shadow_visual_outsets(
    std::span<const MaterialShadow> shadows) noexcept;

} // namespace gui_forms
