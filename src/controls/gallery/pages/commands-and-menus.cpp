#include "pages.hpp"

namespace gui_forms::gallery::pages {
namespace {

// CommandBar item ids; the bar names its buttons "gallery.commands.bar.item.<id>".
constexpr int bar_new = 1;
constexpr int bar_save = 2;
constexpr int bar_delete = 3;
constexpr int bar_wrap = 4;

// One fixture document with a modified flag and a word-wrap flag. Every
// surface on the page (MenuStrip, CommandBar, ContextMenu, DropDownButton
// menus, a plain Button, a CheckBox and Ctrl+S) converges on the same
// application-owned Commands, so enabled and checked state is authored once.
class CommandsAndMenusPage final : public Component {
public:
    CommandsAndMenusPage(const std::shared_ptr<Surface>& body,
                         const std::shared_ptr<PageContext>& context)
        : export_menu_("gallery.commands.export.menu"),
          run_menu_("gallery.commands.run.menu"),
          context_menu_("gallery.commands.context") {
        static_cast<void>(context);
        (*body).add_at(label("gallery.commands-and-menus.heading", "COMMANDS AND MENUS", 22.0,
                             700, shell_blue_dark), {24.0, 18.0, 620.0, 34.0});
        (*body).add_at(label("gallery.commands-and-menus.lead",
                             "One Command per action: the menu bar, command bar, context menu, drop-down menus, a button, a check box and Ctrl+S all invoke the same retained authority.",
                             12.0, 400, muted), {26.0, 52.0, 920.0, 24.0});
        build_commands();
        build_menu_strip(body);
        build_command_row(body);
        build_shared_state(body);
        build_context_menu(body);
        build_strips(body);
        build_container(body);
        build_ribbon(body);
        status_ = label("gallery.commands.status",
                        "Ready · toggle \"Document modified\" to enable Save on every surface",
                        11.0, 600, green);
        (*body).add_at(status_, {24.0, 600.0, 930.0, 26.0});
        on(dirty_.changed(), *this, &CommandsAndMenusPage::on_dirty_changed);
        on(wrap_.changed(), *this, &CommandsAndMenusPage::on_wrap_changed);
        on_dirty_changed(dirty_.get());
    }

private:
    [[nodiscard]] static std::shared_ptr<Command> make_command(
        const std::string_view id, const std::string_view text,
        const std::string_view description, const std::string_view shortcut = {}) {
        std::shared_ptr<Command> command =
            std::make_shared<Command>(std::string(id), std::string(text));
        (*command).set_description(std::string(description));
        if (!shortcut.empty()) (*command).set_shortcut(std::string(shortcut));
        return command;
    }

    [[nodiscard]] static MenuItemSpec item(const std::string_view id,
                                           const std::shared_ptr<Command>& command,
                                           const MenuItemKind kind = MenuItemKind::command) {
        return MenuItemSpec(std::string(id), kind, command);
    }

    [[nodiscard]] static MenuItemSpec separator(const std::string_view id) {
        return MenuItemSpec(std::string(id), MenuItemKind::separator);
    }

    [[nodiscard]] std::vector<MenuItemSpec> export_items(const std::string_view prefix) const {
        std::vector<MenuItemSpec> items;
        for (std::size_t index = 0; index < export_formats_.size(); ++index) {
            items.push_back(item(std::string(prefix) + "." + export_names_[index],
                                 export_formats_[index]));
        }
        return items;
    }

