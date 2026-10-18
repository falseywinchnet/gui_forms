// Application-scope windowing on Win32 (stage DK phase W0, decisions 6 and
// 7). Included inside windows_host.cpp's anonymous namespace after
// WindowsHostState and window_procedure.
//
// The run owns every window's WindowsHostState through a slot. Initial
// windows keep their slot until the run returns, as before; a window that
// closes during the run keeps its slot until the end of the loop turn in
// which it closed (reclaim), so no state is released inside one of its own
// messages.
//
// Desktop units: the process is per-monitor DPI aware, so Win32 screen
// coordinates are device pixels. Frames, the pointer and work areas are
// converted to desktop logical units through the portable per-monitor
// mapping (gui_forms/host/windowing/desktop_coordinates.hpp); a window's
// extent uses the window's own DPI.

struct WindowsRunSlot final {
    std::unique_ptr<WindowsHostState> state{};
    HostWindowId id{};
    std::string stable_id{};
    bool initial{};
    bool print_metrics_on_close{};
    std::function<void(std::string_view metrics_json, std::string_view host_json)> final_snapshot{};
    std::string capture_path{};
};

BOOL CALLBACK collect_device_monitor(HMONITOR monitor, HDC, LPRECT, LPARAM context) {
    std::vector<HostMonitor>& monitors = *reinterpret_cast<std::vector<HostMonitor>*>(context);
    MONITORINFOEXW info{};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(monitor, &info)) return TRUE;
    HostMonitor value;
    value.id = utf8_from_wide(info.szDevice);
    if (value.id.empty()) {
        value.id = "win32.monitor." + std::to_string(reinterpret_cast<std::uintptr_t>(monitor));
    }
    value.frame = {static_cast<double>(info.rcMonitor.left), static_cast<double>(info.rcMonitor.top),
                   static_cast<double>(info.rcMonitor.right - info.rcMonitor.left),
                   static_cast<double>(info.rcMonitor.bottom - info.rcMonitor.top)};
    value.work_area = {static_cast<double>(info.rcWork.left), static_cast<double>(info.rcWork.top),
                       static_cast<double>(info.rcWork.right - info.rcWork.left),
                       static_cast<double>(info.rcWork.bottom - info.rcWork.top)};
    value.scale = native_monitor_scale(monitor);
    value.primary = (info.dwFlags & MONITORINFOF_PRIMARY) != 0U;
    monitors.push_back(std::move(value));
    return TRUE;
}

// The monitors in Win32 device desktop units, as HostServices reports them.
std::vector<HostMonitor> device_monitors() {
    std::vector<HostMonitor> monitors;
    monitors.reserve(8U);
    EnumDisplayMonitors(nullptr, nullptr, &collect_device_monitor,
                        reinterpret_cast<LPARAM>(&monitors));
    return monitors;
}

using DwmGetWindowAttributeFunction = HRESULT(WINAPI*)(HWND, DWORD, PVOID, DWORD);

// The visible window bounds without the invisible resize borders Windows 10
// and later add around a sizable window (DWMWA_EXTENDED_FRAME_BOUNDS, 9),
// read through dwmapi.dll when it is present; else GetWindowRect.
DwmGetWindowAttributeFunction load_dwm_get_window_attribute() noexcept {
    HMODULE library = LoadLibraryW(L"dwmapi.dll");
    if (library == nullptr) return nullptr;
    const DwmGetWindowAttributeFunction function =
        load_function<DwmGetWindowAttributeFunction>(library, "DwmGetWindowAttribute");
    return function;
}

bool visible_window_bounds(HWND window, RECT& bounds) noexcept {
    static const DwmGetWindowAttributeFunction get_attribute = load_dwm_get_window_attribute();
    constexpr DWORD extended_frame_bounds = 9U;
    if (get_attribute != nullptr &&
        SUCCEEDED(get_attribute(window, extended_frame_bounds, &bounds, sizeof(bounds)))) {
        return true;
    }
    const bool measured = GetWindowRect(window, &bounds) != FALSE;
    return measured;
}

class WindowsApplicationRun final : public HostWindowing {
public:
    // The maximum slots: every initial window (at most 64) plus the live
    // runtime windows (at most 64 together with the initial ones that are
    // still open) and those closed in the current turn.
    static constexpr std::size_t reserved_slots = 2U * HostWindowing::maximum_windows;

