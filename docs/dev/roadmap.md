# Roadmap

GUI.Forms is pre-1.0. This page says what is done, what is being worked on
now, and what has to be true before the API is declared stable. It is a
summary of the planning documents in the repository's `planning/` directory,
which remain the detailed source; where they disagree with the code, the code
and its tests are the authority.

```{image} ../_static/roadmap.svg
:alt: Releases toward 1.0: 0.1 Shipped, 0.2 Verified, 0.3 Themeable, 0.4 Flexible, 0.5 ABI candidate, 0.6 Bindings, 0.7 Designers, then 1.0 Stable.
:width: 100%
:class: roadmap-figure
```

## 0.1 Shipped

What GUI.Forms is today, tested in CI on macOS, Windows and Linux. The
details are under [Done and tested](#done-and-tested).

- A retained core with explicit ownership, typed dirtiness and nested update
  scopes, painted on the CPU only where something changed.
- One text engine with identical output on every platform.
- More than 70 stock Forms controls, with menus, commands and data binding.
- Native hosts for macOS, Windows and X11, which also runs under XWayland.
- An installed CMake SDK with fast, reproducible cached builds.

## 0.2 Verified

- **Accessibility verified on every host.** Tests read each platform's own
  accessibility API; Windows gains a UI Automation provider and text fields
  expose editable text ranges; then the capability is advertised.
- **Font substitution.** Families that applications ask for map to bundled
  faces with matching metrics.
- **Articulated controls.** Every control is built from named parts with
  states and public geometry, described by one schema that the C++ API, form
  documents, themes and the visual tools share.

## 0.3 Themeable

- **A theme engine.** Themes are data: properties, layered effects,
  transitions and state machines, and bounded expressions with pointer
  position and application values as inputs. New kinds of effect are added to
  the engine in C++. An idle window still draws nothing.
- **CSS-syntax themes** addressed to the named parts.
- **The default theme**, in light and dark, following the
  design-language specification (`planning/DESIGN_LANGUAGE_SPECIFICATION.md`).
- **Windows-familiar themes** beside it.
- **Compositor cooperation:** transparency, glass and behind-window materials
  where the host provides them.

## 0.4 Flexible

- **Flex and grid layout** containers, checked against a browser's layout.
- **Form documents that round-trip:** hand edits and visual edits never
  overwrite each other.
- **Damage-preserving painting:** each damaged region is painted and presented
  on its own, not as one bounding box.
- **More Windows Forms controls:** a data grid, a list view, and complete tool
  and status strips, built on the theme engine.

## 0.5 ABI candidate

- **An installed, versioned C ABI** with an error model.
- **A breaking-change audit** of its function tables, with the C++ wrappers as
  conveniences over them rather than a second contract.
- **Two independent native consumers** passing the ownership, thread and
  callback conformance tests against the installed SDK.

## 0.6 Bindings

- **C# bindings** over the C ABI with a Windows Forms-shaped API, so a Windows
  Forms project can switch to GUI.Forms and run on macOS and Linux.
- **The rest of the Windows Forms controls** that .NET 8 and later support.
  Controls that exist only in .NET Framework, or that .NET removed, are out
  of scope.

## 0.7 Designers

- **An interface builder** written in GUI.Forms, whose documents can also be
  edited by hand without anything being overwritten.
- **HTML import and a layout designer** for static HTML and CSS.
- **A theme designer.**

## 1.0 Stable

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

The full plan, with a measured gate for each stage, is
`planning/FORWARD_PROGRAMME.md`.

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
| Native accessibility on every host | Publication of the semantic tree through each platform's accessibility API, exercised by tests | in progress: macOS, Windows (MSAA and UI Automation) and Linux (ATK/AT-SPI) publish; UI Automation and ATK publish editable text ranges (TextPattern, AtkText); no host advertises the capability |
| Themes, assets and localization | Immutable theme records with named visual roles and high-contrast recipes, atomic theme replacement, layered surface materials, OKLab/OKLCH colour and a fixed bundled font set | in progress: data-only theme and language packs, message catalogues and runtime configuration are not implemented |
| Authoring manifest for compiled forms | A versioned manifest of controls, properties, events, layout and resources that form compilers consume, with conformance fixtures | in progress: the control registry lists every public control type; member-level entries and conformance fixtures are open |
| Stable C ABI | Versioned function tables and an error model that pass a breaking-change audit, an export allowlist, and C++ wrappers as RAII conveniences over them | planned; an experimental 0.x C ABI is built and tested but not installed |
| Windows and Linux hosts | The same protocol on Win32 (DIB presentation, MSAA, dialogs, clipboard, automation channel) and X11 (XDND drops, ATK/AT-SPI, dialogs, clipboard) | in progress: Windows lacks preedit, OLE drag and drop and occlusion; X11 lacks preedit and run-time scale notifications |
| Packaging and release | A pipeline that builds, tests, packages and launch-tests every program on every platform, then publishes archives, installers, checksums and licence notices | in progress: release signing and notarization, a software bill of materials and reproducible builds remain |
| Coherent latest-state presentation | Separate content, rendered and presented revisions; one paint lease, one coalesced wake and one deferred follow-up per window; exact presentation receipts; owner-callback failure restores the previous chunks | done |
| Pressure, input and stall conformance | Under 100 mutations per frame, nested paints, resize during replay, occlusion, backend faults and owner retirement: one queued drain, latest-state rendering and a return to idle; input deferred outside the paint lease with a fixed bound | in progress: the deterministic gates pass; a native wall-clock soak remains |
| Atomic candidate raster | Replay into a private candidate raster and publish it only after every backend command succeeds, keeping the last presented raster across allocation, renderer and upload faults | in progress: an opt-in Windows transactional frame store exists; default hosts replay into the live raster |
| Damage-preserving painting | Painting and presenting each damage rectangle instead of their bounding box | in progress: every host paints and presents the rectangle set (paint contract version 2), and live frames follow one frame sequence; the gate is recorded on Windows, macOS and X11 records remain |
| Presentation availability record | Publishing the mandatory paint-contract version and optional host extensions (partial present, zero-copy import, presentation fences) | done: `Window::presentation_availability()` reports contract version 2 (partial present is now part of the contract), the extensions and the live paint lease; no host offers zero-copy import or fences yet |
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
: Input-method preedit on Windows and X11, the macOS accessibility text
  attributes, drag and drop on Windows, outbound drag on every host, and advertising the
  accessibility capability where native publication already exists.

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
the macOS accessibility text attributes (UI Automation's TextPattern and ATK's AtkText are implemented); preedit composition on
Windows and X11; occlusion notifications on Windows and run-time scale
notifications on X11; native menu-bar publication; per-monitor DPI policy as
a host service beyond scale notifications; designer serialization; preserving
sparse damage regions through painting. Linux already publishes accessibility through ATK/AT-SPI, and
table cells already align and stretch children through Dock and Anchor. The
detailed planning records may describe earlier states; check the current
implementation and tests when assessing a remaining gap.
