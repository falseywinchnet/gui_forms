# Cooperative workers

**GIVEN:** consumers need one atomic cancellation and join pattern without
`std::jthread` or `std::stop_token`. Include `<gui_forms/threading.hpp>` and link
`GUIForms::Core`. The installed SDK exports this utility and its thread dependency.

`CancellationFlag::request()` permanently sets a release/acquire atomic flag.
`requested()` is safe on the worker thread. A request does not interrupt I/O,
unlock a mutex, or wake a condition variable. Work must check at bounded task
boundaries and give blocking operations their own wake or timeout mechanism.

`AtomicThreadPool` exposes the owner's `threadpool_atomic_fast` synchronous batch
algorithm with reused task storage. Named `AtomicTask` entries are `noexcept`,
borrow contexts through `run`, and must not reenter the same pool. Small batches
retain its caller execution path; larger batches use atomic range claims across
pthread workers. The wrapper's methods belong to one owner thread. Windows uses
MSYS2 CLANG64 winpthreads, macOS and Linux use system pthreads.

`Worker(entry, context)` submits one asynchronous task to the same pool
implementation with one pthread worker. The adapter separates dispatch and wait
so cancellation can be requested before join; synchronous batch behavior stays
unchanged. The entry is a named function
with signature `void(const CancellationFlag&, void*)`; the context is borrowed.
The worker is a unique, noncopyable, nonmovable owner. Declare context before the
worker, so context remains alive through cleanup. Thread creation failures throw
before construction succeeds. A null entry throws `std::invalid_argument`.

`request_cancel()` is concurrent-safe while the worker lives. All other worker
operations belong to its creating thread. `join()` waits, publishes completion, and
rethrows an entry exception once after joining. It does not request cancellation.
Repeated joins are harmless. Destruction requests cancellation and joins; it
contains an unobserved entry failure. Work that never cooperates can therefore
block destruction. Explicitly join to observe failures. Never destroy a worker
from its own entry; self-destruction violates ownership and terminates. A direct
self-join throws `std::logic_error` before waiting.

Worker code must not mutate retained controls. Publish immutable results through
the application's existing UI posting boundary, with a lifetime-valid recipient.
Cancellation does not revoke an already posted callback or extend its owner's
lifetime. This utility does not add a task pool or an Orchestrator service edge.

**OBSERVED provenance:** the historical reference remains in
[`falseywinchnet/backend`](https://github.com/falseywinchnet/backend)
at `orchestrator/third_party/threadpool_atomic_fast/`, carried from File Manager
commit `b880fe9703cc69d653d9a29ab85a90821b0688ef`. That C11/pthread atomic ticket
pool is a synchronous batch control, explicitly not a cancellation protocol.
The GUI.Forms projection lives in `src/core/threading/atomic_pool/`, with original
file hashes in `provenance.json` and its MIT notice preserved. Private C symbols
are prefixed to avoid collisions with a consumer linking the backend too. The
only algorithm extension is split async dispatch/wait for the one-worker utility;
the measured synchronous small-batch/range-claim paths are retained. No original
pool performance claim is transferred without a new measurement.
Existing optional audio APIs retain their separate source-compatible stop-token
signature. New consumer workers can use this utility without that dependency.

The executable installed-SDK example is `examples/reference/cooperative_worker.cpp`.
`tests/threading_tests.cpp` covers repeated 10,000-task batches, empty/small/invalid batches, cancellation, normal completion, destructor
join, repeated join, invalid entry and exception propagation. CTest bounds a
broken cancellation/join regression with a timeout.
