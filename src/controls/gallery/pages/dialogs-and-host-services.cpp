#include "pages.hpp"

namespace gui_forms::gallery::pages {

void add_dialogs_and_host_services(
    const std::shared_ptr<Surface>& page,
    const std::shared_ptr<PageContext>& context) {
    (*page).add_at(label("gallery.host.heading", "DIALOGS AND HOST SERVICES", 22.0,
                       700, shell_blue_dark), {24.0, 18.0, 620.0, 34.0});
    (*page).add_at(label(
        "gallery.host.lead",
        "One portable contract for modal results, clipboard, monitor geometry, and semantic sound across native hosts.",
        12.0, 400, muted), {26.0, 52.0, 900.0, 24.0});

    std::shared_ptr<LayoutGroup> messages = group("gallery.host.messages",
                          "Message dialogs · buttons, icons, default choices",
                          {930.0, 132.0});
    (*page).add_at(messages, {24.0, 92.0, 930.0, 132.0});
    struct MessageCase final {
        std::string_view label;
        HostMessageButtons buttons;
        HostMessageIcon icon;
        HostDialogChoice default_choice;
    };

    class DialogResultPublisher final {
    public:
        explicit DialogResultPublisher(
            std::shared_ptr<PageContext> context) noexcept
            : context_(std::move(context)) {}

        void operator()(const HostDialogResult& result) const
        {
            if (!(*context_).host_services_status) return;
            if (!result.status.accepted()) {
                (*(*context_).host_services_status).set_text(
                    std::string("Host rejected request · ") +
                    host_service_error_name(result.status.error));
                return;
            }
            std::visit(*this, result.payload);
        }

        void operator()(const HostMessageDialogResult& value) const
        {
            const std::string detail = std::string("Message ") +
                host_dialog_outcome_name(value.outcome) + " · " +
                host_dialog_choice_name(value.choice);
            if (value.outcome == HostDialogOutcome::accepted) {
                (*context_).last_dialog_acceptance = detail;
            }
            (*(*context_).host_services_status).set_text(
                value.outcome == HostDialogOutcome::cancelled
                    ? detail + " · preserved: " +
                          (*context_).last_dialog_acceptance
                    : detail);
        }

        void operator()(const HostPathDialogResult& value) const
        {
            if (value.outcome == HostDialogOutcome::accepted) {
                (*context_).last_dialog_acceptance =
                    std::to_string(value.paths.size()) + " path" +
                    (value.paths.size() == 1U ? "" : "s") + " · " +
                    value.paths.front();
                (*(*context_).host_services_status).set_text(
                    "Accepted · " + (*context_).last_dialog_acceptance);
            } else {
                publish_cancelled();
            }
        }

        void operator()(const HostColorDialogResult& value) const
        {
            if (value.outcome == HostDialogOutcome::accepted) {
                char encoded[16]{};
                std::snprintf(encoded, sizeof(encoded), "#%08X", value.rgba);
                (*context_).last_dialog_acceptance =
                    std::string("color ") + encoded;
                (*(*context_).host_services_status).set_text(
                    "Accepted · " + (*context_).last_dialog_acceptance);
            } else {
                publish_cancelled();
            }
        }

    private:
        void publish_cancelled() const
        {
            (*(*context_).host_services_status).set_text(
                "Cancelled · preserved: " +
                (*context_).last_dialog_acceptance);
        }

        std::shared_ptr<PageContext> context_;
    };

    class DialogInvoker final {
    public:
        DialogInvoker(std::shared_ptr<PageContext> context,
                      DialogResultPublisher publisher) noexcept
            : context_(std::move(context)), publisher_(std::move(publisher)) {}

        void invoke(ButtonBase& source,
                    HostDialogRequestPayload payload) const
        {
            HostServices* services = source.attached_window() == nullptr
                ? nullptr : (*source.attached_window()).host_services();
            if (services == nullptr) {
                HostDialogResult unavailable;
                unavailable.status.error = HostServiceError::unsupported;
                publisher_(unavailable);
                return;
            }
            HostDialogRequest request;
            request.request_id = (*context_).next_host_request_id++;
            request.owner_id = std::string(source.stable_id().value());
            request.payload = std::move(payload);
            publisher_((*services).show_dialog(request));
        }

    private:
        std::shared_ptr<PageContext> context_;
        DialogResultPublisher publisher_;
    };

    class MessageDialogClick final {
    public:
        MessageDialogClick(DialogInvoker invoker, MessageCase value) noexcept
            : invoker_(std::move(invoker)), value_(value) {}

        void operator()(ButtonBase& source) const
        {
            HostMessageDialogRequest request;
            request.title = "GUI.Forms message contract";
            request.message =
                "This modal result travels through the portable HostServices boundary.";
            request.buttons = value_.buttons;
            request.icon = value_.icon;
            request.default_choice = value_.default_choice;
            invoker_.invoke(source, std::move(request));
        }

    private:
        DialogInvoker invoker_;
        MessageCase value_;
    };

    class PathDialogClick final {
    public:
        PathDialogClick(DialogInvoker invoker, std::size_t index) noexcept
            : invoker_(std::move(invoker)), index_(index) {}

        void operator()(ButtonBase& source) const
        {
            if (index_ <= 1U) {
                HostOpenFileDialogRequest request;
                request.title = index_ == 0U
                    ? "Open one fixture" : "Open fixture set";
                request.filters = {{"Text and logs", {"txt", "log"}},
                                   {"PNG images", {"png"}}};
                request.allow_multiple = index_ == 1U;
                invoker_.invoke(source, std::move(request));
            } else if (index_ == 2U) {
                HostSaveFileDialogRequest request;
                request.title = "Save GUI.Forms evidence";
                request.suggested_name = "gui-forms-evidence";
                request.default_extension = "txt";
                request.filters = {{"Text evidence", {"txt"}}};
                request.confirm_overwrite = true;
                invoker_.invoke(source, std::move(request));
            } else if (index_ == 3U) {
                invoker_.invoke(source, HostFolderDialogRequest{
                    "Choose an evidence directory", {}});
            } else {
                invoker_.invoke(source, HostColorDialogRequest{
                    "Choose an accent color", 0x2774B8FFU, false});
            }
        }

    private:
        DialogInvoker invoker_;
        std::size_t index_{};
    };

    class PrimaryMonitorPredicate final {
    public:
        bool operator()(const HostMonitor& monitor) const noexcept
        {
            return monitor.primary;
        }
    };

    class HostServiceClick final {
    public:
        HostServiceClick(std::shared_ptr<PageContext> context,
                         std::size_t index) noexcept
            : context_(std::move(context)), index_(index) {}

        void operator()(ButtonBase& source) const
        {
            HostServices* services = source.attached_window() == nullptr
                ? nullptr : (*source.attached_window()).host_services();
            if (services == nullptr) {
                (*(*context_).host_services_status).set_text(
                    "Host service unavailable · portable control remains responsive");
                return;
            }
            if (index_ == 0U) {
                write_clipboard(*services);
            } else if (index_ == 1U) {
                read_clipboard(*services);
            } else if (index_ == 2U) {
                inspect_monitors(*services);
            } else {
                play_sound(*services);
            }
        }

    private:
        void write_clipboard(HostServices& services) const
        {
            const HostServiceStatus result = services.write_clipboard_text(
                (*(*context_).clipboard_editor).text());
            (*(*context_).host_services_status).set_text(
                std::string("Clipboard write · ") +
                host_service_error_name(result.error));
        }

        void read_clipboard(HostServices& services) const
        {
            const HostClipboardTextResult result = services.read_clipboard_text();
            if (result.status.accepted() && result.has_text) {
                (*(*context_).clipboard_editor).set_text(result.text_utf8);
            }
            (*(*context_).host_services_status).set_text(
                std::string("Clipboard read · ") +
                host_service_error_name(result.status.error) +
                (result.has_text ? " · text restored" : " · empty"));
        }

        void inspect_monitors(HostServices& services) const
        {
            const HostMonitorResult result = services.query_monitors();
            if (!result.status.accepted() || result.monitors.empty()) {
                (*(*context_).host_services_status).set_text(
                    std::string("Monitor query · ") +
                    host_service_error_name(result.status.error));
                return;
            }
            const std::vector<HostMonitor>::const_iterator primary =
                std::find_if(result.monitors.begin(), result.monitors.end(),
                             PrimaryMonitorPredicate());
            const HostMonitor& monitor = primary == result.monitors.end()
                ? result.monitors.front() : *primary;
            (*(*context_).host_services_status).set_text(
                "Monitors " + std::to_string(result.monitors.size()) +
                " · primary " + monitor.id + " · scale " +
                std::to_string(monitor.scale));
        }

        void play_sound(HostServices& services) const
        {
            const std::chrono::steady_clock::duration now =
                std::chrono::steady_clock::now().time_since_epoch();
            const std::chrono::nanoseconds::rep stamp =
                std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
            const HostSoundCue cue = index_ == 3U
                ? HostSoundCue::operation_complete : HostSoundCue::warning;
            const HostServiceStatus result = services.play_sound_cue(
                {cue, 0.72, static_cast<std::uint64_t>(stamp)});
            (*(*context_).host_services_status).set_text(
                std::string("Sound ") + host_sound_cue_name(cue) +
                " · " + host_service_error_name(result.error));
        }

        std::shared_ptr<PageContext> context_;
        std::size_t index_{};
    };

    constexpr std::array<MessageCase, 4> message_cases{{
        {"Information · OK", HostMessageButtons::ok,
         HostMessageIcon::information, HostDialogChoice::ok},
        {"Warning · OK/Cancel", HostMessageButtons::ok_cancel,
         HostMessageIcon::warning, HostDialogChoice::cancel},
        {"Question · Yes/No", HostMessageButtons::yes_no,
         HostMessageIcon::question, HostDialogChoice::yes},
        {"Error · Retry/Cancel", HostMessageButtons::retry_cancel,
         HostMessageIcon::error, HostDialogChoice::retry},
    }};

    const DialogResultPublisher publish_dialog_result(context);
    const DialogInvoker invoke_dialog(context, publish_dialog_result);

    for (std::size_t index = 0U; index < message_cases.size(); ++index) {
        const MessageCase value = message_cases[index];
        std::shared_ptr<gui_forms::Button> button = make_control<Button>(
            StableId("gallery.host.message." + std::to_string(index)),
            std::string(value.label));
        (*button).set_visual_style(index == 2U ? ButtonVisualStyle::accent
                                             : ButtonVisualStyle::command);
        (*messages).add_at(button, {24.0 + static_cast<double>(index) * 220.0,
                                  44.0, 202.0, 42.0});
        (*context).subscriptions.push_back((*button).clicked().subscribe(
            *messages, MessageDialogClick(invoke_dialog, value)));
    }

    std::shared_ptr<LayoutGroup> paths = group("gallery.host.paths",
                       "Path and color dialogs · typed requests and cancel preservation",
                       {930.0, 142.0});
    (*page).add_at(paths, {24.0, 242.0, 930.0, 142.0});
    const std::array<std::string_view, 5> path_labels{
        "Open file", "Open multiple", "Save file", "Choose folder", "Choose color"};
    for (std::size_t index = 0U; index < path_labels.size(); ++index) {
        std::shared_ptr<gui_forms::Button> button = make_control<Button>(
            StableId("gallery.host.dialog." + std::to_string(index)),
            std::string(path_labels[index]));
        (*button).set_visual_style(index == 4U ? ButtonVisualStyle::accent
                                             : ButtonVisualStyle::standard);
        (*paths).add_at(button, {24.0 + static_cast<double>(index) * 176.0,
                              44.0, 158.0, 40.0});
        (*context).subscriptions.push_back((*button).clicked().subscribe(
            *paths, PathDialogClick(invoke_dialog, index)));
    }
    (*paths).add_at(label("gallery.host.paths.note",
                        "Cancel never overwrites the last accepted path, choice, or color.",
                        11.0, 400, muted), {24.0, 98.0, 700.0, 24.0});

    std::shared_ptr<LayoutGroup> services_group = group(
        "gallery.host.services",
        "Host services · clipboard, monitors, completion cue",
        {930.0, 224.0});
    (*page).add_at(services_group, {24.0, 402.0, 930.0, 224.0});
    (*context).clipboard_editor = make_control<TextBox>(
        StableId("gallery.host.clipboard.editor"));
    (*(*context).clipboard_editor).set_text("GUI.Forms clipboard probe Ω");
    (*(*context).clipboard_editor).set_accessible_name("Clipboard probe text");
    (*services_group).add_at((*context).clipboard_editor, {24.0, 42.0, 432.0, 32.0});

    const std::array<std::string_view, 5> service_labels{
        "Copy to host", "Paste from host", "Inspect monitors",
        "Completion cue", "Warning cue"};
    for (std::size_t index = 0U; index < service_labels.size(); ++index) {
        std::shared_ptr<gui_forms::Button> button = make_control<Button>(
            StableId("gallery.host.service." + std::to_string(index)),
            std::string(service_labels[index]));
        (*button).set_visual_style(ButtonVisualStyle::command);
        (*services_group).add_at(button,
            {24.0 + static_cast<double>(index) * 176.0, 88.0, 158.0, 36.0});
        (*context).subscriptions.push_back((*button).clicked().subscribe(
            *services_group, HostServiceClick(context, index)));
    }
    (*context).host_services_status = label(
        "gallery.host.status",
        "Ready · native host results remain typed, bounded, and owner-modal",
        11.0, 600, green);
    (*(*context).host_services_status).set_text_wrapping(TextWrapping::word);
    (*(*context).host_services_status).set_vertical_alignment(VerticalAlignment::near);
    (*services_group).add_at((*context).host_services_status,
                           {24.0, 142.0, 872.0, 56.0});
}

} // namespace gui_forms::gallery::pages
