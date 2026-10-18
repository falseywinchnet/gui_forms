# Hosts and capabilities

The retained engine knows nothing about AppKit, Win32 or X11. A host adapter
translates one platform's windows, events and services into the portable host
protocol, and the engine asks the adapter what it can do rather than
assuming. Host service requests report `HostServiceStatus`, including
`unsupported` where a service is unavailable. Capability flags describe the
advertised protocol surface; some native integrations are not advertised yet.

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
| `clipboard`, `clipboard_images` | bounded UTF-8 text and straight-alpha RGBA8 images | yes | yes | yes |
| `typed_drag_destination` | inbound text, file-list and media-typed drops | yes | no | yes |
| `dialogs` | the five portable common-dialog request/result families | yes | yes | yes |
| `sound_cues` | system alert sounds | yes | yes | yes |
| `frame_capture` | encode the presented framebuffer as PNG | yes | yes | yes |
| `notification_area` | notification-area (tray) icons with click, double-click and context events (`NotifyIcon`) | yes (status item) | yes | when a system tray owns `_NET_SYSTEM_TRAY_Sn` at startup |
| `notification_area_messages` | notification text from a tray icon | no | yes | no |
| `notification_area_menus` | a native menu shown from a tray icon | yes | yes | no |
| `menus` | native menu-bar publication | no | no | no |
| `font_discovery` | host font enumeration | no, by design | no, by design | no, by design |
| `accessibility` | native accessibility publication of the semantic tree | not yet advertised | not yet advertised | not yet advertised |
| `typed_drag_source` | outbound drag initiation | no | no | no |
| `runtime_windows` | owned windows opened and destroyed while the application runs | yes | yes | yes |
| `window_frames` | window frames read and requested in desktop logical units, with a report for every move and resize | yes | yes | yes (the window manager decides) |
| `screen_pointer` | the pointer's desktop position | yes | yes | yes |
| `window_at_point` | the application's topmost window at a desktop point | yes | yes | yes |
| `window_ordering` | bring a window to the front of the application's own windows | yes | yes | yes (the window manager may decline) |

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
  software alpha blending. The process asks for per-monitor DPI awareness
  (`SetProcessDpiAwarenessContext`, resolved at run time, falling back to
  `SetProcessDPIAware`); the scale is the window DPI divided by 96, never
  below 1, and `WM_DPICHANGED` delivers a scale notification and applies the
  suggested window rectangle. WIC decodes the PNG resources the core has
  already validated. Text comes from the shared engine, not GDI, using the
  bundled faces in `fonts\` beside the executable, or in the directory
  `GUI_FORMS_FONT_DIR` names; with an incomplete font pack the renderer name
  says so and no text metrics provider is installed. An opt-in `WM_COPYDATA`
  automation channel, off unless the host options set `automation_enabled`
  (the gallery sets it from `--automation`), addresses controls by stable
  ID; it accepts commands of at most 4096 bytes with the prefix
  `GUI.Forms.Automation/1`.
  MSAA publication is implemented through `IAccessible`; a separate UIA
  provider is not implemented. The adapter advertises `text_composition`, but
  its current input path forwards committed `WM_CHAR` text rather than a TSF
  preedit session. OLE drag/drop and native menu publication are not implemented.

Linux (X11)
: `src/host/linux/`, built with `-DGUI_FORMS_ENABLE_LINUX_HOST=ON`. Xlib
  window and event handling, Xcursor cursors, X11 clipboard selections,
  XDND drag destinations, and native ATK/AT-SPI accessibility publication.
  The CPU raster is presented through X11 images. There is no Wayland adapter;
  Wayland sessions run the X11 host through XWayland.

Headless
: `src/host/headless/`, built everywhere and linked by the tests. It emits
  attach, drains one initialization turn, then delivers configured synthetic
  activation and input, producing a byte-stable trace. It is the reference
  ordering every native adapter must match. This adapter is a private build
  target, not an installed SDK component. SDK consumers can link
  `GUIForms::Controls` to test retained models without a native host.

The missing `accessibility` bit does not mean native publication is absent:
the adapters have accessibility implementations. It means that the current
capability record does not advertise that integration.

## What stays portable

Event order, focus and capture semantics, the close sequence, dialog request
and result shapes, clipboard payload limits, the scheduler's wake rules and
coordinate conversion up to window client space are defined once in the core.
No adapter may change them. Platform truths that differ (whether scale can
change at run time, whether composition exists, which dialogs are native) are
exposed as capability bits and `HostServiceStatus` results, not as hidden
behaviour differences.

Two consequences are worth knowing. Inside a window, coordinates are the
window's client coordinates; across windows, the windowing seam (below) uses
desktop logical units. And the core never encodes one
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
`toggle_full_screen`, `set_title`, `request_client_size` and `request_close`;
requests after close return `after_shutdown`. `request_client_size` asks the
native window for a client size; the model follows when the host's resize
event arrives, as for a user resize. `Window::resize` only reports a size the
host already has, so calling it from application code leaves the model and
the screen disagreeing. One window's handlers may mutate another window's
model and have it present on the next wake.

### Runtime windows and the desktop

`ApplicationWindowHandle::windows()` returns the run's `ApplicationWindows`
(stage DK decisions 6 and 7). `open_window` opens an owned or tool window
while the application runs and `destroy_window` destroys one without asking
its closing handler; at most 64 windows are live at once, and identities
(`HostWindowId`) are never reused within a run. `ApplicationWindowHandle::
frame` and `request_frame` read and place a window's client rectangle,
`frame_changed` reports every move and resize, `pointer_position` and
`window_at` answer where the pointer is and which of the application's
windows is under a point, and `bring_to_front` orders a window above the
application's other windows without activating it and never above other
applications' windows. Each is a capability bit above; a host that cannot
do one reports `unsupported`.

These requests use desktop logical units: origin at the top left of the
primary monitor, y downwards. macOS points are such units already. Windows
(per-monitor DPI aware) and X11 address their desktops in device pixels;
there each monitor keeps its device origin and its extent is divided by its
scale (`gui_forms/host/windowing/desktop_coordinates.hpp`), and
`ApplicationWindows::monitors` returns the monitors in those units, while
`HostServices::query_monitors` keeps reporting device units. On X11 a frame
is a request to the window manager, which may adjust it; whether XWayland
honours client positions is measured per compositor, not assumed.

The portable rules (identities, bounds, ownership, validation, thread and
shutdown checks) live in the `HostWindowing` base class, so every adapter
applies them the same way; the headless reference host
(`src/host/headless/application/`) implements the same seam on a simulated
desktop for tests.

## Choosing a host at build time

Enable exactly one native host per build (`GUI_FORMS_ENABLE_MACOS_HOST`,
`GUI_FORMS_ENABLE_WINDOWS_HOST`, `GUI_FORMS_ENABLE_LINUX_HOST`). The installed
`GUIForms::Application` target is that host; a consumer that needs another
platform installs the SDK built there. Renderer-free consumers link
`GUIForms::Controls` and are host independent.
