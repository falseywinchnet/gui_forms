// Round-trips a small image through the renderer-neutral PNG writer: the
// toolkit's own PNG validator must accept the stream, and the stored deflate
// blocks must decode back to the same pixels.
#include "../src/core/image/png_writer.hpp"
#include "gui_forms/resources/types/image_resource_types.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using namespace gui_forms;

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

std::uint32_t read_u32(const std::byte* bytes) noexcept {
    return (std::to_integer<std::uint32_t>(bytes[0]) << 24U) |
           (std::to_integer<std::uint32_t>(bytes[1]) << 16U) |
           (std::to_integer<std::uint32_t>(bytes[2]) << 8U) |
           std::to_integer<std::uint32_t>(bytes[3]);
}

// Concatenates every IDAT payload (the validator already proved the chunk
// layout and CRCs).
std::vector<std::byte> idat_payload(std::span<const std::byte> png) {
    std::vector<std::byte> result;
    std::size_t offset = 8;
    while (offset + 12U <= png.size()) {
        const std::uint32_t length = read_u32(png.data() + offset);
        const std::string type(reinterpret_cast<const char*>(png.data() + offset + 4U), 4U);
        if (type == "IDAT") {
            result.insert(result.end(), png.begin() + static_cast<std::ptrdiff_t>(offset + 8U),
                          png.begin() + static_cast<std::ptrdiff_t>(offset + 8U + length));
        }
        offset += 12U + length;
    }
    return result;
}

// Decodes a zlib stream made only of stored (BTYPE=00) deflate blocks, which
// is exactly the dependency-free form the writer emits, and checks adler32.
std::vector<std::byte> inflate_stored(std::span<const std::byte> zlib) {
    require(zlib.size() >= 6U, "zlib stream too short");
    const std::uint8_t cmf = std::to_integer<std::uint8_t>(zlib[0]);
    const std::uint8_t flg = std::to_integer<std::uint8_t>(zlib[1]);
    require((cmf & 0x0fU) == 8U, "zlib compression method is not deflate");
    require(((static_cast<unsigned>(cmf) << 8U) | flg) % 31U == 0U, "zlib header check failed");
    require((flg & 0x20U) == 0U, "unexpected preset dictionary");
    std::vector<std::byte> raw;
    std::size_t offset = 2;
    bool final_block = false;
    while (!final_block) {
        require(offset + 5U <= zlib.size(), "truncated stored block header");
        const std::uint8_t header = std::to_integer<std::uint8_t>(zlib[offset]);
        final_block = (header & 1U) != 0U;
        require(((header >> 1U) & 3U) == 0U, "block is not a stored block");
        const std::uint16_t len = static_cast<std::uint16_t>(
            std::to_integer<std::uint16_t>(zlib[offset + 1U]) |
            (std::to_integer<std::uint16_t>(zlib[offset + 2U]) << 8U));
        const std::uint16_t nlen = static_cast<std::uint16_t>(
            std::to_integer<std::uint16_t>(zlib[offset + 3U]) |
            (std::to_integer<std::uint16_t>(zlib[offset + 4U]) << 8U));
        require(static_cast<std::uint16_t>(~len) == nlen, "stored block NLEN mismatch");
        offset += 5U;
        require(offset + len <= zlib.size(), "truncated stored block payload");
        raw.insert(raw.end(), zlib.begin() + static_cast<std::ptrdiff_t>(offset),
                   zlib.begin() + static_cast<std::ptrdiff_t>(offset + len));
        offset += len;
    }
    require(offset + 4U == zlib.size(), "unexpected trailing zlib bytes");
    require(read_u32(zlib.data() + offset) == image::png_adler32(raw), "adler32 mismatch");
    return raw;
}

// 3x2 straight-alpha RGBA with a padded stride and a translucent pixel.
constexpr std::uint32_t width = 3;
constexpr std::uint32_t height = 2;
constexpr std::uint64_t stride = 16; // 12 pixel bytes + 4 padding
const std::array<std::uint8_t, 28> source_rgba{
    255, 0, 0, 255,   0, 255, 0, 255,   0, 0, 255, 128,   9, 9, 9, 9,
    10, 20, 30, 0,    40, 50, 60, 255,  70, 80, 90, 200};

std::span<const std::byte> source_bytes() {
    return std::as_bytes(std::span<const std::uint8_t>(source_rgba));
}

void rgba_round_trip() {
    image::PngEncodeInput input;
    input.width = width;
    input.height = height;
    input.row_bytes = stride;
    input.pixels = source_bytes();
    input.order = image::PngChannelOrder::rgba;
    std::vector<std::byte> png;
    require(image::encode_png(input, png), "encode_png rejected a valid RGBA image");

    const PngValidationResult validated = validate_png(png);
    require(static_cast<bool>(validated), image_resource_error_name(validated.error).data());
    require(validated.metadata.width == width && validated.metadata.height == height,
            "IHDR dimensions differ");
    require(validated.metadata.bit_depth == 8, "bit depth is not 8");
    require(validated.metadata.color_type == PngColorType::truecolor_alpha,
            "RGBA output is not truecolor_alpha");
    require(!validated.metadata.interlaced, "output must not be interlaced");

    const std::vector<std::byte> raw = inflate_stored(idat_payload(png));
    require(raw.size() == static_cast<std::size_t>(height) * (1U + width * 4U),
            "decoded scanline size differs");
    for (std::uint32_t row = 0; row < height; ++row) {
        const std::byte* line = raw.data() + row * (1U + width * 4U);
        require(line[0] == std::byte{0}, "filter byte is not none");
        for (std::uint32_t column = 0; column < width * 4U; ++column) {
            const std::uint8_t expected = source_rgba[row * stride + column];
            require(line[1U + column] == std::byte{expected}, "decoded RGBA pixel differs");
        }
    }
}

