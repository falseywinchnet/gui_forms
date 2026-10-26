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
`WM_NCHITTEST` about a point over the clear column, and reads
`DWMWA_SYSTEMBACKDROP_TYPE` back with `DwmGetWindowAttribute`. It then activates
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
5. **The attribute reads back.** `DwmGetWindowAttribute` returns
   `DWMWA_SYSTEMBACKDROP_TYPE` with `S_OK`: 0 (`DWMSBT_AUTO`) on a window
   that never set it, and the type a case set. An adapter can learn that
   the attribute exists without setting it.
6. **Input.** A clear pixel still hits the window (`HTCLIENT`, and
   `WindowFromPoint` returns the window) on every route, as owner decision
   T5-Q6 requires: no click-through.

For the adapter: blur behind with an empty region enables the client's
alpha for every non-opaque grant. `DWMWA_SYSTEMBACKDROP_TYPE` (main window
2, transient 3) selects a material where the attribute reads back, and
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

From the `windows-x64` native job of this PR, 2026-10-25 (the
`gui-forms-evidence-windows-x64` artifact, `SDKLastTest.log`). The
runner's session composes: composition on, not a remote session,
`EnableTransparency` 1 and `AdvancedEffectsEnabled` true, high contrast
off.

| Case | Clear, active | Clear, inactive | Half | Opaque | Caption | Clear pixel hits |
|---|---|---|---|---|---|---|
| none (today) | 0, 0, 0 | 0, 0, 0 | 100, 0, 0 | 0, 0, 220 | 255, 255, 255 | the window (client) |
| extend_frame | 255, 255, 255 | 255, 255, 255 | 227, 127, 127 | 0, 0, 220 | 255, 255, 255 | the window (client) |
| blur_behind | **0, 160, 0** | **0, 160, 0** | **100, 80, 0** | 0, 0, 220 | 255, 255, 255 | the window (client) |
| mica, acrylic, mica_alt | 255, 255, 255 | 255, 255, 255 | 227, 127, 127 | 0, 0, 220 | 255, 255, 255 | the window (client) |
| blur_mica | 0, 160, 0 | 0, 160, 0 | 100, 80, 0 | 0, 0, 220 | 255, 255, 255 | the window (client) |
| mica, then none | 0, 0, 0 | 0, 0, 0 | 100, 0, 0 | 0, 0, 220 | 255, 255, 255 | the window (client) |
| blur_behind, then mica | 255, 255, 255 | 255, 255, 255 | 227, 127, 127 | 0, 0, 220 | 255, 255, 255 | the window (client) |
| mica, then blur_behind | 0, 160, 0 | 0, 160, 0 | 100, 80, 0 | 0, 0, 220 | 255, 255, 255 | the window (client) |

Setting `DWMWA_SYSTEMBACKDROP_TYPE` returned `E_INVALIDARG` (0x80070057)
in every material case, and reading it back returned `E_INVALIDARG` on
every window. Every other call returned `S_OK`.

What this shows:

1. **Blur behind with an empty region is `transparent` here too,** with
   the same exact premultiplied composition, active and inactive.
2. **There are no materials below build 22621,** and the read-back says
   so before anything is set: the attribute's `E_INVALIDARG` is the
   adapter's `os_version` answer. A failed set leaves the route beneath
   it as it was (blur_mica still shows the backdrop; mica over an
   extended frame shows the frame's white).
3. **Withdrawing still restores today's window,** and a clear pixel still
   hits the window.

The two builds agree on everything the adapter uses. CI's Windows runner
can therefore run the C2 gate's `transparent` step for real, and its
material step exercises the `os_version` fallback to `transparent`.

### The owner's gate runs, Shadow Windows, 2026-10-25

The C2 gate (`gui_forms_windows_compositor_tests`, built from
`claude/t5-c2-windows` 58d61a18), run by the owner in three states. Two
found defects on our side; neither run is evidence of the setting itself.

1. **High contrast on.** The state line read `"compositing":"absent"`
   with `"high_contrast":true`, and `material` and `transparent` were
   granted `opaque` with the reason `no_compositor`. The gate failed on
   "DWM composes on every supported Windows". Two defects:
   - `DwmIsCompositionEnabled` answers FALSE under a contrast theme on
     build 26200, although DWM cannot be off from Windows 8 on. The
     adapter now reports compositing present on Windows 8 and later and
     asks `DwmIsCompositionEnabled` only on an older system.
   - High contrast was checked after the host's refusals, so a misread
     compositor hid the user's setting. It is now checked right after
     `not_declared` (C0, `window_backdrop_refusal`), and the gate checks
     that a withdrawn backdrop gives the user's setting as its reason.
2. **Transparency effects off.** The state line read
   `"transparency_enabled":true,"reduced_transparency":false`, and every
   step was granted as at normal settings. Either the setting had not
   taken effect when the program read it, or the reader is wrong; the run
   cannot say which. The gate's state line now carries the raw sources,
   read independently of the host: `EnableTransparency` and its read
   status, `UISettings.AdvancedEffectsEnabled`, `SPI_GETHIGHCONTRAST`'s
   flags, `DwmIsCompositionEnabled` and the build. The run scripts show
   the registry value before the run starts.
3. **Restored (normal settings)** passed, as at the first gate run.

The rerun of 1 and 2 with the fixed program is the owner's.

## C3. macOS (the window server)

**How.** `gui_forms_macos_compositor_probe` (`tests/macos_compositor_probe.mm`,
AppKit only, nothing of GUI.Forms) shows a solid green backdrop window
(0, 160, 0) and, over it, a titled window whose view draws premultiplied
pixels with the copy blend mode, as the host presents today, in the same
three columns as C2 (clear, half red, opaque blue). For each case it shows
a fresh window and reads:

- the composed screen at each column's centre and in the title bar
  (`CGWindowListCreateImage` of the window and what lies below it);
- the window's own image with its alpha (the window alone);
- the window the window server reports under the clear column
  (`windowNumberAtPoint:belowWindowWithWindowNumber:`), and the view a
  click posted there through the HID event tap reaches;
- what AppKit reads back: the window's opaque flag, its background
  alpha, `ignoresMouseEvents`, and the effect view's material, blending
  mode, state and appearance.

The cases:

- `none`: today's window;
- `clear`: the window non-opaque on a clear background;
- `clear_receiving`: as `clear`, with `ignoresMouseEvents` set to NO
  explicitly;
- one case per `NSVisualEffectMaterial`: a behind-window, follows-active
  effect view under the drawing view, in a non-opaque window;
- `under_window_background_opaque_window`: the effect view in an opaque
  window;
- `clear_then_none` and `effect_then_clear`: a change on a shown window.

### macOS 15.7.9 (CI `macos-15`, arm64), 2026-10-25

From the `macos-arm64` native job of #338 (run 40248265605, the
`gui-forms-evidence-macos-arm64` artifact, `SDKLastTest.log`). The
runner has **Reduce transparency on**; increase contrast and
differentiate without colour off; appearance aqua; one screen at backing
scale 1; screen capture and accessibility granted.

