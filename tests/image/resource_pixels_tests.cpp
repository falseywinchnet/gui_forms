// The codec-neutral resource seam: the registry admits CNG and PNG files by
// signature (load_image, replace_image) with neutral metadata, keeps
// load_png and replace_png PNG-only, and refuses damaged CNG framing; the
// ResourcePixelDecoder decodes either encoding through every target layout
// to the same bytes, which also match an independent reference.
#include "../../src/core/image/cng_pixels.hpp"
#include "../../src/core/image/resource_pixels.hpp"
#include "gui_forms/resources.hpp"
#include "stx_cng/codec.hpp"
#include "stx_png/encoder.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <span>
#include <stdexcept>
#include <vector>

namespace {

using namespace gui_forms;

using Bytes = std::vector<std::byte>;

void require(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

constexpr std::uint32_t width = 37;
constexpr std::uint32_t height = 23;

constexpr std::array<image::PixelTarget, 4> targets{{
    {image::PixelOrder::rgba, image::PixelAlpha::straight},
    {image::PixelOrder::rgba, image::PixelAlpha::premultiplied},
    {image::PixelOrder::bgra, image::PixelAlpha::straight},
    {image::PixelOrder::bgra, image::PixelAlpha::premultiplied}}};

// Straight RGBA samples with every alpha class: zero, full and partial.
Bytes rgba_samples() {
    Bytes samples(static_cast<std::size_t>(width) * height * 4U);
    for (std::size_t pixel = 0; pixel < static_cast<std::size_t>(width) * height; ++pixel) {
        samples[pixel * 4U] = static_cast<std::byte>((pixel * 7U) & 255U);
        samples[pixel * 4U + 1U] = static_cast<std::byte>((pixel * 13U + 50U) & 255U);
        samples[pixel * 4U + 2U] = static_cast<std::byte>((pixel / 3U) & 255U);
        std::size_t alpha = pixel % 5U == 0U ? 0U : pixel % 5U == 1U ? 255U : (pixel * 29U) & 255U;
        samples[pixel * 4U + 3U] = static_cast<std::byte>(alpha);
    }
    return samples;
}

Bytes rgb_of(const Bytes& rgba) {
    Bytes rgb(rgba.size() / 4U * 3U);
    for (std::size_t pixel = 0; pixel < rgba.size() / 4U; ++pixel) {
        rgb[pixel * 3U] = rgba[pixel * 4U];
        rgb[pixel * 3U + 1U] = rgba[pixel * 4U + 1U];
        rgb[pixel * 3U + 2U] = rgba[pixel * 4U + 2U];
    }
    return rgb;
}

Bytes cng_file(const Bytes& samples, stx_cng::CngChannels channels) {
    stx_cng::CngImage image{width, height, channels};
    std::unique_ptr<stx_cng::CngEncoder> encoder = std::make_unique<stx_cng::CngEncoder>();
    Bytes file(stx_cng::cng_encoded_bound(image));
    std::uint64_t row_bytes = static_cast<std::uint64_t>(width) * static_cast<std::size_t>(channels);
    stx_cng::CngEncodeResult encoded = (*encoder).encode(stx_cng::CngEncodeInput{image, row_bytes, samples}, file);
    require(encoded.error == stx_cng::CngError::none, "fixture: CNG encode");
    file.resize(encoded.encoded_bytes);
    return file;
}

Bytes png_file(const Bytes& rgba, stx_png::PngSourceAlpha alpha) {
    stx_png::PngEncodeInput input{};
    input.width = width;
    input.height = height;
    input.row_bytes = static_cast<std::uint64_t>(width) * 4U;
    input.pixels = rgba;
    input.alpha = alpha;
    Bytes file;
    require(stx_png::encode_png(input, file), "fixture: PNG encode");
    return file;
}

// The expected pixels, computed independently of both codecs.
Bytes reference(const Bytes& rgba, image::PixelTarget target) {
    Bytes pixels(rgba.size());
    bool bgra = target.order == image::PixelOrder::bgra;
    bool premultiplied = target.alpha == image::PixelAlpha::premultiplied;
    for (std::size_t pixel = 0; pixel < rgba.size() / 4U; ++pixel) {
        unsigned alpha = std::to_integer<unsigned>(rgba[pixel * 4U + 3U]);
        for (std::size_t channel = 0; channel < 3U; ++channel) {
            unsigned value = std::to_integer<unsigned>(rgba[pixel * 4U + channel]);
            if (premultiplied) {
                // Nearest integer to value * alpha / 255 (no tie: 255 is odd).
                unsigned product = value * alpha;
                value = product / 255U + (product % 255U * 2U > 255U ? 1U : 0U);
            }
            std::size_t slot = bgra && channel != 1U ? 2U - channel : channel;
            pixels[pixel * 4U + slot] = static_cast<std::byte>(value);
        }
        pixels[pixel * 4U + 3U] = static_cast<std::byte>(alpha);
    }
    return pixels;
}

Bytes decode(image::ResourcePixelDecoder& decoder, const ImageResourceView& view, image::PixelTarget target) {
    Bytes pixels(static_cast<std::size_t>(width) * height * 4U);
    image::ResourcePixelResult decoded = decoder.decode(view, target, pixels, static_cast<std::size_t>(width) * 4U);
    require(decoded.error == image::ResourcePixelError::none, "resource decodes");
    require(decoded.width == width && decoded.height == height, "decoded dimensions");
    return pixels;
}

ImageResourceView found(const ImageRegistry& registry, ImageLoadResult loaded) {
    require(static_cast<bool>(loaded), "image admitted");
    std::optional<ImageResourceView> view = registry.find(loaded.image);
    require(view.has_value(), "admitted image found");
    return *view;
}

void test_admission(const Bytes& rgba) {
    Bytes cng = cng_file(rgba, stx_cng::CngChannels::rgba);
    Bytes cng_rgb = cng_file(rgb_of(rgba), stx_cng::CngChannels::rgb);
    Bytes png = png_file(rgba, stx_png::PngSourceAlpha::straight);
    ImageRegistry registry;

    ImageResourceView native = found(registry, registry.load_image(cng));
    require(native.encoding == ImageResourceEncoding::cng, "CNG encoding recorded");
    require(native.metadata.width == width && native.metadata.height == height, "CNG dimensions");
    require(native.metadata.alpha == ImageAlpha::straight, "RGBA CNG has straight alpha");
    require(native.metadata.decoded_byte_count == static_cast<std::uint64_t>(width) * height * 4U, "CNG decoded bytes");
    require(native.row_bytes == static_cast<std::uint64_t>(width) * 4U, "CNG sample row bytes");
    ImageResourceView opaque = found(registry, registry.load_image(cng_rgb));
    require(opaque.metadata.alpha == ImageAlpha::opaque && opaque.row_bytes == static_cast<std::uint64_t>(width) * 3U,
            "RGB CNG is opaque with three-byte rows");
    ImageResourceView interchange = found(registry, registry.load_image(png));
    require(interchange.encoding == ImageResourceEncoding::png && interchange.metadata.alpha == ImageAlpha::straight,
            "PNG through load_image");
    ImageResourceView opaque_png = found(registry, registry.load_png(png_file(rgba, stx_png::PngSourceAlpha::opaque)));
    require(opaque_png.metadata.alpha == ImageAlpha::opaque, "truecolor PNG is opaque");

    require(registry.load_png(cng).error == ImageResourceError::invalid_signature, "load_png stays PNG-only");
    Bytes garbage(64, std::byte{0x5a});
    require(registry.load_image(garbage).error == ImageResourceError::invalid_signature, "neither format refused");
    require(registry.load_image(Bytes{}).error == ImageResourceError::empty_input, "empty file refused");
    Bytes truncated(cng.begin(), cng.end() - 1);
    require(registry.load_image(truncated).error == ImageResourceError::truncated_chunk, "truncated CNG refused");
    Bytes trailing = cng;
    trailing.push_back(std::byte{0});
    require(registry.load_image(trailing).error == ImageResourceError::trailing_data, "trailing byte refused");
    Bytes damaged = cng;
    damaged[60] ^= std::byte{0x10};
    require(registry.load_image(damaged).error == ImageResourceError::crc_mismatch, "damaged band refused");
    Bytes header = cng;
    header[16] ^= std::byte{1};
    require(registry.load_image(header).error == ImageResourceError::crc_mismatch, "damaged header refused");

    ImageRegistryLimits narrow{};
    narrow.maximum_width = width - 1U;
    ImageRegistry limited(narrow);
    require(limited.load_image(cng).error == ImageResourceError::dimension_limit_exceeded, "registry limits apply to CNG");

    ImageRegistrySnapshot before = registry.snapshot();
    ImageLoadResult replaced = registry.replace_image(interchange.image, cng);
    require(static_cast<bool>(replaced) && replaced.image.value != interchange.image.value, "replace_image takes CNG");
    require(registry.find(replaced.image).has_value() && (*registry.find(replaced.image)).encoding == ImageResourceEncoding::cng,
            "replaced image is CNG");
    ImageRegistrySnapshot after = registry.snapshot();
    require(after.encoded_bytes == before.encoded_bytes - png.size() + cng.size(), "replacement accounting");
    require(registry.replace_png(replaced.image, cng).error == ImageResourceError::invalid_signature,
            "replace_png stays PNG-only");
    require(registry.find(replaced.image).has_value(), "refused replacement leaves the image");
    require(image_resource_error_name(ImageResourceError::invalid_image_data) == "invalid_image_data", "error names");
}

void test_decoding(const Bytes& rgba) {
    ImageRegistry registry;
    ImageResourceView native = found(registry, registry.load_image(cng_file(rgba, stx_cng::CngChannels::rgba)));
    ImageResourceView interchange = found(registry, registry.load_image(png_file(rgba, stx_png::PngSourceAlpha::straight)));
    Bytes opaque_rgba = rgba;
    for (std::size_t pixel = 0; pixel < opaque_rgba.size() / 4U; ++pixel) {
        opaque_rgba[pixel * 4U + 3U] = std::byte{255};
    }
    ImageResourceView opaque = found(registry, registry.load_image(cng_file(rgb_of(rgba), stx_cng::CngChannels::rgb)));
    std::unique_ptr<image::ResourcePixelDecoder> decoder = std::make_unique<image::ResourcePixelDecoder>();
    for (const image::PixelTarget& target : targets) {
        Bytes expected = reference(rgba, target);
        require(decode(*decoder, native, target) == expected, "CNG decode matches the reference");
        require(decode(*decoder, interchange, target) == expected, "PNG decode matches the reference");
        require(decode(*decoder, opaque, target) == reference(opaque_rgba, target), "RGB CNG decodes opaque");
    }
    (*decoder).release();
    require(decode(*decoder, native, targets[1]) == reference(rgba, targets[1]), "decoder works after release");

    Bytes small(static_cast<std::size_t>(width) * height * 4U - 1U);
    image::ResourcePixelResult refused = (*decoder).decode(native, targets[0], small, static_cast<std::size_t>(width) * 4U);
    require(refused.error == image::ResourcePixelError::insufficient_storage, "short destination refused");
    ImageResourceView pixels = found(registry, registry.load_bgra32_premultiplied(
        width, height, static_cast<std::uint64_t>(width) * 4U, reference(rgba, targets[3])));
    require(pixels.metadata.alpha == ImageAlpha::premultiplied, "BGRA resources are premultiplied");
    Bytes destination(small.size() + 1U);
    require((*decoder).decode(pixels, targets[0], destination, static_cast<std::size_t>(width) * 4U).error ==
                image::ResourcePixelError::unsupported_encoding,
            "BGRA resources are not decoded");

    // The CNG decoder refuses an overlapping destination before writing.
    Bytes file = cng_file(rgba, stx_cng::CngChannels::rgba);
    file.resize(static_cast<std::size_t>(width) * height * 4U + file.size());
    std::span<const std::byte> encoded(file.data(), file.size() - static_cast<std::size_t>(width) * height * 4U);
    std::span<std::byte> overlapping(file.data(), static_cast<std::size_t>(width) * height * 4U);
    image::CngPixelDecoder cng;
    require(cng.decode(encoded, targets[0], overlapping, static_cast<std::size_t>(width) * 4U).error ==
                image::CngPixelError::invalid_argument,
            "overlap refused");
}

} // namespace

int main() {
    try {
        Bytes rgba = rgba_samples();
        test_admission(rgba);
        test_decoding(rgba);
    } catch (const std::exception& error) {
        std::cerr << "resource_pixels_tests: " << error.what() << '\n';
        return 1;
    }
    std::cout << "resource_pixels_tests: pass\n";
    return 0;
}
