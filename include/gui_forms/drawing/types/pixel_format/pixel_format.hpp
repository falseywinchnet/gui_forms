#pragma once

#include <cstdint>

namespace gui_drawing {

// Every bitmap, image and snapshot holds premultiplied BGRA8 (blue, green,
// red, alpha in memory), the framebuffer's order on every host.
enum class PixelFormat : std::uint8_t {
    bgra32_premultiplied,
};

} // namespace gui_drawing
