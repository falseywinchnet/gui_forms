#include "glyph_run_raster.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace gui_forms::render::text {
namespace {

constexpr FT_Int32 light_flags = FT_LOAD_NO_BITMAP | FT_LOAD_TARGET_LIGHT | FT_LOAD_FORCE_AUTOHINT;
// Coverage dimensions above this are refused rather than allocated.
constexpr std::int64_t maximum_extent = 16384;

std::int64_t floor_div_64(const std::int64_t value) noexcept {
    std::int64_t quotient = value / 64;
    if (value % 64 != 0 && value < 0) --quotient;
    return quotient;
}

// One rendered glyph, copied out of FreeType's slot before the next load.
struct RenderedGlyph final {
    std::int64_t left{};
    std::int64_t top{};
    std::uint32_t width{};
    std::uint32_t height{};
    std::size_t offset{};
};

} // namespace

GlyphRunOrigin glyph_run_origin(const double device_x, const double device_y) noexcept {
    GlyphRunOrigin origin{};
    const std::int64_t fixed_x = std::llround(device_x * 64.0);
    origin.pixel_x = floor_div_64(fixed_x);
    origin.phase_x = static_cast<std::int32_t>(fixed_x - origin.pixel_x * 64);
    origin.pixel_y = std::llround(device_y);
    return origin;
}

class GlyphRunRasterizer::Impl final {
public:
    explicit Impl(const HarfBuzzFontEngine& engine) : engine_(engine) {
        if (FT_Init_FreeType(&library_) != 0) throw std::runtime_error("Glyph run FreeType initialization failed");
    }
    ~Impl() {
        for (OpenFace& face : faces_) FT_Done_Face(face.face);
        FT_Done_FreeType(library_);
    }
    Impl(const Impl&) = delete;
    Impl& operator=(const Impl&) = delete;

    // Nullptr when the engine has no source for this ID.
    FT_Face face(const FontFaceId id) {
        for (const OpenFace& open : faces_) {
            if (open.id == id) return open.face;
        }
        const std::optional<FontFaceSource> source = engine_.face_source(id);
        if (!source) return nullptr;
        FT_Face opened = nullptr;
        const FT_Error status = FT_New_Memory_Face(library_, reinterpret_cast<const FT_Byte*>((*source).bytes.data()),
                                                   static_cast<FT_Long>((*source).bytes.size()),
                                                   static_cast<FT_Long>((*source).face_index), &opened);
        if (status != 0) throw std::runtime_error("Glyph run face open failed");
        faces_.push_back({id, opened, (*source).owner});
        return opened;
    }

private:
    struct OpenFace final {
        FontFaceId id{};
        FT_Face face{};
        // Keeps the encoded bytes alive for FreeType's memory face.
        std::shared_ptr<const void> owner;
    };

    const HarfBuzzFontEngine& engine_;
    FT_Library library_{};
    std::vector<OpenFace> faces_;
};

GlyphRunRasterizer::GlyphRunRasterizer(const HarfBuzzFontEngine& engine) : impl_(std::make_unique<Impl>(engine)) {}

GlyphRunRasterizer::~GlyphRunRasterizer() = default;

