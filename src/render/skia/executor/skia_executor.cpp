#include "skia_executor.hpp"

#include "include/core/SkBlendMode.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColorFilter.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkData.h"
#include "include/core/SkImage.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkPathBuilder.h"
#include "include/core/SkPixmap.h"
#include "include/core/SkSamplingOptions.h"
#include "include/core/SkSurface.h"
#include "include/effects/SkColorMatrixFilter.h"
#include "include/effects/SkDashPathEffect.h"
#include "include/effects/SkGradient.h"
#if defined(__APPLE__)
#include "core_text_font_bytes.hpp"
#endif

#include "../../../core/image/png_pixels.hpp"
#include "../../../core/image/png_writer.hpp"
#include "../../../core/text/composite/text_composite.hpp"
#include "../../text/glyph_run/glyph_run_raster.hpp"
#include "../../text/harfbuzz/harfbuzz_font_engine.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_TRUETYPE_TABLES_H

#include <algorithm>
#include <array>
#include <optional>
#include <cmath>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace gui_drawing::render {
namespace {

[[nodiscard]] double pixel_font_size(const FontSnapshot& font) noexcept {
    constexpr double dpi = 96.0;
    switch (font.unit) {
    case GraphicsUnit::display: return font.size * dpi / 75.0;
    case GraphicsUnit::point: return font.size * dpi / 72.0;
    case GraphicsUnit::inch: return font.size * dpi;
    case GraphicsUnit::document: return font.size * dpi / 300.0;
    case GraphicsUnit::millimeter: return font.size * dpi / 25.4;
    case GraphicsUnit::world:
    case GraphicsUnit::pixel: return font.size;
    }
    return font.size;
}

[[nodiscard]] std::string ascii_lower(std::string_view value) {
    std::string result(value);
    for (char& entry : result) {
        if (entry >= 'A' && entry <= 'Z') {
            entry = static_cast<char>(entry + ('a' - 'A'));
        }
    }
    return result;
}

[[nodiscard]] std::size_t first_utf8_codepoint_size(std::string_view text) noexcept {
    if (text.empty()) return 0U;
    const unsigned char first = static_cast<unsigned char>(text.front());
    if ((first & 0x80U) == 0U) return 1U;
    if ((first & 0xe0U) == 0xc0U) return std::min<std::size_t>(2U, text.size());
    if ((first & 0xf0U) == 0xe0U) return std::min<std::size_t>(3U, text.size());
    return std::min<std::size_t>(4U, text.size());
}

[[nodiscard]] SkColor to_sk_color(Color color) noexcept {
    return color.is_empty() ? SK_ColorTRANSPARENT :
        SkColorSetARGB(color.alpha(), color.red(), color.green(), color.blue());
}

[[nodiscard]] std::uint8_t premultiply(std::uint8_t channel,
                                       std::uint8_t alpha) noexcept {
    return static_cast<std::uint8_t>(
        (static_cast<unsigned>(channel) * alpha + 127U) / 255U);
}

[[nodiscard]] std::uint8_t unpremultiply(std::uint8_t channel,
                                         std::uint8_t alpha) noexcept {
    if (alpha == 0U) return 0U;
    return static_cast<std::uint8_t>(std::min(
        255U, (static_cast<unsigned>(channel) * 255U + alpha / 2U) / alpha));
}

void assign_raster_error(RasterError* output, RasterError value) noexcept {
    if (output != nullptr) *output = value;
}

template <std::size_t Count>
void mark_hatch_pixel(std::array<SkColor, Count>& pixels,
                      int extent, int x, int y, SkColor color) noexcept {
    pixels[static_cast<std::size_t>(y * extent + x)] = color;
}

[[nodiscard]] std::vector<std::byte> remapped_pixels(
    const ImageSnapshot& image, const ImageAttributesSnapshot& attributes) {
    std::vector<std::byte> result(image.pixels().begin(), image.pixels().end());
    if (attributes.remap_table.empty()) return result;
    for (std::size_t offset = 0; offset + 3U < result.size(); offset += 4U) {
        const std::uint8_t first = std::to_integer<std::uint8_t>(result[offset]);
        const std::uint8_t green = std::to_integer<std::uint8_t>(result[offset + 1U]);
        const std::uint8_t third = std::to_integer<std::uint8_t>(result[offset + 2U]);
        const std::uint8_t alpha = std::to_integer<std::uint8_t>(result[offset + 3U]);
        const std::uint8_t red = image.pixel_format == PixelFormat::bgra32_premultiplied
            ? third : first;
        const std::uint8_t blue = image.pixel_format == PixelFormat::bgra32_premultiplied
            ? first : third;
        const Color source = Color::from_argb(
            alpha, unpremultiply(red, alpha), unpremultiply(green, alpha),
            unpremultiply(blue, alpha));
        std::vector<ImageAttributesSnapshot::ColorRemap>::const_iterator found =
            attributes.remap_table.begin();
        while (found != attributes.remap_table.end() &&
               (*found).old_color.argb() != source.argb()) {
            ++found;
        }
        if (found == attributes.remap_table.end()) continue;
        const Color replacement = (*found).new_color;
        const std::uint8_t out_alpha = replacement.alpha();
        const std::uint8_t out_red = premultiply(replacement.red(), out_alpha);
        const std::uint8_t out_green = premultiply(replacement.green(), out_alpha);
        const std::uint8_t out_blue = premultiply(replacement.blue(), out_alpha);
        if (image.pixel_format == PixelFormat::bgra32_premultiplied) {
            result[offset] = static_cast<std::byte>(out_blue);
            result[offset + 2U] = static_cast<std::byte>(out_red);
        } else {
            result[offset] = static_cast<std::byte>(out_red);
            result[offset + 2U] = static_cast<std::byte>(out_blue);
        }
        result[offset + 1U] = static_cast<std::byte>(out_green);
        result[offset + 3U] = static_cast<std::byte>(out_alpha);
    }
    return result;
}

[[nodiscard]] SkRect to_sk_rect(RectF rectangle) noexcept {
    return SkRect::MakeXYWH(static_cast<SkScalar>(rectangle.x),
                            static_cast<SkScalar>(rectangle.y),
                            static_cast<SkScalar>(rectangle.width),
                            static_cast<SkScalar>(rectangle.height));
}

[[nodiscard]] SkImageInfo image_info(std::uint32_t width, std::uint32_t height,
                                     PixelFormat format) {
    return SkImageInfo::Make(
        static_cast<int>(width), static_cast<int>(height),
        format == PixelFormat::bgra32_premultiplied ?
            kBGRA_8888_SkColorType : kRGBA_8888_SkColorType,
        kPremul_SkAlphaType, SkColorSpace::MakeSRGB());
}

[[nodiscard]] SkMatrix to_sk_matrix(const Matrix& matrix) {
    SkMatrix result;
    result.setAll(static_cast<SkScalar>(matrix.m11()),
                  static_cast<SkScalar>(matrix.m21()),
                  static_cast<SkScalar>(matrix.dx()),
                  static_cast<SkScalar>(matrix.m12()),
                  static_cast<SkScalar>(matrix.m22()),
                  static_cast<SkScalar>(matrix.dy()),
                  0.0F, 0.0F, 1.0F);
    return result;
}

[[nodiscard]] SkPath to_sk_path(const PathSnapshot& source) {
    SkPathBuilder builder(source.fill_mode == FillMode::alternate ?
        SkPathFillType::kEvenOdd : SkPathFillType::kWinding);
    bool has_current_point = false;
    PointF current_point{};
    for (const PathElement& element : source.elements) {
        switch (element.verb) {
        case PathVerb::start_figure:
            has_current_point = false;
            break;
        case PathVerb::line:
            if (!has_current_point || current_point != element.first) {
                builder.moveTo(static_cast<SkScalar>(element.first.x),
                               static_cast<SkScalar>(element.first.y));
            }
            builder.lineTo(static_cast<SkScalar>(element.second.x),
                           static_cast<SkScalar>(element.second.y));
            current_point = element.second;
            has_current_point = true;
            break;
        case PathVerb::quadratic:
            if (!has_current_point || current_point != element.first) {
                builder.moveTo(static_cast<SkScalar>(element.first.x),
                               static_cast<SkScalar>(element.first.y));
            }
            builder.quadTo(static_cast<SkScalar>(element.second.x),
                           static_cast<SkScalar>(element.second.y),
                           static_cast<SkScalar>(element.third.x),
                           static_cast<SkScalar>(element.third.y));
            current_point = element.third;
            has_current_point = true;
            break;
        case PathVerb::bezier:
            if (!has_current_point || current_point != element.first) {
                builder.moveTo(static_cast<SkScalar>(element.first.x),
                               static_cast<SkScalar>(element.first.y));
            }
            builder.cubicTo(static_cast<SkScalar>(element.second.x),
                            static_cast<SkScalar>(element.second.y),
                            static_cast<SkScalar>(element.third.x),
                            static_cast<SkScalar>(element.third.y),
                            static_cast<SkScalar>(element.fourth.x),
                            static_cast<SkScalar>(element.fourth.y));
            current_point = element.fourth;
            has_current_point = true;
            break;
        case PathVerb::rectangle:
            builder.addRect(to_sk_rect(element.rect));
            has_current_point = false;
            break;
        case PathVerb::ellipse:
            builder.addOval(to_sk_rect(element.rect));
            has_current_point = false;
            break;
        case PathVerb::arc:
            // GDI+ GraphicsPath.AddArc extends the current figure. Skia's
            // addArc starts a fresh contour, which left rounded rectangles as
            // four disconnected corner outlines with an empty centre. arcTo
            // preserves the implicit connecting edges used by WinForms custom
            // controls while still starting a contour for the first arc.
            builder.arcTo(to_sk_rect(element.rect),
                          static_cast<SkScalar>(element.start_angle),
                          static_cast<SkScalar>(element.sweep_angle),
                          !has_current_point);
            {
                constexpr double degrees_to_radians =
                    0.01745329251994329576923690768489;
                const double angle =
                    (element.start_angle + element.sweep_angle) *
                    degrees_to_radians;
                current_point = {
                    element.rect.x + element.rect.width * 0.5 +
                        element.rect.width * 0.5 * std::cos(angle),
                    element.rect.y + element.rect.height * 0.5 +
                        element.rect.height * 0.5 * std::sin(angle),
                };
            }
            has_current_point = true;
            break;
        case PathVerb::close_figure:
            builder.close();
            has_current_point = false;
            break;
        }
    }
    return builder.detach();
}

void apply_image_attributes(SkPaint& paint,
                            const ImageAttributesSnapshot& attributes) {
    if (!attributes.has_color_matrix) return;
    float matrix[20]{};
    for (std::size_t output = 0; output < 4U; ++output) {
        for (std::size_t input = 0; input < 4U; ++input) {
            matrix[output * 5U + input] = static_cast<float>(
                attributes.color_matrix[input * 5U + output]);
        }
        matrix[output * 5U + 4U] = static_cast<float>(
            attributes.color_matrix[20U + output] * 255.0);
    }
    paint.setColorFilter(SkColorFilters::Matrix(matrix));
}

[[nodiscard]] SkSamplingOptions sampling_for(const GraphicsState& state) {
    return state.interpolation == InterpolationMode::nearest ?
        SkSamplingOptions(SkFilterMode::kNearest) :
        SkSamplingOptions(SkFilterMode::kLinear);
}

void configure_paint(SkPaint& paint, const GraphicsState& state) {
    paint.setAntiAlias(state.smoothing != SmoothingMode::none &&
                       state.smoothing != SmoothingMode::high_speed);
    paint.setBlendMode(state.compositing == CompositingMode::source_copy ?
        SkBlendMode::kSrc : SkBlendMode::kSrcOver);
}

// The on/off lengths of a preset dash style for a pen `width` wide; none
// for a solid line or a custom pattern (the pen's own).
std::vector<SkScalar> preset_dash_pattern(DashStyle style, SkScalar width) {
    switch (style) {
    case DashStyle::dash: return {3.0F * width, width};
    case DashStyle::dot: return {width, width};
    case DashStyle::dash_dot: return {3.0F * width, width, width, width};
    case DashStyle::dash_dot_dot: return {3.0F * width, width, width, width, width, width};
    case DashStyle::solid: return {};
    case DashStyle::custom: return {};
    }
    // A style outside the enumeration draws solid.
    return {};
}

void configure_pen(SkPaint& paint, const PenSnapshot& pen,
                   const GraphicsState& state) {
    configure_paint(paint, state);
    paint.setColor(to_sk_color(pen.color));
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(static_cast<SkScalar>(pen.width));
    std::vector<SkScalar> pattern;
    if (pen.dash_style == DashStyle::custom) {
        pattern.reserve(pen.dash_pattern.size());
        for (const double entry : pen.dash_pattern) {
            pattern.push_back(static_cast<SkScalar>(entry));
        }
    } else {
        pattern = preset_dash_pattern(pen.dash_style, static_cast<SkScalar>(pen.width));
    }
    if (pattern.size() >= 2U && pattern.size() % 2U == 0U) {
        paint.setPathEffect(SkDashPathEffect::Make(
            SkSpan<const SkScalar>(pattern.data(), pattern.size()), 0.0F));
    }
}

[[nodiscard]] SkTileMode tile_mode(WrapMode mode) noexcept {
    switch (mode) {
    case WrapMode::tile: return SkTileMode::kRepeat;
    case WrapMode::tile_flip_x:
    case WrapMode::tile_flip_y:
    case WrapMode::tile_flip_xy: return SkTileMode::kMirror;
    case WrapMode::clamp: return SkTileMode::kClamp;
    }
    return SkTileMode::kClamp;
}

[[nodiscard]] SkTileMode texture_tile_mode_x(WrapMode mode) noexcept {
    return mode == WrapMode::tile_flip_x || mode == WrapMode::tile_flip_xy
        ? SkTileMode::kMirror
        : mode == WrapMode::clamp ? SkTileMode::kClamp : SkTileMode::kRepeat;
}

[[nodiscard]] SkTileMode texture_tile_mode_y(WrapMode mode) noexcept {
    return mode == WrapMode::tile_flip_y || mode == WrapMode::tile_flip_xy
        ? SkTileMode::kMirror
        : mode == WrapMode::clamp ? SkTileMode::kClamp : SkTileMode::kRepeat;
}

void configure_brush(SkPaint& paint, const BrushSnapshot& brush,
                     const GraphicsState& state) {
    configure_paint(paint, state);
    paint.setStyle(SkPaint::kFill_Style);
    if (brush.kind == BrushKind::solid) {
        paint.setColor(to_sk_color(brush.primary));
        return;
    }
    if (brush.kind == BrushKind::hatch) {
        constexpr int extent = 8;
        std::array<SkColor, extent * extent> pixels;
        pixels.fill(to_sk_color(brush.secondary));
        const SkColor hatch_color = to_sk_color(brush.primary);
        for (int coordinate = 0; coordinate < extent; ++coordinate) {
            switch (brush.hatch_style) {
            case HatchStyle::horizontal:
                mark_hatch_pixel(pixels, extent, coordinate, 3, hatch_color);
                break;
            case HatchStyle::vertical:
                mark_hatch_pixel(pixels, extent, 3, coordinate, hatch_color);
                break;
            case HatchStyle::forward_diagonal:
                mark_hatch_pixel(pixels, extent, coordinate,
                                 extent - 1 - coordinate, hatch_color);
                break;
            case HatchStyle::backward_diagonal:
                mark_hatch_pixel(pixels, extent, coordinate, coordinate,
                                 hatch_color);
                break;
            case HatchStyle::cross:
                mark_hatch_pixel(pixels, extent, coordinate, 3, hatch_color);
                mark_hatch_pixel(pixels, extent, 3, coordinate, hatch_color);
                break;
            case HatchStyle::diagonal_cross:
                mark_hatch_pixel(pixels, extent, coordinate, coordinate,
                                 hatch_color);
                mark_hatch_pixel(pixels, extent, coordinate,
                                 extent - 1 - coordinate, hatch_color);
                break;
            }
        }
        const SkImageInfo info = SkImageInfo::MakeN32Premul(extent, extent);
        sk_sp<SkData> data = SkData::MakeWithCopy(pixels.data(), sizeof(pixels));
        sk_sp<SkImage> image = SkImages::RasterFromData(
            info, std::move(data), extent * sizeof(SkColor));
        if (image) {
            paint.setShader((*image).makeShader(
                SkTileMode::kRepeat, SkTileMode::kRepeat,
                SkSamplingOptions(SkFilterMode::kNearest)));
        }
        return;
    }
    if (brush.kind == BrushKind::texture) {
        if (!brush.image.has_pixels()) {
            paint.setColor(SK_ColorTRANSPARENT);
            return;
        }
        const SkImageInfo info = image_info(
            brush.image.width, brush.image.height, brush.image.pixel_format);
        sk_sp<SkData> data = SkData::MakeWithCopy(
            brush.image.pixels().data(), brush.image.pixels().size());
        sk_sp<SkImage> image = SkImages::RasterFromData(
            info, std::move(data), brush.image.row_bytes());
        if (!image) {
            paint.setColor(SK_ColorTRANSPARENT);
            return;
        }
        const SkMatrix local = to_sk_matrix(brush.transform);
        paint.setShader((*image).makeShader(
            texture_tile_mode_x(brush.wrap_mode),
            texture_tile_mode_y(brush.wrap_mode),
            sampling_for(state), local));
        return;
    }

    std::vector<SkColor4f> colors;
    std::vector<float> positions;
    colors.reserve(brush.colors.size());
    positions.reserve(brush.positions.size());
    for (const Color color : brush.colors) {
        colors.push_back(SkColor4f::FromColor(to_sk_color(color)));
    }
    for (const double position : brush.positions) {
        positions.push_back(static_cast<SkScalar>(position));
    }
    if (colors.size() < 2U || colors.size() != positions.size()) {
        paint.setColor(to_sk_color(brush.primary));
        return;
    }
    const SkGradient::Colors gradient_colors(
        SkSpan<const SkColor4f>(colors.data(), colors.size()),
        SkSpan<const float>(positions.data(), positions.size()),
        tile_mode(brush.wrap_mode), SkColorSpace::MakeSRGB());
    const SkGradient gradient(gradient_colors, SkGradient::Interpolation{});
    if (brush.kind == BrushKind::linear_gradient) {
        constexpr double degrees_to_radians = 0.017453292519943295769;
        const double radians = brush.angle * degrees_to_radians;
        const double half_length = std::abs(std::cos(radians)) * brush.bounds.width / 2.0 +
            std::abs(std::sin(radians)) * brush.bounds.height / 2.0;
        const PointF center{brush.bounds.x + brush.bounds.width / 2.0,
                            brush.bounds.y + brush.bounds.height / 2.0};
        const SkPoint points[] = {
            {static_cast<SkScalar>(center.x - std::cos(radians) * half_length),
             static_cast<SkScalar>(center.y - std::sin(radians) * half_length)},
            {static_cast<SkScalar>(center.x + std::cos(radians) * half_length),
             static_cast<SkScalar>(center.y + std::sin(radians) * half_length)},
        };
        paint.setShader(SkShaders::LinearGradient(points, gradient));
        return;
    }
    double radius{};
    for (const PointF point : brush.points) {
        radius = std::max(radius, std::hypot(point.x - brush.center.x,
                                             point.y - brush.center.y));
    }
    if (radius <= 0.0) {
        paint.setColor(to_sk_color(brush.primary));
        return;
    }
    paint.setShader(SkShaders::RadialGradient(
        {static_cast<SkScalar>(brush.center.x),
         static_cast<SkScalar>(brush.center.y)},
        static_cast<SkScalar>(radius), gradient));
}

class BitmapUnlock final {
public:
    BitmapUnlock(Bitmap& bitmap, std::uint64_t token)
        : bitmap_(&bitmap), token_(token) {}
    ~BitmapUnlock() {
        if (bitmap_ != nullptr) {
            try { (*bitmap_).unlock(token_); } catch (...) {}
        }
    }
    void release() {
        (*bitmap_).unlock(token_);
        bitmap_ = nullptr;
    }

private:
    Bitmap* bitmap_;
    std::uint64_t token_;
};

} // namespace

