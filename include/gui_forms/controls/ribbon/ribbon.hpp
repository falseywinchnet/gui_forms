#pragma once

#include "gui_forms/commands.hpp"
#include "gui_forms/components/context_menu/context_menu.hpp"
#include "gui_forms/control.hpp"
#include "gui_forms/controls/button_base/check_box/check_box.hpp"
#include "gui_forms/controls/panel/combo_box/combo_box.hpp"
#include "gui_forms/controls/panel/numeric_up_down/numeric_up_down.hpp"
#include "gui_forms/controls/panel/text_box/text_box.hpp"
#include "gui_forms/event.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace gui_forms {

// The kinds of ribbon command (stage RB, owner decision 2: commands are
// records in a command item system, hosted editors are child controls).
// - button runs its Command;
// - toggle flips its Command's checked state, then runs it;
// - split runs its Command from its face and opens its menu from its arrow;
// - drop_down opens its menu;
// - combo_box, numeric_up_down, check_box and text_box hold a real
//   ComboBox, NumericUpDown, CheckBox or TextBox child control.
enum class RibbonCommandKind : std::uint8_t {
    button,
    toggle,
    split,
    drop_down,
    combo_box,
    numeric_up_down,
    check_box,
    text_box,
};

// A button-family command's size as authored: large (a 32-unit icon over its
// label, the group's full height) or small (a 16-unit icon beside its label,
// three to a column). Phase 1 shows every group at its largest size; phase 2
// adds scaling.
enum class RibbonCommandSize : std::uint8_t { large, small };

// One command record. `stable_id` is unique within the ribbon (across tabs,
// groups and commands) and names the command in events, semantics and the
// published anatomy.
//
// A button, toggle or split takes its text (when `text` is empty), enabled,
// visible and checked state from its required `command`, and runs it on a
// click; a drop-down's command is optional. `text` may carry a mnemonic
// ("&Paste"). Labels are always shown (owner decision 6); a small command
// with an image may leave its label out (`show_label` false: an authored
// icon-only command, decision 30). `drop_down` is the menu of a split or
// drop-down command. A hosted command's `text` is its control's accessible
// name (and a check box's caption); `choices` are a combo box's items and
// `width` its width in logical units (zero for the default).
struct RibbonCommandSpec final {
    std::string stable_id{};
    RibbonCommandKind kind{RibbonCommandKind::button};
    RibbonCommandSize size{RibbonCommandSize::large};
    std::shared_ptr<Command> command{};
    std::string text{};
    ImageId image{};
    bool show_label{true};
    std::vector<MenuItemSpec> drop_down{};
    std::vector<std::string> choices{};
    double width{};
};

// A group of commands on a tab, with a caption beneath and, when
// `dialog_launcher` is set, a launcher button in the caption's corner.
struct RibbonGroupSpec final {
    std::string stable_id{};
    std::string caption{};
    bool dialog_launcher{};
    std::vector<RibbonCommandSpec> commands{};
};

// A core tab. `text` may carry a mnemonic ("&Home").
struct RibbonTabSpec final {
    std::string stable_id{};
    std::string text{};
    std::vector<RibbonGroupSpec> groups{};
};

// A click on a button-family command (or its split face), after its Command
// ran.
struct RibbonCommandClick final {
    std::string ribbon_id{};
    std::string command_id{};
};

// A choice from a split or drop-down command's menu.
struct RibbonDropDownInvocation final {
    std::string ribbon_id{};
    std::string command_id{};
    MenuItemInvocation choice{};
};

// A click on a group's dialog launcher.
struct RibbonDialogLaunch final {
    std::string ribbon_id{};
    std::string group_id{};
};

// The selected tab after it changed.
struct RibbonTabChange final {
    std::string ribbon_id{};
    std::size_t tab{};
    std::string tab_id{};
};

// A ribbon (stage RB, phase 1): the File tab and the core tabs on a tab row
// that is always visible, and the command plane of the selected tab with its
// groups, captions, dividers, dialog launchers and commands. Clicking the
// selected tab minimizes the command plane or shows it again (owner decision
// 19); clicking another tab selects it and shows the plane. The File tab
// opens the application menu over the ribbon (decisions 3, 23). Commands are
// records bound to shared Commands, laid out as the "command" item system;
// hosted editors are child controls placed by the ribbon's layout. Every
// part is painted through the by-role paint path.
//
// Phase 1 shows every group at its largest size; groups past the right edge
// are clipped (scaling is phase 2). Not yet: contextual tabs, galleries, the
// Quick Access Toolbar, the minimized popup, enhanced tooltips and Backstage.
class Ribbon final : public Control {
public:
    // Development bounds, not measured capacity.
    static constexpr std::size_t maximum_tabs = 32U;
    static constexpr std::size_t maximum_groups_per_tab = 32U;
    static constexpr std::size_t maximum_commands_per_group = 64U;

