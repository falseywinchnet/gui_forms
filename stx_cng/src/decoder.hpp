#pragma once

// Decoding one tile (a segment or the mosaic, FORMAT.md section 7).
#include "format.hpp"
#include "storage.hpp"
#include "tables.hpp"

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace stx_cng::detail {

// Storage for decoding one tile at a time. Every residual is written before
// it is read (matches and the activity read only earlier pixels), so only the
// energy ring is cleared between tiles.
struct TileWorkspace final {
    // Per pixel of the tile: the plane residuals, one byte each.
    std::vector<std::uint32_t> residuals;
    // Three padded rows of packed energies: 3 * (width + 3) words.
    std::vector<std::uint64_t> energy;
    // Per column: the row-above part of every pixel's activity sum.
    std::vector<std::uint64_t> above;
};

// Grows the workspace for a tile of this shape; false when allocation fails.
[[nodiscard]] bool reserve_tile(TileWorkspace& workspace, Shape tile) noexcept;

// Decodes one payload of `tile` shape into `destination`, the tile's top-left
// sample in an interleaved image whose rows are `row_bytes` apart. The
// workspace must be reserved for the shape. False when the payload is
// invalid, leaving the tile's rectangle partly written.
[[nodiscard]] bool decode_tile(const DecodeTables& tables, std::span<const std::uint8_t> payload, Shape tile,
                               TileWorkspace& workspace, std::uint8_t* destination, std::size_t row_bytes) noexcept;

} // namespace stx_cng::detail
