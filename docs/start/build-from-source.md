# Build from source

[Install](install) gives the shortest route to an installed SDK. This page
covers the full build: options, dependencies, tests, the demonstration
programs, Windows build constraints and the compiler cache that CI publishes.
All commands run from the repository root.

## Standard native build

First follow [Install](install.md#configure-build-install) to fetch the text
sources and configure for your platform. Linux requires a separately prepared
Skia archive and an enabled X11 host; Windows uses `GUI_FORMS_ENABLE_SKIA=OFF`.
Then:

```sh
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

On macOS the result includes `build/GUI.Forms Gallery.app`; open it with
`open "build/GUI.Forms Gallery.app"`. On Windows the executable is
`build/GUI.Forms Gallery.exe` with a `fonts/` directory beside it. On Linux
the gallery needs `-DGUI_FORMS_ENABLE_LINUX_HOST=ON` and an X11 display
(`xvfb-run` works for tests).

## Options

| Option | Default | Effect |
|---|---|---|
| `GUI_FORMS_BUILD_GALLERY` | ON when top level | Build the control gallery demonstration |
| `GUI_FORMS_BUILD_TESTS` | ON when top level | Build the automated tests |
| `GUI_FORMS_ENABLE_SKIA` | ON | Build the private CPU-only Skia painter (macOS and Linux) |
| `GUI_FORMS_ENABLE_MACOS_HOST` | ON on macOS | Build the AppKit host and `GUIForms::Application` |
| `GUI_FORMS_ENABLE_WINDOWS_HOST` | ON on Windows | Build the Win32 DIB host |
| `GUI_FORMS_ENABLE_LINUX_HOST` | OFF | Build the X11 host; requires Linux and Skia |
| `GUI_FORMS_ENABLE_HARFBUZZ_TEXT` | ON with Skia or Win32 | Build the pinned HarfBuzz/FreeType text engine |
| `GUI_FORMS_BUILD_AUDIO` | OFF | Build `GUIForms::Audio` |
| `GUI_FORMS_AUDIO_VORBIS_BACKEND` | `stx` | Ogg decoder: `stx` or `stb` |
| `GUI_FORMS_BUILD_FILE_MANAGER_DEMOBOARD` | OFF | Build the fixture-backed explorer consumer |
| `GUI_FORMS_BUILD_BENCHMARKS`, `GUI_FORMS_BUILD_FUZZERS` | OFF | Measurement harnesses and libFuzzer entry points |

The remaining `GUI_FORMS_BUILD_TEXT_*` and `GUI_FORMS_BUILD_PREPARED_TEXT` /
`GUI_FORMS_BUILD_TEXT_MASKS` options build development-only text services and
probes. They are not installed.

## Third-party dependencies

Dependencies are pinned. Fetch HarfBuzz and FreeType explicitly before
configuration. CMake schedules the default Skia fetch/build at build time;
Linux native builds use the platform-specific script and prebuilt options
shown in [Install](install). SheenBidi is already vendored in the repository.

| Script | Purpose |
|---|---|
| `fetch_skia_cpu.sh` | Fetch Skia and its pinned libpng, zlib and FreeType sources, with the PNG-only decoder patch applied |
| `build_skia_cpu.sh`, `build_skia_cpu_linux.sh` | Build the fetched CPU-only Skia for macOS or Linux; they do not fetch sources |
| `fetch_text_stack.sh` | HarfBuzz and FreeType |
| `fetch_linebreak.sh` | libunibreak, required before configuring `GUI_FORMS_BUILD_TEXT_MASKS=ON` |

Audio pulls miniaudio through `FetchContent` with digest checks. No package
manager is consulted at configure time.

## Tests

`ctest` runs the renderer-free core and control tests, the headless host
conformance traces, the painter and text-identity tests, the boundary audits
(no Skia, AppKit, Win32 or X11 type in a public header; no GPU symbol in any
Skia archive) and, when a native host is enabled, that platform's host tests.
The text-identity test compares rendered text against checked-in hashes; it is
the regression check for the covered corpus and rendering profile, not an
exhaustive proof for all text.

Native host tests that open real windows run in CI on each platform. On a
development machine without a display, configure with the host option off
and the renderer-free tests still cover the retained engine.

## Windows builds and Wine

Use the native MSYS2 CLANG64 instructions in [Install](install). The older
`cmake/toolchains/x86_64-w64-mingw32.cmake` selects GCC and is rejected by
the current LLVM 22.1.x configure check. It is not a working cross-build
recipe for the current tree.

`tools/run_windows_wine_smoke.sh <build-dir>` launches the gallery with
`--automation`, activates controls by stable ID, captures the framebuffer,
closes through the real `WM_CLOSE` path and asserts on the structured
snapshot. The native Windows build in MSYS2 uses the same host sources with
`cmake/llvm22.cmake`.

## Install prefix and SDK layout

`cmake --install build` installs headers, libraries, the CMake package and the
font directory under `CMAKE_INSTALL_PREFIX`. The exact contents are listed in
[Install](install.md#what-gets-installed). CI installs into a scratch prefix and
then builds `examples/reference/` against it, so the installed package is
tested on every platform, not only the build tree.

## Compiler cache

CI compiles through `ccache` with `CCACHE_COMPILERCHECK=content` and, after
tests pass, publishes a cache archive per platform to a `build-<sha>`
pre-release. Those releases are build inputs, not application releases.
A consumer that restores one still compiles and tests its own code; a cache
miss is an ordinary compilation. The archive layout, manifest schema and the
macOS runtime payload are described in `docs/COMPILER_CACHE.md` in the
repository.

## Dependency boundaries

The build enforces the project's boundaries as tests rather than as
conventions: C++20 everywhere, Objective-C++ only in `src/host/macos/`, Win32
only in `src/host/windows/`, X11 only in `src/host/linux/`, Skia only in
`src/render/skia/`, PNG as the only image decoder, and no GPU API anywhere.
A change that violates one of these fails `ctest` before it reaches review.
