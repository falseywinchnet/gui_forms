#pragma once

#include "gui_forms/event.hpp"
#include "gui_forms/host/types/host_types.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

namespace gui_forms {

// Identity of one window within one application run. Identities are
// assigned from 1 in admission order and never reused within a run, so a
// destroyed window's identity cannot address a later window.
struct HostWindowId final {
    std::uint64_t value{};

    [[nodiscard]] bool valid() const noexcept { return value != 0U; }
    friend bool operator==(HostWindowId left, HostWindowId right) noexcept {
        return left.value == right.value;
    }
    friend bool operator!=(HostWindowId left, HostWindowId right) noexcept {
        return left.value != right.value;
    }
};

// What a window is to the run. The kind is fixed when the window is
// admitted.
enum class HostWindowKind : std::uint8_t {
    // The run's one main window: no owner, never destroyed through the
    // windowing seam (it closes, and the run ends).
    primary,
    // An owned window with ordinary decorations (ApplicationWindow with an
    // owner_id and no tool_window flag).
    standard,
    // ApplicationWindow::tool_window: the host's utility style, with each
    // host's existing behaviour (see docs/user/hosts-and-capabilities.md).
    tool,
};

[[nodiscard]] const char* host_window_kind_name(HostWindowKind kind) noexcept;

// What an adapter tells the windowing seam about a window it is about to
// create. `stable_id` is borrowed for the call.
struct HostWindowDescription final {
    std::string_view stable_id{};
    // Invalid for a window without an owner.
    HostWindowId owner{};
    HostWindowKind kind{HostWindowKind::standard};
};

struct HostWindowAdmission final {
    HostServiceStatus status{};
    HostWindowId window{};
};

// A window's rectangle in desktop logical units (desktop_coordinates.hpp).
struct HostWindowFrame final {
    // The client area: the rectangle the window's model lays out and paints.
    Rect content{};
    // The whole window including native decorations where the host measures
    // them (outer_measured); equal to `content` where it cannot (an X11
    // window manager that publishes no frame extents).
    Rect outer{};
    bool outer_measured{};

    friend bool operator==(const HostWindowFrame& left,
                           const HostWindowFrame& right) noexcept {
        return left.content == right.content && left.outer == right.outer &&
               left.outer_measured == right.outer_measured;
    }
};

struct HostWindowFrameResult final {
    HostServiceStatus status{};
    HostWindowFrame frame{};
};

// Published once for every native move or resize the adapter reports,
// whoever caused it (the user, a frame request, the platform).
struct HostWindowFrameEvent final {
    HostWindowId window{};
    HostWindowFrame frame{};
};

struct HostScreenPointResult final {
    HostServiceStatus status{};
    Point position{};
};

// An accepted result whose `window` is invalid means no window of this run
// is the topmost window at the point (the desktop, or another
// application's window, is).
struct HostWindowAtResult final {
    HostServiceStatus status{};
    HostWindowId window{};
};

struct HostWindowingSnapshot final {
    HostCapabilities capabilities{};
    std::size_t live_windows{};
    std::uint64_t admitted{};
    std::uint64_t retired{};
    std::uint64_t frame_requests{};
    std::uint64_t frame_reports{};
    std::uint64_t rejected_requests{};
    bool started{};
    bool shutdown{};

    [[nodiscard]] std::string to_json() const;
};

// Application-scope windowing for one run (stage DK decisions 6 and 7):
// the table of the run's live windows, runtime destruction, window frames
// in desktop logical units, the pointer's desktop position, the window
// under a desktop point and ordering within the application.
//
// The base class owns the UI-thread check, capability gating, identities,
// the bounds and validation, so every adapter applies the same rules; an
// adapter implements the `*_impl` operations for its platform. A request a
// host cannot perform reports `unsupported` (HostCapability bits
// runtime_windows, window_frames, screen_pointer, window_at_point and
// window_ordering).
//
// Opening a window is not a member: it needs the window's model and
// callbacks, which each adapter receives in its own entry type. The adapter
// calls admit() before it creates the native window and retire() when the
// native window is gone, whatever closed it.
//
// Every member runs on the thread that constructed the object. Requests
// from another thread report wrong_thread; requests after shutdown()
// report after_shutdown; unknown or retired identities are
// invalid_argument.
class HostWindowing {
public:
    static constexpr std::size_t maximum_windows = 64U;
    static constexpr std::size_t maximum_stable_id_bytes = 1024U;
    // Frame requests: each extent 1 to 32768 logical units, each origin
    // coordinate within plus or minus this bound.
    static constexpr double maximum_desktop_coordinate = 1000000.0;

    explicit HostWindowing(HostCapabilities capabilities);
    virtual ~HostWindowing();
    HostWindowing(const HostWindowing&) = delete;
    HostWindowing& operator=(const HostWindowing&) = delete;

    // Program requests.

