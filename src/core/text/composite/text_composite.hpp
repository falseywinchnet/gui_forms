#pragma once

#include "gui_forms/types.hpp"

#include <cstddef>
#include <cstdint>
#include <span>

namespace gui_forms::detail {

// ADR-004 shared text blending. Every host composites text coverage through
// this one integer arithmetic, so the same coverage, colour, clip and
// destination give the same bytes on every platform and architecture.

// Memory order of the four 8-bit channels of each 32-bit pixel. Pixels are
// premultiplied; the hosts are little-endian.
enum class TextPixelOrder : std::uint8_t { bgra, rgba };

struct TextCompositeTarget final {
    std::span<std::uint32_t> pixels{};
    std::uint32_t width{};
    std::uint32_t height{};
    // Pixels per row, at least width.
    std::size_t stride{};
    TextPixelOrder order{TextPixelOrder::bgra};
    // True for surfaces whose alpha channel is not maintained (GDI DIBs):
    // the result is written fully opaque, as before ADR-004.
    bool opaque{};
};

// Clip in device pixels. Edges at fractional positions give partial coverage
// equal to the covered area of the pixel; a rounded clip's corners give
// coverage from the pixel centre's signed distance to the arc. All coverage
// is computed in 1/256-pixel fixed point.
struct TextCompositeClip final {
    Rect rect{};
    bool rounded{};
    Rect rounded_rect{};
    double radius{};
};

// Grayscale coverage, 0..255, row-major with stride == width, placed with its
// top-left pixel at (left, top) in the target's device pixels.
struct TextCoverage final {
    std::span<const std::uint8_t> pixels{};
    std::uint32_t width{};
    std::uint32_t height{};
    std::int64_t left{};
    std::int64_t top{};
};

enum class TextCompositeStatus : std::uint8_t { success, invalid_geometry };

// source_over blends colour (alpha included) over the destination. source
// (GDI+ SourceCopy) replaces the destination with the premultiplied colour,
// alpha included, in proportion to coverage x clip. The two agree for opaque
// colours.
enum class TextCompositeMode : std::uint8_t { source_over, source };

// Blends colour over the target with alpha = coverage * colour.alpha * clip,
// rounded once, and each channel rounded once:
//   out = (source * alpha + destination * (255 - alpha) + 127) / 255.
// Validates everything before writing; no allocation, no partial write on
// failure. Coverage and target must not overlap.
[[nodiscard]] TextCompositeStatus composite_text_coverage(TextCoverage coverage, TextCompositeTarget target,
                                                          TextCompositeClip clip, Color color,
                                                          TextCompositeMode mode = TextCompositeMode::source_over) noexcept;

// Clip coverage of one device pixel, 0..256. Exposed for tests.
[[nodiscard]] std::uint32_t text_clip_coverage(TextCompositeClip clip, std::int64_t x, std::int64_t y) noexcept;

// The same blend for the theme engine's part rasters (T2 plan, "One blend";
// GIVEN A2-2 and C1-1). A part raster is premultiplied RGBA8, memory order
// red, green, blue, alpha (PartRasterView), placed with its top-left pixel
// at (left, top) in the target's device pixels, one raster pixel per device
// pixel.
struct PartRasterSource final {
    std::span<const std::uint8_t> pixels{};
    std::uint32_t width{};
    std::uint32_t height{};
    // Bytes from one row to the next, at least width * 4.
    std::size_t row_bytes{};
    std::int64_t left{};
    std::int64_t top{};
};

// A part opacity in [0, 1] as the byte the blend reads: floor(o * 255 + 0.5),
// the engine's one quantization rule. NaN is 0; values outside [0, 1] are
// clamped.
[[nodiscard]] std::uint8_t part_composite_opacity(double opacity) noexcept;

// Premultiplied source-over of the raster onto the target, weighted by
// opacity (0..255) times clip coverage (0..256). With w that weight and s a
// source channel (alpha included), each channel is rounded twice:
//   s' = (s * w + 32640) / 65280
//   out = s' + (destination * (255 - alpha') + 127) / 255
// where alpha' is s' of the source alpha; an opaque target is written with
// alpha 255. A fully opaque raster at full opacity and full coverage is
// copied exactly. The result never exceeds 255 for a premultiplied source,
// and is clamped for one that is not. Validates everything before writing;
// no allocation, no partial write on failure. Source and target must not
// overlap.
[[nodiscard]] TextCompositeStatus composite_part_raster(PartRasterSource source, TextCompositeTarget target,
                                                        TextCompositeClip clip, std::uint8_t opacity) noexcept;

} // namespace gui_forms::detail
