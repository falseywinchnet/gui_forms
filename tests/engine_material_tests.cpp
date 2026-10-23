// The material primitive (T2 plan, phase E8; owner decision E8-1 (a) and the
// owner's direction of 2026-10-22): its `recipe` choices are the theme's
// recipe roles in enum order; it refuses a target without a valid material,
// with an image fill whose image is not available, or whose estimated bake
// exceeds the bound, and the engine then draws the built-in recipe and
// counts the refusal; it draws a material's fills, shadows, border, edges
// and keylines where paint_surface_material puts them, its outer shadows
// within its outsets; image fills are resampled with the selected filter
// (provisionally Mitchell–Netravali's cubic) in stretch, tile and
// nine-patch modes, from BGRA and PNG resources;
// the engine sizes its raster to the outsets and composites it at the
// part's origin less them, bakes it once per state and caches it.
//
// The reference images are FNV-1a hashes of each raster's rows in RGBA
// order, checked by every CI platform. GUI_FORMS_ENGINE_IDENTITY_PRINT=1
// prints the observed table instead of checking it.
//
// `--measure` prints the bake costs of material_bake_policy.hpp instead of
// testing: the median of nine bakes of a 256 × 256 part at scale 1 for each
// layer alone over the base material, in nanoseconds per device pixel.

#include "gui_forms/control.hpp"
#include "gui_forms/controls/panel/panel.hpp"
#include "gui_forms/engine.hpp"
#include "gui_forms/surface_material/presets/material_presets.hpp"
#include "gui_forms/resources/image_registry/image_registry.hpp"
#include "gui_forms/theme.hpp"
#include "gui_forms/window.hpp"

#include "../src/core/engine/primitives/material/image_variant.hpp"
#include "../src/core/engine/primitives/material/material_bake_policy.hpp"
#include "../src/core/engine/primitives/material/material_primitive.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <span>
#include <stdexcept>
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
    std::cerr << "gui_forms_engine_material_tests: " << message << '\n';
    std::exit(1);
}

bool print_mode() {
    const char* const value = std::getenv("GUI_FORMS_ENGINE_IDENTITY_PRINT");
    const bool printing = value != nullptr && std::strcmp(value, "1") == 0;
    return printing;
}

const EnginePrimitive& material_primitive() {
    const EnginePrimitive* const primitive = builtin_primitive_registry().find("material");
    require(primitive != nullptr, "the built-in registry has the material primitive");
    return *primitive;
}

PrimitiveParameterBlock recipe(std::string_view role) {
    const PrimitiveDescriptor& descriptor = material_primitive().descriptor();
    PrimitiveParameterBlock parameters = default_parameters(descriptor);
    require(set_choice_parameter(parameters, descriptor, "recipe", role) == ParameterAssignment::assigned,
            "a recipe role is a choice");
    return parameters;
}

// A raster the tests own, its rows padded so that padding is exercised.
struct TestRaster final {
    std::vector<std::uint8_t> bytes;
    DeviceExtent extent{};
    std::size_t row_bytes{};

    [[nodiscard]] PartRasterView view() {
        const PartRasterView borrowed{std::span<std::uint8_t>(bytes.data(), bytes.size()), extent, row_bytes};
        return borrowed;
    }
    [[nodiscard]] std::array<std::uint8_t, 4> pixel(std::uint32_t column, std::uint32_t row) const {
        const std::size_t offset = static_cast<std::size_t>(row) * row_bytes + static_cast<std::size_t>(column) * 4U;
        const std::array<std::uint8_t, 4> value{bytes[offset], bytes[offset + 1U], bytes[offset + 2U],
                                                bytes[offset + 3U]};
        return value;
    }
};

// The destination a material part of `logical` size needs: grown by the
// outsets of 8 on every side.
TestRaster make_raster(Size logical, double scale) {
    const std::uint32_t width = static_cast<std::uint32_t>(std::ceil((logical.width + 16.0) * scale));
    const std::uint32_t height = static_cast<std::uint32_t>(std::ceil((logical.height + 16.0) * scale));
    TestRaster raster{};
    raster.extent = DeviceExtent{width, height};
    raster.row_bytes = static_cast<std::size_t>(width) * 4U + 8U;
    raster.bytes.assign(raster.row_bytes * height, 0xA5U);
    return raster;
}

std::uint64_t raster_hash(const TestRaster& raster) {
    std::uint64_t hash = 14695981039346656037ULL;
    for (std::uint32_t row = 0U; row < raster.extent.height; ++row) {
        const std::size_t start = static_cast<std::size_t>(row) * raster.row_bytes;
        const std::size_t end = start + static_cast<std::size_t>(raster.extent.width) * 4U;
        for (std::size_t index = start; index < end; ++index) {
            hash = hash ^ raster.bytes[index];
            hash = hash * 1099511628211ULL;
        }
    }
    return hash;
}

PrimitiveTarget target_for(Size logical, double scale, const SurfaceMaterial* material,
                           const ImageRegistry* images = nullptr) {
    PrimitiveTarget target{};
    target.logical_size = logical;
    target.scale = DeviceScale{scale};
    target.material = material;
    target.images = images;
    return target;
}

// Makes the image variants `target`'s fills need into `lent` and lends
// them, as the engine does before a bake.
void lend_variants(PrimitiveTarget& target, LentImageVariants& lent) {
    if (target.images == nullptr || target.material == nullptr) return;
    const EnginePrimitive& primitive = material_primitive();
    const PrimitiveParameterBlock parameters = recipe("button");
    std::array<ImageVariantRequest, maximum_image_variants> requests{};
    const std::size_t count = primitive.image_variants(parameters, target, requests);
    for (std::size_t index = 0U; index < count; ++index) {
        const ImageVariantRequest& request = requests[index];
        lent.layers[request.layer] = engine_primitives::make_image_variant(*target.images, request);
        require(lent.layers[request.layer] != nullptr, "the image variant is made");
    }
    target.variants = &lent;
}

RasterOutcome try_draw(const PrimitiveTarget& target, TestRaster& raster) {
    const EnginePrimitive& primitive = material_primitive();
    const PrimitiveParameterBlock parameters = recipe("button");
    PrimitivePreparation preparation{};
    require(primitive.prepare(parameters, target, preparation) == PreparationOutcome::prepared, "the part prepares");
    LentImageVariants lent{};
    PrimitiveTarget lending = target;
    lend_variants(lending, lent);
    const RasterOutcome outcome = primitive.rasterize(parameters, lending, preparation, raster.view());
    return outcome;
}

TestRaster draw(const SurfaceMaterial& material, Size logical, double scale, const ImageRegistry* images = nullptr) {
    TestRaster raster = make_raster(logical, scale);
    const PrimitiveTarget target = target_for(logical, scale, &material, images);
    require(try_draw(target, raster) == RasterOutcome::painted, "the material paints");
    return raster;
}

SurfaceMaterial solid_material(Color colour) {
    SurfaceMaterial material{};
    material.fills.clear();
    material.fills.push_back(MaterialFillLayer::solid(colour));
    return material;
}

constexpr Color red{255, 0, 0, 255};
constexpr Color blue{0, 0, 255, 255};
constexpr Color black{0, 0, 0, 255};
constexpr Color clear{0, 0, 0, 0};

int channel(const TestRaster& raster, std::uint32_t column, std::uint32_t row, std::size_t index) {
    const int value = static_cast<int>(raster.pixel(column, row)[index]);
    return value;
}

// -- the descriptor and the choices -------------------------------------------------

