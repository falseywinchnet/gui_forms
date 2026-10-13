# T1 gate measurement: anatomy for every registry control

Date: 2026-10-13. Stage: T1, anatomy — articulated controls and one schema
(`planning/stages/T1-anatomy/README.md`). Status: **closed 2026-10-13**,
owner signed off. Measured on Windows x64; CI ran the same tests on macOS,
Linux and Windows for each pull request (PRs #46 to #57 for the gate,
#62 to #67 for the close-out).

## The gate

> Every control in `manifests/controls.json` lists its parts and states in
> the schema; a test reads each part's geometry from a laid-out Gallery; the
> PlaySuite workarounds that copied private geometry (the track bar inset)
> are no longer needed.

| Condition | Result |
|---|---|
| Every registry control lists its parts and states | 60 of 60 controls declare their own anatomy. The other 12 registry entries are not controls (11 components and the LiveSurface service type) and have no parts; the coverage test lists them by name. |
| A test reads each part's geometry from a laid-out Gallery | `gui_forms_gallery_pages_tests` (`tests/gallery_pages/anatomy_coverage_tests.cpp`) selects and lays out every Gallery page, then reads the part geometry and states of all 28,830 control instances and of every realized item. Every anatomy validates, every published state is declared, every geometry is finite and absent parts are empty. Its pending list is empty. |
| The track bar inset is no longer needed | `TrackBar::value_span_bounds()` and `(*track).parts().track` publish the span the pointer maps across. `gui_forms_anatomy_tests` checks that a pointer at 75 % of the published span sets the value to 75. PlaySuite's `SettingSlider` (`constexpr double inset = 10`) lives in `falseywinchnet/games` and has not been changed here; it can now read the span instead. |

The coverage table is in [anatomy-table.md](anatomy-table.md).

## Workload and environment

- Machine: the owner's Windows 11 desktop ("Shadow Windows"), x64.
- Toolchain: the project's pinned MSYS2 CLANG64 tree, clang 22.1.8, restored
  with `tools/windows_toolchain.py restore` at `D:\a\_temp\msys64`.
- Configuration: the Windows CI configuration from `tools/ci_build.py`
  (Ninja, Release, `cmake/llvm22.cmake`, HarfBuzz on, Skia off) plus
  `-DGUI_FORMS_BUILD_GALLERY=ON`, the text-mask profile and the Windows DIB
  profile, as the CI `native` and `programs` jobs build them.
- Source: branch `claude/t1-collections`, stacked on PRs #46–#56.

## Commands

From the workspace above the checkout, inside the CLANG64 shell:

```text
cmake --build .build/native-windows-x64/gui-forms --parallel 8
ctest --test-dir .build/native-windows-x64/gui-forms --timeout 120 --output-on-failure
GUI_FORMS_ANATOMY_TABLE=1 ctest --test-dir .build/native-windows-x64/gui-forms \
    -R '^gui_forms_gallery_pages_tests$' -V
python3 -B tools/check_house_style.py include src demo tests examples tools file_manager_demoboard stx_vorbis
python3 tools/generate_library_docs.py --check
```

## Results

- `ctest`: 124 of 124 tests passed (43.6 s).
- `gui_forms_gallery_pages_tests`: "anatomy coverage: 60 of 60 registry
  controls (12 nonvisual entries excluded); 28830 control instances
  inspected".
- House style: no findings. Library docs check: pass.

## What the schema now is

- Part roles from design language §8.1 and the §8.4 state set, with
  names as identities (owner decisions 1, 3 and 4), stored in snake_case and
  written in kebab-case by theme and form documents (decision 8).
- Typed records for C++: `parts()` and `states()` with dot members; names
  never used to read or set state from C++ (owner decision 4).
- Application-defined finite states for controls and for items (owner
  decision 2).
- Item systems for repeated rows, tabs, menu items, breadcrumb segments,
  tree nodes and cards, identified by their semantic virtual child's stable
  ID (owner decision 5).

## Close-out (owner decision 6, 2026-10-13)

The owner chose what closes T1: the schema export, interactions in the
schema, per-part states and details-view column headers as items. All four
are built on stacked branches after the gate:

| Item | Branch | Evidence |
|---|---|---|
| Column headers as items | `claude/t1-item-systems` | Item systems in the schema; ObjectView's `column` system. `gui_forms_anatomy_tests` (`test_object_view_columns_are_items`, `test_item_systems_are_distinct`); the Gallery coverage test checks every realized item against its own system. |
| Per-part states | `claude/t1-part-states` | ScrollBar, TrackBar, scrollable thumbs, DateTimePicker, DropDownButton and SplitContainer publish states on the part that has them. `gui_forms_anatomy_tests` checks a dragged thumb and a pressed arrow; the coverage test fails on any undeclared part state. |
| Interactions | `claude/t1-interactions` | 49 types declare 134 interactions; `Component::interactions()` lists them base types first. `gui_forms_anatomy_tests` checks order, payload spelling and a component. |
| Schema export | `claude/t1-schema-export` | `manifests/control-schema.json`, written by `gui_forms_gallery_pages_tests` (which fails when it is stale); `tools/control_schema.py check` (every type's interactions equal its headers' public events) and `render --check` (`docs/controls/generated/schema.md`) in CI. |

Measured on the same machine and configuration as the gate, on
`claude/t1-schema-export`:

- `ctest`: 122 of 122 tests passed. `gui_forms_gallery_pages_tests`
  reports "anatomy
  coverage: 60 of 60 registry controls (12 nonvisual entries excluded); 28830
  control instances inspected" and "control schema: 72 types match the
  committed manifest".
- `python3 tools/control_schema.py check`: "72 types OK". The export has 60
  controls, 11 components and the LiveSurface service, 1,244 property rows
  and 971 interaction rows.
- House style, control registry, library docs and the tool unit tests pass.

The coverage table was regenerated on this branch; it now shows each part's
own states and every item system.

## Cross-host evidence

PR #67, the last of the close-out, passed all twelve CI checks before it
merged: the native and programs jobs on macOS arm64, Linux x64, Linux arm64
and Windows x64, house style and the docs build. The Gallery page tests in
those jobs compare their schema export with the committed
`manifests/control-schema.json`, so the schema is identical on every host.

## Not required to close T1

- Hit regions derived from part geometry (design language §7.4 rule 1):
  proposed for T2, beside cue geometry (where focus and preselect rings are
  stroked), which already moves there.
- ObjectView, CorrespondenceView and PropertyList publish text as layout
  slots rather than measured extents: proposed for T2's text work.
