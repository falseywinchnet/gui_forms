// The forensic frame-drag probe's seam on X11 (src/host/probe/
// probe_input.hpp): XTest for injection, XWarpPointer for the in-process
// warp. XTest is linked when the build finds it (libXtst; CMake's
// X11::Xtst); without it the probe runs in-process. No refresh source is
// read (the Present and RandR extensions are not linked): the probe assumes
// 60 Hz and confirms frames by ConfigureNotify, through the host's
// frame_changed. Xlib types stay in this file.
#include "probe/probe_input.hpp"

#include <X11/Xlib.h>
#if defined(GUI_FORMS_PROBE_HAS_XTEST)
#include <X11/extensions/XTest.h>
#endif

#include <cmath>
#include <mutex>

namespace gui_forms::probe {
namespace {

// The seam's own connection, separate from the host's. Its calls come from
// the UI thread (cursor reads and warps) and from the injector worker, one
// at a time under `mutex`.
struct Connection final {
    std::mutex mutex{};
    Display* display{};
    bool xtest{};
    bool opened{};
    std::string xtest_reason{};
};

Connection connection{};

// Opens the connection and asks for XTest once. Caller holds the mutex.
void open_locked() {
    if (connection.opened) return;
    connection.opened = true;
    connection.display = XOpenDisplay(nullptr);
    if (connection.display == nullptr) {
        connection.xtest_reason = "XOpenDisplay failed (no DISPLAY?)";
        return;
    }
#if defined(GUI_FORMS_PROBE_HAS_XTEST)
    int event_base = 0;
    int error_base = 0;
    int major = 0;
    int minor = 0;
    connection.xtest = XTestQueryExtension(connection.display, &event_base, &error_base, &major, &minor) != False;
    if (!connection.xtest) connection.xtest_reason = "the X server does not offer the XTEST extension";
#else
    connection.xtest_reason = "the probe was built without libXtst (install libxtst-dev and reconfigure)";
#endif
}

} // namespace

InputSupport input_support() {
    const std::scoped_lock<std::mutex> lock(connection.mutex);
    open_locked();
    InputSupport support{};
    support.injection_api = "XTest";
    support.warp_api = "XWarpPointer";
    support.os_injection = connection.xtest;
    support.unavailable_reason = connection.xtest_reason;
    return support;
}

bool native_cursor(NativePoint& position) {
    const std::scoped_lock<std::mutex> lock(connection.mutex);
    open_locked();
    if (connection.display == nullptr) return false;
    const Window root = DefaultRootWindow(connection.display);
    Window root_return = 0;
    Window child_return = 0;
    int root_x = 0;
    int root_y = 0;
    int window_x = 0;
    int window_y = 0;
    unsigned int mask = 0U;
    const Bool found = XQueryPointer(connection.display, root, &root_return, &child_return, &root_x, &root_y,
                                     &window_x, &window_y, &mask);
    if (found == False) return false;
    position = {static_cast<double>(root_x), static_cast<double>(root_y)};
    return true;
}

bool warp_native_cursor(NativePoint position) {
    const std::scoped_lock<std::mutex> lock(connection.mutex);
    open_locked();
    if (connection.display == nullptr) return false;
    const Window root = DefaultRootWindow(connection.display);
    const int x = static_cast<int>(std::lround(position.x));
    const int y = static_cast<int>(std::lround(position.y));
    XWarpPointer(connection.display, None, root, 0, 0, 0U, 0U, x, y);
    XSync(connection.display, False);
    return true;
}

bool inject(PointerStep step, NativePoint position) {
    const std::scoped_lock<std::mutex> lock(connection.mutex);
    open_locked();
    if (!connection.xtest) return false;
#if defined(GUI_FORMS_PROBE_HAS_XTEST)
    const int x = static_cast<int>(std::lround(position.x));
    const int y = static_cast<int>(std::lround(position.y));
    // Screen -1 is the screen the pointer is on; delay 0 is CurrentTime.
    int status = XTestFakeMotionEvent(connection.display, -1, x, y, 0UL);
    if (step == PointerStep::press && status != 0) status = XTestFakeButtonEvent(connection.display, 1U, True, 0UL);
    if (step == PointerStep::release && status != 0) status = XTestFakeButtonEvent(connection.display, 1U, False, 0UL);
    XFlush(connection.display);
    const bool accepted = status != 0;
    return accepted;
#else
    static_cast<void>(step);
    static_cast<void>(position);
    return false;
#endif
}

DisplayTiming display_timing() {
    DisplayTiming timing{};
    timing.source = "none (X11 Present and RandR are not used; 60 Hz assumed)";
    return timing;
}

bool sample_refresh(RefreshSample&) noexcept {
    return false;
}

void release() noexcept {
    const std::scoped_lock<std::mutex> lock(connection.mutex);
    if (connection.display != nullptr) XCloseDisplay(connection.display);
    connection.display = nullptr;
    connection.xtest = false;
    connection.opened = false;
}

} // namespace gui_forms::probe
