#include "skia_raster.hpp"
#include "gui_forms/engine/part_raster/part_raster.hpp"
#include "gui_forms/paint_framebuffer.hpp"

#include "gui_forms/live_surface.hpp"
#if defined(GUI_FORMS_PREPARED_TEXT)
#include "prepared_skia_frame.hpp"
#endif

#include "../../text/harfbuzz/harfbuzz_font_engine.hpp"
#include "../../text/glyph_run/glyph_run_raster.hpp"
#include "../../../core/text/composite/text_composite.hpp"
#include "../../../core/image/resource_pixels.hpp"
#include "../../../core/image/png_writer.hpp"
#include "../../../core/damage/device_damage/device_damage.hpp"

#include "include/core/SkCanvas.h"
#include "include/core/SkBlurTypes.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkData.h"
#include "include/core/SkImage.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkMaskFilter.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkPathBuilder.h"
#include "include/core/SkPixmap.h"
#include "include/core/SkRegion.h"
#include "include/core/SkRRect.h"
#include "include/core/SkStream.h"
#include "include/core/SkSurface.h"
#include "include/effects/SkShaderMaskFilter.h"
#include "include/effects/SkGradient.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <cmath>
#include <limits>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace gui_forms::render {
namespace {

SkColor to_sk_color(Color color) noexcept {
    return SkColorSetARGB(color.alpha, color.red, color.green, color.blue);
}

SkRect to_sk_rect(Rect rect) noexcept {
    return SkRect::MakeXYWH(static_cast<SkScalar>(rect.x),
                            static_cast<SkScalar>(rect.y),
                            static_cast<SkScalar>(rect.width),
                            static_cast<SkScalar>(rect.height));
}

// Resolve the complete canvas transform, including device scale and retained
// translations. Exact equality deliberately refuses fractional resampling.
bool live_surface_device_rect(const SkCanvas& canvas, const Rect destination,
                              const LiveSurfaceFrame& frame, SkIRect& result) {
    const SkMatrix matrix = canvas.getTotalMatrix();
    if (!matrix.isScaleTranslate() || matrix.getScaleX() <= 0.0f ||
        matrix.getScaleY() <= 0.0f) return false;
    SkRect device = to_sk_rect(destination);
    matrix.mapRect(&device);
    if (!device.isFinite() || device.width() != static_cast<SkScalar>(frame.width()) ||
        device.height() != static_cast<SkScalar>(frame.height())) return false;
    const double left = device.left();
    const double top = device.top();
    const double right = device.right();
    const double bottom = device.bottom();
    if (left != std::floor(left) || top != std::floor(top) ||
        left < std::numeric_limits<int>::min() || top < std::numeric_limits<int>::min() ||
        right > std::numeric_limits<int>::max() || bottom > std::numeric_limits<int>::max()) return false;
    result = SkIRect::MakeLTRB(static_cast<int>(left), static_cast<int>(top),
                              static_cast<int>(right), static_cast<int>(bottom));
    return true;
}

SkRRect to_sk_rrect(Rect rect, double radius) noexcept {
    const SkScalar bounded = static_cast<SkScalar>(std::clamp(
        radius, 0.0, std::max(0.0, std::min(rect.width, rect.height) * 0.5)));
    return SkRRect::MakeRectXY(to_sk_rect(rect), bounded, bounded);
}

struct SkGradientData final {
    std::vector<SkColor4f> colors;
    std::vector<float> positions;
};

SkGradientData to_sk_gradient(std::span<const GradientStop> stops) {
    SkGradientData result;
    result.colors.reserve(stops.size());
    result.positions.reserve(stops.size());
    for (const GradientStop& stop : stops) {
        result.colors.push_back(SkColor4f::FromColor(to_sk_color(stop.color)));
        result.positions.push_back(static_cast<float>(stop.offset));
    }
    return result;
}

SkTileMode to_sk_tile_mode(GradientSpreadMode spread) noexcept {
    switch (spread) {
    case GradientSpreadMode::pad: return SkTileMode::kClamp;
    case GradientSpreadMode::repeat: return SkTileMode::kRepeat;
    case GradientSpreadMode::reflect: return SkTileMode::kMirror;
    }
    return SkTileMode::kClamp;
}

SkGradient make_sk_gradient(
    const SkGradientData& data,
    GradientSpreadMode spread = GradientSpreadMode::pad) {
    const SkGradient::Colors colors(
        SkSpan<const SkColor4f>(data.colors.data(), data.colors.size()),
        SkSpan<const float>(data.positions.data(), data.positions.size()),
        to_sk_tile_mode(spread), SkColorSpace::MakeSRGB());
    SkGradient::Interpolation interpolation;
    interpolation.fInPremul = SkGradient::Interpolation::InPremul::kYes;
    interpolation.fColorSpace = SkGradient::Interpolation::ColorSpace::kSRGB;
    return SkGradient(colors, interpolation);
}

SkPaint make_paint(Color color) {
    SkPaint paint;
    paint.setColor(to_sk_color(color));
    paint.setAntiAlias(true);
    return paint;
}

// Edge snapping (owner decision L-Q8, detail::snap_device_edge): the local
// rectangle whose device edges are `rect`'s, each rounded to a whole device
// pixel. Only an axis-aligned transform (scale and translation) snaps;
// under any other, false, and the caller keeps antialiased geometry. At whole
// device pixels a fill without antialiasing covers exactly the pixels
// between the edges.
bool snapped_local_rect(const SkCanvas& canvas, const Rect rect, SkRect& local) {
    const SkMatrix matrix = canvas.getTotalMatrix();
    const double scale_x = static_cast<double>(matrix.getScaleX());
    const double scale_y = static_cast<double>(matrix.getScaleY());
    if (!matrix.isScaleTranslate() || !rect.finite() || scale_x == 0.0 || scale_y == 0.0) return false;
    const double translate_x = static_cast<double>(matrix.getTranslateX());
    const double translate_y = static_cast<double>(matrix.getTranslateY());
    const double left = gui_forms::detail::snap_device_edge(rect.x * scale_x + translate_x);
    const double top = gui_forms::detail::snap_device_edge(rect.y * scale_y + translate_y);
    const double right = gui_forms::detail::snap_device_edge((rect.x + rect.width) * scale_x + translate_x);
    const double bottom = gui_forms::detail::snap_device_edge((rect.y + rect.height) * scale_y + translate_y);
    local = SkRect::MakeLTRB(static_cast<SkScalar>((left - translate_x) / scale_x),
                             static_cast<SkScalar>((top - translate_y) / scale_y),
                             static_cast<SkScalar>((right - translate_x) / scale_x),
                             static_cast<SkScalar>((bottom - translate_y) / scale_y));
    local.sort();
    return true;
}

// The same snapped edges in device space, as whole numbers. An antialiased
// shape whose edges must cover whole pixels is drawn here, with the canvas
// transform reset: the local rectangle above, mapped back through the
// transform in single precision, can land a hair inside a pixel edge, and
// antialiasing then shows that pixel partly covered.
bool snapped_device_rect(const SkMatrix& matrix, const Rect rect, SkRect& device) {
    const double scale_x = static_cast<double>(matrix.getScaleX());
    const double scale_y = static_cast<double>(matrix.getScaleY());
    if (!matrix.isScaleTranslate() || !rect.finite() || scale_x == 0.0 || scale_y == 0.0) return false;
    const double translate_x = static_cast<double>(matrix.getTranslateX());
    const double translate_y = static_cast<double>(matrix.getTranslateY());
    const double left = gui_forms::detail::snap_device_edge(rect.x * scale_x + translate_x);
    const double top = gui_forms::detail::snap_device_edge(rect.y * scale_y + translate_y);
    const double right = gui_forms::detail::snap_device_edge((rect.x + rect.width) * scale_x + translate_x);
    const double bottom = gui_forms::detail::snap_device_edge((rect.y + rect.height) * scale_y + translate_y);
    device = SkRect::MakeLTRB(static_cast<SkScalar>(left), static_cast<SkScalar>(top),
                              static_cast<SkScalar>(right), static_cast<SkScalar>(bottom));
    device.sort();
    return true;
}

// A logical corner radius in device pixels along each axis, no more than
// half of the device box it rounds.
SkVector device_radii(const SkMatrix& matrix, const double radius, const SkRect& device) {
    const double across = std::abs(static_cast<double>(matrix.getScaleX())) * radius;
    const double down = std::abs(static_cast<double>(matrix.getScaleY())) * radius;
    const double half_width = static_cast<double>(device.width()) * 0.5;
    const double half_height = static_cast<double>(device.height()) * 0.5;
    const SkVector radii = SkVector::Make(static_cast<SkScalar>(std::clamp(across, 0.0, half_width)),
                                          static_cast<SkScalar>(std::clamp(down, 0.0, half_height)));
    return radii;
}

// The rectangle to draw for a box filled with `paint`: snapped, with
// antialiasing off, when the transform allows; otherwise the box as given.
SkRect snapped_box(const SkCanvas& canvas, const Rect rect, SkPaint& paint) {
    SkRect local = to_sk_rect(rect);
    if (snapped_local_rect(canvas, rect, local)) paint.setAntiAlias(false);
    return local;
}

Rect outset_rect(const Rect rect, const double amount) noexcept {
    const Rect result{rect.x - amount, rect.y - amount, rect.width + amount * 2.0, rect.height + amount * 2.0};
    return result;
}

} // namespace

class SkiaRaster::Impl {
public:
    struct DecodedImage final {
        std::uint64_t content_hash{};
        sk_sp<SkImage> image;
    };

    using ImageMap = std::unordered_map<std::uint64_t, DecodedImage>;

    struct FontSource {
        std::optional<FontRole> role;
        std::uint16_t weight;
        bool italic;
        sk_sp<SkData> data;
    };
    std::vector<FontSource> font_sources;
    sk_sp<SkSurface> surface;
#if defined(GUI_FORMS_PREPARED_TEXT)
    PreparedSkiaFrame prepared_frame{};
    bool prepared_mode{false};
#endif
    ImageMap images;
    text::HarfBuzzFontEngine text_engine;
    // Identity of text_engine for shaped-text memos. A memo holding an
    // expired or different identity was shaped by another raster.
    std::shared_ptr<const bool> text_engine_identity{std::make_shared<const bool>(true)};
    // Draws the run's shared-rasterizer coverage through Skia as an alpha
    // mask. Used only when the canvas pixels cannot be composited directly;
    // Skia then blends, so the bytes may differ from the shared rule.
    void draw_shaped_text(SkCanvas& target, const text::ShapedText& shaped, Point origin,
                          FontSpec font_spec, Color color);

