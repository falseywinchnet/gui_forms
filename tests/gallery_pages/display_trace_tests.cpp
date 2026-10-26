#include "support.hpp"

#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <string>
#include <string_view>

namespace gallery_pages_tests {
namespace {

// Every Gallery page's display commands, recorded as text and hashed (T2
// plan, phase E2: with no theme rules, painting through the theme engine
// records exactly the commands the built-in recipe recorded before it).
//
// The trace writes each command with its arguments. Lengths are written in
// 1/64 logical px, rounded, so that a last-bit difference between hosts in
// layout arithmetic does not change the trace; colours, images, text and
// fonts are written exactly. The expected table was recorded on the
// commit before the engine entry point (main at 13d1c7e0), re-recorded only
// under the rule above the table, and is checked on every CI platform.
// GUI_FORMS_GALLERY_TRACE_PRINT=1 prints the observed
// table instead of checking it, and GUI_FORMS_GALLERY_TRACE_DIR=<directory>
// writes each page's trace to <directory>/<page>.trace.
//
// The month calendars show "today", which the system clock supplies; the
// test pins it so that the trace does not depend on the date.
//
// Two pages show values that the page's own content does not fix:
// - live-surfaces reports its surface's pixel format, which is the
//   platform's native one (native_live_surface_pixel_format: BGRA on
//   Windows, RGBA elsewhere), so it has one digest for Windows and one for
//   the other platforms, which differ only in that word;
// - typography-and-inspection's inspector shows the inspected control's
//   RuntimeId, a process-wide counter of every control made so far. The
//   test therefore runs alone in a process of its own, its own ctest entry
//   (gui_forms_gallery_display_trace_tests, running
//   gui_forms_gallery_pages_tests --display-trace), so the counter has the
//   same value on every run and host whatever other tests do.

class TracePainter final : public Painter {
public:
    void save() override { begin("save"); end(); }
    void restore() override { begin("restore"); end(); }
    void translate(Point offset) override {
        begin("translate");
        point(offset);
        end();
    }
    void clip_rect(Rect rect) override {
        begin("clip_rect");
        rectangle(rect);
        end();
    }
    void clip_rounded_rect(Rect rect, double radius) override {
        begin("clip_rounded_rect");
        rectangle(rect);
        length(radius);
        end();
    }
    void fill_rect(Rect rect, Color color) override {
        begin("fill_rect");
        rectangle(rect);
        colour(color);
        end();
    }
    void fill_rounded_rect(Rect rect, double radius, Color color) override {
        begin("fill_rounded_rect");
        rectangle(rect);
        length(radius);
        colour(color);
        end();
    }
    void stroke_rect(Rect rect, Color color, double width) override {
        begin("stroke_rect");
        rectangle(rect);
        colour(color);
        length(width);
        end();
    }
    void stroke_rounded_rect(Rect rect, double radius, Color color, double width) override {
        begin("stroke_rounded_rect");
        rectangle(rect);
        length(radius);
        colour(color);
        length(width);
        end();
    }
    void fill_linear_gradient(Rect rect, Point start, Point finish, std::span<const GradientStop> stops) override {
        begin("fill_linear_gradient");
        rectangle(rect);
        point(start);
        point(finish);
        gradient(stops);
        end();
    }
    void fill_linear_gradient_spread(Rect rect, Point start, Point finish, std::span<const GradientStop> stops,
                                     GradientSpreadMode spread) override {
        begin("fill_linear_gradient_spread");
        rectangle(rect);
        point(start);
        point(finish);
        gradient(stops);
        number(static_cast<long long>(spread));
        end();
    }
    void fill_radial_gradient(Rect rect, Point center, Size radii, std::span<const GradientStop> stops) override {
        begin("fill_radial_gradient");
        rectangle(rect);
        point(center);
        length(radii.width);
        length(radii.height);
        gradient(stops);
        end();
    }
    void draw_box_shadow(Rect rect, double corner_radius, Point offset, double blur_radius, double spread,
                         Color color) override {
        begin("draw_box_shadow");
        shadow(rect, corner_radius, offset, blur_radius, spread, color);
        end();
    }
    void draw_retained_box_shadow(Rect rect, double corner_radius, Point offset, double blur_radius, double spread,
                                  Color color, const std::shared_ptr<RetainedDrawCache>&) override {
        begin("draw_box_shadow");
        shadow(rect, corner_radius, offset, blur_radius, spread, color);
        end();
    }
    void draw_inset_box_shadow(Rect rect, double corner_radius, Point offset, double blur_radius, double spread,
                               Color color) override {
        begin("draw_inset_box_shadow");
        shadow(rect, corner_radius, offset, blur_radius, spread, color);
        end();
    }
    void draw_line(Point from, Point to, Color color, double width) override {
        begin("draw_line");
        point(from);
        point(to);
        colour(color);
        length(width);
        end();
    }
    void draw_text_utf8(Point origin, std::string_view text, FontSpec font, Color color) override {
        begin("draw_text");
        text_run(origin, text, font, color);
        end();
    }
    void draw_retained_text_utf8(Point origin, std::string_view text, FontSpec font, Color color,
                                 const std::shared_ptr<RetainedDrawCache>&) override {
        begin("draw_text");
        text_run(origin, text, font, color);
        end();
    }
    void draw_turned_text_utf8(Point origin, std::string_view text, FontSpec font, Color color,
                               TextOrientation orientation) override {
        begin("draw_turned_text");
        text_run(origin, text, font, color);
        number(static_cast<long long>(orientation));
        end();
    }
    void draw_image(ImageId image, Rect destination, double opacity) override {
        begin("draw_image");
        number(static_cast<long long>(image.value));
        rectangle(destination);
        length(opacity);
        end();
    }
    void draw_live_surface(std::shared_ptr<LiveSurface>, Rect destination, double opacity) override {
        begin("draw_live_surface");
        rectangle(destination);
        length(opacity);
        end();
    }
    void draw_part_raster(const std::shared_ptr<const PartRaster>& raster, Point origin, double opacity) override {
        begin("draw_part_raster");
        if (raster) {
            const DeviceExtent extent = (*raster).extent();
            number(static_cast<long long>(extent.width));
            number(static_cast<long long>(extent.height));
            length((*raster).scale().value);
        }
        point(origin);
        length(opacity);
        end();
    }
    void draw_image_region(ImageId image, Rect source, Rect destination, double opacity) override {
        begin("draw_image_region");
        number(static_cast<long long>(image.value));
        rectangle(source);
        rectangle(destination);
        length(opacity);
        end();
    }
    void draw_image_region_sampled(ImageId image, Rect source, Rect destination, ImageSampling sampling,
                                   double opacity) override {
        begin("draw_image_region_sampled");
        number(static_cast<long long>(image.value));
        rectangle(source);
        rectangle(destination);
        number(static_cast<long long>(sampling));
        length(opacity);
        end();
    }
    void fill_image_pattern(ImageId image, Size source_pixel_size, Rect destination, Size logical_tile_size,
                            ImagePatternWrap wrap, double opacity) override {
        begin("fill_image_pattern");
        number(static_cast<long long>(image.value));
        length(source_pixel_size.width);
        length(source_pixel_size.height);
        rectangle(destination);
        length(logical_tile_size.width);
        length(logical_tile_size.height);
        number(static_cast<long long>(wrap));
        length(opacity);
        end();
    }

