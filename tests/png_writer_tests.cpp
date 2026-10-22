// The shared owned PNG encoder: round trips through the toolkit's validator
// and owned decoder, every source layout (RGBA/BGRA; straight, premultiplied,
// opaque), every compression level, filter selection, refusals, and the
// cross-platform identity hash of the encoded bytes.
#include "../src/core/image/png_pixels.hpp"
#include "../src/core/image/png_writer.hpp"
#include "gui_forms/resources/types/image_resource_types.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <limits>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
using namespace gui_forms;
using Bytes = std::vector<std::byte>;

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

struct IsNonzero final {
    bool operator()(unsigned value) const noexcept { return value != 0U; }
};

std::uint32_t read_u32(const std::byte* bytes) noexcept {
    return (std::to_integer<std::uint32_t>(bytes[0]) << 24U) |
           (std::to_integer<std::uint32_t>(bytes[1]) << 16U) |
           (std::to_integer<std::uint32_t>(bytes[2]) << 8U) |
           std::to_integer<std::uint32_t>(bytes[3]);
}

// FNV-1a, 64-bit: a stable digest for identity, not a security property.
struct Fnv1a final {
    std::uint64_t value = 14695981039346656037ULL;
    void add(std::span<const std::byte> bytes) noexcept {
        for (const std::byte item : bytes) {
            value ^= std::to_integer<std::uint64_t>(item);
            value *= 1099511628211ULL;
        }
    }
};

// Decodes with the owned decoder to straight RGBA8, tight rows.
Bytes decode_straight(const Bytes& png, std::uint32_t& width, std::uint32_t& height) {
    image::PngPixelDecoder decoder{};
    image::PngPixelRequest request{};
    request.target = {image::PixelOrder::rgba, image::PixelAlpha::straight};
    const image::PngPixelResult inspected = decoder.inspect(png, request);
    require(inspected.error == image::PngPixelError::none, "encoded PNG refused by the owned decoder");
    width = inspected.width;
    height = inspected.height;
    Bytes pixels(static_cast<std::size_t>(width) * height * 4U);
    const image::PngPixelResult decoded =
        decoder.decode(png, request, pixels, static_cast<std::size_t>(width) * 4U);
    require(decoded.error == image::PngPixelError::none, "encoded PNG failed to decode");
    return pixels;
}

