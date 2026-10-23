#include "gui_forms/window.hpp"

#include "../../display/chunk/display_chunk.hpp"
#include "backplane_composite.hpp"

#include "gui_forms/engine/part_raster/part_raster.hpp"

#include <cstdint>
#include <memory>
#include <vector>

// The window's backplane image (E9-B; owner, 2026-10-22, option 1). See
// backplane_composite.hpp.

namespace gui_forms {

Window::EngineBackplane Window::engine_backplane(Point raster_origin, DeviceExtent extent, DeviceScale scale,
                                                 std::uint32_t margin) {
    EngineBackplane lent{};
    lent.generation = backplane_generation_;
    const double grown = static_cast<double>(margin);
    lent.origin = Point{raster_origin.x * scale.value - grown, raster_origin.y * scale.value - grown};
    BackplaneRegion region{};
    region.origin = lent.origin;
    region.extent = DeviceExtent{extent.width + margin * 2U, extent.height + margin * 2U};
    region.scale = scale;
    const std::shared_ptr<const PartRaster> held =
        engine_parts_.held_backplane(region, backplane_generation_, paint_leases_completed_);
    if (held) {
        lent.image = held;
        return lent;
    }
    const std::shared_ptr<PartRaster> made = PartRaster::create(region.extent, scale);
    if (!made || theme_ == nullptr) return lent;
    const PartRasterView image = (*made).view();
    // The window's backplane material, as the presentation paints it.
    ControlVisualContext context;
    context.surface = active_ ? ControlSurfaceState::normal : ControlSurfaceState::deactivated;
    context.high_contrast = presentation_settings_.high_contrast;
    const SurfaceMaterial& material = (*theme_).resolve(ControlVisualRole::window, context).material;
    engine_parts_.bake_backplane_material(material, client_size_, region, &image_resources(), image,
                                          paint_leases_completed_);
    // Then the backplane plane's engine part rasters over it.
    std::uint64_t skipped = 0U;
    const double width = static_cast<double>(region.extent.width) / scale.value;
    const double height = static_cast<double>(region.extent.height) / scale.value;
    const Rect covered{region.origin.x / scale.value, region.origin.y / scale.value, width, height};
    if (root_) composite_backplane_subtree(root_, covered, region, image, skipped);
    engine_parts_.hold_backplane(region, backplane_generation_, made, skipped, paint_leases_completed_);
    lent.image = made;
    return lent;
}

void Window::composite_backplane_subtree(const Control::Ptr& control, Rect clip, const BackplaneRegion& region,
                                         const PartRasterView& image, std::uint64_t& skipped) const {
    const Control& node = *control;
    if (!node.is_alive() || node.window_ != this || !node.visible_ || node.layout_collapsed_) return;
    const Rect bounds = absolute_bounds_of(node);
    const Rect visual = visual_bounds_of(node, node.effective_visual_outsets());
    const Rect drawn = Rect::intersection(visual, clip);
    const std::shared_ptr<const detail::DisplayChunk>& chunk = node.display_chunk_;
    if (!drawn.empty() && node.paint_plane_ == PaintPlane::backplane && chunk &&
        (*chunk).plane() == PaintPlane::backplane) {
        skipped = skipped + detail::composite_backplane_chunk(*chunk, Point{bounds.x, bounds.y}, drawn, region, image);
    }
    const Rect viewport = node.child_viewport_rectangle();
    const Rect children_clip =
        Rect::intersection(Rect{bounds.x + viewport.x, bounds.y + viewport.y, viewport.width, viewport.height}, clip);
    if (children_clip.empty()) return;
    for (const Control::Ptr& child : node.children_) {
        if (!child) continue;
        composite_backplane_subtree(child, children_clip, region, image, skipped);
    }
}

} // namespace gui_forms