namespace text = gui_forms::render::text;

namespace {

// Vertical metrics per em, unrounded, from the face's tables as FreeType
// reads them (hhea, falling back to OS/2).
struct FaceMetrics final {
    double ascent{};
    double descent{};
    double line_gap{};
};

// One encoded face of a family: the shared bytes, the collection index and
// its metrics.
struct FaceRecord final {
    std::shared_ptr<const std::vector<std::byte>> bytes;
    std::uint32_t index{};
    FaceMetrics metrics{};
};

// A family's faces, regular and bold, with a private HarfBuzz engine that
// shapes them and a rasterizer over the same bytes (ADR-004). Declared in
// that order so the rasterizer is destroyed before the engine it borrows.
struct TextFamily final {
    std::array<std::optional<FaceRecord>, 2> faces;
    // True when the faces came from the host catalog, not register_typeface.
    bool located{};
    std::unique_ptr<text::HarfBuzzFontEngine> engine;
    std::unique_ptr<text::GlyphRunRasterizer> rasterizer;
};

constexpr std::uint16_t regular_weight = 400U;
constexpr std::uint16_t bold_weight = 700U;
constexpr std::size_t maximum_font_bytes = 64U * 1024U * 1024U;

// A font request resolved to one registered face and the styles synthesized
// for it.
struct ResolvedFont final {
    TextFamily* family{};
    gui_forms::FontSpec spec{};
    text::GlyphRunSynthesis synthesis{};
    FaceMetrics metrics{};
};

class FreeTypeLibrary final {
public:
    FreeTypeLibrary() {
        if (FT_Init_FreeType(&library_) != 0) throw std::runtime_error("FreeType initialization failed");
    }
    ~FreeTypeLibrary() { FT_Done_FreeType(library_); }
    FreeTypeLibrary(const FreeTypeLibrary&) = delete;
    FreeTypeLibrary& operator=(const FreeTypeLibrary&) = delete;
    [[nodiscard]] FT_Library get() const noexcept { return library_; }

private:
    FT_Library library_{};
};

class FreeTypeFace final {
public:
    FreeTypeFace(FT_Library library, std::span<const std::byte> bytes, FT_Long index) {
        status_ = FT_New_Memory_Face(library, reinterpret_cast<const FT_Byte*>(bytes.data()),
                                     static_cast<FT_Long>(bytes.size()), index, &face_);
    }
    ~FreeTypeFace() {
        if (status_ == 0) FT_Done_Face(face_);
    }
    FreeTypeFace(const FreeTypeFace&) = delete;
    FreeTypeFace& operator=(const FreeTypeFace&) = delete;
    // Nullptr when the bytes are not a font or the index is out of range.
    [[nodiscard]] FT_Face get() const noexcept { return status_ == 0 ? face_ : nullptr; }

private:
    FT_Face face_{};
    FT_Error status_{};
};

[[nodiscard]] int face_weight(FT_Face face) {
    const TT_OS2* const os2 = static_cast<const TT_OS2*>(FT_Get_Sfnt_Table(face, FT_SFNT_OS2));
    if (os2 != nullptr && (*os2).usWeightClass != 0U) return static_cast<int>((*os2).usWeightClass);
    const int weight = ((*face).style_flags & FT_STYLE_FLAG_BOLD) != 0 ? bold_weight : regular_weight;
    return weight;
}

[[nodiscard]] FaceMetrics face_metrics(FT_Face face) noexcept {
    FaceMetrics result{};
    const double units = (*face).units_per_EM == 0U ? 1000.0 : static_cast<double>((*face).units_per_EM);
    result.ascent = static_cast<double>((*face).ascender) / units;
    result.descent = -static_cast<double>((*face).descender) / units;
    const double height = static_cast<double>((*face).height) / units;
    result.line_gap = std::max(0.0, height - result.ascent - result.descent);
    return result;
}

// Device clip for text: the command clip snapped to whole pixels as Skia's
// non-antialiased clipRect does for every other command, or the target.
[[nodiscard]] gui_forms::Rect text_clip_rect(const GraphicsState& state, const std::uint32_t width,
                                             const std::uint32_t height) noexcept {
    gui_forms::Rect result{0.0, 0.0, static_cast<double>(width), static_cast<double>(height)};
    if (!state.clip) return result;
    const RectF clip = *state.clip;
    const double left = std::floor(clip.x + 0.5);
    const double top = std::floor(clip.y + 0.5);
    const double right = std::floor(clip.x + clip.width + 0.5);
    const double bottom = std::floor(clip.y + clip.height + 0.5);
    result = {left, top, std::max(0.0, right - left), std::max(0.0, bottom - top)};
    return result;
}

} // namespace

