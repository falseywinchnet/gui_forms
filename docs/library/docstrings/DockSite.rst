The docked part of a docking workspace in one window: tool stacks with captions around the document area, separated by seams.

A dock site shows the main site of its `DockWorkspace`'s layout. Each split
of the layout becomes a seam between its two children; each tool stack
becomes a captioned `PageView` with its tabs at the bottom when it holds two
or more panels; the document area holds the document stacks, each a
`PageView` with its tabs on top. The site follows every layout change, and
the user's seam moves, tab choices and caption commands go back to the
layout as transactions. Use it as the main area of an application window.

Parameters
----------
stable_id
    Identity of the control, unique within its window. Each stack's view is
    ``<stable_id>.stack.<node id>``, its caption buttons
    ``<view id>.menu``, ``.pin`` and ``.close``, each seam
    ``<stable_id>.seam.<split id>``, the navigator ``<stable_id>.navigator``
    and the position menu ``<stable_id>.position_menu``.
workspace
    The workspace to show, borrowed: it must outlive the site, or be
    destroyed first, which unbinds the site.

Properties
----------
workspace : default the constructor's
    The bound workspace, or null after `release_workspace`.
keyboard_increment : default ``8``
    How far one arrow key moves a focused seam, in logical units, from 1 to
    256; outside that range throws `std::out_of_range`.
document_area_maximized : default ``false``
    Whether every tool stack is hidden so the documents fill the site
    (Ctrl+Shift+F12). Focus in a tool panel moves to the active document.
realized_stacks
    The stacks shown, collapsed ones included, in tree order.
collapsed_stacks
    The tool stacks hidden by shrinking or by the maximized document area
    after the last layout pass, in the order they collapsed.
minimum_size
    The smallest size that shows every stack: the panels' minimum sizes,
    the captions, the tab strips and the seams.
focused_seam : default none
    The seam the keyboard is on while the site has focus.
position_menu_open
    Whether a position menu is open.
navigator_open
    Whether the navigator is open.
navigator : default none
    The open navigator.
realized_seams
    The seams after the last layout pass, with their parts and states.
realized_stack_items
    The stacks shown after the last layout pass, with their parts and
    states.
realized_tiles
    While dragging: the drop tiles shown, the hovered pane's five then the
    window's four edge tiles, with their parts and states.
drag_phase : default ``DockDragPhase::idle``
    Where a drag is: ``armed`` after a press on a caption, ``dragging``,
    ``settling`` while the stamp grows into its place.
drag_subject : default none
    The stack or panel being dragged.
drag_targets
    While dragging, every target the drag offers, in the keyboard's order:
    each pane's top, left, centre, right and bottom (those the
    restrictions accept), panes in tree order, then the window's top, left,
    right and bottom edges.
drag_panes
    The panes the targets were planned over: the stacks as laid out with
    the dragged subject's place closed up, or the empty document area.
drag_target : default none
    The target a drop would use now.
drag_suppressed : default ``false``
    Whether Ctrl suppresses docking.
drag_stamp
    The stamp's rectangle while dragging or settling.
drag_announcement
    What the drag announces ("Output: dock left of Properties"), also the
    site's semantic description while the drag runs.
drag_threshold : default ``4``
    How far, in logical units along either axis, a caption press moves
    before it drags; outside 1 to 64 throws `std::out_of_range`.

Events
------
drag_ended
    Raised once when a drag that started ends, with how it ended
    (`DockDragEnd`: dropped, escaped, capture lost, deactivated, layout
    changed, rejected), its subject and, for a drop, the target and the
    transaction's result. Raised after a drop's transaction.

Methods
-------
initialize_control_tree
    Realizes the workspace's layout. `make_control` calls it; do not call it
    yourself.
release_workspace
    Unbinds the workspace; the site then shows nothing.
stack_view
    The `PageView` showing a stack, or null.
focus_seam
    Focuses the site with the keyboard on a seam; false when there is no
    such seam.
move_seam
    Moves a seam by a distance along its split, within both sides'
    minimums, as a user edit; false when it cannot move.
show_position_menu
    Opens a docked panel's position menu at its caption's menu button, or
    below a document stack's tabs. False for any other panel or a detached
    site; an unknown opening mode throws `std::invalid_argument`.
close_position_menu
    Closes the position menu.
position_command_enabled
    Whether a position command (Float, Dock, Dock as Tabbed Document, Auto
    Hide, Hide, Close) is available for a panel now.
run_position_command
    Runs a position command as the menu does; false when it is not
    available.
open_navigator
    Opens the navigator over the site on the tool panels or the documents,
    with the row after the active one current (or the last), choosing when
    the given modifier is released. False when there is nothing to list or
    the site is detached.
