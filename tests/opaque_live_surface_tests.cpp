#include "gui_forms/live_surface.hpp"
#include "gui_forms/window.hpp"
#include "../src/host/macos/application/live_surface_damage.hpp"

#include <array>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

namespace {
using namespace gui_forms;

void require(const bool condition, const char* message) {
    if (condition) return;
    std::cerr << message << '\n';
    std::exit(1);
}

void description_and_frame_lifetime() {
    const LiveSurfaceDescription defaults{};
    require(!defaults.opaque, "opaque defaults false");
    const std::shared_ptr<LiveSurface> surface = LiveSurface::create(
        {.width = 2U, .height = 2U, .opaque = true});
    require(surface && (*surface).snapshot().description.opaque, "create preserves opaque");
    LiveSurfaceWriteLease write = (*surface).try_acquire_write();
    require(static_cast<bool>(write), "write lease acquired");
    for (std::byte& pixel : write.pixels()) pixel = std::byte{255};
    require(write.publish() != 0U, "opaque frame published");
    const LiveSurfaceFrame old_frame = (*surface).acquire_latest();
    require(old_frame.opaque(), "frame holds opaque promise");
    require(!(*surface).reconfigure({.opaque = false}), "invalid reconfigure refused");
    require((*surface).snapshot().description.opaque, "refusal preserves promise");
    require((*surface).reconfigure({.width = 3U, .height = 2U}), "reconfigure to translucent");
    require(!(*surface).snapshot().description.opaque, "snapshot follows reconfigure");
    require(old_frame.opaque() && old_frame.width() == 2U, "old frame keeps its own description");
    write = (*surface).try_acquire_write();
    require(static_cast<bool>(write), "replacement write lease acquired");
    require(write.publish() != 0U, "translucent frame published");
    const LiveSurfaceFrame new_frame = (*surface).acquire_latest();
    require(!new_frame.opaque(), "replacement frame is translucent");
    require((*surface).reconfigure({.width = 2U, .height = 3U, .opaque = true}), "reconfigure to opaque");
    require((*surface).snapshot().description.opaque, "reconfigure preserves true");
    const LiveSurfaceFrame empty{};
    require(!empty.opaque(), "empty frame grants no coverage");
}

void format_and_frame_lifetime() {
    const LiveSurfaceDescription defaults{};
    require(defaults.pixel_format == LiveSurfacePixelFormat::bgra32_premultiplied_srgb, "BGRA remains default");
#if defined(_WIN32)
    constexpr LiveSurfacePixelFormat native = LiveSurfacePixelFormat::bgra32_premultiplied_srgb;
#else
    constexpr LiveSurfacePixelFormat native = LiveSurfacePixelFormat::rgba32_premultiplied_srgb;
#endif
    require(native_live_surface_pixel_format() == native, "native query before attachment");
    const std::array<LiveSurfacePixelFormat, 2> formats{
        LiveSurfacePixelFormat::rgba32_premultiplied_srgb,
        LiveSurfacePixelFormat::bgra32_premultiplied_srgb};
    for (const LiveSurfacePixelFormat format : formats) {
        const std::shared_ptr<LiveSurface> surface = LiveSurface::create(
            {.width = 2U, .height = 2U, .pixel_format = format, .opaque = true});
        require(surface && (*surface).snapshot().description.pixel_format == format, "create round trips format");
        LiveSurfaceWriteLease write = (*surface).try_acquire_write();
        require(static_cast<bool>(write), "format write acquired");
        const LiveSurfacePixelFormat replacement = format == formats[0] ? formats[1] : formats[0];
        const LiveSurfaceDescription next{.width = 2U, .height = 2U, .pixel_format = replacement};
        require(!(*surface).reconfigure(next), "active writer prevents relabelling");
        for (std::byte& value : write.pixels()) value = std::byte{255};
        require(write.publish() != 0U, "format frame published");
        const LiveSurfaceFrame old = (*surface).acquire_latest();
        require(old.pixel_format() == format, "frame has producer format");
        require((*surface).reconfigure(next), "same size format change accepted");
        require((*surface).snapshot().description.pixel_format == replacement, "snapshot follows format change");
        require(old.pixel_format() == format && old.opaque(), "old lease keeps format and alpha promise");
        write = (*surface).try_acquire_write();
        require(static_cast<bool>(write), "replacement format write acquired");
        require(write.publish() != 0U, "replacement format published");
        const LiveSurfaceFrame current = (*surface).acquire_latest();
        require(current.pixel_format() == replacement && !current.opaque(), "new frame has new description");
        const LiveSurfaceDescription invalid{.width = 2U, .height = 2U,
            .pixel_format = static_cast<LiveSurfacePixelFormat>(255)};
        require(!LiveSurface::create(invalid) && !(*surface).reconfigure(invalid), "unknown format rejected");
        require((*surface).snapshot().description.pixel_format == replacement, "invalid format preserves state");
        const std::shared_ptr<Control> root = make_control<Control>(StableId("native-query"));
        Window window(root, {2.0, 2.0});
        window.perform_layout();
        require(window.queue_live_surface_presentation(root, surface), "format surface attached");
        require(native_live_surface_pixel_format() == native, "native query after attachment");
    }
}

void damage_coverage() {
    const Rect dirty{0.0, 0.0, 10.0, 8.0};
    const std::array<Rect, 2> adjacent{{{0.0, 0.0, 4.0, 8.0}, {4.0, 0.0, 6.0, 8.0}}};
    require(detail::opaque_live_damage_covers(dirty, 1.0, adjacent, true, false), "union covers live-only damage");
    require(!detail::opaque_live_damage_covers(dirty, 1.0, adjacent, true, true), "retained damage requires paint");
    require(!detail::opaque_live_damage_covers(dirty, 1.0, adjacent, false, false), "first frame/resize/recovery requires paint");
    require(!detail::opaque_live_damage_covers(dirty, 1.0, {}, true, false), "translucent/absent frames cannot cover damage");
    const std::array<Rect, 4> hole{{{0.0, 0.0, 10.0, 2.0}, {0.0, 6.0, 10.0, 2.0},
                                  {0.0, 2.0, 2.0, 4.0}, {8.0, 2.0, 2.0, 4.0}}};
    require(!detail::opaque_live_damage_covers(dirty, 1.0, hole, true, false), "overlay hole is not bounding-box coverage");
    require(detail::opaque_live_damage_covers({0.0, 0.0, 10.0, 2.0}, 1.0, hole, true, false), "damage outside overlay remains covered");
    const std::array<Rect, 3> overlapping{{{0.0, 0.0, 6.0, 8.0}, {3.0, 0.0, 7.0, 4.0}, {3.0, 4.0, 7.0, 4.0}}};
    require(detail::opaque_live_damage_covers(dirty, 1.0, overlapping, true, false), "overlap and vertical union cover");
    const double scale = 1.25;
    const Rect fraction{0.1, 0.1, 7.8, 7.8};
    const std::array<Rect, 1> insufficient{{detail::opaque_live_clip(fraction, scale)}};
    require(!detail::opaque_live_damage_covers(fraction, scale, insufficient, true, false), "fractional boundary cannot promise outward pixels");
    const std::array<Rect, 1> complete{{detail::opaque_live_clip({0.0, 0.0, 8.0, 8.0}, scale)}};
    require(detail::opaque_live_damage_covers(fraction, scale, complete, true, false), "fractional damage rounds outward to covered device pixels");
    const std::array<Rect, 2> fractional_gap{{detail::opaque_live_clip({0.0, 0.0, 3.9, 8.0}, scale),
                                           detail::opaque_live_clip({4.1, 0.0, 3.9, 8.0}, scale)}};
    require(!detail::opaque_live_damage_covers({0.0, 0.0, 8.0, 8.0}, scale, fractional_gap, true, false), "subpixel overlay gap is not rounded away");
    require(!detail::opaque_live_damage_covers(dirty, 0.0, adjacent, true, false), "invalid scale refused");
    require(detail::opaque_live_clip(dirty, std::numeric_limits<double>::infinity()).empty(), "nonfinite scale refused");
}

void refreshed_presentations(const LiveSurfacePixelFormat format) {
    const std::shared_ptr<Control> root = make_control<Control>(StableId("root"));
    const std::shared_ptr<Control> first = make_control<Control>(StableId("first"));
    const std::shared_ptr<Control> second = make_control<Control>(StableId("second"));
    const std::shared_ptr<Control> overlay = make_control<Control>(StableId("overlay"));
    (*first).set_requested_bounds({0.0, 0.0, 4.0, 8.0});
    (*second).set_requested_bounds({4.0, 0.0, 4.0, 8.0});
    (*overlay).set_requested_bounds({0.0, 0.0, 4.0, 8.0});
    (*overlay).set_paint_plane(PaintPlane::overlay);
    (*overlay).set_visible(false);
    (*root).add_child(first);
    (*root).add_child(second);
    (*root).add_child(overlay);
    Window window(root, {8.0, 8.0});
    window.perform_layout();
    const std::shared_ptr<LiveSurface> surface_a = LiveSurface::create({.width = 4U, .height = 8U, .pixel_format = format, .opaque = true});
    const std::shared_ptr<LiveSurface> surface_b = LiveSurface::create({.width = 4U, .height = 8U, .pixel_format = format, .opaque = true});
    require(surface_a && surface_b, "retry surfaces created");
    LiveSurfaceWriteLease write_a = (*surface_a).try_acquire_write();
    LiveSurfaceWriteLease write_b = (*surface_b).try_acquire_write();
    require(write_a && write_b, "retry write leases acquired");
    for (std::byte& value : write_a.pixels()) value = std::byte{255};
    for (std::byte& value : write_b.pixels()) value = std::byte{255};
    const std::uint64_t generation_a = write_a.publish();
    const std::uint64_t generation_b = write_b.publish();
    require(generation_a != 0U && generation_b != 0U, "initial generations published");
    require(window.queue_live_surface_presentation(first, surface_a), "first registered");
    require(window.queue_live_surface_presentation(second, surface_b), "second registered");
    require(window.take_live_surface_presentations().size() == 2U, "initial batch sampled");
    require(window.take_live_surface_presentations().empty(), "unchanged drain stays empty by default");
    // A failed to present; only B changes before the retry.
    write_b = (*surface_b).try_acquire_write(true);
    require(static_cast<bool>(write_b), "second replacement acquired");
    require(write_b.publish() != 0U, "only second advances");
    const std::vector<LiveSurfacePresentation> retry = window.take_live_surface_presentations(true);
    require(retry.size() == 2U, "refresh retains unchanged failed surface A");
    (*overlay).set_visible(true);
    (*second).set_visible(false);
    window.perform_layout();
    require(window.take_live_surface_presentations(true).empty(), "refresh discards newly covered and hidden surfaces");
}
void publish_damage(LiveSurface& surface, const Rect damage) {
    LiveSurfaceWriteLease write = surface.try_acquire_write(true);
    require(static_cast<bool>(write), "damage write acquired");
    require(write.publish(damage) != 0U, "damage published");
}

void presentation_region() {
    const std::shared_ptr<LiveSurface> surface = LiveSurface::create(
        {.width = 100U, .height = 50U, .opaque = true});
    require(static_cast<bool>(surface), "region surface created");
    const Rect full{0.0, 0.0, 100.0, 50.0};
    const int presenter{};
    const int other{};
    publish_damage(*surface, {10.0, 10.0, 5.0, 5.0});
    require((*surface).acquire_for_presentation(&presenter).damage() == full, "first take is the whole surface");
    require((*surface).acquire_for_presentation(&presenter).damage().empty(), "nothing published since the take");

    publish_damage(*surface, {10.0, 10.0, 5.0, 5.0});
    publish_damage(*surface, {40.0, 20.0, 2.0, 3.0});
    require((*surface).acquire_latest().damage() == Rect(40.0, 20.0, 2.0, 3.0), "acquire_latest reads the newest publication");
    require((*surface).acquire_latest().damage() == Rect(40.0, 20.0, 2.0, 3.0), "acquire_latest never consumes");
    const LiveSurfaceFrame merged = (*surface).acquire_for_presentation(&presenter);
    require(merged.damage() == Rect(10.0, 10.0, 32.0, 13.0), "unpresented publications merge into one region");
    require(merged.generation() == 3U, "region and frame are taken together");
    require((*surface).acquire_for_presentation(&presenter).damage().empty(), "taking validates the region");

    publish_damage(*surface, {1.0, 1.0, 1.0, 1.0});
    require((*surface).acquire_for_presentation(&other).damage() == full, "another presenter copies everything");
    require((*surface).acquire_for_presentation(&presenter).damage() == full, "so does the presenter it displaced");
    publish_damage(*surface, {200.0, 0.0, 5.0, 5.0});
    require((*surface).acquire_for_presentation(&presenter).damage() == full, "outside damage is the whole surface");

    require((*surface).reconfigure({.width = 100U, .height = 50U, .opaque = true}), "reconfigured");
    require(!(*surface).acquire_for_presentation(&presenter), "no frame before the new epoch publishes");
    publish_damage(*surface, {1.0, 1.0, 1.0, 1.0});
    require((*surface).acquire_for_presentation(&presenter).damage() == full, "reconfigured surface is whole");
}

bool all_damage_limited(const std::vector<LiveSurfacePresentation>& presentations, const bool expected) {
    for (const LiveSurfacePresentation& presentation : presentations) {
        if (presentation.damage_limited != expected) return false;
    }
    return !presentations.empty();
}

void overlay_presentations() {
    const std::shared_ptr<Control> root = make_control<Control>(StableId("scene.root"));
    const std::shared_ptr<Control> scene = make_control<Control>(StableId("scene"));
    const std::shared_ptr<Control> capsule = make_control<Control>(StableId("capsule"));
    (*scene).set_requested_bounds({0.0, 0.0, 100.0, 80.0});
    (*capsule).set_requested_bounds({10.0, 10.0, 20.0, 10.0});
    (*capsule).set_paint_plane(PaintPlane::overlay);
    (*root).add_child(scene);
    (*root).add_child(capsule);
    Window window(root, {100.0, 80.0});
    window.perform_layout();
    const std::shared_ptr<LiveSurface> surface = LiveSurface::create(
        {.width = 100U, .height = 80U, .opaque = true});
    require(static_cast<bool>(surface), "scene surface created");
    publish_damage(*surface, {});
    require(window.queue_live_surface_presentation(scene, surface), "scene registered");

    const std::vector<LiveSurfacePresentation> placed = window.take_live_surface_presentations();
    require(placed.size() == 4U && all_damage_limited(placed, false), "first placement copies complete fragments");
    require(window.take_live_surface_presentations().empty(), "a static overlay does not re-present an unchanged surface");

    publish_damage(*surface, {50.0, 50.0, 5.0, 5.0});
    const std::vector<LiveSurfacePresentation> changed = window.take_live_surface_presentations();
    require(changed.size() == 4U && all_damage_limited(changed, true), "same placement permits region copies");

    (*capsule).set_requested_bounds({30.0, 10.0, 20.0, 10.0});
    window.perform_layout();
    const std::vector<LiveSurfacePresentation> moved = window.take_live_surface_presentations();
    require(moved.size() == 4U && all_damage_limited(moved, false), "a moved overlay re-presents completely");
    require(window.take_live_surface_presentations().empty(), "the moved overlay settles");
    require(all_damage_limited(window.take_live_surface_presentations(true), false), "retries copy complete clips");

    (*capsule).set_visible(false);
    window.perform_layout();
    const std::vector<LiveSurfacePresentation> uncovered = window.take_live_surface_presentations();
    require(uncovered.size() == 1U && all_damage_limited(uncovered, false), "a removed overlay re-presents completely");

    (*scene).set_visible(false);
    require(window.take_live_surface_presentations().empty(), "hidden surface is not presented");
    (*scene).set_visible(true);
    const std::vector<LiveSurfacePresentation> shown = window.take_live_surface_presentations();
    require(shown.size() == 1U && all_damage_limited(shown, false), "a re-shown surface copies completely");
}

void host_region_helpers() {
    // 1:1 at scale 2: a 400-pixel surface in a 200-point destination.
    const Rect destination{10.0, 20.0, 200.0, 100.0};
    require(detail::live_damage_in_window({100.0, 50.0, 10.0, 4.0}, destination, 400U, 200U, 2.0) ==
                Rect(59.5, 44.5, 6.0, 3.0), "region maps through destination with one-pixel widening");
    require(detail::live_damage_in_window({0.0, 0.0, 400.0, 200.0}, destination, 400U, 200U, 2.0) == destination,
            "whole region is clamped to the destination");
    require(detail::live_damage_in_window({}, destination, 400U, 200U, 2.0).empty(), "empty region maps to nothing");

    // Live fragments around an overlay leave exactly the overlay uncovered.
    const std::array<Rect, 4> fragments{{{0.0, 0.0, 10.0, 2.0}, {0.0, 6.0, 10.0, 2.0},
                                         {0.0, 2.0, 2.0, 4.0}, {8.0, 2.0, 2.0, 4.0}}};
    std::vector<Rect> uncovered;
    detail::append_uncovered_live_damage({0.0, 0.0, 10.0, 8.0}, 1.0, fragments, uncovered);
    require(uncovered.size() == 1U && uncovered[0] == Rect(2.0, 2.0, 6.0, 4.0), "bounding box retains only the overlay");
    uncovered.clear();
    detail::append_uncovered_live_damage({0.0, 0.0, 10.0, 2.0}, 1.0, fragments, uncovered);
    require(uncovered.empty(), "covered dirty rectangle needs no retained painting");
    detail::append_uncovered_live_damage({0.0, 0.0, 3.0, 3.0}, 1.0, {}, uncovered);
    require(uncovered.size() == 1U && uncovered[0] == Rect(0.0, 0.0, 3.0, 3.0), "no live clips leave everything");
    std::vector<Rect> scattered;
    for (int index = 0; index < 40; ++index) {
        const double offset = static_cast<double>(index * 2 + 1);
        scattered.push_back({offset, offset, 1.0, 1.0});
    }
    uncovered.clear();
    detail::append_uncovered_live_damage({0.0, 0.0, 100.0, 100.0}, 1.0, scattered, uncovered);
    require(uncovered.size() == 1U && uncovered[0] == Rect(0.0, 0.0, 100.0, 100.0),
            "fragment overflow falls back to the whole dirty rectangle");
}
} // namespace

int main() {
    description_and_frame_lifetime();
    presentation_region();
    overlay_presentations();
    host_region_helpers();
    format_and_frame_lifetime();
    damage_coverage();
    refreshed_presentations(LiveSurfacePixelFormat::bgra32_premultiplied_srgb);
    refreshed_presentations(LiveSurfacePixelFormat::rgba32_premultiplied_srgb);
    return 0;
}
