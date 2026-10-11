# ADR-014: One portable lifecycle contract with separate retained, session, native and lease identities

Status: accepted (recorded retroactively); implemented for the portable core,
headless, AppKit, Win32, X11, the C ABI host bridge and the generated Forms
facade

Date: 2026-10-11 (decision in force since the lifecycle contract was published;
first written down as an ADR on this date)

## Context

Forms toolkits historically fuse several lifetimes into one object: the
control, its window handle, its platform window and its "created" state.
Consumers then read handle creation as readiness, platform destruction as
disposal, and reparenting as recreation. GUI.Forms has three presenting hosts
with different native creation orders (Win32 reenters the window procedure
during `CreateWindowExW`; AppKit delivers view notifications before the
window is key; X11 maps asynchronously) and a headless host used as the
conformance reference. One hidden order per host would make traces
unstable and tests platform-specific.

This decision was cited as ADR-014 by `docs/LIFECYCLE_CONTRACT.md`, now
`docs/user/lifecycle.md` (which it
governs) and `docs/README.md` before this file existed.

## Decision

Four identities are kept deliberately separate, each with its own state
machine:

| Identity | Authority | Lifetime |
|---|---|---|
| retained control | properties, tree membership, events, paint state | construction to disposal |
| retained window/session | portable geometry, input, presentation, close order | one host run |
| native host identity | OS presentation and platform services | native create to native destroy |
| compatibility handle lease | bounded native/GDI interoperability only | first acquisition to disposal |

A retained identity is never cast to or returned as a native handle. A lease
never becomes the visual or input authority for the retained tree.

The portable session phase machine is
`constructed → attached → close_authorized → closed → shutdown`; a cancelled
close stays attached, a forced native close may jump to closed, every other
transition is rejected without mutating retained state, and `closed`
synchronously revokes dispatcher work, frame requests, capture and drag state.

Every presenting backend follows one initialization transaction: construct
controls, construct the window and complete attachment, create and privately
bind the native surface, establish geometry, scale and services, emit exactly
one attach event, publish services, drain one FIFO snapshot of pre-posted work
(where the facade raises `Load` and the application's `ready` runs), commit
and present, then accept input. Work posted during that drain waits for an
ordinary later turn.

Retained attachment (`detached → attaching → attached → detaching →
detached`, `disposed` terminal) is a separate axis: transactional, rolled
back completely on failure, and never tied to a platform window.

On Windows, reading `Control.Handle` through the compatibility facade leases
an invisible, disabled, offscreen native surface (`absent → leased →
released`) raising `HandleCreated` once and `HandleDestroyed` once. It is not
parented, shown, focused, hit-tested or composited.

## Consequences

- The headless adapter is the reference ordering; every native adapter must
  produce the same portable phase trace for equivalent events, and a
  conformance suite enforces the required cases (rejection before attach,
  duplicate attach, cancelled and allowed close, closed-accepts-only-shutdown,
  `Load → input → closing → closed`, single-turn initialization drain, exactly
  one handle event pair).
- Platform-specific creation quirks are contained inside adapters: Win32
  fences creation-time messages until attach; AppKit completes services before
  `makeKeyAndOrderFront:`; X11 maps after the drain.
- `IsHandleCreated` means the lease and nothing else; retained attachment and
  the top-level host window are queried separately.
- The detailed contract text lives in `docs/user/lifecycle.md`; this
  record fixes the decision and its rationale.

## Rejected options

- **One fused "created" state.** Rejected: it would reintroduce the ambiguity
  that makes reparenting, headless testing and handle interop unsafe.
- **Per-host lifecycle orders.** Rejected: it multiplies the conformance
  surface and makes traces platform-specific, contradicting ADR-003.
- **Make the compatibility lease a real native child.** Rejected: a native
  child would compete with the retained tree for composition, hit testing
  and accessibility.