    explicit WindowsApplicationRun(HINSTANCE instance)
        : HostWindowing(windows_capabilities()), instance_(instance) {
        slots_.reserve(reserved_slots);
    }
    WindowsApplicationRun(const WindowsApplicationRun&) = delete;
    WindowsApplicationRun& operator=(const WindowsApplicationRun&) = delete;

    struct OpenOutcome final {
        HostWindowAdmission admission{};
        // run_windows_application's exit code for an initial window that
        // could not be created: 2 invalid, 4 native creation, 5 attach.
        int failure_code{};
    };

    // Creates one window: admission, the native window owned by its owner's
    // window, attachment and the ready callbacks. A runtime window is then
    // shown when initially_visible; initial windows are shown by the caller
    // once every initial window exists.
    OpenOutcome open(WindowsApplicationWindow entry, bool runtime);

    [[nodiscard]] std::size_t slot_count() const noexcept { return slots_.size(); }
    [[nodiscard]] WindowsRunSlot& slot_at(std::size_t index) noexcept { return *slots_[index]; }

    // Reports the native window's current frame (WM_MOVE, WM_SIZE).
    void report_native_frame(HostWindowId window, HWND native);
    // Releases runtime windows that closed during the turn, after printing
    // their metrics as the run prints every window's at exit.
    void reclaim();
    // Prints one window's final metrics and snapshot.
    static void publish_final_metrics(WindowsRunSlot& slot);

protected:
    HostWindowFrameResult frame_impl(HostWindowId window) override;
    HostServiceStatus request_frame_impl(HostWindowId window, Rect content) override;
    HostScreenPointResult pointer_position_impl() override;
    HostWindowAtResult window_at_impl(Point point) override;
    HostServiceStatus bring_to_front_impl(HostWindowId window) override;
    HostServiceStatus destroy_window_impl(HostWindowId window) override;
    HostMonitorResult query_monitors_impl() override;

private:
    [[nodiscard]] WindowsRunSlot* slot(HostWindowId window) noexcept;
    [[nodiscard]] WindowsRunSlot* slot(std::string_view stable_id) noexcept;
    [[nodiscard]] HWND native(HostWindowId window) noexcept;

    HINSTANCE instance_{};
    // Stable addresses: a window's callbacks may open another window while
    // its own slot is in use.
    std::vector<std::unique_ptr<WindowsRunSlot>> slots_;
};

// The window frame in desktop logical units.
HostWindowFrame native_window_frame(HWND window) {
    HostWindowFrame frame;
    RECT client{};
    POINT origin{0, 0};
    if (!GetClientRect(window, &client) || !ClientToScreen(window, &origin)) return frame;
    const std::vector<HostMonitor> monitors = device_monitors();
    const double scale = query_scale(window);
    const Point content_origin = desktop_logical_point(
        monitors, {static_cast<double>(origin.x), static_cast<double>(origin.y)});
    frame.content = {content_origin.x, content_origin.y,
                     static_cast<double>(client.right - client.left) / scale,
                     static_cast<double>(client.bottom - client.top) / scale};
    RECT outer{};
    if (visible_window_bounds(window, outer)) {
        const Point outer_origin = desktop_logical_point(
            monitors, {static_cast<double>(outer.left), static_cast<double>(outer.top)});
        frame.outer = {outer_origin.x, outer_origin.y,
                       static_cast<double>(outer.right - outer.left) / scale,
                       static_cast<double>(outer.bottom - outer.top) / scale};
        frame.outer_measured = true;
    } else {
        frame.outer = frame.content;
    }
    return frame;
}

WindowsRunSlot* WindowsApplicationRun::slot(HostWindowId window) noexcept {
    for (std::size_t index = 0U; index < slots_.size(); ++index) {
        WindowsRunSlot& candidate = *slots_[index];
        if (candidate.id == window && candidate.state && !(*candidate.state).destroyed()) {
            return &candidate;
        }
    }
    return nullptr;
}

WindowsRunSlot* WindowsApplicationRun::slot(std::string_view stable_id) noexcept {
    for (std::size_t index = 0U; index < slots_.size(); ++index) {
        WindowsRunSlot& candidate = *slots_[index];
        if (candidate.stable_id == stable_id && live(candidate.id)) return &candidate;
    }
    return nullptr;
}

HWND WindowsApplicationRun::native(HostWindowId window) noexcept {
    WindowsRunSlot* found = slot(window);
    if (found == nullptr) return nullptr;
    return (*(*found).state).native_window();
}

