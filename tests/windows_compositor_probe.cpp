// Stage T5, phase C2's measurement (notes:planning/stages/T5-compositor/PLAN.md
// §1.1 and §5 C2): what the Desktop Window Manager composes from a window
// whose client area is a premultiplied 32-bit DIB, through each documented
// route, with the native caption kept. Nothing of GUI.Forms is used: the
// probe measures the platform, so that the host adapter is built on
// recorded behaviour rather than on assumptions.
//
// A solid green backdrop window lies behind a captioned window whose
// client area is three columns: clear (alpha 0), red at half alpha
// (premultiplied 100, 0, 0, 128) and opaque blue. For each route the probe
// shows a fresh window, waits a bounded number of compositions, reads the
// composed screen at each column's centre and in the caption, and asks
// which window a point over the clear column hits, then reads the clear
// column again while another window is active. Some cases change a shown
// window's route, as an adapter does when a grant changes. It prints one
// JSON line per case, then one with the settings the adapter will read.
//
// It measures and never judges: it fails only when it cannot make its
// windows or read the screen. The verdicts ("per_pixel_alpha",
// "material_shown") are its reading of the colours, within a tolerance of
// 12 per channel.
#include <windows.h>
#include <dwmapi.h>
#include <inspectable.h>
#include <roapi.h>
#include <winstring.h>
#include <windows.ui.viewmanagement.h>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cwchar>

namespace {

// Client size in physical pixels (the process is per-monitor DPI aware),
// three columns of column_width.
constexpr int client_width = 240;
constexpr int client_height = 120;
constexpr int column_width = 80;
// Compositions waited for after showing a window (DwmFlush), enough for
// DWM to apply a backdrop and finish its fade in.
constexpr int composition_waits = 40;
constexpr int channel_tolerance = 12;

struct ProbeColour final {
    int red{};
    int green{};
    int blue{};

