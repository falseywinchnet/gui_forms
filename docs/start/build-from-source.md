# Build from source

[Install](install) gives the shortest route to an installed SDK. This page
covers the full build: options, dependencies, tests, the demonstration
programs, the Windows cross-build and the compiler cache that CI publishes.
All commands run from the repository root.

## Standard native build

```sh
export CC=clang CXX=clang++ OBJCXX=clang++
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=cmake/llvm22.cmake
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

Every dependency is pinned to an exact revision and fetched by a script in
`third_party/`; CMake runs the scripts when the sources are absent.

| Script | Fetches |
|---|---|
| `fetch_skia_cpu.sh` | Skia, libpng and zlib at the recorded revisions, with the PNG-only decoder patch applied |
| `build_skia_cpu.sh`, `build_skia_cpu_linux.sh`, `build_skia_cpu_windows_mingw.sh` | A CPU-only Skia with no font backend and no GPU code, enforced by an archive policy test |
| `fetch_text_stack.sh` | HarfBuzz, FreeType and SheenBidi |
| `fetch_linebreak.sh` | libunibreak |

Audio pulls miniaudio through `FetchContent` with digest checks. Nothing else
is downloaded, and no package manager is consulted at configure time.

## Tests

`ctest` runs the renderer-free core and control tests, the headless host
conformance traces, the painter and text-identity tests, the boundary audits
(no Skia, AppKit, Win32 or X11 type in a public header; no GPU symbol in any
Skia archive) and, when a native host is enabled, that platform's host tests.
The text-identity test compares rendered text against checked-in hashes; it is
the test that proves identical output across platforms.

Native host tests that open real windows run in CI on each platform. On a
development machine without a display, configure with the host option off
and the renderer-free tests still cover the retained engine.

## Windows cross-build and Wine

The checked-in MinGW-w64 toolchain file produces a PE64 gallery from a macOS or
Linux machine:

```sh
cmake -S . -B build-windows-x64 \
    -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/x86_64-w64-mingw32.cmake \
    -DGUI_FORMS_ENABLE_SKIA=OFF \
    -DGUI_FORMS_ENABLE_MACOS_HOST=OFF \
    -DGUI_FORMS_ENABLE_WINDOWS_HOST=ON \
    -DGUI_FORMS_BUILD_TESTS=OFF \
    -DCMAKE_BUILD_TYPE=Release
cmake --build build-windows-x64 --parallel
wine "build-windows-x64/GUI.Forms Gallery.exe"
```

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
