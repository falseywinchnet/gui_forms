The value command of a font control: a shared Command and a font choice.

A font command is a ribbon value command (owner decision RB-13). It holds a
shared `Command`, so its stable ID, text, description, enabled and visible
state are that command's, and a committed choice is announced through the
command's `invoked` event. What it adds is a typed `FontChoice`: a family
name and a size in points. An empty family or a zero size is unknown, as for
a selection whose text mixes fonts; a `RibbonFontControl` shows that field
empty.

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
value : default unknown family and size
    The font choice. `set_value` is the program's change: it raises
    `value_changed` when the value differs and invokes nothing. A family with
    invalid UTF-8 or a line break, or a size that is negative, not finite or
    above `FontCommand::maximum_size` (1638), throws `std::invalid_argument`.

Events
------
value_changed
    Raised after the value changes, by `set_value` or `commit`.

Methods
-------
commit
    The user's choice: sets the value (raising `value_changed` when it
    differs), then executes the command with the given source. Returns false,
    changing nothing, while the command is disabled, for a value `set_value`
    refuses, or for one with neither a family nor a size.
valid_value
    Whether `set_value` accepts a choice.

See Also
--------
RibbonFontControl : The control that shows and commits it.
GalleryCommand : The first value command, with live preview.
ColorCommand : The colour picker's value command.

Notes
-----
Events arrive on the UI thread; on a commit `value_changed` comes first and
then the command's `invoked`, so a handler of `invoked` reads the new value.
There is no live preview (owner decision RB-13: previews on gallery
commands only).

Examples
--------
A font command an application applies when it is committed:

.. code-block:: cpp

   std::shared_ptr<gf::Command> font = std::make_shared<gf::Command>("doc.font", "Font");
   std::shared_ptr<gf::FontCommand> command = std::make_shared<gf::FontCommand>(font);
   (*command).set_value({"Portsmouth Rapids", 11.0});   // value_changed, no invocation
   (*command).commit({"Carlito", 11.0}, "doc.font-control");   // value_changed, then font's invoked