    friend bool operator==(const ProbeColour& left, const ProbeColour& right) noexcept {
        return left.red == right.red && left.green == right.green && left.blue == right.blue;
    }
};

constexpr ProbeColour backdrop_colour{0, 160, 0};
// What each column composes to over the backdrop when DWM honours the
// client's premultiplied alpha: clear shows the backdrop, the half red
// column is 100 + 0 red and 160 x 127 / 255 green, opaque blue is itself.
constexpr ProbeColour clear_over_backdrop{0, 160, 0};
constexpr ProbeColour half_over_backdrop{100, 80, 0};
constexpr ProbeColour opaque_column{0, 0, 220};

bool near_colour(const ProbeColour& observed, const ProbeColour& wanted) noexcept {
    const bool red = std::abs(observed.red - wanted.red) <= channel_tolerance;
    const bool green = std::abs(observed.green - wanted.green) <= channel_tolerance;
    const bool blue = std::abs(observed.blue - wanted.blue) <= channel_tolerance;
    const bool near_enough = red && green && blue;
    return near_enough;
}

enum class ProbeRoute : std::uint8_t {
    // No DWM call: today's window.
    none,
    // DwmExtendFrameIntoClientArea with margins of -1.
    extend_frame,
    // DwmEnableBlurBehindWindow with an empty blur region.
    blur_behind,
    // The frame extended, then DWMWA_SYSTEMBACKDROP_TYPE.
    mica,
    acrylic,
    mica_alt,
    // Blur behind with an empty region, then DWMWA_SYSTEMBACKDROP_TYPE
    // for Mica, the frame not extended.
    blur_mica,
};

const char* route_name(ProbeRoute route) noexcept {
    switch (route) {
    case ProbeRoute::none: return "none";
    case ProbeRoute::extend_frame: return "extend_frame";
    case ProbeRoute::blur_behind: return "blur_behind";
    case ProbeRoute::mica: return "mica";
    case ProbeRoute::acrylic: return "acrylic";
    case ProbeRoute::mica_alt: return "mica_alt";
    case ProbeRoute::blur_mica: return "blur_mica";
    }
    return "unknown";
}

// One measured case: a window shown with `first`; when `changes`, the
// shown window's route is then withdrawn and `then` applied, as an
// adapter changes a live window's grant.
struct ProbeCase final {
    const char* name{};
    ProbeRoute first{ProbeRoute::none};
    ProbeRoute then{ProbeRoute::none};
    bool changes{};
};

constexpr std::array<ProbeCase, 10> cases{{
    {"none", ProbeRoute::none, ProbeRoute::none, false},
    {"extend_frame", ProbeRoute::extend_frame, ProbeRoute::none, false},
    {"blur_behind", ProbeRoute::blur_behind, ProbeRoute::none, false},
    {"mica", ProbeRoute::mica, ProbeRoute::none, false},
    {"acrylic", ProbeRoute::acrylic, ProbeRoute::none, false},
    {"mica_alt", ProbeRoute::mica_alt, ProbeRoute::none, false},
    {"blur_mica", ProbeRoute::blur_mica, ProbeRoute::none, false},
    {"mica_then_none", ProbeRoute::mica, ProbeRoute::none, true},
    {"blur_behind_then_mica", ProbeRoute::blur_behind, ProbeRoute::mica, true},
    {"mica_then_blur_behind", ProbeRoute::mica, ProbeRoute::blur_behind, true},
}};

// The DWMWA_SYSTEMBACKDROP_TYPE value for a material route (DWM_SYSTEMBACKDROP_TYPE,
// dwmapi.h: DWMSBT_MAINWINDOW 2, DWMSBT_TRANSIENTWINDOW 3, DWMSBT_TABBEDWINDOW 4);
// 0 for a route that sets none.
int route_backdrop_type(ProbeRoute route) noexcept {
    switch (route) {
    case ProbeRoute::mica:
    case ProbeRoute::blur_mica:
        return DWMSBT_MAINWINDOW;
    case ProbeRoute::acrylic: return DWMSBT_TRANSIENTWINDOW;
    case ProbeRoute::mica_alt: return DWMSBT_TABBEDWINDOW;
    case ProbeRoute::none:
    case ProbeRoute::extend_frame:
    case ProbeRoute::blur_behind:
        return 0;
    }
    return 0;
}

// The client's premultiplied BGRA image, selected into a memory DC; the
// probe window paints it in WM_PAINT. Owned by run_route, borrowed by the
// window through GWLP_USERDATA while the window lives.
struct ClientImage final {
    HDC memory_dc{};
    HBITMAP bitmap{};
    HGDIOBJ previous{};
    std::uint32_t* pixels{};
};

LRESULT CALLBACK backdrop_procedure(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    if (message == WM_ERASEBKGND) return 1;
    if (message == WM_PAINT) {
        PAINTSTRUCT paint{};
        const HDC dc = BeginPaint(window, &paint);
        const HBRUSH brush = CreateSolidBrush(RGB(backdrop_colour.red, backdrop_colour.green, backdrop_colour.blue));
        FillRect(dc, &paint.rcPaint, brush);
        DeleteObject(brush);
        EndPaint(window, &paint);
        return 0;
    }
    return DefWindowProcW(window, message, wparam, lparam);
}

LRESULT CALLBACK probe_procedure(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    if (message == WM_ERASEBKGND) return 1;
    if (message == WM_PAINT) {
        PAINTSTRUCT paint{};
        const HDC dc = BeginPaint(window, &paint);
        const ClientImage* const image = reinterpret_cast<const ClientImage*>(GetWindowLongPtrW(window, GWLP_USERDATA));
        if (image != nullptr) BitBlt(dc, 0, 0, client_width, client_height, (*image).memory_dc, 0, 0, SRCCOPY);
        EndPaint(window, &paint);
        return 0;
    }
    return DefWindowProcW(window, message, wparam, lparam);
}

// A top-down 32-bit DIB section of `width` x `height` selected into a new
// memory DC. False, with nothing left behind, when any step fails.
bool make_image(int width, int height, ClientImage& image) {
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    const HBITMAP bitmap = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (bitmap == nullptr || bits == nullptr) return false;
    const HDC memory_dc = CreateCompatibleDC(nullptr);
    if (memory_dc == nullptr) {
        DeleteObject(bitmap);
        return false;
    }
    image.memory_dc = memory_dc;
    image.bitmap = bitmap;
    image.previous = SelectObject(memory_dc, bitmap);
    image.pixels = static_cast<std::uint32_t*>(bits);
    return true;
}

void release_image(ClientImage& image) {
    if (image.memory_dc != nullptr) {
        SelectObject(image.memory_dc, image.previous);
        DeleteDC(image.memory_dc);
    }
    if (image.bitmap != nullptr) DeleteObject(image.bitmap);
    image = ClientImage{};
}

// The three columns, premultiplied BGRA in memory order (0xAARRGGBB).
void fill_columns(ClientImage& image) {
    constexpr std::uint32_t clear = 0x00000000U;
    constexpr std::uint32_t half_red = 0x80640000U;
    constexpr std::uint32_t opaque_blue = 0xFF0000DCU;
    for (int y = 0; y < client_height; ++y) {
        for (int x = 0; x < client_width; ++x) {
            std::uint32_t pixel = opaque_blue;
            if (x < column_width) {
                pixel = clear;
            } else if (x < 2 * column_width) {
                pixel = half_red;
            }
            image.pixels[static_cast<std::size_t>(y) * client_width + static_cast<std::size_t>(x)] = pixel;
        }
    }
}

// The composed screen pixel at a screen point, through a one-pixel
// capture (BitBlt from the screen DC, CAPTUREBLT for layered windows).
bool screen_colour(const POINT point, ProbeColour& colour) {
    const HDC screen = GetDC(nullptr);
    if (screen == nullptr) return false;
    ClientImage sample{};
    if (!make_image(1, 1, sample)) {
        ReleaseDC(nullptr, screen);
        return false;
    }
    const bool copied = BitBlt(sample.memory_dc, 0, 0, 1, 1, screen, point.x, point.y, SRCCOPY | CAPTUREBLT) != FALSE;
    GdiFlush();
    const std::uint32_t pixel = sample.pixels[0];
    release_image(sample);
    ReleaseDC(nullptr, screen);
    if (!copied) return false;
    colour.red = static_cast<int>((pixel >> 16U) & 255U);
    colour.green = static_cast<int>((pixel >> 8U) & 255U);
    colour.blue = static_cast<int>(pixel & 255U);
    return true;
}

// What one case showed. The statuses are the last route's: `then` for a
// case that changes.
struct RouteResult final {
    HRESULT route_status{S_OK};
    HRESULT backdrop_status{S_OK};
    // Withdrawing the first route (a case that changes): blur behind off,
    // the margins 0 and DWMSBT_AUTO, the first failing call's status.
    HRESULT withdraw_status{S_OK};
    bool active{};
    // The clear column while another window is active.
    ProbeColour clear_inactive{};
    ProbeColour clear{};
    ProbeColour half{};
    ProbeColour opaque{};
    ProbeColour caption{};
    bool clear_hits_window{};
    // DwmGetWindowAttribute(DWMWA_SYSTEMBACKDROP_TYPE) once the case is
    // applied: whether the attribute can be read, so that an adapter can
    // learn it exists without setting it, and the type it reads back.
    HRESULT read_status{S_OK};
    int read_type{-1};
    LRESULT hit_test{};
};

// Applies `route` to a window, shown or not.
void apply_route(HWND window, ProbeRoute route, RouteResult& result) {
    result.route_status = S_OK;
    result.backdrop_status = S_OK;
    if (route == ProbeRoute::none) return;
    if (route == ProbeRoute::blur_behind || route == ProbeRoute::blur_mica) {
        const HRGN empty = CreateRectRgn(0, 0, -1, -1);
        DWM_BLURBEHIND blur{};
        blur.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
        blur.fEnable = TRUE;
        blur.hRgnBlur = empty;
        result.route_status = DwmEnableBlurBehindWindow(window, &blur);
        if (empty != nullptr) DeleteObject(empty);
    } else {
        const MARGINS whole{-1, -1, -1, -1};
        result.route_status = DwmExtendFrameIntoClientArea(window, &whole);
    }
    const int backdrop_type = route_backdrop_type(route);
    if (backdrop_type == 0) return;
    result.backdrop_status =
        DwmSetWindowAttribute(window, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop_type, sizeof(backdrop_type));
}

// Withdraws every route from a shown window: blur behind off, the frame
// margins 0, and DWMSBT_AUTO (DWM's own choice, which a window that never
// set the attribute has) where the attribute exists. Keeps the first
// failing status; the attribute's failure below build 22621 is expected
// and not kept.
void withdraw_routes(HWND window, RouteResult& result) {
    DWM_BLURBEHIND blur{};
    blur.dwFlags = DWM_BB_ENABLE;
    blur.fEnable = FALSE;
    const HRESULT unblurred = DwmEnableBlurBehindWindow(window, &blur);
    const MARGINS none{0, 0, 0, 0};
    const HRESULT unextended = DwmExtendFrameIntoClientArea(window, &none);
    const int backdrop_type = DWMSBT_AUTO;
    static_cast<void>(
        DwmSetWindowAttribute(window, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop_type, sizeof(backdrop_type)));
    result.withdraw_status = FAILED(unblurred) ? unblurred : unextended;
}

// Pumps this thread's messages, then waits for one composition.
void settle(int compositions) {
    for (int index = 0; index < compositions; ++index) {
        MSG message{};
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE) != FALSE) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
        static_cast<void>(DwmFlush());
    }
}

