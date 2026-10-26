// Stage T5, phase C4's measurement (planning/stages/T5-compositor/PLAN.md
// §1.3 and §5 C4): what an X server and its compositing manager do with a
// window whose pixels carry premultiplied alpha. Nothing of GUI.Forms is
// used: the probe measures the platform, so that the host adapter is built
// on recorded behaviour rather than on assumptions.
//
// A solid green backdrop window lies behind a probe window whose content is
// three columns: clear (alpha 0), red at half alpha (premultiplied 100, 0,
// 0, 128) and opaque blue. Each case makes the probe window on the default
// visual or on a 32-bit TrueColor visual and reads:
//
// - the screen (the root window's image) at each column's centre;
// - the window's own image, with its alpha byte where it has one;
// - which top-level window the server finds under the clear column;
// - the window's depth as the server reports it.
//
// The cases run under each compositor state the probe can reach: the one
// it finds (none under a bare Xvfb, the desktop's under a session), and
// xcompmgr's when xcompmgr is installed and no compositor was found. It
// then traces the XFixes selection events for _NET_WM_CM_Sn as a window
// takes and drops the selection, as a compositor starting and stopping
// does (by xcompmgr, or by the probe itself standing in for one). A
// settings line records the server, its extensions, the 32-bit visual,
// and whether the server is XWayland, with the X errors the probe met: a
// read the server refuses (Xwayland's rootless root window has no image to
// get) is recorded as an unread pixel, not a crash.
//
// It prints one JSON line per case. It measures and never judges: it fails
// only when it cannot open the display or make its windows. Verdicts
// ("per_pixel_alpha") are its reading of the colours, within a tolerance of
// 12 per channel.
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xfixes.h>

#include <signal.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

extern char** environ;

