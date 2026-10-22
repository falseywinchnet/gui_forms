#pragma once

#include "color_conversion.hpp"
#include "png_decoder.hpp"

#include <cstddef>
#include <cstdint>
#include <span>

namespace gui_forms::image {

// PNG colour metadata, read into the codec-neutral ColorEncoding (stage I6; owner decision of
// 2026-10-20). Output is always sRGB. Chunk precedence follows the PNG
// specification (W3C PNG, third edition, 4.3): cICP, then iCCP, then sRGB,
// then cHRM and gAMA.
//
//   cICP     sRGB-equivalent values (primaries 1, transfer 13, matrix 0,
//            full range 1) are sRGB; any other values are refused
//            (unsupported_color_profile); a malformed chunk is refused
//            (invalid_color_metadata).
//   iCCP     decided by the purpose's IccProfileRule before this stage; an
//            ignored profile means the samples are sRGB.
//   sRGB     the samples are sRGB and pass through unchanged.
//   gAMA     decode gamma: linear = sample ^ (100000 / value).
//   cHRM     primaries and white point; converted to sRGB primaries, with
//            Bradford adaptation of the image white to D65.
//   neither  the samples are sRGB and pass through unchanged.
//
// gAMA without cHRM assumes sRGB primaries and D65. cHRM without gAMA assumes
// the sRGB transfer function. No "approximately sRGB" special case: gAMA
// 45455 with sRGB cHRM is converted literally.
//
// Degenerate values are ignored, as if the chunk were absent, and the
// result reports which: a gAMA value outside 16..625000000 (the range the
// previous libpng paths accepted), or a cHRM whose chromaticities have
// y = 0, x + y > 1, a singular primary matrix, or a white point outside the
// primaries' triangle. The PNG specification lets a decoder ignore an
// erroneous ancillary chunk; structural errors (length, order, duplicates,
// gAMA 0) are already refused by the decoder.

enum class PngColorError { none, unsupported_color_profile, invalid_color_metadata };

struct PngColorResolution final {
    PngColorError error = PngColorError::none;
    ColorEncoding encoding{};
};

// Reads the colour chunks named by records (from a successful decode of
// encoded) and applies the precedence above. icc_ignored states whether the
// caller's rule ignored an iCCP chunk; a refused profile never reaches here.
// A record outside encoded, or a gAMA/cHRM record of the wrong length, is
// refused as invalid_color_metadata rather than read.
[[nodiscard]] PngColorResolution resolve_png_color(std::span<const std::byte> encoded,
    std::span<const PngMetadataRecord> records, bool icc_ignored) noexcept;

} // namespace gui_forms::image
