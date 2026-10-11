#include "pages.hpp"

namespace gui_forms::gallery::pages {

void add_checked_collections(const std::shared_ptr<Surface>& page,
                             const std::shared_ptr<PageContext>& context) {
    class CheckedItemStatus final {
    public:
        explicit CheckedItemStatus(std::weak_ptr<Label> status) noexcept
            : status_(std::move(status)) {}

        void operator()(std::size_t index, CheckState state) const
        {
            if (const std::shared_ptr<Label> status = status_.lock()) {
                (*status).set_text(
                    "Item " + std::to_string(index + 1U) + " committed " +
                    (state == CheckState::checked ? "checked" :
                     state == CheckState::indeterminate ? "mixed" :
                     "unchecked") +
                    " · selection unchanged");
            }
        }

    private:
        std::weak_ptr<Label> status_;
    };

    enum class CheckedListCommand {
        check_all,
        clear_all,
        set_mixed,
    };

    class CheckedListCommandClick final {
    public:
        CheckedListCommandClick(std::weak_ptr<CheckedListBox> list,
                                CheckedListCommand command) noexcept
            : list_(std::move(list)), command_(command) {}

        void operator()(ButtonBase&) const
        {
            const std::shared_ptr<CheckedListBox> list = list_.lock();
            if (!list) return;
            if (command_ == CheckedListCommand::set_mixed) {
                if (!(*list).items().empty()) {
                    (*list).set_item_check_state(3U, CheckState::indeterminate);
                }
                return;
            }
            const bool checked = command_ == CheckedListCommand::check_all;
            for (std::size_t index = 0U; index < (*list).items().size(); ++index) {
                (*list).set_item_checked(index, checked);
            }
        }

    private:
        std::weak_ptr<CheckedListBox> list_;
        CheckedListCommand command_{};
    };

    (*page).add_at(label("gallery.checked.heading", "CHECKED COLLECTIONS", 22.0,
                       700, shell_blue_dark), {24.0, 18.0, 620.0, 34.0});
    (*page).add_at(label("gallery.checked.lead",
                       "Check state remains independent of selection, with cancellable pre-change events, keyboard control, and checkable virtual rows.",
                       12.0, 400, muted), {26.0, 52.0, 920.0, 24.0});

    std::shared_ptr<LayoutGroup> deliberate_group = group("gallery.checked.deliberate",
                                  "CheckedListBox · deliberate two-click policy",
                                  {450.0, 400.0});
    (*page).add_at(deliberate_group, {24.0, 92.0, 450.0, 400.0});
    std::shared_ptr<gui_forms::CheckedListBox> deliberate = make_control<CheckedListBox>(
        StableId("gallery.checked.deliberate.list"));
    (*deliberate).set_accessible_name("Deliberate feature selection");
    (*deliberate).add_item("Renderer-neutral core", CheckState::checked);
    (*deliberate).add_item("Window-owned resources", CheckState::checked);
    (*deliberate).add_item("Native accessibility", CheckState::indeterminate);
    (*deliberate).add_item("Stable semantic IDs", CheckState::checked);
    (*deliberate).add_item("Renderer-free build", CheckState::unchecked);
    (*deliberate).add_item("Sustained soak gate", CheckState::unchecked);
    (*deliberate_group).add_at(deliberate, {24.0, 40.0, 400.0, 282.0});
    std::shared_ptr<Label> deliberate_note = label("gallery.checked.deliberate.note",
                                 "First click selects. Click the selected row again or press Space to toggle.",
                                 10.0, 400, muted);
    (*deliberate_note).set_text_wrapping(TextWrapping::word);
    (*deliberate_note).set_vertical_alignment(VerticalAlignment::near);
    (*deliberate_group).add_at(deliberate_note, {24.0, 332.0, 400.0, 48.0});

    std::shared_ptr<LayoutGroup> immediate_group = group("gallery.checked.immediate",
                                 "CheckedListBox · CheckOnClick + extended selection",
                                 {460.0, 400.0});
    (*page).add_at(immediate_group, {494.0, 92.0, 460.0, 400.0});
    std::shared_ptr<gui_forms::CheckedListBox> immediate = make_control<CheckedListBox>(
        StableId("gallery.checked.immediate.list"));
    (*immediate).set_accessible_name("Immediate capability selection");
    (*immediate).set_check_on_click(true);
    (*immediate).set_selection_mode(ListSelectionMode::multiple_extended);
    (*immediate).add_item("Pointer activation", CheckState::checked);
    (*immediate).add_item("Space-key toggle", CheckState::checked);
    (*immediate).add_item("Cancelled transition", CheckState::unchecked);
    (*immediate).add_item("Indeterminate state", CheckState::indeterminate);
    (*immediate).add_item("Mutation remapping", CheckState::checked);
    (*immediate).add_item("Semantic press", CheckState::unchecked);
    (*immediate).select_index(1U);
    (*immediate).select_index(4U, true, false);
    (*immediate_group).add_at(immediate, {24.0, 40.0, 410.0, 282.0});
    std::shared_ptr<Label> immediate_note = label("gallery.checked.immediate.note",
                                "CheckOnClick toggles immediately; Shift/Ctrl selection remains a separate model.",
                                10.0, 400, muted);
    (*immediate_note).set_text_wrapping(TextWrapping::word);
    (*immediate_note).set_vertical_alignment(VerticalAlignment::near);
    (*immediate_group).add_at(immediate_note, {24.0, 332.0, 410.0, 48.0});

    std::shared_ptr<LayoutGroup> commands = group("gallery.checked.commands",
                          "Check-state commands and ordered telemetry", {930.0, 112.0});
    (*page).add_at(commands, {24.0, 514.0, 930.0, 112.0});
    std::shared_ptr<gui_forms::Button> check_all = make_control<Button>(StableId("gallery.checked.command.all"),
                                          "Check all");
    std::shared_ptr<gui_forms::Button> clear = make_control<Button>(StableId("gallery.checked.command.clear"),
                                      "Clear all");
    std::shared_ptr<gui_forms::Button> mixed = make_control<Button>(StableId("gallery.checked.command.mixed"),
                                      "Set mixed");
    (*check_all).set_visual_style(ButtonVisualStyle::accent);
    (*commands).add_at(check_all, {22.0, 42.0, 130.0, 34.0});
    (*commands).add_at(clear, {166.0, 42.0, 130.0, 34.0});
    (*commands).add_at(mixed, {310.0, 42.0, 130.0, 34.0});
    std::shared_ptr<Label> status = label("gallery.checked.status",
                        "Ready · pre-change event precedes committed state",
                        10.0, 600, green);
    (*status).set_text_wrapping(TextWrapping::word);
    (*status).set_vertical_alignment(VerticalAlignment::near);
    (*commands).add_at(status, {470.0, 38.0, 420.0, 48.0});
    const std::weak_ptr<CheckedListBox> weak_immediate = immediate;
    const std::weak_ptr<Label> weak_status = status;
    (*context).subscriptions.push_back((*immediate).item_check_state_changed().subscribe(
        *status, CheckedItemStatus(weak_status)));
    (*context).subscriptions.push_back((*check_all).clicked().subscribe(
        *immediate, CheckedListCommandClick(
            weak_immediate, CheckedListCommand::check_all)));
    (*context).subscriptions.push_back((*clear).clicked().subscribe(
        *immediate, CheckedListCommandClick(
            weak_immediate, CheckedListCommand::clear_all)));
    (*context).subscriptions.push_back((*mixed).clicked().subscribe(
        *immediate, CheckedListCommandClick(
            weak_immediate, CheckedListCommand::set_mixed)));
}

} // namespace gui_forms::gallery::pages
