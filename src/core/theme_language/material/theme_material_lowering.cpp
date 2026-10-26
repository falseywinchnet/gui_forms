#include "theme_material_lowering.hpp"

#include "../source_text/theme_source_text.hpp"

#include "gui_forms/engine/authored_gradient/authored_gradient.hpp"
#include "gui_forms/surface_material/painting/surface_material_painting.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace gui_forms::theme_language {

namespace {

// -- the names --------------------------------------------------------------------

struct NamedMaterialProperty final {
    std::string_view name{};
    MaterialProperty property{MaterialProperty::background};
};

// Sorted by name.
constexpr std::array<NamedMaterialProperty, 30> material_properties{{
    {.name = "background", .property = MaterialProperty::background},
    {.name = "background-blend-mode", .property = MaterialProperty::background_blend_mode},
    {.name = "background-color", .property = MaterialProperty::background_color},
    {.name = "background-image", .property = MaterialProperty::background_image},
    {.name = "border", .property = MaterialProperty::border},
    {.name = "border-bottom", .property = MaterialProperty::border_bottom},
    {.name = "border-bottom-color", .property = MaterialProperty::border_bottom_color},
    {.name = "border-bottom-left-radius", .property = MaterialProperty::border_bottom_left_radius},
    {.name = "border-bottom-right-radius", .property = MaterialProperty::border_bottom_right_radius},
    {.name = "border-bottom-style", .property = MaterialProperty::border_bottom_style},
    {.name = "border-bottom-width", .property = MaterialProperty::border_bottom_width},
    {.name = "border-color", .property = MaterialProperty::border_color},
    {.name = "border-left", .property = MaterialProperty::border_left},
    {.name = "border-left-color", .property = MaterialProperty::border_left_color},
    {.name = "border-left-style", .property = MaterialProperty::border_left_style},
    {.name = "border-left-width", .property = MaterialProperty::border_left_width},
    {.name = "border-radius", .property = MaterialProperty::border_radius},
    {.name = "border-right", .property = MaterialProperty::border_right},
    {.name = "border-right-color", .property = MaterialProperty::border_right_color},
    {.name = "border-right-style", .property = MaterialProperty::border_right_style},
    {.name = "border-right-width", .property = MaterialProperty::border_right_width},
    {.name = "border-style", .property = MaterialProperty::border_style},
    {.name = "border-top", .property = MaterialProperty::border_top},
    {.name = "border-top-color", .property = MaterialProperty::border_top_color},
    {.name = "border-top-left-radius", .property = MaterialProperty::border_top_left_radius},
    {.name = "border-top-right-radius", .property = MaterialProperty::border_top_right_radius},
    {.name = "border-top-style", .property = MaterialProperty::border_top_style},
    {.name = "border-top-width", .property = MaterialProperty::border_top_width},
    {.name = "border-width", .property = MaterialProperty::border_width},
    {.name = "box-shadow", .property = MaterialProperty::box_shadow},
}};

// CSS blend modes the engine does not composite (CSS_COVERAGE section 7).
// Sorted.
constexpr std::array<std::string_view, 15> blend_modes_not_admitted{
    "color", "color-burn", "color-dodge", "darken", "difference", "exclusion", "hard-light", "hue",
    "lighten", "luminosity", "overlay", "plus-darker", "plus-lighter", "saturation", "soft-light",
};
static_assert(std::is_sorted(blend_modes_not_admitted.begin(), blend_modes_not_admitted.end()),
              "binary_search needs the table sorted");

struct NamedGradient final {
    std::string_view name{};
    MaterialFillKind kind{MaterialFillKind::linear_gradient};
    bool repeating{};
};

constexpr std::array<NamedGradient, 6> gradient_functions{{
    {.name = "linear-gradient", .kind = MaterialFillKind::linear_gradient, .repeating = false},
    {.name = "radial-gradient", .kind = MaterialFillKind::radial_gradient, .repeating = false},
    {.name = "conic-gradient", .kind = MaterialFillKind::conic_gradient, .repeating = false},
    {.name = "repeating-linear-gradient", .kind = MaterialFillKind::linear_gradient, .repeating = true},
    {.name = "repeating-radial-gradient", .kind = MaterialFillKind::radial_gradient, .repeating = true},
    {.name = "repeating-conic-gradient", .kind = MaterialFillKind::conic_gradient, .repeating = true},
}};

[[nodiscard]] const NamedGradient* gradient_function(std::string_view name) noexcept {
    for (const NamedGradient& gradient : gradient_functions) {
        if (equals_ascii_folded(name, gradient.name)) return &gradient;
    }
    return nullptr;
}

// A position keyword (CSS <position>), or `none` for a length-percentage.
enum class PositionWord : std::uint8_t { none, left, center, right, top, bottom };

[[nodiscard]] bool across_word(PositionWord word) noexcept {
    const bool across = word == PositionWord::left || word == PositionWord::center || word == PositionWord::right;
    return across;
}

[[nodiscard]] bool down_word(PositionWord word) noexcept {
    const bool down = word == PositionWord::top || word == PositionWord::center || word == PositionWord::bottom;
    return down;
}

[[nodiscard]] BoxLength word_length(PositionWord word) noexcept {
    switch (word) {
    case PositionWord::left:
    case PositionWord::top: return BoxLength{0.0, 0.0};
    case PositionWord::right:
    case PositionWord::bottom: return BoxLength{1.0, 0.0};
    case PositionWord::center:
    case PositionWord::none: break;
    }
    return BoxLength{0.5, 0.0};
}

// The first of an edge's three fields, its style; its width and colour
// follow.
[[nodiscard]] MaterialField edge_style_field(std::size_t edge) noexcept {
    const std::size_t first = static_cast<std::size_t>(MaterialField::border_top_style) + edge * 3U;
    return static_cast<MaterialField>(first);
}

[[nodiscard]] MaterialField edge_field(std::size_t edge, std::size_t part) noexcept {
    const std::size_t field = static_cast<std::size_t>(edge_style_field(edge)) + part;
    return static_cast<MaterialField>(field);
}

[[nodiscard]] MaterialField corner_field(std::size_t corner) noexcept {
    const std::size_t field = static_cast<std::size_t>(MaterialField::corner_top_left) + corner;
    return static_cast<MaterialField>(field);
}

// A field that keeps the floor's value: what a shorthand sets for a part it
// omits (the language's `initial`, the built-in recipe's value).
[[nodiscard]] MaterialDeclaration initial_field(MaterialField field) {
    MaterialDeclaration declaration{};
    declaration.field = field;
    declaration.initial = true;
    return declaration;
}

// The edges or corners CSS's one to four values set, in CSS's order: one
// value sets all four; two, the first and third and then the second and
// fourth; three, the first, the second and fourth, and the third.
constexpr std::array<std::array<std::size_t, 4>, 4> four_sides{{
    {0U, 0U, 0U, 0U},
    {0U, 1U, 0U, 1U},
    {0U, 1U, 2U, 1U},
    {0U, 1U, 2U, 3U},
}};

// Components [first, end) of a value, at one nesting level.
struct ComponentRange final {
    std::size_t first{};
    std::size_t end{};
};

// One lowering: reads `value_` and writes `lowering_`. Each reading
// function returns false once it has recorded a problem.
class MaterialReader final {
public:
    MaterialReader(const ThemeValue& value, std::uint32_t start, MaterialLowering& lowering) noexcept
        : value_(value), start_(start), lowering_(lowering) {}

