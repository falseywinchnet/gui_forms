#pragma once

#include "gui_forms/metrics/types/metrics_types.hpp"

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace gui_forms {

class Metrics {
public:
    [[nodiscard]] MetricsSnapshot snapshot() const;
    void reset_activity() noexcept;

    void set_population(std::uint64_t controls, std::uint64_t stable_ids) noexcept;
    void record_mutation() noexcept;
    void record_dirty_mark(double requested_damage_area) noexcept;
    void record_measure(std::uint64_t nodes_visited,
                        std::uint64_t callbacks) noexcept;
    void record_arrange(std::uint64_t nodes_visited,
                        std::uint64_t callbacks) noexcept;
    void record_paint(std::uint64_t nodes_visited,
                      std::uint64_t controls,
                      std::uint64_t invalidations_consumed,
                      std::uint64_t chunks_rebuilt,
                      std::uint64_t chunks_reused,
                      std::uint64_t commands_replayed,
                      double area,
                      double bounding_area,
                      std::uint64_t rectangles,
                      bool full_window) noexcept;
    void set_display_cache(std::uint64_t entries,
                           std::uint64_t generation) noexcept;
    void record_input() noexcept;
    void record_focus_transition() noexcept;
    void record_focus_scope_opened(std::size_t depth) noexcept;
    void record_focus_scope_closed(bool restored_focus,
                                   std::size_t depth) noexcept;
    void record_focus_scope_rejection() noexcept;
    void record_activation() noexcept;
    void record_disposal(std::uint64_t count = 1) noexcept;
    // A theme engine part record was made; `live` records remain.
    void record_engine_part_created(bool index_fallback, std::uint64_t live) noexcept;
    // `released` part records were released; `live` records remain.
    void record_engine_parts_released(std::uint64_t released, std::uint64_t live) noexcept;
    // `evicted` item-part records were evicted; `live` records remain.
    void record_engine_parts_evicted(std::uint64_t evicted, std::uint64_t live) noexcept;
    void record_wrong_thread_rejection() noexcept;
    void record_subscription_connected() noexcept;
    void record_subscription_disconnected() noexcept;
    void record_callback_emitted(std::uint64_t count = 1) noexcept;
    void record_focus_revocation() noexcept;
    void record_capture_revocation() noexcept;
    void record_press_revocation() noexcept;
    void record_undeclared_mutation() noexcept;
    void record_damage_region(std::size_t rectangle_count,
                              std::uint64_t compactions,
                              std::uint64_t collapses) noexcept;
    void enter_update_scope() noexcept;
    void leave_update_scope() noexcept;
    void record_flush(bool read_barrier) noexcept;
    void record_pass_limit_hit() noexcept;
    void record_callback_arbitration_retry(bool limit_hit) noexcept;
    void record_frame_request() noexcept;
    void record_frame_poll(std::uint64_t deadlines_fired,
                           std::uint64_t active_surface_ticks,
                           std::uint64_t motion_clock_ticks,
                           std::uint64_t coalesced_requests) noexcept;
    // The motion clock: a move started, a value settled on a frame, a value
    // jumped to its end at once, and the values moving now.
    void record_motion_started() noexcept;
    void record_motion_settled() noexcept;
    void record_motion_finished() noexcept;
    void set_motion_values_moving(std::uint64_t moving) noexcept;
    void record_frame_callback_fault() noexcept;
    void record_reentrant_frame_poll() noexcept;
    void set_active_surface_count(std::size_t count) noexcept;
    void record_occlusion_transition(bool occluded) noexcept;
    void record_occluded_frame_poll() noexcept;
    void record_present(std::uint64_t duration_nanoseconds) noexcept;
    // A retained present that copied `device_pixels` in `rectangles`.
    void record_present(std::uint64_t duration_nanoseconds, std::uint64_t device_pixels,
                        std::uint64_t rectangles) noexcept;
    // Time spent only copying pixels to the screen.
    void record_screen_copy(std::uint64_t nanoseconds) noexcept;
    // One live-surface frame copied to the screen.
    void record_live_present(std::uint64_t duration_nanoseconds, std::uint64_t device_pixels,
                             std::uint64_t rectangles) noexcept;
    void set_renderer(std::string_view name, bool cpu_only);

private:
    MetricsSnapshot values_;
    std::atomic<std::uint64_t> wrong_thread_rejections_{};
};

} // namespace gui_forms
