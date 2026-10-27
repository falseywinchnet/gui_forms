# First GUI.Forms source review

Generated on 2026-10-26 from GUI.Forms main revision `0067c679` with:

```sh
python3 -B code_integrity_checker/check.py include src \
  --json code_integrity_checker/reports/gui_forms-2026-10-26.json
```

The report covers 963 first-party C/C++ files and contains 44 individual
review candidates: 21 narrow size conversions (I05), 14 raw extent products
at storage sinks (I06), and 9 empty-buffer pointer endpoints (I04). Every
record identifies the exact source line, house-style section and corrective
review question. The `house_style_status` field says the required invariant is
not evident to this local source check; some instances may satisfy a wider
contract, which requires inspection before editing.

As a calibration check, the checker found the former count-before-construction
pattern in the preserved `SkTBlockList.h` reference and the one-byte-late
poison boundary in the preserved `SkBlockAllocator.h` reference. It reported
neither pattern in the corresponding corrected candidate headers. The checker
does not claim that those are its only possible findings or that the current
GUI.Forms report proves the absence of defects.

When using this report to guide an implementation pass, read the finding's
source and its caller contract. Prefer making the invariant clear in the code:
validate shape before storage work, establish bounds in Release, and avoid
forming addresses from potentially empty storage. A human review remains
necessary for candidates whose bounds are established outside the local
function. Rerun the tool after changes; this file is a snapshot, not a live
baseline or a list of accepted exceptions.

# Triage of the first report (2026-10-27)

Phase E, item E1. The rebase onto main (61e4bf85) found 45 candidates: the 44 above plus one new one, row 26. Each is sorted into one class:

- **defect**: the invariant does not hold;
- **guarded**: it holds through a guard, either one the checker now recognizes or one outside the function, recorded in `../reviews.json`;
- **false shape**: the rule should not have matched.

*trap?* marks sites that a trap might already stop: ASan in the sanitizer tree, or a hardened library on a later indexed access. These are left undecided until the owner defines the "maps to trap" class.

