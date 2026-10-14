Runs work on a worker thread and reports progress and completion on the UI thread.

A background worker runs one work function at a time off the UI thread, as
Windows Forms' BackgroundWorker does. Each run is a `Worker`: the work is a
named target bound with `Delegate` (`set_do_work`) and receives a
`BackgroundWorkerTask`, the one object it may touch on the worker thread. The
task carries the argument, takes the result, answers whether cancellation was
requested and posts progress reports. Progress and completion travel back
through the window's dispatcher, so `progress_changed` and
`run_worker_completed` are raised on the UI thread, in order with input,
layout and paint. Use `Worker` directly for work that needs no UI-thread
reports, and `AtomicThreadPool` for batches of short tasks.

Parameters
----------
window
    The window whose UI thread owns the worker and whose dispatcher delivers
    its events. Construct the worker on that thread.

Properties
----------
has_do_work
    Whether a work function is set. `set_do_work` sets it; a change applies
    to the next run, not one in progress. A run without work completes at
    once with an empty result.
worker_reports_progress : default ``false``
    Whether the work may call `BackgroundWorkerTask::report_progress`.
worker_supports_cancellation : default ``false``
    Whether `cancel_async` is allowed.
is_busy
    Whether a run has started and its completion has not been delivered. A
    run whose completion the dispatcher refused, because the window shut
    down, stops being busy once its work has returned.
cancellation_pending
    Whether `cancel_async` was called during the current run.

Events
------
progress_changed
    Raised on the UI thread for each report the work posted, in order, with
    the percentage and the optional value. Reports from a run that has
    completed are dropped.
run_worker_completed
    Raised on the UI thread when a run ends, with the work's result, the
    exception it threw (and its message), and whether it acknowledged a
    cancellation. The worker is idle when this is raised, so a handler may
    start the next run.

Methods
-------
run_worker_async
    Starts a run with an optional argument. Throws `std::logic_error` while
    busy, after the window's dispatcher shut down, or off the UI thread.
cancel_async
    Asks the current run to stop: the work sees
    `BackgroundWorkerTask::cancellation_pending` and decides when to return.
    Throws `std::logic_error` when the worker does not support cancellation;
    does nothing when idle.

See Also
--------
Worker : The thread each run uses.
Timer : Work at intervals on the UI thread.
Window : Its dispatcher delivers the worker's events.

Notes
-----
Every member of the worker belongs to the UI thread of its window. The work
runs on the worker thread and may touch only its task and data it owns; it
must not reach controls, and it must not keep the task after returning.
`BackgroundWorkerTask::report_progress` throws `std::logic_error` when the
worker does not report progress and `std::out_of_range` for a percentage
outside 0 to 100. A report the dispatcher cannot queue (its window closed, or
its queue is full) is dropped; the completion waits for room in a full
queue.

To report a cancellation, the work calls
`BackgroundWorkerTask::set_cancelled`, as Windows Forms' handler sets
``e.Cancel``. An exception the work throws ends the run: the completion
carries it in ``error`` with its ``what()`` text in ``error_message``, and an
empty result.

When the window closes while work runs, its dispatcher stops accepting
callbacks: reports and the completion are dropped, no event is raised, and
the run ends when its work returns. The worker then refuses to start again.
Disposing or destroying the worker requests cancellation and waits for the
work to return, so work that never checks `cancellation_pending` blocks the
UI thread there; no event is raised after disposal. Work must not call
`Window::invoke` while the UI thread may be waiting for it in disposal.

The worker is a component: it publishes its events as interactions and has
no anatomy.

Examples
--------
Sum the numbers up to an argument in the background:

.. code-block:: cpp

   struct Sums final {
       void work(gf::BackgroundWorkerTask& task) {
           const std::int64_t limit = gf::binding_value_to_signed(task.argument()).value_or(0);
           std::int64_t total = 0;
           for (std::int64_t value = 1; value <= limit && !task.cancellation_pending(); ++value) {
               total += value;
           }
           task.set_result(gf::BindingValue{total});
       }
   };

   void example() {
       std::shared_ptr<gf::Panel> root = gf::make_control<gf::Panel>(gf::StableId("doc.root"));
       gf::Window window(root, {320.0, 200.0});
       Sums sums;
       gf::BackgroundWorker worker(window);
       worker.set_do_work(gf::BackgroundWorker::Work::bind<Sums, &Sums::work>(sums));
       worker.run_worker_async(gf::BindingValue{std::int64_t{100}});
       // run_worker_completed reports 5050 when the window's dispatcher runs.
   }
