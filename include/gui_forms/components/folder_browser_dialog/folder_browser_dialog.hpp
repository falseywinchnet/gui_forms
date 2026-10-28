#pragma once

#include "gui_forms/component.hpp"
#include "gui_forms/control/control/control.hpp"
#include "gui_forms/host/services/host_services.hpp"

#include <memory>
#include <string>

namespace gui_forms {

class Window;

// The host's dialog for choosing a folder (Windows Forms'
// FolderBrowserDialog). The dialog holds its settings and, after the user
// accepts, the chosen folder; show_dialog shows it modally over a window
// through that window's HostServices (capability `dialogs`) and returns when
// it closes. The host draws the dialog: SHBrowseForFolderW on Windows (the
// description above its tree), NSOpenPanel choosing directories on macOS,
// and a dialog of GUI.Forms controls on X11.
//
// Text settings must be valid UTF-8 without NUL of at most
// HostServices::maximum_dialog_text_bytes; anything else throws
// std::invalid_argument and changes nothing. The dialog is used on the UI
// thread of the window it is shown over.
class FolderBrowserDialog final : public Component {
public:
    FolderBrowserDialog();
    ~FolderBrowserDialog() override;

    // The text the dialog shows the user: above the tree on Windows, as the
    // title elsewhere. Empty gives the host's own.
    [[nodiscard]] const std::string& description() const noexcept { return description_; }
    void set_description(std::string description);
    // The folder the dialog starts in when no folder is selected; empty lets
    // the host choose.
    [[nodiscard]] const std::string& initial_directory() const noexcept { return initial_directory_; }
    void set_initial_directory(std::string directory);
    // The chosen folder, or empty. A folder set here before the dialog is
    // shown is the one it starts in.
    [[nodiscard]] const std::string& selected_path() const noexcept { return selected_path_; }
    void set_selected_path(std::string path);

    // The host's answer to the last show_dialog.
    [[nodiscard]] HostServiceStatus status() const noexcept { return status_; }

    // Shows the dialog over `owner` and returns when it closes: ok when the
    // user chose a folder, cancel when the user cancelled or the dialog was
    // disposed or destroyed while it was showing, and none when the dialog
    // could not be shown, with status() saying why. Only ok changes
    // selected_path. Throws std::logic_error for a disposed dialog, for one
    // already showing, and off the window's UI thread.
    DialogResult show_dialog(Window& owner);

    // Restores every setting to its default and forgets the chosen folder.
    void reset();

private:
    // Observed across the host's modal loop, which may run callbacks that
    // destroy the dialog: an expired observer means `this` is gone.
    struct Presence final {};

    std::string description_{};
    std::string initial_directory_{};
    std::string selected_path_{};
    HostServiceStatus status_{};
    bool showing_{};
    std::shared_ptr<Presence> presence_{std::make_shared<Presence>()};
};

} // namespace gui_forms
