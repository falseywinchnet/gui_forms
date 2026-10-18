#pragma once

#include "gui_forms/commands.hpp"
#include "gui_forms/components/context_menu/context_menu.hpp"
#include "gui_forms/control.hpp"
#include "gui_forms/controls/docking/dock_navigator/dock_navigator.hpp"
#include "gui_forms/controls/docking/dock_workspace/dock_workspace.hpp"
#include "gui_forms/controls/pages/page_view/page_view.hpp"
#include "gui_forms/docking.hpp"
#include "gui_forms/event.hpp"
#include "gui_forms/window.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace gui_forms {

// The rows of a panel's position menu (owner decision DK-30; docking study
// 5.7). Float waits for floating windows (phase D4) and Auto Hide for
// auto-hide (phase D5); both are listed, disabled. Move starts the keyboard
// move mode (phase D3, G3-1).
enum class DockPositionCommand : std::uint8_t {
    float_panel,
    dock,
    dock_as_document,
    auto_hide,
    hide,
    close,
    move,
};

// The phases of a drag (docking study 5.1, phase D3): `armed` from a press
// on a caption until the pointer has moved the drag threshold; `dragging`
// while the stamp follows the pointer, or the keyboard steps through the
// targets; `settling` while the stamp grows into its new place, or back into
// its old one. Input other than the drag's own is not taken while settling.
enum class DockDragPhase : std::uint8_t { idle, armed, dragging, settling };

// How a drag ended: dropped on a target (one layout transaction); Escape;
// the pointer capture lost; the window deactivated; the layout changed
// under it (by the program, so the drag never applies to a stale tree); or
// released where nothing accepts it, which snaps back until floating
// windows exist (D4-1; phase D4 floats instead).
enum class DockDragEnd : std::uint8_t { dropped, escaped, capture_lost, deactivated, layout_changed, rejected };

struct DockDragOutcome final {
    DockDragEnd end{DockDragEnd::escaped};
    DockSubject subject{};
    // The target dropped on, for `dropped`.
    std::optional<DockDropTarget> target{};
    // The layout transaction's result, for `dropped`.
    DockTransactionResult result{};
};