WindowsApplicationRun::OpenOutcome WindowsApplicationRun::open(WindowsApplicationWindow entry,
                                                               bool runtime) {
    OpenOutcome outcome;
    if (!entry.model || entry.stable_id.empty()) {
        outcome.admission.status.error = HostServiceError::invalid_argument;
        outcome.failure_code = 2;
        return outcome;
    }
    HostWindowId owner_id{};
    HWND owner{};
    if (!entry.owner_id.empty()) {
        WindowsRunSlot* owner_slot = slot(std::string_view(entry.owner_id));
        if (owner_slot == nullptr) {
            outcome.admission.status.error = HostServiceError::invalid_argument;
            outcome.failure_code = 2;
            return outcome;
        }
        owner_id = (*owner_slot).id;
        owner = (*(*owner_slot).state).native_window();
    }
    HostWindowKind kind = HostWindowKind::standard;
    if (entry.tool_window) kind = HostWindowKind::tool;
    else if (!owner_id.valid()) kind = HostWindowKind::primary;
    HostWindowDescription description;
    description.stable_id = entry.stable_id;
    description.owner = owner_id;
    description.kind = kind;
    outcome.admission = admit(description);
    if (!outcome.admission.status.accepted()) {
        outcome.failure_code = 2;
        return outcome;
    }
    const HostWindowId id = outcome.admission.window;

    std::unique_ptr<WindowsRunSlot> created = std::make_unique<WindowsRunSlot>();
    WindowsRunSlot& window_slot = *created;
    window_slot.id = id;
    window_slot.stable_id = entry.stable_id;
    window_slot.initial = !runtime;
    window_slot.print_metrics_on_close = entry.options.print_metrics_on_close;
    window_slot.final_snapshot = entry.options.final_snapshot;
    window_slot.capture_path = entry.options.capture_path;

    WindowsHostOptions host_options = entry.options;
    host_options.quit_thread_on_close = kind == HostWindowKind::primary;
    const bool initially_visible = entry.options.initially_visible;
    const bool close_after_launch = entry.options.close_after_launch_for_testing;
    window_slot.state = std::make_unique<WindowsHostState>(std::move(entry.model), std::move(host_options));
    (*window_slot.state).attach_run(*this, id);
    const DWORD style = entry.options.popup_window
        ? (WS_POPUP | WS_BORDER)
        : (entry.options.minimizable ? WS_OVERLAPPEDWINDOW
                                     : (WS_OVERLAPPEDWINDOW & ~WS_MINIMIZEBOX));
    const DWORD ex_style = entry.tool_window ? WS_EX_TOOLWINDOW : 0;
    const double initial_scale = query_scale(owner);
    const Size initial_size = entry.options.capture_size.has_value()
        ? *entry.options.capture_size : entry.options.initial_size;
    RECT frame{0, 0, static_cast<LONG>(std::ceil(initial_size.width * initial_scale)),
               static_cast<LONG>(std::ceil(initial_size.height * initial_scale))};
    adjust_client_frame(frame, style, ex_style, initial_scale);
    const std::wstring title = wide_from_utf8(entry.options.title);
    // The slot is published before creation: callbacks during initialization
    // (the ready callback) may already ask for this window's frame.
    slots_.push_back(std::move(created));
    WindowsHostState& state = *(*slots_.back()).state;
    HWND window = CreateWindowExW(ex_style, window_class_name, title.c_str(), style, CW_USEDEFAULT,
                                  CW_USEDEFAULT, frame.right - frame.left, frame.bottom - frame.top,
                                  owner, nullptr, instance_, &state);
    if (window == nullptr || !state.initialize(window)) {
        if (window != nullptr) DestroyWindow(window);
        retire(id);
        outcome.admission.status.error = HostServiceError::backend_failure;
        outcome.admission.window = {};
        outcome.failure_code = window == nullptr ? 4 : 5;
        return outcome;
    }
    report_native_frame(id, window);
    if (!runtime) return outcome;
    if (!state.will_show()) {
        DestroyWindow(window);
        retire(id);
        outcome.admission.status.error = HostServiceError::backend_failure;
        outcome.admission.window = {};
        outcome.failure_code = 5;
        return outcome;
    }
    if (initially_visible) {
        ShowWindow(window, kind == HostWindowKind::tool ? SW_SHOWNOACTIVATE : SW_SHOWNORMAL);
        UpdateWindow(window);
    }
    if (close_after_launch) PostMessageW(window, WM_CLOSE, 0, 0);
    return outcome;
}

void WindowsApplicationRun::report_native_frame(HostWindowId window, HWND native_window) {
    if (!live(window) || native_window == nullptr) return;
    const HostWindowFrame frame = native_window_frame(native_window);
    report_frame(window, frame);
}

