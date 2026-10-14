Four ToolStripPanels around a content panel.

A tool strip container frames a window's content with strip panels on its
top, bottom, left and right. The top and bottom panels span the width and
are as tall as their strips; the left and right panels fill the height
between them and are as wide as their (vertical) strips; the content panel
takes the rest. Join strips to a panel and put the window's controls in the
content panel.

Parameters
----------
stable_id
    Identity of the container, unique within its window. Its panels are
    child controls named ``<stable_id>.top``, ``.bottom``, ``.left``,
    ``.right`` and ``.content``.

Properties
----------
top_panel
    The top `ToolStripPanel`. `bottom_panel`, `left_panel` and `right_panel`
    are the others.
content_panel
    The `Panel` between the strip panels.
top_panel_visible : default ``true``
    Whether the top panel is shown. A hidden panel takes no room and hides
    its strips. `bottom_panel_visible`, `left_panel_visible` and
    `right_panel_visible` do the same for the others.

See Also
--------
ToolStripPanel : One band of strips.
ToolStrip : The strips the panels hold.
StatusStrip : A strip for the bottom panel.

Notes
-----
The container paints nothing of its own; its panels and content do. It
publishes ``region`` and the composed parts ``top_panel``,
``bottom_panel``, ``left_panel``, ``right_panel`` (each a
``tool_strip_panel``) and ``content_panel`` (a ``panel``); an empty or
hidden panel is absent. The content panel comes first among the children,
so Tab reaches the content before the strips. It is a group to
accessibility, exposed when it has an accessible name.

Not supported yet: dragging strips between panels, several strips sharing a
row, and rafting panels outside a container.

Examples
--------
A frame with a top strip and a status strip:

.. code-block:: cpp

   std::shared_ptr<gf::ToolStripContainer> frame =
       gf::make_control<gf::ToolStripContainer>(gf::StableId("frame"));
   std::shared_ptr<gf::ToolStrip> tools =
       gf::make_control<gf::ToolStrip>(gf::StableId("frame.tools"));
   std::shared_ptr<gf::StatusStrip> status =
       gf::make_control<gf::StatusStrip>(gf::StableId("frame.status"));
   (*(*frame).top_panel()).join(tools);
   (*(*frame).bottom_panel()).join(status);