    // ADR-004 text clipping: the device clip that text compositing honours,
    // one entry per canvas save level from text_clip_base upward. Skia cannot
    // report a clip's fractional edges, so SkiaRaster records them itself.
    struct TextClipState final {
        SkRect rect{};
        bool rounded{};
        SkRect rounded_rect{};
        double radius{};
        // Disjoint device rectangles of the frame damage and clip_damage.
        bool region_limited{};
        SkRegion region;
    };
    std::vector<TextClipState> text_clips;
    int text_clip_base{};
    std::unique_ptr<text::GlyphRunRasterizer> glyph_runs;

    void reset_text_clips(SkCanvas& target, const SkRegion* region) {
        text_clips.clear();
        TextClipState base{};
        base.rect = SkRect::MakeIWH(target.imageInfo().width(), target.imageInfo().height());
        if (region != nullptr) {
            base.region_limited = true;
            base.region = *region;
        }
        text_clips.push_back(std::move(base));
        text_clip_base = target.getSaveCount();
    }

    // Null when the recorded clips no longer describe the canvas.
    [[nodiscard]] TextClipState* current_text_clip(const SkCanvas& target) noexcept {
        if (text_clips.empty()) return nullptr;
        const int expected = text_clip_base + static_cast<int>(text_clips.size()) - 1;
        if (target.getSaveCount() != expected) return nullptr;
        return &text_clips.back();
    }

    // Rasterizes (or reuses) the run's coverage and composites it with the
    // shared rule into the canvas pixels. False when the canvas cannot be
    // addressed this way; the caller then draws through Skia.
    bool composite_run(SkCanvas& target, const text::ShapedText& shaped, FontSpec font, Point origin, Color color,
                       text::GlyphRunCoverage* kept, double* kept_scale, std::int32_t* kept_phase);
    // The shared compositing step alone: device coverage into the canvas
    // pixels under the recorded clips. False when the canvas cannot be
    // addressed or its clips are unknown.
    bool composite_coverage(SkCanvas& target, const detail::TextCoverage& source, Color color);

    // The canvas pixels and the recorded clip as the shared blends take
    // them, for text coverage and part rasters alike.
    struct SharedComposite final {
        TextClipState* state{};
        detail::TextCompositeTarget target{};
        detail::TextCompositeClip clip{};
    };
    // None when the canvas cannot be addressed as premultiplied 8-bit RGBA
    // or BGRA, or its clips are unknown.
    [[nodiscard]] std::optional<SharedComposite> shared_composite(SkCanvas& target);

    // A part raster into the canvas pixels under the recorded clips. False
    // when the canvas is not a uniform scale and translation equal to the
    // raster's scale, or cannot be addressed.
    bool composite_part(SkCanvas& target, const PartRaster& raster, Point origin, double opacity);
    bool composite_part_scaled(SkCanvas& target, const PartRaster& raster, Rect destination, double opacity);
    // The shared step alone: premultiplied pixels placed in the canvas's
    // device pixels, composited under the recorded clips. False when the
    // canvas cannot be addressed or its clips are unknown.
    bool composite_source(SkCanvas& target, const detail::PartRasterSource& source, double opacity);

    // Paint groups: one level per open group. An isolated level keeps the
    // parent canvas's recorded clips and save floor while its own surface
    // is drawn.
    struct GroupLevel final {
        sk_sp<SkSurface> surface;
        bool isolated{};
        SkIRect device{};
        double opacity{};
        PaintGroupComposite composite{PaintGroupComposite::source_over};
        std::vector<TextClipState> parent_text_clips;
        int parent_text_clip_base{};
        int parent_save_floor{};
    };
    std::array<GroupLevel, maximum_paint_group_depth> groups{};
    std::size_t group_depth{};
    std::size_t refused_open{};
    std::uint64_t group_refusals{};

    // The canvas of the innermost isolated group, or null.
    [[nodiscard]] SkCanvas* group_canvas() const noexcept {
        for (std::size_t level = group_depth; level > 0U; --level) {
            const GroupLevel& open = groups[level - 1U];
            if (open.isolated) return (*open.surface).getCanvas();
        }
        return nullptr;
    }
    // Ends every open group without compositing (a frame's start or end).
    void close_groups() noexcept {
        for (std::size_t level = group_depth; level > 0U; --level) {
            GroupLevel& open = groups[level - 1U];
            if (!open.isolated) continue;
            (*(*open.surface).getCanvas()).restoreToCount(1);
            text_clips = std::move(open.parent_text_clips);
            text_clip_base = open.parent_text_clip_base;
            save_floor = open.parent_save_floor;
            open.isolated = false;
        }
        group_depth = 0U;
        refused_open = 0U;
    }

#if defined(GUI_FORMS_PREPARED_TEXT)
    // Lets prepared text composite through composite_coverage.
    class PreparedSink final : public PreparedCoverageSink {
    public:
        explicit PreparedSink(Impl& owner) : owner_(owner) {}
        bool composite(SkCanvas& target, const detail::TextCoverage& coverage, const Color color) override {
            const bool composited = owner_.composite_coverage(target, coverage, color);
            return composited;
        }
    private:
        Impl& owner_;
    };
#endif
    Size logical_size{};
    double scale{1.0};
    int save_floor{1};
    const ImageRegistry* image_registry{};
    std::uint64_t image_registry_revision{std::numeric_limits<std::uint64_t>::max()};
    bool image_registry_synchronized{true};

    [[nodiscard]] bool register_font_data(std::optional<FontRole> role,
                                          std::uint16_t weight, bool italic,
                                          sk_sp<SkData> data) {
        if (!data || (*data).isEmpty() || (*data).size() > 64U * 1024U * 1024U) return false;
        const sk_sp<SkData> retained_data = data;
        const std::span<const std::byte> bytes(
            static_cast<const std::byte*>((*data).data()), (*data).size());
        const std::shared_ptr<const sk_sp<SkData>> owner =
            std::make_shared<const sk_sp<SkData>>(std::move(data));
        const std::optional<FontFaceId> text_face = text_engine.register_owned_typeface(
            role, weight, italic, bytes, owner);
        if (!text_face) return false;
        font_sources.push_back({role, weight, italic, retained_data});
        return true;
    }

    [[nodiscard]] SkCanvas* canvas() const noexcept {
        SkCanvas* result = group_canvas();
        if (result != nullptr) return result;
#if defined(GUI_FORMS_PREPARED_TEXT)
        if (prepared_mode) {
            result = prepared_frame.canvas();
            return result;
        }
#endif
        if (surface) result = (*surface).getCanvas();
        return result;
    }

    [[nodiscard]] SkSurface* front_surface() const noexcept {
        SkSurface* result = surface.get();
#if defined(GUI_FORMS_PREPARED_TEXT)
        if (prepared_mode) result = prepared_frame.front();
#endif
        return result;
    }
};

SkiaRaster::SkiaRaster() : impl_(std::make_unique<Impl>()) {}
SkiaRaster::~SkiaRaster() = default;

bool SkiaRaster::register_typeface(FontRole role, std::uint16_t weight,
                                   bool italic, std::span<const std::byte> encoded) {
    if (encoded.empty() || encoded.size() > 64U * 1024U * 1024U) return false;
    sk_sp<SkData> data = SkData::MakeWithCopy(encoded.data(), encoded.size());
    const bool registered = (*impl_).register_font_data(role, weight, italic, std::move(data));
    return registered;
}

bool SkiaRaster::register_fallback_typeface(
    std::uint16_t weight, bool italic, std::span<const std::byte> encoded) {
    if (encoded.empty() || encoded.size() > 64U * 1024U * 1024U) return false;
    sk_sp<SkData> data = SkData::MakeWithCopy(encoded.data(), encoded.size());
    const bool registered = (*impl_).register_font_data(std::nullopt, weight, italic, std::move(data));
    return registered;
}

bool SkiaRaster::register_typeface_file(FontRole role, std::uint16_t weight,
                                        bool italic, const char* path) {
    sk_sp<SkData> data = SkData::MakeFromFileName(path);
    const bool registered = (*impl_).register_font_data(role, weight, italic, std::move(data));
    return registered;
}

bool SkiaRaster::register_fallback_typeface_file(
    std::uint16_t weight, bool italic, const char* path) {
    sk_sp<SkData> data = SkData::MakeFromFileName(path);
    const bool registered = (*impl_).register_font_data(std::nullopt, weight, italic, std::move(data));
    return registered;
}

class SkiaFramebuffer final : public PaintFramebuffer {
public:
    SkiaRaster raster;
    bool begin(const ImageRegistry& images, Rect damage) override {
        if (!raster.synchronize_images(images)) return false;
        DamageRegion region;
        region.add(damage);
        raster.begin_frame(region);
        return true;
    }
    void end() override { raster.end_frame(); }
    Painter& painter() noexcept override { return raster; }
    std::span<std::byte> pixels() noexcept override {
        return {static_cast<std::byte*>(const_cast<void*>(raster.pixels())), raster.byte_size()};
    }
    std::size_t row_bytes() const noexcept override { return raster.row_bytes(); }
    std::uint32_t width() const noexcept override { return raster.pixel_width(); }
    std::uint32_t height() const noexcept override { return raster.pixel_height(); }
    FramebufferChannelOrder channel_order() const noexcept override { return FramebufferChannelOrder::rgba; }
};
std::unique_ptr<PaintFramebuffer> SkiaRaster::create_framebuffer(Size logical_size, double scale) {
    if (!std::isfinite(scale) || scale <= 0 || !std::isfinite(logical_size.width) ||
        !std::isfinite(logical_size.height) || logical_size.width <= 0 || logical_size.height <= 0 ||
        std::ceil(logical_size.width * scale) * std::ceil(logical_size.height * scale) > 16777216.0)
        return {};
    std::unique_ptr<SkiaFramebuffer> result = std::make_unique<SkiaFramebuffer>();
    for (const Impl::FontSource& source : (*impl_).font_sources)
        if (!(*(*result).raster.impl_).register_font_data(source.role, source.weight, source.italic, source.data))
            return {};
    if (!(*result).raster.resize(logical_size, scale)) return {};
    return result;
}

