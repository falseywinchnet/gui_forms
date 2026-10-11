The permanent name of a control, unique within its window.

Every control is constructed with a stable ID, and keeps it for its whole
life. Logs, the instrumentation snapshot, the semantic (accessibility) tree,
automation and `Window::find` all address a control by this string, so a test
can say "press `dialog.ok`" without coordinates. Choose IDs that describe the
control's role in the form, such as `"settings.wrap"`; the toolkit attaches
no meaning to dots or other characters. For a display name that may change,
use the control's `name` property instead.

Parameters
----------
value
    The identity. Any nonempty string is accepted; an empty string throws
    `std::invalid_argument`.

Properties
----------
value
    The identity as a view of the stored string. It stays valid as long as
    the `StableId` (or the control holding it) is alive.

See Also
--------
Control : Takes a `StableId` as its first constructor argument.
Window : Looks controls up by stable ID and rejects duplicates.
RuntimeId : The process-local numeric identity, not stable across runs.

Notes
-----
A `StableId` is immutable and compares equal to another with the same
string. Uniqueness is checked by the window, not by the ID: a detached tree
may hold two controls with the same ID, but constructing a `Window` around
it, or adding a subtree that repeats an ID already in the window, throws
`std::logic_error` and changes nothing. When a control leaves the
window (removal or disposal) its ID becomes free again.

Examples
--------
Construct an ID and compare it:

.. code-block:: cpp

   gf::StableId id("settings.wrap");
   std::string_view text = id.value();               // "settings.wrap"
   bool same = id == gf::StableId("settings.wrap");  // true
   static_cast<void>(text);
   static_cast<void>(same);

Find a control by its ID, and see a duplicate rejected:

.. code-block:: cpp

   std::shared_ptr<gf::Panel> root = gf::make_control<gf::Panel>(gf::StableId("settings"));
   (*root).add_child(gf::make_control<gf::CheckBox>(gf::StableId("settings.wrap"), "&Wrap lines"));
   gf::Window window(root, gf::Size{320.0, 200.0});
   gf::Control::Ptr wrap = window.find("settings.wrap");   // the check box

   try {
       (*root).add_child(gf::make_control<gf::Label>(gf::StableId("settings.wrap")));
   } catch (const std::logic_error&) {
       // duplicate stable ID: the label was not added
   }
   static_cast<void>(wrap);