| # | Rule | Site | Class | Basis | trap? |
|---|---|---|---|---|---|
| 1 | I05 | src/abi/drawing_skia_c_api.cpp:195 | defect (low); Skia, ignored | the wide string's length is narrowed to int with no stated limit (2 GiB input) | |
| 2 | I05 | src/abi/drawing_skia_c_api.cpp:201 | defect (low); Skia, ignored | as row 1 | |
| 3 | I05 | src/abi/drawing_skia_c_api.cpp:280 | guarded; Skia, ignored | `requested.size() >= LF_FACESIZE` returns (line 263) | |
| 4 | I05 | src/abi/drawing_skia_c_api.cpp:335 | defect (low); Skia, ignored | trace `%.*s` precision; a negative value reads the view past its end | yes |
| 5 | I05 | src/abi/drawing_skia_c_api.cpp:342 | defect (low); Skia, ignored | as row 4 | yes |
| 6 | I05 | src/audio/audio.cpp:382 | false shape | `std::array<float, 8192>`: a literal extent (rule refined) | |
| 7 | I06 | src/controls/data_grid_view/data_grid_view_parts.cpp:965 | guarded (reviewed) | the realized window's row and column counts, bounded by the viewport | yes |
| 8 | I04 | src/controls/docking/dock_site/dock_site.cpp:2321 | false shape | `std::string_view`: null plus zero is defined in C++ (rule refined) | |
| 9 | I04 | src/controls/pages/navigation_pane/navigation_pane.cpp:1705 | false shape | as row 8 | |
| 10 | I04 | src/controls/panel/list_box/checked_list_box/checked_list_box.cpp:231 | false shape | as row 8 | |
| 11 | I04 | src/controls/panel/list_box/checked_list_box/checked_list_box.cpp:233 | false shape | as row 8 | |
| 12 | I04 | src/controls/panel/list_box/list_box.cpp:484 | false shape | as row 8 | |
| 13 | I04 | src/controls/panel/list_box/list_box.cpp:486 | false shape | as row 8 | |
| 14 | I04 | src/core/binding/value/binding_value.cpp:93 | false shape | as row 8; `format.size() < 2` also returns first | |
| 15 | I04 | src/core/binding/value/binding_value.cpp:94 | false shape | as row 14 | |
| 16 | I04 | src/core/docking/dock_layout/dock_layout_persistence.cpp:80 | false shape | as row 8 | |
| 17 | I06 | src/core/drawing/bitmap/bitmap.cpp:11 | guarded (detected) | sides bounded by maximum_dimension and thrown on; 64-bit product checked against maximum_bytes | |
| 18 | I06 | src/core/engine/primitives/kernel/box_blur.cpp:132 | guarded (reviewed) | callers pass PartRaster extents; PartRaster::create bounds both sides | yes |
| 19 | I06 | src/core/engine/primitives/material/image_variant.cpp:250 | guarded (reviewed) | stated in the comment: registry and PartRaster::create bound the sides | yes |
| 20 | I06 | src/core/engine/primitives/procedural/procedural_pattern.cpp:134 | guarded (reviewed) | set.count is at most maximum_octaves; width is a PartRaster side | yes |
| 21 | I06 | src/core/engine/primitives/procedural/procedural_pattern.cpp:135 | guarded (reviewed) | as row 20 | yes |
| 22 | I06 | src/core/engine/primitives/procedural/procedural_pattern.cpp:136 | guarded (reviewed) | as row 20 | yes |
| 23 | I06 | src/core/engine/primitives/procedural/procedural_pattern.cpp:137 | guarded (reviewed) | as row 20 | yes |
| 24 | I06 | src/core/engine/primitives/procedural/procedural_pattern.cpp:511 | guarded (reviewed) | fresco_blooms is a constant; the side is a PartRaster side | yes |
| 25 | I06 | src/core/engine/primitives/procedural/procedural_pattern.cpp:512 | guarded (reviewed) | as row 24 | yes |
| 26 | I06 | src/core/layout/grid_solver/grid_solver.cpp:943 | guarded (detected); new since the report | `wanted > maximum_grid_cells / across_` returns false first | |
| 27 | I06 | src/core/layout/style_reader/layout_style_reader.cpp:1132 | guarded (reviewed) | the total is refused above maximum_grid_tracks (line 1129); both factors are bounded | |
| 28 | I06 | src/core/theme_language/selector/theme_selector.cpp:486 | guarded (detected) | `left * right > budget_.rules` reports and returns first | |
| 29 | I06 | src/core/window/presentation/window_presentation.cpp:88 | guarded (detected) | the span check returns first; on a 32-bit `size_t`, its own product could wrap at the 32768-pixel host limit | yes |
| 30 | I05 | src/host/linux/accessibility/linux_accessibility.cpp:1036 | guarded (reviewed) | the vector was sized from an int count | |
| 31 | I05 | src/host/linux/accessibility/linux_accessibility.cpp:1037 | guarded (reviewed) | as row 30 | |
| 32 | I05 | src/host/linux/accessibility/linux_accessibility.cpp:1044 | guarded (detected) | as row 30; the count check at line 1037 exits | |
| 33 | I05 | src/host/linux/application/linux_host.cpp:1128 | false shape | a vector of 128 bytes: a literal extent (rule refined) | |
| 34 | I05 | src/host/linux/application/linux_host.cpp:1131 | false shape | as row 33 | |
| 35 | I05 | src/host/linux/application/linux_host.cpp:1137 | false shape | as row 33 | |
| 36 | I05 | src/host/linux/application/linux_host.cpp:1140 | false shape | as row 33 | |
| 37 | I05 | src/host/linux/services/linux_services.cpp:288 | false shape (reviewed) | three atoms | |
| 38 | I05 | src/host/linux/services/linux_services.cpp:325 | guarded (detected) | above 64 KiB the transfer goes through INCR; this branch is at most 64 KiB | |
| 39 | I05 | src/host/windows/application/windows_host.cpp:3915 | defect (low, reviewed) | trace `%.*s` precision from a stable ID's length; a negative value reads past the view | yes |
| 40 | I06 | src/host/windows/raster/dib_frame_store.cpp:99 | guarded (detected) | `columns > pixel_limit / rows` returns first | |
| 41 | I05 | src/host/windows/services/windows_notify_icons.cpp:18 | guarded (reviewed) | NotifyIcon refuses text above maximum_tray_tooltip_bytes (notify_icon.cpp:90) | |
| 42 | I05 | src/host/windows/services/windows_notify_icons.cpp:21 | guarded (reviewed) | as row 41 | |
| 43 | I05 | src/render/skia/raster/skia_raster.cpp:320 | false shape; Skia, ignored | the clip stack is at most the canvas save count, an int | |
| 44 | I05 | src/render/text/harfbuzz/harfbuzz_font_engine.cpp:451 | guarded (reviewed) | the shaping entry refuses input above limits.input_bytes or INT_MAX (line 863) | |
| 45 | I05 | src/render/text/text_mask/text_mask_shape.cpp:134 | guarded (detected, named failure) | `work_.admit(line.size())` throws MaskLimitFailure above 4 MiB | |

**Totals.**
- **defect: 5.** Four are in the Skia C ABI (rows 1, 2, 4 and 5) and are ignored. One is in GUI.Forms: row 39, a trace-only over-read.
- **guarded: 24.** The checker now detects 9 of these, and 15 are recorded in `../reviews.json`.
- **false shape: 16.** The refined I04 and I05 rules remove 14. One is recorded (row 37), and one is in Skia (row 43).

The checker's run on this tree, `gui_forms-2026-10-27.json`, has 39 candidates: 0 raised, 14 guarded, 17 reviewed and 8 ignored. It also lists six earlier-suppressed sites (windows_host.cpp 104 to 123, windows_semantic_source.cpp 205 and 220) that are now shown as guarded instead of being dropped silently.