bool SkiaRaster::resize(Size logical_size, double scale) {
#if defined(GUI_FORMS_PREPARED_TEXT)
    // A host using transactions must not replace its coherent front via the
    // historical resize path. Size admission belongs to begin_prepared_frame.
    if ((*impl_).prepared_mode) return false;
#endif
    if (!std::isfinite(scale) || scale <= 0.0 || logical_size.width <= 0.0 ||
        logical_size.height <= 0.0) {
        return false;
    }
    const double requested_width = std::ceil(logical_size.width * scale);
    const double requested_height = std::ceil(logical_size.height * scale);
    if (requested_width > static_cast<double>(std::numeric_limits<int>::max()) ||
        requested_height > static_cast<double>(std::numeric_limits<int>::max())) {
        return false;
    }
    const int width = std::max(1, static_cast<int>(requested_width));
    const int height = std::max(1, static_cast<int>(requested_height));
    if ((*impl_).surface && width == (*(*impl_).surface).width() &&
        height == (*(*impl_).surface).height() && scale == (*impl_).scale) {
        (*impl_).logical_size = logical_size;
        return false;
    }

    // The host-facing surface contract is premultiplied RGBA8 on every target.
    // Skia's N32 alias follows the build's native channel order (RGBA on the
    // macOS archive, BGRA on the MinGW archive), which makes identical renderer
    // commands expose different bytes across hosts.
    const SkImageInfo info = SkImageInfo::Make(
        width, height, kRGBA_8888_SkColorType, kPremul_SkAlphaType,
        SkColorSpace::MakeSRGB());
    sk_sp<SkSurface> replacement = SkSurfaces::Raster(info);
    if (!replacement) {
        return false;
    }
    (*(*replacement).getCanvas()).clear(SK_ColorTRANSPARENT);
    (*impl_).surface = std::move(replacement);
    (*impl_).logical_size = logical_size;
    (*impl_).scale = scale;
    return true;
}

void SkiaRaster::begin_frame(const DamageRegion& damage) {
    (*impl_).close_groups();
    SkCanvas* canvas = (*impl_).canvas();
    if (!canvas) {
        return;
    }
    (*canvas).restoreToCount(1);
    (*canvas).resetMatrix();
    (*canvas).scale(static_cast<SkScalar>((*impl_).scale),
                  static_cast<SkScalar>((*impl_).scale));
    (*canvas).save();
    (*impl_).save_floor = (*canvas).getSaveCount();

    if (!damage.empty()) {
        SkRegion region;
        for (Rect rect : damage.rectangles()) {
            const SkIRect pixels = SkIRect::MakeLTRB(
                static_cast<int>(std::floor(rect.x * (*impl_).scale)),
                static_cast<int>(std::floor(rect.y * (*impl_).scale)),
                static_cast<int>(std::ceil((rect.x + rect.width) * (*impl_).scale)),
                static_cast<int>(std::ceil((rect.y + rect.height) * (*impl_).scale)));
            region.op(pixels, SkRegion::kUnion_Op);
        }
        (*canvas).resetMatrix();
        (*canvas).clipRegion(region);
        (*canvas).scale(static_cast<SkScalar>((*impl_).scale),
                      static_cast<SkScalar>((*impl_).scale));
        (*impl_).reset_text_clips(*canvas, &region);
    } else {
        (*impl_).reset_text_clips(*canvas, nullptr);
    }
}

void SkiaRaster::clip_damage(const DamageRegion& damage) {
    SkCanvas* const canvas = (*impl_).canvas();
    if (canvas == nullptr || damage.empty()) return;
    SkRegion region;
    for (const Rect rect : damage.rectangles()) {
        const SkIRect pixels = SkIRect::MakeLTRB(
            static_cast<int>(std::floor(rect.x * (*impl_).scale)),
            static_cast<int>(std::floor(rect.y * (*impl_).scale)),
            static_cast<int>(std::ceil((rect.x + rect.width) * (*impl_).scale)),
            static_cast<int>(std::ceil((rect.y + rect.height) * (*impl_).scale)));
        region.op(pixels, SkRegion::kUnion_Op);
    }
    // Regions are in device pixels; restore the logical matrix afterwards.
    const SkMatrix matrix = (*canvas).getTotalMatrix();
    (*canvas).resetMatrix();
    (*canvas).clipRegion(region);
    (*canvas).setMatrix(matrix);
    if (Impl::TextClipState* const state = (*impl_).current_text_clip(*canvas)) {
        if ((*state).region_limited) (*state).region.op(region, SkRegion::kIntersect_Op);
        else (*state).region = region;
        (*state).region_limited = true;
    }
}

void SkiaRaster::end_frame() {
    (*impl_).close_groups();
    if (SkCanvas* canvas = (*impl_).canvas()) {
        (*canvas).restoreToCount(1);
    }
    (*impl_).text_clips.clear();
}

#if defined(GUI_FORMS_PREPARED_TEXT)
PreparedTextStatus SkiaRaster::begin_prepared_frame(const Size logical_size,
    const double scale, DamageRegion& damage) {
    if (!(*impl_).prepared_frame.on_executor()) return PreparedTextStatus::wrong_executor;
    const PreparedTextStatus status = (*impl_).prepared_frame.begin(logical_size, scale, damage);
    if (status == PreparedTextStatus::success) {
        // Select the route only after admission. Failed initial geometry and
        // allocation preserve any ordinary surface; it has no prepared receipt.
        (*impl_).prepared_mode = true;
        (*impl_).surface.reset();
        (*impl_).logical_size = logical_size;
        (*impl_).scale = scale;
        try { begin_frame(damage); }
        catch (...) { (*impl_).prepared_frame.abort(); throw; }
    }
    return status;
}
PreparedTextStatus SkiaRaster::commit_prepared_frame(const PaintReceipt receipt) {
    const PreparedTextStatus status = (*impl_).prepared_frame.commit(receipt);
    return status;
}
void SkiaRaster::abort_prepared_frame() noexcept { (*impl_).prepared_frame.abort(); }
PaintReceipt SkiaRaster::prepared_front_receipt() const noexcept {
    const PaintReceipt result = (*impl_).prepared_frame.receipt();
    return result;
}
double SkiaRaster::prepared_front_scale() const noexcept {
    const double result = (*impl_).prepared_frame.front_scale();
    return result;
}
bool SkiaRaster::prepared_front_matches(const Size logical_size, const double scale) const noexcept {
    const bool result = (*impl_).prepared_frame.matches(logical_size, scale);
    return result;
}
PreparedTextPaintResult SkiaRaster::draw_prepared_text(const PreparedTextLayout& layout,
    const LayoutAuthority expected, const Point baseline, const Color color) {
    Impl::PreparedSink sink(*impl_);
    const PreparedTextPaintResult result = (*impl_).prepared_frame.draw(layout, expected, baseline, color, &sink);
    return result;
}
#endif

bool SkiaRaster::synchronize_images(const ImageRegistry& registry) {
    const ImageRegistrySnapshot snapshot = registry.snapshot();
    const bool same_registry = &registry == (*impl_).image_registry;
    if (same_registry && snapshot.revision == (*impl_).image_registry_revision) {
        return (*impl_).image_registry_synchronized;
    }
    if (!same_registry) {
        (*impl_).images.clear();
    }

    bool synchronized = true;
    // One shared owned decoder for the batch (src/core/image/resource_pixels).
    gui_forms::image::ResourcePixelDecoder decoder{};
    std::unordered_set<std::uint64_t> active;
    for (const ImageId id : registry.image_ids()) {
        active.insert(id.value);
        const std::optional<ImageResourceView> resource = registry.find(id);
        if (!resource) {
            synchronized = false;
            continue;
        }
        const Impl::ImageMap::const_iterator existing = (*impl_).images.find(id.value);
        if (existing != (*impl_).images.end() &&
            (*existing).second.content_hash == (*resource).content_hash) {
            continue;
        }

        if ((*resource).metadata.width >
                static_cast<std::uint32_t>(std::numeric_limits<int>::max()) ||
            (*resource).metadata.height >
                static_cast<std::uint32_t>(std::numeric_limits<int>::max()) ||
            (*resource).metadata.decoded_byte_count >
                std::numeric_limits<std::size_t>::max()) {
            (*impl_).images.erase(id.value);
            synchronized = false;
            continue;
        }

        const SkImageInfo output_info = SkImageInfo::Make(
            static_cast<int>((*resource).metadata.width),
            static_cast<int>((*resource).metadata.height),
            (*resource).encoding == ImageResourceEncoding::bgra32_premultiplied
                ? kBGRA_8888_SkColorType : kRGBA_8888_SkColorType,
            kPremul_SkAlphaType,
            SkColorSpace::MakeSRGB());
        const std::size_t row_bytes = output_info.minRowBytes();
        if (row_bytes != static_cast<std::size_t>((*resource).metadata.width) * 4U ||
            output_info.computeByteSize(row_bytes) !=
                (*resource).metadata.decoded_byte_count) {
            (*impl_).images.erase(id.value);
            synchronized = false;
            continue;
        }
        std::vector<std::byte> pixels;
        sk_sp<SkData> pixel_data;
        if ((*resource).encoding == ImageResourceEncoding::bgra32_premultiplied) {
            if ((*resource).row_bytes != row_bytes ||
                (*resource).encoded.size() != (*resource).metadata.decoded_byte_count) {
                (*impl_).images.erase(id.value);
                synchronized = false;
                continue;
            }
            pixel_data = SkData::MakeWithCopy(
                (*resource).encoded.data(), (*resource).encoded.size());
        } else {
            const gui_forms::image::PixelTarget target{gui_forms::image::PixelOrder::rgba,
                                                       gui_forms::image::PixelAlpha::premultiplied};
            pixels.resize((*resource).metadata.decoded_byte_count);
            const gui_forms::image::ResourcePixelResult decoded =
                decoder.decode(*resource, target, pixels, row_bytes);
            if (decoded.error != gui_forms::image::ResourcePixelError::none ||
                decoded.width != (*resource).metadata.width ||
                decoded.height != (*resource).metadata.height) {
                (*impl_).images.erase(id.value);
                synchronized = false;
                continue;
            }
        }
        if (!pixel_data) {
            pixel_data = SkData::MakeWithCopy(pixels.data(), pixels.size());
        }
        sk_sp<SkImage> image =
            SkImages::RasterFromData(output_info, std::move(pixel_data), row_bytes);
        if (!image) {
            (*impl_).images.erase(id.value);
            synchronized = false;
            continue;
        }
        (*impl_).images[id.value] = {(*resource).content_hash, std::move(image)};
    }
    for (Impl::ImageMap::iterator iterator = (*impl_).images.begin();
         iterator != (*impl_).images.end();) {
        if (!active.contains((*iterator).first)) {
            iterator = (*impl_).images.erase(iterator);
        } else {
            ++iterator;
        }
    }
    (*impl_).image_registry_revision = snapshot.revision;
    (*impl_).image_registry = &registry;
    (*impl_).image_registry_synchronized = synchronized;
    return synchronized;
}

