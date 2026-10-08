# Source consumption and compiler-cache artifacts

Consumers pin this repository's source and build it normally through ccache.
CI checks out the toolkit at `gui_forms/` below the build workspace and builds
under `.build/native-<platform>/gui-forms`. Consumers use the same relative
layout, compiler/runtime, build options and ccache compiler-content check.
The native workflow emits a platform-specific cache archive only after tests
pass. Cache snapshots accelerate compilation; they do not replace consumer tests.

Restore the archive below the consumer workspace, set `CCACHE_DIR` to `.ccache`,
`CCACHE_BASEDIR` to the workspace, `CCACHE_COMPILERCHECK=content`, and all three
CMake C/C++/Objective-C++ compiler launchers to `ccache`. A miss compiles the
source normally. Do not weaken header, compiler or option validation for hits.
Caches are bounded at 500 MB. The macOS archive additionally carries the matched
LLVM runtime headers and libraries; only the three dylibs enter an application.
Runtime restoration verifies the compiler binary, SDK, configured headers,
dylib bytes and symlink targets against the producer manifest. A mismatch fails
closed; rebuild the pinned runtime payload or restore its matching release.

The portable proof in `tools/prove_compiler_cache.py` uses a clean source
snapshot and a new scratch directory. It builds the real retained-control
event test in different producer/consumer locations, requires identical final
executable bytes, and checks implementation/header/compiler-option invalidation.
It measures the fixture only; native application/package evidence is separate.

The owner approved independent source/cache publication on 2026-10-07.
History was extracted from `falseywinchnet/file_manager` at
`0dc39f989f2d077d5485027dba9046f884e43bc7`; its `gui_forms/` subtree became this
repository root. First-party changes follow `planning/PROGRAMMING_HOUSE_STYLE.md`.
ThinLTO and static Application
linking remain separate, unmeasured candidates, not consequences of caching.

The macOS and Linux Skia GN builds also use `cc_wrapper = "ccache"` when
the CMake C++ launcher is ccache. Provider main builds publish all four tested
cache archives as a `build-<full-source-SHA>` prerelease. File Manager's dependency
lock pins that release and each archive digest; cache absence falls back to source
compilation. No arbitrary release archive is treated as a cache entry.

Local M4 proof receipt: `experiments/compiler-cache/macos-m4.json` (cold 67.63 s,
relocated warm 1.25 s, 176 reused compilations, identical 1,672,184-byte executable).
The retained rejected trial used an unused preprocessor definition as its supposed
invalidating option. Ccache correctly reused equivalent preprocessed source; the
accepted proof changes `-fno-inline-functions` instead. These are fixture timings.

`docs/FILE_MANAGER_COMMIT_MAP.txt` maps original commits to this filtered history.

## Consumer linking rule

`GUIForms::Threading` is a small shared library containing Worker,
CancellationFlag and the atomic pthread pool. `GUIForms::Audio` stays static and
links Threading publicly, **without Core**. Application and static Core also
link the same Threading library. A component adapter therefore needs only:

```cmake
find_package(GUIForms CONFIG REQUIRED COMPONENTS Audio Application)
add_library(audio_adapter STATIC audio_adapter.cpp)
target_link_libraries(audio_adapter PRIVATE GUIForms::Audio)
target_link_libraries(audio_tests PRIVATE audio_adapter)
target_link_libraries(my_application PRIVATE GUIForms::Application audio_adapter)
```

For workers alone, request the Threading component and link
`GUIForms::Threading`. For a renderer-free retained-tree consumer, use static
Core/Controls/Drawing. For a native Application consumer, use Application:
**never also link static Core, Controls or Drawing**, including through an
adapter. Application already contains their implementation. CMake rejects this
mixed ownership during generation, including private static-adapter and
interface-adapter dependencies. A private static adapter produces a generation
diagnostic naming `ERROR_do_not_link_static_Core_with_Application`; this is an
intentional rejection, not a missing SDK component. Raw archive filenames or
hand-written linker commands bypass the CMake check.

Ship the matching Threading shared library alongside GUI.Forms: on macOS,
`libgui_forms_threading.0.dylib` in the app's Frameworks directory; on Linux,
`libgui_forms_threading.so.0` in the application's runtime library directory;
on Windows, `libgui_forms_threading.dll` beside the executable (plus the matching
winpthreads/compiler runtimes). Use the same copy for Audio and Application.
The macOS rpath/signing instructions below apply to Threading too. The installed
reference fixtures stage Windows runtime DLLs using CMake's transitive target
list. Rebuild consumers against matching headers and libraries when upgrading.