// Filter type of every row, read back by inflating the IDAT with stx_zlib.
std::vector<unsigned> filter_types(const Bytes& png, std::size_t row_bytes, std::uint32_t height) {
    Bytes zlib{};
    std::size_t offset = 8;
    while (offset + 12U <= png.size()) {
        const std::uint32_t length = read_u32(png.data() + offset);
        const std::string type(reinterpret_cast<const char*>(png.data() + offset + 4U), 4U);
        if (type == "IDAT") {
            zlib.insert(zlib.end(), png.begin() + static_cast<std::ptrdiff_t>(offset + 8U),
                        png.begin() + static_cast<std::ptrdiff_t>(offset + 8U + length));
        }
        offset += 12U + length;
    }
    std::unique_ptr<compression::InflateStream> stream = std::make_unique<compression::InflateStream>();
    compression::InflateOptions options{};
    options.limits.input_bytes.value = zlib.size();
    options.limits.output_bytes.value = (row_bytes + 1U) * height;
    options.limits.work.value = 1ULL << 40U;
    require((*stream).reset(options) == compression::InflateError::none, "inflate reset");
    Bytes raw((row_bytes + 1U) * height);
    const compression::InflateResult result = (*stream).process(zlib, raw, true);
    require(result.status == compression::InflateStatus::stream_end, "IDAT did not inflate");
    std::vector<unsigned> filters{};
    for (std::uint32_t row = 0; row < height; ++row) {
        filters.push_back(std::to_integer<unsigned>(raw[row * (row_bytes + 1U)]));
    }
    return filters;
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
    for (int level = 0; level <= 9; ++level) {
        image::PngEncodeInput input;
        input.width = width;
        input.height = height;
        input.row_bytes = stride;
        input.pixels = source_bytes();
        input.order = image::PngChannelOrder::rgba;
        input.compression_level = level;
        Bytes png;
        require(image::encode_png(input, png), "encode_png rejected a valid RGBA image");
        const PngValidationResult validated = validate_png(png);
        require(static_cast<bool>(validated), image_resource_error_name(validated.error).data());
        require(validated.metadata.width == width && validated.metadata.height == height,
                "IHDR dimensions differ");
        require(validated.metadata.bit_depth == 8, "bit depth is not 8");
        require(validated.metadata.color_type == PngColorType::truecolor_alpha,
                "RGBA output is not truecolor_alpha");
        require(!validated.metadata.interlaced, "output must not be interlaced");
        std::uint32_t decoded_width = 0;
        std::uint32_t decoded_height = 0;
        const Bytes pixels = decode_straight(png, decoded_width, decoded_height);
        require(decoded_width == width && decoded_height == height, "decoded dimensions differ");
        for (std::uint32_t row = 0; row < height; ++row) {
            for (std::uint32_t column = 0; column < width * 4U; ++column) {
                const std::uint8_t expected = source_rgba[row * stride + column];
                require(pixels[row * width * 4U + column] == std::byte{expected},
                        "decoded RGBA pixel differs");
            }
        }
        if (level == 0) {
            const std::vector<unsigned> filters = filter_types(png, width * 4U, height);
            require(filters[0] == 0U && filters[1] == 0U, "level 0 used a filter");
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
    input.alpha = image::PngSourceAlpha::opaque;
    Bytes png;
    require(image::encode_png(input, png), "encode_png rejected a valid BGRA image");
    const PngValidationResult validated = validate_png(png);
    require(static_cast<bool>(validated), image_resource_error_name(validated.error).data());
    require(validated.metadata.color_type == PngColorType::truecolor,
            "opaque output is not truecolor");
    std::uint32_t decoded_width = 0;
    std::uint32_t decoded_height = 0;
    const Bytes pixels = decode_straight(png, decoded_width, decoded_height);
    for (std::uint32_t row = 0; row < height; ++row) {
        for (std::uint32_t column = 0; column < width; ++column) {
            const std::uint8_t* pixel = source_rgba.data() + row * stride + column * 4U;
            const std::size_t at = (static_cast<std::size_t>(row) * width + column) * 4U;
            // BGRA source: byte 2 is red, byte 0 is blue.
            require(pixels[at] == std::byte{pixel[2]}, "red channel differs");
            require(pixels[at + 1U] == std::byte{pixel[1]}, "green channel differs");
            require(pixels[at + 2U] == std::byte{pixel[0]}, "blue channel differs");
            require(pixels[at + 3U] == std::byte{255}, "opaque alpha differs");
        }
    }
}

// Premultiplied sources are unpremultiplied with the stated nearest rule, and
// a premultiplied pixel survives encode then premultiplied decode exactly.
void premultiplied_round_trip() {
    Bytes premultiplied{};
    for (unsigned alpha = 0; alpha < 256; alpha += 5) {
        for (unsigned value = 0; value <= alpha; value += 3) {
            premultiplied.push_back(static_cast<std::byte>(value));
            premultiplied.push_back(static_cast<std::byte>(alpha - value));
            premultiplied.push_back(static_cast<std::byte>(value / 2U));
            premultiplied.push_back(static_cast<std::byte>(alpha));
        }
    }
    const std::uint32_t count = static_cast<std::uint32_t>(premultiplied.size() / 4U);
    image::PngEncodeInput input;
    input.width = count;
    input.height = 1;
    input.row_bytes = premultiplied.size();
    input.pixels = premultiplied;
    input.alpha = image::PngSourceAlpha::premultiplied;
    Bytes png;
    require(image::encode_png(input, png), "premultiplied source refused");
    std::uint32_t decoded_width = 0;
    std::uint32_t decoded_height = 0;
    const Bytes straight = decode_straight(png, decoded_width, decoded_height);
    for (std::size_t at = 0; at < premultiplied.size(); at += 4U) {
        const unsigned alpha = std::to_integer<unsigned>(premultiplied[at + 3U]);
        for (std::size_t channel = 0; channel < 3U; ++channel) {
            const unsigned value = std::to_integer<unsigned>(premultiplied[at + channel]);
            const unsigned expected = alpha == 0U ? 0U : std::min(255U, (value * 255U + alpha / 2U) / alpha);
            require(std::to_integer<unsigned>(straight[at + channel]) == expected, "unpremultiply rule");
        }
    }
    image::PngPixelDecoder decoder{};
    image::PngPixelRequest request{};
    request.target = {image::PixelOrder::rgba, image::PixelAlpha::premultiplied};
    Bytes again(premultiplied.size());
    require(decoder.decode(png, request, again, premultiplied.size()).error == image::PngPixelError::none,
            "premultiplied decode");
    require(again == premultiplied, "premultiplied pixels did not survive a round trip");
}

// A smooth gradient makes Sub/Up/Average/Paeth win over None on most rows.
Bytes gradient(std::uint32_t w, std::uint32_t h) {
    Bytes pixels(static_cast<std::size_t>(w) * h * 4U);
    for (std::uint32_t y = 0; y < h; ++y) {
        for (std::uint32_t x = 0; x < w; ++x) {
            const std::size_t at = (static_cast<std::size_t>(y) * w + x) * 4U;
            pixels[at] = static_cast<std::byte>((x * 3U + y) & 255U);
            pixels[at + 1U] = static_cast<std::byte>((y * 2U) & 255U);
            pixels[at + 2U] = static_cast<std::byte>(((x + y) / 2U) & 255U);
            pixels[at + 3U] = static_cast<std::byte>(255U - ((x * y) & 63U));
        }
    }
    return pixels;
}

void filter_selection_and_levels() {
    const std::uint32_t w = 97;
    const std::uint32_t h = 61;
    const Bytes pixels = gradient(w, h);
    image::PngEncodeInput input;
    input.width = w;
    input.height = h;
    input.row_bytes = static_cast<std::uint64_t>(w) * 4U;
    input.pixels = pixels;
    std::size_t stored_size = 0;
    std::size_t default_size = 0;
    for (int level = 0; level <= 9; ++level) {
        input.compression_level = level;
        Bytes png;
        require(image::encode_png(input, png), "gradient refused");
        std::uint32_t dw = 0;
        std::uint32_t dh = 0;
        require(decode_straight(png, dw, dh) == pixels, "gradient changed by a round trip");
        if (level == 0) stored_size = png.size();
        if (level == image::default_png_compression_level) {
            default_size = png.size();
            const std::vector<unsigned> filters = filter_types(png, w * 4U, h);
            const bool filtered = std::any_of(filters.begin(), filters.end(), IsNonzero{});
            require(filtered, "filter selection never chose a filter for a gradient");
        }
    }
    require(default_size < stored_size / 2U, "default level did not compress a gradient");
    input.compression_level = 10;
    Bytes refused;
    require(!image::encode_png(input, refused) && refused.empty(), "level 10 accepted");
    input.compression_level = -1;
    require(!image::encode_png(input, refused) && refused.empty(), "level -1 accepted");
}

void rejects_invalid_input() {
    Bytes png;
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
    input.width = width;
    input.alpha = static_cast<image::PngSourceAlpha>(9);
    require(!image::encode_png(input, png) && png.empty(), "invalid alpha mode accepted");
}

void rejects_stride_overflow_and_alias() {
    const std::array<std::byte, 4> pixel{std::byte{1}, std::byte{2}, std::byte{3}, std::byte{255}};
    image::PngEncodeInput input{.width=1, .height=3,
        .row_bytes=std::uint64_t{1} << 63, .pixels=pixel};
    Bytes png{std::byte{7}};
    require(!image::encode_png(input, png) && png.empty(), "wrapped stride multiplication accepted");
    input.height = 2;
    input.row_bytes = std::numeric_limits<std::uint64_t>::max();
    require(!image::encode_png(input, png) && png.empty(), "wrapped final row addition accepted");
    input.height = 1;
    require(image::encode_png(input, png), "unused one-row stride rejected");
    std::uint32_t dw = 0;
    std::uint32_t dh = 0;
    const Bytes decoded = decode_straight(png, dw, dh);
    require(decoded.size() == 4 && std::equal(pixel.begin(), pixel.end(), decoded.begin()),
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

// The encoded bytes for fixed inputs are the same on every host; CI checks
// this digest on all four platforms.
struct IdentityCase final {
    std::uint32_t w = 0;
    std::uint32_t h = 0;
    image::PngChannelOrder order = image::PngChannelOrder::rgba;
    image::PngSourceAlpha alpha = image::PngSourceAlpha::straight;
    int level = image::default_png_compression_level;
};

void identity_hashes() {
    const std::array<IdentityCase, 6> cases{{
        {97, 61, image::PngChannelOrder::rgba, image::PngSourceAlpha::straight, 6},
        {97, 61, image::PngChannelOrder::bgra, image::PngSourceAlpha::opaque, 6},
        {200, 150, image::PngChannelOrder::rgba, image::PngSourceAlpha::premultiplied, 6},
        {200, 150, image::PngChannelOrder::bgra, image::PngSourceAlpha::straight, 1},
        {200, 150, image::PngChannelOrder::rgba, image::PngSourceAlpha::straight, 9},
        {64, 64, image::PngChannelOrder::rgba, image::PngSourceAlpha::straight, 0}}};
    Fnv1a digest{};
    for (const IdentityCase& item : cases) {
        const Bytes pixels = gradient(item.w, item.h);
        image::PngEncodeInput input;
        input.width = item.w;
        input.height = item.h;
        input.row_bytes = static_cast<std::uint64_t>(item.w) * 4U;
        input.pixels = pixels;
        input.order = item.order;
        input.alpha = item.alpha;
        input.compression_level = item.level;
        Bytes png;
        require(image::encode_png(input, png), "identity case refused");
        Bytes again;
        require(image::encode_png(input, again) && again == png, "encoder is not deterministic");
        digest.add(png);
    }
    constexpr std::uint64_t expected = 0x8728e27ff2636f0eULL;
    std::printf("png_writer identity: 0x%016llxULL\n", static_cast<unsigned long long>(digest.value));
    require(digest.value == expected, "encoded PNG identity hash changed");
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
        premultiplied_round_trip();
        filter_selection_and_levels();
        rejects_invalid_input();
        rejects_stride_overflow_and_alias();
        identity_hashes();
    } catch (const std::exception& error) {
        std::cerr << "png_writer_tests: " << error.what() << '\n';
        return 1;
    }
    std::cout << "png_writer_tests: ok\n";
    return 0;
}