class SkiaExecutor::Impl final {
public:
    FreeTypeLibrary library;
    // Keyed by ASCII-lowercased family name.
    std::unordered_map<std::string, std::unique_ptr<TextFamily>> families;
#if defined(__APPLE__)
    // Family and bold bit already asked of the host catalog.
    std::unordered_set<std::string> located_requests;
#endif

    // Chooses the collection face best matching family and weight, scanning
    // at most 64 faces. Nullopt when the bytes hold no usable face.
    [[nodiscard]] std::optional<FaceRecord> select_face(std::string_view family,
                                                        std::shared_ptr<const std::vector<std::byte>> bytes,
                                                        const int requested_weight) const {
        const std::string requested_family = ascii_lower(family);
        std::optional<FaceRecord> best;
        int best_score = std::numeric_limits<int>::min();
        const FreeTypeFace collection(library.get(), *bytes, -1);
        if (collection.get() == nullptr) return best;
        const FT_Long count = std::min<FT_Long>((*collection.get()).num_faces, 64);
        for (FT_Long index = 0; index < count; ++index) {
            const FreeTypeFace candidate(library.get(), *bytes, index);
            const FT_Face face = candidate.get();
            if (face == nullptr || !FT_IS_SCALABLE(face)) continue;
            const bool exact_family = (*face).family_name != nullptr &&
                ascii_lower((*face).family_name) == requested_family;
            const int weight_distance = std::abs(face_weight(face) - requested_weight);
            const int score = (exact_family ? 100000 : 0) - weight_distance;
            if (score > best_score) {
                best_score = score;
                best = FaceRecord{bytes, static_cast<std::uint32_t>(index), face_metrics(face)};
            }
            if (exact_family && weight_distance == 0) break;
        }
        return best;
    }

