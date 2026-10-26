// CSS's material properties (stage T3, phase P6b): background-color,
// background-image with the six gradient functions, the `background`
// shorthand, background-blend-mode, the border family, border-radius and
// box-shadow lower onto the engine's material fields, and a rule of fields
// alone selects the material primitive (owner C7). The authored gradients resolve as CSS places them: stop fixup, sides
// and corners, the radial extents, repeating periods, lengths along the
// gradient. Gate: for each form, a sheet and the same material written in
// C++ bake identical pixels.

#include "../../src/core/theme_language/compiler/theme_compiler.hpp"
#include "../support/theme_definition_heap.hpp"
#include "gui_forms/gui_forms.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#if defined(__clang__)
#pragma clang fp contract(off)
#endif

namespace {

using namespace gui_forms;
using theme_language::CompileOutcome;
using theme_language::DiagnosticCode;
using theme_language::SelectorVocabulary;
using theme_language::ThemeCompileInput;
using theme_language::ThemeCompiler;
using theme_language::ThemeDiagnostic;
using theme_language::ThemeDiagnosticList;
using theme_language::ThemeSource;
using theme_language::diagnostic_code_name;

int failures = 0;

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        ++failures;
    }
}

constexpr double pi = 3.14159265358979323846264338327950288;

const Color red = Color::rgba(255, 0, 0);
const Color blue = Color::rgba(0, 0, 255);
const Color green = Color::rgba(0, 128, 0);

AuthoredGradientStop stop(Color colour) {
    AuthoredGradientStop made{};
    made.colour = colour;
    return made;
}

AuthoredGradientStop stop_at(Color colour, double fraction, double units = 0.0) {
    AuthoredGradientStop made{};
    made.colour = colour;
    made.positioned = true;
    made.position = BoxLength{fraction, units};
    return made;
}

AuthoredGradient gradient_of(MaterialFillKind kind, std::vector<AuthoredGradientStop> stops) {
    AuthoredGradient gradient{};
    gradient.kind = kind;
    gradient.stops = std::move(stops);
    return gradient;
}

MaterialFillLayer resolved(const AuthoredGradient& gradient, Size box) {
    require(valid_authored_gradient(gradient), "the gradient is valid");
    MaterialFillLayer layer{};
    resolve_authored_gradient(gradient, box, layer);
    return layer;
}

std::vector<GradientStop> stops_of(std::initializer_list<GradientStop> stops) {
    std::vector<GradientStop> made(stops);
    return made;
}

constexpr double separation = 1.0 / 1048576.0;

// -- the resolution ----------------------------------------------------------------

void test_placement() {
    AuthoredGradient linear = gradient_of(MaterialFillKind::linear_gradient, {stop(red), stop(green), stop(blue)});
    linear.angle_degrees = 135.0;
    require(authored_gradient_fixed(linear), "a gradient of angles and fractions is fixed");
    require(resolved(linear, Size{40.0, 24.0}) ==
                MaterialFillLayer::linear_css_angle(135.0, stops_of({{0.0, red}, {0.5, green}, {1.0, blue}})),
            "unplaced stops spread evenly, as the CSS angle form C++ writes");

    linear.stops = {stop_at(red, 0.2), stop(green), stop_at(blue, 0.8)};
    require(resolved(linear, Size{}).stops ==
                stops_of({{0.0, red}, {0.2, red}, {0.5, green}, {0.8, blue}, {1.0, blue}}),
            "stops short of the ends are padded with their colours");

    linear.stops = {stop_at(red, 0.5), stop_at(blue, 0.2)};
    require(resolved(linear, Size{}).stops ==
                stops_of({{0.0, red}, {0.5, red}, {0.5 + separation, blue}, {1.0, blue}}),
            "a stop before an earlier one moves to it: a hard stop, separated by 2^-20");

    linear.stops = {stop_at(red, -0.5), stop_at(blue, 1.5)};
    const MaterialFillLayer clipped = resolved(linear, Size{});
    require(clipped.stops.size() == 2U && clipped.stops[0].color == Color::rgba(191, 0, 64) &&
                clipped.stops[1].color == Color::rgba(64, 0, 191),
            "stops past the ends are clipped to the colours at the ends");

    linear.stops = {stop(red)};
    require(resolved(linear, Size{}) == MaterialFillLayer::solid(red), "a single stop is a solid");
}

