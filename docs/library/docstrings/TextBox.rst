An editable text field: one line by default, or a plain-text document with `multiline`.

The user clicks to place the caret, drags or uses Shift with the arrow keys
to select, types to replace the selection, and uses the usual editing and
clipboard shortcuts. In a single-line box Enter raises `committed` and Escape
raises `cancelled`, which suits search fields and inline editors. Set a
`password_character`, or `use_system_password_character`, to mask a secret.
Use a `Label` for text the user only reads, a `ComboBox` to choose from a
list, and a `NumericUpDown` for numbers.

Parameters
----------
stable_id
    Identity of the control, unique within its window. Logs, automation and
    semantic actions address the control by this ID.
text
    Initial text. It must be valid UTF-8, or the constructor throws
    `std::invalid_argument`. The caret starts at its end.

Properties
----------
text : default ``""``
    The UTF-8 content. Setting a different value replaces it, puts the caret
    at the end, clears the undo history and raises `text_changed` and then
    `selection_changed`. It is not limited by `maximum_length`. Invalid
    UTF-8 throws `std::invalid_argument`, and more than 16 MiB throws
    `std::length_error`.
placeholder_text : default ``""``
    Hint drawn in a muted colour while the box is empty, and used as the
    accessible description when none is set. It must be valid UTF-8.
read_only : default ``false``
    Whether the user may only select and copy. Typing, deleting, pasting,
    cutting, undo, redo, `replace_selection` and assistive-technology
    set-value are refused.
multiline : default ``false``
    Whether the box edits several lines. Enter then inserts
    `newline_sequence` instead of raising `committed`; Up and Down move by
    visual line and Page Up and Page Down by a screenful, Home and End go to
    the start and end of the visual line, and Ctrl or Cmd with Home or End to
    the start and end of the text. A multi-line document is limited to 1 MiB
    and 4096 bytes per line, and edits that would exceed the limits are
    refused. Turning it on throws `std::invalid_argument` while the box is
    masked or the text exceeds the limits.
word_wrap : default ``false``
    Whether a multi-line box wraps long lines at word boundaries to its
    width. Wrapped text scrolls only vertically.
newline_sequence : default ``"\n"``
    What Enter inserts in a multi-line box: `"\n"`, `"\r"` or `"\r\n"`; any
    other value throws `std::invalid_argument`. Line endings already in the
    text are kept as they are.
accepts_tab : default ``false``
    Whether Tab inserts a tab character in a multi-line box instead of
    moving focus.
maximum_length : default ``0``
    Largest number of Unicode scalar values the user may produce; zero means
    no limit. An edit, paste or assistive-technology set-value whose result
    would exceed it is refused entirely rather than truncated. Because the
    limit applies to the result, a longer text assigned with `set_text`
    refuses every edit, deletions included, until one edit brings it within
    the limit. Values above 16 Mi throw `std::out_of_range`.
password_character : default ``U'\0'``
    Character drawn once per grapheme in place of the text; `U'\0'` shows the
    text. It must be a single printable Unicode scalar, or the setter throws
    `std::invalid_argument`. A multi-line box cannot be masked.
use_system_password_character : default ``false``
    Whether to mask the text with bullets (U+2022). It takes precedence over
    `password_character`.
password_protected
    Whether the text is masked by either property.
font : default ``FontSpec{FontRole::content, 12.0, 400, false}``
    Font of the text, scaled by the window's text scale. An invalid
    specification throws `std::invalid_argument`.
selection
    The selection as `anchor` and `caret` UTF-8 byte offsets; they are equal
    when nothing is selected. Change it with `select` or `select_all`.
selected_text
    A copy of the selected text, which is empty when nothing is selected.
can_undo
    Whether `undo` would restore an earlier state.
can_redo
    Whether `redo` would reapply an undone edit.

Events
------
text_changed
    Raised after the text changes, by typing, deleting, cutting, pasting,
    undo, redo, a setter or an assistive technology. It carries the new text,
    and is followed by `selection_changed`.
selection_changed
    Raised after the selection or caret moves, and after every text change.
    It carries the new selection.
committed
    Raised when the user presses Enter in a single-line box, even a read-only
    one. It carries the current text.
cancelled
    Raised when the user presses Escape, in either mode. The text is left
    unchanged; restore it in the handler if the edit should be discarded.

Methods
-------
select
    Sets the selection from an anchor and a caret offset. Both must lie on
    grapheme boundaries of the text, or it throws `std::out_of_range`.
select_all
    Selects the whole text.
replace_selection
    Replaces the selection with UTF-8 text as one undoable edit, as typing
    does, and leaves the caret after it. It returns false, changing nothing,
    when the box is read-only, the text is not valid UTF-8, or a limit would
    be exceeded.
