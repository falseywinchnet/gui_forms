A shared scalar that the application owns and controls can bind to.

A `Value` holds one number, `bool` or enumeration, such as a volume, a
"wrap lines" setting or a selected mode. Several controls can bind to the
same value with `CheckBox::bind`, `RangeControl::bind` or
`ChoiceGroup::bind`, and the rest of the application observes it through
`changed`. Writing an equal value does nothing, so two bound controls never
echo a change back and forth. Keep strings and objects in your own model;
`Value` accepts only scalar types, and the compiler rejects others.

Parameters
----------
initial
    The starting value, `T{}` by default. A floating-point value must be
    finite; NaN or infinity throws `std::invalid_argument`.

Properties
----------
get
    The current value. Reading it is always allowed, also after disposal.

Events
------
validating
    Raised by `set` before a new value is committed, with the candidate. A
    handler rejects the candidate by throwing; the exception reaches the
    caller of `set` and the value stays as it was. Bound controls use this to
    apply their own limits. It is not raised for an equal value.
changed
    Raised after a new value is committed, with that value. An exception from
    a handler propagates to the caller of `set`, but the new value is already
    stored.

Methods
-------
set
    Changes the value. An equal value does nothing. Otherwise `validating` is
    raised, the value is stored and `changed` is raised. Throws
    `std::invalid_argument` for a non-finite floating-point value and
    `std::logic_error` when the value is disposed or when called with a
    different value while the value's own handlers are running.
validate_candidate
    Runs the same checks and `validating` handlers as `set` without storing
    anything, so a control can find out whether a change would be accepted
    before it changes its own state. It throws as `set` does.

See Also
--------
CheckBox : Binds its checked state to a `Value<bool>`.
RangeControl : Binds its position to a `Value<double>`.
ChoiceGroup : Binds its selected index to a `Value<int>`.
Command : Shared enabled and checked state for buttons and menus.

Notes
-----
A value is a `Component`. Disposing it, or destroying it, ends every
connection to `changed` and `validating`, including those of bound controls,
which keep the last state they showed. After disposal, `set` and
`validate_candidate` throw `std::logic_error`. Destroying a value from inside
one of its own `changed` handlers is allowed; the remaining handlers are
skipped.

While `validating` or `changed` handlers run, a further `set` with the same
value does nothing and one with a different value throws `std::logic_error`.
This stops observers that correct each other from looping; to make a
follow-up change, do it after the notification returns.

Notifications are synchronous on the thread that calls `set`. Use a value on
the UI thread of the windows whose controls bind to it. It never starts a
timer or schedules work.

Examples
--------
Observe a setting, and see an equal write do nothing:

.. code-block:: cpp

   enum class Mode { edit, preview };

   struct StatusBar final : gf::Component {
       int updates = 0;
       void on_mode(Mode) { ++updates; }
   };

   void example() {
       gf::Value<Mode> mode(Mode::edit);
       StatusBar status;
       gf::on(mode.changed(), status, &StatusBar::on_mode);
       mode.set(Mode::preview);   // status.updates == 1
       mode.set(Mode::preview);   // equal: nothing is raised
   }

Reject out-of-range values with a `validating` handler:

.. code-block:: cpp

   struct VolumeLimit final : gf::Component {
       void check(int candidate) {
           if (candidate > 100) throw std::out_of_range("volume above 100");
       }
   };

   void example() {
       gf::Value<int> volume(50);
       VolumeLimit limit;
       gf::on(volume.validating(), limit, &VolumeLimit::check);
       volume.set(80);            // accepted
       try {
           volume.set(120);       // rejected before it is stored
       } catch (const std::out_of_range&) {
       }
       // volume.get() == 80
   }
