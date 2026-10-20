#pragma once

#include "gui_forms/engine.hpp"
#include "gui_forms/types.hpp"
#include "text_identity_scene.hpp"

#include <array>
#include <cmath>
#include <cstdint>
#include <memory>
#include <stdexcept>

// The theme engine's composite identity scene (T2 plan, E2b; GIVEN A2-2):
// part rasters drawn by the engine's own primitives, composited through each
// host's painter with Painter::draw_part_raster. The same painter calls must
// produce the same bytes on every platform. Hashes and dumps use the text
// identity scene's helpers (RGBA order).
namespace gui_forms::part_composite_identity {

enum class Case : std::uint8_t { plain, faded, translated, fractional_clip, rounded_clip };

inline constexpr std::array<Case, 5> cases{Case::plain, Case::faded, Case::translated, Case::fractional_clip,
                                           Case::rounded_clip};
inline constexpr std::array<const char*, 5> case_names{"plain", "faded", "translated", "fractional_clip",
                                                       "rounded_clip"};
// Every origin below lands on a whole device pixel at each scale, so no host
// rounds a half.
inline constexpr std::array<double, 3> scales{1.0, 1.5, 2.0};
inline constexpr Size scene_size{120.0, 48.0};
inline constexpr Size part_size{40.0, 28.0};

// Expected FNV-1a hashes of the raster's rows in RGBA order, [case][scale],
// produced by the Windows DIB painter (Windows x64, clang 22), 2026-10-20.
// GUI_FORMS_PART_COMPOSITE_PRINT=1 prints the observed table instead.
inline constexpr std::array<std::array<std::uint64_t, 3>, 5> expected{{
    {0x76930cce6775bc50ULL, 0xf84b6b81bb71753bULL, 0x8909e0d61d0afca7ULL},
    {0x2879fea809874149ULL, 0xf2d5c2ad77f8e3c1ULL, 0x52535b4d03fe0b25ULL},
    {0x20f5676ad1fd236eULL, 0xff8276d6c39faf91ULL, 0x372daa5e29ddbfafULL},
    {0xc61f56c660cdabd8ULL, 0x0ac903046e6e2f4bULL, 0x6b5e8248d3012bc8ULL},
    {0xf502e0731d9858dbULL, 0xaf4544f70dbfb94fULL, 0xf8149137f584bfd2ULL},
}};

// One part raster: a built-in primitive drawn at `scale` into a raster the
// size of part_size at that scale, rounded up.
inline std::shared_ptr<const PartRaster> make_part(const char* primitive_name, bool pressed, double scale) {
    const EnginePrimitive* const primitive = builtin_primitive_registry().find(primitive_name);
    if (primitive == nullptr) throw std::runtime_error("the built-in primitive is registered");
    const DeviceExtent extent{static_cast<std::uint32_t>(std::ceil(part_size.width * scale)),
                              static_cast<std::uint32_t>(std::ceil(part_size.height * scale))};
    const std::shared_ptr<PartRaster> raster = PartRaster::create(extent, DeviceScale{scale});
    if (!raster) throw std::runtime_error("the part raster is made");
    AnatomyStateSet states{};
    if (pressed) states.insert(AnatomyState::pressed);
    const PrimitiveTarget target{part_size, DeviceScale{scale}, states};
    const PrimitiveParameterBlock parameters = default_parameters((*primitive).descriptor());
    PrimitivePreparation preparation{};
    if ((*primitive).prepare(parameters, target, preparation) != PreparationOutcome::prepared) {
        throw std::runtime_error("the part prepares");
    }
    if ((*primitive).rasterize(parameters, target, preparation, (*raster).view()) != RasterOutcome::painted) {
        throw std::runtime_error("the part rasterizes");
    }
    return raster;
}

// Paints the scene after the frame has begun, at the painter's `scale`.
inline void paint_scene(Painter& painter, const Case kind, const double scale) {
    painter.fill_rect({0.0, 0.0, scene_size.width, scene_size.height}, Color::rgba(222, 232, 246, 255));
    painter.fill_rect({0.0, 24.0, scene_size.width, 24.0}, Color::rgba(40, 70, 110, 255));
    // The dithered bulge is opaque dots over transparency; the smooth bulge
    // is translucent premultiplied colour.
    const std::shared_ptr<const PartRaster> dithered = make_part("dithered_bulge", false, scale);
    const std::shared_ptr<const PartRaster> smooth = make_part("bulge", true, scale);
    const double opacity = kind == Case::faded ? 0.5 : 1.0;
    painter.save();
    if (kind == Case::fractional_clip) painter.clip_rect({9.3, 3.4, 70.5, 33.75});
    if (kind == Case::rounded_clip) painter.clip_rounded_rect({10.0, 9.0, 84.0, 26.0}, 10.0);
    if (kind == Case::translated) painter.translate({2.0, 4.0});
    painter.draw_part_raster(dithered, Point{6.0, 6.0}, opacity);
    painter.draw_part_raster(smooth, Point{58.0, 12.0}, opacity);
    painter.restore();
}

} // namespace gui_forms::part_composite_identity
