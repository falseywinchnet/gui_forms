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
