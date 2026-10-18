The value command of a gallery: a shared Command, the chosen item, and live preview.

A gallery command is the first of the ribbon's value commands (owner decision
RB-13). It holds a shared `Command`, so its stable ID, text, description,
icon, enabled and visible state are that command's, a button or menu row
bound to the same command shows the same state, and a committed choice is
announced through the command's `invoked` event. What it adds is a typed
value, the stable ID of the chosen gallery item, and the live-preview events
a `RibbonGallery` raises while the pointer or keyboard focus rests on an item.

Parameters
----------
command
    The shared `Command`. A null command throws `std::invalid_argument`.

Properties
----------
command
    The shared `Command`.
stable_id
    The command's stable ID.
state
    The command's state.
value : default none
    The chosen item's stable ID. `set_value` is the program's change: it
    raises `value_changed` when the value differs and invokes nothing. An empty
    or invalid ID throws `std::invalid_argument`.
previewing
    The item an outstanding preview shows, or none.

Events
------
value_changed
    Raised after the value changes, by `set_value` or `commit`.
preview_requested
    Raised when a gallery asks for a preview of an item, with the item and the
    gallery's stable ID. A preview of another item replaces the outstanding
    one; no cancellation is raised between them.
preview_cancelled
    Raised when the outstanding preview is withdrawn, with its item.

Methods
-------
commit
    The user's choice: ends any outstanding preview without a cancellation,
    sets the value (raising `value_changed` when it differs), then executes
    the command with the given source. Returns false, changing nothing, while
    the command is disabled or for an empty or invalid ID.
commit_routed
    As `commit`, routing the command through a window's command chain
    (`Command::execute_routed`, decision DK-47); a `RibbonGallery` with a
    window commits this way.
preview
    Asks for a preview of an item. Returns false, raising nothing, while the
    command is disabled, for an invalid ID, or when that item is already
    previewed.
cancel_preview
    Withdraws the outstanding preview, if any.

See Also
--------
RibbonGallery : The gallery that commits and previews through it.
Command : The shared identity and state.

Notes
-----
The value commands that follow (spinner, combo box, colour and font) take
the same shape: a shared `Command`, one typed value, `set_value` for the
program's changes, `commit` for the user's choice, and a refused commit while
the command is disabled. Only the gallery command has previews (owner
decision RB-13: preview events on gallery value commands only).

Events arrive on the UI thread, in this order: previews and their
cancellation as the user moves; on a commit, `value_changed` and then the
command's `invoked`, so a handler of `invoked` reads the new value. A preview
is never an undo step: the application shows it and restores the document
when it is cancelled.

Examples
--------
A styles command whose previews and choices an application follows:

.. code-block:: cpp

   std::shared_ptr<gf::Command> styles = std::make_shared<gf::Command>("doc.styles", "Styles");
   std::shared_ptr<gf::GalleryCommand> gallery = std::make_shared<gf::GalleryCommand>(styles);
   (*gallery).preview("style.heading", "doc.gallery");   // preview_requested
   (*gallery).commit("style.heading", "doc.gallery");    // value_changed, then styles' invoked
