// Native verification on X11 (stage DK decision 50): one behaviour of the
// X11 host under a real window manager, checked without anyone watching,
// written as one JSON document.
//
//   gui_forms_x11_behaviour_probe --check NAME --out FILE.json
//
// Checks (tools/native_verification/SIBLING_PROMPT.md says what each means):
//   utility_type              floating windows are _NET_WM_WINDOW_TYPE_UTILITY,
//                             the main window is not
//   skip_taskbar              floating windows keep _NET_WM_STATE_SKIP_TASKBAR,
//                             also after being hidden and shown
//   transient_for             floating windows name the main window in
//                             WM_TRANSIENT_FOR
//   restack_above_owner_only  floating windows stay above the main window and
//                             below another application's window
//   minimized_state           floating windows leave with a minimized main
//                             window and return when it is restored
//   net_wm_state              full screen is requested and released through
//                             _NET_WM_STATE, and the floating windows keep
//                             their state meanwhile
//   xwayland_position         under XWayland, the compositor honours a
//                             client's requested position (skipped on X11)
//
// The probe reads its own windows' properties through a second Xlib
// connection and makes one plain window of its own to stand for another
// application. It exits 0 on pass or skipped, 1 on fail and 2 when the
// result is inconclusive (no EWMH window manager, for example).

// GUI.Forms first: Xlib's macros (None, True, Bool, Status) must not reach
// its headers.
#include "gui_forms/application.hpp"
#include "gui_forms/basic_controls.hpp"
#include "gui_forms/control.hpp"
#include "gui_forms/timer.hpp"

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace {

std::uint64_t now_milliseconds() {
    const std::chrono::steady_clock::duration since = std::chrono::steady_clock::now().time_since_epoch();
    const std::uint64_t value = static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(since).count());
    return value;
}

constexpr const char* main_title = "GUI.Forms behaviour probe";
constexpr const char* tool_title = "GUI.Forms behaviour probe tool";
constexpr const char* document_title = "GUI.Forms behaviour probe document";
constexpr const char* foreign_title = "GUI.Forms behaviour probe foreign window";
constexpr std::uint64_t probe_deadline_ms = 60000U;

enum class Check : std::uint8_t {
    utility_type,
    skip_taskbar,
    transient_for,
    restack_above_owner_only,
    minimized_state,
    net_wm_state,
    xwayland_position,
    unknown,
};

Check parse_check(const char* name) {
    if (std::strcmp(name, "utility_type") == 0) return Check::utility_type;
    if (std::strcmp(name, "skip_taskbar") == 0) return Check::skip_taskbar;
    if (std::strcmp(name, "transient_for") == 0) return Check::transient_for;
    if (std::strcmp(name, "restack_above_owner_only") == 0) return Check::restack_above_owner_only;
    if (std::strcmp(name, "minimized_state") == 0) return Check::minimized_state;
    if (std::strcmp(name, "net_wm_state") == 0) return Check::net_wm_state;
    if (std::strcmp(name, "xwayland_position") == 0) return Check::xwayland_position;
    return Check::unknown;
}

struct Observation final {
    std::string name{};
    std::string json{};
};

std::string json_string(const std::string& text) {
    std::string result{"\""};
    for (const char character : text) {
        const unsigned char code = static_cast<unsigned char>(character);
        if (character == '"' || character == '\\') {
            result.push_back('\\');
            result.push_back(character);
        } else if (code < 0x20U) {
            char escaped[8];
            std::snprintf(escaped, sizeof(escaped), "\\u%04x", static_cast<unsigned int>(code));
            result.append(escaped);
        } else {
            result.push_back(character);
        }
    }
    result.push_back('"');
    return result;
}

std::string json_bool(bool value) {
    const std::string result = value ? "true" : "false";
    return result;
}

// The probe's own connection: reads properties, sends EWMH requests and owns
// the foreign window. UI thread only.
struct Connection final {
    Display* display{};
    ::Window root{};
    ::Window foreign{};
};

