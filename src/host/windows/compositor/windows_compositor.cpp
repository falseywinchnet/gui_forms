#include "windows_compositor.hpp"

#include <dwmapi.h>
#include <versionhelpers.h>

#include <cstring>
#include <cwchar>

namespace gui_forms::host::detail {
namespace {

// dwmapi.dll's entry points, typed from their declarations (house style
// section 2) and loaded once. A missing library or entry point leaves its
// pointer null.
using DwmIsCompositionEnabledFunction = decltype(&::DwmIsCompositionEnabled);
using DwmEnableBlurBehindWindowFunction = decltype(&::DwmEnableBlurBehindWindow);
using DwmSetWindowAttributeFunction = decltype(&::DwmSetWindowAttribute);
using DwmGetWindowAttributeFunction = decltype(&::DwmGetWindowAttribute);

struct DwmFunctions final {
    DwmIsCompositionEnabledFunction composition_enabled{};
    DwmEnableBlurBehindWindowFunction enable_blur_behind{};
    DwmSetWindowAttributeFunction set_attribute{};
    DwmGetWindowAttributeFunction get_attribute{};
};

template <typename Function>
Function dwm_function(HMODULE library, const char* name) noexcept {
    static_assert(sizeof(Function) == sizeof(FARPROC));
    const FARPROC raw = GetProcAddress(library, name);
    Function function{};
    std::memcpy(&function, &raw, sizeof(function));
    return function;
}

DwmFunctions load_dwm_functions() noexcept {
    DwmFunctions functions{};
    const HMODULE library = LoadLibraryW(L"dwmapi.dll");
    if (library == nullptr) return functions;
    functions.composition_enabled = dwm_function<DwmIsCompositionEnabledFunction>(library, "DwmIsCompositionEnabled");
    functions.enable_blur_behind =
        dwm_function<DwmEnableBlurBehindWindowFunction>(library, "DwmEnableBlurBehindWindow");
    functions.set_attribute = dwm_function<DwmSetWindowAttributeFunction>(library, "DwmSetWindowAttribute");
    functions.get_attribute = dwm_function<DwmGetWindowAttributeFunction>(library, "DwmGetWindowAttribute");
    return functions;
}

// Loaded on first use; the library stays loaded for the process.
const DwmFunctions& dwm() noexcept {
    static const DwmFunctions functions = load_dwm_functions();
    return functions;
}

// The transparency-effects setting: HKCU\...\Themes\Personalize
// EnableTransparency, on when the value is absent (as Windows treats it).
// MEASUREMENTS.md C2 found it agreeing with UISettings.AdvancedEffectsEnabled.
bool transparency_effects_enabled() noexcept {
    DWORD value = 1U;
    DWORD size = sizeof(value);
    const LSTATUS status =
        RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"EnableTransparency", RRF_RT_REG_DWORD, nullptr, &value, &size);
    if (status != ERROR_SUCCESS) return true;
    const bool enabled = value != 0U;
    return enabled;
}

bool high_contrast_on() noexcept {
    HIGHCONTRASTW contrast{};
    contrast.cbSize = sizeof(contrast);
    if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0) == FALSE) return false;
    const bool on = (contrast.dwFlags & HCF_HIGHCONTRASTON) != 0U;
    return on;
}

// Whether DWMWA_SYSTEMBACKDROP_TYPE exists: it reads back on build 22621
// and later (MEASUREMENTS.md C2), so reading it changes nothing.
bool backdrop_attribute_readable(HWND window) noexcept {
    const DwmFunctions& functions = dwm();
    if (functions.get_attribute == nullptr || window == nullptr) return false;
    int type = 0;
    const HRESULT read = functions.get_attribute(window, DWMWA_SYSTEMBACKDROP_TYPE, &type, sizeof(type));
    const bool readable = SUCCEEDED(read);
    return readable;
}

HRESULT set_backdrop_type(HWND window, int type) noexcept {
    const HRESULT set = dwm().set_attribute(window, DWMWA_SYSTEMBACKDROP_TYPE, &type, sizeof(type));
    return set;
}

// Blur behind on with an empty region (the client's alpha shown, nothing
// blurred), or off.
HRESULT set_client_alpha(HWND window, bool enabled) noexcept {
    DWM_BLURBEHIND blur{};
    blur.dwFlags = DWM_BB_ENABLE;
    blur.fEnable = enabled ? TRUE : FALSE;
    HRGN empty = nullptr;
    if (enabled) {
        empty = CreateRectRgn(0, 0, -1, -1);
        if (empty == nullptr) return E_OUTOFMEMORY;
        blur.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
        blur.hRgnBlur = empty;
    }
    const HRESULT applied = dwm().enable_blur_behind(window, &blur);
    if (empty != nullptr) DeleteObject(empty);
    return applied;
}

} // namespace

