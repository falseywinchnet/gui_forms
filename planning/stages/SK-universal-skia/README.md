# Stage SK: universal Skia

Release: 0.3 Themeable (a renderer change under every later stage; it
lands before T4's theme images are recorded). Depends on: nothing; it can
start now. Status: **brief written 2026-10-26**; SK1 next.

## Why

Owner decision, 2026-10-26, in the owner's words: "skia is mandatory on all
platforms, and we should go universal. DibPainter and CoreGraphicsRaster go
away. one pixel format everywhere, RGBA. no coregraphics, no dibpainter."

GUI.Forms has three CPU painters today: `SkiaRaster` (macOS and X11), the
first-party `DibPainter` inside the Windows host, and `CoreGraphicsRaster`
(macOS, beside Skia). Each paints the same `Painter` calls a little
differently, so every visual rule is implemented and tested three times.
Stage L-Q8's edge snapping (#340) is the latest example. It took a separate
fix in each painter for ties, single-precision transforms, clip rounding and
dithering. It also needed a per-painter tolerance in its test, because Quartz
dithers gradients and the other two do not.

The owner's reference rule ("our engine is the reference") means a single
painter whose output is identical on every host. Two pixel orders (BGRA on
Windows and MinGW Skia, RGBA elsewhere) double every compositing kernel,
every image path and every golden. Universal Skia means one painter and one
pixel format, RGBA8 premultiplied sRGB. The byte order differs only at a
host's final copy to the screen, in one measured kernel.

## Read first

- `AGENTS.md`: the hard boundaries (CPU only, no Skia type in public headers,
  PNG the only codec, AppKit and Win32 types only in their hosts).
- `planning/PROGRAMMING_HOUSE_STYLE.md`.
- `D:/gui_forms_workers/stx_skia/REQUIREMENTS.md`: what GUI.Forms uses from
  Skia. Its "Required" list is the whole admitted surface for this stage.
- `third_party/SKIA_BUILD_EVIDENCE.md` and `third_party/README.md`: the pinned
  fetch and build.
- `docs/user/painting-and-damage.md` and the DP brief
  (`planning/stages/DP-damage-preserving-painting/README.md`): the paint
  contract, receipts, leases and live surfaces that every phase keeps.

## What exists today (main, 2026-10-26)

**Build.**
- `GUI_FORMS_ENABLE_SKIA` is an option, `ON` by default (`CMakeLists.txt`). The
  macOS host refuses to configure without it.
- Windows builds turn it off everywhere: `tools/ci_build.py` (the Windows branch),
  `.github/workflows/release.yml` (`renderer=-DGUI_FORMS_ENABLE_SKIA=OFF` on
  Windows), `.github/workflows/state-machines.yml`,
  `tools/prove_compiler_cache.py`, and the local worker trees' `build.sh`.
- Skia is fetched at a pinned commit (`third_party/fetch_skia_cpu.sh`,
  `skia_revision=2a9b593b...`). It is built by `build_skia_cpu.sh` (macOS, arm64
  args in `skia_cpu_args.gn`) and `build_skia_cpu_linux.sh`. CI caches the output
  per platform (`native-platform.yml`, the `skia` paths).
- A MinGW profile exists (`build_skia_cpu_windows_mingw.sh`,
  `skia_cpu_windows_mingw_args.gn`, `skia_windows_mingw.patch`). It is a
  cross-compile from a macOS Homebrew toolchain (its compiler and include paths
  are `/opt/homebrew/...`), and its archive is BGRA. No CI job builds it.

**Painters.**
- `SkiaRaster` (`src/render/skia/raster/`): the macOS and X11 hosts. It already
  has paths for both orders (`kRGBA_8888` and `kBGRA_8888` in `skia_raster.cpp`
  and `skia_executor.cpp`).
- `DibPainter` (`src/host/windows/application/windows_host.cpp`): first-party,
  BGRA, inside the Windows host. It owns the transactional DIB frame store
  (`GUI_FORMS_WINDOWS_TRANSACTIONAL_DIB`).
- `CoreGraphicsRaster` (`src/render/coregraphics/`, library
  `gui_forms_coregraphics`): not used by the macOS host for drawing. It
  appears in its own tests (`coregraphics_raster_edge_snapping_tests`,
  `layered_material_raster_tests`, `coregraphics_raster_smoke.cpp`) and in a
  CMake check that compares adapters (`COREGRAPHICS_ADAPTER`).

**Pixel order.** BGRA appears in public and private types:
- `gui_drawing::PixelFormat` (`bgra32_premultiplied`, `rgba32_premultiplied`);
- `LiveSurfacePixelFormat::bgra32_premultiplied_srgb`;
- `FramebufferChannelOrder`;
- the C ABI (`GF_PIXEL_FORMAT_BGRA32_PREMULTIPLIED`, `GD_PIXEL_BGRA32_PREMULTIPLIED`,
  `gf_windows_paint_endpoint_submit_bgra_v1`);
- image resources, text compositing (`TextPixelOrder`), live surfaces, the
  T2 engine's feature samples and material images, CNG pixels and the Windows
  notify icons.

`grep -rni bgra include src` lists them. SK2 starts from that inventory.

**Screen copies.**
- X11 already converts at present time, per presented rectangle, into the
  visual's layout (`src/host/linux/application/linux_host.cpp`,
  `convert_native_pixels32_bytes` and its two fallbacks), and times the copy.
