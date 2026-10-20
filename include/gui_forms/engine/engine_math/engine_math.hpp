#pragma once

namespace gui_forms {

// Deterministic elementary functions for engine kernels (T2 plan, "Numerics
// and identical pixels"). The host libraries' pow, exp and log differ in
// their last bits between macOS, glibc and the Windows runtime, so an engine
// kernel never calls them. These are built only from + − × ÷ and sqrt, which
// IEEE 754 rounds exactly, and frexp and ldexp, which are exact, and are
// compiled without fused multiply-add. The same inputs give the same bits on
// every host.
//
// Error bounds, against the exact mathematical value:
// - engine_logarithm: absolute error at most 2^-50 · (1 + |ln x|);
// - engine_exponential: relative error at most 2^-49 for results from
//   2^-1000 to 2^1000;
// - engine_power: relative error at most 2^-49 · (1 + |exponent · ln base|)
//   for results in the same range.
// The tests check them against the host library with a margin.

// Natural logarithm of `x`. Requires x > 0 and finite; returns NaN for
// x < 0 or NaN, −infinity for 0, and +infinity for +infinity.
[[nodiscard]] double engine_logarithm(double x) noexcept;

// e raised to `x`. Returns 0 below about −745, +infinity above about 709.8,
// and NaN for NaN.
[[nodiscard]] double engine_exponential(double x) noexcept;

// `base` raised to `exponent`, for base ≥ 0. Exact cases: base^0 is 1,
// 1^exponent is 1, and 0^exponent is 0 for exponent > 0. Returns NaN for a
// negative or NaN base and for a NaN exponent.
[[nodiscard]] double engine_power(double base, double exponent) noexcept;

} // namespace gui_forms