// The main window's part of a docking layout, in one window (stage DK phase
// D2; owner decisions DK-18 to DK-23, DK-28 to DK-30, DK-41): binary splits
// separated by seams, tool stacks with captions, and the one document area
// with its document stacks, each stack a PageView over the stack's panels.
//
// The site borrows its workspace: the workspace must outlive it, or be
// destroyed first, which unbinds the site (it then shows nothing). The site
// realizes the main site's tree after every layout change: one PageView per
// stack (`<site id>.stack.<node id>`), reused while the stack keeps its node
// identity, its pages the stack's panels in order with the stack's
// selection; tool stacks show their tabs at the bottom only when they hold
// two or more panels, document stacks at the top. A panel's content is its
// workspace control, created when the panel is first shown selected (lazy
// creation). Floating sites and auto-hidden stacks are not realized until
// phases D4 and D5.
//
// Parts: seams (item system `seam`), 3 units, with a grip of dots, between
// the two children of each split; a seam is dragged with the pointer or,
// while the site has focus, moved with the arrow keys (Tab and Shift+Tab
// step between seams), within both sides' minimums (DockLayout's solver).
// Each tool stack has a caption (item system `stack`): the identity key,
// the selected panel's title, the position menu, the pin (root-level items
// only, owner decision DK-28: checked while docked, disabled until D5) and
// close (which hides the panel). The caption of the stack holding the
// active panel is `current`: the look (a pale tint with a 2-unit accent line
// on top, decision DK-41) is the default theme's (T4); until then it paints
// through paint_part with today's recipes.
//
// Shrinking: when the site is smaller than the tree's minimum, tool stacks
// collapse in increasing collapse priority (the largest priority of their
// panels; ties collapse the later stack in tree order first) until the rest
// fits (owner decision DK-23). Until auto-hide exists (D5) a collapsed stack
// is hidden in place: the layout model is unchanged and its panels keep
// their places, and the stack returns when room returns. A stack whose
// collapse would not reduce the shortfall is kept.
//
// Keyboard (owner decision DK-30), while focus is in the site: F6 and
// Shift+F6 move between the stacks (each a focus region of the window, in
// tree order); Ctrl+Tab and Ctrl+Shift+Tab open the navigator on the
// documents, Alt+F7 and Alt+Shift+F7 on the tool panels, and releasing the
// modifier activates the current row; Ctrl+F6 and Ctrl+Shift+F6 activate the
// next and previous document in most-recently-used order (DK-46); Escape
// in a tool panel (unless its content used it) returns to the active
// document, Shift+Escape hides
// the active tool panel; Ctrl+Shift+F12 toggles the maximized document area
// (every tool stack hidden, as by collapsing, and restored); Alt+Hyphen,
// Shift+F10 and the context-menu key open the active panel's position menu.
// A focused TabControl keeps Ctrl+Tab (PageView's Ctrl+Tab policy is `pass`
// inside the site). Every key but F6 is a named keyboard command
// (KeyCommand, G6-1), so the window's key map, as the host reports it and
// the application rebinds it, decides the chords; the navigator commits on
// the release of the chord's Ctrl, Alt or Meta.
//
// Activation: focus entering a stack (a click, Tab, F6, a tab chosen)
// makes its selected panel the workspace's active panel; a press on a
// caption activates it too.
//
// Semantics: each stack is a `pane` named by its selected panel (the
// document stacks "Documents"), holding the PageView's tab list and tab
// panel and its caption buttons; each seam is a `split_pane` with its
// position as the value and increment, decrement and set_value.
//
// Dragging (phase D3; owner decisions DK-24 to DK-26, DK-30, DK-41): a press
// on a caption that moves the drag threshold (4 units, D5-1) drags the whole
// stack; a tab dragged off its strip drags one panel (D7-1). A tool stack's
// strip does not reorder, so its tab drag is claimed at once
// (page_drag_started); a document strip reorders by its live slide until the
// pointer leaves the strip's band, and then the panel tears off. At the
// start the subject leaves its place and the place closes up in one layout
// pass of the site; the layout model is unchanged until the drop (docking
// study 5.1, invariant 4), so Escape leaves it as it was. A miniature of the
// panel (its caption or tab, label and content well, drawn by role) is
// interpolated from the panel's rectangle down to a stamp about 100 units
// long over 220 ms and follows the pointer; at the drop it grows into the
// panel's new place, on Escape back into its old one; reduced motion skips
// the motion. Targets (DockDropTarget, planned when the drag starts and at
// every layout pass): over the hovered pane only, a cross of five quiet
// tiles (top, left, tab into, right, bottom) appears, fading in over 100 ms;
// the pointer's position within the pane (or the tile under it) selects
// one, which is lit; the main window's edges are global targets with a tile
// at each edge's middle; the area the subject would take is tinted. Targets
// the restrictions refuse are not planned and their tiles are disabled.
// Ctrl held suppresses docking. A release on a target applies one user
// transaction and activates the moved panel; a release elsewhere snaps
// back. Escape, losing the pointer capture, the window deactivating and a
// layout change by the program cancel.
//
// The keyboard move mode (G3-1, docking study 5.7.1): the position menu's
// Move or the move_panel key command (Ctrl+Alt+Shift+M) on the active
// panel. The arrows step through the current pane's tiles (Left and Right
// along left, centre, right; Up and Down along top, centre, bottom), Tab and
// Shift+Tab to the next and previous pane and then the window edges, over
// exactly the targets the pointer reaches; each step updates the preview and
// the announcement (drag_announcement, also the site's semantic
// description while a drag runs). Enter commits through the same
// transaction as a drop; Escape cancels.
class DockSite : public Control {
public:
    static constexpr bool initialize_tree_after_construction = true;
    // Borrows `workspace` (see the lifetime rule above).
    DockSite(StableId stable_id, DockWorkspace& workspace);
    ~DockSite() override;
    // Realizes the workspace's layout. make_control calls it; do not call it
    // yourself.
    void initialize_control_tree();

