#pragma once

// The per-image rANS tables (FORMAT.md section 5) as the decoder holds them,
// and their reading from the tables part's Zstandard frame.
#include "format.hpp"
#include "storage.hpp"
#include "stx_cng/codec.hpp"

#include <array>
#include <cstdint>
#include <span>

struct ZSTD_DCtx_s;

namespace stx_cng::detail {

// One context's table: the frequencies and starts of its entries (the listed
// symbols, then the escape at index `escape`), and the entry owning each of
// the 4,096 slots. Only the first escape + 1 entries are meaningful.
struct DecodeTable final {
    std::array<std::uint16_t, maximum_table_entries> frequency{};
    std::array<std::uint16_t, maximum_table_entries> start{};
    std::array<std::uint16_t, total_frequency> slot{};
    std::uint32_t escape = 0;
    // False when the context codes nothing in this image: reading a symbol
    // from it makes the segment invalid.
    bool used = false;
};

using DecodeTables = std::array<DecodeTable, contexts>;

// What reading a tables frame needs, owned by a decoder: the static
// decompression context and the decompressed body.
struct TablesReader final {
    ContextStorage context_storage{};
    // Lives in context_storage; null until prepare_reader succeeds.
    ZSTD_DCtx_s* context = nullptr;
    std::array<std::uint8_t, maximum_tables_body> body{};
    NormalizeWorkspace normalize{};
};

// Allocates the static context once; false when allocation fails.
[[nodiscard]] bool prepare_reader(TablesReader& reader) noexcept;

// Exactly one complete Zstandard frame spanning `frame`, not a skippable one.
[[nodiscard]] bool single_frame(std::span<const std::uint8_t> frame) noexcept;

// Decompresses and parses a tables frame into `tables`. invalid_tables for a
// frame other than exactly one, a body that does not decompress or is longer
// than the format allows, an entry count above 572, a q byte above 25, or a
// body of any other length. On failure `tables` is unspecified.
[[nodiscard]] CngError read_tables(std::span<const std::uint8_t> frame, TablesReader& reader, DecodeTables& tables) noexcept;

} // namespace stx_cng::detail
