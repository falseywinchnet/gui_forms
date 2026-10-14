A mask and the text assigned to it, without a control.

A masked text provider parses a mask such as ``(000) 000-0000`` into
positions: each mask character makes one editable position or one literal.
Edits place characters into the editable positions, test each one against
its position, and leave the provider unchanged when any character is
refused. `MaskedTextBox` keeps its text in one; a program can use one on its
own to check or format masked input.

Parameters
----------
mask : default empty
    The mask. ``0`` is a required digit, ``9`` an optional digit or space,
    ``#`` an optional digit, space, ``+`` or ``-``; ``L`` a required and
    ``?`` an optional ASCII letter; ``&`` a required and ``C`` an optional
    character (any non-control character); ``A`` a required and ``a`` an
    optional ASCII letter or digit. ``.``, ``,``, ``:``, ``/`` and ``$`` are
    the culture's decimal separator, group separator, time separator, date
    separator and currency symbol, as literals. ``<`` lower-cases and ``>``
    upper-cases the letters typed into the positions after it, ``|`` stops
    converting. ``\`` makes the next character a literal; every other
    character is a literal. A mask that is not valid UTF-8, ends in a lone
    backslash or holds a control character throws `std::invalid_argument`.
culture : default the invariant culture
    The symbols of `MaskCulture`; each scalar of a symbol is one literal
    position. An empty symbol or invalid UTF-8 throws
    `std::invalid_argument`.

Properties
----------
mask
    The mask as given.
culture
    The culture symbols.
length
    The number of positions, literal and editable.
edit_position_count
    The number of editable positions.
assigned_edit_position_count
    The number of editable positions holding a character.
available_edit_position_count
    The number of editable positions still free.
mask_completed
    Whether every required position is assigned.
mask_full
    Whether every editable position is assigned.
last_assigned_position
    The last assigned position, or none.
prompt_character : default ``'_'``
    What an unassigned position shows. `set_prompt_character` throws
    `std::invalid_argument` for a control character.
allow_prompt_as_input : default ``true``
    Whether the prompt character may be typed.
reset_on_prompt : default ``true``
    Whether a typed prompt character clears its position.
reset_on_space : default ``true``
    Whether a typed space clears its position instead of being tested.
ascii_only : default ``false``
    Whether characters past the ASCII range are refused everywhere.
skip_literals : default ``true``
    Whether an input character equal to the literal at its position is taken
    as that literal. Otherwise input at a literal goes to the next editable
    position.
display_text
    Every position as the field shows it: assigned characters, prompts and
    literals, one Unicode scalar per position.

Methods
-------
is_edit_position
    Whether a position is editable; false past the end.
is_assigned
    Whether a position holds a character.
find_edit_position
    The first editable position at or after a position (forward), or at or
    before it (backward), or none.
verify_character
    Whether a character could be assigned at a position, without changing
    anything.
insert_at
    Inserts text at a position; the characters at and after it move to later
    editable positions.
replace_at
    Writes text over the positions from a position on.
replace_range
    Removes a range of positions, then inserts text at its start.
remove_range
    Removes the characters in a range; the later characters move back.
set_text
    Clears every position, then places text from the first.
clear
    Unassigns every position.
to_string
    The text in a `MaskTextFormat`: with or without prompts, with or without
    literals.
display_offset
    The byte offset in `display_text` where a position starts.
display_position
    The position at a byte offset in `display_text`.

See Also
--------
MaskedTextBox : A text box that takes input through a provider.

Notes
-----
Every edit returns a `MaskEditResult`: a `MaskResultHint` and a position. On
success the position is the last one the edit wrote or passed over; on
failure it is where the refused character was tested, and nothing changed.
Moving characters (insertion, removal) tests each moved character against
its new position too, so removing a letter before a digit in ``LL00`` is
refused rather than moving the digit into a letter position.

Letters and alphanumerics are ASCII only, and case conversion applies to
ASCII letters only: GUI.Forms has no Unicode character-category data.

Examples
--------
Formatting a part number:

.. code-block:: cpp

   gf::MaskedTextProvider part(">LL-0000");
   const gf::MaskEditResult placed = part.set_text("ab1234");
   const std::string shown = part.display_text();   // "AB-1234"
   const std::string bare = part.to_string(gf::MaskTextFormat::exclude_prompt_and_literals);   // "AB1234"
