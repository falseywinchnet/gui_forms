# GUI.Forms implementation guardrails

This subtree is the independent GUI.Forms proving project. It may be built from
this directory without File Manager. The repository root may consume it; this
subtree may never include, link, inspect, or depend on sources outside
`gui_forms/`, except pinned third-party dependencies explicitly declared by its
own build.

## Code style (mandatory)

All first-party C++ and Objective-C++ in this repository follows
`planning/PROGRAMMING_HOUSE_STYLE.md`, a verbatim copy of
`programming-house-style.md` in `falseywinchnet/EpistemicGPT`; change both
together. Read it in full before writing or changing code. One line per
section, so a skim still covers all eleven:

1. **Types and values:** spell types; separate confusable quantities (byte
   offsets and character positions, identities and indices) with wrapper
   types; one numeric precision per computation; named record fields, enums
   and explicit initialization; state array shape and capacity versus live
   length.
2. **Statements and expressions:** write in execution order; no lambdas or
   anonymous callbacks; one recognizable operation per expression; calculate,
   then return; break chains that hide types, state or allocation.
3. **Repeated kernels:** choose mode and implementation before the loop;
   handle empty inputs and edges apart from the interior; validate whole
   buffers up front; no per-element allocation, logging, locking or
   callbacks; numerical shortcuts only within stated error bounds.
4. **Procedures and data:** state inputs, outputs and permitted mutation;
   fill caller-supplied storage for bulk work; name intermediate forms; state
   overlap rules.
5. **Memory use:** every datum has an owner and a lifetime; allocate before
   repeated processing and reuse; transfer ownership explicitly; raw
   addresses only at boundaries; name storage operations (fill, reserve,
   reset, clear, release) precisely; check size arithmetic; keep borrows
   valid through use (a function returning a borrow of an argument deletes
   its rvalue overload; declare a borrower after what it borrows); memory
   errors and undefined behaviour are defects in tests too, checked by
   sanitizer runs of the whole suite.
6. **Objects and authority:** parents own children and back links only
   observe; cross authority boundaries through named capabilities; define
   the fallback when an owner disappears (reject stale identities, stop
   revoked callbacks).
7. **Admitted abstractions:** standard algorithms with named predicates;
   templates for declared choices; virtual dispatch and type erasure only
   where runtime variation is required; no universal callable frameworks or
   speculative portability layers.
8. **Callbacks and execution:** named targets with explicit context; specify
   event order, reentrancy and revocation; name asynchronous work,
   completion, cancellation and shutdown; one execution context per piece of
   mutable state.
9. **Failure and foreign interfaces:** say what a failure leaves behind;
   validate before mutating; explicit results for expected alternatives and
   exceptions only at subsystem boundaries; foreign interfaces state types,
   counts, ownership and errors, and take entry-point types from their
   declarations, never retyped by hand.
10. **C++ spelling:** no `auto`, trailing returns or structured bindings;
    `(*pointer).member` instead of `->`; no lambdas; hand-written comparisons
    (no `= default`); no coroutines or ranges pipelines.
11. **Source layout and comments:** meaningful source boundaries with the
    main sequence easy to find; no accessors or wrappers that add no
    invariant; explain non-obvious dimensions, lifetimes and surprising
    operations; remove stale comments.

Match the surrounding code's naming and comment density.

Before committing, run the spelling check over everything you touched; the
`house-style` CI job runs it over every first-party directory and fails on any
finding:

```text
python3 -B tools/check_house_style.py include src demo tests examples tools file_manager_demoboard stx_vorbis
```

The check covers spelling only. Ownership, lifetime, initialization and
naming still need review against the full document.

## Forward programme (read before starting new work)

The current programme (articulated controls, one theme engine, the design
language, accessibility verification, the C ABI, the C# sibling and the
visual tools) is defined in `planning/FORWARD_PROGRAMME.md`, with the visual
rules in `planning/DESIGN_LANGUAGE_SPECIFICATION.md`. Find your stage there,
start it only once its dependencies' gates are recorded, and record your own
gate's measurement. Open owner decisions are listed in both files; do not
assume them. Each stage has a working brief in `planning/stages/` (why, what,
how, current code, house-style points, open decisions and lessons from the
owner's discussions); read `planning/stages/README.md` and your stage's
brief before starting. A readable stage board for sibling threads is published at
https://claude.ai/artifact/D9d2SUvkiKQETiVnmwnEt4 (read it with the Artifact
tool); the planning files are authoritative where they differ.

## Scope of the first proving slice

Build a native macOS control-gallery demonstration and automated tests proving:

- retained controls and explicit ownership;
- stateful imperative mutation;
- typed layout/paint/hit-test dirtiness;
- nested update scopes and bounded flushes;
- damage-limited CPU painting;
- Forms-like focus, pointer, keyboard, and activation behavior;
- a Web.Forms-produced compiled/static form description with stable IDs, with
  the checked-in Gallery DML retained only as provisional evidence;
- instrumentation visible in logs and an optional on-screen diagnostics panel;
- CPU-only Skia hidden behind GUI.Forms drawing vocabulary;
- and a native AppKit host which can be launched, inspected, and closed.