    // The bound workspace, or null after release_workspace.
    [[nodiscard]] DockWorkspace* workspace() const noexcept { return workspace_; }
    // Unbinds the workspace: the site removes its stacks and shows nothing.
    void release_workspace() noexcept;

    // The step of a keyboard seam move, in logical units; 8 by default.
    // Throws std::out_of_range outside 1 to 256.
    [[nodiscard]] double keyboard_increment() const noexcept { return keyboard_increment_; }
    void set_keyboard_increment(double increment);

    // The maximized document area (Ctrl+Shift+F12): every tool stack hidden
    // in place, as by collapsing, until it is restored. Focus in a tool
    // panel moves to the active document.
    [[nodiscard]] bool document_area_maximized() const noexcept { return maximized_; }
    void set_document_area_maximized(bool maximized);

    // The PageView realizing a stack of the main site, or null.
    [[nodiscard]] std::shared_ptr<PageView> stack_view(DockNodeId stack) const;
    // The realized stacks in tree order, collapsed ones included.
    [[nodiscard]] std::vector<DockNodeId> realized_stacks() const;
    // The tool stacks hidden by shrinking or by the maximized document area,
    // after the last layout pass, in the order they collapsed.
    [[nodiscard]] std::vector<DockNodeId> collapsed_stacks() const;
    // The tree's minimum size with nothing collapsed, from the panels'
    // minimums, the captions, the tab strips and the seams.
    [[nodiscard]] Size minimum_size() const;

    // Focuses the site with the keyboard on a seam. False when there is no
    // such seam or the site cannot take focus.
    bool focus_seam(std::size_t seam);
    // The seam the keyboard is on while the site has focus, or none.
    [[nodiscard]] std::optional<std::size_t> focused_seam() const noexcept;
    // Moves a seam by `delta` logical units along its split's axis, within
    // both sides' minimums, as a user edit. False when it cannot move.
    bool move_seam(std::size_t seam, double delta);

    // The position menu of a panel docked in the main site, anchored at its
    // caption's menu button (a tool stack) or below its tab strip (a
    // document stack). False, opening nothing, for any other panel or a
    // detached site. Throws std::invalid_argument for an unknown mode.
    bool show_position_menu(const DockPanelId& panel, MenuOpenMode mode = MenuOpenMode::keyboard);
    void close_position_menu() noexcept;
    [[nodiscard]] bool position_menu_open() const noexcept;
    // Whether a position command is available for a panel now, and runs
    // it, as the menu does. Running an unavailable command is refused
    // (false).
    [[nodiscard]] bool position_command_enabled(const DockPanelId& panel, DockPositionCommand command) const;
    bool run_position_command(const DockPanelId& panel, DockPositionCommand command);

    // The navigator popup, centred over the site, on `list` with the row
    // after the active one current (`direction` +1) or the last (-1);
    // releasing `release` (control or alt; none for Enter only) chooses.
    // False when both lists are empty or the site is detached.
    bool open_navigator(DockNavigatorList list, int direction, Modifier release);
    void close_navigator() noexcept;
    [[nodiscard]] bool navigator_open() const noexcept { return navigator_open_; }
    // The open navigator, or null.
    [[nodiscard]] std::shared_ptr<DockNavigator> navigator() const noexcept;

    // Activates a document by most-recently-used order (owner decision
    // DK-46): +1 the document used before the active one, -1 the one used
    // longest ago (documents never active come last, in tab order),
    // wrapping. False when there is no document. Ctrl+F6 and Ctrl+Shift+F6
    // step on along one order while their modifier is held.
    bool activate_relative_document(int direction);