Atom atom(const Connection& connection, const char* name) {
    const Atom value = XInternAtom(connection.display, name, False);
    return value;
}

// A 32-bit property as a list of values (Xlib returns them as longs).
std::vector<unsigned long> property_values(const Connection& connection, ::Window window, const char* name) {
    std::vector<unsigned long> values{};
    Atom type = 0;
    int format = 0;
    unsigned long count = 0UL;
    unsigned long after = 0UL;
    unsigned char* data = nullptr;
    const int status = XGetWindowProperty(connection.display, window, atom(connection, name), 0L, 4096L, False,
                                          AnyPropertyType, &type, &format, &count, &after, &data);
    if (status == Success && data != nullptr && format == 32) {
        const unsigned long* const items = reinterpret_cast<const unsigned long*>(data);
        values.assign(items, items + count);
    }
    if (data != nullptr) XFree(data);
    return values;
}

std::string property_text(const Connection& connection, ::Window window, const char* name) {
    std::string text{};
    Atom type = 0;
    int format = 0;
    unsigned long count = 0UL;
    unsigned long after = 0UL;
    unsigned char* data = nullptr;
    const int status = XGetWindowProperty(connection.display, window, atom(connection, name), 0L, 4096L, False,
                                          AnyPropertyType, &type, &format, &count, &after, &data);
    if (status == Success && data != nullptr && format == 8) text.assign(reinterpret_cast<const char*>(data), count);
    if (data != nullptr) XFree(data);
    return text;
}

std::string window_name(const Connection& connection, ::Window window) {
    std::string name = property_text(connection, window, "_NET_WM_NAME");
    if (name.empty()) name = property_text(connection, window, "WM_NAME");
    return name;
}

// Finds a client window by title: through _NET_CLIENT_LIST when a window
// manager publishes it, else by walking the tree (reparenting frames hold
// clients two or three levels down).
::Window find_in_tree(const Connection& connection, ::Window parent, const std::string& title, int depth) {
    if (depth > 3) return 0;
    ::Window root_return = 0;
    ::Window parent_return = 0;
    ::Window* children = nullptr;
    unsigned int count = 0U;
    if (XQueryTree(connection.display, parent, &root_return, &parent_return, &children, &count) == 0) return 0;
    ::Window found = 0;
    for (unsigned int index = 0U; index < count && found == 0; ++index) {
        if (window_name(connection, children[index]) == title) found = children[index];
    }
    for (unsigned int index = 0U; index < count && found == 0; ++index) {
        found = find_in_tree(connection, children[index], title, depth + 1);
    }
    if (children != nullptr) XFree(children);
    return found;
}

::Window find_window(const Connection& connection, const char* title) {
    const std::string wanted{title};
    const std::vector<unsigned long> clients = property_values(connection, connection.root, "_NET_CLIENT_LIST");
    for (const unsigned long client : clients) {
        if (window_name(connection, static_cast<::Window>(client)) == wanted) return static_cast<::Window>(client);
    }
    const ::Window found = find_in_tree(connection, connection.root, wanted, 0);
    return found;
}

bool has_atom(const Connection& connection, ::Window window, const char* property, const char* value) {
    const Atom wanted = atom(connection, value);
    const std::vector<unsigned long> values = property_values(connection, window, property);
    for (const unsigned long item : values) {
        if (static_cast<Atom>(item) == wanted) return true;
    }
    return false;
}

bool viewable(const Connection& connection, ::Window window) {
    if (window == 0) return false;
    XWindowAttributes attributes{};
    if (XGetWindowAttributes(connection.display, window, &attributes) == 0) return false;
    const bool result = attributes.map_state == IsViewable;
    return result;
}

std::string manager_name(const Connection& connection) {
    const std::vector<unsigned long> check = property_values(connection, connection.root, "_NET_SUPPORTING_WM_CHECK");
    if (check.empty()) return std::string{};
    const std::string name = window_name(connection, static_cast<::Window>(check[0]));
    return name;
}

