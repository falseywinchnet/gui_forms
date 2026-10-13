// Stage DP on the Windows raster: painting two distant damage rectangles
// changes pixels only inside them, and the painted area is their sum.
// Exercises the private DIB painter in the same translation unit.
#include "gui_forms/basic_controls.hpp"
#include "gui_forms/window.hpp"
#include "../src/host/windows/application/windows_host.cpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <span>
#include <string_view>
#include <vector>

using namespace gui_forms;

namespace {

void expect(bool condition, std::string_view message) {
    if (condition) return;
    std::cerr << "windows damage rectangles failure: " << message << '\n';
    std::exit(EXIT_FAILURE);
}

// The 32-bit top-down pixels of a BMP save_bmp wrote, after its 54-byte
// file and info headers.
std::vector<std::uint32_t> read_pixels(const char* path, std::size_t count) {
    std::ifstream file(path, std::ios::binary);
    std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(file)), {});
    file.close();
    std::remove(path);
    constexpr std::size_t header_bytes = 54U;
    expect(bytes.size() == header_bytes + count * 4U, "a saved raster has the expected size");
    std::vector<std::uint32_t> pixels(count);
    for (std::size_t index = 0; index < count; ++index) {
        const std::size_t at = header_bytes + index * 4U;
        pixels[index] = static_cast<std::uint32_t>(bytes[at]) |
            (static_cast<std::uint32_t>(bytes[at + 1U]) << 8U) |
            (static_cast<std::uint32_t>(bytes[at + 2U]) << 16U) |
            (static_cast<std::uint32_t>(bytes[at + 3U]) << 24U);
    }
    return pixels;
}

bool inside_any(double x, double y, std::span<const Rect> rectangles) {
    for (std::size_t index = 0; index < rectangles.size(); ++index) {
        if (rectangles[index].contains(Point{x, y})) return true;
    }
    return false;
}

} // namespace

int main() {
#if defined(GUI_FORMS_WINDOWS_TRANSACTIONAL_DIB)
    // The transactional store exposes only committed frames; the default
    // live raster is the one this probe reads.
    std::cout << "gui_forms_windows_damage_rectangles_tests: skipped for the transactional DIB profile\n";
    return EXIT_SUCCESS;
#else
    const std::shared_ptr<Panel> root = make_control<Panel>(StableId("dp.root"));
    const std::shared_ptr<Panel> top_left = make_control<Panel>(StableId("dp.top-left"));
    const std::shared_ptr<Panel> bottom_right = make_control<Panel>(StableId("dp.bottom-right"));
    (*top_left).set_bounds({10.0, 10.0, 40.0, 30.0});
    (*bottom_right).set_bounds({340.0, 250.0, 40.0, 30.0});
    (*root).add_child(top_left);
    (*root).add_child(bottom_right);
    const Size size{400.0, 300.0};
    Window window(root, size);
    window.perform_layout();

    host::DibPainter painter;
    expect(painter.resize(size, 1.0), "the raster is allocated");
    expect(painter.synchronize_images(window.image_resources()), "images synchronize");
    painter.begin_frame();
    expect(window.paint(painter).has_value(), "the first frame paints");
    static_cast<void>(window.take_damage());
    expect(painter.save_bmp(L"dp_before.bmp"), "the first frame saves");

    (*top_left).set_background(Color::rgba(200, 40, 40));
    (*bottom_right).set_background(Color::rgba(40, 40, 200));
    window.flush();
    const DamageRegion damage = window.take_damage();
    expect(damage.rectangle_count() == 2U, "two distant mutations keep two rectangles");
    const std::span<const Rect> rectangles = damage.rectangles();

    const MetricsSnapshot before = window.metrics_snapshot();
    painter.begin_frame();
    expect(window.paint(painter, rectangles).has_value(), "the rectangle set paints");
    const MetricsSnapshot after = window.metrics_snapshot();
    expect(painter.save_bmp(L"dp_after.bmp"), "the second frame saves");

    const std::size_t width = static_cast<std::size_t>(size.width);
    const std::size_t height = static_cast<std::size_t>(size.height);
    const std::vector<std::uint32_t> first = read_pixels("dp_before.bmp", width * height);
    const std::vector<std::uint32_t> second = read_pixels("dp_after.bmp", width * height);
    std::size_t changed_inside = 0;
    std::size_t changed_outside = 0;
    for (std::size_t y = 0; y < height; ++y) {
        for (std::size_t x = 0; x < width; ++x) {
            const std::size_t index = y * width + x;
            if (first[index] == second[index]) continue;
            const double centre_x = static_cast<double>(x) + 0.5;
            const double centre_y = static_cast<double>(y) + 0.5;
            if (inside_any(centre_x, centre_y, rectangles)) {
                ++changed_inside;
            } else {
                ++changed_outside;
            }
        }
    }
    double expected_area = 0.0;
    for (std::size_t index = 0; index < rectangles.size(); ++index) {
        expected_area += rectangles[index].area();
    }
    const double painted = after.painted_damage_area - before.painted_damage_area;
    const double bounding = after.paint_bounding_area - before.paint_bounding_area;
    std::cout << "rectangles=" << rectangles.size() << " painted_area=" << painted
              << " bounding_area=" << bounding << " changed_inside=" << changed_inside
              << " changed_outside=" << changed_outside << '\n';
    expect(changed_outside == 0U, "no pixel outside the damaged rectangles changes");
    expect(changed_inside > 0U, "the damaged rectangles repaint");
    const double difference = painted - expected_area;
    expect(difference < 1.0e-6 && difference > -1.0e-6, "painted area equals the rectangles' sum");
    expect(bounding > painted * 10.0, "the bounding box would have cost far more");
    std::cout << "gui_forms_windows_damage_rectangles_tests: all tests passed\n";
    return EXIT_SUCCESS;
#endif
}
