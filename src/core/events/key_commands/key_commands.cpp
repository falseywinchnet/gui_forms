#include "gui_forms/events/key_commands/key_commands.hpp"

#include <stdexcept>

namespace gui_forms {
namespace {

constexpr std::array<std::string_view, key_command_count> key_command_names{{
    "close_document",
    "next_tab",
    "previous_tab",
    "move_tab_later",
    "move_tab_earlier",
    "cycle_forward",
    "cycle_backward",
    "context_menu",
    "next_document",
    "previous_document",
    "next_tool_panel",
    "previous_tool_panel",
    "position_menu",
    "return_to_document",
    "hide_tool_panel",
    "maximize_document_area",
}};

static_assert(key_command_count == 16U, "every KeyCommand has a name");

constexpr std::uint8_t modifier_bits = static_cast<std::uint8_t>(
    Modifier::shift | Modifier::control | Modifier::alt | Modifier::meta);

[[nodiscard]] bool command_valid(KeyCommand command) noexcept {
    const std::size_t index = static_cast<std::size_t>(command);
    const bool valid = index < key_command_count;
    return valid;
}

[[nodiscard]] bool chord_valid(KeyChord chord) noexcept {
    const std::uint8_t bits = static_cast<std::uint8_t>(chord.modifiers);
    if ((bits & static_cast<std::uint8_t>(~modifier_bits)) != 0U) return false;
    if (chord.empty() && chord.modifiers != Modifier::none) return false;
    return true;
}

[[nodiscard]] bool chord_matches(KeyChord chord, const KeyEvent& event) noexcept {
    if (chord.empty()) return false;
    const bool matched = event.physical_key == chord.physical_key && event.modifiers == chord.modifiers;
    return matched;
}

} // namespace

std::string_view key_command_name(KeyCommand command) noexcept {
    if (!command_valid(command)) return "unknown";
    const std::string_view name = key_command_names[static_cast<std::size_t>(command)];
    return name;
}

bool KeyBinding::matches(const KeyEvent& event) const noexcept {
    if (event.action != KeyAction::down) return false;
    const bool matched = chord_matches(primary, event) || chord_matches(secondary, event);
    return matched;
}

const KeyBinding& KeyMap::binding(KeyCommand command) const {
    if (!command_valid(command)) throw std::invalid_argument("KeyMap command is outside KeyCommand");
    return bindings_[static_cast<std::size_t>(command)];
}

void KeyMap::set_binding(KeyCommand command, KeyBinding binding) {
    if (!command_valid(command)) throw std::invalid_argument("KeyMap command is outside KeyCommand");
    if (!chord_valid(binding.primary) || !chord_valid(binding.secondary)) {
        throw std::invalid_argument("KeyMap chord has unknown modifiers or modifiers without a key");
    }
    bindings_[static_cast<std::size_t>(command)] = binding;
}

bool KeyMap::matches(KeyCommand command, const KeyEvent& event) const noexcept {
    if (!command_valid(command)) return false;
    const bool matched = bindings_[static_cast<std::size_t>(command)].matches(event);
    return matched;
}

KeyMap pc_key_map() {
    const Modifier control = Modifier::control;
    const Modifier control_shift = Modifier::control | Modifier::shift;
    const Modifier alt_shift = Modifier::alt | Modifier::shift;
    KeyMap map;
    map.set_binding(KeyCommand::close_document, {{PhysicalKey::f4, control}, {}});
    map.set_binding(KeyCommand::next_tab, {{PhysicalKey::page_down, control}, {}});
    map.set_binding(KeyCommand::previous_tab, {{PhysicalKey::page_up, control}, {}});
    map.set_binding(KeyCommand::move_tab_later, {{PhysicalKey::page_down, control_shift}, {}});
    map.set_binding(KeyCommand::move_tab_earlier, {{PhysicalKey::page_up, control_shift}, {}});
    map.set_binding(KeyCommand::cycle_forward, {{PhysicalKey::tab, control}, {}});
    map.set_binding(KeyCommand::cycle_backward, {{PhysicalKey::tab, control_shift}, {}});
    map.set_binding(KeyCommand::context_menu,
                    {{PhysicalKey::f10, Modifier::shift}, {PhysicalKey::context_menu, Modifier::none}});
    map.set_binding(KeyCommand::next_document, {{PhysicalKey::f6, control}, {}});
    map.set_binding(KeyCommand::previous_document, {{PhysicalKey::f6, control_shift}, {}});
    map.set_binding(KeyCommand::next_tool_panel, {{PhysicalKey::f7, Modifier::alt}, {}});
    map.set_binding(KeyCommand::previous_tool_panel, {{PhysicalKey::f7, alt_shift}, {}});
    map.set_binding(KeyCommand::position_menu, {{PhysicalKey::minus, Modifier::alt}, {}});
    map.set_binding(KeyCommand::return_to_document, {{PhysicalKey::escape, Modifier::none}, {}});
    map.set_binding(KeyCommand::hide_tool_panel, {{PhysicalKey::escape, Modifier::shift}, {}});
    map.set_binding(KeyCommand::maximize_document_area, {{PhysicalKey::f12, control_shift}, {}});
    return map;
}

} // namespace gui_forms