// Asks the window manager to activate a window (EWMH _NET_ACTIVE_WINDOW,
// source 2: a pager, so it is not refused as focus stealing).
void activate(const Connection& connection, ::Window window) {
    XEvent event{};
    event.xclient.type = ClientMessage;
    event.xclient.window = window;
    event.xclient.message_type = atom(connection, "_NET_ACTIVE_WINDOW");
    event.xclient.format = 32;
    event.xclient.data.l[0] = 2;
    event.xclient.data.l[1] = CurrentTime;
    XSendEvent(connection.display, connection.root, False, SubstructureRedirectMask | SubstructureNotifyMask, &event);
    XFlush(connection.display);
}

int stacking_index(const std::vector<unsigned long>& stacking, ::Window window) {
    for (std::size_t index = 0U; index < stacking.size(); ++index) {
        if (static_cast<::Window>(stacking[index]) == window) return static_cast<int>(index);
    }
    return -1;
}

struct Probe final {
    Check check{Check::unknown};
    std::string check_name{};
    std::string out_path{};
    Connection connection{};
    gui_forms::ApplicationWindowHandle main{};
    gui_forms::ApplicationWindowHandle tool{};
    gui_forms::ApplicationWindowHandle document{};
    std::unique_ptr<gui_forms::Timer> timer{};
    gui_forms::SubscriptionToken tick{};
    int step{};
    std::uint64_t wait_until_ms{};
    std::uint64_t step_deadline_ms{};
    std::uint64_t started_ms{};
    bool finished{};
    std::string result{"inconclusive"};
    std::string reason{};
    std::vector<Observation> observations{};
    ::Window main_window{};
    ::Window tool_window{};
    ::Window document_window{};
};

void observe(Probe& probe, const std::string& name, const std::string& json) {
    probe.observations.push_back({name, json});
}

std::string atom_names(const Connection& connection, ::Window window, const char* property) {
    std::string list{"["};
    const std::vector<unsigned long> values = property_values(connection, window, property);
    for (std::size_t index = 0U; index < values.size(); ++index) {
        char* const name = XGetAtomName(connection.display, static_cast<Atom>(values[index]));
        if (index != 0U) list.push_back(',');
        list.append(json_string(name != nullptr ? std::string(name) : std::string{}));
        if (name != nullptr) XFree(name);
    }
    list.push_back(']');
    return list;
}

void observe_window(Probe& probe, const std::string& prefix, ::Window window) {
    const Connection& connection = probe.connection;
    observe(probe, prefix + ".found", json_bool(window != 0));
    if (window == 0) return;
    observe(probe, prefix + ".viewable", json_bool(viewable(connection, window)));
    observe(probe, prefix + ".type", atom_names(connection, window, "_NET_WM_WINDOW_TYPE"));
    observe(probe, prefix + ".state", atom_names(connection, window, "_NET_WM_STATE"));
    const std::vector<unsigned long> wm_state = property_values(connection, window, "WM_STATE");
    observe(probe, prefix + ".wm_state", std::to_string(wm_state.empty() ? -1L : static_cast<long>(wm_state[0])));
}

bool write_result(const Probe& probe) {
    std::FILE* const file = std::fopen(probe.out_path.c_str(), "wb");
    if (file == nullptr) return false;
    const char* const session = std::getenv("XDG_SESSION_TYPE");
    const char* const wayland = std::getenv("WAYLAND_DISPLAY");
    const std::string manager = probe.connection.display != nullptr ? manager_name(probe.connection) : std::string{};
    std::fprintf(file, "{\"probe\":\"x11_behaviour\",\"schema\":1,\"check\":%s,\"result\":%s,\"reason\":%s,"
                       "\"window_manager\":%s,\"session_type\":%s,\"wayland_display\":%s,\"observations\":{",
                 json_string(probe.check_name).c_str(), json_string(probe.result).c_str(),
                 json_string(probe.reason).c_str(), json_string(manager).c_str(),
                 json_string(session != nullptr ? session : "").c_str(),
                 json_string(wayland != nullptr ? wayland : "").c_str());
    for (std::size_t index = 0U; index < probe.observations.size(); ++index) {
        const Observation& observation = probe.observations[index];
        std::fprintf(file, "%s%s:%s", index == 0U ? "" : ",", json_string(observation.name).c_str(),
                     observation.json.c_str());
    }
    std::fputs("}}\n", file);
    const bool written = std::ferror(file) == 0;
    const bool closed = std::fclose(file) == 0;
    return written && closed;
}

