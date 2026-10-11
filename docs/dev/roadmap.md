# Roadmap

GUI.Forms is pre-1.0. This page says what is done, what is being worked on
now, and what has to be true before the API is declared stable. It is a
summary of the planning documents in the repository's `planning/` directory,
which remain the detailed source; where they disagree with the code, the code
and its tests are the authority.

## Milestones

The implementation programme is a chain of independently testable contracts.
The table lists them in dependency order, with the paint-pipeline closure
gates as their own rows. "Done" means the deliverable exists and a test in
`tests/` exercises it; a row's open remainder is named in its status.

| Milestone | What it delivers | Status |
|---|---|---|
| Retained kernel and headless reference host | Component ownership apart from the visual tree, stable identity, transactional attach and detach, tokenized events, UI-thread enforcement, typed dirtiness, and a deterministic headless host whose trace is the ordering reference | done |
| Display chunks, damage and the CPU renderer | Retained display chunks, bounded multi-rectangle damage per paint plane, a bounded PNG registry with a fuzz target, the renderer-free drawing core (paths, gradients, transforms, clips, images), and the private CPU-only Skia adapter behind symbol audits | done |
| Portable host protocol and the macOS host | A versioned, capability-reported protocol; on AppKit: lifecycle and close cancellation, monitors, occlusion, capture, cursors, clipboard text and images, inbound typed drops, the five common dialogs, sound cues and IME composition | done; outbound drag and native menus are open |
| Unicode text, shaping and editing | A strict UTF-8 text store with distinct position types, Unicode grapheme boundaries, HarfBuzz/FreeType shaping over bundled faces with script fallback, bidirectional analysis, and single and multiline editing with selection, clipboard and undo | in progress: preedit on Windows and X11 and accessible editable text ranges remain |
| Layout, scrolling and virtualization | Measure and arrange with Dock/Anchor, flow, table, split and responsive-track containers; scrollable containers with real scrollbars; read barriers and bounded passes; lists and trees that realize only visible rows | in progress: million-item list, tree and grid fixtures are not in place |
| Basic Forms controls | Panel, GroupBox, Label, LinkLabel, Button, CheckBox, RadioButton, TextBox, ListBox, ComboBox, NumericUpDown, scroll bars, TrackBar, ProgressBar, PictureBox, ContainerControl, UserControl, Timer, ToolTip, ErrorProvider, HelpProvider, ImageList and commands, with initialization scopes, mnemonics and accept/cancel targets | done |
| Collection, command and data controls | TreeView, ObjectView, CheckedListBox, TabControl, SplitContainer, MasterDetailView, MenuStrip, ContextMenu, CommandBar, DateTimePicker and a metadata-driven PropertyGrid; `Value<T>` and `Command` bindings with record currency and validation | in progress: a data grid and a status strip are not implemented |
| Semantic tree | A retained semantic graph with stable-ID snapshots, default adapters for stock controls, virtual children and semantic actions, independent of authored metadata | done |
| Native accessibility on every host | Publication of the semantic tree through each platform's accessibility API, exercised by tests | in progress: macOS, Windows (MSAA) and Linux (ATK/AT-SPI) publish; only Windows has a registered test; no host advertises the capability; no UI Automation provider |
| Themes, assets and localization | Immutable theme records with named visual roles and high-contrast recipes, atomic theme replacement, layered surface materials, OKLab/OKLCH colour and a fixed bundled font set | in progress: data-only theme and language packs, message catalogues and runtime configuration are not implemented |
| Authoring manifest for compiled forms | A versioned manifest of controls, properties, events, layout and resources that form compilers consume, with conformance fixtures | in progress: the control registry lists every public control type; member-level entries and conformance fixtures are open |
| Stable C ABI | Versioned function tables and an error model that pass a breaking-change audit, an export allowlist, and C++ wrappers as RAII conveniences over them | planned; an experimental 0.x C ABI is built and tested but not installed |
| Windows and Linux hosts | The same protocol on Win32 (DIB presentation, MSAA, dialogs, clipboard, automation channel) and X11 (XDND drops, ATK/AT-SPI, dialogs, clipboard) | in progress: Windows lacks preedit, OLE drag and drop, occlusion and UI Automation; X11 lacks preedit and run-time scale notifications |
| Packaging and release | A pipeline that builds, tests, packages and launch-tests every program on every platform, then publishes archives, installers, checksums and licence notices | in progress: release signing and notarization, a software bill of materials and reproducible builds remain |
| Coherent latest-state presentation | Separate content, rendered and presented revisions; one paint lease, one coalesced wake and one deferred follow-up per window; exact presentation receipts; owner-callback failure restores the previous chunks | done |
| Pressure, input and stall conformance | Under 100 mutations per frame, nested paints, resize during replay, occlusion, backend faults and owner retirement: one queued drain, latest-state rendering and a return to idle; input deferred outside the paint lease with a fixed bound | in progress: the deterministic gates pass; a native wall-clock soak remains |
| Atomic candidate raster | Replay into a private candidate raster and publish it only after every backend command succeeds, keeping the last presented raster across allocation, renderer and upload faults | in progress: an opt-in Windows transactional frame store exists; default hosts replay into the live raster |
| Damage-preserving painting | Painting and presenting each damage rectangle instead of their bounding box | planned |
| Presentation availability record | Publishing the mandatory paint-contract version and optional host extensions (partial present, zero-copy import, presentation fences) | planned |
| API stability (1.0) | The conditions listed under "Before 1.0" below | planned |

