#pragma once

#include "gui_forms/inspection/inspection_types.hpp"

#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace gui_forms {

class PropertyList final : public Panel {
public:
    explicit PropertyList(StableId stable_id);
    ~PropertyList() override;

    [[nodiscard]] const std::vector<PropertyGroupSpec>& groups() const noexcept;
    void set_groups(std::vector<PropertyGroupSpec> groups);
    bool set_value(std::string_view row_id, std::string value);
    bool set_description(std::string_view row_id, std::string description);
    bool set_validation(std::string_view row_id, std::string message);
    bool set_reset_enabled(std::string_view row_id, bool enabled);
    bool set_group_expanded(std::string_view group_id, bool expanded);
    bool set_row_expanded(std::string_view row_id, bool expanded);
    [[nodiscard]] std::optional<bool> row_expanded(
        std::string_view row_id) const;
    [[nodiscard]] std::optional<std::string> value(
        std::string_view row_id) const;
    [[nodiscard]] Control::Ptr editor(std::string_view row_id) const;
    bool replace_editor(std::string_view row_id, Control::Ptr editor);
    [[nodiscard]] std::shared_ptr<Button> reset_button(
        std::string_view row_id) const;
    void set_header_content(Control::Ptr content, double height);
    [[nodiscard]] Control::Ptr header_content() const noexcept;
    [[nodiscard]] double header_height() const noexcept;
    void set_header_height(double height);

    [[nodiscard]] double label_width() const noexcept;
    void set_label_width(double width);
    [[nodiscard]] FontSpec font() const noexcept;
    void set_font(FontSpec font);
    // Minimum logical row height; editable rows retain five extra pixels.
    // Font-fit and stacked/validation content may require more space.
    [[nodiscard]] double row_height() const noexcept;
    void set_row_height(double height);
    [[nodiscard]] double scroll_offset() const noexcept;
    void set_scroll_offset(double offset);
    [[nodiscard]] double content_height() const noexcept;

    [[nodiscard]] Event<const PropertyValueChange&>& value_changed() noexcept {
        return value_changed_;
    }
    [[nodiscard]] Event<const PropertyValueChange&>& value_committed() noexcept {
        return value_committed_;
    }
    [[nodiscard]] Event<const PropertyGroupChange&>& group_changed() noexcept {
        return group_changed_;
    }
    [[nodiscard]] Event<const PropertyRowExpansionChange&>&
    row_expansion_changed() noexcept { return row_expansion_changed_; }
    [[nodiscard]] Event<const PropertyResetRequest&>& reset_requested() noexcept {
        return reset_requested_;
    }

    [[nodiscard]] Size measure(Size available) override;
    void arrange(Rect final_bounds) override;
    void on_paint(Painter& painter, Rect local_damage) override;
    void on_pointer(PointerEvent& event) override;
    void on_key(KeyEvent& event) override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;
    [[nodiscard]] std::vector<SemanticNode> semantic_virtual_children() const override;
    bool on_semantic_child_action(std::string_view stable_id,
                                  SemanticAction action,
                                  std::string_view value) override;
    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;
    [[nodiscard]] std::size_t realized_item_count() const override;
    [[nodiscard]] std::string realized_item_stable_id(std::size_t item) const override;
    [[nodiscard]] AnatomyStateSet realized_item_anatomy_states(
        std::size_t item) const override;
    // Stage T1 anatomy, as dot members.
    struct Parts final {
        ScrollableControl::Parts scrollable;
        PartGeometry face;
    };
    [[nodiscard]] Parts parts() const;
    struct ItemParts final {
        PartGeometry group_header;
        PartGeometry group_expander;
        PartGeometry group_title;
        PartGeometry property_row;
        PartGeometry property_name;
        PartGeometry property_value;
        PartGeometry row_expander;
        PartGeometry validation;
        PartGeometry row_divider;
    };
    struct ItemStates final {
        bool open{};
        bool closed{};
        bool invalid{};
        bool read_only{};
        bool disabled{};
        bool group_header{};
    };
    struct Item final {
        std::size_t index{};
        std::string stable_id;
        ItemParts parts;
        ItemStates states;
    };
    // The items currently realized, in paint order (owner decision 5).
    [[nodiscard]] std::vector<Item> realized_items() const;

protected:
    void append_interactions(std::vector<InteractionDeclaration>& interactions) const override;
    void on_dispose() noexcept override;
    void publish_part_geometry(std::span<PartGeometry> geometry) const override;
    void publish_realized_item_geometry(std::size_t item,
                                        std::span<PartGeometry> geometry) const override;

private:
    struct Impl;
    // One drawn row: a group header, or a visible property row of an
    // expanded group. `group_y` is a group header's top in local coordinates.
    struct ItemEntry final {
        bool group{};
        std::size_t group_index{};
        std::size_t row_index{};
        double group_y{};
    };
    [[nodiscard]] std::vector<ItemEntry> item_entries() const;
    std::unique_ptr<Impl> impl_;
    Event<const PropertyValueChange&> value_changed_;
    Event<const PropertyValueChange&> value_committed_;
    Event<const PropertyGroupChange&> group_changed_;
    Event<const PropertyRowExpansionChange&> row_expansion_changed_;
    Event<const PropertyResetRequest&> reset_requested_;
};

} // namespace gui_forms
