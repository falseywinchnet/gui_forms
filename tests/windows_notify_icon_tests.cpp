// Win32 notification-area icons (stage W, NotifyIcon): icon handles from the
// application's pixels, Shell_NotifyIconW add, update, notification and
// removal, the callback message routed to portable events, and the
// TaskbarCreated broadcast. Shell calls need an interactive desktop with a
// running shell; without one the add is refused as backend_failure and the
// routing checks are skipped, which the output says.

#include "services/windows_notify_icons.hpp"

#include <shellapi.h>

#include <cstdlib>
#include <iostream>
#include <vector>

namespace {
using namespace gui_forms;
using gui_forms::host::detail::WindowsNotifyIcons;

void require(bool condition, const char* message) {
    if (condition) return;
    std::cerr << message << '\n';
    std::exit(1);
}

HostImage solid_image(std::uint32_t side) {
    HostImage image;
    image.width = side;
    image.height = side;
    image.row_bytes = static_cast<std::uint64_t>(side) * 4U;
    image.pixels.assign(static_cast<std::size_t>(image.row_bytes) * side, std::byte{0xC0});
    return image;
}

struct Recorder final {
    std::vector<HostTrayIconEvent> events;
    void on_event(const HostTrayIconEvent& event) { events.push_back(event); }
};

void icon_handles() {
    std::vector<HostImage> images;
    images.push_back(solid_image(16U));
    images.push_back(solid_image(48U));
    HICON icon = gui_forms::host::detail::create_notify_icon_handle(images, 32);
    require(icon != nullptr, "an icon handle is made from pixels");
    ICONINFO info{};
    require(GetIconInfo(icon, &info) != FALSE, "the icon has bitmaps");
    BITMAP colour{};
    GetObjectW(info.hbmColor, sizeof(colour), &colour);
    require(colour.bmWidth == 48 && colour.bmHeight == 48, "the image nearest the wanted size is used");
    DeleteObject(info.hbmColor);
    DeleteObject(info.hbmMask);
    DestroyIcon(icon);
    require(gui_forms::host::detail::create_notify_icon_handle({}, 16) == nullptr, "no images make no icon");
}

LPARAM callback(UINT event, UINT shell_id) {
    const LPARAM packed = static_cast<LPARAM>(MAKELONG(event, shell_id));
    return packed;
}

void shell_icons() {
    Recorder recorder;
    WindowsNotifyIcons icons(
        Delegate<const HostTrayIconEvent&>::bind<Recorder, &Recorder::on_event>(recorder));
    HostTrayIconRequest request;
    request.images.push_back(solid_image(16U));
    request.images.push_back(solid_image(32U));
    request.tooltip = "GUI.Forms notify icon test";
    const HostServiceStatus added = icons.add(7U, request);
    if (!added.accepted()) {
        require(added.error == HostServiceError::backend_failure, "a shell refusal is a backend failure");
        std::cout << "Shell_NotifyIconW refused the icon (no interactive shell): routing checks skipped\n";
        return;
    }
    require(icons.message_window() != nullptr && icons.shell_id(7U).has_value(),
            "a shown icon has a window and a shell identity");
    const UINT shell_id = *icons.shell_id(7U);
    HWND window = icons.message_window();
    SendMessageW(window, WindowsNotifyIcons::callback_message, 0, callback(NIN_SELECT, shell_id));
    SendMessageW(window, WindowsNotifyIcons::callback_message, 0, callback(WM_LBUTTONDBLCLK, shell_id));
    SendMessageW(window, WindowsNotifyIcons::callback_message, 0, callback(WM_CONTEXTMENU, shell_id));
    SendMessageW(window, WindowsNotifyIcons::callback_message, 0, callback(NIN_BALLOONUSERCLICK, shell_id));
    SendMessageW(window, WindowsNotifyIcons::callback_message, 0, callback(NIN_BALLOONTIMEOUT, shell_id));
    SendMessageW(window, WindowsNotifyIcons::callback_message, 0, callback(WM_MOUSEMOVE, shell_id));
    SendMessageW(window, WindowsNotifyIcons::callback_message, 0, callback(NIN_SELECT, shell_id + 1U));
    require(recorder.events.size() == 5U && recorder.events[0].icon_id == 7U &&
                recorder.events[0].action == HostTrayIconAction::click &&
                recorder.events[1].action == HostTrayIconAction::double_click &&
                recorder.events[2].action == HostTrayIconAction::context &&
                recorder.events[3].action == HostTrayIconAction::notification_clicked &&
                recorder.events[4].action == HostTrayIconAction::notification_closed,
            "shell callbacks become portable events; others and unknown icons are ignored");

    request.tooltip = "GUI.Forms notify icon test, updated";
    require(icons.update(7U, request).accepted(), "the icon updates");
    HostTrayNotificationRequest notification;
    notification.title = "GUI.Forms";
    notification.text = "Notification text from a test";
    notification.icon = HostTrayNotificationIcon::information;
    require(icons.notify(7U, notification).accepted(), "notification text is shown");
    // A restarted shell broadcasts TaskbarCreated; the icon is shown again.
    SendMessageW(window, icons.taskbar_created_message(), 0, 0);
    require(icons.shell_id(7U).has_value(), "the icon survives a shell restart");
    icons.remove(7U);
    require(!icons.shell_id(7U).has_value(), "a removed icon is gone");
    std::cout << "Shell_NotifyIconW accepted the icon: routing, update, notification and restart checked\n";
}

} // namespace

int main() {
    icon_handles();
    shell_icons();
    std::cout << "Windows notify icons passed\n";
    return 0;
}