void finish(Probe& probe, const char* result, const std::string& reason) {
    if (probe.finished) return;
    probe.finished = true;
    probe.result = result;
    probe.reason = reason;
    if (probe.timer) (*probe.timer).stop();
    if (!write_result(probe)) probe.result = "error";
    std::printf("%s: %s%s%s\n", probe.check_name.c_str(), probe.result.c_str(), probe.reason.empty() ? "" : ": ",
                probe.reason.c_str());
    std::fflush(stdout);
    static_cast<void>(probe.main.request_close());
}

void next(Probe& probe, std::uint64_t wait_ms, std::uint64_t deadline_ms) {
    ++probe.step;
    probe.wait_until_ms = now_milliseconds() + wait_ms;
    probe.step_deadline_ms = probe.wait_until_ms + deadline_ms;
}

bool past_deadline(const Probe& probe) {
    const bool past = now_milliseconds() > probe.step_deadline_ms;
    return past;
}

// Looks the three windows up once they are managed. False until all are.
bool locate(Probe& probe) {
    probe.main_window = find_window(probe.connection, main_title);
    probe.tool_window = find_window(probe.connection, tool_title);
    probe.document_window = find_window(probe.connection, document_title);
    const bool found = probe.main_window != 0 && probe.tool_window != 0 && probe.document_window != 0;
    return found;
}

void make_foreign_window(Probe& probe) {
    Connection& connection = probe.connection;
    const int screen = DefaultScreen(connection.display);
    connection.foreign = XCreateSimpleWindow(connection.display, connection.root, 300, 200, 360U, 240U, 0U,
                                             BlackPixel(connection.display, screen),
                                             WhitePixel(connection.display, screen));
    XStoreName(connection.display, connection.foreign, foreign_title);
    const std::string title{foreign_title};
    XChangeProperty(connection.display, connection.foreign, atom(connection, "_NET_WM_NAME"),
                    atom(connection, "UTF8_STRING"), 8, PropModeReplace,
                    reinterpret_cast<const unsigned char*>(title.c_str()), static_cast<int>(title.size()));
    XMapRaised(connection.display, connection.foreign);
    XFlush(connection.display);
}

void run_utility_type(Probe& probe) {
    const Connection& connection = probe.connection;
    observe_window(probe, "main", probe.main_window);
    observe_window(probe, "tool", probe.tool_window);
    observe_window(probe, "document", probe.document_window);
    const bool tool = has_atom(connection, probe.tool_window, "_NET_WM_WINDOW_TYPE", "_NET_WM_WINDOW_TYPE_UTILITY");
    const bool document =
        has_atom(connection, probe.document_window, "_NET_WM_WINDOW_TYPE", "_NET_WM_WINDOW_TYPE_UTILITY");
    const bool main_utility =
        has_atom(connection, probe.main_window, "_NET_WM_WINDOW_TYPE", "_NET_WM_WINDOW_TYPE_UTILITY");
    const bool passed = tool && document && !main_utility;
    finish(probe, passed ? "pass" : "fail",
           passed ? std::string{} : "floating windows must be utility windows and the main window must not");
}

