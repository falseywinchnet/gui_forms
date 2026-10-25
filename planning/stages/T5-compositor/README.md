# Stage T5: compositor cooperation

Release: 0.3 Themeable. Depends on: T2. Runs in parallel with T3 and T4.
Status 2026-10-25: plan accepted ([`PLAN.md`](PLAN.md)); the owner took
the recommendation on all sixteen questions (below). C0 is next. Authority:
`planning/FORWARD_PROGRAMME.md` §4 T5.

Read [the shared brief](../README.md) first.

## Why

The owner allows "partial or total transparency, reflections, glass effects.
To a degree, those require the system window manager and frameworks to
facilitate, and all we do is provide the necessary capability to *allow* for
it." And: "I merely want us to facilitate the system compositor demands
without friction. We don't necessarily theme them, we're just able to
cooperate because that is native to our design."

So there are two halves. Inside the window, glass, reflection and
translucency are engine effects (T2) that sample only the style backplane.
Seeing what is *behind* the window needs the system compositor. GUI.Forms
asks for it as a host capability and falls back to an opaque window when the
host cannot provide it.

## What exists today

- The host protocol reports capabilities as bits in `HostCapability`
  (`include/gui_forms/host/types/host_capability/host_capability.hpp`):
  lifecycle, occlusion, frame capture, accessibility and others. None covers
  window transparency or materials.
- Hosts present a CPU framebuffer: AppKit (`src/host/macos/`), Win32 DIB
  presentation (`src/host/windows/`), X11 (`src/host/linux/`). The paint
  pipeline has coherent paint leases and exact presentation receipts.
- Custom-chrome and layered-material labs exist on macOS and Windows
  (`custom_chrome_lab_main`, `layered_material_lab_main`).

## What to deliver

1. Capability bits, reported per host and per compositor state:
   - non-opaque windows with per-pixel alpha;
   - behind-window materials: macOS vibrancy (`NSVisualEffectView`
     materials), Windows layered windows and DWM system backdrops (acrylic,
     mica), and X11 ARGB visuals, which need a running compositor.
2. A way for a window (and a theme through the engine) to request them, with
   a declared fallback when the host cannot honour the request.
3. Notification when the capability changes at run time: an X11 compositor
   starting or stopping, or a Windows transparency setting toggled.
4. The presented framebuffer carries correct premultiplied alpha where the
   window is non-opaque.
5. Reflection (moved from T2's E9, owner 2026-10-22): a bounded copy of a
   part's own content or of the backplane within its grant. It sits beside
   full-fidelity glass, the host `render_backplane` upgrade behind T2's
   backplane borrow.

## How

- Capability-reported adapter behaviour, never hidden core policy
  (`AGENTS.md` GIVEN). No portable semantic may assume one platform's model.
- AppKit types stay in `src/host/macos/`, Win32 and DWM in
  `src/host/windows/`, X11 in `src/host/linux/`.
- CPU only. Behind-window blur is the compositor's own work; GUI.Forms never
  touches a GPU API to get it. In-window blur is a bounded CPU operation
  cached against the backplane's damage generation (design language §4.6).
- Never theme the compositor: no window-manager decorations, no
  compositor-side effects requested beyond the material the window asks for.
- Design language limits: translucency only where it clarifies layer and
  depth, never as a constant surface effect (C104); dark or translucent side
  structure is rejected in the default (V101 R). These limit the default
  theme, not the capability.
- Linux: the X11 host, also under XWayland, is the supported path. Do not
  add a Wayland-native adapter or describe its absence as a gap.
- House style §8: compositor notifications state their thread and reach the
  UI thread through the dispatcher; §9: requesting an unsupported material
  returns an explicit result, not a silent no-op.

## Gate

Capability reported correctly per host and per compositor state; a test
window renders with and without the capability and falls back without error.
Record per host, including X11 with and without a compositor, under
`results/`.

## Owner decisions, interview round 001 (2026-10-25)

The owner took option 1, the recommendation, on every question of
[`PLAN.md`](PLAN.md) section 8. Cited as T5-Q1 to T5-Q16.

1. **T5-Q1, the backdrop vocabulary.** Portable purposes, a closed set:
   `opaque`, `transparent`, `window_material`, `transient_material`, each
   host mapping them to its own materials.
2. **T5-Q2, undocumented interfaces.** Documented interfaces only:
   Windows 10 gets `transparent`, system materials from Windows 11 22H2
   (build 22621).
3. **T5-Q3, who asks.** The application declares that a window may be
   non-opaque; the theme chooses the backdrop. Both are needed.
4. **T5-Q4, when.** Declared at window creation; at run time only the
   grant changes.
5. **T5-Q5, fallback.** A declared chain of up to three entries ending at
   `opaque`; the default chain is the preferred backdrop then `opaque`,
   and `transparent` is a fallback only when listed. GUI.Forms draws the
   opaque fallback itself, never leaving clear pixels to the compositor's
   own fallback colour.
6. **T5-Q6, input.** The whole client area receives input on every host;
   no click-through in T5.
7. **T5-Q7, host preferences.** T5 builds a general host preference
   signal (high contrast on or off, reduced transparency) feeding
   `PresentationSettings`; T4 extends it with the dark preference and the
   high-contrast palette. macOS "increase contrast" is reported but does
   not set `high_contrast`.
8. **T5-Q8, the theme's view.** A window-level theme record: the backdrop
   the theme asks for and the backplane base over it. Rules see the grant
   as a per-window condition through T3's `@media` (EC3), with CSS's
   `prefers-reduced-transparency` and a GUI.Forms feature for the granted
   backdrop.
9. **T5-Q9, X11 libraries.** Link libXfixes (selection notifications) and
   libXrender (the visual's alpha format); two runtime dependencies are
   added to the `.deb`.
10. **T5-Q10, X11 blur.** None in T5: X11 offers `transparent` only.
11. **T5-Q11, macOS 26 glass.** Not in T5.
12. **T5-Q12, Linux CI.** The Linux CI job installs `xcompmgr`, and the
    gate test starts and stops it under Xvfb.
13. **T5-Q13, the backplane replay.** A renderer service every private
    renderer implements, reported in the renderer's capabilities
    (`MetricsSnapshot`, beside `renderer_name`).
14. **T5-Q14, reflection's own source.** The part's engine raster only
    (face, material, glow); text is not reflected.
15. **T5-Q15, reflection's reach.** At most 8 logical units, the blur and
    glow cap.
16. **T5-Q16, reduced transparency inside the window.** Glass is drawn
    flattened (its tint over the backplane colour, opaque, unblurred);
    reflections are omitted.

### Open owner concern: Wayland (with T5-Q9)

The owner's note on T5-Q9, verbatim: "wayland is growing in popularity. we
shipped on x11 because as we understood x11 programs work on wayland. linux
is fragmented here, and i am not entirely confident how to proceed."

This is a concern, not a decision. The coordinator is taking the Wayland
direction to the owner; T5 plans no Wayland-native work. To inform that
discussion, T5's X11 measurements (C4 and the stage gate) include the
XWayland case: compositor detection (`_NET_WM_CM_S0` ownership) and the
depth-32 visual, per desktop measured, recorded under `results/`.

The Windows 7 (Aero-like) Windows-familiar theme (T4) will be the first
theme to request behind-window glass.

## Read first

- `planning/DESIGN_LANGUAGE_SPECIFICATION.md` §4.1, §4.6, §9.3
- `docs/user/hosts-and-capabilities.md`
- `planning/interview/GUI_FORMS_INTERVIEW_LEDGER.md` GF001 (host protocol),
  GF002