CompositorState read_windows_compositor_state(HWND window, CompositorGeneration generation) noexcept {
    CompositorState state{};
    state.generation = generation;
    const DwmFunctions& functions = dwm();
    // DWM composes every desktop from Windows 8 on and cannot be turned
    // off; DwmIsCompositionEnabled answers FALSE under a contrast theme
    // there all the same (the owner's run, MEASUREMENTS.md C2), so it is
    // asked only on an older system.
    state.compositing = CompositorPresence::unknown;
    if (IsWindows8OrGreater()) {
        state.compositing = CompositorPresence::present;
    } else {
        BOOL composing = FALSE;
        const bool known =
            functions.composition_enabled != nullptr && SUCCEEDED(functions.composition_enabled(&composing));
        if (known) state.compositing = composing != FALSE ? CompositorPresence::present : CompositorPresence::absent;
    }
    const bool alpha_available = functions.enable_blur_behind != nullptr;
    const bool materials_available = functions.set_attribute != nullptr && backdrop_attribute_readable(window);
    const WindowBackdropSupport transparent_support =
        alpha_available ? WindowBackdropSupport::supported : WindowBackdropSupport::host_unsupported;
    WindowBackdropSupport material_support = WindowBackdropSupport::os_version;
    if (!alpha_available || functions.set_attribute == nullptr) material_support = WindowBackdropSupport::host_unsupported;
    if (materials_available) material_support = WindowBackdropSupport::supported;
    state.support[static_cast<std::size_t>(WindowBackdrop::transparent)] = transparent_support;
    state.support[static_cast<std::size_t>(WindowBackdrop::window_material)] = material_support;
    state.support[static_cast<std::size_t>(WindowBackdrop::transient_material)] = material_support;
    // Windows has one switch for both: transparency effects off is also
    // reduced transparency for what the window draws (PLAN.md §4.5).
    const bool effects = transparency_effects_enabled();
    state.transparency_enabled = effects;
    state.reduced_transparency = !effects;
    state.high_contrast = high_contrast_on();
    return state;
}

bool windows_compositor_message(UINT message, WPARAM wparam, LPARAM lparam) noexcept {
    if (message == WM_THEMECHANGED || message == WM_DWMCOMPOSITIONCHANGED) return true;
    if (message != WM_SETTINGCHANGE) return false;
    if (wparam == SPI_SETHIGHCONTRAST) return true;
    if (lparam == 0) return false;
    const wchar_t* const area = reinterpret_cast<const wchar_t*>(lparam);
    const bool colour_set = std::wcscmp(area, L"ImmersiveColorSet") == 0;
    return colour_set;
}

HostServiceStatus apply_windows_backdrop(HWND window, WindowBackdrop backdrop) noexcept {
    const DwmFunctions& functions = dwm();
    if (window == nullptr) return {HostServiceError::backend_failure};
    const bool materials = functions.set_attribute != nullptr && backdrop_attribute_readable(window);
    if (backdrop == WindowBackdrop::opaque) {
        // Nothing was applied without the library.
        if (functions.enable_blur_behind == nullptr) return {};
        const HRESULT unblurred = set_client_alpha(window, false);
        const HRESULT automatic = materials ? set_backdrop_type(window, DWMSBT_AUTO) : S_OK;
        if (FAILED(unblurred) || FAILED(automatic)) return {HostServiceError::backend_failure};
        return {};
    }
    if (functions.enable_blur_behind == nullptr) return {HostServiceError::backend_failure};
    int type = DWMSBT_AUTO;
    if (backdrop == WindowBackdrop::window_material) type = DWMSBT_MAINWINDOW;
    if (backdrop == WindowBackdrop::transient_material) type = DWMSBT_TRANSIENTWINDOW;
    if (type != DWMSBT_AUTO && !materials) return {HostServiceError::backend_failure};
    const HRESULT alpha = set_client_alpha(window, true);
    if (FAILED(alpha)) return {HostServiceError::backend_failure};
    const HRESULT material = materials ? set_backdrop_type(window, type) : S_OK;
    if (FAILED(material)) return {HostServiceError::backend_failure};
    return {};
}

} // namespace gui_forms::host::detail
