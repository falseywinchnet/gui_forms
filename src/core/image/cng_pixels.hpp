#pragma once

#include "gui_forms/resources/types/image_resource_types.hpp"
#include "pixel_target.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>

namespace gui_forms::image {

// CNG 1.0, the native resource image format (stx_cng), in GUI.Forms' own
// vocabulary. This header names no stx_cng or Zstandard type, and
// cng_pixels.cpp is the only GUI.Forms source that includes stx_cng. CNG
// files hold 8-bit sRGB samples in blue, green, red(, alpha) order with
// straight alpha, and carry no colour metadata, so there is no
// colour-profile policy to apply.

// The registry's admission check of a CNG file, without decoding
// (stx_cng::validate_cng_framing): the signature, the front header and
// directory, the exact length they imply, every checksum, and both copies
// of every redundant part identical. A file with one damaged copy, which
// the format's own decoder would still read, is not admitted.
struct CngFraming final {
    ImageResourceError error{ImageResourceError::invalid_signature};
    std::uint32_t width{};
    std::uint32_t height{};
    // Four channels (straight alpha) rather than three (opaque).
    bool alpha{};
};

// invalid_signature when the bytes are not a CNG file; the other refusals
// are truncated_chunk (shorter than the header implies), trailing_data,
// crc_mismatch (a checksum, or a second copy that differs from the first),
// invalid_image_header (fields outside the format, CNG's own extent of 2^31
// samples included, or another version) and invalid_image_data. The
// registry applies its own dimension limits afterwards.
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

// Decodes CNG files to 8-bit pixels of a PixelTarget, on the calling
// thread. Owns the stx_cng decoder, whose storage depends on the segment
// size, not the image, and stays until release() or destruction. One
// execution context per decoder.
class CngPixelDecoder final {
public:
    CngPixelDecoder() noexcept;
    ~CngPixelDecoder();
    CngPixelDecoder(const CngPixelDecoder&) = delete;
    CngPixelDecoder& operator=(const CngPixelDecoder&) = delete;
    CngPixelDecoder(CngPixelDecoder&&) = delete;
    CngPixelDecoder& operator=(CngPixelDecoder&&) = delete;

    // Decodes into caller pixels (layout as valid_pixel_destination states).
    // The codec writes the rows itself: a BGRA file is the BGRA straight
    // target with no further pass; a BGR file is widened to four bytes in
    // place, alpha 255. Colour is premultiplied in place as round(c * a /
    // 255) when the target asks for it, as on the PNG path, and an RGBA
    // target swaps red and blue in the same pass. encoded and pixels must
    // be disjoint. Bytes between rows are left as they were. On failure the
    // pixels hold an unverified partial decode: discard them.
    [[nodiscard]] CngPixelResult decode(std::span<const std::byte> encoded, PixelTarget target,
                                        std::span<std::byte> pixels, std::size_t row_bytes) noexcept;

    // Frees the retained storage; the next decode allocates again.
    void release() noexcept;

private:
    std::unique_ptr<CngPixelStorage> storage_;
};

} // namespace gui_forms::image