    void build_commands() {
        new_ = make_command("gallery.commands.new", "New document", "Start a new fixture document", "Ctrl+N");
        save_ = make_command("gallery.commands.save", "Save", "Save the fixture document", "Ctrl+S");
        delete_ = make_command("gallery.commands.delete", "Delete", "Delete the fixture document");
        (*delete_).set_enabled(false);
        (*delete_).set_destructive(true);
        (*delete_).set_availability_reason("Fixture documents are read-only");
        wrap_command_ = make_command("gallery.commands.wrap", "Word wrap", "Wrap long lines");
        (*wrap_command_).set_checked(false);
        icons_ = make_command("gallery.commands.view.icons", "Icons", "Show documents as icons");
        details_ = make_command("gallery.commands.view.details", "Details", "Show documents as rows");
        (*icons_).set_checked(true);
        (*details_).set_checked(false);
        run_ = make_command("gallery.commands.run", "Run", "Run the fixture document");
        about_ = make_command("gallery.commands.about", "About commands", "Explain this page");
        for (std::size_t index = 0; index < export_names_.size(); ++index) {
            export_formats_[index] = make_command(
                std::string("gallery.commands.export.") + export_names_[index],
                std::string("Export ") + export_labels_[index],
                std::string("Export the document as ") + export_labels_[index]);
            on((*export_formats_[index]).invoked(), *this,
               &CommandsAndMenusPage::on_export, static_cast<int>(index));
        }
        run_options_[0] = make_command("gallery.commands.run.verbose", "Run verbose", "Run with a trace");
        run_options_[1] = make_command("gallery.commands.run.watch", "Run and watch", "Run and re-run on change");
        on((*run_options_[0]).invoked(), *this, &CommandsAndMenusPage::on_run_option, 0);
        on((*run_options_[1]).invoked(), *this, &CommandsAndMenusPage::on_run_option, 1);
        on((*new_).invoked(), *this, &CommandsAndMenusPage::on_new);
        on((*save_).invoked(), *this, &CommandsAndMenusPage::on_save);
        on((*wrap_command_).invoked(), *this, &CommandsAndMenusPage::on_wrap_invoked);
        on((*icons_).invoked(), *this, &CommandsAndMenusPage::on_view_mode, 0);
        on((*details_).invoked(), *this, &CommandsAndMenusPage::on_view_mode, 1);
        on((*run_).invoked(), *this, &CommandsAndMenusPage::on_run);
        on((*about_).invoked(), *this, &CommandsAndMenusPage::on_about);
    }

    void build_menu_strip(const std::shared_ptr<Surface>& body) {
        std::shared_ptr<MenuStrip> menu = make_control<MenuStrip>(StableId("gallery.commands.menu"));
        (*menu).set_font({FontRole::control, 11.5, 400, false, 0.12});
        (*menu).set_accessible_name("Document menu");
        (*menu).set_items({
            {"gallery.commands.menu.file", "&File",
             {item("file.new", new_), item("file.save", save_), separator("file.separator"),
              MenuItemSpec("file.export", MenuItemKind::submenu, {}, "Export",
                           export_items("file.export"))}},
            {"gallery.commands.menu.edit", "&Edit",
             {item("edit.delete", delete_)}},
            {"gallery.commands.menu.view", "&View",
             {item("view.wrap", wrap_command_, MenuItemKind::check), separator("view.separator"),
              item("view.icons", icons_, MenuItemKind::radio),
              item("view.details", details_, MenuItemKind::radio)}},
            {"gallery.commands.menu.help", "&Help", {item("help.about", about_)}},
        });
        (*body).add_at(menu, {24.0, 88.0, 930.0, 28.0});
        on((*menu).item_invoked(), *this, &CommandsAndMenusPage::on_menu_invoked);
    }

