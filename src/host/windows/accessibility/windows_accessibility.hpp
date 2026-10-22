#pragma once

// The Win32 host's accessibility publisher for one window's semantic tree,
// served through WM_GETOBJECT: MSAA (IAccessible) objects for OBJID_CLIENT
// and UI Automation providers (windows_automation.hpp) for
// UiaRootObjectId. Every member is called on the window's UI thread.

#include "gui_forms/window.hpp"

#include <windows.h>

#include <memory>

namespace gui_forms::host {

struct SemanticSource;

class WindowsAccessibility final {
public:
    WindowsAccessibility();
    ~WindowsAccessibility();
    WindowsAccessibility(const WindowsAccessibility&) = delete;
    WindowsAccessibility& operator=(const WindowsAccessibility&) = delete;

    // Publishes `model`'s tree for `hwnd`. The window must outlive the
    // attachment; detach ends it.
    void attach(HWND hwnd, Window& model, double scale);
    // Ends the attachment: objects already handed out stay valid COM
    // objects and report CO_E_OBJNOTCONNECTED (MSAA) or
    // UIA_E_ELEMENTNOTAVAILABLE (UI Automation) from then on, and UI
    // Automation's references for the window are released.
    void detach() noexcept;
    // Answers WM_GETOBJECT for OBJID_CLIENT with the client object (an
    // LRESULT from LresultFromObject), or 0 when nothing is attached.
    LRESULT object(WPARAM parameter) noexcept;
    // Answers WM_GETOBJECT for UiaRootObjectId with the window's fragment
    // root (an LRESULT from UiaReturnRawElementProvider), or 0 when nothing
    // is attached.
    LRESULT automation_object(WPARAM wparam, LPARAM lparam) noexcept;
    // Whether a WM_GETOBJECT `lparam` asks for the UI Automation root.
    [[nodiscard]] static bool automation_request(LPARAM lparam) noexcept;
    // Raises the MSAA and UI Automation events for a tree that changed since
    // the last call.
    void notify(double scale) noexcept;

private:
    // Keeps the calling thread's COM apartment for the rest of the thread's
    // life (see windows_accessibility.cpp).
    static void pin_automation_apartment() noexcept;

    std::shared_ptr<SemanticSource> source_;
};

} // namespace gui_forms::host