    void lower(MaterialProperty property);

private:
    bool fail(DiagnosticCode code, std::uint32_t token, std::string detail);
    bool drop_unknown_functions();
    [[nodiscard]] std::size_t next(std::size_t index) const noexcept;
    [[nodiscard]] std::size_t count(ComponentRange range) const noexcept;
    void split(ComponentRange range, std::vector<ComponentRange>& parts) const;
    [[nodiscard]] bool keyword(std::size_t index, std::string_view lower_name) const noexcept;
    [[nodiscard]] PositionWord position_word(std::size_t index) const noexcept;

    bool read_colour(std::size_t index, Color& colour);
    bool read_length(std::size_t index, bool percentages, bool negatives, BoxLength& length);
    bool read_angle(std::size_t index, double& degrees);
    bool read_turn_fraction(std::size_t index, double& fraction);
    bool read_position(ComponentRange range, BoxLength& across, BoxLength& down);
    bool read_stops(std::span<const ComponentRange> arguments, MaterialFillKind kind,
                    std::vector<AuthoredGradientStop>& stops);
    bool read_linear_configuration(ComponentRange range, AuthoredGradient& gradient, bool& configured);
    bool read_radial_configuration(ComponentRange range, AuthoredGradient& gradient, bool& configured);
    bool read_conic_configuration(ComponentRange range, AuthoredGradient& gradient, bool& configured);
    bool read_gradient(std::size_t index, AuthoredGradient& gradient);
    bool read_image(std::size_t index, AuthoredGradient& gradient);
    bool push_fills(std::span<const AuthoredGradient> written);
    bool read_style(std::size_t index, std::optional<MaterialBorderStyle>& style);
    bool read_width(std::size_t index, double& width);
    bool read_radius(std::size_t index, MaterialDeclaration& corner);
    bool read_shadow(ComponentRange range, MaterialShadow& shadow);
    [[nodiscard]] bool style_word(std::size_t index) const noexcept;
    [[nodiscard]] bool width_word(std::size_t index) const noexcept;

    bool lower_background_colour();
    bool lower_background_image();
    bool lower_blend_modes();
    bool lower_background();
    bool lower_edges(std::size_t first_edge, std::size_t edge_count);
    bool lower_sides(std::size_t part);
    bool lower_edge_field(std::size_t edge, std::size_t part);
    bool lower_radii();
    bool lower_corner(std::size_t corner);
    bool lower_shadows();

