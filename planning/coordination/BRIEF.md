# GUI.Forms stage W: worker brief

You are implementing one or more Windows Forms controls for GUI.Forms, a C++20
retained GUI library (CPU-only Skia/DIB painting, native hosts for Windows,
macOS and X11). A coordinator (another Claude session) hands out the work,
reviews it and integrates the pull requests. The owner is GitHub
`falseywinchnet`; pull requests land through GitHub auto-merge (see "Merging"). Do not ask the owner questions; when
you hit a decision that is the owner's, take the conservative option, keep
going, and list it in your final report under "Owner decisions".

## Agents (owner policy, 2026-10-19; lasting)

- Do not start agents of any kind: no subagents, helper agents, background
  agents or workflows. Only the coordinator (the agent holding the owner's
  admin hat) starts agents.
- If you need a standalone task outside your domain, promote it to the
  coordinator instead: stop, and report the request (what, why, the inputs,
  what you need back). The coordinator routes it to the one standby agent
  that handles out-of-domain work, and returns the result.
- The reason is the owner's: "trees of families of agents may not
  necessarily adhere to standards."

## Read first (in full, before writing code)

1. `AGENTS.md` (repository rules and hard boundaries).
2. `planning/PROGRAMMING_HOUSE_STYLE.md`. It is mandatory. The short version:
   - spell every type: no `auto`, no lambdas, no trailing returns, no
     structured bindings, no `= default` comparisons;
   - write `(*pointer).member`, never `->`;
   - no ranges pipelines or coroutines;
   - name callbacks as small structs with explicit context (see
     `EditorChangeCallback` in `src/controls/panel/domain_up_down/domain_up_down.cpp`),
     or bind members with `Delegate<...>::bind` (see
     `src/controls/tool_strip/tool_strip.cpp`);
   - validate before mutating; explicit results for expected alternatives;
     exceptions only for programmer errors at API boundaries;
   - one owner and lifetime per datum; no per-element allocation in loops;
   - match the surrounding code's naming and comment density.
   - **New rules (owner, 2026-10-19, after a sanitizer sweep):** a function
     that returns a pointer or reference into an argument deletes its
     rvalue overload, so a temporary cannot be passed; declare a borrower
     after what it borrows, so it is destroyed first; take a foreign
     entry point's type from its declaration (`decltype` or a
     `static_assert`), never retype it; memory errors and undefined
     behaviour are defects in tests too. Never keep a pointer into the
     result of a call such as `find(x.realized_items(), ...)`: hold the
     returned collection in a named local first.
3. `planning/stages/README.md` (the owner, how to work with them, lessons
   already paid for) and `planning/stages/W-winforms-controls/README.md`.
4. `planning/DESIGN_LANGUAGE_SPECIFICATION.md` §8 (parts, roles, states) and
   §12.7 (the ruling below).

## The ruling you build under (owner, 2026-10-14)

The atom vocabulary is the canonical source of what can be addressed and
manipulated: part roles (`PartRole`), states (`AnatomyState`), value types
and the engine's inputs, in `include/gui_forms/anatomy/types/anatomy_types.hpp`.
A control declares which atoms it instantiates; the theme engine (stage T2,
not built yet) declares which it consumes. Therefore:

- **Paint every part through the by-role paint path**,
  `paint_part` in `include/gui_forms/theme/part_painting/part_painting.hpp`.
  Describe each part with its `PartRole`, bounds, `AnatomyStateSet`, recipe
  family (`ControlVisualRole`) and content (text, image or a `PartMark`).
  Never call `fill_rect`, `draw_line`, `draw_text_utf8`, gradients or
  `paint_surface_material` yourself in a new control. Never hard-code colours.
- **One geometry function per part**, used by both `on_paint` and
  `publish_part_geometry` (see `spin_halves`/`spin_glyph` in
  `src/controls/panel/up_down/spin_buttons.cpp`).
- **Use only existing roles, states and marks.** If a part needs a role,
  state or `PartMark` that does not exist, do not add one: choose the
  nearest existing atom, mark it with a comment, and list it in your report
  as a vocabulary decision for the owner. The same applies to
  `SemanticRole` (there is no grid, table or status-bar role, for example).
- "Flat at rest" and other looks are the theme's business. If a family's
  recipe draws a plate where the design language wants none, report it
  rather than skipping the paint in control code. (The `menu_item` family is
  flat at rest; `ToolStrip` uses it for its items.)

## Docking and PageView work (from 2026-10-17)

