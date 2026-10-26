#pragma once

// The material properties' lowering (stage T3, PLAN.md P6b): CSS's
// background, border, border-radius and box-shadow families, read from a declaration's
// value, become the engine's material fields (EC1), each CSS longhand one
// field, so that each cascades as CSS cascades it. The compiler reads the
// value and handles the CSS-wide keywords; this lowers what is left into
// a result record and reports nothing itself.

#include "../diagnostics/theme_diagnostics.hpp"
#include "../value/theme_value.hpp"

#include "gui_forms/engine/theme_rules/theme_rules.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace gui_forms::theme_language {

// The CSS properties lowered onto material fields: each family's
// shorthands, then its longhands. The edges' and the corners' come in
// CSS's order (top, right, bottom, left; top-left, top-right,
// bottom-right, bottom-left), each edge's in the order of its fields
// (style, width, colour).
enum class MaterialProperty : std::uint8_t {
    background,
    background_color,
    background_image,
    background_blend_mode,
    border,
    border_top,
    border_right,
    border_bottom,
    border_left,
    border_style,
    border_width,
    border_color,
    border_top_style,
    border_top_width,
    border_top_color,
    border_right_style,
    border_right_width,
    border_right_color,
    border_bottom_style,
    border_bottom_width,
    border_bottom_color,
    border_left_style,
    border_left_width,
    border_left_color,
    border_radius,
    border_top_left_radius,
    border_top_right_radius,
    border_bottom_right_radius,
    border_bottom_left_radius,
    box_shadow,
};

// The property `name` (lower case) names, or none for another property.
[[nodiscard]] std::optional<MaterialProperty> material_property(std::string_view name) noexcept;

// Whether `name`, in any ASCII case, is one of CSS's blend modes the
// engine does not composite (CSS_COVERAGE section 7): an error, not an
// unknown keyword.
[[nodiscard]] bool css_blend_mode_not_admitted(std::string_view name) noexcept;

// The material fields `property` sets, in order, into `fields`, which is
// cleared first: what CSS's `initial` resets.
void material_property_fields(MaterialProperty property, std::vector<MaterialField>& fields);

enum class MaterialLoweringOutcome : std::uint8_t {
    // `declarations` hold the fields the value sets.
    lowered,
    // A warning (`problem`): the declaration is dropped, the rule kept.
    dropped,
    // An error (`problem`) refuses the sheet.
    refused,
};

// What one lowering found wrong: the diagnostic the compiler reports, at
// the sheet token `token`.
struct MaterialLoweringProblem final {
    DiagnosticCode code{DiagnosticCode::value_syntax};
    std::uint32_t token{};
    std::string detail{};
};

// One declaration's lowering, caller-owned and reused: a lowering
// replaces the contents and keeps the capacity.
struct MaterialLowering final {
    MaterialLoweringOutcome outcome{MaterialLoweringOutcome::refused};
    std::vector<MaterialDeclaration> declarations{};
    MaterialLoweringProblem problem{};
};

// Lowers `value`, a value of `property` that is not a CSS-wide keyword,
// into `lowering`. `start` is the token the value starts at, where a
// problem with the whole value points. `value` is borrowed for the call;
// the declarations own what they hold.
void lower_material_value(MaterialProperty property, const ThemeValue& value, std::uint32_t start,
                          MaterialLowering& lowering);

} // namespace gui_forms::theme_language
