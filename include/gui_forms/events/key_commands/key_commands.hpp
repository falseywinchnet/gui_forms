#pragma once

#include "gui_forms/events/input_events/input_events.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace gui_forms {

// One key with the exact modifiers held: Ctrl+F4 is {PhysicalKey::f4,
// Modifier::control}. A chord whose key is 0 is no chord (an unused slot of
// a binding); it has no modifiers and matches no event.
struct KeyChord final {
    std::uint32_t physical_key{};
    Modifier modifiers{Modifier::none};

    [[nodiscard]] bool empty() const noexcept { return physical_key == 0U; }

    friend bool operator==(const KeyChord& left, const KeyChord& right) noexcept {
        const bool equal = left.physical_key == right.physical_key && left.modifiers == right.modifiers;
        return equal;
    }
    friend bool operator!=(const KeyChord& left, const KeyChord& right) noexcept {
        const bool different = !(left == right);
        return different;
    }
};

// The keyboard commands whose keys a host chooses and an application may
// rebind (owner decision DK-30, interview G5-1 and G6-1). A control asks its
// window's key map whether a key event is one of these commands instead of
// naming the keys itself; the keys of moving within a control (arrows, Home,
// End, Space, Enter, Escape ending a drag) stay the control's own.
//
// The defaults given here are the PC convention, as Windows and X11 desktops
// use it (pc_key_map). A host that follows another convention reports its own
// map in HostCapabilities::key_map: the macOS host binds close_document to
// Cmd+W and keeps the rest (interview G5-1).
enum class KeyCommand : std::uint8_t {
    // Ctrl+F4: close the selected document or closable page (DK-38).
    close_document,
    // Ctrl+PageDown and Ctrl+PageUp: select the next or previous tab, in
    // every view (DK-38).
    next_tab,
    previous_tab,
    // Ctrl+Shift+PageDown and Ctrl+Shift+PageUp: move the selected tab one
    // place later or earlier (DK-38).
    move_tab_later,
    move_tab_earlier,
    // Ctrl+Tab and Ctrl+Shift+Tab. The most local owner takes them (G1-2): a
    // TabControl with focus inside it, or a PageView whose Ctrl+Tab policy
    // selects; otherwise docking's navigator (stage DK, phase D2).
    cycle_forward,
    cycle_backward,
    // Shift+F10, or the context-menu key: the context menu of what has the
    // keyboard (a tab's, or a docked panel's position menu, G2-1).
    context_menu,
    // Docking's keys (G2-1, G7-1), which DockSite takes: Ctrl+F6 and Ctrl+Shift+F6
    // (next and previous document), Alt+F7 and Alt+Shift+F7 (next and
    // previous tool panel), Alt+Hyphen (the position menu), Escape in a tool
    // panel (to the active document), Shift+Escape (hide the active tool
    // panel), Ctrl+Shift+F12 (maximize the document area).
    next_document,
    previous_document,
    next_tool_panel,
    previous_tool_panel,
    position_menu,
    return_to_document,
    hide_tool_panel,
    maximize_document_area,
    count,
};

inline constexpr std::size_t key_command_count = static_cast<std::size_t>(KeyCommand::count);

// The registered (snake_case) name of a command, as the capability report
// lists it; "unknown" for a value outside the enumeration.
[[nodiscard]] std::string_view key_command_name(KeyCommand command) noexcept;

// The chords that invoke one command: at most two (Shift+F10 and the
// context-menu key). Either may be empty; a binding with both empty leaves
// the command without keys.
struct KeyBinding final {
    KeyChord primary{};
    KeyChord secondary{};

    [[nodiscard]] bool matches(const KeyEvent& event) const noexcept;

    friend bool operator==(const KeyBinding& left, const KeyBinding& right) noexcept {
        const bool equal = left.primary == right.primary && left.secondary == right.secondary;
        return equal;
    }
    friend bool operator!=(const KeyBinding& left, const KeyBinding& right) noexcept {
        const bool different = !(left == right);
        return different;
    }
};

// One binding per KeyCommand: a value, copied whole. A key event matches a
// command when it is a key-down (a repeat included) of one of the
// command's chords with exactly that chord's modifiers. Two commands may
// share a chord; the control asking decides which it means. A map made with
// no arguments leaves every command without keys.
class KeyMap final {
public:
    // Throws std::invalid_argument for a command outside the enumeration.
    [[nodiscard]] const KeyBinding& binding(KeyCommand command) const;
    // Throws std::invalid_argument for a command outside the enumeration, or
    // a chord with modifier bits outside shift, control, alt and meta, or an
    // empty chord with modifiers; the map is then unchanged.
    void set_binding(KeyCommand command, KeyBinding binding);
    // False for a command outside the enumeration.
    [[nodiscard]] bool matches(KeyCommand command, const KeyEvent& event) const noexcept;

    friend bool operator==(const KeyMap& left, const KeyMap& right) noexcept {
        const bool equal = left.bindings_ == right.bindings_;
        return equal;
    }
    friend bool operator!=(const KeyMap& left, const KeyMap& right) noexcept {
        const bool different = !(left == right);
        return different;
    }

private:
    std::array<KeyBinding, key_command_count> bindings_{};
};

// The PC convention's map (see KeyCommand): what the Windows, X11 and
// headless hosts report, and what a window uses until a host reports its
// own.
[[nodiscard]] KeyMap pc_key_map();

} // namespace gui_forms