    explicit Ribbon(StableId stable_id);
    ~Ribbon() override;

    [[nodiscard]] std::span<const RibbonTabSpec> tabs() const noexcept { return tabs_; }
    // Replaces the tabs, keeping the selection on the tab with the same ID
    // (or the first). Throws std::invalid_argument for an empty or repeated
    // ID (IDs are one namespace across tabs, groups and commands), invalid
    // UTF-8, a button, toggle or split without a command, a menu on a kind
    // that has none or a split without one, an icon-only command that is not
    // small or has no image, choices on a command that is not a combo box, a
    // width outside 0 to 1024, an invalid menu, or more than the bounds.
    void set_tabs(std::vector<RibbonTabSpec> tabs);

    [[nodiscard]] std::optional<std::size_t> selected_tab() const noexcept { return selected_tab_; }
    // Throws std::out_of_range for a tab past the end.
    void set_selected_tab(std::size_t tab);
    // Whether the command plane is hidden, leaving the tab row.
    [[nodiscard]] bool minimized() const noexcept { return minimized_; }
    void set_minimized(bool minimized);

    // The File tab: its text (with an optional mnemonic) and its menu.
    [[nodiscard]] const std::string& application_text() const noexcept { return application_text_; }
    void set_application_text(std::string text);
    [[nodiscard]] const std::vector<MenuItemSpec>& application_menu() const noexcept { return application_menu_; }
    // Throws std::invalid_argument for an invalid menu.
    void set_application_menu(std::vector<MenuItemSpec> items);
    // Opens the application menu below the File tab; false without items or
    // a window.
    bool show_application_menu(MenuOpenMode mode = MenuOpenMode::keyboard);
    // Opens a split or drop-down command's menu; false for another kind, a
    // disabled command or one not on the selected tab.
    bool show_drop_down(std::string_view command_id, MenuOpenMode mode = MenuOpenMode::keyboard);
    void close_menus() noexcept;
    [[nodiscard]] bool menu_open() const noexcept;

    // The hosted control of a command. Throws std::out_of_range for an ID
    // that names no command of that kind.
    [[nodiscard]] std::shared_ptr<ComboBox> combo_box(std::string_view command_id) const;
    [[nodiscard]] std::shared_ptr<NumericUpDown> numeric_up_down(std::string_view command_id) const;
    [[nodiscard]] std::shared_ptr<CheckBox> check_box(std::string_view command_id) const;
    [[nodiscard]] std::shared_ptr<TextBox> text_box(std::string_view command_id) const;

    // Whether the ribbon adds itself to its window's F6 regions when it is
    // attached (owner decision 17). On by default.
    [[nodiscard]] bool focus_region() const noexcept { return focus_region_; }
    void set_focus_region(bool region);

    [[nodiscard]] FontSpec font() const noexcept { return font_; }
    void set_font(FontSpec font);

    [[nodiscard]] Event<const RibbonCommandClick&>& command_clicked() noexcept { return command_clicked_; }
    [[nodiscard]] Event<const RibbonDropDownInvocation&>& drop_down_item_invoked() noexcept {
        return drop_down_item_invoked_;
    }
    [[nodiscard]] Event<const RibbonDialogLaunch&>& dialog_launcher_clicked() noexcept {
        return dialog_launcher_clicked_;
    }
    [[nodiscard]] Event<const RibbonTabChange&>& selected_tab_changed() noexcept { return selected_tab_changed_; }
    [[nodiscard]] Event<bool>& minimized_changed() noexcept { return minimized_changed_; }
    [[nodiscard]] Event<const MenuItemInvocation&>& application_menu_item_invoked() noexcept {
        return application_menu_item_invoked_;
    }

    [[nodiscard]] Size measure(Size available) override;
    void arrange(Rect final_bounds) override;
    void on_paint(Painter& painter, Rect local_damage) override;
    void on_pointer(PointerEvent& event) override;
    void on_key(KeyEvent& event) override;
    void on_focus_changed(bool focused) override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;
    [[nodiscard]] std::vector<SemanticNode> semantic_virtual_children() const override;
    bool on_semantic_child_action(std::string_view stable_id, SemanticAction action,
                                  std::string_view value) override;
    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;
    [[nodiscard]] std::size_t realized_item_count() const override;
    [[nodiscard]] std::string realized_item_stable_id(std::size_t item) const override;
    [[nodiscard]] AnatomyStateSet realized_item_anatomy_states(std::size_t item) const override;