    void build_command_row(const std::shared_ptr<Surface>& body) {
        std::shared_ptr<LayoutGroup> row = group(
            "gallery.commands.row", "CommandBar · DropDownButton (menu and split modes)",
            {930.0, 116.0});
        (*body).add_at(row, {24.0, 128.0, 930.0, 116.0});
        std::shared_ptr<CommandBar> bar = make_control<CommandBar>(StableId("gallery.commands.bar"));
        (*bar).set_style(page_style());
        (*bar).set_accessible_name("Document commands");
        (*bar).set_items({
            {.id = bar_new, .label = "New", .accessible_name = "New document",
             .shortcut = KeyGesture{PhysicalKey::n, Modifier::control}, .command = new_.get()},
            {.id = bar_save, .label = "Save", .accessible_name = "Save",
             .shortcut = KeyGesture{PhysicalKey::s, Modifier::control}, .command = save_.get()},
            {.id = bar_delete, .label = "Delete", .accessible_name = "Delete",
             .command = delete_.get()},
            {.id = bar_wrap, .label = "Word wrap", .accessible_name = "Word wrap",
             .command = wrap_command_.get()},
        });
        for (int index = 0; index < static_cast<int>((*bar).items().size()); ++index) {
            Button& button = (*bar).item(index);
            button.set_style(page_style());
            button.set_visual_style(ButtonVisualStyle::command);
        }
        (*row).add_at(bar, {22.0, 36.0, 520.0, 36.0});
        on((*bar).invoked(), *this, &CommandsAndMenusPage::on_bar_invoked);

        export_ = make_control<DropDownButton>(StableId("gallery.commands.export"), "Export");
        (*export_).set_style(page_style());
        (*export_).set_accessible_name("Export document");
        (*row).add_at(export_, {566.0, 36.0, 150.0, 36.0});
        on((*export_).drop_down_requested(), *this, &CommandsAndMenusPage::on_export_requested);
        on((*export_).drop_down_close_requested(), *this, &CommandsAndMenusPage::on_export_close);
        export_menu_.set_items(export_items("export"));
        on(export_menu_.open_changed(), *this, &CommandsAndMenusPage::on_export_menu_open);

        run_button_ = make_control<DropDownButton>(StableId("gallery.commands.run"), "Run",
                                                   DropDownButtonMode::split);
        (*run_button_).set_style(page_style());
        (*run_button_).set_accessible_name("Run document");
        (*run_button_).bind(*run_); // Primary region: the ordinary Click path.
        (*row).add_at(run_button_, {732.0, 36.0, 170.0, 36.0});
        on((*run_button_).drop_down_requested(), *this, &CommandsAndMenusPage::on_run_requested);
        on((*run_button_).drop_down_close_requested(), *this, &CommandsAndMenusPage::on_run_close);
        run_menu_.set_items({item("run.verbose", run_options_[0]), item("run.watch", run_options_[1])});
        on(run_menu_.open_changed(), *this, &CommandsAndMenusPage::on_run_menu_open);

        (*row).add_at(label("gallery.commands.row.note",
                            "Ctrl+N and Ctrl+S are CommandBar shortcuts · Arrow keys rove the bar · Alt+Down opens a focused drop-down · the split button keeps a primary click region",
                            10.0, 400, muted), {22.0, 80.0, 890.0, 20.0});
    }

    void build_shared_state(const std::shared_ptr<Surface>& body) {
        std::shared_ptr<LayoutGroup> state = group(
            "gallery.commands.state", "Command · one authority, projected everywhere",
            {450.0, 336.0});
        (*body).add_at(state, {24.0, 256.0, 450.0, 336.0});
        std::shared_ptr<CheckBox> modified = make_control<CheckBox>(
            StableId("gallery.commands.modified"), "Document modified (enables Save)");
        (*modified).set_indicator_style(ChoiceIndicatorStyle::modern);
        (*modified).bind(dirty_);
        (*state).add_at(modified, {22.0, 36.0, 400.0, 26.0});
        std::shared_ptr<Button> save = make_control<Button>(
            StableId("gallery.commands.save.button"), "Save");
        (*save).set_visual_style(ButtonVisualStyle::accent);
        (*save).set_style(page_style());
        (*save).bind(*save_);
        (*state).add_at(save, {22.0, 74.0, 150.0, 34.0});
        std::shared_ptr<Button> remove = make_control<Button>(
            StableId("gallery.commands.delete.button"), "Delete");
        (*remove).set_style(page_style());
        (*remove).bind(*delete_);
        (*state).add_at(remove, {186.0, 74.0, 150.0, 34.0});
        std::shared_ptr<CheckBox> wrap = make_control<CheckBox>(
            StableId("gallery.commands.wrap.check"), "Word wrap");
        (*wrap).set_indicator_style(ChoiceIndicatorStyle::toggle);
        (*wrap).bind(wrap_);
        (*state).add_at(wrap, {22.0, 120.0, 300.0, 26.0});
        saves_label_ = label("gallery.commands.saves", "Saves: 0", 11.0, 600, shell_blue_dark);
        (*state).add_at(saves_label_, {22.0, 156.0, 400.0, 22.0});
        std::shared_ptr<Label> note = label(
            "gallery.commands.state.note",
            "Save is one Command: the bar item, File > Save, the context-menu row, this button and Ctrl+S invoke it, and its enabled state follows the modified flag. Delete is disabled with an availability reason. Word wrap is a Value bound to the check box and projected onto the check Command, so the bar item, the View menu and the context menu stay in step.",
            10.0, 400, muted);
        (*note).set_text_wrapping(TextWrapping::word);
        (*note).set_vertical_alignment(VerticalAlignment::near);
        (*state).add_at(note, {22.0, 186.0, 400.0, 136.0});
    }

