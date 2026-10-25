# Stage T5: compositor cooperation

Release: 0.3 Themeable. Depends on: T2. Runs in parallel with T3 and T4.
Status 2026-10-25: plan proposed ([`PLAN.md`](PLAN.md)), awaiting the
owner's answers to its open questions; no code. Authority:
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

## Open decisions (ask, do not decide)

None recorded yet; the plan's section 8 lists Q1 to Q16 for the owner.
The Windows 7 (Aero-like) Windows-familiar theme (T4) will be the first
theme to request behind-window glass.

## Read first

- `planning/DESIGN_LANGUAGE_SPECIFICATION.md` §4.1, §4.6, §9.3
- `docs/user/hosts-and-capabilities.md`
- `planning/interview/GUI_FORMS_INTERVIEW_LEDGER.md` GF001 (host protocol),
  GF002