GlyphRunCoverage GlyphRunRasterizer::rasterize(const ShapedText& shaped, const FontSpec font, const double scale,
                                               const std::int32_t phase_x) {
    if (!std::isfinite(scale) || scale <= 0.0 || !std::isfinite(font.size) || phase_x < 0 || phase_x > 63) {
        throw std::invalid_argument("Invalid glyph run placement");
    }
    const double device_size = std::clamp(font.size, 1.0, 4096.0) * scale;
    const FT_F26Dot6 size = static_cast<FT_F26Dot6>(std::llround(device_size * 64.0));
    GlyphRunCoverage result{};
    std::vector<RenderedGlyph> glyphs;
    std::vector<std::uint8_t> bitmaps;
    std::int64_t left = std::numeric_limits<std::int64_t>::max();
    std::int64_t top = std::numeric_limits<std::int64_t>::max();
    std::int64_t right = std::numeric_limits<std::int64_t>::min();
    std::int64_t bottom = std::numeric_limits<std::int64_t>::min();
    for (const ShapedFontRun& run : shaped.runs) {
        const FT_Face face = (*impl_).face(run.face);
        if (face == nullptr) {
            result.missing_glyphs += static_cast<std::uint32_t>(run.glyphs.size());
            continue;
        }
        if (FT_Set_Char_Size(face, 0, size, 72U, 72U) != 0) throw std::runtime_error("Glyph run size failed");
        for (const ShapedGlyph& glyph : run.glyphs) {
            // Integer 26.6 placement relative to the origin pixel.
            const std::int64_t device_x = phase_x + std::llround(static_cast<double>(glyph.x) * scale * 64.0);
            const std::int64_t device_y = std::llround(static_cast<double>(glyph.y) * scale * 64.0);
            const std::int64_t pixel_x = floor_div_64(device_x);
            const std::int64_t pixel_y = floor_div_64(device_y);
            FT_Vector delta{static_cast<FT_Pos>(device_x - pixel_x * 64), -static_cast<FT_Pos>(device_y - pixel_y * 64)};
            FT_Set_Transform(face, nullptr, &delta);
            if (FT_Load_Glyph(face, glyph.glyph.value, light_flags) != 0) throw std::runtime_error("Glyph load failed");
            const FT_GlyphSlot slot = (*face).glyph;
            if ((*slot).format != FT_GLYPH_FORMAT_OUTLINE) throw std::runtime_error("Glyph is not an outline");
            if (FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL) != 0) throw std::runtime_error("Glyph render failed");
            const FT_Bitmap& bitmap = (*slot).bitmap;
            if (bitmap.width == 0 || bitmap.rows == 0) continue;
            if (bitmap.pixel_mode != FT_PIXEL_MODE_GRAY || bitmap.num_grays != 256 || bitmap.pitch <= 0 ||
                static_cast<unsigned>(bitmap.pitch) < bitmap.width) {
                throw std::runtime_error("Unexpected glyph bitmap");
            }
            RenderedGlyph rendered{};
            rendered.left = pixel_x + (*slot).bitmap_left;
            rendered.top = pixel_y - (*slot).bitmap_top;
            rendered.width = bitmap.width;
            rendered.height = bitmap.rows;
            rendered.offset = bitmaps.size();
            for (unsigned row = 0; row < bitmap.rows; ++row) {
                const std::uint8_t* const line = bitmap.buffer + static_cast<std::size_t>(row) * static_cast<std::size_t>(bitmap.pitch);
                bitmaps.insert(bitmaps.end(), line, line + bitmap.width);
            }
            left = std::min(left, rendered.left);
            top = std::min(top, rendered.top);
            right = std::max(right, rendered.left + static_cast<std::int64_t>(rendered.width));
            bottom = std::max(bottom, rendered.top + static_cast<std::int64_t>(rendered.height));
            glyphs.push_back(rendered);
        }
    }
    if (glyphs.empty()) return result;
    if (right - left > maximum_extent || bottom - top > maximum_extent) throw std::length_error("Glyph run too large");
    result.left = static_cast<std::int32_t>(left);
    result.top = static_cast<std::int32_t>(top);
    result.width = static_cast<std::uint32_t>(right - left);
    result.height = static_cast<std::uint32_t>(bottom - top);
    result.pixels.assign(static_cast<std::size_t>(result.width) * result.height, 0U);
    for (const RenderedGlyph& glyph : glyphs) {
        for (std::uint32_t row = 0; row < glyph.height; ++row) {
            const std::size_t source = glyph.offset + static_cast<std::size_t>(row) * glyph.width;
            const std::size_t target_row = static_cast<std::size_t>(glyph.top - top) + row;
            const std::size_t target = target_row * result.width + static_cast<std::size_t>(glyph.left - left);
            for (std::uint32_t column = 0; column < glyph.width; ++column) {
                const unsigned coverage = bitmaps[source + column];
                const unsigned previous = result.pixels[target + column];
                const unsigned combined = coverage + previous * (255U - coverage) / 255U;
                result.pixels[target + column] = static_cast<std::uint8_t>(combined);
            }
        }
    }
    return result;
}

} // namespace gui_forms::render::text
