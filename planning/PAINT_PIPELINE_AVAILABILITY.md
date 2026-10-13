# GUI.Forms paint-pipeline contract and availability

Status: **GIVEN / MEASURED PARTIAL / REQUIRED CLOSURE**. Date: 2026-08-06.
Amended 2026-10-13 (contract version 2): damage-preserving partial present
moves from the optional extensions into the required contract (stage DP,
owner decision 3).

## Required invariant

**GIVEN:** mutation frequency may exceed backing-render frequency, and
backing-render frequency may exceed presentation frequency, without an
unbounded queue or incomplete pixels becoming visible.

This is a baseline GUI.Forms correctness contract. It is not a platform feature
that an adapter may report as unavailable. A conforming host must provide the
behavior itself or use the portable fallback. Optional host capabilities may
describe zero-copy import, presentation fences or external native surface
leases; they may not disable coherent latest-state presentation.

**GIVEN (contract version 2, 2026-10-13):** a host presents the bounded
rectangle set it paints, not the union of those rectangles. Pixels outside the
set are neither repainted nor copied to the screen, so two distant changes
cost the sum of their areas. A candidate raster that swaps atomically (M12-P2)
carries the presented front forward and copies only the rectangle set, and the
swap stays all-or-nothing. Hosts compose and present in one frame sequence
(gather, compose, present; `planning/stages/DP-damage-preserving-painting/`
decision 2), so the screen is always a copy of the raster and a frame capture
is evidence of what users see.

This was an optional availability item until every host met it:

| Host | Rectangle-set present | Evidence |
|---|---|---|
| Windows, default profile | each rectangle copied separately; live frames through the frame sequence | #59, #72, #74; `results/2026-10-13-dp-gate-windows` |
| Windows, transactional DIB profile | `DibFrameStore::commit` copies each rectangle, swaps only when all succeed | #73; `test_rectangle_set_commit` |
| macOS | AppKit clips each copy to the rectangles being drawn | #60, #74 |
| X11 | each rectangle converted and copied with `XPutImage` | #76, #77; PlaySuite's Lima `x11_screen_check.py` runs (changes stay inside the damaged rectangles) |

The three commits have distinct authorities:

| Commit | Current authority | Meaning |
|---|---|---|
| state | `content_revision` | retained properties, layout, drawing, or resources changed |
| backing surface | `rendered_revision` | one exclusive lease completed a coherent candidate |
| presentation | `presented_revision` | the host released the latest completed raster at its presentation boundary |

`PaintLeaseSnapshot` exposes these revisions, surface epoch, lease outcomes,
the coalesced-wake counters/bit, and the exact diagnostic states `clean`,
`dirty_queued`, `rendering`, `rendering_dirty`, `ready`, `occluded_dirty`, and
`retired`. The old `dirty` spelling remains an alias for source compatibility.

## Premise ledger