    // The window's current frame, read from the platform.
    [[nodiscard]] HostWindowFrameResult frame(HostWindowId window);
    // Asks the platform to give the window's client area the rectangle
    // `content` in desktop logical units. Acceptance means the native
    // request was submitted; the new frame arrives through frame_changed,
    // possibly before this returns, possibly on a later event (X11), and the
    // platform or window manager may adjust it. Extents below the window's
    // minimum size are invalid_argument (adapter rule, as
    // ApplicationWindowHandle::request_client_size).
    [[nodiscard]] HostServiceStatus request_frame(HostWindowId window, Rect content);
    [[nodiscard]] HostScreenPointResult pointer_position();
    // The run's window that is topmost at `point` (desktop logical units),
    // counting native decorations where the host can.
    [[nodiscard]] HostWindowAtResult window_at(Point point);
    // Orders `window` above every other window of this run, never above
    // other applications' windows, without activating it. Owned windows
    // stay above their owners as each platform keeps them.
    [[nodiscard]] HostServiceStatus bring_to_front(HostWindowId window);
    // Closes and destroys a window opened by this run without asking its
    // closing handler: the program decided. Its closed callback runs before
    // this returns and its identity is retired. The primary window and a
    // window that still owns live windows are invalid_argument.
    [[nodiscard]] HostServiceStatus destroy_window(HostWindowId window);
    // Monitors in desktop logical units.
    [[nodiscard]] HostMonitorResult query_monitors();

    // Table queries (no capability needed).
    [[nodiscard]] std::optional<HostWindowId> find(std::string_view stable_id) const noexcept;
    [[nodiscard]] bool live(HostWindowId window) const noexcept;
    [[nodiscard]] std::optional<HostWindowKind> kind(HostWindowId window) const noexcept;
    // The owner, or an invalid identity for a window without one. Empty for
    // an unknown window.
    [[nodiscard]] std::optional<HostWindowId> owner(HostWindowId window) const noexcept;
    [[nodiscard]] HostWindowId primary() const noexcept { return primary_; }
    [[nodiscard]] std::size_t window_count() const noexcept { return windows_.size(); }
    [[nodiscard]] const HostCapabilities& capabilities() const noexcept {
        return snapshot_.capabilities;
    }
    [[nodiscard]] Event<const HostWindowFrameEvent&>& frame_changed() noexcept {
        return frame_changed_;
    }
    [[nodiscard]] HostWindowingSnapshot snapshot() const;
    // Ends the run's windowing: every later request reports after_shutdown
    // and the table is cleared. Called by the adapter as the run ends.
    void shutdown() noexcept;

    // Adapter protocol: only the adapter that owns this object calls these,
    // on the UI thread.

    // Validates and records a window before its native creation. Rules: the
    // stable id is 1 to 1024 bytes of UTF-8 without NUL and unique among
    // live windows; at most maximum_windows live windows; the owner, when
    // given, is live; exactly one primary, admitted first, without an
    // owner; a standard window has an owner. After mark_started() an
    // admission needs runtime_windows and may not be primary. A refused
    // admission changes nothing.
    [[nodiscard]] HostWindowAdmission admit(const HostWindowDescription& description);
    // The initial window set is complete; later admissions open windows
    // during the run.
    void mark_started() noexcept;
    // The native window is gone. Unknown identities are ignored.
    void retire(HostWindowId window) noexcept;
    // A native move or resize happened (or the first frame is known). The
    // frame is recorded and frame_changed is published.
    void report_frame(HostWindowId window, const HostWindowFrame& frame);

protected:
    // Called only after validation: a live identity, a finite request in
    // range, the capability present, the UI thread.
    [[nodiscard]] virtual HostWindowFrameResult frame_impl(HostWindowId window) = 0;
    [[nodiscard]] virtual HostServiceStatus request_frame_impl(HostWindowId window,
                                                               Rect content) = 0;
    [[nodiscard]] virtual HostScreenPointResult pointer_position_impl() = 0;
    // May return any native result; the base accepts only a live identity
    // or an invalid one.
    [[nodiscard]] virtual HostWindowAtResult window_at_impl(Point point) = 0;
    [[nodiscard]] virtual HostServiceStatus bring_to_front_impl(HostWindowId window) = 0;
    // Must close the native window, run its closed notification and call
    // retire() before returning accepted.
    [[nodiscard]] virtual HostServiceStatus destroy_window_impl(HostWindowId window) = 0;
    [[nodiscard]] virtual HostMonitorResult query_monitors_impl() = 0;

private:
    struct WindowRecord final {
        HostWindowId id{};
        std::string stable_id{};
        HostWindowId owner{};
        HostWindowKind kind{HostWindowKind::standard};
        HostWindowFrame frame{};
        bool frame_known{};
    };

    [[nodiscard]] HostServiceStatus validate_request(HostCapability capability) noexcept;
    [[nodiscard]] HostServiceStatus validate_window_request(HostCapability capability,
                                                            HostWindowId window) noexcept;
    [[nodiscard]] HostServiceStatus reject(HostServiceError error) noexcept;
    [[nodiscard]] WindowRecord* record(HostWindowId window) noexcept;
    [[nodiscard]] const WindowRecord* record(HostWindowId window) const noexcept;

    std::thread::id ui_thread_;
    HostWindowingSnapshot snapshot_;
    // Live windows in admission order. Capacity maximum_windows is reserved
    // at construction; admission refuses beyond it, so the table never
    // reallocates during a run.
    std::vector<WindowRecord> windows_;
    std::uint64_t next_window_{1U};
    HostWindowId primary_{};
    Event<const HostWindowFrameEvent&> frame_changed_;
};

} // namespace gui_forms
