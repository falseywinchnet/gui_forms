// The forensic frame-drag probe's seam on macOS (src/host/probe/
// probe_input.hpp): CGEventPost for injection (needs the Accessibility
// permission), CGWarpMouseCursorPosition for the in-process warp (needs no
// permission), and CVDisplayLink only as a source of the nominal refresh
// period. No GPU presentation timestamp is read: CAMetalLayer and drawable
// presentation are GPU paths GUI.Forms does not use. Quartz types stay in
// this file; it is plain C++ over the C APIs.
#include "probe/probe_input.hpp"

#include <ApplicationServices/ApplicationServices.h>
#include <CoreVideo/CoreVideo.h>

#include <cmath>
#include <cstdint>

namespace gui_forms::probe {
namespace {

CGEventType event_type(PointerStep step) {
    switch (step) {
    case PointerStep::move: return kCGEventMouseMoved;
    case PointerStep::press: return kCGEventLeftMouseDown;
    case PointerStep::drag: return kCGEventLeftMouseDragged;
    case PointerStep::release: return kCGEventLeftMouseUp;
    }
    return kCGEventMouseMoved;
}

CGDirectDisplayID display_under_cursor() {
    CGDirectDisplayID display = CGMainDisplayID();
    NativePoint cursor{};
    if (!native_cursor(cursor)) return display;
    CGDirectDisplayID found = 0U;
    std::uint32_t count = 0U;
    const CGError located = CGGetDisplaysWithPoint(CGPointMake(cursor.x, cursor.y), 1U, &found, &count);
    if (located == kCGErrorSuccess && count == 1U) display = found;
    return display;
}

} // namespace

InputSupport input_support() {
    InputSupport support{};
    support.injection_api = "CGEventPost";
    support.warp_api = "CGWarpMouseCursorPosition";
    // Posting events needs the Accessibility permission. The probe never
    // asks for it (no prompt): the owner grants it, or the probe runs
    // in-process.
    support.os_injection = AXIsProcessTrusted() != false;
    if (!support.os_injection) {
        support.unavailable_reason =
            "this process is not trusted for Accessibility (System Settings, Privacy & Security, Accessibility)";
    }
    return support;
}

bool native_cursor(NativePoint& position) {
    const CGEventRef event = CGEventCreate(nullptr);
    if (event == nullptr) return false;
    const CGPoint location = CGEventGetLocation(event);
    CFRelease(event);
    position = {static_cast<double>(location.x), static_cast<double>(location.y)};
    return true;
}

bool warp_native_cursor(NativePoint position) {
    const CGError warped = CGWarpMouseCursorPosition(CGPointMake(position.x, position.y));
    // A warp suspends the association of mouse and cursor briefly; restore
    // it so later hardware or posted motion moves the cursor at once.
    static_cast<void>(CGAssociateMouseAndMouseCursorPosition(true));
    const bool placed = warped == kCGErrorSuccess;
    return placed;
}

bool inject(PointerStep step, NativePoint position) {
    const CGEventRef event =
        CGEventCreateMouseEvent(nullptr, event_type(step), CGPointMake(position.x, position.y), kCGMouseButtonLeft);
    if (event == nullptr) return false;
    CGEventPost(kCGHIDEventTap, event);
    CFRelease(event);
    return true;
}

DisplayTiming display_timing() {
    DisplayTiming timing{};
    CVDisplayLinkRef link = nullptr;
    const CVReturn created = CVDisplayLinkCreateWithCGDisplay(display_under_cursor(), &link);
    if (created != kCVReturnSuccess || link == nullptr) {
        timing.source = "CVDisplayLinkCreateWithCGDisplay failed";
        return timing;
    }
    // The nominal period needs no running link and no callback thread.
    const CVTime nominal = CVDisplayLinkGetNominalOutputVideoRefreshPeriod(link);
    CVDisplayLinkRelease(link);
    timing.source = "CVDisplayLinkGetNominalOutputVideoRefreshPeriod";
    const bool indefinite = (nominal.flags & kCVTimeIsIndefinite) != 0;
    if (!indefinite && nominal.timeScale > 0 && nominal.timeValue > 0) {
        const double period = static_cast<double>(nominal.timeValue) * 1.0e9 / static_cast<double>(nominal.timeScale);
        timing.refresh_period_nanoseconds = static_cast<std::uint64_t>(std::llround(period));
        timing.period_available = timing.refresh_period_nanoseconds > 0U;
    }
    return timing;
}

bool sample_refresh(RefreshSample&) noexcept {
    return false;
}

void release() noexcept {}

} // namespace gui_forms::probe
