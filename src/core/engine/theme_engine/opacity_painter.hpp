#pragma once

#include "gui_forms/types/painter/painter.hpp"

#include <vector>

namespace gui_forms::detail {

// A part's opacity (T2 GIVEN C1-1) for a part the built-in recipe draws
// (E7). The recipe draws with Painter commands, not into a raster, so the
// opacity cannot be applied in the shared blend as it is for a primitive's
// raster: this painter forwards every command to `target` with each colour's
// alpha, and each image, live surface and raster opacity, multiplied by
// `opacity`. Where the part's own commands overlap (a plate and its edge),
// each is blended at that opacity in turn, so a half-faded part shows its
// lower commands through its upper ones; at opacity 1 nothing changes, and
// a part at opacity 0 is not drawn at all (the engine skips it).
//
// It borrows `target` for its own life, which is one part's paint.
class OpacityPainter final : public Painter {
public:
    OpacityPainter(Painter& target, double opacity) noexcept;

    [[nodiscard]] std::unique_ptr<PaintFramebuffer> create_framebuffer(Size logical_size, double scale) override;
#if defined(GUI_FORMS_PREPARED_TEXT)
    [[nodiscard]] PreparedTextPaintResult draw_prepared_text(const PreparedTextLayout& layout,
                                                             const LayoutAuthority expected, const Point baseline,
                                                             const Color color) override;
#endif
    void save() override;
    void restore() override;
    void translate(Point offset) override;
    void clip_rect(Rect rect) override;
    void clip_rounded_rect(Rect rect, double radius) override;
    void fill_rect(Rect rect, Color color) override;
    void fill_rounded_rect(Rect rect, double radius, Color color) override;
    void stroke_rect(Rect rect, Color color, double width) override;
    void stroke_rounded_rect(Rect rect, double radius, Color color, double width) override;
    void fill_linear_gradient(Rect rect, Point start, Point end, std::span<const GradientStop> stops) override;
    void fill_linear_gradient_spread(Rect rect, Point start, Point end, std::span<const GradientStop> stops,
                                     GradientSpreadMode spread) override;
    void fill_radial_gradient(Rect rect, Point center, Size radii, std::span<const GradientStop> stops) override;
    void draw_box_shadow(Rect rect, double corner_radius, Point offset, double blur_radius, double spread,
                         Color color) override;
    void draw_retained_box_shadow(Rect rect, double corner_radius, Point offset, double blur_radius, double spread,
                                  Color color, const std::shared_ptr<RetainedDrawCache>& cache) override;
    void draw_inset_box_shadow(Rect rect, double corner_radius, Point offset, double blur_radius, double spread,
                               Color color) override;
    void draw_line(Point from, Point to, Color color, double width) override;
    void draw_text_utf8(Point origin, std::string_view text, FontSpec font, Color color) override;
    void draw_retained_text_utf8(Point origin, std::string_view text, FontSpec font, Color color,
                                 const std::shared_ptr<RetainedDrawCache>& cache) override;
    void draw_turned_text_utf8(Point origin, std::string_view text, FontSpec font, Color color,
                               TextOrientation orientation) override;
    [[nodiscard]] Size measure_text_utf8(std::string_view text, FontSpec font) override;
    [[nodiscard]] ResolvedTextLayout resolve_text_layout_utf8(std::string_view text, FontSpec font) override;
    void draw_image(ImageId image, Rect destination, double opacity) override;
    void draw_live_surface(std::shared_ptr<LiveSurface> surface, Rect destination, double opacity) override;
    void draw_part_raster(const std::shared_ptr<const PartRaster>& raster, Point origin, double opacity) override;
    void draw_image_region(ImageId image, Rect source, Rect destination, double opacity) override;
    void draw_image_region_sampled(ImageId image, Rect source, Rect destination, ImageSampling sampling,
                                   double opacity) override;
    void fill_image_pattern(ImageId image, Size source_pixel_size, Rect destination, Size logical_tile_size,
                            ImagePatternWrap wrap, double opacity) override;

private:
    [[nodiscard]] Color faded(Color color) const noexcept;
    [[nodiscard]] double faded(double opacity) const noexcept;
    // The stops with their colours faded, in stops_scratch_, which is
    // reused by every gradient so that a fade allocates once.
    [[nodiscard]] std::span<const GradientStop> faded(std::span<const GradientStop> stops);

    Painter* target_;
    double opacity_;
    std::vector<GradientStop> stops_scratch_;
};

} // namespace gui_forms::detail
