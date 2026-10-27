#pragma once
// The input settings a host reports (HostInputSettingsEvent) and the window
// keeps in PresentationSettings. Kept apart so that the host protocol's
// types can name them without the window's presentation types.
#include <cstdint>

namespace gui_forms {

// Where a setting the host reports came from.
enum class SettingOrigin : std::uint8_t {
    // GUI.Forms' stated default: the host has no such setting (a desktop
    // session that publishes none), or no host has reported yet.
    default_value,
    // The platform's own setting, read by the host.
    host,
};

// The double-click interval GUI.Forms states where the host has none, and
// the longest one admitted (Windows' own limit). Perceptual durations, in
// milliseconds.
inline constexpr std::uint32_t default_double_click_milliseconds = 400U;
inline constexpr std::uint32_t maximum_double_click_milliseconds = 5000U;

} // namespace gui_forms