This is a control gallery, not File Manager and not a file browser.

The separately specified `file_manager_demoboard/` is an admitted first-party
consumer exception to the gallery's visual subject: it reproduces the File
Manager interface using deterministic in-memory fixtures and only public
GUI.Forms seams. It is still not the shipping frontend, may not read the real
filesystem or parent sources at runtime, and may not implement missing reusable
GUI.Forms capabilities inside the consumer project. Its local `AGENTS.md` is
mandatory for work in that directory.

## Hard boundaries

- C++20 is admitted. Objective-C++ is admitted only in the macOS host adapter.
- No .NET, Java, browser engine, or Godot dependency.
- GUI.Forms has no GPU capability. Do not compile, link, initialize, probe, or
  expose Ganesh, Graphite, GL, Vulkan, Metal, Direct3D, Dawn, or WebGPU.
- Skia may be used only through a private CPU renderer adapter. No Skia type may
  appear in public headers, DML, demo-facing control APIs, C ABI, or tests of the
  renderer-neutral core.
- **GIVEN:** the GUI.Forms core and host protocol target Windows, macOS, and
  Linux equally. No portable semantic, event order, path/dialog model, or
  scheduler rule may encode AppKit, Win32, Wayland, or X11 assumptions. Honest
  platform differences are capability-reported adapter behavior, not hidden
  core policy. Private Skia extensions may use platform-specific CPU details but
  may not change or leak through the portable contract.
- PNG is the only image decoder admitted to the renderer/resource core. Other
  codecs are excluded.
- AppKit types remain inside `src/host/macos/`.
- Win32, COM, WIC, MSAA, and UIA types remain inside `src/host/windows/`.
- The core is retained; do not introduce an immediate-mode control API or a
  perpetual redraw loop.
- ADR-013's reservation of persistent side panels to File Manager was retired
  by the owner on 2026-10-13: any consumer may compose panels from the
  reusable panel controls, and owned dialogs remain first-class capabilities.
- Authored accessibility/help metadata is optional secondary hook data and is
  not required to instantiate a control. Supported stock controls still supply
  default semantic adapters; File Manager requires native accessibility
  publication and does not treat it as a user-disabled product mode.
- Portsmouth Rapids 1.0 faces are supplied for the current Gallery.
  That bounded historical proving slice retains Lucida Grande for editable,
  collection, metric, and field content until its fixture is deliberately
  migrated. **GIVEN forward direction:** M4/M9 use HarfBuzz, FreeType, bundled
  Portsmouth control faces, a selected bundled Tahoma/Calibri-like body face,
  and bounded bundled fallback packs. Portsmouth is the repository owner's own
  typeface, bundled under MIT (`assets/fonts/LICENSE-PortsmouthRapids.txt`);
  body-face coverage assets remain a gate; do not silently use arbitrary host
  fonts.
- Preserve user files and unrelated working-tree changes.

## Public seam

- Portable C++ headers live under `include/gui_forms/`.
- Renderer-neutral implementation lives under `src/core/` and
  `src/controls/`.
- Skia implementation lives under `src/render/skia/`.
- Native macOS code lives under `src/host/macos/`.
- Native Windows code lives under `src/host/windows/`.
- Demonstrations and their provisional static descriptions live under `demo/`;
  authoritative Web.Forms language work lives in the sibling `web_forms/`
  project and consumes only an explicitly negotiated public manifest.
- The standalone fixture-backed File Manager consumer specification and its
  eventual installed-package consumer live under `file_manager_demoboard/`.
- Tests live under `tests/`.
- Instrumentation must be queryable as structured counters/snapshots, not only
  prose logs.

The public ABI may begin as disciplined portable C++ for the spike, but no
compiler-specific type may be assumed to be the eventual stable C ABI.

Cross-project consumption is negotiated through
`docs/notes/ORCHESTRATOR_INTERFACE_NEGOTIATION.md`. GUI.Forms records its reply and
evidence there; Orchestrator reconciles the canonical program contract. Do not
freeze an ABI merely because the current frontend experiment calls it.

## Agent ownership during the initial parallel build

- Core agent: `include/gui_forms/`, `src/core/`, core instrumentation, and core
  tests.
- Control-gallery agent: `src/controls/`, `demo/`, demo resources, and gallery
  interaction tests.
- Skia/host agent: `third_party/`, `src/render/skia/`, `src/host/macos/`, and
  dependency/build integration specific to those paths.
- Root agent: guardrails, cross-cutting build integration, final review,
  execution, visual inspection, and closure.

Do not edit another agent's owned files without first messaging that agent and
root. Communicate interface needs early. Preserve negative results and report
exact commands and environment.

## Build contract

The lower build must eventually support commands equivalent to:

```text
cmake -S gui_forms -B gui_forms/build
cmake --build gui_forms/build
ctest --test-dir gui_forms/build --output-on-failure
gui_forms/build/.../gui_forms_gallery
```

No benchmark result may be labelled fast or lightweight without recording the
workload and measurement. A working demonstration is evidence only for the
behaviors it exercises.