void WindowsApplicationRun::publish_final_metrics(WindowsRunSlot& run_slot) {
    if (!run_slot.state) return;
    const std::string metrics = (*run_slot.state).metrics_json();
    const std::string host = (*run_slot.state).host_json();
    if (run_slot.print_metrics_on_close) {
        std::fprintf(stdout, "{\"stable_id\":\"%s\",\"window\":%s,\"host\":%s}\n",
                     run_slot.stable_id.c_str(), metrics.c_str(), host.c_str());
    }
    if (run_slot.final_snapshot) run_slot.final_snapshot(metrics, host);
}

void WindowsApplicationRun::reclaim() {
    for (std::size_t index = slots_.size(); index > 0U; --index) {
        WindowsRunSlot& candidate = *slots_[index - 1U];
        if (candidate.initial || !candidate.state || !(*candidate.state).destroyed()) continue;
        publish_final_metrics(candidate);
        slots_.erase(slots_.begin() + static_cast<std::ptrdiff_t>(index - 1U));
    }
}

HostWindowFrameResult WindowsApplicationRun::frame_impl(HostWindowId window) {
    HostWindowFrameResult result;
    const HWND native_window = native(window);
    if (native_window == nullptr) {
        result.status.error = HostServiceError::after_shutdown;
        return result;
    }
    result.frame = native_window_frame(native_window);
    return result;
}