- macOS copies RGBA through CoreGraphics (`CGContextDrawImage`). That stays:
  it is the host's screen copy, not a painter.
- Windows paints BGRA directly into its DIB.

## The phases

One pull request per phase, each from main, each with its gate recorded in
its description. Stacks stay at most two deep.

### SK1: the build

- `GUI_FORMS_ENABLE_SKIA` stops being optional. The `OFF` path is removed,
  and configuring without the pinned Skia fails with a message that names
  `third_party/fetch_skia_cpu.sh`.
- The Windows CI job fetches, builds and caches the CPU Skia like the other
  platforms. It uses the MinGW/CLANG64 profile, made a native MSYS2 CLANG64
  build on the runner: no Homebrew paths, the repository's LLVM 22
  toolchain, and the same GPU, codec and font switches as `skia_cpu_args.gn`.
- The profile's colour type becomes RGBA.
- The Windows host links Skia but keeps `DibPainter` until SK3.
- Every Windows `OFF` above becomes `ON`: `ci_build.py`, `release.yml`,
  `state-machines.yml`, `prove_compiler_cache.py`, and the local build
  scripts. Local trees depend on SK-Q2.
- `docs/user/install.md` lists Skia as required on every platform, with the
  fetch and build steps for Windows.
- **Measurement:** the Windows CI job's own time (job start to completion,
  queueing excluded), cold and steady-state, before SK1 and after it.
- **Gate:** steady-state reuse, with the Skia archive restored from the cache,
  keeps main's wall time near today's (about 8.3 minutes). The cold build's
  cost is recorded, not gated.
- The link-map audit (`tools/audit_link_map.py --forbid-foreign`) passes on
  Windows with the Skia archive: no GPU, codec or font symbols.

### SK2: one pixel format

- Every surface and resource is RGBA8 premultiplied sRGB, on every
  platform, MinGW included: rasters, part rasters, live surfaces, image
  resources, text compositing, engine samples and materials, CNG.
- Where a host's screen copy needs another order (GDI `BitBlt` and
  `SetDIBitsToDevice`, typical X11 visuals), the host converts at present
  time. The conversion is one named kernel per host, applied per presented
  damage rectangle, and validated over the whole rectangle set before any
  pixel is written.
- X11's existing conversion is that kernel; the Windows host gets its own in
  SK3.
- Every other byte-order branch is removed: the `kBGRA_8888` paths in
  `src/render/skia/`, the `bgra` text composite order, and the BGRA cases in
  image, live-surface and engine code.
- The public BGRA spellings depend on SK-Q1.
- **Measurement:** the conversion's time and pixels per presented frame,
  queryable as counters beside the existing copy timer. Record them for the
  Gallery idle and the DP gate's workloads on Windows and X11.
- **Gate:**
  - `grep -rni bgra src include` finds only the hosts' conversion kernels and
    whatever SK-Q1 keeps at the public boundary;
  - every touched test passes plain and under the sanitizer;
  - the reference images don't move except where a test read BGRA bytes
    directly. Those moves are listed one by one, with their reason.

### SK3: the Windows host draws with Skia

- The Windows host paints with `SkiaRaster` into its own RGBA raster and
  presents through SK2's conversion into its DIB.
- `DibPainter` is deleted with its private helpers. The transactional DIB
  frame store keeps its all-or-nothing commit of the rectangle set over the
  new raster: a frame that fails converts nothing.
- Every DP, receipt, lease and live-surface guarantee and its test is kept:
  the `windows_` DP tests, the live frame sequence, the copy timer, the
  occlusion and reveal rules.
- **Gate:**
  - all `windows_` tests pass plain and under the sanitizer;
  - the DP gate record is re-run on Windows and matches
    `results/2026-10-13-dp-gate-windows`'s rules;
  - the edge-snapping scene passes on Windows through Skia.
  - The Windows canvas reference images move here. They are not re-recorded
    piecemeal: SK5 does that once.