    // Stores a face in a family's regular or bold slot and rebuilds the
    // family's engine from its records, so a replaced face takes effect.
    void install(const std::string& key, const std::size_t slot, FaceRecord record, const bool located) {
        std::unique_ptr<TextFamily>& entry = families[key];
        if (!entry) entry = std::make_unique<TextFamily>();
        TextFamily& family = *entry;
        family.faces[slot] = std::move(record);
        family.located = located;
        family.rasterizer.reset();
        family.engine = std::make_unique<text::HarfBuzzFontEngine>();
        for (std::size_t index = 0; index < family.faces.size(); ++index) {
            const std::optional<FaceRecord>& face = family.faces[index];
            if (!face) continue;
            const std::uint16_t weight = index == 1U ? bold_weight : regular_weight;
            if (!(*family.engine).register_shared_typeface(gui_forms::FontRole::control, weight, false,
                                                          (*face).bytes, (*face).index)) {
                throw std::runtime_error("font registration failed");
            }
        }
        family.rasterizer = std::make_unique<text::GlyphRunRasterizer>(*family.engine);
    }

    [[nodiscard]] TextFamily* registered(std::string_view family) {
        using FamilyMap =
            std::unordered_map<std::string, std::unique_ptr<TextFamily>>;
        const FamilyMap::iterator found = families.find(ascii_lower(family));
        TextFamily* const result = found == families.end() ? nullptr : (*found).second.get();
        return result;
    }

