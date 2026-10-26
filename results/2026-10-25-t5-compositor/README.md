# T5 compositor gate runs, 2026-10-25

Stage T5's gate record (planning/stages/T5-compositor/PLAN.md §6), one
file per host and compositor state. Phase C2 adds the Windows runs; C3,
C4 and C8 add theirs.

## Windows 11, build 26200 (Shadow Windows): granted state

**MEASURED:** `windows-11-26200-granted.jsonl`, the output of

```text
.build/native-windows-x64/gui-forms/gui_forms_windows_compositor_tests.exe
```

built from `claude/t5-c2-windows` (Release, the pinned clang64 toolchain,
Skia off: the DIB painter). Light theme, transparency effects on, high
contrast off, a local session. Three runs printed identical lines.

The test shows a green Win32 window (0, 160, 0) behind an
`Application`-run window whose theme draws the window clear, then reads
the composed screen at the client's centre after each grant:

| Step | Request | Grant, reason | Clear pixel | Hits the window |
|---|---|---|---|---|
| material | `window_material`, falling back to `transparent` | `window_material`, granted | 240, 244, 249 (Mica) | yes |
| transparent | `transparent` | `transparent`, granted | 0, 160, 0 (the window behind) | yes |
| opaque | `opaque` | `opaque`, granted | 0, 0, 0 (alpha ignored again) | yes |

`CompositorState` as reported: generation 1, compositing present, every
backdrop supported, transparency enabled, reduced transparency off, high
contrast off.

## Not recorded yet

- Windows on CI (Server 2022, build 20348): the same test's fallback
  state (`transparent` granted for the material with reason
  `os_version`), from the C2 PR's `gui-forms-evidence-windows-x64`
  artifact.
- Shadow Windows with transparency effects off, then high contrast on:
  these are the owner's user settings to change; the run waits for the
  owner.
