# T2 engine gate: sealed pending CI (2026-10-23)

T2 (the engine core) is declared sealed by the owner, pending CI. It is
sealed once the gate run below shows every host identical, and reopened if
any host differs.

- **Gate run:** https://github.com/falseywinchnet/gui_forms/actions/runs/40027241879
  (`GUI.Forms source and compiler cache`, job `engine-gate`), on #295's
  head `1c088165`. Its `t2-engine-gate-record` artifact is the filled
  record. How the evidence is made: `../2026-10-22-t2-engine-gate/README.md`.
- **Hosts:** 4 (windows-x64, macos-arm64, linux-x64, linux-arm64).
- **Primitives:** 12, from `builtin_primitive_registry()`; 144 images (four
  scales, at rest, hot and pressed).
- **Feature samples:** 46, from `engine_samples::builtin_feature_samples()`;
  308 sample images (four scales; at rest, at the pointer, and motion frames
  at 32, 80, 144 and 224 ms and once settled).
- **Census:** 77 features of the engine's vocabulary, each drawn by at least
  one sample.
- **Idle counters:** every window, once settled, runs no frame for two
  seconds (part 2). Cost fallback and reduced motion (parts 3 and 4) are
  the primitives' own tests, listed in the gate README.
- **Bake costs:** measured on a quiet machine, #293
  (`planning/stages/T2-engine-core/README.md`, "Bake costs on a quiet
  machine").

Sealed pending CI; reopened if any host differs.
