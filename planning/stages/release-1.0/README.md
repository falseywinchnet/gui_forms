# Release 1.0: the stability gate

Release: 1.0 Stable. Depends on: every stage. Status 2026-10-12: conditions
defined, none met. Authority: `docs/dev/roadmap.md` "1.0 Stable".

Read [the shared brief](../README.md) first.

## Why

The owner, 2026-10-12, when the public roadmap was drawn: "all is before
1.0." Nothing on the roadmap is deferred past 1.0. 1.0 is the point at which
the API is declared stable, so the conditions are about the contract, not
about features.

## The conditions, and which stage satisfies each

| Condition (from `docs/dev/roadmap.md`) | Satisfied by |
|---|---|
| The C function tables and error model pass a breaking-change audit, and the C++ wrappers are RAII conveniences over them rather than a second contract | C |
| Two independent native consumers pass the ownership, thread and callback conformance tests against the installed SDK | C: PlaySuite and Plan Paint, then the interface builder and the demoboard program (owner decision, 2026-10-13) |
| A machine-readable manifest lists every control and member as supported, deferred or excluded, and the documentation is generated from it | T1 (schema), plus the docs generators (`tools/control_registry.py`, `tools/generate_library_docs.py`) extended to members |
| Windows, macOS and the X11 host pass the same normative traces, including lifecycle, input order, focus and paint receipts | the host work across all stages; ADR-003, ADR-014 |
| Native accessibility publication of the semantic tree exists on every host and is exercised by a test, not only the deterministic snapshot | A |
| Packaging is reproducible: licences, integrity metadata and a software bill of materials ship with every artefact | release pipeline (`.github/workflows/release.yml`); signing, notarization, SBOM and reproducible builds remain |

Every 0.x release's stages (the stage index in the shared brief) are also
before 1.0: 0.2 Verified (A, F, T1), 0.3 Themeable (T2–T5), 0.4 Flexible
(L, FD, DP), 0.5 ABI candidate (C), 0.6 Bindings (S), 0.7 Designers (D, H,
V). R is off the public roadmap but precedes S.

## How

- Do not declare anything stable early. "Do not freeze an ABI merely because
  the current frontend experiment calls it" (`AGENTS.md`).
- Keep `docs/dev/roadmap.md`, its chevron image (`STAGES` in
  `tools/make_roadmap_svg.py`, regenerated with
  `python3 tools/make_roadmap_svg.py`) and the stage index in step.

## Gate

Every condition above recorded with evidence under `results/`, and the owner
declares 1.0.

## Read first

- `docs/dev/roadmap.md`
- `planning/FORWARD_PROGRAMME.md`
- `decisions/ADR-003-observable-ordering.md`, `ADR-014-lifecycle-contract.md`
