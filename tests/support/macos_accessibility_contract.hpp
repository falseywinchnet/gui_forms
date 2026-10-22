#pragma once

#include "accessibility_actions.hpp"

#include "gui_forms/gui_forms.hpp"

#include <string>
#include <string_view>

namespace gui_forms::test_support {

// What a semantic node must publish through macOS accessibility (stage A):
// the contract both macOS accessibility tests check, the in-process one
// through the NSAccessibility protocol and the out-of-process one through
// AXUIElement. It is written apart from the host, so a host change that
// alters the projection fails the tests. Roles, subroles and action names
// are the AX attribute and action strings.
struct AccessibilityProjection final {
    std::string role;
    std::string subrole;
    // AXDescription, which NSAccessibility's label sets.
    std::string label;
    std::string identifier;
    // The value in one text form (see expected_ax_value).
    std::string value;
    bool enabled{};
    bool focused{};
    bool selected{};
    bool expanded{};
    // AXPress, AXShowMenu, AXIncrement and AXDecrement.
    bool press{};
    bool show_menu{};
    bool increment{};
    bool decrement{};
    // Whether AXFocused, AXValue and AXSelected are settable.
    bool focus_settable{};
    bool value_settable{};
    bool selected_settable{};
    // A custom action named "Close".
    bool close_action{};
};

[[nodiscard]] const char* expected_ax_role(SemanticRole role);
// The subrole, or "" for none.
[[nodiscard]] const char* expected_ax_subrole(SemanticRole role);
// A check box, a check list item and a radio button publish 0, 1 or 2
// (mixed); a tab publishes 1 when selected and 0 otherwise, as AppKit's tab
// buttons do; a node with a numeric value publishes the number (in
// semantic_number_text's form); any other node its value text.
[[nodiscard]] std::string expected_ax_value(const SemanticNode& node);
[[nodiscard]] AccessibilityProjection expected_ax_projection(const SemanticNode& node);

} // namespace gui_forms::test_support
