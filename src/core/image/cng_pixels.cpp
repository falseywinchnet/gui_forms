#include "cng_pixels.hpp"

#include "stx_cng/codec.hpp"

#include <cstdint>
#include <functional>
#include <limits>
#include <new>
#include <stdexcept>
#include <vector>

namespace gui_forms::image {

class CngPixelStorage final {
public:
    stx_cng::CngDecoder decoder;
    // Tightly packed 8-bit samples of the last image decoded; capacity is the
    // largest image's width * height * channels.
    std::vector<std::byte> samples;
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
    case stx_cng::CngError::invalid_chunk: return ImageResourceError::invalid_chunk_type;
    case stx_cng::CngError::crc_mismatch: return ImageResourceError::crc_mismatch;
    case stx_cng::CngError::trailing_data: return ImageResourceError::trailing_data;
    case stx_cng::CngError::unsupported_dimensions: return ImageResourceError::dimension_limit_exceeded;
    case stx_cng::CngError::invalid_header: return ImageResourceError::invalid_image_header;
    default: return ImageResourceError::invalid_image_data;
    }
}

// round(c * a / 255): 255 is odd, so c * a / 255 is never half way and
// adding 127 before the division rounds to nearest.
std::uint8_t premultiply(std::uint8_t colour, std::uint8_t alpha) noexcept {
    unsigned product = static_cast<unsigned>(colour) * alpha + 127U;
    std::uint8_t result = static_cast<std::uint8_t>(product / 255U);
    return result;
}

// Tight samples of `channels` bytes per pixel to four-byte pixels of the
// given order and alpha, row by row; bytes between destination rows are
// left as they were.
template<std::size_t channels, PixelOrder order, PixelAlpha alpha>
void convert_rows(const std::byte* samples, std::uint32_t width, std::uint32_t height, std::byte* pixels,
                  std::size_t row_bytes) noexcept {
    constexpr std::size_t red_slot = order == PixelOrder::rgba ? 0U : 2U;
    constexpr std::size_t blue_slot = 2U - red_slot;
    std::size_t source_row_bytes = static_cast<std::size_t>(width) * channels;
    for (std::size_t y = 0; y < height; ++y) {
        const std::byte* source = samples + y * source_row_bytes;
        std::byte* destination = pixels + y * row_bytes;
        for (std::size_t x = 0; x < width; ++x) {
            std::uint8_t red = std::to_integer<std::uint8_t>(source[0]);
            std::uint8_t green = std::to_integer<std::uint8_t>(source[1]);
            std::uint8_t blue = std::to_integer<std::uint8_t>(source[2]);
            std::uint8_t coverage = 255U;
            if constexpr (channels == 4U) {
                coverage = std::to_integer<std::uint8_t>(source[3]);
                if constexpr (alpha == PixelAlpha::premultiplied) {
                    red = premultiply(red, coverage);
                    green = premultiply(green, coverage);
                    blue = premultiply(blue, coverage);
                }
            }
            destination[red_slot] = static_cast<std::byte>(red);
            destination[1] = static_cast<std::byte>(green);
            destination[blue_slot] = static_cast<std::byte>(blue);
            destination[3] = static_cast<std::byte>(coverage);
            source += channels;
            destination += 4;
        }
    }
}

template<std::size_t channels>
void convert_samples(const std::byte* samples, std::uint32_t width, std::uint32_t height, PixelTarget target,
                     std::byte* pixels, std::size_t row_bytes) noexcept {
    bool rgba = target.order == PixelOrder::rgba;
    bool premultiplied = target.alpha == PixelAlpha::premultiplied;
    if (rgba && premultiplied) {
        convert_rows<channels, PixelOrder::rgba, PixelAlpha::premultiplied>(samples, width, height, pixels, row_bytes);
    } else if (rgba) {
        convert_rows<channels, PixelOrder::rgba, PixelAlpha::straight>(samples, width, height, pixels, row_bytes);
    } else if (premultiplied) {
        convert_rows<channels, PixelOrder::bgra, PixelAlpha::premultiplied>(samples, width, height, pixels, row_bytes);
    } else {
        convert_rows<channels, PixelOrder::bgra, PixelAlpha::straight>(samples, width, height, pixels, row_bytes);
    }
}

// Grows `samples` to `count` bytes, reserving exactly that capacity; false
// when allocation fails, leaving it as it was.
bool grow_samples(std::vector<std::byte>& samples, std::size_t count) noexcept {
    if (samples.size() >= count) {
        return true;
    }
    try {
        samples.reserve(count);
        samples.resize(count);
    } catch (const std::bad_alloc&) {
        return false;
    } catch (const std::length_error&) {
        return false;
    }
    return true;
}

} // namespace

CngFraming validate_cng_framing(std::span<const std::byte> encoded) noexcept {
    stx_cng::CngInspectResult validated = stx_cng::validate_cng_framing(encoded);
    CngFraming framing;
    framing.error = framing_error(validated.error);
    if (framing.error == ImageResourceError::none) {
        framing.width = validated.image.width;
        framing.height = validated.image.height;
        framing.alpha = validated.image.channels == stx_cng::CngChannels::rgba;
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
    CngPixelStorage& storage = *storage_;
    // Admitted CNG extents (at most 2^26 pixels of four bytes) fit size_t.
    std::size_t sample_bytes = stx_cng::cng_sample_bytes(inspected.image);
    if (!grow_samples(storage.samples, sample_bytes)) {
        result.error = CngPixelError::allocation_failed;
        return result;
    }
    std::span<std::byte> samples(storage.samples.data(), sample_bytes);
    stx_cng::CngDecodeResult decoded = storage.decoder.decode(encoded, samples);
    if (decoded.error == stx_cng::CngError::allocation_failed) {
        result.error = CngPixelError::allocation_failed;
        return result;
    }
    if (decoded.error != stx_cng::CngError::none) {
        result.error = CngPixelError::decode_failed;
        return result;
    }
    if (inspected.image.channels == stx_cng::CngChannels::rgba) {
        convert_samples<4U>(samples.data(), result.width, result.height, target, pixels.data(), row_bytes);
    } else {
        convert_samples<3U>(samples.data(), result.width, result.height, target, pixels.data(), row_bytes);
    }
    result.error = CngPixelError::none;
    return result;
}

} // namespace gui_forms::image