## Done and tested

- Retained tree with explicit ownership, stable identity, transactional
  attachment and synchronous disposal.
- Typed dirtiness, nested update scopes, bounded flushes, read barriers.
- Damage-limited CPU painting with coherent paint leases, exact presentation
  receipts, paint planes, live surfaces and per-frame counters.
- One text engine (HarfBuzz shaping, FreeType light auto-hinting, shared
  sRGB blending) with identical output on macOS, Windows and Linux, enforced
  by a hash-compared identity test. Skia is built without any font backend.
- Forms-like focus, Tab order, focus scopes, mnemonics, accept/cancel
  targets, validation, pointer capture, deferred input during paint.
- One UI thread per window, a bounded FIFO dispatcher, synchronous `invoke`,
  timers, cooperative workers and cancellation.
- A portable host protocol with capability discovery; AppKit, Win32 and X11
  adapters plus a deterministic headless adapter that defines the reference
  trace.
- Stock controls: panels and groups, labels and links, buttons, check and
  radio, text boxes (single and multiline with scrollbars), list, checked
  list, combo, tree, tab control, track bar, progress bar, numeric up/down,
  date/time picker, property grid, picture box, image list, menus and context
  menus, tooltips, error and help providers, split and master/detail
  containers, flow and table layout, command bar, choice group, expandable
  sections, breadcrumb trail, object and correspondence views, raster
  canvas, drawing surface and diagnostic views.
- Application-owned `Value<T>` and `Command` state with two-way control
  bindings, record currency and binding-aware validation.
- An installed CMake SDK consumed by standalone examples on every CI platform,
  with generation-time rejection of unsafe linking combinations.
- A compiler-cache pipeline that publishes tested object caches per platform.
- This documentation site, with control indexes and an API reference
  generated from the control registry and the public headers.
- Three demonstration programs (the Gallery and the Explore and
  Correspondence previews) on macOS, Windows and Linux, and a release
  pipeline that builds, tests, packages and launch-tests each of them before
  publishing archives, installers and checksums.

## In progress

Documentation
: Reviewed descriptions for the public members the API reference still lists
  by signature only, and screenshots captured by CI rather than by hand.

Demonstration programs
: The Explore preview's remaining interaction gates, tracked in that
  program's status file.

Hosts
: Input-method preedit on Windows and X11, a Windows UI Automation provider,
  drag and drop on Windows, outbound drag on every host, and advertising the
  accessibility capability where native publication already exists.

## Before 1.0

The API is declared stable only when all of the following hold:

- The C function tables and error model pass a breaking-change audit, and the
  C++ wrappers are RAII conveniences over them rather than a second contract.
- Two independent native consumers pass the ownership, thread and callback
  conformance tests against the installed SDK.
- A machine-readable manifest lists every control and member as supported,
  deferred or excluded, and the documentation is generated from it.
- Windows, macOS and the X11 host pass the same normative traces,
  including lifecycle, input order, focus and paint receipts.
- Native accessibility publication of the semantic tree exists on every
  host and is exercised by a test, not only the deterministic snapshot.
- Packaging is reproducible: licences, integrity metadata and a software bill
  of materials ship with every artefact.

A design-language and theming model, in which controls are nested regions a
style can address part by part, is proposed but not implemented
(`planning/DESIGN_LANGUAGE_SYSTEM_MODEL.md`,
`planning/CONTROL_DNA_AND_STYLE_ADDRESSABILITY.md`,
`planning/AUTHORITATIVE_CONTROL_REGIONS.md`).

## Not planned

GPU rendering of any kind; a browser engine, CSS or an HTML runtime; a .NET,
Java or Godot dependency in the native runtime; immediate-mode controls or a
perpetual redraw loop; host font discovery for control text; LCD subpixel
text; ActiveX, browser hosting, printing, MDI and obsolete Forms families.

## Linux and other Unix desktops

GUI.Forms aims to be agnostic about the display server and to target what
already works everywhere. Today that is the X11 host: it runs on X11 desktops
and, through XWayland, in Wayland sessions, with the same behaviour. A
Wayland-native adapter is not planned. FreeBSD is a desired target; it has
not yet been built or tested.

## Known gaps

Outbound drag initiation on every host, and inbound drag and drop on Windows;
a separate Windows UIA provider (MSAA is implemented); preedit composition on
Windows and X11; occlusion notifications on Windows and run-time scale
notifications on X11; native menu-bar publication; per-monitor DPI policy as
a host service beyond scale notifications; designer serialization; preserving
sparse damage regions through painting. Linux already publishes accessibility through ATK/AT-SPI, and
table cells already align and stretch children through Dock and Anchor. The
detailed planning records may describe earlier states; check the current
implementation and tests when assessing a remaining gap.