void test_linear_geometry() {
    AuthoredGradient corner = gradient_of(MaterialFillKind::linear_gradient, {stop(red), stop(blue)});
    corner.corner = GradientCorner::top_right;
    require(!authored_gradient_fixed(corner), "a corner reads the box");
    const double diagonal = engine_arctangent2(24.0, 40.0) * (180.0 / pi);
    require(resolved(corner, Size{40.0, 24.0}).angle_degrees == diagonal, "to top right: the diagonal's angle");
    corner.corner = GradientCorner::bottom_left;
    require(resolved(corner, Size{40.0, 24.0}).angle_degrees == 180.0 + diagonal, "to bottom left: opposite");

    AuthoredGradient lengths = gradient_of(MaterialFillKind::linear_gradient,
                                           {stop_at(red, 0.0, 6.0), stop_at(blue, 0.0, 18.0)});
    lengths.angle_degrees = 0.0;
    require(resolved(lengths, Size{40.0, 24.0}).stops ==
                stops_of({{0.0, red}, {0.25, red}, {0.75, blue}, {1.0, blue}}),
            "lengths are fractions of the box's gradient line");

    AuthoredGradient repeating = lengths;
    repeating.repeating = true;
    repeating.stops = {stop_at(red, 0.0, 0.0), stop_at(blue, 0.0, 6.0)};
    require(resolved(repeating, Size{40.0, 24.0}) ==
                MaterialFillLayer::repeating_linear(Point{20.0, 24.0}, Point{20.0, 18.0},
                                                    stops_of({{0.0, red}, {1.0, blue}}),
                                                    MaterialCoordinateSpace::logical),
            "a repeating period narrower than the line is the line's span in logical units");
    repeating.stops = {stop(red), stop(blue)};
    require(authored_gradient_fixed(repeating) &&
                resolved(repeating, Size{}) ==
                    MaterialFillLayer::linear_css_angle(0.0, stops_of({{0.0, red}, {1.0, blue}}),
                                                        GradientSpreadMode::repeat),
            "a repeating gradient over the whole line keeps its CSS angle");
    repeating.stops = {stop_at(red, 0.5), stop_at(blue, 0.5)};
    require(resolved(repeating, Size{40.0, 24.0}) == MaterialFillLayer::solid(Color::rgba(128, 0, 128)),
            "a repeating gradient of no length is CSS's average colour");
}

void test_radial_geometry() {
    AuthoredGradient ellipse = gradient_of(MaterialFillKind::radial_gradient, {stop(red), stop(blue)});
    require(authored_gradient_fixed(ellipse), "an ellipse of keywords is fixed");
    const double corner = 0.5 * 1.41421356237309504880168872420969808;
    require(resolved(ellipse, Size{}) ==
                MaterialFillLayer::radial(Point{0.5, 0.5}, Size{corner, corner}, stops_of({{0.0, red}, {1.0, blue}})),
            "CSS's default ellipse reaches the corners, in normalized coordinates");
    ellipse.extent = RadialExtent::closest_side;
    ellipse.centre_x = BoxLength{0.25, 0.0};
    require(resolved(ellipse, Size{}).radii == (Size{0.25, 0.5}), "closest-side, each axis on its own");

    AuthoredGradient circle = ellipse;
    circle.circle = true;
    require(!authored_gradient_fixed(circle), "a circle reads the box");
    require(resolved(circle, Size{40.0, 24.0}) ==
                MaterialFillLayer::radial(Point{10.0, 12.0}, Size{10.0, 10.0}, stops_of({{0.0, red}, {1.0, blue}}),
                                          MaterialCoordinateSpace::logical),
            "a closest-side circle in logical units");

    AuthoredGradient beyond = gradient_of(MaterialFillKind::radial_gradient, {stop(red), stop_at(blue, 2.0)});
    const MaterialFillLayer far = resolved(beyond, Size{});
    require(far.radii == (Size{corner * 2.0, corner * 2.0}) && far.stops == stops_of({{0.0, red}, {1.0, blue}}),
            "a stop past the ray widens the radii");

    AuthoredGradient rings = gradient_of(MaterialFillKind::radial_gradient,
                                         {stop_at(red, 0.0, 25.0), stop_at(blue, 0.0, 75.0)});
    rings.repeating = true;
    rings.circle = true;
    rings.extent = RadialExtent::lengths;
    rings.radius_x = BoxLength{0.0, 100.0};
    const MaterialFillLayer ringed = resolved(rings, Size{40.0, 24.0});
    const Color middle = Color::rgba(128, 0, 128);
    require(ringed.spread == GradientSpreadMode::repeat && ringed.radii == (Size{50.0, 50.0}) &&
                ringed.stops == stops_of({{0.0, middle}, {0.5, blue}, {0.5 + separation, red}, {1.0, middle}}),
            "a repeating period that begins off the centre is rotated to begin there");

    AuthoredGradient wrong = circle;
    wrong.extent = RadialExtent::lengths;
    wrong.radius_x = BoxLength{0.5, 0.0};
    require(!valid_authored_gradient(wrong), "a circle's radius is a length, not a percentage");
}