    // Asks the host catalog for a family nobody registered. Only macOS has
    // one here; Windows registers GDI's bytes before drawing.
    [[nodiscard]] TextFamily* locate(std::string_view family, const bool bold) {
#if defined(__APPLE__)
        const std::string key = ascii_lower(family);
        if (family.empty() || !located_requests.insert(key + (bold ? "\x1f" "1" : "\x1f" "0")).second) {
            return registered(family);
        }
        std::optional<std::vector<std::byte>> located = core_text_font_bytes(family, bold, maximum_font_bytes);
        if (!located) return registered(family);
        const std::shared_ptr<const std::vector<std::byte>> owner =
            std::make_shared<const std::vector<std::byte>>(std::move(*located));
        const FreeTypeFace face(library.get(), *owner, 0);
        if (face.get() == nullptr || !FT_IS_SCALABLE(face.get())) return registered(family);
        // The face CoreText chose may not be the requested weight; its own
        // weight decides the slot.
        const std::size_t slot = face_weight(face.get()) >= 600 ? 1U : 0U;
        install(key, slot, FaceRecord{owner, 0U, face_metrics(face.get())}, true);
        return registered(family);
#else
        static_cast<void>(bold);
        return registered(family);
#endif
    }

    // Registered family, then the host catalog, then Portsmouth Rapids.
    // Bold without a bold face emboldens; italic always shears (only the
    // bold bit selects registered faces).
    [[nodiscard]] std::optional<ResolvedFont> resolve(const FontSnapshot& spec) {
        const bool bold = (spec.style & 1U) != 0U;
        TextFamily* family = registered(spec.family);
        if (family == nullptr || ((*family).located && !(*family).faces[bold ? 1U : 0U])) {
            family = locate(spec.family, bold);
        }
        if (family == nullptr) family = registered("Portsmouth Rapids");
        std::optional<ResolvedFont> result;
        if (family == nullptr) return result;
        std::size_t slot = bold && (*family).faces[1] ? 1U : 0U;
        if (!(*family).faces[slot]) slot = 1U - slot;
        const double size = pixel_font_size(spec);
        if (!std::isfinite(size) || size <= 0.0) throw std::invalid_argument("font size is invalid");
        ResolvedFont resolved{};
        resolved.family = family;
        resolved.spec = gui_forms::FontSpec{gui_forms::FontRole::control, size, slot == 1U ? bold_weight : regular_weight, false};
        resolved.synthesis.embolden = bold && slot == 0U;
        resolved.synthesis.oblique = (spec.style & 2U) != 0U;
        resolved.metrics = (*(*family).faces[slot]).metrics;
        result = resolved;
        return result;
    }

    void draw_text(const DrawingCommand& command, const gui_forms::detail::TextCompositeTarget& target);
};

