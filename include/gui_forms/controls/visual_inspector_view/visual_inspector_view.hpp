#pragma once

#include "gui_forms/control.hpp"

#include <span>
#include <string>

namespace gui_forms {

// A reusable, renderer-neutral presentation of Window visual inspection. The
// view consumes only the public snapshot and is suitable for demoboards and
// local application diagnostics; it is never required for ordinary rendering.
class VisualInspectorView final : public Control {
public:
    explicit VisualInspectorView(StableId stable_id,
                                 std::string target_stable_id = {});

    [[nodiscard]] const std::string& target_stable_id() const noexcept {
        return target_stable_id_;
    }
    void set_target_stable_id(std::string stable_id);
    void refresh();

    void on_frame(FrameTime now) override;
    void on_paint(Painter& painter, Rect local_damage) override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;
    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;
    // Stage T1 anatomy, as dot members (see Control::Parts).
    struct Parts final {
        PartGeometry region;
        PartGeometry face;
        PartGeometry header;
        PartGeometry accent_strip;
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
    [[nodiscard]] std::string accessible_value() const;
    std::string target_stable_id_;
};

// Transparent overlay companion which draws the target's arranged, visual,
// and effective clipped bounds in three distinct colors. It does not alter hit
// testing or the inspected control tree.
class VisualInspectorOverlay final : public Control {
public:
    explicit VisualInspectorOverlay(StableId stable_id,
                                    std::string target_stable_id = {});

    [[nodiscard]] const std::string& target_stable_id() const noexcept {
        return target_stable_id_;
    }
    void set_target_stable_id(std::string stable_id);
    void refresh();

    void on_frame(FrameTime now) override;
    void on_paint(Painter& painter, Rect local_damage) override;
    [[nodiscard]] bool hit_test_local(Point local_point) const override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;
    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;
    // Stage T1 anatomy, as dot members (see Control::Parts).
    struct Parts final {
        PartGeometry region;
        PartGeometry highlight;
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
    std::string target_stable_id_;
};

} // namespace gui_forms