    void build_context_menu(const std::shared_ptr<Surface>& body) {
        std::shared_ptr<LayoutGroup> context_group = group(
            "gallery.commands.context-group", "ContextMenu · stable rows over bound Commands",
            {460.0, 336.0});
        (*body).add_at(context_group, {494.0, 256.0, 460.0, 336.0});
        context_button_ = make_control<Button>(
            StableId("gallery.commands.context.open"), "Open context menu");
        (*context_button_).set_style(page_style());
        (*context_group).add_at(context_button_, {22.0, 36.0, 240.0, 34.0});
        on((*context_button_).clicked(), *this, &CommandsAndMenusPage::on_context_requested);
        context_menu_.set_items({
            item("save", save_), item("delete", delete_), separator("separator.edit"),
            item("wrap", wrap_command_, MenuItemKind::check), separator("separator.view"),
            MenuItemSpec("export", MenuItemKind::submenu, {}, "Export", export_items("export")),
            separator("separator.help"), item("about", about_),
        });
        on(context_menu_.item_invoked(), *this, &CommandsAndMenusPage::on_context_invoked);
        on(context_menu_.open_changed(), *this, &CommandsAndMenusPage::on_context_open);
        std::shared_ptr<Label> note = label(
            "gallery.commands.context.note",
            "Rows are temporary popup presentations over stable MenuItemSpec identities. Arrow keys and Enter invoke; Escape and click-away revoke the popup and restore the invoker's focus. Command state (enabled, checked, destructive, availability reason) is snapshotted as the menu opens.",
            10.0, 400, muted);
        (*note).set_text_wrapping(TextWrapping::word);
        (*note).set_vertical_alignment(VerticalAlignment::near);
        (*context_group).add_at(note, {22.0, 84.0, 410.0, 120.0});
    }

    // A ToolStrip over the same Commands with a hosted combo box, narrow
    // enough that its last buttons overflow, and a StatusStrip with a spring
    // label and a hosted progress bar.
    void build_strips(const std::shared_ptr<Surface>& body) {
        std::shared_ptr<LayoutGroup> strips = group(
            "gallery.commands.strips", "ToolStrip · StatusStrip · by-role painted items", {930.0, 132.0});
        (*body).add_at(strips, {24.0, 636.0, 930.0, 132.0});
        std::shared_ptr<ToolStrip> tool = make_control<ToolStrip>(StableId("gallery.commands.tool-strip"));
        (*tool).set_accessible_name("Document tools");
        std::vector<ToolStripItemSpec> tools;
        tools.push_back({.stable_id = "strip.new", .command = new_});
        tools.push_back({.stable_id = "strip.save", .command = save_});
        tools.push_back({.stable_id = "strip.delete", .command = delete_});
        tools.push_back({.stable_id = "strip.separator", .kind = ToolStripItemKind::separator});
        tools.push_back({.stable_id = "strip.wrap", .command = wrap_command_});
        tools.push_back({.stable_id = "strip.zoom", .kind = ToolStripItemKind::combo_box, .text = "Zoom",
                         .choices = {"50%", "100%", "200%"}, .width = 72.0});
        tools.push_back({.stable_id = "strip.export", .kind = ToolStripItemKind::drop_down_button,
                         .text = "Export", .drop_down = export_items("strip.export")});
        tools.push_back({.stable_id = "strip.run", .kind = ToolStripItemKind::split_button,
                         .command = run_,
                         .drop_down = {item("strip.run.verbose", run_options_[0]),
                                       item("strip.run.watch", run_options_[1])}});
        tools.push_back({.stable_id = "strip.about", .command = about_});
        (*tool).set_items(std::move(tools));
        (*strips).add_at(tool, {22.0, 36.0, 430.0, 27.0});
        std::shared_ptr<StatusStrip> status = make_control<StatusStrip>(StableId("gallery.commands.status-strip"));
        (*status).set_accessible_name("Document status");
        (*status).set_items({
            {.stable_id = "status.state", .kind = ToolStripItemKind::label, .text = "Ready", .spring = true},
            {.stable_id = "status.separator", .kind = ToolStripItemKind::separator},
            {.stable_id = "status.position", .kind = ToolStripItemKind::label, .text = "Ln 1, Col 1"},
            {.stable_id = "status.encoding", .kind = ToolStripItemKind::label, .text = "UTF-8"},
            {.stable_id = "status.progress", .kind = ToolStripItemKind::progress_bar, .text = "Indexing"},
        });
        (*(*status).progress_bar("status.progress")).set_value(60.0);
        (*strips).add_at(status, {22.0, 84.0, 880.0, 24.0});
    }

