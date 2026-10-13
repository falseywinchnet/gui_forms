#pragma once

#include "gui_forms/controls/button_base/button_base.hpp"

#include <cstdint>

namespace gui_forms {

enum class ButtonVisualStyle : std::uint8_t {
    standard,
    flat,
    accent,
    command,
};

class Button : public ButtonBase {
public:
    explicit Button(StableId stable_id, std::string text = {});

    [[nodiscard]] bool default_button() const noexcept { return default_button_; }
    void set_default_button(bool is_default);
    [[nodiscard]] DialogResult dialog_result() const noexcept {
        return dialog_result_;
    }
    void set_dialog_result(DialogResult result);
    [[nodiscard]] Event<DialogResult>& dialog_result_changed() noexcept {
        return dialog_result_changed_;
    }
    [[nodiscard]] ButtonVisualStyle visual_style() const noexcept {
        return visual_style_;
    }
    void set_visual_style(ButtonVisualStyle style);
    [[nodiscard]] bool selected() const noexcept { return selected_; }
    void set_selected(bool selected);
    [[nodiscard]] Event<bool>& selected_changed() noexcept { return selected_changed_; }
    [[nodiscard]] double flat_border_width() const noexcept {
        return flat_border_width_;
    }
    void set_flat_border_width(double width);
    void on_paint(Painter& painter, Rect local_damage) override;
    [[nodiscard]] Insets visual_outsets() const noexcept override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;
    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;
    // Stage T1 anatomy, as dot members (see Control::Parts).
    struct Parts final {
        PartGeometry region;
        PartGeometry face;
        PartGeometry icon;
        PartGeometry label;
        PartGeometry preselect;
        PartGeometry focus;
    };
    struct States final {
        bool normal{};
        bool hot{};
        bool pressed{};
        bool preselect{};
        bool selected{};
        bool enabled{};
        bool disabled{};
        bool focused{};
        bool focus_visible{};
        bool open{};
        bool closed{};
    };
    [[nodiscard]] Parts parts() const;
    [[nodiscard]] States states() const;
    [[nodiscard]] AnatomyStateSet anatomy_states() const override;

protected:
    void publish_part_geometry(std::span<PartGeometry> geometry) const override;
    // The button's parts computed for its current style.
    [[nodiscard]] Parts button_parts() const;

private:
    Event<bool> selected_changed_{};
    void notify_default(bool value) override;
    void on_activate() override;
    [[nodiscard]] DialogResult command_dialog_result() const noexcept override {
        return dialog_result_;
    }
    void assign_cancel_dialog_result() override;
    bool default_button_{};
    DialogResult dialog_result_{DialogResult::none};
    Event<DialogResult> dialog_result_changed_;
    ButtonVisualStyle visual_style_{ButtonVisualStyle::standard};
    bool selected_{};
    double flat_border_width_{1.0};
};

} // namespace gui_forms
