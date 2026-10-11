# Roadmap

GUI.Forms is pre-1.0. This page says what is done, what is being worked on
now, and what has to be true before the API is declared stable. It is a
summary of the planning documents in the repository's `planning/` directory,
which remain the detailed source.

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
- Three demonstration programs (Gallery, Explore preview, Correspondence) on
  macOS, Windows and Linux, and a release pipeline that builds, tests,
  packages and launch-tests each of them before publishing archives,
  installers and checksums.

## In progress

Documentation
: Reviewed descriptions for the public members the API reference still lists
  by signature only, and screenshots captured by CI rather than by hand.

Demonstration programs
: The Explore preview's remaining interaction gates, tracked in the
  demoboard's status file.

Hosts
: Input-method preedit on Windows and X11, a Windows UI Automation provider,
  outbound drag on every host, and advertising the accessibility capability
  where native publication already exists.

## Before 1.0

The API is declared stable only when all of the following hold:

- The C function tables and error model pass a breaking-change audit, and the
  C++ wrappers are RAII conveniences over them rather than a second contract.
- Two independent native consumers pass the ownership, thread and callback
  conformance tests against the installed SDK.
- A machine-readable manifest lists every control and member as supported,
  deferred or excluded, and the documentation is generated from it.
- Windows, macOS and the selected Linux host pass the same normative traces,
  including lifecycle, input order, focus and paint receipts.
- Native accessibility publication of the semantic tree exists on every
  host and is exercised by a test, not only the deterministic snapshot.
- Packaging is reproducible: licences, integrity metadata and a software bill
  of materials ship with every artefact.

## Not planned

GPU rendering of any kind; a browser engine, CSS or an HTML runtime; a .NET,
Java or Godot dependency in the native runtime; immediate-mode controls or a
perpetual redraw loop; host font discovery for control text; LCD subpixel text; ActiveX,
browser hosting, printing, MDI and obsolete Forms families; Wayland (the X11
host runs under XWayland).

## Known gaps

Outbound drag initiation; a separate Windows UIA provider (MSAA is implemented);
preedit composition on Windows and X11; native menu-bar publication;
per-monitor DPI policy as a host service beyond scale notifications;
designer serialization; preserving sparse damage regions through painting.
Linux already publishes accessibility through ATK/AT-SPI, and table cells
already align and stretch children through Dock and Anchor. The detailed
planning records may describe earlier states; check the current implementation
and tests when assessing a remaining gap.
