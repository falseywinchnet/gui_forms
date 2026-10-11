# Polish stage plan: documentation engine, landing page, control docs, downloadable demos

Date: 2026-10-11. Branch of record: `claude/text-engine-performance`.
Status: proposal with the six owner decisions recorded in section F
(2026-10-11); nothing in this plan is implemented yet.

This record answers "figure out what needs to be done" to take a functioning
engine to the appearance of usability for others. It covers (A) the text
engine checkup, (B) the measured current state, (C) the reference patterns
adopted from bfft, PlaySuite, WinForms, Dear ImGui, Avalonia and Modern.Forms,
(D) the workstreams, (E) sequencing, and (F) decisions only the owner can make.

## A. Text engine checkup (last five commits, 70be8b3..cda98ad)

CI is green on all four platforms for the head commit. Nothing found blocks
merging. Items, smallest first:

1. **Stale living docs still describe GDI/Uniscribe as current.**
   `planning/CONTROL_COMPLETENESS_MATRIX.md` row 7 says "The bounded Win32
   adapter uses Uniscribe placement plus GDI glyph-index output". ADR-004
   phase 6 removed that. Update the row. (`experiments/*` and `results/*`
   are dated records and stay as written.)
2. **Benchmark numbers live only in a commit message.** cda98ad records the
   workload and the stage timings in prose. Repo convention is a dated
   `results/<topic>-<date>/` folder with the raw output. Add
   `results/text-engine-2026-10-11/` from a fresh run of
   `gui_forms_text_engine_benchmark` so the numbers are citable.
3. **`TextLayoutPhase::hb_font` now times almost nothing.** After cda98ad the
   phase measures a cached pointer read except on first use. Any diagnostics
   doc or probe that interprets that phase should say "font creation only".
4. **CoreText rebuild and variable fonts.** `core_text_font_bytes` rebuilds
   the matched face from its tables, so a variable font instance that CoreText
   matched for weight 700 comes back as the default instance; `face_weight`
   then files it in the regular slot. Helvetica (the tested case) is static,
   so the test cannot see this. Either reject faces with an `fvar` table or
   document the limit in the header comment.
5. **Placement of platform code.** `src/render/skia/executor/core_text_font_bytes.cpp`
   is CoreText code under the Skia executor. AGENTS.md admits platform-specific
   CPU details in private Skia extensions, so this is allowed, but it is the
   only Apple-framework file outside `src/host/macos/`. Note it in
   `docs/LIBRARY_AND_ASSEMBLY_GUIDE.md` so the next audit does not flag it.
6. **Smoke scripts hard-code `-eq 12` identity lines and require `rg`.**
   Pre-existing style; acceptable, but the count will silently need editing
   when the identity scene grows.

Verified sound: HarfBuzz font caching is per-engine and engines are per
worker (`worker_probe.cpp` builds its own), so no new cross-thread state;
every shape call still sets the FreeType size first, so the cached scale
and advance cache stay consistent; diagnostic injected fonts are never kept;
move constructor and destructor ordering are correct; the tooltip
device-pixel math terminates and the test now covers 1.1 and 1.25; the sfnt
rebuild header fields (searchRange, entrySelector, rangeShift, 'OTTO') are
right and FreeType ignores the unadjusted head checksum.

## B. Current state (measured 2026-10-11)

- **README.md** (177 lines, last touched 2026-08-10 by an automated backup):
  no badges, screenshots, feature list, license section or consume guide.
  Hard-codes `/Users/quentinkuttenkuler/...`, says the Showcase has twelve
  pages (there are 16), calls the demoboard "not an implemented frontend"
  while its own README says D0–D6 are measured partial, still describes
  Lucida Grande and an unpackaged Windows Skia DLL, and its final bullet is
  a spliced fragment. Tone is internal process prose throughout.
- **GitHub metadata:** description mentions "compiler-cache artifacts", no
  homepage, no topics, Pages not enabled, license detected as "Other"
  because the dedication line breaks licensee's MIT match.
- **Releases:** 24 releases and 23 tags, all `build-<sha>` compiler-cache
  pre-releases whose body says "These are build inputs, not an application
  release". A visitor clicking Releases finds 340 MB toolchain tarballs.
- **CI** builds with `-DGUI_FORMS_BUILD_GALLERY=OFF`; no demo is built or
  smoke-launched in CI. No Linux demo mains exist. No CPack, bundle, zip,
  installer, signing or notarization anywhere.
