#include "pages.hpp"

namespace gui_forms::gallery::pages {
namespace {

// CommandBar item ids. The bar names its retained buttons
// "gallery.text.command.item.<id>"; the numbering is the retired page's.
constexpr int command_select_all = 0;
constexpr int command_replace = 1;
constexpr int command_undo = 2;
constexpr int command_redo = 3;
constexpr int command_copy = 4;
constexpr int command_cut = 5;
constexpr int command_paste = 6;

class TextAndInputPage final : public Component {
public:
    TextAndInputPage(const std::shared_ptr<Surface>& body,
                     const std::shared_ptr<PageContext>& context) {
        static_cast<void>(context);
        (*body).add_at(label("gallery.text.heading", "TEXT AND INPUT", 22.0, 700,
                             shell_blue_dark), {24.0, 18.0, 500.0, 34.0});
        (*body).add_at(label("gallery.text.lead",
                             "Unicode editing, word navigation, host clipboard commands, protected text, captured drag, caret deadlines, and history.",
                             12.0, 400, muted), {26.0, 52.0, 900.0, 24.0});
        build_fields(body);
        build_commands(body);
        build_masked(body);
        build_auto_complete(body);
        refresh_state();
    }

private:
    void build_fields(const std::shared_ptr<Surface>& body) {
        std::shared_ptr<LayoutGroup> editing = group("gallery.text.editing",
                                                     "Editable TextBox states", {930.0, 290.0});
        (*body).add_at(editing, {24.0, 92.0, 930.0, 290.0});
        (*editing).add_at(label("gallery.text.primary.caption", "Primary field", 11.0, 600, muted),
                          {24.0, 42.0, 130.0, 24.0});
        primary_ = make_control<TextBox>(StableId("gallery.text.primary"));
        (*primary_).set_accessible_name("Primary field");
        (*primary_).set_placeholder_text(
            "Click, type, drag-select, use Shift+Arrow, Cmd/Ctrl+A, Z, Y");
        (*editing).add_at(primary_, {154.0, 36.0, 700.0, 36.0});

        (*editing).add_at(label("gallery.text.unicode.caption", "Unicode field", 11.0, 600, muted),
                          {24.0, 96.0, 130.0, 24.0});
        std::shared_ptr<TextBox> unicode = make_control<TextBox>(
            StableId("gallery.text.unicode"), "Graphemes: a\xCC\x81 · Ω · 日本語 · 🚀");
        (*unicode).set_accessible_name("Unicode field");
        (*editing).add_at(unicode, {154.0, 90.0, 700.0, 36.0});

        (*editing).add_at(label("gallery.text.readonly.caption", "Read-only", 11.0, 600, muted),
                          {24.0, 150.0, 130.0, 24.0});
        std::shared_ptr<TextBox> read_only = make_control<TextBox>(
            StableId("gallery.text.readonly"), "Selection remains available; mutation is blocked.");
        (*read_only).set_accessible_name("Read-only field");
        (*read_only).set_read_only(true);
        (*editing).add_at(read_only, {154.0, 144.0, 700.0, 36.0});

        (*editing).add_at(label("gallery.text.password.caption", "Protected", 11.0, 600, muted),
                          {24.0, 204.0, 130.0, 24.0});
        std::shared_ptr<TextBox> password = make_control<TextBox>(
            StableId("gallery.text.password"), "Portsmouth-Ω-2026");
        (*password).set_accessible_name("Protected credential field");
        (*password).set_accessible_description(
            "Password text is editable but excluded from paint traces, clipboard export, and semantic values.");
        (*password).set_use_system_password_character(true);
        (*editing).add_at(password, {154.0, 198.0, 700.0, 36.0});

        state_ = label("gallery.text.state", "Selection: empty · History: clean", 11.0, 600, green);
        (*editing).add_at(state_, {154.0, 250.0, 700.0, 26.0});
        // Both are state events; the handler reads the field, so it takes no
        // event argument and serves either publisher.
        on((*primary_).text_changed(), *this, &TextAndInputPage::refresh_state);
        on((*primary_).selection_changed(), *this, &TextAndInputPage::refresh_state);
    }

