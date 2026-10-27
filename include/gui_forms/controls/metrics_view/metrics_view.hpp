#pragma once

#include "gui_forms/basic_controls.hpp"

#include <span>
#include <string>

namespace gui_forms {

// Renderer-neutral view over the structured Window metrics snapshot.
class MetricsView final : public Control {
public:
    explicit MetricsView(StableId stable_id,
                         std::string title = "Runtime metrics");

    [[nodiscard]] const std::string& title() const noexcept { return title_; }
    void set_title(std::string title);
    [[nodiscard]] const BasicControlStyle& style() const noexcept {
        return style_;
    }
    void set_style(BasicControlStyle style);
    [[nodiscard]] double accent_width() const noexcept { return accent_width_; }
    void set_accent_width(double width);

    void on_paint(Painter& painter, Rect local_damage) override;
    [[nodiscard]] bool hit_test_local(Point local_point) const override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;
    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;
    // Stage T1 anatomy, as dot members (see Control::Parts).
    struct Parts final {
        PartGeometry region;
        PartGeometry face;
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
    [[nodiscard]] std::string metrics_text() const;
    std::string title_;
    // The counters as the card last painted them, and its accessible value.
    // Hosts keep a published tree until the window's semantic generation
    // changes, so a value read live at each query (it changes with every
    // paint) would reach them a paint late or never. A paint that changes
    // this text invalidates semantics, which advances the generation; until
    // the first paint it is empty.
    std::string painted_metrics_;
    BasicControlStyle style_;
    double accent_width_{5.0};
};

} // namespace gui_forms
