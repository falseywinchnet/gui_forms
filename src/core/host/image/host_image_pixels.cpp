#include "host_image_pixels.hpp"

#include <algorithm>

namespace gui_forms::detail {
std::uint8_t premultiplied_channel(const std::uint8_t straight, const std::uint8_t alpha) noexcept {
    return static_cast<std::uint8_t>((static_cast<unsigned>(straight) * alpha + 127U) / 255U);
}

std::uint8_t straight_channel(const std::uint8_t premultiplied, const std::uint8_t alpha) noexcept {
    if (alpha == 0U) return 0U;
    return static_cast<std::uint8_t>(std::min(255U, (static_cast<unsigned>(premultiplied) * 255U + alpha / 2U) / alpha));
}

void straight_rgba_to_premultiplied_bgra(const std::span<const std::byte> source, const std::span<std::byte> destination,
                                         const std::uint32_t width) noexcept {
    for (std::size_t offset = 0U; offset < static_cast<std::size_t>(width) * 4U; offset += 4U) {
        const std::uint8_t red = std::to_integer<std::uint8_t>(source[offset]);
        const std::uint8_t green = std::to_integer<std::uint8_t>(source[offset + 1U]);
        const std::uint8_t blue = std::to_integer<std::uint8_t>(source[offset + 2U]);
        const std::uint8_t alpha = std::to_integer<std::uint8_t>(source[offset + 3U]);
        destination[offset] = std::byte{premultiplied_channel(blue, alpha)};
        destination[offset + 1U] = std::byte{premultiplied_channel(green, alpha)};
        destination[offset + 2U] = std::byte{premultiplied_channel(red, alpha)};
        destination[offset + 3U] = std::byte{alpha};
    }
}

void premultiplied_bgra_to_straight_bgra(const std::span<const std::byte> source, const std::span<std::byte> destination,
                                         const std::uint32_t width) noexcept {
    for (std::size_t offset = 0U; offset < static_cast<std::size_t>(width) * 4U; offset += 4U) {
        const std::uint8_t alpha = std::to_integer<std::uint8_t>(source[offset + 3U]);
        for (std::size_t channel = 0U; channel < 3U; ++channel) {
            const std::uint8_t value = std::to_integer<std::uint8_t>(source[offset + channel]);
            destination[offset + channel] = std::byte{straight_channel(value, alpha)};
        }
        destination[offset + 3U] = std::byte{alpha};
    }
}
} // namespace gui_forms::detail
