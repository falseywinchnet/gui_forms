#pragma once

#include "gui_forms/basic_controls.hpp"

#include <functional>
#include <span>

namespace gui_forms {

// Public owner-render surface. Applications configure drawing through the
// renderer-neutral Painter vocabulary without creating a Control subclass.
class DrawingSurface final : public Control {
public:
    using PaintCallback = std::function<void(Painter&, Rect, Rect)>;

    explicit DrawingSurface(StableId stable_id);

    void set_paint_callback(PaintCallback callback);
    [[nodiscard]] bool has_paint_callback() const noexcept {
        return static_cast<bool>(paint_callback_);
    }
    [[nodiscard]] bool hit_test_visible() const noexcept {
        return hit_test_visible_;
    }
    void set_hit_test_visible(bool visible);
    [[nodiscard]] SemanticRole semantic_role() const noexcept {
        return semantic_role_;
    }
    void set_semantic_role(SemanticRole role);
    [[nodiscard]] Color background() const noexcept { return background_; }
    void set_background(Color color);

    void on_paint(Painter& painter, Rect local_damage) override;
    [[nodiscard]] bool hit_test_local(Point local_point) const override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;
    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;
    // Stage T1 anatomy, as dot members (see Control::Parts).
    struct Parts final {
        PartGeometry region;
        PartGeometry face;
        PartGeometry focus;
    };
    struct States final {
        bool normal{};
        bool enabled{};
        bool disabled{};
        bool focused{};
        bool focus_visible{};
    };
    [[nodiscard]] Parts parts() const;
    [[nodiscard]] States states() const;

protected:
    void publish_part_geometry(std::span<PartGeometry> geometry) const override;

private:
    PaintCallback paint_callback_;
    SemanticRole semantic_role_{SemanticRole::image};
    Color background_{Color::rgba(0, 0, 0, 0)};
    bool hit_test_visible_{};
};

} // namespace gui_forms