namespace {

constexpr int backdrop_x = 100;
constexpr int backdrop_y = 100;
constexpr unsigned backdrop_width = 440U;
constexpr unsigned backdrop_height = 360U;
constexpr int probe_x = 200;
constexpr int probe_y = 200;
constexpr unsigned probe_width = 240U;
constexpr unsigned probe_height = 120U;
constexpr unsigned column_width = 80U;
constexpr int channel_tolerance = 12;
// The backdrop's colour, (red, green, blue).
constexpr std::array<int, 3> backdrop_rgb{0, 160, 0};
// The columns as premultiplied ARGB words: clear, half red, opaque blue.
constexpr std::array<std::uint32_t, 3> column_argb{0x00000000U, 0x80640000U, 0xFF0000DCU};

enum class ProbeVisual {
    default_visual,
    argb32,
};

// The X errors the probe met. Xlib's default handler ends the process on
// the first one; the probe's handler records it, and the failed request's
// result (a null image) reads as an unread pixel.
struct ErrorRecord final {
    unsigned count{};
    int last_request{};
    int last_code{};
};

// Written only by record_x_error, on the probe's one thread, while Xlib
// processes a reply; read by print_settings.
ErrorRecord probe_errors{};

int record_x_error(Display* display, XErrorEvent* error) {
    static_cast<void>(display);
    probe_errors.count = probe_errors.count + 1U;
    probe_errors.last_request = static_cast<int>((*error).request_code);
    probe_errors.last_code = static_cast<int>((*error).error_code);
    return 0;
}

const char* visual_name(ProbeVisual visual) noexcept {
    switch (visual) {
    case ProbeVisual::default_visual: return "default";
    case ProbeVisual::argb32: return "argb32";
    }
    return "unknown";
}

// One read pixel and whether it was read. Alpha is -1 where the image has
// no alpha channel.
struct Sample final {
    int red{};
    int green{};
    int blue{};
    int alpha{-1};
    bool read{};
};

bool near_rgb(const Sample& sample, int red, int green, int blue) noexcept {
    const bool within = std::abs(sample.red - red) <= channel_tolerance &&
                        std::abs(sample.green - green) <= channel_tolerance &&
                        std::abs(sample.blue - blue) <= channel_tolerance;
    return sample.read && within;
}

int channel_of(unsigned long pixel, unsigned long mask) noexcept {
    if (mask == 0UL) return 0;
    unsigned shift = 0U;
    while (((mask >> shift) & 1UL) == 0UL) shift = shift + 1U;
    const unsigned long maximum = mask >> shift;
    const unsigned long value = (pixel & mask) >> shift;
    const int scaled = static_cast<int>((value * 255UL + maximum / 2UL) / maximum);
    return scaled;
}

// The pixel at (x, y) of a drawable. alpha_mask is the bits outside the
// colour masks when the drawable is 32 deep, else 0.
Sample read_pixel(Display* display, Drawable drawable, int x, int y, unsigned long alpha_mask) {
    Sample sample{};
    XImage* const image = XGetImage(display, drawable, x, y, 1U, 1U, AllPlanes, ZPixmap);
    if (image == nullptr) return sample;
    const unsigned long pixel = XGetPixel(image, 0, 0);
    sample.red = channel_of(pixel, (*image).red_mask);
    sample.green = channel_of(pixel, (*image).green_mask);
    sample.blue = channel_of(pixel, (*image).blue_mask);
    if (alpha_mask != 0UL) sample.alpha = channel_of(pixel, alpha_mask);
    sample.read = true;
    XDestroyImage(image);
    return sample;
}

void wait_for(std::chrono::milliseconds duration) {
    std::this_thread::sleep_for(duration);
}

// Drains the queue, keeping the XFixes selection events in arrival order.
struct SelectionTrace final {
    int event_base{};
    std::vector<std::string> events;
};

void drain(Display* display, SelectionTrace& trace) {
    while (XPending(display) > 0) {
        XEvent event{};
        XNextEvent(display, &event);
        if (trace.event_base == 0 || event.type != trace.event_base + XFixesSelectionNotify) continue;
        const XFixesSelectionNotifyEvent* const selection = reinterpret_cast<const XFixesSelectionNotifyEvent*>(&event);
        std::string name = "unknown";
        if ((*selection).subtype == XFixesSetSelectionOwnerNotify) {
            name = (*selection).owner == None ? "owner_none" : "owner_set";
        }
        if ((*selection).subtype == XFixesSelectionWindowDestroyNotify) name = "owner_destroyed";
        if ((*selection).subtype == XFixesSelectionClientCloseNotify) name = "owner_client_closed";
        trace.events.push_back(name);
    }
}

// Waits up to the deadline for the selection's owner to become present
// (or absent), draining events meanwhile.
bool wait_for_owner(Display* display, Atom selection, bool present, SelectionTrace& trace) {
    for (int attempt = 0; attempt < 40; ++attempt) {
        const Window owner = XGetSelectionOwner(display, selection);
        if ((owner != None) == present) return true;
        drain(display, trace);
        wait_for(std::chrono::milliseconds(50));
    }
    return false;
}

// The 32-bit TrueColor visual, if the server offers one.
struct ArgbVisual final {
    Visual* visual{};
    unsigned long alpha_mask{};
    bool found{};
};

ArgbVisual find_argb_visual(Display* display, int screen) {
    ArgbVisual found{};
    XVisualInfo info{};
    if (XMatchVisualInfo(display, screen, 32, TrueColor, &info) == 0) return found;
    found.visual = info.visual;
    const unsigned long colour = info.red_mask | info.green_mask | info.blue_mask;
    found.alpha_mask = 0xFFFFFFFFUL & ~colour;
    found.found = true;
    return found;
}

// The probe's display state, owned for the run.
struct Probe final {
    Display* display{};
    int screen{};
    Window root{};
    Window backdrop{};
    ArgbVisual argb{};
};

Window make_backdrop(const Probe& probe) {
    const unsigned long green = (static_cast<unsigned long>(backdrop_rgb[1]) << 8U);
    XSetWindowAttributes attributes{};
    attributes.background_pixel = green;
    attributes.override_redirect = True;
    const Window window = XCreateWindow(probe.display, probe.root, backdrop_x, backdrop_y, backdrop_width,
                                        backdrop_height, 0U, DefaultDepth(probe.display, probe.screen), InputOutput,
                                        DefaultVisual(probe.display, probe.screen), CWBackPixel | CWOverrideRedirect,
                                        &attributes);
    XMapRaised(probe.display, window);
    XSync(probe.display, False);
    return window;
}

// What one case read.
struct CaseReading final {
    Sample clear{};
    Sample half{};
    Sample opaque{};
    Sample own_clear{};
    Sample own_half{};
    Sample own_opaque{};
    int depth{};
    const char* window_at_clear{"none"};
    bool made{};
};

// Writes the three columns into the window: packed through the visual's
// masks, with the alpha byte only for the 32-bit visual.
void put_columns(const Probe& probe, Window window, Visual* visual, int depth, bool with_alpha) {
    std::vector<std::uint32_t> pixels(static_cast<std::size_t>(probe_width) * probe_height, 0U);
    for (unsigned row = 0U; row < probe_height; ++row) {
        for (unsigned column = 0U; column < probe_width; ++column) {
            const std::uint32_t argb = column_argb[column / column_width];
            const std::uint32_t packed = with_alpha ? argb : (argb & 0x00FFFFFFU);
            pixels[static_cast<std::size_t>(row) * probe_width + column] = packed;
        }
    }
    XImage image{};
    image.width = static_cast<int>(probe_width);
    image.height = static_cast<int>(probe_height);
    image.format = ZPixmap;
    image.data = reinterpret_cast<char*>(pixels.data());
    image.byte_order = LSBFirst;
    image.bitmap_unit = 32;
    image.bitmap_bit_order = LSBFirst;
    image.bitmap_pad = 32;
    image.depth = depth;
    image.bytes_per_line = static_cast<int>(probe_width * 4U);
    image.bits_per_pixel = 32;
    image.red_mask = (*visual).red_mask;
    image.green_mask = (*visual).green_mask;
    image.blue_mask = (*visual).blue_mask;
    if (XInitImage(&image) == 0) return;
    const GC context = XCreateGC(probe.display, window, 0UL, nullptr);
    XPutImage(probe.display, window, context, &image, 0, 0, 0, 0, probe_width, probe_height);
    XFreeGC(probe.display, context);
    XSync(probe.display, False);
}

CaseReading run_case(const Probe& probe, ProbeVisual kind, SelectionTrace& trace) {
    CaseReading reading{};
    Visual* visual = DefaultVisual(probe.display, probe.screen);
    int depth = DefaultDepth(probe.display, probe.screen);
    Colormap colormap = None;
    XSetWindowAttributes attributes{};
    attributes.override_redirect = True;
    attributes.event_mask = ExposureMask | StructureNotifyMask;
    unsigned long mask = CWOverrideRedirect | CWEventMask;
    if (kind == ProbeVisual::argb32) {
        if (!probe.argb.found) return reading;
        visual = probe.argb.visual;
        depth = 32;
        colormap = XCreateColormap(probe.display, probe.root, visual, AllocNone);
        attributes.colormap = colormap;
        attributes.border_pixel = 0UL;
        attributes.background_pixel = 0UL;
        mask = mask | CWColormap | CWBorderPixel | CWBackPixel;
    }
    const Window window = XCreateWindow(probe.display, probe.root, probe_x, probe_y, probe_width, probe_height, 0U,
                                        depth, InputOutput, visual, mask, &attributes);
    if (window == None) return reading;
    reading.made = true;
    XMapRaised(probe.display, window);
    XSync(probe.display, False);
    drain(probe.display, trace);
    wait_for(std::chrono::milliseconds(200));
    const bool with_alpha = kind == ProbeVisual::argb32;
    put_columns(probe, window, visual, depth, with_alpha);
    // A compositor repaints after damage; give it time to.
    wait_for(std::chrono::milliseconds(400));
    drain(probe.display, trace);
    const int middle = static_cast<int>(probe_height / 2U);
    const int clear_x = static_cast<int>(column_width / 2U);
    const int half_x = static_cast<int>(column_width + column_width / 2U);
    const int opaque_x = static_cast<int>(column_width * 2U + column_width / 2U);
    reading.clear = read_pixel(probe.display, probe.root, probe_x + clear_x, probe_y + middle, 0UL);
    reading.half = read_pixel(probe.display, probe.root, probe_x + half_x, probe_y + middle, 0UL);
    reading.opaque = read_pixel(probe.display, probe.root, probe_x + opaque_x, probe_y + middle, 0UL);
    const unsigned long alpha_mask = with_alpha ? probe.argb.alpha_mask : 0UL;
    reading.own_clear = read_pixel(probe.display, window, clear_x, middle, alpha_mask);
    reading.own_half = read_pixel(probe.display, window, half_x, middle, alpha_mask);
    reading.own_opaque = read_pixel(probe.display, window, opaque_x, middle, alpha_mask);
    XWindowAttributes actual{};
    if (XGetWindowAttributes(probe.display, window, &actual) != 0) reading.depth = actual.depth;
    // The server's own choice of the top-level under the clear column.
    XWarpPointer(probe.display, None, probe.root, 0, 0, 0U, 0U, probe_x + clear_x, probe_y + middle);
    XSync(probe.display, False);
    Window root_return = None;
    Window child = None;
    int root_x = 0;
    int root_y = 0;
    int window_x = 0;
    int window_y = 0;
    unsigned buttons = 0U;
    if (XQueryPointer(probe.display, probe.root, &root_return, &child, &root_x, &root_y, &window_x, &window_y,
                      &buttons) != 0) {
        if (child == window) reading.window_at_clear = "probe";
        if (child == probe.backdrop) reading.window_at_clear = "backdrop";
    }
    XDestroyWindow(probe.display, window);
    if (colormap != None) XFreeColormap(probe.display, colormap);
    XSync(probe.display, False);
    wait_for(std::chrono::milliseconds(100));
    drain(probe.display, trace);
    return reading;
}

void print_sample(const char* name, const Sample& sample) {
    if (!sample.read) {
        std::printf(",\"%s\":null", name);
        return;
    }
    if (sample.alpha >= 0) {
        std::printf(",\"%s\":[%d,%d,%d,%d]", name, sample.red, sample.green, sample.blue, sample.alpha);
        return;
    }
    std::printf(",\"%s\":[%d,%d,%d]", name, sample.red, sample.green, sample.blue);
}

void print_case(const char* compositor, ProbeVisual kind, const CaseReading& reading) {
    std::printf("{\"probe\":\"x11_compositor\",\"compositor\":\"%s\",\"visual\":\"%s\",\"made\":%s,\"depth\":%d",
                compositor, visual_name(kind), reading.made ? "true" : "false", reading.depth);
    print_sample("clear", reading.clear);
    print_sample("half", reading.half);
    print_sample("opaque", reading.opaque);
    print_sample("own_clear", reading.own_clear);
    print_sample("own_half", reading.own_half);
    print_sample("own_opaque", reading.own_opaque);
    const bool backdrop_shown = near_rgb(reading.clear, backdrop_rgb[0], backdrop_rgb[1], backdrop_rgb[2]);
    std::printf(",\"per_pixel_alpha\":%s,\"window_at_clear\":\"%s\"}\n", backdrop_shown ? "true" : "false",
                reading.window_at_clear);
    std::fflush(stdout);
}

void run_cases(const Probe& probe, const char* compositor, SelectionTrace& trace) {
    const std::array<ProbeVisual, 2> kinds{ProbeVisual::default_visual, ProbeVisual::argb32};
    for (const ProbeVisual kind : kinds) {
        const CaseReading reading = run_case(probe, kind, trace);
        print_case(compositor, kind, reading);
    }
}

void print_trace(const char* source, const SelectionTrace& trace, bool owner_seen, bool owner_gone) {
    std::printf("{\"probe\":\"x11_compositor\",\"selection_trace\":\"%s\",\"owner_seen\":%s,\"owner_gone\":%s,"
                "\"events\":[",
                source, owner_seen ? "true" : "false", owner_gone ? "true" : "false");
    for (std::size_t index = 0U; index < trace.events.size(); ++index) {
        std::printf("%s\"%s\"", index == 0U ? "" : ",", trace.events[index].c_str());
    }
    std::printf("]}\n");
    std::fflush(stdout);
}

// xcompmgr, started without shadows; 0 when it cannot be started.
pid_t start_xcompmgr() {
    pid_t child = 0;
    std::array<char*, 3> arguments{const_cast<char*>("xcompmgr"), const_cast<char*>("-n"), nullptr};
    const int started = posix_spawnp(&child, "xcompmgr", nullptr, nullptr, arguments.data(), environ);
    if (started != 0) return 0;
    return child;
}

void stop_process(pid_t child) {
    if (child <= 0) return;
    kill(child, SIGTERM);
    int status = 0;
    waitpid(child, &status, 0);
}

bool has_extension(Display* display, const char* name) {
    int opcode = 0;
    int event = 0;
    int error = 0;
    const bool present = XQueryExtension(display, name, &opcode, &event, &error) != 0;
    return present;
}

void print_settings(const Probe& probe, const char* found_compositor, bool xcompmgr_available) {
    const char* const session = std::getenv("XDG_SESSION_TYPE");
    const char* const wayland = std::getenv("WAYLAND_DISPLAY");
    const char* const vendor = ServerVendor(probe.display);
    std::printf("{\"probe\":\"x11_compositor\",\"settings\":true,\"vendor\":\"%s\",\"release\":%d,"
                "\"default_depth\":%d,\"argb32_visual\":%s,\"argb32_alpha_mask\":\"0x%08lx\","
                "\"composite\":%s,\"xfixes\":%s,\"render\":%s,\"damage\":%s,\"shape\":%s,\"xwayland\":%s,"
                "\"found_compositor\":\"%s\",\"xcompmgr_available\":%s,\"xdg_session_type\":\"%s\","
                "\"wayland_display\":%s,\"x_errors\":%u,\"last_error_request\":%d,\"last_error_code\":%d}\n",
                vendor != nullptr ? vendor : "", VendorRelease(probe.display),
                DefaultDepth(probe.display, probe.screen), probe.argb.found ? "true" : "false",
                probe.argb.alpha_mask, has_extension(probe.display, "Composite") ? "true" : "false",
                has_extension(probe.display, "XFIXES") ? "true" : "false",
                has_extension(probe.display, "RENDER") ? "true" : "false",
                has_extension(probe.display, "DAMAGE") ? "true" : "false",
                has_extension(probe.display, "SHAPE") ? "true" : "false",
                has_extension(probe.display, "XWAYLAND") ? "true" : "false", found_compositor,
                xcompmgr_available ? "true" : "false", session != nullptr ? session : "",
                wayland != nullptr ? "true" : "false", probe_errors.count, probe_errors.last_request,
                probe_errors.last_code);
    std::fflush(stdout);
}

} // namespace

