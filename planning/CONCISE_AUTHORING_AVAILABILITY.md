# Concise control authoring: GUI.Forms availability plan

Date: 2026-10-07.

Status: **GIVEN owner direction; event, state and collection development implementation; CANDIDATE grid,
form and skin contracts.** This record does not accept a new architecture or
advertise the unimplemented capabilities as available.

## Objective and authority

The 2026-10-06 proposal, “making GUI.Forms controls less work than WinForms,”
asks for concise retained control declarations, one declarative layout engine,
named owner-held handlers, rich reusable skins, and a common target for hand
authoring and generated forms. All four goals remain requirements: expressive
appearance, bounded runtime cost, precise design, and readable house-style C++.

**GIVEN:** this work belongs to GUI.Forms. Do not edit PlaySuite or manage its
adoption. GUI.Forms publishes working capabilities, installed examples, tests
and limitations; application managers decide when and how to consume them.
The proposal's PlaySuite pixel comparisons and authoring trials remain useful
consumer acceptance evidence, but cannot be claimed from a toolkit fixture.

**GIVEN:** compact owning handles are acceptable in plain form structs when
they conform to `planning/PROGRAMMING_HOUSE_STYLE.md` and retain the existing
parent-owns-child model. By-value controls with borrowed tree links are not
required. A source record remains data; named owner methods hold behavior.

GUI.Forms owns retained construction, layout, parts, state, input, semantics,
inspection and drawing. Web.Forms owns the bounded HTML/CSS grammar, lowering
and generated-source profile. A browser designer is an authoring tool, not an
admitted runtime dependency. These boundaries follow the existing design-
language system model and ORC-GUI-002; this proposal does not reopen them.

## Current facts and availability

| Capability | Status at this change | Public evidence or remaining requirement |
|---|---|---|
| Owner-held `on(event, owner, &Owner::method)` | **Implemented development C++** | `event.hpp`, API reference, compiled `examples/reference/owned_events.cpp`, `owned_event_tests.cpp` |
| Immediate publisher teardown | **Implemented development C++** | 1,000-button lifetime/allocation test; no retained owner token or store capacity after publisher destruction |
| Omitted trailing event arguments and inline bound values | **Implemented development C++** | Zero, one or two bound values; 16-byte total; compiler diagnostic fixtures |
| Shared `Value<T>` and non-owning `control.bind(model)` | **Implemented development C++** | Scalar values, CheckBox, RangeControl, ChoiceGroup; synchronous notifications and validation |
| Shared Command binding and shortcuts | **Implemented development C++** | `ButtonBase::bind`, checked/enabled projection, `Command::bind_shortcut`; existing owning CommandBinding remains compatible |
| CommandBar, ChoiceGroup, ExpandableSections | **Implemented development C++** | Retained item controls, group events, public named item/header/body parts; installed examples and lifetime tests |
| Item list activation | **Existing** | `ListBox::item_activated()` reports `std::size_t`; no duplicate list control introduced |
| Explicit token transfer | **Implemented development C++** | `Component::own_subscription`; ordinary caller-held tokens remain supported |
| Retained parent ownership and mandatory stable IDs | **Existing** | `Control`, `make_control`, attachment/lifecycle tests |
| Table and flow controls | **Existing; not the proposed common grid** | Separate `layout_children` implementations and `layout_panel_tests.cpp` |
| Fixed/auto/weighted tracks with bounds, cell alignment and concise grammar | **Not available as `Grid`** | Grid tranche below; no advertised placeholder API |
| `Options` on every built-in and one form attachment operation | **Not available** | Form tranche below; inventory and per-control coverage required |
| Named parts across every control and two complete skins | **Not available** | Skin tranche below; existing palette is not full coverage |
| Struct-producing HTML subset and round-trip browser designer | **Not available from GUI.Forms** | Negotiated provider manifest first; implementation remains with the authoring project |