// Shows a fresh probe window for `probe` over the backdrop at `origin`
// and measures it. `away` is a window that takes activation for the
// inactive reading. False when the window or the capture cannot be made.
bool run_case(HINSTANCE instance, POINT origin, const ProbeCase& probe, HWND away, RouteResult& result) {
    ClientImage image{};
    if (!make_image(client_width, client_height, image)) return false;
    fill_columns(image);
    constexpr DWORD style = WS_OVERLAPPEDWINDOW;
    constexpr DWORD extended = WS_EX_TOPMOST;
    RECT frame{0, 0, client_width, client_height};
    AdjustWindowRectEx(&frame, style, FALSE, extended);
    const HWND window = CreateWindowExW(extended, L"GuiFormsCompositorProbe", L"GUI.Forms compositor probe", style,
                                        origin.x, origin.y, frame.right - frame.left, frame.bottom - frame.top,
                                        nullptr, nullptr, instance, nullptr);
    if (window == nullptr) {
        release_image(image);
        return false;
    }
    SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&image));
    apply_route(window, probe.first, result);
    ShowWindow(window, SW_SHOWNORMAL);
    SetForegroundWindow(window);
    UpdateWindow(window);
    settle(composition_waits);
    if (probe.changes) {
        withdraw_routes(window, result);
        apply_route(window, probe.then, result);
        InvalidateRect(window, nullptr, FALSE);
        UpdateWindow(window);
        settle(composition_waits);
    }
    result.active = GetForegroundWindow() == window;
    result.read_status =
        DwmGetWindowAttribute(window, DWMWA_SYSTEMBACKDROP_TYPE, &result.read_type, sizeof(result.read_type));

    POINT client_origin{0, 0};
    ClientToScreen(window, &client_origin);
    const int middle = client_height / 2;
    const POINT clear_point{client_origin.x + column_width / 2, client_origin.y + middle};
    const POINT half_point{client_origin.x + column_width + column_width / 2, client_origin.y + middle};
    const POINT opaque_point{client_origin.x + 2 * column_width + column_width / 2, client_origin.y + middle};
    RECT bounds{};
    if (FAILED(DwmGetWindowAttribute(window, DWMWA_EXTENDED_FRAME_BOUNDS, &bounds, sizeof(bounds)))) {
        GetWindowRect(window, &bounds);
    }
    // The caption's middle row, at the client's horizontal centre: clear of
    // the title at the left and the buttons at the right.
    const POINT caption_point{client_origin.x + client_width / 2, (bounds.top + client_origin.y) / 2};
    const bool measured = screen_colour(clear_point, result.clear) && screen_colour(half_point, result.half) &&
                          screen_colour(opaque_point, result.opaque) &&
                          screen_colour(caption_point, result.caption);
    result.clear_hits_window = WindowFromPoint(clear_point) == window;
    result.hit_test = SendMessageW(window, WM_NCHITTEST, 0, MAKELPARAM(clear_point.x, clear_point.y));
    // Inactive: another window takes activation.
    SetForegroundWindow(away);
    settle(composition_waits);
    const bool inactive_measured = screen_colour(clear_point, result.clear_inactive);

    SetWindowLongPtrW(window, GWLP_USERDATA, 0);
    DestroyWindow(window);
    settle(2);
    release_image(image);
    const bool complete = measured && inactive_measured;
    return complete;
}

