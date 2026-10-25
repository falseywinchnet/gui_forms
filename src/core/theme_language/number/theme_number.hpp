#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace gui_forms::theme_language {

// The conversion of a theme number's text to a double (PLAN.md phase P1).
// It is first-party and correctly rounded (to nearest, ties to even), so the
// same text gives the same bits on every host whatever its C library does.
//
// A theme number is bounded so that the exact conversion stays small (a
// derived limit, as T3 round 002 item 14's are):
// - at most maximum_significant_digits significant digits, counted after
//   leading zeros and trailing zeros are removed (`1.50000` has two);
// - zero, or a magnitude from 10^minimum_decimal_magnitude up to but not
//   including 10^maximum_decimal_magnitude.
// Every such value is a normal double, so neither overflow nor subnormals
// arise.
inline constexpr std::size_t maximum_significant_digits = 19U;
inline constexpr int minimum_decimal_magnitude = -40;
inline constexpr int maximum_decimal_magnitude = 40;

enum class NumberDefect : std::uint8_t {
    none,
    // The text is not a CSS number: [+-]? (D+ ('.' D+)? | '.' D+)
    // ([eE] [+-]? D+)?, with D a decimal digit. The tokenizer never passes
    // such text; the check keeps the conversion total.
    not_a_number,
    too_precise,
    out_of_range,
};

struct NumberConversion final {
    // The value, when the defect is none; 0 otherwise. A negative zero
    // stays negative.
    double value{};
    NumberDefect defect{NumberDefect::none};
    // CSS's integer type flag: no '.' and no exponent.
    bool integer{};
};

// Converts `text`, the whole of a number's text (sign, digits, fraction and
// exponent; no unit and no '%').
[[nodiscard]] NumberConversion convert_theme_number(std::string_view text) noexcept;

} // namespace gui_forms::theme_language
