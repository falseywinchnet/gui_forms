#include "gui_forms/host/windowing/host_windowing.hpp"
#include "gui_forms/host/windowing/desktop_coordinates.hpp"
#include "gui_forms/text/types/text_types.hpp"

#include <algorithm>
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

// Positions closer than half a logical unit are the same position: hosts
// round to device pixels.
bool same_point(Point left, Point right) noexcept {
    const bool same = std::fabs(left.x - right.x) < 0.5 && std::fabs(left.y - right.y) < 0.5;
    return same;
}

Point rect_center(Rect rect) noexcept {
    const Point center{rect.x + rect.width / 2.0, rect.y + rect.height / 2.0};
    return center;
}

// The work area a follow move is clamped to: the screen the window lands
// on (holding its centre), else the primary window's screen, else the
// screen nearest to the window's centre. `monitors` must not be empty.
Rect follow_work_area(const std::vector<HostMonitor>& monitors, Rect target_outer,
                      Rect primary_outer) noexcept {
    const Point target_center = rect_center(target_outer);
    for (std::size_t index = 0U; index < monitors.size(); ++index) {
        if (monitors[index].work_area.contains(target_center)) return monitors[index].work_area;
    }
    const Point primary_center = rect_center(primary_outer);
    for (std::size_t index = 0U; index < monitors.size(); ++index) {
        if (monitors[index].work_area.contains(primary_center)) return monitors[index].work_area;
    }
    const std::size_t nearest = nearest_monitor(monitors, target_center);
    return monitors[nearest].work_area;
}
} // namespace

HostWindowKindTraits host_window_kind_traits(HostWindowKind kind) noexcept {
    HostWindowKindTraits traits;
    if (kind == HostWindowKind::floating_tool || kind == HostWindowKind::floating_document) {
        traits.floating = true;
        traits.follows_main_window = true;
        traits.joins_main_full_screen = true;
        traits.hides_when_application_inactive = kind == HostWindowKind::floating_tool;
    }
    return traits;
}

HostWindowKind host_window_kind(bool has_owner, bool tool_window,
                                HostWindowFloating floating) noexcept {
    if (floating == HostWindowFloating::tool) return HostWindowKind::floating_tool;
    if (floating == HostWindowFloating::document) return HostWindowKind::floating_document;
    if (tool_window) return HostWindowKind::tool;
    if (has_owner) return HostWindowKind::standard;
    return HostWindowKind::primary;
}

Rect clamp_to_work_area(Rect outer, Rect work_area) noexcept {
    Rect result = outer;
    if (outer.width >= work_area.width) {
        result.x = work_area.x;
    } else {
        result.x = std::clamp(outer.x, work_area.x, work_area.x + work_area.width - outer.width);
    }
    if (outer.height >= work_area.height) {
        result.y = work_area.y;
    } else {
        result.y = std::clamp(outer.y, work_area.y, work_area.y + work_area.height - outer.height);
    }
    return result;
}