void test_conic_geometry() {
    AuthoredGradient conic = gradient_of(MaterialFillKind::conic_gradient, {stop(red), stop(blue)});
    conic.angle_degrees = 90.0;
    require(resolved(conic, Size{}) ==
                MaterialFillLayer::conic(Point{0.5, 0.5}, 90.0, stops_of({{0.0, red}, {1.0, blue}})),
            "a conic gradient from 90 degrees");
    conic.repeating = true;
    conic.stops = {stop_at(red, 0.0), stop_at(blue, 0.25)};
    require(resolved(conic, Size{}) ==
                MaterialFillLayer::repeating_conic(Point{0.5, 0.5}, 90.0, 90.0, stops_of({{0.0, red}, {1.0, blue}})),
            "a repeating conic gradient's period is its stops' span");
}

// -- the compilation ---------------------------------------------------------------

constexpr std::array<PartDeclaration, 1> pad_parts{{
    {.name = "face", .role = PartRole::face, .composed_from = {}, .states = {}},
}};
const std::array<ControlAnatomy, 1> controls{{
    {.control_id = "button", .parts = pad_parts, .states = {}, .custom_states = {}, .item_systems = {}},
}};
const std::array<std::string_view, 1> resources{{"icon.check"}};

struct Compilation final {
    CompileOutcome outcome{CompileOutcome::refused};
    ThemeRuleSheet sheet{};
    std::vector<ThemeDiagnostic> diagnostics{};
};

Compilation compile(std::string_view text) {
    const PrimitiveRegistry& registry = builtin_primitive_registry();
    ThemeSource source{};
    source.file = "test.theme.css";
    source.text = text;
    ThemeCompileInput input{.source = source,
                            .registry = registry,
                            .vocabulary = SelectorVocabulary{},
                            .published_values = {},
                            .resources = resources,
                            .requires_engine = current_engine_version};
    input.vocabulary.controls = controls;
    ThemeCompiler compiler{};
    ThemeDiagnosticList diagnostics{};
    Compilation result{};
    result.outcome = compiler.compile(input, result.sheet, diagnostics);
    for (const ThemeDiagnostic& diagnostic : diagnostics.records()) result.diagnostics.push_back(diagnostic);
    if (result.outcome == CompileOutcome::compiled) {
        const CompiledThemeRules engine = CompiledThemeRules::compile(result.sheet, registry);
        require(engine.diagnostics().empty(), std::string("the engine accepts the sheet: ") + std::string(text));
    }
    return result;
}

const ThemeRule& only_rule(const Compilation& compilation, std::string_view sheet) {
    require(compilation.outcome == CompileOutcome::compiled && compilation.sheet.rules.size() == 1U,
            std::string("one rule: ") + std::string(sheet));
    static const ThemeRule none{};
    if (compilation.sheet.rules.size() != 1U) return none;
    return compilation.sheet.rules[0];
}

const ThemeRule& only_rule(Compilation&& compilation, std::string_view sheet) = delete;

const MaterialDeclaration* field_of(const ThemeRule& rule, MaterialField field) {
    for (const MaterialDeclaration& declaration : rule.materials) {
        if (declaration.field == field) return &declaration;
    }
    return nullptr;
}
const MaterialDeclaration* field_of(ThemeRule&& rule, MaterialField field) = delete;

bool selects_material(const ThemeRule& rule) {
    for (const RuleDeclaration& declaration : rule.declarations) {
        if (declaration == RuleDeclaration::primitive("material")) return true;
    }
    return false;
}

