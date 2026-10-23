#pragma once

#include "../../display/chunk/display_chunk.hpp"

#include "gui_forms/engine/part_table/engine_part_table.hpp"
#include "gui_forms/engine/types/engine_types.hpp"
#include "gui_forms/types/geometry/geometry.hpp"

#include <cstdint>

// The window's backplane image (E9-B; owner, 2026-10-22, option 1): the core
// bakes it from the window's backplane material (EnginePartTable::
// bake_backplane_material) and then composites, here, the engine part
// rasters the backplane plane's display chunks recorded. Every other
// drawing command in those chunks (text, fills, images and the rest that a
// host painter draws) is skipped and counted, so that glass shows what it
// omits (RasterCacheStatistics::backplane_skipped_commands). A host
// capability that replays the whole backplane plane (option 2) is T5's
// upgrade behind the same borrow.

namespace gui_forms::detail {

// Composites the part rasters of `chunk` into `image`, which covers
// `region`, source-over at their opacity and their groups' opacity, each
// raster mapped to its device rectangle by its nearest pixel. The chunk was
// recorded for a control whose top-left corner is `origin` in the window's
// logical units, and what it draws is clipped to `clip` (window logical
// units) and to its own clips (a rounded clip by its bounds). Returns how
// many drawing commands it skipped. A chunk whose saves or groups nest
// deeper than it can follow is skipped from there on, counted.
[[nodiscard]] std::uint64_t composite_backplane_chunk(const DisplayChunk& chunk, Point origin, Rect clip,
                                                      const BackplaneRegion& region,
                                                      const PartRasterView& image) noexcept;

} // namespace gui_forms::detail