**OBSERVED:** the current table path creates occupancy rows, item/child
snapshots, minimum/span arrays and offsets per layout. Flow creates item and
line vectors and a live-child set. Neither is evidence for an allocation-free
common layout engine. Existing demand-driven painting and live surfaces are
valuable baseline capabilities, but do not establish that every new authoring
or theme path meets the proposal's cost goal.

## Delivery A: named handlers

Available syntax: `gui_forms::on(event, owner, &Owner::method)`. The owner must
explicitly derive from `Component`; no method-name detection selects lifetime
behavior. Ordinary, const, noexcept and inherited member pointers preserve the
event's exact argument types. Null members fail in release and debug builds.

The owner stores the token, and the event stores the named member binding with
its slot. The binding borrows the owner without a shared ownership cycle.
Disposal revokes before the owner's disposal hook; natural Component
destruction also revokes. An active invocation retains its binding even if it
disposes the owner or destroys the publisher. A derived destructor that emits
events must dispose before tearing down handler state.

The existing registration boundary, nesting, exception and cancellation rules
apply. Registering with a dead owner is a no-op. A failed registration leaves
no live connection; an independently cancellable connection still uses a
caller-held `SubscriptionToken`. No event helper starts a timer, posts work or
adds an idle poll.

Token storage uses links inside the existing connection slots. Publisher
disconnection immediately unlinks and releases its owner-held connection.
There is no separately allocated owner token store or retained vector capacity.
Caller-held and owner-held revocation still share reverse acquisition order. Member emission has no
`std::function` copy or allocation. This is not a claim about arbitrary legacy
owning-callable emission or user handler work.

Acceptance includes real button, text and timer event signatures; const and
inherited handlers; natural/explicit disposal; publisher and target destruction
during dispatch; nested registration; null members; exceptions; token transfer;
allocation failure at every registration allocation; and allocation counting
over 10,000 emissions. Platform results and the source review are recorded in
`../experiments/CONCISE_AUTHORING_EVENT_FOUNDATION.md`.

## Required follow-up before form records: events, shared state and collections

**GIVEN:** all requirements in the 2026-10-07 PlaySuite sibling follow-up are
mandatory GUI.Forms deliverables, not optional extensions. The delivery order
is publisher cleanup (0), omitted arguments and small typed bound values (1–2),
shared state (3), and item-reporting collections (4). Form records depend on
these capabilities. The independent grid work does not remove or replace them.
No PlaySuite source or adoption work belongs in this task.

The development implementation covers:

- Immediate owner-token reclamation after publisher death, tested with 1,000
  short-lived buttons and balanced live allocation counts.
- Named handlers accepting a leading prefix of event arguments, including none;
  one or two trivial bound values of at most 16 total bytes, inline in the
  existing connection allocation. Registration allocates; bound payload storage
  and steady-state dispatch do not introduce separate allocations.
- Application-owned scalar Value and Command state. Controls borrow models,
  stop using them after model disposal/destruction, and do not echo matching
  model state into the originating control. Programmatic setters raise state
  notifications without input-only notifications. Bind-time model state wins.
- Command bar item identity and Command/shortcut association; choice-group
  radio state and Value binding; retained expandable sections with individual
  model state and optional single-open behavior. Group subscriptions survive
  replacement. Existing ListBox provides typed item activation.
- Named retained skin parts: `item` on command/choice groups; `header` and `body`
  on sections. These expose the existing controls' visual recipes, image/text
  layout and typography. They are the conserved group anatomy for the later
  skin tranche, not a claim that two complete new skins have shipped.

Required acceptance remains the complete sibling request: destruction in either
order and during dispatch; no propagation loops or steady-state allocation;
item rebuilding, keyboard and accessibility; installed-SDK examples; ownership
and error contracts; dispatch/idle/paint evidence and coherent C++ rebuild notes.
See `../docs/CONCISE_STATE_AND_COLLECTIONS.md` for exact semantics and limits,
and `../experiments/CONCISE_STATE_AND_COLLECTIONS.md` for measured qualification.
All platform claims must follow actual results; source availability alone is
not native-host qualification.

## Delivery B: one retained grid and compatibility migration

