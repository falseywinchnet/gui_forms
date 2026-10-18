The docking layout model: where every panel of a docking workspace is.

A dock layout describes one main window and the floating windows it owns,
as plain records with no controls, painting or host behind them. The main
site's tree is made of binary splits (two children and a ratio), tool
stacks and exactly one document area, which is never removed. Documents
stand in document stacks inside the document area; a tool panel joins them
only when its descriptor says "dock as document". Floating sites hold their
own trees with a frame in screen logical units and a monitor ID. Tool stacks
pinned out to an edge of the main site are auto-hidden, each with its flyout
extent. Every panel the layout has held keeps a record, the placeholder
that remembers its last docked, floating and auto-hidden places, so a hidden
panel returns where it was.

The layout never creates panels. The application registers a
`DockPanelDescriptor` per panel, and the controls that realize the layout ask
`collect_visible_panels` which panels are in stacks and which are selected:
only those need to exist (lazy creation).

Properties
----------
state
    The whole layout as records (`DockLayoutState`): nodes sorted by ID, the
    main site's root, floating sites, auto-hidden stacks and panel records.
    Read-only; every change goes through `apply` or `restore_xml`.
generation
    Increases by one for every applied transaction and every restore.
document_area
    The ID of the main site's document area.
check_invariants
    Whether the tree obeys every rule (two children per split, ratios in
    (0, 1), no empty stack, one document area, each panel in at most one
    stack, documents only in document stacks, at most 32 floating sites), and
    the first broken rule if not. It holds between transactions; the tests
    use it to prove that.
write_xml
    The layout in the current XML format, `dock_layout_format_version`. The
    same layout always gives the same bytes.

Events
------
changed
    Raised once per applied transaction and once per restore, after the new
    layout is in place, with the new generation, the edits' origin and their
    count. A refused transaction raises nothing. A handler may apply another
    transaction; that one raises its own change.

Methods
-------
register_panel
    Registers a panel's descriptor. Registering does not place the panel:
    `DockEdit::show` does. Returns false for an empty or repeated ID.
find_descriptor
    The registered descriptor for a panel ID, or null.
set_group
    Adds or replaces a dock group's descriptor: which other groups may share
    a stack with its panels.
set_main_site_restrictions
    What the main site accepts: tools, documents, dock groups.
set_floating_site_restrictions
    What every floating site accepts.
set_user_docking_locked
    While locked, transactions of user origin that move panels are refused;
    selection, ratios and floating frames still change, and the program's
    own transactions are unaffected.
set_main_window_frame
    Records the main window's frame, monitor and maximized state, to be saved
    and restored with the layout. Not a layout change.
set_application_version
    The application's own layout version, saved with the layout.
apply
    Applies a `DockTransaction`: every edit is applied in order to a copy and
    checked against the restrictions; only when all are accepted and the tree
    is valid is the copy committed and `changed` raised. Otherwise the result
    names the refused edit, the reason and the restriction layer (panel,
    group, site or model), and nothing changes.
check
    What `apply` would answer, without committing; for deciding whether a
    drop target is offered.
find_node
    A node by ID, or null.
find_floating_site
    A floating site by ID, or null.
locate
    Where a panel is: docked, floating, auto-hidden, hidden (a placeholder)
    or absent, with its stack, index and floating site.
pin_owner
    The root-level item whose pin governs a panel: its stack, when that is a
    docked tool stack of the main site or auto-hidden. A panel inside a group
    has no pin of its own; floating panels and documents have none.
collect_pinnable_items
    Fills a vector with every stack that carries a pin.
collect_visible_panels
    Fills a vector with every panel in a stack, in tree order, saying whether
    it is selected and whether it is docked, floating or auto-hidden.
minimum_size
    The smallest size a subtree can take, from its panels' minimum sizes and
    the seam thickness.
arrange
    Lays out a subtree in a rectangle with `solve_dock_split`, giving every
    node its rectangle.
restore_xml
    Replaces the layout with a saved one; see Notes.

See Also
--------
DockEdit : The primitive edits a transaction is made of.
DockPanelDescriptor : What a panel declares: kind, group, allowed placements.
XmlElement : The XML subset layouts are saved in.

Notes
-----
Restrictions come in three layers, checked by every edit. The panel layer
is the descriptor's `DockAllowedPlacements` and its kind; the group layer
lets two panels share a stack only when their dock groups are equal or one
group's `DockGroupDescriptor` admits the other (the default group is the
empty ID, so programs that ignore groups are unaffected); the site layer is
`DockSiteRestrictions` and the user lock.