| Case | Clear | Half | Own clear (with alpha) | Window under the clear pixel |
|---|---|---|---|---|
| none (today) | 246, 246, 246 | 223, 123, 123 | 246, 246, 246, 255 | the probe |
| clear | **0, 160, 0** | 103, 66, 3 | **0, 0, 0, 0** | **the backdrop** |
| clear_receiving | **0, 160, 0** | 103, 66, 3 | 0, 0, 0, 0 | **the probe** |
| windowBackground, menu, sheet | 246, 246, 246 | 223, 123, 123 | 246, 246, 246, 255 | the probe |
| underWindowBackground, underPageBackground, sidebar, titlebar, popover, toolTip | 236, 236, 236 | 218, 118, 118 | 236, 236, 236, 255 | the probe |
| contentBackground, headerView | 255, 255, 255 | 227, 127, 127 | 255, 255, 255, 255 | the probe |
| hudWindow | 240, 240, 240 | 220, 120, 120 | 240, 240, 240, 255 | the probe |
| underWindowBackground, opaque window | 236, 236, 236 | 218, 118, 118 | 236, 236, 236, 255 | the probe |
| clear, then none | 246, 246, 246 | 223, 123, 123 | 246, 246, 246, 255 | the probe |
| underWindowBackground, then clear | **0, 160, 0** | 100, 80, 0 | 0, 0, 0, 0 | the backdrop |

In every case the window's opaque flag and background alpha read back as
set, and every effect view read back as behind-window and
follows-active-state. No posted click reached any view, so the click
column is left out; the hit test is the window server's answer.

What this shows:

1. **A non-opaque window on a clear background is `transparent`:** its
   clear pixels show exactly what lies behind, even with reduce
   transparency on, and the window's own image keeps the premultiplied
   alpha (0, 0, 0, 0). The half column read (103, 66, 3) on a fresh
   window and (100, 80, 0), the exact premultiplied composite, after a
   change; the difference is colour management of the device-RGB image
   and does not affect clear or opaque pixels.
2. **Clear pixels are click-through unless `ignoresMouseEvents` is set to
   NO explicitly,** although it reads back NO before it is set. The
   adapter sets it on every non-opaque grant (owner decision T5-Q6).
3. **With reduce transparency on, every material is AppKit's solid
   fallback,** grey to white by material (236 to 255), never the
   backdrop: the materials cannot be told apart from their fallbacks on
   this runner. Whether `underWindowBackground` and `popover` look right
   for `window_material` and `transient_material` is the bench Mac run's
   question (the C3 run sheet in the T5 track); CI's gate records the
   host's withdrawal instead and reports the material's screen check as
   not exercised.
4. **Withdrawing restores today's window:** opaque again, the clear
   pixel the window's own background.

**What the adapter takes from it** (`src/host/macos/compositor/`): the
window server always composites; `transparent` is a non-opaque window on
a clear background; the materials are a behind-window effect view under
the raster view, inside a container made with the window, because moving
the raster view out of its window stops its display link; reduce
transparency comes from `NSWorkspace` and its change notification;
`ignoresMouseEvents` is set to NO on every non-opaque grant.

### The bench Mac run (owner), 2026-10-25

The probe's windows were never active, so its material readings are not
evidence. On macOS 14 and later `[NSApp activate]` is granted only when
the active application yields, and Terminal, which started the probe,
does not. `lsappinfo` reported the probe frontmost, but the screenshots
show grey traffic lights and Terminal's menu bar. Every effect view
(`followsWindowActiveState`) therefore drew its inactive solid colour:
all 12 materials failed criterion 4 in Light and Dark, and no case
received its posted click. Criteria 1 to 3 passed.

The probe is now an application bundle launched through LaunchServices
(`open`, `tests/support/run_macos_probe_bundle.sh`), which activates it
as an ordinary application, with `activateIgnoringOtherApps:` only as a
fallback; its settings line records which one worked. Each case records
the window's `isKeyWindow` and `NSApp.isActive`, and a material case read
while either is false prints only `"not_exercised":"window inactive"`,
after which the run fails with "not exercised: window inactive". The
effect views still follow the window's active state, as the adapter's
do: the rerun confirms the materials in an active window, not a forced
state. The rerun is the owner's, from the fresh bench sheet.
