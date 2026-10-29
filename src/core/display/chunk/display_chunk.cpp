#include "display_chunk.hpp"

#include <utility>

namespace gui_forms::detail {

DisplayChunk::DisplayChunk(std::uint64_t generation,
                           PaintPlane plane,
                           Rect logical_bounds,
                           std::vector<DisplayCommand> commands,
                           std::optional<Rect> opaque_cover)
    : generation_(generation), plane_(plane), logical_bounds_(logical_bounds),
      commands_(std::move(commands)), opaque_cover_(opaque_cover) {}

DisplayChunkInfo DisplayChunk::info() const noexcept {
    return {generation_, plane_, logical_bounds_, commands_.size()};
}

} // namespace gui_forms::detail
