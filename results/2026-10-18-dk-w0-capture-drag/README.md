# DK W0: capture drag and workspace transaction measurement

Date: 2026-10-18. Stage: DK, phase W0 (`planning/stages/DK-docking/PLAN.md`),
decisions 9 and 12. Status: headless trace recorded; the native measurement
on each host is the forensic probe below, not yet run on a real desktop.

## What is measured

Decision 12: a floating window is moved by the framework's own capture drag
(`WindowFrameDrag`), which reads the desktop pointer and requests the
window's frame on every pointer move, and a host that cannot keep up falls
back to the native move loop. The number that decides it is how often the
host actually moves the window, against how often it was asked:
`WindowFrameDragStatistics` counts pointer moves, frame requests and the
host's frame reports, and `keeps_up` holds when nine in ten requests were
reported, or when reports still arrive at 30 Hz or more (a compositor
coalescing moves to its frame rate).

Decision 9: a workspace transaction lays out each window once, shows new
windows before emptied ones are destroyed and publishes its notifications
once at the end.

## Headless (recorded)

`gui_forms_workspace_transaction_tests`, Windows build of this branch
(CLANG64), on the headless reference host; its output is
`headless-trace.txt` beside this file.

| Condition | Headless result |
|---|---|
| Two-window transaction, one layout per window | the main window's `arrange_passes` rose by 1; the new floating window's is 1 |
| One paint per window | `paint_passes` rose by 1 in each after the commit |
| Created hidden, shown before the emptied window is destroyed | trace: `create id=3`, `open id=3 ... visible=0`, `show id=3`, then `destroy id=2` |
| Notifications once, after the windows changed | two notifications, in order, each seeing the destroy |
| Capture drag, 30 moves at 16 ms | 30 requests, 30 reports, 60.5 Hz achieved, keeps up; the window kept the grab offset |
| Escape | the frame returned to where the drag began |
| Late reports (X11-like) | 0 reports during the drag, 10 after the flush, all counted |
| Host without the desktop pointer | `mode_for` gives `native_move_loop`; `begin` reports `unsupported` |

The headless host reports a frame synchronously, so 60 Hz there is the
pointer's rate, not a host's. It proves the bookkeeping, not smoothness.

## Native measurement: the forensic probe (decision 49)

The owner works remotely and the only local machine is a Mac, so no
measurement depends on a person watching (decision 49, interview part 16).
`gui_forms_frame_drag_probe` replaced the human-watched probe. It drives
`WindowFrameDrag` with synthetic pointer input, records every step and
writes a machine-readable verdict. It opens two windows, moves the cursor
and presses the primary button on its own window, so it is built but never
run by ctest; it was not run on the Windows development machine.

```text
gui_forms_frame_drag_probe [--out DIR] [--input auto|os|in-process]
```

- **Input.** `os` posts input through the operating system: Windows
  `SendInput`, macOS `CGEventPost` (needs the Accessibility permission),
  X11 XTest (linked when libXtst is installed). `in-process` warps the cursor
  (`SetCursorPos`, `CGWarpMouseCursorPosition`, `XWarpPointer`) and hands
  the pointer events to the window model itself, skipping the OS input
  path; the model ignores the events the host delivers. `auto` (the
  default) uses `os` where the host allows it. Every result names the path
  and the API used, and why OS injection was not used.
- **Calibration.** The probe warps the cursor and reads
  `ApplicationWindows::pointer_position` to map native pointer coordinates
  to desktop logical units; it assumes nothing about scaling.
- **Runs.** Three drags along a circle of radius 100 units: `slow` (90
  steps of 3 units every 16 ms), `medium` (120 of 6 every 8 ms) and `fast`
  (150 of 12 every 4 ms). In-process steps follow the UI timer, so their
  real rate is the timer's.
- **Per step it records** the input's time and position, when the move
  reached the window model, when `WindowFrameDrag::move` requested the
  frame and the content origin it asked for (`requested_origin`), and every
  frame the host reported, through `frame_changed`, with its time and
  position (Windows `WM_MOVE`, sent from `WM_WINDOWPOSCHANGED`; macOS
  `windowDidMove`; X11 `ConfigureNotify`). On Windows each report also carries DWM's refresh
  counter and latest vertical blank (`DwmGetCompositionTimingInfo`). macOS
  gives the nominal refresh period (`CVDisplayLink`, used only as a clock;
  no GPU presentation timestamp is read). X11 gives none, and 60 Hz is
  assumed and marked as assumed.
- **It computes** latency percentiles (request to frame, input to request,
  input to frame: p50, p95, p99, max), requests reported exactly, coalesced
  into a later report or never reported, reports at positions nobody asked
  for, missed refreshes (refreshes that passed while a request waited, from
  DWM's counter where it can, from the period otherwise), position error
  (each report against the latest target, and the final one), and
  `WindowFrameDragStatistics::keeps_up`, the verdict that decides the
  native-loop fallback.
- **It writes** `frame-drag-<platform>-<input>-<profile>.json` for each run
  and `summary.md` into `--out` (default `frame-drag-results`), closes its
  windows and exits 0 when every run kept up, 1 when one did not, and 2
  when a run could not be measured (the press never arrived, the host
  refused the drag, a 120-second deadline passed).
- `gui_forms_frame_drag_analysis_tests` (ctest, headless) checks the
  analysis: matching, coalescing, missed refreshes, percentiles and the
  JSON writer.

A frame report shows that the host accepted the position, not that a
refresh displayed it; the DWM counter is the nearest the probe gets to a
presentation timestamp. CI runs the probe without blocking on the macOS
and Linux (Xvfb with Openbox) native jobs and uploads the JSON
(`frame-drag-probe-<platform>`); a CI runner is not an owner's desktop,
so those numbers say whether the probe works and how a virtual display
behaves, not how a real one feels.

| Host | Input | slow / medium / fast frames_hz | keeps_up | Fallback needed |
|---|---|---|---|---|
| Windows (Win32) | not measured | | | |
| macOS (AppKit) | not measured | | | |
| X11 (Xorg) | not measured | | | |
| X11 under XWayland | not measured | | | |

The plan asks for XWayland to be measured, not assumed: the same probe, run
under XWayland on GNOME (Mutter) and KDE (KWin), also shows whether the
compositor honours client positions (`foreign_reports`, an unchanged
position, or a `final` position error near the whole path means it does
not).
