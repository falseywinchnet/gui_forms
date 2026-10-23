# T2 engine gate record

Filled by `python tools/engine_gate.py compare` in the `engine-gate` job of
`native.yml`. See `README.md` in this directory for how the evidence is made
and for the tests behind parts 3 and 4 of the gate.

- **Commit:** `5a87ab47fd08a534111431f24c4f0c5a2bc09eca`
- **CI run:** https://github.com/falseywinchnet/gui_forms/actions/runs/39910508309
- **Verdict:** identical on every host that ran

## Part 1: reference images per host

Primitives (from `builtin_primitive_registry()`): `bulge`, `dithered_bulge`, `etched_divider`, `glass`, `gold_plate`, `grip`, `inset_well`, `material`, `overlay_shadow`, `pill_plate`, `seam_line`, `sheen`

144 images: each primitive at scales 1, 1.25, 1.5 and 2, at rest,
hot and pressed.

| Host | Registered primitives | Images | Matches every other host |
|---|---:|---:|---|
| linux-arm64 | 12 | 144 | yes |
| linux-x64 | 12 | 144 | yes |
| macos-arm64 | 12 | 144 | yes |
| windows-x64 | 12 | 144 | yes |

### Differences

None: every image has the same hash on every host.

## Parts 2 and 4: the window counters

The idle counters cover the two seconds after each window settled; every one
must be 0. The reduced-motion counters cover the ribbon hovered under
reduced motion: no motion clock tick, and the rise jumped to its end.