void choices_are_the_recipe_roles() {
    const PrimitiveDescriptor& descriptor = material_primitive().descriptor();
    require(descriptor.parameters.size() == 2U && descriptor.parameters[0].name == "recipe",
            "the material's first parameter is its recipe (its shape is E9.6's)");
    const std::span<const std::string_view> choices = descriptor.parameters[0].choices;
    require(choices.size() == static_cast<std::size_t>(ControlVisualRole::count), "one choice per recipe role");
    for (std::size_t index = 0U; index < choices.size(); ++index) {
        const ControlVisualRole role = static_cast<ControlVisualRole>(index);
        require(choices[index] == control_visual_role_name(role), "the choices are the roles' names in enum order");
        const PrimitiveParameterBlock parameters = recipe(choices[index]);
        const std::optional<ControlVisualRole> chosen = material_primitive().theme_material(parameters);
        require(chosen && *chosen == role, "the chosen recipe is the theme material's role");
    }
    require(descriptor.outsets == (Insets{8.0, 8.0, 8.0, 8.0}), "the material draws 8 units outside its box");
    require(descriptor.inputs_read.empty(), "a material reads no inputs, so it never bakes per frame");
    for (const PrimitiveParameterDeclaration& parameter : descriptor.parameters) {
        require(!parameter.animated, "a material has no animated parameter");
    }
    const EnginePrimitive* const plate = builtin_primitive_registry().find("pill_plate");
    require(plate != nullptr, "the registry has the pill plate");
    const std::optional<ControlVisualRole> pill =
        (*plate).theme_material(default_parameters((*plate).descriptor()));
    require(!pill, "a relief primitive draws no theme material");
}

// -- refusals ----------------------------------------------------------------------

void refusals() {
    const EnginePrimitive& primitive = material_primitive();
    const PrimitiveParameterBlock parameters = recipe("button");
    const Size logical{20.0, 10.0};
    TestRaster raster = make_raster(logical, 1.0);
    const std::uint64_t untouched = raster_hash(raster);

    const PrimitiveTarget none = target_for(logical, 1.0, nullptr);
    require(!primitive.accepts(parameters, none), "no material is refused");
    require(try_draw(none, raster) == RasterOutcome::invalid_target && raster_hash(raster) == untouched,
            "a refused target writes nothing");

    SurfaceMaterial empty{};
    empty.fills.clear();
    require(!primitive.accepts(parameters, target_for(logical, 1.0, &empty)), "a material without fills is refused");

    SurfaceMaterial imaged = solid_material(red);
    imaged.fills.push_back(MaterialFillLayer::stretched_image(ImageId{77U}, Size{2.0, 2.0}));
    ImageRegistry registry{};
    require(!primitive.accepts(parameters, target_for(logical, 1.0, &imaged, nullptr)),
            "an image fill without the window's images is refused");
    require(!primitive.accepts(parameters, target_for(logical, 1.0, &imaged, &registry)),
            "an image fill whose image is missing is refused");

    // A plain material is accepted at a modest size. A huge one whose
    // whole bake passes the bound is still accepted when it slices (E7r),
    // since only its reference is baked; one that cannot slice, such as a
    // radial gradient, is refused.
    const SurfaceMaterial plain = solid_material(red);
    require(primitive.accepts(parameters, target_for(logical, 1.0, &plain)), "a solid material is accepted");
    const Size huge{4000.0, 4000.0};
    require(engine_primitives::material_bake_estimate(plain, target_for(huge, 1.0, &plain)) >
                engine_primitives::maximum_material_bake_nanoseconds,
            "a 4000 by 4000 part is over the bake bound");
    require(primitive.accepts(parameters, target_for(huge, 1.0, &plain)), "and is accepted, since it slices");
    SurfaceMaterial radial = plain;
    const std::vector<GradientStop> radial_stops{GradientStop{0.0, red}, GradientStop{1.0, blue}};
    radial.fills.clear();
    radial.fills.push_back(MaterialFillLayer::radial(Point{0.5, 0.5}, Size{0.5, 0.5}, radial_stops));
    require(!primitive.accepts(parameters, target_for(huge, 1.0, &radial)), "a radial one is refused");
}

// -- layers ------------------------------------------------------------------------

void solid_and_border() {
    SurfaceMaterial material = solid_material(red);
    material.border = MaterialBorder{blue, 1.0};
    const TestRaster raster = draw(material, Size{20.0, 10.0}, 1.0);
    require(raster.extent.width == 36U && raster.extent.height == 26U, "the destination covers the outsets");
    require(raster.pixel(2U, 2U) == (std::array<std::uint8_t, 4>{0U, 0U, 0U, 0U}),
            "nothing is drawn outside the box without a shadow");
    require(raster.pixel(8U, 13U) == (std::array<std::uint8_t, 4>{0U, 0U, 255U, 255U}),
            "the border is the box's first device pixel");
    require(raster.pixel(18U, 13U) == (std::array<std::uint8_t, 4>{255U, 0U, 0U, 255U}), "the fill is inside it");
    require(raster.pixel(27U, 17U) == (std::array<std::uint8_t, 4>{0U, 0U, 255U, 255U}),
            "and its last device pixel");
    require(raster.pixel(28U, 13U)[3] == 0U, "nothing past the box");
}

void shadow_offset_and_clip() {
    SurfaceMaterial material = solid_material(red);
    material.shadows.push_back(MaterialShadow{Point{0.0, 4.0}, 0.0, 0.0, black, false});
    const TestRaster raster = draw(material, Size{20.0, 10.0}, 1.0);
    // Below the box, within the offset: the shadow, its edge anti-aliased
    // by the pixel's own filter only.
    require(raster.pixel(18U, 20U) == (std::array<std::uint8_t, 4>{0U, 0U, 0U, 255U}), "the shadow lies below");
    require(raster.pixel(18U, 6U)[3] == 0U, "and not above");
    // An offset past the outsets is clipped at the destination's edge.
    SurfaceMaterial far = solid_material(red);
    far.shadows.push_back(MaterialShadow{Point{0.0, 30.0}, 0.0, 0.0, black, false});
    const TestRaster clipped = draw(far, Size{20.0, 10.0}, 1.0);
    require(clipped.extent.height == 26U, "the destination does not grow with the shadow");
    require(clipped.pixel(18U, 25U)[3] == 0U, "the far shadow is clipped beyond the outsets");
}

void straight_edge_blur_is_half() {
    // A clear box whose shadow is moved half a unit right, so that its right
    // edge falls on a pixel centre: the blurred coverage there is one half.
    SurfaceMaterial material = solid_material(clear);
    material.shadows.push_back(MaterialShadow{Point{0.5, 0.0}, 4.0, 0.0, black, false});
    const TestRaster raster = draw(material, Size{20.0, 40.0}, 1.0);
    const int alpha = channel(raster, 28U, 28U, 3U);
    require(alpha >= 127 && alpha <= 128, "a straight blurred edge is half covered");
    require(channel(raster, 18U, 28U, 3U) == 255, "and the interior fully");
    require(std::abs(engine_primitives::gaussian_integral(0.0) - 0.5) < 1e-12, "Φ(0) is one half");
    require(engine_primitives::gaussian_integral(4.0) == 1.0 && engine_primitives::gaussian_integral(-4.0) == 0.0,
            "Φ saturates beyond 3.5");
    double worst = 0.0;
    for (int step = -400; step <= 400; ++step) {
        const double u = static_cast<double>(step) / 100.0;
        const double exact = 0.5 * std::erfc(-u / std::sqrt(2.0));
        worst = std::max(worst, std::abs(engine_primitives::gaussian_integral(u) - exact));
    }
    require(worst < 4e-4, "Φ is within 4e-4 of the normal integral");
}

