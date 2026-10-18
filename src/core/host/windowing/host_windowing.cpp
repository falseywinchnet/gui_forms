#include "gui_forms/host/windowing/host_windowing.hpp"
#include "gui_forms/text/types/text_types.hpp"

#include <cmath>
#include <sstream>

namespace gui_forms {
namespace {
bool valid_stable_id(std::string_view value) noexcept {
    const bool valid = !value.empty() &&
        value.size() <= HostWindowing::maximum_stable_id_bytes &&
        value.find('\0') == std::string_view::npos && validate_utf8(value).valid();
    return valid;
}

bool valid_frame_request(Rect content) noexcept {
    const double bound = HostWindowing::maximum_desktop_coordinate;
    const bool valid = content.finite() && content.width >= 1.0 && content.height >= 1.0 &&
        content.width <= 32768.0 && content.height <= 32768.0 &&
        std::fabs(content.x) <= bound && std::fabs(content.y) <= bound;
    return valid;
}

bool valid_desktop_point(Point point) noexcept {
    const double bound = HostWindowing::maximum_desktop_coordinate;
    const bool valid = std::isfinite(point.x) && std::isfinite(point.y) &&
        std::fabs(point.x) <= bound && std::fabs(point.y) <= bound;
    return valid;
}

bool valid_frame(const HostWindowFrame& frame) noexcept {
    const bool valid = frame.content.finite() && frame.outer.finite() &&
        frame.content.width >= 0.0 && frame.content.height >= 0.0 &&
        frame.outer.width >= 0.0 && frame.outer.height >= 0.0;
    return valid;
}
} // namespace

const char* host_window_kind_name(HostWindowKind kind) noexcept {
    switch (kind) {
    case HostWindowKind::primary: return "primary";
    case HostWindowKind::standard: return "standard";
    case HostWindowKind::tool: return "tool";
    }
    return "unknown";
}

std::string HostWindowingSnapshot::to_json() const {
    std::ostringstream output;
    output << "{\"capabilities\":" << capabilities.to_json()
           << ",\"live_windows\":" << live_windows
           << ",\"admitted\":" << admitted
           << ",\"retired\":" << retired
           << ",\"frame_requests\":" << frame_requests
           << ",\"frame_reports\":" << frame_reports
           << ",\"rejected_requests\":" << rejected_requests
           << ",\"started\":" << (started ? "true" : "false")
           << ",\"shutdown\":" << (shutdown ? "true" : "false") << '}';
    return output.str();
}

HostWindowing::HostWindowing(HostCapabilities capabilities)
    : ui_thread_(std::this_thread::get_id()) {
    snapshot_.capabilities = std::move(capabilities);
    windows_.reserve(maximum_windows);
}

HostWindowing::~HostWindowing() = default;

HostServiceStatus HostWindowing::reject(HostServiceError error) noexcept {
    ++snapshot_.rejected_requests;
    const HostServiceStatus status{error};
    return status;
}

HostServiceStatus HostWindowing::validate_request(HostCapability capability) noexcept {
    if (std::this_thread::get_id() != ui_thread_) return reject(HostServiceError::wrong_thread);
    if (snapshot_.shutdown) return reject(HostServiceError::after_shutdown);
    if (!snapshot_.capabilities.supports(capability)) return reject(HostServiceError::unsupported);
    return {};
}

HostServiceStatus HostWindowing::validate_window_request(HostCapability capability,
                                                         HostWindowId window) noexcept {
    const HostServiceStatus status = validate_request(capability);
    if (!status.accepted()) return status;
    if (record(window) == nullptr) return reject(HostServiceError::invalid_argument);
    return {};
}

HostWindowing::WindowRecord* HostWindowing::record(HostWindowId window) noexcept {
    if (!window.valid()) return nullptr;
    for (std::size_t index = 0U; index < windows_.size(); ++index) {
        if (windows_[index].id == window) return &windows_[index];
    }
    return nullptr;
}

const HostWindowing::WindowRecord* HostWindowing::record(HostWindowId window) const noexcept {
    if (!window.valid()) return nullptr;
    for (std::size_t index = 0U; index < windows_.size(); ++index) {
        if (windows_[index].id == window) return &windows_[index];
    }
    return nullptr;
}

HostWindowFrameResult HostWindowing::frame(HostWindowId window) {
    HostWindowFrameResult result;
    result.status = validate_window_request(HostCapability::window_frames, window);
    if (!result.status.accepted()) return result;
    result = frame_impl(window);
    if (result.status.accepted() && !valid_frame(result.frame)) {
        result.status = reject(HostServiceError::backend_failure);
        result.frame = {};
    } else if (!result.status.accepted()) {
        ++snapshot_.rejected_requests;
    }
    return result;
}

HostServiceStatus HostWindowing::request_frame(HostWindowId window, Rect content) {
    HostServiceStatus status = validate_window_request(HostCapability::window_frames, window);
    if (!status.accepted()) return status;
    if (!valid_frame_request(content)) return reject(HostServiceError::invalid_argument);
    ++snapshot_.frame_requests;
    status = request_frame_impl(window, content);
    if (!status.accepted()) ++snapshot_.rejected_requests;
    return status;
}

HostScreenPointResult HostWindowing::pointer_position() {
    HostScreenPointResult result;
    result.status = validate_request(HostCapability::screen_pointer);
    if (!result.status.accepted()) return result;
    result = pointer_position_impl();
    if (result.status.accepted() && !valid_desktop_point(result.position)) {
        result.status = reject(HostServiceError::backend_failure);
        result.position = {};
    } else if (!result.status.accepted()) {
        ++snapshot_.rejected_requests;
    }
    return result;
}

HostWindowAtResult HostWindowing::window_at(Point point) {
    HostWindowAtResult result;
    result.status = validate_request(HostCapability::window_at_point);
    if (!result.status.accepted()) return result;
    if (!valid_desktop_point(point)) {
        result.status = reject(HostServiceError::invalid_argument);
        return result;
    }
    result = window_at_impl(point);
    if (!result.status.accepted()) {
        ++snapshot_.rejected_requests;
        result.window = {};
        return result;
    }
    // A native answer naming a window the table no longer holds (one being
    // destroyed in this turn) is reported as no window.
    if (result.window.valid() && record(result.window) == nullptr) result.window = {};
    return result;
}

HostServiceStatus HostWindowing::bring_to_front(HostWindowId window) {
    HostServiceStatus status = validate_window_request(HostCapability::window_ordering, window);
    if (!status.accepted()) return status;
    status = bring_to_front_impl(window);
    if (!status.accepted()) ++snapshot_.rejected_requests;
    return status;
}

HostServiceStatus HostWindowing::destroy_window(HostWindowId window) {
    HostServiceStatus status = validate_window_request(HostCapability::runtime_windows, window);
    if (!status.accepted()) return status;
    if (window == primary_) return reject(HostServiceError::invalid_argument);
    for (std::size_t index = 0U; index < windows_.size(); ++index) {
        if (windows_[index].owner == window) return reject(HostServiceError::invalid_argument);
    }
    status = destroy_window_impl(window);
    if (!status.accepted()) {
        ++snapshot_.rejected_requests;
        return status;
    }
    // The adapter retires the window as its native close completes; make
    // sure no stale record survives an adapter that did not.
    retire(window);
    return status;
}

HostMonitorResult HostWindowing::query_monitors() {
    HostMonitorResult result;
    result.status = validate_request(HostCapability::monitor_geometry);
    if (!result.status.accepted()) return result;
    result = query_monitors_impl();
    if (result.status.accepted() && result.monitors.empty()) {
        result.status = reject(HostServiceError::backend_failure);
    } else if (!result.status.accepted()) {
        ++snapshot_.rejected_requests;
        result.monitors.clear();
    }
    return result;
}

std::optional<HostWindowId> HostWindowing::find(std::string_view stable_id) const noexcept {
    for (std::size_t index = 0U; index < windows_.size(); ++index) {
        if (windows_[index].stable_id == stable_id) return windows_[index].id;
    }
    return std::nullopt;
}

bool HostWindowing::live(HostWindowId window) const noexcept {
    const bool result = record(window) != nullptr;
    return result;
}

std::optional<HostWindowKind> HostWindowing::kind(HostWindowId window) const noexcept {
    const WindowRecord* entry = record(window);
    if (entry == nullptr) return std::nullopt;
    return (*entry).kind;
}

std::optional<HostWindowId> HostWindowing::owner(HostWindowId window) const noexcept {
    const WindowRecord* entry = record(window);
    if (entry == nullptr) return std::nullopt;
    return (*entry).owner;
}

HostWindowingSnapshot HostWindowing::snapshot() const {
    HostWindowingSnapshot result = snapshot_;
    result.live_windows = windows_.size();
    return result;
}

void HostWindowing::shutdown() noexcept {
    snapshot_.shutdown = true;
    windows_.clear();
}

HostWindowAdmission HostWindowing::admit(const HostWindowDescription& description) {
    HostWindowAdmission result;
    if (std::this_thread::get_id() != ui_thread_) {
        result.status = reject(HostServiceError::wrong_thread);
        return result;
    }
    if (snapshot_.shutdown) {
        result.status = reject(HostServiceError::after_shutdown);
        return result;
    }
    if (snapshot_.started && !snapshot_.capabilities.supports(HostCapability::runtime_windows)) {
        result.status = reject(HostServiceError::unsupported);
        return result;
    }
    const bool has_owner = description.owner.valid();
    const bool primary = description.kind == HostWindowKind::primary;
    bool valid = valid_stable_id(description.stable_id) && !find(description.stable_id).has_value();
    if (has_owner && record(description.owner) == nullptr) valid = false;
    if (primary && (has_owner || primary_.valid() || snapshot_.started)) valid = false;
    if (!primary && !primary_.valid()) valid = false;
    if (description.kind == HostWindowKind::standard && !has_owner) valid = false;
    if (!valid) {
        result.status = reject(HostServiceError::invalid_argument);
        return result;
    }
    if (windows_.size() >= maximum_windows) {
        result.status = reject(HostServiceError::too_large);
        return result;
    }
    WindowRecord entry;
    entry.id = HostWindowId{next_window_};
    entry.stable_id.assign(description.stable_id);
    entry.owner = description.owner;
    entry.kind = description.kind;
    ++next_window_;
    windows_.push_back(std::move(entry));
    if (primary) primary_ = windows_.back().id;
    ++snapshot_.admitted;
    result.window = windows_.back().id;
    return result;
}

void HostWindowing::mark_started() noexcept {
    snapshot_.started = true;
}

void HostWindowing::retire(HostWindowId window) noexcept {
    for (std::size_t index = 0U; index < windows_.size(); ++index) {
        if (windows_[index].id != window) continue;
        windows_.erase(windows_.begin() + static_cast<std::ptrdiff_t>(index));
        ++snapshot_.retired;
        return;
    }
}

void HostWindowing::report_frame(HostWindowId window, const HostWindowFrame& frame) {
    if (std::this_thread::get_id() != ui_thread_ || snapshot_.shutdown) return;
    WindowRecord* entry = record(window);
    if (entry == nullptr || !valid_frame(frame)) return;
    (*entry).frame = frame;
    (*entry).frame_known = true;
    ++snapshot_.frame_reports;
    const HostWindowFrameEvent event{window, frame};
    frame_changed_.emit(event);
}

} // namespace gui_forms
