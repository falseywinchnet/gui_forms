#pragma once

// CNG 1.0 format constants and the definitions the encoder and the decoder
// share: container layout, contexts and alphabets, residual and prefix codes,
// checksums and table normalisation. docs/FORMAT.md is the specification and
// every definition here follows it; section numbers refer to it.
#include "stx_zlib/checksum.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <span>

namespace stx_cng::detail {

// ---------------------------------------------------------------- container (section 3)
inline constexpr std::array<std::uint8_t, 8> image_signature{0x89, 0x43, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
inline constexpr std::uint8_t version = 1;
// Width, height, channels, version, segment width and height, then the
// mosaic's and the tables' lengths and CRC32s.
inline constexpr std::size_t field_bytes = 28;
// The fields with their CRC32: the block that is stored twice.
inline constexpr std::size_t field_block_bytes = field_bytes + 4;
inline constexpr std::size_t front_bytes = image_signature.size() + field_block_bytes;
inline constexpr std::uint32_t segment_columns = 128;
inline constexpr std::uint32_t segment_rows = 32;
inline constexpr std::uint32_t cell = 16;
inline constexpr std::uint64_t maximum_samples = std::uint64_t{1} << 31;
// Bytes of one directory entry: payload length and CRC32.
inline constexpr std::size_t directory_entry_bytes = 8;
// The reference encoder's Zstandard level for the tables frame (section 11).
inline constexpr int tables_level = 19;

// ---------------------------------------------------------------- rANS (section 6)
inline constexpr std::uint32_t precision = 12;
inline constexpr std::uint32_t total_frequency = std::uint32_t{1} << precision;
inline constexpr std::uint32_t lower_bound = std::uint32_t{1} << 23;
inline constexpr std::uint32_t maximum_raw_bits = precision;

// ---------------------------------------------------------------- contexts and alphabets (section 7)
// Planes are numbered in sample order: 0 blue, 1 green, 2 red, 3 alpha.
inline constexpr std::uint32_t blue_plane = 0;
inline constexpr std::uint32_t green_plane = 1;
inline constexpr std::uint32_t red_plane = 2;
inline constexpr std::uint32_t alpha_plane = 3;
inline constexpr std::uint32_t maximum_channels = 4;
// Payload stream order: green, blue, red, alpha.
inline constexpr std::array<std::uint32_t, maximum_channels> stream_planes{green_plane, blue_plane, red_plane, alpha_plane};

inline constexpr std::uint32_t classes = 5;
inline constexpr std::uint32_t blue_context0 = classes;
inline constexpr std::uint32_t red_context0 = blue_context0 + 3 * classes;
inline constexpr std::uint32_t alpha_context0 = red_context0 + 3 * classes;
inline constexpr std::uint32_t distance_context = alpha_context0 + 2;
inline constexpr std::uint32_t contexts = distance_context + 1;
// Classes at or above this code blue and red as differences from green.
inline constexpr std::uint32_t gate_class = 3;

inline constexpr std::uint32_t minimum_length = 2;
inline constexpr std::uint32_t length_codes = 30;
inline constexpr std::uint32_t repeats = 3;
inline constexpr std::uint32_t specials = 6;
inline constexpr std::uint32_t distance_symbols = repeats + specials + length_codes;
// Green alphabet: literals, whole-pixel match lengths, cache hits, green
// match lengths.
inline constexpr std::uint32_t literal_symbols = 256;
inline constexpr std::uint32_t whole_length0 = literal_symbols;
inline constexpr std::uint32_t cache0 = whole_length0 + length_codes;
inline constexpr std::uint32_t cache_symbols = 256;
inline constexpr std::uint32_t green_only0 = cache0 + cache_symbols;
inline constexpr std::uint32_t green_symbols = green_only0 + length_codes;
// Blue, red and alpha alphabet: literals, then plane match lengths.
inline constexpr std::uint32_t plane_length0 = literal_symbols;
// A context table lists at most every green symbol, then the escape.
inline constexpr std::size_t maximum_table_entries = green_symbols + 1;
// The tables body: per context a u16 entry count and at most 573 q bytes.
inline constexpr std::size_t maximum_tables_body = contexts * (2 + maximum_table_entries);
inline constexpr std::uint8_t maximum_quantized = 25;

// level[q] for q = 1 to 25: max(1, round(2^((q - 1) / 2))) (section 5.1).
inline constexpr std::array<std::uint16_t, 25> levels{1, 1, 2, 3, 4, 6, 8, 11, 16, 23, 32, 45, 64, 91, 128, 181, 256,
                                                      362, 512, 724, 1024, 1448, 2048, 2896, 4096};

// ---------------------------------------------------------------- bytes and checksums
[[nodiscard]] inline std::uint32_t read_u32(std::span<const std::uint8_t> bytes, std::size_t offset) noexcept {
    std::uint32_t value = 0;
    for (std::size_t index = 0; index < 4; ++index) {
        value = (value << 8) | bytes[offset + index];
    }
    return value;
}

inline void write_u32(std::span<std::uint8_t> bytes, std::size_t offset, std::uint32_t value) noexcept {
    for (std::size_t index = 0; index < 4; ++index) {
        bytes[offset + index] = static_cast<std::uint8_t>(value >> (24 - 8 * index));
    }
}

// The format's CRC32 is zlib's, which stx_zlib implements once.
[[nodiscard]] inline std::uint32_t crc_of(std::span<const std::uint8_t> bytes) noexcept {
    std::span<const std::byte> view(reinterpret_cast<const std::byte*>(bytes.data()), bytes.size());
    std::uint32_t crc = stx_zlib::crc32(view);
    return crc;
}

[[nodiscard]] inline std::uint32_t crc_extend(std::uint32_t crc, std::span<const std::uint8_t> bytes) noexcept {
    std::span<const std::byte> view(reinterpret_cast<const std::byte*>(bytes.data()), bytes.size());
    std::uint32_t extended = stx_zlib::crc32_update(crc, view);
    return extended;
}

// A block of `body` bytes followed by their CRC32.
[[nodiscard]] inline bool block_valid(std::span<const std::uint8_t> block, std::size_t body) noexcept {
    if (block.size() < body + 4) {
        return false;
    }
    bool valid = read_u32(block, body) == crc_of(block.subspan(0, body));
    return valid;
}

// ---------------------------------------------------------------- residuals and codes
// The signed byte with the low eight bits of `value`.
[[nodiscard]] inline std::int32_t wrap(std::int32_t value) noexcept {
    std::int32_t wrapped = static_cast<std::int8_t>(static_cast<std::uint8_t>(value & 255));
    return wrapped;
}

// 0, -1, 1, -2, 2, ... to 0, 1, 2, 3, 4, ...
[[nodiscard]] inline std::uint32_t zig(std::int32_t value) noexcept {
    std::uint32_t symbol = value >= 0 ? static_cast<std::uint32_t>(2 * value) : static_cast<std::uint32_t>(-2 * value - 1);
    return symbol;
}

[[nodiscard]] inline std::int32_t unzig(std::uint32_t symbol) noexcept {
    std::int32_t magnitude = static_cast<std::int32_t>(symbol >> 1);
    std::int32_t sign = -static_cast<std::int32_t>(symbol & 1U);
    std::int32_t value = magnitude ^ sign;
    return value;
}

// Raw bits of an escaped symbol (section 5.2).
[[nodiscard]] inline std::uint32_t raw_bits(std::uint32_t context) noexcept {
    if (context < blue_context0) {
        return 10;
    }
    if (context == distance_context) {
        return 6;
    }
    return 9;
}

// Extra bits of prefix code `code` (section 7.6).
[[nodiscard]] inline std::uint32_t code_bits(std::uint32_t code) noexcept {
    std::uint32_t bits = code < 4 ? 0 : (code >> 1) - 1;
    return bits;
}

[[nodiscard]] inline std::uint32_t unprefix(std::uint32_t code, std::uint32_t extra) noexcept {
    if (code < 4) {
        return code;
    }
    std::uint32_t half = code >> 1;
    std::uint32_t value = ((2U | (code & 1U)) << (half - 1)) | extra;
    return value;
}

struct PrefixCode final {
    std::uint32_t code = 0;
    std::uint32_t bits = 0;
    std::uint32_t extra = 0;
};

[[nodiscard]] inline PrefixCode prefix(std::uint32_t value) noexcept {
    PrefixCode result;
    if (value < 4) {
        result.code = value;
        return result;
    }
    // floor(log2(value)), at least 2 here.
    std::uint32_t half = static_cast<std::uint32_t>(std::bit_width(value)) - 1;
    result.bits = half - 1;
    result.code = 2 * half + ((value >> result.bits) & 1U);
    result.extra = value & ((std::uint32_t{1} << result.bits) - 1);
    return result;
}

// ---------------------------------------------------------------- table normalisation (section 5.1)
// Fixed storage for normalising one context: no allocation per table.
struct NormalizeWorkspace final {
    std::array<std::uint32_t, maximum_table_entries> scaled{};
    // Sort keys: (4096 - scaled) above the entry index, so ascending keys are
    // descending scaled values with equal values in index order.
    std::array<std::uint32_t, maximum_table_entries> order{};
};

// Fills frequency[0, q.size()) with the frequencies of q (each q at most 25,
// at most maximum_table_entries entries). Returns false, leaving frequency
// unspecified, when every q is 0: the context is unused.
inline bool normalize(std::span<const std::uint8_t> q, std::span<std::uint16_t> frequency, NormalizeWorkspace& workspace) noexcept {
    std::size_t count = q.size();
    std::uint64_t total = 0;
    for (std::size_t entry = 0; entry < count; ++entry) {
        std::uint32_t level = q[entry] == 0 ? 0U : levels[q[entry] - 1U];
        workspace.scaled[entry] = level;
        total += level;
    }
    if (total == 0) {
        return false;
    }
    std::int64_t sum = 0;
    for (std::size_t entry = 0; entry < count; ++entry) {
        std::uint64_t level = workspace.scaled[entry];
        if (level != 0) {
            std::uint64_t share = level * total_frequency / total;
            workspace.scaled[entry] = static_cast<std::uint32_t>(std::max<std::uint64_t>(1, share));
        }
        sum += workspace.scaled[entry];
        workspace.order[entry] = ((total_frequency - workspace.scaled[entry]) << 10) | static_cast<std::uint32_t>(entry);
    }
    std::sort(workspace.order.begin(), workspace.order.begin() + static_cast<std::ptrdiff_t>(count));
    std::int64_t difference = static_cast<std::int64_t>(total_frequency) - sum;
    // Every entry gets at least one slot and there are at most 573, so a
    // negative difference is repaid from entries above one in at most a few
    // cycles of the order.
    std::size_t step = 0;
    while (difference != 0) {
        std::size_t entry = workspace.order[step % count] & 1023U;
        std::int64_t scaled = workspace.scaled[entry];
        if (difference > 0) {
            workspace.scaled[entry] = static_cast<std::uint32_t>(scaled + difference);
            difference = 0;
        } else if (scaled > 1) {
            std::int64_t taken = std::min<std::int64_t>(scaled - 1, -difference);
            workspace.scaled[entry] = static_cast<std::uint32_t>(scaled - taken);
            difference += taken;
        }
        ++step;
    }
    for (std::size_t entry = 0; entry < count; ++entry) {
        frequency[entry] = static_cast<std::uint16_t>(workspace.scaled[entry]);
    }
    return true;
}

// ---------------------------------------------------------------- image geometry
struct Shape final {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t channels = 0;
};

[[nodiscard]] inline bool admitted_shape(Shape shape) noexcept {
    bool channels = shape.channels == 3 || shape.channels == 4;
    bool sides = shape.width >= 1 && shape.height >= 1;
    bool samples = static_cast<std::uint64_t>(shape.width) * shape.height * shape.channels <= maximum_samples;
    bool admitted = channels && sides && samples;
    return admitted;
}

[[nodiscard]] inline std::uint64_t segments_across(Shape shape) noexcept {
    std::uint64_t across = (static_cast<std::uint64_t>(shape.width) + segment_columns - 1) / segment_columns;
    return across;
}

[[nodiscard]] inline std::uint64_t segment_count(Shape shape) noexcept {
    std::uint64_t down = (static_cast<std::uint64_t>(shape.height) + segment_rows - 1) / segment_rows;
    std::uint64_t count = segments_across(shape) * down;
    return count;
}

[[nodiscard]] inline Shape mosaic_shape(Shape shape) noexcept {
    Shape mosaic;
    mosaic.width = (shape.width + cell - 1) / cell;
    mosaic.height = (shape.height + cell - 1) / cell;
    mosaic.channels = shape.channels;
    return mosaic;
}

// The rectangle of segment `index` within the image.
struct SegmentRectangle final {
    std::uint32_t x = 0;
    std::uint32_t y = 0;
    Shape shape{};
};

[[nodiscard]] inline SegmentRectangle segment_rectangle(Shape image, std::uint64_t index) noexcept {
    std::uint64_t across = segments_across(image);
    SegmentRectangle rectangle;
    rectangle.x = static_cast<std::uint32_t>(index % across) * segment_columns;
    rectangle.y = static_cast<std::uint32_t>(index / across) * segment_rows;
    rectangle.shape.width = std::min(segment_columns, image.width - rectangle.x);
    rectangle.shape.height = std::min(segment_rows, image.height - rectangle.y);
    rectangle.shape.channels = image.channels;
    return rectangle;
}

} // namespace stx_cng::detail