    // The trace so far, one command per line.
    std::string trace;

private:
    void begin(const char* name) { trace += name; }
    void end() { trace += '\n'; }
    void number(long long value) {
        trace += ' ';
        trace += std::to_string(value);
    }
    // A length in 1/64 logical px, rounded; non-finite values are written
    // by name.
    void length(double value) {
        if (!std::isfinite(value)) {
            trace += value != value ? " nan" : (value > 0.0 ? " inf" : " -inf");
            return;
        }
        const double scaled = value * 64.0;
        number(std::llround(scaled));
    }
    void point(Point value) {
        length(value.x);
        length(value.y);
    }
    void rectangle(Rect value) {
        length(value.x);
        length(value.y);
        length(value.width);
        length(value.height);
    }
    void colour(Color value) {
        const unsigned int packed = (static_cast<unsigned int>(value.red) << 24U) |
                                    (static_cast<unsigned int>(value.green) << 16U) |
                                    (static_cast<unsigned int>(value.blue) << 8U) | value.alpha;
        std::array<char, 12> text{};
        std::snprintf(text.data(), text.size(), " #%08x", packed);
        trace += text.data();
    }
    void gradient(std::span<const GradientStop> stops) {
        number(static_cast<long long>(stops.size()));
        for (const GradientStop& stop : stops) {
            length(stop.offset);
            colour(stop.color);
        }
    }
    void shadow(Rect rect, double corner_radius, Point offset, double blur_radius, double spread, Color color) {
        rectangle(rect);
        length(corner_radius);
        point(offset);
        length(blur_radius);
        length(spread);
        colour(color);
    }
    void text_run(Point origin, std::string_view text, FontSpec font, Color color) {
        point(origin);
        trace += " \"";
        trace += text;
        trace += '"';
        number(static_cast<long long>(font.role));
        length(font.size);
        number(static_cast<long long>(font.weight));
        number(font.italic ? 1 : 0);
        length(font.letter_spacing);
        colour(color);
    }
};

std::uint64_t trace_hash(std::string_view trace) {
    std::uint64_t hash = 14695981039346656037ULL;
    for (const char character : trace) {
        hash = hash ^ static_cast<unsigned char>(character);
        hash = hash * 1099511628211ULL;
    }
    return hash;
}

// Pins every month calendar's "today" in the subtree, so that no trace
// depends on the date the test runs.
void pin_today(const Control::Ptr& control) {
    if (const std::shared_ptr<MonthCalendar> calendar = std::dynamic_pointer_cast<MonthCalendar>(control)) {
        (*calendar).set_today_date(DateTimeValue{2026, 10, 19});
    }
    for (const Control::Ptr& child : (*control).children()) pin_today(child);
}

struct PageDigest final {
    std::string_view page;
    std::uint64_t hash;
};

// Recorded on main at 13d1c7e0, before the theme engine's entry point, and
// re-recorded when stage L, phase L3 added the css-layout-panels page.
//
// Re-recording is allowed only when the page-by-page trace diffs
// (GUI_FORMS_GALLERY_TRACE_DIR, before and after) show intended changes
// alone, and the pull request's body must list them. L3's diffs: the new
// page; on every page the navigation sidebar's new entry, its taller paper
// and its scroll; on states-and-diagnostics the live control count and the
// paint chunks reused; on typography-and-inspection the inspected
// specimen's RuntimeId, its generation and the control count. Nothing else
// changed.
//
// Main's ThreadExceptionDialog, OpenFileDialog and SaveFileDialog lines on
// dialogs-and-host-services, and its TabControl images on tabs-and-pages,
// were recorded on main as well; with L3 rebased onto them, those two
// pages, states-and-diagnostics (1247 controls) and
// typography-and-inspection were recorded again, and every other page kept
// L3's digest.
//
// typography-and-inspection's entry is the trace with the RuntimeId the
// specimen has when this test runs first (#1280 since L3 on main); the
// digest changes whenever a page made before it makes a control more or
// less.
constexpr std::array<PageDigest, 30> expected_digests{{
    {"basics", 0x39b168d11c02f1a0ULL},
    {"values", 0x7d3d6f7a4762c460ULL},
    {"retained-rows", 0x64869a82d1cde5efULL},
    {"instrument", 0x2c1fcdba71058e01ULL},
    {"control-spectrum", 0x321965cb85e7988dULL},
    {"ranges-and-progress", 0x3fce5fb922ccc1b3ULL},
    {"containers-and-focus", 0x3db877c0a3a4159dULL},
    {"animation-laboratory", 0x5cdbb66bceae7737ULL},
    {"states-and-diagnostics", 0x3507e70b182fac55ULL},
    {"text-and-input", 0x355fc10c66b1625aULL},
    {"collections-and-popups", 0xed03175675f86512ULL},
    {"values-and-spinners", 0xc39e52352d46689fULL},
    {"images-and-drawing", 0x3e2ade93d0061b3bULL},
    {"tabs-and-pages", 0xb8177e51c8f143e6ULL},
    {"checked-collections", 0x0f98e570c6e049a2ULL},
    {"timing-and-tooltips", 0x8acc5f5800f761e3ULL},
    {"dates-and-calendar", 0x7ca7f0d58684b984ULL},
    {"dialogs-and-host-services", 0x795ab0672bc3aa46ULL},
    {"retained-layout-panels", 0xeb41e855ad458af7ULL},
    {"css-layout-panels", 0x33c900923ba2ed92ULL},
    {"dock-and-anchor", 0x62078a167b51337fULL},
    {"commands-and-menus", 0xea7a7b2ac5c63701ULL},
    {"tree-and-object-views", 0x7a6a3c9d088ac325ULL},
    {"choices-and-sections", 0x50d153a9d9d56c55ULL},
    {"value-editors", 0xedf1413988628bbdULL},
    {"responsive-layouts", 0x30aba8f5526dfd0cULL},
    {"anchored-popups", 0xc8319f42bf15c17aULL},
    {"live-surfaces", 0xe474c9d97d329af1ULL},
    {"typography-and-inspection", 0x0857e60787f17785ULL},
    {"semantic-feedback", 0x48ae5da6e449bdc4ULL},
}};

// live-surfaces names the platform's native live-surface pixel format.
#if defined(_WIN32)
constexpr std::uint64_t live_surfaces_digest = 0xe474c9d97d329af1ULL;
#else
constexpr std::uint64_t live_surfaces_digest = 0x0d3c5e3226cbaef1ULL;
#endif

std::uint64_t expected_digest(std::string_view page) {
    if (page == "live-surfaces") return live_surfaces_digest;
    for (const PageDigest& digest : expected_digests) {
        if (digest.page == page) return digest.hash;
    }
    return 0U;
}

} // namespace

void test_display_commands_unchanged() {
    const char* const print_value = std::getenv("GUI_FORMS_GALLERY_TRACE_PRINT");
    const bool printing = print_value != nullptr && std::strcmp(print_value, "1") == 0;
    const char* const directory = std::getenv("GUI_FORMS_GALLERY_TRACE_DIR");
    std::unique_ptr<Window> owned = gallery::make_gallery();
    Window& window = *owned;
    window.perform_layout();
    std::string mismatches;
    for (const gallery::GalleryPageSpec& page : gallery::gallery_pages) {
        select_page(window, page.id);
        pin_today(window.root());
        window.perform_layout();
        TracePainter painter;
        window.paint(painter, {0.0, 0.0, 1280.0, 820.0});
        const std::uint64_t hash = trace_hash(painter.trace);
        if (directory != nullptr) {
            const std::string path = std::string(directory) + "/" + std::string(page.id) + ".trace";
            std::ofstream file(path, std::ios::binary);
            file << painter.trace;
        }
        if (printing) {
            std::cout << "    {\"" << page.id << "\", 0x" << std::hex << std::setw(16) << std::setfill('0') << hash
                      << std::dec << "ULL},\n";
            continue;
        }
        if (hash != expected_digest(page.id)) {
            mismatches += std::string(page.id) + " ";
        }
    }
    if (!mismatches.empty()) {
        std::cerr << "display commands differ on pages: " << mismatches << '\n';
    }
    require(mismatches.empty(), "every Gallery page records the commands it recorded before the theme engine");
}

} // namespace gallery_pages_tests
