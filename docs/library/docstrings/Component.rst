The base class for everything whose lifetime GUI.Forms tracks.

A component is alive until it is disposed. While alive it can own event
connections and other revocable work, and disposing it, or destroying it,
cuts them all off. Every `Control` is a component, as are nonvisual
providers such as `ToolTip` and `Timer`, application state such as `Value`,
and your own handler classes: derive a class from `Component` and register
its member functions with `on(event, owner, &Owner::method)`, and the
connections end when the object goes away. To dispose a group of components
together, the way a form disposes its providers, put them in a
`ComponentContainer`.

Properties
----------
component_state : default ``ComponentState::alive``
    Where the component is in its life: `ComponentState::alive`, then
    `ComponentState::disposing` while `dispose` runs, then
    `ComponentState::disposed`. Destruction passes through the same states.
is_alive
    Whether the component is still `ComponentState::alive`. Registering a
    handler with `on` on a component that is not alive does nothing.
is_disposed
    Whether disposal has finished.
owned_subscription_count
    How many event connections the component currently owns. It walks the
    list, so use it for diagnostics and tests rather than in a loop.

Methods
-------
dispose
    Disposes the component: it becomes `ComponentState::disposing`, every
    owned connection and piece of work is revoked, `on_dispose` runs, and the
    state becomes `ComponentState::disposed`. Calling it again, including
    from inside `on_dispose`, does nothing.
own_subscription
    Takes ownership of a `SubscriptionToken`, so the connection ends when the
    component is disposed or destroyed. A disconnected token is ignored, and
    a component that is no longer alive disconnects the token at once. `on`
    does this for you.
own_revocable
    Observes revocable work without taking it over: the caller keeps its
    token, and disposing the component revokes the work if it is still
    connected. Window services such as `Window::schedule_ui_timer` and
    `Window::register_accelerator` use it.
on_dispose
    Override to release your own resources during `dispose`. It runs after
    every owned connection has been revoked, so no handler of this component
    can run while it executes. It must not throw. Natural destruction does
    not call it.
verify_dispose_thread
    Called by `dispose` before anything changes; throw from an override to
    refuse disposal. The base version accepts every caller. `Control`
    overrides it to require the window's UI thread and to refuse disposal
    during an attach or detach notification.
revoke_owned_work
    Revokes everything the component owns, newest first, without changing
    its state. `dispose` and the destructor call it.

See Also
--------
ComponentContainer : Owns and disposes a group of components.
Event : The publisher whose connections a component owns.
SubscriptionToken : The deprecated caller-held connection handle.
Control : The component with geometry, input and a place in the tree.

Notes
-----
Owned work is revoked in the reverse of the order it was acquired, whether it
came from `on`, `own_subscription` or `own_revocable`.

Natural destruction also revokes everything the component owns, but it does
so in the base destructor, after the members of derived classes have been
destroyed, and it does not call `on_dispose` or `verify_dispose_thread`. If a
derived class needs its `on_dispose` hook, or holds state that a handler
could still reach while the object is being destroyed, call `dispose` from
its own destructor.

Disposing a component from inside one of its own handlers is allowed: the
handler that is running finishes, and the component's remaining handlers for
that emission are skipped. Do not touch an owner after deleting it.

A component cannot be copied or moved. Its own state is not tied to a
thread; the event connections it owns follow the thread rules of their
publishers.

Examples
--------
An owner whose handler stops when it is disposed:

.. code-block:: cpp

   struct Counter final : gf::Component {
       int changes = 0;
       void on_changed(int) { ++changes; }
   };

   void example() {
       gf::Value<int> count(0);
       Counter counter;
       gf::on(count.changed(), counter, &Counter::on_changed);
       count.set(1);         // counter.changes == 1
       counter.dispose();    // revokes the connection
       count.set(2);         // counter.changes is still 1
   }

Run a teardown hook on destruction as well as on explicit disposal:

.. code-block:: cpp

   struct Session final : gf::Component {
       bool closed = false;
       ~Session() override { dispose(); }

   protected:
       void on_dispose() noexcept override { closed = true; }
   };

   void example() {
       Session session;
       session.dispose();    // session.closed is now true
   }                         // the destructor's dispose() does nothing more
