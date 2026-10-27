#include "gui_forms/components/folder_browser_dialog/folder_browser_dialog.hpp"

#include "../file_dialog/file_dialog_support.hpp"
#include "gui_forms/components/file_dialog_types/file_dialog_types.hpp"
#include "gui_forms/window.hpp"

#include <stdexcept>
#include <utility>
#include <variant>

namespace gui_forms {
namespace {

void require_dialog_text(std::string_view text, const char* message) {
    if (!file_dialog_text_valid(text)) throw std::invalid_argument(message);
}

} // namespace

FolderBrowserDialog::FolderBrowserDialog() = default;

FolderBrowserDialog::~FolderBrowserDialog() = default;

void FolderBrowserDialog::set_description(std::string description) {
    require_dialog_text(description, "FolderBrowserDialog description must be valid UTF-8 without NUL");
    description_ = std::move(description);
}

void FolderBrowserDialog::set_initial_directory(std::string directory) {
    require_dialog_text(directory, "FolderBrowserDialog initial directory must be valid UTF-8 without NUL");
    initial_directory_ = std::move(directory);
}

void FolderBrowserDialog::set_selected_path(std::string path) {
    require_dialog_text(path, "FolderBrowserDialog selected path must be valid UTF-8 without NUL");
    selected_path_ = std::move(path);
}

void FolderBrowserDialog::reset() {
    description_.clear();
    initial_directory_.clear();
    selected_path_.clear();
    status_ = {};
}

DialogResult FolderBrowserDialog::show_dialog(Window& owner) {
    if (!is_alive()) throw std::logic_error("GUI.Forms cannot show a disposed FolderBrowserDialog");
    owner.verify_access("FolderBrowserDialog::show_dialog");
    if (showing_) throw std::logic_error("GUI.Forms cannot show a FolderBrowserDialog that is already showing");
    const std::weak_ptr<Presence> presence = presence_;
    const detail::ShowingScope showing(showing_, presence);
    HostServices* const services = owner.host_services();
    if (services == nullptr) {
        status_ = {HostServiceError::unsupported};
        return DialogResult::none;
    }
    HostFolderDialogRequest payload;
    payload.title = description_;
    // A selected folder is where the dialog starts, as in Windows Forms.
    payload.initial_directory = selected_path_.empty() ? initial_directory_ : selected_path_;
    HostDialogRequest host_request;
    host_request.request_id = detail::take_dialog_request_id();
    const Control::Ptr root = owner.root();
    if (root) host_request.owner_id.assign((*root).stable_id().value());
    host_request.payload = std::move(payload);
    const HostDialogResult answer = (*services).show_dialog(host_request);
    if (presence.expired()) return DialogResult::cancel;
    status_ = answer.status;
    if (!answer.status.accepted()) return DialogResult::none;
    // HostServices has checked that an accepted answer to a folder request
    // carries exactly one path.
    const HostPathDialogResult* const paths = std::get_if<HostPathDialogResult>(&answer.payload);
    if (paths == nullptr || (*paths).outcome != HostDialogOutcome::accepted) return DialogResult::cancel;
    if (!is_alive()) return DialogResult::cancel;
    selected_path_ = (*paths).paths.front();
    return DialogResult::ok;
}

} // namespace gui_forms
