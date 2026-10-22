#pragma once

#include "png_color.hpp"
#include "png_decoder.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <vector>

namespace gui_forms::image {

// The one PNG pixel path shared by every host and renderer. The owned decoder
// reconstructs straight-alpha RGBA16 samples; one kernel converts them to the
// four 8-bit channels each consumer stores. Nothing here calls a host codec.
//
// Colour: output is sRGB. iCCP follows icc_profile_rule; cICP, sRGB, gAMA
// and cHRM follow png_color.hpp (stage I6), which converts gAMA/cHRM images
// to sRGB before the 8-bit kernel.

// Memory order of the four 8-bit channels of one destination pixel.
enum class PixelOrder { rgba, bgra };
// Straight alpha keeps colour independent of coverage (interchange, PNG
// files); premultiplied scales every colour channel by alpha (compositing).
enum class PixelAlpha { straight, premultiplied };

struct PixelTarget final {
    PixelOrder order = PixelOrder::rgba;
    PixelAlpha alpha = PixelAlpha::premultiplied;
};

// Converts top-down straight-alpha RGBA16 samples to 8-bit pixels.
//
// Rounding rule: every output channel is the integer nearest the exact
// rational value, computed in unsigned 64-bit integers with no floating point.
//   straight colour and every alpha: (c * 255 + 32767) / 65535
//   premultiplied colour:            (c * a * 255 + 2147418112) / 4294836225
// where c and a are the 16-bit samples and 4294836225 = 65535 * 65535. Both
// denominators are odd and every numerator is an integer, so an exact half
// cannot occur and the rule needs no tie break. Eight-bit sources (stored as
// v * 257) convert back to exactly v; premultiplied 8-bit results equal
// round(v * alpha / 255).
//
// samples holds width * height * 4 live values. pixels holds at least
// row_bytes * (height - 1) + width * 4 bytes, and row_bytes >= width * 4;
// bytes between rows are left unchanged. The two spans must be disjoint.
// Returns false, writing nothing, when the shape or target is invalid.
[[nodiscard]] bool convert_rgba16_samples(std::span<const std::uint16_t> samples,
    std::uint32_t width, std::uint32_t height, PixelTarget target,
    std::span<std::byte> pixels, std::size_t row_bytes) noexcept;

// What an embedded ICC profile (iCCP) does to a pixel request. GUI.Forms has
// no ICC transform: refuse fails the request; ignore treats the samples as
// sRGB, so an interchange path still accepts the image.
enum class IccProfileRule { refuse, ignore };

// Who asks for pixels. Colour-profile policy belongs to the purpose, and
// icc_profile_rule is the one place that maps a purpose to its rule.
enum class PngPixelPurpose {
    // A registry image resource drawn by a renderer.
    resource,
    // GUI.Drawing's bitmap decode of caller-supplied PNG bytes.
    drawing_decode,
    // A PNG read from the system clipboard, written by another program.
    clipboard
};

// Owner decision D1, option 2 (2026-10-20): registry resources and GUI.Drawing
// decode refuse iCCP (unsupported_color_profile); clipboard reads ignore the
// profile and treat the samples as sRGB.
[[nodiscard]] IccProfileRule icc_profile_rule(PngPixelPurpose purpose) noexcept;

enum class PngPixelError {
    none, invalid_argument, decode_failed, unsupported_color_profile,
    invalid_color_metadata, insufficient_storage, allocation_failed
};

struct PngPixelRequest final {
    PngDecodeLimits limits{};
    PixelTarget target{};
    PngPixelPurpose purpose = PngPixelPurpose::resource;
};

struct PngPixelResult final {
    PngPixelError error = PngPixelError::none;
    // The decoder's own reason when error is decode_failed.
    PngDecodeError decode_error = PngDecodeError::none;
    compression::InflateError inflate_error = compression::InflateError::none;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
};

// Finite decode limits for an image another validator already admitted with
// these dimensions and encoded size: the image's own dimensions and pixel
// count, filtered bytes bounded by the 16-bit RGBA worst case including
// Adam7 filter bytes, the decoder's provisional metadata quota, and its
// provisional work quota, raised to four units per filtered byte when that
// is larger.
[[nodiscard]] PngDecodeLimits png_limits_for_image(std::uint32_t width,
    std::uint32_t height, std::size_t encoded_bytes) noexcept;

// Finite decode limits for an image of unknown dimensions from outside the
// toolkit (the clipboard): any width and height up to the PNG maximum whose
// product is at most maximum_pixels. No pass has more rows than the image has
// pixels, so 15 * maximum_pixels bounds the filtered bytes (eight bytes per
// 16-bit RGBA pixel plus at most seven filter bytes per image row).
[[nodiscard]] PngDecodeLimits png_limits_for_pixels(std::uint64_t maximum_pixels,
    std::size_t encoded_bytes) noexcept;

// Owns the inflate workspace and the reusable decode storage: two filter
// rows, the RGBA16 sample image, a fixed 4 KiB metadata scratch and one
// record per ancillary chunk. Compressed metadata is validated through the
// scratch and discarded, never stored. Storage grows to the largest image
// decoded and stays until release() or destruction, so a caller that decodes
// many images keeps one decoder for the batch and lets it go afterwards.
// Peak storage for a W x H image is about 8 * W * H bytes of samples beside
// the caller's 4 * W * H destination. One execution context per decoder.
class PngPixelDecoder final {
public:
    PngPixelDecoder();
    ~PngPixelDecoder();
    PngPixelDecoder(const PngPixelDecoder&) = delete;
    PngPixelDecoder& operator=(const PngPixelDecoder&) = delete;
    PngPixelDecoder(PngPixelDecoder&&) = delete;
    PngPixelDecoder& operator=(PngPixelDecoder&&) = delete;

    // Validates the file's structure and reports its dimensions, so the
    // caller can size its destination. Applies the purpose's iCCP rule.
    // No inflate and no storage.
    [[nodiscard]] PngPixelResult inspect(std::span<const std::byte> encoded,
                                         const PngPixelRequest& request) const noexcept;

    // Decodes into caller pixels (layout as convert_rgba16_samples states).
    // encoded and pixels must be disjoint. On failure the destination may
    // hold a partial, unverified prefix: discard it.
    [[nodiscard]] PngPixelResult decode(std::span<const std::byte> encoded,
        const PngPixelRequest& request, std::span<std::byte> pixels,
        std::size_t row_bytes) noexcept;

    // Frees the retained storage; the next decode allocates again.
    void release() noexcept;

private:
    std::unique_ptr<compression::InflateStream> workspace_;
    std::vector<std::byte> rows_;
    std::vector<std::uint16_t> samples_;
    std::vector<std::byte> scratch_;
    std::vector<PngMetadataRecord> records_;
    PngColorWorkspace color_work_;
};

} // namespace gui_forms::image