const void* SkiaRaster::pixels() const noexcept {
    SkPixmap pixmap{};
    SkSurface* const surface = (*impl_).front_surface();
    const void* result = nullptr;
    if (surface != nullptr && (*surface).peekPixels(&pixmap)) result = pixmap.addr();
    return result;
}

std::size_t SkiaRaster::row_bytes() const noexcept {
    SkPixmap pixmap{};
    SkSurface* const surface = (*impl_).front_surface();
    std::size_t result = 0;
    if (surface != nullptr && (*surface).peekPixels(&pixmap)) result = pixmap.rowBytes();
    return result;
}

std::uint32_t SkiaRaster::pixel_width() const noexcept {
    SkSurface* const surface = (*impl_).front_surface();
    std::uint32_t result = 0;
    if (surface != nullptr) result = static_cast<std::uint32_t>((*surface).width());
    return result;
}

std::uint32_t SkiaRaster::pixel_height() const noexcept {
    SkSurface* const surface = (*impl_).front_surface();
    std::uint32_t result = 0;
    if (surface != nullptr) result = static_cast<std::uint32_t>((*surface).height());
    return result;
}

std::size_t SkiaRaster::byte_size() const noexcept {
    return row_bytes() * pixel_height();
}

std::vector<std::byte> SkiaRaster::encode_png() const {
    std::vector<std::byte> result;
    SkPixmap pixmap{};
    SkSurface* const surface = (*impl_).front_surface();
    if (surface == nullptr || !(*surface).peekPixels(&pixmap) ||
        pixmap.width() <= 0 || pixmap.height() <= 0) {
        return result;
    }
    // The front surface is premultiplied 8-bit RGBA or BGRA; the shared owned
    // encoder unpremultiplies so the file carries straight alpha, as on
    // every host.
    const bool rgba = pixmap.colorType() == kRGBA_8888_SkColorType;
    const bool bgra = pixmap.colorType() == kBGRA_8888_SkColorType;
    if ((!rgba && !bgra) || pixmap.alphaType() != kPremul_SkAlphaType) return result;
    gui_forms::image::PngEncodeInput input{};
    input.width = static_cast<std::uint32_t>(pixmap.width());
    input.height = static_cast<std::uint32_t>(pixmap.height());
    input.row_bytes = pixmap.rowBytes();
    input.pixels = std::span<const std::byte>(static_cast<const std::byte*>(pixmap.addr()),
                                              pixmap.computeByteSize());
    input.order = rgba ? gui_forms::image::PngChannelOrder::rgba
                       : gui_forms::image::PngChannelOrder::bgra;
    input.alpha = gui_forms::image::PngSourceAlpha::premultiplied;
    if (!gui_forms::image::encode_png(input, result)) result.clear();
    return result;
}

void SkiaRaster::save() {
    if (SkCanvas* canvas = (*impl_).canvas()) {
        if (Impl::TextClipState* const state = (*impl_).current_text_clip(*canvas)) {
            const Impl::TextClipState copy = *state;
            (*impl_).text_clips.push_back(copy);
        }
        (*canvas).save();
    }
}

void SkiaRaster::restore() {
    if (SkCanvas* canvas = (*impl_).canvas();
        canvas && (*canvas).getSaveCount() > (*impl_).save_floor) {
        if ((*impl_).current_text_clip(*canvas) != nullptr && (*impl_).text_clips.size() > 1U) {
            (*impl_).text_clips.pop_back();
        }
        (*canvas).restore();
    }
}

void SkiaRaster::translate(Point offset) {
    if (SkCanvas* canvas = (*impl_).canvas()) {
        (*canvas).translate(static_cast<SkScalar>(offset.x), static_cast<SkScalar>(offset.y));
    }
}

void SkiaRaster::clip_rect(Rect rect) {
    if (SkCanvas* canvas = (*impl_).canvas()) {
        (*canvas).clipRect(to_sk_rect(rect), SkClipOp::kIntersect, false);
        if (Impl::TextClipState* const state = (*impl_).current_text_clip(*canvas)) {
            const SkRect device = (*canvas).getTotalMatrix().mapRect(to_sk_rect(rect));
            if (!(*state).rect.intersect(device)) (*state).rect.setEmpty();
        }
    }
}

void SkiaRaster::clip_rounded_rect(Rect rect, double radius) {
    if (SkCanvas* canvas = (*impl_).canvas()) {
        (*canvas).clipRRect(to_sk_rrect(rect, radius), SkClipOp::kIntersect, true);
        if (Impl::TextClipState* const state = (*impl_).current_text_clip(*canvas)) {
            const SkMatrix matrix = (*canvas).getTotalMatrix();
            const SkRect device = matrix.mapRect(to_sk_rect(rect));
            // One rounded clip is exact. An enclosing rounded clip is kept as
            // its bounds only: nested rounded clips are approximated.
            if ((*state).rounded && !(*state).rect.intersect((*state).rounded_rect)) (*state).rect.setEmpty();
            (*state).rounded = true;
            (*state).rounded_rect = device;
            (*state).radius = std::max(0.0, radius) * static_cast<double>(matrix.getScaleX());
            if (!(*state).rect.intersect(device)) (*state).rect.setEmpty();
        }
    }
}

void SkiaRaster::fill_rect(Rect rect, Color color) {
    if (SkCanvas* canvas = (*impl_).canvas()) {
        SkPaint paint = make_paint(color);
        const SkRect box = snapped_box(*canvas, rect, paint);
        (*canvas).drawRect(box, paint);
    }
}

void SkiaRaster::fill_rounded_rect(Rect rect, double radius, Color color) {
    if (SkCanvas* canvas = (*impl_).canvas()) {
        // The bounds snap, in device space; the corners keep their
        // antialiasing.
        const SkMatrix matrix = (*canvas).getTotalMatrix();
        SkRect device{};
        if (snapped_device_rect(matrix, rect, device)) {
            const double bounded = std::clamp(radius, 0.0, std::max(0.0, std::min(rect.width, rect.height) * 0.5));
            const SkVector radii = device_radii(matrix, bounded, device);
            (*canvas).save();
            (*canvas).resetMatrix();
            (*canvas).drawRRect(SkRRect::MakeRectXY(device, radii.x(), radii.y()), make_paint(color));
            (*canvas).restore();
            return;
        }
        const SkRect bounds = to_sk_rect(rect);
        const SkScalar bounded = static_cast<SkScalar>(std::clamp(
            radius, 0.0, std::max(0.0, static_cast<double>(std::min(bounds.width(), bounds.height())) * 0.5)));
        (*canvas).drawRRect(SkRRect::MakeRectXY(bounds, bounded, bounded), make_paint(color));
    }
}

void SkiaRaster::stroke_rect(Rect rect, Color color, double width) {
    if (SkCanvas* canvas = (*impl_).canvas()) {
        SkPaint paint = make_paint(color);
        // The stroke is centred on the edges. Snapped, it is the ring between
        // its snapped outer and inner edges, filled without antialiasing.
        const double half = width * 0.5;
        SkRect outer{};
        SkRect inner{};
        const bool snapped = std::isfinite(half) && half > 0.0 &&
            snapped_local_rect(*canvas, outset_rect(rect, half), outer) &&
            snapped_local_rect(*canvas, outset_rect(rect, -half), inner);
        if (snapped) {
            SkPathBuilder ring(SkPathFillType::kEvenOdd);
            ring.addRect(outer);
            if (!inner.isEmpty()) ring.addRect(inner);
            const SkPath path = ring.detach();
            paint.setAntiAlias(false);
            (*canvas).drawPath(path, paint);
            return;
        }
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStrokeWidth(static_cast<SkScalar>(width));
        (*canvas).drawRect(to_sk_rect(rect), paint);
    }
}

void SkiaRaster::stroke_rounded_rect(Rect rect, double radius, Color color,
                                    double width) {
    if (SkCanvas* canvas = (*impl_).canvas()) {
        SkPaint paint = make_paint(color);
        // The stroke is centred on the edges. Snapped, it is the ring between
        // its snapped outer and inner bounds, in device space as a rounded
        // fill snaps its bounds; the corners keep their antialiasing.
        const double half = width * 0.5;
        const SkMatrix matrix = (*canvas).getTotalMatrix();
        SkRect outer{};
        SkRect inner{};
        const bool snapped = std::isfinite(half) && half > 0.0 &&
            snapped_device_rect(matrix, outset_rect(rect, half), outer) &&
            snapped_device_rect(matrix, outset_rect(rect, -half), inner);
        if (snapped) {
            const double bounded = std::clamp(radius, 0.0, std::max(0.0, std::min(rect.width, rect.height) * 0.5));
            const SkVector outer_radii = device_radii(matrix, bounded + half, outer);
            const SkRRect outer_shape = SkRRect::MakeRectXY(outer, outer_radii.x(), outer_radii.y());
            (*canvas).save();
            (*canvas).resetMatrix();
            if (inner.isEmpty()) {
                (*canvas).drawRRect(outer_shape, paint);
            } else {
                const SkVector inner_radii = device_radii(matrix, std::max(0.0, bounded - half), inner);
                const SkRRect inner_shape = SkRRect::MakeRectXY(inner, inner_radii.x(), inner_radii.y());
                (*canvas).drawDRRect(outer_shape, inner_shape, paint);
            }
            (*canvas).restore();
            return;
        }
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStrokeWidth(static_cast<SkScalar>(width));
        (*canvas).drawRRect(to_sk_rrect(rect, radius), paint);
    }
}

void SkiaRaster::fill_linear_gradient(
    Rect rect, Point start, Point end,
    std::span<const GradientStop> stops) {
    fill_linear_gradient_spread(rect, start, end, stops,
                                GradientSpreadMode::pad);
}