- **Docs:** `docs/CURRENT_API_REFERENCE.md` is hand-written, dated 2026-08-07,
  and omits ListBox, ComboBox, TreeView, ObjectView, TabControl,
  DateTimePicker, NumericUpDown, PropertyGrid, CorrespondenceView,
  InstrumentRack. `docs/library/` is a 617-page generated atlas
  (`tools/generate_library_docs.py`) with Markdown mirrors, 54 manual
  captures, not run in CI, missing nine newer controls. The clang-based
  reference generator (`tools/api_reference*.py`, CMake target
  `gui_forms_api_reference`) has tests but **no committed inputs**:
  `docs/reference/` does not exist. Twelve of 21 `docs/*.md` are dated
  working notes mixed with user docs. ADRs 001, 002, 006, 013, 014 are cited
  but only 003 and 004 exist.
- **Controls:** 64 headers, roughly 70 public types. No registry links the
  Gallery (4 hard-coded categories), the Showcase (16 hard-coded pages), the
  atlas and the README. No per-control catalogue beyond the atlas mirror.
- **Fonts:** Portsmouth Rapids has no licence file and an open
  redistribution gate, yet every demo bundles it and the SDK `install()`
  ships it. Carlito, Cousine and the Noto faces are OFL.
- **Mockups:** `file_manager_demoboard/` is the fixture-backed explorer
  (opt-in, not in CI, D7/D8/D10 not started). There is no mail-client
  mockup.

## C. Reference patterns adopted

| Source | Take | Leave |
|---|---|---|
| bfft | Sphinx + MyST-Parser + Furo built with `sphinx-build -W`, `.readthedocs.yaml`, RTD badge, `index.md` with User guide / API reference / Project toctrees, `documentation/README.md` for maintainers | Hand-copied snippets (we generate), hard-coded version in four places, no docs CI job |
| PlaySuite | `applications.yml` matrix with package-then-install-then-launch smoke, `publication_version.py` scheme, `publish_tested_release.py` idempotent draft→publish with `SHA256SUMS.txt`, ad hoc macOS signing, zip + pkg + NSIS + deb + tar.gz, `GAME.json`-driven README table regenerated by a workflow, the `04-controls.md` shape (canon table control → header → use, patterns, skins table, lifecycle, "what not to do") | Nothing control-gallery-shaped exists there to copy |
| WinForms docs | Controls-by-function table (Function / Control / Description) plus alphabetical list; per-control hub → overview → how-tos; every paragraph ends in a link | Card-grid landing page (too heavy for one toolkit) |
| Dear ImGui | Badge row at top, screenshot pair right after the first snippet, "the demo is the documentation", dated demo zips with version/branch label, release notes with Breaking Changes separated and grouped by area | Wiki as doc hub (we have a generator) |
| Avalonia | ControlCatalog = one page per control/feature with per-platform heads; per-control page: intro, Common properties table, Example, task sections, See also (API, source, related) | NuGet-only distribution |
| Modern.Forms | Honest status line up top; ControlGallery / Explore / Outlaw as the three sample shapes; per-OS screenshots of the same sample | Single badge buried at the bottom; "no formal API documentation" |

None of the four external projects attach demo binaries to GitHub Releases.
We will, because the user asked for downloadable built programs and because
PlaySuite already proves the pipeline on this toolkit.

## D. Workstreams

Each workstream is sized for one sub-model. Owners follow AGENTS.md
ownership: docs and demo work is control-gallery/root territory; CMake and
workflow changes are root/Skia-host territory.

### W1. Text engine polish (small; do first, on this branch)

Items A.1–A.5 above. Acceptance: matrix row updated, results folder present,
header comment on the variable-font limit, assembly guide note.

### W2. Documentation engine

Goal: one `docs/` site built by Sphinx + MyST + Furo, published on Read the
Docs, with the two existing generators feeding it instead of competing.

1. Add `docs/conf.py`, `docs/requirements.txt`, root `.readthedocs.yaml`,
   `docs/index.md` with toctrees: **Start here** (install, consume the SDK,
   first window, build from source), **Controls** (by-function table,
   alphabetical index, one page per control), **Concepts** (retained tree,
   update scopes and dirtiness, layout, painting and damage, text engine,
   focus/pointer/keyboard, threading, lifecycle, hosts and capabilities),
   **Reference** (generated API), **Project** (ADRs, roadmap, contributing,
   evidence index).
2. Make `tools/generate_library_docs.py` emit its Markdown mirror into
   `docs/controls/generated/` and have Sphinx include it; stop committing
   the 617 HTML pages.