    void build_commands(const std::shared_ptr<Surface>& body) {
        std::shared_ptr<LayoutGroup> commands = group("gallery.text.commands",
                                                      "Programmatic editing surface", {930.0, 216.0});
        (*body).add_at(commands, {24.0, 404.0, 930.0, 216.0});
        // Undo and Redo are Commands so their availability (the field's
        // history) projects onto the bar's retained buttons; the other five
        // items dispatch through the bar's single group notification.
        bar_ = make_control<CommandBar>(StableId("gallery.text.command"));
        (*bar_).set_style(page_style());
        (*bar_).set_accessible_name("Editing commands");
        (*bar_).set_items({
            {.id = command_select_all, .label = "Select all"},
            {.id = command_copy, .label = "Copy"},
            {.id = command_cut, .label = "Cut"},
            {.id = command_paste, .label = "Paste"},
            {.id = command_replace, .label = "Replace"},
            {.id = command_undo, .label = "Undo", .command = &undo_},
            {.id = command_redo, .label = "Redo", .command = &redo_},
        });
        for (int index = 0; index < static_cast<int>((*bar_).items().size()); ++index) {
            (*bar_).item(index).set_style(page_style());
        }
        (*bar_).item(4).set_visual_style(ButtonVisualStyle::accent);
        (*commands).add_at(bar_, {24.0, 42.0, 870.0, 36.0});
        on((*bar_).invoked(), *this, &TextAndInputPage::on_command);
        on(undo_.invoked(), *this, &TextAndInputPage::on_undo);
        on(redo_.invoked(), *this, &TextAndInputPage::on_redo);

        clipboard_status_ = label("gallery.text.clipboard.status",
                                  "Clipboard: select text, then use buttons or Cmd/Ctrl+C, X, V",
                                  11.0, 600, green);
        (*commands).add_at(clipboard_status_, {24.0, 92.0, 830.0, 26.0});
        std::shared_ptr<Label> guarantee = label(
            "gallery.text.guarantee",
            "TextBox + TextStore own editing. HostServices only transports bounded UTF-8 clipboard data; protected fields never export values.",
            11.0, 600, shell_blue_dark);
        (*guarantee).set_text_wrapping(TextWrapping::word);
        (*guarantee).set_vertical_alignment(VerticalAlignment::near);
        (*commands).add_at(guarantee, {24.0, 132.0, 830.0, 52.0});
    }

    // MaskedTextBox specimens: a telephone mask, and a part code whose
    // letters are shifted to upper and then lower case. The telephone
    // field's refusals and completion report to the status label.
    void build_masked(const std::shared_ptr<Surface>& body) {
        std::shared_ptr<LayoutGroup> masked = group("gallery.text.masked",
                                                    "MaskedTextBox · input through a mask", {930.0, 132.0});
        (*body).add_at(masked, {24.0, 640.0, 930.0, 132.0});
        (*masked).add_at(label("gallery.text.masked.phone.caption", "Telephone", 11.0, 600, muted),
                         {24.0, 42.0, 130.0, 24.0});
        phone_ = make_control<MaskedTextBox>(StableId("gallery.text.masked.phone"), "(000) 000-0000");
        (*phone_).set_accessible_name("Telephone number");
        (*masked).add_at(phone_, {154.0, 36.0, 260.0, 36.0});
        (*masked).add_at(label("gallery.text.masked.code.caption", "Part code", 11.0, 600, muted),
                         {440.0, 42.0, 110.0, 24.0});
        std::shared_ptr<MaskedTextBox> code = make_control<MaskedTextBox>(
            StableId("gallery.text.masked.code"), ">LL-0000-<aa");
        (*code).set_accessible_name("Part code");
        (*masked).add_at(code, {550.0, 36.0, 300.0, 36.0});
        masked_status_ = label("gallery.text.masked.status",
                               "Telephone: digits only; letters are refused and reported", 11.0, 600, green);
        (*masked).add_at(masked_status_, {154.0, 86.0, 700.0, 26.0});
        on((*phone_).mask_input_rejected(), *this, &TextAndInputPage::masked_rejected);
        on((*phone_).text_changed(), *this, &TextAndInputPage::masked_changed);
    }