| Counter | linux-arm64 | linux-x64 | macos-arm64 | windows-x64 |
|---|---:|---:|---:|---:|
| `primitive.bulge.idle_frames_run` | 0 | 0 | 0 | 0 |
| `primitive.bulge.idle_frame_requests` | 0 | 0 | 0 | 0 |
| `primitive.bulge.idle_paint_passes` | 0 | 0 | 0 | 0 |
| `primitive.bulge.idle_motion_clock_ticks` | 0 | 0 | 0 | 0 |
| `primitive.bulge.idle_ambient_frames` | 0 | 0 | 0 | 0 |
| `primitive.dithered_bulge.idle_frames_run` | 0 | 0 | 0 | 0 |
| `primitive.dithered_bulge.idle_frame_requests` | 0 | 0 | 0 | 0 |
| `primitive.dithered_bulge.idle_paint_passes` | 0 | 0 | 0 | 0 |
| `primitive.dithered_bulge.idle_motion_clock_ticks` | 0 | 0 | 0 | 0 |
| `primitive.dithered_bulge.idle_ambient_frames` | 0 | 0 | 0 | 0 |
| `primitive.pill_plate.idle_frames_run` | 0 | 0 | 0 | 0 |
| `primitive.pill_plate.idle_frame_requests` | 0 | 0 | 0 | 0 |
| `primitive.pill_plate.idle_paint_passes` | 0 | 0 | 0 | 0 |
| `primitive.pill_plate.idle_motion_clock_ticks` | 0 | 0 | 0 | 0 |
| `primitive.pill_plate.idle_ambient_frames` | 0 | 0 | 0 | 0 |
| `primitive.gold_plate.idle_frames_run` | 0 | 0 | 0 | 0 |
| `primitive.gold_plate.idle_frame_requests` | 0 | 0 | 0 | 0 |
| `primitive.gold_plate.idle_paint_passes` | 0 | 0 | 0 | 0 |
| `primitive.gold_plate.idle_motion_clock_ticks` | 0 | 0 | 0 | 0 |
| `primitive.gold_plate.idle_ambient_frames` | 0 | 0 | 0 | 0 |
| `primitive.inset_well.idle_frames_run` | 0 | 0 | 0 | 0 |
| `primitive.inset_well.idle_frame_requests` | 0 | 0 | 0 | 0 |
| `primitive.inset_well.idle_paint_passes` | 0 | 0 | 0 | 0 |
| `primitive.inset_well.idle_motion_clock_ticks` | 0 | 0 | 0 | 0 |
| `primitive.inset_well.idle_ambient_frames` | 0 | 0 | 0 | 0 |
| `primitive.etched_divider.idle_frames_run` | 0 | 0 | 0 | 0 |
| `primitive.etched_divider.idle_frame_requests` | 0 | 0 | 0 | 0 |
| `primitive.etched_divider.idle_paint_passes` | 0 | 0 | 0 | 0 |
| `primitive.etched_divider.idle_motion_clock_ticks` | 0 | 0 | 0 | 0 |
| `primitive.etched_divider.idle_ambient_frames` | 0 | 0 | 0 | 0 |
| `primitive.seam_line.idle_frames_run` | 0 | 0 | 0 | 0 |
| `primitive.seam_line.idle_frame_requests` | 0 | 0 | 0 | 0 |
| `primitive.seam_line.idle_paint_passes` | 0 | 0 | 0 | 0 |
| `primitive.seam_line.idle_motion_clock_ticks` | 0 | 0 | 0 | 0 |
| `primitive.seam_line.idle_ambient_frames` | 0 | 0 | 0 | 0 |
| `primitive.grip.idle_frames_run` | 0 | 0 | 0 | 0 |
| `primitive.grip.idle_frame_requests` | 0 | 0 | 0 | 0 |
| `primitive.grip.idle_paint_passes` | 0 | 0 | 0 | 0 |
| `primitive.grip.idle_motion_clock_ticks` | 0 | 0 | 0 | 0 |
| `primitive.grip.idle_ambient_frames` | 0 | 0 | 0 | 0 |
| `primitive.material.idle_frames_run` | 0 | 0 | 0 | 0 |
| `primitive.material.idle_frame_requests` | 0 | 0 | 0 | 0 |
| `primitive.material.idle_paint_passes` | 0 | 0 | 0 | 0 |
| `primitive.material.idle_motion_clock_ticks` | 0 | 0 | 0 | 0 |
| `primitive.material.idle_ambient_frames` | 0 | 0 | 0 | 0 |
| `primitive.overlay_shadow.idle_frames_run` | 0 | 0 | 0 | 0 |
| `primitive.overlay_shadow.idle_frame_requests` | 0 | 0 | 0 | 0 |
| `primitive.overlay_shadow.idle_paint_passes` | 0 | 0 | 0 | 0 |
| `primitive.overlay_shadow.idle_motion_clock_ticks` | 0 | 0 | 0 | 0 |
| `primitive.overlay_shadow.idle_ambient_frames` | 0 | 0 | 0 | 0 |
| `primitive.sheen.idle_frames_run` | 0 | 0 | 0 | 0 |
| `primitive.sheen.idle_frame_requests` | 0 | 0 | 0 | 0 |
| `primitive.sheen.idle_paint_passes` | 0 | 0 | 0 | 0 |
| `primitive.sheen.idle_motion_clock_ticks` | 0 | 0 | 0 | 0 |
| `primitive.sheen.idle_ambient_frames` | 0 | 0 | 0 | 0 |
| `primitive.glass.idle_frames_run` | 0 | 0 | 0 | 0 |
| `primitive.glass.idle_frame_requests` | 0 | 0 | 0 | 0 |
| `primitive.glass.idle_paint_passes` | 0 | 0 | 0 | 0 |
| `primitive.glass.idle_motion_clock_ticks` | 0 | 0 | 0 | 0 |
| `primitive.glass.idle_ambient_frames` | 0 | 0 | 0 | 0 |
| `ribbon.motion_clock_ticks_while_moving` | 19 | 19 | 19 | 19 |
| `ribbon.idle_frames_run` | 0 | 0 | 0 | 0 |
| `ribbon.idle_frame_requests` | 0 | 0 | 0 | 0 |
| `ribbon.idle_paint_passes` | 0 | 0 | 0 | 0 |
| `ribbon.idle_motion_clock_ticks` | 0 | 0 | 0 | 0 |
| `ribbon.idle_ambient_frames` | 0 | 0 | 0 | 0 |
| `reduced_motion.ribbon.motion_clock_ticks` | 0 | 0 | 0 | 0 |
| `reduced_motion.ribbon.values_jumped` | 1 | 1 | 1 | 1 |
