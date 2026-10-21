// A part's opacity for the built-in recipe (T2 plan, phase E7): every
// command forwarded with its alpha and opacity multiplied.

#include "opacity_painter.hpp"

#include "gui_forms/paint_framebuffer.hpp"
#if defined(GUI_FORMS_PREPARED_TEXT)
#include "gui_forms/prepared_text/types/prepared_text_types.hpp"
#endif

#include <cmath>
#include <utility>

// The faded alpha, alpha × opacity + 0.5 rounded down, must be the same on
// every host: no fused multiply-add (T2 plan, "Numerics").
#if defined(__clang__)
#pragma clang fp contract(off)
#endif

namespace gui_forms::detail {

OpacityPainter::OpacityPainter(Painter& target, double opacity) noexcept : target_(&target), opacity_(opacity) {}

Color OpacityPainter::faded(Color color) const noexcept {
    // Straight alpha: only the alpha channel scales, rounded to nearest.
    const double alpha = static_cast<double>(color.alpha) * opacity_;
    const double rounded = std::floor(alpha + 0.5);
    color.alpha = static_cast<std::uint8_t>(rounded);
    return color;
}

double OpacityPainter::faded(double opacity) const noexcept {
    const double product = opacity * opacity_;
    return product;
}

std::span<const GradientStop> OpacityPainter::faded(std::span<const GradientStop> stops) {
    stops_scratch_.assign(stops.begin(), stops.end());
    for (GradientStop& stop : stops_scratch_) stop.color = faded(stop.color);
    const std::span<const GradientStop> result(stops_scratch_.data(), stops_scratch_.size());
    return result;
}

std::unique_ptr<PaintFramebuffer> OpacityPainter::create_framebuffer(Size logical_size, double scale) {
    std::unique_ptr<PaintFramebuffer> framebuffer = (*target_).create_framebuffer(logical_size, scale);
    return framebuffer;
}

#if defined(GUI_FORMS_PREPARED_TEXT)
PreparedTextPaintResult OpacityPainter::draw_prepared_text(const PreparedTextLayout& layout,
                                                           const LayoutAuthority expected, const Point baseline,
                                                           const Color color) {
    const PreparedTextPaintResult result = (*target_).draw_prepared_text(layout, expected, baseline, faded(color));
    return result;
}
#endif

void OpacityPainter::save() { (*target_).save(); }
void OpacityPainter::restore() { (*target_).restore(); }
void OpacityPainter::translate(Point offset) { (*target_).translate(offset); }
void OpacityPainter::clip_rect(Rect rect) { (*target_).clip_rect(rect); }
void OpacityPainter::clip_rounded_rect(Rect rect, double radius) { (*target_).clip_rounded_rect(rect, radius); }

void OpacityPainter::fill_rect(Rect rect, Color color) { (*target_).fill_rect(rect, faded(color)); }

void OpacityPainter::fill_rounded_rect(Rect rect, double radius, Color color) {
    (*target_).fill_rounded_rect(rect, radius, faded(color));
}

void OpacityPainter::stroke_rect(Rect rect, Color color, double width) {
    (*target_).stroke_rect(rect, faded(color), width);
}

void OpacityPainter::stroke_rounded_rect(Rect rect, double radius, Color color, double width) {
    (*target_).stroke_rounded_rect(rect, radius, faded(color), width);
}

void OpacityPainter::fill_linear_gradient(Rect rect, Point start, Point end, std::span<const GradientStop> stops) {
    const std::span<const GradientStop> shown = faded(stops);
    (*target_).fill_linear_gradient(rect, start, end, shown);
}

void OpacityPainter::fill_linear_gradient_spread(Rect rect, Point start, Point end,
                                                 std::span<const GradientStop> stops, GradientSpreadMode spread) {
    const std::span<const GradientStop> shown = faded(stops);
    (*target_).fill_linear_gradient_spread(rect, start, end, shown, spread);
}

void OpacityPainter::fill_radial_gradient(Rect rect, Point center, Size radii, std::span<const GradientStop> stops) {
    const std::span<const GradientStop> shown = faded(stops);
    (*target_).fill_radial_gradient(rect, center, radii, shown);
}

void OpacityPainter::draw_box_shadow(Rect rect, double corner_radius, Point offset, double blur_radius,
                                     double spread, Color color) {
    (*target_).draw_box_shadow(rect, corner_radius, offset, blur_radius, spread, faded(color));
}

void OpacityPainter::draw_retained_box_shadow(Rect rect, double corner_radius, Point offset, double blur_radius,
                                              double spread, Color color,
                                              const std::shared_ptr<RetainedDrawCache>& cache) {
    (*target_).draw_retained_box_shadow(rect, corner_radius, offset, blur_radius, spread, faded(color), cache);
}

void OpacityPainter::draw_inset_box_shadow(Rect rect, double corner_radius, Point offset, double blur_radius,
                                           double spread, Color color) {
    (*target_).draw_inset_box_shadow(rect, corner_radius, offset, blur_radius, spread, faded(color));
}

void OpacityPainter::draw_line(Point from, Point to, Color color, double width) {
    (*target_).draw_line(from, to, faded(color), width);
}

void OpacityPainter::draw_text_utf8(Point origin, std::string_view text, FontSpec font, Color color) {
    (*target_).draw_text_utf8(origin, text, font, faded(color));
}

void OpacityPainter::draw_retained_text_utf8(Point origin, std::string_view text, FontSpec font, Color color,
                                             const std::shared_ptr<RetainedDrawCache>& cache) {
    (*target_).draw_retained_text_utf8(origin, text, font, faded(color), cache);
}

void OpacityPainter::draw_turned_text_utf8(Point origin, std::string_view text, FontSpec font, Color color,
                                           TextOrientation orientation) {
    (*target_).draw_turned_text_utf8(origin, text, font, faded(color), orientation);
}

Size OpacityPainter::measure_text_utf8(std::string_view text, FontSpec font) {
    const Size measured = (*target_).measure_text_utf8(text, font);
    return measured;
}

ResolvedTextLayout OpacityPainter::resolve_text_layout_utf8(std::string_view text, FontSpec font) {
    const ResolvedTextLayout resolved = (*target_).resolve_text_layout_utf8(text, font);
    return resolved;
}

void OpacityPainter::draw_image(ImageId image, Rect destination, double opacity) {
    (*target_).draw_image(image, destination, faded(opacity));
}

void OpacityPainter::draw_live_surface(std::shared_ptr<LiveSurface> surface, Rect destination, double opacity) {
    (*target_).draw_live_surface(std::move(surface), destination, faded(opacity));
}

void OpacityPainter::draw_part_raster(const std::shared_ptr<const PartRaster>& raster, Point origin,
                                      double opacity) {
    (*target_).draw_part_raster(raster, origin, faded(opacity));
}

void OpacityPainter::draw_image_region(ImageId image, Rect source, Rect destination, double opacity) {
    (*target_).draw_image_region(image, source, destination, faded(opacity));
}

void OpacityPainter::draw_image_region_sampled(ImageId image, Rect source, Rect destination, ImageSampling sampling,
                                               double opacity) {
    (*target_).draw_image_region_sampled(image, source, destination, sampling, faded(opacity));
}

void OpacityPainter::fill_image_pattern(ImageId image, Size source_pixel_size, Rect destination,
                                        Size logical_tile_size, ImagePatternWrap wrap, double opacity) {
    (*target_).fill_image_pattern(image, source_pixel_size, destination, logical_tile_size, wrap, faded(opacity));
}

} // namespace gui_forms::detail
