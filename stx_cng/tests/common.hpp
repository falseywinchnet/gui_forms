#pragma once

// Shared fixtures for the stx_cng tests: deterministic generated images and
// one encode/decode helper. Everything is in memory; no test writes files.
#include "stx_cng/codec.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <span>
#include <vector>

namespace stx_cng_test {

inline int failures = 0;

inline void check(bool condition, const char* what) {
    if (!condition) {
        std::fprintf(stderr, "FAIL: %s\n", what);
        ++failures;
    }
}

enum class Pattern : std::uint8_t { ramp, steps, repeats, noise, sparse_noise, flat };
inline constexpr Pattern patterns[] = {Pattern::ramp, Pattern::steps, Pattern::repeats,
                                       Pattern::noise, Pattern::sparse_noise, Pattern::flat};

// Tightly packed samples of `image` following `pattern`; the seed varies noise.
inline std::vector<std::byte> generate(stx_cng::CngImage image, Pattern pattern, std::uint32_t seed) {
    std::size_t channels = static_cast<std::size_t>(image.channels);
    std::vector<std::byte> samples(static_cast<std::size_t>(image.width) * image.height * channels);
    std::uint32_t state = 20261022U + seed;
    for (std::size_t index = 0; index < samples.size(); ++index) {
        state = state * 1664525U + 1013904223U;
        std::size_t pixel = index / channels;
        std::size_t channel = index % channels;
        std::size_t x = pixel % image.width;
        std::size_t y = pixel / image.width;
        std::uint32_t value = state >> 24;
        switch (pattern) {
        case Pattern::ramp: value = static_cast<std::uint32_t>(3 * x + y + 67 * channel); break;
        case Pattern::steps: value = x < image.width / 2 ? 0U : 255U; break;
        case Pattern::repeats: value = static_cast<std::uint32_t>((x % 7) * 19 + channel * 31); break;
        case Pattern::noise: break;
        case Pattern::sparse_noise: value = (state >> 20) % 5 == 0 ? value : static_cast<std::uint32_t>(x / 9 * 13 + y); break;
        case Pattern::flat: value = channel == 3 ? 255U : 40U; break;
        }
        samples[index] = static_cast<std::byte>(value & 255U);
    }
    return samples;
}

struct Encoded final {
    stx_cng::CngEncodeResult result{};
    std::vector<std::byte> bytes;
};

inline Encoded encode(stx_cng::CngEncoder& encoder, stx_cng::CngImage image, std::span<const std::byte> samples,
                      std::uint64_t row_bytes) {
    Encoded encoded;
    encoded.bytes.resize(stx_cng::cng_encoded_bound(image));
    stx_cng::CngEncodeInput input{image, row_bytes, samples};
    encoded.result = encoder.encode(input, encoded.bytes);
    encoded.bytes.resize(encoded.result.encoded_bytes);
    return encoded;
}

inline std::uint64_t tight_row_bytes(stx_cng::CngImage image) {
    std::uint64_t bytes = static_cast<std::uint64_t>(image.width) * static_cast<std::size_t>(image.channels);
    return bytes;
}

inline int finish(const char* name) {
    if (failures != 0) {
        std::fprintf(stderr, "%s: %d failure(s)\n", name, failures);
        return EXIT_FAILURE;
    }
    std::printf("%s: pass\n", name);
    return EXIT_SUCCESS;
}

} // namespace stx_cng_test