void test_lowering() {
    const std::string_view colour_sheet = "#pad::part(face) { background-color: red }";
    const Compilation colour = compile(colour_sheet);
    const ThemeRule& coloured = only_rule(colour, colour_sheet);
    const MaterialDeclaration* const background = field_of(coloured, MaterialField::background_colour);
    require(background != nullptr && (*background).colour == red, "background-color is the colour field");
    require(selects_material(coloured) && coloured.materials.size() == 1U,
            "a rule of fields alone selects the material primitive (C7)");

    const std::string_view named_sheet = "#pad::part(face) { primitive: sheen; background-color: red }";
    const Compilation named = compile(named_sheet);
    require(!selects_material(only_rule(named, named_sheet)), "a rule naming a primitive keeps it");

    const std::string_view layers_sheet =
        "#pad::part(face) { background-image: linear-gradient(red, blue), radial-gradient(circle, red, blue) }";
    const Compilation layers = compile(layers_sheet);
    const MaterialDeclaration* const images = field_of(only_rule(layers, layers_sheet), MaterialField::fills);
    require(images != nullptr && (*images).fills.size() == 2U && (*images).gradients.size() == 2U,
            "two layers, one of them read per box");
    if (images != nullptr && (*images).gradients.size() == 2U) {
        require((*images).gradients[0].kind == MaterialFillKind::radial_gradient && (*images).gradients[0].circle,
                "the layers are bottom first: CSS's last layer is the circle");
        require((*images).gradients[1].kind == MaterialFillKind::solid &&
                    (*images).fills[1] ==
                        MaterialFillLayer::linear_css_angle(180.0, stops_of({{0.0, red}, {1.0, blue}})),
                "a fixed layer is resolved when the sheet compiles");
    }

    const std::string_view shorthand_sheet = "#pad::part(face) { background: linear-gradient(red, blue) green }";
    const Compilation shorthand = compile(shorthand_sheet);
    const ThemeRule& short_rule = only_rule(shorthand, shorthand_sheet);
    const MaterialDeclaration* const short_colour = field_of(short_rule, MaterialField::background_colour);
    require(short_colour != nullptr && (*short_colour).colour == green && field_of(short_rule, MaterialField::fills),
            "background sets the colour and the images");
    const std::string_view none_sheet = "#pad::part(face) { background: none }";
    const Compilation none = compile(none_sheet);
    const MaterialDeclaration* const no_colour = field_of(only_rule(none, none_sheet), MaterialField::background_colour);
    require(no_colour != nullptr && (*no_colour).colour.alpha == 0U, "background: none is transparent");

    const std::string_view later_sheet = "#pad::part(face) { background: red; background-color: blue }";
    const Compilation later = compile(later_sheet);
    const MaterialDeclaration* const later_colour =
        field_of(only_rule(later, later_sheet), MaterialField::background_colour);
    require(later_colour != nullptr && (*later_colour).colour == blue, "a later longhand replaces the shorthand's");
    const std::string_view earlier_sheet = "#pad::part(face) { background-color: blue; background: red }";
    const Compilation earlier = compile(earlier_sheet);
    const MaterialDeclaration* const earlier_colour =
        field_of(only_rule(earlier, earlier_sheet), MaterialField::background_colour);
    require(earlier_colour != nullptr && (*earlier_colour).colour == red, "and a later shorthand the longhand's");

    const std::string_view blend_sheet = "#pad::part(face) { background-blend-mode: multiply, screen }";
    const Compilation blend = compile(blend_sheet);
    const MaterialDeclaration* const modes = field_of(only_rule(blend, blend_sheet), MaterialField::fill_blend_modes);
    require(modes != nullptr &&
                (*modes).blend_modes == (std::vector<MaterialBlendMode>{MaterialBlendMode::multiply,
                                                                        MaterialBlendMode::screen}),
            "background-blend-mode keeps CSS's order");

    const std::string_view initial_sheet = "#pad::part(face) { background: initial }";
    const Compilation initial = compile(initial_sheet);
    const ThemeRule& reset = only_rule(initial, initial_sheet);
    const MaterialDeclaration* const reset_colour = field_of(reset, MaterialField::background_colour);
    const MaterialDeclaration* const reset_images = field_of(reset, MaterialField::fills);
    require(reset_colour != nullptr && (*reset_colour).initial && reset_images != nullptr && (*reset_images).initial,
            "initial resets each field the shorthand sets");
}

