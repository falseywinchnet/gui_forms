One primitive edit of a dock layout.

A `DockTransaction` is a list of edits applied together by
`DockLayout::apply`: validated first, then applied all or nothing, with one
change published. A drag that moves a panel, closes up the place it left and
opens a new one is one transaction. Build edits with the named factories;
each sets the fields its kind reads. The subject of a moving edit is a
`DockSubject`: one panel (a tab drag) or a whole stack (a caption drag).

Methods
-------
dock_into
    Puts the subject into a stack at an index (past the end appends), or
    into the document area's first document stack. The panel it brings is
    selected; a whole stack brings its selection.
dock_beside
    Puts the subject beside any node of a main or floating site, on a side,
    in a new split where it takes the given share (in (0, 1)). Beside a node
    inside the document area it becomes a document stack.
dock_at_edge
    Puts the subject against the main site's outer edge: a global target.
float_subject
    Puts the subject in a new floating site with a frame in screen logical
    units and a monitor ID; at most 32 floating sites.
auto_hide
    Pins a root-level item out to an edge with a flyout extent: a stack, or a
    panel standing alone in its stack. Only docked tool stacks of the main
    site auto-hide.
unpin
    Returns an auto-hidden stack beside the node it stood beside, else
    against the main site's edge.
hide
    Takes a panel out of its stack and keeps its placeholder.
show
    Places a hidden or never-placed panel: where it was last (its stack and
    index, else beside the same neighbour; its floating frame; its edge),
    else at its descriptor's default placement.
close
    Takes a panel out. A closed document is forgotten; a closed tool panel
    keeps its placeholder.
move
    Moves a panel to another index of its own stack.
select
    Selects a panel in its stack.
set_ratio
    Sets a split's first child's share, in (0, 1).
set_floating_frame
    Sets a floating site's frame and monitor, as the window moves.

See Also
--------
DockLayout : Applies transactions of edits.
DockSubject : A panel or a stack, the subject of a moving edit.

Notes
-----
Every edit is checked against the panel, group and site restrictions and
against the model (unknown IDs, ratios, frames, limits). When an edit is
refused, the transaction's result names its index, the reason and the layer,
and nothing in the transaction is applied.

Removing a panel or a stack prunes the tree: a stack left empty is removed,
a split left with one child is replaced by it, and a floating site left
empty closes. The document area is never removed.

Examples
--------
Tab a panel into another's stack and select it, in one transaction:

.. code-block:: cpp

   gf::DockLayout layout;
   gf::DockPanelDescriptor solution;
   solution.id = gf::DockPanelId("tools.solution");
   layout.register_panel(solution);
   gf::DockPanelDescriptor classes;
   classes.id = gf::DockPanelId("tools.classes");
   layout.register_panel(classes);

   gf::DockTransaction start;
   start.edits.push_back(gf::DockEdit::show(gf::DockPanelId("tools.solution")));
   layout.apply(start);
   gf::DockNodeId stack = layout.locate(gf::DockPanelId("tools.solution")).stack;

   gf::DockTransaction group;
   group.edits.push_back(gf::DockEdit::dock_into(
       gf::DockSubject::of_panel(gf::DockPanelId("tools.classes")), stack));
   group.edits.push_back(gf::DockEdit::select(gf::DockPanelId("tools.solution")));
   gf::DockTransactionResult result = layout.apply(group);   // both or neither
   static_cast<void>(result);