void run_skip_taskbar(Probe& probe) {
    const Connection& connection = probe.connection;
    switch (probe.step) {
    case 0: {
        observe_window(probe, "mapped.tool", probe.tool_window);
        observe_window(probe, "mapped.document", probe.document_window);
        const bool skip = has_atom(connection, probe.tool_window, "_NET_WM_STATE", "_NET_WM_STATE_SKIP_TASKBAR") &&
            has_atom(connection, probe.document_window, "_NET_WM_STATE", "_NET_WM_STATE_SKIP_TASKBAR");
        probe.result = skip ? "pass" : "fail";
        if (!skip) probe.reason = "a floating window lacks _NET_WM_STATE_SKIP_TASKBAR once mapped";
        static_cast<void>(probe.document.hide());
        next(probe, 600U, 0U);
        return;
    }
    case 1:
        static_cast<void>(probe.document.show());
        next(probe, 800U, 0U);
        return;
    default: {
        observe_window(probe, "reshown.document", probe.document_window);
        const bool skip =
            has_atom(connection, probe.document_window, "_NET_WM_STATE", "_NET_WM_STATE_SKIP_TASKBAR");
        if (probe.result == "pass" && !skip) {
            probe.result = "fail";
            probe.reason = "a floating window lost _NET_WM_STATE_SKIP_TASKBAR after being hidden and shown";
        }
        const std::string result = probe.result;
        finish(probe, result.c_str(), probe.reason);
        return;
    }
    }
}

void run_transient_for(Probe& probe) {
    const Connection& connection = probe.connection;
    ::Window tool_owner = 0;
    ::Window document_owner = 0;
    const Status tool_found = XGetTransientForHint(connection.display, probe.tool_window, &tool_owner);
    const Status document_found = XGetTransientForHint(connection.display, probe.document_window, &document_owner);
    observe(probe, "tool.transient_for_main", json_bool(tool_found != 0 && tool_owner == probe.main_window));
    observe(probe, "document.transient_for_main",
            json_bool(document_found != 0 && document_owner == probe.main_window));
    const bool passed = tool_found != 0 && document_found != 0 && tool_owner == probe.main_window &&
        document_owner == probe.main_window;
    finish(probe, passed ? "pass" : "fail", passed ? std::string{} : "WM_TRANSIENT_FOR does not name the main window");
}

std::string stacking_json(const Probe& probe, const std::vector<unsigned long>& stacking) {
    char text[160];
    std::snprintf(text, sizeof(text), "{\"main\":%d,\"tool\":%d,\"document\":%d,\"foreign\":%d,\"count\":%zu}",
                  stacking_index(stacking, probe.main_window), stacking_index(stacking, probe.tool_window),
                  stacking_index(stacking, probe.document_window), stacking_index(stacking, probe.connection.foreign),
                  stacking.size());
    const std::string result{text};
    return result;
}

void run_restack_above_owner_only(Probe& probe) {
    const Connection& connection = probe.connection;
    const std::vector<unsigned long> stacking =
        property_values(connection, connection.root, "_NET_CLIENT_LIST_STACKING");
    switch (probe.step) {
    case 0:
        if (stacking.empty()) {
            finish(probe, "inconclusive", "the window manager publishes no _NET_CLIENT_LIST_STACKING");
            return;
        }
        make_foreign_window(probe);
        next(probe, 800U, 0U);
        return;
    case 1:
        activate(connection, probe.main_window);
        next(probe, 800U, 0U);
        return;
    case 2: {
        observe(probe, "main_active", stacking_json(probe, stacking));
        const int owner = stacking_index(stacking, probe.main_window);
        const bool above = stacking_index(stacking, probe.tool_window) > owner &&
            stacking_index(stacking, probe.document_window) > owner && owner >= 0;
        probe.result = above ? "pass" : "fail";
        if (!above) probe.reason = "with the main window active, a floating window is not above it";
        activate(connection, connection.foreign);
        next(probe, 800U, 0U);
        return;
    }
    default: {
        observe(probe, "foreign_active", stacking_json(probe, stacking));
        const int foreign = stacking_index(stacking, connection.foreign);
        const int owner = stacking_index(stacking, probe.main_window);
        const int tool = stacking_index(stacking, probe.tool_window);
        const int document = stacking_index(stacking, probe.document_window);
        if (foreign < 0) {
            finish(probe, "inconclusive", "the window manager did not manage the foreign window");
            return;
        }
        const bool owner_only = tool < foreign && document < foreign && tool > owner && document > owner;
        if (probe.result == "pass" && !owner_only) {
            probe.result = "fail";
            probe.reason = "with another application's window active, a floating window is above it or below its owner";
        }
        const std::string result = probe.result;
        finish(probe, result.c_str(), probe.reason);
        return;
    }
    }
}

