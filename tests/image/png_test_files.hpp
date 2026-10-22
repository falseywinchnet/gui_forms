#pragma once
// Builds small PNG files for tests: every legal colour type and depth,
// optional Adam7, filters cycling 0..4 by row, tRNS where legal, extra
// ancillary chunks before PLTE, and IDAT compressed by the owned encoder.
// Decoded pixels depend only on the raw scanlines, never on the encoder.
#include "../../src/core/image/png_writer.hpp"
#include "stx_zlib/checksum.hpp"
#include "stx_zlib/deflate.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <span>
#include <stdexcept>
#include <vector>

namespace gui_forms_png_files {
using Bytes = std::vector<std::byte>;

inline void require_file(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

inline void put32(Bytes& data, std::uint32_t value) {
    for (unsigned shift = 32; shift != 0; shift -= 8) {
        data.push_back(static_cast<std::byte>((value >> (shift - 8)) & 255U));
    }
}
inline void chunk(Bytes& data, const char* name, const Bytes& payload) {
    put32(data, static_cast<std::uint32_t>(payload.size()));
    const std::size_t begin = data.size();
    for (unsigned index = 0; index < 4; ++index) data.push_back(static_cast<std::byte>(name[index]));
    data.insert(data.end(), payload.begin(), payload.end());
    const std::span<const std::byte> checked(data.data() + begin, payload.size() + 4);
    put32(data, stx_zlib::crc32(checked));
}

// Compresses with the owned encoder; decoded pixels depend only on the raw
// bytes, so the identity hashes do not depend on the encoder's choices.
inline Bytes deflate(const Bytes& raw) {
    std::unique_ptr<stx_zlib::DeflateStream> stream = std::make_unique<stx_zlib::DeflateStream>();
    stx_zlib::DeflateOptions options{};
    options.input_limit.value = raw.size();
    options.output_limit.value = raw.size() * 2U + 1024U;
    options.work_limit.value = 1ULL << 40U;
    require_file((*stream).reset(options) == stx_zlib::DeflateError::none, "deflate reset failed");
    Bytes output(static_cast<std::size_t>(options.output_limit.value));
    std::size_t consumed = 0;
    std::size_t produced = 0;
    for (;;) {
        const std::span<const std::byte> input(raw.data() + consumed, raw.size() - consumed);
        const std::span<std::byte> space(output.data() + produced, output.size() - produced);
        const stx_zlib::DeflateResult step = (*stream).process(input, space, stx_zlib::DeflateFlush::finish);
        require_file(step.status != stx_zlib::DeflateStatus::error, "deflate failed");
        consumed += step.input_consumed;
        produced += step.output_produced;
        if (step.status == stx_zlib::DeflateStatus::stream_end) break;
    }
    output.resize(produced);
    return output;
}

struct Format final { unsigned color = 0; unsigned depth = 0; unsigned channels = 0; };
inline constexpr std::array<Format, 15> formats{{
    {0,1,1}, {0,2,1}, {0,4,1}, {0,8,1}, {0,16,1},
    {2,8,3}, {2,16,3}, {3,1,1}, {3,2,1}, {3,4,1}, {3,8,1},
    {4,8,2}, {4,16,2}, {6,8,4}, {6,16,4}}};

inline unsigned sample_at(unsigned x, unsigned y, unsigned channel, const Format& format) {
    const unsigned mask = format.depth == 16 ? 65535U : (1U << format.depth) - 1U;
    const unsigned value = (x * 7919U + y * 104729U + channel * 3571U + x * y * 31U) & mask;
    return value;
}

inline unsigned predictor(unsigned filter, unsigned a, unsigned b, unsigned c) {
    if (filter == 1) return a;
    if (filter == 2) return b;
    if (filter == 3) return (a + b) / 2;
    if (filter == 4) {
        const int p = static_cast<int>(a) + static_cast<int>(b) - static_cast<int>(c);
        const int pa = std::abs(p - static_cast<int>(a));
        const int pb = std::abs(p - static_cast<int>(b));
        const int pc = std::abs(p - static_cast<int>(c));
        if (pa <= pb && pa <= pc) return a;
        if (pb <= pc) return b;
        return c;
    }
    return 0;
}

// Filtered scanlines with the filter type cycling through 0..4 by row, so
// every reconstruction kernel contributes to the identity hashes.
inline Bytes scanlines(unsigned width, unsigned height, const Format& format, bool interlaced) {
    constexpr std::array<unsigned, 7> sx{0,4,0,2,0,1,0};
    constexpr std::array<unsigned, 7> sy{0,0,4,0,2,0,1};
    constexpr std::array<unsigned, 7> dx{8,8,4,4,2,2,1};
    constexpr std::array<unsigned, 7> dy{8,8,8,4,4,2,2};
    Bytes raw{};
    const unsigned passes = interlaced ? 7U : 1U;
    const unsigned stride = std::max(1U, format.channels * format.depth / 8U);
    unsigned row_number = 0;
    for (unsigned pass = 0; pass < passes; ++pass) {
        const unsigned start_x = interlaced ? sx[pass] : 0U;
        const unsigned start_y = interlaced ? sy[pass] : 0U;
        const unsigned step_x = interlaced ? dx[pass] : 1U;
        const unsigned step_y = interlaced ? dy[pass] : 1U;
        if (start_x >= width || start_y >= height) continue;
        const unsigned count = (width - start_x + step_x - 1U) / step_x;
        const std::size_t row_bytes = (count * format.channels * format.depth + 7U) / 8U;
        Bytes previous(row_bytes, std::byte{0});
        for (unsigned y = start_y; y < height; y += step_y) {
            Bytes row(row_bytes, std::byte{0});
            std::size_t index = 0;
            for (unsigned x = start_x; x < width; x += step_x) {
                for (unsigned channel = 0; channel < format.channels; ++channel) {
                    const unsigned value = sample_at(x, y, channel, format);
                    if (format.depth == 16) {
                        row[index * 2U] = static_cast<std::byte>(value >> 8U);
                        row[index * 2U + 1U] = static_cast<std::byte>(value & 255U);
                    } else if (format.depth == 8) {
                        row[index] = static_cast<std::byte>(value);
                    } else {
                        const std::size_t bit = index * format.depth;
                        const unsigned shift = 8U - format.depth - static_cast<unsigned>(bit % 8U);
                        row[bit / 8U] |= static_cast<std::byte>(value << shift);
                    }
                    ++index;
                }
            }
            const unsigned filter = row_number % 5U;
            ++row_number;
            raw.push_back(static_cast<std::byte>(filter));
            for (std::size_t offset = 0; offset < row_bytes; ++offset) {
                const unsigned a = offset < stride ? 0U : std::to_integer<unsigned>(row[offset - stride]);
                const unsigned b = std::to_integer<unsigned>(previous[offset]);
                const unsigned c = offset < stride ? 0U : std::to_integer<unsigned>(previous[offset - stride]);
                const unsigned current = std::to_integer<unsigned>(row[offset]);
                raw.push_back(static_cast<std::byte>((current - predictor(filter, a, b, c)) & 255U));
            }
            previous = row;
        }
    }
    return raw;
}

struct Extra final {
    const char* type = nullptr;
    Bytes payload{};
};

inline Bytes png_file(unsigned width, unsigned height, const Format& format, bool interlaced,
               std::span<const Extra> extras) {
    Bytes png{std::byte{137},std::byte{80},std::byte{78},std::byte{71},
              std::byte{13},std::byte{10},std::byte{26},std::byte{10}};
    Bytes ihdr{};
    put32(ihdr, width);
    put32(ihdr, height);
    ihdr.push_back(static_cast<std::byte>(format.depth));
    ihdr.push_back(static_cast<std::byte>(format.color));
    ihdr.push_back(std::byte{0});
    ihdr.push_back(std::byte{0});
    ihdr.push_back(interlaced ? std::byte{1} : std::byte{0});
    chunk(png, "IHDR", ihdr);
    for (const Extra& extra : extras) chunk(png, extra.type, extra.payload);
    if (format.color == 3) {
        Bytes palette{};
        Bytes alpha{};
        const unsigned entries = 1U << format.depth;
        for (unsigned index = 0; index < entries; ++index) {
            palette.push_back(static_cast<std::byte>((index * 37U) & 255U));
            palette.push_back(static_cast<std::byte>(255U - index));
            palette.push_back(static_cast<std::byte>((index * 101U) & 255U));
            alpha.push_back(static_cast<std::byte>((index * 59U) & 255U));
        }
        chunk(png, "PLTE", palette);
        chunk(png, "tRNS", alpha);
    } else if (format.color == 0) {
        Bytes key{};
        const unsigned value = sample_at(1, 1, 0, format);
        key.push_back(static_cast<std::byte>(value >> 8U));
        key.push_back(static_cast<std::byte>(value & 255U));
        chunk(png, "tRNS", key);
    } else if (format.color == 2) {
        Bytes key{};
        for (unsigned channel = 0; channel < 3; ++channel) {
            const unsigned value = sample_at(2, 1, channel, format);
            key.push_back(static_cast<std::byte>(value >> 8U));
            key.push_back(static_cast<std::byte>(value & 255U));
        }
        chunk(png, "tRNS", key);
    }
    chunk(png, "IDAT", deflate(scanlines(width, height, format, interlaced)));
    chunk(png, "IEND", {});
    return png;
}

inline Bytes gamma_payload(std::uint32_t gamma) {
    Bytes payload{};
    put32(payload, gamma);
    return payload;
}
inline Bytes chromaticity_payload() {
    Bytes payload{};
    const std::array<std::uint32_t, 8> values{31270, 32900, 64000, 33000, 30000, 60000, 15000, 6000};
    for (const std::uint32_t value : values) put32(payload, value);
    return payload;
}

// An iCCP chunk payload: keyword "p", method 0, and a small compressed body.
// The body is not a valid ICC profile; GUI.Forms never interprets one.
inline Bytes icc_profile_payload() {
    Bytes profile{std::byte{'p'}, std::byte{0}, std::byte{0}};
    const Bytes compressed = deflate(Bytes(128, std::byte{0x41}));
    profile.insert(profile.end(), compressed.begin(), compressed.end());
    return profile;
}

// The file with every chunk of type `name` removed. Requires a well-formed
// file (every chunk envelope inside the span).
inline Bytes without_chunk(const Bytes& png, const char* name) {
    Bytes result(png.begin(), png.begin() + 8);
    std::size_t offset = 8;
    while (offset + 12U <= png.size()) {
        const std::size_t length = (std::to_integer<std::size_t>(png[offset]) << 24U) |
            (std::to_integer<std::size_t>(png[offset + 1U]) << 16U) |
            (std::to_integer<std::size_t>(png[offset + 2U]) << 8U) |
            std::to_integer<std::size_t>(png[offset + 3U]);
        bool same = true;
        for (std::size_t index = 0; index < 4U; ++index) {
            same = same && png[offset + 4U + index] == static_cast<std::byte>(name[index]);
        }
        const std::size_t next = offset + length + 12U;
        if (!same) {
            result.insert(result.end(), png.begin() + static_cast<std::ptrdiff_t>(offset),
                          png.begin() + static_cast<std::ptrdiff_t>(next));
        }
        offset = next;
    }
    return result;
}

// The file with one chunk inserted straight after IHDR (8 + 25 bytes).
inline Bytes with_chunk_after_header(const Bytes& png, const char* name, const Bytes& payload) {
    Bytes result(png.begin(), png.begin() + 33);
    chunk(result, name, payload);
    result.insert(result.end(), png.begin() + 33, png.end());
    return result;
}

} // namespace gui_forms_png_files
