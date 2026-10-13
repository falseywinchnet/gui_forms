# Stage C: stable installed C ABI

Release: 0.5 ABI candidate. Depends on: T1 (the schema it exposes), T2 (the
engine it exposes). Unblocks S. Status 2026-10-12: an experimental 0.x ABI
(version 0.26) is built and tested but not installed. Authority:
`planning/FORWARD_PROGRAMME.md` §5 C and the 1.0 conditions in
`docs/dev/roadmap.md`.

Read [the shared brief](../README.md) first.

## Why

The owner, 2026-10-12: "the ABI should be our own ABI, but to the degree
that we intend to present plug-and-play 'swap our framework for WinForms, get
cross-platform capability' to C# and others in Visual Studio, we should
superset that capability into what it presents." So the ABI is GUI.Forms'
own, not a Windows Forms ABI, and it is a superset of what the Forms-shaped
C# surface (S) needs.

This was a hidden dependency found while planning: the old SDRSharp facade
already calls the C ABI through P/Invoke, but that ABI is experimental and
not installed. The C# sibling's "intermediary layer" is this ABI, finished.
`AGENTS.md`: "Do not freeze an ABI merely because the current frontend
experiment calls it", and "no compiler-specific type may be assumed to be the
eventual stable C ABI".

## What exists today

- `include/gui_forms/c_api.h` and `c_api.hpp` ("Experimental 0.x ABI.
  Nothing in this header is a GUI.Forms 1.0 promise"), versions
  `GF_ABI_VERSION_0_1` to `0_26`; `include/gui_forms/drawing_c_api.h` for
  GUI.Drawing.
- Implementation in `src/abi/`: `c_api.cpp`, `control_adapters/` (including
  `abi_property_object_control`), `error/abi_error.*`, `registry/`,
  `paint_endpoint/`, and the drawing C API with its platform files.
- Tests: `tests/c_api_c11_tests.c`, `c_api_cpp_tests.cpp`,
  `abi_registry_lifetime_tests.cpp`, `drawing_c_api_c11_tests.c`,
  `drawing_raster_c_api_tests.cpp`.
- Ownership model (interview GF017): parents hold strong references to
  children; children hold non-owning parent links; detached controls die on
  their last strong release; the C ABI uses opaque generational handles so a
  stale numeric identity cannot become a use-after-free; subscriptions are
  tokenized and weak where a strong edge would cycle.
- Master plan §4.10 and M11 describe the earlier ABI plan: gate A1 (C11 and
  C++20 consumers against an exported-symbol allowlist; stale handles,
  retain/release, reparenting, callback disposal, errors, allocator mismatch,
  thread violations and version negotiation under ASan, UBSan and TSan) and
  gate A2 (C# gallery and an SDR-style plugin sample, repeated load and unload
  with no leaked callbacks or handles).

## What to deliver

1. Versioned function tables with version negotiation, installed with the
   SDK (`find_package(GUIForms)` consumers get the header, the library and the
   export allowlist).
2. An error model: every call returns an explicit status; error detail is
   retrievable without global mutable state crossing threads.
3. The T1 schema exposed through the ABI (enumerate control types, parts,
   states, properties and interactions; set properties and register
   interactions by schema identity), and the T2 engine's surface (load a
   theme, publish named inputs).
4. A breaking-change audit of the function tables, with the C++ wrappers as
   RAII conveniences over them rather than a second contract.
5. Two independent native consumers passing the ownership, thread and
   callback conformance tests against the installed SDK.
6. Coverage of what S needs: the capture tool (`tools/compat_capture/`)
   measures which Forms members real applications use; the ABI supersets
   that set.

## How

- House style §9 governs every function: state types, counts, ownership,
  destruction, errors, callback context and identity validity; contain
  exceptions at the boundary; keep language-specific layouts behind it.
- No C++ standard-library type, compiler-specific layout or Skia type in the
  ABI (`AGENTS.md`; resources record §1).
- Callbacks (house style §8): a callback is a function pointer plus an
  explicit context; registration returns a token; revocation during emission
  is defined; callbacks run on the UI thread of the owning window, and
  calling from another thread fails with a status, never undefined
  behaviour.
- Keep the experimental ABI working until consumers migrate; the facade (R)
  and the examples use it.

## Gate

The breaking-change audit and the two-consumer conformance condition of the
1.0 list in `docs/dev/roadmap.md`: the C function tables and error model pass
a breaking-change audit, the C++ wrappers are RAII conveniences over them, and
two independent native consumers pass the ownership, thread and callback
conformance tests against the installed SDK. Record the audit and the
conformance runs under `results/`.

## Owner decisions, 2026-10-13

1. **Consumers.** PlaySuite and Plan Paint both count, plus new ones: first
   the interface builder (D), second the File Manager demoboard program
   (`file_manager_demoboard/`). PlaySuite and Plan Paint can carry this
   stage's gate at 0.5; the builder and the demoboard follow as they move to
   the installed SDK and the stable ABI.

## Read first

- `include/gui_forms/c_api.h`, `src/abi/`
- `planning/interview/GUI_FORMS_INTERVIEW_LEDGER.md` GF017, GF014–GF016
- `planning/MASTER_IMPLEMENTATION_PLAN.md` §4.10 and M11
- `docs/user/consume-the-sdk.md`, `docs/dev/roadmap.md` "1.0 Stable"