**CANDIDATE recommendation:** expose both typed tracks and compact strings.
Typed tracks are the canonical model; the compact string is a construction-time
parser, never a second layout engine. Validate in release builds as well as
debug. The initial token forms are fixed nonnegative logical units, `auto`,
`*`, and positive weighted `2*`. Specify min/max spelling and diagnostics before
publishing it to Web.Forms; do not silently accept CSS beyond that grammar.

The grid owns its immutable parsed tracks and reusable measure/arrange
workspace. Fixed, auto and weighted tracks each carry explicit minimum and
maximum constraints. Cells carry row, column, spans and independent axis
alignment. Existing control margin belongs outside the control's allocated
rectangle; grid padding and inter-track gaps have separate meanings.

Resolve before implementation:

- finite dimensions, count/span limits, empty input, negative and zero weights,
  overflow, invalid enum values, and errors with source positions;
- deterministic span demand distribution, clamping and redistribution when a
  weighted track reaches its maximum, and overflow when minima cannot fit;
- precedence of Dock versus cell alignment versus Anchor, with WinForms
  compatibility cases covering all edge combinations and resizing;
- hidden versus removed cells, duplicate control placement, explicit versus
  automatic placement, z order and stable-ID collisions;
- measurement callbacks that mutate, reparent or dispose controls, and nested
  layout entry while a workspace is active; reusable scratch cannot invalidate
  a borrowed child snapshot during a callback.

Migration must make table and flow adapters over the same engine. Table keeps
its growth, span, border and lookup behavior. Flow lowers wrapping, flow breaks,
reverse direction, grow weights and all existing alignments into tracks/cells.
It cannot become a wrapper around a second private arranging algorithm. Keep
compatibility tests as the reference and remove replaced solvers only after
their adapter tests pass. ScaledPanel's coordinate-transform contract needs an
explicit compatibility review; it is not silently redefined as a grid.

Provider acceptance: deterministic expected geometry and raster comparisons at
600 × 420 and a declared size/DPI matrix; fixed/auto/weighted and min/max cases;
all cell alignments and spans; Dock/Anchor; flow wrapping and autoscroll; callback
mutation safety; exact dirty-pass counters; and allocation-free repeated layout
after configuration/capacity warm-up. Reconfiguration may allocate, but must
publish valid replacement state atomically or leave the old layout usable.

## Delivery C: plain form records over owned controls

**GIVEN:** preserve shared retained-tree ownership. **CANDIDATE syntax:** a
small explicitly named owning control handle constructed from the control's
`Options`, with ordinary typed-reference access for repeated work. Evaluate that
against an ordinary `std::shared_ptr<ControlType>` and `make_control` overload
before introducing a new wrapper. A convenience type must name construction
and ownership, not hide them behind punctuation.

Every built-in requires a documented Options record, including defaults,
units, ranges, inherited values, invalid states and construction effects.
Mandatory `.id` must reject missing/empty identity through normal validation.
Do not invent `.glyph` or `.radius` properties on stock Button merely because a
consumer subclass has them. The later skin/part contract supplies appearance
without importing that application's private enums or design.

C++20 designated initialization cannot name inherited aggregate fields, so the
public shape of shared control options requires an explicit decision. Compare
flat per-control records with a named common-options member. Avoid macros,
reflection or accidental protocol detection to make the examples look shorter.
The API reference and autocomplete must show the actual accepted fields.

Form records own handles; an attached parent also owns its children. Destroying
the record must not destroy still-attached controls; removing a child must not
destroy a control still held by the record. Grid cells keep a declared owner or
bounded borrow, never an unchecked raw pointer to a movable form member.

`attach(parent, layout)` must validate ownership, cycles and stable IDs before
mutation, and define rollback on attachment notification/allocation failure.
Attaching the same form twice, attaching an already parented control, and
destroying either party all need explicit outcomes. Configuration, construction,
attachment and event wiring remain separate, visible operations.

