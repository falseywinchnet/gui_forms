The tab headers of a page set in one row, at the top or the bottom of their content.

A page strip shows the pages of a `PageSet` as tabs, in the set's order, and
shows and changes the set's selection. It has no content area of its own:
`PageView` puts one under a strip, and a docking workspace uses a strip alone
for its auto-hide edges. Tabs are sized to their labels (or fixed, or sharing
the row), and when they do not fit the strip shows scroll buttons and a list
button at its end. The arrow keys move between tabs and select as they move,
unless the strip is set to manual selection.

Parameters
----------
stable_id
    Identity of the control, unique within its window. Tabs are published to
    accessibility as ``<stable_id>.tab.<page id>``.
pages
    The set whose pages the strip shows. The strip borrows it: the set must
    stay alive while the strip is bound to it, and its owner calls
    `release_page_set` first if the strip may outlive it.

Properties
----------
page_set
    The bound set, or null after `release_page_set`.
placement : default ``PageStripPlacement::top``
    The edge of the content the strip sits on, top or bottom. The tabs touch
    that edge, and the selected tab's seam lies along it.
item_sizing : default ``PageItemSizing::content``
    ``content`` sizes each tab to its icon and label plus padding, within the
    minimum and maximum widths; ``fixed`` gives every tab the fixed width;
    ``fill`` shares the row's width equally, within the minimum and maximum.
item_widths : default minimum ``40``, maximum ``240``, fixed ``120``
    The tab widths in logical units at text scale 1. `set_item_widths`
    throws `std::invalid_argument` unless 0 < minimum <= maximum <= 1024 and
    0 < fixed <= 1024, and keeps the old widths.
activation : default ``PageStripActivation::automatic``
    Whether the arrow keys, Home and End select the tab they move to
    (automatic) or only move the keyboard's position, Space or Enter then
    selecting it (manual, for pages that are slow to show).
mnemonics_enabled : default ``true``
    Whether ``&`` in a title marks a mnemonic: Alt and the letter select the
    page and focus its tab, and the marker is not shown. When off, titles are
    shown exactly as written.
font
    The tab label font.
image_list
    The images that pages' image keys name; a tab whose key is in the list
    shows the image before its label. `set_image_list` throws
    `std::invalid_argument` for a disposed list or one that belongs to
    another window.
overflowing
    Whether the tabs do not all fit, so the scroll and list buttons show.
visible_pages
    The pages whose tabs are laid out, in order.
focused_page
    The page the keyboard is on while the strip has focus, or none.
page_list_open
    Whether the page list is open.

Events
------
page_invoked
    Raised when a tab is clicked, chosen with the keyboard, by its mnemonic,
    from the page list or through accessibility, after its page is selected
    (it may have been selected already). It is not raised for a programmatic
    selection, nor when a handler of the set's ``selecting`` cancelled.
page_close_requested
    Raised when a closable page's close is asked for; in this phase only the
    accessibility ``close`` action asks. The strip never removes the page:
    the set's owner decides, and sets ``handled`` when it acts.

Methods
-------
release_page_set
    Unbinds the set. The strip closes the page list, shows no tabs, and its
    page operations do nothing from then on.
ensure_visible
    Scrolls so a page's tab is shown; false for a page not in the set.
scroll_backward
    Scrolls the row one tab towards the start; false at the start.
scroll_forward
    Scrolls the row one tab towards the end; false when the last tab already
    ends the row.
page_at
    The page whose tab is under a point in window coordinates, or none.
show_page_list
    Opens the list of every page, with the selected one checked; false when
    the strip is not overflowing or not in a window.
close_page_list
    Closes the page list.
select_relative
    Selects the next (+1) or previous (-1) page, wrapping, and raises
    `page_invoked`.
tab_identity
    The accessibility identity of a page's tab.

See Also
--------
PageSet : The pages, their order and their selection.
PageView : A strip with the selected page's content below it.
TabControl : The Windows Forms tab control.

Notes
-----
Layout keeps the selected page visible: a selection, a change to the pages,
a resize or a change of setting scrolls the selected tab (under manual
selection, the keyboard's tab) into view. The scroll buttons and the mouse
wheel scroll by one whole tab and may move the selected tab out of view until
the next of those. A page chosen from the list is selected and scrolled into
view; its place in the order does not change.

A press on a tab selects first and then focuses the strip. The order of
events is the set's ``selecting``, its ``selected_page_changed``, then
`page_invoked`; a cancelled selection raises nothing more and leaves the
keyboard's tab where it was. Ctrl+PageDown and Ctrl+PageUp select the next
and previous page while the strip has focus.

Every page is published to accessibility as a tab of the strip's tab list
(role ``tab_group``, horizontal): those scrolled out of view are published
without bounds and without the visible state, so every tab can be reached.
Tabs carry their position in the set and offer ``select``, and ``close`` when
the page is closable; they never offer ``press``. While the strip overflows,
its scroll buttons and its list button (a ``menu_button`` with a popup)
follow the tabs.

The tabs are the ``page`` item system: each tab has the parts ``tab``,
``tab_icon``, ``tab_label``, ``tab_seam`` (the selected tab only) and
``tab_focus``, the states hot, pressed, selected, disabled and focus-visible,
and the custom state ``pinned``. They are painted through the by-role paint
path with the menu-item family, flat at rest, until the default theme draws
the tab stack. Painting reads a layout cached between changes and allocates
nothing per tab.

Examples
--------
A settings strip whose arrow keys only move the focus, Space selecting:

.. code-block:: cpp

   gf::PageSet pages{};
   const std::array<std::string_view, 3> titles{"&General", "&Advanced", "Ab&out"};
   for (std::size_t index = 0; index < titles.size(); ++index) {
       gf::PageRecord record{};
       record.id = gf::PageId{"settings." + std::to_string(index)};
       record.title = std::string(titles[index]);
       static_cast<void>(pages.insert(record, gf::PagePlacement::end_of_group()));
   }
   std::shared_ptr<gf::PageStrip> strip =
       gf::make_control<gf::PageStrip>(gf::StableId("settings.tabs"), pages);
   (*strip).set_activation(gf::PageStripActivation::manual);
   static_cast<void>((*strip).select_relative(1, gf::PageSelectionCause::programmatic));
   // pages.selected_page() is now "settings.1", titled "&Advanced".
