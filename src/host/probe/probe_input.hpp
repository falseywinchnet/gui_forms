#pragma once

// The forensic frame-drag probe's host seam (stage DK decision 49): synthetic
// pointer input and display timing for gui_forms_frame_drag_probe only. It is
// not part of GUI.Forms: no library links it, and no GUI.Forms type crosses
// it. Each host implements it in src/host/<host>/probe/ with that host's own
// API, so no AppKit, Win32 or X11 type leaves its host directory.
//
// Coordinates are the platform's native global pointer coordinates (Windows
// virtual-screen pixels, macOS Quartz global display points, X11 root-window
// pixels). The probe never assumes how they map to GUI.Forms desktop units:
// it calibrates the mapping by warping the cursor and reading
// ApplicationWindows::pointer_position.
//
// Timestamps are std::chrono::steady_clock nanoseconds, the probe's clock.

#include <cstdint>
#include <string>

namespace gui_forms::probe {

// Where synthetic pointer input enters.
enum class InputPath : std::uint8_t {
    // Posted to the operating system's input stream (Windows SendInput,
    // macOS CGEventPost, X11 XTest): the events reach the window as a user's
    // would, through the host's own pointer handling.
    os_injection,
    // The cursor is warped without generating input, and the probe hands the
    // matching pointer event to the window model on the UI thread. It skips
    // the OS input path; it is the mode for a machine that refuses
    // injection (macOS without Accessibility permission).
    in_process,
};

[[nodiscard]] const char* input_path_name(InputPath path) noexcept;

enum class PointerStep : std::uint8_t {
    move,     // pointer moves with no button down
    press,    // primary button down at the point
    drag,     // pointer moves with the primary button down
    release,  // primary button up at the point
};

struct NativePoint final {
    double x{};
    double y{};
};

struct InputSupport final {
    // Whether os_injection can be used now.
    bool os_injection{};
    // The API os_injection uses ("SendInput", "CGEventPost", "XTest").
    std::string injection_api{};
    // Why os_injection is unavailable; empty when it is available.
    std::string unavailable_reason{};
    // The API the in-process mode warps the cursor with.
    std::string warp_api{};
};

// The display's refresh timing, sampled when called.
struct DisplayTiming final {
    // Whether refresh_period_nanoseconds came from the platform; when false
    // the probe assumes 60 Hz and says so in its results.
    bool period_available{};
    // Whether sample_refresh can report the compositor's vertical blanks
    // (DWM on Windows).
    bool refresh_counter{};
    // The API the values came from, or why there is none.
    std::string source{};
    std::uint64_t refresh_period_nanoseconds{};
};

// The compositor's latest vertical blank, in the probe's steady clock, and
// its refresh counter (from an arbitrary origin).
struct RefreshSample final {
    std::uint64_t last_vblank_nanoseconds{};
    std::uint64_t refresh_count{};
};

// What this host offers. Opens any connection the seam needs; call it on
// the UI thread before the other functions.
[[nodiscard]] InputSupport input_support();

// The cursor's current native position. False when it cannot be read.
[[nodiscard]] bool native_cursor(NativePoint& position);

// Moves the cursor. Windows and X11 also report the move to the window as
// pointer motion; the probe's in-process mode ignores pointer events the host
// delivers, so only its own dispatched events drive the drag. UI thread.
[[nodiscard]] bool warp_native_cursor(NativePoint position);

// Posts one step to the OS input stream (os_injection only). Callable from
// one injecting thread at a time; the probe's injector worker is that
// thread. False when the platform refused the event.
[[nodiscard]] bool inject(PointerStep step, NativePoint position);

// Samples the display timing for the display the cursor is on. UI thread;
// once per run (it may allocate).
[[nodiscard]] DisplayTiming display_timing();

// Samples the refresh counter without allocating, for every frame report.
// False where DisplayTiming::refresh_counter is false. UI thread.
[[nodiscard]] bool sample_refresh(RefreshSample& sample) noexcept;

// Closes any connection input_support opened. UI thread, after the
// injector has stopped.
void release() noexcept;

} // namespace gui_forms::probe
