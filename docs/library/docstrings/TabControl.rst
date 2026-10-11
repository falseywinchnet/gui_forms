A set of pages shown one at a time, chosen from a row of tabs.

A tab control holds `TabPage` controls and shows the selected one in its
display area, with a tab for every page along one edge. The user selects a
page by clicking its tab, with the arrow keys, Home and End while the tab
strip has focus, or with Ctrl+Tab and Ctrl+Shift+Tab from anywhere inside
the control. Use `ExpandableSections` when several parts should be visible
at once, and a `SplitContainer` to show two panes side by side.

Parameters
----------
stable_id
    Identity of the control, unique within its window. Each tab is
    published to accessibility as ``<page stable id>.tab``.

Properties
----------
pages
    The pages in tab order. Only pages that are still children of the
    control are listed.
page_count
    The number of pages.
selected_index
    The index of the selected page, or empty when there are no pages.
    `set_selected_index` hides the old page, shows the new one and raises
    `selected_index_changed`; an index outside the pages throws
    `std::out_of_range`. Selecting the current page does nothing.
selected_tab
    The selected page, or null. `set_selected_tab` selects a page of this
    control and throws `std::invalid_argument` for any other page.
alignment : default ``TabAlignment::top``
    The edge the tabs run along: top or bottom with tabs side by side, left
    or right with tabs stacked. The display area takes the rest.
appearance : default ``TabAppearance::normal``
    How tabs are drawn: `TabAppearance::normal` joins the selected tab to
    the page, `TabAppearance::buttons` insets the unselected tabs like
    buttons, and `TabAppearance::flat_buttons` gives the unselected tabs a
    lighter face.
item_size : default ``{120.0, 30.0}``
    The size of every tab, in logical pixels before the window's text
    scale. Smaller than 24 by 18, or not finite, throws
    `std::invalid_argument`.
style
    The colours of the tab strip and the page frame. The default is the
    built-in `BasicControlStyle`; it does not follow the window's theme.
display_bounds
    The area given to the selected page, in the control's coordinates. With
    the default alignment it starts one pixel above the bottom of the tab
    strip.

Events
------
selected_index_changed
    Raised after the selection changes, by the user or by a setter, with a
    `TabSelectionChange` holding the old and new index (either may be
    empty). By then the old page is hidden and the new page shown, and
    after `set_selected_index` or user input, focus has been moved as
    described in the Notes. Adding the first page raises it with no old
    index.
    Removing a page raises it when the selected page or its index changes,
    so removing a page before the selected one raises it although the same
    page stays selected.

Methods
-------
add_page
    Adds a page at the end, making it a child of the control. The first
    page added is selected; later pages are hidden until selected. Adding a
    page that is already here does nothing; a null page throws
    `std::invalid_argument`; the errors of `add_child` also apply.
remove_page
    Removes a page and returns it, or returns null when it is not a page of
    this control. When the selected page is removed, the page that takes its
    index, or else the last page, is selected and shown.
page_at
    The page at an index; an index outside the pages throws
    `std::out_of_range`.
tab_bounds
    The rectangle of the tab at an index, in the control's coordinates; an
    index outside the pages throws `std::out_of_range`.

See Also
--------
TabPage : One page of a tab control.
TabAlignment : The edge the tabs run along.
TabAppearance : How the tabs are drawn.
ExpandableSections : Several collapsible parts visible at once.

Notes
-----
Each page is arranged into `display_bounds`, and the control shows only the
selected page and hides the rest, so a hidden page's controls are neither
painted, hit-tested nor focusable. The control sets each page's `visible`
whenever it lays out its pages, so do not set it yourself. Children added
with `add_child`
instead of `add_page` are not arranged.

Every tab is `item_size` wide (or tall, for left and right alignment), and
the tabs do not wrap or scroll: tabs beyond the control's edge are cut off.
Captions are drawn as written; `&` has no mnemonic meaning here.

A primary press on a tab focuses the control and selects the tab
immediately. The control takes focus as one tab stop. While it has focus,
Left and Right (top and bottom alignment) or Up and Down (left and right
alignment) select the previous or next tab, wrapping around, and Home and
End select the first and last. Ctrl+Tab and Ctrl+Shift+Tab (Meta works in
place of Ctrl) cycle forwards and backwards, wrapping around, while focus is
anywhere inside the control. When the selection changes while focus is
inside the old page, the control remembers that focused control and moves
focus to the control last focused in the new page, or else to the tab
strip. A click on a tab moves focus to the tab strip before it selects, so
it does not remember the old page's focus.

The control is published to accessibility as a `tab_group` whose value is
the selected page's caption. Each page has a virtual `tab` child named by
its caption, with the selected state; its focus, select and press actions
all focus the control and select that tab. The selected page is published
as a `group` named by its caption.

The control does not scroll; leave `auto_scroll` off and scroll the pages
instead.

Examples
--------
Build a two-page settings dialog and select its second page:

.. code-block:: cpp

   std::shared_ptr<gf::TabControl> tabs =
       gf::make_control<gf::TabControl>(gf::StableId("settings.tabs"));
   std::shared_ptr<gf::TabPage> general =
       gf::make_control<gf::TabPage>(gf::StableId("settings.general"), "General");
   std::shared_ptr<gf::TabPage> advanced =
       gf::make_control<gf::TabPage>(gf::StableId("settings.advanced"), "Advanced");
   std::shared_ptr<gf::CheckBox> wrap =
       gf::make_control<gf::CheckBox>(gf::StableId("settings.general.wrap"), "&Wrap lines");
   (*wrap).set_requested_bounds({12.0, 12.0, 160.0, 24.0});   // in the page's coordinates
   (*general).add_child(wrap);
   (*tabs).add_page(general);    // the first page is selected
   (*tabs).add_page(advanced);
   (*tabs).set_selected_tab(advanced);

Follow the selection with an owned handler, and change it from the keyboard:

.. code-block:: cpp

   struct Settings final : gf::Component {
       std::shared_ptr<gf::TabControl> tabs =
           gf::make_control<gf::TabControl>(gf::StableId("settings.tabs"));
       std::size_t shown = 0;

       Settings() {
           gf::on((*tabs).selected_index_changed(), *this, &Settings::on_page);
       }
       void on_page(const gf::TabSelectionChange& change) {
           if (change.new_index) {
               shown = *change.new_index;
           }
       }
   };

   void example() {
       Settings settings;
       (*settings.tabs).add_page(
           gf::make_control<gf::TabPage>(gf::StableId("settings.general"), "General"));
       (*settings.tabs).add_page(
           gf::make_control<gf::TabPage>(gf::StableId("settings.advanced"), "Advanced"));

       gf::Window window(settings.tabs, {480.0, 300.0});
       window.perform_layout();
       window.request_focus(settings.tabs);
       window.dispatch_key({gf::KeyAction::down, gf::PhysicalKey::right});
       // the second page is selected: settings.shown is 1
   }
