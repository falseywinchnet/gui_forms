#pragma once

#include "gui_forms/inspection/inspection_types.hpp"

#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace gui_forms {

class ColorValueEditor final : public Panel {
public:
    static constexpr bool initialize_tree_after_construction = true;
    explicit ColorValueEditor(StableId stable_id, Color value = {});
    void initialize_control_tree();

    [[nodiscard]] Color value() const noexcept { return value_; }
    void set_value(Color value);
    [[nodiscard]] std::shared_ptr<TextBox> editor() const noexcept {
        return editor_;
    }
    [[nodiscard]] double swatch_width() const noexcept { return swatch_width_; }
    void set_swatch_width(double width);
    [[nodiscard]] Event<Color>& value_changed() noexcept {
        return value_changed_;
    }
    [[nodiscard]] Event<const PropertyEditorInputError&>& edit_failed() noexcept {
        return edit_failed_;
    }
    [[nodiscard]] static std::string format_value(Color value);
    [[nodiscard]] static std::optional<Color> parse_value(std::string_view text);

    [[nodiscard]] Size measure(Size available) override;
    void arrange(Rect final_bounds) override;
    void on_paint(Painter& painter, Rect local_damage) override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;
    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;
    [[nodiscard]] AnatomyStateSet anatomy_states() const override;
    // Stage T1 anatomy, as dot members.
    struct Parts final {
        ScrollableControl::Parts scrollable;
        PartGeometry swatch;
        PartGeometry field;
    };
    struct States final {
        bool normal{};
        bool pressed{};
        bool captured{};
        bool enabled{};
        bool disabled{};
        bool focused{};
        bool focus_visible{};
        bool invalid{};
        bool pending{};
    };
    [[nodiscard]] Parts parts() const;
    [[nodiscard]] States states() const;

protected:
    void append_interactions(std::vector<InteractionDeclaration>& interactions) const override;
    void on_dispose() noexcept override;
    void publish_part_geometry(std::span<PartGeometry> geometry) const override;

private:
    static std::optional<std::uint8_t> parse_nibble(char value) noexcept;
    void commit_text(const std::string& text);
    void commit(std::string_view text);
    void cancel();

    Color value_{};
    std::shared_ptr<TextBox> editor_;
    SubscriptionToken committed_;
    SubscriptionToken cancelled_;
    double swatch_width_{30.0};
    bool synchronizing_{};
    Event<Color> value_changed_;
    Event<const PropertyEditorInputError&> edit_failed_;
};

} // namespace gui_forms