Provider acceptance: one compile-checked example per built-in Options record,
construction/default/invalid-input tests, an installed-package form example,
transactional attach tests, layout edits confined to that form's data, and
source review against the complete house style. API-reference-only authoring
trials must actually be run and recorded; an existing example passing is not
evidence that an independent person or model succeeded.

## Delivery D: conserved parts and shared scrolling behavior

The part vocabulary must implement, rather than replace,
`DESIGN_LANGUAGE_SYSTEM_MODEL.md` and
`CONTROL_DNA_AND_STYLE_ADDRESSABILITY.md`: conserved nested anatomy, part
relationships, semantic identity separate from style identity, and state that
can be styled without reproducing input behavior.

Start with a coverage inventory mapping each built-in painter to parts and
states. Include face/content/focus, selection/check, track/thumb/arrows and
disabled/hover/pressed/focused/checked combinations. Theme metrics affecting
layout invalidate layout; paint-only changes invalidate painting. Theme swaps
must preserve focus, capture, selection, semantic bounds and active edits.

Default vectors, bounded local PNG nine-slice assets and explicitly trusted
named drawing callbacks are candidates. Do not promote arbitrary executable
skins or assume asset licensing. The current palette becomes a compatibility
theme. Ship two complete, visibly distinct skins with a coverage report;
changing several palette colors does not satisfy the requested skin work.

Unify scrolling behavior before replacing both painters: extract shared range,
geometry, hit testing, repeat and capture state, or compose ScrollBar where that
preserves ownership and semantics. Compare standalone and embedded keyboard,
wheel, thumb drag, repeat, disabled and accessibility traces. A matching arrow
image does not establish matching behavior.

Provider acceptance: all built-ins in both skins, per-part state coverage,
native raster inspection, no control-specific theme forks, one scrolling
behavior implementation, and recorded idle/paint/allocation baselines before
and after. XP-like skin fidelity needs declared assets and visual references;
it cannot be inferred from the names Royale or Watercolor.

## Delivery E: make a reliable target available to authoring tools

GUI.Forms supplies a versioned manifest of constructible controls, typed options,
stable IDs, layout semantics, parts/states, diagnostics and source-to-runtime
inspection identity. It also supplies installed C++ examples and validation
fixtures that do not inspect private renderer state. It must be possible to
reject an unsupported feature before generating code.

Web.Forms negotiates that manifest and owns the HTML/CSS mapping and generator.
The browser designer consumes the same model; it is outside this GUI.Forms
implementation change. Native rendering and controls remain authoritative for
the preview fidelity claim. A browser approximation must identify its limits.

**CANDIDATE round-trip policy:** only a declared data region is machine-editable.
Preserve unknown source bytes verbatim, or refuse an unsupported edit with an
exact diagnostic. Do not overwrite code that cannot be represented in the model.
Retain named owner handlers outside generated regions. No regex-only rewrite of
arbitrary C++ is an accepted preservation guarantee.

Acceptance requires unsupported-source diagnostics, deterministic output,
source/struct/model round trips with unknown hand edits preserved, house-style
checking of emitted code, and native compile/run fixtures against an installed
GUI.Forms SDK. GUI.Forms availability does not itself claim those authoring-tool
features have shipped.

## Evidence, review and publication

Each tranche carries exact supported scope, negative results, platform and
compiler, workload, raw timings/counters and sample count. Compare identical
workloads and distinguish layout, dispatch, raster paint, host presentation and
idle cost. Run portable tests on macOS, Windows and Linux; state separately
which native paths were exercised. Do not substitute one platform for another.

Source review covers explicit types, named behavior and retained callback state,
ownership/borrows, evaluation order, initialization, conversions, failure
states and repeated-loop storage. Spelling checks supplement that review. The
stricter generated profile applies only to its declared output, not ordinary
toolkit implementation.

Development C++ additions require a coherent library/consumer rebuild. Publish
provider changes and evidence through the GUI.Forms negotiation record before
Orchestrator reconciles canonical availability. Grid syntax, owning-handle
shape, theme package compatibility and generated form profile remain candidates
until their respective contracts and acceptance gates are resolved.
