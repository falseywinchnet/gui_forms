// The engine's primitive registry and its first primitives (T2 plan, phase
// E1): registration and parameter checks refuse without changing anything,
// a destination is validated before any byte is written, and the ribbon's
// bulge and dithered bulge (planning/stages/T4-default-theme/
// RENDERING_REFERENCE.md §2, §3) draw what the reference describes, with
// the same bytes on every CI platform.
//
// The reference images are FNV-1a hashes of each raster's rows in RGBA
// order. The expected values were produced on Windows x64 and are checked
// by every CI platform. GUI_FORMS_ENGINE_IDENTITY_PRINT=1 prints the
// observed table instead of checking it.

#include "gui_forms/engine/primitive_registry/primitive_registry.hpp"

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace {
using namespace gui_forms;

void require(bool condition, const char* message) {
    if (condition) return;
    std::cerr << message << '\n';
    std::exit(1);
}

bool print_mode() {
    const char* const value = std::getenv("GUI_FORMS_ENGINE_IDENTITY_PRINT");
    const bool printing = value != nullptr && std::strcmp(value, "1") == 0;
    return printing;
}

const EnginePrimitive& builtin(std::string_view name) {
    const EnginePrimitive* const primitive = builtin_primitive_registry().find(name);
    require(primitive != nullptr, "the built-in registry has the primitive");
    return *primitive;
}

// A raster the tests own, with a row stride wider than its pixels so that
// padding is exercised.
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

constexpr std::uint8_t padding_byte = 0xA5;

TestRaster make_raster(Size logical, double scale) {
    const std::uint32_t width = static_cast<std::uint32_t>(std::ceil(logical.width * scale));
    const std::uint32_t height = static_cast<std::uint32_t>(std::ceil(logical.height * scale));
    TestRaster raster{};
    raster.extent = DeviceExtent{width, height};
    raster.row_bytes = static_cast<std::size_t>(width) * 4U + 8U;
    raster.bytes.assign(raster.row_bytes * height, padding_byte);
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

bool padding_untouched(const TestRaster& raster) {
    for (std::uint32_t row = 0U; row < raster.extent.height; ++row) {
        const std::size_t start = static_cast<std::size_t>(row) * raster.row_bytes;
        const std::size_t padding = start + static_cast<std::size_t>(raster.extent.width) * 4U;
        for (std::size_t index = padding; index < start + raster.row_bytes; ++index) {
            if (raster.bytes[index] != padding_byte) return false;
        }
    }
    return true;
}

AnatomyStateSet states_of(bool pressed, bool active) {
    AnatomyStateSet states{};
    if (pressed) states.insert(AnatomyState::pressed);
    if (active) states.insert(AnatomyState::active_tool);
    return states;
}

RasterOutcome draw(const EnginePrimitive& primitive, const PrimitiveParameterBlock& parameters, TestRaster& raster,
                   Size logical, double scale, AnatomyStateSet states) {
    const PrimitiveTarget target{logical, DeviceScale{scale}, states};
    const RasterOutcome outcome = primitive.rasterize(parameters, target, raster.view());
    return outcome;
}

// -- registry ------------------------------------------------------------------

// A primitive whose descriptor the tests choose.
class FixturePrimitive final : public EnginePrimitive {
public:
    explicit FixturePrimitive(const PrimitiveDescriptor& described) : described_(described) {}
    [[nodiscard]] const PrimitiveDescriptor& descriptor() const noexcept override { return described_; }

private:
    void fill(const PrimitiveParameterBlock&, const PrimitiveTarget&, const PartRasterView&) const noexcept override {}
    PrimitiveDescriptor described_;
};

const PartRole face_role[] = {PartRole::face};
const PrimitiveParameterDeclaration one_number[] = {{"strength", PrimitiveParameterKind::number, 0.0, 1.0, 0.5, {}}};
const PrimitiveParameterDeclaration bad_default[] = {{"strength", PrimitiveParameterKind::number, 0.0, 1.0, 2.0, {}}};
const PrimitiveParameterDeclaration bad_name[] = {{"Strength", PrimitiveParameterKind::number, 0.0, 1.0, 0.5, {}}};
const PrimitiveParameterDeclaration twice[] = {{"strength", PrimitiveParameterKind::number, 0.0, 1.0, 0.5, {}},
                                               {"strength", PrimitiveParameterKind::number, 0.0, 1.0, 0.5, {}}};

void test_builtin_registry() {
    const PrimitiveRegistry& registry = builtin_primitive_registry();
    require(registry.primitives().size() == 2U, "the built-in registry holds the bulge pair");
    require(registry.find("not_a_primitive") == nullptr, "an unknown name finds nothing");
    const char* const names[] = {"bulge", "dithered_bulge"};
    for (const char* const name : names) {
        const PrimitiveDescriptor& descriptor = builtin(name).descriptor();
        require(descriptor.name == name, "a primitive is found under its own name");
        require(descriptor.roles.size() == 1U && descriptor.roles[0] == PartRole::face, "the bulge paints faces");
        std::set<AnatomyState> read(descriptor.states_read.begin(), descriptor.states_read.end());
        require(read.size() == 2U && read.count(AnatomyState::pressed) == 1U &&
                    read.count(AnatomyState::active_tool) == 1U,
                "the bulge reads pressed and active_tool only");
        require(descriptor.introduced == current_engine_version, "the bulge pair is engine 0.1");
        require(descriptor.cost.per_device_pixel > 0U, "the bulge declares its cost");
        const PrimitiveParameterBlock defaults = default_parameters(descriptor);
        require(validate_parameters(descriptor, defaults).defect == ParameterDefect::none, "defaults validate");
    }
}

void test_registration_refusals() {
    PrimitiveRegistry registry{};
    const PrimitiveDescriptor good{"fixture", EngineVersion{0, 1}, face_role, {}, one_number, PrimitiveCost{1U, 1U}, {}};
    const FixturePrimitive first{good};
    require(registry.register_primitive(first) == RegistrationOutcome::registered, "a valid primitive registers");
    const FixturePrimitive again{good};
    require(registry.register_primitive(again) == RegistrationOutcome::duplicate_name, "a duplicate name is refused");
    require(registry.primitives().size() == 1U, "a refusal leaves the registry unchanged");

    PrimitiveDescriptor descriptor = good;
    descriptor.name = "Fixture";
    const FixturePrimitive upper{descriptor};
    require(registry.register_primitive(upper) == RegistrationOutcome::name_not_snake_case, "names are snake_case");

    descriptor = good;
    descriptor.name = "no_roles";
    descriptor.roles = {};
    const FixturePrimitive roleless{descriptor};
    require(registry.register_primitive(roleless) == RegistrationOutcome::no_roles, "a primitive paints a role");

    descriptor = good;
    descriptor.name = "late";
    descriptor.introduced = EngineVersion{current_engine_version.major, static_cast<std::uint16_t>(current_engine_version.minor + 1U)};
    const FixturePrimitive late{descriptor};
    require(registry.register_primitive(late) == RegistrationOutcome::introduced_after_engine,
            "a primitive from a later engine is refused");

    descriptor = good;
    descriptor.name = "bad_default";
    descriptor.parameters = bad_default;
    const FixturePrimitive out_of_range{descriptor};
    require(registry.register_primitive(out_of_range) == RegistrationOutcome::default_out_of_range,
            "a default outside its range is refused");

    descriptor = good;
    descriptor.name = "bad_parameter";
    descriptor.parameters = bad_name;
    const FixturePrimitive misnamed{descriptor};
    require(registry.register_primitive(misnamed) == RegistrationOutcome::parameter_name_not_snake_case,
            "parameter names are snake_case");

    descriptor = good;
    descriptor.name = "twice";
    descriptor.parameters = twice;
    const FixturePrimitive duplicated{descriptor};
    require(registry.register_primitive(duplicated) == RegistrationOutcome::duplicate_parameter_name,
            "a parameter name is declared once");
    require(registry.primitives().size() == 1U, "every refusal left the registry unchanged");
}

void test_parameter_refusals() {
    const PrimitiveDescriptor& descriptor = builtin("bulge").descriptor();
    PrimitiveParameterBlock block = default_parameters(descriptor);
    const PrimitiveParameterBlock before = block;
    require(set_number_parameter(block, descriptor, "amplitude", 1.5) == ParameterAssignment::out_of_range,
            "an out-of-range number is refused");
    require(set_number_parameter(block, descriptor, "amplitude", std::nan("")) == ParameterAssignment::not_finite,
            "NaN is refused");
    require(set_number_parameter(block, descriptor, "light_colour", 0.5) == ParameterAssignment::wrong_kind,
            "a number is not a colour");
    require(set_colour_parameter(block, descriptor, "amplitude", Color{}) == ParameterAssignment::wrong_kind,
            "a colour is not a number");
    require(set_number_parameter(block, descriptor, "glow", 0.5) == ParameterAssignment::unknown_name,
            "an undeclared name is refused");
    bool unchanged = true;
    for (std::size_t index = 0U; index < block.count; ++index) {
        if (!(block.values[index] == before.values[index])) unchanged = false;
    }
    require(unchanged, "refused assignments leave the block unchanged");
    require(set_number_parameter(block, descriptor, "amplitude", 0.25) == ParameterAssignment::assigned,
            "an in-range number is assigned");

    PrimitiveParameterBlock short_block = block;
    short_block.count = block.count - 1U;
    require(validate_parameters(descriptor, short_block).defect == ParameterDefect::wrong_count,
            "a block for another primitive is refused");
    PrimitiveParameterBlock forged = block;
    forged.values[0].number = 3.0;
    const ParameterValidation validation = validate_parameters(descriptor, forged);
    require(validation.defect == ParameterDefect::out_of_range && validation.parameter == 0U,
            "validation names the parameter out of range");
}

// -- destination ---------------------------------------------------------------

void test_destination_refusals() {
    const EnginePrimitive& bulge = builtin("bulge");
    const PrimitiveParameterBlock defaults = default_parameters(bulge.descriptor());
    const Size logical{20.0, 10.0};

    TestRaster raster = make_raster(logical, 1.0);
    const std::vector<std::uint8_t> before = raster.bytes;
    PartRasterView view = raster.view();
    view.pixels = view.pixels.first(view.pixels.size() - 9U);
    const PrimitiveTarget target{logical, DeviceScale{1.0}, {}};
    require(bulge.rasterize(defaults, target, view) == RasterOutcome::invalid_view, "a short buffer is refused");
    view = raster.view();
    view.row_bytes = 20U * 4U - 4U;
    require(bulge.rasterize(defaults, target, view) == RasterOutcome::invalid_view, "a short stride is refused");
    view.row_bytes = 20U * 4U + 2U;
    require(bulge.rasterize(defaults, target, view) == RasterOutcome::invalid_view, "a split-pixel stride is refused");
    view = raster.view();
    view.extent = DeviceExtent{maximum_part_raster_side + 1U, 1U};
    require(bulge.rasterize(defaults, target, view) == RasterOutcome::invalid_view, "an oversized extent is refused");

    const PrimitiveTarget no_scale{logical, DeviceScale{0.25}, {}};
    require(bulge.rasterize(defaults, no_scale, raster.view()) == RasterOutcome::invalid_target,
            "a scale below 0.5 is refused");
    const PrimitiveTarget no_size{Size{0.0, 10.0}, DeviceScale{1.0}, {}};
    require(bulge.rasterize(defaults, no_size, raster.view()) == RasterOutcome::invalid_target,
            "an empty part is refused");
    PrimitiveParameterBlock forged = defaults;
    forged.values[1].number = 0.0;
    require(bulge.rasterize(forged, target, raster.view()) == RasterOutcome::invalid_parameters,
            "invalid parameters are refused");
    require(raster.bytes == before, "every refusal left the destination's bytes unchanged");

    view = raster.view();
    view.extent = DeviceExtent{0U, 0U};
    require(bulge.rasterize(defaults, target, view) == RasterOutcome::empty, "an empty destination paints nothing");
    require(raster.bytes == before, "an empty destination is not written");
}

// -- what the bulge draws ------------------------------------------------------

constexpr Size large_button{40.0, 56.0};

void test_amplitude_zero_is_the_plane() {
    const char* const names[] = {"bulge", "dithered_bulge"};
    for (const char* const name : names) {
        const EnginePrimitive& primitive = builtin(name);
        PrimitiveParameterBlock parameters = default_parameters(primitive.descriptor());
        require(set_number_parameter(parameters, primitive.descriptor(), "amplitude", 0.0) ==
                    ParameterAssignment::assigned,
                "amplitude set to zero");
        const bool variants[] = {false, true};
        for (const bool pressed : variants) {
            for (const bool active : variants) {
                TestRaster raster = make_raster(large_button, 1.5);
                require(draw(primitive, parameters, raster, large_button, 1.5, states_of(pressed, active)) ==
                            RasterOutcome::painted,
                        "the flat bulge paints");
                bool transparent = true;
                for (std::uint32_t row = 0U; row < raster.extent.height; ++row) {
                    for (std::uint32_t column = 0U; column < raster.extent.width; ++column) {
                        if (raster.pixel(column, row)[3] != 0U) transparent = false;
                    }
                }
                require(transparent, "at amplitude 0 the plane shows everywhere");
                require(padding_untouched(raster), "row padding is never written");
            }
        }
    }
}

void test_smooth_bulge_is_premultiplied_and_edgeless() {
    const EnginePrimitive& bulge = builtin("bulge");
    const PrimitiveParameterBlock parameters = default_parameters(bulge.descriptor());
    TestRaster raster = make_raster(large_button, 2.0);
    require(draw(bulge, parameters, raster, large_button, 2.0, {}) == RasterOutcome::painted, "the bulge paints");
    bool premultiplied = true;
    for (std::uint32_t row = 0U; row < raster.extent.height; ++row) {
        for (std::uint32_t column = 0U; column < raster.extent.width; ++column) {
            const std::array<std::uint8_t, 4> pixel = raster.pixel(column, row);
            if (pixel[0] > pixel[3] || pixel[1] > pixel[3] || pixel[2] > pixel[3]) premultiplied = false;
        }
    }
    require(premultiplied, "no channel exceeds alpha");
    // Every field is zero at the top-left corner: the plane shows there.
    require(raster.pixel(0U, 0U)[3] == 0U, "the top-left corner is the plane");
    // The lit slope is white and the shaded one blue-grey: unpremultiplied,
    // the pixel at the light's centre is redder (whiter) than the one at the
    // shade's centre.
    const std::array<std::uint8_t, 4> lit = raster.pixel(static_cast<std::uint32_t>(0.42 * 80.0),
                                                         static_cast<std::uint32_t>(0.38 * 112.0));
    const std::array<std::uint8_t, 4> shaded = raster.pixel(static_cast<std::uint32_t>(0.60 * 80.0),
                                                            static_cast<std::uint32_t>(0.64 * 112.0));
    require(lit[3] > 0U && shaded[3] > 0U, "both slopes cover the plane");
    const double lit_red = static_cast<double>(lit[0]) / static_cast<double>(lit[3]);
    const double shaded_red = static_cast<double>(shaded[0]) / static_cast<double>(shaded[3]);
    require(lit_red > shaded_red, "the lit slope is whiter than the shaded slope");
    require(padding_untouched(raster), "row padding is never written");
}

struct DotCounts final {
    std::size_t lit_upper_left{};
    std::size_t lit_lower_right{};
};

bool is_lit_dot(const std::array<std::uint8_t, 4>& pixel) {
    // The default lit steps are near white; the shaded steps are blue.
    const bool lit = pixel[3] == 255U && pixel[0] >= 236U;
    return lit;
}

DotCounts count_lit_dots(const TestRaster& raster) {
    DotCounts counts{};
    const std::uint32_t half_width = raster.extent.width / 2U;
    const std::uint32_t half_height = raster.extent.height / 2U;
    for (std::uint32_t row = 0U; row < raster.extent.height; ++row) {
        for (std::uint32_t column = 0U; column < raster.extent.width; ++column) {
            if (!is_lit_dot(raster.pixel(column, row))) continue;
            if (column < half_width && row < half_height) counts.lit_upper_left = counts.lit_upper_left + 1U;
            if (column >= half_width && row >= half_height) counts.lit_lower_right = counts.lit_lower_right + 1U;
        }
    }
    return counts;
}

void test_dithered_lattice_and_tones() {
    const EnginePrimitive& dithered = builtin("dithered_bulge");
    const PrimitiveParameterBlock parameters = default_parameters(dithered.descriptor());
    TestRaster raster = make_raster(large_button, 1.0);
    require(draw(dithered, parameters, raster, large_button, 1.0, {}) == RasterOutcome::painted, "the dither paints");
    std::set<std::uint32_t> colours{};
    bool on_lattice = true;
    bool opaque_or_clear = true;
    for (std::uint32_t row = 0U; row < raster.extent.height; ++row) {
        for (std::uint32_t column = 0U; column < raster.extent.width; ++column) {
            const std::array<std::uint8_t, 4> pixel = raster.pixel(column, row);
            if (pixel[3] == 0U) continue;
            if (pixel[3] != 255U) opaque_or_clear = false;
            if (column % 2U != 1U || row % 2U != 1U) on_lattice = false;
            const std::uint32_t rgb = (static_cast<std::uint32_t>(pixel[0]) << 16U) |
                                      (static_cast<std::uint32_t>(pixel[1]) << 8U) | pixel[2];
            colours.insert(rgb);
        }
    }
    require(on_lattice, "at scale 1 dots fall only on odd positions");
    require(opaque_or_clear, "a dot is opaque; between dots the plane shows");
    // Not a two-colour dither: three lit steps and the shaded steps appear.
    // With the reference's default gains the tone never falls below about
    // -0.63, short of the -0.68 the third shaded step needs, so only two
    // shaded steps show (the dark variant reaches all three; see below).
    require(colours.count(0xECF4FEU) == 1U && colours.count(0xF6FAFFU) == 1U && colours.count(0xFFFFFFU) == 1U,
            "all three lit steps appear");
    require(colours.count(0xB0C6E8U) == 1U && colours.count(0x84A2D4U) == 1U, "two shaded steps appear");
    require(colours.size() == 5U, "only the palette's colours appear");

    const DotCounts bulge = count_lit_dots(raster);
    require(bulge.lit_upper_left > bulge.lit_lower_right, "the bulge is lit toward the upper-left light");
    TestRaster dent_raster = make_raster(large_button, 1.0);
    require(draw(dithered, parameters, dent_raster, large_button, 1.0, states_of(true, false)) == RasterOutcome::painted,
            "the dent paints");
    const DotCounts dent = count_lit_dots(dent_raster);
    require(dent.lit_lower_right > dent.lit_upper_left, "the dent is lit on the far slope");
}

// The dark variant's gains (shade 1.0) reach about -0.80, so all three of
// its shaded steps appear.
void apply_dark_palette(PrimitiveParameterBlock& parameters, const PrimitiveDescriptor& descriptor);

void test_dark_variant_reaches_every_step() {
    const EnginePrimitive& dithered = builtin("dithered_bulge");
    PrimitiveParameterBlock parameters = default_parameters(dithered.descriptor());
    apply_dark_palette(parameters, dithered.descriptor());
    TestRaster raster = make_raster(large_button, 1.0);
    require(draw(dithered, parameters, raster, large_button, 1.0, {}) == RasterOutcome::painted, "the dark dither paints");
    std::set<std::uint32_t> colours{};
    for (std::uint32_t row = 0U; row < raster.extent.height; ++row) {
        for (std::uint32_t column = 0U; column < raster.extent.width; ++column) {
            const std::array<std::uint8_t, 4> pixel = raster.pixel(column, row);
            if (pixel[3] == 0U) continue;
            const std::uint32_t rgb = (static_cast<std::uint32_t>(pixel[0]) << 16U) |
                                      (static_cast<std::uint32_t>(pixel[1]) << 8U) | pixel[2];
            colours.insert(rgb);
        }
    }
    require(colours.count(0x0E120FU) == 1U && colours.count(0x080A09U) == 1U && colours.count(0x000000U) == 1U,
            "all three dark shaded steps appear");
    require(colours.count(0x344238U) == 1U && colours.count(0x4E6252U) == 1U && colours.count(0x76966CU) == 1U,
            "all three dark lit steps appear");
}

void test_dot_size_follows_scale() {
    const EnginePrimitive& dithered = builtin("dithered_bulge");
    const PrimitiveParameterBlock parameters = default_parameters(dithered.descriptor());
    TestRaster raster = make_raster(large_button, 2.0);
    require(draw(dithered, parameters, raster, large_button, 2.0, {}) == RasterOutcome::painted, "the dither paints");
    // At scale 2 the dot at logical (x, y) covers device [2x, 2x + 2) on each
    // axis, so device rows and columns 0 and 1 of every 4 stay clear.
    bool exact = true;
    for (std::uint32_t row = 0U; row < raster.extent.height; ++row) {
        for (std::uint32_t column = 0U; column < raster.extent.width; ++column) {
            const bool inside_dot = column % 4U >= 2U && row % 4U >= 2U;
            if (!inside_dot && raster.pixel(column, row)[3] != 0U) exact = false;
        }
    }
    require(exact, "at scale 2 each dot is a 2 × 2 device square on the 4-pixel lattice");
}

// -- reference images ----------------------------------------------------------

struct ReferenceCase final {
    const char* primitive;
    const char* variant;
    bool pressed;
    bool active;
    double amplitude;
    bool dark;
};

constexpr std::array<ReferenceCase, 9> reference_cases{{
    {"bulge", "rest", false, false, 1.0, false},
    {"bulge", "pressed", true, false, 1.0, false},
    {"bulge", "active", false, true, 1.0, false},
    {"bulge", "half", false, false, 0.5, false},
    {"dithered_bulge", "rest", false, false, 1.0, false},
    {"dithered_bulge", "pressed", true, false, 1.0, false},
    {"dithered_bulge", "active", false, true, 1.0, false},
    {"dithered_bulge", "half", false, false, 0.5, false},
    {"dithered_bulge", "dark", false, false, 1.0, true},
}};

constexpr std::array<double, 4> reference_scales{1.0, 1.25, 1.5, 2.0};

// Expected hashes, [case][scale].
constexpr std::array<std::array<std::uint64_t, 4>, 9> expected_hashes{{
    {0x11f9d12a3f56dc56ULL, 0x61b646439800fd9bULL, 0x31ea1814e17e8f60ULL, 0x96a785b1da65cb4aULL}, // bulge rest
    {0x86797a9c95bfa496ULL, 0xee41b09f75d6affaULL, 0x6c935b6cc7110de0ULL, 0x23a0399065abafdbULL}, // bulge pressed
    {0xae9d9cfdef9517baULL, 0x4ed51eb848557c67ULL, 0x4f5b4e8d0dc6a8bbULL, 0x0ff0e6fe00b6e4d4ULL}, // bulge active
    {0xc3ae7106580c8b8eULL, 0x8b55306473494b7aULL, 0xe1fd3dae3506bc64ULL, 0x1b214abcbfdca34aULL}, // bulge half
    {0x53cca0febc6ccef7ULL, 0x09c71636306ad837ULL, 0xad00e1ff56523eb5ULL, 0xe7555604ef50bcf5ULL}, // dithered rest
    {0x5ecafcd3609907c7ULL, 0x3edaea0674a3d037ULL, 0xe451b3df306bb9ddULL, 0x64067dc3a4ee299dULL}, // dithered pressed
    {0xad170a954e2e3b4cULL, 0x1236e56c286ef7ccULL, 0x7674a8a01cd5bf55ULL, 0xd840f9e4182893d5ULL}, // dithered active
    {0x2c3fc566a9e7e9d2ULL, 0x33f268d0ee940702ULL, 0xdfb5f96dd1ffb4edULL, 0x82b6a73be196ccadULL}, // dithered half
    {0x5208613e5af59721ULL, 0xe3bb4ccf88b74cd1ULL, 0x9e76a329d67671c5ULL, 0x30da795d91c6d9c5ULL}, // dithered dark
}};

// GUI_FORMS_ENGINE_DUMP=<directory> writes each reference raster's pixels
// (rows of RGBA, no padding) to <primitive>_<variant>_<scale x 100>_<w>x<h>.rgba,
// for comparing them by eye with the rendering reference.
void dump_raster(const TestRaster& raster, const char* primitive, const char* variant, double scale) {
    const char* const directory = std::getenv("GUI_FORMS_ENGINE_DUMP");
    if (directory == nullptr) return;
    const int percent = static_cast<int>(scale * 100.0 + 0.5);
    const std::string path = std::string(directory) + "/" + primitive + "_" + variant + "_" + std::to_string(percent) +
                             "_" + std::to_string(raster.extent.width) + "x" + std::to_string(raster.extent.height) +
                             ".rgba";
    std::ofstream file(path, std::ios::binary);
    for (std::uint32_t row = 0U; row < raster.extent.height; ++row) {
        const std::size_t start = static_cast<std::size_t>(row) * raster.row_bytes;
        const char* const bytes = reinterpret_cast<const char*>(raster.bytes.data() + start);
        file.write(bytes, static_cast<std::streamsize>(raster.extent.width) * 4);
    }
}

// The dark variant's dithered palette (reference §3, "Dark variant").
void apply_dark_palette(PrimitiveParameterBlock& parameters, const PrimitiveDescriptor& descriptor) {
    struct NamedColour final {
        const char* name;
        Color colour;
    };
    const NamedColour dark[] = {
        {"lit_1", {52, 66, 56, 255}},      {"lit_2", {78, 98, 82, 255}},        {"lit_3", {118, 150, 108, 255}},
        {"shaded_1", {14, 18, 15, 255}},   {"shaded_2", {8, 10, 9, 255}},       {"shaded_3", {0, 0, 0, 255}},
        {"gold_lit_1", {150, 90, 120, 255}}, {"gold_lit_2", {190, 120, 150, 255}}, {"gold_lit_3", {225, 160, 190, 255}},
        {"gold_body_1", {110, 58, 84, 255}}, {"gold_body_2", {140, 72, 104, 255}}, {"gold_body_3", {170, 92, 128, 255}},
        {"gold_shaded_1", {70, 30, 50, 255}}, {"gold_shaded_2", {52, 20, 36, 255}}, {"gold_shaded_3", {34, 10, 22, 255}},
    };
    for (const NamedColour& entry : dark) {
        require(set_colour_parameter(parameters, descriptor, entry.name, entry.colour) == ParameterAssignment::assigned,
                "a dark palette colour is assigned");
    }
    require(set_number_parameter(parameters, descriptor, "lit_gain", 0.9) == ParameterAssignment::assigned, "lit gain");
    require(set_number_parameter(parameters, descriptor, "shade_gain", 1.0) == ParameterAssignment::assigned,
            "shade gain");
    require(set_number_parameter(parameters, descriptor, "crown_gain", 0.2) == ParameterAssignment::assigned,
            "crown gain");
}

void test_reference_images() {
    const bool printing = print_mode();
    bool all_match = true;
    for (std::size_t case_index = 0U; case_index < reference_cases.size(); ++case_index) {
        const ReferenceCase& reference = reference_cases[case_index];
        const EnginePrimitive& primitive = builtin(reference.primitive);
        PrimitiveParameterBlock parameters = default_parameters(primitive.descriptor());
        require(set_number_parameter(parameters, primitive.descriptor(), "amplitude", reference.amplitude) ==
                    ParameterAssignment::assigned,
                "the case's amplitude is assigned");
        if (reference.dark) apply_dark_palette(parameters, primitive.descriptor());
        if (printing) std::cout << "    {";
        for (std::size_t scale_index = 0U; scale_index < reference_scales.size(); ++scale_index) {
            const double scale = reference_scales[scale_index];
            TestRaster raster = make_raster(large_button, scale);
            const AnatomyStateSet states = states_of(reference.pressed, reference.active);
            require(draw(primitive, parameters, raster, large_button, scale, states) == RasterOutcome::painted,
                    "a reference case paints");
            const std::uint64_t hash = raster_hash(raster);
            dump_raster(raster, reference.primitive, reference.variant, scale);
            if (printing) {
                std::cout << "0x" << std::hex << std::setw(16) << std::setfill('0') << hash << std::dec << "ULL";
                if (scale_index + 1U < reference_scales.size()) std::cout << ", ";
                continue;
            }
            if (hash != expected_hashes[case_index][scale_index]) {
                std::cerr << "reference image differs: " << reference.primitive << ' ' << reference.variant
                          << " at scale " << scale << ": 0x" << std::hex << hash << std::dec << '\n';
                all_match = false;
            }
        }
        if (printing) std::cout << "}, // " << reference.primitive << ' ' << reference.variant << '\n';
    }
    require(all_match, "every reference image has the same bytes on every host");
}

} // namespace

int main() {
    test_builtin_registry();
    test_registration_refusals();
    test_parameter_refusals();
    test_destination_refusals();
    test_amplitude_zero_is_the_plane();
    test_smooth_bulge_is_premultiplied_and_edgeless();
    test_dithered_lattice_and_tones();
    test_dark_variant_reaches_every_step();
    test_dot_size_follows_scale();
    test_reference_images();
    std::cout << "engine primitive tests passed\n";
    return 0;
}