void gradients() {
    SurfaceMaterial material{};
    material.fills.clear();
    const GradientStop stops[] = {GradientStop{0.0, red}, GradientStop{1.0, blue}};
    material.fills.push_back(MaterialFillLayer::linear(Point{0.0, 0.5}, Point{1.0, 0.5}, stops, 2U));
    const TestRaster linear = draw(material, Size{20.0, 10.0}, 1.0);
    require(channel(linear, 8U, 13U, 0U) > 240 && channel(linear, 8U, 13U, 2U) < 15, "the start is the first stop");
    require(channel(linear, 27U, 13U, 2U) > 240 && channel(linear, 27U, 13U, 0U) < 15, "the end is the last");

    SurfaceMaterial angled{};
    angled.fills.clear();
    angled.fills.push_back(MaterialFillLayer::linear_css_angle(180.0, stops, 2U));
    const TestRaster down = draw(angled, Size{20.0, 10.0}, 1.0);
    require(channel(down, 18U, 8U, 0U) > 220 && channel(down, 18U, 17U, 2U) > 220, "180 degrees runs downward");

    SurfaceMaterial radial{};
    radial.fills.clear();
    radial.fills.push_back(MaterialFillLayer::radial(Point{0.5, 0.5}, Size{0.5, 0.5}, stops, 2U));
    const TestRaster round = draw(radial, Size{20.0, 20.0}, 1.0);
    require(channel(round, 18U, 18U, 0U) > 230, "the radial centre is the first stop");
    require(channel(round, 8U, 8U, 2U) == 255 && channel(round, 8U, 8U, 0U) == 0, "its corners pad the last");

    SurfaceMaterial repeated{};
    repeated.fills.clear();
    repeated.fills.push_back(MaterialFillLayer::repeating_linear(Point{0.0, 0.0}, Point{10.0, 0.0}, stops, 2U));
    const TestRaster stripes = draw(repeated, Size{20.0, 10.0}, 1.0);
    require(channel(stripes, 8U, 13U, 0U) == channel(stripes, 18U, 13U, 0U), "a repeat starts again each period");
}

void keylines_and_inset_shadow() {
    SurfaceMaterial material = solid_material(red);
    material.corner_radius = 6.0;
    material.keylines.push_back(MaterialKeyline{MaterialEdge::top, blue, 1.0, 0.0});
    const TestRaster raster = draw(material, Size{20.0, 12.0}, 1.0);
    require(raster.pixel(8U, 8U)[3] == 0U, "a rounded corner clips the keyline");
    require(raster.pixel(18U, 8U) == (std::array<std::uint8_t, 4>{0U, 0U, 255U, 255U}), "its middle is drawn");

    SurfaceMaterial inset = solid_material(red);
    inset.shadows.push_back(MaterialShadow{Point{0.0, 0.0}, 0.0, 2.0, black, true});
    const TestRaster shaded = draw(inset, Size{20.0, 12.0}, 1.0);
    require(shaded.pixel(8U, 14U) == (std::array<std::uint8_t, 4>{0U, 0U, 0U, 255U}), "an inset shadow lines the edge");
    require(shaded.pixel(18U, 14U) == (std::array<std::uint8_t, 4>{255U, 0U, 0U, 255U}), "not the middle");
    require(shaded.pixel(4U, 14U)[3] == 0U, "and stays inside the box");

    SurfaceMaterial edged = solid_material(red);
    edged.border_edges.bottom = MaterialBorder{blue, 2.0};
    const TestRaster edge = draw(edged, Size{20.0, 12.0}, 1.0);
    require(edge.pixel(18U, 19U) == (std::array<std::uint8_t, 4>{0U, 0U, 255U, 255U}) &&
                edge.pixel(18U, 18U) == (std::array<std::uint8_t, 4>{0U, 0U, 255U, 255U}) &&
                edge.pixel(18U, 17U) == (std::array<std::uint8_t, 4>{255U, 0U, 0U, 255U}),
            "a bottom edge is its width of rows");
}

// -- glow and gloss (E9-C) -----------------------------------------------------------

// The reference hashes of the glow and gloss bakes, at scale 1 and 2.
constexpr std::uint64_t expected_glow_hashes[] = {0x047bc7d0bde0012dULL, 0xda184a1654d76385ULL};
constexpr std::uint64_t expected_gloss_hashes[] = {0x4507eda3b5101fcbULL, 0xce445232e6045c6dULL};

// Checks `raster`'s hash against `expected`, or prints it in print mode.
bool reference_matches(const TestRaster& raster, const char* name, std::uint64_t expected) {
    const std::uint64_t hash = raster_hash(raster);
    if (print_mode()) {
        std::cout << "    " << name << " 0x" << std::hex << hash << std::dec << "ULL\n";
        return true;
    }
    if (hash == expected) return true;
    std::cerr << name << " hash 0x" << std::hex << hash << std::dec << '\n';
    return false;
}

MaterialGlow glow_of(Color colour, double blur, double spread) {
    MaterialGlow glow{};
    glow.color = colour;
    glow.blur_radius = blur;
    glow.spread = spread;
    return glow;
}

constexpr Color white_half{255, 255, 255, 128};
constexpr Color black_half{0, 0, 0, 128};

void glows() {
    // A glow lies around the box, under the face's fills.
    SurfaceMaterial lit = solid_material(red);
    lit.glows.push_back(glow_of(white_half, 2.0, 0.0));
    const TestRaster raster = draw(lit, Size{20.0, 10.0}, 1.0);
    require(raster.pixel(18U, 13U) == (std::array<std::uint8_t, 4>{255U, 0U, 0U, 255U}), "the face covers its glow");
    const std::array<std::uint8_t, 4> halo = raster.pixel(18U, 7U);
    require(halo[3] > 0U && halo[0] == halo[1] && halo[1] == halo[2], "the glow lies just outside the box");
    require(raster.pixel(18U, 0U)[3] == 0U, "and fades within its reach");

    // It adds light to the shadow beneath it instead of covering it.
    SurfaceMaterial shaded = solid_material(red);
    shaded.shadows.push_back(MaterialShadow{Point{0.0, 0.0}, 2.0, 0.0, black_half, false});
    SurfaceMaterial glowing = solid_material(red);
    glowing.glows.push_back(glow_of(white_half, 2.0, 0.0));
    SurfaceMaterial both = shaded;
    both.glows.push_back(glow_of(white_half, 2.0, 0.0));
    const TestRaster shadow_only = draw(shaded, Size{20.0, 10.0}, 1.0);
    const TestRaster glow_only = draw(glowing, Size{20.0, 10.0}, 1.0);
    const TestRaster added = draw(both, Size{20.0, 10.0}, 1.0);
    const int sum = std::min(255, channel(shadow_only, 18U, 7U, 3U) + channel(glow_only, 18U, 7U, 3U));
    require(std::abs(channel(added, 18U, 7U, 3U) - sum) <= 1, "a glow's alpha adds to the shadow's");
    require(channel(added, 18U, 7U, 0U) >= channel(glow_only, 18U, 7U, 0U) - 1,
            "and the shadow does not darken the glow's light");

    // Its reach, spread + 2 × blur, is within the granted overflow.
    SurfaceMaterial widest = solid_material(red);
    widest.glows.push_back(glow_of(white_half, 2.0, 4.0));
    require(valid_surface_material(widest), "a glow reaching 8 units is valid");
    const Insets outsets = surface_material_visual_outsets(widest);
    require(outsets.left == 8.0 && outsets.top == 8.0 && outsets.right == 8.0 && outsets.bottom == 8.0,
            "its visual outsets are its reach");
    const TestRaster edge = draw(widest, Size{20.0, 10.0}, 2.0);
    require(edge.pixel(36U, 0U)[3] <= 1U, "it fades by the destination's edge");
    const EnginePrimitive& primitive = material_primitive();
    const PrimitiveParameterBlock parameters = recipe("button");
    SurfaceMaterial beyond = solid_material(red);
    beyond.glows.push_back(glow_of(white_half, 2.0, 4.5));
    require(!valid_surface_material(beyond), "a glow reaching past 8 units is invalid");
    require(!primitive.accepts(parameters, target_for(Size{20.0, 10.0}, 1.0, &beyond)), "and refused");
    SurfaceMaterial negative = solid_material(red);
    negative.glows.push_back(glow_of(white_half, -1.0, 0.0));
    require(!valid_surface_material(negative), "a negative blur is invalid");
    SurfaceMaterial crowded = solid_material(red);
    for (std::size_t index = 0U; index <= SurfaceMaterial::maximum_glows; ++index) {
        crowded.glows.push_back(glow_of(white_half, 1.0, 0.0));
    }
    require(!valid_surface_material(crowded), "a material holds at most maximum_glows glows");

    // Its bytes, over a rounded face.
    SurfaceMaterial reference = solid_material(Color{90, 120, 200, 255});
    reference.corner_radius = 4.0;
    reference.glows.push_back(glow_of(Color{255, 200, 80, 140}, 3.0, 1.0));
    const bool first = reference_matches(draw(reference, Size{40.0, 20.0}, 1.0), "glow_1", expected_glow_hashes[0]);
    const bool second = reference_matches(draw(reference, Size{40.0, 20.0}, 2.0), "glow_2", expected_glow_hashes[1]);
    require(first && second, "the glow matches its reference images");
}

