#pragma once

#include "gui_forms/events/input_events/input_events.hpp"

#include <windows.h>

namespace gui_forms::host::detail {

// Preserve the current virtual-key-derived vocabulary. VK_OEM_2 is slash and
// VK_OEM_MINUS is hyphen on the tested US layout; this is not a
// layout-independent character shortcut translator or a scan-code-based
// physical-position normalization.
[[nodiscard]] inline std::uint32_t physical_key_from_virtual_key(WPARAM key) noexcept {
    if (key >= 'A' && key <= 'Z') {
        const WPARAM relative = key - 'A';
        const std::uint32_t index = static_cast<std::uint32_t>(relative);
        const std::uint32_t usage = 0x04U + index;
        return usage;
    }
    if (key >= '1' && key <= '9') {
        const WPARAM relative = key - '1';
        const std::uint32_t index = static_cast<std::uint32_t>(relative);
        const std::uint32_t usage = 0x1EU + index;
        return usage;
    }
    if (key == '0') { return 0x27U; }
    switch (key) {
    case VK_RETURN: return PhysicalKey::enter;
    case VK_ESCAPE: return PhysicalKey::escape;
    case VK_BACK: return PhysicalKey::backspace;
    case VK_TAB: return PhysicalKey::tab;
    case VK_SPACE: return PhysicalKey::space;
    case VK_OEM_2: return PhysicalKey::slash;
    case VK_INSERT: return PhysicalKey::insert;
    case VK_HOME: return PhysicalKey::home;
    case VK_PRIOR: return PhysicalKey::page_up;
    case VK_END: return PhysicalKey::end;
    case VK_NEXT: return PhysicalKey::page_down;
    case VK_DELETE: return PhysicalKey::delete_forward;
    // The docking keys (owner decision DK-30): F6 regions, Ctrl+F6, Alt+F7,
    // Shift+F10, Ctrl+Shift+F12, Alt+Hyphen, the context-menu key, and the
    // Ctrl release that commits the Ctrl+Tab navigator.
    case VK_OEM_MINUS: return PhysicalKey::minus;
    case VK_F1: return PhysicalKey::f1;
    case VK_F2: return PhysicalKey::f2;
    case VK_F3: return PhysicalKey::f3;
    case VK_F4: return PhysicalKey::f4;
    case VK_F5: return PhysicalKey::f5;
    case VK_F6: return PhysicalKey::f6;
    case VK_F7: return PhysicalKey::f7;
    case VK_F8: return PhysicalKey::f8;
    case VK_F9: return PhysicalKey::f9;
    case VK_F10: return PhysicalKey::f10;
    case VK_F11: return PhysicalKey::f11;
    case VK_F12: return PhysicalKey::f12;
    case VK_APPS: return PhysicalKey::context_menu;
    // WM_KEYDOWN reports the generic VK_CONTROL without its side.
    case VK_CONTROL: return PhysicalKey::left_control;
    case VK_LCONTROL: return PhysicalKey::left_control;
    case VK_RCONTROL: return PhysicalKey::right_control;
    case VK_RIGHT: return PhysicalKey::right;
    case VK_LEFT: return PhysicalKey::left;
    case VK_DOWN: return PhysicalKey::down;
    case VK_UP: return PhysicalKey::up;
    default: return 0;
    }
}

} // namespace gui_forms::host::detail
