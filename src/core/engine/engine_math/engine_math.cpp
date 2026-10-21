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

// π/2 split in three (Cody and Waite; fdlibm's pio2_1, pio2_2 and pio2_2t):
// the first two parts have at most 32 significant bits, so n times either
// is exact for every |n| below 2^21, and the three sum to π/2 within 2^-122.
constexpr double half_pi_first = 1.57079632673412561417e+00;
constexpr double half_pi_second = 6.07710050630396597660e-11;
constexpr double half_pi_rest = 2.02226624879595063154e-21;
constexpr double inverse_half_pi = 6.36619772367581382433e-01;
// The sine and cosine arguments the reduction serves.
constexpr double largest_angle = 1048576.0;

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

// sin r for |r| ≤ π/4 (plus the reduction's rounding), by its Taylor series
// through r¹⁹/19!; the omitted tail is below (π/4)²¹/21!, about 2^-72.
double sine_of_reduced(double r) noexcept {
    const double r2 = r * r;
    double series = -1.0 / 121645100408832000.0;
    series = series * r2 + 1.0 / 355687428096000.0;
    series = series * r2 - 1.0 / 1307674368000.0;
    series = series * r2 + 1.0 / 6227020800.0;
    series = series * r2 - 1.0 / 39916800.0;
    series = series * r2 + 1.0 / 362880.0;
    series = series * r2 - 1.0 / 5040.0;
    series = series * r2 + 1.0 / 120.0;
    series = series * r2 - 1.0 / 6.0;
    series = series * r2 + 1.0;
    const double sine = r * series;
    return sine;
}

// cos r for |r| ≤ π/4, by its Taylor series through r¹⁸/18!; the omitted
// tail is below (π/4)²⁰/20!, about 2^-68.
double cosine_of_reduced(double r) noexcept {
    const double r2 = r * r;
    double series = 1.0 / 6402373705728000.0;
    series = series * r2 - 1.0 / 20922789888000.0;
    series = series * r2 + 1.0 / 87178291200.0;
    series = series * r2 - 1.0 / 479001600.0;
    series = series * r2 + 1.0 / 3628800.0;
    series = series * r2 - 1.0 / 40320.0;
    series = series * r2 + 1.0 / 720.0;
    series = series * r2 - 1.0 / 24.0;
    series = series * r2 + 1.0 / 2.0;
    const double cosine = 1.0 - r2 * series;
    return cosine;
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

EngineSineCosine engine_sine_cosine(double x) noexcept {
    EngineSineCosine result{};
    if (!(std::fabs(x) <= largest_angle)) {
        result.sine = std::numeric_limits<double>::quiet_NaN();
        result.cosine = result.sine;
        return result;
    }
    // x = n π/2 + r with n the nearest integer to x / (π/2), so |r| ≤ π/4
    // up to rounding. Each product with a part of π/2 is exact.
    const double n = std::floor(x * inverse_half_pi + 0.5);
    const double first = x - n * half_pi_first;
    const double second = first - n * half_pi_second;
    const double r = second - n * half_pi_rest;
    const double sine = sine_of_reduced(r);
    const double cosine = cosine_of_reduced(r);
    // The quadrant: n modulo 4, as 0 to 3 for negative n too.
    const long long whole = static_cast<long long>(n);
    const long long quadrant = ((whole % 4) + 4) % 4;
    if (quadrant == 0) {
        result.sine = sine;
        result.cosine = cosine;
    } else if (quadrant == 1) {
        result.sine = cosine;
        result.cosine = -sine;
    } else if (quadrant == 2) {
        result.sine = -sine;
        result.cosine = -cosine;
    } else {
        result.sine = -cosine;
        result.cosine = sine;
    }
    return result;
}

} // namespace gui_forms