void SkiaRaster::fill_linear_gradient_spread(
    Rect rect, Point start, Point end,
    std::span<const GradientStop> stops, GradientSpreadMode spread) {
    SkCanvas* canvas = (*impl_).canvas();
    if (canvas == nullptr || rect.empty() || !valid_gradient_stops(stops) ||
        (spread != GradientSpreadMode::pad &&
         spread != GradientSpreadMode::repeat &&
         spread != GradientSpreadMode::reflect)) {
        return;
    }
    const SkGradientData data = to_sk_gradient(stops);
    const SkPoint points[2]{{static_cast<SkScalar>(start.x),
                             static_cast<SkScalar>(start.y)},
                            {static_cast<SkScalar>(end.x),
                             static_cast<SkScalar>(end.y)}};
    sk_sp<SkShader> shader = SkShaders::LinearGradient(
        points, make_sk_gradient(data, spread));
    if (!shader) return;
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setShader(std::move(shader));
    const SkRect box = snapped_box(*canvas, rect, paint);
    (*canvas).drawRect(box, paint);
}

void SkiaRaster::fill_radial_gradient(
    Rect rect, Point center, Size radii,
    std::span<const GradientStop> stops) {
    SkCanvas* canvas = (*impl_).canvas();
    if (canvas == nullptr || rect.empty() || radii.width <= 0.0 ||
        radii.height <= 0.0 || !valid_gradient_stops(stops)) {
        return;
    }
    const SkGradientData data = to_sk_gradient(stops);
    sk_sp<SkShader> shader = SkShaders::RadialGradient(
        {0.0F, 0.0F}, 1.0F, make_sk_gradient(data));
    if (!shader) return;
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setShader(std::move(shader));
    (*canvas).save();
    SkRect box = to_sk_rect(rect);
    const bool snapped = snapped_local_rect(*canvas, rect, box);
    (*canvas).clipRect(box, SkClipOp::kIntersect, !snapped);
    (*canvas).translate(static_cast<SkScalar>(center.x),
                      static_cast<SkScalar>(center.y));
    (*canvas).scale(static_cast<SkScalar>(radii.width),
                  static_cast<SkScalar>(radii.height));
    (*canvas).drawRect(SkRect::MakeLTRB(
        static_cast<SkScalar>((rect.x - center.x) / radii.width),
        static_cast<SkScalar>((rect.y - center.y) / radii.height),
        static_cast<SkScalar>((rect.x + rect.width - center.x) / radii.width),
        static_cast<SkScalar>((rect.y + rect.height - center.y) / radii.height)),
        paint);
    (*canvas).restore();
}

void SkiaRaster::draw_box_shadow(Rect rect, double corner_radius, Point offset,
                                 double blur_radius, double spread,
                                 Color color) {
    SkCanvas* canvas = (*impl_).canvas();
    if (canvas == nullptr || rect.empty() || color.alpha == 0U ||
        blur_radius < 0.0) {
        return;
    }
    Rect shadow{rect.x + offset.x - spread, rect.y + offset.y - spread,
                rect.width + spread * 2.0, rect.height + spread * 2.0};
    if (shadow.empty()) return;
    SkPaint paint = make_paint(color);
    if (blur_radius > 0.0) {
        paint.setMaskFilter(SkMaskFilter::MakeBlur(
            kNormal_SkBlurStyle, static_cast<SkScalar>(blur_radius * 0.5),
            false));
    }
    (*canvas).drawRRect(to_sk_rrect(
        shadow, std::max(0.0, corner_radius + spread)), paint);
}

namespace {

// Blurred coverage of one retained draw_box_shadow command. Its geometry is
// fixed by the command; the mask depends only on the device scale and the
// subpixel phase of the shadow's device origin, not on its position.
class ShadowMaskMemo final : public RetainedDrawMemo {
public:
    SkScalar scale_x{};
    SkScalar scale_y{};
    SkScalar phase_x{};
    SkScalar phase_y{};
    // Mask pixel (0, 0) lies at floor(device origin) - margin.
    int margin{};
    sk_sp<SkImage> mask;
};

} // namespace

void SkiaRaster::draw_retained_box_shadow(const Rect rect, const double corner_radius,
                                          const Point offset, const double blur_radius,
                                          const double spread, const Color color,
                                          const std::shared_ptr<RetainedDrawCache>& cache) {
    SkCanvas* const canvas = (*impl_).canvas();
    if (canvas == nullptr || rect.empty() || color.alpha == 0U || blur_radius < 0.0) return;
    const SkMatrix matrix = (*canvas).getTotalMatrix();
    // Unblurred shadows, rotations and reflections draw directly.
    if (!cache || blur_radius == 0.0 || !matrix.isScaleTranslate() ||
        matrix.getScaleX() <= 0.0F || matrix.getScaleY() <= 0.0F) {
        draw_box_shadow(rect, corner_radius, offset, blur_radius, spread, color);
        return;
    }
    const Rect shadow{rect.x + offset.x - spread, rect.y + offset.y - spread,
                      rect.width + spread * 2.0, rect.height + spread * 2.0};
    if (shadow.empty()) return;
    const SkRRect local = to_sk_rrect(shadow, std::max(0.0, corner_radius + spread));
    const SkRect device = matrix.mapRect(local.rect());
    const SkScalar origin_x = std::floor(device.left());
    const SkScalar origin_y = std::floor(device.top());
    const SkScalar phase_x = device.left() - origin_x;
    const SkScalar phase_y = device.top() - origin_y;
    ShadowMaskMemo* memo = dynamic_cast<ShadowMaskMemo*>((*cache).memo.get());
    const bool current = memo != nullptr && (*memo).mask &&
        (*memo).scale_x == matrix.getScaleX() && (*memo).scale_y == matrix.getScaleY() &&
        (*memo).phase_x == phase_x && (*memo).phase_y == phase_y;
    if (!current) {
        // The mask filter's sigma is in device pixels (respectCTM is false).
        const double sigma = blur_radius * 0.5;
        const int margin = static_cast<int>(std::ceil(sigma * 3.0)) + 2;
        const int width = static_cast<int>(std::ceil(device.right() - origin_x)) + margin * 2;
        const int height = static_cast<int>(std::ceil(device.bottom() - origin_y)) + margin * 2;
        sk_sp<SkSurface> coverage = SkSurfaces::Raster(SkImageInfo::MakeA8(width, height));
        if (!coverage) {
            draw_box_shadow(rect, corner_radius, offset, blur_radius, spread, color);
            return;
        }
        SkCanvas& target = *(*coverage).getCanvas();
        target.clear(SK_ColorTRANSPARENT);
        // Same geometry, matrix and blur as draw_box_shadow, moved by whole
        // pixels so every coverage value is the one it would have produced.
        SkMatrix placement = matrix;
        placement.postTranslate(static_cast<SkScalar>(margin) - origin_x,
                                static_cast<SkScalar>(margin) - origin_y);
        target.setMatrix(placement);
        SkPaint opaque{};
        opaque.setAntiAlias(true);
        opaque.setColor(SK_ColorWHITE);
        opaque.setMaskFilter(SkMaskFilter::MakeBlur(
            kNormal_SkBlurStyle, static_cast<SkScalar>(sigma), false));
        target.drawRRect(local, opaque);
        std::unique_ptr<ShadowMaskMemo> replacement = std::make_unique<ShadowMaskMemo>();
        (*replacement).scale_x = matrix.getScaleX();
        (*replacement).scale_y = matrix.getScaleY();
        (*replacement).phase_x = phase_x;
        (*replacement).phase_y = phase_y;
        (*replacement).margin = margin;
        (*replacement).mask = (*coverage).makeImageSnapshot();
        memo = replacement.get();
        (*cache).memo = std::move(replacement);
    }
    // Apply the kept coverage through a mask filter so the color is blended
    // by the same mask blitter as draw_box_shadow; an alpha-image draw rounds
    // differently by one or two levels.
    const SkScalar left = origin_x - static_cast<SkScalar>((*memo).margin);
    const SkScalar top = origin_y - static_cast<SkScalar>((*memo).margin);
    const SkMatrix placement = SkMatrix::Translate(left, top);
    SkPaint paint = make_paint(color);
    paint.setMaskFilter(SkShaderMaskFilter::Make(
        (*(*memo).mask).makeShader(SkSamplingOptions(), &placement)));
    (*canvas).save();
    (*canvas).resetMatrix();
    (*canvas).drawRect(SkRect::MakeXYWH(left, top, static_cast<SkScalar>((*(*memo).mask).width()),
                                        static_cast<SkScalar>((*(*memo).mask).height())), paint);
    (*canvas).restore();
}

void SkiaRaster::draw_inset_box_shadow(
    Rect rect, double corner_radius, Point offset, double blur_radius,
    double spread, Color color) {
    SkCanvas* canvas = (*impl_).canvas();
    if (canvas == nullptr || rect.empty() || color.alpha == 0U ||
        blur_radius < 0.0) {
        return;
    }
    const Rect hole{rect.x + offset.x + spread,
                    rect.y + offset.y + spread,
                    rect.width - spread * 2.0,
                    rect.height - spread * 2.0};
    const double reach = std::max(rect.width, rect.height) +
                         std::abs(offset.x) + std::abs(offset.y) +
                         std::abs(spread) + blur_radius * 3.0 + 1.0;
    const Rect outside{rect.x - reach, rect.y - reach,
                       rect.width + reach * 2.0,
                       rect.height + reach * 2.0};
    // The fill type goes to the constructor, as in the executor's builders.
    // Set afterwards with setFillType, it did not reach the built path in
    // the sanitized build, whose toolkit is instrumented while libskia is
    // not: the even-odd hole was filled (#167).
    SkPathBuilder complement_builder(SkPathFillType::kEvenOdd);
    complement_builder.addRect(to_sk_rect(outside));
    if (!hole.empty()) {
        complement_builder.addRRect(to_sk_rrect(
            hole, std::max(0.0, corner_radius - spread)));
    }
    const SkPath complement = complement_builder.detach();
    SkPaint paint = make_paint(color);
    if (blur_radius > 0.0) {
        paint.setMaskFilter(SkMaskFilter::MakeBlur(
            kNormal_SkBlurStyle, static_cast<SkScalar>(blur_radius * 0.5),
            false));
    }
    (*canvas).save();
    (*canvas).clipRRect(to_sk_rrect(rect, corner_radius),
                       SkClipOp::kIntersect, true);
    (*canvas).drawPath(complement, paint);
    (*canvas).restore();
}

