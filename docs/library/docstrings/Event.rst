A notification that a publisher raises and any number of handlers observe.

Controls and models expose their events through accessors such as
`ButtonBase::clicked()` or `Value::changed()`, and raise them with the
arguments in the template parameter list, so `Event<bool>` carries one
`bool`. Observe an event with `on(event, owner, &Owner::method)`, where the
owner is a `Component`: the owner keeps the connection, and disposing or
destroying either side ends it. Raise your own events with `emit`, for
example from a model class that other parts of the application observe.

Properties
----------
statistics
    Counts of connections made, connections ended and handler calls, for
    tests and diagnostics.

Methods
-------
subscribe
    Deprecated. Connects a callback and returns a caller-held
    `SubscriptionToken`; destroying or disconnecting the token ends the
    connection, and the overloads that take a `Component` also end it when
    that component is disposed. Write `on(event, owner, &Owner::method)`
    instead. These overloads remain for existing code that must cancel one
    connection on its own. An empty callback returns an empty token.
emit
    Calls every connected handler, in the order they were connected, with
    the given arguments. An exception from a handler propagates to the
    caller of `emit` and the handlers after it are not called for that
    emission; the event stays usable.
disconnect_all
    Ends every connection to the event. The destructor calls it.

See Also
--------
Component : The owner that keeps connections made with `on`.
Value : Application-owned state whose `changed` event controls observe.
SubscriptionToken : The handle the deprecated `subscribe` overloads return.

Notes
-----
`on(event, owner, &Owner::method, bound...)` takes a named member function
of the owner, which must return `void`. The method receives up to two bound
values first, then a leading part of the event's arguments: a handler for
`Event<ButtonBase&>` may take the sender or nothing at all. Bound values must
be trivially copyable and total at most 16 bytes; pass an index into the
owner's data rather than a string. The compiler rejects other signatures
with a message naming the rule. Registration on an owner that is no longer
alive does nothing, and a null member pointer throws `std::invalid_argument`.
Registration may allocate; raising an event to member handlers does not.

With `on`, arguments declared as values reach handlers as `const`
references, so one handler cannot change what later handlers receive.
Arguments declared as non-`const` references, such as
`ControlValidationEvent&`, are shared, which is how a `validating` handler
sets `cancel`.

Each emission calls the handlers that were connected when it started.
Handlers connected during an emission are first called by the next one; a
handler disconnected before its turn is skipped. A handler may raise the
same event again; the nested emission has its own starting set. A handler
may also dispose its owner, disconnect itself or destroy the object that
owns the event: the running handler finishes, and the handlers that were
cut off are skipped.

An event has no thread affinity of its own. Raise and observe it on the
thread that owns its publisher; for a control, that is its window's UI
thread.

Examples
--------
A model class that publishes progress, and a view that observes it:

.. code-block:: cpp

   class Download final {
   public:
       gf::Event<int>& progress() { return progress_; }
       void advance(int percent) { progress_.emit(percent); }

   private:
       gf::Event<int> progress_;
   };

   struct ProgressView final : gf::Component {
       int shown = 0;
       void on_progress(int percent) { shown = percent; }
   };

   void example() {
       Download download;
       ProgressView view;
       gf::on(download.progress(), view, &ProgressView::on_progress);
       download.advance(40);   // view.shown == 40
   }

One handler for several buttons, told apart by a bound value:

.. code-block:: cpp

   struct Toolbar final : gf::Component {
       std::shared_ptr<gf::Button> bold =
           gf::make_control<gf::Button>(gf::StableId("format.bold"), "&Bold");
       std::shared_ptr<gf::Button> italic =
           gf::make_control<gf::Button>(gf::StableId("format.italic"), "&Italic");
       int last_tool = -1;

       Toolbar() {
           gf::on((*bold).clicked(), *this, &Toolbar::on_tool, 0);
           gf::on((*italic).clicked(), *this, &Toolbar::on_tool, 1);
       }
       void on_tool(int tool) { last_tool = tool; }
   };

   void example() {
       Toolbar toolbar;
       (*toolbar.italic).perform_click();   // toolbar.last_tool == 1
   }
