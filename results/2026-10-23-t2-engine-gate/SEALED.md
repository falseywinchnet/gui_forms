# T2 engine gate: sealed 2026-10-23

T2 (the engine core) is sealed. Every evidence run below shows all four
hosts identical. The seal reopens if any later gate run finds a host that
differs.

- **Evidence runs** (`GUI.Forms source and compiler cache`, job
  `engine-gate`; each run's `t2-engine-gate-record` artifact holds the
  filled record). Each run printed the same gate line:
  `engine gate: 4 hosts, 144 images, 308 sample images, 77 features, 0 differences, 0 busy idle counters, missing []`
  - PR run https://github.com/falseywinchnet/gui_forms/actions/runs/40027309396,
    on #295.
  - Main run https://github.com/falseywinchnet/gui_forms/actions/runs/40033304315,
    on `8e9be54a`, after #295 and #296.
  - Final main run https://github.com/falseywinchnet/gui_forms/actions/runs/40038536082,
    on `79747bc7`, after #300. #300 adds the effort budget, with BakeEffort
    at 20M units; the manifest is byte-identical to the one before it.

  The gate README explains how the evidence is made:
  `../2026-10-22-t2-engine-gate/README.md`.
- **Hosts:** 4 (windows-x64, macos-arm64, linux-x64, linux-arm64).
- **Primitives:** 12, from `builtin_primitive_registry()`. They give 144
  images: four scales, each at rest, hot and pressed.
- **Feature samples:** 46, from `engine_samples::builtin_feature_samples()`.
  They give 308 sample images: four scales, each at rest, at the pointer,
  in motion frames at 32, 80, 144 and 224 ms, and once settled.
- **Census:** 77 features of the engine's vocabulary. At least one sample
  draws each of them.
- **Idle counters:** once settled, every window runs no frame for two
  seconds (part 2). Cost fallback and reduced motion (parts 3 and 4) are
  covered by the primitives' own tests, which the gate README lists.
- **Owner sign-off on the specimen sheet** (#299's flag): the owner
  accepted the finish constants and starting tokens as starting values.
- **The effort amendment:** the owner's rule is "cure effort, not time".
  Work bounds, including bake bounds and budgets, are counted in integer
  effort units (#300). Perceptual durations, such as motion and settle
  times, are still measured in time.
- **Costs:** the quiet-machine measurement (#293,
  `planning/stages/T2-engine-core/README.md`, "Bake costs on a quiet
  machine") is now used only to choose the relative effort weights.

Sealed 2026-10-23. Reopened if any host differs.
