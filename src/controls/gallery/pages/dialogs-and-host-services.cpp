#include "pages.hpp"

#include <variant>

namespace gui_forms::gallery::pages {

// Owns the host-services page: one Command per message dialog, path/color
// dialog and host service, each bound to its button; the typed dialog results
// and the last accepted value are page state, not shell state.
class HostServicesPage final : public gui_forms::Component {
public:
    HostServicesPage(const std::shared_ptr<Surface>& body);

private:
    struct MessageCase final {
        std::string_view label;
        HostMessageButtons buttons;
        HostMessageIcon icon;
        HostDialogChoice default_choice;
    };
    static constexpr std::array<MessageCase, 4> message_cases{{
        {"Information · OK", HostMessageButtons::ok,
         HostMessageIcon::information, HostDialogChoice::ok},
        {"Warning · OK/Cancel", HostMessageButtons::ok_cancel,
         HostMessageIcon::warning, HostDialogChoice::cancel},
        {"Question · Yes/No", HostMessageButtons::yes_no,
         HostMessageIcon::question, HostDialogChoice::yes},
        {"Error · Retry/Cancel", HostMessageButtons::retry_cancel,
         HostMessageIcon::error, HostDialogChoice::retry},
    }};
    static constexpr std::array<std::string_view, 5> path_labels{
        "Open file", "Open multiple", "Save file", "Choose folder", "Choose color"};
    static constexpr std::array<std::string_view, 5> service_labels{
        "Copy to host", "Paste from host", "Inspect monitors",
        "Completion cue", "Warning cue"};

    void build_messages(const std::shared_ptr<Surface>& body);
    void build_paths(const std::shared_ptr<Surface>& body);
    void build_services(const std::shared_ptr<Surface>& body);

    void on_message_dialog(int index, const CommandInvocation& invocation);
    void on_path_dialog(int index, const CommandInvocation& invocation);
    void on_service(int index);

    [[nodiscard]] HostServices* services() const;
    void show_dialog(const CommandInvocation& invocation,
                     HostDialogRequestPayload payload);
    void publish(const HostDialogResult& result);
    void publish_accepted(std::string acceptance);
    void publish_cancelled();
    void write_clipboard(HostServices& host);
    void read_clipboard(HostServices& host);
    void inspect_monitors(HostServices& host);
    void play_sound(HostServices& host, HostSoundCue cue);

