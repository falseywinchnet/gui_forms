#pragma once

#include "gui_forms/c_api.h"

#include "gui_forms/basic_controls.hpp"
#include "gui_forms/control.hpp"
#include "gui_forms/inspection_controls.hpp"
#include "gui_forms/live_surface.hpp"
#include "gui_forms/range_controls.hpp"
#include "gui_forms/scrolling.hpp"
#include "gui_forms/text.hpp"
#include "gui_forms/window.hpp"
#include "headless_host.hpp"
#if defined(GF_C_API_HAS_WINDOWS_HOST)
#include "windows_host.hpp"
#endif
#if defined(GF_C_API_HAS_MACOS_HOST)
#include "macos_host.hpp"
#endif

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstring>
#include <deque>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

namespace gui_forms::abi::detail {

using gui_forms::ComponentState;
using gui_forms::Control;
using gui_forms::ImageResourceEncoding;
using gui_forms::Rect;
using gui_forms::Size;
using gui_forms::StableId;
using gui_forms::Window;
enum class FieldControlKind {
    text_box,
    combo_box,
    list_box,
    picture_box,
    data_grid,
    tool_strip,
    numeric_up_down,
};

struct RasterPointerSample final {
    std::uint32_t event_kind{};
    double x{};
    double y{};
    double wheel_delta{};
    std::uint32_t button{};
};

// The C ABI event kind for a pointer action.
[[nodiscard]] inline std::uint32_t pointer_event_kind(gui_forms::PointerAction action) noexcept {
    switch (action) {
    case gui_forms::PointerAction::down: return GF_EVENT_MOUSE_DOWN;
    case gui_forms::PointerAction::up: return GF_EVENT_MOUSE_UP;
    case gui_forms::PointerAction::wheel: return GF_EVENT_MOUSE_WHEEL;
    case gui_forms::PointerAction::enter: return GF_EVENT_MOUSE_ENTER;
    case gui_forms::PointerAction::leave: return GF_EVENT_MOUSE_LEAVE;
    case gui_forms::PointerAction::move: return GF_EVENT_MOUSE_MOVE;
    }
    // An action outside the enumeration is reported as a move.
    return GF_EVENT_MOUSE_MOVE;
}

struct RasterKeySample final {
    std::uint32_t event_kind{};
    std::uint32_t physical_key{};
    std::uint32_t modifiers{};
    bool repeat{};
    bool handled{};
};

struct RasterTextSample final {
    std::string text;
    bool composing{};
    std::int32_t replacement_start{-1};
    std::int32_t replacement_length{};
};

[[nodiscard]] inline gui_forms::Color color_from_argb(
    std::uint32_t argb) noexcept {
    return gui_forms::Color::rgba(
        static_cast<std::uint8_t>((argb >> 16U) & 0xffU),
        static_cast<std::uint8_t>((argb >> 8U) & 0xffU),
        static_cast<std::uint8_t>(argb & 0xffU),
        static_cast<std::uint8_t>((argb >> 24U) & 0xffU));
}


} // namespace gui_forms::abi::detail