// A painter that logs the order of what paint_surface_material draws.
class LayerLog final : public Painter {
public:
    void save() override {}
    void restore() override {}
    void translate(Point) override {}
    void clip_rect(Rect) override {}
    void fill_rect(Rect, Color) override { events.push_back('f'); }
    void stroke_rect(Rect, Color, double) override {}
    void draw_line(Point, Point, Color, double) override {}
    void draw_text_utf8(Point, std::string_view, FontSpec, Color) override {}
    void draw_image(ImageId, Rect, double) override {}
    void draw_box_shadow(Rect, double, Point offset, double blur_radius, double spread, Color color) override {
        events.push_back('s');
        last_offset = offset;
        last_blur = blur_radius;
        last_spread = spread;
        last_colour = color;
    }
    std::string events{};
    Point last_offset{};
    double last_blur{};
    double last_spread{};
    Color last_colour{};
};

void glow_painter_path() {
    SurfaceMaterial material = solid_material(red);
    material.shadows.push_back(MaterialShadow{Point{0.0, 2.0}, 2.0, 0.0, black_half, false});
    material.glows.push_back(glow_of(white_half, 1.5, 1.0));
    LayerLog log{};
    paint_surface_material(log, Rect{0.0, 0.0, 20.0, 10.0}, material);
    require(log.events == "ssf", "the built-in path draws the shadow, then the glow, then the fill");
    require(log.last_offset == Point{} && log.last_blur == 1.5 && log.last_spread == 1.0 &&
                log.last_colour == white_half,
            "the glow is a shadow at no offset");
}

void gloss_preset() {
    const GlossColours colours{};
    const SurfaceMaterial gloss = gloss_material(colours, 3.0);
    require(valid_surface_material(gloss), "the gloss is a valid material");
    require(gloss.fills.size() == 1U && gloss.fills[0].kind == MaterialFillKind::linear_gradient,
            "one gradient fill");
    const std::vector<GradientStop>& stops = gloss.fills[0].stops;
    require(stops.size() == 4U && stops[0] == GradientStop{0.0, colours.top} &&
                stops[1] == GradientStop{0.48, colours.above_break} &&
                stops[2] == GradientStop{0.52, colours.below_break} && stops[3] == GradientStop{1.0, colours.bottom},
            "four stops with a hard break at mid-face");
    require(gloss.keylines.size() == 1U && gloss.keylines[0].edge == MaterialEdge::top &&
                gloss.keylines[0].width == 1.0 && gloss.keylines[0].inset == 1.0 &&
                gloss.keylines[0].color == colours.shine,
            "a 1-unit shine under the top, inside the border");
    require(gloss.border && (*gloss.border).width == 1.0 && gloss.corner_radius == 3.0, "a 1-unit border");
    bool refused = false;
    try {
        static_cast<void>(gloss_material(colours, -1.0));
    } catch (const std::invalid_argument&) {
        refused = true;
    }
    require(refused, "a negative corner radius is refused");

    // The face of a 75 × 23 part at scale 1 spans rows 8 to 30.
    const TestRaster raster = draw(gloss, Size{75.0, 23.0}, 1.0);
    const int above = channel(raster, 45U, 18U, 0U);
    const int below = channel(raster, 45U, 20U, 0U);
    require(above - below > 30, "the colour steps across the break");
    require(std::abs(channel(raster, 45U, 12U, 0U) - channel(raster, 45U, 14U, 0U)) < 10,
            "and changes gently above it");
    require(channel(raster, 45U, 9U, 0U) > channel(raster, 45U, 10U, 0U), "the shine lies under the border");
    const std::array<std::uint8_t, 4> top_edge{colours.border.red, colours.border.green, colours.border.blue, 255U};
    require(raster.pixel(45U, 8U) == top_edge, "under the top border");
    const bool first = reference_matches(raster, "gloss_1", expected_gloss_hashes[0]);
    const bool second = reference_matches(draw(gloss, Size{75.0, 23.0}, 2.0), "gloss_2", expected_gloss_hashes[1]);
    require(first && second, "the gloss matches its reference images");
}

// -- image fills -------------------------------------------------------------------

// A 4 × 4 premultiplied BGRA image: red, green, blue and white quadrants.
std::vector<std::byte> quadrant_pixels() {
    std::vector<std::byte> pixels(4U * 4U * 4U);
    for (std::size_t row = 0U; row < 4U; ++row) {
        for (std::size_t column = 0U; column < 4U; ++column) {
            const std::size_t quadrant = (row / 2U) * 2U + column / 2U;
            const std::array<std::uint8_t, 4> bgra[] = {
                {0U, 0U, 255U, 255U}, {0U, 255U, 0U, 255U}, {255U, 0U, 0U, 255U}, {255U, 255U, 255U, 255U}};
            const std::size_t offset = (row * 4U + column) * 4U;
            for (std::size_t index = 0U; index < 4U; ++index) {
                pixels[offset + index] = static_cast<std::byte>(bgra[quadrant][index]);
            }
        }
    }
    return pixels;
}

std::uint32_t crc32(std::span<const std::uint8_t> bytes) {
    std::uint32_t crc = 0xFFFFFFFFU;
    for (const std::uint8_t byte : bytes) {
        crc = crc ^ byte;
        for (int bit = 0; bit < 8; ++bit) {
            const std::uint32_t mask = (crc & 1U) != 0U ? 0xEDB88320U : 0U;
            crc = (crc >> 1U) ^ mask;
        }
    }
    return crc ^ 0xFFFFFFFFU;
}

void append_big_endian(std::vector<std::uint8_t>& out, std::uint32_t value) {
    out.push_back(static_cast<std::uint8_t>(value >> 24U));
    out.push_back(static_cast<std::uint8_t>(value >> 16U));
    out.push_back(static_cast<std::uint8_t>(value >> 8U));
    out.push_back(static_cast<std::uint8_t>(value));
}

void append_chunk(std::vector<std::uint8_t>& out, const char* type, const std::vector<std::uint8_t>& data) {
    append_big_endian(out, static_cast<std::uint32_t>(data.size()));
    std::vector<std::uint8_t> typed(type, type + 4);
    typed.insert(typed.end(), data.begin(), data.end());
    out.insert(out.end(), typed.begin(), typed.end());
    append_big_endian(out, crc32(typed));
}

// An RGBA8 PNG of `width` × `height` pixels from its filtered rows (a
// filter byte, then the row's pixels), its zlib stream stored, not
// compressed, in blocks of at most 65535 bytes.
std::vector<std::byte> encode_png(std::uint32_t width, std::uint32_t height, const std::vector<std::uint8_t>& raw) {
    std::vector<std::uint8_t> zlib{0x78U, 0x01U};
    std::size_t offset = 0U;
    while (offset < raw.size()) {
        const std::size_t remaining = raw.size() - offset;
        const std::uint16_t length = static_cast<std::uint16_t>(std::min<std::size_t>(remaining, 65535U));
        const bool final_block = offset + length == raw.size();
        zlib.push_back(final_block ? 0x01U : 0x00U);
        const std::uint16_t complement = static_cast<std::uint16_t>(~length);
        zlib.push_back(static_cast<std::uint8_t>(length & 0xFFU));
        zlib.push_back(static_cast<std::uint8_t>(length >> 8U));
        zlib.push_back(static_cast<std::uint8_t>(complement & 0xFFU));
        zlib.push_back(static_cast<std::uint8_t>(complement >> 8U));
        zlib.insert(zlib.end(), raw.begin() + static_cast<std::ptrdiff_t>(offset),
                    raw.begin() + static_cast<std::ptrdiff_t>(offset + length));
        offset = offset + length;
    }
    std::uint32_t a = 1U;
    std::uint32_t b = 0U;
    for (const std::uint8_t byte : raw) {
        a = (a + byte) % 65521U;
        b = (b + a) % 65521U;
    }
    append_big_endian(zlib, (b << 16U) | a);
    std::vector<std::uint8_t> png{0x89U, 'P', 'N', 'G', 0x0DU, 0x0AU, 0x1AU, 0x0AU};
    std::vector<std::uint8_t> header{};
    append_big_endian(header, width);
    append_big_endian(header, height);
    header.push_back(8U);
    header.push_back(6U);
    header.push_back(0U);
    header.push_back(0U);
    header.push_back(0U);
    append_chunk(png, "IHDR", header);
    append_chunk(png, "IDAT", zlib);
    append_chunk(png, "IEND", {});
    std::vector<std::byte> bytes(png.size());
    for (std::size_t index = 0U; index < png.size(); ++index) bytes[index] = static_cast<std::byte>(png[index]);
    return bytes;
}

