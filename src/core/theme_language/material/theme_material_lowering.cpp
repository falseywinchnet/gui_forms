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

constexpr std::array<NamedMaterialProperty, 4> material_properties{{
    {.name = "background", .property = MaterialProperty::background},
    {.name = "background-blend-mode", .property = MaterialProperty::background_blend_mode},
    {.name = "background-color", .property = MaterialProperty::background_color},
    {.name = "background-image", .property = MaterialProperty::background_image},
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

    bool lower_background_colour();
    bool lower_background_image();
    bool lower_blend_modes();
    bool lower_background();

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
    }
}

void lower_material_value(MaterialProperty property, const ThemeValue& value, std::uint32_t start,
                          MaterialLowering& lowering) {
    MaterialReader reader(value, start, lowering);
    reader.lower(property);
}

} // namespace gui_forms::theme_language
