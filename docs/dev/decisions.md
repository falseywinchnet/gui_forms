# Architecture decisions

Significant decisions are recorded as numbered architecture decision records.
Each record states the context, the decision, its consequences and the
options that lost. The pages below are the records; they are edited in
`docs/dev/adr/`.

| ADR | Title | Status | Date |
|---|---|---|---|
| [ADR-001](adr/ADR-001) | Port handle-dependent compatibility controls to portable behaviour; no general Win32 emulation | accepted (recorded retroactively) | 2026-10-11 |
| [ADR-002](adr/ADR-002) | Implement the captured interface-level facade first; an unchanged-binary load is a late experiment, not a gate | accepted (recorded retroactively) | 2026-10-11 |
| [ADR-003](adr/ADR-003) | Use one deterministic event order and preserve DML assignment order | accepted | 2026-08-04 |
| [ADR-004](adr/ADR-004) | One text rasterization engine with identical output on every platform | accepted; implemented | 2026-10-11 |
| [ADR-006](adr/ADR-006) | GUI.Forms and Orchestrator are independent predecessors; consumption is negotiated through a published manifest | accepted (recorded retroactively) | 2026-10-11 |
| [ADR-013](adr/ADR-013) | Persistent side-panel composition is reserved to File Manager; demonstrations may compose panels | retired | 2026-10-13 |
| [ADR-014](adr/ADR-014) | One portable lifecycle contract with separate retained, session, native and lease identities | accepted (recorded retroactively) | 2026-10-11 |

Numbers 005 and 007 through 012 are not assigned in this repository; the
numbering follows the wider programme's sequence, and records that belong to
other projects are kept there.

```{toctree}
:hidden:

adr/ADR-001
adr/ADR-002
adr/ADR-003
adr/ADR-004
adr/ADR-006
adr/ADR-013
adr/ADR-014
```
