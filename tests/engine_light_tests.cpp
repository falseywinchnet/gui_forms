// The engine's light primitives (T2 plan, phase E9). The overlay shadow
// (GIVEN E1-1) is an engine primitive with the first-party blur: a shadow
// part is drawn by the engine as the material kernel draws the theme's
// overlay shadow layers as outer shadows, identically on every host, with
// the same layers and damage outsets as before and none in high contrast;
// a shadow reaching past the primitive's outsets falls back to
// Painter::draw_box_shadow, counted.

#include "gui_forms/gui_forms.hpp"

#include "../src/core/engine/primitives/material/material_primitive.hpp"
#include "../src/core/engine/primitives/material/overlay_shadow_primitive.hpp"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#if defined(__clang__)
#pragma clang fp contract(off)
#endif

namespace {
using namespace gui_forms;

void require(bool condition, const char* message) {
    if (condition) return;
    std::cerr << "gui_forms_engine_light_tests: " << message << '\n';
    std::exit(1);
}

bool print_mode(int argc, char** argv) {
    const bool printing = argc > 1 && std::string_view(argv[1]) == "--print";
    return printing;
}

// FNV-1a over bytes, 64 bits.
std::uint64_t hash_bytes(std::span<const std::uint8_t> bytes) {
    std::uint64_t hash = 0xcbf29ce484222325ULL;
    for (const std::uint8_t byte : bytes) {
        hash = hash ^ byte;
        hash = hash * 0x100000001b3ULL;
    }
    return hash;
}

// A painter that keeps the part rasters and box shadows composited, and
// draws nothing.
class ShadowWatch final : public Painter {
public:
    void save() override {}
    void restore() override {}
    void translate(Point) override {}
    void clip_rect(Rect) override {}
    void fill_rect(Rect, Color) override {}
    void stroke_rect(Rect, Color, double) override {}
    void draw_line(Point, Point, Color, double) override {}
    void draw_text_utf8(Point, std::string_view, FontSpec, Color) override {}
    void draw_image(ImageId, Rect, double) override {}
    void draw_box_shadow(Rect, double, Point, double, double, Color) override { box_shadows = box_shadows + 1U; }
    void draw_part_raster(const std::shared_ptr<const PartRaster>& raster, Point origin, double) override {
        rasters.push_back(raster);
        origins.push_back(origin);
    }
    std::vector<std::shared_ptr<const PartRaster>> rasters;
    std::vector<Point> origins;
    std::size_t box_shadows{};
};

// A control casting a shadow from a 120 by 40 surface of radius 4.
class Caster final : public Control {
public:
    explicit Caster(StableId stable_id) : Control(std::move(stable_id)) {}
    void on_paint(Painter& painter, Rect) override {
        PartPaint part{};
        part.role = PartRole::shadow;
        part.family = ControlVisualRole::tool_tip;
        part.bounds = surface;
        part.corner_radius = 4.0;
        part.key = PartKey{"shadow", {}, {}};
        paint_part(painter, *this, part);
    }
    Rect surface{20.0, 20.0, 120.0, 40.0};
};

struct Fixture final {
    std::shared_ptr<Panel> root;
    std::shared_ptr<Caster> caster;
    std::unique_ptr<Window> window;
    ShadowWatch painter{};
};

std::unique_ptr<Fixture> make_fixture(const ThemeDefinition& definition, double scale) {
    std::unique_ptr<Fixture> fixture = std::make_unique<Fixture>();
    Fixture& made = *fixture;
    made.root = make_control<Panel>(StableId("root"));
    made.caster = make_control<Caster>(StableId("caster"));
    (*made.caster).set_requested_bounds(Rect{0.0, 0.0, 200.0, 120.0});
    (*made.root).add_child(made.caster);
    made.window = std::make_unique<Window>(made.root, Size{240.0, 160.0});
    Window& window = *made.window;
    window.set_scale(scale);
    window.set_theme(Theme::create(definition));
    window.perform_layout();
    window.paint(made.painter);
    return fixture;
}

// The material the engine lends for `layers`: one transparent fill and the
// shadows, at radius 4.
SurfaceMaterial overlay_material(const std::vector<MaterialShadow>& layers) {
    SurfaceMaterial material{};
    material.fills.assign(1U, MaterialFillLayer::solid(Color::rgba(0, 0, 0, 0)));
    material.shadows = layers;
    material.corner_radius = 4.0;
    return material;
}

// The whole bake of the overlay shadow for the caster's surface at `scale`.
std::vector<std::uint8_t> whole_bake(const std::vector<MaterialShadow>& layers, double scale) {
    const SurfaceMaterial material = overlay_material(layers);
    PrimitiveTarget target{};
    target.logical_size = Size{120.0, 40.0};
    target.scale = DeviceScale{scale};
    target.material = &material;
    const double outset = engine_primitives::overlay_shadow_outset;
    const DeviceExtent extent{static_cast<std::uint32_t>(std::ceil((120.0 + outset * 2.0) * scale)),
                              static_cast<std::uint32_t>(std::ceil((40.0 + outset * 2.0) * scale))};
    const std::shared_ptr<PartRaster> raster = PartRaster::create(extent, DeviceScale{scale});
    require(raster != nullptr, "a raster is made");
    engine_primitives::bake_material(material, target, (*raster).view(), outset);
    const std::span<const std::uint8_t> pixels = (*raster).pixels();
    std::vector<std::uint8_t> bytes(pixels.begin(), pixels.end());
    return bytes;
}

// The overlay shadow's bits at scale 1 for the provisional layers, recorded
// on Windows x64 and checked on every CI platform.
constexpr std::uint64_t expected_shadow_hash = 0x12c9cfd27f7ea098ULL;

void overlay_shadow_is_an_engine_primitive(bool printing) {
    require(builtin_primitive_registry().find("overlay_shadow") != nullptr, "the overlay shadow is registered");
    const ThemeDefinition definition = windows_professional_theme_definition();
    const double scales[] = {1.0, 1.5, 2.0};
    for (const double scale : scales) {
        std::unique_ptr<Fixture> fixture = make_fixture(definition, scale);
        Fixture& made = *fixture;
        require(made.painter.box_shadows == 0U, "no host blur draws the shadow");
        require(made.painter.rasters.size() == 1U, "the engine composites one shadow raster");
        const PartRaster& raster = *made.painter.rasters[0];
        const double outset = engine_primitives::overlay_shadow_outset;
        require(made.painter.origins[0].x == 20.0 - outset && made.painter.origins[0].y == 20.0 - outset,
                "the raster covers the surface grown by the outsets");
        const std::span<const std::uint8_t> pixels = raster.pixels();
        const std::vector<std::uint8_t> drawn(pixels.begin(), pixels.end());
        require(drawn == whole_bake(definition.overlay_shadow, scale),
                "the shadow is the material kernel's outer shadows, byte for byte, sliced or not");
        if (scale != 1.0) continue;
        const std::uint64_t hash = hash_bytes(pixels);
        if (printing) {
            std::cout << "overlay shadow hash 0x" << std::hex << hash << std::dec << '\n';
            continue;
        }
        require(hash == expected_shadow_hash, "the shadow has the same bits on every host");
    }
}

void high_contrast_casts_none() {
    ThemeDefinition definition = windows_professional_theme_definition();
    std::unique_ptr<Fixture> fixture = make_fixture(definition, 1.0);
    Fixture& made = *fixture;
    PresentationSettings settings = (*made.window).presentation_settings();
    settings.high_contrast = true;
    (*made.window).set_presentation_settings(settings);
    made.painter.rasters.clear();
    made.painter.box_shadows = 0U;
    (*made.window).perform_layout();
    (*made.window).paint(made.painter);
    require(made.painter.rasters.empty() && made.painter.box_shadows == 0U, "high contrast casts no shadow");
}

void too_wide_falls_back() {
    ThemeDefinition definition = windows_professional_theme_definition();
    definition.overlay_shadow = {MaterialShadow{.offset = {2.0, 4.0}, .blur_radius = 24.0, .spread = 0.0,
                                                .color = Color::rgba(20, 30, 42, 64), .inset = false}};
    std::unique_ptr<Fixture> fixture = make_fixture(definition, 1.0);
    Fixture& made = *fixture;
    require(made.painter.rasters.empty() && made.painter.box_shadows == 1U,
            "a shadow reaching past the outsets is drawn by the host's blur");
    require((*made.window).metrics_snapshot().engine_primitive_refusals == 1U, "and the refusal is counted");
}

} // namespace

int main(int argc, char** argv) {
    const bool printing = print_mode(argc, argv);
    overlay_shadow_is_an_engine_primitive(printing);
    high_contrast_casts_none();
    too_wide_falls_back();
    std::cout << "engine light tests passed\n";
    return 0;
}