void SkiaRaster::draw_line(Point from, Point to, Color color, double width) {
    if (SkCanvas* canvas = (*impl_).canvas()) {
        SkPaint paint = make_paint(color);
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStrokeWidth(static_cast<SkScalar>(width));
        (*canvas).drawLine(static_cast<SkScalar>(from.x), static_cast<SkScalar>(from.y),
                         static_cast<SkScalar>(to.x), static_cast<SkScalar>(to.y), paint);
    }
}

void SkiaRaster::Impl::draw_shaped_text(SkCanvas& target, const text::ShapedText& shaped,
                                        const Point origin, const FontSpec font_spec, const Color color) {
    const SkMatrix matrix = target.getTotalMatrix();
    if (matrix.hasPerspective()) return;
    if (!glyph_runs) glyph_runs = std::make_unique<text::GlyphRunRasterizer>(text_engine);
    const SkPoint device = matrix.mapPoint(SkPoint::Make(static_cast<SkScalar>(origin.x), static_cast<SkScalar>(origin.y)));
    text::GlyphRunOrigin placement{};
    text::GlyphRunCoverage coverage{};
    if (matrix.isScaleTranslate() && matrix.getScaleX() > 0.0F && matrix.getScaleX() == matrix.getScaleY()) {
        placement = text::glyph_run_origin(static_cast<double>(device.x()), static_cast<double>(device.y()));
        coverage = (*glyph_runs).rasterize(shaped, font_spec, static_cast<double>(matrix.getScaleX()),
                                           placement.phase_x);
    } else {
        const text::GlyphRunTransform transform{static_cast<double>(matrix.getScaleX()),
                                                static_cast<double>(matrix.getSkewX()),
                                                static_cast<double>(matrix.getSkewY()),
                                                static_cast<double>(matrix.getScaleY())};
        coverage = (*glyph_runs).rasterize_transformed(shaped, font_spec, transform, static_cast<double>(device.x()),
                                                       static_cast<double>(device.y()), {}, placement);
    }
    if (coverage.pixels.empty()) return;
    const SkPixmap mask(SkImageInfo::MakeA8(static_cast<int>(coverage.width), static_cast<int>(coverage.height)),
                        coverage.pixels.data(), static_cast<std::size_t>(coverage.width));
    const sk_sp<SkImage> image = SkImages::RasterFromPixmapCopy(mask);
    if (!image) return;
    // An alpha-only image draws in the paint's colour.
    const SkPaint paint = make_paint(color);
    target.save();
    target.resetMatrix();
    target.drawImage(image, static_cast<SkScalar>(placement.pixel_x + coverage.left),
                     static_cast<SkScalar>(placement.pixel_y + coverage.top), SkSamplingOptions(), &paint);
    target.restore();
}

namespace {

// Shaping of one retained draw_text_utf8 command. Text and font are fixed by
// the command; the result also depends on which faces the engine holds.
class ShapedTextMemo final : public RetainedDrawMemo {
public:
    std::weak_ptr<const bool> engine;
    std::size_t face_count{};
    FontSpec font{};
    std::size_t text_bytes{};
    text::ShapedText shaped;
    // ADR-004 coverage of the shaped run at one device scale and phase.
    bool has_coverage{};
    double coverage_scale{};
    std::int32_t coverage_phase{};
    text::GlyphRunCoverage coverage;
};

} // namespace

void SkiaRaster::draw_retained_text_utf8(const Point origin, const std::string_view text,
                                         const FontSpec font_spec, const Color color,
                                         const std::shared_ptr<RetainedDrawCache>& cache) {
    SkCanvas* const canvas = (*impl_).canvas();
    if (canvas == nullptr || text.empty()) return;
    if (!cache) {
        draw_text_utf8(origin, text, font_spec, color);
        return;
    }
    const std::size_t face_count = (*impl_).text_engine.face_count();
    ShapedTextMemo* memo = dynamic_cast<ShapedTextMemo*>((*cache).memo.get());
    const bool current = memo != nullptr &&
        (*memo).engine.lock() == (*impl_).text_engine_identity &&
        (*memo).face_count == face_count && (*memo).font == font_spec &&
        (*memo).text_bytes == text.size();
    if (!current) {
        std::unique_ptr<ShapedTextMemo> replacement = std::make_unique<ShapedTextMemo>();
        (*replacement).engine = (*impl_).text_engine_identity;
        (*replacement).face_count = face_count;
        (*replacement).font = font_spec;
        (*replacement).text_bytes = text.size();
        (*replacement).shaped = (*impl_).text_engine.shape(text, font_spec);
        memo = replacement.get();
        (*cache).memo = std::move(replacement);
    }
    if ((*impl_).composite_run(*canvas, (*memo).shaped, font_spec, origin, color, &(*memo).coverage,
                               &(*memo).coverage_scale, &(*memo).coverage_phase)) {
        (*memo).has_coverage = true;
        return;
    }
    (*impl_).draw_shaped_text(*canvas, (*memo).shaped, origin, font_spec, color);
}

bool SkiaRaster::Impl::composite_run(SkCanvas& target, const text::ShapedText& shaped, const FontSpec font,
                                     const Point origin, const Color color, text::GlyphRunCoverage* const kept,
                                     double* const kept_scale, std::int32_t* const kept_phase) {
    TextClipState* const state = current_text_clip(target);
    if (state == nullptr) return false;
    SkPixmap pixmap{};
    if (!target.peekPixels(&pixmap) || pixmap.alphaType() != kPremul_SkAlphaType ||
        (pixmap.colorType() != kRGBA_8888_SkColorType && pixmap.colorType() != kBGRA_8888_SkColorType) ||
        pixmap.rowBytes() % 4U != 0U) {
        return false;
    }
    const SkMatrix matrix = target.getTotalMatrix();
    if (!matrix.isScaleTranslate() || matrix.getScaleX() <= 0.0F || matrix.getScaleX() != matrix.getScaleY()) return false;
    const double device_scale = static_cast<double>(matrix.getScaleX());
    const SkPoint device = matrix.mapPoint(SkPoint::Make(static_cast<SkScalar>(origin.x), static_cast<SkScalar>(origin.y)));
    const text::GlyphRunOrigin placement = text::glyph_run_origin(static_cast<double>(device.x()), static_cast<double>(device.y()));
    if (!glyph_runs) glyph_runs = std::make_unique<text::GlyphRunRasterizer>(text_engine);
    text::GlyphRunCoverage fresh{};
    const text::GlyphRunCoverage* coverage = &fresh;
    if (kept != nullptr && kept_scale != nullptr && kept_phase != nullptr && *kept_scale == device_scale &&
        *kept_phase == placement.phase_x && !(*kept).pixels.empty()) {
        coverage = kept;
    } else {
        fresh = (*glyph_runs).rasterize(shaped, font, device_scale, placement.phase_x);
        if (kept != nullptr && kept_scale != nullptr && kept_phase != nullptr) {
            *kept = std::move(fresh);
            *kept_scale = device_scale;
            *kept_phase = placement.phase_x;
            coverage = kept;
        }
    }
    if ((*coverage).pixels.empty()) return true;
    const detail::TextCoverage source{.pixels = (*coverage).pixels, .width = (*coverage).width,
                                      .height = (*coverage).height,
                                      .left = placement.pixel_x + (*coverage).left,
                                      .top = placement.pixel_y + (*coverage).top};
    const bool composited = composite_coverage(target, source, color);
    return composited;
}

std::optional<SkiaRaster::Impl::SharedComposite> SkiaRaster::Impl::shared_composite(SkCanvas& target) {
    TextClipState* const state = current_text_clip(target);
    if (state == nullptr) return std::nullopt;
    SkPixmap pixmap{};
    if (!target.peekPixels(&pixmap) || pixmap.alphaType() != kPremul_SkAlphaType ||
        (pixmap.colorType() != kRGBA_8888_SkColorType && pixmap.colorType() != kBGRA_8888_SkColorType) ||
        pixmap.rowBytes() % 4U != 0U) {
        return std::nullopt;
    }
    const std::size_t stride = pixmap.rowBytes() / 4U;
    const std::size_t pixel_count = stride * static_cast<std::size_t>(pixmap.height());
    detail::TextCompositeTarget composite_target{};
    composite_target.pixels = std::span<std::uint32_t>(pixmap.writable_addr32(0, 0), pixel_count);
    composite_target.width = static_cast<std::uint32_t>(pixmap.width());
    composite_target.height = static_cast<std::uint32_t>(pixmap.height());
    composite_target.stride = stride;
    composite_target.order = pixmap.colorType() == kRGBA_8888_SkColorType ? detail::TextPixelOrder::rgba
                                                                          : detail::TextPixelOrder::bgra;
    composite_target.opaque = false;
    detail::TextCompositeClip clip{};
    clip.rect = {static_cast<double>((*state).rect.left()), static_cast<double>((*state).rect.top()),
                 static_cast<double>((*state).rect.width()), static_cast<double>((*state).rect.height())};
    if ((*state).rounded) {
        clip.rounded = true;
        clip.rounded_rect = {static_cast<double>((*state).rounded_rect.left()), static_cast<double>((*state).rounded_rect.top()),
                             static_cast<double>((*state).rounded_rect.width()), static_cast<double>((*state).rounded_rect.height())};
        clip.radius = (*state).radius;
    }
    const SharedComposite shared{state, composite_target, clip};
    return shared;
}

bool SkiaRaster::Impl::composite_coverage(SkCanvas& target, const detail::TextCoverage& source, const Color color) {
    const std::optional<SharedComposite> shared = shared_composite(target);
    if (!shared) return false;
    const TextClipState* const state = (*shared).state;
    if ((*state).rect.isEmpty()) return true;
    const detail::TextCompositeTarget composite_target = (*shared).target;
    const detail::TextCompositeClip clip = (*shared).clip;
    if (!(*state).region_limited) {
        static_cast<void>(detail::composite_text_coverage(source, composite_target, clip, color));
        return true;
    }
    // Disjoint region rectangles: no pixel is blended twice.
    for (SkRegion::Iterator piece((*state).region); !piece.done(); piece.next()) {
        const SkIRect bounds = piece.rect();
        detail::TextCompositeClip piece_clip = clip;
        const Rect piece_rect{static_cast<double>(bounds.left()), static_cast<double>(bounds.top()),
                              static_cast<double>(bounds.width()), static_cast<double>(bounds.height())};
        piece_clip.rect = Rect::intersection(clip.rect, piece_rect);
        if (piece_clip.rect.empty()) continue;
        static_cast<void>(detail::composite_text_coverage(source, composite_target, piece_clip, color));
    }
    return true;
}