3. Commit the clang reference inputs: run `tools/api_reference.py` to create
   `docs/reference/inventory.json`, seed `docs/reference/contracts/` for the
   stock controls, and add `--check` to CI so the inventory cannot go stale.
   Emit Markdown (or `html_extra_path`) so the site carries it.
4. Move dated notes (`*_2026-*.md`, CI_TIMING, LLVM22 validation, ORCHESTRATOR
   negotiation, FILE_MANAGER_COMMIT_MAP, DEVELOPMENT_001) to `docs/notes/`
   or `planning/`; keep LIBRARY_AND_ASSEMBLY_GUIDE, LIFECYCLE_CONTRACT,
   THREADING, CONCISE_STATE_AND_COLLECTIONS, APPLICATION_TEXT_LINKAGE,
   COMPILER_CACHE, WINDOWS_WINE_HOST as user docs after a tone pass.
5. Retire `docs/CURRENT_API_REFERENCE.md` into the concept pages plus the
   generated reference, or keep it as "narrative reference" with a
   regeneration date and the missing controls added.
6. Write the five missing ADRs (001, 002, 006, 013, 014) from the places that
   cite them, or remove the citations. ADR-013 matters for W5.
7. CI job `docs`: `sphinx-build -W`, atlas regeneration check, api
   reference `--check`. bfft lacks this job; add it here.

Acceptance: `sphinx-build -W` passes locally and on RTD; every public control
type has a page; no dated note sits in the user toctree.

### W3. README, badges, landing, repo metadata

1. New README order: title and one-line pitch; status line in bold
   (Modern.Forms style, honest: "retained, CPU-only, macOS/Windows native,
   Linux X11 opt-in, API not yet stable"); badge row; two screenshots (Gallery
   on macOS and Windows) directly under a 15-line "first window" snippet;
   Downloads (links to the latest app release per platform); Features;
   Supported platforms table; Build from source (commands that work from the
   repo root, not from a parent); Consume the SDK; Documentation link;
   Controls table regenerated from the registry (W4) between
   `<!-- controls:start -->` markers, PlaySuite style; Contributing;
   License.
2. Badges: CI (`native.yml`), Docs (RTD), Release (shields `github/v/release`,
   filtered to app releases once W6 separates them), License MIT, C++20,
   Platforms, Sponsor if `.github/FUNDING.yml` is added.
3. Repo metadata via `gh repo edit`: description without the cache phrasing,
   homepage = RTD URL, topics (`cpp20`, `gui-toolkit`, `retained-mode`,
   `winforms`, `skia`, `harfbuzz`, `macos`, `windows`, `linux`).
4. Remove the hard-coded user paths from README and
   `tools/run_*_in_m4_aqua.sh` (take the path as an argument).
5. Fix the spliced final bullet, the Showcase count, the demoboard
   contradiction and the pre-ADR-004 font/Skia claims; move the milestone
   log to `planning/`.

### W4. Control registry and per-control documentation

1. Add `demo/controls.json` (or `manifests/controls.json`): one record per
   public control: id, type name, header, category (WinForms by-function
   categories: Commands, Text editing, Information display, Selection from a
   list, Value setting, Date setting, Grouping, Layout, Scrolling, Menus and
   commands, Dialogs and popups, Drawing, Diagnostics), one-line purpose,
   WinForms counterpart or "GUI.Forms-only", status (stable / provisional /
   lab), gallery page id, doc page path.
2. Consumers: the reworked Gallery (W5) builds its navigation from it; the
   README controls table and the docs by-function/alphabetical indexes are
   generated from it; a test asserts every header under
   `include/gui_forms/controls/` with a public type appears exactly once.
3. Per-control page template (WinForms + Avalonia synthesis, in the PlaySuite
   `04-controls.md` voice): purpose sentence; canon row (header, type,
   WinForms counterpart); Common properties and events table (code name, one
   line, link into the generated reference); minimal example (compiled as a
   test fixture, not pasted); capture (from the gallery page, W5.4); states
   and enums table; keyboard and pointer behaviour; accessibility default
   adapter; skinning parts; "what not to do"; See also.
4. Write the nine missing control pages first (VisualInspectorView,
   TypographySpecimen, ChoiceGroup, ExpandableSections, CommandBar,
   DropDownButton, ResponsiveTrackPanel, CommandOverflowPanel,
   BreadcrumbTrail), then fill the template for the rest.

### W5. Demo applications

Three downloadable programs, mirroring Modern.Forms' ControlGallery, Explore
and Outlaw, built from `demo/` and only public seams.

