# Stage T5 plan: compositor cooperation

Status 2026-10-25: proposed, for the owner's review. No T5 code exists
yet, and none is written until the open questions in section 8 are
answered. The brief is [`README.md`](README.md); the authority is
`planning/FORWARD_PROGRAMME.md` §4 T5. Where this plan and a recorded owner
decision disagree, the decision governs.

It builds on:

- design language §4.1 (the two planes; in-window effects sample the
  backplane, never other controls), §4.6 (translucency only where it
  clarifies layer and depth; blur is a bounded CPU operation; window
  transparency is a reported host capability, opaque without it) and §9.3
  item 4;
- interview GF001 (the host is the framework's treaty with the operating
  system, H6: a native loop plus the GUI.Forms host protocol) and GF002
  (no GPU capability: the operating system may composite the finished
  window bitmap, which does not make GUI.Forms GPU-aware);
- T2's E9-B and E9.3: glass already blurs a cached backplane image inside
  the window; full-fidelity glass (the `render_backplane` upgrade behind
  the same borrow) and reflection moved here (owner, 2026-10-22 and
  2026-10-23);
- `AGENTS.md` GIVEN: honest platform differences are capability-reported
  adapter behaviour, never hidden core policy.

## What T5 builds

Two halves, kept apart.

1. **Behind the window** (the compositor's work). A window may ask the
   window system to show what lies behind it: plain per-pixel
   transparency, or a system material (a blurred, tinted backdrop the
   compositor draws). GUI.Forms never reads the desktop and never blurs
   it; it presents a framebuffer with correct premultiplied alpha and asks
   the host for the material. Each host reports what it can do now, and a
   request it cannot honour comes back with an explicit result and an
   opaque window, never an error and never a silent no-op.
2. **Inside the window** (the engine's work). Glass gains full fidelity:
   the backplane image it blurs includes text and application drawing,
   not only engine part rasters. Reflection arrives: a bounded, mirrored,
   faded copy of a part's own raster or of the backplane within its
   grant. Both stay CPU operations cached against damage generations.

The two meet at the window's backplane base: where a backdrop is granted,
the backplane may be partly clear and the compositor's material shows
through; where it is not, the theme's opaque window material is drawn, as
today.

High contrast and reduced transparency (the operating-system settings, and
the application's own switch) turn every part of this off: the backdrop
request is withdrawn, the window is opaque, and in-window glass and
reflection draw their opaque fallbacks (section 4.5).

What T5 does not build: window-manager decorations or caption theming,
whole-window opacity (`_NET_WM_WINDOW_OPACITY`, `SetLayeredWindowAttributes`
with a constant alpha), shaped or click-through windows, a Wayland-native
adapter, and any GPU path.

## 1. What each window system offers

The minimum versions below are those of the documented interfaces. Each
phase's first gate measures the behaviour on real machines; where this
section says "measured in Cn", the plan does not assume the result.

GUI.Forms' own minimums today: Windows 10 x64, macOS 14.0 on Apple
silicon (`CMAKE_OSX_DEPLOYMENT_TARGET 14.0`), Ubuntu 24.04 with an X11
display or XWayland (`docs/user/download.md`).

### 1.1 Windows (DWM)

Desktop composition is always on from Windows 8, so on every supported
Windows the compositor is present; what varies is the OS build and the
user's settings.

| Facility | Interface | Minimum | Notes |
|---|---|---|---|
| Per-pixel alpha in the client area, native frame kept | `DwmExtendFrameIntoClientArea` (margins −1), or `DwmEnableBlurBehindWindow` with an empty blur region | Windows Vista (both documented); Windows 10 for GUI.Forms | Keeps `WM_PAINT` and today's 32-bit top-down DIB presentation; DWM reads the DIB's alpha as premultiplied. From Windows 8 blur-behind no longer blurs; it only makes the client alpha visible. Which of the two routes gives clean per-pixel alpha with the native caption on Windows 10 and 11 is measured in C2. |
| Per-pixel alpha for a borderless window | `WS_EX_LAYERED` with `UpdateLayeredWindowIndirect` (`ULW_ALPHA`, `prcDirty`) | Windows Vista for the dirty rectangle | The whole window is the bitmap: no system frame, no `WM_PAINT`. Fits popups and drag images, not top-level windows with a native caption. Fully clear pixels pass input through natively (section 2.5). |
| Mica (the window material) | `DwmSetWindowAttribute(DWMWA_SYSTEMBACKDROP_TYPE = 38, DWMSBT_MAINWINDOW = 2)` | Windows 11 22H2, build 22621 | Needs the frame extended into the client area, so the client's clear pixels show it. DWM itself shows a solid colour while the window is inactive, under energy saver and with transparency effects off. |
| Mica Alt | `DWMSBT_TABBEDWINDOW = 4` | build 22621 | A stronger wallpaper tint, for tabbed title areas. |
| Acrylic (the transient material) | `DWMSBT_TRANSIENTWINDOW = 3` | build 22621 | Meant for popups and transient surfaces. |
| Earlier Mica | `DWMWA_MICA_EFFECT` (1029) | Windows 11 21H2, build 22000 | Undocumented. Not used unless the owner admits it (question Q2). |
| Windows 10 acrylic or blur | `SetWindowCompositionAttribute` with an accent policy | Windows 10 1803 | Undocumented, and known to lag on resize. Not used unless admitted (Q2). |
| Transparency effects setting | `HKCU\Software\Microsoft\Windows\CurrentVersion\Themes\Personalize` `EnableTransparency`, re-read on `WM_SETTINGCHANGE` with `"ImmersiveColorSet"` | Windows 10 | The documented equivalent is WinRT's `UISettings.AdvancedEffectsEnabled`; C2 measures that the registry value and the broadcast agree with it. |
| High contrast | `SystemParametersInfoW(SPI_GETHIGHCONTRAST)`, `HCF_HIGHCONTRASTON`; `WM_SETTINGCHANGE` with `SPI_SETHIGHCONTRAST` | Windows 2000 | |

On Windows 10 the documented interfaces give per-pixel transparency but no
system material; on Windows 11 22H2 and later they give both. The CI runner
(`windows-2022`, Windows Server 2022, build 20348) has no system backdrop,
so CI exercises the fallback; the Shadow Windows machine (Windows 11, build
26200) exercises the granted path. Whether DWM composes per-pixel alpha in
the CI runner's session at all is measured in C2.

The host already loads `dwmapi.dll` at run time and types the entry point
from its declaration (`windows_application_run.inc`); T5 does the same for
`DwmSetWindowAttribute`, `DwmExtendFrameIntoClientArea` and
`DwmEnableBlurBehindWindow`. Constants absent from the toolchain's headers
(the backdrop attribute and its values) are named once, with the SDK
documentation they come from.

### 1.2 macOS (AppKit)

The window server always composites.

| Facility | Interface | Minimum | Notes |
|---|---|---|---|
| Non-opaque window | `NSWindow.opaque = NO`, `backgroundColor = NSColor.clearColor`; the raster view's layer not opaque | macOS 10.0 | The view already draws a premultiplied RGBA `CGImage` (`kCGImageAlphaPremultipliedLast`) with `kCGBlendModeCopy`, so clear pixels stay clear. |
| Behind-window materials | `NSVisualEffectView`, `blendingMode = NSVisualEffectBlendingModeBehindWindow`, `state` following the window's active state | semantic materials from macOS 10.14 | The effect view becomes the window's content view and the raster view sits above it. Materials: `windowBackground`, `underWindowBackground`, `sidebar`, `menu`, `popover`, `hudWindow`, `titlebar`, `headerView`, `sheet`, `toolTip` and others. AppKit draws an opaque fallback itself under reduced transparency. |
| Reduced transparency, increased contrast | `NSWorkspace.accessibilityDisplayShouldReduceTransparency`, `accessibilityDisplayShouldIncreaseContrast`; `NSWorkspaceAccessibilityDisplayOptionsDidChangeNotification` on the workspace notification centre | macOS 10.10 | Observed with `addObserver:selector:name:object:`, a named target, not the block form. |
| Glass effect view | `NSGlassEffectView` | macOS 26 | Not in T5's first phases (question Q11). |

Everything in the table is available at the macOS 14.0 deployment target.
CI runs `macos-15`. Recording what the screen shows behind a window needs
screen-recording permission; the gate therefore records the host's own
state (section 6) and the presented framebuffer, and screenshots come from
the owner's Mac mini bench only if the owner agrees.

### 1.3 X11

| Facility | Interface | Minimum | Notes |
|---|---|---|---|
| ARGB visual | `XMatchVisualInfo(display, screen, 32, TrueColor, ...)`; the alpha mask confirmed with `XRenderFindVisualFormat` | an X server with the Composite extension (which provides the depth-32 visual) and XRender | The window is created with its own colormap, `background_pixel` 0 and `border_pixel` 0 (otherwise `BadMatch`). A window's visual is fixed at creation. By convention compositors treat ARGB pixels as premultiplied. |
| Compositor presence | `XGetSelectionOwner` of `_NET_WM_CM_S<screen>` | EWMH | Owned by any compositing manager: Mutter, KWin, xfwm4 with compositing on, picom, xcompmgr. |
| Compositor starting and stopping | `XFixesSelectSelectionInput` on that selection: set-owner, owner-window-destroyed and client-close notifications | XFixes 1.0 | Arrives in the X event queue the UI thread already reads. A dependency on libXfixes is question Q9. |
| No compositor | | | The server ignores alpha and shows the premultiplied colour, so clear areas show black. The window is then presented opaque (section 2.3). |
| Blur behind | `_KDE_NET_WM_BLUR_BEHIND_REGION` | KWin | A compositor-specific hint; no portable X11 material exists. Question Q10. |

Xvfb, which CI uses (`xvfb-run -s "-screen 0 1280x1024x24"`), has the
Composite extension but no compositor; whether it offers the depth-32
visual is measured in C4. Starting a small compositor (`xcompmgr`) under
Xvfb in the Linux CI job gives the with-compositor state and the start and
stop transitions (question Q12). Under XWayland the Wayland compositor
always composites; whether `_NET_WM_CM_S0` is owned there, per desktop, is
measured, not assumed.

### 1.4 Wayland (note only)

Wayland compositors always composite, and a surface with an alpha format
is translucent; blur needs a compositor-specific protocol. GUI.Forms has no
Wayland adapter by design: Wayland sessions run the X11 host through
XWayland, and T5 adds nothing Wayland-native.

## 2. The capability model

### 2.1 What the host reports

Static capability bits, additive to `HostCapability` (no protocol version
change for the bits themselves), each true only if the adapter implements
it:

- `transparent_windows`: the adapter can present a window with per-pixel
  premultiplied alpha whenever the compositor state allows it;
- `window_backdrops`: the adapter can ask the window system for a material
  behind a transparent window;
- `compositor_notifications`: the adapter reports compositor and
  preference changes at run time;
- `presentation_preferences`: the adapter reports the host's high
  contrast and reduced-transparency settings (question Q7).

The live state is a record, because it changes while the program runs:

```text
CompositorState
  generation            CompositorGeneration (its own wrapper type, never
                        mixed with the backplane's damage generation)
  compositing           present | absent | unknown
  transparency_enabled  the user's transparency-effects setting
  reduced_transparency  host setting
  high_contrast         host setting
  available             WindowBackdropSet: the backdrops a window could be
                        granted now
```

`Application::compositor_state()` returns it before any window exists, as
`capabilities()` does. `HostCapabilities::to_json` and the gate record
both. Adding the record to the host protocol raises `protocol_version` to
9.

### 2.2 The portable backdrop vocabulary

A closed set of purposes, not platform names (question Q1):

| `WindowBackdrop` | Meaning | Windows | macOS | X11 |
|---|---|---|---|---|
| `opaque` | today's window; always available | | | |
| `transparent` | clear pixels show what is behind, unblurred | client alpha (C2's route) | non-opaque window | ARGB visual under a compositor |
| `window_material` | the system's material for a main window | Mica (22621 and later) | `underWindowBackground` (C3 confirms the choice) | none (Q10) |
| `transient_material` | the system's material for popups and transient panels | Acrylic (22621 and later) | `popover` or `menu` (C3 confirms) | none (Q10) |

A host maps each purpose to its own material and reports the mapping in
the gate record; the portable core never names Mica, Acrylic or
`NSVisualEffectMaterial`.

### 2.3 Requests, grants and fallback

The application declares at window creation that a window may be
non-opaque (`ApplicationWindow` option; question Q4), because the X11
visual and the Windows window style are fixed then. The theme chooses which
backdrop it wants for that window (question Q3). The window is granted the
first entry of its fallback chain that the host can honour now (question
Q5; the chain always ends at `opaque`).

```text
WindowBackdropRequest  preferred backdrop, then up to two fallbacks
WindowBackdropGrant    granted backdrop
                       reason: granted | not_declared | host_unsupported |
                               os_version | no_compositor |
                               transparency_disabled | reduced_transparency |
                               high_contrast | after_shutdown
                       compositor generation it was decided at
```

- Resolving a grant is a pure function of the request and the
  `CompositorState`, in the core, tested headless. Adapters report state
  and apply grants; they do not decide.
- `ApplicationWindowHandle::request_backdrop` returns the grant
  synchronously, as an explicit result. An undeclared window gets
  `opaque` with `not_declared`; a call after close gets `after_shutdown`.
- An `opaque` grant is drawn by GUI.Forms, with the theme's opaque window
  material, rather than left to the compositor's own fallback colour
  (Mica's inactive solid, X11's black): the window always looks like the
  theme, and the pixels are what the frame capture records.
- Native state is validated before it is changed: if the host call that
  applies a material fails, the window keeps its previous grant and the
  failure is reported in the grant's reason.

### 2.4 Changes at run time

- **Sources.** X11: the XFixes selection notifications. Windows:
  `WM_SETTINGCHANGE` (`ImmersiveColorSet`, `SPI_SETHIGHCONTRAST`) and
  `WM_DWMCOMPOSITIONCHANGED` (kept for completeness, constant on
  Windows 8 and later). macOS: the accessibility display options
  notification.
- **Thread.** All three arrive on the UI thread (the window procedure, the
  X event loop, the main run loop). The adapter does not act on them in
  place: it posts one `compositor_state_changed` host event through the
  dispatcher, so it takes its place in the event trace like any other host
  event, and the headless host can replay it byte for byte.
- **Order.** For one change: the application's `CompositorState` is
  replaced and its generation advanced; then each window's grant is
  resolved again, in window-open order; each window whose grant changed
  raises `backdrop_changed`, advances its backplane generation and damages
  the whole backplane plane; the next flush paints and presents once. A
  change that arrives inside an update scope waits for the scope's flush
  boundary. Handlers that call `request_backdrop` during
  `backdrop_changed` get a grant against the new state; the event is not
  raised again for the same generation.
- **No error.** A compositor stopping is not a failure: the window turns
  opaque at the next frame and its grant says why.

### 2.5 Input over clear pixels

GUI.Forms hit testing is the retained geometry, and the input region of a
window is its whole client area on every host. Where a native mechanism
passes input through fully clear pixels (layered windows on Windows,
borderless non-opaque windows on macOS), the adapter keeps input on the
window, and C2 and C3 measure that it does. Click-through and shaped
windows are not offered in T5 (question Q6).

## 3. The presented framebuffer

The engine's rasters are premultiplied RGBA8, and the hosts already
present premultiplied formats (BGRA DIBs on Windows, a premultiplied
`CGImage` on macOS). T5 makes the alpha meaningful:

- an `opaque` window presents alpha 255 in every pixel, and its presented
  bytes are unchanged from today (every existing reference hash holds);
- a window with a non-opaque grant starts its frame from clear, not from an
  opaque fill, and its backplane base may leave pixels partly or fully
  clear;
- on X11 the depth-32 visual packs alpha into the native pixel (the
  conversion in `linux_host.cpp` gains the alpha channel; the 24-bit path
  is unchanged);
- damage-limited presentation (stage DP) still copies, never blends, so a
  region that becomes clear is cleared on screen (macOS already uses
  `kCGBlendModeCopy`; Windows' `BitBlt` and X11's `XPutImage` copy);
- `frame_capture` keeps the alpha channel, so the gate's captures show it.

## 4. How it joins the engine

### 4.1 The window's backplane base

Today the backplane image and the window's first paint use the theme's
opaque window material. With a non-opaque grant the theme supplies a second
base for that backdrop: typically a tint at partial alpha over a
`window_material` grant, or fully clear around a drawn shape over
`transparent`. The grant selects which base is baked; a grant change
advances the backplane generation, so every cached backplane image and
every part that sampled it is baked again (E9.3's damage rule, unchanged).
How a theme names the base for a backdrop, and how its rules see the grant,
is question Q8.

### 4.2 Glass over a clear backplane

In-window glass keeps E9.3's kernel and borrow. Over a region where the
backplane is clear, it blurs clear pixels and adds its tint, so the
compositor's material shows through the tint: the blur behind the window is
the compositor's, the tint is the theme's. Glass never samples the desktop,
other windows or other controls (design language §4.1 item 3; GF002).

### 4.3 Full-fidelity glass: the backplane replay

E9.3's backplane image composites only the backplane plane's engine part
rasters; text and application drawing are skipped and counted in
`backplane_skipped_commands`. T5 adds the upgrade E9-B named: the window's
renderer replays the backplane plane's whole command list for a region into
a caller-supplied raster of the borrow's extent, with the same saves,
groups, translations and clips the window paint uses.

- It sits behind the same borrow: `PrimitiveTarget::backplane`, the same
  margin, origin and generation. A primitive cannot tell which path made
  the image, except by its bytes.
- Every renderer implements it (the Skia CPU adapter on macOS and Linux,
  the DIB compositor on Windows), and because they share the blending
  arithmetic and the text engine, the image is the same on every host;
  the reference hashes say so. The headless renderer implements it too.
- It is reported, so a renderer that cannot replay keeps E9.3's
  part-raster image and the counter keeps counting (question Q13 asks
  whether it is a renderer or a host capability).
- Cost: a replay paints the backplane commands of the region once per
  backplane generation and region. It shares the 4 MB engine budget and its
  least-recently-used order; counters `backplane_replays`, and
  `backplane_skipped_commands` at 0 where replay is used.

### 4.4 Reflection

A part may carry a reflection (CSS_COVERAGE row 10): a copy of its own
raster or of the backplane under it, mirrored about one of its edges,
offset by a gap, faded by a linear alpha from a start to an end value, and
clipped to the part's overflow grant.

- The own-raster source is the part's engine raster as baked (face,
  material, glow); whether it also carries the part's text through the
  replay is question Q14.
- The backplane source is the backplane borrow mirrored, so it shows the
  replayed backplane when 4.3 is in use.
- Bounds: the reflection's reach is at most the part's grant and at most a
  cap (question Q15); a rule asking for more is rejected when the theme
  compiles (`parameter_out_of_range`), as glass's blur is.
- Caching follows E7r: an own-raster reflection is made once per bake of
  the raster it copies; a backplane reflection is keyed to the backplane
  generation and region, never shared or keyframed, and damages its part
  when the backplane under it changes. A part in the backplane plane never
  reflects the backplane.
- It is a material layer or a primitive parameter, not a new control
  vocabulary atom; if it needs a new `PartRole` or state, that is reported
  to the owner, not added.

### 4.5 High contrast and reduced transparency

- **The switch.** `PresentationSettings` gains `reduced_transparency`
  beside `high_contrast` and `reduced_motion`. A host that reports
  `presentation_preferences` sets both from the operating system and
  updates them on change (Windows high contrast and transparency effects;
  macOS reduce transparency and increase contrast, the latter feeding
  `high_contrast` only if the owner wants it, Q7). The application may
  also set them, as it sets reduced motion today.
- **Behind the window.** Either setting withdraws the backdrop: the grant
  becomes `opaque` with the reason `high_contrast` or
  `reduced_transparency`, whatever the host could otherwise do.
- **Inside the window.** Under high contrast, glass, reflection and every
  translucent layer are dropped, since high-contrast recipes are rebuilt as
  solid plates (as §9.3 does for glow). Under reduced transparency, glass
  draws flattened (its tint composited over the backplane colour, opaque,
  unblurred) and reflections are omitted (question Q16).
- These rules are engine rules, tested headless; the themes do not
  re-implement them.

## 5. Phases

Each phase is one to three PRs on main with its own small gate; stacks
stay at most two deep. Host code stays in `src/host/windows/`,
`src/host/macos/` and `src/host/linux/`; the portable model goes in
`include/gui_forms/host/` and `src/core/host/`; engine changes in
`src/core/engine/` with T2's tests run beside T5's.

**C0. The portable model, headless.** `WindowBackdrop`, `WindowBackdropSet`,
`CompositorState`, `CompositorGeneration`, `WindowBackdropRequest`,
`WindowBackdropGrant`, the four capability bits, protocol version 9, the
resolution function, `request_backdrop`, `backdrop_changed`,
`compositor_state_changed` and its event order; the headless host gains
options for a compositor state and a scheduled change at a given turn.
Docs: `hosts-and-capabilities.md` gains the rows (all "no" until each host
lands). Gate: resolution covers every request against every state;
headless traces of a declared and an undeclared window through a
compositor stopping and starting, and through high contrast on and off,
are byte-stable; refusals return explicit grants. 2 PRs.

**C1. Premultiplied presentation.** The frame starts clear for a
non-opaque grant; opaque windows present alpha 255 everywhere;
`frame_capture` keeps alpha. Gate: headless raster tests of both; every
existing reference hash unchanged on all CI hosts. 1 PR.

**C2. Windows.** The measurement first, recorded: per-pixel alpha in the
client area through each documented route, with the native caption, on
Windows 10 behaviour (Server 2022 on CI) and Windows 11 (Shadow Windows);
then `transparent`, `window_material` and `transient_material` through
`DWMWA_SYSTEMBACKDROP_TYPE`, the transparency-effects and high-contrast
readings and their `WM_SETTINGCHANGE` updates, input over clear pixels.
Gate: `gui_forms_windows_compositor_tests` on CI (fallback state) and the
same run on Shadow Windows (granted state, and transparency effects
toggled), recorded. 2 to 3 PRs.

**C3. macOS.** Non-opaque `NSWindow`, the `NSVisualEffectView` behind the
raster view with the material mapping, the accessibility display options
observer, input over clear pixels. Gate: `gui_forms_macos_compositor_tests`
on `macos-15` reading back the window's opacity, the effect view's
material, blending mode and state, and the presented framebuffer's alpha,
recorded. 2 PRs.

**C4. X11.** The depth-32 visual at creation for declared windows, alpha
packing, `_NET_WM_CM_Sn` presence and the XFixes notifications. Gate:
`gui_forms_linux_compositor_tests` under Xvfb with no compositor (grant
`opaque`, `no_compositor`, alpha 255), with `xcompmgr` running (grant
`transparent`), and across `xcompmgr` stopping and starting (two
`backdrop_changed` events in order), recorded. 2 PRs.

**C5. The engine join.** The backdrop base of 4.1, glass over a clear
backplane (4.2), `reduced_transparency` and the high-contrast and
reduced-transparency rules of 4.5, the theme-side carrier of question Q8.
Gate: `gui_forms_engine_backdrop_tests`: reference hashes of a window and
a glass part under each grant; a grant change rebakes the backplane image
once and damages only the parts that sampled it; reduced transparency
flattens glass and drops reflection; high contrast drops both. 2 PRs.

**C6. Full-fidelity glass.** The backplane replay of 4.3 in the headless,
Skia and DIB renderers. Gate: `gui_forms_engine_glass_tests` gains glass
over backplane text, with `backplane_skipped_commands` 0 and identical
hashes on all three CI hosts; E9.3's existing hashes are unchanged for the
part-raster path; the replay's cost recorded. 2 PRs.

**C7. Reflection.** The layer of 4.4, both sources, validation, caching
and damage. Gate: `gui_forms_engine_reflection_tests`: reference hashes
of each source and edge at scale 1 and 2, the cap rejected at compile, an
own-raster reflection baked once per raster bake, a backplane reflection
rebaked on backplane damage under it and not on damage elsewhere; the cost
recorded. 2 PRs.

**C8. The stage gate.** A small compositor lab program per host (beside
`layered_material_lab`), a window with glass, a reflection and a declared
backdrop, and the gate record of section 6. Docs: the capability table in
`hosts-and-capabilities.md`, a user page on transparency and its
fallbacks, the T5 README and the stage board. 1 PR.

C0 and C1 come first. C2, C3 and C4 can then run in any order, and C5 can
start beside them, since the headless host exercises every grant. C6 needs
C5; C7 needs C6 for its backplane source and can land its own-raster
source after C5.

## 6. The stage gate

From the brief: the capability is reported correctly per host and per
compositor state; a test window renders with and without the capability
and falls back without error. Recorded under
`results/<date>-t5-compositor/`, one file per host and state, each with the
machine, the OS build, the exact command, and:

- `HostCapabilities::to_json` and `CompositorState` as reported;
- each request made, the grant returned, and the reason;
- the host's own read-back of what it applied (Windows: the
  `DwmGetWindowAttribute` value and the extended frame; macOS: the window's
  `opaque` flag and the effect view's material, blending mode and state;
  X11: the window's visual depth and the selection owner);
- the presented framebuffer's alpha (minimum, maximum and the count of
  non-opaque pixels) and its frame capture;
- for each run-time change, the event sequence as traced.

The states: Windows on CI (Server 2022: transparent only, materials
`os_version`) and on Shadow Windows (Windows 11: every backdrop, then
transparency effects off, then high contrast on); macOS on `macos-15`
(every backdrop, and reduced transparency where the runner allows setting
it; otherwise on the bench with the owner's agreement); X11 under Xvfb
without a compositor, with `xcompmgr`, and across it stopping and
starting. Every state must end in a rendered window and no error.

## 7. House-style points

- **Types.** `CompositorGeneration` is its own wrapper, never compared
  with the backplane's damage generation. `WindowBackdrop` and the grant
  reasons are closed enums; every switch over them has its fallback
  (house style §9). A grant is a record with named fields, never a bare
  boolean.
- **One decision point.** Grant resolution is one pure core function.
  Adapters report state and apply grants; they never decide policy
  (`AGENTS.md` GIVEN).
- **Foreign interfaces.** Every dynamically loaded entry point
  (`DwmSetWindowAttribute`, `DwmExtendFrameIntoClientArea`,
  `DwmEnableBlurBehindWindow`, and the XFixes and XRender calls if they are
  resolved at run time) is typed from its declaration with `decltype` or a
  static assertion, never retyped. Each states its counts, ownership and
  errors: the `HRESULT` is kept in the grant's diagnostics; XFixes' event
  base comes from `XFixesQueryExtension` before use.
- **Ownership.** The `NSVisualEffectView` is owned by the window's view
  hierarchy and released with it; the X11 colormap is freed with its window;
  the XFixes selection input and the workspace observer are removed at
  shutdown, before the owner they report into is destroyed.
- **Callbacks.** The macOS observer is a named selector on the host's
  window controller, not a block; X11 and Windows notifications are
  handled in the existing event loops. Each posts one host event through
  the dispatcher (§8); none mutates a window in place.
- **Validate before mutating.** A material is applied only after the
  request is resolved; a failed native call leaves the previous grant and
  native state.
- **No compensating checks.** An opaque grant makes the engine bake an
  opaque base; the host does not also force alpha to 255 in its copy loop.
- **Kernels.** The replay and the reflection allocate their targets before
  painting and reuse them through the engine budget; no per-pixel
  allocation, logging or callbacks. The mirror copy handles empty and
  short extents first.
- **C++ spelling.** No `auto`, lambdas, trailing returns or structured
  bindings; `(*pointer).member`; hand-written comparisons.
- **Docs agree with the build.** The capability table changes in the PR
  that changes what a host advertises.

## 8. Open questions for the owner

Each is a numbered option list with a recommendation (marked **R**).

**Q1. The backdrop vocabulary.**
1. Portable purposes, a closed set: `opaque`, `transparent`,
   `window_material`, `transient_material`, each host mapping them to its
   own materials. **R**
2. Platform materials by name (Mica, Acrylic, `NSVisualEffectMaterial`
   names), each capability-reported.
3. Only `opaque` and `transparent`; no system materials.

**Q2. Undocumented operating-system interfaces** (Windows 10 accent
policy, Windows 11 21H2's attribute 1029).
1. Documented interfaces only: Windows 10 gets `transparent`, materials
   from Windows 11 22H2. **R**
2. Admit them behind version checks and their own capability reasons.

**Q3. Who asks for a backdrop.**
1. The application declares a window may be non-opaque; the theme chooses
   the backdrop. Both are needed. **R**
2. The application alone.
3. The theme alone, for any window.

**Q4. When a window becomes able to be non-opaque.**
1. Declared at creation; at run time only the grant changes. **R**
2. At any time, the host recreating the native window where needed (X11
   visual, Windows styles).

**Q5. Fallback.**
1. A declared chain, up to three entries ending at `opaque`; the default
   chain is the preferred backdrop then `opaque`, and `transparent` is a
   fallback only when listed. GUI.Forms draws the opaque fallback itself
   rather than leaving clear pixels to the compositor's own fallback
   colour. **R**
2. Always straight to `opaque`, no chain.
3. Leave clear pixels to the compositor's fallback (Mica's solid colour,
   AppKit's opaque material) where it has one.

**Q6. Input over clear pixels.**
1. The whole client area receives input on every host; no click-through
   in T5. **R**
2. Offer click-through over fully clear pixels as a further capability.

**Q7. Host accessibility preferences.**
1. T5 builds a general host preference signal (high contrast on or off,
   reduced transparency), feeding `PresentationSettings`; T4 extends it
   with the dark preference and the high-contrast palette. macOS
   "increase contrast" is reported but does not set `high_contrast`. **R**
2. As 1, and macOS "increase contrast" sets `high_contrast`.
3. T5 reads only reduced transparency; reading high contrast waits for T4.

**Q8. How a theme sees the grant and names the backdrop base.**
1. A window-level theme record: the backdrop the theme asks for and the
   backplane base over it; rules see the grant as a per-window condition
   through T3's `@media` (EC3), with CSS's own `prefers-reduced-transparency`
   and a GUI.Forms feature for the granted backdrop. **R**
2. A new environment state atom beside `high_contrast` and
   `reduced_motion` (a vocabulary change).
3. Both.

**Q9. X11 libraries.**
1. Link libXfixes (selection notifications) and libXrender (the visual's
   alpha format); both ship with every X desktop and add two runtime
   dependencies to the `.deb`. **R**
2. No new libraries: detect the compositor by polling the selection owner
   at each wake and assume the depth-32 visual's spare byte is alpha.

**Q10. Blur behind on X11.**
1. None in T5: X11 offers `transparent` only. **R**
2. Set KWin's `_KDE_NET_WM_BLUR_BEHIND_REGION` when KWin is detected and
   report `window_material` there.

**Q11. macOS 26's glass effect view.**
1. Not in T5; revisit when the deployment target or an availability
   check makes it worthwhile. **R**
2. Map `window_material` to it on macOS 26 and later.

**Q12. A compositor in Linux CI.**
1. The Linux CI job installs `xcompmgr` and the gate test starts and
   stops it under Xvfb. **R**
2. The with-compositor state is recorded only on a desktop machine.

**Q13. Where the backplane replay lives.**
1. A renderer service every private renderer implements, reported in the
   renderer's capabilities (`MetricsSnapshot` beside `renderer_name`). **R**
2. A host capability bit, as E9-B worded it.

**Q14. Reflection's own-raster source.**
1. The part's engine raster only (face, material, glow); text is not
   reflected. **R**
2. The part's whole content, text included, through the replay.

**Q15. Reflection's reach cap.**
1. 8 logical units, the blur and glow cap. **R**
2. 16 logical units.
3. The part's grant only, with no further cap.

**Q16. In-window effects under reduced transparency.**
1. Glass drawn flattened (tint over the backplane colour, opaque,
   unblurred); reflections omitted. **R**
2. Unchanged: only the behind-window backdrop is withdrawn.
3. Glass replaced by the part's built-in recipe.

## 9. Size

Nine phases, C0 to C8, of about 16 to 17 PRs: C0 2, C1 1, C2 2 to 3, C3 2,
C4 2, C5 2, C6 2, C7 2, C8 1. About 7 of them (C1, C5, C6 and C7) change
the engine or the core paint path and run T2's tests with T5's. No timing
is claimed.
