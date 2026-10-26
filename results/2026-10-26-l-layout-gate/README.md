# Stage L, L7: the stage gate (2026-10-26)

Phase L7 of `planning/stages/L-layout/PLAN.md`: the whole corpus runs at
every profile and is checked against Chromium. This record holds the
report, the counted bounds of section 5, and what the gate does not yet
exercise. `docs/user/layout.md` is rewritten to describe the containers
as they now are.

The owner settled the gate's scope on 2026-10-26 (option 1):
- the gate runs on the box fixtures, in both directions;
- the text-scale dimension is recorded as not yet exercised;
- text fixtures are phase L8.

## What L7 adds

- Right-to-left profiles. `corpus.xml` gains eight profiles with
  `direction="rtl"`: the four viewports at both scales.
  - They apply only to fixtures marked `bidirectional="true"`.
  - A fixture is marked when Chromium lays it out differently right to
    left at some profile: 165 of the 181. Chromium places the boxes of the
    other 16 identically in both directions: vertical stacks, and the
    fixtures that are already right to left.
    `tools/layout_oracle/test_layout_oracle.py` checks the marking.
- Chromium side (`capture.py`): before each right-to-left profile is read,
  the capture sets `dir="rtl"` on `<html>`. Before each left-to-right
  profile it removes the attribute.
- GUI.Forms side (`gui_forms_layout_corpus_tests`): the fixture's root is
  set to `LayoutDirection::right_to_left`.
- The box files: a right-to-left profile carries `direction="rtl"`, and its
  report key ends in `/rtl`.
- Every left-to-right reference is byte-identical to the one checked in
  before L7.

## Machine and Chromium

- Shadow Windows (Windows 11, x64), workspace `D:\gui_forms_w4`, LLVM 22
  Release tree.
- Chrome for Testing `chrome-headless-shell` 155.0.8059.39 (owner decision
  L-Q10), win64 package: 124,737,213 bytes, SHA-256
  `20798f7c51a22def0b3d02e526a8f52e7ee2c7eddcde9d81a4c23f618e8b5436`,
  unpacked outside the repository.
- The `layout-oracle` CI job regenerates the references on Linux from the
  linux64 package: 124,203,329 bytes, SHA-256
  `39dcb8c46550632a3d911850ab3b8af840b4e3f6d8622faa2018eb8756278786`
  (`tools/layout_oracle/chromium_pin.json`).

## Profiles and tolerances

| Dimension | Values | Fixtures |
| --- | --- | --- |
| Viewport | 320 x 240, 800 x 600, 1280 x 800, 1440 x 900 | all |
| Device scale | 1, 2 | all |
| Direction | ltr, rtl | ltr: all; rtl: the 165 marked bidirectional |
| Text scale | 100 %, 150 % | **not yet exercised**: the corpus has no text fixture (phase L8) |

Tolerances (decision L-Q9):
- box fixtures: 0.05 logical px per edge;
- text fixtures: 0.51 px per edge and 1.0 px on the first baseline. These
  are not yet exercised, since there are no text fixtures.

## The corpus

`tests/layout_corpus/corpus.xml` lists the fixtures. `report.json` lists
each one with its class, its direction marking and its per-node
differences.

| Group | Fixtures | Bidirectional | Node comparisons | Over tolerance | Recorded | Largest difference outside recorded nodes |
| --- | --- | --- | --- | --- | --- | --- |
| flex | 51 | 50 | 4,368 | 0 | 0 | 0.0375 px |
| grid | 52 | 52 | 5,088 | 0 | 0 | 0.026 px |
| block | 31 | 18 | 3,312 | 0 | 0 | 0.022 px |
| position | 21 | 19 | 2,032 | 0 | 0 | 0.010 px |
| flow (FlowLayoutPanelCompat) | 17 | 17 | 1,904 | 0 | 144 | within 0.05 px |
| table (TableLayoutPanelCompat) | 9 | 9 | 736 | 0 | 16 | within 0.05 px |
| **Total** | **181** | **165** | **17,440** | **0** | **160** | |