The installed examples test Threading alone, an Audio-only static adapter, the
same adapter with Application, and three rejected static-Core combinations.
This fixes Worker ownership without changing Audio's existing static packaging;
applications should still share their one audio service across their components.

## LLVM 22 matching contract

**GIVEN (2026-10-07):** LLVM 22.1.x, Apple silicon/macOS 14.0 and Windows 10.
The native workflow currently requires **22.1.8** and records both full version
text and a SHA-256 of the compiler executable in schema 2 `cache-manifest.json`.
A changed package must be explicitly admitted, not silently relabelled as the
same compiler. Equal version strings alone do not establish cache compatibility.

| Target | Runner image | Compiler distribution | Minimum |
|---|---|---|---|
| macOS arm64 | `macos-15` | Homebrew `llvm@22`, 22.1.8 | macOS 14.0, arm64 only |
| Linux x64 | `ubuntu-24.04` | apt.llvm.org `llvm-toolchain-noble-22`, clang-22 | Ubuntu 24.04 system ABI |
| Linux arm64 | `ubuntu-24.04-arm` | same LLVM 22 release series | Ubuntu 24.04 system ABI |
| Windows x64 | `windows-2022` | MSYS2 CLANG64 clang 22.1.8 | Windows 10 (`0x0A00`), MSYS2 winpthreads |

Put the selected compiler's `bin` first in PATH. Set `CC=clang`, `CXX=clang++`,
`OBJCXX=clang++` explicitly. In particular, Windows `cc.exe`/`c++.exe` are not
interchangeable compiler identities for this contract. Pass
`-DCMAKE_TOOLCHAIN_FILE=<workspace>/gui_forms/cmake/llvm22.cmake` to both toolkit
and consumer CMake configurations. Use a fresh build directory when changing
compilers; cached CMake compiler selections do not follow a changed PATH.

Restore below the workspace root, with source **`gui_forms/`** and toolkit output
**`.build/native-<platform>/gui-forms/`** (three levels down). Set:

```sh
export CC=clang CXX=clang++ OBJCXX=clang++
export CCACHE_DIR="$PWD/.ccache" CCACHE_BASEDIR="$PWD"
export CCACHE_COMPILERCHECK=content
export CMAKE_C_COMPILER_LAUNCHER=ccache
export CMAKE_CXX_COMPILER_LAUNCHER=ccache
export CMAKE_OBJCXX_COMPILER_LAUNCHER=ccache
```

Match Release, C++20, PIC, toolkit options, source revision and all generated
headers. The exact runner image **revision**, SDK version, compiler text/hash,
ccache version, minimum and layout are in the manifest. A runner label alone
does not pin SDK headers: GitHub updates images. MSYS2 and the LLVM release
package channel can also rebuild a version. Such changes may legitimately miss.
The LLVM repository is the numbered **22 release** channel, not the unnumbered
nightly channel; its full package compiler identity remains recorded. Consumers
verify the release/archive digest and source revision before restoration. Do not
set sloppy system-header or compiler checks to manufacture hits.

### macOS runtime and packaging

Homebrew's compiler is a build-host tool. Its prebuilt libc++ may require an OS
newer than the application's floor (the local 22.1.8 bottle declares 26.0).
`tools/build_macos_runtimes.py` builds libc++, libc++abi and libunwind from the
SHA-256-pinned LLVM 22.1.8 release source for **arm64 / 14.0**. It installs matching
headers, including `__config_site`, with vendor OS availability annotations off:
the application ships these runtimes, rather than assuming Apple's system C++ ABI
has newer functions. This does not disable AppKit or other OS availability checks.

The macOS cache archive contains `.build/toolchain/llvm-22.1.8-macos14/`.
Set `GUI_FORMS_LLVM_RUNTIME` to that restored directory. The toolchain supplies:

```text
CMAKE_OSX_ARCHITECTURES=arm64
CMAKE_OSX_DEPLOYMENT_TARGET=14.0
-stdlib=libc++ -nostdinc++ -isystem <runtime>/include/c++/v1
-L<runtime>/lib -Wl,-rpath,<runtime>/lib -lunwind
```

Skia uses the same compiler, runtime headers, SDK and 14.0 target. Do not mix
Homebrew's configured headers or Apple's libc++ objects with this runtime profile.
Copy `libc++.1.dylib`, `libc++abi.1.dylib`, `libunwind.1.dylib` from its `lib/` into
`Your.app/Contents/Frameworks`, and retain their runtime license notices. Their
install IDs and mutual dependencies use `@rpath`. Give each executable and shipped
dylib the appropriate `@executable_path/../Frameworks` or `@loader_path` rpath;
remove the build-workspace runtime rpath before signing. Sign the copied libraries
before signing the enclosing app. Never copy a newer-minimum Homebrew dylib and
change only its load-command version.