void test_border_lowering() {
    const std::string_view border_sheet = "#pad::part(face) { border: 2px solid red }";
    const Compilation border = compile(border_sheet);
    const ThemeRule& bordered = only_rule(border, border_sheet);
    require(bordered.materials.size() == 12U, "border sets the twelve longhands");
    const MaterialDeclaration* const left_colour = field_of(bordered, MaterialField::border_left_colour);
    const MaterialDeclaration* const top_width = field_of(bordered, MaterialField::border_top_width);
    require(left_colour != nullptr && (*left_colour).colour == red && top_width != nullptr &&
                (*top_width).number == 2.0,
            "each edge takes the width and the colour");

    const std::string_view style_sheet = "#pad::part(face) { border: dashed }";
    const Compilation style = compile(style_sheet);
    const MaterialDeclaration* const omitted = field_of(only_rule(style, style_sheet), MaterialField::border_top_width);
    const MaterialDeclaration* const dash =
        field_of(only_rule(style, style_sheet), MaterialField::border_bottom_style);
    require(omitted != nullptr && (*omitted).initial && dash != nullptr &&
                (*dash).border_style == MaterialBorderStyle::dashed,
            "a part the shorthand omits keeps the floor's");

    const std::string_view colour_sheet = "#pad::part(face) { border-color: red green blue }";
    const Compilation colours = compile(colour_sheet);
    const ThemeRule& coloured = only_rule(colours, colour_sheet);
    const MaterialDeclaration* const right = field_of(coloured, MaterialField::border_right_colour);
    const MaterialDeclaration* const left = field_of(coloured, MaterialField::border_left_colour);
    const MaterialDeclaration* const bottom = field_of(coloured, MaterialField::border_bottom_colour);
    require(right != nullptr && left != nullptr && bottom != nullptr && (*right).colour == green &&
                (*left).colour == green && (*bottom).colour == blue,
            "three values: top, the sides, bottom");

    const std::string_view none_sheet = "#pad::part(face) { border-left-style: none; border-right-width: thick }";
    const Compilation none = compile(none_sheet);
    const MaterialDeclaration* const no_line = field_of(only_rule(none, none_sheet), MaterialField::border_left_style);
    const MaterialDeclaration* const thick = field_of(only_rule(none, none_sheet), MaterialField::border_right_width);
    require(no_line != nullptr && !(*no_line).border_style && thick != nullptr && (*thick).number == 5.0,
            "none is no line; thick is 5px");

    const std::string_view radius_sheet = "#pad::part(face) { border-radius: 4px 50%; border-top-left-radius: 3px }";
    const Compilation radius = compile(radius_sheet);
    const ThemeRule& rounded = only_rule(radius, radius_sheet);
    const MaterialDeclaration* const top_left = field_of(rounded, MaterialField::corner_top_left);
    const MaterialDeclaration* const top_right = field_of(rounded, MaterialField::corner_top_right);
    const MaterialDeclaration* const bottom_right = field_of(rounded, MaterialField::corner_bottom_right);
    require(top_left != nullptr && (*top_left).number == 3.0 && !(*top_left).relative && top_right != nullptr &&
                (*top_right).relative && (*top_right).number == 0.5 && bottom_right != nullptr &&
                (*bottom_right).number == 4.0,
            "border-radius sets the corners, a percentage relative, and a later corner replaces its own");

    const std::string_view shadow_sheet = "#pad::part(face) { box-shadow: 0 2px 4px red, inset 1px 1px blue }";
    const Compilation shadow = compile(shadow_sheet);
    const MaterialDeclaration* const shadows = field_of(only_rule(shadow, shadow_sheet), MaterialField::shadows);
    require(shadows != nullptr && (*shadows).shadows.size() == 2U, "two shadows");
    if (shadows != nullptr && (*shadows).shadows.size() == 2U) {
        require((*shadows).shadows[0].inset && (*shadows).shadows[0].color == blue &&
                    (*shadows).shadows[1].blur_radius == 4.0 && (*shadows).shadows[1].offset == (Point{0.0, 2.0}),
                "CSS's first shadow is on top: the list is bottom first");
    }

    const std::string_view reset_sheet = "#pad::part(face) { border: initial }";
    const Compilation reset = compile(reset_sheet);
    const ThemeRule& reset_rule = only_rule(reset, reset_sheet);
    bool all_initial = reset_rule.materials.size() == 12U;
    for (const MaterialDeclaration& declaration : reset_rule.materials) all_initial = all_initial && declaration.initial;
    require(all_initial, "border: initial resets the twelve longhands");
}

struct Refusal final {
    std::string_view sheet{};
    DiagnosticCode code{};
};