Pins belong to root-level items (a stack standing in the tree), never to a
panel inside a group: `DockEdit::auto_hide` of a panel that shares its stack
is refused with `not_root_level`, and auto-hiding the stack takes the whole
group. Tabbing a panel into a docked stack makes it part of a group whose pin
is not auto-hidden.

`restore_xml` reads and checks the whole record before changing anything.
An unreadable, invalid or rejected record leaves the layout as it was (at
startup, the program's default layout) and reports why. An older format is
migrated step by step (format 1 to 2 is built in); a newer one is read as far
as it is understood. Unknown elements and attributes (a newer format's
additions) are reported and kept (owner decision DK-48), attached to the
known element they were found on (`DockLayoutState::unknown_fields`), and
saving writes them back there: the attributes after the known ones, each
unknown child element whole, after as many known children as came before it
in the file. A round trip through this framework therefore gives a newer
file's text back except for these normalizations: the ``format`` attribute
is the one this framework writes (2), comments are not kept, attributes and
elements are written in the writer's own layout and indentation, and an
unknown child is placed by its count of known siblings, so it may move when
those siblings change. An edit that removes an element (a pruned node, a
hidden panel's ``<panel>``, a memory no longer held) drops that element's
unknown fields and lists them in `DockTransactionResult::dropped_unknown_fields`;
a restore that removes one reports ``unknown_fields_dropped``. The fields of
a ``<panel>`` follow the panel to the stack that holds it. An unknown
element that holds ``<panel>`` references is not kept but dropped, and the
panels in it become placeholders, so that a panel shown again is never
written twice. A different application layout
version calls the program's `DockLayoutUpgradeHook`, which may rename panels
with `rename_dock_panel` or reject the layout. A panel ID with no registered
descriptor (a removed plugin) is kept as a placeholder, so registering the
plugin later and showing its panel puts it back. Saved frames go through
`place_on_monitor`: the same monitor ID, else the monitor the frame overlaps
most, else the primary, then clamped so the caption can be reached. The
restore is one change.

The model has one thread, its owner's; nothing in it is synchronized.

Examples
--------
Register panels, show them, then tear one off and save:

.. code-block:: cpp

   gf::DockLayout layout;
   gf::DockPanelDescriptor solution;
   solution.id = gf::DockPanelId("tools.solution");
   solution.default_placement.side = gf::DockSide::left;
   layout.register_panel(solution);
   gf::DockPanelDescriptor readme;
   readme.id = gf::DockPanelId("doc:readme");
   readme.kind = gf::DockPanelKind::document;
   layout.register_panel(readme);

   gf::DockTransaction start;
   start.edits.push_back(gf::DockEdit::show(gf::DockPanelId("tools.solution")));
   start.edits.push_back(gf::DockEdit::show(gf::DockPanelId("doc:readme")));
   gf::DockTransactionResult shown = layout.apply(start);   // one change for both

   gf::DockTransaction tear_off;
   tear_off.edits.push_back(gf::DockEdit::float_subject(
       gf::DockSubject::of_panel(gf::DockPanelId("tools.solution")),
       gf::Rect{200.0, 120.0, 320.0, 480.0}, "DISPLAY1"));
   gf::DockTransactionResult floated = layout.apply(tear_off);
   std::string saved = layout.write_xml().text;
   static_cast<void>(shown);
   static_cast<void>(floated);
   static_cast<void>(saved);

A restriction refuses an edit and leaves the layout unchanged:

.. code-block:: cpp

   gf::DockLayout layout;
   gf::DockPanelDescriptor output;
   output.id = gf::DockPanelId("tools.output");
   output.allowed.floating = false;
   layout.register_panel(output);
   gf::DockTransaction show;
   show.edits.push_back(gf::DockEdit::show(gf::DockPanelId("tools.output")));
   layout.apply(show);

   gf::DockTransaction tear_off;
   tear_off.edits.push_back(gf::DockEdit::float_subject(
       gf::DockSubject::of_panel(gf::DockPanelId("tools.output")),
       gf::Rect{0.0, 0.0, 300.0, 200.0}, ""));
   gf::DockTransactionResult refused = layout.apply(tear_off);
   bool panel_layer = refused.refusal.layer == gf::DockRefusalLayer::panel;   // true
   static_cast<void>(panel_layer);