    // -- dragging (phase D3) --
    [[nodiscard]] DockDragPhase drag_phase() const noexcept { return drag_.phase; }
    // The subject of the drag under way (armed or dragging), or none.
    [[nodiscard]] std::optional<DockSubject> drag_subject() const;
    // While dragging: the planned targets in keyboard order, the panes they
    // were planned over (the layout with the subject's place closed up),
    // and the selected target, or none (between targets, over a refused
    // zone, outside the site, or with Ctrl held).
    [[nodiscard]] std::span<const DockDropTarget> drag_targets() const noexcept;
    [[nodiscard]] std::span<const DockDropPane> drag_panes() const noexcept;
    [[nodiscard]] std::optional<DockDropTarget> drag_target() const;
    // Whether Ctrl suppresses docking now (D6-1).
    [[nodiscard]] bool drag_suppressed() const noexcept { return drag_.suppressed; }
    // The stamp's rectangle in the site's coordinates while dragging or
    // settling, else empty.
    [[nodiscard]] Rect drag_stamp() const noexcept;
    // What the drag announces now ("Output: dock right of Properties"), or
    // empty while idle.
    [[nodiscard]] const std::string& drag_announcement() const noexcept { return drag_.announcement; }
    // The distance, in logical units along either axis, a caption press
    // moves before it drags (owner decision DK-26, D5-1: 4, or a host's
    // metric; no host reports one yet). Throws std::out_of_range outside 1
    // to 64.
    [[nodiscard]] double drag_threshold() const noexcept { return drag_threshold_; }
    void set_drag_threshold(double threshold);
    // Starts the keyboard move mode for a panel docked in the main site, on
    // the first planned target. False, starting nothing, for any other
    // panel, while a drag runs, or when there is no target.
    bool begin_keyboard_move(const DockPanelId& panel);
    // Ends a drag as Escape does. False when none runs.
    bool cancel_drag();
    // Raised once when a drag that left the armed phase ends, after the
    // layout transaction of a drop.
    [[nodiscard]] Event<const DockDragOutcome&>& drag_ended() noexcept { return drag_ended_; }

    [[nodiscard]] Size measure(Size available) override;
    void arrange(Rect final_bounds) override;
    void on_paint(Painter& painter, Rect local_damage) override;
    void on_pointer_preview(PointerEvent& event) override;
    void on_pointer(PointerEvent& event) override;
    void on_key_preview(KeyEvent& event) override;
    void on_key(KeyEvent& event) override;
    void on_key_bubble(KeyEvent& event) override;
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
        // The document area's rectangle, its well showing while it is empty.
        PartGeometry document_area;
        PartGeometry focus;
        // The drag layer (present only while dragging or settling): the
        // stamp, its label, the tinted area the subject would take
        // (drop_accepted) and the cue at the pointer where nothing accepts
        // a drop (drop_rejected).
        PartGeometry drag_stamp;
        PartGeometry drag_stamp_label;
        PartGeometry drop_area;
        PartGeometry drop_cue;
    };
    [[nodiscard]] Parts parts() const;
    // Item system seam: the bar between a split's two children and its grip.
    struct SeamParts final {
        PartGeometry seam_bar;
        PartGeometry seam_grip;
    };
    struct SeamStates final {
        bool hot{};
        bool captured{};
        bool focus_visible{};
        bool disabled{};
    };
    struct Seam final {
        DockNodeId split{};
        DockAxis axis{DockAxis::horizontal};
        std::string stable_id;
        SeamParts parts;
        SeamStates states;
    };
    // Item system stack: a stack's area and, for a tool stack, its caption
    // and the caption's parts (absent for a document stack). The actuators
    // carry their own hot, pressed, open, checked and disabled states.
    struct StackParts final {
        PartGeometry stack_area;
        PartGeometry caption;
        PartGeometry caption_accent;
        PartGeometry caption_key;
        PartGeometry caption_label;
        PartGeometry caption_menu;
        PartGeometry caption_menu_glyph;
        PartGeometry caption_pin;
        PartGeometry caption_pin_glyph;
        PartGeometry caption_close;
        PartGeometry caption_close_glyph;
    };
    struct StackStates final {
        // The stack holds the active panel.
        bool current{};
        bool disabled{};
    };
    struct Stack final {
        DockNodeId stack{};
        DockNodeKind kind{DockNodeKind::tool_stack};
        // The stack's selected panel.
        DockPanelId panel{};
        std::string stable_id;
        StackParts parts;
        StackStates states;
    };
    // Item system drop_tile: a target tile and its dock-shape glyph, over
    // the hovered pane (five) and at the window's edges (four), while
    // dragging. `selected` is the lit tile; `disabled` a choice the
    // restrictions refuse; `expanding` while the cross fades in.
    struct TileParts final {
        PartGeometry tile;
        PartGeometry tile_glyph;
    };
    struct TileStates final {
        bool selected{};
        bool disabled{};
        bool expanding{};
    };
    struct Tile final {
        DockDropTargetKind kind{DockDropTargetKind::pane};
        DockNodeId pane{};
        DockDropZone zone{DockDropZone::center};
        std::string stable_id;
        TileParts parts;
        TileStates states;
    };
    // The seams, then the stacks that are not collapsed, in tree order;
    // then, while dragging, the tiles (the hovered pane's cross, then the
    // edges).
    [[nodiscard]] std::vector<Seam> realized_seams() const;
    [[nodiscard]] std::vector<Stack> realized_stack_items() const;
    [[nodiscard]] std::vector<Tile> realized_tiles() const;

    // The semantic identities: `<site id>.seam.<split id>` for a seam,
    // `<site id>.stack.<node id>` for a stack's view and
    // `<view id>.menu`, `.pin` and `.close` for its caption buttons.
    [[nodiscard]] std::string seam_identity(DockNodeId split) const;
    [[nodiscard]] std::string stack_identity(DockNodeId stack) const;