// The quadrant image as an RGBA8 PNG.
std::vector<std::byte> quadrant_png() {
    std::vector<std::uint8_t> raw{};
    const std::array<std::uint8_t, 4> rgba[] = {
        {255U, 0U, 0U, 255U}, {0U, 255U, 0U, 255U}, {0U, 0U, 255U, 255U}, {255U, 255U, 255U, 255U}};
    for (std::size_t row = 0U; row < 4U; ++row) {
        raw.push_back(0U);
        for (std::size_t column = 0U; column < 4U; ++column) {
            const std::size_t quadrant = (row / 2U) * 2U + column / 2U;
            raw.insert(raw.end(), rgba[quadrant].begin(), rgba[quadrant].end());
        }
    }
    const std::vector<std::byte> png = encode_png(4U, 4U, raw);
    return png;
}

// With GUI_FORMS_ENGINE_MATERIAL_DUMP set to a directory, writes `raster`
// there as <name>.png, in straight alpha, for inspection.
void dump_raster(const TestRaster& raster, const char* name) {
    const char* const directory = std::getenv("GUI_FORMS_ENGINE_MATERIAL_DUMP");
    if (directory == nullptr) return;
    std::vector<std::uint8_t> raw{};
    for (std::uint32_t row = 0U; row < raster.extent.height; ++row) {
        raw.push_back(0U);
        for (std::uint32_t column = 0U; column < raster.extent.width; ++column) {
            const std::array<std::uint8_t, 4> pixel = raster.pixel(column, row);
            const unsigned alpha = pixel[3];
            for (std::size_t index = 0U; index < 3U; ++index) {
                const unsigned straight = alpha == 0U ? 0U : (pixel[index] * 255U + alpha / 2U) / alpha;
                raw.push_back(static_cast<std::uint8_t>(std::min(straight, 255U)));
            }
            raw.push_back(pixel[3]);
        }
    }
    const std::vector<std::byte> png = encode_png(raster.extent.width, raster.extent.height, raw);
    const std::string path = std::string(directory) + "/" + name + ".png";
    std::ofstream file(path, std::ios::binary);
    file.write(reinterpret_cast<const char*>(png.data()), static_cast<std::streamsize>(png.size()));
}

struct ImageCase final {
    const char* name;
    std::uint64_t hash;
};

// The image fills' reference hashes, in the order image_fills draws them.
constexpr ImageCase image_cases[] = {
    {"stretch_bgra", 0x62d23c31627fdeb5ULL},
    {"stretch_png", 0x62d23c31627fdeb5ULL},
    {"tile", 0x7f68c692b6b84925ULL},
    {"nine_patch", 0x937e6be15e8d40fdULL},
    {"shrink", 0x55ad2e0e76d50405ULL},
};

void image_fills() {
    ImageRegistry registry{};
    const std::vector<std::byte> bgra = quadrant_pixels();
    const ImageLoadResult loaded = registry.load_bgra32_premultiplied(4U, 4U, 16U, bgra);
    require(static_cast<bool>(loaded), "the BGRA image loads");
    const std::vector<std::byte> png = quadrant_png();
    const ImageLoadResult decoded = registry.load_png(png);
    require(static_cast<bool>(decoded), "the PNG image loads");
    const Size four{4.0, 4.0};

    std::vector<std::uint64_t> hashes{};
    SurfaceMaterial stretch = solid_material(clear);
    stretch.fills.push_back(MaterialFillLayer::stretched_image(loaded.image, four));
    const TestRaster stretched = draw(stretch, Size{20.0, 20.0}, 1.0, &registry);
    require(stretched.pixel(9U, 9U) == (std::array<std::uint8_t, 4>{255U, 0U, 0U, 255U}),
            "the top-left quadrant is red");
    require(stretched.pixel(26U, 26U) == (std::array<std::uint8_t, 4>{255U, 255U, 255U, 255U}),
            "the bottom-right white");
    require(stretched.pixel(4U, 4U)[3] == 0U, "nothing outside the box");
    dump_raster(stretched, "image_stretch");
    hashes.push_back(raster_hash(stretched));

    SurfaceMaterial from_png = solid_material(clear);
    from_png.fills.push_back(MaterialFillLayer::stretched_image(decoded.image, four));
    const TestRaster png_raster = draw(from_png, Size{20.0, 20.0}, 1.0, &registry);
    require(raster_hash(png_raster) == raster_hash(stretched), "a PNG and its BGRA pixels draw the same");
    hashes.push_back(raster_hash(png_raster));

    SurfaceMaterial tile = solid_material(clear);
    tile.fills.push_back(MaterialFillLayer::tiled_image(loaded.image, four, 1.0));
    const TestRaster tiled = draw(tile, Size{16.0, 16.0}, 1.0, &registry);
    require(tiled.pixel(8U, 8U) == tiled.pixel(12U, 8U) && tiled.pixel(8U, 8U) == tiled.pixel(8U, 12U),
            "tiles repeat every four units");
    dump_raster(tiled, "image_tile");
    hashes.push_back(raster_hash(tiled));

    SurfaceMaterial nine = solid_material(clear);
    nine.fills.push_back(MaterialFillLayer::nine_patch(loaded.image, four, Insets{1.0, 1.0, 1.0, 1.0}, 1.0));
    const TestRaster patched = draw(nine, Size{20.0, 20.0}, 1.0, &registry);
    require(patched.pixel(8U, 8U) == (std::array<std::uint8_t, 4>{255U, 0U, 0U, 255U}),
            "a nine-patch keeps its corner");
    dump_raster(patched, "image_nine_patch");
    hashes.push_back(raster_hash(patched));

    SurfaceMaterial shrink = solid_material(clear);
    shrink.fills.push_back(MaterialFillLayer::stretched_image(loaded.image, four, 0.5));
    const TestRaster shrunk = draw(shrink, Size{2.0, 2.0}, 1.0, &registry);
    const std::array<std::uint8_t, 4> mean = shrunk.pixel(8U, 8U);
    require(mean[3] >= 126U && mean[3] <= 128U, "a shrunk image averages, at its opacity");
    hashes.push_back(raster_hash(shrunk));

    if (print_mode()) {
        for (std::size_t index = 0U; index < hashes.size(); ++index) {
            std::cout << "    {\"" << image_cases[index].name << "\", 0x" << std::hex << hashes[index] << std::dec
                      << "ULL},\n";
        }
        return;
    }
    for (std::size_t index = 0U; index < hashes.size(); ++index) {
        if (hashes[index] == image_cases[index].hash) continue;
        std::cerr << "image fill " << image_cases[index].name << " hash 0x" << std::hex << hashes[index] << std::dec
                  << '\n';
        require(false, "an image fill matches its reference image");
    }
}

