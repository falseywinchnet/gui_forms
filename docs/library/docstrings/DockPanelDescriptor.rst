What a dockable panel declares about itself.

The application registers one descriptor per panel with
`DockLayout::register_panel`, before or after a layout is restored. The
descriptor is all the layout knows of the panel: its stable ID, whether it
is a tool panel or a document, whether a tool panel may also stand among
documents (``dock_as_document``), its dock group, where it may go
(``allowed``), its collapse priority, its minimum size and where
`DockEdit::show` puts it the first time (``default_placement``).

See Also
--------
DockLayout : The layout model the descriptors are registered with.
DockEdit : The edits that move panels.

Notes
-----
``id`` is the application's stable string for the panel, saved in layouts
and used to ask a factory for its content; choose one that survives
versions, such as ``"tools.properties"``. ``kind`` separates tool panels
(placement and visibility) from documents (activation and close prompts);
documents always live in the document area or float.

``allowed`` is the panel layer of the restrictions; every field starts
permitted. ``group`` is the dock group: panels share a stack only when their
groups are equal or one group's `DockGroupDescriptor` admits the other. The
empty group is the default.

``collapse_priority`` orders which tool panels collapse to auto-hide first
when a window becomes too small for every minimum; the layout model only
records it, for the controls that realize the layout. ``minimum_size`` is
used by `DockLayout::minimum_size` and the split solver.

Examples
--------
A tool panel that may not float and never shares a stack:

.. code-block:: cpp

   gf::DockPanelDescriptor toolbox;
   toolbox.id = gf::DockPanelId("tools.toolbox");
   toolbox.allowed.floating = false;
   toolbox.allowed.tabbed = false;
   toolbox.minimum_size = gf::Size{180.0, 120.0};
   toolbox.default_placement.side = gf::DockSide::left;
   toolbox.default_placement.ratio = 0.2;