close_navigator
    Closes the navigator without choosing.
activate_relative_document
    Activates the next or previous document in tab order, wrapping; false
    when there is no document.
begin_keyboard_move
    Starts the keyboard move mode for a docked panel, on the first target;
    false for any other panel, while a drag runs, or with no target.
cancel_drag
    Ends a drag as Escape does; false when none runs.
seam_identity
    The semantic identity of a split's seam.
stack_identity
    The identity of a stack's view.

See Also
--------
DockWorkspace : The layout, the panels and the activation tracks.
DockNavigator : The Ctrl+Tab switcher.
PageView : Each stack's tabs and content.
DockLayout : The layout model.

Notes
-----
Captions (owner decision DK-41): a tool stack's caption shows the identity
key, the selected panel's title, the position menu button, the pin and
close. The pin is shown on root-level items only (decision DK-28): every
docked tool stack carries one, the tabs inside a stack none; it is checked
while the stack is docked and stays unavailable until auto-hide (phase D5).
Close hides the panel (its placeholder stays), or closes one that may not
be hidden. The caption of the stack holding the active panel is
``current``; the default theme (T4) draws it as a pale tint with a 2-unit
accent line on top, and until then today's recipes draw it.

The position menu lists Float and Auto Hide, both unavailable until phases
D4 and D5; Dock, checked while the panel stands in a tool stack and
available for a tool panel docked as a document (it returns to its default
side); Dock as Tabbed Document, for a tool panel whose descriptor allows it;
Hide; Close; and Move, the keyboard move mode. A document's menu lists
Float, Close and Move.

Seams are 3 units with a grip of dots (decision DK-41). Dragging one, or
moving it with the arrow keys along its split while the site has focus,
changes the split's ratio within both sides' minimums. Tab and Shift+Tab
step between seams and leave the site past either end.

Shrinking (decision DK-23): when the site is smaller than its minimum, tool
stacks collapse in increasing collapse priority (a stack's priority is its
panels' largest; on equal priority the later stack in tree order goes
first), each only if its collapse reduces the shortfall, until the rest
fits. Until auto-hide exists a collapsed stack is hidden in place: the
layout does not change, its panels keep their places and nothing is saved
differently, and the stack returns as soon as there is room. The maximized
document area hides every tool stack the same way.

Keyboard (decision DK-30), while focus is in the site: F6 and Shift+F6 move
between the stacks, which are the window's focus regions in tree order
(the program's other regions keep their places); Ctrl+Tab and
Ctrl+Shift+Tab open the navigator on the documents and Alt+F7 and
Alt+Shift+F7 on the tool panels, and releasing Ctrl or Alt activates the
current row; Ctrl+F6 and Ctrl+Shift+F6 activate the next and previous
document; Escape in a tool panel, when its content leaves it unhandled,
returns to the active document; Shift+Escape hides the active tool panel;
Ctrl+Shift+F12 toggles the maximized document area; Alt+Hyphen, Shift+F10
and the context-menu key open the active panel's position menu. A focused
`TabControl` keeps Ctrl+Tab: the stacks pass it on. Every key but F6 is a
named keyboard command (`KeyCommand`: ``cycle_forward``,
``cycle_backward``, ``next_tool_panel``, ``previous_tool_panel``,
``next_document``, ``previous_document``, ``return_to_document``,
``hide_tool_panel``, ``maximize_document_area``, ``position_menu`` and
``context_menu``), so `Window::rebind_key_command` changes it and the
navigator commits on the release of the new chord's Ctrl, Alt or Meta.

Activation: focus entering a stack (a click, Tab, F6, a tab chosen) makes
its selected panel the workspace's active panel; a press on a caption, or
anywhere in a panel, activates it and moves focus to the control that last
had focus there, else its first focusable control. When the panel holding
focus is hidden, closed, moved or collapsed, focus goes to the active panel,
else the active document.

Dragging (phase D3, decisions DK-24 to DK-26): a press on a caption that
moves 4 units drags the whole stack; a tool tab dragged drags one panel at
once; a document tab first slides along its strip (reordering) and tears
off when the pointer leaves the strip's band. At the start the subject
leaves its place and the place closes up; the layout itself does not change
until the drop, so Escape leaves it byte-identical. A miniature of the
panel (its caption strip, title and content well, drawn by role; not a
picture of its content) shrinks from the panel's rectangle to about 100
units over 220 ms and follows the pointer; at the end it grows into the
panel's new place, or back into its old one. Over the pane under the
pointer, and only that pane, a cross of five tiles appears (fading in over
100 ms); where the pointer is in the pane, or the tile under it, selects
top, left, right or bottom (split the pane) or the centre (tab into it),
and that tile is lit; the area the subject would take is tinted. A tool
panel over a document stack's side docks beside the whole document area.
The window's edges are global targets throughout, with a tile at each
edge's middle. Targets the restrictions (dock groups, allowed placements,
the user lock) refuse are not offered and their tiles are disabled. Ctrl
held suppresses docking. A release on a target applies one transaction
and activates the moved panel; a release elsewhere snaps back with a
rejected cue (floating arrives in phase D4). Escape, losing the pointer
capture, the window deactivating and a layout change by the program end a
drag with the layout unchanged.

