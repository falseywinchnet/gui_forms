A list-and-details layout that shows both side by side on wide windows and one at a time on narrow ones.

A master/detail view holds two controls that you supply: a master, such as a
list of items or categories, and a detail that shows the selected item. On a
wide window they sit side by side, divided by a `SplitContainer` seam; when
the window is narrower than the compact threshold, the view shows only one of
them, and your application switches between them with `show_detail` and
`show_master`. Use it for settings pages, inspectors and browsers. Use a
`SplitContainer` directly when both panes must always be visible.

Parameters
----------
stable_id
    Identity of the view, unique within its window. Its split container
    takes the identity ``<stable_id>.split``.

Properties
----------
master
    The control in the master role, or null. `set_master(control)` docks the
    control to fill the first pane and returns the control it replaced,
    detached, or null; passing null empties the role. A control that is
    already the detail, already has a parent, or is part of the view's own
    composition throws `std::logic_error` and changes nothing. Setting the
    current master again does nothing.
detail
    The control in the detail role, or null. `set_detail` has the same rules
    as `set_master`, for the second pane.
split_container
    The `SplitContainer` that holds the two roles.
master_detail_layout : default ``MasterDetailLayout{}``
    The explicitly set layout: orientation, master extent, seam width and hit
    width, master and detail minimums, compact threshold, and whether the
    user may resize. Setting it makes it the layout in use and stops the
    view from following the theme. Every extent must be finite and between 0
    and 16384, the seam width positive, the hit width at least the seam
    width, and the master extent at least the master minimum; otherwise
    `std::invalid_argument` is thrown.
uses_theme_layout : default ``true``
    Whether the layout in use comes from the effective theme's structural
    geometry: its navigation extent (260 by default) and minimum (120),
    content minimum (240), splitter widths (3 and 9) and compact breakpoint
    (720), with a vertical, resizable split. Setting
    `master_detail_layout` turns it off;
    `reset_master_detail_layout_to_theme` turns it back on.
effective_master_detail_layout
    The layout in use: the theme's while `uses_theme_layout` is true,
    otherwise `master_detail_layout`.
display_mode : default ``MasterDetailDisplayMode::automatic``
    `MasterDetailDisplayMode::automatic` chooses the presentation from the
    view's size. `side_by_side`, `master_only` and `detail_only` force one
    presentation whatever the size. An unknown value throws
    `std::invalid_argument`.
effective_display_mode : default ``MasterDetailDisplayMode::side_by_side``
    The presentation applied by the last layout: `side_by_side`,
    `master_only` or `detail_only`.
compact_detail_visible : default ``false``
    Which role the automatic mode shows when the view is compact: the detail
    when true, the master when false. `show_detail` and `show_master` set it.

Events
------
presentation_changed
    Raised during layout when `effective_display_mode` changes, after the
    panes have been collapsed or restored. It carries the previous and the
    current presentation, and `automatic` is true when `display_mode` is
    `automatic`.

Methods
-------
show_detail
    Shows the detail when the view is compact; sets `compact_detail_visible`
    to true. A side-by-side view is not affected until it becomes compact.
show_master
    Shows the master when the view is compact; sets `compact_detail_visible`
    to false.
reset_master_detail_layout_to_theme
    Makes the view follow the theme's layout again.
initialize_control_tree
    Adds the split container as the view's child. `make_control` calls it;
    you do not need to.

See Also
--------
SplitContainer : Two panes with a draggable seam, always both visible.
ResponsiveTrackPanel : Regions that collapse in an authored order.

Notes
-----
In automatic mode the view is side by side while its width, or its height
for a horizontal layout, is at least the larger of the compact threshold and
the master minimum plus seam width plus detail minimum. Below that it shows
the master, or the detail if `compact_detail_visible` is true. The mode is
applied when the view is arranged, so `effective_display_mode` and
`presentation_changed` follow the next layout pass, not the call that
changed the size or the mode.

A compact presentation collapses the other role's pane in the split
container, which sets that pane's `visible` property to false. If focus was
inside the role that disappears, it moves to the seam.

Each layout pass applies the effective layout to `split_container`: its
orientation, seam widths, pane minimums, whether the seam is fixed, a fixed
first pane, and `master_extent` as the splitter distance. A distance set on
the split container, by the program or by the user dragging the seam, is
replaced at the next layout pass.

The view measures, side by side, to the two roles' preferred sizes plus the
seam; in a compact presentation, to the shown role's preferred size.

The view publishes the accessibility role `group` only when it has an
accessible name or description, with the value ``side-by-side``, ``master``
or ``detail``.

Persistent side-panel layouts in shipped applications other than File
Manager are ruled out by ADR-013; such applications use owned dialogs for
secondary tools.

Examples
--------
A settings window whose owner learns when the view turns compact:

.. code-block:: cpp

   struct Settings final : gf::Component {
       std::shared_ptr<gf::MasterDetailView> view =
           gf::make_control<gf::MasterDetailView>(gf::StableId("settings"));
       bool compact = false;

       Settings() {
           static_cast<void>((*view).set_master(
               gf::make_control<gf::Panel>(gf::StableId("settings.categories"))));
           static_cast<void>((*view).set_detail(
               gf::make_control<gf::Panel>(gf::StableId("settings.page"))));
           gf::on((*view).presentation_changed(), *this, &Settings::on_presentation);
       }
       void on_presentation(const gf::MasterDetailPresentationChange& change) {
           compact = change.current != gf::MasterDetailDisplayMode::side_by_side;
       }
   };

   void example() {
       Settings settings;
       gf::Window window(settings.view, {900.0, 420.0});
       window.perform_layout();          // side by side
       window.resize({520.0, 420.0});
       window.perform_layout();          // master only; settings.compact is true
       (*settings.view).show_detail();
       window.perform_layout();          // detail only
   }
