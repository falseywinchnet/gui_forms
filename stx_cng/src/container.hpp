#pragma once

// Reading the CNG 1.0 container (FORMAT.md section 3): the header fields, the
// directory, the tables and mosaic copies and the payload region. Nothing here
// decompresses or decodes; spans borrow the file.
#include "format.hpp"
#include "stx_cng/codec.hpp"

#include <cstdint>
#include <span>

namespace stx_cng::detail {

// How strictly the container is read.
enum class ContainerReading : std::uint8_t {
    // Section 3.4: the signature, then the first intact copy of each part.
    decoder,
    // Section 10: as decoder, without the signature.
    salvage
};

struct Container final {
    Shape shape{};
    std::uint64_t segments = 0;
    // The intact directory copy: 8 bytes per segment (length, CRC32).
    std::span<const std::uint8_t> directory{};
    // The intact tables copy.
    std::span<const std::uint8_t> tables{};
    // An intact mosaic copy; empty when both are damaged.
    std::span<const std::uint8_t> mosaic{};
    // The payload region: segment payloads back to back, in segment order.
    std::size_t payload_begin = 0;
    std::size_t payload_end = 0;
};

// Reads the container. invalid_signature, invalid_header (no intact header
// copy, or fields out of range), unsupported_version, invalid_directory (no
// intact directory, or lengths that do not account for the payload region)
// or invalid_tables (no intact tables copy). Segment checksums are not
// checked here.
[[nodiscard]] CngError read_container(std::span<const std::uint8_t> file, ContainerReading reading, Container& container) noexcept;

// Strict framing of validate_cng_framing; fills `container` on success.
[[nodiscard]] CngError validate_framing(std::span<const std::uint8_t> file, Container& container) noexcept;

// Segment `index`'s payload length and CRC32 from the directory.
[[nodiscard]] inline std::uint32_t payload_length(const Container& container, std::uint64_t index) noexcept {
    std::uint32_t length = read_u32(container.directory, static_cast<std::size_t>(index) * directory_entry_bytes);
    return length;
}

[[nodiscard]] inline std::uint32_t payload_crc(const Container& container, std::uint64_t index) noexcept {
    std::uint32_t crc = read_u32(container.directory, static_cast<std::size_t>(index) * directory_entry_bytes + 4);
    return crc;
}

} // namespace stx_cng::detail