    std::array<Command, 4> message_commands_{};
    std::array<Command, 5> dialog_commands_{};
    std::array<Command, 5> service_commands_{};
    std::shared_ptr<Label> status_;
    std::shared_ptr<TextBox> clipboard_;
    std::string last_acceptance_{"No accepted dialog value yet"};
    std::uint64_t next_request_id_{1U};
};

HostServicesPage::HostServicesPage(const std::shared_ptr<Surface>& body) {
    (*body).add_at(label("gallery.host.heading", "DIALOGS AND HOST SERVICES", 22.0,
                         700, shell_blue_dark), {24.0, 18.0, 620.0, 34.0});
    (*body).add_at(label(
        "gallery.host.lead",
        "One portable contract for modal results, clipboard, monitor geometry, and semantic sound across native hosts.",
        12.0, 400, muted), {26.0, 52.0, 900.0, 24.0});
    build_messages(body);
    build_paths(body);
    build_services(body);
}

void HostServicesPage::build_messages(const std::shared_ptr<Surface>& body) {
    std::shared_ptr<LayoutGroup> messages = group(
        "gallery.host.messages", "Message dialogs · buttons, icons, default choices",
        {930.0, 132.0});
    (*body).add_at(messages, {24.0, 92.0, 930.0, 132.0});
    for (std::size_t index = 0U; index < message_cases.size(); ++index) {
        Command& command = message_commands_[index];
        command.set_text(std::string(message_cases[index].label));
        command.set_description("Show a modal message dialog through HostServices");
        std::shared_ptr<Button> button = make_control<Button>(
            StableId("gallery.host.message." + std::to_string(index)),
            std::string(message_cases[index].label));
        (*button).set_visual_style(index == 2U ? ButtonVisualStyle::accent
                                               : ButtonVisualStyle::command);
        (*button).bind(command);
        (*messages).add_at(button, {24.0 + static_cast<double>(index) * 220.0,
                                    44.0, 202.0, 42.0});
        on(command.invoked(), *this, &HostServicesPage::on_message_dialog,
           static_cast<int>(index));
    }
}

void HostServicesPage::build_paths(const std::shared_ptr<Surface>& body) {
    std::shared_ptr<LayoutGroup> paths = group(
        "gallery.host.paths",
        "Path and color dialogs · typed requests and cancel preservation",
        {930.0, 142.0});
    (*body).add_at(paths, {24.0, 242.0, 930.0, 142.0});
    for (std::size_t index = 0U; index < path_labels.size(); ++index) {
        Command& command = dialog_commands_[index];
        command.set_text(std::string(path_labels[index]));
        command.set_description("Show a typed path or color dialog through HostServices");
        std::shared_ptr<Button> button = make_control<Button>(
            StableId("gallery.host.dialog." + std::to_string(index)),
            std::string(path_labels[index]));
        (*button).set_visual_style(index == 4U ? ButtonVisualStyle::accent
                                               : ButtonVisualStyle::standard);
        (*button).bind(command);
        (*paths).add_at(button, {24.0 + static_cast<double>(index) * 176.0,
                                 44.0, 158.0, 40.0});
        on(command.invoked(), *this, &HostServicesPage::on_path_dialog,
           static_cast<int>(index));
    }
    (*paths).add_at(label("gallery.host.paths.note",
                          "Cancel never overwrites the last accepted path, choice, or color.",
                          11.0, 400, muted), {24.0, 98.0, 700.0, 24.0});
}

void HostServicesPage::build_services(const std::shared_ptr<Surface>& body) {
    std::shared_ptr<LayoutGroup> services_group = group(
        "gallery.host.services", "Host services · clipboard, monitors, completion cue",
        {930.0, 224.0});
    (*body).add_at(services_group, {24.0, 402.0, 930.0, 224.0});
    clipboard_ = make_control<TextBox>(StableId("gallery.host.clipboard.editor"));
    (*clipboard_).set_text("GUI.Forms clipboard probe Ω");
    (*clipboard_).set_accessible_name("Clipboard probe text");
    (*services_group).add_at(clipboard_, {24.0, 42.0, 432.0, 32.0});

    for (std::size_t index = 0U; index < service_labels.size(); ++index) {
        Command& command = service_commands_[index];
        command.set_text(std::string(service_labels[index]));
        command.set_description("Call one portable HostServices operation");
        std::shared_ptr<Button> button = make_control<Button>(
            StableId("gallery.host.service." + std::to_string(index)),
            std::string(service_labels[index]));
        (*button).set_visual_style(ButtonVisualStyle::command);
        (*button).bind(command);
        (*services_group).add_at(button,
            {24.0 + static_cast<double>(index) * 176.0, 88.0, 158.0, 36.0});
        on(command.invoked(), *this, &HostServicesPage::on_service,
           static_cast<int>(index));
    }
    status_ = label("gallery.host.status",
                    "Ready · native host results remain typed, bounded, and owner-modal",
                    11.0, 600, green);
    (*status_).set_text_wrapping(TextWrapping::word);
    (*status_).set_vertical_alignment(VerticalAlignment::near);
    (*services_group).add_at(status_, {24.0, 142.0, 872.0, 56.0});
}

HostServices* HostServicesPage::services() const {
    Window* const window = (*status_).attached_window();
    return window == nullptr ? nullptr : (*window).host_services();
}

void HostServicesPage::on_message_dialog(const int index,
                                         const CommandInvocation& invocation) {
    const MessageCase& value = message_cases[static_cast<std::size_t>(index)];
    HostMessageDialogRequest request;
    request.title = "GUI.Forms message contract";
    request.message =
        "This modal result travels through the portable HostServices boundary.";
    request.buttons = value.buttons;
    request.icon = value.icon;
    request.default_choice = value.default_choice;
    show_dialog(invocation, std::move(request));
}

void HostServicesPage::on_path_dialog(const int index,
                                      const CommandInvocation& invocation) {
    if (index <= 1) {
        HostOpenFileDialogRequest request;
        request.title = index == 0 ? "Open one fixture" : "Open fixture set";
        request.filters = {{"Text and logs", {"txt", "log"}}, {"PNG images", {"png"}}};
        request.allow_multiple = index == 1;
        show_dialog(invocation, std::move(request));
    } else if (index == 2) {
        HostSaveFileDialogRequest request;
        request.title = "Save GUI.Forms evidence";
        request.suggested_name = "gui-forms-evidence";
        request.default_extension = "txt";
        request.filters = {{"Text evidence", {"txt"}}};
        request.confirm_overwrite = true;
        show_dialog(invocation, std::move(request));
    } else if (index == 3) {
        show_dialog(invocation,
                    HostFolderDialogRequest{"Choose an evidence directory", {}});
    } else {
        show_dialog(invocation,
                    HostColorDialogRequest{"Choose an accent color", 0x2774B8FFU, false});
    }
}

void HostServicesPage::on_service(const int index) {
    HostServices* const host = services();
    if (host == nullptr) {
        (*status_).set_text(
            "Host service unavailable · portable control remains responsive");
        return;
    }
    switch (index) {
    case 0: write_clipboard(*host); break;
    case 1: read_clipboard(*host); break;
    case 2: inspect_monitors(*host); break;
    case 3: play_sound(*host, HostSoundCue::operation_complete); break;
    default: play_sound(*host, HostSoundCue::warning); break;
    }
}

void HostServicesPage::show_dialog(const CommandInvocation& invocation,
                                   HostDialogRequestPayload payload) {
    HostServices* const host = services();
    if (host == nullptr) {
        HostDialogResult unavailable;
        unavailable.status.error = HostServiceError::unsupported;
        publish(unavailable);
        return;
    }
    HostDialogRequest request;
    request.request_id = next_request_id_++;
    request.owner_id = invocation.source_id;
    request.payload = std::move(payload);
    publish((*host).show_dialog(request));
}

void HostServicesPage::publish(const HostDialogResult& result) {
    if (!result.status.accepted()) {
        (*status_).set_text(std::string("Host rejected request · ") +
                            host_service_error_name(result.status.error));
        return;
    }
    if (const HostMessageDialogResult* message =
            std::get_if<HostMessageDialogResult>(&result.payload)) {
        const std::string detail = std::string("Message ") +
            host_dialog_outcome_name((*message).outcome) + " · " +
            host_dialog_choice_name((*message).choice);
        if ((*message).outcome == HostDialogOutcome::accepted) {
            last_acceptance_ = detail;
        }
        (*status_).set_text((*message).outcome == HostDialogOutcome::cancelled
                                ? detail + " · preserved: " + last_acceptance_
                                : detail);
        return;
    }
    if (const HostPathDialogResult* path =
            std::get_if<HostPathDialogResult>(&result.payload)) {
        if ((*path).outcome != HostDialogOutcome::accepted || (*path).paths.empty()) {
            publish_cancelled();
            return;
        }
        publish_accepted(std::to_string((*path).paths.size()) + " path" +
                         ((*path).paths.size() == 1U ? "" : "s") + " · " +
                         (*path).paths.front());
        return;
    }
    if (const HostColorDialogResult* color =
            std::get_if<HostColorDialogResult>(&result.payload)) {
        if ((*color).outcome != HostDialogOutcome::accepted) {
            publish_cancelled();
            return;
        }
        char encoded[16]{};
        std::snprintf(encoded, sizeof(encoded), "#%08X", (*color).rgba);
        publish_accepted(std::string("color ") + encoded);
    }
}

void HostServicesPage::publish_accepted(std::string acceptance) {
    last_acceptance_ = std::move(acceptance);
    (*status_).set_text("Accepted · " + last_acceptance_);
}

void HostServicesPage::publish_cancelled() {
    (*status_).set_text("Cancelled · preserved: " + last_acceptance_);
}

void HostServicesPage::write_clipboard(HostServices& host) {
    const HostServiceStatus result = host.write_clipboard_text((*clipboard_).text());
    (*status_).set_text(std::string("Clipboard write · ") +
                        host_service_error_name(result.error));
}

void HostServicesPage::read_clipboard(HostServices& host) {
    const HostClipboardTextResult result = host.read_clipboard_text();
    if (result.status.accepted() && result.has_text) {
        (*clipboard_).set_text(result.text_utf8);
    }
    (*status_).set_text(std::string("Clipboard read · ") +
                        host_service_error_name(result.status.error) +
                        (result.has_text ? " · text restored" : " · empty"));
}

void HostServicesPage::inspect_monitors(HostServices& host) {
    const HostMonitorResult result = host.query_monitors();
    if (!result.status.accepted() || result.monitors.empty()) {
        (*status_).set_text(std::string("Monitor query · ") +
                            host_service_error_name(result.status.error));
        return;
    }
    const HostMonitor* primary = &result.monitors.front();
    for (const HostMonitor& monitor : result.monitors) {
        if (monitor.primary) {
            primary = &monitor;
            break;
        }
    }
    (*status_).set_text("Monitors " + std::to_string(result.monitors.size()) +
                        " · primary " + (*primary).id + " · scale " +
                        std::to_string((*primary).scale));
}

void HostServicesPage::play_sound(HostServices& host, const HostSoundCue cue) {
    const std::chrono::steady_clock::duration now =
        std::chrono::steady_clock::now().time_since_epoch();
    const std::chrono::nanoseconds::rep stamp =
        std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
    const HostServiceStatus result =
        host.play_sound_cue({cue, 0.72, static_cast<std::uint64_t>(stamp)});
    (*status_).set_text(std::string("Sound ") + host_sound_cue_name(cue) + " · " +
                        host_service_error_name(result.error));
}

void add_dialogs_and_host_services(
    const std::shared_ptr<Surface>& page,
    const std::shared_ptr<PageContext>& context) {
    (*context).components.add(std::make_shared<HostServicesPage>(page));
}

} // namespace gui_forms::gallery::pages
