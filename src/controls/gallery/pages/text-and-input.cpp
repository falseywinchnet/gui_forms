#include "pages.hpp"

namespace gui_forms::gallery::pages {

void add_text_input(const std::shared_ptr<Surface>& page,
                    const std::shared_ptr<PageContext>& context) {
    class TextStateUpdater final {
    public:
        TextStateUpdater(std::weak_ptr<TextBox> field,
                         std::weak_ptr<Label> state) noexcept
            : field_(std::move(field)), state_(std::move(state)) {}

        void operator()(const std::string&) const { update(); }
        void operator()(const TextSelection&) const { update(); }

    private:
        void update() const
        {
            const std::shared_ptr<TextBox> field = field_.lock();
            const std::shared_ptr<Label> state = state_.lock();
            if (!field || !state) return;
            const TextSelection selection = (*field).selection();
            (*state).set_text(
                "Selection: " + std::to_string(selection.start().value()) +
                ".." + std::to_string(selection.end().value()) +
                " bytes · Undo: " + ((*field).can_undo() ? "ready" : "clean") +
                " · Redo: " + ((*field).can_redo() ? "ready" : "clean"));
        }

        std::weak_ptr<TextBox> field_;
        std::weak_ptr<Label> state_;
    };

    enum class TextCommand {
        select_all,
        replace,
        undo,
        redo,
        copy,
        cut,
        paste,
    };

    class TextCommandClick final {
    public:
        TextCommandClick(std::weak_ptr<TextBox> field,
                         std::weak_ptr<Label> clipboard_status,
                         TextCommand command) noexcept
            : field_(std::move(field)),
              clipboard_status_(std::move(clipboard_status)),
              command_(command) {}

        void operator()(ButtonBase&) const
        {
            const std::shared_ptr<TextBox> field = field_.lock();
            if (!field) return;
            switch (command_) {
            case TextCommand::select_all:
                (*field).select_all();
                return;
            case TextCommand::replace:
                static_cast<void>((*field).replace_selection("GUI.Forms"));
                return;
            case TextCommand::undo:
                static_cast<void>((*field).undo());
                return;
            case TextCommand::redo:
                static_cast<void>((*field).redo());
                return;
            case TextCommand::copy:
                publish_clipboard_status(*field, "copy", (*field).copy());
                return;
            case TextCommand::cut:
                publish_clipboard_status(*field, "cut", (*field).cut());
                return;
            case TextCommand::paste:
                publish_clipboard_status(*field, "paste", (*field).paste());
                return;
            }
        }

    private:
        void publish_clipboard_status(TextBox& field,
                                      std::string_view command,
                                      bool accepted) const
        {
            if (const std::shared_ptr<Label> status = clipboard_status_.lock()) {
                (*status).set_text(
                    std::string("Clipboard ") + std::string(command) +
                    (accepted ? " · accepted" : " · unavailable or empty") +
                    " · " + std::to_string(field.text().size()) +
                    " UTF-8 bytes retained");
            }
        }

        std::weak_ptr<TextBox> field_;
        std::weak_ptr<Label> clipboard_status_;
        TextCommand command_{};
    };

    (*page).add_at(label("gallery.text.heading", "TEXT AND INPUT", 22.0, 700,
                       shell_blue_dark), {24.0, 18.0, 500.0, 34.0});
    (*page).add_at(label("gallery.text.lead",
                       "Unicode editing, word navigation, host clipboard commands, protected text, captured drag, caret deadlines, and history.",
                       12.0, 400, muted), {26.0, 52.0, 900.0, 24.0});

    std::shared_ptr<LayoutGroup> editing = group("gallery.text.editing", "Editable TextBox states",
                         {930.0, 290.0});
    (*page).add_at(editing, {24.0, 92.0, 930.0, 290.0});
    (*editing).add_at(label("gallery.text.primary.caption", "Primary field", 11.0,
                          600, muted), {24.0, 42.0, 130.0, 24.0});
    std::shared_ptr<gui_forms::TextBox> primary = make_control<TextBox>(StableId("gallery.text.primary"));
    (*primary).set_accessible_name("Primary field");
    (*primary).set_placeholder_text("Click, type, drag-select, use Shift+Arrow, Cmd/Ctrl+A, Z, Y");
    (*editing).add_at(primary, {154.0, 36.0, 700.0, 36.0});

    (*editing).add_at(label("gallery.text.unicode.caption", "Unicode field", 11.0,
                          600, muted), {24.0, 96.0, 130.0, 24.0});
    std::shared_ptr<gui_forms::TextBox> unicode = make_control<TextBox>(StableId("gallery.text.unicode"),
                                         "Graphemes: a\xCC\x81 · Ω · 日本語 · 🚀");
    (*unicode).set_accessible_name("Unicode field");
    (*editing).add_at(unicode, {154.0, 90.0, 700.0, 36.0});

    (*editing).add_at(label("gallery.text.readonly.caption", "Read-only", 11.0,
                          600, muted), {24.0, 150.0, 130.0, 24.0});
    std::shared_ptr<gui_forms::TextBox> read_only = make_control<TextBox>(StableId("gallery.text.readonly"),
                                           "Selection remains available; mutation is blocked.");
    (*read_only).set_accessible_name("Read-only field");
    (*read_only).set_read_only(true);
    (*editing).add_at(read_only, {154.0, 144.0, 700.0, 36.0});

    (*editing).add_at(label("gallery.text.password.caption", "Protected", 11.0,
                          600, muted), {24.0, 204.0, 130.0, 24.0});
    std::shared_ptr<gui_forms::TextBox> password = make_control<TextBox>(StableId("gallery.text.password"),
                                           "Portsmouth-Ω-2026");
    (*password).set_accessible_name("Protected credential field");
    (*password).set_accessible_description(
        "Password text is editable but excluded from paint traces, clipboard export, and semantic values.");
    (*password).set_use_system_password_character(true);
    (*editing).add_at(password, {154.0, 198.0, 700.0, 36.0});

    std::shared_ptr<Label> state = label("gallery.text.state", "Selection: empty · History: clean",
                       11.0, 600, green);
    (*editing).add_at(state, {154.0, 250.0, 700.0, 26.0});
    const std::weak_ptr<TextBox> weak_primary = primary;
    const std::weak_ptr<Label> weak_state = state;
    (*context).subscriptions.push_back((*primary).text_changed().subscribe(
        *state, TextStateUpdater(weak_primary, weak_state)));
    (*context).subscriptions.push_back((*primary).selection_changed().subscribe(
        *state, TextStateUpdater(weak_primary, weak_state)));

    std::shared_ptr<LayoutGroup> commands = group("gallery.text.commands", "Programmatic editing surface",
                          {930.0, 216.0});
    (*page).add_at(commands, {24.0, 404.0, 930.0, 216.0});
    const std::array<std::pair<std::size_t, std::string_view>, 7> command_specs{{
        {0U, "Select all"}, {4U, "Copy"}, {5U, "Cut"}, {6U, "Paste"},
        {1U, "Replace"}, {2U, "Undo"}, {3U, "Redo"},
    }};
    std::array<std::shared_ptr<Button>, 7> command_buttons;
    for (std::size_t position = 0; position < command_specs.size(); ++position) {
        const std::size_t index = command_specs[position].first;
        std::shared_ptr<gui_forms::Button> button = make_control<Button>(
            StableId("gallery.text.command." + std::to_string(index)),
            std::string(command_specs[position].second));
        (*button).set_visual_style(index == 1U ? ButtonVisualStyle::accent
                                             : ButtonVisualStyle::standard);
        (*commands).add_at(button,
            {24.0 + static_cast<double>(position) * 126.0, 42.0, 114.0, 36.0});
        command_buttons[index] = button;
    }
    (*context).subscriptions.push_back((*command_buttons[0]).clicked().subscribe(
        *primary, TextCommandClick(weak_primary, {}, TextCommand::select_all)));
    (*context).subscriptions.push_back((*command_buttons[1]).clicked().subscribe(
        *primary, TextCommandClick(weak_primary, {}, TextCommand::replace)));
    (*context).subscriptions.push_back((*command_buttons[2]).clicked().subscribe(
        *primary, TextCommandClick(weak_primary, {}, TextCommand::undo)));
    (*context).subscriptions.push_back((*command_buttons[3]).clicked().subscribe(
        *primary, TextCommandClick(weak_primary, {}, TextCommand::redo)));
    std::shared_ptr<Label> clipboard_status = label(
        "gallery.text.clipboard.status",
        "Clipboard: select text, then use buttons or Cmd/Ctrl+C, X, V",
        11.0, 600, green);
    (*commands).add_at(clipboard_status, {24.0, 92.0, 830.0, 26.0});
    const std::weak_ptr<Label> weak_clipboard_status = clipboard_status;
    (*context).subscriptions.push_back((*command_buttons[4]).clicked().subscribe(
        *primary, TextCommandClick(
            weak_primary, weak_clipboard_status, TextCommand::copy)));
    (*context).subscriptions.push_back((*command_buttons[5]).clicked().subscribe(
        *primary, TextCommandClick(
            weak_primary, weak_clipboard_status, TextCommand::cut)));
    (*context).subscriptions.push_back((*command_buttons[6]).clicked().subscribe(
        *primary, TextCommandClick(
            weak_primary, weak_clipboard_status, TextCommand::paste)));
    std::shared_ptr<Label> guarantee = label("gallery.text.guarantee",
                           "TextBox + TextStore own editing. HostServices only transports bounded UTF-8 clipboard data; protected fields never export values.",
                           11.0, 600, shell_blue_dark);
    (*guarantee).set_text_wrapping(TextWrapping::word);
    (*guarantee).set_vertical_alignment(VerticalAlignment::near);
    (*commands).add_at(guarantee, {24.0, 132.0, 830.0, 52.0});
}

} // namespace gui_forms::gallery::pages
