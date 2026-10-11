# Contributing

GUI.Forms accepts contributions through pull requests on GitHub. Before you
open one, read this page and the project's `AGENTS.md`, which states the
boundaries the build enforces. A change that honours the boundaries, carries
a test and reports what it measured is easy to review; a change that argues
for a new dependency or a new platform assumption is not.

## Boundaries that are not negotiable

- C++20. Objective-C++ only under `src/host/macos/`. Win32, COM, WIC, MSAA
  and UIA only under `src/host/windows/`. X11 only under `src/host/linux/`.
- No .NET, Java, browser engine or Godot dependency in the native runtime.
- No GPU: nothing compiles, links, probes or exposes Ganesh, Graphite, GL,
  Vulkan, Metal, Direct3D, Dawn or WebGPU.
- Skia only through the private CPU adapter in `src/render/skia/`; no Skia
  type in a public header, control API, DML, C ABI or renderer-neutral test.
- PNG is the only image decoder. HarfBuzz, FreeType, SheenBidi and libunibreak
  are the only text dependencies, pinned by revision.
- The core is retained. No immediate-mode control API, no perpetual redraw.
- The portable core and host protocol encode no AppKit, Win32, Wayland or X11
  assumption. Honest platform differences are capability-reported adapter
  behaviour.
- Fonts are the bundled set. Do not consult host fonts.

Boundary tests run under `ctest`, so a violation fails before review.

## Code style

The project follows `planning/PROGRAMMING_HOUSE_STYLE.md`, which can be
summarised as: spell the type, name the behaviour, write in execution order.
In practice:

- Explicit types at declarations; no `auto`. Distinguish types whose
  confusion causes bugs (byte offsets vs. character positions, identities vs.
  indices) with wrapper types.
- Named functions, methods and functors instead of lambdas and anonymous
  callbacks. Retained state has named fields.
- One recognisable operation per expression; calculate, then return.
- Initialize every variable. Group related data in records; use enums for
  alternatives.
- Validate shape, extent and capacity before a kernel, not per element. Keep
  allocation, logging and callbacks out of per-element loops.
- Explicit ownership and explicit session shutdown; declare context before
  the worker that borrows it.

`tools/check_house_style.py` reports spelling violations for the paths it is
given, and CI runs it over the files the project has brought into scope. It
is a review aid, not a certification: ownership, lifetimes and initialization
still need a human reader.

## Tests and evidence

Every behavioural change ships with a test. Core and control behaviour is
tested renderer-free against the headless host; painting against the
identity and raster tests; host behaviour in the platform job for that host.
A visual claim ("looks right") is not evidence; a counter, a trace or a hash
is.

Performance claims follow the same rule. Nothing is called fast or
lightweight without the workload and the measurement being recorded, and
negative results are kept. Measurements live under `results/` and
`experiments/`; see [Evidence](evidence).

## Documentation

User documentation is this Sphinx site under `docs/`, built with
`sphinx-build -W`; see `docs/README.md` for how to build it locally. Write in
the present tense for a user of the library. Control pages are generated
from `manifests/controls.json` and the headers; add a control there, not in
the generated files. Architecture decisions are ADRs in `decisions/`,
rendered under [Architecture decisions](decisions).

## Workflow

1. Fork and branch from `main`.
2. Build and run `ctest` on at least one platform; CI runs macOS arm64, Linux
   x64, Linux arm64 and Windows x64.
3. Keep unrelated working-tree changes out of the commit. Preserve user files.
4. Describe in the pull request what changed, which tests cover it, and the
   exact commands and environment you used for any measurement.
5. Expect review to ask for the boundary and the test first and the feature
   second.

## Licence

GUI.Forms is MIT licensed. Bundled fonts carry their own licences in
`assets/fonts/`; third-party sources keep theirs under `third_party/` and
are installed under `share/licenses/`. A contribution is accepted under the
project licence.
