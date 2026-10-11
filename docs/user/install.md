# Install

GUI.Forms is distributed as source. You build it once with the pinned LLVM 22
toolchain, install it to a prefix, and consume that prefix from your own CMake
project through `find_package(GUIForms)`. This page gets you from an empty
machine to an installed SDK. [Building from source](../building/index) covers
every option, the tests and the demonstration programs.

## Supported platforms

| Platform | Minimum | Host | Notes |
|---|---|---|---|
| macOS | 14.0, Apple silicon (arm64) only | AppKit | Default build on a Mac |
| Windows | 10 (x64) | Win32 | Native build in MSYS2 CLANG64 |
| Linux | Ubuntu 24.04 CI targets (x64, arm64) | X11 | Opt-in; needs X11 and ATK development packages |

Intel Macs are not supported. Wayland sessions need XWayland for the X11
host. The portable core (`Core`, `Controls`, `Drawing`, `Threading`) is host
independent, but its build still requires the pinned LLVM version below.

## Prerequisites

GUI.Forms requires **LLVM 22.1.x**. The configure step fails with any other
compiler, including the system Clang, because the compiler identity is part of
the compiler-cache contract. You also need CMake 3.25 or newer, Ninja, Git,
Python 3.12 or newer and, optionally, `ccache`.

macOS
: Install the Xcode command-line tools and `brew install cmake ninja ccache llvm@22`.
  Put `$(brew --prefix llvm@22)/bin` first on `PATH`. Homebrew's prebuilt
  libc++ targets a newer OS than GUI.Forms supports, so build the matching
  runtimes once and point the toolchain file at them:

  ```sh
  python3 tools/build_macos_runtimes.py --work /tmp/llvm-runtimes \
      --prefix "$PWD/.build/toolchain/llvm-22.1.8-macos14" --jobs 4
  export GUI_FORMS_LLVM_RUNTIME="$PWD/.build/toolchain/llvm-22.1.8-macos14"
  ```

Linux (Ubuntu 24.04)
: Add the `llvm-toolchain-noble-22` repository from apt.llvm.org, then
  install `clang-22 cmake ninja-build ccache generate-ninja pkg-config
  libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxfixes-dev
  libatk1.0-dev libatk-bridge2.0-dev`. Put `/usr/lib/llvm-22/bin` first on
  `PATH`.

Windows
: Install MSYS2 and, in the CLANG64 environment, the packages listed in
  `tools/windows_toolchain.json` (clang 22.1.8, lld, cmake, ninja, ccache,
  winpthreads). Build from the CLANG64 shell.

On every platform export the compiler explicitly:

```sh
export CC=clang CXX=clang++ OBJCXX=clang++
```

## Configure, build, install

Run all commands from the repository root. Fetch the text sources before
configuring any native build; CMake stops if they are absent:

```sh
sh third_party/fetch_text_stack.sh
```

Then use the configure command for your platform.

macOS (with `GUI_FORMS_LLVM_RUNTIME` set as above):

```sh
cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=cmake/llvm22.cmake \
    -DCMAKE_INSTALL_PREFIX="$PWD/sdk"
```

Linux (build Skia with the system GN and Ninja first):

```sh
export GUI_FORMS_SYSTEM_BUILD_TOOLS=1
sh third_party/fetch_skia_cpu.sh
sh third_party/build_skia_cpu_linux.sh "$PWD/build-skia-linux"
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=cmake/llvm22.cmake \
    -DCMAKE_INSTALL_PREFIX="$PWD/sdk" \
    -DGUI_FORMS_ENABLE_LINUX_HOST=ON \
    -DGUI_FORMS_SKIA_PREBUILT=ON \
    -DGUI_FORMS_SKIA_OUT="$PWD/build-skia-linux"
```

Windows (in MSYS2 CLANG64, using the DIB renderer):

```sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=cmake/llvm22.cmake \
    -DCMAKE_INSTALL_PREFIX="$PWD/sdk" \
    -DGUI_FORMS_ENABLE_SKIA=OFF -DGUI_FORMS_ENABLE_WINDOWS_HOST=ON
```

Build, test and install the selected configuration:

```sh
cmake --build build --parallel
ctest --test-dir build --output-on-failure
cmake --install build
```

On macOS the first build fetches and builds the private CPU-only Skia;
configure does not build it. HarfBuzz and FreeType come from the explicit text
fetch above. SheenBidi is vendored; libunibreak is needed only for the optional
development text-mask profile. Linux native tests need an X11 display;
use `xvfb-run -a ctest --test-dir build --output-on-failure` on a headless runner.

Pass the absolute path to the same `cmake/llvm22.cmake` to every consumer
project. On macOS the toolchain also fixes the SDK, the arm64
architecture, the 14.0 deployment target and the bundled libc++ runtime, and a
consumer built with different settings is not binary compatible.

## What gets installed

| Path under the prefix | Content |
|---|---|
| `include/gui_forms/` | Public portable headers |
| `${CMAKE_INSTALL_LIBDIR}/` (usually `lib/`) | `GUIForms::Core`, `Controls`, `Drawing` (static), `GUIForms::Threading` (shared), `GUIForms::Application` (shared, when a native host is enabled), `GUIForms::Audio` (static, when enabled); Windows runtime DLLs go in `bin/` |
| `${CMAKE_INSTALL_LIBDIR}/cmake/GUIForms/` | `GUIFormsConfig.cmake` and exported target files |
| `share/GUIForms/fonts/` | Bundled font set and licences, installed with a native host; exported as `GUIForms_FONT_DIR` |
| `share/licenses/` | Third-party notices |

Nothing from Skia, AppKit, Win32 or X11 appears in the installed headers;
those remain private adapters inside the `Application` library.

## Verify the installation

The repository ships standalone consumer examples that use only the installed
package. Build them against your prefix:

```sh
cmake -S examples/reference -B build-examples -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_PREFIX_PATH="$PWD/sdk" \
    -DCMAKE_TOOLCHAIN_FILE="$PWD/cmake/llvm22.cmake"
cmake --build build-examples --parallel
ctest --test-dir build-examples --output-on-failure
```

Three of those tests deliberately configure a consumer that links both the
static core and `Application`; they pass only when CMake rejects the
combination. That rejection is the installed SDK working as intended. See
[Consume the SDK](consume-the-sdk) for the linking rule behind it.