1. **Control Gallery rework.** Replace the four hard-coded DML categories
   with registry-driven navigation (W4): one page per control, grouped by
   function, with a properties panel that mutates the live instance
   (Avalonia ControlCatalog shape). Fold the Showcase's 16 pages in as
   gallery pages where they demonstrate a control; keep lab-only pages
   (vsync, seam proximity, layered material) as a "Labs" group. Decide
   whether the Complete Showcase stays as a separate target or becomes the
   Gallery (recommendation: one program, "GUI.Forms Gallery", with the
   Showcase retired after its pages migrate; fewer targets to package).
2. **Explore equivalent.** Package `file_manager_demoboard/` as "GUI.Forms
   Explore (preview)". It is already the fixture-backed, public-seams-only
   explorer mockup and is the admitted ADR-013 exception. Do not write a
   second explorer under `demo/`. Enable its CMake option in the packaging
   job, add a Linux main, and label the download with its STATUS.md state.
   Its AGENTS.md says native accessibility is required for *completion*;
   shipping a preview build does not claim completion.
3. **Outlaw equivalent.** New `demo/correspondence/` ("GUI.Forms
   Correspondence"): a mail-client mockup over in-memory fixtures using
   MasterDetailView, SplitContainer, CorrespondenceView, TreeView, MenuStrip,
   CommandBar, ToolTip, ContextMenu, dialogs. Decided (F.2): the three-pane
   layout is permitted as a gallery demonstration of reusable panel controls;
   ADR-013 is written down with that classification as part of W2.6.
4. **Capture pipeline.** Add a `--capture <dir>` automation verb (the
   Windows probe and PlaySuite's `--dev --script` already do this) that
   writes one PNG per gallery page per platform at scale 1 and 2. Docs (W4.3)
   and README screenshots come from CI, not from Screen Sharing sessions.
5. **Linux mains** for the three programs (`gui_forms_host_linux` exists;
   no `*_main` does). Windows post-build font copy must cover every target
   (two labs are missed today).
6. Each program gets an About page with version, commit, renderer name and
   the instrumentation snapshot (the "diagnostics panel" the proving slice
   already requires), so a download is self-describing.

### W5.7. Gallery pages onto the current authoring (open; next branch)

Status after the 2026-10-11 rework: the Gallery shell is registry-driven,
but its sixteen migrated pages are the Showcase bodies unchanged: 62 raw
subscribe/connect calls, no `gf::on`, no `Value<T>`, no `Command`, and none
of ChoiceGroup, CommandBar, ExpandableSections, DropDownButton or
BreadcrumbTrail. Twenty-nine registry records have no page (DropDownButton,
CommandBar, ReviewCard, TreeView, ObjectView, CorrespondenceView,
ChoiceGroup, BreadcrumbTrail, ColorValueEditor, FlagsValueEditor,
PropertyList, InstrumentRack, GroupBox, Card, ExpandableSections,
ResponsiveTrackPanel, CommandOverflowPanel, MasterDetailView,
ScrollableControl, MenuStrip, ContextMenu, Command, AnchoredPopupLayer,
LiveSurface, TypographySpecimen, VisualInspectorView,
VisualInspectorOverlay, SemanticFeedback, BindingContext). Correspondence is
the only program written on the current authoring (owned `on(...)`
subscriptions, Commands, CommandBar).

Work, one page per commit, built and tested on the M4 through `m4build`:
rewrite each page body on `gf::on`, `Value<T>` and `Command` as
`docs/CONCISE_STATE_AND_COLLECTIONS.md` describes; add a page for every
record above (grouped by registry category) so `gallery_page` is non-null
for all 72; keep `tests/gallery_pages_tests.cpp` green per page; regenerate
the catalog, tables and README after each batch.

### W6. Release pipeline

1. **Keep cache publications here; prune and label.** App releases use
   `v*` tags, which GitHub's `releases/latest` and the shields release badge
   already prefer over pre-releases, so no consumer of the `build-<sha>`
   scheme changes. Add a retention step to `publish_cache.py` (or a weekly
   job) that deletes cache pre-releases no consumer pins; PlaySuite pins
   `5bb7a8b` and File Manager pins per its dependency lock, so the pinned set
   is enumerable. Rename the pre-release title to "Compiler cache <short
   sha>" so the list reads honestly. A rolling tag was considered and
   rejected: it would break sha pinning and the never-replace-an-asset rule.
2. `release.yml` (adapted from PlaySuite `applications.yml`): matrix
   macos-15 / windows-2022 / ubuntu-24.04 / ubuntu-24.04-arm; configure with
   `GUI_FORMS_BUILD_GALLERY=ON` and the demoboard option; build the three
   programs; run ctest; package; install the package on the runner and
   launch each program with `--automation` to a clean close (W5.4 gives the
   capture); upload.
3. Artifacts per program per platform: macOS `.zip` (ditto) and `.pkg`,
   Windows `.zip` and NSIS `-setup.exe`, Linux `.tar.gz` and `.deb`;
   `SHA256SUMS.txt`; test-result receipts. Ad hoc `codesign --sign -` on
   macOS and state plainly in the notes that there is no notarization or
   Windows signing (PlaySuite wording).
4. Versioning: `project(GUIForms VERSION 0.x.y)` as the baseline; main
   publishes `0.x.(y+run_number)`; tags `v*`; release title "GUI.Forms 0.x.y";
   notes from a generated header plus `packaging/RELEASE_NOTES.md` with
   Breaking Changes separated from Other Changes, grouped by control/area,
   one screenshot for anything visual (ImGui shape).
5. Downloads section in README and a `docs/start/download.md` page link to
   `releases/latest` with per-platform caveats (X11 required on Linux, ad hoc
   signing on macOS, SmartScreen on Windows).

### W7. Font rights

Decided 2026-10-11: Portsmouth Rapids is the owner's own typeface and ships
in public downloads under the owner's MIT grant. The owner notes that some
source material it draws on is not open and that the Open Font License is
not wanted; that is the owner's call as author and is recorded here, not
re-litigated. Work:

1. Add `assets/fonts/LICENSE-PortsmouthRapids.txt` (MIT, owner as copyright
   holder) so the font has a licence file like every other bundled face.
2. Rewrite the Portsmouth entry in `assets/fonts/ATTRIBUTION.md`: remove the
   "evaluation build" and "redistribution-rights gate" language and the
   `/Users/quentinkuttenkuler/...` source path; record the licence file.
3. Amend AGENTS.md: the sentence "Production Portsmouth rights and exact
   body/coverage assets remain gates" becomes a statement that Portsmouth is
   bundled under MIT and only body-face coverage remains open.
4. No Carlito-only build variant is needed; identity hashes stay as they are.

### W8. Hygiene

Hard-coded user paths (README, two scripts, ATTRIBUTION); stale README
claims; two Windows labs missing the font copy; CI `GALLERY=OFF` (keep for
the cache job, turn on in the release job); LICENSE dedication moved to README (F.3);
`docs/README.md` link above the title; `planning/CONTROL_COMPLETENESS_MATRIX.md`
row 7.

## D-bis. Local build constraint (found 2026-10-11)

The project's CMake configure requires LLVM 22.1.x through
`cmake/llvm22.cmake` plus a macOS 14 libc++ runtime prefix that CI builds
with `tools/build_macos_runtimes.py`. The owner's machine has only
AppleClang 21, so C++ changes made here are validated by syntax-only compiles
with the system compiler and by CI on push. Item A.2 (a `results/` record of
the text engine benchmark) stays open until a run on an LLVM 22 machine or a
CI step produces it.

## E. Sequencing

1. W1 and W8 immediately (small, no decisions).
2. W4.1 registry and W7 option next: everything else consumes the registry,
   and packaging is blocked on fonts.
3. W5.1 Gallery rework, W5.4 capture verb and W5.5 Linux mains in parallel
   with W2.1–W2.3 (site shell, atlas into Sphinx, reference inputs).
4. W6 release pipeline once one program packages on all four runners.
5. W3 README last, because its screenshots, controls table and badges come
   from W4, W5.4 and W6.
6. W5.2 Explore packaging and W5.3 Correspondence once the Gallery packages.

## F. Owner decisions (asked and answered 2026-10-11)

1. **One program.** The Complete Showcase folds into a registry-driven
   Gallery; the Showcase target is retired after its 16 pages migrate.
2. **ADR-013 and the mail mockup.** Permitted as a gallery demonstration of
   reusable panel controls. Write ADR-013 down with that classification;
   the Correspondence demo is not a consumer product.
3. **LICENSE.** Move the dedication line to the top of README (bfft
   placement) so GitHub detects MIT and the licence badge is truthful.
4. **Cache releases.** Keep them in this repository; prune unpinned ones and
   retitle them. App releases use `v*` tags. See W6.1.
5. **Portsmouth.** Ships under the owner's MIT grant. See W7.
6. **Docs host.** Read the Docs, for parity with bfft.

All six are now inputs to the workstreams above; none remain open.