// Shapes and draws one draw_string command through the shared text engine
// and compositor (ADR-004).
void SkiaExecutor::Impl::draw_text(const DrawingCommand& command,
                                   const gui_forms::detail::TextCompositeTarget& target) {
    const bool source = command.state.compositing == CompositingMode::source_copy;
    const Color colour = command.color;
    const std::uint8_t alpha = colour.is_empty() ? 0U : colour.alpha();
    if (alpha == 0U && !source) return;
    const std::optional<ResolvedFont> resolved = resolve(command.font);
    if (!resolved) return;
    const ResolvedFont& font = *resolved;
    TextFamily& family = *font.family;
    const double size = font.spec.size;
    const double overhang = (command.format.flags & UINT32_C(0x0004)) == 0U ? size / 6.0 : 0.0;
    const double first_baseline = command.first.y + font.metrics.ascent * size + overhang;
    const double line_height = std::max(0.0, (font.metrics.ascent + font.metrics.descent + font.metrics.line_gap) * size +
                                                 overhang);
    const Matrix& matrix = command.state.transform;
    const bool uniform = matrix.m12() == 0.0 && matrix.m21() == 0.0 && matrix.m11() == matrix.m22() &&
        std::isfinite(matrix.m11()) && matrix.m11() > 0.0;
    const gui_forms::detail::TextCompositeClip clip{text_clip_rect(command.state, target.width, target.height)};
    gui_forms::Color text_colour = gui_forms::Color::rgba(0, 0, 0, 0);
    if (!colour.is_empty()) text_colour = gui_forms::Color::rgba(colour.red(), colour.green(), colour.blue(), alpha);
    const gui_forms::detail::TextCompositeMode mode =
        source ? gui_forms::detail::TextCompositeMode::source : gui_forms::detail::TextCompositeMode::source_over;
    std::string_view remaining = command.text;
    std::size_t line_index{};
    for (;;) {
        const std::size_t newline = remaining.find('\n');
        std::string_view line = remaining.substr(0U, newline);
        if (!line.empty() && line.back() == '\r') line.remove_suffix(1U);
        if (!line.empty()) {
            const text::ShapedText shaped = (*family.engine).shape(line, font.spec);
            double x = command.first.x + overhang;
            if (command.format.alignment == StringAlignment::center) x -= shaped.width / 2.0;
            if (command.format.alignment == StringAlignment::far) x -= shaped.width;
            const double y = first_baseline + line_height * static_cast<double>(line_index);
            const double device_x = matrix.m11() * x + matrix.m21() * y + matrix.dx();
            const double device_y = matrix.m12() * x + matrix.m22() * y + matrix.dy();
            text::GlyphRunOrigin origin{};
            text::GlyphRunCoverage coverage{};
            if (uniform) {
                origin = text::glyph_run_origin(device_x, device_y);
                coverage = (*family.rasterizer).rasterize(shaped, font.spec, matrix.m11(), origin.phase_x, font.synthesis);
            } else {
                const text::GlyphRunTransform linear{matrix.m11(), matrix.m21(), matrix.m12(), matrix.m22()};
                coverage = (*family.rasterizer).rasterize_transformed(shaped, font.spec, linear, device_x, device_y,
                                                                     font.synthesis, origin);
            }
            const gui_forms::detail::TextCoverage placed{coverage.pixels, coverage.width, coverage.height,
                                                         origin.pixel_x + coverage.left, origin.pixel_y + coverage.top};
            if (gui_forms::detail::composite_text_coverage(placed, target, clip, text_colour, mode) !=
                gui_forms::detail::TextCompositeStatus::success) {
                throw std::invalid_argument("text placement is outside the supported range");
            }
        }
        if (newline == std::string_view::npos) break;
        remaining.remove_prefix(newline + 1U);
        ++line_index;
    }
}


SkiaExecutor::SkiaExecutor()
    : impl_(std::make_unique<Impl>()), owner_thread_(std::this_thread::get_id()) {}
SkiaExecutor::~SkiaExecutor() = default;

bool SkiaExecutor::register_typeface(std::string_view family,
                                     std::span<const std::byte> encoded,
                                     std::uint32_t style) {
    if (!owner_thread() || family.empty() || family.size() > 4096U || encoded.empty() ||
        encoded.size() > maximum_font_bytes) {
        return false;
    }
    try {
        const std::shared_ptr<const std::vector<std::byte>> bytes =
            std::make_shared<const std::vector<std::byte>>(encoded.begin(), encoded.end());
        const bool bold = (style & 1U) != 0U;
        // Collections (TTC/OTC) hold several faces; choose the requested
        // family and weight instead of silently using face zero.
        std::optional<FaceRecord> record = (*impl_).select_face(family, bytes, bold ? bold_weight : regular_weight);
        if (!record) return false;
        (*impl_).install(ascii_lower(family), bold ? 1U : 0U, std::move(*record), false);
    } catch (const std::exception&) {
        return false;
    }
    return true;
}

SizeF SkiaExecutor::measure_string(std::string_view utf8,
                                  const FontSnapshot& font_spec,
                                  const StringFormatSnapshot& format,
                                  double layout_width) {
    if (!owner_thread() || !std::isfinite(layout_width) || layout_width < 0.0) {
        throw std::invalid_argument("text measurement arguments are invalid");
    }
    if (utf8.empty()) return {};
    const std::optional<ResolvedFont> resolved = (*impl_).resolve(font_spec);
    if (!resolved) return {};
    const ResolvedFont& font = *resolved;
    text::HarfBuzzFontEngine& engine = *(*font.family).engine;
    const double size = font.spec.size;
    const bool fit_black_box = (format.flags & UINT32_C(0x0004)) != 0U;
    const double overhang = fit_black_box ? 0.0 : size / 6.0;
    const double line_height = std::max(0.0,
        (font.metrics.ascent + font.metrics.descent + font.metrics.line_gap) * size + overhang);
    const bool wrap = layout_width > 0.0 && (format.flags & UINT32_C(0x1000)) == 0U;
    const double content_width = wrap ? std::max(0.0, layout_width - overhang * 2.0) : 0.0;
    double maximum_width{};
    std::size_t line_count{};
    std::size_t offset{};
    do {
        const std::size_t newline = utf8.find('\n', offset);
        const std::size_t end = newline == std::string_view::npos ? utf8.size() : newline;
        std::string_view line = utf8.substr(offset, end - offset);
        if (!line.empty() && line.back() == '\r') line.remove_suffix(1U);
        if (!wrap || line.empty()) {
            const double width = line.empty() ? 0.0 : engine.shape(line, font.spec).width;
            maximum_width = std::max(maximum_width, width);
            ++line_count;
        } else {
            while (!line.empty()) {
                // Advance per code point: each glyph's advance belongs to the
                // code point starting its cluster (left-to-right lines).
                const text::ShapedText shaped = engine.shape(line, font.spec);
                std::vector<double> advance_at(line.size(), 0.0);
                for (const text::ShapedFontRun& run : shaped.runs) {
                    for (const text::ShapedGlyph& glyph : run.glyphs) {
                        const std::size_t cluster = glyph.cluster.value();
                        if (cluster < advance_at.size()) advance_at[cluster] += static_cast<double>(glyph.advance_x);
                    }
                }
                std::vector<std::size_t> byte_ends;
                std::vector<double> widths;
                for (std::string_view rest = line; !rest.empty();) {
                    const std::size_t start = line.size() - rest.size();
                    const std::size_t count = first_utf8_codepoint_size(rest);
                    byte_ends.push_back(start + count);
                    widths.push_back(advance_at[start]);
                    rest.remove_prefix(count);
                }
                double width{};
                std::size_t fitted{};
                while (fitted < widths.size() && width + widths[fitted] <= content_width) {
                    width += widths[fitted++];
                }
                if (fitted == 0U) {
                    fitted = 1U;
                    width = widths.empty() ? 0.0 : widths.front();
                }
                std::size_t consumed = byte_ends[std::min(fitted, byte_ends.size()) - 1U];
                if (consumed < line.size()) {
                    const std::size_t space = line.substr(0U, consumed).find_last_of(" \t");
                    if (space != std::string_view::npos && space != 0U) {
                        consumed = space;
                        width = engine.shape(line.substr(0U, consumed), font.spec).width;
                    }
                }
                maximum_width = std::max(maximum_width, std::min(content_width, width));
                ++line_count;
                line.remove_prefix(consumed);
                while (!line.empty() && (line.front() == ' ' || line.front() == '\t')) {
                    line.remove_prefix(1U);
                }
            }
        }
        if (newline == std::string_view::npos) break;
        offset = newline + 1U;
    } while (offset <= utf8.size());
    return {wrap ? std::min(layout_width, maximum_width + overhang * 2.0) :
                   maximum_width + overhang * 2.0,
            line_height * static_cast<double>(line_count)};
}

