# Application-owned development text

**GIVEN, 2026-10-08:** PlaySuite must consume Application, text masks and Audio
with its loop transport without a second Core. Preserve the Core ownership
guard introduced in `f57d0fa`.

**OBSERVED:** the previous `gui_forms_text_masks` static target publicly linked
`gui_forms_core`. Linking it beside Application therefore introduced a second
copy of the retained engine. The generation failure correctly identified this
graph. Prepared text had the same public static-Core dependency.

## Target contract

- `GUIForms::TextMasks` aliases the `gui_forms_text_masks` consumer interface.
- `GUIForms::PreparedText` aliases the `gui_forms_prepared_text` interface when
  the separate prepared-text option is enabled.
- With Application enabled, both interfaces link only Application and advertise
  `INTERFACE_GUI_FORMS_CORE_LINKAGE=application`. Their implementation archives
  are privately included in Application in full, including entry points unused
  by the native host. The ordinary Core/Application rejection stays intact.
- Provider renderer/host code and tests of private implementation use the
  `_impl` archives. Those archives retain static Core usage requirements and
  are not consumer interfaces.
- With no native Application target, the interfaces retain static Core
  ownership for hostless text consumers. They cannot be mixed with a separately
  imported Application library.
- The prepared-text compile definition propagates through Application so that
  public Window and development declarations agree with the built library.
- Audio remains a separate static library using shared Threading. Neither its
  normal target nor its loop transport links Core.

Whole-archive inclusion uses CMake's platform implementation on macOS, Windows
and Linux. A per-library override covers an implementation archive also reached
through a host or renderer's ordinary private dependency. HarfBuzz, FreeType,
SheenBidi and libunibreak remain private dependencies with their existing pins.

No dependency pin, text behavior, mask ownership, shaping algorithm or installed
SDK admission changes here. These text and loop-transport profiles remain
development/source-only; installation still refuses them. The source build
continues to require the declared LLVM 22 toolchain and pinned text dependencies.

## Consumer validation

`tests/application_text_consumer` uses only public headers and links Application,
TextMasks and Audio, including the loop transport. When enabled it also links
PreparedText. Its executable creates a retained Window, opens and explicitly
joins a mask session, instantiates the prepared service, and renders offline
audio through the loop-transport-enabled engine.

The provider test suite configures the same consumer from scratch twice:
the admitted combination must generate successfully; adding static Core must
fail specifically with the Core ownership diagnostic. Both native CI development
text stages build/run the executable and these configure tests on all three
platforms. Existing installed mixed-Core rejection tests remain unchanged.

House-style source review covers `cmake/ApplicationText.cmake`,
`cmake/check_application_text.cmake`, the consumer fixture CMake and C++, and
the root/workflow target wiring. C++ uses explicit initialized types, named
execution, retained owners and explicit session shutdown. CMake has named
target setup and explicit configuration steps. No vendor implementation is
modified or certified by this review.

## Windows adoption receipt — 2026-10-08

**MEASURED:** GUI.Forms implementation `d5ca1e3a013a63704e4f57d2701a6121adce2097`
with PlaySuite `cbd768021fded1d511ee8391bc96090ba715717c`
(`codex-handoff/stx-adoption`), Windows x64, LLVM 22.1.8, Ninja, Release, two
compiler jobs. PlaySuite's actual top-level project configured and built its
complete application and UI tests. No consumer source or dependency pin changed.
The separate prepared-text option was also enabled. A local CMake project hook
enabled GUI.Forms' provider tests in this integration build; it did not modify
PlaySuite's source. Native CI separately tests the standalone provider profiles.

The source snapshot and products are preserved under
`C:/Users/Shadow/gui_forms/.build/playsuite-cbd7680-source` and
`C:/Users/Shadow/gui_forms/.build/playsuite-cbd7680`. The matching runtime artifact
from PlaySuite run `37714300260` passed its source inventory verification:
496 audio files, 15 font/license files. Test saves use a separate generated root.

The combined run passed 168 of 170 tests initially. The provider clipboard-file
readback assertion passed on its isolated rerun, accounting for all 99 provider
tests. PlaySuite passed 70 of 71; `game_catalog` failed before its path-rejection
assertion because Windows refused symlink creation with `WinError 1314`.
That pure Python fixture and catalog implementation are unchanged from its main
branch. No test was disabled and no system privileges were changed.
The three Application/text consumer checks were rebuilt and passed again against
the exact committed source, including the required mixed-Core rejection.

### Core ownership audit

`ninja -t commands games.exe` shows the Application import library and Audio's
archives; the executable link contains no static Core, text-mask implementation
or prepared-text implementation archive. `llvm-nm --defined-only` finds the
out-of-line Window and text-mask service implementations in
`libgui_forms_application.dll`. The executable's same-name text symbols are
import thunks, verified with `llvm-objdump -d --demangle`:

- `Window::request_focus`: DLL implementation `0x18009c1d0`; executable thunk
  `0x14071fb70` jumps through `__imp_...Window13request_focus` at `0x140805af0`.
- `TextMaskService::open_session`: DLL implementation `0x18027c1c0`; executable
  thunk `0x140721140` jumps through its import pointer at `0x140805940`.
- The PE import table names one `libgui_forms_application.dll` and the shared
  `libgui_forms_threading.dll`. There is one Core implementation in Application;
  the executable imports it. C++ constructor aliases and import thunks are not
  additional Core instances.

Audited SHA-256 values:

```text
games.exe
99b5cc7eaa90baa5cb47b70052b0bc7e026e53a24844c422d6639e75e20e65a0
libgui_forms_application.dll
09102be493630bb412efe19c8e52ec1dcd588bccc6176fe59eef07712ed46581
```

Logs, JUnit results, link command, symbol tables and thunk disassembly are
preserved as `.build/playsuite-adoption-*`. The implementation commit passed
all four native PR jobs (macOS arm64, Windows x64, Linux x64 and Linux arm64)
and the house-style job in GUI.Forms run `37749792055`. This receipt changes
documentation only; it does not claim a packaged PlaySuite release or native
macOS PlaySuite dogfooding.
