#include <gui_forms/collection_controls.hpp>

namespace gf = gui_forms;

class Actions final : public gf::Component {
public:
    void command(const gf::CommandItem& item) { last_command = item.id; }
    void choice(const int index) { last_choice = index; }
    void section(const int index, const bool expanded) {
        last_section = index;
        section_open = expanded;
    }
    int last_command{-1};
    int last_choice{-1};
    int last_section{-1};
    bool section_open{};
};

int main() {
    Actions actions{};
    gf::Value<int> selection(0);
    gf::Value<bool> expanded(false);
    const std::shared_ptr<gf::CommandBar> commands =
        gf::make_control<gf::CommandBar>(gf::StableId("commands"));
    const std::shared_ptr<gf::ChoiceGroup> choices =
        gf::make_control<gf::ChoiceGroup>(gf::StableId("choices"));
    const std::shared_ptr<gf::ExpandableSections> sections =
        gf::make_control<gf::ExpandableSections>(gf::StableId("sections"));
    const gf::Control::Ptr help = gf::make_control<gf::Control>(gf::StableId("help.body"));
    (*commands).set_items({{.id = 1, .label = "Back"}, {.id = 2, .label = "Help"}});
    (*choices).set_items({{.id = 10, .label = "Classic"}, {.id = 20, .label = "Modern"}});
    (*choices).bind(selection);
    (*sections).set_items({{.id = 1, .label = "Help", .content = help, .expanded = &expanded}});
    gf::on((*commands).invoked(), actions, &Actions::command);
    gf::on((*choices).changed(), actions, &Actions::choice);
    gf::on((*sections).toggled(), actions, &Actions::section);
    (*commands).item(1).perform_click();
    selection.set(1);
    expanded.set(true);
    if (actions.last_command != 2 || actions.last_choice != 1 ||
        actions.last_section != 0 || !actions.section_open) return 1;
    // Replacing data preserves the three group subscriptions above.
    (*commands).set_items({{.id = 3, .label = "Close"}});
    (*commands).item(0).perform_click();
    if (actions.last_command != 3) return 1;
    return 0;
}
