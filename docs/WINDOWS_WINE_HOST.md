# Windows and Wine Gallery host

Date: 2026-08-04

Status: **MEASURED W0-W3 proving slice; experimental 0.x host**.

GUI.Forms now builds the same retained Gallery model as a native x86-64 PE
executable. The Windows adapter owns all Win32 types and calls. Wine is the
first execution oracle; physical Windows dogfood remains a later gate.

## Implemented boundary

The private adapter in `src/host/windows/` provides:

- an ordinary resizable Win32 top-level window targeting `_WIN32_WINNT=0x0601`;
- runtime-loaded DPI-awareness calls, with a Windows 7-compatible DPI fallback;
- top-down 32-bit DIB presentation with software alpha blending;
- text through the shared ADR-004 engine: HarfBuzz shaping and FreeType
  rasterization of the bundled Portsmouth Rapids, Carlito, Cousine, Noto
  Sans CJK JP, Noto script and monochrome Noto Emoji faces (read from
  `fonts\` beside the executable, or `GUI_FORMS_FONT_DIR`), blended into the
  DIB by the shared compositor; no GDI or Uniscribe text, and tooltips use
  the same engine;
- WIC decoding of the core's already-validated PNG-only image resources;
- pointer, wheel, keyboard, UTF-16/UTF-8 committed text, native pointer capture,
  cursor, resize, scale, activation, scheduled wake, and close translation;
- the same protocol-v5 `HostSession` ordering and metrics used by headless and
  AppKit adapters; and
- an opt-in bounded `WM_COPYDATA` automation channel keyed by stable control ID.

No Win32 type appears in `include/gui_forms/`, `src/core/`, `src/controls/`,
the Gallery model, DML, or the C ABI.

## Build

Configure accepts only LLVM 22.1.x. The checked-in
`cmake/toolchains/x86_64-w64-mingw32.cmake` selects MinGW-w64 GCC, so the
configure step rejects it. A cross-build needs a toolchain file that drives
LLVM 22 `clang`/`clang++` with `--target=x86_64-w64-mingw32`, the MinGW-w64
sysroot, and static `libgcc`/`libstdc++`; the native MSYS2 CLANG64 build in
the installation guide is the supported route. Configure with
`-DGUI_FORMS_ENABLE_WINDOWS_HOST=ON -DGUI_FORMS_ENABLE_MACOS_HOST=OFF`.

The output directory contains:

- `GUI.Forms Gallery.exe`, a PE32+ x86-64 executable;
- `libgui_forms_threading.dll`, the shared threading library it imports;
- `gui_forms_windows_probe.exe`, the opt-in automation client; and
- `fonts/`, the bundled faces and their notices.

`libgui_forms_threading.dll` imports `libwinpthread-1.dll`. A cross-build does
not copy it, so place it beside the executable before running under Wine;
without it the loader fails before `main`.

## Wine smoke and instrumentation

Run the complete interaction/close gate with:

```sh
tools/run_windows_wine_smoke.sh <build-directory>
```

The runner launches the Gallery with `--automation`, waits for the window,
clicks six stable IDs (three page rows and three controls on those pages),
captures the current framebuffer, requests a structured snapshot, closes
through the real `WM_CLOSE` path, and asserts:

- all six pointer down/up pairs are handled;
- the WIC/Win32 CPU renderer is active;
- host event rejection count is zero;
- exactly one close request is observed; and
- final host state is closed and shut down.

Evidence is written to `wine-gallery-smoke.jsonl` and
`gallery-automation.bmp` in the build directory. Automation is disabled unless
the process is launched with `--automation`; commands are capped at 4096 bytes,
accept only the versioned prefix `GUI.Forms.Automation/1`, and resolve controls
through the retained stable-ID registry.

The Gallery text fallback gate additionally runs:

```sh
tools/run_windows_gallery_text_smoke.sh build-windows-x64
```

It requires actual selection of both Noto fallback faces, a non-default
astral-plane rocket glyph, successful glyph-index output, a captured framebuffer,
and a clean native close. (`GUI_FORMS_TRACE_WIN32_TEXT` was removed with
GDI text in ADR-004 phase 6.)

## Accessibility boundary

**OBSERVED:** Wine publishes the top-level custom-rendered window, but this
environment did not expose GUI.Forms child controls through the macOS
accessibility bridge. That does not prevent deterministic manipulation: the
stable-ID channel is the current test seam, and framebuffer capture supplies a
visual oracle without coordinate guessing.

**NOT YET IMPLEMENTED:** MSAA `IAccessible`, UI Automation providers,
TSF/IME composition, OLE drag/drop, occlusion notifications, and native menu
publication. Clipboard, monitor geometry, common dialogs, sound cues, and the
bounded stable-ID automation channel are implemented. The host does not
advertise the accessibility capability bit. A future accessibility round
must publish one retained semantic tree to both MSAA/UIA and the deterministic
snapshot interface; it must not create an unrelated automation-only control
model.

## Measured 2026-08-04 result

Environment: macOS 14.8 arm64, Wine devel 11.10, MinGW-w64 GCC 15.2.0.

- W0 toolchain: **PASS** — both executables are PE32+ x86-64 and execute in Wine.
- W1 portable build: **PASS** — core, controls, C ABI, Gallery model, and Win32
  host cross-compile with no renderer or AppKit dependency.
- W2 host trace: **PASS for implemented events** — 23 accepted, 0 rejected in
  the recorded smoke; pointer actions produced 4 focus transitions and 4
  activations.
- W3 Gallery: **PASS for this slice** — 49 retained controls, real PNG, scheduled
  instrument updates, deterministic framebuffer capture, clean close, exit 0.
- W4 boundary audit: **PASS for this slice** — portable headers/core/controls/
  demo are scanned for Win32 and COM types; PE imports are rejected if they
  include a GPU or managed runtime, while GDI32 and USER32 are required.
- Strict cross-build: **PASS** with GCC `-Wall -Wextra -Wpedantic -Werror`, with
  the existing aggregate designated-initializer warning explicitly disabled.
- Renderer-free native regression: **PASS**, 17/17 tests.

These are proving-slice results, not a claim of complete WinForms, UIA, or
physical Windows compatibility.
