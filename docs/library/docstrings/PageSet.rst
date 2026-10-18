The pages behind a page strip, a page view or a docking stack, with their order and selection.

A page set holds pages identified by a stable `PageId`, each with a
`PageRecord` (title, image key, closable and pinned flags) and the content
its presenter attached to it. It keeps the order, one selected page and an
activation history, and it knows nothing about controls, painting or hosts:
``PageStrip``, ``PageView`` and docking draw and drive it. Every edit names pages
by identity; positions appear only as `PagePosition` values in the current
order.

Parameters
----------
options
    The fallback policy, whether pinning is allowed, and the capacity (from 1
    to ``PageSetLimits::maximum_capacity``; outside that range the
    constructor throws `std::invalid_argument`).

Properties
----------
options
    The options the set was constructed with.
count
    The number of pages.
pinned_count
    The number of pinned pages; they occupy the first positions.
revision
    Increases by one for every applied edit, selections included.
selected_page
    The selected page. A non-empty set always has one; an empty set has none.
activation_history
    Pages in the order they were last selected, most recent first, with the
    selected page at the front. Pages leave it when they leave the set.
batch_open
    Whether a `PageSetBatch` is open on the set.

Events
------
selecting
    Raised before a requested selection changes, with the old and new page
    and the cause. Setting ``cancel`` keeps the current selection. Handlers
    may not edit the set; their edits are refused with
    ``selecting_in_progress``.
selected_page_changed
    Raised after the selected page changed, including the selections the set
    makes itself: the first page of an empty set (cause ``first_page``) and
    the fallback after the selected page leaves (cause ``fallback``).
pages_changed
    Raised after pages entered, left, moved or changed, with flags saying
    which aspects changed and how many edits are covered.

Methods
-------
find
    The position of a page, or none when it is not in the set.
record_at
    The record at a position; a position past the end throws
    `std::out_of_range`.
content
    The content of a page, borrowed, or null when it has none.
insert
    Adds a page without content, at the end of its group or at a position
    inside it. The first page of an empty set is selected.
put
    Adds a page taken from another set, with its content. On success the
    `TakenPage` is left empty; on a refusal it is unchanged.
remove
    Removes a page and destroys its content. When it was selected, the
    fallback policy selects another page in the same edit.
take
    Removes a page and returns it with its content, without destroying
    anything, for `put` into another set.
move
    Moves a page to a position inside its own group.
set_pinned
    Pins a page (it moves to the end of the pinned group) or unpins it (to the
    start of the ordinary pages). Refused unless pinning is enabled.
set_title
    Changes a page's title.
set_image_key
    Changes a page's image key.
set_closable
    Changes whether a page shows its close affordance.
set_content
    Attaches content to a page that has none (late content).
select
    Raises `selecting`, then selects the page unless a handler cancelled.

See Also
--------
PageSetBatch : Groups several edits into one notification.
PageRecord : The descriptive fields of one page.
TakenPage : A page with its content, between two sets.

Notes
-----
Every edit returns a `PageEditStatus`. ``applied`` and ``unchanged`` succeed;
any other value is a refusal that left the set, its selection, its history and
the caller's arguments exactly as they were. Refusals are ordinary results,
not exceptions.

The pinned group is opt-in (``PageSetOptions::pinning_enabled``). No edit
moves a page across the boundary between pinned and ordinary pages: an insert,
put or move that would is refused with ``crosses_pinned_boundary``, and
pinning or unpinning is the only way across.

When the selected page leaves, the ``neighbour`` fallback selects the page that
took its position, else the one before it (the strip policy); ``most_recent``
selects the most recently selected page still in the set (the document
policy), or the neighbour when there is none.

For one edit the order is: `selecting` (for `select` only, before anything
changes), the edit, the destruction of a removed page's content, then
`pages_changed`, then `selected_page_changed`. Inside a batch only
`selecting` is raised as edits happen. Edits made by handlers of the last two
events are published after the notifications already queued, so every
handler sees changes in the order they were made.

A page chosen from a strip's overflow list takes the last visible position.
The visible count belongs to the strip's layout, so the strip calls `move`
with that position; the set holds the pinned boundary on that path as on
every other.

The set belongs to one UI thread. A handler may not destroy the set it is
handling; if it does, publication stops.

Examples
--------
Three pages, a selection, and a removal that falls back to the next page:

.. code-block:: cpp

   gf::PageSet pages;
   gf::PageRecord home{};
   home.id = gf::PageId{"home"};
   home.title = "Home";
   gf::PageRecord view{};
   view.id = gf::PageId{"view"};
   view.title = "View";
   gf::PageRecord help{};
   help.id = gf::PageId{"help"};
   help.title = "Help";
   gf::PageEditStatus status = pages.insert(home, gf::PagePlacement::end_of_group());
   status = pages.insert(view, gf::PagePlacement::end_of_group());
   status = pages.insert(help, gf::PagePlacement::end_of_group());
   // "home" was selected when it entered the empty set.
   status = pages.select(gf::PageId{"view"}, gf::PageSelectionCause::programmatic);
   status = pages.remove(gf::PageId{"view"});
   // The selection fell back to "help", the page that took its position.
   bool fell_back = *pages.selected_page() == gf::PageId{"help"};
   static_cast<void>(status);
   static_cast<void>(fell_back);

Moving a page with its content between two sets in one batch each:

.. code-block:: cpp

   gf::PageSet left;
   gf::PageSet right;
   gf::PageRecord record{};
   record.id = gf::PageId{"notes"};
   record.title = "Notes";
   gf::PageEditStatus status = left.insert(record, gf::PagePlacement::end_of_group());
   gf::PageTakeResult taken = left.take(gf::PageId{"notes"});
   {
       gf::PageSetBatch batch(right);
       status = right.put(taken.page, gf::PagePlacement::end_of_group());
       status = right.select(gf::PageId{"notes"}, gf::PageSelectionCause::programmatic);
   }
   // One pages_changed and one selected_page_changed were published by
   // `right` when the batch closed.
   static_cast<void>(status);
