# T5 measurements

Each phase's first gate measures the platform before an adapter relies on
it (PLAN.md §1, "measured in Cn"). This file records what was measured,
where and how, and what the adapter takes from it. A measurement that has
not run yet says so.

## C2. Windows (DWM)

**How.** `gui_forms_windows_compositor_probe` (`tests/windows_compositor_probe.cpp`,
Win32 only, nothing of GUI.Forms) shows a solid green backdrop window
(0, 160, 0) and, over it, a captioned `WS_OVERLAPPEDWINDOW` whose client is
a premultiplied 32-bit top-down DIB painted with `BitBlt` in `WM_PAINT`, as
the host presents today. The client has three columns:

- clear: alpha 0;
- half: red at alpha 128, premultiplied (100, 0, 0, 128);
- opaque: blue (0, 0, 220).

For each case it shows a fresh window, waits 40 compositions (`DwmFlush`),
and reads the composed screen with a one-pixel `BitBlt` from the screen DC
at each column's centre and in the caption. It asks `WindowFromPoint` and
`WM_NCHITTEST` about a point over the clear column. It then activates
another window and reads the clear column again. Over the backdrop, a
client whose alpha DWM honours composes to (0, 160, 0), (100, 80, 0) and
(0, 0, 220). It prints one JSON line per case and one with the settings.

The routes:

- `extend_frame`: `DwmExtendFrameIntoClientArea` with margins −1;
- `blur_behind`: `DwmEnableBlurBehindWindow` with an empty region;
- `mica`, `acrylic` and `mica_alt`: the frame extended, then
  `DWMWA_SYSTEMBACKDROP_TYPE` 2, 3 and 4;
- `blur_mica`: blur behind, then type 2;
- three changes on a shown window: withdraw the first route (blur behind
  off, margins 0, `DWMSBT_AUTO`), then apply the second.

### Windows 11, build 26200 (Shadow Windows), 2026-10-25

Light theme, transparency effects on, high contrast off, a local session.
The colours are (red, green, blue).

| Case | Clear, active | Clear, inactive | Half | Opaque | Caption | Clear pixel hits |
|---|---|---|---|---|---|---|
| none (today) | 0, 0, 0 | 0, 0, 0 | 100, 0, 0 | 0, 0, 220 | 240, 243, 249 | the window (client) |
| extend_frame | 255, 255, 255 | 255, 255, 255 | 227, 127, 127 | 0, 0, 220 | 255, 255, 255 | the window (client) |
| blur_behind | **0, 160, 0** | **0, 160, 0** | **100, 80, 0** | 0, 0, 220 | 240, 243, 249 | the window (client) |
| mica | 240, 244, 249 | 243, 243, 243 | 220, 122, 124 | 0, 0, 220 | 240, 243, 249 | the window (client) |
| acrylic | 95, 221, 95 | 211, 211, 211 | 147, 110, 47 | 0, 0, 220 | 95, 221, 95 | the window (client) |
| mica_alt | 208, 220, 236 | 232, 232, 232 | 204, 110, 118 | 0, 0, 220 | 208, 220, 236 | the window (client) |
| blur_mica | 240, 244, 249 | 243, 243, 243 | 220, 122, 124 | 0, 0, 220 | 240, 243, 249 | the window (client) |
| mica, then none | 0, 0, 0 | 0, 0, 0 | 100, 0, 0 | 0, 0, 220 | 240, 243, 249 | the window (client) |
| blur_behind, then mica | 240, 244, 249 | 243, 243, 243 | 220, 122, 124 | 0, 0, 220 | 240, 243, 249 | the window (client) |
| mica, then blur_behind | 0, 160, 0 | 0, 160, 0 | 100, 80, 0 | 0, 0, 220 | 240, 243, 249 | the window (client) |

Every call returned `S_OK`. Settings: `EnableTransparency` 1, and
`UISettings.AdvancedEffectsEnabled` true, which agree in this one state.
Composition was on.

What this shows:

1. **`transparent` is blur behind with an empty region.** It is the only
   route whose clear pixels show what lies behind the window. The client's
   alpha composes exactly as premultiplied, the native caption is
   unchanged, and it holds while the window is inactive.
2. **Extending the frame is not a `transparent` route on Windows 11.**
   Clear pixels show the frame's own solid colour (white in the light
   theme), the caption turns white, and partial alpha composes over that
   colour, not over the content behind.
3. **The materials** come from `DWMWA_SYSTEMBACKDROP_TYPE` over either an
   extended frame or blur behind. Clear and partly clear client pixels
   compose over the material, and the caption takes it too. While the
   window is inactive DWM shows its own solid colour: Mica 243, Acrylic
   211, Mica Alt 232 (grey).
4. **A live window's route can change.** Withdrawing it with blur behind
   off, margins 0 and `DWMSBT_AUTO` restores today's window, caption
   included. An earlier run withdrew with `DWMSBT_NONE` instead, and that
   left a plain white caption. `DWMSBT_AUTO` is what a window that never
   set the attribute has.
5. **Input.** A clear pixel still hits the window (`HTCLIENT`, and
   `WindowFromPoint` returns the window) on every route, as owner decision
   T5-Q6 requires: no click-through.

For the adapter: blur behind with an empty region enables the client's
alpha for every non-opaque grant. `DWMWA_SYSTEMBACKDROP_TYPE` (main window
2, transient 3) selects a material on build 22621 and later, and
`DWMSBT_AUTO` goes with withdrawing. `opaque` withdraws everything. One
mechanism serves every grant, so changing between grants never changes
the frame.

Not measured yet:
- Toggling transparency effects, to compare the registry value, the
  `WM_SETTINGCHANGE` broadcast and `AdvancedEffectsEnabled` as the setting
  changes. Changing that user setting is the owner's to do on Shadow
  Windows; the C2 gate's run records it.
- High contrast on.
- The dark theme.

### Windows Server 2022, build 20348 (CI `windows-2022`)

Pending: the probe runs in this PR's Windows job, and its JSON lines are
in the `gui-forms-evidence-windows-x64` artifact's `LastTest.log`. The next
C2 PR records them here: whether DWM composes per-pixel alpha in the
runner's session at all, and what `DWMWA_SYSTEMBACKDROP_TYPE` returns below
build 22621.
