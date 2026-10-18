A docking workspace: the layout, the panels and their factories, and which panel and document are active.

A workspace owns a `DockLayout` and, for every panel the program registers,
its title and a factory that builds its content the first time the panel is
shown. It keeps two activation tracks: the active panel (the panel that has,
or last had, keyboard focus, tool or document) and the active document (the
document last active), each with a most-recently-active history. A
`DockSite` shows the workspace in a window. Use it for an application with
tool panels around a document area.

Parameters
----------
stable_id
    The workspace's identity, used in diagnostics.

Properties
----------
layout
    The layout. Restrictions, dock groups, the workspace lock, saving and
    restoring go through it directly; register panels through
    `register_panel` so that their titles and factories are known.
created_panel_count
    The number of factory runs that produced content.
active_panel : default none
    The active panel.
active_document : default none
    The active document. Activating a tool panel leaves it unchanged.
tool_history
    The placed tool panels that have been active, most recent first.
document_history
    The placed documents that have been active, most recent first.
site : default none
    The `DockSite` showing the workspace, or none.
flyout_opening : default ``DockFlyoutOpening::hover_or_click``
    What opens an auto-hidden panel's flyout from its side tab (owner
    decision DK-27, E1-1): resting the pointer on the tab or clicking it, or
    (``click``) only clicking it.

Events
------
activation_changed
    Raised after a track changed, with the track, the old and new panel and
    the cause; when one activation changes both, the panel track is raised
    first.

Methods
-------
register_panel
    Registers a panel's descriptor with the layout and keeps its title and
    factory; registering places nothing. False, changing nothing, for an
    empty or repeated identity.
find_registration
    The registration of a panel, or null.
set_panel_title
    Retitles a panel's caption, tab and navigator row; false for an
    unregistered panel.
panel_title
    The registered title, or the identity when there is none.
apply
    Applies a transaction to the layout, as `DockLayout::apply` does.
show_panel
    Shows a hidden or new panel where it was last and activates it; a
    placed panel is only activated.
hide_panel
    Hides a panel, keeping its placeholder and its content.
close_panel
    Closes a panel and releases its content. A closed tool panel keeps its
    placeholder and reopens where it was; a closed document leaves.
panel_content
    The panel's content, or null before its factory ran.
create_panel_content
    Runs the panel's factory if it has no content yet and returns the
    content.
activate_panel
    Makes a panel docked in the main site active: selects it in its stack,
    creates its content, moves focus into it through the site and updates
    the tracks. A panel behind a side tab (auto-hidden, or in a stack
    collapsed by shrinking) opens in its flyout with focus inside; an
    auto-hidden stack's selection is not edited. False, changing nothing,
    for any other panel.

See Also
--------
DockSite : Shows the workspace in a window.
DockLayout : The layout model the workspace owns.
DockNavigator : The Ctrl+Tab switcher over the two histories.

Notes
-----
Lazy creation (owner decision DK-34): a factory runs the first time its
panel is shown selected in a stack the site shows or in a flyout. A panel
behind another tab, hidden, behind a side tab whose flyout has not opened,
in a stack hidden by the maximized document area, or floating (phase D4
shows those) is not created. Hiding keeps the content; closing releases it,
and a closed panel shown again is built again by its factory.

Command routing (owner decision DK-29): the workspace names, on its site's
window, the command target that the active panel's content control carries
(`Control::set_command_target`) for the active-panel stage, and the active
document's for the active-document stage. A document command such as Save
bound as a shortcut therefore reaches the active document while focus is in
a tool panel.

After every layout change the workspace drops panels that left the layout
from its histories, releases the content of a document that left, and
replaces an active document that left with the next one in its history
(cause ``fallback``) and an active panel that left with the active
document. The site realizes the change after that. A factory is a named
delegate; its object must outlive the workspace.

Examples
--------
Two documents and a tool panel whose controls are made only when shown:

.. code-block:: cpp

   struct Views final {
       int made = 0;
       void make(gf::DockPanelCreation& creation) {
           ++made;
           creation.content = gf::make_control<gf::TextBox>(gf::StableId("view." + creation.panel.value));
       }
   };

   void example() {
       Views views;
       gf::DockWorkspace workspace("editor");
       const gf::Delegate<gf::DockPanelCreation&> factory =
           gf::Delegate<gf::DockPanelCreation&>::bind<Views, &Views::make>(views);
       gf::DockPanelRegistration outline{};
       outline.descriptor.id = gf::DockPanelId("outline");
       outline.descriptor.default_placement.side = gf::DockSide::left;
       outline.title = "Outline";
       outline.factory = factory;
       static_cast<void>(workspace.register_panel(outline));
       const std::array<std::string_view, 2> files{"a.txt", "b.txt"};
       for (const std::string_view file : files) {
           gf::DockPanelRegistration document{};
           document.descriptor.id = gf::DockPanelId(std::string(file));
           document.descriptor.kind = gf::DockPanelKind::document;
           document.title = std::string(file);
           document.factory = factory;
           static_cast<void>(workspace.register_panel(document));
           static_cast<void>(workspace.show_panel(document.descriptor.id));
       }
       static_cast<void>(workspace.show_panel(gf::DockPanelId("outline")));
       // views.made is 0: nothing is shown until a site lays the workspace out.
       // The active panel is the outline and the active document b.txt.
   }
