#pragma once

#include "gui_forms/controls/range_control/range_control.hpp"

#include <cstdint>
#include <span>
#include <string_view>

namespace gui_forms {

enum class TrackBarVisualStyle : std::uint8_t {
    classic,
    filled,
    compact,
};

class TrackBar : public RangeControl {
public:
    explicit TrackBar(StableId stable_id);

    [[nodiscard]] double tick_frequency() const noexcept { return tick_frequency_; }
    void set_tick_frequency(double frequency);
    [[nodiscard]] bool show_ticks() const noexcept { return show_ticks_; }
    void set_show_ticks(bool show);
    [[nodiscard]] TrackBarVisualStyle visual_style() const noexcept {
        return visual_style_;
    }
    void set_visual_style(TrackBarVisualStyle style);

    // Public part geometry in local coordinates (stage T1). The value maps
    // linearly across value_span_bounds(): minimum at its left (horizontal)
    // or bottom (vertical) edge. track_bounds() is the painted groove, which
    // shares the value span's extent along the axis.
    [[nodiscard]] Rect value_span_bounds() const noexcept;
    [[nodiscard]] Rect track_bounds() const noexcept;
    // The value portion of the groove; empty in the classic style.
    [[nodiscard]] Rect fill_bounds() const noexcept;
    [[nodiscard]] Rect thumb_bounds() const noexcept;
    // The band the tick marks occupy; empty when ticks are not drawn.
    [[nodiscard]] Rect ticks_bounds() const noexcept;

    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;
    [[nodiscard]] AnatomyStateSet anatomy_states() const override;

    [[nodiscard]] Size measure(Size available) override;
    void on_paint(Painter& painter, Rect local_damage) override;
    void on_pointer(PointerEvent& event) override;
    void on_key(KeyEvent& event) override;
    void on_focus_changed(bool focused) override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;
    bool on_semantic_action(SemanticAction action,
                            std::string_view value) override;

protected:
    void publish_part_geometry(std::span<PartGeometry> geometry) const override;

private:
    [[nodiscard]] bool ticks_drawn() const noexcept;
    [[nodiscard]] double value_from_window_point(Point point) const noexcept;

    double tick_frequency_{10.0};
    bool show_ticks_{true};
    TrackBarVisualStyle visual_style_{TrackBarVisualStyle::classic};
    bool tracking_{};
    bool focused_{};
};

} // namespace gui_forms