const char* hit_test_name(LRESULT hit) noexcept {
    if (hit == HTCLIENT) return "client";
    if (hit == HTCAPTION) return "caption";
    if (hit == HTTRANSPARENT) return "transparent";
    if (hit == HTNOWHERE) return "nowhere";
    return "other";
}

void print_colour(const char* name, const ProbeColour& colour) {
    std::printf(",\"%s\":[%d,%d,%d]", name, colour.red, colour.green, colour.blue);
}

void print_case(const ProbeCase& probe, const RouteResult& result) {
    const bool per_pixel_alpha = near_colour(result.clear, clear_over_backdrop) &&
                                 near_colour(result.half, half_over_backdrop) &&
                                 near_colour(result.opaque, opaque_column);
    // A material shows when the clear column is neither the backdrop nor
    // black (the colour DWM shows for clear pixels it does not compose).
    const ProbeColour black{0, 0, 0};
    const bool material_shown = !near_colour(result.clear, backdrop_colour) && !near_colour(result.clear, black);
    std::printf("{\"probe\":\"windows_compositor\",\"case\":\"%s\",\"first\":\"%s\",\"then\":\"%s\","
                "\"route_status\":\"0x%08lx\",\"backdrop_status\":\"0x%08lx\",\"withdraw_status\":\"0x%08lx\","
                "\"active\":%s",
                probe.name, route_name(probe.first), probe.changes ? route_name(probe.then) : "",
                static_cast<unsigned long>(result.route_status), static_cast<unsigned long>(result.backdrop_status),
                static_cast<unsigned long>(result.withdraw_status), result.active ? "true" : "false");
    print_colour("clear", result.clear);
    print_colour("clear_inactive", result.clear_inactive);
    print_colour("half", result.half);
    print_colour("opaque", result.opaque);
    print_colour("caption", result.caption);
    std::printf(",\"clear_hits_window\":%s,\"hit_test\":\"%s\",\"read_status\":\"0x%08lx\",\"read_type\":%d,"
                "\"per_pixel_alpha\":%s,\"material_shown\":%s}\n",
                result.clear_hits_window ? "true" : "false", hit_test_name(result.hit_test),
                static_cast<unsigned long>(result.read_status), result.read_type, per_pixel_alpha ? "true" : "false",
                material_shown ? "true" : "false");
}

