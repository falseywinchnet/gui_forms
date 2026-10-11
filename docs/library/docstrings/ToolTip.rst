A provider of short help text that pops up when the pointer rests on a control or the control receives keyboard focus.

A tool tip belongs to one window and holds a text for each control you
register with `set_tool_tip`. When the pointer enters a registered control,
or the control receives focus, the tip appears after `initial_delay` and
disappears after `auto_pop_delay`, or as soon as the pointer leaves or focus
moves on. The tip is drawn over the window but never intercepts the pointer.
One tool tip can serve every control in a window; several tool tips in one
window are independent of each other. Use a `HelpProvider` for help that the
user asks for, and an `ErrorProvider` to flag invalid input.

Parameters
----------
window
    The window whose controls the tool tip serves. The tool tip cannot be
    changed or shown once that window has shut down.

Properties
----------
initial_delay : default ``500 ms``
    How long the pointer must rest on a control, or the control must have
    focus, before the tip appears. A negative delay throws
    `std::invalid_argument`.
reshow_delay : default ``100 ms``
    The shorter delay used instead when this tool tip hid a tip no more than
    `initial_delay` ago, so that moving from one control to the next shows
    each tip quickly. A negative delay throws `std::invalid_argument`.
auto_pop_delay : default ``5000 ms``
    How long a tip stays up. Zero keeps it up until the pointer leaves, focus
    moves or `hide` is called. A negative delay throws
    `std::invalid_argument`.
show_on_hover : default ``true``
    Whether the pointer entering a control shows its tip.
show_on_focus : default ``true``
    Whether a control receiving focus shows its tip, placed below the
    control.
show_always : default ``false``
    Whether tips also appear for disabled controls, for example to explain
    why a command is unavailable. Hidden controls never show tips.
maximum_width : default ``336.0``
    The widest the tip's text may be before it wraps onto further lines. It
    must be finite and between 80 and 2048, or `std::invalid_argument` is
    thrown. Changing it reshapes a tip that is showing.
visible : default ``false``
    Whether a tip is showing.
active_control
    The control whose tip is showing, or null.

Events
------
visibility_changed
    Raised when a tip appears and when it disappears, with the control's
    stable ID, the text, whether it is now shown, and whether keyboard focus
    showed it. Showing a tip while another is up raises the hiding of the
    old one first.

Methods
-------
set_tool_tip
    `set_tool_tip(control, text)` registers or changes the text of a
    control; empty text removes it. The control must be live and attached
    to the tool tip's window, and the text valid UTF-8, or
    `std::invalid_argument` is thrown. Changing the text of the tip that is
    showing updates it at once.
tool_tip
    The text registered for a control, or an empty string.
remove_tool_tip
    Removes a control's text, hiding its tip and cancelling a pending one.
    Returns false when the control has none.
clear
    Hides any tip and removes every registered text.
show
    `show(control)` shows a registered control's tip at once, placed below
    the control, for `auto_pop_delay`; `show(control, duration)` shows it
    for `duration`, where zero means until it is hidden. A control without
    text throws `std::invalid_argument`, as does a negative duration. A
    hidden control shows nothing, and so does a disabled one unless
    `show_always` is true.
hide
    Hides the tip that is showing and cancels one that is waiting for its
    delay.

See Also
--------
HelpProvider : Help text that the user requests.
ErrorProvider : An error icon and message beside a control.
AnchoredPopupLayer : An interactive popup placed against a control.

Notes
-----
A tip shown by the pointer appears just below and to the right of the pointer;
a tip shown by focus or by `show` appears just below the control's left edge.
Either way it moves above when there is no room below and stays at least 4
pixels inside the window. A tip that is showing follows its control when the
control is laid out somewhere else.

The tip ignores the pointer, so the control underneath keeps receiving
clicks and hover. Disposing or hiding the control closes its tip, and
disposing the tool tip closes its tip and cancels any pending one.

Changing, showing or hiding a tool tip must happen on the window's UI
thread. Doing so after the tool tip is disposed, or after its window has
shut down, throws `std::logic_error`.

The tip publishes the accessibility role `tool_tip`, named by its text,
while it is showing.

Examples
--------
Register help for a toolbar button, then show it immediately:

.. code-block:: cpp

   std::shared_ptr<gf::Panel> toolbar =
       gf::make_control<gf::Panel>(gf::StableId("toolbar"));
   std::shared_ptr<gf::Button> save =
       gf::make_control<gf::Button>(gf::StableId("toolbar.save"), "Save");
   (*save).set_requested_bounds({8.0, 8.0, 80.0, 28.0});
   (*toolbar).add_child(save);
   gf::Window window(toolbar, {320.0, 200.0});

   gf::ToolTip tips(window);
   tips.set_tool_tip(save, "Save the document (Ctrl+S)");
   // Hovering over or focusing the button now shows the tip after 500 ms.

   tips.show(save, std::chrono::milliseconds(0));   // until hide()
   // tips.visible() is true and tips.active_control() is save.
   tips.hide();

Mirror each tip in a status bar text as it appears and disappears:

.. code-block:: cpp

   struct StatusHelp final : gf::Component {
       std::string status;

       void on_tip(const gf::ToolTipEvent& event) {
           status = event.shown ? event.text : std::string();
       }
   };

   void example() {
       std::shared_ptr<gf::Button> open =
           gf::make_control<gf::Button>(gf::StableId("open"), "Open");
       gf::Window window(open, {120.0, 40.0});
       gf::ToolTip tips(window);
       StatusHelp help;
       gf::on(tips.visibility_changed(), help, &StatusHelp::on_tip);

       tips.set_tool_tip(open, "Open a file");
       tips.show(open, std::chrono::milliseconds(0));  // help.status is "Open a file"
       tips.hide();                                    // and now empty
   }