void run_minimized_state(Probe& probe) {
    const Connection& connection = probe.connection;
    switch (probe.step) {
    case 0:
        XIconifyWindow(connection.display, probe.main_window, DefaultScreen(connection.display));
        XFlush(connection.display);
        next(probe, 0U, 4000U);
        return;
    case 1: {
        const std::vector<unsigned long> state = property_values(connection, probe.main_window, "WM_STATE");
        const bool iconic = (!state.empty() && state[0] == IconicState) ||
            has_atom(connection, probe.main_window, "_NET_WM_STATE", "_NET_WM_STATE_HIDDEN");
        if (!iconic) {
            if (past_deadline(probe)) finish(probe, "inconclusive", "the window manager did not minimize the main window");
            return;
        }
        next(probe, 800U, 0U);
        return;
    }
    case 2: {
        observe_window(probe, "minimized.main", probe.main_window);
        observe_window(probe, "minimized.tool", probe.tool_window);
        observe_window(probe, "minimized.document", probe.document_window);
        const bool gone = !viewable(connection, probe.tool_window) && !viewable(connection, probe.document_window);
        probe.result = gone ? "pass" : "fail";
        if (!gone) probe.reason = "a floating window stayed viewable with its main window minimized";
        activate(connection, probe.main_window);
        next(probe, 1000U, 0U);
        return;
    }
    default: {
        observe_window(probe, "restored.main", probe.main_window);
        observe_window(probe, "restored.tool", probe.tool_window);
        observe_window(probe, "restored.document", probe.document_window);
        const bool back = viewable(connection, probe.tool_window) && viewable(connection, probe.document_window);
        if (probe.result == "pass" && !back) {
            probe.result = "fail";
            probe.reason = "a floating window did not return with its restored main window";
        }
        const std::string result = probe.result;
        finish(probe, result.c_str(), probe.reason);
        return;
    }
    }
}

void run_net_wm_state(Probe& probe) {
    const Connection& connection = probe.connection;
    const bool full = has_atom(connection, probe.main_window, "_NET_WM_STATE", "_NET_WM_STATE_FULLSCREEN");
    switch (probe.step) {
    case 0:
        static_cast<void>(probe.main.toggle_full_screen());
        next(probe, 0U, 4000U);
        return;
    case 1:
        if (!full) {
            if (past_deadline(probe)) {
                finish(probe, "fail", "_NET_WM_STATE_FULLSCREEN never appeared on the main window");
            }
            return;
        }
        observe_window(probe, "full_screen.main", probe.main_window);
        observe_window(probe, "full_screen.tool", probe.tool_window);
        observe_window(probe, "full_screen.document", probe.document_window);
        static_cast<void>(probe.main.toggle_full_screen());
        next(probe, 0U, 4000U);
        return;
    default: {
        if (full) {
            if (past_deadline(probe)) {
                finish(probe, "fail", "_NET_WM_STATE_FULLSCREEN stayed on the main window after leaving full screen");
            }
            return;
        }
        observe_window(probe, "windowed.tool", probe.tool_window);
        const bool skip = has_atom(connection, probe.tool_window, "_NET_WM_STATE", "_NET_WM_STATE_SKIP_TASKBAR");
        finish(probe, skip ? "pass" : "fail",
               skip ? std::string{} : "the floating tool lost _NET_WM_STATE_SKIP_TASKBAR across full screen");
        return;
    }
    }
}

