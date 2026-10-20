// Content is drawn exactly as given (T2 GIVEN E2-1 and H1-1), also when its
// part is disabled: through the Windows DIB painter, a disabled image part
// and a disabled content colour write their given pixels, while a disabled
// icon (a symbol, not content) is dimmed.
#include "gui_forms/control.hpp"
#include "gui_forms/theme.hpp"
#include "../src/host/windows/application/windows_host.cpp"

#include <array>
#include <cstdint>
#include <iostream>

using namespace gui_forms;

namespace {

int failures = 0;

void require(bool condition, const char* message) {
    if (condition) return;
    std::cerr << message << '\n';
    failures = failures + 1;
}

// The DIB pixel at (x, y) as 0xAARRGGBB, with the alpha the DIB keeps.
std::uint32_t pixel_at(PaintFramebuffer& framebuffer, std::uint32_t x, std::uint32_t y) {
    const std::span<std::byte> pixels = framebuffer.pixels();
    const std::size_t offset = static_cast<std::size_t>(y) * framebuffer.row_bytes() + static_cast<std::size_t>(x) * 4U;
    const std::uint32_t blue = static_cast<std::uint32_t>(pixels[offset]);
    const std::uint32_t green = static_cast<std::uint32_t>(pixels[offset + 1U]);
    const std::uint32_t red = static_cast<std::uint32_t>(pixels[offset + 2U]);
    const std::uint32_t value = (red << 16U) | (green << 8U) | blue;
    return value;
}

} // namespace

int main() {
    // A 2 x 2 opaque picture, premultiplied BGRA rows.
    const std::array<std::uint8_t, 16> picture{
        10, 20, 200, 255,  30, 160, 40, 255,
        220, 90, 15, 255,  5, 5, 5, 255,
    };
    const std::array<std::uint32_t, 4> picture_rgb{0xC8140AU, 0x28A01EU, 0x0F5ADCU, 0x050505U};
    ImageRegistry registry{};
    const ImageLoadResult loaded = registry.load_bgra32_premultiplied(
        2U, 2U, 8U, std::span<const std::byte>(reinterpret_cast<const std::byte*>(picture.data()), picture.size()));
    require(loaded.error == ImageResourceError::none, "the picture loads");

    const std::shared_ptr<Control> control = make_control<Control>(StableId("content"));
    host::DibPainter host_painter;
    const std::unique_ptr<PaintFramebuffer> framebuffer = host_painter.create_framebuffer({12.0, 4.0}, 1.0);
    require(framebuffer != nullptr, "the framebuffer is allocated");
    if (framebuffer == nullptr) return 1;
    require((*framebuffer).begin(registry, {0.0, 0.0, 12.0, 4.0}), "the frame begins with the picture");
    Painter& painter = (*framebuffer).painter();
    painter.fill_rect({0.0, 0.0, 12.0, 4.0}, Color::rgba(255, 255, 255, 255));

    AnatomyStateSet disabled{};
    disabled.insert(AnatomyState::disabled);
    PartPaint image{};
    image.role = PartRole::image;
    image.bounds = {0.0, 0.0, 2.0, 2.0};
    image.image = loaded.image;
    image.states = disabled;
    paint_part(painter, *control, image);

    PartPaint icon = image;
    icon.role = PartRole::icon;
    icon.bounds = {4.0, 0.0, 2.0, 2.0};
    paint_part(painter, *control, icon);

    const Color sample = Color::rgba(200, 40, 90, 255);
    PartPaint swatch{};
    swatch.role = PartRole::fill;
    swatch.bounds = {8.0, 0.0, 2.0, 2.0};
    swatch.content_color = sample;
    swatch.states = disabled;
    paint_part(painter, *control, swatch);
    (*framebuffer).end();

    for (std::uint32_t y = 0U; y < 2U; ++y) {
        for (std::uint32_t x = 0U; x < 2U; ++x) {
            require(pixel_at(*framebuffer, x, y) == picture_rgb[y * 2U + x],
                    "a disabled image part writes its picture's pixels exactly");
            require(pixel_at(*framebuffer, 8U + x, y) == 0xC8285AU,
                    "a disabled content colour writes its given colour exactly");
        }
    }
    require(pixel_at(*framebuffer, 4U, 0U) != picture_rgb[0], "a disabled icon is dimmed, unlike content");
    if (failures != 0) return 1;
    std::cout << "windows content pixel tests passed\n";
    return 0;
}