| Attached premise | Status | Evidence or required gate |
|---|---|---|
| State, backing completion, and presentation are separate | **MEASURED** | independent content/rendered/presented revisions; exact revision/epoch `PaintReceipt`; native acknowledgement after successful copy |
| Many touches collapse into one queued render opportunity | **MEASURED** | one `paint_wake_pending` bit; repeated invalidations increment a coalesced counter rather than enqueueing another wake |
| One active paint lease | **MEASURED** | UI-thread-exclusive `Window::paint`; nested paint becomes `rendering_dirty` and returns |
| One deferred follow-up while rendering | **MEASURED** | `dirty_after_render`, later damage, and revision comparison preserve one latest-state pass; no revision jobs are stored |
| Separate ready and presented stages | **MEASURED** | a complete current-epoch replay returns a receipt and becomes `ready`; only an accepted exact host release advances `presented_revision`; duplicate/backward/replaced receipts are counted and rejected |
| Spatial dirty coalescing | **MEASURED** | bounded multi-rectangle `DamageRegion`, per-plane damage, dirty-subtree traversal, and retained chunk reuse are real; `Window::paint` paints each rectangle of the set under its own clip, and every host presents the set rather than its union (contract version 2; table above) |
| Temporal coalescing | **MEASURED** | a queued wake is boolean and revisions are sampled at lease start; intermediate revisions are never replayed as jobs |
| Last completed surface survives stalls | **MEASURED PARTIAL** | host CPU rasters persist across unrelated paints and callback failure never replays a partial command list; backend replay can still alter the live raster before throwing |
| Callback failure cannot expose partial owner paint | **MEASURED** | candidate display commands are complete before replay and retained chunks/damage restore on callback failure |
| Backend failure cannot expose partial raster | **REQUIRED M12-P2** | replay into a candidate raster and atomically swap only after success; preserve the prior raster on allocation, renderer, or upload fault |
| Occluded surfaces merge work without churn | **MEASURED PARTIAL** | 100 deterministic occluded mutations produce no wake/paint, exposure produces one wake and one latest-state render; prolonged native wall-clock soak remains |
| `DoubleBuffered` is idempotent, reflected, and retained | **MEASURED PARTIAL M11h-P1** | protected getter/setter and `GetStyle`/`SetStyle` agree under physical Wine; repeated assignment retains one surface while disable retires it. Handle-recreation and subclass-order corpus remain |
| `DoubleBuffered=true` isolates compatibility drawing | **MEASURED PARTIAL M11h-P1** | managed owner paint reuses one size-matched PArgb surface; background and foreground share one `Graphics`, resize rejects the stale epoch and posts one replacement pass, and callback failure preserves the last published raster without self-retry. Direct GDI uses its separate private HWND/DIB lease |
| GDI `Flush`/`ReleaseHdc`/`EndPaint` are dirty signals | **MEASURED PARTIAL M11h-P1** | the generated bridge now updates a persistent private Graphics bitmap, marks revisions at known boundaries, and imports once per callback/posted drain. Sixteen Flush calls plus ReleaseHdc produce one Wine capture/import. Rectangle damage and full overload ordering remain |
| `Update()`/`Refresh()` may synchronously drain only their target | **MEASURED PARTIAL M11h-P1** | target-scoped facade drains are real; Update during OnPaint reaches depth one and one deferred follow-up under Wine. Broader .NET ordering/exception parity remains |
| Managed `Invalidate` preserves useful damage | **MEASURED PARTIAL M11h-P1** | all rectangle/region/child overloads clip and synchronously notify; queued rectangles union before one persistent owner-paint lease, `PaintEventArgs.ClipRectangle` and `Graphics` clip agree, failed leases restore consumed damage, and ephemeral paint promotes to full safety. Region damage is conservatively bounded and native upload remains full-surface |
| Input cannot re-enter application painting | **MEASURED PARTIAL M11h-P1 / M12-P3** | generated managed pointer/key/text ingress uses one UI-thread-local queue bounded at 1,024 entries; physical Wine proves pointer-before-key delivery, depth-one paint, one localized follow-up, and disposal abandonment. Portable `Window` applies the same outermost-lease boundary to pointer/key/text, drag, and stable-identity semantic actions, compacts adjacent pointer moves and same-session drag overs, isolates per-event faults, and exposes `DeferredInputSnapshot`. Normal and renderer-free tests prove 100 moves plus down/key/text become four ordered deliveries, 32 drag overs reuse only a prior valid effect and become one delivery, semantic Press resolves once after release, and a 1,025-key retirement probe retains the fixed bound and reports one rejection. Managed key-preview return parity, physical native reentry, and stall breadth remain |
| No catch-up loop under sustained producer pressure | **MEASURED PARTIAL / REQUIRED M12-P3** | 100 replay-boundary mutations, one deferred pointer, and eight nested paint attempts produce one old receipt, one latest-state follow-up, one wake/drain, exact out-of-order receipt rejection, and zero idle work. Backend fault, replay-time resize, retirement, and 100-mutation occlusion gates also pass. Native wall-clock/cadence soak remains |

