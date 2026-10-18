#pragma once

#include "gui_forms/components/context_menu/context_menu.hpp"
#include "gui_forms/controls/tool_strip/tool_strip.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace gui_forms {

// Where the toolbar sits: on the ribbon's tab row beside the File tab, or in
// its own row below the ribbon (owner decision RB-12). The toolbar only
// records the choice; the Ribbon that hosts it applies it.
enum class QuickAccessToolbarPlacement : std::uint8_t { above_ribbon, below_ribbon };

// One command the toolbar may show. Every declared command is listed, with a
// check, in the customize menu; `shown` is whether the toolbar shows it
// before the user customizes. `image` is the button's icon: a command with
// one shows only its icon, a command without one its text.
struct QuickAccessToolbarCommand final {
    std::shared_ptr<Command> command{};
    ImageId image{};
    bool shown{true};
};

// The user showed or hid a command from the customize menu, or the
// application did through set_command_shown.
struct QuickAccessToolbarCustomization final {
    std::string toolbar_id{};
    std::string command_id{};
    bool shown{};
};

struct QuickAccessToolbarPlacementChange final {
    std::string toolbar_id{};
    QuickAccessToolbarPlacement placement{QuickAccessToolbarPlacement::above_ribbon};
};

// The Quick Access Toolbar of a ribbon window (owner decisions RB-12 and
// RB-24): a ToolStrip of flat command buttons with a customize button at its
// end. The customize menu lists every declared command with a check that
// shows or hides it, then the placement choice. Buttons that do not fit move
// to the strip's overflow menu, opened from the overflow button left of the
// customize button, which stays visible.
//
// It is a ToolStrip, so it keeps ToolStrip's items, overflow, keyboard model
// and item anatomy; it admits only command buttons (commands only, D2-2), and
// set_items with any other item throws std::invalid_argument. The
// application declares commands with set_commands instead.
class QuickAccessToolbar final : public ToolStrip {
public:
    // The most commands the toolbar may declare.
    static constexpr std::size_t maximum_commands = 20U;

    explicit QuickAccessToolbar(StableId stable_id);
    ~QuickAccessToolbar() override;

    [[nodiscard]] const std::vector<QuickAccessToolbarCommand>& commands() const noexcept {
        return commands_;
    }
    // Declares the commands, in toolbar order, and shows those marked shown,
    // replacing any customization. Throws std::invalid_argument for a missing
    // command, an empty or repeated command ID, or more than
    // maximum_commands.
    void set_commands(std::vector<QuickAccessToolbarCommand> commands);
    // The IDs of the commands the toolbar shows, in toolbar order. After
    // set_shown_command_ids this may include IDs that name no declared
    // command: they are kept in place and show nothing, so a saved state
    // from a later version survives a round trip.
    [[nodiscard]] const std::vector<std::string>& shown_command_ids() const noexcept {
        return shown_ids_;
    }
    // Shows exactly these commands, in this order (restoring saved state).
    // Emits no customized events. Throws std::invalid_argument for an empty,
    // invalid UTF-8 or repeated ID, or more than maximum_commands IDs.
    void set_shown_command_ids(std::vector<std::string> ids);
    // Whether a declared command is shown. Throws std::out_of_range for an
    // undeclared ID.
    [[nodiscard]] bool command_shown(std::string_view command_id) const;
    // Shows a declared command (at the toolbar's end) or hides it, and emits
    // customized when that changes anything. Throws std::out_of_range for an
    // undeclared ID.
    void set_command_shown(std::string_view command_id, bool shown);

    [[nodiscard]] QuickAccessToolbarPlacement placement() const noexcept { return placement_; }
    // Records the placement and emits placement_changed when it changes.
    // Throws std::invalid_argument for a value outside the enumeration.
    void set_placement(QuickAccessToolbarPlacement placement);

    // Opens the customize menu below the customize button, or closes it.
    bool show_customize_menu(MenuOpenMode mode = MenuOpenMode::keyboard);
    void close_customize_menu() noexcept;
    [[nodiscard]] bool customize_menu_open() const noexcept;
    // The customize menu's rows: one check row per declared command (ID
    // "<toolbar>.customize.<command>"), a separator, and the placement row
    // (ID "<toolbar>.customize.placement").
    [[nodiscard]] const std::vector<MenuItemSpec>& customize_menu_items() const noexcept;

    [[nodiscard]] Event<const QuickAccessToolbarCustomization&>& customized() noexcept {
        return customized_;
    }
    [[nodiscard]] Event<const QuickAccessToolbarPlacementChange&>& placement_changed() noexcept {
        return placement_changed_;
    }

    void on_paint(Painter& painter, Rect local_damage) override;
    void on_pointer(PointerEvent& event) override;
    void on_key(KeyEvent& event) override;
    void on_focus_changed(bool focused) override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;
    [[nodiscard]] std::vector<SemanticNode> semantic_virtual_children() const override;
    bool on_semantic_child_action(std::string_view stable_id, SemanticAction action,
                                  std::string_view value) override;
    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;

    // Stage T1 anatomy, as dot members: ToolStrip's parts and the customize
    // button. Its states are Control's; its item system is ToolStrip's.
    struct Parts final {
        PartGeometry region;
        PartGeometry face;
        PartGeometry grip;
        PartGeometry overflow_button;
        PartGeometry overflow_glyph;
        PartGeometry customize_button;
        PartGeometry customize_glyph;
        PartGeometry focus;
    };
    [[nodiscard]] Parts parts() const;

protected:
    void validate_items(const std::vector<ToolStripItemSpec>& items) const override;
    void append_interactions(std::vector<InteractionDeclaration>& interactions) const override;
    void on_dispose() noexcept override;
    void publish_part_geometry(std::span<PartGeometry> geometry) const override;

private:
    // The customize button and its glyph, from the strip's one layout pass;
    // on_paint, hit testing and publish_part_geometry all read it.
    struct CustomizeGeometry final {
        Rect button{};
        Rect glyph{};
    };
    [[nodiscard]] CustomizeGeometry customize_geometry() const;
    [[nodiscard]] bool customize_at(Point window_point) const;
    [[nodiscard]] std::size_t declared_index(std::string_view command_id) const;
    [[nodiscard]] std::string customize_row_id(std::string_view command_id) const;
    void rebuild_items();
    void rebuild_menu();
    void on_menu_invoked(const MenuItemInvocation& invocation);
    void on_menu_open_changed(bool open);

    std::vector<QuickAccessToolbarCommand> commands_{};
    std::vector<std::string> shown_ids_{};
    // One private check Command per declared command, for its menu row: the
    // row's check shows the command's presence on the toolbar, and choosing
    // the row must not execute the command itself.
    std::vector<std::shared_ptr<Command>> row_commands_{};
    std::shared_ptr<Command> placement_command_{};
    std::vector<MenuItemSpec> menu_items_{};
    std::unique_ptr<ContextMenu> menu_{};
    SubscriptionToken menu_invoked_{};
    SubscriptionToken menu_changed_{};
    QuickAccessToolbarPlacement placement_{QuickAccessToolbarPlacement::above_ribbon};
    bool customize_hot_{};
    bool customize_open_{};
    bool focused_{};
    Event<const QuickAccessToolbarCustomization&> customized_{};
    Event<const QuickAccessToolbarPlacementChange&> placement_changed_{};
};

} // namespace gui_forms
