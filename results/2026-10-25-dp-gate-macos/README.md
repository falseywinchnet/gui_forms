# DP gate measurement: macOS

Date: 2026-10-25. Stage: DP, damage-preserving painting
(`planning/stages/DP-damage-preserving-painting/README.md`). Status:
macOS paints the sum and leaves every pixel outside the damage unchanged,
in the raster and on the screen. The copy to the screen is not the
rectangle set: AppKit merges the two requests into their union and copies
that union, over pixels the frame left unchanged. Whether that meets
"hosts that support partial present receive the rectangle set" is open
for the owner. Windows was recorded on 2026-10-13
([2026-10-13-dp-gate-windows](../2026-10-13-dp-gate-windows/README.md)), and
X11 on 2026-10-25 ([2026-10-25-dp-gate-x11](../2026-10-25-dp-gate-x11/README.md)).

## The gate

> With two distant damaged rectangles, the painted area equals their sum (not
> their bounding box) on every host, and pixels outside them are untouched;
> hosts that support partial present receive the rectangle set; the focused
> model tests, renderer fault tests and host probes of
> `PAINT_PIPELINE_AVAILABILITY.md` "Acceptance" still pass.

## The test

`tests/macos_damage_rectangles_tests.mm` (ctest
`gui_forms_macos_damage_rectangles_tests`, a bundle fixture run through
`run_macos`) uses the Windows and X11 workload:
- The window is 400 × 300.
- It holds two distant 40 × 30 blocks, at (10, 10) and (340, 250).
- After the first frame, the test changes both blocks' colours in one turn
  and collects the damage once.
- The sum of the two blocks is 2,400 px². Their bounding box is 370 × 270,
  or 99,900 px².

It reads the window two ways, before and after the change:
- **The raster:** the host's front raster. The test occludes the model and
  reads it with `cacheDisplayInRect:`, which copies that raster without
  painting; the test requires the paint-pass counter to stay unchanged.
- **The screen:** what the display shows. The test reads the view's
  rectangle with `/usr/sbin/screencapture -x -R`. Before the change it
  checks that the capture shows the window: the background and a block
  must differ. Without Screen Recording permission, or with the window
  covered, the test fails with that message rather than passing on a
  picture of something else.

It then compares each pair pixel by pixel. A pixel counts as inside when its
centre lies in a block. The method follows PlaySuite's screen check
(`tools/screen_check.py` in falseywinchnet/games, read for this study only).
Its point is that the screen, not a frame capture, shows whether a pixel
outside the damage changed. The test's own code was written for this
repository.

Each stage of the fixture prints when it starts, and each screen capture
prints its region, so a failure names the stage it stopped in.

## The host change this record measures

Before PR #320 the macOS host painted the rectangles AppKit handed
`drawRect:`. AppKit merges the host's two `setNeedsDisplayInRect:` calls
into one rectangle on a layer-backed view. In run
[40235599394](https://github.com/falseywinchnet/gui_forms/actions/runs/40235599394)
(head `65f3b43e`) the view was layer-backed with `wantsLayer` 0, the implicit
layer backing of the current SDK, and the host painted that union:
99,900 px² for a 2,400 px² sum.

PR #320 applies to retained damage the rule 6e9a8171 set for live surfaces.
When the raster still holds the last presentation, the rectangles AppKit
hands `drawRect:` are copy requests:
- retained painting takes only the model's gathered damage;
- AppKit's request stays retained damage only inside live-surface
  destinations;
- a request with nothing to paint (activation, deminiaturize, re-exposure)
  keeps AppKit's rectangles, as before.

AppKit then copies its union from a raster that is unchanged outside the
damage. That is safe because of DP decision 2: the screen is a copy of the
raster.

## Result

CI run [40239056547](https://github.com/falseywinchnet/gui_forms/actions/runs/40239056547)
(PR #320, head `bd88d8e1`, event `pull_request`), job 121306975899
(`native (macos-15, macos-arm64, bash) / build`). The test passed in 1.84 s.

| Measure | macos-arm64 | Required |
|---|---:|---|
| Backing scale | 1 | 0.5 to 4 |
| Painted area (`painted_damage_area`) | 2,400 | the sum, 2,400 |
| Bounding area (`paint_bounding_area`) | 99,900 | more than 10 × painted |
| Frames presented | 1 | 1 |
| Rectangle AppKit handed `drawRect:` (`last_native_dirty_area`) | 99,900 | reported |
| Core Graphics clip bounds of the copy (`last_cg_clip_bounds_area`) | 99,900 | reported |
| Rectangles copied to the screen (`retained_rectangles_presented`) | 1 | reported |
| Pixels copied to the screen (`retained_pixels_presented`) | 99,900 | at least 2,400 |
| Raster pixels changed inside / outside | 2,400 / 0 | 2,400 / 0 |
| Screen pixels changed inside / outside (`screencapture`) | 2,400 / 0 | 2,400 / 0 |

The test's own lines, from `SDKLastTest.log`:

```text
dp-gate-stage|first_frame|presented_revision=0
dp-gate-stage|drain|presented_revision=6
dp-gate-stage|settle|presented_revision=7
dp-gate-capture|before|region=312,142,400,300
dp-gate-stage|changed|presented_revision=7
dp-gate-stage|shown|presented_revision=9
dp-gate-capture|after|region=312,142,400,300
dp-gate|host=macos|scale=1|sum_area=2400|painted_area=2400|bounding_area=99900|presents=1|rectangles_presented=1|pixels_presented=99900|expected_pixels=2400|native_dirty_area=99900|cg_clip_bounds_area=99900|raster_changed_inside=2400|raster_changed_outside=0|screen_changed_inside=2400|screen_changed_outside=0|wants_layer=0|layer_backed=1|layer_contents_redraw_policy=2
```

| Condition | macOS result |
|---|---|
| Painted area equals the sum, pixels outside untouched | Yes. Painted 2,400 against a 99,900 bounding box. 0 pixels changed outside the blocks, both in the raster and on the screen (`screencapture`), and every pixel inside changed. |
| Hosts offering partial present receive the rectangle set | Not as a rectangle set. The host requests the two rectangles. AppKit hands `drawRect:` their 370 × 270 union, and the host's copy is clipped to it: 1 rectangle, 99,900 pixels. The copied pixels outside the blocks are the unchanged pixels of the previous frame, so the screen does not change there. Open for the owner. |
| Model, fault and host tests still pass | Yes. ctest ran 217 of 217 passed on the macOS job, and every other job of the run passed, including the Sanitizers job (ASan + UBSan + LSan). |

## Environment

- GitHub-hosted `macos-15-arm64` runner image, version 20260828.587.
- Clang 22.1.8.
- Release build with the macOS host and Skia CPU on (`tools/ci_build.py`).
- The output comes from the run's `gui-forms-evidence-macos-arm64` artifact
  (`SDKLastTest.log`).
