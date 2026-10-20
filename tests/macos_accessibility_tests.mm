#import <AppKit/AppKit.h>

#include "macos_host.hpp"

#include "gui_forms/gui_forms.hpp"
#include "support/stock_control_fixture.hpp"

#include <cstdint>
#include <cstdio>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

// Stage A, macOS in-process regression test. The stock-control fixture runs
// on a real AppKit window; the test walks the window's NSAccessibility tree
// through the NSAccessibility protocol, compares every element with the
// semantic node it projects, performs each control's default action through
// its element and observes the change, and checks that an element held past
// its control's removal refuses actions. It runs in process, so it needs no
// Accessibility permission (CI); the out-of-process AXUIElement client is
// macos_ax_client_tests.

namespace {

using gui_forms::SemanticAction;
using gui_forms::SemanticNode;
using gui_forms::SemanticRole;
using gui_forms::SemanticSnapshot;
using gui_forms::SemanticState;
using gui_forms::has_semantic_state;
using gui_forms::test_support::StockControlEntry;
using gui_forms::test_support::StockControlFixture;

constexpr const char* fixture_title = "GUI.Forms accessibility fixture";

// ---------------------------------------------------------------------------
// The projection contract: what a semantic node must publish through
// NSAccessibility. Written here, apart from the host, so a host change that
// alters the projection fails this test.

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

bool has_action(const SemanticNode& node, SemanticAction action) {
    for (const SemanticAction published : node.actions) {
        if (published == action) return true;
    }
    return false;
}

std::string number_text(double value) {
    char buffer[64] = {};
    std::snprintf(buffer, sizeof(buffer), "%g", value);
    return std::string(buffer);
}

// The value an element publishes, in one text form: a check box, a check
// list item and a radio button publish 0, 1 or 2 (mixed); a tab publishes 1
// when selected and 0 otherwise, as AppKit's tab buttons do; a node with a
// numeric value publishes the number; any other node its value text.
std::string expected_value(const SemanticNode& node) {
    if (node.role == SemanticRole::check_box ||
        node.role == SemanticRole::check_list_item ||
        node.role == SemanticRole::radio_button) {
        if (has_semantic_state(node.states, SemanticState::mixed)) return "2";
        return has_semantic_state(node.states, SemanticState::checked) ? "1" : "0";
    }
    if (node.role == SemanticRole::tab) {
        return has_semantic_state(node.states, SemanticState::selected) ? "1" : "0";
    }
    if (node.numeric_value) return number_text(*node.numeric_value);
    return node.value;
}

struct Projection final {
    std::string role;
    std::string subrole;
    std::string label;
    std::string identifier;
    std::string value;
    bool enabled{};
    bool focused{};
    bool selected{};
    bool expanded{};
    bool press{};
    bool show_menu{};
    bool increment{};
    bool decrement{};
    bool focus_settable{};
    bool value_settable{};
    bool selected_settable{};
    bool close_action{};
};

Projection expected_projection(const SemanticNode& node) {
    Projection expected;
    expected.role = expected_ax_role(node.role);
    expected.subrole = expected_ax_subrole(node.role);
    expected.label = node.name;
    expected.identifier = node.stable_id;
    expected.value = expected_value(node);
    expected.enabled = has_semantic_state(node.states, SemanticState::enabled);
    expected.focused = has_semantic_state(node.states, SemanticState::focused);
    expected.selected = has_semantic_state(node.states, SemanticState::selected);
    expected.expanded = has_semantic_state(node.states, SemanticState::expanded);
    expected.press = has_action(node, SemanticAction::press) ||
                     has_action(node, SemanticAction::select) ||
                     has_action(node, SemanticAction::expand) ||
                     has_action(node, SemanticAction::collapse);
    expected.show_menu = has_action(node, SemanticAction::show_menu) ||
                         has_action(node, SemanticAction::expand);
    expected.increment = has_action(node, SemanticAction::increment);
    expected.decrement = has_action(node, SemanticAction::decrement);
    expected.focus_settable = has_action(node, SemanticAction::focus);
    expected.value_settable = has_action(node, SemanticAction::set_value);
    expected.selected_settable = has_action(node, SemanticAction::select);
    expected.close_action = has_action(node, SemanticAction::close);
    return expected;
}

// ---------------------------------------------------------------------------
// What an element publishes, read through the NSAccessibility protocol.

std::string utf8(NSString* text) {
    if (text == nil) return {};
    const char* bytes = text.UTF8String;
    return bytes == nullptr ? std::string{} : std::string(bytes);
}

std::string value_text(id value) {
    if (value == nil) return {};
    if ([value isKindOfClass:[NSNumber class]]) {
        NSNumber* number = (NSNumber*)value;
        return number_text(number.doubleValue);
    }
    if ([value isKindOfClass:[NSString class]]) return utf8((NSString*)value);
    return utf8([value description]);
}

bool selector_allowed(id element, SEL selector) {
    if (![element respondsToSelector:@selector(isAccessibilitySelectorAllowed:)]) return NO;
    return [element isAccessibilitySelectorAllowed:selector];
}

bool has_custom_action(id element, NSString* name) {
    NSArray<NSAccessibilityCustomAction*>* actions = [element accessibilityCustomActions];
    for (NSAccessibilityCustomAction* action in actions) {
        if ([action.name isEqualToString:name]) return true;
    }
    return false;
}

Projection published_projection(id element) {
    Projection published;
    published.role = utf8([element accessibilityRole]);
    published.subrole = utf8([element accessibilitySubrole]);
    published.label = utf8([element accessibilityLabel]);
    published.identifier = utf8([element accessibilityIdentifier]);
    published.value = value_text([element accessibilityValue]);
    published.enabled = [element isAccessibilityEnabled];
    published.focused = [element isAccessibilityFocused];
    published.selected = [element isAccessibilitySelected];
    published.expanded = [element isAccessibilityExpanded];
    published.press = selector_allowed(element, @selector(accessibilityPerformPress));
    published.show_menu = selector_allowed(element, @selector(accessibilityPerformShowMenu));
    published.increment = selector_allowed(element, @selector(accessibilityPerformIncrement));
    published.decrement = selector_allowed(element, @selector(accessibilityPerformDecrement));
    published.focus_settable = selector_allowed(element, @selector(setAccessibilityFocused:));
    published.value_settable = selector_allowed(element, @selector(setAccessibilityValue:));
    published.selected_settable = selector_allowed(element, @selector(setAccessibilitySelected:));
    published.close_action = has_custom_action(element, @"Close");
    return published;
}

// ---------------------------------------------------------------------------
// Comparison and the per-control report.

struct Mismatch final {
    std::string phase;
    std::string owner;
    std::string stable_id;
    std::string field;
    std::string expected;
    std::string published;
};

struct ControlTally final {
    std::string manifest_id;
    std::size_t nodes{};
    std::size_t matched{};
    std::string default_action;
    bool action_observed{};
};

struct Report final {
    std::vector<ControlTally> controls;
    std::vector<Mismatch> mismatches;
    std::vector<std::string> failures;
};

ControlTally& tally_for(Report& report, std::string_view manifest_id) {
    for (ControlTally& tally : report.controls) {
        if (tally.manifest_id == manifest_id) return tally;
    }
    report.controls.push_back(ControlTally{std::string(manifest_id)});
    return report.controls.back();
}

bool is_entry(const StockControlFixture& fixture, std::string_view stable_id) {
    return gui_forms::test_support::find_stock_control(fixture, stable_id) != nullptr;
}

class TreeComparison final {
public:
    TreeComparison(const StockControlFixture& fixture, Report& report, std::string phase)
        : fixture_(fixture), report_(report), phase_(std::move(phase)) {}

