#pragma once

#include "gui_forms/basic_controls.hpp"
#include "gui_forms/surface_material.hpp"

#include <span>

namespace gui_forms {

// General retained material container. It is deliberately content-agnostic.
class MaterialPanel : public Panel {
public:
    explicit MaterialPanel(StableId stable_id);

    [[nodiscard]] const SurfaceMaterial& material() const noexcept {
        return material_;
    }
    void set_material(SurfaceMaterial material);
    [[nodiscard]] Event<const SurfaceMaterial&>& material_changed() noexcept {
        return material_changed_;
    }

    void on_paint(Painter& painter, Rect local_damage) override;
    [[nodiscard]] Insets visual_outsets() const noexcept override;
    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;

protected:
    void append_interactions(std::vector<InteractionDeclaration>& interactions) const override;
    void on_attached_to_window() override;
    void publish_part_geometry(std::span<PartGeometry> geometry) const override;

private:
    void validate_window_images(const SurfaceMaterial& material) const;

    SurfaceMaterial material_;
    Event<const SurfaceMaterial&> material_changed_;
};

} // namespace gui_forms