bool position_honoured(Probe& probe, const char* label, gui_forms::Point requested) {
    const Connection& connection = probe.connection;
    int x = 0;
    int y = 0;
    ::Window child = 0;
    XTranslateCoordinates(connection.display, probe.document_window, connection.root, 0, 0, &x, &y, &child);
    const gui_forms::HostWindowFrameResult frame = probe.document.frame();
    char text[200];
    std::snprintf(text, sizeof(text), "{\"requested\":[%.1f,%.1f],\"x11\":[%d,%d],\"reported\":[%.1f,%.1f]}",
                  requested.x, requested.y, x, y, frame.frame.content.x, frame.frame.content.y);
    observe(probe, label, text);
    const bool honoured = std::fabs(frame.frame.content.x - requested.x) <= 2.0 &&
        std::fabs(frame.frame.content.y - requested.y) <= 2.0;
    return honoured;
}

void run_xwayland_position(Probe& probe) {
    const char* const wayland = std::getenv("WAYLAND_DISPLAY");
    switch (probe.step) {
    case 0: {
        if (wayland == nullptr || wayland[0] == '\0') {
            finish(probe, "skipped", "not an XWayland session (WAYLAND_DISPLAY is not set)");
            return;
        }
        const gui_forms::HostWindowFrameResult frame = probe.document.frame();
        const gui_forms::Rect target{200.0, 200.0, frame.frame.content.width, frame.frame.content.height};
        static_cast<void>(probe.document.request_frame(target));
        next(probe, 1000U, 0U);
        return;
    }
    case 1: {
        const bool first = position_honoured(probe, "first", {200.0, 200.0});
        probe.result = first ? "pass" : "fail";
        const gui_forms::HostWindowFrameResult frame = probe.document.frame();
        const gui_forms::Rect target{520.0, 320.0, frame.frame.content.width, frame.frame.content.height};
        static_cast<void>(probe.document.request_frame(target));
        next(probe, 1000U, 0U);
        return;
    }
    default: {
        const bool second = position_honoured(probe, "second", {520.0, 320.0});
        if (!second) probe.result = "fail";
        if (probe.result == "fail") probe.reason = "the compositor did not place the window where the client asked";
        const std::string result = probe.result;
        finish(probe, result.c_str(), probe.reason);
        return;
    }
    }
}

struct Tick final {
    Probe* state{};
    void operator()() const {
        Probe& probe = *state;
        if (probe.finished) return;
        if (now_milliseconds() - probe.started_ms > probe_deadline_ms) {
            finish(probe, "inconclusive", "the check did not finish within 60 seconds");
            return;
        }
        if (now_milliseconds() < probe.wait_until_ms) return;
        if (probe.main_window == 0 || probe.tool_window == 0 || probe.document_window == 0) {
            if (!locate(probe)) {
                if (now_milliseconds() - probe.started_ms > 10000U) {
                    finish(probe, "inconclusive", "the probe's windows were not found by title");
                }
                return;
            }
        }
        switch (probe.check) {
        case Check::utility_type: run_utility_type(probe); return;
        case Check::skip_taskbar: run_skip_taskbar(probe); return;
        case Check::transient_for: run_transient_for(probe); return;
        case Check::restack_above_owner_only: run_restack_above_owner_only(probe); return;
        case Check::minimized_state: run_minimized_state(probe); return;
        case Check::net_wm_state: run_net_wm_state(probe); return;
        case Check::xwayland_position: run_xwayland_position(probe); return;
        case Check::unknown: finish(probe, "error", "unknown check"); return;
        }
    }
};

struct MainClosed final {
    Probe* state{};
    void operator()() const {
        Probe& probe = *state;
        probe.tick.disconnect();
        if (probe.timer) (*probe.timer).dispose();
        probe.timer.reset();
    }
};