Stage DK's plan is accepted: `planning/stages/DK-docking/PLAN.md`, with
decisions 1 to 43 in `planning/stages/DK-docking/README.md` and the
verbatim record in `planning/interview/DOCKING_INTERVIEW_ROUND_001.md`.
Read the plan, the decisions and the phase you are given; the studies
(`DOCKING_STUDY.md`, `PAGEVIEW_STUDY.md` in the same folder) explain the
reasoning. Where the plan and a decision disagree, the decision governs.

- Phase V0 is the one place allowed to add vocabulary atoms, and only the
  ones decisions 35 and 40 list; every other phase uses existing atoms
  and reports gaps as above.
- Renderer-free models (`PageSet`, `DockLayout`) live in the core with no
  control, painter or host dependency, and are tested headless.
- Host work (phase W0) keeps AppKit, Win32 and X11 types inside
  `src/host/<host>/`, and every new request is a capability that reports
  `unsupported` where a host cannot do it (AGENTS.md GIVEN rule).

## Patterns to copy

- `DomainUpDown` (`include/gui_forms/controls/panel/domain_up_down/`,
  `src/controls/panel/domain_up_down/`, `tests/domain_up_down_tests.cpp`,
  `docs/library/docstrings/DomainUpDown.rst`): a composite control over a
  child `TextBox`, parts with their own states.
- `ToolStrip` and `StatusStrip` (`src/controls/tool_strip/`,
  `src/controls/status_strip/`, `tests/tool_strip_tests.cpp`): records laid
  out as an **item system** (`ItemSystemDeclaration`, `realized_item_count`,
  `publish_realized_item_geometry`, `realized_items()`), popups through
  `ContextMenu`, shared `Command`s, keyboard and semantic virtual children.
  Item part names may not repeat the control's own part names (hence
  `item_face`).
- `ObjectView` (`src/controls/panel/object_view/`) for a virtualized
  details list with a `column` item system and sort glyph states.
- `MenuStrip` for popup handling.

## Behaviour

GUI.Forms' own principled behaviour, broadly familiar to a Windows Forms
programmer; not strict compatibility (interview GF009 to GF011; do not copy
historical misordering). Scope is the .NET 8+ surface: look up your type's
rows in `planning/generated/WINFORMS_API_CATALOGUE.tsv` (column `owner`) to
choose the core members, and use GUI.Forms naming (snake_case,
`std::optional` for "no selection", events as `Event<...>& name()`
accessors). Large collections virtualize: only visible rows or items are
realized. Controls stay platform-neutral: no Win32, AppKit or X11 code
outside `src/host/`.

## Checklist for a new control (all of it, every time)

1. Header `include/gui_forms/controls/<area>/<name>/<name>.hpp`, source under
   `src/controls/...`; add the source to the controls source list in
   `CMakeLists.txt`; include the header from the right umbrella header in
   `include/gui_forms/*.hpp`.
2. Anatomy: parts, states, item systems; `Parts`/`States` (and item)
   dot-member structs; `parts()`, `states()`; interactions table in
   `append_interactions` (payload strings must match the `Event<...>` type
   text in the header; `tools/control_schema.py check` verifies them).
3. Semantics: `semantic_descriptor`, virtual children for items, semantic
   actions.
4. Tests: `tests/<name>_tests.cpp` registered in `CMakeLists.txt` next to
   `gui_forms_tool_strip_tests` (link `gui_forms_controls`). Cover behaviour,
   events, keyboard, semantics, published parts and states, refusals.
5. Registry: an entry in `manifests/controls.json`, then
   `python tools/control_registry.py emit-cpp`, `render`,
   `readme-table --write README.md`, `check`.
6. Gallery: a specimen on a fitting page in `src/controls/gallery/pages/`.
   The schema export and the anatomy coverage test find controls only
   through the Gallery.
7. Docstring `docs/library/docstrings/<Type>.rst` in the format of
   `DomainUpDown.rst` (argument-free getters go under Properties, not
   Methods), with a short compiled example; then
   `python tools/generate_library_docs.py` and `--check`.
8. Schema: `sh /d/gui_forms_wN/write_schema.sh`, then
   `python tools/control_schema.py check` and `render`.
9. `python -B tools/check_house_style.py include src tests tools demo`.
10. `sh /d/gui_forms_wN/build.sh '<regex of the tests your change touches>'`:
    those tests must pass. Do NOT run the whole suite yourself; see
    "Verification and CI" below. There is no accepted flake: an intermittent
    failure is a bug to root-cause.
11. `sh /d/gui_forms_workers/sanitize.sh /d/gui_forms_wN '<same regex>'`: no
    ASan or UBSan report (owner, 2026-10-19: sanitizers are a standard part of
    every build tree). A report is a defect to fix, never to suppress.