delete_selection
    Removes the selection as one undoable edit. It returns false when nothing
    is selected or the box is read-only.
undo
    Restores the text and selection before the last edit, and returns false
    when there is nothing to undo or the box is read-only. At most 128 steps
    and 8 MiB of earlier text are kept; the oldest are dropped first.
redo
    Reapplies the last undone edit, and returns false when there is none or
    the box is read-only. A new edit clears the redo steps.
clear_undo_history
    Forgets all undo and redo steps without changing the text or raising
    events, for example after saving.
copy
    Copies the selection to the clipboard. It returns false when nothing is
    selected, the text is masked, or the window has no host clipboard.
cut
    Copies the selection and then deletes it. It returns false when nothing
    is selected, the box is read-only or masked, or the copy fails.
paste
    Replaces the selection with clipboard text. It returns false when the box
    is read-only, the window has no host clipboard, the clipboard holds no
    text, or the edit is refused.
set_edit_filter
    Installs a filter that sees every user edit (typing, deletion, paste and
    assistive set-value, as a `TextEdit` with its `TextEditKind`) before it
    is applied, and accepts it, refuses it, or rewrites the whole text and
    places the caret. `set_text`, undo and redo do not pass through it. A
    filter may set the text itself only when it refuses the edit, to show a
    value it looked up (a read-only `DomainUpDown` does); an edit during whose
    filter call the text changed is abandoned whatever the verdict. The delegate does not own its target; clear
    it with an empty `Delegate` before the target goes away. `MaskedTextBox`
    uses it to put input through its mask.
visual_line_count
    The number of displayed lines after wrapping; 1 for a single-line box.
validate_multiline_text
    Checks text against the multi-line limits without changing anything,
    returning the first problem found.
set_auto_scroll
    In a multi-line box, true lets the document's extent drive the inherited
    `Panel` scroll bars; a wrapped document overflows only vertically.

See Also
--------
Label : Read-only text that names a field.
ComboBox : A text field with a drop-down list.
NumericUpDown : A field for a number with step buttons.

Notes
-----
Keyboard, while the box has focus: Left and Right move by grapheme, with Ctrl
or Alt by word, and with Cmd (Meta) to the start or end of the text; Home and
End go to the start and end of a single-line box; Shift with any of these
extends the selection. Backspace and Delete remove the selection, or else one
grapheme, a word with Ctrl or Alt, or everything before or after the caret
with Cmd. Ctrl or Cmd with A selects all; with C, X and V they copy, cut and
paste; with Z they undo, and with Shift+Z or Y they redo. Ctrl and Cmd are
both accepted on every platform. Text input replaces the selection.

The box handles Enter and Escape itself. While it has focus, they never reach
the window's accept and cancel buttons; handle `committed` and `cancelled`
instead.

Pointer: pressing the primary button focuses the box and places the caret,
with Shift extending the selection, and dragging selects. In a multi-line box
the wheel scrolls three lines per notch.

The selection is drawn only while the box has focus. The caret blinks every
530 ms while focused; each blink is one scheduled repaint, and nothing is
scheduled when the box does not have focus. Clipboard commands use the
clipboard of the host attached to the window, and return false when there is
none.

The control publishes the accessibility role `text_box`. Its name is the
accessible name only, so give every box one with `set_accessible_name`; the
placeholder becomes the description, not the name. The value is the text, or
empty when masked, together with the protected-content state. It reports
read-only when `read_only` is true, and otherwise offers a set-value action,
which replaces the whole text as one undoable edit within `maximum_length`.

Examples
--------
A search field that acts when the user presses Enter:

.. code-block:: cpp

   struct Search final : gf::Component {
       std::shared_ptr<gf::TextBox> field =
           gf::make_control<gf::TextBox>(gf::StableId("search.query"));
       std::string query;

       Search() {
           (*field).set_placeholder_text("Search");
           (*field).set_accessible_name("Search");
           gf::on((*field).committed(), *this, &Search::on_commit);
       }
       void on_commit(const std::string& text) { query = text; }
   };

   void example() {
       Search search;
       gf::Window window(search.field, gf::Size{200.0, 28.0});
       window.request_focus(search.field);
       window.dispatch_text(gf::TextInputEvent{"retained"});
       window.dispatch_key(gf::KeyEvent{gf::KeyAction::down, gf::PhysicalKey::enter});
       // search.query is now "retained"
   }

Edit a multi-line note. Each replacement is one undo step:

.. code-block:: cpp

   std::shared_ptr<gf::TextBox> note =
       gf::make_control<gf::TextBox>(gf::StableId("note"), "Draft");
   (*note).set_multiline(true);
   (*note).set_word_wrap(true);
   (*note).select_all();
   (*note).replace_selection("First line\nSecond line");
   (*note).undo();   // the text is "Draft" again, and can_redo() is true
