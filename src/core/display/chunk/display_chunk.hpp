#pragma once

#include "../command/display_command.hpp"

#include <cstdint>
#include <optional>
#include <vector>

namespace gui_forms::detail {

class DisplayChunk final {
public:
    DisplayChunk(std::uint64_t generation, PaintPlane plane,
                 Rect logical_bounds, std::vector<DisplayCommand> commands,
                 std::optional<Rect> opaque_cover = std::nullopt);

    [[nodiscard]] DisplayChunkInfo info() const noexcept;
    [[nodiscard]] std::uint64_t generation() const noexcept { return generation_; }
    [[nodiscard]] PaintPlane plane() const noexcept { return plane_; }
    [[nodiscard]] Rect logical_bounds() const noexcept { return logical_bounds_; }
    [[nodiscard]] const std::vector<DisplayCommand>& commands() const noexcept {
        return commands_;
    }
    // A rectangle, in the chunk's own coordinates, that replaying it leaves
    // opaque whatever lay beneath (RecordingPainter::mark_opaque_cover), or
    // none.
    [[nodiscard]] std::optional<Rect> opaque_cover() const noexcept { return opaque_cover_; }

private:
    std::uint64_t generation_{};
    PaintPlane plane_{PaintPlane::control};
    Rect logical_bounds_{};
    std::vector<DisplayCommand> commands_;
    std::optional<Rect> opaque_cover_;
};

} // namespace gui_forms::detail