HostServiceStatus WindowsApplicationRun::request_frame_impl(HostWindowId window, Rect content) {
    WindowsRunSlot* found = slot(window);
    if (found == nullptr) return {HostServiceError::after_shutdown};
    WindowsHostState& state = *(*found).state;
    const HWND native_window = state.native_window();
    if (native_window == nullptr) return {HostServiceError::after_shutdown};
    const Size minimum = state.minimum_size();
    if (content.width < minimum.width || content.height < minimum.height) {
        return {HostServiceError::invalid_argument};
    }
    if (state.frame_fixed()) return {HostServiceError::unsupported};
    const std::vector<HostMonitor> monitors = device_monitors();
    double target_scale = 1.0;
    const Point device_origin = desktop_device_point(monitors, {content.x, content.y}, target_scale);
    RECT frame{0, 0, static_cast<LONG>(std::lround(content.width * target_scale)),
               static_cast<LONG>(std::lround(content.height * target_scale))};
    adjust_client_frame(frame, static_cast<DWORD>(GetWindowLongPtrW(native_window, GWL_STYLE)),
                        static_cast<DWORD>(GetWindowLongPtrW(native_window, GWL_EXSTYLE)),
                        target_scale);
    // `frame` now holds the decorations as negative left and top offsets
    // around the client rectangle placed at the origin.
    const int x = static_cast<int>(std::lround(device_origin.x)) + frame.left;
    const int y = static_cast<int>(std::lround(device_origin.y)) + frame.top;
    // WM_MOVE and WM_SIZE report the new frame before SetWindowPos returns.
    const BOOL moved = SetWindowPos(native_window, nullptr, x, y, frame.right - frame.left,
                                    frame.bottom - frame.top,
                                    SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
    if (moved == FALSE) return {HostServiceError::backend_failure};
    return {};
}

HostScreenPointResult WindowsApplicationRun::pointer_position_impl() {
    HostScreenPointResult result;
    POINT device{};
    if (!GetCursorPos(&device)) {
        result.status.error = HostServiceError::backend_failure;
        return result;
    }
    const std::vector<HostMonitor> monitors = device_monitors();
    result.position = desktop_logical_point(
        monitors, {static_cast<double>(device.x), static_cast<double>(device.y)});
    return result;
}

HostWindowAtResult WindowsApplicationRun::window_at_impl(Point point) {
    HostWindowAtResult result;
    const std::vector<HostMonitor> monitors = device_monitors();
    double scale = 1.0;
    const Point device = desktop_device_point(monitors, point, scale);
    const POINT native_point{static_cast<LONG>(std::lround(device.x)),
                             static_cast<LONG>(std::lround(device.y))};
    // WindowFromPoint skips hidden and disabled windows; its result may be a
    // child, so the top-level window is taken. Owned windows are top level.
    const HWND hit = WindowFromPoint(native_point);
    if (hit == nullptr) return result;
    const HWND top = GetAncestor(hit, GA_ROOT);
    for (std::size_t index = 0U; index < slots_.size(); ++index) {
        WindowsRunSlot& candidate = *slots_[index];
        if (!candidate.state || (*candidate.state).native_window() != top) continue;
        if (live(candidate.id)) result.window = candidate.id;
        break;
    }
    return result;
}

HostServiceStatus WindowsApplicationRun::bring_to_front_impl(HostWindowId window) {
    const HWND native_window = native(window);
    if (native_window == nullptr) return {HostServiceError::after_shutdown};
    // The topmost window of this run in the desktop z-order.
    HWND topmost_own{};
    for (HWND cursor = GetTopWindow(nullptr); cursor != nullptr && topmost_own == nullptr;
         cursor = GetWindow(cursor, GW_HWNDNEXT)) {
        for (std::size_t index = 0U; index < slots_.size(); ++index) {
            WindowsRunSlot& candidate = *slots_[index];
            if (candidate.state && (*candidate.state).native_window() == cursor &&
                live(candidate.id)) {
                topmost_own = cursor;
                break;
            }
        }
    }
    if (topmost_own == native_window) return {};
    // Insert directly above that window: after the window that precedes it,
    // or at the top of the non-topmost band when it is first or follows a
    // topmost window (inserting after a topmost window would not keep this
    // window out of the topmost band). Owned windows follow their owner, as
    // Win32 keeps them, because SWP_NOOWNERZORDER is not passed.
    HWND insert_after = HWND_TOP;
    if (topmost_own != nullptr) {
        const HWND previous = GetWindow(topmost_own, GW_HWNDPREV);
        const LONG_PTR previous_style = previous == nullptr ? 0 : GetWindowLongPtrW(previous, GWL_EXSTYLE);
        if (previous != nullptr && (previous_style & WS_EX_TOPMOST) == 0) insert_after = previous;
    }
    const BOOL ordered = SetWindowPos(native_window, insert_after, 0, 0, 0, 0,
                                      SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    if (ordered == FALSE) return {HostServiceError::backend_failure};
    return {};
}

HostServiceStatus WindowsApplicationRun::destroy_window_impl(HostWindowId window) {
    WindowsRunSlot* found = slot(window);
    if (found == nullptr) return {HostServiceError::after_shutdown};
    // WM_DESTROY publishes the closed event and retires the identity before
    // DestroyWindow returns.
    (*(*found).state).destroy_for_application();
    return {};
}

HostMonitorResult WindowsApplicationRun::query_monitors_impl() {
    HostMonitorResult result;
    const std::vector<HostMonitor> monitors = device_monitors();
    result.monitors.reserve(monitors.size());
    for (std::size_t index = 0U; index < monitors.size(); ++index) {
        result.monitors.push_back(logical_monitor(monitors[index]));
    }
    if (result.monitors.empty()) result.status.error = HostServiceError::backend_failure;
    return result;
}

void WindowsHostState::report_frame_to_run() {
    if (run_ == nullptr || hwnd_ == nullptr) return;
    if (native_phase_ != NativePhase::ready && native_phase_ != NativePhase::visible) return;
    (*run_).report_native_frame(window_id_, hwnd_);
}

void WindowsHostState::retire_from_run() noexcept {
    if (run_ == nullptr) return;
    (*run_).retire(window_id_);
}

// The run's open operation. Only the UI thread reaches the run, and only
// while it lives.
struct WindowsRunOpenState final {
    const DWORD thread{GetCurrentThreadId()};
    WindowsApplicationRun* run{};
};

struct WindowsRunOpen final {
    std::weak_ptr<WindowsRunOpenState> owner{};
    HostWindowAdmission operator()(WindowsApplicationWindow entry) const {
        HostWindowAdmission refused;
        const std::shared_ptr<WindowsRunOpenState> state = owner.lock();
        if (!state || (*state).run == nullptr) {
            refused.status.error = HostServiceError::after_shutdown;
            return refused;
        }
        if ((*state).thread != GetCurrentThreadId()) {
            refused.status.error = HostServiceError::wrong_thread;
            return refused;
        }
        const WindowsApplicationRun::OpenOutcome outcome =
            (*(*state).run).open(std::move(entry), true);
        return outcome.admission;
    }
};

// Revokes the run's open operation and tells the runtime the windowing is
// ending; the run itself is shut down and released by the caller.
void end_run_windowing(WindowsRunOpenState& open_state, WindowsApplicationOptions& options,
                       bool published) {
    open_state.run = nullptr;
    if (published && options.windowing_ended) options.windowing_ended();
}
