# DP gate measurement: X11

Date: 2026-10-25. Stage: DP, damage-preserving painting
(`planning/stages/DP-damage-preserving-painting/README.md`). Status: X11
passes the gate. Windows was recorded on 2026-10-13
([2026-10-13-dp-gate-windows](../2026-10-13-dp-gate-windows/README.md)).
macOS has its own record.

## The gate

> With two distant damaged rectangles, the painted area equals their sum (not
> their bounding box) on every host, and pixels outside them are untouched;
> hosts that support partial present receive the rectangle set; the focused
> model tests, renderer fault tests and host probes of
> `PAINT_PIPELINE_AVAILABILITY.md` "Acceptance" still pass.

## The test

`damage_rectangles_gate` in `tests/linux_host_tests.cpp` (ctest
`gui_forms_linux_host_tests`) uses the Windows test's workload:
- The window is 400 × 300 and opened through the Linux host's own runtime.
- It holds two distant 40 × 30 blocks, at (10, 10) and (340, 250).
- After the first frame, the test changes both blocks' colours in one turn.
- The sum of the two blocks is 2,400 px². Their bounding box is 370 × 270,
  or 99,900 px².

It reads the window two ways, before and after the change:
- **The raster:** the host's CPU frame.
- **The screen:** the X server's copy of the window (`XGetImage`), which is
  what the screen shows.

It then compares each pair pixel by pixel. A pixel counts as inside when its
centre lies in a block.

## Result

CI run [40234395220](https://github.com/falseywinchnet/gui_forms/actions/runs/40234395220)
(PR #320, head `1374fa59`, event `pull_request`), both Linux jobs, under
Xvfb (`-screen 0 1280x1024x24`):

| Measure | linux-x64 (job 121292978454) | linux-arm64 (job 121292978424) | Required |
|---|---:|---:|---|
| Painted area (`painted_damage_area`) | 2,400 | 2,400 | the sum, 2,400 |
| Bounding area (`paint_bounding_area`) | 99,900 | 99,900 | more than 10 × painted |
| Frames presented | 1 | 1 | 1 |
| Rectangles copied to the window (`retained_rectangles_presented`) | 2 | 2 | 2 |
| Pixels copied to the window (`retained_pixels_presented`) | 2,400 | 2,400 | 2,400 |
| Raster pixels changed inside / outside | 2,400 / 0 | 2,400 / 0 | 2,400 / 0 |
| Screen pixels changed inside / outside | 2,400 / 0 | 2,400 / 0 | 2,400 / 0 |

The test's own line, the same on both jobs:

```text
dp-gate|host=x11|sum_area=2400|painted_area=2400|bounding_area=99900|presents=1|rectangles_presented=2|pixels_presented=2400|raster_changed_inside=2400|raster_changed_outside=0|screen_changed_inside=2400|screen_changed_outside=0
```

| Condition | X11 result |
|---|---|
| Painted area equals the sum, pixels outside untouched | Yes. Painted 2,400 against a 99,900 bounding box, and 0 pixels changed outside the blocks, in both the raster and on the X server. |
| Hosts offering partial present receive the rectangle set | Yes. X11 declares partial present (paint contract 2). `present_raster_rectangles` copied the two rectangles (2 `XPutImage` calls, 2,400 pixels), not their union. |
| Model, fault and host tests still pass | Yes. ctest ran 207 of 207 passed on each Linux job, in the CI configuration (Skia CPU / HarfBuzz / FreeType / X11). The Sanitizers job (ASan + UBSan + LSan) passed in the same run. |

## Environment

- GitHub-hosted `ubuntu-24.04` (x64) and `ubuntu-24.04-arm` (arm64) runners.
- Clang 22 from apt.llvm.org (`tools/install_linux_ci_packages.sh`).
- Release build with the Linux host and Skia CPU on (`tools/ci_build.py`).
- The output comes from the run's `gui-forms-evidence-linux-x64` and
  `gui-forms-evidence-linux-arm64` artifacts (`SDKLastTest.log`).
