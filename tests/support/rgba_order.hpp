#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace gui_forms::test_support {

// BGRA8 pixel bytes (blue, green, red, alpha) in red, green, blue, alpha
// order. The engine's buffers were RGBA8 until SK2 made them BGRA8; the tests
// hash pixels in this order so the hashes recorded before still hold, which
// shows the change only reordered bytes. `bgra` holds whole pixels.
[[nodiscard]] inline std::vector<std::uint8_t> rgba_order(const std::span<const std::uint8_t> bgra) {
    std::vector<std::uint8_t> rgba(bgra.begin(), bgra.end());
    for (std::size_t at = 0U; at + 3U < rgba.size(); at += 4U) {
        rgba[at] = bgra[at + 2U];
        rgba[at + 2U] = bgra[at];
    }
    return rgba;
}

} // namespace gui_forms::test_support