## Verification and CI (owner, 2026-10-21; lasting)

The owner cut the repeated verification and the waiting, which made work slow
and expensive. One stack, each layer once:

1. **You:** touched tests plus the sanitizer over them (steps 10 and 11), the
   checkers, and the docs/registry/schema checks you touched.
2. **CI:** every platform, plus the Linux whole-suite sanitizer job.
3. **The coordinator's integration build:** one shared tree builds the ready
   PRs together (where their work does not overlap) and runs the whole suite
   once. You do not run `build.sh all`.
4. **Syncing with main is server-side** (`gh pr update-branch`); CI on that
   head is what counts. Merge main locally only to resolve a conflict.

**Never wait on CI.** After pushing: send a short report (branch, head, what
you ran, "CI pending") and END YOUR TURN. Do not poll `gh pr checks`, do not
sleep, and do not start watchers. The coordinator watches CI and wakes you only
when a run fails or when your next step depends on it. Then read the failed
job's log (`gh run view --job <id> --log-failed`) and fix it.

Keep stacks short: at most two PRs on top of each other. A PR is merged as soon
as it is green, so main moves under you less.

`python` means `C:/Users/Shadow/AppData/Local/Programs/Python/Python314/python.exe`
(the bare `python` on PATH is a Store stub). Run tools from the checkout
directory `/d/gui_forms_wN/gui_forms`.

## Your workspace

You own `D:\gui_forms_wN` (N is in your task). Work only there:

- checkout `/d/gui_forms_wN/gui_forms` (Git Bash path), on a local branch at
  the top of the current stack (named in your task);
- `sh /d/gui_forms_wN/build.sh [ctest-regex|all]` configures (first run),
  builds and tests; logs land beside it (`build.log`, `ctest.log`);
- `sh /d/gui_forms_wN/write_schema.sh` rewrites the control schema.
- `sh /d/gui_forms_workers/sanitize.sh /d/gui_forms_wN [ctest-regex]` builds
  and tests the sanitizer tree (`sanitize.log`).

Long builds: run `build.sh` with a generous timeout (10 minutes) or in the
background. Other workers build at the same time on the same 8 cores; do not
raise the job count. Do not touch `D:\gui_forms_main`, the
`C:\Users\Shadow\gui_forms` checkout or another worker's workspace. Do not
install or download anything. Linux and macOS cannot be built here; CI is
their first compile, so keep the code portable and say so in the PR.

## Git

- Create your branch from the current one: `git checkout -b claude/w-<name>`.
- Commit in logical steps. End every commit message with
  `Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>`.
- Push to `origin` (GitHub) and open a pull request with
  `gh pr create --repo falseywinchnet/gui_forms --base <the branch named in your task>`.
  Never push to `main`, and never merge by hand: a ready PR lands through
  auto-merge (see "Merging" below).
- PR description: plain English. What the control does, what it does not do
  yet, what was checked (with test counts), what CI must prove. End it with
  `🤖 Generated with [Claude Code](https://claude.com/claude-code)`.

## Writing

Plain, literal English in comments, docs and PRs; no marketing; say what was
measured and what was not. Docs that disagree with the build are worse than
none.

## Final report (your last message)

Five lines at most unless asked for more. The PR description holds the detail.

- branch and PR URL;
- what is implemented, and what is deferred (and why);
- tests: names and counts of what you ran (touched tests, sanitizer);
- vocabulary gaps (roles, states, marks, semantic roles) and how you bridged
  them;
- owner decisions you took conservatively;
- anything in shared code you changed (for example a new `TextBox` hook),
  with the reason.

- **Scratch files** (2026-10-22): keep edit scripts and logs inside your own workspace (`D:\gui_forms_wN\dev\`). Never write to the coordinator's scratchpad or another worker's files.

- **ci_build `--tests` "Not Run"** (2026-10-22): fixed on main by #251 (fb32cad9). If you see it, merge main into your branch first.

## Merging (owner, 2026-10-24; lasting)
main is protected (8 required checks, up to date before merge), and the repo allows auto-merge.
- When a PR is ready to land, mark it ready and enable auto-merge: `gh pr ready N` then `gh pr merge N --auto --merge`. GitHub merges it when every required check passes, even if this machine is off.
- Only push work that is safe to land alone: green means merged.
- Never use --admin or bypass the checks. Never change branch protection yourself.
- If you rename a CI job, the required check names on main change. Tell the coordinator the new names BEFORE the PR merges.