## Named closure gates

### M11h-P1 — compatibility-surface drain (**measured partial**)

- Give every admitted compatibility surface dirty revision, merged dirty region,
  queued-drain bit, active-lease bit, and deferred-follow-up bit.
- Treat `Graphics.Flush`, `ReleaseHdc`, `EndPaint`, and callback return as touch
  boundaries. They finish drawing visibility to the private surface but do not
  force whole-window capture/presentation.
- Capture/import only the newest dirty revision at the next drain. Known
  boundaries create no polling work. Uninstrumentable foreign HDC writers use
  a bounded adaptive 33–250 ms hash probe that backs off while unchanged.
- Implement target-scoped `Update()` and `Refresh()` without a nested message
  loop, unrelated-surface traversal, or paint reentry.

The revision/queue/lease center above is implemented and physically measured in
`experiments/M11H_COMPATIBILITY_SURFACE_DRAINS.md`. Managed owner-paint damage
is now rectangle-aware, and both managed compatibility input and portable
retained pointer/key/text input wait outside active leases; arbitrary
nonrectangular region fidelity,
rectangle-aware direct-GDI damage, managed/native presentation acknowledgement,
physical native-host input reentry and the full ordering/failure corpus remain before
the combined M11h-P1/M12-P3 boundary is closed.

### M12-P2 — atomic candidate raster

- Replay each completed retained transaction into a private candidate raster.
- Publish it as latest-ready only after every backend command succeeds.
- Keep the last presented raster usable across allocation, renderer, upload,
  device-loss, and callback faults.
- Preserve multi-rectangle damage through host copying: carry the presented
  front into the candidate and copy only the rectangle set (contract version 2).
  Copying the union of the rectangles is no longer conforming.

### M12-P3 — pressure, input, and stall conformance

- Inject at least 100 mutations per presentation with a deliberately slower
  renderer and presenter.
- Assert at most one queued drain, one active lease, and one deferred follow-up
  per surface at every observation.
- Assert monotonic revisions, latest-state rendering, no intermediate-job
  backlog, no incomplete visible frame, and return to zero idle work.
- Repeat while occluded, exposed, resizing, failing replay, dispatching
  synthetic input, and disposing the owner. Apply the managed queue invariant to
  native adapters; the portable retained queue and its fixed capacity are now
  measured in `experiments/M12P3_RETAINED_INPUT_PRESSURE.md` and
  `experiments/M12P3_PRESENTATION_PRESSURE_AND_FRAME_FAULTS.md`.

### M11h-P4 / M12-P4 — availability projection

The baseline invariant is reported as a contract/version, not a capability
bit. Availability may truthfully expose only optional or platform-bound
extensions:

| Availability item | Required meaning |
|---|---|
| native compatibility-surface lease | opaque platform lease exists; no HWND/HDC enters portable control API |
| zero-copy surface import | completed private pixels can be imported without a CPU copy |
| explicit presentation receipt/fence | host can distinguish submission from compositor acknowledgement |

Damage-preserving partial present (the host consumes a bounded rectangle set
instead of its union) left this table with contract version 2: it is part of
the contract. `PresentationExtensions::partial_present` stays in the record
for readers of version 1, and is true for every host that declares version 2;
`Window::declare_presentation` refuses a version-2 declaration without it.

The availability record (`Window::presentation_availability()`) publishes the
mandatory contract version, the extensions and the live diagnostic snapshot.
If a host cannot meet the contract, attachment fails; it must not silently
advertise a degraded mode. No host offers the three extensions above yet.

## Acceptance

This contract is complete only when the focused model tests, renderer fault
tests, strict Win64/Wine probes, native macOS probes, and named Linux host lanes
all pass the same state/revision/queue assertions. A visually flicker-free demo
alone is not evidence of temporal coalescing or atomic presentation.
