#pragma once

#include <cstdint>

namespace gui_forms::detail {

// What the macOS host knows, at the start of a native draw, about the pixels
// its raster holds (the front) and about what is owed. Plain values, so the
// rule below is testable without AppKit.
struct FrontReuse final {
    // A frame has been presented into this backing since it was last
    // allocated or resized.
    bool raster_has_content{false};
    // The front's logical size and scale are the view's now (a prepared
    // front records both; an ordinary raster is resized before this check).
    bool front_matches_extent{false};
    // The surface epoch of the receipt the front presents, and the model's
    // current one. A size or scale change starts a new epoch.
    std::uint64_t front_surface_epoch{0};
    std::uint64_t surface_epoch{0};
    // The model owes no damage the host has not taken.
    bool lease_clean{false};
    // A live region was taken from its surface and not presented, so the
    // raster lacks it.
    bool live_region_lost{false};
};

// The front still holds the last presentation, at the view's size and
// scale, in the model's current epoch: the host's rectangles may be copied
// from it rather than repainted.
[[nodiscard]] inline bool front_held(const FrontReuse& reuse) noexcept {
    const bool held = reuse.raster_has_content && reuse.front_matches_extent &&
        reuse.front_surface_epoch != 0U && reuse.front_surface_epoch == reuse.surface_epoch &&
        reuse.lease_clean;
    return held;
}

// A draw that copies the front as it stands and paints nothing: the front is
// held, no live region is lost, and nothing is pending. `damage_pending` is
// the host's own pending damage, which a failed, aborted or unacknowledged
// frame keeps, so a discarded candidate always repaints. `live_pending` is a
// live presentation waiting to compose.
[[nodiscard]] inline bool copy_only_draw(const FrontReuse& reuse, const bool damage_pending,
                                         const bool live_pending) noexcept {
    const bool copy = front_held(reuse) && !reuse.live_region_lost && !damage_pending && !live_pending;
    return copy;
}

} // namespace gui_forms::detail