    void compare(const SemanticNode& node, id element, const std::string& inherited_owner) {
        std::string owner = inherited_owner;
        if (is_entry(fixture_, node.stable_id)) {
            owner = node.stable_id;
        } else if (owner.empty()) {
            owner = owner_by_prefix(node.stable_id);
        }
        const bool attributed = !owner.empty();
        bool node_matches = true;
        if (element == nil) {
            add(owner, node.stable_id, "element", "present", "missing");
            node_matches = false;
        } else {
            const Projection expected = expected_projection(node);
            const Projection published = published_projection(element);
            node_matches = compare_projection(owner, node.stable_id, expected, published);
        }
        NSArray* children = element == nil ? @[] : [element accessibilityChildren];
        const std::size_t published_count = children == nil ? 0U : children.count;
        if (published_count != node.children.size()) {
            add(owner, node.stable_id, "children", std::to_string(node.children.size()),
                std::to_string(published_count));
            node_matches = false;
        }
        if (attributed) {
            ControlTally& tally = tally_for(report_, owner);
            ++tally.nodes;
            if (node_matches) ++tally.matched;
        }
        for (std::size_t index = 0U; index < node.children.size(); ++index) {
            id child = index < published_count ? children[index] : nil;
            compare(node.children[index], child, owner);
        }
    }

private:
    // The placed control whose stable id prefixes this one ("list_box" for
    // "list_box.item.0"), for a node whose control publishes no node of its
    // own; the longest such prefix, or empty.
    std::string owner_by_prefix(const std::string& stable_id) const {
        std::string owner;
        for (const StockControlEntry& entry : fixture_.controls) {
            const std::string prefix = entry.manifest_id + ".";
            if (stable_id.compare(0U, prefix.size(), prefix) == 0 && entry.manifest_id.size() > owner.size()) {
                owner = entry.manifest_id;
            }
        }
        return owner;
    }

