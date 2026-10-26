// The theme language's number conversion (stage T3, phase P1): correctly
// rounded, and the same bits on every host. The expected bits were computed
// with Python's float(), which rounds correctly; a host whose conversion
// differs fails here, which is how CI proves the three hosts identical.

#include "../../src/core/theme_language/number/theme_number.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using gui_forms::theme_language::NumberConversion;
using gui_forms::theme_language::NumberDefect;
using gui_forms::theme_language::convert_theme_number;

int failures = 0;

void require(bool condition, std::string_view message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        ++failures;
    }
}

struct BitsCase final {
    std::string_view text{};
    std::uint64_t bits{};
};

// Fast-path and exact-path values, ties to even, and the admitted range's
// edges.
constexpr std::array<BitsCase, 47> bits_cases{{
    {"0", 0x0000000000000000ULL},
    {"-0", 0x8000000000000000ULL},
    {"0.0", 0x0000000000000000ULL},
    {"1", 0x3FF0000000000000ULL},
    {"-1", 0xBFF0000000000000ULL},
    {"0.1", 0x3FB999999999999AULL},
    {"0.2", 0x3FC999999999999AULL},
    {"0.3", 0x3FD3333333333333ULL},
    {"1.5", 0x3FF8000000000000ULL},
    {"1.50000", 0x3FF8000000000000ULL},
    {"3.14159", 0x400921F9F01B866EULL},
    {"100", 0x4059000000000000ULL},
    {"1e22", 0x4480F0CF064DD592ULL},
    {"1e23", 0x44B52D02C7E14AF6ULL},
    {"1e-22", 0x3B5E392010175EE6ULL},
    {"1e-23", 0x3B282DB34012B251ULL},
    {"9007199254740993", 0x4340000000000000ULL},
    {"9007199254740995", 0x4340000000000002ULL},
    {"9007199254740992", 0x4340000000000000ULL},
    {"123456789012345678e-5", 0x4271F71FB04CB74FULL},
    {"1234567890123456789", 0x43B12210F47DE981ULL},
    {"123456789012345678900", 0x441AC53A7E04BCDAULL},
    {"9.999999999999999999e39", 0x483D6329F1C35CA5ULL},
    {"1e-40", 0x37A16C262777579CULL},
    {"4.9406564584124654e-40", 0x37C585041B2C477EULL},
    {"2.2250738585072014e-38", 0x381E494034E79E5CULL},
    {"0.000000000000000000000000000001", 0x39B4484BFEEBC2A0ULL},
    {"1e39", 0x48078287F49C4A1DULL},
    {"1.7976931348623157e39", 0x481521BC6A6B555CULL},
    {"8.98846567431158e38", 0x480521BC6A6B555DULL},
    {".5", 0x3FE0000000000000ULL},
    {"+.5", 0x3FE0000000000000ULL},
    {"-.25", 0xBFD0000000000000ULL},
    {"0.30000000000000004", 0x3FD3333333333334ULL},
    {"2.5e-30", 0x39C95A5EFEA6B347ULL},
    {"7.2057594037927933e16", 0x4370000000000000ULL},
    {"1e+5", 0x40F86A0000000000ULL},
    {"1E5", 0x40F86A0000000000ULL},
    {"00012", 0x4028000000000000ULL},
    {"0.1e1", 0x3FF0000000000000ULL},
    {"6.02214076e23", 0x44DFE185CA57C517ULL},
    {"1.602176634e-19", 0x3C07A4DA290C1653ULL},
    {"299792458", 0x41B1DE784A000000ULL},
    {"3.333333333333333333", 0x400AAAAAAAAAAAABULL},
    {"0.3333333333333333333", 0x3FD5555555555555ULL},
    {"1e30", 0x46293E5939A08CEAULL},
    {"123e-30", 0x3A237D7906F68D05ULL},
}};

void test_bits() {
    for (const BitsCase& entry : bits_cases) {
        const NumberConversion conversion = convert_theme_number(entry.text);
        const std::uint64_t bits = std::bit_cast<std::uint64_t>(conversion.value);
        const bool converted = conversion.defect == NumberDefect::none && bits == entry.bits;
        require(converted, std::string("bits of ") + std::string(entry.text));
    }
}

