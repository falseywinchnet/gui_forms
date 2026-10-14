A ribbon: the File tab and core tabs over a command plane of groups.

A ribbon (stage RB, phase 1) shows a tab row that is always visible (the File
tab, then the core tabs) over the command plane of the selected tab. The plane
holds groups side by side, each with its commands, a caption beneath and,
when authored, a dialog launcher in the caption's corner. Commands are
`RibbonCommandSpec` records bound to shared `Command` objects: buttons,
toggles, split buttons and drop-down buttons at a large or small authored
size, and hosted combo boxes, numeric spinners, check boxes and text boxes,
which are real child controls the ribbon places. Every part is painted
through the by-role paint path.

Parameters
----------
stable_id
    Identity of the ribbon, unique within its window. Its menus are shown by
    a `ContextMenu` named ``<stable_id>.menu``; hosted controls are named
    ``<stable_id>.<command id>``.

Properties
----------
tabs
    The core tabs, each with its groups and their commands. `set_tabs`
    replaces them, keeps the selection on a tab with the same ID, keeps a
    hosted control while a command of the same ID and kind remains, and
    throws `std::invalid_argument` for an empty or repeated ID (one namespace
    across tabs, groups and commands), invalid UTF-8, a button, toggle or
    split without a `Command`, a menu on a kind that has none or a split or
    drop-down without one, an icon-only command that is not small or has no
    image, choices on a command that is not a combo box, a width outside 0
    to 1024, an invalid menu, or more than the development bounds.
selected_tab
    The selected tab, or none without tabs. `set_selected_tab` throws
    `std::out_of_range` for a tab past the end.
minimized : default ``false``
    Whether the command plane is hidden, leaving the tab row.
application_text : default ``"File"``
    The File tab's text, with an optional mnemonic.
application_menu
    The File tab's menu. `set_application_menu` throws
    `std::invalid_argument` for an invalid menu.
menu_open
    Whether the application menu or a command's menu is open.
focus_region : default ``true``
    Whether the ribbon adds itself to its window's F6 regions when it is
    attached.
font
    The font of tab, caption and command text.
realized_tabs
    The tabs' parts and states.
realized_groups
    The selected tab's groups and their parts.
realized_commands
    The selected tab's commands, their parts and states.

Events
------
command_clicked
    Raised after a button, toggle or split face runs its command, by
    pointer, keyboard, mnemonic or accessibility.
drop_down_item_invoked
    Raised when the user chooses from a split or drop-down command's menu.
dialog_launcher_clicked
    Raised when a group's dialog launcher is clicked or pressed.
selected_tab_changed
    Raised after the selected tab changes.
minimized_changed
    Raised after the command plane is hidden or shown.
application_menu_item_invoked
    Raised when the user chooses from the application menu.

Methods
-------
show_application_menu
    Opens the application menu over the ribbon, below the File tab. Returns
    false without items or a window.
show_drop_down
    Opens a split or drop-down command's menu below it. Returns false for
    another kind, a disabled command or one not on the selected tab.
close_menus
    Closes any open menu.
combo_box
    The `ComboBox` a combo box command holds. `numeric_up_down`, `check_box`
    and `text_box` return the other hosted controls. Each throws
    `std::out_of_range` for an ID that names no command of that kind.

See Also
--------
ToolStrip : A strip of records and hosted controls.
Command : The shared action a command carries.
ContextMenu : The popups the ribbon opens.

Notes
-----
Clicking the selected tab minimizes the command plane, and clicking it again
shows it; clicking another tab selects it and shows the plane (owner
decision 19). There is no minimize button. The minimized ribbon's temporary
popup comes in a later phase.

A large command shows a 32-unit icon over its label and fills the group's
height; small and hosted commands take one of three rows, stacked in
columns. Labels are shown at every size (decision 6); a small command with an
image may be authored icon-only (decision 30). A large split's face is its
icon, and its arrow is the label and glyph beneath; a small split's arrow is
the zone at its right end. Every group is shown at its largest size: groups
past the right edge are clipped until scaling arrives in phase 2.

The ribbon publishes ``region``, ``tab_rail``, ``application_button``,
``application_label``, ``command_plane`` and ``focus``, and three item
systems: ``tab`` (``tab``, ``tab_label``, ``tab_focus``; hot, selected,
focus_visible), ``group`` (``group_region``, ``group_caption``,
``group_divider``, ``group_launcher``, ``group_launcher_glyph``,
``group_launcher_focus``) and ``command`` (``command_face``,
``command_icon``, ``command_label``, ``command_arrow``,
``command_arrow_glyph``, ``command_focus`` and the hosted
``command_combo_box``, ``command_numeric_up_down``, ``command_check_box`` and
``command_text_box``; hot, pressed, checked, open, disabled, focus_visible).
Until the default theme (T4), command faces use the menu-item family (flat
at rest), the plane and tab row the panel family and the File tab the
accent-button family. A command whose enabled or checked state changes
repaints only its own bounds.

Keyboard: the ribbon is one Tab stop holding its own stops: the File tab,
the tabs, then each group's commands and launcher. Left and Right move along
the tab row (selection follows), Down enters the plane, the arrows move
through it, Up or Left from its first stop returns to the tab row, Down opens
a split or drop-down command's menu, Enter and Space activate, Escape closes
a menu or returns to the tab row, and Tab and Shift+Tab move through every
stop and then leave the ribbon. Hosted controls are ordinary Tab stops after
the ribbon. Mnemonics in the File tab's, tabs' and the selected tab's
command texts (``&Home``) open, select or activate them. The ribbon adds
itself to its window's F6 regions.

To accessibility the ribbon is a group holding the File tab (a
`menu_button` with a popup), the tab row (a `tab_group` of positioned tabs)
and, while shown, the command plane (a group named after the selected tab)
with its groups, named by caption, and their commands: `button`,
`split_button` or `menu_button`, with positions in their group and
has-popup, and each launcher as a button. Hosted controls describe
themselves as children of the ribbon.

Not yet: contextual tabs, adaptive scaling, galleries, the Quick Access
Toolbar, the minimized popup, enhanced tooltips, Backstage and the
application menu's recent-items pane (the File tab opens a `ContextMenu`).

Examples
--------
A ribbon with one tab and one group:

.. code-block:: cpp

   std::shared_ptr<gf::Command> paste = std::make_shared<gf::Command>("edit.paste", "&Paste");
   std::shared_ptr<gf::Ribbon> ribbon = gf::make_control<gf::Ribbon>(gf::StableId("main.ribbon"));
   gf::RibbonGroupSpec clipboard{.stable_id = "clipboard", .caption = "Clipboard", .dialog_launcher = true};
   clipboard.commands.push_back({.stable_id = "paste", .command = paste});
   gf::RibbonTabSpec home{.stable_id = "home", .text = "&Home"};
   home.groups.push_back(std::move(clipboard));
   (*ribbon).set_tabs({std::move(home)});