    const ThemeValue& value_;
    std::uint32_t start_{};
    MaterialLowering& lowering_;
};

bool MaterialReader::fail(DiagnosticCode code, std::uint32_t token, std::string detail) {
    const bool warning = diagnostic_severity(code) == DiagnosticSeverity::warning;
    lowering_.outcome = warning ? MaterialLoweringOutcome::dropped : MaterialLoweringOutcome::refused;
    lowering_.declarations.clear();
    lowering_.problem.code = code;
    lowering_.problem.token = token;
    lowering_.problem.detail = std::move(detail);
    return false;
}

// A function the material properties do not know is dropped with a warning
// (B2: an unknown function), as it is anywhere in a sheet.
bool MaterialReader::drop_unknown_functions() {
    for (const ValueComponent& component : value_.components) {
        if (component.kind != ValueComponentKind::function || gradient_function(component.text) != nullptr) continue;
        return fail(DiagnosticCode::unknown_function, component.token, clipped_theme_detail(component.text));
    }
    return true;
}

std::size_t MaterialReader::next(std::size_t index) const noexcept {
    const std::size_t after = value_.components[index].subtree_end;
    return after;
}

std::size_t MaterialReader::count(ComponentRange range) const noexcept {
    std::size_t components = 0U;
    for (std::size_t index = range.first; index < range.end; index = next(index)) components = components + 1U;
    return components;
}

// The comma-separated parts of `range`, into `parts`, which is cleared
// first. An empty part is kept, for its reader to refuse.
void MaterialReader::split(ComponentRange range, std::vector<ComponentRange>& parts) const {
    parts.clear();
    std::size_t first = range.first;
    for (std::size_t index = range.first; index < range.end; index = next(index)) {
        if (value_.components[index].kind != ValueComponentKind::comma) continue;
        parts.push_back(ComponentRange{first, index});
        first = index + 1U;
    }
    parts.push_back(ComponentRange{first, range.end});
}

bool MaterialReader::keyword(std::size_t index, std::string_view lower_name) const noexcept {
    const ValueComponent& component = value_.components[index];
    const bool matches = component.kind == ValueComponentKind::keyword && equals_ascii_folded(component.text, lower_name);
    return matches;
}

PositionWord MaterialReader::position_word(std::size_t index) const noexcept {
    if (keyword(index, "left")) return PositionWord::left;
    if (keyword(index, "center")) return PositionWord::center;
    if (keyword(index, "right")) return PositionWord::right;
    if (keyword(index, "top")) return PositionWord::top;
    if (keyword(index, "bottom")) return PositionWord::bottom;
    return PositionWord::none;
}

// -- values -----------------------------------------------------------------------

bool MaterialReader::read_colour(std::size_t index, Color& colour) {
    const ValueComponent& component = value_.components[index];
    if (component.kind == ValueComponentKind::colour) {
        colour = component.colour;
        return true;
    }
    if (component.kind == ValueComponentKind::expression) {
        return fail(DiagnosticCode::value_form_not_admitted, component.token, "an expression that reads inputs");
    }
    if (component.kind == ValueComponentKind::keyword) {
        // P8 builds the colour roles, and currentcolor with them.
        if (equals_ascii_folded(component.text, "currentcolor")) {
            return fail(DiagnosticCode::not_yet_built, component.token, "currentcolor");
        }
        const std::optional<Color> named = named_colour(component.text);
        if (named) {
            colour = *named;
            return true;
        }
    }
    return fail(DiagnosticCode::value_wrong_kind, component.token, "a colour");
}

// A <length> (px, or a unitless 0) or, with `percentages`, a <percentage>
// of the box or gradient; negative only with `negatives`.
bool MaterialReader::read_length(std::size_t index, bool percentages, bool negatives, BoxLength& length) {
    const ValueComponent& component = value_.components[index];
    const std::string expected = percentages ? "a length or a percentage" : "a length";
    if (component.kind == ValueComponentKind::expression) {
        return fail(DiagnosticCode::value_form_not_admitted, component.token, "an expression that reads inputs");
    }
    if (component.kind != ValueComponentKind::number) {
        return fail(DiagnosticCode::value_wrong_kind, component.token, expected);
    }
    if (component.unit == ValueUnit::em) return fail(DiagnosticCode::not_yet_built, component.token, "em");
    const bool zero = component.unit == ValueUnit::none && component.number == 0.0;
    const bool pixels = component.unit == ValueUnit::px;
    const bool percent = percentages && component.unit == ValueUnit::percent;
    if (!zero && !pixels && !percent) return fail(DiagnosticCode::value_wrong_kind, component.token, expected);
    if (!negatives && component.number < 0.0) {
        return fail(DiagnosticCode::value_out_of_range, component.token, clipped_theme_detail(component.text));
    }
    length = percent ? BoxLength{component.number / 100.0, 0.0} : BoxLength{0.0, component.number};
    return true;
}

// An <angle> (read in degrees), or a unitless 0.
bool MaterialReader::read_angle(std::size_t index, double& degrees) {
    const ValueComponent& component = value_.components[index];
    const bool number = component.kind == ValueComponentKind::number;
    const bool angle = number && component.unit == ValueUnit::degrees;
    const bool zero = number && component.unit == ValueUnit::none && component.number == 0.0;
    if (!angle && !zero) return fail(DiagnosticCode::value_wrong_kind, component.token, "an angle");
    degrees = component.number;
    return true;
}

// A conic stop's position: an angle or a percentage, as a fraction of a
// turn.
bool MaterialReader::read_turn_fraction(std::size_t index, double& fraction) {
    const ValueComponent& component = value_.components[index];
    if (component.kind == ValueComponentKind::number && component.unit == ValueUnit::percent) {
        fraction = component.number / 100.0;
        return true;
    }
    double degrees = 0.0;
    if (!read_angle(index, degrees)) {
        return fail(DiagnosticCode::value_wrong_kind, component.token, "an angle or a percentage");
    }
    fraction = degrees / 360.0;
    return true;
}

// CSS's <position> of one or two values; three or four are not admitted.
bool MaterialReader::read_position(ComponentRange range, BoxLength& across, BoxLength& down) {
    const std::size_t values = count(range);
    const std::uint32_t token = range.first < range.end ? value_.components[range.first].token : start_;
    if (values == 0U) return fail(DiagnosticCode::value_syntax, token, "a position");
    if (values > 2U) return fail(DiagnosticCode::value_form_not_admitted, token, "a position of three or four values");
    const std::size_t first = range.first;
    const PositionWord first_word = position_word(first);
    across = BoxLength{0.5, 0.0};
    down = BoxLength{0.5, 0.0};
    if (values == 1U) {
        if (first_word == PositionWord::top || first_word == PositionWord::bottom) {
            down = word_length(first_word);
            return true;
        }
        if (first_word != PositionWord::none) {
            across = word_length(first_word);
            return true;
        }
        return read_length(first, true, true, across);
    }
    const std::size_t second = next(first);
    const PositionWord second_word = position_word(second);
    const bool words = first_word != PositionWord::none && second_word != PositionWord::none;
    // Two keywords may come either way round: `top left` is `left top`.
    const bool swapped = words && (first_word == PositionWord::top || first_word == PositionWord::bottom ||
                                   second_word == PositionWord::left || second_word == PositionWord::right);
    const std::size_t across_index = swapped ? second : first;
    const std::size_t down_index = swapped ? first : second;
    const PositionWord across_value = swapped ? second_word : first_word;
    const PositionWord down_value = swapped ? first_word : second_word;
    if (across_value != PositionWord::none && !across_word(across_value)) {
        return fail(DiagnosticCode::value_syntax, value_.components[across_index].token, "a position");
    }
    if (down_value != PositionWord::none && !down_word(down_value)) {
        return fail(DiagnosticCode::value_syntax, value_.components[down_index].token, "a position");
    }
    if (across_value != PositionWord::none) {
        across = word_length(across_value);
    } else if (!read_length(across_index, true, true, across)) {
        return false;
    }
    if (down_value != PositionWord::none) {
        down = word_length(down_value);
        return true;
    }
    return read_length(down_index, true, true, down);
}

// The colour stops, one argument each: a colour and up to two positions,
// each position a stop of that colour.
bool MaterialReader::read_stops(std::span<const ComponentRange> arguments, MaterialFillKind kind,
                                std::vector<AuthoredGradientStop>& stops) {
    stops.clear();
    for (const ComponentRange& argument : arguments) {
        const std::size_t values = count(argument);
        if (values == 0U) return fail(DiagnosticCode::value_syntax, start_, "a colour stop");
        const ValueComponent& first = value_.components[argument.first];
        if (values == 1U && first.kind == ValueComponentKind::number) {
            return fail(DiagnosticCode::value_form_not_admitted, first.token, "a colour hint");
        }
        if (values > 3U) return fail(DiagnosticCode::value_syntax, first.token, "a colour and up to two positions");
        AuthoredGradientStop stop{};
        if (!read_colour(argument.first, stop.colour)) return false;
        if (values == 1U) {
            stops.push_back(stop);
            continue;
        }
        for (std::size_t index = next(argument.first); index < argument.end; index = next(index)) {
            stop.positioned = true;
            if (kind == MaterialFillKind::conic_gradient) {
                double fraction = 0.0;
                if (!read_turn_fraction(index, fraction)) return false;
                stop.position = BoxLength{fraction, 0.0};
            } else if (!read_length(index, true, true, stop.position)) {
                return false;
            }
            stops.push_back(stop);
        }
    }
    if (stops.size() > maximum_authored_gradient_stops) {
        return fail(DiagnosticCode::value_out_of_range, value_.components[arguments.front().first].token,
                    "at most " + std::to_string(maximum_authored_gradient_stops) + " colour stops");
    }
    return true;
}

// `<angle>` or `to <side-or-corner>`; `configured` is false for a first
// argument that is a colour stop.
bool MaterialReader::read_linear_configuration(ComponentRange range, AuthoredGradient& gradient, bool& configured) {
    configured = false;
    const std::size_t values = count(range);
    if (values == 0U) return true;
    const ValueComponent& first = value_.components[range.first];
    if (keyword(range.first, "to")) {
        configured = true;
        if (values < 2U || values > 3U) return fail(DiagnosticCode::value_syntax, first.token, "a side or a corner");
        const std::size_t side = next(range.first);
        const PositionWord one = position_word(side);
        const PositionWord other = values == 3U ? position_word(next(side)) : PositionWord::none;
        if (values == 2U) {
            if (one == PositionWord::top) gradient.angle_degrees = 0.0;
            if (one == PositionWord::right) gradient.angle_degrees = 90.0;
            if (one == PositionWord::bottom) gradient.angle_degrees = 180.0;
            if (one == PositionWord::left) gradient.angle_degrees = 270.0;
            if (one == PositionWord::none || one == PositionWord::center) {
                return fail(DiagnosticCode::value_syntax, value_.components[side].token, "a side or a corner");
            }
            return true;
        }
        const bool one_down = one == PositionWord::top || one == PositionWord::bottom;
        const PositionWord vertical = one_down ? one : other;
        const PositionWord horizontal = one_down ? other : one;
        const bool corner = (vertical == PositionWord::top || vertical == PositionWord::bottom) &&
                            (horizontal == PositionWord::left || horizontal == PositionWord::right);
        if (!corner) return fail(DiagnosticCode::value_syntax, value_.components[side].token, "a side or a corner");
        const bool top = vertical == PositionWord::top;
        const bool left = horizontal == PositionWord::left;
        if (top && left) gradient.corner = GradientCorner::top_left;
        if (top && !left) gradient.corner = GradientCorner::top_right;
        if (!top && !left) gradient.corner = GradientCorner::bottom_right;
        if (!top && left) gradient.corner = GradientCorner::bottom_left;
        return true;
    }
    const bool angle = first.kind == ValueComponentKind::number &&
                       (first.unit == ValueUnit::degrees || (first.unit == ValueUnit::none && first.number == 0.0));
    if (!angle) return true;
    configured = true;
    if (values != 1U) return fail(DiagnosticCode::value_syntax, value_.components[next(range.first)].token, "a comma");
    return read_angle(range.first, gradient.angle_degrees);
}

// `[<ending-shape> || <size>]? [at <position>]?`; `configured` is false
// for a first argument that is a colour stop.
bool MaterialReader::read_radial_configuration(ComponentRange range, AuthoredGradient& gradient, bool& configured) {
    configured = false;
    if (range.first >= range.end) return true;
    const ValueComponent& first = value_.components[range.first];
    const bool word = keyword(range.first, "circle") || keyword(range.first, "ellipse") ||
                      keyword(range.first, "closest-side") || keyword(range.first, "farthest-side") ||
                      keyword(range.first, "closest-corner") || keyword(range.first, "farthest-corner") ||
                      keyword(range.first, "at");
    if (!word && first.kind != ValueComponentKind::number) return true;
    configured = true;
    bool shape = false;
    bool sized = false;
    std::array<std::size_t, 2> lengths{};
    std::size_t length_count = 0U;
    std::size_t index = range.first;
    for (; index < range.end && !keyword(index, "at"); index = next(index)) {
        const ValueComponent& component = value_.components[index];
        if (keyword(index, "circle") || keyword(index, "ellipse")) {
            if (shape) return fail(DiagnosticCode::value_syntax, component.token, "one shape");
            shape = true;
            gradient.circle = keyword(index, "circle");
            continue;
        }
        std::optional<RadialExtent> extent{};
        if (keyword(index, "closest-side")) extent = RadialExtent::closest_side;
        if (keyword(index, "farthest-side")) extent = RadialExtent::farthest_side;
        if (keyword(index, "closest-corner")) extent = RadialExtent::closest_corner;
        if (keyword(index, "farthest-corner")) extent = RadialExtent::farthest_corner;
        if (extent) {
            if (sized) return fail(DiagnosticCode::value_syntax, component.token, "one size");
            sized = true;
            gradient.extent = *extent;
            continue;
        }
        if (component.kind != ValueComponentKind::number || length_count == lengths.size() ||
            (sized && length_count == 0U)) {
            return fail(DiagnosticCode::value_syntax, component.token, "a shape, a size or `at`");
        }
        lengths[length_count] = index;
        length_count = length_count + 1U;
    }
    if (length_count > 0U) {
        if (sized) return fail(DiagnosticCode::value_syntax, value_.components[lengths[0]].token, "one size");
        gradient.extent = RadialExtent::lengths;
        if (!shape) gradient.circle = length_count == 1U;
        const bool one_for_circle = gradient.circle == (length_count == 1U);
        if (!one_for_circle) {
            const char* const expected = gradient.circle ? "one length for a circle" : "two radii for an ellipse";
            return fail(DiagnosticCode::value_syntax, value_.components[lengths[0]].token, expected);
        }
        if (!read_length(lengths[0], !gradient.circle, false, gradient.radius_x)) return false;
        if (length_count == 2U && !read_length(lengths[1], true, false, gradient.radius_y)) return false;
    }
    if (index >= range.end) return true;
    const ComponentRange position{next(index), range.end};
    return read_position(position, gradient.centre_x, gradient.centre_y);
}

// `[from <angle>]? [at <position>]?`; `configured` is false for a first
// argument that is a colour stop.
bool MaterialReader::read_conic_configuration(ComponentRange range, AuthoredGradient& gradient, bool& configured) {
    configured = false;
    if (range.first >= range.end) return true;
    if (!keyword(range.first, "from") && !keyword(range.first, "at")) return true;
    configured = true;
    gradient.angle_degrees = 0.0;
    std::size_t index = range.first;
    if (keyword(index, "from")) {
        const std::size_t angle = next(index);
        if (angle >= range.end) return fail(DiagnosticCode::value_syntax, value_.components[index].token, "an angle");
        if (!read_angle(angle, gradient.angle_degrees)) return false;
        index = next(angle);
    }
    if (index >= range.end) return true;
    if (!keyword(index, "at")) return fail(DiagnosticCode::value_syntax, value_.components[index].token, "`at`");
    const ComponentRange position{next(index), range.end};
    return read_position(position, gradient.centre_x, gradient.centre_y);
}

bool MaterialReader::read_gradient(std::size_t index, AuthoredGradient& gradient) {
    const ValueComponent& function = value_.components[index];
    const NamedGradient& named = *gradient_function(function.text);
    gradient = AuthoredGradient{};
    gradient.kind = named.kind;
    gradient.repeating = named.repeating;
    std::vector<ComponentRange> arguments{};
    split(ComponentRange{index + 1U, function.subtree_end}, arguments);
    bool configured = false;
    bool read = false;
    switch (named.kind) {
    case MaterialFillKind::linear_gradient: read = read_linear_configuration(arguments[0], gradient, configured); break;
    case MaterialFillKind::radial_gradient: read = read_radial_configuration(arguments[0], gradient, configured); break;
    case MaterialFillKind::conic_gradient: read = read_conic_configuration(arguments[0], gradient, configured); break;
    case MaterialFillKind::solid:
    case MaterialFillKind::image:
    case MaterialFillKind::procedural: break;
    }
    if (!read) return false;
    const std::span<const ComponentRange> stop_arguments =
        std::span<const ComponentRange>(arguments).subspan(configured ? 1U : 0U);
    if (stop_arguments.empty()) return fail(DiagnosticCode::value_syntax, function.token, "a colour stop");
    if (!read_stops(stop_arguments, named.kind, gradient.stops)) return false;
    if (!valid_authored_gradient(gradient)) {
        return fail(DiagnosticCode::value_out_of_range, function.token, clipped_theme_detail(function.text));
    }
    return true;
}

bool MaterialReader::read_image(std::size_t index, AuthoredGradient& gradient) {
    const ValueComponent& component = value_.components[index];
    if (component.kind == ValueComponentKind::function) return read_gradient(index, gradient);
    if (component.kind == ValueComponentKind::resource) {
        // P6c binds a theme's image resources to the engine's images.
        return fail(DiagnosticCode::not_yet_built, component.token, "an image resource as a background");
    }
    return fail(DiagnosticCode::value_wrong_kind, component.token, "an image or a gradient");
}

// The `fills` declaration of `written`, in CSS's order (the top layer
// first): the layers bottom first, a fixed gradient resolved now and the
// others when the material is assembled for a part.
bool MaterialReader::push_fills(std::span<const AuthoredGradient> written) {
    MaterialDeclaration declaration{};
    declaration.field = MaterialField::fills;
    bool box_relative = false;
    for (const AuthoredGradient& gradient : written) box_relative = box_relative || !authored_gradient_fixed(gradient);
    SurfaceMaterial checked{};
    checked.fills.clear();
    for (std::size_t from_top = written.size(); from_top > 0U; --from_top) {
        const AuthoredGradient& gradient = written[from_top - 1U];
        const bool fixed = authored_gradient_fixed(gradient);
        MaterialFillLayer layer{};
        // A box only a gradient that reads the box uses; the check takes
        // a sample one.
        resolve_authored_gradient(gradient, Size{100.0, 100.0}, layer);
        checked.fills.push_back(layer);
        if (!fixed) layer = MaterialFillLayer{};
        declaration.fills.push_back(layer);
        if (!box_relative) continue;
        AuthoredGradient entry = fixed ? AuthoredGradient{} : gradient;
        if (fixed) entry.kind = MaterialFillKind::solid;
        declaration.gradients.push_back(std::move(entry));
    }
    if (!checked.fills.empty() && !valid_surface_material(checked)) {
        return fail(DiagnosticCode::value_out_of_range, start_, "a gradient no material draws");
    }
    lowering_.declarations.push_back(std::move(declaration));
    return true;
}

// -- borders, radii and shadows --------------------------------------------------

bool MaterialReader::style_word(std::size_t index) const noexcept {
    constexpr std::array<std::string_view, 10> styles{"none",   "hidden", "solid", "dashed", "dotted",
                                                      "double", "groove", "ridge", "inset",  "outset"};
    for (const std::string_view style : styles) {
        if (keyword(index, style)) return true;
    }
    return false;
}

bool MaterialReader::width_word(std::size_t index) const noexcept {
    const bool word = keyword(index, "thin") || keyword(index, "medium") || keyword(index, "thick");
    return word;
}

// A <line-style>: empty for none (CSS's `none` and `hidden`).
bool MaterialReader::read_style(std::size_t index, std::optional<MaterialBorderStyle>& style) {
    const ValueComponent& component = value_.components[index];
    if (component.kind != ValueComponentKind::keyword) {
        return fail(DiagnosticCode::value_wrong_kind, component.token, "a border style");
    }
    if (keyword(index, "none") || keyword(index, "hidden")) {
        style.reset();
        return true;
    }
    if (keyword(index, "solid")) {
        style = MaterialBorderStyle::solid;
        return true;
    }
    if (keyword(index, "dashed")) {
        style = MaterialBorderStyle::dashed;
        return true;
    }
    if (keyword(index, "dotted")) {
        style = MaterialBorderStyle::dotted;
        return true;
    }
    // P6c lowers the three to keyline pairs.
    if (keyword(index, "double") || keyword(index, "groove") || keyword(index, "ridge")) {
        return fail(DiagnosticCode::not_yet_built, component.token, clipped_theme_detail(component.text));
    }
    if (keyword(index, "inset") || keyword(index, "outset")) {
        return fail(DiagnosticCode::value_form_not_admitted, component.token, clipped_theme_detail(component.text));
    }
    return fail(DiagnosticCode::keyword_unknown, component.token, clipped_theme_detail(component.text));
}

// A <line-width>: a length, or CSS's thin (1px), medium (3px) or thick
// (5px).
bool MaterialReader::read_width(std::size_t index, double& width) {
    if (keyword(index, "thin")) {
        width = 1.0;
        return true;
    }
    if (keyword(index, "medium")) {
        width = 3.0;
        return true;
    }
    if (keyword(index, "thick")) {
        width = 5.0;
        return true;
    }
    BoxLength length{};
    if (!read_length(index, false, false, length)) return false;
    width = length.units;
    return true;
}

// One corner's radius: a length, or a percentage, a fraction of the box's
// shorter side (R2-11).
bool MaterialReader::read_radius(std::size_t index, MaterialDeclaration& corner) {
    BoxLength length{};
    if (!read_length(index, true, false, length)) return false;
    corner.relative = length.units == 0.0 && length.fraction != 0.0;
    corner.number = corner.relative ? length.fraction : length.units;
    return true;
}

// `border` and an edge's shorthand: `<line-width> || <line-style> ||
// <color>` for edges [first_edge, first_edge + edge_count); an omitted
// part keeps the floor's (the language's `initial`).
bool MaterialReader::lower_edges(std::size_t first_edge, std::size_t edge_count) {
    std::optional<std::size_t> style_at{};
    std::optional<std::size_t> width_at{};
    std::optional<std::size_t> colour_at{};
    for (std::size_t index = 0U; index < value_.components.size(); index = next(index)) {
        const ValueComponent& component = value_.components[index];
        const bool width = width_word(index) || component.kind == ValueComponentKind::number;
        std::optional<std::size_t>& slot = width ? width_at : (style_word(index) ? style_at : colour_at);
        if (slot) return fail(DiagnosticCode::value_syntax, component.token, "one width, one style and one colour");
        slot = index;
    }
    std::optional<MaterialBorderStyle> style{};
    double width = 0.0;
    Color colour{};
    if (style_at && !read_style(*style_at, style)) return false;
    if (width_at && !read_width(*width_at, width)) return false;
    if (colour_at && !read_colour(*colour_at, colour)) return false;
    for (std::size_t edge = first_edge; edge < first_edge + edge_count; ++edge) {
        MaterialDeclaration styled = initial_field(edge_field(edge, 0U));
        MaterialDeclaration wide = initial_field(edge_field(edge, 1U));
        MaterialDeclaration coloured = initial_field(edge_field(edge, 2U));
        if (style_at) {
            styled.initial = false;
            styled.border_style = style;
        }
        if (width_at) {
            wide.initial = false;
            wide.number = width;
        }
        if (colour_at) {
            coloured.initial = false;
            coloured.colour = colour;
        }
        lowering_.declarations.push_back(std::move(styled));
        lowering_.declarations.push_back(std::move(wide));
        lowering_.declarations.push_back(std::move(coloured));
    }
    return true;
}

// `border-style`, `border-width` or `border-color` (`part` 0, 1 or 2): one
// to four values, CSS's order.
bool MaterialReader::lower_sides(std::size_t part) {
    const std::size_t values = count(ComponentRange{0U, value_.components.size()});
    if (values > 4U) {
        return fail(DiagnosticCode::value_syntax, value_.components.back().token, "one to four values");
    }
    std::array<MaterialDeclaration, 4> read{};
    std::size_t index = 0U;
    for (std::size_t value = 0U; value < values; ++value) {
        MaterialDeclaration& declaration = read[value];
        if (part == 0U && !read_style(index, declaration.border_style)) return false;
        if (part == 1U && !read_width(index, declaration.number)) return false;
        if (part == 2U && !read_colour(index, declaration.colour)) return false;
        index = next(index);
    }
    const std::array<std::size_t, 4>& sides = four_sides[values - 1U];
    for (std::size_t edge = 0U; edge < 4U; ++edge) {
        MaterialDeclaration declaration = read[sides[edge]];
        declaration.field = edge_field(edge, part);
        lowering_.declarations.push_back(std::move(declaration));
    }
    return true;
}

bool MaterialReader::lower_edge_field(std::size_t edge, std::size_t part) {
    if (value_.components.size() != 1U) {
        return fail(DiagnosticCode::value_syntax, value_.components[next(0U)].token, "one value");
    }
    MaterialDeclaration declaration{};
    declaration.field = edge_field(edge, part);
    if (part == 0U && !read_style(0U, declaration.border_style)) return false;
    if (part == 1U && !read_width(0U, declaration.number)) return false;
    if (part == 2U && !read_colour(0U, declaration.colour)) return false;
    lowering_.declarations.push_back(std::move(declaration));
    return true;
}

// `border-radius`: one to four radii, CSS's order; elliptical radii (`/`)
// are not admitted (CSS_COVERAGE section 2).
bool MaterialReader::lower_radii() {
    for (const ValueComponent& component : value_.components) {
        if (component.kind == ValueComponentKind::slash) {
            return fail(DiagnosticCode::value_form_not_admitted, component.token, "elliptical radii");
        }
    }
    const std::size_t values = count(ComponentRange{0U, value_.components.size()});
    if (values > 4U) {
        return fail(DiagnosticCode::value_syntax, value_.components.back().token, "one to four radii");
    }
    std::array<MaterialDeclaration, 4> read{};
    std::size_t index = 0U;
    for (std::size_t value = 0U; value < values; ++value) {
        if (!read_radius(index, read[value])) return false;
        index = next(index);
    }
    const std::array<std::size_t, 4>& sides = four_sides[values - 1U];
    for (std::size_t corner = 0U; corner < 4U; ++corner) {
        MaterialDeclaration declaration = read[sides[corner]];
        declaration.field = corner_field(corner);
        lowering_.declarations.push_back(std::move(declaration));
    }
    return true;
}

bool MaterialReader::lower_corner(std::size_t corner) {
    if (value_.components.size() != 1U) {
        return fail(DiagnosticCode::value_form_not_admitted, value_.components[next(0U)].token, "elliptical radii");
    }
    MaterialDeclaration declaration{};
    declaration.field = corner_field(corner);
    if (!read_radius(0U, declaration)) return false;
    lowering_.declarations.push_back(std::move(declaration));
    return true;
}

// One shadow: `inset? && <length>{2,4} && <color>`. CSS's default colour,
// currentcolor, arrives with the colour roles (P8).
bool MaterialReader::read_shadow(ComponentRange range, MaterialShadow& shadow) {
    shadow = MaterialShadow{};
    shadow.offset = Point{};
    shadow.blur_radius = 0.0;
    bool inset = false;
    bool coloured = false;
    std::array<double, 4> lengths{};
    std::size_t length_count = 0U;
    bool lengths_done = false;
    for (std::size_t index = range.first; index < range.end; index = next(index)) {
        const ValueComponent& component = value_.components[index];
        if (keyword(index, "inset")) {
            if (inset) return fail(DiagnosticCode::value_syntax, component.token, "one inset");
            inset = true;
            lengths_done = length_count > 0U;
            continue;
        }
        if (component.kind == ValueComponentKind::number) {
            if (lengths_done || length_count == lengths.size()) {
                return fail(DiagnosticCode::value_syntax, component.token, "two to four adjacent lengths");
            }
            BoxLength length{};
            const bool negatives = length_count != 2U;
            if (!read_length(index, false, negatives, length)) return false;
            lengths[length_count] = length.units;
            length_count = length_count + 1U;
            continue;
        }
        if (coloured) return fail(DiagnosticCode::value_syntax, component.token, "one colour");
        coloured = true;
        lengths_done = length_count > 0U;
        if (!read_colour(index, shadow.color)) return false;
    }
    const std::uint32_t token = range.first < range.end ? value_.components[range.first].token : start_;
    if (length_count < 2U) return fail(DiagnosticCode::value_syntax, token, "two to four lengths");
    if (!coloured) return fail(DiagnosticCode::not_yet_built, token, "a shadow without a colour (currentcolor)");
    shadow.offset = Point{lengths[0], lengths[1]};
    shadow.blur_radius = lengths[2];
    shadow.spread = lengths[3];
    shadow.inset = inset;
    return true;
}

// `box-shadow`: none, or up to SurfaceMaterial::maximum_shadows shadows,
// CSS's first on top (the material draws its list bottom first).
bool MaterialReader::lower_shadows() {
    MaterialDeclaration declaration{};
    declaration.field = MaterialField::shadows;
    const ComponentRange whole{0U, value_.components.size()};
    if (count(whole) == 1U && keyword(0U, "none")) {
        lowering_.declarations.push_back(std::move(declaration));
        return true;
    }
    std::vector<ComponentRange> shadows{};
    split(whole, shadows);
    if (shadows.size() > SurfaceMaterial::maximum_shadows) {
        return fail(DiagnosticCode::too_many_layers, start_,
                    std::to_string(SurfaceMaterial::maximum_shadows) + " shadows");
    }
    declaration.shadows.resize(shadows.size());
    for (std::size_t shadow = 0U; shadow < shadows.size(); ++shadow) {
        if (!read_shadow(shadows[shadow], declaration.shadows[shadows.size() - 1U - shadow])) return false;
    }
    SurfaceMaterial checked{};
    checked.shadows = declaration.shadows;
    if (!valid_surface_material(checked)) return fail(DiagnosticCode::value_out_of_range, start_, "a shadow");
    lowering_.declarations.push_back(std::move(declaration));
    return true;
}

// -- the properties ---------------------------------------------------------------

bool MaterialReader::lower_background_colour() {
    if (value_.components.size() != 1U) {
        return fail(DiagnosticCode::value_syntax, value_.components[next(0U)].token, "one colour");
    }
    MaterialDeclaration declaration{};
    declaration.field = MaterialField::background_colour;
    if (!read_colour(0U, declaration.colour)) return false;
    lowering_.declarations.push_back(std::move(declaration));
    return true;
}

bool MaterialReader::lower_background_image() {
    const ComponentRange whole{0U, value_.components.size()};
    std::vector<AuthoredGradient> written{};
    if (count(whole) == 1U && keyword(0U, "none")) return push_fills(written);
    std::vector<ComponentRange> layers{};
    split(whole, layers);
    if (layers.size() > SurfaceMaterial::maximum_fill_layers) {
        return fail(DiagnosticCode::too_many_layers, start_, std::to_string(SurfaceMaterial::maximum_fill_layers));
    }
    written.resize(layers.size());
    for (std::size_t layer = 0U; layer < layers.size(); ++layer) {
        const ComponentRange range = layers[layer];
        if (count(range) != 1U) return fail(DiagnosticCode::value_syntax, start_, "one image per layer");
        if (keyword(range.first, "none")) {
            // Kept out so that the blend modes count the drawn layers.
            return fail(DiagnosticCode::value_form_not_admitted, value_.components[range.first].token,
                        "none among other layers");
        }
        if (!read_image(range.first, written[layer])) return false;
    }
    return push_fills(written);
}

bool MaterialReader::lower_blend_modes() {
    std::vector<ComponentRange> modes{};
    split(ComponentRange{0U, value_.components.size()}, modes);
    MaterialDeclaration declaration{};
    declaration.field = MaterialField::fill_blend_modes;
    for (const ComponentRange& mode : modes) {
        if (count(mode) != 1U) return fail(DiagnosticCode::value_syntax, start_, "one blend mode per layer");
        const ValueComponent& component = value_.components[mode.first];
        if (keyword(mode.first, "normal")) {
            declaration.blend_modes.push_back(MaterialBlendMode::normal);
        } else if (keyword(mode.first, "multiply")) {
            declaration.blend_modes.push_back(MaterialBlendMode::multiply);
        } else if (keyword(mode.first, "screen")) {
            declaration.blend_modes.push_back(MaterialBlendMode::screen);
        } else if (component.kind == ValueComponentKind::keyword && css_blend_mode_not_admitted(component.text)) {
            return fail(DiagnosticCode::value_form_not_admitted, component.token, clipped_theme_detail(component.text));
        } else if (component.kind == ValueComponentKind::keyword) {
            return fail(DiagnosticCode::keyword_unknown, component.token, clipped_theme_detail(component.text));
        } else {
            return fail(DiagnosticCode::value_wrong_kind, component.token, "a blend mode");
        }
    }
    lowering_.declarations.push_back(std::move(declaration));
    return true;
}

// The shorthand: the image layers, and the last layer's colour (CSS's
// initial, transparent, when it names none). A layer's position, size,
// repeat, attachment, origin and clip arrive with the image layers (P6c).
bool MaterialReader::lower_background() {
    std::vector<ComponentRange> layers{};
    split(ComponentRange{0U, value_.components.size()}, layers);
    if (layers.size() > SurfaceMaterial::maximum_fill_layers) {
        return fail(DiagnosticCode::too_many_layers, start_, std::to_string(SurfaceMaterial::maximum_fill_layers));
    }
    MaterialDeclaration colour{};
    colour.field = MaterialField::background_colour;
    colour.colour = Color::rgba(0, 0, 0, 0);
    std::vector<AuthoredGradient> written{};
    written.reserve(layers.size());
    for (std::size_t layer = 0U; layer < layers.size(); ++layer) {
        const ComponentRange range = layers[layer];
        const bool last = layer + 1U == layers.size();
        if (range.first >= range.end) return fail(DiagnosticCode::value_syntax, start_, "a background layer");
        bool imaged = false;
        bool coloured = false;
        bool none = false;
        for (std::size_t index = range.first; index < range.end; index = next(index)) {
            const ValueComponent& component = value_.components[index];
            const bool image =
                component.kind == ValueComponentKind::function || component.kind == ValueComponentKind::resource;
            if (image || keyword(index, "none")) {
                if (imaged || none) return fail(DiagnosticCode::value_syntax, component.token, "one image per layer");
                none = !image;
                imaged = image;
                if (!image) continue;
                written.emplace_back();
                if (!read_image(index, written.back())) return false;
                continue;
            }
            const bool named = named_colour(component.text).has_value() ||
                               equals_ascii_folded(component.text, "currentcolor");
            const bool colour_word = component.kind == ValueComponentKind::keyword && named;
            if (component.kind == ValueComponentKind::colour || colour_word) {
                if (!last || coloured) {
                    return fail(DiagnosticCode::value_syntax, component.token, "a colour only in the last layer");
                }
                coloured = true;
                if (!read_colour(index, colour.colour)) return false;
                continue;
            }
            return fail(DiagnosticCode::not_yet_built, component.token,
                        "a background layer's position, size, repeat, attachment, origin or clip");
        }
        if (none && layers.size() > 1U) {
            return fail(DiagnosticCode::value_form_not_admitted, value_.components[range.first].token,
                        "none among other layers");
        }
    }
    lowering_.declarations.push_back(std::move(colour));
    return push_fills(written);
}

void MaterialReader::lower(MaterialProperty property) {
    lowering_.outcome = MaterialLoweringOutcome::lowered;
    lowering_.declarations.clear();
    lowering_.problem = MaterialLoweringProblem{};
    if (value_.components.empty()) {
        fail(DiagnosticCode::value_syntax, start_, "a value");
        return;
    }
    if (!drop_unknown_functions()) return;
    switch (property) {
    case MaterialProperty::background: static_cast<void>(lower_background()); return;
    case MaterialProperty::background_color: static_cast<void>(lower_background_colour()); return;
    case MaterialProperty::background_image: static_cast<void>(lower_background_image()); return;
    case MaterialProperty::background_blend_mode: static_cast<void>(lower_blend_modes()); return;
    case MaterialProperty::border: static_cast<void>(lower_edges(0U, 4U)); return;
    case MaterialProperty::border_top: static_cast<void>(lower_edges(0U, 1U)); return;
    case MaterialProperty::border_right: static_cast<void>(lower_edges(1U, 1U)); return;
    case MaterialProperty::border_bottom: static_cast<void>(lower_edges(2U, 1U)); return;
    case MaterialProperty::border_left: static_cast<void>(lower_edges(3U, 1U)); return;
    case MaterialProperty::border_style: static_cast<void>(lower_sides(0U)); return;
    case MaterialProperty::border_width: static_cast<void>(lower_sides(1U)); return;
    case MaterialProperty::border_color: static_cast<void>(lower_sides(2U)); return;
    case MaterialProperty::border_top_style: static_cast<void>(lower_edge_field(0U, 0U)); return;
    case MaterialProperty::border_top_width: static_cast<void>(lower_edge_field(0U, 1U)); return;
    case MaterialProperty::border_top_color: static_cast<void>(lower_edge_field(0U, 2U)); return;
    case MaterialProperty::border_right_style: static_cast<void>(lower_edge_field(1U, 0U)); return;
    case MaterialProperty::border_right_width: static_cast<void>(lower_edge_field(1U, 1U)); return;
    case MaterialProperty::border_right_color: static_cast<void>(lower_edge_field(1U, 2U)); return;
    case MaterialProperty::border_bottom_style: static_cast<void>(lower_edge_field(2U, 0U)); return;
    case MaterialProperty::border_bottom_width: static_cast<void>(lower_edge_field(2U, 1U)); return;
    case MaterialProperty::border_bottom_color: static_cast<void>(lower_edge_field(2U, 2U)); return;
    case MaterialProperty::border_left_style: static_cast<void>(lower_edge_field(3U, 0U)); return;
    case MaterialProperty::border_left_width: static_cast<void>(lower_edge_field(3U, 1U)); return;
    case MaterialProperty::border_left_color: static_cast<void>(lower_edge_field(3U, 2U)); return;
    case MaterialProperty::border_radius: static_cast<void>(lower_radii()); return;
    case MaterialProperty::border_top_left_radius: static_cast<void>(lower_corner(0U)); return;
    case MaterialProperty::border_top_right_radius: static_cast<void>(lower_corner(1U)); return;
    case MaterialProperty::border_bottom_right_radius: static_cast<void>(lower_corner(2U)); return;
    case MaterialProperty::border_bottom_left_radius: static_cast<void>(lower_corner(3U)); return;
    case MaterialProperty::box_shadow: static_cast<void>(lower_shadows()); return;
    }
}

} // namespace

