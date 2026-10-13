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
    [[nodiscard]] AnatomyStateSet anatomy_states() const override;

protected:
    void publish_part_geometry(std::span<PartGeometry> geometry) const override;

private:
    [[nodiscard]] double text_baseline() const noexcept;
    [[nodiscard]] double underline_width() const;

    bool visited_{};
};

} // namespace gui_forms
