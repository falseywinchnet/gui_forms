#include "macos_accessibility_contract.hpp"

namespace gui_forms::test_support {

const char* expected_ax_role(SemanticRole role) {
    switch (role) {
    case SemanticRole::static_text: return "AXStaticText";
    case SemanticRole::button: return "AXButton";
    case SemanticRole::check_box: return "AXCheckBox";
    case SemanticRole::radio_button: return "AXRadioButton";
    case SemanticRole::toolbar: return "AXToolbar";
    case SemanticRole::radio_group: return "AXRadioGroup";
    case SemanticRole::link: return "AXLink";
    case SemanticRole::text_box: return "AXTextField";
    case SemanticRole::numeric_field: return "AXTextField";
    case SemanticRole::list: return "AXList";
    case SemanticRole::list_item: return "AXRow";
    case SemanticRole::check_list_item: return "AXCheckBox";
    case SemanticRole::combo_box: return "AXComboBox";
    case SemanticRole::slider: return "AXSlider";
    case SemanticRole::scroll_bar: return "AXScrollBar";
    case SemanticRole::progress_bar: return "AXProgressIndicator";
    case SemanticRole::image: return "AXImage";
    case SemanticRole::tab_group: return "AXTabGroup";
    case SemanticRole::tab: return "AXRadioButton";
    case SemanticRole::split_pane: return "AXSplitGroup";
    case SemanticRole::tool_tip: return "AXStaticText";
    case SemanticRole::date_picker: return "AXComboBox";
    case SemanticRole::date_cell: return "AXButton";
    case SemanticRole::property_row: return "AXStaticText";
    case SemanticRole::menu_bar: return "AXMenuBar";
    case SemanticRole::menu_bar_item: return "AXMenuBarItem";
    case SemanticRole::menu: return "AXMenu";
    case SemanticRole::menu_item: return "AXMenuItem";
    case SemanticRole::split_button: return "AXButton";
    case SemanticRole::menu_button: return "AXMenuButton";
    case SemanticRole::generic:
    case SemanticRole::group:
    case SemanticRole::calendar:
    case SemanticRole::property_grid:
    case SemanticRole::property_group:
    case SemanticRole::separator:
    case SemanticRole::pane:
    case SemanticRole::tab_panel:
    case SemanticRole::count:
        return "AXGroup";
    }
    return "AXGroup";
}

const char* expected_ax_subrole(SemanticRole role) {
    if (role == SemanticRole::tab_panel) return "AXTabPanel";
    if (role == SemanticRole::tab) return "AXTabButton";
    return "";
}

std::string expected_ax_value(const SemanticNode& node) {
    if (node.role == SemanticRole::check_box ||
        node.role == SemanticRole::check_list_item ||
        node.role == SemanticRole::radio_button) {
        if (has_semantic_state(node.states, SemanticState::mixed)) return "2";
        return has_semantic_state(node.states, SemanticState::checked) ? "1" : "0";
    }
    if (node.role == SemanticRole::tab) {
        return has_semantic_state(node.states, SemanticState::selected) ? "1" : "0";
    }
    if (node.numeric_value) return semantic_number_text(*node.numeric_value);
    return node.value;
}

AccessibilityProjection expected_ax_projection(const SemanticNode& node) {
    AccessibilityProjection expected;
    expected.role = expected_ax_role(node.role);
    expected.subrole = expected_ax_subrole(node.role);
    expected.label = node.name;
    expected.identifier = node.stable_id;
    expected.value = expected_ax_value(node);
    expected.enabled = has_semantic_state(node.states, SemanticState::enabled);
    expected.focused = has_semantic_state(node.states, SemanticState::focused);
    expected.selected = has_semantic_state(node.states, SemanticState::selected);
    expected.expanded = has_semantic_state(node.states, SemanticState::expanded);
    // AXPress performs press, else select, else collapse, else expand.
    expected.press = has_semantic_action(node, SemanticAction::press) ||
                     has_semantic_action(node, SemanticAction::select) ||
                     has_semantic_action(node, SemanticAction::expand) ||
                     has_semantic_action(node, SemanticAction::collapse);
    expected.show_menu = has_semantic_action(node, SemanticAction::show_menu) ||
                         has_semantic_action(node, SemanticAction::expand);
    expected.increment = has_semantic_action(node, SemanticAction::increment);
    expected.decrement = has_semantic_action(node, SemanticAction::decrement);
    expected.focus_settable = has_semantic_action(node, SemanticAction::focus);
    expected.value_settable = has_semantic_action(node, SemanticAction::set_value);
    expected.selected_settable = has_semantic_action(node, SemanticAction::select);
    expected.close_action = has_semantic_action(node, SemanticAction::close);
    return expected;
}

} // namespace gui_forms::test_support