Use these per-image rpaths, not a global `DYLD_LIBRARY_PATH` override: Apple's
system frameworks must continue to load their own system C++ runtime. CI extracts
the actual cache archive into a fresh directory, verifies its manifest and minimums,
rewrites a native threading test's rpath, signs it and requires successful execution
with all three restored dylibs loaded before uploading the archive.

Run `python tools/audit_macos_minimum.py <bundle-or-library-directory>` to reject
non-arm64 images, minimums above 14.0, and absolute/system libc++ dependencies.
The provider audits runtimes, its installed SDK and reference executables.
Actual execution on macOS 14 still requires a macOS 14 machine; a newer runner
plus availability diagnostics and Mach-O checks is not that runtime test.

Each passing main build publishes **four** archives in `build-<full-SHA>`.
The relocated cache proof checks source relocation plus implementation, header
and compiler-option invalidation; it is not a promise that another distribution's
clang, SDK, or flags will hit. Consumers still build and test their applications.

## Reusing CI work

The native workflow now separates three kinds of reuse:

- **Object cache:** ccache still verifies compiler contents, headers and options.
  A platform-level fallback can restore objects from an earlier compiler/runner
  cache; ccache decides which entries remain valid. This does not weaken its checks.
- **Cache-mechanism proof:** the expensive cold/relocated/invalidation experiment
  has a receipt keyed by compiler/build environment and the proof, workflow and
  CMake contract. Ordinary implementation edits do not repeat this experiment;
  they still run native compilation and tests. `force_cache_proof` repeats it.
- **Native validation:** an identical complete Git tree, platform, compiler, SDK,
  runner image, build-tool versions and package inventory can reuse a successful
  native job's receipt and cache archive. This permits PR-to-main reuse despite
  their different commit IDs and GitHub's branch-scoped object caches. A changed
  tree or environment runs native tests again. `force_validation` bypasses reuse.

Receipts come only from this repository's native workflow and a successful native
job. The producer's actual Git tree is checked through GitHub's API, as well as
receipt fields, artifact ownership, artifact digest, cache-archive digest and
manifest identity. Fork-produced artifacts are excluded. Missing, expired or
rejected receipts fall back to fresh validation. A bad downloaded payload fails
validation. Later runs wait up to 30 minutes for an older matching job; they never
wait on newer runs or themselves. After timeout or failure they build normally.

Reuse is reported explicitly in the job summary and `cache-manifest.json`, with
original validation run/revision provenance. The archive manifest is regenerated
for the current commit; the object cache and matched LLVM runtime are retained.
All four archives are still published by a successful main workflow. Artifact
retention is 30 days, so eviction is a normal cache miss, not a build dependency.
There is no promise of zero CI startup, transfer or packaging time.

Windows now caches the fetched text-stack and audio dependencies too. The native
archive remains a compilation accelerator, not a reusable consumer test result.

## Choosing the Ogg decoder

`GUI_FORMS_AUDIO_VORBIS_BACKEND` is the single producer build switch:

```sh
cmake -S gui_forms -B .build/native-macos-arm64/gui-forms -DGUI_FORMS_BUILD_AUDIO=ON -DGUI_FORMS_AUDIO_VORBIS_BACKEND=stx
```

The default is `stx`, the decoder from `stx_vorbis/`. Fresh build directories
select it without an override; CI explicitly selects it when reusing a cache.
Existing CMake caches retain their selected value: configure with
`-DGUI_FORMS_AUDIO_VORBIS_BACKEND=stx` to change an older build. The explicit
fallback remains `-DGUI_FORMS_AUDIO_VORBIS_BACKEND=stb`. Rebuild and reinstall the
SDK after changing it. Consumers continue linking `GUIForms::Audio`: an stx SDK
installs its decoder/compatibility archives and resolves those dependencies in
`find_package(GUIForms)`. No consumer API or extra manual library list is needed.
Only the selected decoder is linked; miniaudio remains the audio-device backend.
The existing cancellation, bounded arena, clip quota and publication tests run
against both choices, together with the installed Worker/Ogg examples in CI.
The internal stb seek-boundary test applies only to stb; stx's own seek tests live
in its standalone suite. CI builds and installs the default stx SDK, exercises
stb as the alternate, then restores stx for subsequent checks and publication.
This default selects the shipping stx decoder; it does not enable the separate
experimental BFFT/BODFT transform adapter.