### SK4: CoreGraphicsRaster is deleted

- `src/render/coregraphics/`, `gui_forms_coregraphics`, its tests and the
  adapter comparison check go. The macOS host keeps only Skia (its screen
  copy through CoreGraphics stays).
- **Gate:**
  - `grep -rn CoreGraphicsRaster` finds nothing;
  - the macOS host, accessibility and DP tests pass.

### SK5: reference images, once

- Every reference image is re-recorded once, against the single painter,
  with evidence per test: which test, what moved, and why.
- The per-painter tolerances go: the edge-snapping scene's `dither`
  argument (Quartz's allowance from #340) is removed, and every
  edge-snapping check is exact.
- The DIB-specific stroke geometry (inside the rectangle, versus Skia's
  centred stroke) is no longer a branch in that test.
- **Gate:**
  - CI green on all four hosts with one set of goldens;
  - the same image digest on every host for each painter test that runs on more
    than one.

## Swap-ready

The owner is building `stx_skia`, a pruned Skia, in their own thread.
- Skia's source stays behind the existing pinned fetch and build scripts, so
  replacing it with `stx_skia` later is a pin and build-script change only.
- This stage uses nothing outside the "Required" list in
  `D:/gui_forms_workers/stx_skia/REQUIREMENTS.md`.
- It does not touch image filters, Skia's fonts or text, or extra codecs.
- After SK2, the list's "RGBA8 and BGRA8" surfaces become RGBA8 and A8 only.
  Tell the coordinator, so the requirements file can say so.

## Measurements to record

| Phase | What | Where |
|---|---|---|
| SK1 | Windows CI job time, cold and steady-state, before and after | the SK1 PR, from the workflow's job timings |
| SK1 | Windows Skia archive size and link-map audit | the SK1 PR |
| SK2 | Present-time conversion: nanoseconds and pixels per frame, Gallery idle and DP gate workloads, Windows and X11 | counters beside the copy timer; the SK2 PR |
| SK3 | DP gate on Windows, re-run | `results/` record named in the SK3 PR |
| SK5 | Per-test reference image changes, and cross-host digest equality | the SK5 PR |

Every number names its workload and machine. Nothing is called fast without
one.

## Open decisions (for the owner, through the coordinator)

**SK-Q1. The public BGRA spellings.** BGRA is part of the public C++ types
and the C ABI (experimental 0.26, not frozen).
1. Remove BGRA from every public type and the C ABI. Callers convert before
   they hand pixels in.
2. Keep BGRA only as an input interchange format at the boundary. A caller's
   BGRA pixels are converted to RGBA once, in one named kernel, as they
   enter, and are never stored or rendered as BGRA.
3. As 2, but only for the Windows compatibility paint endpoint
   (`gf_windows_paint_endpoint_submit_bgra_v1`), where GDI's own BGRA is the
   foreign interface; everything else is RGBA only.

Recommendation: 3. It is the owner's "one pixel format everywhere" inside
GUI.Forms, and keeps the one boundary where the operating system's format
is the contract.

**SK-Q2. Skia in local Windows worker trees.** After SK1, a tree cannot
configure without Skia, so the D: worker trees need it.
1. Approve the pinned fetch (`fetch_skia_cpu.sh`) and a local CLANG64 build
   on this machine.
2. Approve downloading the Windows Skia archive our own CI built and cached,
   checked against its digest, with no local Skia compile.
3. Both: 2 for everyday trees, 1 for the tree that changes the Skia build.

Recommendation: 3. Until one is approved, SK1 can be built and verified only
in CI, and local verification of every later phase stops.

## Lessons already paid for

- **#340, L-Q8 snapping:** three painters needed three fixes.
  - **Ties:** Skia's single-precision translation put a 42.5 edge below the tie,
    and Quartz rounded ties in its bottom-up device space.
  - **Clips:** Quartz's aliased clip took the next row after a round trip.
  - **Dithering:** Quartz dithers gradients.

  One painter removes all three classes. Until SK5, keep #340's tests: they
  are the cross-painter check.
- **The macOS screen copy is AppKit's union over unchanged pixels** (DP
  decision 6). It does not change with this stage: the raster changes, not the
  copy.
- **Skia's matrix is single precision.** `SkiaRaster` keeps the caller's
  translation in double for snapping (#340, `SnapTransform`). Keep that.

## Status log

- 2026-10-26: brief written (this file); the owner's decision recorded
  above. SK1 next.