struct DefectCase final {
    std::string_view text{};
    NumberDefect defect{};
};

constexpr std::array<DefectCase, 17> defect_cases{{
    {"12345678901234567891", NumberDefect::too_precise},
    {"1.2345678901234567891", NumberDefect::too_precise},
    {"1e40", NumberDefect::out_of_range},
    {"10000000000000000000000000000000000000000", NumberDefect::out_of_range},
    {"9e-41", NumberDefect::out_of_range},
    {"1e-41", NumberDefect::out_of_range},
    {"1e999999999999", NumberDefect::out_of_range},
    {"1e-999999999999", NumberDefect::out_of_range},
    {"", NumberDefect::not_a_number},
    {"+", NumberDefect::not_a_number},
    {"12.", NumberDefect::not_a_number},
    {"1e", NumberDefect::not_a_number},
    {"1e+", NumberDefect::not_a_number},
    {"e5", NumberDefect::not_a_number},
    {"1.2.3", NumberDefect::not_a_number},
    {"--1", NumberDefect::not_a_number},
    {"1px", NumberDefect::not_a_number},
}};

void test_defects() {
    for (const DefectCase& entry : defect_cases) {
        const NumberConversion conversion = convert_theme_number(entry.text);
        const bool refused = conversion.defect == entry.defect && conversion.value == 0.0;
        require(refused, std::string("defect of ") + std::string(entry.text));
    }
    // A zero is never out of range, however it is written.
    const NumberConversion zero = convert_theme_number("0e999999");
    require(zero.defect == NumberDefect::none && zero.value == 0.0, "zero with a large exponent");
    // Trailing zeros are not significant digits.
    const NumberConversion long_zeros = convert_theme_number("1.000000000000000000000000000000");
    require(long_zeros.defect == NumberDefect::none && long_zeros.value == 1.0, "trailing zeros");
}

void test_integer_flag() {
    require(convert_theme_number("12").integer, "12 is an integer");
    require(convert_theme_number("-3").integer, "-3 is an integer");
    require(!convert_theme_number("1.0").integer, "1.0 is not an integer");
    require(!convert_theme_number("1e2").integer, "1e2 is not an integer");
}

// One xorshift64 step.
std::uint64_t next_random(std::uint64_t& state) noexcept {
    state ^= state << 13U;
    state ^= state >> 7U;
    state ^= state << 17U;
    return state;
}

// Random admitted numbers against the host's strtod, which every CI host
// rounds correctly too: a mismatch is a defect in one of them, so the case
// is printed.
void test_against_host() {
    std::uint64_t state = 0x9e3779b97f4a7c15ULL;
    std::size_t mismatches = 0U;
    for (std::size_t run = 0U; run < 200000U; ++run) {
        const std::size_t digit_count = 1U + static_cast<std::size_t>(next_random(state) % 19U);
        std::string text{};
        for (std::size_t digit = 0U; digit < digit_count; ++digit) {
            const std::uint64_t draw = next_random(state) % 10U;
            const char character = static_cast<char>('0' + (digit == 0U && draw == 0U ? 1U : draw));
            text.push_back(character);
            if (digit == 0U && digit_count > 1U) text.push_back('.');
        }
        // value in [1, 10) × 10^exponent; the admitted range is [-40, 39].
        const int exponent = static_cast<int>(next_random(state) % 80U) - 40;
        text += 'e';
        text += std::to_string(exponent);
        const NumberConversion conversion = convert_theme_number(text);
        const double host = std::strtod(text.c_str(), nullptr);
        const std::uint64_t ours = std::bit_cast<std::uint64_t>(conversion.value);
        const std::uint64_t theirs = std::bit_cast<std::uint64_t>(host);
        if (conversion.defect != NumberDefect::none || ours != theirs) {
            ++mismatches;
            if (mismatches <= 10U) std::cerr << "mismatch: " << text << '\n';
        }
    }
    require(mismatches == 0U, "conversion agrees with the host's strtod");
}

} // namespace

int main() {
    test_bits();
    test_defects();
    test_integer_flag();
    test_against_host();
    if (failures != 0) {
        std::cerr << failures << " failure(s)\n";
        return 1;
    }
    std::cout << "theme number tests passed\n";
    return 0;
}