    void add(const std::string& owner, const std::string& stable_id, const char* field,
             const std::string& expected, const std::string& published) {
        report_.mismatches.push_back(Mismatch{phase_, owner, stable_id, field, expected, published});
    }

    bool compare_text(const std::string& owner, const std::string& stable_id, const char* field,
                      const std::string& expected, const std::string& published) {
        if (expected == published) return true;
        add(owner, stable_id, field, expected, published);
        return false;
    }

    bool compare_flag(const std::string& owner, const std::string& stable_id, const char* field,
                      bool expected, bool published) {
        if (expected == published) return true;
        add(owner, stable_id, field, expected ? "true" : "false", published ? "true" : "false");
        return false;
    }

    bool compare_projection(const std::string& owner, const std::string& id,
                            const Projection& expected, const Projection& published) {
        bool same = true;
        same = compare_text(owner, id, "role", expected.role, published.role) && same;
        same = compare_text(owner, id, "subrole", expected.subrole, published.subrole) && same;
        same = compare_text(owner, id, "label", expected.label, published.label) && same;
        same = compare_text(owner, id, "identifier", expected.identifier, published.identifier) && same;
        same = compare_text(owner, id, "value", expected.value, published.value) && same;
        same = compare_flag(owner, id, "enabled", expected.enabled, published.enabled) && same;
        same = compare_flag(owner, id, "focused", expected.focused, published.focused) && same;
        same = compare_flag(owner, id, "selected", expected.selected, published.selected) && same;
        same = compare_flag(owner, id, "expanded", expected.expanded, published.expanded) && same;
        same = compare_flag(owner, id, "AXPress", expected.press, published.press) && same;
        same = compare_flag(owner, id, "AXShowMenu", expected.show_menu, published.show_menu) && same;
        same = compare_flag(owner, id, "AXIncrement", expected.increment, published.increment) && same;
        same = compare_flag(owner, id, "AXDecrement", expected.decrement, published.decrement) && same;
        same = compare_flag(owner, id, "AXFocused settable", expected.focus_settable, published.focus_settable) && same;
        same = compare_flag(owner, id, "AXValue settable", expected.value_settable, published.value_settable) && same;
        same = compare_flag(owner, id, "AXSelected settable", expected.selected_settable, published.selected_settable) && same;
        same = compare_flag(owner, id, "Close action", expected.close_action, published.close_action) && same;
        return same;
    }