// An image variant is made once, from its image, and lent to every bake
// that needs it (owner, 2026-10-22); a change to the window's images or a
// theme change drops it.
void image_variant_made_once() {
    ImageRegistry registry{};
    const std::vector<std::byte> bgra = quadrant_pixels();
    const ImageLoadResult loaded = registry.load_bgra32_premultiplied(4U, 4U, 16U, bgra);
    require(static_cast<bool>(loaded), "the BGRA image loads");
    SurfaceMaterial stretch = solid_material(clear);
    stretch.fills.push_back(MaterialFillLayer::stretched_image(loaded.image, Size{4.0, 4.0}));
    const Size logical{20.0, 20.0};
    const TestRaster expected = draw(stretch, logical, 1.0, &registry);

    PartRasterRequest request{};
    request.primitive = &material_primitive();
    request.parameters = recipe("button");
    request.target = target_for(logical, 1.0, &stretch, &registry);
    request.extent = expected.extent;
    EnginePartTable table{};
    const PartRasterResult first = table.raster_without_record(request, 1U);
    const PartRasterResult second = table.raster_without_record(request, 2U);
    require(first.raster != nullptr && second.raster != nullptr, "the table bakes the image face");
    const RasterCacheStatistics made = table.cache_statistics();
    require(made.bakes == 2U, "the part is baked twice");
    require(made.image_variant_bakes == 1U && made.image_variant_hits == 1U && made.image_variants == 1U,
            "its variant is made once and reused");
    require(made.shared_bytes == 20U * 20U * 4U, "the variant's bytes count with the shared bakes");
    // The table's bake is the variant composited, as the primitive's is.
    const PartRaster& baked = *second.raster;
    for (std::uint32_t row = 0U; row < expected.extent.height; ++row) {
        const std::size_t from = static_cast<std::size_t>(row) * baked.row_bytes();
        const std::size_t to = static_cast<std::size_t>(row) * expected.row_bytes;
        const bool same = std::memcmp(baked.pixels().data() + from, expected.bytes.data() + to,
                                      static_cast<std::size_t>(expected.extent.width) * 4U) == 0;
        require(same, "the table's bake matches the primitive's");
    }

    // Another image loaded: the registry's revision moves on, and the
    // variant is made again.
    const ImageLoadResult other = registry.load_bgra32_premultiplied(4U, 4U, 16U, bgra);
    require(static_cast<bool>(other), "a second image loads");
    static_cast<void>(table.raster_without_record(request, 3U));
    const RasterCacheStatistics remade = table.cache_statistics();
    require(remade.image_variant_bakes == 2U && remade.image_variants == 1U,
            "a change to the images makes the variant again");
    table.invalidate_rasters();
    const RasterCacheStatistics cleared = table.cache_statistics();
    require(cleared.image_variants == 0U && cleared.shared_bytes == 0U, "a theme change drops the variants");
}

// Making a variant is preparation, not bake (owner, 2026-10-22): the bake
// bound counts the composite alone, so a large image, which takes far
// longer to resample than the bound allows, is accepted and baked.
void image_variant_outside_the_bound() {
    ImageRegistry registry{};
    // 1024 × 1024 premultiplied grey at half alpha.
    const std::vector<std::byte> large(1024U * 1024U * 4U, std::byte{0x80});
    const ImageLoadResult loaded = registry.load_bgra32_premultiplied(1024U, 1024U, 4096U, large);
    const std::vector<std::byte> small(4U * 4U * 4U, std::byte{0x80});
    const ImageLoadResult tiny = registry.load_bgra32_premultiplied(4U, 4U, 16U, small);
    require(static_cast<bool>(loaded) && static_cast<bool>(tiny), "the images load");
    SurfaceMaterial shrunk = solid_material(clear);
    shrunk.fills.push_back(MaterialFillLayer::stretched_image(loaded.image, Size{1024.0, 1024.0}));
    SurfaceMaterial magnified = solid_material(clear);
    magnified.fills.push_back(MaterialFillLayer::stretched_image(tiny.image, Size{4.0, 4.0}));
    const Size logical{240.0, 240.0};
    const double shrunk_estimate =
        engine_primitives::material_bake_estimate(shrunk, target_for(logical, 1.0, &shrunk, &registry));
    const double magnified_estimate =
        engine_primitives::material_bake_estimate(magnified, target_for(logical, 1.0, &magnified, &registry));
    require(shrunk_estimate == magnified_estimate, "the image's size does not enter the bake estimate");
    require(shrunk_estimate <= engine_primitives::maximum_material_bake_nanoseconds, "the composite is within the bound");

    PartRasterRequest request{};
    request.primitive = &material_primitive();
    request.parameters = recipe("button");
    request.target = target_for(logical, 1.0, &shrunk, &registry);
    request.extent = make_raster(logical, 1.0).extent;
    require(material_primitive().accepts(request.parameters, request.target), "the large image face is accepted");
    EnginePartTable table{};
    const PartRasterResult baked = table.raster_without_record(request, 1U);
    require(baked.raster != nullptr && baked.status == PartRasterStatus::uncached, "the large image face bakes");
    require(table.cache_statistics().image_variant_bakes == 1U, "its variant is made on first need");
    // The interior shows the image's grey at half alpha.
    const PartRaster& raster = *baked.raster;
    const std::size_t centre = 128U * raster.row_bytes() + 128U * 4U;
    require(raster.pixels()[centre + 3U] == 0x80U && raster.pixels()[centre] == 0x80U,
            "the shrunk image keeps its colour");
}

// -- reference images: the professional theme's button ---------------------------

struct ReferenceCase final {
    const char* name;
    ControlSurfaceState state;
    double scale;
    std::uint64_t hash;
};

constexpr ReferenceCase reference_cases[] = {
    {"button_normal_1", ControlSurfaceState::normal, 1.0, 0xed9a62ab804b8e96ULL},
    {"button_hot_1", ControlSurfaceState::hot, 1.0, 0x660b69d77ff04a61ULL},
    {"button_pressed_1", ControlSurfaceState::pressed, 1.0, 0x5c9d147f035bb561ULL},
    {"button_normal_1.25", ControlSurfaceState::normal, 1.25, 0x2a9264cd54cf3516ULL},
    {"button_hot_1.25", ControlSurfaceState::hot, 1.25, 0xc834436ae1082069ULL},
    {"button_pressed_1.25", ControlSurfaceState::pressed, 1.25, 0x20ffbc54f04c58faULL},
    {"button_normal_1.5", ControlSurfaceState::normal, 1.5, 0xbb080fd1e9248ab4ULL},
    {"button_hot_1.5", ControlSurfaceState::hot, 1.5, 0xfda6b6cef8a8320dULL},
    {"button_pressed_1.5", ControlSurfaceState::pressed, 1.5, 0x3239deab7a5a66fULL},
    {"button_normal_2", ControlSurfaceState::normal, 2.0, 0x54af290b2cb4826dULL},
    {"button_hot_2", ControlSurfaceState::hot, 2.0, 0x1e700223dc0caa85ULL},
    {"button_pressed_2", ControlSurfaceState::pressed, 2.0, 0x488d18b42278dce9ULL},
};

void reference_images() {
    const std::shared_ptr<const Theme> theme = Theme::create(windows_professional_theme_definition());
    const bool printing = print_mode();
    bool all_match = true;
    for (const ReferenceCase& reference : reference_cases) {
        ControlVisualContext context{};
        context.surface = reference.state;
        const SurfaceMaterial& material = (*theme).resolve(ControlVisualRole::button, context).material;
        const TestRaster raster = draw(material, Size{75.0, 23.0}, reference.scale);
        dump_raster(raster, reference.name);
        const std::uint64_t hash = raster_hash(raster);
        if (printing) {
            std::cout << "    {\"" << reference.name << "\", ..., 0x" << std::hex << hash << std::dec << "ULL},\n";
            continue;
        }
        if (hash == reference.hash) continue;
        all_match = false;
        std::cerr << reference.name << " hash 0x" << std::hex << hash << std::dec << '\n';
    }
    require(printing || all_match, "the professional button matches its reference images");
}

// -- the engine --------------------------------------------------------------------

