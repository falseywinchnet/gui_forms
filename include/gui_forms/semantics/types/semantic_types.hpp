#pragma once

#include "gui_forms/types.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace gui_forms {

// Semantic roles (design language 8.5). split_button (a button with a
// separate menu arrow) and menu_button (a button that opens a menu) were
// admitted with the ribbon plan (owner decision RB-22, 12.8); pane (a tool
// panel or another F6 region) and tab_panel (the content a tab shows) with
// the docking plan (owner decisions DK-35 and DK-40, 12.10). `count` is not
// a role; it sizes tables over the roles.
enum class SemanticRole : std::uint8_t {
    generic, group, static_text, button, check_box, radio_button, link,
    text_box, list, list_item, check_list_item, combo_box, slider,
    scroll_bar, progress_bar, image, tab_group, tab, split_pane,
    numeric_field, tool_tip, date_picker, calendar, date_cell, property_grid,
    property_group, property_row, menu_bar, menu_bar_item, menu, menu_item,
    separator, toolbar, radio_group, split_button, menu_button, pane,
    tab_panel,
    count,
};

inline constexpr std::size_t semantic_role_count = static_cast<std::size_t>(SemanticRole::count);

enum class SemanticState : std::uint32_t {
    none = 0,
    enabled = 1U << 0U,
    visible = 1U << 1U,
    focusable = 1U << 2U,
    focused = 1U << 3U,
    selected = 1U << 4U,
    checked = 1U << 5U,
    mixed = 1U << 6U,
    read_only = 1U << 7U,
    expanded = 1U << 8U,
    busy = 1U << 9U,
    protected_content = 1U << 10U,
    invalid = 1U << 11U,
};

[[nodiscard]] constexpr SemanticState operator|(
    SemanticState left, SemanticState right) noexcept {
    return static_cast<SemanticState>(static_cast<std::uint32_t>(left) |
                                      static_cast<std::uint32_t>(right));
}
constexpr SemanticState& operator|=(SemanticState& left,
                                    SemanticState right) noexcept {
    left = left | right;
    return left;
}
[[nodiscard]] constexpr bool has_semantic_state(
    SemanticState states, SemanticState requested) noexcept {
    return (static_cast<std::uint32_t>(states) &
            static_cast<std::uint32_t>(requested)) != 0U;
}

// Semantic actions. close (a document tab's or a panel's close, owner
// decision DK-35, 12.10) was admitted with the docking plan; tabs publish
// select and, where they can be closed, close, but not press. `count` is
// not an action; it sizes tables over the actions.
enum class SemanticAction : std::uint8_t {
    focus, press, select, increment, decrement, expand, collapse, show_menu,
    set_value, close,
    count,
};

inline constexpr std::size_t semantic_action_count = static_cast<std::size_t>(SemanticAction::count);

// Whether a tab list's tabs run in a row or a column (owner decision
// DK-40).
enum class SemanticOrientation : std::uint8_t { horizontal, vertical };

// The position of an element among its siblings of one set (a list's items,
// a strip's buttons) and the set's size, both counting from one, and whether
// the element opens a popup (a menu, a list or a dialog) when activated.
// These node fields were admitted with the ribbon plan (owner decision
// RB-22); a position is published with its set size or not at all.
//
// `controlled_panel` and `orientation` were admitted with the docking plan
// (owner decision DK-40). On a tab, `controlled_panel` is the stable
// identity of the tab_panel node the tab shows (the tab-to-panel
// relation), or empty; the panel need not be in the same snapshot, since a
// hidden page's panel is not. On a tab list (tab_group), `orientation` says
// whether its tabs run in a row or a column; other nodes leave it empty.
struct SemanticDescriptor final {
    SemanticRole role{SemanticRole::generic};
    std::string name;
    std::string value;
    std::string description;
    std::optional<double> numeric_value;
    std::optional<double> minimum_value;
    std::optional<double> maximum_value;
    std::optional<std::size_t> position_in_set;
    std::optional<std::size_t> set_size;
    bool has_popup{};
    std::string controlled_panel;
    std::optional<SemanticOrientation> orientation;
    SemanticState states{SemanticState::none};
    std::vector<SemanticAction> actions;
    bool exposed{};
    bool include_descendants{true};
};

struct SemanticNode final {
    std::uint64_t runtime_id{};
    std::string stable_id;
    SemanticRole role{SemanticRole::generic};
    std::string name;
    std::string value;
    std::string description;
    std::optional<double> numeric_value;
    std::optional<double> minimum_value;
    std::optional<double> maximum_value;
    std::optional<std::size_t> position_in_set;
    std::optional<std::size_t> set_size;
    bool has_popup{};
    std::string controlled_panel;
    std::optional<SemanticOrientation> orientation;
    Rect bounds{};
    SemanticState states{SemanticState::none};
    std::vector<SemanticAction> actions;
    std::vector<SemanticNode> children;
};

struct SemanticSnapshot final {
    std::uint64_t generation{};
    std::size_t node_count{};
    std::vector<SemanticNode> roots;
    [[nodiscard]] std::string to_json() const;
};

[[nodiscard]] const char* semantic_role_name(SemanticRole role) noexcept;
[[nodiscard]] const char* semantic_action_name(SemanticAction action) noexcept;
[[nodiscard]] const char* semantic_orientation_name(SemanticOrientation orientation) noexcept;

// The runtime ID of a virtual semantic child (a tab, an item, a caption
// button: a SemanticNode with no control of its own), from its stable ID: a
// 64-bit FNV-1a-style hash of the ID's bytes with the top bit set, so it
// never meets a control's own runtime ID (those count up from 1). The same
// stable ID always gives the same runtime ID, on every host.
// A virtual child the control publishes with runtime ID zero is given this
// one when the window assembles its snapshot, so no node below the window
// has runtime ID zero.
[[nodiscard]] std::uint64_t virtual_semantic_runtime_id(std::string_view stable_id) noexcept;

// True for a runtime ID virtual_semantic_runtime_id gives (its top bit is
// set), false for a control's own runtime ID and for zero.
[[nodiscard]] bool is_virtual_semantic_runtime_id(std::uint64_t runtime_id) noexcept;

} // namespace gui_forms