RasterResult SkiaExecutor::execute(const GraphicsRecorder& recorder,
                                   Bitmap& target,
                                   std::size_t first_command) {
    if (!owner_thread()) return {RasterError::wrong_thread};
    try {
        const std::span<const DrawingCommand> commands = recorder.commands();
        if (first_command > commands.size()) return {RasterError::invalid_argument};
        if (first_command == commands.size()) return {};
        std::unique_ptr<Bitmap> scratch = target.clone({0, 0, static_cast<std::int32_t>(target.width()),
                                      static_cast<std::int32_t>(target.height())});
        const BitmapLockView scratch_lock = (*scratch).lock(BitmapLockMode::write);
        BitmapUnlock unlock_scratch(*scratch, scratch_lock.token);
        const SkImageInfo info = image_info(scratch_lock.width, scratch_lock.height,
                                            scratch_lock.pixel_format);
        sk_sp<SkSurface> surface = SkSurfaces::WrapPixels(
            info, scratch_lock.writable_data, scratch_lock.row_bytes);
        if (!surface) return {RasterError::target_unavailable};
        SkCanvas* canvas = (*surface).getCanvas();
        // Text composites straight into the pixels Skia draws into; raster
        // canvases draw immediately, so command order is preserved.
        if (scratch_lock.row_bytes % sizeof(std::uint32_t) != 0U) return {RasterError::target_unavailable};
        const std::size_t text_stride = scratch_lock.row_bytes / sizeof(std::uint32_t);
        gui_forms::detail::TextCompositeTarget text_target{};
        text_target.pixels = std::span<std::uint32_t>(reinterpret_cast<std::uint32_t*>(scratch_lock.writable_data),
                                                      text_stride * scratch_lock.height);
        text_target.width = scratch_lock.width;
        text_target.height = scratch_lock.height;
        text_target.stride = text_stride;
        text_target.order = scratch_lock.pixel_format == PixelFormat::bgra32_premultiplied ?
            gui_forms::detail::TextPixelOrder::bgra : gui_forms::detail::TextPixelOrder::rgba;
        std::size_t executed{};
        for (const DrawingCommand& command : commands.subspan(first_command)) {
            (*canvas).restoreToCount(1);
            (*canvas).resetMatrix();
            if (command.state.clip) {
                (*canvas).clipRect(to_sk_rect(*command.state.clip),
                                 SkClipOp::kIntersect, false);
            }
            (*canvas).setMatrix(to_sk_matrix(command.state.transform));

            SkPaint paint;
            configure_paint(paint, command.state);
            switch (command.kind) {
            case CommandKind::save:
            case CommandKind::restore:
            case CommandKind::translate:
            case CommandKind::set_transform:
            case CommandKind::set_clip:
            case CommandKind::reset_clip:
            case CommandKind::set_quality:
                // State commands: their effect is in command.state, applied above.
                break;
            case CommandKind::clear:
                (*canvas).clear(to_sk_color(command.color));
                break;
            case CommandKind::fill_rectangle:
                configure_brush(paint, command.brush, command.state);
                (*canvas).drawRect(to_sk_rect(command.rect), paint);
                break;
            case CommandKind::draw_rectangle:
                configure_pen(paint, command.pen, command.state);
                (*canvas).drawRect(to_sk_rect(command.rect), paint);
                break;
            case CommandKind::draw_line:
                configure_pen(paint, command.pen, command.state);
                (*canvas).drawLine(static_cast<SkScalar>(command.first.x),
                                 static_cast<SkScalar>(command.first.y),
                                 static_cast<SkScalar>(command.second.x),
                                 static_cast<SkScalar>(command.second.y), paint);
                break;
            case CommandKind::draw_string:
                (*impl_).draw_text(command, text_target);
                break;
            case CommandKind::draw_ellipse:
                configure_pen(paint, command.pen, command.state);
                (*canvas).drawOval(to_sk_rect(command.rect), paint);
                break;
            case CommandKind::fill_ellipse:
                configure_brush(paint, command.brush, command.state);
                (*canvas).drawOval(to_sk_rect(command.rect), paint);
                break;
            case CommandKind::fill_polygon: {
                SkPathBuilder builder(command.path.fill_mode == FillMode::alternate ?
                    SkPathFillType::kEvenOdd : SkPathFillType::kWinding);
                builder.moveTo(static_cast<SkScalar>(command.points.front().x),
                               static_cast<SkScalar>(command.points.front().y));
                for (std::size_t index = 1; index < command.points.size(); ++index) {
                    builder.lineTo(static_cast<SkScalar>(command.points[index].x),
                                   static_cast<SkScalar>(command.points[index].y));
                }
                builder.close();
                configure_brush(paint, command.brush, command.state);
                (*canvas).drawPath(builder.detach(), paint);
                break;
            }
            case CommandKind::draw_path:
                configure_pen(paint, command.pen, command.state);
                (*canvas).drawPath(to_sk_path(command.path), paint);
                break;
            case CommandKind::fill_path:
                configure_brush(paint, command.brush, command.state);
                (*canvas).drawPath(to_sk_path(command.path), paint);
                break;
            case CommandKind::draw_image: {
                if (!command.image.has_pixels()) {
                    return {RasterError::missing_image_pixels, executed};
                }
                const SkImageInfo source_info = image_info(
                    command.image.width, command.image.height,
                    command.image.pixel_format);
                const std::vector<std::byte> pixels = remapped_pixels(
                    command.image, command.image_attributes);
                sk_sp<SkData> data = SkData::MakeWithCopy(
                    pixels.data(), pixels.size());
                sk_sp<SkImage> image = SkImages::RasterFromData(
                    source_info, std::move(data), command.image.row_bytes());
                if (!image) return {RasterError::missing_image_pixels, executed};
                apply_image_attributes(paint, command.image_attributes);
                const RectF source{command.first.x, command.first.y,
                                   command.second.x, command.second.y};
                (*canvas).drawImageRect(image.get(), to_sk_rect(source),
                                      to_sk_rect(command.rect),
                                      sampling_for(command.state), &paint,
                                      SkCanvas::kStrict_SrcRectConstraint);
                break;
            }
            }
            ++executed;
        }
        unlock_scratch.release();
        const ImageSnapshot rendered = (*scratch).snapshot();
        const BitmapLockView target_lock = target.lock(BitmapLockMode::write);
        BitmapUnlock unlock_target(target, target_lock.token);
        if (rendered.pixels().size() !=
            target_lock.row_bytes * static_cast<std::size_t>(target_lock.height)) {
            return {RasterError::target_unavailable};
        }
        std::memcpy(target_lock.writable_data, rendered.pixels().data(),
                    rendered.pixels().size());
        unlock_target.release();
        return {RasterError::none, executed};
    } catch (const std::invalid_argument&) {
        return {RasterError::invalid_argument};
    } catch (const std::logic_error&) {
        return {RasterError::invalid_argument};
    } catch (...) {
        return {RasterError::internal_error};
    }
}