// The build, for the record only: HKLM\...\Windows NT\CurrentVersion
// CurrentBuildNumber (GetVersionExW reports 9200 to an executable without
// a compatibility manifest). 0 when it cannot be read. The adapter will not
// read it: it learns whether the backdrop attribute exists by setting it.
unsigned long operating_system_build() {
    std::array<wchar_t, 16> text{};
    DWORD size = static_cast<DWORD>(text.size() * sizeof(wchar_t));
    const LSTATUS status = RegGetValueW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
                                        L"CurrentBuildNumber", RRF_RT_REG_SZ, nullptr, text.data(), &size);
    if (status != ERROR_SUCCESS) return 0UL;
    const unsigned long build = std::wcstoul(text.data(), nullptr, 10);
    return build;
}

// HKCU\...\Themes\Personalize EnableTransparency: 1 on, 0 off, -1 absent.
int transparency_setting() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    const LSTATUS status =
        RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"EnableTransparency", RRF_RT_REG_DWORD, nullptr, &value, &size);
    if (status != ERROR_SUCCESS) return -1;
    const int setting = value != 0U ? 1 : 0;
    return setting;
}

// WinRT's UISettings.AdvancedEffectsEnabled (IUISettings4, Windows 10
// 1803): 1 on, 0 off, -1 unavailable. The interface identifier is the one
// windows.ui.viewmanagement.h declares for IUISettings4.
constexpr GUID ui_settings4_interface{0x52bb3002, 0x919b, 0x4d6b, {0x9b, 0x78, 0x8d, 0xd6, 0x6f, 0xf4, 0xb9, 0x3b}};

int advanced_effects_setting() {
    const HRESULT initialized = RoInitialize(RO_INIT_SINGLETHREADED);
    const bool uninitialize = SUCCEEDED(initialized);
    int setting = -1;
    HSTRING_HEADER header{};
    HSTRING class_name = nullptr;
    const wchar_t* const name = RuntimeClass_Windows_UI_ViewManagement_UISettings;
    if (SUCCEEDED(WindowsCreateStringReference(name, static_cast<UINT32>(std::wcslen(name)), &header, &class_name))) {
        IInspectable* instance = nullptr;
        if (SUCCEEDED(RoActivateInstance(class_name, &instance)) && instance != nullptr) {
            void* queried = nullptr;
            if (SUCCEEDED((*instance).QueryInterface(ui_settings4_interface, &queried)) && queried != nullptr) {
                ABI::Windows::UI::ViewManagement::IUISettings4* const settings =
                    static_cast<ABI::Windows::UI::ViewManagement::IUISettings4*>(queried);
                boolean enabled = 0;
                if (SUCCEEDED((*settings).get_AdvancedEffectsEnabled(&enabled))) setting = enabled != 0 ? 1 : 0;
                (*settings).Release();
            }
            (*instance).Release();
        }
    }
    if (uninitialize) RoUninitialize();
    return setting;
}