    // A ToolStripContainer: a strip in the top panel, a vertical strip in
    // the left panel and a status strip in the bottom panel around content.
    void build_container(const std::shared_ptr<Surface>& body) {
        std::shared_ptr<LayoutGroup> frame_group = group(
            "gallery.commands.container", "ToolStripContainer · top, left and bottom panels around content",
            {930.0, 230.0});
        (*body).add_at(frame_group, {24.0, 780.0, 930.0, 230.0});
        std::shared_ptr<ToolStripContainer> frame =
            make_control<ToolStripContainer>(StableId("gallery.commands.container.frame"));
        (*frame).set_accessible_name("Drawing frame");
        std::shared_ptr<ToolStrip> top = make_control<ToolStrip>(StableId("gallery.commands.container.top-strip"));
        (*top).set_accessible_name("File tools");
        (*top).set_items({
            {.stable_id = "frame.new", .command = new_},
            {.stable_id = "frame.save", .command = save_},
            {.stable_id = "frame.rule", .kind = ToolStripItemKind::separator},
            {.stable_id = "frame.find", .kind = ToolStripItemKind::text_box, .text = "Find"},
        });
        (*(*frame).top_panel()).join(top);
        std::shared_ptr<ToolStrip> left = make_control<ToolStrip>(StableId("gallery.commands.container.left-strip"));
        (*left).set_accessible_name("Drawing tools");
        (*left).set_items({
            {.stable_id = "frame.pen", .text = "Pen", .check_on_click = true, .checked = true},
            {.stable_id = "frame.brush", .text = "Brush", .check_on_click = true},
            {.stable_id = "frame.tools-rule", .kind = ToolStripItemKind::separator},
            {.stable_id = "frame.fill", .text = "Fill"},
        });
        (*(*frame).left_panel()).join(left);
        std::shared_ptr<StatusStrip> status =
            make_control<StatusStrip>(StableId("gallery.commands.container.status-strip"));
        (*status).set_accessible_name("Drawing status");
        (*status).set_items({{.stable_id = "frame.status", .kind = ToolStripItemKind::label,
                              .text = "Content panel between the strips", .spring = true}});
        (*(*frame).bottom_panel()).join(status);
        std::shared_ptr<Label> content = label("gallery.commands.container.content",
                                               "Content: the application's controls go in content_panel().", 11.0,
                                               400, muted);
        (*content).set_dock(DockStyle::fill);
        (*(*frame).content_panel()).add_child(content);
        (*frame_group).add_at(frame, {22.0, 36.0, 880.0, 176.0});
    }

