#pragma once

#include "gui_forms/control/control/control.hpp"
#include "gui_forms/host/services/host_services.hpp"

#include <cstdint>
#include <string>
#include <variant>

namespace gui_forms {

class Window;

// Which of a message box's buttons is the default, counting from the first
// (Windows Forms' MessageBoxDefaultButton). The buttons are those of
// HostMessageButtons in their order: OK, Cancel; Yes, No, Cancel; Retry,
// Cancel. A default past the last button is the first button, as Windows'
// own message box takes it.
enum class MessageBoxDefaultButton : std::uint8_t { button1, button2, button3 };

// The arguments of Windows Forms' MessageBox.Show: the message, the caption
// in the title bar, the buttons, the icon and the default button.
struct MessageBoxRequest final {
    std::string text{};
    std::string caption{};
    HostMessageButtons buttons{HostMessageButtons::ok};
    HostMessageIcon icon{HostMessageIcon::none};
    MessageBoxDefaultButton default_button{MessageBoxDefaultButton::button1};
};

// A message box that closed: the button the user chose, as Windows Forms'
// DialogResult (none for a Yes/No box the host closed without a choice).
struct MessageBoxClosed final {
    DialogResult result{DialogResult::none};
};

// A message box that was not shown: the host's refusal, never
// HostServiceError::none.
struct MessageBoxRefused final {
    HostServiceError error{HostServiceError::unsupported};
};

// What show_message_box returns: either the box closed, with the chosen
// button, or it was not shown, with the reason. Built only through
// `closed` and `refused`.
class MessageBoxResult final {
public:
    [[nodiscard]] static MessageBoxResult closed(DialogResult result) noexcept;
    // Throws std::invalid_argument for HostServiceError::none, which is not
    // a refusal.
    [[nodiscard]] static MessageBoxResult refused(HostServiceError error);

    // Whether the box was shown and closed.
    [[nodiscard]] bool shown() const noexcept;
    // The chosen button; none when the box was not shown.
    [[nodiscard]] DialogResult result() const noexcept;
    // Why the box was not shown; none when it was shown.
    [[nodiscard]] HostServiceError error() const noexcept;

private:
    using State = std::variant<MessageBoxClosed, MessageBoxRefused>;

    explicit MessageBoxResult(State state) noexcept;

    State state_;
};

// Shows a message box over `owner`, modally, and returns when it closes
// (Windows Forms' MessageBox.Show). The host draws the box (owner decision
// W-Q3): MessageBoxW on Windows, NSAlert on macOS, and a dialog of GUI.Forms
// controls on X11, through the window's HostServices (capability `dialogs`).
// TaskDialog stays the themed, richer dialog drawn in the window.
//
// The result is the chosen button. A box the user closes without choosing
// (Escape or the title bar's close button) returns cancel, or ok for a box
// whose only button is OK, as in Windows Forms; a Yes/No box the host closes
// without a choice returns none. Without a host or without `dialogs` the
// result is refused, with HostServiceError::unsupported. Throws std::invalid_argument
// for text or a caption that is not valid UTF-8, contains NUL or is longer
// than HostServices::maximum_dialog_text_bytes, and for an unknown button
// set, icon or default button; throws std::logic_error off the window's UI
// thread.
MessageBoxResult show_message_box(Window& owner, const MessageBoxRequest& request);

} // namespace gui_forms
