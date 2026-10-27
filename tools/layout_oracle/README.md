# Layout oracle

Chromium as a test oracle for GUI.Forms layout (stage L,
[`planning/stages/L-layout/PLAN.md`](https://github.com/malkuth-software/notes/blob/gui_forms/planning/stages/L-layout/PLAN.md) section 6). Chromium never runs inside
GUI.Forms, at import or at run time (L README, owner decision 2). It runs
only here: when a fixture is written, and in the `layout-oracle` CI job.

| File | What it does |
|---|---|
| `chromium_pin.json` | The pinned Chrome for Testing `chrome-headless-shell` 155.0.8059.39, by URL, size and SHA-256, for linux64 (CI) and win64 (local runs); owner decision L-Q10 |
| `fetch_chromium.py` | Downloads or takes an archive, verifies its size and hash, and unpacks it into a new, empty directory outside the repository; refuses without `--approved` |
| `devtools.py` | A standard-library Chrome DevTools Protocol client |
| `page_template.html` | The page every fixture is laid out in: the reset (border boxes, no default margins, zero borders) and the bundled faces |
| `capture.py` | Lays every fixture out at every profile of `tests/layout_corpus/corpus.xml` and writes Chromium's boxes |
| `corpus.py` | The manifest, fixture and box-file formats, and the one edge-by-edge comparison |
| `compare.py` | `report`: a native snapshot against the references, per node; `drift`: fresh references against the checked-in ones |
| `test_layout_oracle.py` | The tools' own tests, without Chromium |

## The corpus

`tests/layout_corpus/`:

- `corpus.xml`: the profiles (four viewports at scale 1 and 2), the
  tolerances (0.05 logical px per edge for box fixtures, 0.51 px per edge
  and 1.0 px on the baseline for text fixtures; owner decision L-Q9) and the
  fixtures in order.
- `fixtures/<group>/<name>.html`: one well-formed HTML fragment per fixture,
  which is also XML. Each element has a `data-gf-id` and a `data-gf-kind`
  (the GUI.Forms control, kebab-case); `style` holds layout declarations. A
  known difference is recorded on its node with
  `data-gf-expected-difference="<reason>"`; it is still reported.
- `references/<group>/<name>.xml`: Chromium's boxes, checked in.

`gui_forms_layout_corpus_tests` reads the same files on every CI host and
compares GUI.Forms' own layout against the references; no Chromium is
needed there (owner decision L-Q11).

## Adding or changing a fixture

```sh
python -B tools/layout_oracle/fetch_chromium.py --platform win64 --approved \
    --archive <the approved zip> --unpack <new empty directory outside the repository>
python -B tools/layout_oracle/capture.py --chromium <unpacked chrome-headless-shell> \
    --corpus tests/layout_corpus --output <new empty directory> --platform win64
# copy the new reference files into tests/layout_corpus/references/, then
gui_forms_layout_corpus_tests tests/layout_corpus --native-out <directory>
python -B tools/layout_oracle/compare.py report --corpus tests/layout_corpus \
    --native <directory> --output report.json
```

The `layout-oracle` job regenerates every reference on Linux and fails if
any box moved by more than one LayoutUnit (1/64 px) from the checked-in one.