    // A Ribbon over the same Commands (stage RB, phase 1): Home with a
    // launcher, a large split, small commands and a toggle; Insert with
    // hosted controls; the File tab's application menu.
    void build_ribbon(const std::shared_ptr<Surface>& body) {
        std::shared_ptr<LayoutGroup> ribbon_group = group(
            "gallery.commands.ribbon", "Ribbon · phase 1: tabs, groups, launchers, hosted controls", {930.0, 160.0});
        (*body).add_at(ribbon_group, {24.0, 1030.0, 930.0, 160.0});
        std::shared_ptr<Ribbon> ribbon = make_control<Ribbon>(StableId("gallery.commands.ribbon.bar"));
        (*ribbon).set_accessible_name("Document ribbon");
        std::vector<RibbonTabSpec> tabs;
        RibbonTabSpec home{.stable_id = "ribbon.home", .text = "&Home"};
        RibbonGroupSpec document{.stable_id = "ribbon.document", .caption = "Document", .dialog_launcher = true};
        document.commands.push_back({.stable_id = "ribbon.new", .command = new_});
        document.commands.push_back({.stable_id = "ribbon.save", .kind = RibbonCommandKind::split, .command = save_,
                                     .drop_down = export_items("ribbon.save.export")});
        document.commands.push_back({.stable_id = "ribbon.delete", .size = RibbonCommandSize::small,
                                     .command = delete_});
        document.commands.push_back({.stable_id = "ribbon.wrap", .kind = RibbonCommandKind::toggle,
                                     .size = RibbonCommandSize::small, .command = wrap_command_});
        document.commands.push_back({.stable_id = "ribbon.export", .kind = RibbonCommandKind::drop_down,
                                     .size = RibbonCommandSize::small, .text = "Export",
                                     .drop_down = export_items("ribbon.export")});
        RibbonGroupSpec view{.stable_id = "ribbon.view", .caption = "View"};
        view.commands.push_back({.stable_id = "ribbon.icons", .size = RibbonCommandSize::small, .command = icons_});
        view.commands.push_back({.stable_id = "ribbon.details", .size = RibbonCommandSize::small,
                                 .command = details_});
        view.commands.push_back({.stable_id = "ribbon.zoom", .kind = RibbonCommandKind::combo_box, .text = "Zoom",
                                 .choices = {"50%", "100%", "200%"}, .width = 80.0});
        RibbonGroupSpec run{.stable_id = "ribbon.run-group", .caption = "Run"};
        run.commands.push_back({.stable_id = "ribbon.run", .kind = RibbonCommandKind::split, .command = run_,
                                .drop_down = {item("ribbon.run.verbose", run_options_[0]),
                                              item("ribbon.run.watch", run_options_[1])}});
        home.groups.push_back(std::move(document));
        home.groups.push_back(std::move(view));
        home.groups.push_back(std::move(run));
        RibbonTabSpec insert{.stable_id = "ribbon.insert", .text = "&Insert"};
        RibbonGroupSpec find{.stable_id = "ribbon.find-group", .caption = "Find", .dialog_launcher = true};
        find.commands.push_back({.stable_id = "ribbon.find", .kind = RibbonCommandKind::text_box, .text = "Find"});
        find.commands.push_back({.stable_id = "ribbon.matches", .kind = RibbonCommandKind::numeric_up_down,
                                 .text = "Matches"});
        find.commands.push_back({.stable_id = "ribbon.case", .kind = RibbonCommandKind::check_box,
                                 .text = "Match case"});
        insert.groups.push_back(std::move(find));
        tabs.push_back(std::move(home));
        tabs.push_back(std::move(insert));
        (*ribbon).set_tabs(std::move(tabs));
        (*ribbon).set_application_menu({item("ribbon.file.new", new_), item("ribbon.file.save", save_),
                                        separator("ribbon.file.separator"), item("ribbon.file.about", about_)});
        on((*ribbon).dialog_launcher_clicked(), *this, &CommandsAndMenusPage::on_ribbon_launcher);
        (*ribbon_group).add_at(ribbon, {22.0, 36.0, 880.0, 112.0});
    }

    void on_ribbon_launcher(const RibbonDialogLaunch& launch) {
        (*status_).set_text("Ribbon dialog launcher · " + launch.group_id);
    }

    // Shared application state.
    void on_dirty_changed(const bool dirty) {
        (*save_).set_enabled(dirty);
        (*status_).set_text(dirty ? "Document modified · Save enabled on the bar, the menus, the button and Ctrl+S"
                                  : "Document clean · Save disabled everywhere");
    }

    void on_wrap_changed(const bool wrap) {
        (*wrap_command_).set_checked(wrap);
        (*status_).set_text(std::string("Word wrap ") + (wrap ? "on" : "off") +
                            " · Value projected onto the check Command");
    }

    // Command handlers; CommandInvocation names the surface that invoked.
    void on_new(const CommandInvocation& invocation) {
        dirty_.set(true);
        (*status_).set_text("New document · invoked from " + invocation.source_id);
    }

    void on_save(const CommandInvocation& invocation) {
        ++saves_;
        (*saves_label_).set_text("Saves: " + std::to_string(saves_));
        dirty_.set(false);
        (*status_).set_text("Save #" + std::to_string(saves_) + " invoked from " +
                            invocation.source_id + " · document clean again");
    }

    void on_wrap_invoked(const CommandInvocation& invocation) {
        wrap_.set(!wrap_.get());
        (*status_).set_text(std::string("Word wrap ") + (wrap_.get() ? "on" : "off") +
                            " · toggled from " + invocation.source_id);
    }

