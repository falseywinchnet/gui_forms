#pragma once

#include "gui_forms/anatomy.hpp"
#include "gui_forms/control.hpp"

#include <cstdint>

namespace gui_forms {

// The spin column of an up-down control (NumericUpDown, DomainUpDown): an
// upper and a lower actuator, each with an arrow glyph. Paint and the owning
// control's published anatomy both take their rectangles from spin_halves and
// spin_glyph, so they describe the same parts.
struct SpinHalves final {
    Rect upper{};
    Rect lower{};
};

// Splits `column` where its halves meet: the upper half takes the floor of
// half the height, the lower half the rest.
[[nodiscard]] SpinHalves spin_halves(Rect column) noexcept;

// The arrow glyph of one half: a square half as wide as the half's shorter
// side, centred in it, on whole units.
[[nodiscard]] Rect spin_glyph(Rect half) noexcept;

enum class SpinHalf : std::uint8_t { none, upper, lower };

class SpinButtons final : public Control {
public:
    explicit SpinButtons(StableId stable_id);
    // +1 when the upper half is pressed, -1 for the lower.
    [[nodiscard]] Event<int>& stepped() noexcept { return stepped_; }
    // The states one half shows: hot while the pointer is over it, pressed
    // while the primary button went down on it and is still down.
    [[nodiscard]] AnatomyStateSet half_states(SpinHalf half) const noexcept;
    void on_paint(Painter& painter, Rect local_damage) override;
    void on_pointer(PointerEvent& event) override;

private:
    [[nodiscard]] SpinHalf half_at(Point window_position) const;
    void paint_half(Painter& painter, Rect half, SpinHalf which, PartMark arrow) const;
    void show(SpinHalf hot, SpinHalf pressed);

    Event<int> stepped_;
    SpinHalf hot_{SpinHalf::none};
    SpinHalf pressed_{SpinHalf::none};
};

} // namespace gui_forms
