# Hosts and capabilities

The retained engine knows nothing about AppKit, Win32 or X11. A host adapter
translates one platform's windows, events and services into the portable host
protocol, and the engine asks the adapter what it can do rather than
assuming. Platform differences are reported, never simulated: a capability
that is absent returns `unsupported`, and your program can decide what to do.

## The host protocol

`HostCapabilities` carries `protocol_version` (currently 7), a `platform`
string and a bit set of `HostCapability` flags. `Application::capabilities()`
returns the set for the host your program was linked against, before any
window exists. `supports(flag)` answers one question; `to_json()` dumps the
whole record for logs.

| Capability | Meaning | macOS | Windows | Linux X11 |
|---|---|:-:|:-:|:-:|
| `lifecycle` | attach, activation, close, shutdown events | yes | yes | yes |
| `scale_notifications` | logical scale changes at run time | yes | yes | no |
| `monitor_geometry` | monitor and work-area records and change events | yes | yes | yes |
| `occlusion` | window hidden/exposed notifications that gate painting | yes | no | yes |
| `scheduled_wake` | timed wake for frames and timers | yes | yes | yes |
| `pointer_input`, `keyboard_input` | translated pointer and key events | yes | yes | yes |
| `text_composition` | input-method composition bridged to the window | yes | yes | no |
| `pointer_capture` | synchronous capture routed to one control | yes | yes | yes |
| `cursor` | portable cursor kinds applied natively | yes | yes | yes |
| `clipboard`, `clipboard_images` | bounded UTF-8 text and PNG images | yes | yes | yes |
| `typed_drag_destination` | inbound text, file-list and media-typed drops | yes | no | yes |
| `dialogs` | the five portable common-dialog request/result families | yes | yes | yes |
| `sound_cues` | system alert sounds | yes | yes | yes |
| `menus` | native menu-bar publication | no | no | no |
| `font_discovery` | host font enumeration | no, by design | no, by design | no, by design |
| `accessibility` | native accessibility publication of the semantic tree | not yet advertised | not yet advertised | not yet advertised |
| `typed_drag_source` | outbound drag initiation | no | no | no |

The table reflects the bits each adapter currently advertises; read
`Application::capabilities()` rather than this page in code.

## The three native hosts and the headless one

macOS (AppKit)
: `src/host/macos/`, the only place Objective-C++ is allowed. One `NSWindow`
  owns a custom view that presents the CPU raster; a display link drives
  scheduled frames; NSTextInputClient bridges IME composition; Retina scale
  arrives as a scale notification. Pointer capture maps onto the native
  implicit drag sequence rather than a global event tap. Fonts are read from
  the bundle.

Windows (Win32)
: `src/host/windows/`, the only place Win32, COM and WIC types are allowed. An
  ordinary resizable top-level window presents a top-down 32-bit DIB with
  software alpha blending; DPI awareness is loaded at run time with a fallback
  for older systems; WIC decodes the PNG resources the core has already
  validated; text comes from the shared engine, not GDI. An opt-in
  `WM_COPYDATA` automation channel addresses controls by stable ID.
  MSAA/UIA providers, TSF composition beyond committed text, OLE drag/drop
  and native menu publication are not implemented.

Linux (X11)
: `src/host/linux/`, built with `-DGUI_FORMS_ENABLE_LINUX_HOST=ON`. Xlib
  window and event handling, Xcursor cursors, Xfixes selection monitoring,
  XDND drag destinations, and ATK groundwork for accessibility. The CPU
  raster is presented through X11 images. There is no Wayland adapter;
  Wayland sessions run the X11 host through XWayland.

Headless
: `src/host/headless/`, built everywhere and linked by the tests. It emits
  attach, drains one initialization turn, then delivers configured synthetic
  activation and input, producing a byte-stable trace. It is the reference
  ordering every native adapter must match, and it is what you link for
  deterministic UI tests of your own.

## What stays portable

Event order, focus and capture semantics, the close sequence, dialog request
and result shapes, clipboard payload limits, the scheduler's wake rules and
coordinate conversion up to window client space are defined once in the core.
No adapter may change them. Platform truths that differ (whether scale can
change at run time, whether composition exists, which dialogs are native) are
exposed as capability bits and `HostServiceStatus` results, not as hidden
behaviour differences.

Two consequences are worth knowing. Coordinate conversion ends at window
client coordinates; a desktop-screen origin and multi-monitor transform are
host facts you query through monitor records. And the core never encodes one
platform's assumption about dialogs, paths or menus; a request a host cannot
honour comes back `unsupported` immediately.

## Rendering adapters

The host presents pixels; it does not paint them. Painting goes through the
`Painter` implemented by a private renderer:

- A CPU-only Skia build (no GPU backend, no font backend, PNG as the only
  codec) on macOS and Linux, confined to `src/render/skia/`.
- A DIB compositor on Windows that implements the same blending arithmetic
  byte for byte.

`MetricsSnapshot::renderer_name` names the active one and `cpu_only` is
always true. No Skia type crosses into a public header, a control or a test
of the renderer-neutral core; a boundary test enforces that.

## Multiple windows

`Application::run(std::vector<ApplicationWindow>)` presents up to 64 windows
in one host run, each with its own `Window` model, stable ID, options and an
optional owner ID for tool windows. The `ready` callback hands you an
`ApplicationWindowHandle`, a weak, thread-affine handle for `show`, `hide`,
`toggle_full_screen`, `set_title` and `request_close`; requests after close
return `after_shutdown`. One window's handlers may mutate another window's
model and have it present on the next wake.

## Choosing a host at build time

Enable exactly one native host per build (`GUI_FORMS_ENABLE_MACOS_HOST`,
`GUI_FORMS_ENABLE_WINDOWS_HOST`, `GUI_FORMS_ENABLE_LINUX_HOST`). The installed
`GUIForms::Application` target is that host; a consumer that needs another
platform installs the SDK built there. Renderer-free consumers link
`GUIForms::Controls` and are host independent.