DecodeResult SkiaExecutor::decode_png(std::span<const std::byte> encoded,
                                      const PngCodecLimits& limits) {
    if (!owner_thread()) return DecodeResult::failure(RasterError::wrong_thread);
    if (encoded.empty()) return DecodeResult::failure(RasterError::decode_failed);
    if (encoded.size() > limits.maximum_encoded_bytes) {
        return DecodeResult::failure(RasterError::encoded_limit_exceeded);
    }
    try {
        // The shared owned decode (src/core/image/png_pixels), as on every
        // host: premultiplied BGRA straight into the bitmap. iCCP is refused
        // for this purpose (owner decision D1, option 2).
        gui_forms::image::PngPixelRequest request{};
        request.limits = gui_forms::image::png_limits_for_pixels(limits.maximum_pixels, encoded.size());
        request.limits.dimension = std::min<std::uint32_t>(
            std::max(limits.maximum_width, limits.maximum_height), 0x7fffffffU);
        request.target = {gui_forms::image::PixelOrder::bgra,
                          gui_forms::image::PixelAlpha::premultiplied};
        request.purpose = gui_forms::image::PngPixelPurpose::drawing_decode;
        gui_forms::image::PngPixelDecoder decoder{};
        const gui_forms::image::PngPixelResult inspected = decoder.inspect(encoded, request);
        if (inspected.error != gui_forms::image::PngPixelError::none) {
            const bool too_large = inspected.decode_error == gui_forms::image::PngDecodeError::resource_limit;
            return DecodeResult::failure(too_large ? RasterError::dimension_limit_exceeded
                                                   : RasterError::decode_failed);
        }
        if (inspected.width > limits.maximum_width || inspected.height > limits.maximum_height) {
            return DecodeResult::failure(RasterError::dimension_limit_exceeded);
        }
        std::unique_ptr<gui_drawing::Bitmap> bitmap = std::make_unique<Bitmap>(
            inspected.width, inspected.height, PixelFormat::bgra32_premultiplied);
        const BitmapLockView lock = (*bitmap).lock(BitmapLockMode::write);
        BitmapUnlock unlock(*bitmap, lock.token);
        const std::size_t byte_count = lock.row_bytes * static_cast<std::size_t>(lock.height);
        const std::span<std::byte> pixels(static_cast<std::byte*>(lock.writable_data), byte_count);
        const gui_forms::image::PngPixelResult decoded =
            decoder.decode(encoded, request, pixels, lock.row_bytes);
        if (decoded.error != gui_forms::image::PngPixelError::none) {
            return DecodeResult::failure(RasterError::decode_failed);
        }
        unlock.release();
        return DecodeResult::success(std::move(bitmap));
    } catch (const std::length_error&) {
        return DecodeResult::failure(RasterError::dimension_limit_exceeded);
    } catch (...) {
        return DecodeResult::failure(RasterError::internal_error);
    }
}

std::vector<std::byte> SkiaExecutor::encode_png(
    const Bitmap& bitmap, RasterError* error, const PngCodecLimits& limits) {
    if (!owner_thread()) {
        assign_raster_error(error, RasterError::wrong_thread);
        return {};
    }
    try {
        const ImageSnapshot snapshot = bitmap.snapshot();
        if (snapshot.width > limits.maximum_width ||
            snapshot.height > limits.maximum_height ||
            static_cast<std::uint64_t>(snapshot.width) * snapshot.height >
                limits.maximum_pixels) {
            assign_raster_error(error, RasterError::dimension_limit_exceeded);
            return {};
        }
        // The shared owned encoder, as on every host; level 6 matches the
        // former SkPngEncoder setting and the encoder's default.
        gui_forms::image::PngEncodeInput input{};
        input.width = snapshot.width;
        input.height = snapshot.height;
        input.row_bytes = snapshot.row_bytes();
        input.pixels = snapshot.pixels();
        input.order = snapshot.pixel_format == PixelFormat::bgra32_premultiplied
            ? gui_forms::image::PngChannelOrder::bgra : gui_forms::image::PngChannelOrder::rgba;
        input.alpha = gui_forms::image::PngSourceAlpha::premultiplied;
        input.compression_level = gui_forms::image::default_png_compression_level;
        std::vector<std::byte> result;
        if (!gui_forms::image::encode_png(input, result) || result.empty()) {
            assign_raster_error(error, RasterError::encode_failed);
            return {};
        }
        if (result.size() > limits.maximum_encoded_bytes) {
            assign_raster_error(error, RasterError::encoded_limit_exceeded);
            return {};
        }
        assign_raster_error(error, RasterError::none);
        return result;
    } catch (const std::logic_error&) {
        assign_raster_error(error, RasterError::invalid_argument);
        return {};
    } catch (...) {
        assign_raster_error(error, RasterError::internal_error);
        return {};
    }
}

bool SkiaExecutor::owner_thread() const noexcept {
    return std::this_thread::get_id() == owner_thread_;
}

} // namespace gui_drawing::render
