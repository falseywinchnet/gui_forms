# Stage R: SDRSharp reflection, current technique

Release: none. This stage stays off the public site and the public roadmap.
Depends on: T4 (the styles must be expressible on the engine), F (fonts).
Unblocks S. Status 2026-10-12: harness exists, last run 2026-08-10, stale.
Authority: `planning/FORWARD_PROGRAMME.md` §5 R.

Read [the shared brief](../README.md) first.

## Why

The owner, 2026-10-12: "the SDRSharp reflection tool should be revisited,
updated to current technique, and run again. Being able to accurately
emulate the stylistic capabilities it exercised in WinForms, in our engine,
is necessarily optimal. This hinges on doing way 1 correct." SDRSharp
"exercises a bit more than" the theming capabilities discussed.

The Windows Forms compatibility facade and this harness were made for
SDRSharp, for a friend of the owner. Keep both off the documentation site
(owner decision, 2026-10-11). The public site may mention C# bindings in
general terms (stage S), not this harness.

## What exists today

- `sdrsharp_reflection_system/`: the harness (`README.md`, `run_reflected.sh`,
  `build_on_m4.sh`, `run_on_m4.command`, the managed runner under `runner/`,
  and two screenshots of the 2026-08-10 run showing the toolbar, dock panels,
  spectrum and sliders, with the waterfall black). It verifies and extracts
  the user-supplied .NET single-file bundle, substitutes generated
  `System.Windows.Forms`, `System.Windows.Forms.Primitives` and
  `System.Drawing.Common` assemblies that call GUI.Forms through the C ABI,
  and invokes the managed entry point under Wine. It reads no application
  methods or fields.
- Generated facades: `generated/facade-v1/`. Tools: `tools/facade_generator/`,
  `tools/compat_capture/` (a deterministic, non-executing capture of the
  Forms-shaped types and members a program calls), `tools/compat_loader_lab/`,
  `tools/facade_smoke/`, `tools/facade_verifier/`,
  `tools/facade_behavior_smoke/`, `tools/drawing_facade_smoke/`,
  `tools/winforms_catalogue/`, `tools/run_facade_surface_smoke.sh`,
  `tools/run_windows_wine_smoke.sh`.
- Decision records: ADR-001 (port handle-dependent controls to portable
  behaviour; no general Win32 emulation) and ADR-002 (the captured facade is
  the programme; an unchanged-binary load under Wine is a late best-effort
  experiment, not a gate). This stage is that experiment, rerun, with a
  differential added.

## What is stale

- The C ABI has moved on (experimental version 0.26 in
  `include/gui_forms/c_api.h`); the facade was generated against an older
  one.
- Text moved to the united HarfBuzz/FreeType engine everywhere; SDRSharp's
  `System.Drawing` text still takes the GUI.Drawing host-font path (see F).
- Path defaults and examples still point into the old combined repository
  (`GUI_FORMS_ROOT=/absolute/path/to/file_manager/gui_forms`).
- No CI job builds or runs any of it; the earlier smoke script never gated
  the 2026-08-10 run.
- The launcher routes the spectrum and waterfall through a direct HWND
  surface lease (`GUI_FORMS_DIRECT_HWND_TYPES`). Check that route against
  ADR-001's bounded compatibility handle lease before relying on it.

## What SDRSharp exercises (acceptance fixtures for T4)

Owner-drawn combo boxes and lists; `ToolStripProfessionalRenderer`
gradients; GDI+ linear, path-gradient and hatch brushes, `GraphicsPath` and
regions; docking panels and splitters; custom range and frequency controls;
Segoe UI, Tahoma and Microsoft Sans Serif text.

## What to deliver

1. The facade regenerated against the current C ABI, with fonts routed
   through F's substitution table.
2. Paths and scripts fixed for this repository, with the environment
   variables in the harness README as the only configuration.
3. A repeatable Wine run on the M4 bench (ask the owner before using it).
4. The missing reference half: capture the real SDRSharp on real Windows
   (Shadow Windows), and compare control by control with the reflected run.

## How

- Never commit SDRSharp binaries, extracted bundle entries, configuration,
  layouts, logs, decompiled output or private member inventories. The
  runtime directory stays outside Git, and the `SDRSharp.exe` hash is pinned
  per run (harness README).
- Audio under Wine: WASAPI worked and MME did not in the reference run. Pin
  devices in the private profile.
- The C# side is generated code. The native side follows the house style in
  full; a ported custom control is a GUI.Forms control tested with the same
  normative traces as a stock control (ADR-001).
- Measure, don't eyeball: per-control captures at a fixed window size and
  scale on both sides, compared with a recorded tolerance.

## Gate

A per-control visual differential between the reflected run and the real
Windows run, within a recorded tolerance, for the listed controls. Record
under `results/` (screenshots of SDRSharp itself may be kept only as the
existing harness does: visual records with no extracted application data).

## Owner decisions, 2026-10-13

1. **The direct HWND route** for the spectrum and waterfall is deferred:
   "when the time comes, this will require fine-tuned work. May be possible
   with our accelerated canvas." Plan the port of those two controls as its
   own careful step; do not decide it inside the rerun.

## Read first

- `sdrsharp_reflection_system/README.md`
- `decisions/ADR-001-portable-compatibility-controls.md`,
  `decisions/ADR-002-captured-facade-before-unchanged-binary.md`
- `tools/compat_capture/README.md`, `tools/facade_generator/README.md`
- `planning/WINFORMS_API_CATALOGUE.md`, `planning/WINFORMS_BEHAVIOR_GAP_CATALOGUE.md`
- [F](../F-font-substitution/README.md) and [T4](../T4-default-theme/README.md) briefs
