The entry point that presents windows on the platform's native host.

`Application::run` takes one or more retained `Window` models, gives each a
native window, pumps native events until the primary window closes and then
returns an `ApplicationResult`. It is the C++ counterpart of Windows Forms'
`Application.Run`. The class has only static members and cannot be
constructed. Link `GUIForms::Application`, which contains the one native host
the SDK was built with; renderer-free programs and tests use `Window`
directly and need no host.

Methods
-------
run
    `run(std::unique_ptr<Window>, ApplicationWindowOptions)` presents one
    window, with the stable ID `"main"`. `run(std::vector<ApplicationWindow>)`
    presents up to `maximum_windows` (64) windows: exactly one primary window
    with no owner, plus owned and tool windows. It returns when the primary
    window has closed. The window models are consumed even when `run`
    fails.
validate
    Checks a set of windows against the rules `run` applies, without
    presenting anything, and returns the first problem as an
    `ApplicationResult`. It is part of `GUIForms::Core` and needs no host.
shutdown
    Ends the rendering library once per process: it frees stx_skia's caches
    and singletons. Call it after the last `run` has returned, when every
    window, surface and font the program made is gone; the window models
    `run` consumed are already destroyed. It is refused with
    `already_running` while a `run` is in progress (from a callback, for
    example) and with `wrong_thread` on macOS off the main thread. After it,
    `run` and `shutdown` return `after_shutdown`. A process may end without
    it, a crash included, and lose nothing: the library holds only process
    memory, which the operating system reclaims.
capabilities
    The capability record of the linked native host: its name and the
    services it offers, such as dialogs, clipboard and cursors. A build
    without a native host reports the name `"unsupported"` and no
    capabilities.

See Also
--------
ApplicationWindowOptions : Title, sizes and callbacks for one window.
ApplicationWindowHandle : Show, hide, retitle or close a running window.
ApplicationWindow : One entry of a multi-window run.
ApplicationResult : What `run`, `validate` and `shutdown` return.
Window : The retained model each native window presents.

Notes
-----
`run` reports problems through `ApplicationResult::error` instead of
throwing:

- `ApplicationError::invalid_argument`: the windows break a rule below;
  `ApplicationResult::window_index` names the offending window.
- `ApplicationError::wrong_thread`: on macOS, `run` was not called on the
  main thread; or a `Window` was constructed on a different thread from the
  one calling `run`.
- `ApplicationError::already_running`: another `run` is in progress in this
  process.
- `ApplicationError::unsupported`: the library was built without a native
  host.
- `ApplicationError::callback_failure`: a callback in
  `ApplicationWindowOptions` threw. The first exception is kept in
  `ApplicationResult::callback_exception`, and the host closes the primary
  window to end the run.
- `ApplicationError::backend_failure`: the native host exited with a nonzero
  `ApplicationResult::native_exit_code`.
- `ApplicationError::after_shutdown`: `shutdown` has already run in this
  process.

The rules `validate` checks: between 1 and 64 windows; every stable ID
nonempty, unique, at most 1024 bytes of valid UTF-8 without NUL; an owner ID,
when present, naming another window, with no ownership cycles; a title of at
most 65536 bytes of valid UTF-8 without NUL; initial and minimum sizes finite,
between 1 and 32768, with the minimum no larger than the initial size; and
exactly one primary window (no owner ID, not a tool window), which must be
initially visible and must not hide on close. `run` with a single window
makes that window the primary, so its options must keep
`initially_visible` true and `hide_on_close` false.

For each window the callbacks run in this order: `wake_ready`, then `ready`
once the native window and its services exist and before it is first shown,
then input and painting, then `closing` for each close request, and `closed`
once it has closed. All of them run on the thread that called `run`.

Examples
--------
Present one window and report how the run ended. `run` needs a native host,
so this example is compiled but not run:

.. code-block:: cpp

   // compile only
   struct Greeter final : gf::Component {
       std::shared_ptr<gf::Panel> root = gf::make_control<gf::Panel>(gf::StableId("root"));
       std::shared_ptr<gf::Button> greet =
           gf::make_control<gf::Button>(gf::StableId("greet"), "&Greet");

       Greeter() {
           (*greet).set_bounds({24.0, 24.0, 120.0, 36.0});
           (*root).add_child(greet);
       }
   };

   void example() {
       Greeter greeter;
       std::unique_ptr<gf::Window> window =
           std::make_unique<gf::Window>(greeter.root, gf::Size{480.0, 140.0});
       gf::ApplicationWindowOptions options;
       options.title = "Greeter";
       options.initial_size = {480.0, 140.0};
       options.minimum_size = {320.0, 140.0};
       gf::ApplicationResult result = gf::Application::run(std::move(window), std::move(options));
       if (!result.accepted()) {
           std::fprintf(stderr, "run failed: %d\n", static_cast<int>(result.error));
       }
       // Once, after the program's last run.
       gf::ApplicationResult ended = gf::Application::shutdown();
       if (!ended.accepted()) {
           std::fprintf(stderr, "shutdown failed: %d\n", static_cast<int>(ended.error));
       }
   }