bool SkiaRaster::Impl::composite_part(SkCanvas& target, const PartRaster& raster, const Point origin,
                                      const double opacity) {
    const SkMatrix matrix = target.getTotalMatrix();
    const SkScalar raster_scale = static_cast<SkScalar>(raster.scale().value);
    if (!matrix.isScaleTranslate() || matrix.getScaleX() != raster_scale || matrix.getScaleY() != raster_scale) {
        return false;
    }
    // The raster's first pixel goes on the device pixel nearest its origin.
    const SkPoint device = matrix.mapPoint(SkPoint::Make(static_cast<SkScalar>(origin.x), static_cast<SkScalar>(origin.y)));
    const DeviceExtent extent = raster.extent();
    const detail::PartRasterSource source{.pixels = raster.pixels(), .width = extent.width, .height = extent.height,
                                          .row_bytes = raster.row_bytes(),
                                          .left = std::llround(static_cast<double>(device.x())),
                                          .top = std::llround(static_cast<double>(device.y()))};
    const bool composited = composite_source(target, source, opacity);
    return composited;
}

// As composite_part, stretched over `destination`: its corners go on the
// device pixels nearest them, and the shared blend samples the raster.
bool SkiaRaster::Impl::composite_part_scaled(SkCanvas& target, const PartRaster& raster, const Rect destination,
                                             const double opacity) {
    const SkMatrix matrix = target.getTotalMatrix();
    const SkScalar raster_scale = static_cast<SkScalar>(raster.scale().value);
    if (!matrix.isScaleTranslate() || matrix.getScaleX() != raster_scale || matrix.getScaleY() != raster_scale) {
        return false;
    }
    const SkPoint first = matrix.mapPoint(SkPoint::Make(static_cast<SkScalar>(destination.x),
                                                        static_cast<SkScalar>(destination.y)));
    const SkPoint last = matrix.mapPoint(SkPoint::Make(static_cast<SkScalar>(destination.x + destination.width),
                                                       static_cast<SkScalar>(destination.y + destination.height)));
    const long long left = std::llround(static_cast<double>(first.x()));
    const long long top = std::llround(static_cast<double>(first.y()));
    const long long right = std::llround(static_cast<double>(last.x()));
    const long long bottom = std::llround(static_cast<double>(last.y()));
    if (right <= left || bottom <= top) return true;
    const DeviceExtent extent = raster.extent();
    const detail::PartRasterSource source{.pixels = raster.pixels(), .width = extent.width, .height = extent.height,
                                          .row_bytes = raster.row_bytes(), .left = left, .top = top,
                                          .shown_width = static_cast<std::uint32_t>(right - left),
                                          .shown_height = static_cast<std::uint32_t>(bottom - top)};
    const bool composited = composite_source(target, source, opacity);
    return composited;
}

bool SkiaRaster::Impl::composite_source(SkCanvas& target, const detail::PartRasterSource& source,
                                        const double opacity) {
    const std::optional<SharedComposite> shared = shared_composite(target);
    if (!shared) return false;
    const TextClipState* const state = (*shared).state;
    if ((*state).rect.isEmpty()) return true;
    const std::uint8_t byte = detail::part_composite_opacity(opacity);
    const detail::TextCompositeTarget composite_target = (*shared).target;
    const detail::TextCompositeClip clip = (*shared).clip;
    if (!(*state).region_limited) {
        static_cast<void>(detail::composite_part_raster(source, composite_target, clip, byte));
        return true;
    }
    // Disjoint region rectangles: no pixel is blended twice.
    for (SkRegion::Iterator piece((*state).region); !piece.done(); piece.next()) {
        const SkIRect bounds = piece.rect();
        detail::TextCompositeClip piece_clip = clip;
        const Rect piece_rect{static_cast<double>(bounds.left()), static_cast<double>(bounds.top()),
                              static_cast<double>(bounds.width()), static_cast<double>(bounds.height())};
        piece_clip.rect = Rect::intersection(clip.rect, piece_rect);
        if (piece_clip.rect.empty()) continue;
        static_cast<void>(detail::composite_part_raster(source, composite_target, piece_clip, byte));
    }
    return true;
}

void SkiaRaster::draw_text_utf8(Point origin,
                                std::string_view text,
                                FontSpec font_spec,
                                Color color) {
    if (SkCanvas* canvas = (*impl_).canvas(); canvas && !text.empty()) {
        const text::ShapedText shaped = (*impl_).text_engine.shape(text, font_spec);
        if ((*impl_).composite_run(*canvas, shaped, font_spec, origin, color, nullptr, nullptr, nullptr)) return;
        (*impl_).draw_shaped_text(*canvas, shaped, origin, font_spec, color);
    }
}

void SkiaRaster::draw_turned_text_utf8(const Point origin, const std::string_view text, const FontSpec font_spec,
                                       const Color color, const TextOrientation orientation) {
    if (orientation == TextOrientation::horizontal) {
        draw_text_utf8(origin, text, font_spec, color);
        return;
    }
    SkCanvas* const canvas = (*impl_).canvas();
    if (canvas == nullptr || text.empty()) return;
    const text::ShapedText shaped = (*impl_).text_engine.shape(text, font_spec);
    // A positive Skia rotation turns clockwise on the y-down surface:
    // bottom_to_top is a quarter turn anticlockwise, top_to_bottom clockwise.
    const SkScalar degrees = orientation == TextOrientation::bottom_to_top ? -90.0F : 90.0F;
    (*canvas).save();
    (*canvas).translate(static_cast<SkScalar>(origin.x), static_cast<SkScalar>(origin.y));
    (*canvas).rotate(degrees);
    const Point run_origin{0.0, 0.0};
    (*impl_).draw_shaped_text(*canvas, shaped, run_origin, font_spec, color);
    (*canvas).restore();
}

Size SkiaRaster::measure_text_utf8(std::string_view text,
                                   FontSpec font_spec) {
    if (text.empty()) return {0.0, font_spec.size};
    const text::ShapedText shaped = (*impl_).text_engine.shape(text, font_spec);
    return {std::max(0.0, shaped.width), std::max(0.0, shaped.height)};
}

ResolvedTextLayout SkiaRaster::resolve_text_layout_utf8(
    std::string_view text, FontSpec font_spec) {
    return (*impl_).text_engine.resolve(text, font_spec);
}

void SkiaRaster::draw_image(ImageId image, Rect destination, double opacity) {
    const Impl::ImageMap::const_iterator found = (*impl_).images.find(image.value);
    if (SkCanvas* canvas = (*impl_).canvas();
        canvas && found != (*impl_).images.end() && opacity > 0.0) {
        SkPaint paint;
        paint.setAlphaf(static_cast<float>(std::clamp(opacity, 0.0, 1.0)));
        paint.setAntiAlias(true);
        (*canvas).drawImageRect((*found).second.image.get(), to_sk_rect(destination),
                              SkSamplingOptions(SkFilterMode::kLinear), &paint);
    }
}

void SkiaRaster::begin_group(const PaintGroup& group) {
    Impl& impl = *impl_;
    if (impl.group_depth >= maximum_paint_group_depth) {
        // Refused by name: drawn without isolation, limited to its bounds.
        save();
        clip_rect(group.bounds);
        impl.refused_open = impl.refused_open + 1U;
        impl.group_refusals = impl.group_refusals + 1U;
        return;
    }
    Impl::GroupLevel& level = impl.groups[impl.group_depth];
    level.isolated = false;
    level.opacity = group.opacity;
    level.composite = group.composite;
    save();
    clip_rect(group.bounds);
    const bool visible = group.opacity > 0.0;
    if (!visible) clip_rect(Rect{});
    impl.group_depth = impl.group_depth + 1U;
    SkCanvas* const parent = impl.canvas();
    const bool isolate = group.opacity < 1.0 || group.isolated || group.composite != PaintGroupComposite::source_over;
    if (!visible || !isolate || parent == nullptr) return;
    // The group covers the recorded clip, which now includes its bounds, in
    // the parent's device pixels. A parent whose clips or pixels are unknown
    // draws the group directly.
    const Impl::TextClipState* const state = impl.current_text_clip(*parent);
    SkPixmap parent_pixels{};
    if (state == nullptr || !(*parent).peekPixels(&parent_pixels)) return;
    SkIRect device = (*state).rect.roundOut();
    if (!device.intersect(SkIRect::MakeWH(parent_pixels.width(), parent_pixels.height()))) return;
    const int width = device.width();
    const int height = device.height();
    if (!level.surface || (*level.surface).width() < width || (*level.surface).height() < height) {
        const int grown_width = level.surface ? std::max(width, (*level.surface).width()) : width;
        const int grown_height = level.surface ? std::max(height, (*level.surface).height()) : height;
        const SkImageInfo info = SkImageInfo::Make(grown_width, grown_height, kRGBA_8888_SkColorType,
                                                   kPremul_SkAlphaType, SkColorSpace::MakeSRGB());
        sk_sp<SkSurface> grown = SkSurfaces::Raster(info);
        if (!grown) return;
        level.surface = std::move(grown);
    }
    SkCanvas& canvas = *(*level.surface).getCanvas();
    canvas.restoreToCount(1);
    canvas.resetMatrix();
    canvas.save();
    canvas.clipRect(SkRect::MakeIWH(width, height), SkClipOp::kIntersect, false);
    canvas.clear(SK_ColorTRANSPARENT);
    SkMatrix matrix = (*parent).getTotalMatrix();
    matrix.postTranslate(static_cast<SkScalar>(-device.left()), static_cast<SkScalar>(-device.top()));
    canvas.setMatrix(matrix);
    // The group's own recorded clips: its whole surface region. The
    // parent's rounded clip and damage region apply when it composites.
    level.parent_text_clips = std::move(impl.text_clips);
    level.parent_text_clip_base = impl.text_clip_base;
    level.parent_save_floor = impl.save_floor;
    impl.text_clips.clear();
    Impl::TextClipState base{};
    base.rect = SkRect::MakeIWH(width, height);
    impl.text_clips.push_back(std::move(base));
    impl.text_clip_base = canvas.getSaveCount();
    impl.save_floor = canvas.getSaveCount();
    level.device = device;
    level.isolated = true;
}

