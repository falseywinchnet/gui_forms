#pragma once

#include "gui_forms/types/geometry/geometry.hpp"
#include "gui_forms/types/paint_types/paint_types.hpp"
#include "gui_forms/typography.hpp"

#include <memory>
#include <span>
#include <string_view>

namespace gui_forms {

class LiveSurface;
class PaintFramebuffer;
#if defined(GUI_FORMS_PREPARED_TEXT)
class PreparedTextLayout;
struct LayoutAuthority;
struct PreparedTextPaintResult;
#endif

// Renderer-derived data for one retained draw command, such as shaped text.
// The command's recording owns it: a Window creates it when a control
// records the command and releases it when that control's display chunk is
// rebuilt or retired. A renderer fills, validates and replaces the memo.
class RetainedDrawMemo {
public:
    virtual ~RetainedDrawMemo() = default;
};

struct RetainedDrawCache final {
    std::unique_ptr<RetainedDrawMemo> memo;
};

class Painter : public TextMetricsProvider {
public:
    virtual ~Painter() = default;
    // Native renderers preserve their fonts and drawing semantics offscreen.
    // Recording/headless painters explicitly report this facility unavailable.
    [[nodiscard]] virtual std::unique_ptr<PaintFramebuffer> create_framebuffer(
        Size logical_size, double scale);
#if defined(GUI_FORMS_PREPARED_TEXT)
    // Development surface: recording retains immutable storage; a compatible
    // backend stages pixels. Only the host's successful frame commit presents.
    [[nodiscard]] virtual PreparedTextPaintResult draw_prepared_text(
        const PreparedTextLayout& layout, const LayoutAuthority expected, const Point baseline, const Color color);
#endif

    virtual void save() = 0;
    virtual void restore() = 0;
    virtual void translate(Point offset) = 0;
    virtual void clip_rect(Rect rect) = 0;
    // Rich geometry has deterministic renderer-neutral fallbacks so a minimal
    // host stays coherent.  Recording and production raster painters override
    // these operations to preserve the authored material exactly.
    virtual void clip_rounded_rect(Rect rect, double radius);
    virtual void fill_rect(Rect rect, Color color) = 0;
    virtual void fill_rounded_rect(Rect rect, double radius, Color color);
    virtual void stroke_rect(Rect rect, Color color, double width) = 0;
    virtual void stroke_rounded_rect(Rect rect, double radius, Color color,
                                     double width);
    virtual void fill_linear_gradient(
        Rect rect, Point start, Point end,
        std::span<const GradientStop> stops);
    virtual void fill_linear_gradient_spread(
        Rect rect, Point start, Point end,
        std::span<const GradientStop> stops, GradientSpreadMode spread);
    virtual void fill_radial_gradient(
        Rect rect, Point center, Size radii,
        std::span<const GradientStop> stops);
    // Drawn before the owning surface fill.  The shape body may therefore be
    // included by a fallback without changing the final composited result.
    virtual void draw_box_shadow(Rect rect, double corner_radius, Point offset,
                                 double blur_radius, double spread,
                                 Color color);
    // Replay of a retained draw_box_shadow. A renderer may keep the blurred
    // coverage in the cache; the default draws as draw_box_shadow.
    virtual void draw_retained_box_shadow(
        Rect rect, double corner_radius, Point offset, double blur_radius,
        double spread, Color color, const std::shared_ptr<RetainedDrawCache>& cache);
    // Drawn over the owning surface fill and clipped to its interior. The
    // source is the complement of a translated, spread-adjusted inner box, so
    // the recipe relaxes with live owner geometry rather than fixed traces.
    virtual void draw_inset_box_shadow(Rect rect, double corner_radius,
                                       Point offset, double blur_radius,
                                       double spread, Color color);
    virtual void draw_line(Point from, Point to, Color color, double width) = 0;
    virtual void draw_text_utf8(Point origin,
                                std::string_view text,
                                FontSpec font,
                                Color color) = 0;
    // Replay of a retained draw_text_utf8. The cache lives as long as the
    // recorded command; a renderer may keep shaped text there. The default
    // draws as draw_text_utf8 and leaves the cache untouched.
    virtual void draw_retained_text_utf8(
        Point origin, std::string_view text, FontSpec font, Color color,
        const std::shared_ptr<RetainedDrawCache>& cache);
    // Text controls need the same metrics used by the painter for caret hit
    // testing, selection geometry, and horizontal viewport maintenance.  The
    // renderer owns those metrics; controls must not guess a fixed glyph width.
    [[nodiscard]] virtual Size measure_text_utf8(std::string_view text,
                                                 FontSpec font) {
        std::size_t scalars{};
        for (const unsigned char byte : text) {
            if ((byte & 0xc0U) != 0x80U) ++scalars;
        }
        const double tracking = scalars > 1U
            ? static_cast<double>(scalars - 1U) * font.letter_spacing : 0.0;
        return {static_cast<double>(scalars) * font.size * 0.55 + tracking,
                font.size * 1.2};
    }
    [[nodiscard]] ResolvedTextLayout resolve_text_layout_utf8(
        std::string_view text, FontSpec font) override {
        ResolvedTextLayout result = estimate_text_layout_utf8(text, font);
        result.logical_size = measure_text_utf8(text, font);
        return result;
    }
    virtual void draw_image(ImageId image,
                            Rect destination,
                            double opacity = 1.0) = 0;
    // A live raster is a retained renderer-neutral resource whose producer may
    // publish independently of the UI dispatcher. Display lists retain the
    // resource and terminal renderers sample only its newest complete frame.
    virtual void draw_live_surface(std::shared_ptr<LiveSurface> surface,
                                   Rect destination,
                                   double opacity = 1.0);
    // Source-region replay is the renderer-neutral primitive behind image
    // strips, texture fills, and nine-patch materials. `source` is expressed
    // in source-image pixels. Minimal painters may preserve coherence by
    // falling back to whole-image scaling; production painters override it.
    virtual void draw_image_region(ImageId image, Rect source,
                                   Rect destination,
                                   double opacity = 1.0);
    virtual void draw_image_region_sampled(
        ImageId image, Rect source, Rect destination, ImageSampling sampling,
        double opacity = 1.0);
    // One retained command regardless of repetition count. Production
    // painters realize the pattern exactly; the base implementation is a
    // bounded compatibility fallback for deliberately minimal hosts.
    virtual void fill_image_pattern(ImageId image, Size source_pixel_size,
                                    Rect destination, Size logical_tile_size,
                                    ImagePatternWrap wrap = ImagePatternWrap::tile,
                                    double opacity = 1.0);
};

} // namespace gui_forms