The keyboard move mode (Move in the position menu, or the ``move_panel``
key command, Ctrl+Alt+Shift+M): the arrows step through the current
pane's tiles, Left and Right along left, centre and right, Up and Down
along top, centre and bottom; Tab and Shift+Tab move to the next and
previous pane and then the window's edges, over exactly the targets the
pointer reaches. Enter drops; Escape cancels.

Not yet: floating sites (phase D4) and auto-hidden stacks (D5) are kept in
the layout but not shown; document tab close glyphs and the overflow list
arrive with the page view's document preset.

To accessibility the site is a group named "Docking workspace" (or its
accessible name). Each stack is a ``pane`` named by its selected panel, or
"Documents", holding its tab list, its tab panel and, for a tool stack, the
caption buttons: "Window position" (a ``menu_button``), "Pinned" (a checked
``check_box``, unavailable) and "Close". Each seam is a ``split_pane`` whose
value is its first side's extent, between the two sides' minimums, with
focus, increment, decrement and set_value. A document tab's close action
closes the document.

Every part is drawn by the by-role paint path. The site's parts are
``region``, ``document_area`` (the well behind the documents) and
``focus``. The ``seam`` item system has ``seam_bar`` and ``seam_grip`` (the
``grip_dots`` mark), with hot, captured, focus_visible and disabled. The
``stack`` item system has ``stack_area`` and, for a tool stack,
``caption``, ``caption_accent`` (present while current), ``caption_key``,
``caption_label``, ``caption_menu`` and ``caption_menu_glyph`` (a
``chevron_down``), ``caption_pin`` and ``caption_pin_glyph`` (the ``pin``
mark, checked), ``caption_close`` and ``caption_close_glyph``; its states
are current and disabled, and the buttons carry their own hot, pressed,
open, checked and disabled. While dragging, the site's ``drag_stamp``,
``drag_stamp_label``, ``drop_area`` (drop_accepted) and ``drop_cue``
(drop_rejected, the ``close`` mark) are present, and the ``drop_tile``
item system has ``tile`` and ``tile_glyph`` (the ``dock_shape`` mark with
the zone's region), with selected (lit), disabled (refused) and expanding
(fading in).

Examples
--------
A workspace with a tool panel beside one document, shown in a window:

.. code-block:: cpp

   struct Views final {
       void make(gf::DockPanelCreation& creation) {
           creation.content = gf::make_control<gf::TextBox>(gf::StableId("view." + creation.panel.value));
       }
   };

   void example() {
       Views views;
       gf::DockWorkspace workspace("ide");
       const gf::Delegate<gf::DockPanelCreation&> factory =
           gf::Delegate<gf::DockPanelCreation&>::bind<Views, &Views::make>(views);
       gf::DockPanelRegistration files{};
       files.descriptor.id = gf::DockPanelId("files");
       files.descriptor.default_placement.side = gf::DockSide::left;
       files.descriptor.minimum_size = {120.0, 80.0};
       files.title = "Files";
       files.factory = factory;
       gf::DockPanelRegistration readme{};
       readme.descriptor.id = gf::DockPanelId("readme");
       readme.descriptor.kind = gf::DockPanelKind::document;
       readme.title = "README";
       readme.factory = factory;
       static_cast<void>(workspace.register_panel(files));
       static_cast<void>(workspace.register_panel(readme));
       static_cast<void>(workspace.show_panel(gf::DockPanelId("readme")));
       static_cast<void>(workspace.show_panel(gf::DockPanelId("files")));
       std::shared_ptr<gf::DockSite> site = gf::make_control<gf::DockSite>(gf::StableId("ide.site"), workspace);
       gf::Window window(site, {640.0, 400.0});
       window.perform_layout();
       // Two stacks and one seam; both panels' views were made on layout.
       (*site).set_document_area_maximized(true);
       window.perform_layout();
       // The Files stack is hidden; the document fills the site.
   }