    // Stage T1 anatomy, as dot members; its states are Control's.
    struct Parts final {
        PartGeometry region;
        PartGeometry tab_rail;
        PartGeometry application_button;
        PartGeometry application_label;
        PartGeometry command_plane;
        PartGeometry focus;
    };
    [[nodiscard]] Parts parts() const;

    // The "tab" item system: one core tab on the tab row.
    struct TabParts final {
        PartGeometry tab;
        PartGeometry tab_label;
        PartGeometry tab_focus;
    };
    struct TabStates final {
        bool hot{};
        bool selected{};
        bool focus_visible{};
    };
    struct Tab final {
        std::size_t index{};
        std::string stable_id;
        TabParts parts;
        TabStates states;
    };
    [[nodiscard]] std::vector<Tab> realized_tabs() const;

    // The "group" item system: one group of the selected tab.
    struct GroupParts final {
        PartGeometry group_region;
        PartGeometry group_caption;
        PartGeometry group_divider;
        PartGeometry group_launcher;
        PartGeometry group_launcher_glyph;
        PartGeometry group_launcher_focus;
    };
    struct Group final {
        std::size_t index{};
        std::string stable_id;
        GroupParts parts;
    };
    [[nodiscard]] std::vector<Group> realized_groups() const;

    // The "command" item system: one command of the selected tab.
    struct CommandParts final {
        PartGeometry command_face;
        PartGeometry command_icon;
        PartGeometry command_label;
        PartGeometry command_arrow;
        PartGeometry command_arrow_glyph;
        PartGeometry command_focus;
        PartGeometry command_combo_box;
        PartGeometry command_numeric_up_down;
        PartGeometry command_check_box;
        PartGeometry command_text_box;
    };
    struct CommandStates final {
        bool hot{};
        bool pressed{};
        bool checked{};
        bool open{};
        bool disabled{};
        bool focus_visible{};
    };
    struct CommandItem final {
        std::size_t group{};
        std::size_t index{};
        std::string stable_id;
        CommandParts parts;
        CommandStates states;
    };
    [[nodiscard]] std::vector<CommandItem> realized_commands() const;

protected:
    void append_interactions(std::vector<InteractionDeclaration>& interactions) const override;
    void on_attachment_committed() noexcept override;
    void on_dispose() noexcept override;
    [[nodiscard]] bool mnemonic_matches(char32_t character) const noexcept override;
    bool process_mnemonic_self(char32_t character) override;
    void publish_part_geometry(std::span<PartGeometry> geometry) const override;
    [[nodiscard]] ItemSystemIndex publish_realized_item_system(std::size_t item) const override;
    void publish_realized_item_geometry(std::size_t item, std::span<PartGeometry> geometry) const override;

private:
    // A command's position in the selected tab: its group and its place.
    struct CommandAddress final {
        std::size_t group{};
        std::size_t command{};

        friend constexpr bool operator==(CommandAddress left, CommandAddress right) noexcept {
            return left.group == right.group && left.command == right.command;
        }
    };
    // Which part of a command a point or a press is on.
    enum class CommandZone : std::uint8_t { none, main, arrow };
    // A keyboard stop: the File tab, a tab, a command of the selected tab,
    // or a group's dialog launcher.
    enum class StopKind : std::uint8_t { application, tab, command, launcher };
    struct Stop final {
        StopKind kind{StopKind::tab};
        std::size_t index{};
        CommandAddress command{};

        friend constexpr bool operator==(const Stop& left, const Stop& right) noexcept {
            return left.kind == right.kind && left.index == right.index && left.command == right.command;
        }
    };
    // What a point is on.
    struct Hit final {
        enum class Kind : std::uint8_t { none, application, tab, command, launcher } kind{Kind::none};
        std::size_t index{};
        CommandAddress command{};
        CommandZone zone{CommandZone::none};
    };
    // Where everything sits, from one layout pass over the selected tab.
    struct Layout final {
        Rect tab_rail{};
        Rect application{};
        std::vector<Rect> tabs;
        Rect plane{};
        std::vector<Rect> groups;                 // per group of the selected tab
        std::vector<std::vector<Rect>> commands;  // per group, per command
    };
    // A command's hosted control and the command watch subscription.
    struct CommandRecord final {
        Control::Ptr hosted{};
        SubscriptionToken watch{};
        // The command's last text and visibility, to tell a change that
        // needs layout from one that needs only a repaint.
        std::string text{};
        bool visible{true};
    };
    // A named callback: a Command's state changed. `tab`, `group` and
    // `command` locate the record.
    struct CommandWatch final {
        Ribbon* ribbon{};
        std::size_t tab{};
        std::size_t group{};
        std::size_t command{};
        void operator()(const CommandState&) const;
    };

