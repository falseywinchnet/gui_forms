#pragma once

#include "gui_forms/resources/types/image_resource_types.hpp"
#include "pixel_target.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>

namespace gui_forms::image {

// CNG, the native resource image format (stx_cng), in GUI.Forms' own
// vocabulary. This header names no stx_cng or Zstandard type, and
// cng_pixels.cpp is the only GUI.Forms source that includes stx_cng. CNG
// files hold 8-bit sRGB samples, RGB or RGBA with straight alpha, and carry
// no colour metadata, so there is no colour-profile policy to apply.

// The registry's admission check of a CNG file: the signature, the header
// and every chunk's framing and checksum (stx_cng::validate_cng_framing),
// without decoding.
struct CngFraming final {
    ImageResourceError error{ImageResourceError::invalid_signature};
    std::uint32_t width{};
    std::uint32_t height{};
    // Four channels (straight alpha) rather than three (opaque).
    bool alpha{};
};

// invalid_signature when the bytes are not a CNG file; the other refusals
// are truncated_chunk, invalid_chunk_type, crc_mismatch, trailing_data,
// dimension_limit_exceeded (outside CNG's own extent), invalid_image_header
// and invalid_image_data.
[[nodiscard]] CngFraming validate_cng_framing(std::span<const std::byte> encoded) noexcept;

enum class CngPixelError : std::uint8_t {
    none, invalid_argument, decode_failed, insufficient_storage, allocation_failed
};

struct CngPixelResult final {
    CngPixelError error{CngPixelError::none};
    std::uint32_t width{};
    std::uint32_t height{};
};

class CngPixelStorage;

// Decodes CNG files to 8-bit pixels of a PixelTarget. Owns the stx_cng
// decoder and an 8-bit sample image (width * height * channels bytes); both
// grow to the largest image decoded and stay until release() or
// destruction. One execution context per decoder.
class CngPixelDecoder final {
public:
    CngPixelDecoder() noexcept;
    ~CngPixelDecoder();
    CngPixelDecoder(const CngPixelDecoder&) = delete;
    CngPixelDecoder& operator=(const CngPixelDecoder&) = delete;
    CngPixelDecoder(CngPixelDecoder&&) = delete;
    CngPixelDecoder& operator=(CngPixelDecoder&&) = delete;

    // Decodes into caller pixels (layout as valid_pixel_destination states):
    // colour is premultiplied as round(c * a / 255) when the target asks for
    // it, as on the PNG path. encoded and pixels must be disjoint. The file
    // is decoded into the owned samples first, so a failure writes nothing.
    [[nodiscard]] CngPixelResult decode(std::span<const std::byte> encoded, PixelTarget target,
                                        std::span<std::byte> pixels, std::size_t row_bytes) noexcept;

    // Frees the retained storage; the next decode allocates again.
    void release() noexcept;

private:
    std::unique_ptr<CngPixelStorage> storage_;
};

} // namespace gui_forms::image
