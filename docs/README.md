# GUI.Forms documentation (maintainer notes)

This directory is the source of the GUI.Forms documentation site: Markdown
rendered by Sphinx with MyST-Parser and the Furo theme, published on Read the
Docs and built in CI with warnings treated as errors. These notes are for
people editing the site. The user-facing entry point is `index.md`.

## Build locally

From the repository root:

```sh
python3 -m venv .venv
. .venv/bin/activate
python3 -m pip install -r docs/requirements.txt
sphinx-build -W -b html docs docs/_build/html
```

Open `docs/_build/html/index.html`. `docs/_build/` is ignored by Git. Always
build with `-W`: Read the Docs (`.readthedocs.yaml`, `fail_on_warning: true`)
and the `docs` job in `.github/workflows/native.yml` do, so a warning that
passes locally without `-W` fails everywhere else. Add `--keep-going` to see
every warning in one run.

The project version shown in the site title is read from
`project(GUIForms VERSION x.y.z)` in the root `CMakeLists.txt` at build time
(`conf.py`, `read_project_version`). Do not hard-code it anywhere in `docs/`.

## Layout

| Path | Content | Edited by |
|---|---|---|
| `index.md` | Landing page and the five toctrees | hand |
| `start/` | Install, consume the SDK, first window, build from source, download | hand |
| `controls/index.md` | Controls landing page | hand |
| `controls/generated/by-function.md`, `controls/generated/alphabetical.md` | Indexes generated from `manifests/controls.json` by `tools/control_registry.py` | generated; do not edit |
| `concepts/` | Retained tree, update scopes, layout, painting, text engine, input, threading, lifecycle, hosts | hand |
| `api/` | The API reference: one page per control category or library area, generated from the public headers by `tools/generate_library_docs.py` | generated; do not edit |
| `reference/inventory.json`, `reference/contracts/` | The clang declaration inventory and reviewed contracts of `tools/api_reference.py`; the contracts also feed `api/` | generated or reviewed JSON; not Sphinx sources; excluded |
| `project/` | Decisions table, roadmap, contributing, evidence index | hand |
| `project/adr/ADR-NNN.md` | One-line `{include}` of `../../decisions/ADR-NNN-*.md` | add one when adding an ADR |
| `notes/` | Dated internal working notes | kept for provenance; excluded |
| `library/` | `manual.json`, the hand-written descriptions read by `tools/generate_library_docs.py`, and archived captures | hand; excluded |
| `*.md` at this level other than `index.md` | Legacy long-form documents | kept in place; excluded |

## The exclude list

`conf.py` lists every path under `docs/` that is not a site page. The list is
explicit rather than a glob because the build runs with `-W`: a Markdown file
that Sphinx picks up but that no toctree references is a warning, and so is a
page with a broken relative link. The excluded groups are:

- `_build` and OS metadata files;
- this `README.md`;
- `notes/**`, the dated working notes (interface negotiations, proposals,
  audits, development records, the commit map). They cite sibling
  repositories and internal labels, so they are not user pages;
- `library/**`, the hand-written descriptions and archived captures that
  `tools/generate_library_docs.py` reads;
- `reference/**`, the inputs of `tools/api_reference.py`, which are JSON and
  not pages;
- the legacy user documents (`LIBRARY_AND_ASSEMBLY_GUIDE.md`,
  `LIFECYCLE_CONTRACT.md`, `THREADING.md`, `CONCISE_STATE_AND_COLLECTIONS.md`,
  `APPLICATION_TEXT_LINKAGE.md`, `COMPILER_CACHE.md`, `WINDOWS_WINE_HOST.md`,
  `CURRENT_API_REFERENCE.md`). The concept pages supersede them for readers
  of the site; the files stay where they are because other repository
  documents link to them.

When you add a page, add it to a toctree. When you add a non-page file, add
it to the exclude list with a comment saying why.

## Writing rules for site pages

- Present tense, addressed to a user of the library.
- No milestone numbers, no internal claim labels (the given/observed/measured
  vocabulary), no agent or role names. Those belong in `planning/`,
  `experiments/` and `results/`.
- Link only to pages inside `docs/`. Refer to repository files outside
  `docs/` in backticks (`planning/...`); MyST treats a relative Markdown link
  that leaves the source tree as a missing cross-reference, which is an error
  under `-W`.
- ADR text is included, not copied. Edit `decisions/ADR-NNN-*.md`.
- Keep a hand-written page to roughly 150 lines; split rather than scroll.
- Keep the page count small. Furo writes the whole navigation tree into every
  page, so build time and output size grow with the square of the number of
  pages. Generated material goes into a few grouped pages, never one page per
  declaration; the earlier per-type atlas and clang reference reached about
  1,700 pages and exceeded the Read the Docs build limit.

## Generated inputs

Regenerate before building when headers or the registry changed:

```sh
python3 tools/control_registry.py check          # validates manifests/controls.json
python3 tools/control_registry.py render         # writes controls/generated/
python3 tools/generate_library_docs.py           # writes docs/api/
```

`docs/library/README.md` describes where the API reference's descriptions
come from. The `docs` CI job builds the site; the `house-style` job runs the
registry checks, the generator's unit tests and
`generate_library_docs.py --check`. These checks do not rewrite stale output,
so a stale generated page fails the job instead of being silently rewritten.

## The clang declaration inventory is checked by maintainers, not CI

`tools/api_reference.py` keeps an exact declaration inventory,
`docs/reference/inventory.json`, extracted with `clang -ast-dump=json`, and
validates the reviewed contracts in `docs/reference/contracts/` against it.
It no longer writes site pages; `--markdown <dir>` and `--output <dir>` render
its own Markdown or HTML on request. `python3 tools/api_reference.py --check`
compares the committed inventory byte for byte with a fresh dump. The dump
differs between compiler builds, so the check runs locally with LLVM 22.1.8,
the toolkit's compiler, not on CI runners. Making the inventory
compiler-independent is open work.
