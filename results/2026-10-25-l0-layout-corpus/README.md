# Stage L, L0: the layout corpus and the oracle (2026-10-25)

Phase L0 of `planning/stages/L-layout/PLAN.md`: the oracle tools in
`tools/layout_oracle/`, the corpus in `tests/layout_corpus/`, and
`gui_forms_layout_corpus_tests`. The first corpus describes today's
`FlowLayoutPanel` and `TableLayoutPanel`; it measures the containers as
they are, and changes nothing in them.

## Machine and Chromium

- Shadow Windows (Windows 11, x64), workspace `D:\gui_forms_w4`, LLVM 22
  Release tree of `tools/ci_build.py`'s layout.
- Chrome for Testing `chrome-headless-shell` 155.0.8059.39, win64 package,
  124,737,213 bytes, SHA-256
  `20798f7c51a22def0b3d02e526a8f52e7ee2c7eddcde9d81a4c23f618e8b5436`
  (owner decision L-Q10), unpacked outside the repository; 292,750,079
  bytes unpacked. The linux64 package (124,203,329 bytes, SHA-256
  `39dcb8c46550632a3d911850ab3b8af840b4e3f6d8622faa2018eb8756278786`) was
  downloaded only to record its hash for the CI pin.

## Commands

```sh
python -B tools/layout_oracle/capture.py --chromium <win64 chrome-headless-shell.exe> \
    --corpus tests/layout_corpus --output <scratch>/refs --platform win64
python -B tools/layout_oracle/capture.py ... --output <scratch>/refs3   # a second capture
python -B tools/layout_oracle/compare.py drift --corpus tests/layout_corpus --regenerated <scratch>/refs3
gui_forms_layout_corpus_tests tests/layout_corpus --native-out <scratch>/native
python -B tools/layout_oracle/compare.py report --corpus tests/layout_corpus \
    --native <scratch>/native --output report.json
```

## Numbers

- 26 fixtures (17 flow, 9 table), 8 profiles each (320 x 240, 800 x 600,
  1280 x 800 and 1440 x 900, at scale 1 and 2): 1,320 node comparisons.
- Two captures are byte-identical (`drift`: largest movement 0).
- GUI.Forms against Chromium, box tolerance 0.05 logical px per edge:
  **0 nodes over tolerance**; 64 recorded differences; the largest
  difference within tolerance is 0.015625 px (one LayoutUnit, in the
  space-around and space-evenly distributions).
- The C++ test and `compare.py report` agree (`report.json` here).

## Recorded differences

Two patterns, both behaviour of today's containers that CSS does not share.
Each is recorded on its nodes with its reason and still counted:

1. **A wrapping `FlowLayoutPanel` with one line** gives that line the whole
   client height, so `align-items: center` or `flex-end` centres or ends
   the items in the panel. CSS sizes the line of a multi-line container to
   its content (`align-content: flex-start`). Fixtures `flow/align-center`
   and `flow/align-flex-end`, at the 1280 and 1440 viewports where all
   items fit on one line: 56 nodes.
2. **`TableLayoutPanel` keeps auto tracks at their content size.** CSS's
   default `justify-content: normal` stretches auto tracks into free space.
   Fixture `table/auto-tracks-normal`: 8 nodes.

These go to L6, where the two containers are rebuilt on the solvers with
their existing tests as the contract (owner decision L-Q2); whether the
first pattern changes is decided there, against those tests.

## Cross-host record

Pending: CI runs `gui_forms_layout_corpus_tests` on the four native hosts,
and the `layout-oracle` job regenerates the references on Linux with the
linux64 package.