std::optional<MaterialProperty> material_property(std::string_view name) noexcept {
    for (const NamedMaterialProperty& named : material_properties) {
        if (named.name == name) return named.property;
    }
    return std::nullopt;
}

bool css_blend_mode_not_admitted(std::string_view name) noexcept {
    std::string folded(name);
    for (char& character : folded) {
        if (character >= 'A' && character <= 'Z') character = static_cast<char>(character - 'A' + 'a');
    }
    const bool listed = std::binary_search(blend_modes_not_admitted.begin(), blend_modes_not_admitted.end(),
                                           std::string_view(folded));
    return listed;
}

void material_property_fields(MaterialProperty property, std::vector<MaterialField>& fields) {
    fields.clear();
    switch (property) {
    case MaterialProperty::background:
        fields.push_back(MaterialField::background_colour);
        fields.push_back(MaterialField::fills);
        return;
    case MaterialProperty::background_color: fields.push_back(MaterialField::background_colour); return;
    case MaterialProperty::background_image: fields.push_back(MaterialField::fills); return;
    case MaterialProperty::background_blend_mode: fields.push_back(MaterialField::fill_blend_modes); return;
    case MaterialProperty::border:
        for (std::size_t edge = 0U; edge < 4U; ++edge) {
            for (std::size_t part = 0U; part < 3U; ++part) fields.push_back(edge_field(edge, part));
        }
        return;
    case MaterialProperty::border_top:
        for (std::size_t part = 0U; part < 3U; ++part) fields.push_back(edge_field(0U, part));
        return;
    case MaterialProperty::border_right:
        for (std::size_t part = 0U; part < 3U; ++part) fields.push_back(edge_field(1U, part));
        return;
    case MaterialProperty::border_bottom:
        for (std::size_t part = 0U; part < 3U; ++part) fields.push_back(edge_field(2U, part));
        return;
    case MaterialProperty::border_left:
        for (std::size_t part = 0U; part < 3U; ++part) fields.push_back(edge_field(3U, part));
        return;
    case MaterialProperty::border_style:
        for (std::size_t edge = 0U; edge < 4U; ++edge) fields.push_back(edge_field(edge, 0U));
        return;
    case MaterialProperty::border_width:
        for (std::size_t edge = 0U; edge < 4U; ++edge) fields.push_back(edge_field(edge, 1U));
        return;
    case MaterialProperty::border_color:
        for (std::size_t edge = 0U; edge < 4U; ++edge) fields.push_back(edge_field(edge, 2U));
        return;
    case MaterialProperty::border_top_style: fields.push_back(edge_field(0U, 0U)); return;
    case MaterialProperty::border_top_width: fields.push_back(edge_field(0U, 1U)); return;
    case MaterialProperty::border_top_color: fields.push_back(edge_field(0U, 2U)); return;
    case MaterialProperty::border_right_style: fields.push_back(edge_field(1U, 0U)); return;
    case MaterialProperty::border_right_width: fields.push_back(edge_field(1U, 1U)); return;
    case MaterialProperty::border_right_color: fields.push_back(edge_field(1U, 2U)); return;
    case MaterialProperty::border_bottom_style: fields.push_back(edge_field(2U, 0U)); return;
    case MaterialProperty::border_bottom_width: fields.push_back(edge_field(2U, 1U)); return;
    case MaterialProperty::border_bottom_color: fields.push_back(edge_field(2U, 2U)); return;
    case MaterialProperty::border_left_style: fields.push_back(edge_field(3U, 0U)); return;
    case MaterialProperty::border_left_width: fields.push_back(edge_field(3U, 1U)); return;
    case MaterialProperty::border_left_color: fields.push_back(edge_field(3U, 2U)); return;
    case MaterialProperty::border_radius:
        for (std::size_t corner = 0U; corner < 4U; ++corner) fields.push_back(corner_field(corner));
        return;
    case MaterialProperty::border_top_left_radius: fields.push_back(corner_field(0U)); return;
    case MaterialProperty::border_top_right_radius: fields.push_back(corner_field(1U)); return;
    case MaterialProperty::border_bottom_right_radius: fields.push_back(corner_field(2U)); return;
    case MaterialProperty::border_bottom_left_radius: fields.push_back(corner_field(3U)); return;
    case MaterialProperty::box_shadow: fields.push_back(MaterialField::shadows); return;
    }
}

void lower_material_value(MaterialProperty property, const ThemeValue& value, std::uint32_t start,
                          MaterialLowering& lowering) {
    MaterialReader reader(value, start, lowering);
    reader.lower(property);
}

} // namespace gui_forms::theme_language
