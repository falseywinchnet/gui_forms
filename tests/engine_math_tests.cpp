// The engine's deterministic elementary functions (T2 plan, phase E1): their
// error against the host library stays inside the stated bounds, their exact
// cases are exact, and a fixed table of results has the same bits on every
// CI platform. GUI_FORMS_ENGINE_IDENTITY_PRINT=1 prints the observed hash
// instead of checking it.

#include "gui_forms/engine/engine_math/engine_math.hpp"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>

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

double relative_error(double observed, double expected) {
    const double difference = std::fabs(observed - expected);
    const double magnitude = std::fabs(expected);
    const double error = magnitude == 0.0 ? difference : difference / magnitude;
    return error;
}

// FNV-1a over the bytes of each double, in order.
struct BitHash final {
    std::uint64_t value{14695981039346656037ULL};

    void add(double number) {
        std::uint64_t bits = 0U;
        std::memcpy(&bits, &number, sizeof bits);
        for (int shift = 0; shift < 64; shift += 8) {
            const std::uint64_t byte = (bits >> shift) & 0xFFU;
            value = value ^ byte;
            value = value * 1099511628211ULL;
        }
    }
};

void test_logarithm() {
    for (int step = 1; step <= 4000; ++step) {
        const double x = static_cast<double>(step) * 0.00137;
        const double observed = engine_logarithm(x);
        const double expected = std::log(x);
        const double allowed = 1e-15 * (1.0 + std::fabs(expected));
        require(std::fabs(observed - expected) <= allowed, "logarithm within its bound on (0, 5.5]");
    }
    const double wide[] = {1e-300, 1e-100, 3.5e-20, 7.25e15, 1e100, 1e300};
    for (const double x : wide) {
        const double expected = std::log(x);
        const double allowed = 1e-15 * (1.0 + std::fabs(expected));
        require(std::fabs(engine_logarithm(x) - expected) <= allowed, "logarithm within its bound far from 1");
    }
    require(engine_logarithm(1.0) == 0.0, "ln 1 is exactly 0");
    require(std::isinf(engine_logarithm(0.0)) && engine_logarithm(0.0) < 0.0, "ln 0 is minus infinity");
    require(std::isnan(engine_logarithm(-1.0)), "ln of a negative number is NaN");
    require(std::isnan(engine_logarithm(std::numeric_limits<double>::quiet_NaN())), "ln NaN is NaN");
}

void test_exponential() {
    for (int step = -7000; step <= 7000; ++step) {
        const double x = static_cast<double>(step) * 0.1;
        const double observed = engine_exponential(x);
        const double expected = std::exp(x);
        require(relative_error(observed, expected) <= 1e-14, "exponential within its bound on [-700, 700]");
    }
    require(engine_exponential(0.0) == 1.0, "e^0 is exactly 1");
    require(std::isinf(engine_exponential(710.0)), "e^710 overflows to infinity");
    require(engine_exponential(-746.0) == 0.0, "e^-746 underflows to zero");
}

void test_power() {
    const double exponents[] = {0.5, 1.0, 2.2, 3.7, 8.0};
    for (const double exponent : exponents) {
        for (int step = 1; step <= 1000; ++step) {
            const double base = static_cast<double>(step) / 1000.0;
            const double observed = engine_power(base, exponent);
            const double expected = std::pow(base, exponent);
            require(relative_error(observed, expected) <= 1e-13, "power within its bound on (0, 1]");
        }
    }
    require(engine_power(1.0, 2.2) == 1.0, "1^q is exactly 1");
    require(engine_power(0.0, 2.2) == 0.0, "0^q is exactly 0 for q > 0");
    require(engine_power(0.37, 0.0) == 1.0, "b^0 is exactly 1");
    require(engine_power(2.0, 3.0) == 8.0 || relative_error(engine_power(2.0, 3.0), 8.0) <= 1e-15,
            "2^3 is 8 within one rounding");
    require(std::isnan(engine_power(-0.5, 2.0)), "a negative base is NaN");
}

// The bits of a fixed table, the same on every host. The expected value was
// produced on Windows x64 and is checked by every CI platform.
constexpr std::uint64_t expected_table_hash = 0xa2d6d4ebca4d01adULL;

void test_identity() {
    BitHash hash{};
    for (int step = 1; step <= 512; ++step) {
        const double t = static_cast<double>(step) / 512.0;
        hash.add(engine_power(t, 2.2));
        hash.add(engine_logarithm(t * 37.0));
        hash.add(engine_exponential(t * 40.0 - 20.0));
    }
    if (print_mode()) {
        std::cout << "engine math table hash 0x" << std::hex << hash.value << std::dec << '\n';
        return;
    }
    require(hash.value == expected_table_hash, "the math table has the same bits on every host");
}

} // namespace

int main() {
    test_logarithm();
    test_exponential();
    test_power();
    test_identity();
    std::cout << "engine math tests passed\n";
    return 0;
}