protected:
    void append_interactions(std::vector<InteractionDeclaration>& interactions) const override;
    void on_attachment_committed() noexcept override;
    void on_detaching_from_window(Window& former_window) noexcept override;
    void on_dispose() noexcept override;
    void publish_part_geometry(std::span<PartGeometry> geometry) const override;
    [[nodiscard]] ItemSystemIndex publish_realized_item_system(std::size_t item) const override;
    void publish_realized_item_geometry(std::size_t item, std::span<PartGeometry> geometry) const override;

private:
    friend class DockWorkspace;
    class StackView;
    class DragLayer;

    // What the pointer or a press is on.
    enum class Zone : std::uint8_t { none, seam, caption, menu, pin, close };
    struct Hit final {
        Zone zone{Zone::none};
        // The seam's index for Zone::seam; the stack's index in stacks_ for
        // the caption zones.
        std::size_t index{};

        friend bool operator==(const Hit& left, const Hit& right) noexcept {
            const bool equal = left.zone == right.zone && left.index == right.index;
            return equal;
        }
        friend bool operator!=(const Hit& left, const Hit& right) noexcept {
            const bool different = !(left == right);
            return different;
        }
    };
    // One realized stack and its view. `collapsed` is the last layout pass's
    // decision; `bounds` and `caption` are its rectangles there.
    struct StackSlot final {
        DockNodeId stack{};
        DockNodeKind kind{DockNodeKind::tool_stack};
        std::shared_ptr<StackView> view{};
        // The selected panel's title, kept for painting the caption.
        std::string title{};
        bool collapsed{};
        Rect bounds{};
        Rect caption{};
    };
    // One seam from the last layout pass: the split, its axis, the bar and
    // its wider hit band, and what a move needs (the span shared by the two
    // children after the seam, where it starts, and both minimums).
    struct SeamSlot final {
        DockNodeId split{};
        DockAxis axis{DockAxis::horizontal};
        Rect bar{};
        Rect hit{};
        double span_start{};
        double span{};
        double first{};
        double first_minimum{};
        double second_minimum{};
    };
    // A subtree's minimum size; `present` is false when every stack in it is
    // collapsed (it then takes no space).
    struct Extent final {
        bool present{};
        Size minimum{};
    };
    struct CaptionGeometry final {
        Rect caption{};
        Rect accent{};
        Rect key{};
        Rect label{};
        Rect menu{};
        Rect menu_glyph{};
        Rect pin{};
        Rect pin_glyph{};
        Rect close{};
        Rect close_glyph{};
    };

    // -- realization --
    void realize();
    void remove_slot(StackSlot& slot) noexcept;
    void sync_pages(StackSlot& slot, const DockNode& node);
    void supply_content(StackSlot& slot);
    void update_focus_regions();
    void on_layout_changed(const DockLayoutChange& change);
    // The views' events, bound to the stack's node identity.
    void on_view_selection(DockNodeId stack, const PageSelectionChange& change);
    void on_view_content(DockNodeId stack, const PageContentRequest& request);
    void on_view_close(DockNodeId stack, PageCloseRequest& request);
    void on_activation_changed(const DockActivationChange& change);
    void on_focus_moved(const FocusedControlChange& change);
    [[nodiscard]] StackSlot* slot_for(DockNodeId stack) noexcept;
    [[nodiscard]] const StackSlot* slot_for(DockNodeId stack) const noexcept;
    // The index in stacks_ of the stack whose view is or contains `control`,
    // or stacks_.size().
    [[nodiscard]] std::size_t slot_index_of(const Control& control) const;
    [[nodiscard]] std::optional<DockPanelId> selected_panel(const StackSlot& slot) const;

    // -- layout --
    [[nodiscard]] Extent extent_of(DockNodeId node, std::span<const DockNodeId> collapsed) const;
    [[nodiscard]] Size stack_minimum(const DockNode& node) const;
    [[nodiscard]] std::vector<DockNodeId> plan_collapse(Size available) const;
    void place(DockNodeId node, Rect bounds, std::span<const DockNodeId> collapsed);
    [[nodiscard]] CaptionGeometry caption_geometry(Rect caption, bool pin) const;
    [[nodiscard]] double seam_thickness() const;
    [[nodiscard]] bool seams_moved() const noexcept;
    [[nodiscard]] double caption_height() const;

    // -- input and commands --
    [[nodiscard]] Hit hit_at(Point local) const;
    void set_hot(Hit hit);
    void begin_seam_drag(std::size_t seam, Point local);
    void continue_seam_drag(Point local);
    bool set_seam_first(std::size_t seam, double first);
    bool handle_docking_key(KeyEvent& event);
    void collect_document_steps(std::vector<DockPanelId>& out) const;
    bool step_document(int direction, Modifier release);
    void end_document_steps(const KeyEvent& event) noexcept;
    bool handle_seam_key(KeyEvent& event);
    [[nodiscard]] std::optional<DockPanelId> focused_panel() const;
    [[nodiscard]] bool focus_in_tool_stack() const;
    void focus_panel(const DockPanelId& panel);
    void refocus_if_lost(bool focus_was_inside);
    void on_menu_item_invoked(const MenuItemInvocation& invocation);
    void on_menu_open_changed(bool open);
    void on_navigator_chosen(const DockNavigatorChoice& choice);
    void on_navigator_cancelled();
    void on_navigator_revoked();
    void on_window_pointer(const PointerEvent& event);

    // -- paint and states --
    [[nodiscard]] AnatomyStateSet seam_states(std::size_t seam) const;
    [[nodiscard]] AnatomyStateSet stack_states(const StackSlot& slot) const;
    [[nodiscard]] AnatomyStateSet button_states(std::size_t stack, Zone zone) const;
    [[nodiscard]] bool stack_current(const StackSlot& slot) const;
    [[nodiscard]] bool pin_shown(const StackSlot& slot) const;
    [[nodiscard]] SeamParts seam_parts(std::size_t seam) const;
    [[nodiscard]] StackParts stack_parts(const StackSlot& slot, std::size_t index) const;
    [[nodiscard]] std::size_t visible_stack_count() const;
    [[nodiscard]] std::size_t visible_stack_at(std::size_t ordinal) const;
    [[nodiscard]] FontSpec caption_font() const;
    [[nodiscard]] bool focus_cue() const noexcept;

    [[nodiscard]] std::vector<SemanticNode> caption_nodes(const StackSlot& slot, std::size_t index) const;
    bool caption_action(std::size_t index, std::string_view stable_id, SemanticAction action);

    // -- dragging (dock_site_drag.cpp) --
    // One tile as painted and published: where it is, which target it
    // stands for, and whether it is lit or refused.
    struct TileSlot final {
        DockDropTargetKind kind{DockDropTargetKind::pane};
        DockNodeId pane{};
        DockDropZone zone{DockDropZone::center};
        Rect bounds{};
        bool lit{};
        bool refused{};
    };
    // The drag session. `panel_only`: a tab drag of one panel out of a stack
    // that keeps others (its page leaves the view); otherwise the whole
    // source stack leaves the layout pass. `grab` is where the pointer holds
    // the source, as fractions of `source`, the subject's rectangle when the
    // drag started. `panes` and `targets` are planned at the start and at
    // every layout pass; `target` and `hovered_pane` index them.
    struct DragSession final {
        DockDragPhase phase{DockDragPhase::idle};
        DockSubject subject{};
        DockNodeId source_stack{};
        bool panel_only{};
        bool keyboard{};
        bool suppressed{};
        bool over_site{};
        std::uint64_t pointer_id{1U};
        Point press{};
        Point pointer{};
        Point grab{};
        Rect source{};
        std::string title{};
        std::uint64_t generation{};
        FrameTime started{};
        std::vector<DockDropPane> panes{};
        std::vector<DockDropTarget> targets{};
        std::optional<std::size_t> target{};
        std::optional<std::size_t> hovered_pane{};
        FrameTime hovered_since{};
        // The hovered pane's cross is fading in (T2-4, about 100 ms).
        bool tiles_fading{};
        // A panel_only drag's page has left its view (done at the first
        // layout pass, never inside the strip's own drag event).
        bool page_held_out{};
        Rect stamp{};
        // Settling: from the stamp where the drag ended to the panel's
        // rectangle, which the next layout pass finds (`settle_panel`).
        DockPanelId settle_panel{};
        bool settle_found{};
        Rect settle_from{};
        Rect settle_to{};
        FrameTime settle_started{};
        std::string announcement{};
    };

    void create_drag_layer();
    // Whether a tab drag holds `panel` out of `stack`'s view.
    [[nodiscard]] bool dragged_out(DockNodeId stack, const DockPanelId& panel) const noexcept;
    void arm_caption_drag(std::size_t stack, Point local, std::uint64_t pointer_id);
    void on_tab_drag(DockNodeId stack, PageDragStart& start);
    bool tear_off_document_tab(const PointerEvent& event);
    // `press` is where the subject was grabbed and `pointer` where the
    // pointer is now, both in the site's coordinates.
    bool begin_drag(const DockSubject& subject, DockNodeId source_stack, bool panel_only, Point press,
                    Point pointer, bool keyboard, std::uint64_t pointer_id);
    // Posted when the capture is released during a drag: a primary release
    // releases it just before the drop, so only a drag still running after
    // that dispatch lost its capture.
    struct CaptureCheck;
    void check_capture_after_release();
    [[nodiscard]] Rect drag_paint_extent() const;
    void plan_drag_targets();
    void update_drag_pointer(Point local, Modifier modifiers);
    void select_drag_target(std::optional<std::size_t> target, std::optional<std::size_t> hovered_pane);
    void end_drag(DockDragEnd end);
    void finish_settling() noexcept;
    void request_drag_frames();
    void advance_drag(FrameTime now);
    void place_settle_target();
    [[nodiscard]] bool hidden_by_drag(DockNodeId stack) const noexcept;
    [[nodiscard]] bool motion_reduced() const noexcept;
    [[nodiscard]] Rect stamp_at(FrameTime now) const;
    [[nodiscard]] Rect drag_label_bounds() const noexcept;
    [[nodiscard]] Rect drop_cue_bounds() const noexcept;
    [[nodiscard]] std::size_t tile_count() const noexcept;
    [[nodiscard]] TileSlot tile_at(std::size_t tile) const;
    // A tile's glyph: the tile less a fifth of its side all round.
    [[nodiscard]] static Rect tile_glyph_bounds(Rect tile) noexcept;
    [[nodiscard]] AnatomyStateSet tile_states(const TileSlot& tile) const;
    [[nodiscard]] std::string tile_identity(const TileSlot& tile) const;
    [[nodiscard]] std::string describe_target(const DockDropTarget& target) const;
    void announce_drag();
    bool handle_drag_key(KeyEvent& event);
    void step_keyboard_target(int direction);
    void step_keyboard_zone(std::uint32_t key);
    void paint_drag_layer(Painter& painter);
    void on_capture_changed(const PointerCaptureChange& change);
    void on_window_active(bool active);

    DockWorkspace* workspace_{};
    SubscriptionToken layout_token_{};
    SubscriptionToken activation_token_{};
    SubscriptionToken focus_token_{};
    // The realized stacks in tree order.
    std::vector<StackSlot> stacks_{};
    // From the last layout pass.
    std::vector<SeamSlot> seams_{};
    std::vector<DockNodeId> collapsed_{};
    // The pass before, kept for its storage and to tell what moved.
    std::vector<SeamSlot> previous_seams_{};
    std::vector<DockNodeId> previous_collapsed_{};
    Rect document_area_{};
    double keyboard_increment_{8.0};
    bool maximized_{};
    // Set while the site edits its views' page sets, whose selection events
    // it then ignores.
    bool syncing_{};
    bool focused_{};
    Hit hot_{};
    Hit pressed_{};
    std::size_t keyboard_seam_{};
    // The seam drag: the seam and the pointer's offset from the first
    // child's end along the axis.
    bool seam_captured_{};
    std::size_t drag_seam_{};
    double drag_offset_{};
    // The window whose focus the site follows and whose regions it set.
    Window* followed_window_{};
    std::vector<Control::WeakPtr> registered_regions_{};
    // The control that last had focus in each panel, for activation.
    struct RememberedFocus final {
        DockPanelId panel{};
        Control::WeakPtr control{};
    };
    std::vector<RememberedFocus> remembered_focus_{};
    std::unique_ptr<ContextMenu> menu_{};
    SubscriptionToken menu_invoked_{};
    SubscriptionToken menu_changed_{};
    // One command per position row, in DockPositionCommand order.
    std::array<std::shared_ptr<Command>, 7> menu_commands_{};
    DockPanelId menu_panel_{};
    std::size_t menu_stack_{};
    bool menu_open_{};
    std::shared_ptr<DockNavigator> navigator_{};
    PopupToken navigator_token_{};
    FocusScopeId navigator_scope_{};
    bool navigator_open_{};
    bool navigator_closing_{};
    SubscriptionToken navigator_chosen_{};
    SubscriptionToken navigator_cancelled_{};
    SubscriptionToken navigator_revoked_{};
    SubscriptionToken window_pointer_{};
    // The drag: its session, the layer that paints it above the stacks (a
    // child kept last), its frames and the window events that cancel it.
    DragSession drag_{};
    std::shared_ptr<DragLayer> drag_layer_{};
    FrameRequestToken drag_frames_{};
    DispatchOperation capture_check_{};
    SubscriptionToken capture_token_{};
    SubscriptionToken active_token_{};
    double drag_threshold_{4.0};
    // A run of Ctrl+F6 steps: the order taken at its first press, the
    // position reached and the modifier whose release ends it.
    std::vector<DockPanelId> document_steps_{};
    std::size_t document_step_{};
    Modifier document_step_release_{Modifier::none};
    bool document_stepping_{};
    Event<const DockDragOutcome&> drag_ended_{};
};

} // namespace gui_forms
