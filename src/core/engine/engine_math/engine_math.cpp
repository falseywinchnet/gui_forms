#include "gui_forms/engine/engine_math/engine_math.hpp"

#include <cmath>
#include <limits>

// Identical bits on every host need the same operations in the same order.
// Clang contracts a·b + c into a fused multiply-add by default on arm64 and
// not on baseline x86-64, so contraction is off for this translation unit.
#if defined(__clang__)
#pragma clang fp contract(off)
#endif

namespace gui_forms {
namespace {

// ln 2 split in two (Cody and Waite): the high part has 32 significant bits,
// so n · ln2_high is exact for every |n| below 2^21, and the low part
// carries the rest.
constexpr double ln2_high = 6.93147180369123816490e-01;
constexpr double ln2_low = 1.90821492927058770002e-10;
constexpr double inverse_ln2 = 1.44269504088896338700e+00;
constexpr double sqrt_half = 7.07106781186547524401e-01;

// Beyond these, e^x overflows to infinity or underflows to zero.
constexpr double largest_exponent = 709.782712893383973096;
constexpr double smallest_exponent = -745.133219101941108420;

// ln m for m in [sqrt(1/2), sqrt(2)), as 2 atanh(s) with s = (m − 1)/(m + 1)
// and |s| ≤ 0.1716. The series 2 s (1 + s²/3 + s⁴/5 + ... + s¹⁸/19) leaves out
// at most 2 |s|²¹/21, below 2^-55.
double logarithm_of_reduced(double m) noexcept {
    const double s = (m - 1.0) / (m + 1.0);
    const double s2 = s * s;
    double series = 1.0 / 19.0;
    series = series * s2 + 1.0 / 17.0;
    series = series * s2 + 1.0 / 15.0;
    series = series * s2 + 1.0 / 13.0;
    series = series * s2 + 1.0 / 11.0;
    series = series * s2 + 1.0 / 9.0;
    series = series * s2 + 1.0 / 7.0;
    series = series * s2 + 1.0 / 5.0;
    series = series * s2 + 1.0 / 3.0;
    series = series * s2 + 1.0;
    const double logarithm = 2.0 * s * series;
    return logarithm;
}

// e^r for |r| ≤ 0.35, by its Taylor series through r¹³/13!; the omitted
// tail is below 0.35¹⁴/14!, about 2^-58.
double exponential_of_reduced(double r) noexcept {
    double series = 1.0 / 6227020800.0;
    series = series * r + 1.0 / 479001600.0;
    series = series * r + 1.0 / 39916800.0;
    series = series * r + 1.0 / 3628800.0;
    series = series * r + 1.0 / 362880.0;
    series = series * r + 1.0 / 40320.0;
    series = series * r + 1.0 / 5040.0;
    series = series * r + 1.0 / 720.0;
    series = series * r + 1.0 / 120.0;
    series = series * r + 1.0 / 24.0;
    series = series * r + 1.0 / 6.0;
    series = series * r + 1.0 / 2.0;
    series = series * r + 1.0;
    series = series * r + 1.0;
    return series;
}

} // namespace

double engine_logarithm(double x) noexcept {
    if (std::isnan(x) || x < 0.0) return std::numeric_limits<double>::quiet_NaN();
    if (x == 0.0) return -std::numeric_limits<double>::infinity();
    if (std::isinf(x)) return x;
    // x = m · 2^e with m in [0.5, 1), then moved into [sqrt(1/2), sqrt(2)).
    // frexp and the doubling are exact.
    int exponent = 0;
    double mantissa = std::frexp(x, &exponent);
    if (mantissa < sqrt_half) {
        mantissa = mantissa * 2.0;
        exponent = exponent - 1;
    }
    const double e = static_cast<double>(exponent);
    const double reduced = logarithm_of_reduced(mantissa);
    const double low = e * ln2_low + reduced;
    const double logarithm = e * ln2_high + low;
    return logarithm;
}

double engine_exponential(double x) noexcept {
    if (std::isnan(x)) return x;
    if (x > largest_exponent) return std::numeric_limits<double>::infinity();
    if (x < smallest_exponent) return 0.0;
    // x = n ln 2 + r with n the nearest integer to x / ln 2, so |r| ≤ 0.35.
    const double n = std::floor(x * inverse_ln2 + 0.5);
    const double high = x - n * ln2_high;
    const double r = high - n * ln2_low;
    const double reduced = exponential_of_reduced(r);
    const double exponential = std::ldexp(reduced, static_cast<int>(n));
    return exponential;
}

double engine_power(double base, double exponent) noexcept {
    if (std::isnan(base) || std::isnan(exponent) || base < 0.0) {
        return std::numeric_limits<double>::quiet_NaN();
    }
    if (exponent == 0.0 || base == 1.0) return 1.0;
    if (base == 0.0) return exponent > 0.0 ? 0.0 : std::numeric_limits<double>::infinity();
    if (std::isinf(base)) return exponent > 0.0 ? base : 0.0;
    const double logarithm = engine_logarithm(base);
    const double scaled = exponent * logarithm;
    const double power = engine_exponential(scaled);
    return power;
}

} // namespace gui_forms
