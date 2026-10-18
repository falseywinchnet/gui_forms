#include "gui_forms/application/window_frame_drag/window_frame_drag.hpp"
#include "application_state.hpp"

namespace gui_forms {

const char* window_frame_drag_mode_name(WindowFrameDragMode mode) noexcept {
    switch (mode) {
    case WindowFrameDragMode::framework_capture: return "framework_capture";
    case WindowFrameDragMode::native_move_loop: return "native_move_loop";
    }
    return "unknown";
}

double WindowFrameDragStatistics::duration_seconds() const noexcept {
    if (last_timestamp_nanoseconds <= first_timestamp_nanoseconds) return 0.0;
    const std::uint64_t elapsed = last_timestamp_nanoseconds - first_timestamp_nanoseconds;
    const double seconds = static_cast<double>(elapsed) / 1.0e9;
    return seconds;
}

double WindowFrameDragStatistics::achieved_rate_hz() const noexcept {
    const double seconds = duration_seconds();
    if (seconds <= 0.0) return 0.0;
    const double rate = static_cast<double>(frame_reports) / seconds;
    return rate;
}

double WindowFrameDragStatistics::pointer_rate_hz() const noexcept {
    const double seconds = duration_seconds();
    if (seconds <= 0.0) return 0.0;
    const double rate = static_cast<double>(pointer_moves) / seconds;
    return rate;
}

bool WindowFrameDragStatistics::keeps_up(double minimum_rate_hz) const noexcept {
    if (frame_requests == 0U) return true;
    // Nine reports in ten requests: the window followed the pointer.
    if (frame_reports * 10U >= frame_requests * 9U) return true;
    // Fewer reports than requests is fine while the window still moves
    // often enough (a compositor coalescing moves to its frame rate).
    const bool fast_enough = achieved_rate_hz() >= minimum_rate_hz;
    return fast_enough;
}

WindowFrameDrag::~WindowFrameDrag() {
    reports_.disconnect();
}

WindowFrameDragMode WindowFrameDrag::mode_for(const HostCapabilities& capabilities) noexcept {
    const bool capture = capabilities.supports(HostCapability::window_frames) &&
        capabilities.supports(HostCapability::screen_pointer) &&
        capabilities.supports(HostCapability::pointer_capture);
    const WindowFrameDragMode mode =
        capture ? WindowFrameDragMode::framework_capture : WindowFrameDragMode::native_move_loop;
    return mode;
}

void WindowFrameDrag::FrameCounter::operator()(const HostWindowFrameEvent& event) const {
    if (!(*drag).active_ || event.window != (*drag).window_id_) return;
    ++(*drag).statistics_.frame_reports;
}

HostServiceStatus WindowFrameDrag::begin(const ApplicationWindowHandle& window,
                                         std::uint64_t timestamp_nanoseconds) {
    if (active_) return {HostServiceError::invalid_argument};
    const ApplicationWindows windows = window.windows();
    if (!windows.active()) return {HostServiceError::after_shutdown};
    if (mode_for(windows.capabilities()) != WindowFrameDragMode::framework_capture) {
        return {HostServiceError::unsupported};
    }
    const HostWindowFrameResult frame = window.frame();
    if (!frame.status.accepted()) return frame.status;
    const HostScreenPointResult pointer = windows.pointer_position();
    if (!pointer.status.accepted()) return pointer.status;
    const std::shared_ptr<detail::ApplicationWindowState> state =
        detail::ApplicationHandleAccess::state(window);
    const std::shared_ptr<detail::ApplicationRunState> run =
        state ? (*state).run.lock() : std::shared_ptr<detail::ApplicationRunState>{};
    if (!run || (*run).windowing == nullptr) return {HostServiceError::after_shutdown};
    window_ = window;
    window_id_ = window.id();
    start_content_ = frame.frame.content;
    start_pointer_ = pointer.position;
    last_target_ = {start_content_.x, start_content_.y};
    statistics_ = {};
    statistics_.first_timestamp_nanoseconds = timestamp_nanoseconds;
    statistics_.last_timestamp_nanoseconds = timestamp_nanoseconds;
    reports_ = (*(*run).windowing).frame_changed().subscribe(FrameCounter{this});
    active_ = true;
    return {};
}

HostServiceStatus WindowFrameDrag::move(std::uint64_t timestamp_nanoseconds) {
    if (!active_) return {HostServiceError::invalid_argument};
    ++statistics_.pointer_moves;
    statistics_.last_timestamp_nanoseconds = timestamp_nanoseconds;
    const HostScreenPointResult pointer = window_.windows().pointer_position();
    if (!pointer.status.accepted()) return pointer.status;
    const Point target{start_content_.x + (pointer.position.x - start_pointer_.x),
                       start_content_.y + (pointer.position.y - start_pointer_.y)};
    if (target == last_target_) return {};
    last_target_ = target;
    ++statistics_.frame_requests;
    const Rect content{target.x, target.y, start_content_.width, start_content_.height};
    const HostServiceStatus status = window_.request_frame(content);
    if (!status.accepted()) ++statistics_.refused_requests;
    return status;
}

void WindowFrameDrag::stop(std::uint64_t timestamp_nanoseconds) noexcept {
    active_ = false;
    if (timestamp_nanoseconds > statistics_.last_timestamp_nanoseconds) {
        statistics_.last_timestamp_nanoseconds = timestamp_nanoseconds;
    }
    reports_.disconnect();
}

void WindowFrameDrag::end(std::uint64_t timestamp_nanoseconds) noexcept {
    if (!active_) return;
    stop(timestamp_nanoseconds);
}

HostServiceStatus WindowFrameDrag::cancel(std::uint64_t timestamp_nanoseconds) {
    if (!active_) return {HostServiceError::invalid_argument};
    stop(timestamp_nanoseconds);
    const HostServiceStatus status = window_.request_frame(start_content_);
    return status;
}

} // namespace gui_forms