void test_refusals() {
    const std::array<Refusal, 28> refusals{{
        {"#a { background-image: "
         "linear-gradient(red, blue), linear-gradient(red, blue), linear-gradient(red, blue), "
         "linear-gradient(red, blue), linear-gradient(red, blue), linear-gradient(red, blue), "
         "linear-gradient(red, blue), linear-gradient(red, blue), linear-gradient(red, blue) }",
         DiagnosticCode::too_many_layers},
        {"#a { background-image: linear-gradient(red, 40%, blue) }", DiagnosticCode::value_form_not_admitted},
        {"#a { background-image: url(\"resource:icon.check\") }", DiagnosticCode::not_yet_built},
        {"#a { background: red center }", DiagnosticCode::not_yet_built},
        {"#a { background: red, linear-gradient(red, blue) }", DiagnosticCode::value_syntax},
        {"#a { background-image: none, linear-gradient(red, blue) }", DiagnosticCode::value_form_not_admitted},
        {"#a { background-blend-mode: overlay }", DiagnosticCode::value_form_not_admitted},
        {"#a { background-blend-mode: sideways }", DiagnosticCode::keyword_unknown},
        {"#a { background-color: currentcolor }", DiagnosticCode::not_yet_built},
        {"#a { background-color: 3px }", DiagnosticCode::value_wrong_kind},
        {"#a { background-color: inherit }", DiagnosticCode::value_wrong_kind},
        {"#a { background-image: radial-gradient(circle 10%, red, blue) }", DiagnosticCode::value_wrong_kind},
        {"#a { background-image: radial-gradient(-4px 4px, red, blue) }", DiagnosticCode::value_out_of_range},
        {"#a { background-image: linear-gradient(to center, red, blue) }", DiagnosticCode::value_syntax},
        {"#a { background-image: radial-gradient(at left 10px top, red, blue) }",
         DiagnosticCode::value_form_not_admitted},
        {"#a { background-image: linear-gradient(red 1em, blue) }", DiagnosticCode::not_yet_built},
        {"#a { border-style: double }", DiagnosticCode::not_yet_built},
        {"#a { border-style: inset }", DiagnosticCode::value_form_not_admitted},
        {"#a { border-style: wavy }", DiagnosticCode::keyword_unknown},
        {"#a { border-radius: 4px / 2px }", DiagnosticCode::value_form_not_admitted},
        {"#a { border-top-left-radius: 4px 2px }", DiagnosticCode::value_form_not_admitted},
        {"#a { border-width: -1px }", DiagnosticCode::value_out_of_range},
        {"#a { border-width: 1px 1px 1px 1px 1px }", DiagnosticCode::value_syntax},
        {"#a { border: 1px 2px }", DiagnosticCode::value_syntax},
        {"#a { box-shadow: 1px }", DiagnosticCode::value_syntax},
        {"#a { box-shadow: 1px 1px }", DiagnosticCode::not_yet_built},
        {"#a { box-shadow: 1px 1px -2px red }", DiagnosticCode::value_out_of_range},
        {"#a { box-shadow: 1px 1px red, 1px 1px red, 1px 1px red, 1px 1px red, 1px 1px red }",
         DiagnosticCode::too_many_layers},
    }};
    for (const Refusal& refusal : refusals) {
        const Compilation compiled = compile(refusal.sheet);
        const bool refused = compiled.outcome == CompileOutcome::refused && !compiled.diagnostics.empty() &&
                             compiled.diagnostics.back().code == refusal.code;
        if (!refused && !compiled.diagnostics.empty()) {
            std::cerr << "  got " << diagnostic_code_name(compiled.diagnostics.back().code) << '\n';
        }
        require(refused, std::string("refused: ") + std::string(refusal.sheet));
    }
    const Compilation unknown = compile("#a { background-image: felt(red) }");
    require(unknown.outcome == CompileOutcome::compiled && unknown.sheet.rules.empty() &&
                unknown.diagnostics.size() == 1U && unknown.diagnostics[0].code == DiagnosticCode::unknown_function,
            "an unknown function drops the declaration with a warning");
}

// -- the gate: a sheet and C++ bake the same pixels ------------------------------------

class RasterWatch final : public Painter {
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
    void draw_part_raster(const std::shared_ptr<const PartRaster>& raster, Point, double) override { last = raster; }
    std::shared_ptr<const PartRaster> last{};
};

class Pad final : public Control {
public:
    explicit Pad(StableId stable_id) : Control(std::move(stable_id)) {}
    void on_paint(Painter& painter, Rect) override {
        PartPaint part{};
        part.role = PartRole::face;
        part.family = ControlVisualRole::button;
        part.bounds = Rect{20.0, 20.0, 40.0, 24.0};
        part.key = PartKey{"face", {}, {}};
        paint_part(painter, *this, part);
    }
};

