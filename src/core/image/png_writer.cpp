#include "png_writer.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>

namespace gui_forms::image {
namespace {

constexpr std::array<std::uint8_t, 8> png_signature{
    0x89U, 0x50U, 0x4eU, 0x47U, 0x0dU, 0x0aU, 0x1aU, 0x0aU};
// Largest stored deflate block payload (16-bit LEN field).
constexpr std::size_t stored_block_limit = 65535U;
// Matches the renderer core's registry ceiling so a capture never exceeds
// what the toolkit's own PNG validator will accept.
constexpr std::uint32_t dimension_limit = 16384U;

std::array<std::uint32_t, 256> make_crc_table() noexcept {
    std::array<std::uint32_t, 256> table{};
    for (std::uint32_t index = 0; index < 256U; ++index) {
        std::uint32_t value = index;
        for (int bit = 0; bit < 8; ++bit) {
            value = (value & 1U) != 0U ? 0xedb88320U ^ (value >> 1U) : value >> 1U;
        }
        table[index] = value;
    }
    return table;
}

void append_u32_big_endian(std::vector<std::byte>& output, std::uint32_t value) {
    output.push_back(static_cast<std::byte>((value >> 24U) & 0xffU));
    output.push_back(static_cast<std::byte>((value >> 16U) & 0xffU));
    output.push_back(static_cast<std::byte>((value >> 8U) & 0xffU));
    output.push_back(static_cast<std::byte>(value & 0xffU));
}

void append_chunk(std::vector<std::byte>& output, const char (&type)[5],
                  std::span<const std::byte> data) {
    append_u32_big_endian(output, static_cast<std::uint32_t>(data.size()));
    const std::size_t crc_start = output.size();
    for (std::size_t index = 0; index < 4U; ++index) {
        output.push_back(static_cast<std::byte>(type[index]));
    }
    output.insert(output.end(), data.begin(), data.end());
    const std::span<const std::byte> crc_input(output.data() + crc_start,
                                               output.size() - crc_start);
    append_u32_big_endian(output, png_crc32(crc_input));
}

} // namespace

std::uint32_t png_crc32(std::span<const std::byte> bytes) noexcept {
    static const std::array<std::uint32_t, 256> table = make_crc_table();
    std::uint32_t crc = 0xffffffffU;
    for (const std::byte value : bytes) {
        crc = table[(crc ^ std::to_integer<std::uint32_t>(value)) & 0xffU] ^ (crc >> 8U);
    }
    return crc ^ 0xffffffffU;
}

std::uint32_t png_adler32(std::span<const std::byte> bytes) noexcept {
    std::uint32_t a = 1U;
    std::uint32_t b = 0U;
    // 5552 is the largest run before the sums can overflow 32 bits.
    std::size_t offset = 0;
    while (offset < bytes.size()) {
        const std::size_t run = std::min<std::size_t>(5552U, bytes.size() - offset);
        for (std::size_t index = 0; index < run; ++index) {
            a += std::to_integer<std::uint32_t>(bytes[offset + index]);
            b += a;
        }
        a %= 65521U;
        b %= 65521U;
        offset += run;
    }
    return (b << 16U) | a;
}

bool encode_png(const PngEncodeInput& input, std::vector<std::byte>& output) {
    output.clear();
    if (input.width == 0U || input.height == 0U || input.width > dimension_limit ||
        input.height > dimension_limit) {
        return false;
    }
    const std::uint64_t tight_row = static_cast<std::uint64_t>(input.width) * 4U;
    if (input.row_bytes < tight_row) return false;
    // The final row only needs its pixel bytes, not trailing stride padding.
    const std::uint64_t required = input.row_bytes * (input.height - 1U) + tight_row;
    if (required > input.pixels.size()) return false;

    const std::size_t channels = input.opaque ? 3U : 4U;
    const std::size_t scanline = 1U + static_cast<std::size_t>(input.width) * channels;
    const std::size_t raw_size = scanline * static_cast<std::size_t>(input.height);
    if (raw_size / scanline != input.height) return false;

    std::vector<std::byte> raw;
    raw.reserve(raw_size);
    const std::size_t red = input.order == PngChannelOrder::rgba ? 0U : 2U;
    const std::size_t blue = input.order == PngChannelOrder::rgba ? 2U : 0U;
    for (std::uint32_t row = 0; row < input.height; ++row) {
        raw.push_back(std::byte{0}); // filter type: none
        const std::byte* source = input.pixels.data() +
            static_cast<std::size_t>(input.row_bytes) * row;
        for (std::uint32_t column = 0; column < input.width; ++column) {
            const std::byte* pixel = source + static_cast<std::size_t>(column) * 4U;
            raw.push_back(pixel[red]);
            raw.push_back(pixel[1]);
            raw.push_back(pixel[blue]);
            if (!input.opaque) raw.push_back(pixel[3]);
        }
    }

    // zlib header: CM=8 (deflate), CINFO=7 (32K window), no dictionary,
    // FLEVEL=0; FCHECK makes the 16-bit header a multiple of 31.
    std::vector<std::byte> zlib;
    const std::size_t block_count = (raw.size() + stored_block_limit - 1U) / stored_block_limit;
    zlib.reserve(2U + raw.size() + block_count * 5U + 4U);
    zlib.push_back(std::byte{0x78});
    zlib.push_back(std::byte{0x01});
    std::size_t offset = 0;
    do {
        const std::size_t length = std::min(stored_block_limit, raw.size() - offset);
        const bool final_block = offset + length >= raw.size();
        zlib.push_back(std::byte{final_block ? std::uint8_t{1} : std::uint8_t{0}});
        const std::uint16_t len = static_cast<std::uint16_t>(length);
        const std::uint16_t nlen = static_cast<std::uint16_t>(~len);
        zlib.push_back(static_cast<std::byte>(len & 0xffU));
        zlib.push_back(static_cast<std::byte>(len >> 8U));
        zlib.push_back(static_cast<std::byte>(nlen & 0xffU));
        zlib.push_back(static_cast<std::byte>(nlen >> 8U));
        zlib.insert(zlib.end(), raw.begin() + static_cast<std::ptrdiff_t>(offset),
                    raw.begin() + static_cast<std::ptrdiff_t>(offset + length));
        offset += length;
    } while (offset < raw.size());
    append_u32_big_endian(zlib, png_adler32(raw));
    if (zlib.size() > 0x7fffffffU) return false;

    output.reserve(png_signature.size() + 25U + zlib.size() + 12U + 12U);
    for (const std::uint8_t value : png_signature) output.push_back(std::byte{value});

    std::array<std::byte, 13> header{};
    header[0] = static_cast<std::byte>((input.width >> 24U) & 0xffU);
    header[1] = static_cast<std::byte>((input.width >> 16U) & 0xffU);
    header[2] = static_cast<std::byte>((input.width >> 8U) & 0xffU);
    header[3] = static_cast<std::byte>(input.width & 0xffU);
    header[4] = static_cast<std::byte>((input.height >> 24U) & 0xffU);
    header[5] = static_cast<std::byte>((input.height >> 16U) & 0xffU);
    header[6] = static_cast<std::byte>((input.height >> 8U) & 0xffU);
    header[7] = static_cast<std::byte>(input.height & 0xffU);
    header[8] = std::byte{8};                                  // bit depth
    header[9] = std::byte{input.opaque ? std::uint8_t{2} : std::uint8_t{6}}; // color type
    header[10] = std::byte{0};                                 // compression
    header[11] = std::byte{0};                                 // filter method
    header[12] = std::byte{0};                                 // no interlace
    append_chunk(output, "IHDR", header);
    const std::array<std::byte, 1> srgb{std::byte{0}}; // perceptual intent
    append_chunk(output, "sRGB", srgb);
    append_chunk(output, "IDAT", zlib);
    append_chunk(output, "IEND", {});
    return true;
}

} // namespace gui_forms::image