    void on_view_mode(const int mode, const CommandInvocation& invocation) {
        (*icons_).set_checked(mode == 0);
        (*details_).set_checked(mode == 1);
        (*status_).set_text(std::string("View ") + (mode == 0 ? "icons" : "details") +
                            " · radio Commands checked from " + invocation.source_id);
    }

    void on_run(const CommandInvocation& invocation) {
        (*status_).set_text("Run · split button primary region · " + invocation.source_id);
    }

    void on_run_option(const int option, const CommandInvocation& invocation) {
        (*status_).set_text(std::string(option == 0 ? "Run verbose" : "Run and watch") +
                            " · split button menu · " + invocation.source_id);
    }

    void on_export(const int format, const CommandInvocation& invocation) {
        (*status_).set_text(std::string("Export ") + export_labels_[static_cast<std::size_t>(format)] +
                            " · invoked from " + invocation.source_id);
    }

    void on_about() {
        (*status_).set_text("Commands and menus · MenuStrip, CommandBar, ContextMenu and "
                            "DropDownButton all execute shared Command instances");
    }

    // Surface notifications: group events that survive item replacement.
    // The bound Command has already run; the bar reports the item afterwards.
    void on_bar_invoked(const CommandItem& bar_item) {
        (*status_).set_text((*status_).text() + " · bar item " + bar_item.label);
    }

    void on_menu_invoked(const MenuStripInvocation& invocation) {
        (*status_).set_text((*status_).text() + " · menu " + invocation.top_level_id);
    }

    void on_context_invoked(const MenuItemInvocation& invocation) {
        (*status_).set_text((*status_).text() + " · context row " + invocation.item_id);
    }

    void on_context_open(const bool open) {
        if (!open) (*status_).set_text("Context menu closed · invoker focus restored");
    }

    void on_context_requested(ButtonBase& source) {
        const Rect bounds = source.absolute_bounds();
        context_menu_.show(source.shared_from_this(), {bounds.x + 12.0, bounds.y + bounds.height},
                           MenuOpenMode::keyboard);
    }

    // Drop-down buttons own the disclosure state; the menus report open/close.
    void on_export_requested(DropDownButton& source) {
        const Rect bounds = source.absolute_bounds();
        export_menu_.show(source.shared_from_this(), {bounds.x, bounds.y + bounds.height},
                          MenuOpenMode::pointer);
    }

    void on_export_close() { export_menu_.close(); }

    void on_export_menu_open(const bool open) { (*export_).set_drop_down_open(open); }

    void on_run_requested(DropDownButton& source) {
        const Rect bounds = source.absolute_bounds();
        run_menu_.show(source.shared_from_this(), {bounds.x, bounds.y + bounds.height},
                       MenuOpenMode::pointer);
    }

    void on_run_close() { run_menu_.close(); }

    void on_run_menu_open(const bool open) { (*run_button_).set_drop_down_open(open); }

    static constexpr std::array<const char*, 3> export_names_{"pdf", "png", "text"};
    static constexpr std::array<const char*, 3> export_labels_{"PDF", "PNG", "plain text"};

    std::shared_ptr<Command> new_;
    std::shared_ptr<Command> save_;
    std::shared_ptr<Command> delete_;
    std::shared_ptr<Command> wrap_command_;
    std::shared_ptr<Command> icons_;
    std::shared_ptr<Command> details_;
    std::shared_ptr<Command> run_;
    std::shared_ptr<Command> about_;
    std::array<std::shared_ptr<Command>, 3> export_formats_;
    std::array<std::shared_ptr<Command>, 2> run_options_;
    ContextMenu export_menu_;
    ContextMenu run_menu_;
    ContextMenu context_menu_;
    std::shared_ptr<DropDownButton> export_;
    std::shared_ptr<DropDownButton> run_button_;
    std::shared_ptr<Button> context_button_;
    std::shared_ptr<Label> status_;
    std::shared_ptr<Label> saves_label_;
    Value<bool> dirty_{false};
    Value<bool> wrap_{false};
    unsigned saves_{};
};

} // namespace

void add_commands_and_menus(const std::shared_ptr<Surface>& page,
                            const std::shared_ptr<PageContext>& context) {
    (*context).components.add(std::make_shared<CommandsAndMenusPage>(page, context));
}

} // namespace gui_forms::gallery::pages