int main() {
    Probe probe{};
    probe.display = XOpenDisplay(nullptr);
    if (probe.display == nullptr) {
        std::fprintf(stderr, "the probe opens the display\n");
        return 1;
    }
    static_cast<void>(XSetErrorHandler(&record_x_error));
    probe.screen = DefaultScreen(probe.display);
    probe.root = RootWindow(probe.display, probe.screen);
    probe.argb = find_argb_visual(probe.display, probe.screen);
    SelectionTrace trace{};
    int error_base = 0;
    if (XFixesQueryExtension(probe.display, &trace.event_base, &error_base) == 0) trace.event_base = 0;
    const std::string selection_name = "_NET_WM_CM_S" + std::to_string(probe.screen);
    const Atom selection = XInternAtom(probe.display, selection_name.c_str(), False);
    if (trace.event_base != 0) {
        XFixesSelectSelectionInput(probe.display, probe.root, selection,
                                   XFixesSetSelectionOwnerNotifyMask | XFixesSelectionWindowDestroyNotifyMask |
                                       XFixesSelectionClientCloseNotifyMask);
    }
    probe.backdrop = make_backdrop(probe);
    if (probe.backdrop == None) {
        std::fprintf(stderr, "the probe's backdrop window is made\n");
        XCloseDisplay(probe.display);
        return 1;
    }
    wait_for(std::chrono::milliseconds(200));
    const bool compositor_found = XGetSelectionOwner(probe.display, selection) != None;
    const char* const found = compositor_found ? "existing" : "none";
    run_cases(probe, found, trace);
    trace.events.clear();
    bool xcompmgr_available = false;
    if (!compositor_found) {
        // A compositor starting and stopping, through XFixes.
        const pid_t xcompmgr = start_xcompmgr();
        bool owner_seen = false;
        if (xcompmgr > 0) owner_seen = wait_for_owner(probe.display, selection, true, trace);
        xcompmgr_available = owner_seen;
        if (owner_seen) {
            wait_for(std::chrono::milliseconds(300));
            run_cases(probe, "xcompmgr", trace);
        }
        stop_process(xcompmgr);
        bool owner_gone = false;
        if (xcompmgr > 0) owner_gone = wait_for_owner(probe.display, selection, false, trace);
        drain(probe.display, trace);
        if (xcompmgr > 0) print_trace("xcompmgr", trace, owner_seen, owner_gone);
        trace.events.clear();
        // The probe standing in for a compositor: a window takes the
        // selection and is destroyed.
        const Window stand_in = XCreateSimpleWindow(probe.display, probe.root, 0, 0, 1U, 1U, 0U, 0UL, 0UL);
        XSetSelectionOwner(probe.display, selection, stand_in, CurrentTime);
        XSync(probe.display, False);
        const bool stand_in_seen = wait_for_owner(probe.display, selection, true, trace);
        XDestroyWindow(probe.display, stand_in);
        XSync(probe.display, False);
        const bool stand_in_gone = wait_for_owner(probe.display, selection, false, trace);
        wait_for(std::chrono::milliseconds(100));
        drain(probe.display, trace);
        print_trace("stand_in", trace, stand_in_seen, stand_in_gone);
    }
    print_settings(probe, found, xcompmgr_available);
    XDestroyWindow(probe.display, probe.backdrop);
    XCloseDisplay(probe.display);
    return 0;
}
