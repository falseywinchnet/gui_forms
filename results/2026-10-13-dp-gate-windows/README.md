# DP gate measurement: Windows

Date: 2026-10-13. Stage: DP, damage-preserving painting
(`planning/stages/DP-damage-preserving-painting/README.md`). Status: Windows
measured; macOS (M4) and X11 (Lima guest on the M4) to be measured with the
same scripts. The gate is recorded per host; DP closes when every host is.

## The gate

> With two distant damaged rectangles, the painted area equals their sum (not
> their bounding box) on every host, and pixels outside them are untouched;
> hosts that support partial present receive the rectangle set; the focused
> model tests, renderer fault tests and host probes of
> `PAINT_PIPELINE_AVAILABILITY.md` "Acceptance" still pass.

| Condition | Windows result |
|---|---|
| Painted area equals the sum, pixels outside untouched | `gui_forms_windows_damage_rectangles_tests` (DIB raster, #59): two 40 × 30 changes in a 400 × 300 window paint 2,400 px² against a 99,900 px² bounding box; a whole-raster diff finds 2,400 changed pixels inside and 0 outside. |
| Hosts offering partial present receive the rectangle set | Both profiles offer it and declare it in the availability record. Default: each rectangle copied separately (#59), live frames through the frame sequence (#72). Transactional: `DibFrameStore::commit` copies each rectangle and swaps the candidate only when every copy succeeds; `test_rectangle_set_commit` proves only the set reaches the target and a failed copy restores the old front. |
| Model, fault and host tests still pass | ctest 122 of 122 (default profile) and 126 of 126 (transactional profile, which adds the DIB store and lifecycle fixtures), on `claude/dp-availability-record`. |

## Presentation availability record

`Window::presentation_availability()` on the Windows host, both profiles:
`contract_version` 1, `partial_present` true, `zero_copy_import`,
`presentation_fence` and `native_surface_lease` false, with the live lease.
The record is in the host diagnostics JSON under `presentation`. macOS
declares `partial_present` true (AppKit clips each copy to the rectangles it
draws); X11 declares it false (it copies the bounding box).

## The frame sequence on screen

PlaySuite's Stillwater with the capsule open (owner, Windows 11 over Remote
Desktop): before #72 the area under the capsule and the ? button flickered a
frame behind the water; after it, no flicker. A remaining catch-up lag came
from PlaySuite's `render::refresh_overlays`, which on Windows still
invalidates every overlay after each publish; with it disabled (as on macOS
since #69) the owner saw no lag. A trace showed those invalidations painting
10 to 20 ms ahead of the frame beneath them.

## Copy cost (owner decision 4)

PlaySuite's `tools/measure_frames.py` (falseywinchnet/games 8f0d489), two runs
of each scene, 1100 × 720 at scale 1, counters per steady second. Before:
GUI.Forms main cdee77f (counters, no sequence) with PlaySuite unmodified.
After: the frame sequence (#72) with PlaySuite's Windows `refresh_overlays`
disabled locally.

| Scene | Live Mpx/s copied | Retained presents/s | Retained rectangles/s | Overlay repaints deferred/s |
|---|---|---|---|---|
| Stillwater | 34.1 → 34.2 | 24.1 → 12.5 | 48.3 → 12.5 | 0 |
| Mowing | 70.6 → 79.2 | 25.2 → 0.1 | 50.5 → 0.1 | 0 |
| Maze 95 (walking) | 21.8 → 31.7 | 17.8 → 7.0 | 35.5 → 6.8 | 0 |
| Gems (resting) | 86.7 → 86.0 | ~0 | ~0 | 0 |
| Untangle (resting) | 92.2 → 91.9 | 2.9 → 2.1 | 5.8 → 2.1 | 0 |

- The live copy volume is unchanged where a scene repeats (Stillwater, Gems,
  Untangle within 1 %); Mowing and Maze walk differently each run. The
  Windows host already drew live frames into its raster before copying them,
  so the sequence adds no copy on Windows.
- Retained work falls by half or more: overlays are painted once per frame,
  with the frame, instead of twice.
- Durations are not recorded as evidence: the session ran over Remote
  Desktop, where every BitBlt crosses the remote channel, and the subtracted
  present durations were noisy (some negative). `measure_frames.py` has no
  processor time on Windows yet. CPU cost needs a local console run, or the
  M4 and Lima runs.

## Workload and environment

- Machine: the owner's Windows 11 desktop ("Shadow Windows"), x64, reached
  over Remote Desktop.
- Toolchain: MSYS2 CLANG64, clang 22.1.8, restored at `D:\a\_temp\msys64`.
- GUI.Forms: Windows CI configuration (Ninja, Release, HarfBuzz on, Skia off,
  Gallery on), both `GUI_FORMS_WINDOWS_TRANSACTIONAL_DIB` profiles.
- PlaySuite: falseywinchnet/games 8f0d489 built against the local GUI.Forms
  checkout, runtime assets from games run 38415149273.

## Commands

```text
cmake --build .build/native-windows-x64/gui-forms --parallel 8
ctest --test-dir .build/native-windows-x64/gui-forms --timeout 180
python tools/measure_frames.py --app .build/native/app/games.exe --repeat 2 --out after.json
```

## Still open in DP

- macOS and X11 runs of the same tests and scenes.
- X11 presents the bounding box. Decision 3 makes partial present mandatory,
  and `PAINT_PIPELINE_AVAILABILITY.md` is amended, once X11 presents
  rectangle sets too.
- The frame sequence on macOS and X11 (decision 2); on Windows it covers live
  frames, and retained-only changes still present through `WM_PAINT`.
