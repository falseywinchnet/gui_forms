#pragma once

#include "gui_forms/compression/inflate.hpp"

#include <cstddef>
#include <cstdint>
#include <span>

namespace gui_forms::image {

enum class PngDecodeError {
    none, invalid_argument, resource_limit, invalid_signature, truncated_chunk,
    invalid_chunk_type, crc_mismatch, invalid_header, invalid_order,
    invalid_palette, invalid_transparency, invalid_metadata, unknown_critical,
    missing_image, trailing_data, insufficient_storage, inflate_failure,
    invalid_filtered_size, invalid_filter, invalid_palette_index
};

// Provisional experiment limits, not a new production registry policy.
struct PngDecodeLimits final {
    std::uint64_t encoded_bytes = 32ULL * 1024 * 1024;
    std::uint64_t pixels = 4096ULL * 4096;
    std::uint64_t filtered_bytes = 129ULL * 1024 * 1024;
    std::uint64_t metadata_bytes = 4ULL * 1024 * 1024;
    compression::WorkCount inflate_work{512ULL * 1024 * 1024};
    std::uint32_t dimension = 4096;
    std::uint32_t chunks = 4096;
};

struct PngSampleLayout final {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    unsigned depth = 0;
    unsigned color = 0;
    bool interlaced = false;
    std::size_t rgba_samples = 0;
    std::size_t row_storage_bytes = 0;
    std::uint64_t filtered_bytes = 0;
    std::size_t metadata_records = 0;
};

// Offsets refer to the original encoded span, or the separate decoded metadata
// storage. No borrow is retained. A compressed field excludes its keyword and
// other uncompressed prefix, which remains available through encoded_offset.
struct PngMetadataRecord final {
    std::uint32_t type = 0;
    std::size_t encoded_offset = 0;
    std::size_t encoded_length = 0;
    bool compressed = false;
    std::size_t decoded_offset = 0;
    std::size_t decoded_length = 0;
};

struct PngDecodeResult final {
    PngDecodeError error = PngDecodeError::none;
    compression::InflateError inflate_error = compression::InflateError::none;
    PngSampleLayout layout{};
    std::size_t metadata_bytes = 0;
    compression::WorkCount inflate_work{};
};

// Structural inspection does not validate compressed contents or palette indices.
[[nodiscard]] PngDecodeResult inspect_png_samples(
    std::span<const std::byte> encoded, PngDecodeLimits limits) noexcept;

// Private lossless sample seam: native uint16 RGBA, top row first, straight alpha,
// no colour conversion. All spans and workspace must be mutually disjoint.
// Limits are copied at entry, so output cannot mutate the active policy.
// No allocation or retained borrow. One execution context per workspace.
// On any failure, discard all output (possibly partial). Structural validation
// and row/sample/record capacity checks precede mutation; compressed metadata
// exhaustion can follow partial output. Text encoding is checked without
// normalization; ICC profiles and language tags are not semantically validated.
[[nodiscard]] PngDecodeResult decode_png_samples(
    std::span<const std::byte> encoded, PngDecodeLimits limits,
    compression::InflateStream& workspace, std::span<std::byte> rows,
    std::span<std::uint16_t> rgba, std::span<std::byte> metadata,
    std::span<PngMetadataRecord> records) noexcept;

} // namespace gui_forms::image