void print_settings() {
    HIGHCONTRASTW contrast{};
    contrast.cbSize = sizeof(contrast);
    const bool contrast_read = SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0) != FALSE;
    const bool high_contrast = contrast_read && (contrast.dwFlags & HCF_HIGHCONTRASTON) != 0U;
    BOOL composition = FALSE;
    const bool composition_read = SUCCEEDED(DwmIsCompositionEnabled(&composition));
    std::printf("{\"probe\":\"windows_compositor\",\"settings\":true,\"build\":%lu,\"enable_transparency\":%d,"
                "\"advanced_effects\":%d,\"high_contrast\":%s,\"composition\":%s,\"remote_session\":%s}\n",
                operating_system_build(), transparency_setting(),
                advanced_effects_setting(), high_contrast ? "true" : "false",
                composition_read && composition != FALSE ? "true" : "false",
                GetSystemMetrics(SM_REMOTESESSION) != 0 ? "true" : "false");
}

bool register_classes(HINSTANCE instance) {
    WNDCLASSEXW backdrop{};
    backdrop.cbSize = sizeof(backdrop);
    backdrop.lpfnWndProc = &backdrop_procedure;
    backdrop.hInstance = instance;
    backdrop.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    backdrop.lpszClassName = L"GuiFormsCompositorBackdrop";
    WNDCLASSEXW probe = backdrop;
    probe.lpfnWndProc = &probe_procedure;
    probe.lpszClassName = L"GuiFormsCompositorProbe";
    const bool registered = RegisterClassExW(&backdrop) != 0 && RegisterClassExW(&probe) != 0;
    return registered;
}

} // namespace

int main() {
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    const HINSTANCE instance = GetModuleHandleW(nullptr);
    if (!register_classes(instance)) {
        std::fprintf(stderr, "the probe's window classes are registered\n");
        return 1;
    }
    // The backdrop covers the probe window with a margin on every side, on
    // the primary monitor's work area.
    RECT work{};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0);
    const POINT backdrop_origin{work.left + 80, work.top + 80};
    const HWND backdrop = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
                                          L"GuiFormsCompositorBackdrop", L"", WS_POPUP, backdrop_origin.x,
                                          backdrop_origin.y, client_width + 240, client_height + 240, nullptr,
                                          nullptr, instance, nullptr);
    if (backdrop == nullptr) {
        std::fprintf(stderr, "the backdrop window is made\n");
        return 1;
    }
    ShowWindow(backdrop, SW_SHOWNOACTIVATE);
    UpdateWindow(backdrop);
    settle(4);
    // The window that takes activation for the inactive readings, beside
    // the backdrop.
    const HWND away = CreateWindowExW(WS_EX_TOPMOST, L"GuiFormsCompositorBackdrop", L"GUI.Forms compositor probe away",
                                      WS_OVERLAPPED | WS_CAPTION, backdrop_origin.x + client_width + 260,
                                      backdrop_origin.y, 160, 120, nullptr, nullptr, instance, nullptr);
    if (away == nullptr) {
        DestroyWindow(backdrop);
        std::fprintf(stderr, "the activation window is made\n");
        return 1;
    }
    ShowWindow(away, SW_SHOWNOACTIVATE);
    const POINT probe_origin{backdrop_origin.x + 60, backdrop_origin.y + 60};
    bool measured = true;
    for (const ProbeCase& probe : cases) {
        RouteResult result{};
        if (!run_case(instance, probe_origin, probe, away, result)) {
            std::fprintf(stderr, "case %s could not be measured\n", probe.name);
            measured = false;
            continue;
        }
        print_case(probe, result);
    }
    print_settings();
    std::fflush(stdout);
    DestroyWindow(away);
    DestroyWindow(backdrop);
    if (!measured) return 1;
    return 0;
}