const char* host_window_kind_name(HostWindowKind kind) noexcept {
    switch (kind) {
    case HostWindowKind::primary: return "primary";
    case HostWindowKind::standard: return "standard";
    case HostWindowKind::tool: return "tool";
    case HostWindowKind::floating_tool: return "floating_tool";
    case HostWindowKind::floating_document: return "floating_document";
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
           << ",\"policy_hides\":" << policy_hides
           << ",\"policy_shows\":" << policy_shows
           << ",\"follow_moves\":" << follow_moves
           << ",\"clamped_follow_moves\":" << clamped_follow_moves
           << ",\"application_active\":" << (application_active ? "true" : "false")
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
    const bool floating = host_window_kind_traits(description.kind).floating;
    if (floating && (!primary_.valid() || description.owner != primary_)) valid = false;
    if (!valid) {
        result.status = reject(HostServiceError::invalid_argument);
        return result;
    }
    if (windows_.size() >= maximum_windows ||
        (floating && floating_window_count() >= maximum_floating_windows)) {
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
    // A handler may have destroyed the window; look it up again.
    WindowRecord* reported = record(window);
    if (reported == nullptr || snapshot_.shutdown) return;
    if (window == primary_) {
        if (!(*reported).minimized) follow_primary(*reported);
        return;
    }
    if (host_window_kind_traits((*reported).kind).follows_main_window) learn_offset(*reported);
}

std::size_t HostWindowing::floating_window_count() const noexcept {
    std::size_t count = 0U;
    for (std::size_t index = 0U; index < windows_.size(); ++index) {
        if (host_window_kind_traits(windows_[index].kind).floating) ++count;
    }
    return count;
}

bool HostWindowing::policy_visible(HostWindowId window) const noexcept {
    const WindowRecord* entry = record(window);
    if (entry == nullptr) return false;
    const HostWindowKindTraits traits = host_window_kind_traits((*entry).kind);
    if (!traits.floating) return (*entry).requested_visible;
    const WindowRecord* primary = record(primary_);
    const bool primary_minimized = primary != nullptr && (*primary).minimized;
    const bool hidden_by_inactivity =
        traits.hides_when_application_inactive && !snapshot_.application_active;
    const bool visible = (*entry).requested_visible && !hidden_by_inactivity && !primary_minimized;
    return visible;
}

void HostWindowing::apply_visibility(WindowRecord& entry) {
    if (!host_window_kind_traits(entry.kind).floating) return;
    const HostWindowId window = entry.id;
    const bool wanted = policy_visible(window);
    if (wanted == entry.shown) return;
    const HostServiceStatus status = wanted ? show_impl(window) : hide_impl(window);
    WindowRecord* current = record(window);
    if (current == nullptr || !status.accepted()) return;
    (*current).shown = wanted;
    if (wanted) ++snapshot_.policy_shows;
    else ++snapshot_.policy_hides;
}

void HostWindowing::apply_floating_visibility() {
    // A native show or hide may reach handlers that open or destroy windows;
    // walk a copy of the identities (at most maximum_windows).
    std::array<HostWindowId, maximum_windows> identities{};
    std::size_t count = 0U;
    for (std::size_t index = 0U; index < windows_.size() && count < identities.size(); ++index) {
        if (host_window_kind_traits(windows_[index].kind).floating) {
            identities[count] = windows_[index].id;
            ++count;
        }
    }
    for (std::size_t index = 0U; index < count; ++index) {
        WindowRecord* entry = record(identities[index]);
        if (entry != nullptr) apply_visibility(*entry);
    }
}

void HostWindowing::window_created(HostWindowId window, bool initially_visible) noexcept {
    WindowRecord* entry = record(window);
    if (entry == nullptr) return;
    (*entry).requested_visible = initially_visible;
    (*entry).shown = false;
    try {
        apply_visibility(*entry);
    } catch (...) {
        // The adapter's show failed; the window stays hidden and the next
        // policy change tries again.
    }
}

HostServiceStatus HostWindowing::set_visible(HostWindowId window, bool visible) {
    if (std::this_thread::get_id() != ui_thread_) return reject(HostServiceError::wrong_thread);
    if (snapshot_.shutdown) return reject(HostServiceError::after_shutdown);
    WindowRecord* entry = record(window);
    if (entry == nullptr) return reject(HostServiceError::invalid_argument);
    (*entry).requested_visible = visible;
    if (host_window_kind_traits((*entry).kind).floating) {
        apply_visibility(*entry);
        return {};
    }
    const HostServiceStatus status = visible ? show_impl(window) : hide_impl(window);
    WindowRecord* current = record(window);
    if (current != nullptr && status.accepted()) (*current).shown = visible;
    if (!status.accepted()) ++snapshot_.rejected_requests;
    return status;
}

void HostWindowing::report_application_active(bool active) {
    if (std::this_thread::get_id() != ui_thread_ || snapshot_.shutdown) return;
    if (snapshot_.application_active == active) return;
    snapshot_.application_active = active;
    apply_floating_visibility();
}

void HostWindowing::report_minimized(HostWindowId window, bool minimized) {
    if (std::this_thread::get_id() != ui_thread_ || snapshot_.shutdown) return;
    WindowRecord* entry = record(window);
    if (entry == nullptr || (*entry).minimized == minimized) return;
    (*entry).minimized = minimized;
    if (window == primary_) apply_floating_visibility();
}

void HostWindowing::report_hidden_on_close(HostWindowId window) noexcept {
    WindowRecord* entry = record(window);
    if (entry == nullptr) return;
    (*entry).requested_visible = false;
    (*entry).shown = false;
}

void HostWindowing::follow_primary(WindowRecord& primary) {
    const HostWindowId primary_id = primary.id;
    // The primary window's frame now, from the platform: a report may be
    // older than the native state (macOS posts move notifications late).
    const HostWindowFrameResult current = frame_impl(primary_id);
    WindowRecord* main = record(primary_id);
    if (main == nullptr) return;
    const HostWindowFrame main_frame = current.status.accepted() ? current.frame : (*main).frame;
    const Point main_origin{main_frame.content.x, main_frame.content.y};
    (*main).followed_origin = main_origin;
    (*main).followed_known = true;

    std::array<HostWindowId, maximum_floating_windows> followers{};
    std::size_t count = 0U;
    for (std::size_t index = 0U; index < windows_.size() && count < followers.size(); ++index) {
        const WindowRecord& candidate = windows_[index];
        if (candidate.owner != primary_id || !candidate.frame_known || !candidate.offset_known) continue;
        if (!host_window_kind_traits(candidate.kind).follows_main_window) continue;
        followers[count] = candidate.id;
        ++count;
    }
    if (count == 0U) return;
    const HostMonitorResult monitors = query_monitors_impl();
    const bool clamp = monitors.status.accepted() && !monitors.monitors.empty();
    for (std::size_t index = 0U; index < count; ++index) {
        const HostWindowId follower_id = followers[index];
        WindowRecord* follower = record(follower_id);
        if (follower == nullptr) continue;
        const HostWindowFrameResult fresh = frame_impl(follower_id);
        follower = record(follower_id);
        if (follower == nullptr) continue;
        const HostWindowFrame frame = fresh.status.accepted() ? fresh.frame : (*follower).frame;
        // The decorations around the client area, kept as they are.
        const double left = frame.content.x - frame.outer.x;
        const double top = frame.content.y - frame.outer.y;
        const Point target{main_origin.x + (*follower).offset.x, main_origin.y + (*follower).offset.y};
        const Rect target_outer{target.x - left, target.y - top, frame.outer.width, frame.outer.height};
        Rect placed_outer = target_outer;
        if (clamp) {
            const Rect work = follow_work_area(monitors.monitors, target_outer, main_frame.outer);
            placed_outer = clamp_to_work_area(target_outer, work);
        }
        const Rect placed{placed_outer.x + left, placed_outer.y + top, frame.content.width,
                          frame.content.height};
        const Point placed_origin{placed.x, placed.y};
        if (same_point(placed_origin, {frame.content.x, frame.content.y})) continue;
        // Remember the placement before asking for it: Win32 reports the
        // move before request_frame_impl returns.
        for (std::size_t slot = (*follower).recent_targets.size() - 1U; slot > 0U; --slot) {
            (*follower).recent_targets[slot] = (*follower).recent_targets[slot - 1U];
        }
        (*follower).recent_targets[0] = placed_origin;
        if ((*follower).recent_target_count < (*follower).recent_targets.size()) {
            ++(*follower).recent_target_count;
        }
        ++snapshot_.follow_moves;
        if (!(placed_outer == target_outer)) ++snapshot_.clamped_follow_moves;
        static_cast<void>(request_frame_impl(follower_id, placed));
    }
}

void HostWindowing::learn_offset(WindowRecord& follower) {
    const HostWindowId follower_id = follower.id;
    const WindowRecord* main = record(follower.owner);
    if (main == nullptr || !(*main).frame_known || (*main).minimized) return;
    const HostWindowId primary_id = (*main).id;
    const HostWindowFrameResult main_now = frame_impl(primary_id);
    main = record(primary_id);
    if (main == nullptr) return;
    const Rect main_content = main_now.status.accepted() ? main_now.frame.content : (*main).frame.content;
    const Point main_origin{main_content.x, main_content.y};
    // The primary window has moved since its followers were placed: this
    // move is the platform carrying the follower (macOS child windows), and
    // the primary window's own report places it.
    if ((*main).followed_known && !same_point(main_origin, (*main).followed_origin)) return;
    const HostWindowFrameResult follower_now = frame_impl(follower_id);
    WindowRecord* entry = record(follower_id);
    if (entry == nullptr) return;
    const Rect content = follower_now.status.accepted() ? follower_now.frame.content : (*entry).frame.content;
    const Point origin{content.x, content.y};
    for (std::size_t slot = 0U; slot < (*entry).recent_target_count; ++slot) {
        if (same_point(origin, (*entry).recent_targets[slot])) return;
    }
    const Point offset{origin.x - main_origin.x, origin.y - main_origin.y};
    if ((*entry).offset_known && same_point(offset, (*entry).offset)) return;
    (*entry).offset = offset;
    (*entry).offset_known = true;
    (*entry).recent_target_count = 0U;
}

} // namespace gui_forms