void SkiaRaster::end_group() {
    Impl& impl = *impl_;
    if (impl.refused_open > 0U) {
        impl.refused_open = impl.refused_open - 1U;
        restore();
        return;
    }
    if (impl.group_depth == 0U) return;
    Impl::GroupLevel& level = impl.groups[impl.group_depth - 1U];
    if (!level.isolated) {
        impl.group_depth = impl.group_depth - 1U;
        restore();
        return;
    }
    SkCanvas& canvas = *(*level.surface).getCanvas();
    canvas.restoreToCount(1);
    impl.text_clips = std::move(level.parent_text_clips);
    impl.text_clip_base = level.parent_text_clip_base;
    impl.save_floor = level.parent_save_floor;
    level.isolated = false;
    impl.group_depth = impl.group_depth - 1U;
    // Back on the parent: its clip as the group began, then the composite.
    restore();
    SkCanvas* const parent = impl.canvas();
    SkPixmap pixmap{};
    if (parent == nullptr || !(*level.surface).peekPixels(&pixmap)) return;
    const std::size_t width = static_cast<std::size_t>(level.device.width());
    const std::size_t height = static_cast<std::size_t>(level.device.height());
    const std::size_t length = (height - 1U) * pixmap.rowBytes() + width * 4U;
    const std::span<const std::uint8_t> bytes(static_cast<const std::uint8_t*>(pixmap.addr()), length);
    const detail::PartRasterSource source{.pixels = bytes, .width = static_cast<std::uint32_t>(width),
                                          .height = static_cast<std::uint32_t>(height),
                                          .row_bytes = pixmap.rowBytes(), .left = level.device.left(),
                                          .top = level.device.top(), .order = detail::TextPixelOrder::rgba,
                                          .blend = detail::part_composite_blend(level.composite)};
    static_cast<void>(impl.composite_source(*parent, source, level.opacity));
}

std::uint64_t SkiaRaster::group_refusals() const noexcept {
    return (*impl_).group_refusals;
}

void SkiaRaster::draw_part_raster(const std::shared_ptr<const PartRaster>& raster, const Point origin,
                                  const double opacity) {
    SkCanvas* const canvas = (*impl_).canvas();
    if (canvas == nullptr || !raster) return;
    static_cast<void>((*impl_).composite_part(*canvas, *raster, origin, opacity));
}

void SkiaRaster::draw_part_raster_scaled(const std::shared_ptr<const PartRaster>& raster, const Rect destination,
                                         const double opacity) {
    SkCanvas* const canvas = (*impl_).canvas();
    if (canvas == nullptr || !raster) return;
    static_cast<void>((*impl_).composite_part_scaled(*canvas, *raster, destination, opacity));
}

void SkiaRaster::draw_live_surface(const std::shared_ptr<LiveSurface> surface,
                                   const Rect destination, const double opacity) {
    if (!surface) return;
    const LiveSurfaceFrame frame = (*surface).acquire_latest();
    static_cast<void>(draw_live_surface_frame(frame, destination, opacity));
}

bool SkiaRaster::draw_live_surface_frame(const LiveSurfaceFrame& frame,
                                         const Rect destination, const double opacity) {
    SkCanvas* const canvas = (*impl_).canvas();
    if (canvas == nullptr || destination.empty() ||
        !destination.finite() || !std::isfinite(opacity) || opacity <= 0.0) {
        return false;
    }
    if (!frame || frame.width() == 0U || frame.height() == 0U ||
        frame.row_bytes() < static_cast<std::uint64_t>(frame.width()) * 4U ||
        frame.pixels().empty()) {
        return false;
    }
    const bool copy = frame.opaque() && opacity >= 1.0;
    const SkAlphaType alpha = copy ? kOpaque_SkAlphaType : kPremul_SkAlphaType;
    SkColorType color_type = kUnknown_SkColorType;
    switch (frame.pixel_format()) {
    case LiveSurfacePixelFormat::bgra32_premultiplied_srgb:
        color_type = kBGRA_8888_SkColorType;
        break;
    case LiveSurfacePixelFormat::rgba32_premultiplied_srgb:
        color_type = kRGBA_8888_SkColorType;
        break;
    default: return false;
    }
    if (fail_next_live_image_) {
        fail_next_live_image_ = false;
        return false;
    }
    const SkImageInfo info = SkImageInfo::Make(
        static_cast<int>(frame.width()), static_cast<int>(frame.height()),
        color_type, alpha,
        SkColorSpace::MakeSRGB());
    SkIRect device{};
    const bool pixel_exact = live_surface_device_rect(*canvas, destination, frame, device);
    if (copy && pixel_exact && (*canvas).isClipRect()) {
        // writePixels ignores both matrix and clip. The raster's isClipRect
        // admits only full-coverage rectangular clips; complex/AA clips must
        // go through drawImageRect so retained overlays and edges survive.
        SkIRect clipped = device;
        if (!clipped.intersect((*canvas).getDeviceClipBounds())) return true;
        const std::size_t source_y = static_cast<std::size_t>(clipped.top() - device.top());
        const std::size_t source_x = static_cast<std::size_t>(clipped.left() - device.left());
        const std::size_t stride = static_cast<std::size_t>(frame.row_bytes());
        const std::size_t offset = source_y * stride + source_x * 4U;
        // Alpha is promised 255, so it is also valid premultiplied storage.
        // Match the raster alpha type to retain Skia's byte-copy path.
        const SkImageInfo clipped_info = info.makeWH(clipped.width(), clipped.height());
        const SkImageInfo copy_info = clipped_info.makeAlphaType(kPremul_SkAlphaType);
        const bool written = (*canvas).writePixels(copy_info, frame.pixels().data() + offset,
                                                   stride, clipped.left(), clipped.top());
        return written;
    }
    sk_sp<SkData> data = SkData::MakeWithoutCopy(
        frame.pixels().data(), frame.pixels().size());
    sk_sp<SkImage> image = SkImages::RasterFromData(
        info, std::move(data), static_cast<size_t>(frame.row_bytes()));
    if (!image) return false;

    SkPaint paint{};
    if (copy) paint.setBlendMode(SkBlendMode::kSrc);
    paint.setAlphaf(static_cast<float>(std::clamp(opacity, 0.0, 1.0)));
    paint.setAntiAlias(false);
    (*canvas).drawImageRect(
        image.get(),
        SkRect::MakeWH(static_cast<SkScalar>(frame.width()),
                       static_cast<SkScalar>(frame.height())),
        to_sk_rect(destination),
        SkSamplingOptions(copy && pixel_exact ? SkFilterMode::kNearest : SkFilterMode::kLinear), &paint,
        SkCanvas::kStrict_SrcRectConstraint);
    return true;
}

void SkiaRaster::draw_image_region(ImageId image, Rect source,
                                   Rect destination, double opacity) {
    draw_image_region_sampled(image, source, destination,
                              ImageSampling::linear, opacity);
}

void SkiaRaster::draw_image_region_sampled(
    ImageId image, Rect source, Rect destination, ImageSampling sampling,
    double opacity) {
    const Impl::ImageMap::const_iterator found = (*impl_).images.find(image.value);
    SkCanvas* canvas = (*impl_).canvas();
    if (canvas == nullptr || found == (*impl_).images.end() || source.empty() ||
        destination.empty() || !source.finite() || !destination.finite() ||
        !std::isfinite(opacity) || opacity <= 0.0) {
        return;
    }
    const Rect image_bounds{
        0.0, 0.0,
        static_cast<double>((*(*found).second.image).width()),
        static_cast<double>((*(*found).second.image).height())};
    if (!image_bounds.contains(source)) return;
    SkPaint paint;
    paint.setAlphaf(static_cast<float>(std::clamp(opacity, 0.0, 1.0)));
    // Sampling filters pixels; rectangle coverage must stay hard-edged.
    // Coverage AA attenuates a tile edge even through a disjoint hard clip
    // when its one-pixel sampling gutter is subpixel after minification.
    paint.setAntiAlias(false);
    (*canvas).drawImageRect((*found).second.image.get(), to_sk_rect(source),
                          to_sk_rect(destination),
                          SkSamplingOptions(sampling == ImageSampling::nearest
                              ? SkFilterMode::kNearest : SkFilterMode::kLinear),
                          &paint,
                          SkCanvas::kStrict_SrcRectConstraint);
}

void SkiaRaster::fill_image_pattern(
    ImageId image, Size source_pixel_size, Rect destination,
    Size logical_tile_size, ImagePatternWrap wrap, double opacity) {
    const Impl::ImageMap::const_iterator found = (*impl_).images.find(image.value);
    SkCanvas* canvas = (*impl_).canvas();
    if (canvas == nullptr || found == (*impl_).images.end() ||
        wrap != ImagePatternWrap::tile || destination.empty() ||
        !destination.finite() || !std::isfinite(source_pixel_size.width) ||
        !std::isfinite(source_pixel_size.height) ||
        !std::isfinite(logical_tile_size.width) ||
        !std::isfinite(logical_tile_size.height) ||
        source_pixel_size.width != (*(*found).second.image).width() ||
        source_pixel_size.height != (*(*found).second.image).height() ||
        logical_tile_size.width <= 0.0 || logical_tile_size.height <= 0.0 ||
        !std::isfinite(opacity) || opacity <= 0.0) {
        return;
    }
    SkMatrix local;
    local.setScaleTranslate(
        static_cast<SkScalar>(logical_tile_size.width /
                              source_pixel_size.width),
        static_cast<SkScalar>(logical_tile_size.height /
                              source_pixel_size.height),
        static_cast<SkScalar>(destination.x),
        static_cast<SkScalar>(destination.y));
    SkPaint paint;
    paint.setAlphaf(static_cast<float>(std::clamp(opacity, 0.0, 1.0)));
    paint.setAntiAlias(false);
    paint.setShader((*(*found).second.image).makeShader(
        SkTileMode::kRepeat, SkTileMode::kRepeat,
        SkSamplingOptions(SkFilterMode::kLinear), local));
    (*canvas).drawRect(to_sk_rect(destination), paint);
}

} // namespace gui_forms::render