gui_forms::ApplicationWindow floating_window(const char* stable_id, const char* title,
                                             gui_forms::HostWindowFloating kind, gui_forms::Window& owner) {
    gui_forms::ApplicationWindow window{};
    window.stable_id = stable_id;
    window.owner_id = "main";
    window.floating = kind;
    const std::shared_ptr<gui_forms::Panel> root =
        gui_forms::make_control<gui_forms::Panel>(gui_forms::StableId(stable_id));
    window.model = std::make_unique<gui_forms::Window>(root, gui_forms::Size{220.0, 140.0}, owner.resources());
    window.options.title = title;
    window.options.initial_size = {220.0, 140.0};
    return window;
}

struct MainReady final {
    Probe* state{};
    void operator()(gui_forms::Window& window, gui_forms::ApplicationWindowHandle handle) const {
        Probe& probe = *state;
        probe.main = handle;
        probe.started_ms = now_milliseconds();
        probe.timer = std::make_unique<gui_forms::Timer>(window, std::chrono::milliseconds(50));
        probe.tick = (*probe.timer).tick().subscribe(Tick{state});
        const gui_forms::ApplicationWindows windows = handle.windows();
        const gui_forms::ApplicationWindowOpenResult tool = windows.open_window(
            floating_window("tool", tool_title, gui_forms::HostWindowFloating::tool, window));
        const gui_forms::ApplicationWindowOpenResult document = windows.open_window(
            floating_window("document", document_title, gui_forms::HostWindowFloating::document, window));
        probe.tool = tool.window;
        probe.document = document.window;
        observe(probe, "open.tool", json_bool(tool.status.accepted()));
        observe(probe, "open.document", json_bool(document.status.accepted()));
        probe.wait_until_ms = now_milliseconds() + 1200U;
        probe.step_deadline_ms = probe.wait_until_ms;
        (*probe.timer).start();
    }
};

} // namespace

int main(int argc, char** argv) {
    Probe probe{};
    for (int index = 1; index + 1 < argc; index += 2) {
        if (std::strcmp(argv[index], "--check") == 0) probe.check_name = argv[index + 1];
        else if (std::strcmp(argv[index], "--out") == 0) probe.out_path = argv[index + 1];
    }
    probe.check = parse_check(probe.check_name.c_str());
    if (probe.check == Check::unknown || probe.out_path.empty()) {
        std::fprintf(stderr, "usage: gui_forms_x11_behaviour_probe --check NAME --out FILE.json\n"
                             "checks: utility_type skip_taskbar transient_for restack_above_owner_only "
                             "minimized_state net_wm_state xwayland_position\n");
        return 2;
    }
    probe.connection.display = XOpenDisplay(nullptr);
    if (probe.connection.display == nullptr) {
        std::fprintf(stderr, "behaviour probe: no X display\n");
        return 2;
    }
    probe.connection.root = DefaultRootWindow(probe.connection.display);
    gui_forms::ApplicationWindowOptions options{};
    options.title = main_title;
    options.initial_size = {420.0, 280.0};
    options.ready = MainReady{&probe};
    options.closed = MainClosed{&probe};
    const std::shared_ptr<gui_forms::Panel> root = gui_forms::make_control<gui_forms::Panel>(gui_forms::StableId("main"));
    const gui_forms::ApplicationResult run =
        gui_forms::Application::run(std::make_unique<gui_forms::Window>(root, gui_forms::Size{420.0, 280.0}),
                                    std::move(options));
    if (probe.connection.foreign != 0) XDestroyWindow(probe.connection.display, probe.connection.foreign);
    XCloseDisplay(probe.connection.display);
    if (!run.accepted() || !probe.finished) return 2;
    if (probe.result == "pass" || probe.result == "skipped") return 0;
    if (probe.result == "fail") return 1;
    return 2;
}
