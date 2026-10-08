# Multiline document scrollbars — SwiftEdit handoff

**GIVEN, 2026-10-08:** resolve SwiftEdit's missing TextBox scrollbars in GUI.Forms.
SwiftEdit owns its application integration and installer. Provider source belongs
here; no SwiftEdit files or dependency pins are changed by this patch.

**OBSERVED cause:** TextBox retained private horizontal/vertical text offsets,
while its ScrollableControl base knew only child-control extents. TextBox's
pointer override also bypassed the inherited scrollbar input handling. Enabling
auto-scroll could not produce or drive document scrollbars.

## Provider contract

```cpp
editor.set_multiline(true);
editor.set_auto_scroll(true);
```

Use the existing `hscroll()`, `vscroll()`, `scroll_to()`, `scroll_position()`,
`scroll_snapshot()` and `ScrollProperties` APIs. In this opt-in mode, the base
position is the authoritative text offset. The private offsets remain only for
single-line and legacy multiline behavior. No subscription, retained callback
capture, extra timer or document copy is introduced.

Text layout feeds measured document extents to the base viewport arrangement.
Wrapped layout starts from the full width, then measures at most once more if
a vertical bar narrows the viewport. Starting from full width lets a previously
visible bar disappear after content/font/size changes. The cache includes source
revision, effective font, metrics-provider identity, scale, client size and
auto-scroll mode. Scroll-only layout and paint do not remeasure text.

The base's protected position-change hook also covers silent API changes and
range clamping. TextBox uses it to invalidate retained caret geometry and cancel
an obsolete caret reveal after explicit scrolling. Layout preserves its own
pending reveal while updating ranges. Text, selection and undo ownership stay
in TextBox. Bars retain the base control's input capture and accessibility nodes.

## Adoption

SwiftEdit's existing constructor calls `set_multiline(true)`; add
`set_auto_scroll(true)` there when adopting the tested provider revision.
Rebuild the provider and consumer together because C++ headers and virtual
dispatch changed. Do not mix previous SDK archives with these headers. Preserve
old SDKs and update the toolkit dependency explicitly. The sibling's uncommitted
`verify_document_scrollbars` regression remains owned by SwiftEdit.

No change is needed to the public C ABI, audio backend, native host, renderer,
stored line endings, selection model, undo model or single-line editing policy.
Existing provisional document/line limits and prefix-measurement costs remain;
this change does not claim to solve large-document shaping performance.

## Verification and style scope

Provider regression coverage includes both bars and thumbs, wheel/API/semantic
position agreement, caret reveal, text clipping and retained overlay painting,
wrap toggles, resize/content clamping, font/DPI reflow, a viewport narrower than
one grapheme, opt-out and single-line behavior. The fake metrics provider counts
calls to verify that scrolling does not repeat text measurement. Existing editing,
caret blink, generic scrolling, layout, semantic and input tests remain in place.
The installed `examples/reference/multiline_scrollbars.cpp` consumer exercises
the public opt-in sequence, axis agreement, wrap, resize and content replacement
using only `find_package(GUIForms)` and `GUIForms::Controls`. It is part of the
existing installed-reference CI test sequence on all supported platforms.

House-style source review covers the two modified public control headers,
TextBox implementation and new scrolling translation unit, the base control's
position notifications, new multiline tests, installed example and CMake registration. Reviewed
explicit types, named virtual behavior, ordering, cache invalidation on failure,
non-owning metrics identity, authoritative offset ownership and bounded reflow.
The spelling scan covers these seven C++ files and reports zero findings. Legacy
code outside the changed behavior is not certified by this review.

Native validation: Shadow Windows x64, LLVM 22.1.8, Release, renderer-neutral
controls with the Windows host enabled. The 79-test suite passed 78 tests on its
first run; `windows_canvas_raster_tests` failed at `OpenClipboard(owner)` while
the desktop clipboard was unavailable. Its focused rerun passed. All scrollbar,
multiline editing, input, layout and semantic tests passed. The updated multiline
test including retained overlay/clip assertions also passed separately. This is
automated native/provider verification, not a claim of interactive SwiftEdit
dogfooding or completed macOS/Linux CI.

All 11 installed-reference checks pass with the new SDK. The same installed
multiline example fails against the preserved pre-fix SDK with
`document extent must expose both bars`, reproducing the missing connection.