void bgra_opaque_round_trip() {
    image::PngEncodeInput input;
    input.width = width;
    input.height = height;
    input.row_bytes = stride;
    input.pixels = source_bytes();
    input.order = image::PngChannelOrder::bgra;
    input.opaque = true;
    std::vector<std::byte> png;
    require(image::encode_png(input, png), "encode_png rejected a valid BGRA image");
    const PngValidationResult validated = validate_png(png);
    require(static_cast<bool>(validated), image_resource_error_name(validated.error).data());
    require(validated.metadata.color_type == PngColorType::truecolor,
            "opaque output is not truecolor");
    const std::vector<std::byte> raw = inflate_stored(idat_payload(png));
    require(raw.size() == static_cast<std::size_t>(height) * (1U + width * 3U),
            "decoded opaque scanline size differs");
    for (std::uint32_t row = 0; row < height; ++row) {
        const std::byte* line = raw.data() + row * (1U + width * 3U);
        require(line[0] == std::byte{0}, "filter byte is not none");
        for (std::uint32_t column = 0; column < width; ++column) {
            const std::uint8_t* pixel = source_rgba.data() + row * stride + column * 4U;
            // BGRA source: byte 2 is red, byte 0 is blue.
            require(line[1U + column * 3U] == std::byte{pixel[2]}, "red channel differs");
            require(line[2U + column * 3U] == std::byte{pixel[1]}, "green channel differs");
            require(line[3U + column * 3U] == std::byte{pixel[0]}, "blue channel differs");
        }
    }
}

void rejects_invalid_input() {
    std::vector<std::byte> png;
    image::PngEncodeInput input;
    input.width = width;
    input.height = height;
    input.row_bytes = stride;
    input.pixels = source_bytes();
    input.row_bytes = 8; // narrower than width * 4
    require(!image::encode_png(input, png) && png.empty(), "short stride accepted");
    input.row_bytes = stride;
    input.pixels = source_bytes().subspan(0, 20); // final row cut short
    require(!image::encode_png(input, png) && png.empty(), "short buffer accepted");
    input.pixels = source_bytes();
    input.width = 0;
    require(!image::encode_png(input, png) && png.empty(), "zero width accepted");
}

void rejects_stride_overflow_and_alias() {
    const std::array<std::byte, 4> pixel{std::byte{1}, std::byte{2}, std::byte{3}, std::byte{255}};
    image::PngEncodeInput input{.width=1, .height=3,
        .row_bytes=std::uint64_t{1} << 63, .pixels=pixel};
    std::vector<std::byte> png{std::byte{7}};
    require(!image::encode_png(input, png) && png.empty(), "wrapped stride multiplication accepted");
    input.height = 2;
    input.row_bytes = std::numeric_limits<std::uint64_t>::max();
    require(!image::encode_png(input, png) && png.empty(), "wrapped final row addition accepted");
    input.height = 1;
    require(image::encode_png(input, png), "unused one-row stride rejected");
    const std::vector<std::byte> decoded = inflate_stored(idat_payload(png));
    require(decoded.size() == 5 && std::equal(pixel.begin(), pixel.end(), decoded.begin() + 1),
            "one-row pixels changed");
    input.order = static_cast<image::PngChannelOrder>(255);
    require(!image::encode_png(input, png) && png.empty(), "invalid channel order accepted");
    input.order = image::PngChannelOrder::rgba;
    input.row_bytes = 4;
    for (std::size_t offset : {std::size_t{0}, std::size_t{3}}) {
        png.assign(8, std::byte{9});
        input.pixels = std::span<const std::byte>(png.data() + offset, 4);
        const bool accepted = image::encode_png(input, png);
        input.pixels = pixel;
        require(!accepted && png.empty(), "aliased pixel input accepted");
    }
}

void checksums_match_reference_values() {
    const std::array<std::uint8_t, 9> text{'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    const std::span<const std::byte> bytes = std::as_bytes(std::span<const std::uint8_t>(text));
    require(image::png_crc32(bytes) == 0xcbf43926U, "crc32 check value differs");
    require(image::png_adler32(bytes) == 0x091e01deU, "adler32 check value differs");
    require(image::png_crc32({}) == 0U && image::png_adler32({}) == 1U, "empty checksum values");
}

} // namespace

int main() {
    try {
        checksums_match_reference_values();
        rgba_round_trip();
        bgra_opaque_round_trip();
        rejects_invalid_input();
        rejects_stride_overflow_and_alias();
    } catch (const std::exception& error) {
        std::cerr << "png_writer_tests: " << error.what() << '\n';
        return 1;
    }
    std::cout << "png_writer_tests: ok\n";
    return 0;
}
