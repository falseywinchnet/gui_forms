# Lifecycle

GUI.Forms keeps four lifetimes apart on purpose: a control's retained
identity, a window's host session, the native window the platform creates,
and (for compatibility consumers on Windows) a leased native handle. Each has
its own state machine, its own events and its own conformance tests, and the
same portable trace is produced by every host. The full contract is recorded
in ADR-014; this page is the working summary.

## Four identities

| Identity | Created | Authority over | Lives from |
|---|---|---|---|
| Retained control | `make_control` | properties, tree membership, events, paint state | construction to `dispose()` |
| Window session | `Application::run` (or a test pump) | portable geometry, input, presentation, close order | one host run |
| Native host window | inside the platform adapter | OS presentation and platform services | native create to native destroy |
| Compatibility handle lease | first read of `Control.Handle` by a compatible consumer | bounded native/GDI interop only | first acquisition to disposal |

A retained identity is never cast to or returned as a native handle, and a
handle lease never becomes the visual or input authority for the retained
tree.

## Host session phases

`HostSessionSnapshot.phase` is the authority on where a window is:

```text
constructed → attached → close_authorized → closed → shutdown
```

- `constructed → attached` on exactly one accepted attach event.
- `attached` accepts resize, scale, display, activation, occlusion, input and
  drag events, and a close request. A cancelled close stays `attached`.
- An allowed close moves to `close_authorized`; input is suppressed from then
  on. A forced or native close may go straight to `closed`.
- `closed` synchronously revokes dispatcher work, frame requests, pointer
  capture and drag state. Only `shutdown` is accepted afterwards.
- `shutdown` is terminal and detaches host services.

Any other transition returns `HostDispatchError::invalid_lifecycle` and does
not advance the sequence or touch the retained window. Events after shutdown
return `after_shutdown`.

## Initialization order

Every presenting host follows the same sequence, and the headless host is the
reference implementation the conformance tests compare against:

1. Construct retained controls and connect handlers with `on(...)`.
2. Construct the `Window`; its subtree attachment transaction completes.
3. Begin native surface creation.
4. Bind the native surface identity privately inside the adapter.
5. Establish ownership, client geometry, scale, renderer resources and
   services.
6. Emit exactly one accepted attach event.
7. Publish wake, close, dialog, tooltip and clipboard services.
8. Drain one FIFO snapshot of already-posted initialization work. The
   `ready` callback in `ApplicationWindowOptions` runs here. Work posted by
   that callback waits for an ordinary later turn.
9. Commit the adapter as ready and request native presentation.
10. Accept activation, input and paint from the presented surface.

Properties and children may be mutated during step 8; their damage coalesces
into the first presented frame, so no half-initialized frame is ever shown.
The single-turn rule keeps a recursive `begin_invoke` chain from starving the
first presentation.

Adapters respect the order in platform-specific ways. Win32's
`CreateWindowExW` reenters the window procedure before it returns, so
creation-time messages are fenced from portable dispatch until step 6; the
window is shown only after step 8. On AppKit the `NSWindow` owns its view and
all services are in place before `makeKeyAndOrderFront:`. The X11 adapter follows the same
order, with `XMapRaised` as its presentation step.

## Attachment is a separate axis

Controls move through `detached → attaching → attached → detaching →
detached`, with `disposed` terminal, independently of the host phases. A
control attaches when its subtree joins a window's root and detaches when
removed; it never creates or destroys anything native. Attachment hooks are
transactional: structural mutation during a notification is rejected, and a
failed attach rolls the complete subtree back before the exception escapes.
`UserControl::loaded()` fires once per instance on first attachment commit;
`attachment_count()` counts every successful commit.

## Closing

A close request from the user, the OS or `ApplicationWindowHandle::
request_close()` reaches the `closing` callback with a `HostCloseRequest`.
Setting its cancel flag keeps the window `attached`. Otherwise the session
becomes `close_authorized`, the host destroys the native window, and
`closed` fires. Closing the primary window, the one window that has no owner
and is not a tool window, ends the host session: the host closes the remaining windows and
`Application::run` returns an `ApplicationResult`. A close requested from
inside the `ready` callback is honoured before any synthetic or user input is
delivered.

`hide_on_close` turns the close gesture into `hide()`; the window stays
`attached` and can be shown again with the handle. It applies to secondary
and tool windows only. The primary window cannot start hidden or hide on
close, and `run` rejects those options for it with `invalid_argument`.

## Disposal

Disposal is synchronous. `Component::dispose()` first marks the object
`disposing` and revokes its owned subscriptions and work, then calls
`on_dispose()`. The control hook detaches the subtree, releases retained
resources and disposes children. Finally the component is marked `disposed`.
A disposed control rejects mutation. Disposing
the root after `Application::run` returns is ordinary; disposing it while
the window is `attached` detaches it from the live window first.

## Compatibility handle lease (Windows)

For consumers that must call native or GDI APIs on a control, reading
`Control.Handle` through the compatibility facade allocates an invisible,
disabled, offscreen native surface. It is not parented, shown, focused,
hit-tested or composited as a native child. The lease moves
`absent → leased → released`; first acquisition raises `HandleCreated`,
re-reading is idempotent, and disposal raises `HandleDestroyed` once before
release. `IsHandleCreated` reports this lease and nothing else.

## Conformance cases

The headless host and every native host must pass the same cases:
activation or input before attach is rejected; duplicate attach is rejected;
cancelled close returns to attached; allowed close suppresses input; closed
accepts only shutdown; the callback order is ready, input, closing, closed; a
close requested during ready suppresses later input; pre-host posted
callbacks run after attach and before presentation; nested posted callbacks
remain separate turns; a Windows handle lease raises exactly one matching
pair of events; and each host produces the same portable phase trace for
equivalent events.
