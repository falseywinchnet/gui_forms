# LLVM 22 platform validation — 2026-10-07

**GIVEN:** one LLVM 22.1.x compiler series; explicit platform floors; four native
cache archives; the owner's `threadpool_atomic_fast` as the threading reference.
The owner permits building the three macOS runtimes provided they are reliably
cached and consumers receive prebuilt payloads. Full LLVM/Clang compilation is
not part of this workflow.

## Local measurements

**MEASURED:** M4 Mac mini, macOS 26.5 (25F71), 60 Hz screen, scale 1.0,
1060×618 opaque RGBA live surface. Apple Clang 21 baseline at merged source
`2cb2fc53bb26ff6c6cb2a856a60b0a6b9c854558`; LLVM 22.1.8 builds use arm64/macOS
14.0, their own libc++ headers/runtimes and freshly rebuilt Skia. The old host
was instrumented before changing its display link. Raw summaries and CSVs are
under `experiments/llvm22-platform-contract/`.

The native fixture fills and publishes frames from a 4 ms main-queue producer.
Process CPU includes that producer, AppKit and instrumentation; it is not an
isolated renderer cost. One-second settling separates active, attached-idle,
hidden and resumed phases. Callback timestamps are not scanout timestamps.
No ProMotion or multi-display migration performance was measured on this 60 Hz
screen; the view-bound API supplies screen tracking and default refresh policy.

| Build / host | Active median / p95 callback interval | Attached idle ticks | Attached idle process CPU |
|---|---:|---:|---:|
| Apple Clang / CVDisplayLink | 16.67 / 18.10 ms | 189 in 3.15 s | 0.445% |
| LLVM 22 / CVDisplayLink | 16.67 / 18.05 ms | 189 in 3.15 s | 0.523% |
| LLVM 22 / CADisplayLink, final | 16.67 / 16.68 ms | 0 in 3.15 s | 0.012% |

The final static-overlay run also had zero idle callbacks (0.018% process CPU).
All hidden intervals had zero display callbacks. The new host resumed 120 ticks
in 2.00 seconds after showing the window. “Idle zero” means no periodic display
callbacks or paint, not literally zero measured process CPU.

The first attempted existing focused-caret CPU experiment was **REJECTED** by its
own ten-second duration bound. It is not used as a comparison. An initial
unpaired paint comparison also varied with run order; the reported comparison
uses A-B-B-A paired runs, seven 1,000-frame trials per run, with no concurrent
local compilation. Median paint ranges: Apple **0.697–0.710 ms**, LLVM 22
**0.698–0.705 ms**. Empty scheduler polls were **8.84–8.98 ns**. Every image hash
was `6711567839461304635`. This fixture shows no material paint regression; it is
not a PlaySuite benchmark.

Unstripped Application dylib: **10,138,928 → 10,285,520 bytes**, +146,592 (+1.45%).
The retained-paint executable increased **65,104 bytes (+0.92%)**. Compiler,
runtime, platform and implementation changes are combined in these final sizes.
The three shipped runtime dylibs total **1,603,872 bytes** separately. No runtime
headers or compiler-cache entries belong in application packages.

A clean three-runtime rebuild with six jobs took **6.47 s wall**, 23.92 s user,
8.21 s system, excluding source download/configuration. The compiler was prebuilt. The final unified-SDK clean rebuild took 6.56 s wall.
The first `macos-15` CI preparation (download, configure, build and audit) took
**56 s**. Runtime cache keys include compiler binary/SDK/image identity and the
build/integrity recipe. Restores check configured headers, libraries and symlink
integrity; the main cache archive carries the same ready-to-use runtime prefix.
An independently restored prefix passed the native cancellation/join tests with
all three relocated dylibs loaded via the application's rpath. An initial global
`DYLD_LIBRARY_PATH` trial was rejected: it also replaced Apple's system-framework
runtime and failed on an Apple-specific typed-allocation symbol. The supported
per-image rpath leaves system dependencies alone. CI now tests archive extraction,
integrity, minimums and native execution before publishing the macOS archive.

## Minimum-version audit

**OBSERVED:** all three LLVM runtime dylibs, the installed Application library
and six reference executables declare arm64/macOS 14.0. The audit rejects the
local unmodified Homebrew libc++ because its minimum is 26.0. Explicit Xcode SDK
selection prevents Homebrew's default CLT SDK and CMake framework searches from
silently selecting different SDKs.

First-party macOS compilation treats unguarded availability as an error, including
the AppKit host, CoreGraphics renderer and optional CoreAudio service. The newer
macOS 15 diagonal cursor APIs already have `@available(macOS 15.0, *)` guards and
fall back to a crosshair on 14. The display link now uses the API introduced in
14. No unguarded newer API blocked the local build. A deliberately unguarded use of
the macOS 15 cursor API was rejected by the same availability diagnostic flags. Tests ran on **26.5**, not an
actual 14.0 machine; execution on the minimum OS remains unmeasured.

All Windows targets define `_WIN32_WINNT=WINVER=0x0A00`, including exported Core
usage requirements. `GetDpiForWindow` and `AdjustWindowRectExForDpi` are optional
lookups with legacy DPI/window-frame fallbacks. No unconditional post-Windows-10
requirement was introduced. Windows validation uses the agreed `windows-2022`
runner, not a physical first-release Windows 10 machine.

## Validation and reviewed scope

Local native build (including audio): **92/92 CTests**. Installed SDK reference
programs: **5/5**, including the public cooperative worker. The worker suite
covers repeated 10,000-task batches, small/empty/invalid batches, cancellation,
normal completion, native join, destructor cleanup, self-join rejection and exception propagation.
The idle-wake test races a real pthread producer against host arming 100 times,
checks both sides of the race, coalescing, disarming, hidden generations and removal. The native
fixture checks active/idle/hidden/resumed presentation, including a static overlay
that must not keep an unchanged live frame ticking. The final overlay/lifecycle
subset passes 4/4 locally. Existing composition,
format and retry tests remain in the suite.

The LLVM cache relocation proof produced **181 hits / 0 misses** and identical
2,132,568-byte executables. Implementation, header and option changes invalidated
1, 128 and 180 compilations respectively. This receipt predates the final idle
wake addition; the four-platform CI proof exercises the final commit separately.

House-style source review covers the new public threading header, pool adapter,
private C pool projection and provenance, worker/idle-wake/native fixtures,
installed example, idle wake registration/handshake and changed display-link
methods, plus the compiler/runtime/cache tooling. Review includes borrowed
context through native join, exception containment at C callbacks, weak display
link ownership, immutable wake handler state, the arm/publication race, and
reuse of batch and diagnostic storage. The spelling scanner reports no findings
in new C++/Objective-C++ files. Existing unrelated Objective-C blocks, arrow
spelling and other legacy code are not claimed compliant. The inherited C11
pool uses C pointer syntax; the C++ spelling table is not applied to C syntax.

The builds are not warning-free. The reviewed macOS/Windows logs retain the
vendored stb_vorbis tautological pointer comparison and an existing development
inspection switch missing `draw_prepared_text`. macOS additionally reports an
existing CoreGraphics enum conversion, duplicate static library link arguments,
and upstream runtime format-attribute/deprecated linker-option diagnostics.
These do not report newer OS API requirements; they are not hidden by blanket
warning suppression. No HarfBuzz memcpy warning appeared in those logs.

Four-platform CI and final release asset verification are recorded on PR #3.
