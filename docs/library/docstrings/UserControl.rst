A container for building your own reusable composite control.

Derive from UserControl to package a group of controls, such as an address
editor or a search bar, as one control that the rest of the application
places like any other. It adds a one-time `loaded` event, raised the first
time the control is attached to a window, so the composite can finish
setting itself up when it is first shown rather than every time it is
moved. Focus scope, validation, layout and scrolling are those of
`ContainerControl`. Use a `Panel` when you only need to group controls in
place.

Parameters
----------
stable_id
    Identity of the control, unique within its window. Derive the IDs of
    the children from it so that two instances do not collide.

Properties
----------
is_loaded : default ``false``
    Whether `loaded` has been raised. It stays true for the rest of the
    control's life, even after it is detached.
is_attached : default ``false``
    Whether the control is currently attached to a window.
attachment_count : default ``0``
    How many times the control has been attached to a window and the
    attachment completed. Moving it to another parent in the same window
    detaches and reattaches it, so the count goes up again.

Events
------
loaded
    Raised once in the control's life, when it is first attached to a
    window: when a window is created with it in its tree, or when it is
    added to a tree that is already in a window. The whole subtree is
    already bound to the window when it is raised. It is raised at most
    once: later attachments do not raise it, and neither does the next
    attachment after one that raised it and then failed and was rolled
    back.

Methods
-------
on_attached_to_window
    Raises `loaded` the first time. A subclass that overrides it must call
    `UserControl::on_attached_to_window`, or `loaded` is never raised.
on_attachment_committed
    Counts the completed attachment. A subclass that overrides it must call
    the base implementation.
on_detached_from_window
    Clears `is_attached`. A subclass that overrides it must call the base
    implementation.

See Also
--------
ContainerControl : The focus and validation scope this class inherits.
Panel : A themed container for grouping controls in place.

Notes
-----
`loaded` is raised while the window is still attaching the subtree. A
handler may change properties, but adding or removing children anywhere in
the window's tree throws `std::logic_error`, and an exception that leaves
the handler aborts the attachment, which is rolled back before the
exception reaches the caller. Build the child tree before attaching the
control.

A subclass cannot add children from its constructor, because the control is
not yet owned by a `std::shared_ptr`. Declare a public
``static constexpr bool initialize_tree_after_construction = true;`` and a
public ``void initialize_control_tree()``, and `make_control` calls it right
after construction, as in the first example.

The control draws no background of its own and takes no focus. It is
published to accessibility as a `group` when it has an accessible name or
description, and is otherwise left out with its children published in its
place.

Examples
--------
A composite control that builds its children right after construction:

.. code-block:: cpp

   class AddressEditor final : public gf::UserControl {
   public:
       static constexpr bool initialize_tree_after_construction = true;

       explicit AddressEditor(gf::StableId id) : gf::UserControl(std::move(id)) {}

       void initialize_control_tree() {
           const std::string prefix(stable_id().value());
           street = gf::make_control<gf::TextBox>(gf::StableId(prefix + ".street"));
           city = gf::make_control<gf::TextBox>(gf::StableId(prefix + ".city"));
           (*street).set_requested_bounds({0.0, 0.0, 240.0, 24.0});
           (*city).set_requested_bounds({0.0, 30.0, 240.0, 24.0});
           add_child(street);
           add_child(city);
       }

       std::shared_ptr<gf::TextBox> street;
       std::shared_ptr<gf::TextBox> city;
   };

   void example() {
       std::shared_ptr<AddressEditor> billing =
           gf::make_control<AddressEditor>(gf::StableId("billing"));
       std::shared_ptr<AddressEditor> shipping =
           gf::make_control<AddressEditor>(gf::StableId("shipping"));
       (*shipping).set_requested_bounds({0.0, 70.0, 240.0, 60.0});
       std::shared_ptr<gf::Panel> root = gf::make_control<gf::Panel>(gf::StableId("checkout"));
       (*root).add_child(billing);
       (*root).add_child(shipping);
       gf::Window window(root, {240.0, 140.0});   // "billing.street" and "shipping.street" are distinct
   }

Finish setting up when the control is first shown. Reattaching it does not
raise `loaded` again:

.. code-block:: cpp

   struct Search final : gf::Component {
       std::shared_ptr<gf::UserControl> bar =
           gf::make_control<gf::UserControl>(gf::StableId("search"));
       int loads = 0;

       Search() { gf::on((*bar).loaded(), *this, &Search::on_loaded); }
       void on_loaded() { ++loads; }
   };

   void example() {
       Search search;
       std::shared_ptr<gf::Panel> left = gf::make_control<gf::Panel>(gf::StableId("left"));
       std::shared_ptr<gf::Panel> right = gf::make_control<gf::Panel>(gf::StableId("right"));
       std::shared_ptr<gf::Panel> root = gf::make_control<gf::Panel>(gf::StableId("root"));
       (*root).add_child(left);
       (*root).add_child(right);
       (*left).add_child(search.bar);

       gf::Window window(root, {400.0, 300.0});   // loaded: search.loads is 1
       (*right).add_child(search.bar);             // attachment_count() is 2, loads is still 1
   }