    const StockControlFixture& fixture_;
    Report& report_;
    std::string phase_;
};

void compare_window(const StockControlFixture& fixture, gui_forms::Window& window, NSView* view,
                    Report& report, const char* phase) {
    const SemanticSnapshot snapshot = window.semantic_snapshot();
    NSArray* roots = [view accessibilityChildren];
    const std::size_t published_count = roots == nil ? 0U : roots.count;
    if (published_count != snapshot.roots.size()) {
        report.mismatches.push_back(Mismatch{phase, "", "(window)", "roots",
                                             std::to_string(snapshot.roots.size()),
                                             std::to_string(published_count)});
    }
    TreeComparison comparison(fixture, report, phase);
    for (std::size_t index = 0U; index < snapshot.roots.size(); ++index) {
        id element = index < published_count ? roots[index] : nil;
        comparison.compare(snapshot.roots[index], element, "");
    }
}

// ---------------------------------------------------------------------------
// Default actions.

// Returns a borrow into `nodes`; a temporary list is refused.
const SemanticNode* find_node(std::vector<SemanticNode>&& nodes, std::string_view stable_id) = delete;
const SemanticNode* find_node(const std::vector<SemanticNode>& nodes, std::string_view stable_id) {
    for (const SemanticNode& node : nodes) {
        if (node.stable_id == stable_id) return &node;
        const SemanticNode* found = find_node(node.children, stable_id);
        if (found != nullptr) return found;
    }
    return nullptr;
}

id find_element(NSArray* elements, NSString* identifier) {
    for (id element in elements) {
        if ([[element accessibilityIdentifier] isEqualToString:identifier]) return element;
        id found = find_element([element accessibilityChildren], identifier);
        if (found != nil) return found;
    }
    return nil;
}

// The order in which a node's actions are taken as its default action;
// focus alone is not a default action.
constexpr SemanticAction default_action_order[] = {
    SemanticAction::press, SemanticAction::select, SemanticAction::increment,
    SemanticAction::set_value, SemanticAction::show_menu, SemanticAction::expand,
    SemanticAction::collapse,
};

// Selecting an item that is already selected changes nothing, so such an
// item does not stand for its control's default action.
bool default_action_of(const SemanticNode& node, SemanticAction& action) {
    if (!has_semantic_state(node.states, SemanticState::enabled)) return false;
    // An increment at the maximum (an empty range) changes nothing either.
    const bool at_maximum = node.numeric_value && node.maximum_value &&
        *node.numeric_value >= *node.maximum_value;
    for (const SemanticAction candidate : default_action_order) {
        const bool already_selected = candidate == SemanticAction::select &&
            has_semantic_state(node.states, SemanticState::selected);
        const bool cannot_grow = candidate == SemanticAction::increment && at_maximum;
        if (has_action(node, candidate) && !already_selected && !cannot_grow) {
            action = candidate;
            return true;
        }
    }
    return false;
}

// The node whose default action stands for the control: the control's own
// node when it has one, else its first descendant that has one.
const SemanticNode* default_action_node(const SemanticNode& control, SemanticAction& action) {
    if (default_action_of(control, action)) return &control;
    for (const SemanticNode& child : control.children) {
        const SemanticNode* found = default_action_node(child, action);
        if (found != nullptr) return found;
    }
    return nullptr;
}

bool is_iso_date(const std::string& text) {
    if (text.size() != 10U || text[4] != '-' || text[7] != '-') return false;
    for (std::size_t index = 0U; index < text.size(); ++index) {
        if (index == 4U || index == 7U) continue;
        if (text[index] < '0' || text[index] > '9') return false;
    }
    return true;
}

bool has_letter(const std::string& text) {
    for (const char character : text) {
        if ((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z')) return true;
    }
    return false;
}

// A value the node accepts and that differs from its current one: the
// maximum (or one more) for a number, an ISO date for a calendar or date
// picker (whose published value is the long form, but which set an ISO
// date), another day for an ISO date, the text with a suffix for text, and
// digits for anything else (a masked field).
std::string set_value_text(const SemanticNode& node) {
    if (node.role == SemanticRole::calendar || node.role == SemanticRole::date_picker) {
        return node.value.find("15") == std::string::npos ? "2026-10-15" : "2026-10-16";
    }
    if (node.numeric_value) {
        const double target = node.maximum_value ? *node.maximum_value : *node.numeric_value + 1.0;
        return number_text(target);
    }
    if (is_iso_date(node.value)) {
        std::string next = node.value;
        next[8] = next[8] == '1' ? '2' : '1';
        next[9] = '5';
        return next;
    }
    if (has_letter(node.value)) return node.value + " (changed)";
    return "12345";
}

bool perform_through_element(id element, SemanticAction action, const SemanticNode& node) {
    switch (action) {
    case SemanticAction::press:
    case SemanticAction::expand:
    case SemanticAction::collapse:
        return [element accessibilityPerformPress];
    case SemanticAction::select:
        [element setAccessibilitySelected:YES];
        return true;
    case SemanticAction::increment:
        return [element accessibilityPerformIncrement];
    case SemanticAction::set_value: {
        NSString* value = [NSString stringWithUTF8String:set_value_text(node).c_str()];
        [element setAccessibilityValue:value];
        return true;
    }
    case SemanticAction::show_menu:
        return [element accessibilityPerformShowMenu];
    case SemanticAction::focus:
    case SemanticAction::decrement:
    case SemanticAction::close:
    case SemanticAction::count:
        return false;
    }
    return false;
}

// The published state of a subtree in one string: each node's identity,
// states, value and children in order. A default action shows its effect as
// a change here or in the fixture's effect counter.
void append_signature(const SemanticNode& node, std::string& signature) {
    signature += node.stable_id;
    signature += '|';
    signature += std::to_string(static_cast<std::uint32_t>(node.states));
    signature += '|';
    signature += node.value;
    signature += '|';
    if (node.numeric_value) signature += number_text(*node.numeric_value);
    signature += '{';
    for (const SemanticNode& child : node.children) append_signature(child, signature);
    signature += '}';
}

std::string subtree_signature(const std::vector<SemanticNode>& roots, std::string_view stable_id) {
    std::string signature;
    const SemanticNode* node = find_node(roots, stable_id);
    if (node != nullptr) append_signature(*node, signature);
    return signature;
}

bool same_roots(const SemanticSnapshot& left, const SemanticSnapshot& right) {
    if (left.roots.size() != right.roots.size()) return false;
    for (std::size_t index = 0U; index < left.roots.size(); ++index) {
        if (left.roots[index].stable_id != right.roots[index].stable_id) return false;
    }
    return true;
}

// Puts the window back after a default action that opened something: a node
// left expanded is collapsed through the model, and an open popup, which
// replaces the window's semantic roots, is closed with Escape (at most three
// presses).
void restore_after_action(gui_forms::Window& window, const std::string& target_id,
                          const SemanticSnapshot& before) {
    const SemanticSnapshot after = window.semantic_snapshot();
    const SemanticNode* target = find_node(after.roots, target_id);
    if (target != nullptr && has_semantic_state((*target).states, SemanticState::expanded) &&
        has_action(*target, SemanticAction::collapse)) {
        static_cast<void>(window.perform_semantic_action(target_id, SemanticAction::collapse, ""));
    }
    for (int attempt = 0; attempt < 3; ++attempt) {
        const SemanticSnapshot current = window.semantic_snapshot();
        if (same_roots(current, before)) return;
        gui_forms::KeyEvent escape{};
        escape.action = gui_forms::KeyAction::down;
        escape.physical_key = gui_forms::PhysicalKey::escape;
        static_cast<void>(window.dispatch_key(escape));
    }
}

void perform_default_actions(StockControlFixture& fixture, gui_forms::Window& window, NSView* view,
                             Report& report) {
    for (const StockControlEntry& entry : fixture.controls) {
        const SemanticSnapshot before = window.semantic_snapshot();
        const SemanticNode* control_node = find_node(before.roots, entry.manifest_id);
        if (control_node == nullptr) continue;
        SemanticAction action = SemanticAction::focus;
        const SemanticNode* target = default_action_node(*control_node, action);
        ControlTally& tally = tally_for(report, entry.manifest_id);
        if (target == nullptr) {
            tally.default_action = "none";
            continue;
        }
        const SemanticNode target_before = *target;
        tally.default_action = gui_forms::semantic_action_name(action);
        NSString* identifier = [NSString stringWithUTF8String:target_before.stable_id.c_str()];
        id element = find_element([view accessibilityChildren], identifier);
        if (element == nil) {
            report.failures.push_back(entry.manifest_id + ": no element for " + target_before.stable_id);
            continue;
        }
        const std::uint64_t effects_before = (*fixture.effects).total();
        const std::string signature_before = subtree_signature(before.roots, entry.manifest_id);
        const bool performed = perform_through_element(element, action, target_before);
        const SemanticSnapshot after = window.semantic_snapshot();
        const bool effect = (*fixture.effects).total() > effects_before;
        const bool changed = subtree_signature(after.roots, entry.manifest_id) != signature_before;
        tally.action_observed = performed && (effect || changed);
        restore_after_action(window, target_before.stable_id, before);
        if (!tally.action_observed) {
            report.failures.push_back(entry.manifest_id + ": default action " + tally.default_action +
                                      " on " + target_before.stable_id + " showed no effect");
        }
    }
}

// ---------------------------------------------------------------------------
// A stale element: one kept past its control's removal from the window.

void check_stale_element(StockControlFixture& fixture, NSView* view, Report& report) {
    const StockControlEntry* entry = gui_forms::test_support::find_stock_control(fixture, "button");
    if (entry == nullptr) {
        report.failures.push_back("stale element: the fixture has no button");
        return;
    }
    id stale = find_element([view accessibilityChildren], @"button");
    if (stale == nil) {
        report.failures.push_back("stale element: no element for the button");
        return;
    }
    const std::uint64_t presses_before = (*fixture.effects).presses("button");
    const gui_forms::Control::Ptr removed = (*fixture.root).remove_child((*(*entry).control).runtime_id());
    if (!removed) {
        report.failures.push_back("stale element: the button could not be removed");
        return;
    }
    id replaced = find_element([view accessibilityChildren], @"button");
    if (replaced != nil) report.failures.push_back("stale element: the removed button is still published");
    const BOOL pressed = [stale accessibilityPerformPress];
    if (pressed || (*fixture.effects).presses("button") != presses_before) {
        report.failures.push_back("stale element: a press on the removed button's element was performed");
    }
    static_cast<void>([stale accessibilityRole]);
}

// ---------------------------------------------------------------------------

void print_report(const StockControlFixture& fixture, const Report& report) {
    for (const StockControlEntry& entry : fixture.controls) {
        bool tallied = false;
        for (const ControlTally& tally : report.controls) {
            if (tally.manifest_id == entry.manifest_id) tallied = true;
        }
        if (!tallied) std::cout << "control " << entry.manifest_id << " nodes=0 (publishes no semantic node)\n";
    }
    for (const gui_forms::test_support::StockControlOmission& omission : fixture.omissions) {
        std::cout << "omitted " << omission.manifest_id << ": " << omission.reason << '\n';
    }
    std::size_t nodes = 0U;
    std::size_t matched = 0U;
    for (const ControlTally& tally : report.controls) {
        nodes += tally.nodes;
        matched += tally.matched;
        std::cout << "control " << tally.manifest_id << " nodes=" << tally.nodes
                  << " matched=" << tally.matched << " default_action=" << tally.default_action
                  << " observed=" << (tally.action_observed ? "yes" : "no") << '\n';
    }
    for (const Mismatch& mismatch : report.mismatches) {
        std::cout << "mismatch phase=" << mismatch.phase << " control=" << mismatch.owner
                  << " node=" << mismatch.stable_id << " field=" << mismatch.field
                  << " expected=\"" << mismatch.expected << "\" published=\""
                  << mismatch.published << "\"\n";
    }
    for (const std::string& failure : report.failures) std::cout << "failure " << failure << '\n';
    std::cout << "controls=" << report.controls.size() << " nodes=" << nodes << " matched=" << matched
              << " mismatches=" << report.mismatches.size() << " failures=" << report.failures.size()
              << '\n';
}

NSView* fixture_view() {
    NSString* title = [NSString stringWithUTF8String:fixture_title];
    for (NSWindow* window in NSApp.windows) {
        if ([window.title isEqualToString:title]) return window.contentView;
    }
    return nil;
}

// The run's state, owned by main for the whole run: the walk reads the
// fixture and the model, and closes the window when it is done.
struct RunState final {
    StockControlFixture* fixture{};
    gui_forms::Window* window{};
    std::function<void()> request_close;
    // Where to write the window's semantic snapshot (JSON) before the
    // actions, or empty; set by --dump-semantics <path>.
    std::string dump_path;
    Report report;
    bool ran{};
};

// Runs on the main queue once the window is up: the whole check, then close.
void walk_and_close(void* context) {
    RunState& state = *static_cast<RunState*>(context);
    gui_forms::Window& window = *state.window;
    window.perform_layout();
    NSView* view = fixture_view();
    if (view == nil) {
        state.report.failures.push_back("the fixture window has no content view");
    } else {
        if (!state.dump_path.empty()) {
            const std::string json = window.semantic_snapshot().to_json();
            std::FILE* file = std::fopen(state.dump_path.c_str(), "w");
            if (file != nullptr) {
                std::fwrite(json.data(), 1U, json.size(), file);
                std::fclose(file);
            }
        }
        compare_window(*state.fixture, window, view, state.report, "initial");
        perform_default_actions(*state.fixture, window, view, state.report);
        window.perform_layout();
        compare_window(*state.fixture, window, view, state.report, "after-actions");
        check_stale_element(*state.fixture, view, state.report);
    }
    state.ran = true;
    state.request_close();
}

using VoidCallback = std::function<void()>;
using ShowDialogCallback = std::function<gui_forms::HostDialogResult(const gui_forms::HostDialogRequest&)>;
using ShowTooltipCallback = std::function<gui_forms::HostServiceStatus(const gui_forms::HostTooltipRequest&)>;
using ReadClipboardCallback = std::function<gui_forms::HostClipboardTextResult()>;
using WriteClipboardCallback = std::function<gui_forms::HostServiceStatus(std::string_view)>;

class ScheduleWalk final {
public:
    explicit ScheduleWalk(RunState& state) noexcept : state_(state) {}

    void operator()(VoidCallback, VoidCallback request_close, ShowDialogCallback, ShowTooltipCallback,
                    VoidCallback, ReadClipboardCallback, WriteClipboardCallback) const {
        state_.request_close = std::move(request_close);
        const dispatch_time_t when = dispatch_time(DISPATCH_TIME_NOW, 300 * NSEC_PER_MSEC);
        dispatch_after_f(when, dispatch_get_main_queue(), &state_, &walk_and_close);
    }

private:
    RunState& state_;
};

} // namespace

int main(int argc, char** argv) {
    try {
        StockControlFixture fixture = gui_forms::test_support::make_stock_control_fixture();
        std::unique_ptr<gui_forms::Window> window = std::make_unique<gui_forms::Window>(
            fixture.root, gui_forms::test_support::stock_control_fixture_size);
        RunState state;
        state.fixture = &fixture;
        state.window = window.get();
        for (int index = 1; index + 1 < argc; ++index) {
            if (std::string_view(argv[index]) == "--dump-semantics") state.dump_path = argv[index + 1];
        }
        gui_forms::host::MacHostOptions options;
        options.title = fixture_title;
        options.initial_size = gui_forms::test_support::stock_control_fixture_size;
        options.print_metrics_on_close = false;
        options.host_ready = ScheduleWalk(state);
        const int result = gui_forms::host::run_macos(std::move(window), std::move(options));
        if (!state.ran) {
            std::cerr << "the fixture window never became ready (host result " << result << ")\n";
            return 2;
        }
        print_report(fixture, state.report);
        const bool clean = state.report.mismatches.empty() && state.report.failures.empty();
        return clean && result == 0 ? 0 : 1;
    } catch (const std::exception& error) {
        std::cerr << "macos accessibility test failed: " << error.what() << '\n';
        return 1;
    }
}
