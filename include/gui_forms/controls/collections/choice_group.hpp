#pragma once

#include "gui_forms/basic_controls.hpp"
#include "gui_forms/controls/panel/panel.hpp"
#include "gui_forms/detail/scalar_binding.hpp"

#include <span>
#include <vector>

namespace gui_forms {

struct ChoiceItem final {
    int id{};
    std::string label{};
    std::string glyph{};
    ImageId image{};
    std::string accessible_name{};
};

class ChoiceGroup final : public Panel {
public:
    explicit ChoiceGroup(StableId id);
    ~ChoiceGroup() override;
    void set_items(std::vector<ChoiceItem> items);
    [[nodiscard]] std::span<const ChoiceItem> items() const noexcept;
    [[nodiscard]] int selected_index() const noexcept {
        const int index = selection_.get();
        return index;
    }
    void set_selected_index(int index);
    void bind(Value<int>& selection);
    void unbind() noexcept;
    [[nodiscard]] Value<int>& selection() noexcept { return selection_; }
    [[nodiscard]] Event<int>& changed() noexcept {
        Event<int>& event = selection_.changed();
        return event;
    }
    [[nodiscard]] Event<int>& activated() noexcept { return activated_; }
    [[nodiscard]] Button& item(int index) const;
    [[nodiscard]] Control* part(std::string_view name, int index) const;
    void arrange(Rect bounds) override;
    void on_key(KeyEvent& event) override;
    void on_key_bubble(KeyEvent& event) override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;

private:
    struct Entry;
    struct Revision;
    struct ItemClick;
    struct ItemAvailability;
    void refresh_tab_stop();
    void activate_item(int id);
    void apply_selection(int index);
    void validate_selection(int index) const;
    void validate_local_selection(int index) const;
    Value<int> selection_{-1};
    std::unique_ptr<detail::ScalarBinding<ChoiceGroup, int>> binding_{};
    std::shared_ptr<Revision> revision_{};
    Event<int> activated_{};
    bool replacing_{};
};

} // namespace gui_forms
