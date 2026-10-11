# ADR-006: GUI.Forms and Orchestrator are independent predecessors; consumption is negotiated through a published manifest

Status: accepted (recorded retroactively)

Date: 2026-10-11 (decision in force since the first consumption negotiation
in August 2026; first written down in this repository on this date)

## Context

File Manager, the first application built on GUI.Forms, also depends on
Orchestrator, a headless integration component with no GUI of its own.
Building the application frontend before either dependency was ready would
have coupled three unfinished projects through private headers and ad hoc
assumptions.

This decision was cited as ADR-006 by the provider-owned negotiation note
(`docs/notes/ORCHESTRATOR_INTERFACE_NEGOTIATION.md`, formerly
`docs/ORCHESTRATOR_INTERFACE_NEGOTIATION.md`) before this file existed. The
negotiation note records the rounds; this record states the rule they follow.

## Decision

- GUI.Forms and Orchestrator are **two independent technical predecessors**
  of the first File Manager frontend. Orchestrator advances headlessly to its
  own 1.0; GUI.Forms advances to its first named consumption snapshot. The
  frontend starts only after both are ready.
- Orchestrator has no GUI and never calls controls, receives GUI handles,
  callbacks, native windows or renderer objects. What it may hold is a set of
  **capability facts** about GUI.Forms that it can report.
- GUI.Forms publishes a **machine-readable consumption manifest** for each
  named snapshot: ABI family and version, target triple, exported symbol or
  table identity, thread and lifecycle rules, host protocol version,
  implemented and explicitly unsupported capabilities, fixture and trace
  digests, build and source manifest, and compatibility horizon. The manifest
  distinguishes construction-level presence from behavioural support.
- Cross-project needs are **negotiated in writing**: Orchestrator proposes,
  GUI.Forms replies in its provider-owned note with evidence, and
  Orchestrator reconciles the canonical contract. GUI.Forms does not freeze an
  ABI merely because the current frontend experiment calls it.

## Consequences

- The stable consumer seam is the installed CMake package (`GUIForms`), with
  `GUIForms::Application` for native programs and `GUIForms::Controls` for
  independently testable model consumers. Private headers are never a seam.
- Capability discovery is mandatory on the consumer side; absence is reported
  as `unsupported`, never simulated.
- Each snapshot manifest lives under `manifests/` with digests of the built
  artefacts and the exact commands used, so a consumer can pin and verify it.
- The negotiation note remains an internal working record and is excluded from
  the user documentation; this ADR is the durable statement of the rule.

## Rejected options

- **Frontend first, dependencies by convenience.** Rejected: it would have
  fixed private GUI.Forms shapes as the contract through use.
- **Orchestrator as a GUI broker.** Rejected: it has no GUI thread, and
  passing handles across the process boundary would violate the lifecycle and
  thread rules the retained engine depends on.
