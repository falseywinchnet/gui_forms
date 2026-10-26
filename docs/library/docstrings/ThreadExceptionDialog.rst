The dialog for an exception the application did not handle.

A thread exception dialog tells the user that the application hit an
exception it did not handle, as Windows Forms' ThreadExceptionDialog does: an
error icon, a heading, the exception's message, the details behind an
expander, and Continue and Quit buttons. It is a preset over `TaskDialog`, an
owned in-window dialog built from GUI.Forms controls, so it looks and behaves
the same on every host. `show` returns at once; `closed` reports the button.

The dialog reports a `ThreadExceptionReport`: a caption, the message, the
details and whether it is a warning. `describe_exception` makes one from a
caught `std::exception_ptr`. A C++ exception carries neither a portable type
name nor a stack trace, so the application adds the details it has.

Properties
----------
report
    The report the dialog shows, fixed at construction. Its text must be
    valid UTF-8; the constructor throws `std::invalid_argument` otherwise.
result : default none
    The result of the last close: ``cancel`` for Continue, Escape and a dialog
    whose owner left its window; ``abort`` for Quit.
task_dialog
    The task dialog the preset shows, for automation and tests; null once the
    dialog is disposed.

Events
------
closed
    The dialog closed, with ``cancel`` or ``abort``.

Methods
-------
show
    Shows the dialog over `owner`'s window, with focus on Continue. Returns
    false when it is already shown or `owner` is not in a window. Throws
    `std::logic_error` for a disposed dialog.
close
    Closes the dialog as Continue (``cancel``) or Quit (``abort``) would,
    raising `closed`. Throws `std::invalid_argument` for any other result, and
    for ``abort`` on a warning.

See Also
--------
TaskDialog : The dialog under the preset.

Notes
-----
The dialog never ends the application: on ``abort`` the application closes
itself, as Windows Forms' Application does after its dialog. A warning report
(Windows Forms' WarningException) shows the warning icon and offers Continue
only. An empty caption gives "Unhandled exception"; empty details leave the
expander out. Disposing or destroying a shown dialog closes it without
raising `closed`, and a `closed` handler may destroy the dialog.

Not yet supported: an application-wide hook that shows the dialog for an
exception escaping an event handler (Windows Forms' Application.ThreadException),
and a help button for a warning with a help address.

Examples
--------
Reporting a caught exception over a window:

.. code-block:: cpp

   std::shared_ptr<gf::Panel> root = gf::make_control<gf::Panel>(gf::StableId("doc.root"));
   gf::Window window(root, {640.0, 480.0});
   gf::ThreadExceptionReport report;
   try {
       throw std::runtime_error("The index is out of date.");
   } catch (...) {
       report = gf::describe_exception(std::current_exception());
   }
   report.details = "Raised while rebuilding the index.";
   gf::ThreadExceptionDialog dialog(gf::StableId("doc.error"), std::move(report));
   const bool shown = dialog.show(root);
   static_cast<void>(shown);
   dialog.close(gf::DialogResult::cancel);
