#pragma once

#include "png_decoder.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace gui_forms::image {

// Colour handling for decoded PNG samples (stage I6; owner decision of
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

enum class PngColorSource {
    // No colour chunk: sRGB by default.
    srgb_default,
    srgb_chunk,
    srgb_cicp,
    // iCCP ignored by the request's rule: treated as sRGB.
    icc_ignored,
    // gAMA and/or cHRM present and used.
    gamma_chromaticity
};

enum class PngColorError { none, unsupported_color_profile, invalid_color_metadata };

struct PngColorEncoding final {
    PngColorSource source = PngColorSource::srgb_default;
    bool has_gamma = false;
    // gAMA's stored value: image gamma times 100000.
    std::uint32_t gamma = 0;
    bool has_chromaticities = false;
    // cHRM's stored values times 100000: white x, y; red x, y; green x, y; blue x, y.
    std::array<std::uint32_t, 8> chromaticities{};
    bool ignored_gamma = false;
    bool ignored_chromaticities = false;

    [[nodiscard]] bool converts() const noexcept {
        const bool result = source == PngColorSource::gamma_chromaticity;
        return result;
    }
};

struct PngColorResolution final {
    PngColorError error = PngColorError::none;
    PngColorEncoding encoding{};
};

// Reads the colour chunks named by records (from a successful decode of
// encoded) and applies the precedence above. icc_ignored states whether the
// caller's rule ignored an iCCP chunk; a refused profile never reaches here.
[[nodiscard]] PngColorResolution resolve_png_color(std::span<const std::byte> encoded,
    std::span<const PngMetadataRecord> records, bool icc_ignored) noexcept;

// Converts straight-alpha RGBA16 samples in place to sRGB for an encoding
// that converts(); alpha is unchanged. Pipeline, in doubles with contraction
// off and the engine's deterministic power (no host libm):
//   1. decode: s = sample / 65535; linear = s ^ (100000 / gAMA), or the sRGB
//      decoding curve when gAMA is absent or ignored;
//   2. gamut: rgb = M * linear, M = inverse(sRGB primaries) * Bradford(white
//      -> D65) * (image primaries); M is the identity when cHRM is absent or
//      ignored, and the matrix step is skipped;
//   3. clip each channel to [0, 1];
//   4. encode: the 16-bit code whose sRGB-decoded midpoint thresholds bracket
//      the linear value, that is round(65535 * sRGB_encode(linear)), found by
//      binary search over 65535 precomputed thresholds.
// Rounding rule: nearest 16-bit code, a value exactly on a threshold taking
// the upper code. Error bound: within one 16-bit code of the exact result
// (engine_power's 2^-49 relative error moves a value across a threshold only
// within about 1e-13 of it); the subsequent 8-bit conversion rounds again.
// work is reusable scratch (lookup tables) owned by the caller.
struct PngColorWorkspace final {
    // Decoded linear value per possible sample, 256 entries for 8-bit and
    // lower depths (every sample is a multiple of 257), 65536 for 16-bit.
    std::vector<double> linear{};
    // Output code per sample when there is no matrix step.
    std::vector<std::uint16_t> direct{};
};
[[nodiscard]] bool convert_png_samples_to_srgb(const PngColorEncoding& encoding,
    unsigned depth, std::span<std::uint16_t> samples, PngColorWorkspace& work);

} // namespace gui_forms::image
