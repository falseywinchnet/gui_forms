#pragma once

#include "gui_forms/controls/button_base/button_base.hpp"

namespace gui_forms {

class LinkLabel : public ButtonBase {
public:
    explicit LinkLabel(StableId stable_id, std::string text = {});

    [[nodiscard]] bool visited() const noexcept { return visited_; }
    void set_visited(bool visited);
    void on_paint(Painter& painter, Rect local_damage) override;
    void on_activate() override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;
    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;
    // Stage T1 anatomy, as dot members (see Control::Parts).
    struct Parts final {
        PartGeometry region;
        PartGeometry label;
        PartGeometry underline;
        PartGeometry focus;
    };
    struct States final {
        bool normal{};
        bool hot{};
        bool pressed{};
        bool enabled{};
        bool disabled{};
        bool focused{};
        bool focus_visible{};
        bool open{};
        bool closed{};
        bool visited{};
    };
    [[nodiscard]] Parts parts() const;
    [[nodiscard]] States states() const;
    [[nodiscard]] AnatomyStateSet anatomy_states() const override;

protected:
    void publish_part_geometry(std::span<PartGeometry> geometry) const override;

private:
    [[nodiscard]] double text_baseline() const noexcept;
    [[nodiscard]] double underline_width() const;

    bool visited_{};
};

} // namespace gui_forms
