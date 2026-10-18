# DK W0: capture drag and workspace transaction measurement

Date: 2026-10-18. Stage: DK, phase W0 (`planning/stages/DK-docking/PLAN.md`),
decisions 9 and 12. Status: headless trace recorded; the native measurement
on each host is a documented step, not yet run.

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

## Native measurement (to do, with the owner's permission)

The probe puts two windows on the desktop, so it is built but never run by
ctest, and was not run on this machine: the brief allows a native
measurement here only with the owner's permission.

```text
# scripted: 120 frame requests at the 16 ms timer cadence; one JSON line
gui_forms_frame_drag_probe
# manual: drag the "Drag me" window's body with the mouse; one JSON line per
# drag; close the main window to finish
gui_forms_frame_drag_probe --manual
```

Each line reads `{"probe":"frame_drag","mode":...,"platform":...,
"pointer_moves":...,"frame_requests":...,"frame_reports":...,"refused":...,
"duration_s":...,"pointer_hz":...,"frames_hz":...,"keeps_up":...}`. To fill
in per host, for both modes (manual: three drags, slow, medium, fast):

| Host | Scripted frames_hz | Manual frames_hz (slow / medium / fast) | keeps_up | Fallback needed |
|---|---|---|---|---|
| Windows (Win32) | not measured | not measured | | |
| macOS (AppKit) | not measured | not measured | | |
| X11 (Xorg) | not measured | not measured | | |
| X11 under XWayland | not measured | not measured | | |

The plan asks for XWayland to be measured, not assumed: the same probe, run
under XWayland on GNOME (Mutter) and KDE (KWin), also shows whether the
compositor honours client positions (a `frames_hz` near zero, or windows
that do not move, means it does not).
