#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace gui_forms::image {

// Channel order of the 32-bit source pixels. Both orders are read as four
// 8-bit channels per pixel, top row first.
enum class PngChannelOrder : std::uint8_t { rgba, bgra };

struct PngEncodeInput final {
    std::uint32_t width{};
    std::uint32_t height{};
    // Bytes between successive rows; must be at least width * 4. Trailing
    // padding of the final row is not required.
    std::uint64_t row_bytes{};
    std::span<const std::byte> pixels{};
    PngChannelOrder order{PngChannelOrder::rgba};
    // When true the alpha channel is ignored and an opaque truecolor PNG
    // (color type 2) is written; otherwise straight-alpha RGBA (color type 6).
    bool opaque{};
};

// Renderer-neutral, dependency-free PNG encoder: 8-bit truecolor(+alpha),
// filter type 0, zlib stream of stored (uncompressed) deflate blocks with a
// hand-written adler32 and crc32. Output size is roughly the raw pixel size;
// the writer exists so hosts without a compressing codec can still emit a
// portable image. Pixel input must not overlap the live output storage.
// Returns false on invalid input, overlap or overflow; output is then
// left empty.
[[nodiscard]] bool encode_png(const PngEncodeInput& input,
                              std::vector<std::byte>& output);

[[nodiscard]] std::uint32_t png_crc32(std::span<const std::byte> bytes) noexcept;
[[nodiscard]] std::uint32_t png_adler32(std::span<const std::byte> bytes) noexcept;

} // namespace gui_forms::image
