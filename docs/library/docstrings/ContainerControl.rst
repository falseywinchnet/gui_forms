The base class for containers that manage focus and validation for their descendants.

A container control is a scrolling container, like a `Panel`, that also
knows which of its descendants has focus and how validation runs when focus
leaves one of them. It draws no background of its own. Children are added,
arranged and scrolled exactly as in a `ScrollableControl`. Use it as the
root of a form, or derive from it; `UserControl` builds on it for reusable
composite controls, and `TabControl`, `SplitContainer` and the layout
panels derive from it.

Parameters
----------
stable_id
    Identity of the container, unique within its window.

Properties
----------
active_control
    The focused control when it is a descendant of this container, or null
    when focus is elsewhere or the container is not in a window.
auto_validate : default ``AutoValidate::inherit``
    How validation runs when focus leaves a control inside this container.
    `AutoValidate::inherit` uses the policy of the nearest enclosing
    container that sets one. `AutoValidate::enable_prevent_focus_change`
    raises the control's `validating` event and keeps focus on it if a
    handler cancels; `AutoValidate::enable_allow_focus_change` validates but
    lets focus move anyway; `AutoValidate::disable` does not validate on
    focus changes. An unknown value throws `std::invalid_argument`.
effective_auto_validate
    The policy in force: this container's own, or the nearest enclosing
    container's, or `AutoValidate::enable_prevent_focus_change` when none
    sets one.

Events
------
auto_validate_changed
    Raised after `auto_validate` changes, with the new value.

Methods
-------
contains_descendant
    Whether a control is inside this container at any depth. The container
    itself and null are not descendants.
request_active_control
    Moves focus to a descendant and returns whether it took focus. It
    returns false for a control outside the container, when the container is
    not in a window, and when the control cannot take focus or validation of
    the current control keeps focus where it is.
clear_active_control
    Removes focus from the window when a descendant has it, and returns
    whether focus was cleared. It returns false when no descendant has
    focus, and when validation of the focused control keeps focus where it
    is.
validate
    Validates the active control and each of its ancestors up to this
    container, raising their `validating` and `validated` events, and
    returns whether all of them accepted. With ``check_auto_validate`` true
    it returns true without validating when the effective policy is
    `AutoValidate::disable`. With no active control, or when the container
    is not in a window, it returns true.
validate_children
    Validates every descendant selected by ``constraints`` and returns
    whether all accepted. The default, `ValidationConstraints::selectable`,
    selects focusable, selectable controls; add `enabled`, `visible` or
    `tab_stop` to narrow the selection, and `immediate_children` to stop at
    direct children. An unknown flag throws `std::invalid_argument`.

See Also
--------
UserControl : A container for building reusable composite controls.
Panel : A themed scrolling container without focus or validation scope.
ScrollableControl : The scrolling viewport this class inherits.

Notes
-----
Focus validation runs when focus moves away from a control whose
`causes_validation` is true towards a control that also causes validation,
and the policy that applies is the one inherited by the control losing
focus. A handler cancels by setting ``cancel`` on the `ControlValidationEvent`
it receives; `validated` is raised only when no handler cancelled. A
validation handler cannot move focus itself: the request is refused.

The container itself does not take focus. It is published to accessibility
as a `group` when it has an accessible name or description, and is otherwise
left out with its children published in its place.

Examples
--------
Move focus to a descendant and clear it again:

.. code-block:: cpp

   std::shared_ptr<gf::ContainerControl> form =
       gf::make_control<gf::ContainerControl>(gf::StableId("form"));
   std::shared_ptr<gf::Button> apply =
       gf::make_control<gf::Button>(gf::StableId("form.apply"), "&Apply");
   (*apply).set_requested_bounds({12.0, 12.0, 90.0, 28.0});
   (*form).add_child(apply);

   gf::Window window(form, {240.0, 120.0});
   (*form).request_active_control(apply);
   gf::Control::Ptr active = (*form).active_control();   // apply
   (*form).clear_active_control();                       // nothing has focus now

Validate the focused field on demand. A handler that cancels makes
`validate` return false:

.. code-block:: cpp

   struct Form final : gf::Component {
       std::shared_ptr<gf::ContainerControl> root =
           gf::make_control<gf::ContainerControl>(gf::StableId("form"));
       std::shared_ptr<gf::TextBox> name =
           gf::make_control<gf::TextBox>(gf::StableId("form.name"));
       bool accept = false;

       Form() { gf::on((*name).validating(), *this, &Form::on_validating); }
       void on_validating(gf::ControlValidationEvent& event) { event.cancel = !accept; }
   };

   void example() {
       Form form;
       (*form.name).set_requested_bounds({12.0, 12.0, 200.0, 24.0});
       (*form.root).add_child(form.name);
       gf::Window window(form.root, {240.0, 60.0});
       (*form.root).request_active_control(form.name);

       bool valid = (*form.root).validate();   // false: the handler cancelled
       form.accept = true;
       valid = (*form.root).validate();        // true
       static_cast<void>(valid);
   }