    // A realized item located: a tab, a group or a command of the selected
    // tab, or nothing past the end.
    struct RealizedItem final {
        enum class Kind : std::uint8_t { none, tab, group, command } kind{Kind::none};
        std::size_t index{};
        CommandAddress command{};
    };
    [[nodiscard]] RealizedItem locate_realized(const Layout& placed, std::size_t item) const;
    [[nodiscard]] Layout layout() const;
    [[nodiscard]] Layout layout_for(Rect client) const;
    [[nodiscard]] double tab_row_height() const noexcept;
    [[nodiscard]] double plane_height() const noexcept;
    [[nodiscard]] double text_width(std::string_view text) const;
    [[nodiscard]] Size command_size(const RibbonCommandSpec& command) const;
    [[nodiscard]] TabParts tab_parts(const Layout& placed, std::size_t tab) const;
    [[nodiscard]] GroupParts group_parts(const Layout& placed, std::size_t group) const;
    [[nodiscard]] CommandParts command_parts(const Layout& placed, CommandAddress address) const;
    [[nodiscard]] AnatomyStateSet tab_states(std::size_t tab) const;
    [[nodiscard]] AnatomyStateSet command_states(CommandAddress address) const;
    [[nodiscard]] const RibbonGroupSpec* selected_group(std::size_t group) const noexcept;
    [[nodiscard]] const RibbonCommandSpec* selected_command(CommandAddress address) const noexcept;
    [[nodiscard]] const std::string& authored_text(const RibbonCommandSpec& command) const noexcept;
    [[nodiscard]] std::string command_text(const RibbonCommandSpec& command) const;
    [[nodiscard]] bool command_enabled(const RibbonCommandSpec& command) const;
    [[nodiscard]] bool command_visible(const RibbonCommandSpec& command) const;
    [[nodiscard]] bool command_checked(const RibbonCommandSpec& command) const;
    [[nodiscard]] Hit hit_test(const Layout& placed, Point local) const;
    [[nodiscard]] std::vector<Stop> stops() const;
    [[nodiscard]] std::optional<CommandAddress> find_command(std::string_view command_id) const;
    [[nodiscard]] Control::Ptr hosted_control(std::string_view command_id, RibbonCommandKind kind) const;
    [[nodiscard]] Control::Ptr make_hosted(const RibbonCommandSpec& command) const;
    void configure_hosted(const RibbonCommandSpec& command, Control& control) const;

    void select_tab(std::size_t tab);
    void click_tab(std::size_t tab);
    void click_command(CommandAddress address);
    void launch(std::size_t group);
    void set_hot(const Hit& hit);
    void set_stop(std::optional<Stop> stop);
    void activate_stop(const Stop& stop);
    void command_state_changed(std::size_t tab, std::size_t group, std::size_t command);
    void on_popup_item_invoked(const MenuItemInvocation& invocation);
    void on_popup_open_changed(bool open);

    std::vector<RibbonTabSpec> tabs_{};
    // One record per command, indexed [tab][group][command] as tabs_.
    std::vector<std::vector<std::vector<CommandRecord>>> records_{};
    std::optional<std::size_t> selected_tab_{};
    bool minimized_{};
    std::string application_text_{"File"};
    std::vector<MenuItemSpec> application_menu_{};
    std::unique_ptr<ContextMenu> popup_{};
    SubscriptionToken popup_invoked_{};
    SubscriptionToken popup_changed_{};
    // Which menu the popup shows: the application menu, or a command's.
    bool application_open_{};
    std::optional<CommandAddress> open_command_{};
    Hit hot_{};
    std::optional<CommandAddress> pressed_command_{};
    CommandZone pressed_zone_{CommandZone::none};
    std::optional<std::size_t> pressed_launcher_{};
    bool application_pressed_{};
    std::optional<Stop> stop_{};
    bool focused_{};
    bool focus_region_{true};
    FontSpec font_{FontRole::control, 10.5, 400, false, 0.12};
    Event<const RibbonCommandClick&> command_clicked_{};
    Event<const RibbonDropDownInvocation&> drop_down_item_invoked_{};
    Event<const RibbonDialogLaunch&> dialog_launcher_clicked_{};
    Event<const RibbonTabChange&> selected_tab_changed_{};
    Event<bool> minimized_changed_{};
    Event<const MenuItemInvocation&> application_menu_item_invoked_{};
};

} // namespace gui_forms