    // Auto-complete specimens: a TextBox that suggests and appends from its
    // own collection, and an editable ComboBox that completes from its
    // items. Accepted completions report to the status label.
    void build_auto_complete(const std::shared_ptr<Surface>& body) {
        std::shared_ptr<LayoutGroup> completion = group("gallery.text.complete",
                                                        "Auto-complete · suggest and append", {930.0, 132.0});
        (*body).add_at(completion, {24.0, 792.0, 930.0, 132.0});
        (*completion).add_at(label("gallery.text.complete.city.caption", "City", 11.0, 600, muted),
                             {24.0, 42.0, 130.0, 24.0});
        city_ = make_control<TextBox>(StableId("gallery.text.complete.city"));
        (*city_).set_accessible_name("City");
        (*city_).set_placeholder_text("Type a city: Ca, Ed, Lo");
        (*(*city_).auto_complete_custom_source()).add_range(
            {"Calgary", "Cambridge", "Canberra", "Cardiff", "Edinburgh", "Edmonton", "London", "Los Angeles"});
        (*city_).set_auto_complete_source(AutoCompleteSource::custom_source);
        (*city_).set_auto_complete_mode(AutoCompleteMode::suggest_append);
        (*completion).add_at(city_, {154.0, 36.0, 300.0, 36.0});
        (*completion).add_at(label("gallery.text.complete.band.caption", "Band", 11.0, 600, muted),
                             {480.0, 42.0, 70.0, 24.0});
        band_ = make_control<ComboBox>(StableId("gallery.text.complete.band"));
        (*band_).set_accessible_name("Band");
        (*band_).set_items({"Long wave", "Medium wave", "Short wave", "VHF FM", "UHF", "Microwave"});
        (*band_).set_drop_down_style(ComboBoxStyle::drop_down);
        (*band_).set_auto_complete_source(AutoCompleteSource::list_items);
        (*band_).set_auto_complete_mode(AutoCompleteMode::suggest);
        (*completion).add_at(band_, {550.0, 36.0, 300.0, 36.0});
        complete_status_ = label("gallery.text.complete.status",
                                 "Type to see suggestions; Up and Down choose, Enter or Tab accepts, Escape restores",
                                 11.0, 600, green);
        (*completion).add_at(complete_status_, {154.0, 86.0, 700.0, 26.0});
        on((*city_).auto_complete_accepted(), *this, &TextAndInputPage::completion_accepted);
        on((*(*band_).editor()).auto_complete_accepted(), *this, &TextAndInputPage::completion_accepted);
    }

    void completion_accepted(const std::string& text) {
        (*complete_status_).set_text("Accepted \"" + text + "\"");
    }

    void masked_rejected(const MaskInputRejection& rejection) {
        (*masked_status_).set_text("Telephone refused \"" + rejection.text + "\" at position " +
                                   std::to_string(rejection.position.value()) + " · " +
                                   std::string(mask_result_hint_name(rejection.hint)));
    }

    void masked_changed(const std::string& text) {
        (*masked_status_).set_text("Telephone " + text +
                                   ((*phone_).mask_completed() ? " · complete" : " · incomplete"));
    }

    void refresh_state() {
        const TextSelection selection = (*primary_).selection();
        (*state_).set_text("Selection: " + std::to_string(selection.start().value()) + ".." +
                           std::to_string(selection.end().value()) + " bytes · Undo: " +
                           ((*primary_).can_undo() ? "ready" : "clean") + " · Redo: " +
                           ((*primary_).can_redo() ? "ready" : "clean"));
        undo_.set_enabled((*primary_).can_undo());
        redo_.set_enabled((*primary_).can_redo());
    }

    void on_command(const CommandItem& item) {
        switch (item.id) {
        case command_select_all:
            (*primary_).select_all();
            return;
        case command_replace:
            static_cast<void>((*primary_).replace_selection("GUI.Forms"));
            return;
        case command_copy:
            publish_clipboard_status("copy", (*primary_).copy());
            return;
        case command_cut:
            publish_clipboard_status("cut", (*primary_).cut());
            return;
        case command_paste:
            publish_clipboard_status("paste", (*primary_).paste());
            return;
        default:
            return; // Undo and Redo run through their Commands.
        }
    }

    void on_undo() { static_cast<void>((*primary_).undo()); }
    void on_redo() { static_cast<void>((*primary_).redo()); }

    void publish_clipboard_status(const std::string_view command, const bool accepted) {
        (*clipboard_status_).set_text(
            std::string("Clipboard ") + std::string(command) +
            (accepted ? " · accepted" : " · unavailable or empty") + " · " +
            std::to_string((*primary_).text().size()) + " UTF-8 bytes retained");
    }

    std::shared_ptr<TextBox> primary_;
    std::shared_ptr<Label> state_;
    std::shared_ptr<Label> clipboard_status_;
    std::shared_ptr<CommandBar> bar_;
    std::shared_ptr<MaskedTextBox> phone_;
    std::shared_ptr<Label> masked_status_;
    std::shared_ptr<TextBox> city_;
    std::shared_ptr<ComboBox> band_;
    std::shared_ptr<Label> complete_status_;
    Command undo_{"gallery.text.command.undo", "Undo"};
    Command redo_{"gallery.text.command.redo", "Redo"};
};

} // namespace

void add_text_input(const std::shared_ptr<Surface>& page,
                    const std::shared_ptr<PageContext>& context) {
    (*context).components.add(std::make_shared<TextAndInputPage>(page, context));
}

} // namespace gui_forms::gallery::pages
