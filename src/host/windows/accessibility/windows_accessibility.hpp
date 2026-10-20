#pragma once

// The Win32 host's accessibility publisher: MSAA (IAccessible) objects for
// one window's semantic tree, served through WM_GETOBJECT. Every member is
// called on the window's UI thread.

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
    // objects and report CO_E_OBJNOTCONNECTED from then on.
    void detach() noexcept;
    // Answers WM_GETOBJECT for OBJID_CLIENT with the client object (an
    // LRESULT from LresultFromObject), or 0 when nothing is attached.
    LRESULT object(WPARAM parameter) noexcept;
    // Raises the MSAA events for a tree that changed since the last call.
    void notify(double scale) noexcept;

private:
    std::shared_ptr<SemanticSource> source_;
};

} // namespace gui_forms::host