- The corpus runs 2,768 fixture profiles.
- 12 fixtures nest one kind of container in another:
  - `block/block-in-grid`, `block/flex-in-block`;
  - five grid fixtures with flex items;
  - five positioning fixtures across containers.
- Four grid fixtures take their forms from the atlas.
- Against the plan's count: at least 100 fixtures, with about 40 flex, 40
  grid and 20 block and inline. The corpus has 51 flex, 52 grid and 31
  block.

### Recorded differences

Every recorded difference is in one of the two compatibility containers.
The CSS containers have none.

| Fixtures | Profiles | Nodes | Reason |
| --- | --- | --- | --- |
| `flow/align-center`, `flow/align-flex-end` | both directions | 112 | CSS sizes the one line of a wrapping container to its content; FlowLayoutPanelCompat gives a single line the whole client height (L0) |
| `table/auto-tracks-normal` | both directions | 16 | CSS `justify-content: normal` stretches auto tracks into free space; TableLayoutPanelCompat keeps auto tracks at their content size (L0) |
| `flow/margins` | rtl only | 32 | Full mirroring (owner, 2026-10-26): FlowLayoutPanelCompat mirrors its padding and its items' margins, as Windows Forms' `RightToLeftLayout` does, while CSS keeps them physical. New in L7. |

- The first two rows are L0's 64 differences, now counted in both
  directions.
- The third row is the only new pattern. Left to right, the same nodes
  match within tolerance.

## Counted bounds (section 5)

Each panel's test, at 10,000 children:

| Container | Change one item's text | Resize the container | Arrange again |
| --- | --- | --- | --- |
| FlexPanel (`gui_forms_flex_panel_tests`) | 1 measure callback | 0 callbacks | 0 allocations |
| GridPanel (`gui_forms_grid_panel_tests`) | 1 measure callback | 0 callbacks | 0 allocations |
| BlockPanel (`gui_forms_block_panel_tests`) | 1 measure callback | 0 callbacks | 0 allocations |

Limits are refused before storage is touched:
- 65,536 children in each container;
- 10,000 tracks per grid axis;
- 1,048,576 grid auto-placement cells.

The L3 to L5 records give the tests for each limit.

## Not yet exercised, and open

- **Text scale and the text tolerances.** No fixture holds text, so the
  150 % profile, the 0.51 px text tolerance and the 1.0 px baseline
  tolerance have not run. Phase L8 adds:
  - the text fixtures, with a CSS-style text leaf, provisionally named
    `TextBlock`;
  - the bundled engine in the harness;
  - the first-baseline capture;
  - the text-scale profiles.
- **Window resize.** `Window::resize` still marks the whole tree
  measure-dirty, which empties every measure cache. The L3 count stands:
  at 10,000 labels in a FlexPanel root, one window resize costs 10,000
  measure callbacks. A resize of the container itself costs none.
  Invalidating only the root's measure would change what every container
  is told on a window resize. That change needs the whole-suite build, so
  it is left as an open item rather than made here.
- **Scroll bars right to left.** A scrolled right-to-left parent still
  draws its scroll bars left to right (L6).
- **Composite controls right to left.** Composite controls that place
  their own children are not reviewed for right to left (L6).

## Commands

```text
python -B tools/layout_oracle/capture.py --chromium <win64 chrome-headless-shell.exe> --corpus tests/layout_corpus --output <scratch>/refs --platform win64
python -B tools/layout_oracle/capture.py ... --output <scratch>/refs3        # a second capture
python -B tools/layout_oracle/compare.py drift --corpus tests/layout_corpus --regenerated <scratch>/refs3
gui_forms_layout_corpus_tests tests/layout_corpus --native-out <scratch>/native
python -B tools/layout_oracle/compare.py report --corpus tests/layout_corpus --native <scratch>/native --output report.json
gui_forms_flex_panel_tests; gui_forms_grid_panel_tests; gui_forms_block_panel_tests
```

Results:
- The two captures are byte-identical; drift reports a largest movement
  of 0.
- The corpus test reports 181 fixtures, 2,768 fixture profiles, 17,440
  nodes, 0 over tolerance and 160 recorded.
- `compare.py report` agrees with the corpus test.