// A painter that keeps the last part raster composited, and draws nothing.
class RasterWatch final : public Painter {
public:
    void save() override {}
    void restore() override {}
    void translate(Point) override {}
    void clip_rect(Rect) override {}
    void fill_rect(Rect, Color) override { fills = fills + 1U; }
    void stroke_rect(Rect, Color, double) override {}
    void draw_line(Point, Point, Color, double) override {}
    void draw_text_utf8(Point, std::string_view, FontSpec, Color) override {}
    void draw_image(ImageId, Rect, double) override {}
    void draw_part_raster(const std::shared_ptr<const PartRaster>& raster, Point origin, double) override {
        last = raster;
        last_origin = origin;
        rasters = rasters + 1U;
    }
    std::shared_ptr<const PartRaster> last{};
    Point last_origin{};
    std::size_t rasters{};
    std::size_t fills{};
};

class Pad final : public Control {
public:
    explicit Pad(StableId stable_id) : Control(std::move(stable_id)) {}
    void on_paint(Painter& painter, Rect) override { paint_part(painter, *this, face()); }
    [[nodiscard]] PartPaint face() const {
        PartPaint part{};
        part.role = PartRole::face;
        part.family = ControlVisualRole::button;
        part.bounds = Rect{20.0, 20.0, size.width, size.height};
        part.key = PartKey{"face", {}, {}};
        if (!enabled()) part.states.insert(AnatomyState::disabled);
        return part;
    }
    Size size{40.0, 24.0};
};

ThemeRule material_rule() {
    ThemeRule rule{};
    rule.label = "pad";
    rule.level = CascadeLevel::theme_sheet;
    rule.selector.stable_id = "pad";
    rule.selector.part = "face";
    rule.declarations.push_back(RuleDeclaration::primitive("material"));
    rule.declarations.push_back(RuleDeclaration::choice_value("recipe", "button"));
    return rule;
}

struct Fixture final {
    std::shared_ptr<Panel> root;
    std::shared_ptr<Pad> pad;
    std::unique_ptr<Window> window;
    RasterWatch painter{};
};

std::unique_ptr<Fixture> make_fixture(Size size, double scale) {
    std::unique_ptr<Fixture> fixture = std::make_unique<Fixture>();
    Fixture& made = *fixture;
    made.root = make_control<Panel>(StableId("root"));
    made.pad = make_control<Pad>(StableId("pad"));
    (*made.pad).size = size;
    (*made.pad).set_requested_bounds(Rect{0.0, 0.0, size.width + 60.0, size.height + 60.0});
    (*made.root).add_child(made.pad);
    made.window = std::make_unique<Window>(made.root, Size{size.width + 100.0, size.height + 100.0});
    Window& window = *made.window;
    window.set_scale(scale);
    ThemeDefinition definition = windows_professional_theme_definition();
    definition.id = "engine-material-test";
    definition.rules.rules.push_back(material_rule());
    window.set_theme(Theme::create(definition));
    require(window.theme().engine().rules().diagnostics().empty(), "the material theme compiles cleanly");
    window.perform_layout();
    window.paint(made.painter);
    static_cast<void>(window.take_damage());
    return fixture;
}

void repaint(Fixture& fixture) {
    Window& window = *fixture.window;
    window.perform_layout();
    const DamageRegion damage = window.take_damage();
    if (!damage.empty()) window.paint(fixture.painter, damage.bounds());
}

std::uint64_t bakes(const Window& window) {
    const RasterCacheStatistics statistics = window.engine_parts().cache_statistics();
    const std::uint64_t count = statistics.storage_reuses + statistics.storage_allocations + statistics.uncached;
    return count;
}

std::uint64_t pixels_hash(const PartRaster& raster) {
    std::uint64_t hash = 14695981039346656037ULL;
    for (const std::uint8_t byte : raster.pixels()) {
        hash = hash ^ byte;
        hash = hash * 1099511628211ULL;
    }
    return hash;
}

void engine_draws_the_material() {
    for (const double scale : {1.0, 1.5}) {
        std::unique_ptr<Fixture> fixture = make_fixture(Size{40.0, 24.0}, scale);
        Fixture& made = *fixture;
        require(made.painter.rasters == 1U && made.painter.last, "the face is drawn by the material");
        const DeviceExtent extent = (*made.painter.last).extent();
        const std::uint32_t width = static_cast<std::uint32_t>(std::ceil(56.0 * scale));
        const std::uint32_t height = static_cast<std::uint32_t>(std::ceil(40.0 * scale));
        require(extent.width == width && extent.height == height, "the raster includes the outsets");
        // The face is at (20, 20) in the pad; the painter keeps local points.
        require(made.painter.last_origin.x == 12.0 && made.painter.last_origin.y == 12.0,
                "it is composited 8 units up and left of the face");
    }

    std::unique_ptr<Fixture> fixture = make_fixture(Size{40.0, 24.0}, 1.0);
    Fixture& made = *fixture;
    Window& window = *made.window;
    const std::uint64_t first_bakes = bakes(window);
    const std::uint64_t normal = pixels_hash(*made.painter.last);
    (*made.pad).invalidate(Dirty::paint);
    repaint(made);
    require(bakes(window) == first_bakes, "an unchanged part is composited from its cached raster");
    (*made.pad).set_enabled(false);
    repaint(made);
    require(bakes(window) == first_bakes + 1U, "a state change bakes once");
    require(pixels_hash(*made.painter.last) != normal, "a disabled control draws its disabled material");
    (*made.pad).invalidate(Dirty::paint);
    repaint(made);
    require(bakes(window) == first_bakes + 1U, "and is cached again");
}

void engine_counts_refusals() {
    std::unique_ptr<Fixture> fixture = make_fixture(Size{3000.0, 3000.0}, 1.0);
    Fixture& made = *fixture;
    Window& window = *made.window;
    require(made.painter.rasters == 0U, "a refused material composites no raster");
    require(window.metrics_snapshot().engine_primitive_refusals == 1U, "the refusal is counted");
    (*made.pad).invalidate(Dirty::paint);
    repaint(made);
    require(window.metrics_snapshot().engine_primitive_refusals == 1U, "once, when it begins");
}

// -- measuring the bake costs ------------------------------------------------------

// The median of nine bakes of `material` on a 240-unit square part at
// scale 1, through the kernel alone, so that materials past the bound can
// be timed too.
double median_bake_nanoseconds(const SurfaceMaterial& material, const ImageRegistry* images) {
    const Size logical{240.0, 240.0};
    TestRaster raster = make_raster(logical, 1.0);
    PrimitiveTarget target = target_for(logical, 1.0, &material, images);
    require(valid_surface_material(material), "the measured material is valid");
    // The variants are prepared once, apart from the bakes measured.
    LentImageVariants lent{};
    lend_variants(target, lent);
    const PartRasterView view = raster.view();
    std::array<double, 9> samples{};
    for (double& sample : samples) {
        const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        engine_primitives::bake_material(material, target, view);
        const std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        sample = std::chrono::duration<double, std::nano>(end - start).count();
    }
    std::sort(samples.begin(), samples.end());
    return samples[4];
}

// A size × size premultiplied BGRA image of opaque noise.
std::vector<std::byte> noise_pixels(std::size_t size) {
    std::vector<std::byte> pixels(size * size * 4U);
    for (std::size_t index = 0U; index < pixels.size(); ++index) {
        const std::size_t value = index % 4U == 3U ? 255U : (index * 37U) % 251U;
        pixels[index] = static_cast<std::byte>(value);
    }
    return pixels;
}