// The face's pixels, drawn with `rules` over the button recipes' material
// replaced by `recipe`: for a sheet the floor its fields assemble over, for
// C++ the material the fields stand for.
std::uint64_t face_pixels(const ThemeRuleSheet& rules, const SurfaceMaterial& recipe) {
    std::shared_ptr<Panel> root = make_control<Panel>(StableId("root"));
    std::shared_ptr<Pad> pad = make_control<Pad>(StableId("pad"));
    (*pad).set_requested_bounds(Rect{0.0, 0.0, 100.0, 84.0});
    (*root).add_child(pad);
    Window window(root, Size{140.0, 124.0});
    const std::unique_ptr<ThemeDefinition> held = test_support::professional_definition();
    ThemeDefinition& definition = *held;
    definition.id = "theme-material-css-gate";
    ControlRoleRecipes& button = definition.roles[static_cast<std::size_t>(ControlVisualRole::button)];
    for (ControlVisualRecipe& written : button.ordinary) written.material = recipe;
    for (ControlVisualRecipe& written : button.selected) written.material = recipe;
    definition.rules = rules;
    window.set_theme(Theme::create(definition));
    require(window.theme().engine().rules().diagnostics().empty(), "the gate theme compiles cleanly");
    window.perform_layout();
    RasterWatch painter{};
    window.paint(painter);
    require(painter.last != nullptr, "the face is drawn by the material primitive");
    if (painter.last == nullptr) return 0U;
    std::uint64_t hash = 14695981039346656037ULL;
    for (const std::uint8_t byte : (*painter.last).pixels()) {
        hash = hash ^ byte;
        hash = hash * 1099511628211ULL;
    }
    return hash;
}

ThemeRuleSheet written_rules() {
    ThemeRuleSheet sheet{};
    ThemeRule rule{};
    rule.label = "written";
    rule.level = CascadeLevel::stable_id_rule;
    rule.selector.stable_id = "pad";
    rule.selector.part = "face";
    rule.declarations.push_back(RuleDeclaration::primitive("material"));
    sheet.rules.push_back(rule);
    return sheet;
}

struct GateRow final {
    std::string_view declarations{};
    SurfaceMaterial material{};
};

MaterialBorder border_of(Color colour, double width, MaterialBorderStyle style) {
    MaterialBorder border{};
    border.color = colour;
    border.width = width;
    border.style = style;
    return border;
}

MaterialShadow shadow_of(Point offset, double blur, double spread, Color colour, bool inset) {
    MaterialShadow shadow{};
    shadow.offset = offset;
    shadow.blur_radius = blur;
    shadow.spread = spread;
    shadow.color = colour;
    shadow.inset = inset;
    return shadow;
}

// A row whose C++ material is the plain floor with `fills`.
GateRow filled(std::string_view declarations, std::vector<MaterialFillLayer> fills) {
    GateRow row{};
    row.declarations = declarations;
    row.material.fills = std::move(fills);
    return row;
}

