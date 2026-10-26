#pragma once

// Shared fixtures for the stx_cng tests: deterministic generated images, the
// mosaic definition, and encode and decode helpers. Everything is in memory
// except the conformance vectors, which are read from tests/vectors.
#include "stx_cng/codec.hpp"
#include "stx_zlib/checksum.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <span>
#include <vector>

namespace stx_cng_test {

using Bytes = std::vector<std::byte>;

inline int failures = 0;

inline void check(bool condition, const char* what) {
    if (!condition) {
        std::fprintf(stderr, "FAIL: %s\n", what);
        ++failures;
    }
}

inline int finish(const char* name) {
    if (failures != 0) {
        std::fprintf(stderr, "%s: %d failure(s)\n", name, failures);
        return EXIT_FAILURE;
    }
    std::printf("%s: pass\n", name);
    return EXIT_SUCCESS;
}

// The upstream round-trip patterns: gradients, sharp steps, a 7-pixel
// periodic tile, noise, gradients with alpha steps and colour beneath alpha
// 0, and a few flat colours with noise patches.
enum class Pattern : std::uint8_t { gradient, steps, tile, noise, alpha_steps, patches };
inline constexpr Pattern patterns[] = {Pattern::gradient, Pattern::steps, Pattern::tile,
                                       Pattern::noise, Pattern::alpha_steps, Pattern::patches};

inline std::uint32_t pattern_value(Pattern pattern, std::uint32_t state, std::size_t x, std::size_t y, std::size_t channel) {
    std::uint32_t gradient = static_cast<std::uint32_t>(3 * x + 2 * y + 61 * channel);
    std::uint32_t noise = state >> (8 * channel);
    switch (pattern) {
    case Pattern::gradient:
        return gradient;
    case Pattern::steps:
        return static_cast<std::uint32_t>(((x / 9 + y / 5) % 3) * 100 + channel * 20);
    case Pattern::tile:
        return static_cast<std::uint32_t>((x % 7) * 37 + (y % 7) * 11 + channel * 50);
    case Pattern::noise:
        return noise;
    case Pattern::alpha_steps: {
        if (channel != 3) {
            return gradient;
        }
        std::size_t step = (x / 13) % 3;
        std::uint32_t alpha = step == 0 ? 0U : (step == 1 ? 255U : 128U);
        return alpha;
    }
    case Pattern::patches: {
        bool patch = (x / 6 + y / 6) % 11 == 0;
        std::uint32_t flat = static_cast<std::uint32_t>(((x / 40 + y / 30) % 2) * 180 + channel * 10);
        return patch ? noise : flat;
    }
    default:
        // A value outside the enumeration: the gradient, after the switch.
        break;
    }
    return gradient;
}

// Tightly packed samples of `image` following `pattern`; the seed varies noise.
inline Bytes generate(stx_cng::CngImage image, Pattern pattern, std::uint32_t seed) {
    std::size_t channels = static_cast<std::size_t>(image.channels);
    Bytes samples(static_cast<std::size_t>(image.width) * image.height * channels);
    std::uint32_t state = seed * 2654435761U + 12345U;
    for (std::size_t y = 0; y < image.height; ++y) {
        for (std::size_t x = 0; x < image.width; ++x) {
            state = state * 1664525U + 1013904223U;
            for (std::size_t channel = 0; channel < channels; ++channel) {
                std::uint32_t value = pattern_value(pattern, state, x, y, channel);
                samples[(y * image.width + x) * channels + channel] = static_cast<std::byte>(value & 255U);
            }
        }
    }
    return samples;
}

// The mosaic: the rounded mean of every 16 x 16 cell over the pixels inside
// the image (FORMAT.md section 9).
inline Bytes block_means(const Bytes& samples, stx_cng::CngImage image) {
    stx_cng::CngImage mosaic = stx_cng::cng_mosaic_image(image);
    std::size_t channels = static_cast<std::size_t>(image.channels);
    Bytes out(static_cast<std::size_t>(mosaic.width) * mosaic.height * channels);
    for (std::size_t cell_y = 0; cell_y < mosaic.height; ++cell_y) {
        for (std::size_t cell_x = 0; cell_x < mosaic.width; ++cell_x) {
            for (std::size_t channel = 0; channel < channels; ++channel) {
                std::uint64_t sum = 0;
                std::uint64_t count = 0;
                std::size_t y_end = std::min<std::size_t>(image.height, cell_y * 16 + 16);
                std::size_t x_end = std::min<std::size_t>(image.width, cell_x * 16 + 16);
                for (std::size_t y = cell_y * 16; y < y_end; ++y) {
                    for (std::size_t x = cell_x * 16; x < x_end; ++x) {
                        sum += std::to_integer<std::uint64_t>(samples[(y * image.width + x) * channels + channel]);
                        ++count;
                    }
                }
                out[(cell_y * mosaic.width + cell_x) * channels + channel] = static_cast<std::byte>((sum + count / 2) / count);
            }
        }
    }
    return out;
}

inline std::uint64_t tight_row_bytes(stx_cng::CngImage image) {
    std::uint64_t bytes = static_cast<std::uint64_t>(image.width) * static_cast<std::size_t>(image.channels);
    return bytes;
}

struct Encoded final {
    stx_cng::CngEncodeResult result{};
    Bytes bytes;
};

inline Encoded encode(stx_cng::CngEncoder& encoder, stx_cng::CngImage image, std::span<const std::byte> samples,
                      std::uint64_t row_bytes, std::uint32_t threads) {
    Encoded encoded;
    encoded.bytes.resize(static_cast<std::size_t>(stx_cng::cng_encoded_bound(image)));
    stx_cng::CngEncodeInput input{image, row_bytes, samples, threads};
    encoded.result = encoder.encode(input, encoded.bytes);
    encoded.bytes.resize(encoded.result.encoded_bytes);
    return encoded;
}

// A complete file of tightly packed samples; empty when encoding fails.
inline Bytes encode_file(stx_cng::CngImage image, const Bytes& samples) {
    std::unique_ptr<stx_cng::CngEncoder> encoder = std::make_unique<stx_cng::CngEncoder>();
    Encoded encoded = encode(*encoder, image, samples, tight_row_bytes(image), 1);
    return encoded.bytes;
}

struct Decoded final {
    stx_cng::CngDecodeResult result{};
    Bytes samples;
};

// Decodes into tightly packed samples sized from the probe.
inline Decoded decode(stx_cng::CngDecoder& decoder, std::span<const std::byte> file, std::uint32_t threads) {
    Decoded decoded;
    stx_cng::CngInspectResult probe = stx_cng::inspect_cng(file);
    if (probe.error != stx_cng::CngError::none) {
        decoded.result.error = probe.error;
        return decoded;
    }
    decoded.samples.resize(static_cast<std::size_t>(stx_cng::cng_sample_bytes(probe.image)));
    decoded.result = decoder.decode(file, decoded.samples, tight_row_bytes(probe.image), threads);
    return decoded;
}

// ------------------------------------------------------------ file anatomy
// A CNG file's parts (FORMAT.md section 3), and their reassembly with every
// checksum recomputed, so a crafted file passes framing and its hostile
// content reaches the decoding stages.

inline std::uint32_t crc_of(const Bytes& bytes) {
    std::uint32_t crc = stx_zlib::crc32(bytes);
    return crc;
}

inline void append_u32(Bytes& out, std::uint32_t value) {
    for (int shift = 24; shift >= 0; shift -= 8) {
        out.push_back(static_cast<std::byte>((value >> shift) & 255U));
    }
}

inline void write_u32(Bytes& out, std::size_t at, std::uint32_t value) {
    for (std::size_t index = 0; index < 4; ++index) {
        out[at + index] = static_cast<std::byte>((value >> (24 - 8 * index)) & 255U);
    }
}

inline std::uint32_t read_u32(const Bytes& bytes, std::size_t at) {
    std::uint32_t value = 0;
    for (std::size_t index = 0; index < 4; ++index) {
        value = (value << 8) | std::to_integer<std::uint32_t>(bytes[at + index]);
    }
    return value;
}

inline constexpr std::uint8_t signature[8] = {0x89, 0x43, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
inline constexpr std::size_t front_bytes = 40;       // signature, 28 field bytes, their CRC
inline constexpr std::size_t field_block_bytes = 32;  // the 28 field bytes and their CRC

struct Parts final {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t channels = 0;
    std::uint8_t version = 1;
    std::uint8_t segment_columns = 128;
    std::uint8_t segment_rows = 32;
    Bytes tables;
    Bytes mosaic;
    std::vector<Bytes> payloads;
};

inline std::size_t segments_of(std::uint32_t width, std::uint32_t height) {
    std::size_t segments = static_cast<std::size_t>((width + 127) / 128) * ((height + 31) / 32);
    return segments;
}

inline Bytes slice(const Bytes& bytes, std::size_t at, std::size_t count) {
    Bytes part(bytes.begin() + static_cast<std::ptrdiff_t>(at), bytes.begin() + static_cast<std::ptrdiff_t>(at + count));
    return part;
}

// The parts of a well-formed file.
inline Parts split(const Bytes& file) {
    Parts parts;
    parts.width = read_u32(file, 8);
    parts.height = read_u32(file, 12);
    parts.channels = std::to_integer<std::uint32_t>(file[16]);
    std::uint32_t mosaic_bytes = read_u32(file, 20);
    std::uint32_t tables_bytes = read_u32(file, 28);
    std::size_t segments = segments_of(parts.width, parts.height);
    std::size_t at = front_bytes + 8 * segments + 4;
    parts.tables = slice(file, at, tables_bytes);
    at += tables_bytes;
    parts.mosaic = slice(file, at, mosaic_bytes);
    at += mosaic_bytes;
    for (std::size_t index = 0; index < segments; ++index) {
        std::uint32_t length = read_u32(file, front_bytes + 8 * index);
        parts.payloads.push_back(slice(file, at, length));
        at += length;
    }
    return parts;
}

inline void append(Bytes& out, const Bytes& bytes) {
    out.insert(out.end(), bytes.begin(), bytes.end());
}

inline Bytes assemble(const Parts& parts) {
    Bytes fields;
    append_u32(fields, parts.width);
    append_u32(fields, parts.height);
    fields.push_back(static_cast<std::byte>(parts.channels));
    fields.push_back(static_cast<std::byte>(parts.version));
    fields.push_back(static_cast<std::byte>(parts.segment_columns));
    fields.push_back(static_cast<std::byte>(parts.segment_rows));
    append_u32(fields, static_cast<std::uint32_t>(parts.mosaic.size()));
    append_u32(fields, crc_of(parts.mosaic));
    append_u32(fields, static_cast<std::uint32_t>(parts.tables.size()));
    append_u32(fields, crc_of(parts.tables));
    append_u32(fields, crc_of(fields));
    Bytes directory;
    for (const Bytes& payload : parts.payloads) {
        append_u32(directory, static_cast<std::uint32_t>(payload.size()));
        append_u32(directory, crc_of(payload));
    }
    append_u32(directory, crc_of(directory));
    Bytes out;
    for (std::uint8_t value : signature) {
        out.push_back(static_cast<std::byte>(value));
    }
    append(out, fields);
    append(out, directory);
    append(out, parts.tables);
    append(out, parts.mosaic);
    for (const Bytes& payload : parts.payloads) {
        append(out, payload);
    }
    append(out, parts.mosaic);
    if (parts.payloads.size() > 1) {
        append(out, parts.tables);
    }
    append(out, directory);
    append(out, fields);
    return out;
}

// Deterministic generator for the sweeps.
struct Random final {
    std::uint64_t state = 0;
};

inline std::uint32_t next(Random& random) {
    random.state = random.state * 6364136223846793005ULL + 1442695040888963407ULL;
    std::uint32_t value = static_cast<std::uint32_t>(random.state >> 33);
    return value;
}

inline std::uint32_t below(Random& random, std::uint32_t bound) {
    std::uint32_t value = bound == 0 ? 0 : next(random) % bound;
    return value;
}

inline Bytes random_bytes(Random& random, std::size_t count) {
    Bytes out(count);
    for (std::byte& value : out) {
        value = static_cast<std::byte>(next(random) & 255U);
    }
    return out;
}

} // namespace stx_cng_test
