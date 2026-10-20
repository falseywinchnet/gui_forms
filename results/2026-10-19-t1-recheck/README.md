# T1 recheck: the anatomy gate on main after the W, PageView, docking and ribbon work

Date: 2026-10-19. Stage: T1, anatomy (`planning/stages/T1-anatomy/README.md`),
complete since 2026-10-13. The owner asked to clear T1 before the T2
interviews; this record re-runs T1's gate on main after the campaign that
added the Windows Forms controls, PageView, docking, the ribbon and the
editors (PRs #84 to #170), measured exactly as the gate was
(`results/2026-10-13-t1-anatomy-gate/README.md`).

## Result

| Condition | 2026-10-13 gate | 2026-10-19 recheck |
|---|---|---|
| Every registry control lists its parts and states | 60 of 60 controls (12 nonvisual entries excluded) | **85 of 85 controls** (19 nonvisual entries excluded) |
| A test reads each part's geometry from a laid-out Gallery | 28,830 control instances inspected; pending list empty | **35,487 control instances inspected**; pending list empty; every anatomy validates, every published state is declared, every geometry is finite |
| Schema export current | `manifests/control-schema.json` written and checked | **104 types** in the schema (`tools/control_schema.py check`), current with the headers |

T1's gate holds on main: every control added since it closed is articulated
from birth, under the atom-vocabulary ruling (design language §12.7). The
coverage table is in [anatomy-table.md](anatomy-table.md).

## Workload and environment

- Machine: the owner's Windows 11 desktop, x64; the pinned MSYS2 CLANG64
  tree, clang 22.1.8.
- Configuration: as the gate (Ninja, Release, `cmake/llvm22.cmake`, HarfBuzz
  on, Skia off, Gallery on), plus the prepared-text profile.
- Source: main at f73130e6 (after #142).

## Commands and results

```text
cmake --build .build/native-windows-x64/gui-forms --parallel 6
ctest --test-dir .build/native-windows-x64/gui-forms --timeout 600 -j 4
GUI_FORMS_ANATOMY_TABLE=1 ctest --test-dir .build/native-windows-x64/gui-forms -R '^gui_forms_gallery_pages_tests$' -V
python -B tools/control_registry.py check        # 104 records OK; emit-cpp current
python -B tools/control_schema.py check          # 104 types OK
python -B tools/generate_library_docs.py --check # PASS
python -B tools/check_house_style.py ...         # 0 findings
python -B tools/check_lifetime_rules.py ...      # 0 findings (1,170 files)
```

- Full suite: 178 of 178 passed.
- Gallery coverage: "anatomy coverage: 85 of 85 registry controls (19
  nonvisual entries excluded); 35487 control instances inspected".

## Vocabulary gaps bridged since T1, handed to T2

Each was bridged with the nearest existing atom (the ruling forbids local
additions) and reported by its builder; T2 (the engine), T4 (the default
theme) or stage A (accessibility) settles each. Decided but not yet built are
marked.

| Gap | Bridge today | Where | Settles it |
|---|---|---|---|
| Tree, tree-grid, table and column-header semantic roles | `list` / `list_item`, headers as `button`, depth in the description | TreeListView, DataGridView, ListView | stage A (P35.7) |
| Opacity input for parts | fade published as the `expanding` state, not drawn | docking drop tiles (D3) | T2/T4 (DK-52) |
| Image part role | the stamp picture as role `icon` | docking stamp (#158) | T2 |
| A "refused" mark | the `close` mark | docking rejected-drop cue | decided DK-51, not built |
| Live-region semantic | the dock site's description | keyboard move mode announcements | decided DK-51, not built |
| An `open` state for side tabs | `selected` | auto-hide side tabs (D5) | decided DK-51, not built |
| Italic text in menu rows | `MenuItemSpec::italic` (a flag, not an atom) | PageView overflow list | T4 |
| Modified mark | a `radio_dot` glyph part | document tabs (P4) | T4 (PT4-1 decided the dot) |
| A drop-target recipe | the `button` family | tab insertion mark (P6) | T4 |
| An accent-line role | `keyline` with `current` and `AccentIndex{0}` | docking active caption (T4-3) | T4 |
| Tab paint family; the selected tab's open seam | `menu_item`, which draws a rule under the selected tab | PageStrip, TabControl | T4 (DK-40, PT2-1) |
| Text-match highlight | a `selection` part with `hot` | filtering ComboBox (#126) | T4 |
| Token plate and token-field semantics | `chip` of the `choice` family; `list` / `list_item` | TokenBox | T4, stage A |
| Pin semantic action | a checked `check_box` named "Pinned" | docking pins | stage A |
| Navigator role | a `group` of two `list`s | DockNavigator | stage A |
| Menu section headings | a disabled command row | NavigationPane configure menu | T4 |
| Grip dots across a whole box | split into `grip_bar` and `grip` parts | NavigationPane | T2 |
| Keypad asterisk key | handled through text input | TreeListView | host key tables |