std::vector<GateRow> gate_rows() {
    const std::vector<GradientStop> red_blue = stops_of({{0.0, red}, {1.0, blue}});
    const double diagonal = engine_arctangent2(24.0, 40.0) * (180.0 / pi);
    const double corner = 0.5 * 1.41421356237309504880168872420969808;
    std::vector<GateRow> rows{};
    rows.push_back(filled("background: green", {MaterialFillLayer::solid(green)}));
    rows.push_back(filled("background: linear-gradient(135deg, red, blue)",
                          {MaterialFillLayer::linear_css_angle(135.0, red_blue)}));
    rows.push_back(filled("background: linear-gradient(to top right, red, blue)",
                          {MaterialFillLayer::linear_css_angle(diagonal, red_blue)}));
    rows.push_back(filled("background: linear-gradient(red 25%, green, blue 75%)",
                          {MaterialFillLayer::linear_css_angle(
                              180.0, stops_of({{0.0, red}, {0.25, red}, {0.5, green}, {0.75, blue}, {1.0, blue}}))}));
    rows.push_back(filled("background: repeating-linear-gradient(0deg, red 0px, blue 6px)",
                          {MaterialFillLayer::repeating_linear(Point{20.0, 24.0}, Point{20.0, 18.0}, red_blue,
                                                               MaterialCoordinateSpace::logical)}));
    rows.push_back(filled("background: radial-gradient(red, blue)",
                          {MaterialFillLayer::radial(Point{0.5, 0.5}, Size{corner, corner}, red_blue)}));
    rows.push_back(filled("background: radial-gradient(circle closest-side at 25% 50%, red, blue)",
                          {MaterialFillLayer::radial(Point{10.0, 12.0}, Size{10.0, 10.0}, red_blue,
                                                     MaterialCoordinateSpace::logical)}));
    rows.push_back(filled("background: repeating-radial-gradient(closest-side, red, blue 50%)",
                          {MaterialFillLayer::repeating_radial(Point{0.5, 0.5}, Size{0.25, 0.25}, red_blue)}));
    rows.push_back(filled("background: conic-gradient(from 90deg, red, blue)",
                          {MaterialFillLayer::conic(Point{0.5, 0.5}, 90.0, red_blue)}));
    rows.push_back(filled("background: repeating-conic-gradient(red 0deg, blue 90deg)",
                          {MaterialFillLayer::repeating_conic(Point{0.5, 0.5}, 0.0, 90.0, red_blue)}));
    rows.push_back(filled("background: linear-gradient(red, blue) green",
                          {MaterialFillLayer::solid(green), MaterialFillLayer::linear_css_angle(180.0, red_blue)}));
    MaterialFillLayer multiplied = MaterialFillLayer::linear_css_angle(90.0, red_blue);
    multiplied.blend = MaterialBlendMode::multiply;
    rows.push_back(filled("background: linear-gradient(90deg, red, blue) green; background-blend-mode: multiply",
                          {MaterialFillLayer::solid(green), multiplied}));

    GateRow border{};
    border.declarations = "border: 2px solid red";
    border.material.border = border_of(red, 2.0, MaterialBorderStyle::solid);
    rows.push_back(border);
    GateRow dashed{};
    dashed.declarations = "border: 2px dashed red; border-radius: 6px";
    dashed.material.border = border_of(red, 2.0, MaterialBorderStyle::dashed);
    dashed.material.corner_radius = 6.0;
    rows.push_back(dashed);
    GateRow edges{};
    edges.declarations = "border-top: 3px solid blue; border-bottom: 1px solid red";
    edges.material.border_edges.top = border_of(blue, 3.0, MaterialBorderStyle::solid);
    edges.material.border_edges.bottom = border_of(red, 1.0, MaterialBorderStyle::solid);
    rows.push_back(edges);
    GateRow sides{};
    sides.declarations = "border-style: solid; border-width: 1px 2px; border-color: red blue";
    sides.material.border_edges.top = border_of(red, 1.0, MaterialBorderStyle::solid);
    sides.material.border_edges.right = border_of(blue, 2.0, MaterialBorderStyle::solid);
    sides.material.border_edges.bottom = border_of(red, 1.0, MaterialBorderStyle::solid);
    sides.material.border_edges.left = border_of(blue, 2.0, MaterialBorderStyle::solid);
    rows.push_back(sides);
    GateRow radii{};
    radii.declarations = "border-radius: 4px 8px";
    radii.material.corner_radius = 4.0;
    radii.material.corner_radii = MaterialCornerRadii{4.0, 8.0, 4.0, 8.0};
    rows.push_back(radii);
    GateRow relative{};
    relative.declarations = "border-radius: 50%";
    relative.material.corner_radius = 12.0;
    rows.push_back(relative);
    GateRow shadows{};
    shadows.declarations = "box-shadow: 0 2px 4px rgb(0 0 0 / 40%), inset 0 1px 0 white";
    shadows.material.shadows = {shadow_of(Point{0.0, 1.0}, 0.0, 0.0, Color::rgba(255, 255, 255), true),
                                shadow_of(Point{0.0, 2.0}, 4.0, 0.0, Color::rgba(0, 0, 0, 102), false)};
    rows.push_back(shadows);
    return rows;
}

void test_gate() {
    const SurfaceMaterial plain{};
    const std::uint64_t floor_pixels = face_pixels(written_rules(), plain);
    const std::vector<GateRow> rows = gate_rows();
    for (const GateRow& row : rows) {
        const std::string sheet = "#pad::part(face) { " + std::string(row.declarations) + " }";
        const Compilation compiled = compile(sheet);
        require(compiled.outcome == CompileOutcome::compiled, "the row compiles: " + sheet);
        require(valid_surface_material(row.material), "the C++ material is valid: " + sheet);
        const std::uint64_t from_sheet = face_pixels(compiled.sheet, plain);
        const std::uint64_t from_cpp = face_pixels(written_rules(), row.material);
        require(from_sheet == from_cpp, "the sheet bakes the pixels of the C++ material: " + sheet);
        require(from_cpp != floor_pixels, "and the row changes the face: " + sheet);
    }
}

} // namespace

int main() {
    test_placement();
    test_linear_geometry();
    test_radial_geometry();
    test_conic_geometry();
    test_lowering();
    test_border_lowering();
    test_refusals();
    test_gate();
    if (failures != 0) {
        std::cerr << failures << " failure(s)\n";
        return 1;
    }
    return 0;
}
