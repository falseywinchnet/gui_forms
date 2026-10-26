#include "cng_pixels.hpp"

#include "stx_cng/codec.hpp"

#include <cstdint>
#include <functional>
#include <new>

namespace gui_forms::image {

class CngPixelStorage final {
public:
    stx_cng::CngDecoder decoder;
};

namespace {

bool overlaps(std::span<const std::byte> left, std::span<const std::byte> right) noexcept {
    if (left.empty() || right.empty()) {
        return false;
    }
    std::less<const std::byte*> before{};
    const std::byte* left_end = left.data() + left.size();
    const std::byte* right_end = right.data() + right.size();
    bool overlap = before(left.data(), right_end) && before(right.data(), left_end);
    return overlap;
}

ImageResourceError framing_error(stx_cng::CngError error) noexcept {
    switch (error) {
    case stx_cng::CngError::none: return ImageResourceError::none;
    case stx_cng::CngError::invalid_signature: return ImageResourceError::invalid_signature;
    case stx_cng::CngError::truncated: return ImageResourceError::truncated_chunk;
    case stx_cng::CngError::trailing_data: return ImageResourceError::trailing_data;
    // A checksum, or a redundant copy that differs from the one it repeats.
    case stx_cng::CngError::crc_mismatch:
    case stx_cng::CngError::copy_mismatch: return ImageResourceError::crc_mismatch;
    case stx_cng::CngError::invalid_header:
    case stx_cng::CngError::unsupported_version: return ImageResourceError::invalid_image_header;
    // Contract errors framing validation does not raise, and the parts'
    // contents: the image's data is not what the format allows.
    case stx_cng::CngError::invalid_argument:
    case stx_cng::CngError::insufficient_storage:
    case stx_cng::CngError::overlap:
    case stx_cng::CngError::invalid_directory:
    case stx_cng::CngError::invalid_tables:
    case stx_cng::CngError::invalid_segment:
    case stx_cng::CngError::mosaic_unavailable:
    case stx_cng::CngError::invalid_collection: return ImageResourceError::invalid_image_data;
    case stx_cng::CngError::allocation_failed: return ImageResourceError::allocation_failed;
    }
    // A code outside the enumeration is reported as invalid data.
    return ImageResourceError::invalid_image_data;
}

// round(c * a / 255): 255 is odd, so c * a / 255 is never half way and
// adding 127 before the division rounds to nearest.
std::uint8_t premultiply(std::uint8_t colour, std::uint8_t alpha) noexcept {
    unsigned product = static_cast<unsigned>(colour) * alpha + 127U;
    std::uint8_t result = static_cast<std::uint8_t>(product / 255U);
    return result;
}

// Decoded BGRA rows, in place, to the target: alpha premultiplied, then red
// and blue exchanged for RGBA. Not called for the BGRA straight target,
// which the decode already is.
template<PixelOrder order, PixelAlpha alpha>
void finish_rows(std::byte* pixels, std::uint32_t width, std::uint32_t height, std::size_t row_bytes) noexcept {
    constexpr std::size_t red_slot = order == PixelOrder::rgba ? 0U : 2U;
    constexpr std::size_t blue_slot = 2U - red_slot;
    for (std::size_t y = 0; y < height; ++y) {
        std::byte* pixel = pixels + y * row_bytes;
        for (std::size_t x = 0; x < width; ++x) {
            std::uint8_t blue = std::to_integer<std::uint8_t>(pixel[0]);
            std::uint8_t green = std::to_integer<std::uint8_t>(pixel[1]);
            std::uint8_t red = std::to_integer<std::uint8_t>(pixel[2]);
            if constexpr (alpha == PixelAlpha::premultiplied) {
                std::uint8_t coverage = std::to_integer<std::uint8_t>(pixel[3]);
                blue = premultiply(blue, coverage);
                green = premultiply(green, coverage);
                red = premultiply(red, coverage);
            }
            pixel[red_slot] = static_cast<std::byte>(red);
            pixel[1] = static_cast<std::byte>(green);
            pixel[blue_slot] = static_cast<std::byte>(blue);
            pixel += 4;
        }
    }
}

// Decoded BGR rows, each in the first width * 3 bytes of its row, widened in
// place to four bytes per pixel with alpha 255 (opaque: premultiplication
// changes nothing). Each row runs from its last pixel back: pixel x is read
// from bytes [3x, 3x + 3) before [4x, 4x + 4) is written, and every pixel
// still to come lies below 3x, so nothing is overwritten before it is read.
template<PixelOrder order>
void widen_rows(std::byte* pixels, std::uint32_t width, std::uint32_t height, std::size_t row_bytes) noexcept {
    constexpr std::size_t red_slot = order == PixelOrder::rgba ? 0U : 2U;
    constexpr std::size_t blue_slot = 2U - red_slot;
    for (std::size_t y = 0; y < height; ++y) {
        std::byte* row = pixels + y * row_bytes;
        for (std::size_t remaining = width; remaining > 0; --remaining) {
            std::size_t x = remaining - 1;
            std::byte blue = row[3 * x];
            std::byte green = row[3 * x + 1];
            std::byte red = row[3 * x + 2];
            std::byte* pixel = row + 4 * x;
            pixel[red_slot] = red;
            pixel[1] = green;
            pixel[blue_slot] = blue;
            pixel[3] = std::byte{255};
        }
    }
}

// The pass after decoding, chosen once for the file's channels and the
// target.
void finish_pixels(stx_cng::CngChannels channels, PixelTarget target, std::byte* pixels, std::uint32_t width,
                   std::uint32_t height, std::size_t row_bytes) noexcept {
    bool rgba = target.order == PixelOrder::rgba;
    bool premultiplied = target.alpha == PixelAlpha::premultiplied;
    if (channels == stx_cng::CngChannels::bgr) {
        if (rgba) {
            widen_rows<PixelOrder::rgba>(pixels, width, height, row_bytes);
        } else {
            widen_rows<PixelOrder::bgra>(pixels, width, height, row_bytes);
        }
    } else if (rgba && premultiplied) {
        finish_rows<PixelOrder::rgba, PixelAlpha::premultiplied>(pixels, width, height, row_bytes);
    } else if (rgba) {
        finish_rows<PixelOrder::rgba, PixelAlpha::straight>(pixels, width, height, row_bytes);
    } else if (premultiplied) {
        finish_rows<PixelOrder::bgra, PixelAlpha::premultiplied>(pixels, width, height, row_bytes);
    }
}

} // namespace

CngFraming validate_cng_framing(std::span<const std::byte> encoded) noexcept {
    stx_cng::CngInspectResult validated = stx_cng::validate_cng_framing(encoded);
    CngFraming framing;
    framing.error = framing_error(validated.error);
    if (framing.error == ImageResourceError::none) {
        framing.width = validated.image.width;
        framing.height = validated.image.height;
        framing.alpha = validated.image.channels == stx_cng::CngChannels::bgra;
    }
    return framing;
}

CngPixelDecoder::CngPixelDecoder() noexcept {}

CngPixelDecoder::~CngPixelDecoder() {}

void CngPixelDecoder::release() noexcept {
    storage_.reset();
}

CngPixelResult CngPixelDecoder::decode(std::span<const std::byte> encoded, PixelTarget target,
                                       std::span<std::byte> pixels, std::size_t row_bytes) noexcept {
    CngPixelResult result;
    if (!valid_pixel_target(target) || overlaps(encoded, pixels)) {
        result.error = CngPixelError::invalid_argument;
        return result;
    }
    stx_cng::CngInspectResult inspected = stx_cng::inspect_cng(encoded);
    if (inspected.error != stx_cng::CngError::none) {
        result.error = CngPixelError::decode_failed;
        return result;
    }
    result.width = inspected.image.width;
    result.height = inspected.image.height;
    if (!valid_pixel_destination(result.width, result.height, pixels.size(), row_bytes)) {
        result.error = CngPixelError::insufficient_storage;
        return result;
    }
    if (!storage_) {
        try {
            storage_ = std::make_unique<CngPixelStorage>();
        } catch (const std::bad_alloc&) {
            result.error = CngPixelError::allocation_failed;
            return result;
        }
    }
    // Four bytes per destination pixel hold the file's three or four, so the
    // codec writes the destination rows directly.
    stx_cng::CngDecodeResult decoded = (*storage_).decoder.decode(encoded, pixels, row_bytes, 1);
    if (decoded.error == stx_cng::CngError::allocation_failed) {
        result.error = CngPixelError::allocation_failed;
        return result;
    }
    if (decoded.error != stx_cng::CngError::none) {
        result.error = CngPixelError::decode_failed;
        return result;
    }
    finish_pixels(inspected.image.channels, target, pixels.data(), result.width, result.height, row_bytes);
    result.error = CngPixelError::none;
    return result;
}

} // namespace gui_forms::image
