// The forensic frame-drag probe's seam on Windows (src/host/probe/
// probe_input.hpp): SendInput for injection, SetCursorPos for the in-process
// warp, DwmGetCompositionTimingInfo for refresh timing. Win32 types stay in
// this file.
#include "probe/probe_input.hpp"

#include <windows.h>
#include <dwmapi.h>

#include <chrono>
#include <cmath>
#include <cstdint>

namespace gui_forms::probe {
namespace {

std::uint64_t steady_nanoseconds() {
    const std::chrono::steady_clock::duration since = std::chrono::steady_clock::now().time_since_epoch();
    const std::uint64_t value = static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(since).count());
    return value;
}

// QueryPerformanceCounter ticks as nanoseconds; frequency is positive.
double ticks_to_nanoseconds(double ticks, double frequency) {
    const double nanoseconds = ticks * 1.0e9 / frequency;
    return nanoseconds;
}

// A QueryPerformanceCounter value in the probe's steady clock: the counter
// and the steady clock are read together, and the earlier counter value is
// placed that far before the steady reading. This does not assume that
// steady_clock is built on the same counter.
std::uint64_t counter_to_steady(LONGLONG counter, double frequency) {
    LARGE_INTEGER now_counter{};
    QueryPerformanceCounter(&now_counter);
    const std::uint64_t now_steady = steady_nanoseconds();
    const double behind_ticks = static_cast<double>(now_counter.QuadPart - counter);
    const double behind = ticks_to_nanoseconds(behind_ticks, frequency);
    if (behind <= 0.0) return now_steady;
    const double behind_rounded = std::floor(behind);
    if (behind_rounded >= static_cast<double>(now_steady)) return 0U;
    const std::uint64_t value = now_steady - static_cast<std::uint64_t>(behind_rounded);
    return value;
}

} // namespace

InputSupport input_support() {
    InputSupport support{};
    support.os_injection = true;
    support.injection_api = "SendInput";
    support.warp_api = "SetCursorPos";
    return support;
}

bool native_cursor(NativePoint& position) {
    POINT point{};
    if (GetCursorPos(&point) == FALSE) return false;
    position = {static_cast<double>(point.x), static_cast<double>(point.y)};
    return true;
}

bool warp_native_cursor(NativePoint position) {
    const int x = static_cast<int>(std::lround(position.x));
    const int y = static_cast<int>(std::lround(position.y));
    const bool placed = SetCursorPos(x, y) != FALSE;
    return placed;
}

bool inject(PointerStep step, NativePoint position) {
    // Absolute coordinates are normalized to 0..65535 across the virtual
    // screen, so injection does not pass through pointer acceleration.
    const int left = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int top = GetSystemMetrics(SM_YVIRTUALSCREEN);
    const int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    if (width <= 1 || height <= 1) return false;
    const double normal_x = (position.x - static_cast<double>(left)) * 65535.0 / static_cast<double>(width - 1);
    const double normal_y = (position.y - static_cast<double>(top)) * 65535.0 / static_cast<double>(height - 1);
    INPUT input{};
    input.type = INPUT_MOUSE;
    input.mi.dx = static_cast<LONG>(std::lround(normal_x));
    input.mi.dy = static_cast<LONG>(std::lround(normal_y));
    DWORD flags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_VIRTUALDESK;
    if (step == PointerStep::press) flags |= MOUSEEVENTF_LEFTDOWN;
    if (step == PointerStep::release) flags |= MOUSEEVENTF_LEFTUP;
    input.mi.dwFlags = flags;
    const UINT sent = SendInput(1U, &input, static_cast<int>(sizeof(INPUT)));
    const bool accepted = sent == 1U;
    return accepted;
}

DisplayTiming display_timing() {
    DisplayTiming timing{};
    LARGE_INTEGER frequency{};
    if (QueryPerformanceFrequency(&frequency) == FALSE || frequency.QuadPart <= 0) {
        timing.source = "QueryPerformanceFrequency failed";
        return timing;
    }
    DWM_TIMING_INFO info{};
    info.cbSize = sizeof(info);
    // A null window asks for the compositor's timing (Windows 8.1 and later).
    const HRESULT queried = DwmGetCompositionTimingInfo(nullptr, &info);
    if (FAILED(queried)) {
        timing.source = "DwmGetCompositionTimingInfo failed";
        return timing;
    }
    timing.source = "DwmGetCompositionTimingInfo";
    if (info.qpcRefreshPeriod > 0U) {
        const double period = ticks_to_nanoseconds(static_cast<double>(info.qpcRefreshPeriod),
                                                   static_cast<double>(frequency.QuadPart));
        timing.refresh_period_nanoseconds = static_cast<std::uint64_t>(std::llround(period));
        timing.period_available = timing.refresh_period_nanoseconds > 0U;
    }
    timing.refresh_counter = info.qpcVBlank > 0U;
    return timing;
}

bool sample_refresh(RefreshSample& sample) noexcept {
    LARGE_INTEGER frequency{};
    if (QueryPerformanceFrequency(&frequency) == FALSE || frequency.QuadPart <= 0) return false;
    DWM_TIMING_INFO info{};
    info.cbSize = sizeof(info);
    if (FAILED(DwmGetCompositionTimingInfo(nullptr, &info)) || info.qpcVBlank == 0U) return false;
    sample.last_vblank_nanoseconds =
        counter_to_steady(static_cast<LONGLONG>(info.qpcVBlank), static_cast<double>(frequency.QuadPart));
    sample.refresh_count = static_cast<std::uint64_t>(info.cRefresh);
    return true;
}

void release() noexcept {}

} // namespace gui_forms::probe