void measure() {
    // 240 × 240 logical plus the outsets: a 256 × 256 destination.
    const double pixels = 256.0 * 256.0;
    const SurfaceMaterial base = solid_material(red);
    const double base_time = median_bake_nanoseconds(base, nullptr);
    const GradientStop two[] = {GradientStop{0.0, red}, GradientStop{1.0, blue}};
    GradientStop thirty_two[32]{};
    for (std::size_t index = 0U; index < 32U; ++index) {
        thirty_two[index] = GradientStop{static_cast<double>(index) / 31.0, index % 2U == 0U ? red : blue};
    }
    SurfaceMaterial solid = base;
    solid.fills.push_back(MaterialFillLayer::solid(blue));
    SurfaceMaterial linear = base;
    linear.fills.push_back(MaterialFillLayer::linear(Point{0.0, 0.0}, Point{1.0, 1.0}, two, 2U));
    SurfaceMaterial linear_many = base;
    linear_many.fills.push_back(MaterialFillLayer::linear(Point{0.0, 0.0}, Point{1.0, 1.0}, thirty_two, 32U));
    SurfaceMaterial radial = base;
    radial.fills.push_back(MaterialFillLayer::radial(Point{0.5, 0.5}, Size{0.5, 0.5}, two, 2U));
    SurfaceMaterial conic = base;
    conic.fills.push_back(MaterialFillLayer::conic(Point{0.5, 0.5}, 0.0, std::vector<GradientStop>(two, two + 2)));
    // Shadows wide enough that most pixels evaluate their integrals.
    SurfaceMaterial outer = base;
    outer.shadows.push_back(MaterialShadow{Point{0.0, 2.0}, 64.0, 0.0, black, false});
    outer.corner_radius = 8.0;
    // A box rounded to a circle: every pixel is within the shadow's corners.
    SurfaceMaterial round_box = base;
    round_box.corner_radius = 120.0;
    SurfaceMaterial outer_round = round_box;
    outer_round.shadows.push_back(MaterialShadow{Point{0.0, 2.0}, 64.0, 0.0, black, false});
    SurfaceMaterial inset = base;
    inset.shadows.push_back(MaterialShadow{Point{0.0, 2.0}, 64.0, 0.0, black, true});
    inset.corner_radius = 8.0;
    SurfaceMaterial rounded_base = base;
    rounded_base.corner_radius = 8.0;
    SurfaceMaterial border = rounded_base;
    border.border = MaterialBorder{blue, 1.0};
    // A dashed border's pattern (E9.6), past the solid ring's cost, per
    // pixel of the ring, at its dearest: a border 64 wide on a box rounded
    // to a circle, so that every ring pixel is on an arc. The ring lies
    // between radii 120 and 56.
    SurfaceMaterial thick = round_box;
    thick.border = MaterialBorder{blue, 64.0};
    SurfaceMaterial dashed = thick;
    (*dashed.border).style = MaterialBorderStyle::dashed;
    const double ring_pixels = 3.14159265358979323846 * (120.0 * 120.0 - 56.0 * 56.0);
    SurfaceMaterial edge = base;
    edge.border_edges.top = MaterialBorder{blue, 1.0};
    SurfaceMaterial keyline = base;
    keyline.keylines.push_back(MaterialKeyline{MaterialEdge::top, blue, 1.0, 0.0});
    // The part filters (E9.6), each over the plain base. A box blur's cost
    // does not depend on its radius; the largest the reach admits is used.
    SurfaceMaterial colour_filtered = base;
    colour_filtered.colour_filter = MaterialColourMatrix::sepia(1.0);
    SurfaceMaterial drop_shadowed = base;
    drop_shadowed.drop_shadow = MaterialDropShadow{Point{0.0, 2.0}, 4.0, black};
    SurfaceMaterial blurred = base;
    blurred.filter_blur = 5.0;
    // A multiply fill (E9.6) against a normal one: the face gathered apart.
    SurfaceMaterial multiplied = solid;
    multiplied.fills[1].blend = MaterialBlendMode::multiply;

    // An image's bake composites its prepared variant: the source's size
    // does not enter it.
    ImageRegistry registry{};
    const std::vector<std::byte> small = noise_pixels(64U);
    const ImageLoadResult small_image = registry.load_bgra32_premultiplied(64U, 64U, 256U, small);
    require(static_cast<bool>(small_image), "the measured image loads");
    SurfaceMaterial imaged = base;
    imaged.fills.push_back(MaterialFillLayer::stretched_image(small_image.image, Size{64.0, 64.0}));

    const double per_pixel_solid = (median_bake_nanoseconds(solid, nullptr) - base_time) / pixels;
    const double per_pixel_linear = (median_bake_nanoseconds(linear, nullptr) - base_time) / pixels;
    const double per_pixel_many = (median_bake_nanoseconds(linear_many, nullptr) - base_time) / pixels;
    const double per_pixel_radial = (median_bake_nanoseconds(radial, nullptr) - base_time) / pixels;
    const double per_pixel_conic = (median_bake_nanoseconds(conic, nullptr) - base_time) / pixels;
    const double rounded_time = median_bake_nanoseconds(rounded_base, nullptr);
    const double per_pixel_outer = (median_bake_nanoseconds(outer, nullptr) - rounded_time) / pixels;
    const double round_time = median_bake_nanoseconds(round_box, nullptr);
    const double per_pixel_corner = (median_bake_nanoseconds(outer_round, nullptr) - round_time) / pixels - per_pixel_outer;
    const double per_pixel_inset = (median_bake_nanoseconds(inset, nullptr) - rounded_time) / pixels;
    const double per_pixel_border = (median_bake_nanoseconds(border, nullptr) - rounded_time) / pixels;
    const double per_pixel_pattern =
        (median_bake_nanoseconds(dashed, nullptr) - median_bake_nanoseconds(thick, nullptr)) / ring_pixels;
    const double per_pixel_edge = (median_bake_nanoseconds(edge, nullptr) - base_time) / pixels;
    const double per_pixel_keyline = (median_bake_nanoseconds(keyline, nullptr) - base_time) / pixels;
    const double per_pixel_image = (median_bake_nanoseconds(imaged, &registry) - base_time) / pixels;
    const double per_pixel_colour_filter = (median_bake_nanoseconds(colour_filtered, nullptr) - base_time) / pixels;
    const double per_pixel_drop_shadow = (median_bake_nanoseconds(drop_shadowed, nullptr) - base_time) / pixels;
    const double per_pixel_filter_blur = (median_bake_nanoseconds(blurred, nullptr) - base_time) / pixels;
    const double solid_time = median_bake_nanoseconds(solid, nullptr);
    const double per_pixel_blend = (median_bake_nanoseconds(multiplied, nullptr) - solid_time) / pixels;

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "base " << base_time / pixels << '\n';
    std::cout << "rounded_base " << rounded_time / pixels << '\n';
    std::cout << "solid " << per_pixel_solid << '\n';
    std::cout << "linear " << per_pixel_linear << '\n';
    std::cout << "radial " << per_pixel_radial << '\n';
    std::cout << "conic " << per_pixel_conic << '\n';
    std::cout << "gradient_stop " << (per_pixel_many - per_pixel_linear) / 30.0 << '\n';
    std::cout << "outer_shadow " << per_pixel_outer << '\n';
    std::cout << "outer_corner " << per_pixel_corner << '\n';
    std::cout << "inset_shadow " << per_pixel_inset << '\n';
    std::cout << "border " << per_pixel_border << '\n';
    std::cout << "border_pattern " << per_pixel_pattern << '\n';
    std::cout << "border_edge " << per_pixel_edge << '\n';
    std::cout << "keyline " << per_pixel_keyline << '\n';
    std::cout << "image " << per_pixel_image << '\n';
    std::cout << "colour_filter " << per_pixel_colour_filter << '\n';
    std::cout << "drop_shadow " << per_pixel_drop_shadow << '\n';
    std::cout << "filter_blur " << per_pixel_filter_blur << '\n';
    std::cout << "fill_blend " << per_pixel_blend << '\n';
}

} // namespace

int main(int argc, char** argv) {
    if (argc > 1 && std::strcmp(argv[1], "--measure") == 0) {
        measure();
        return 0;
    }
    choices_are_the_recipe_roles();
    refusals();
    solid_and_border();
    shadow_offset_and_clip();
    straight_edge_blur_is_half();
    gradients();
    keylines_and_inset_shadow();
    glows();
    glow_painter_path();
    gloss_preset();
    image_fills();
    image_variant_made_once();
    image_variant_outside_the_bound();
    reference_images();
    engine_draws_the_material();
    engine_counts_refusals();
    std::cout << "gui_forms_engine_material_tests passed\n";
    return 0;
}
