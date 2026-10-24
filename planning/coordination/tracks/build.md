# Track: build, CI and tooling

Written 2026-10-21 by the coordinator at wind-down. The first worker (w1, `D:\gui_forms_w1`) is idle at about 750k tokens; replace it with a `gui-forms-worker` rather than resuming it.

## Where it stands
- **Merged:** the CI speed line #219/#218/#217/#226 (test jobs, skip unchanged setup, sanitize only the needed targets, shared sanitizer cache; HarfBuzz `-pthread` reconfigure fix). main 7ce8f0b2.
- **Open:**
  - #241: the option-11 label (`gallery_accessibility`).
  - #242: the CI seed digest fix (re-fetch a truncated download; skip a bad seed).
  - #234: smaller sanitizer debug info. Green at 23b7eb79, but it stays a draft until the owner's M4 runs the macOS LSan check in `MAC_LSAN_DEBUGINFO_HANDOFF.md`.

## Next
1. After #241 lands, consider `--skip-labels gallery_accessibility` in `D:\gui_forms_workers\sanitize.sh`.
2. `sanitize.sh` passes `--test-jobs 4` now that #219 is merged (coordinator edit).
3. #234: act on the owner's M4 result.

## Rules
Follow `BRIEF.md` "Verification and CI". Full-g debug info is switched on by a flag the build tool checks, for hunting sessions only (owner decision).

## Next (2026-10-22)
First task: change-based test selection and skip-unchanged CI. See STATE.md "PLAN FOR 2026-10-22".

- **Chain result:** #237 merged (192/192 in the integration build) and #244 retargeted to main. #242 merged (197/197); main a4f6ccc8. **#234 now CONFLICTS with main**, probably in ci_build.py after the #219 line merged. It needs a rebase (the M4 LSan check still holds if the conflict resolution leaves the flags unchanged), then CI and a merge. First item on the build track.

## 2026-10-22 (gui-forms-worker in w1)
- **#234:** main merged in at 7fe326c7 (conflict only in native.yml's sanitizer job: main's restore-only cache and seed vs the debug-info mode). tools/ci_build.py and tools/sanitizer/ are byte-identical to 23b7eb79, so the M4 LSan check holds. Resolution: mode step before the cache restore, mode stays in the key; only line-table runs find or pack a seed. CI pending.
- **#241:** green, mergeable, marked ready (no merge needed).
- **Change-based selection (plan steps 1-2):** local branch claude/test-areas in w1 (d5eb8db9), on top of #241 + main (merge 674e8270; rebase onto main after #241 lands, `git rebase --onto origin/main 674e8270`). tools/test_areas.json (areas + path rules), cmake/TestAreas.cmake, tools/change_selection.py (+ test_change_selection.py, 25 tests), ci_build.py --changed-from, native.yml sanitizer job (fetch-depth 0, --changed-from base sha on PRs only).
- **Measured before (CI):** run 39740993646 (main push): sanitizer job 23.8 min (build 18.3 min at 0% ccache hits after a third_party key change, tests 46 s); native Linux 20.8 (build 10.7, tests 16 s), Windows 13.4 (build 7, tests 23 s), macOS 8.5 (tests 105 s); programs jobs 6.6-11 min each, their Test step 1.4-2.2 min re-runs the whole suite. PR runs 39737781850/39737849194: sanitizer 17.6/17.8 min, natives about 1-2 min (reused). Conclusion: in CI the build dominates; test time is at most 2 min per job. Selection pays only through the narrowed build.
- **Next:** local end-to-end check of --changed-from (codec-only and controls-only changes), push PR, then steps 3-4 (programs job re-running the whole suite; docs-only PRs skipping native jobs; sanitize.sh/integration_build.sh using the selection).
- **#251 opened** (claude/test-areas, head 942eeccd, base main, contains #241: merge #241 first). Also fixes #217's narrowed build in fresh trees (unbuilt tests had no command in ctest's JSON, so they were reported Not Run; now read from CTestTestfile.cmake). Local probes on the w1 sanitizer tree: stx_png one-line change: 15 tests, 73 s whole run; src/core one-line change: 179/199 tests, tests 76 s (both gallery a11y tests left out; before, UIA alone took 741 s). CI before: 31 PR runs, mean 45.0 job-min, 48.5 wall, sanitizer 20.9 min. CI after: read from #251's own run (touches everything, so full) and the next codec/controls/docs PRs.
- **Expected conflict:** #234 and #251 both edit ci_build.py's build_toolkit signature and main(); whichever lands second needs a local merge (keep both parameters).
- **Decisions in force:** main pushes and the native/programs jobs keep the whole suite (release record, exact-tree receipts); selection applies to the PR sanitizer job and local runs.
- **Next (after #251 lands):** sanitize.sh `changed:<ref>` mode (+ `--skip-labels gallery_accessibility` default for local runs), build.sh/integration_build.sh via `python tools/change_selection.py --tree <tree> --base origin/main` (prints a -R pattern, ALL or NONE) - coordinator owns integration_build.sh. Step 4: programs job re-runs the whole suite (216/198/199 tests; 132 s macOS, 27 s Linux, 82 s Windows) that native already ran on the same tree; docs-only PRs still run the native matrix.
- **Windows toolchain incident (2026-10-22, urgent from coordinator):** #252 (claude/windows-toolchain-pin, head 5735083a). Cause: restore's release-listing discovery intermittently missed the published LLVM 22.1.8 archive (71 build releases hold it; spec a6c66b10, archive de97be8a), fell through to setup-msys2, which now installs LLVM 23.1.3. Fix: pin file tools/windows_toolchain_release.json (build-5bb7a8ba, prune-protected) read by tag first; ToolchainUnavailable instead of silent install; native.yml dispatch input package_windows_toolchain is the only way to the mirrors; pack refuses a clang not matching spec llvm. Rerun after #252 merges: #234 (native), #246 (native), #243 (programs). Unpinned still: stx-packages.yml and stx-vorbis.yml (install current clang, so now LLVM 23); separate fix, not done.
- **#253 (claude/stx-windows-toolchain, head e87aed94, stacked on #252; retarget to main after #252 merges):** stx-packages.yml and stx-vorbis.yml restore the pinned toolchain via `tools/windows_toolchain.py restore` (checkout at workspace root, so no gui_forms/ prefix). Decision: libvorbis/pkgconf **dropped** on Windows (not in the pinned set; mirrors have only LLVM 23 builds). Windows configures STX_VORBIS_REFERENCE=OFF and skips differential.py and check_pcm.py; the oracle runs on macOS and both Linux runners. Both workflows also trigger on tools/windows_toolchain*. CI pending.
- **Next:** plan step 4 (programs job stops re-running native's suite on the same tree), then step 3 (full GUI/a11y only in integration builds touching those areas, plus once before each release record).
- **#254 (claude/programs-reuse-native-tests, head 53b8ecdd, base main) - plan step 4, programs part:** ci_build.py writes ctest-native.xml (JUnit); cache_manifest.py records `validated_tests` (passed only; reuse carries the producer's list); release.yml on PRs from native extracts cache-manifest.json from the archive and runs only `tools/native_tests.py select` (Gallery/demoboard tests + anything native skipped); ALL when no list; main pushes (release record) and direct runs keep the full suite. Expected saving ~4-5 job-min per PR run (programs Test steps 0.4-2.2 min). Measure from #254's own run. Conflict risk with #251/#234: ci_build.py's ctest call in build_toolkit (trivial).
- **Seen, not fixed:** linux-x64 "Install Linux dependencies" takes 7-11 min on some runs (apt), in native and programs; bigger than step 4's saving. Candidate next item.
- **Step 3 still open:** native PR jobs skipping gui/a11y labels unless touched needs #251's labels and selection; design to propose to the coordinator once #251 lands (native PR job uses --changed-from; integration build runs full GUI/a11y when a batch touches those areas; main push = release record keeps all).
- **Rule (coordinator):** edit scripts live in D:\gui_forms_w1\dev\, never in a shared scratchpad.
- **Linux apt item (coordinator: next):** cause = azure.archive.ubuntu.com mirror at 40-260 kB/s (run 39806663239 programs linux-x64: 173 MB in 11 min 13 s; apt.llvm.org's 140 MB in <1 s; Ubuntu ~30 MB took the rest; cmake 11 MB alone 4.4 min). Branch claude/linux-apt-cache (52fa71c6): tools/install_linux_ci_packages.sh <toolkit|programs> <archives> (same package sets as before), apt Dir::Cache::Archives in an Actions cache keyed by script hash + ImageOS-ImageVersion (restore-keys without the image), apt.llvm.org debs (`*~++*`) dropped from the kept set, saved on non-PR events only. Used by native.yml sanitizer + native Linux and release.yml programs Linux. Before-survey: dev/apt/survey.tsv. After: first PR runs after a main push saves the cache.
- **#234 re-merged with main** (5e2f1eb6) after #252: conflicts only in ci_build.py/test_ci_build.py against #241's skip_labels (build_toolkit takes debug_info then skip_labels; both test classes kept). Flag definitions (SANITIZER_FLAGS, SANITIZER_LINK_FLAGS, DEBUG_INFO_FLAGS, DEBUG_INFO_VARIABLE, sanitizer_options, debug_info_mode) compared programmatically: identical to 23b7eb79; tools/sanitizer/ unchanged. CI pending.
- **#256 (claude/linux-apt-cache, 52fa71c6)** opened. Before (40 native.yml runs 2026-10-22): x64 install step mean 1.89 min native, 3.04 programs, 0.95 sanitizer; p90 7.2/11.5/1.6; arm64 0.55 (never slow). Summary in D:\gui_forms_w1\dev\apt\summary.txt. After: needs a main push to save the cache first; read the PR runs after that.
- **Next:** after figures for #256 and #254; step 3 design once #251 lands.
- **#251 (head a9c40c16):** main merged in (clean; #241, #252 now in main), and a test added for the --tests path on a fresh tree (TestsPatternListingTests). Confirmed: test_listing fills unbuilt commands for --tests as well as --changed-from. On the w1 sanitize tree, the state-machine worker's pattern (png_malformed_sweep, stx_png_core, engine_expression/input/motion/relief, paint_group) resolves to all 7 executables. Main still has the bug until #251 merges.
- **2026-10-22 late:** main b85bf91a has #234 and #254. #251 merged with main at 4845e695 (build_toolkit(..., debug_info, skip_labels, changed_from); the selection feeds the sanitized ctest, and the native run keeps --output-junit). #256 merged with main at bbbdd114 (native Linux step = apt-cache script + main's MSYS2 comment). Tool unit tests pass on both. CI pending. #253 waits on #249 (not mine).
- **#267 (claude/ci-one-tree, head 46640ddc, base main): one tree per job.** Cause of the recurring "missing tool" failures: pull_request runs take their YAML from refs/pull/N/merge, but native.yml (sanitizer, native) and release.yml checked out `head.sha`. Fix: the default ref (merge commit on PRs, pushed commit on pushes) everywhere. The [sanitizer-full-g] check reads the head message by sha. Packages name github.sha. ci_reuse.merge_of_head accepts a receipt whose revision is a merge commit with the producer's head as a parent and the claimed tree (for branches behind their base); up-to-date branches reuse exactly as before. Workspace was clean after the shutdown. CI pending.
- **T2 E11, the engine gate evidence harness (owner: seal T2 tonight). #280 (claude/t2-e11-gate, head 9422ea57, base main).** The test is tests/engine_gate_tests.cpp (CTest gui_forms_engine_gate_tests).
  - It goes through builtin_primitive_registry(), so new primitives (E9.6, E10) are covered once they register.
  - Each primitive is drawn at 3 states × 4 scales and the part rasters are hashed (FNV-1a). There are 12 primitives and 144 images.
  - A settle-then-125-idle-polls check runs per primitive, plus a ribbon motion idle check and a ribbon reduced-motion check.
  - GUI_FORMS_ENGINE_GATE_MANIFEST writes engine-gate-<platform>.tsv.
  - native.yml: the native job sets the variable and adds the manifest to the evidence artifact. A new `engine-gate` job runs tools/engine_gate.py compare, which fails on a hash difference, a missing primitive or a busy idle counter. It writes record.md to the step summary and to the t2-engine-gate-record artifact.
  - results/2026-10-22-t2-engine-gate/ holds README.md (method, the existing cost-fallback and reduced-motion tests, gaps) and record.template.md.
  - Local: Release and sanitized runs pass, their manifests are identical, and every idle counter is 0. The 9 tests in test_engine_gate.py pass, and the checkers report 0 findings. CI pending.
  - **Next:** when a run has all four manifests (a source-changing PR run; reused validations show as missing), commit its record.md and comparison.json into results/2026-10-22-t2-engine-gate/. That seals parts 1 and 2. Not covered: the composite through each host painter, and the specimen sheet.
- **2026-10-23, #280 update:** run 39910508309 had all four hosts: 12 primitives, 144 images, identical everywhere, every idle counter 0. On the coordinator's instruction it is committed as an **interim** record under results/2026-10-22-t2-engine-gate/interim/ (head 83af9208). The sealed record is its own small PR, from a run on main after the last T2 primitive (E9.6 G1-G4, E10 #282) lands.
- **#284 (claude/stack-frame-guard, base main): stack-frame guard.**
  - tools/stack_frames.py check --tree <tree> [--limit 262144] reads every function's prologue in first-party objects (in a <target>.dir, outside third_party/) with llvm-objdump, else GNU objdump. It tracks the registers holding stack addresses.
  - Covered: x86-64 sub/push; the Windows ___chkstk_ms probe; stack-clash loops on x86-64 and AArch64; AArch64 lsl #12 pairs, x9 realignment and movk constants; Mach-O ltmp names from the symbol table.
  - The 12 tests in tools/test_stack_frames.py use listings cross-compiled for all five targets. It reads 16 objects per run in a process pool: 1 min 47 s for w1's 628-object Windows tree.
  - native.yml has a "Check stack frames" step after the sanitizer build (if !cancelled), and the house-style job runs the tests.
  - Coverage: Windows locally, Linux x64 in CI, AArch64 parse-tested only (no CI tree).
  - **Merge after the standby's frame sweep:** its own CI fails on the mains still over 256 KB. w1's tree, which is older than #283, had theme 795 K, part_painting 594 K, composition 413 K, high_contrast 317 K and basic_controls 305 K. The standby's frames2.py (D:\gui_forms_w3\dev) is subsumed.
  - **Suggestion for the coordinator:** sanitize.sh could run `stack_frames.py check --tree <ws>/.build/native-windows-x64/gui-forms-sanitize` after a build.
- **Next:** after-figures for #254 and #256; the step 3 design (full GUI and a11y only in integration builds touching those areas).
- **2026-10-23: #284 merged** (sanitize.sh runs it). **#289 (claude/stack-frames-stale, base main):** stack_frames.py reads only current objects. Objects that `ninja -t targets all` does not list are "no target builds it", and objects in the `ninja -n -v` dry run's `-o` list are "older than its sources". Both are listed apart and never fail. If build.ninja needs regenerating, only orphans are told apart, with a note. Without ninja, every object is read. 17 tests pass, and an end-to-end mini CMake tree behaves as described. CI pending.
- **After-figures** (native.yml runs after #256 merged at 2026-10-22 19:52, 60 runs; script dev/after/survey.py, data dev/after/after.tsv):
  - **#256, apt cache.** The linux-x64 install step on PR runs:
    - Native: mean 0.50 min, p90 0.45. Before: 1.89 and 7.2.
    - Programs: mean 0.37, p90 0.50. Before: 3.04 and 11.5.
    - Sanitizer: mean 0.50, p90 0.47. Before: 0.95 and 1.6.
    - That is about 4.5 job-min saved per PR run, and the 7-12 min tails are gone on PRs.
    - Outliers: two at 9 min, from a stacked PR whose base predated #256 (it still ran the old inline step). On main pushes, 2 of 8 programs x64 jobs took 10-12 min: the debs cache had been evicted, because the repo's Actions cache is at 10.10 GB of its 10 GB limit and evicts least-recently-used first.
  - **#254, programs reuse.** PR programs Test step means: x64 0.19, arm64 0.21, macOS 0.44, Windows 1.39, together 2.2 job-min. The full suite on main pushes takes 0.62, 0.62, 2.13 and 2.04, together 5.4. So about 3.2 job-min is saved per PR run. Windows keeps 1.4 because its Gallery a11y walks run only there.
- **Step 3 design** went to the coordinator as numbered options (see the hand-back).
  - Data: in the integration tree the whole Release suite is 267 s of test time. UIA gallery takes 131 s and MSAA gallery 20 s; the 741 s UIA figure was a sanitizer run. The CI test steps are 2 min or less per job, and a main push already runs everything as the release record.
- **#292 (claude/stack-frames-empty-selection, base main), an urgent fix.** #284's check failed every docs-only PR (#290), because the change selection built nothing and the check exited 2.
  - ci_build.py now writes <tree>.record.json beside the tree: built nothing, selected or everything, plus a reason. The file is removed when a call starts.
  - stack_frames.py passes with "no objects built (...); nothing to check" when the record says nothing was built. It does so before looking for a disassembler.
  - An empty tree that should have objects is still an error (exit 2).
  - 5 new tests. CI pending. The coordinator merges it first, then re-runs #290.
- **Stray file:** D:\d\gui_forms_w1\dev\frames\rec\gui-forms-sanitize.record.json (82 bytes, plus its empty directories) was created by my mistake: an MSYS path was passed to Windows Python. The safety check blocked removing it, so it is left for the owner to delete.
- **Step 3 is decided: option 1.** The coordinator applied it to integration_build.sh: the Gallery a11y walks are skipped unless the batch touches gui or a11y. CI is unchanged. The stray D:\d file has been passed to the owner; do not retry the removal.
- **State: idle.** The track is clear once #292 merges. A successor waits for new work.
- **#294 (claude/engine-gate-push, base main), urgent: the T2 seal was blocked.** Main push run 40019820396 compared 0 hosts and passed.
  - Cause: on a push to main every native job promotes the PR's exact-tree validation and skips build and test, so no manifest is written. A PR job that reuses a validation behaves the same way.
  - Fix, part one: `engine_gate.py carry` reads .ci/reused.json for validation_run, downloads that run's gui-forms-evidence-<platform> and copies its manifest with a provenance comment. native.yml runs it before the evidence upload whenever promotion or reuse is true.
  - Fix, part two: compare now fails on any missing host and when nothing is found. --require-all is gone.
  - 13 tests pass. Checked against the real run: run 40019820396's linux-x64 reused.json carried validation run 40014869777's manifest (12 primitives, 144 images).
  - **Flag for the coordinator:** main f9fde863's builtin_primitive_registry still holds 12 primitives. E9.6 and E10 registered none, so the sealing run will report 12.
- **#296 (claude/build-size-plan, base main, head eaa80579): the build-size plan.** The owner asked to "reduce build sizes". This PR is docs only, with no CI change: planning/BUILD_SIZE_PLAN_2026-10-23.md, indexed in planning/README.md.
  - Recommended order: (d) `ccache --evict-older-than <job age>` before each save on main, (a) prune superseded generations on main and limit the deps-v3 save to main, (b) one native+programs object cache per platform, (e) gc-sections and strip for non-sanitizer test executables, then (c) third-party as release assets.
  - Owner decisions: run prune_cache_releases.py --apply, and stop copying the toolchain into each build-* pre-release.
  - Measurement scripts and data are in D:\gui_forms_w1\dev\sizes\ (saves.py, sections.py, symbols.py, overlap.json, rel.tsv, san.tsv, caches_now.tsv). The cctest\ subfolder holds the ccache eviction trial: a hit refreshes the entry's timestamp and evict-older-than keeps it.
  - Next step: implement each option as its own PR once the coordinator accepts the order. (d) goes first, trialled on one platform, comparing PR hit rates.
- **Owner decisions on #296 (coordinator, 2026-10-23).**
  - **(A) Prune the build-* pre-releases.**
    - The dry run is saved at D:\gui_forms_w1\dev\prune\dry_run.txt: 116 releases, keep 6 (12.9 GB), prune 110 (172.6 GB; per-release sizes in sizes.tsv).
    - The keep list is verified. It holds the pinned toolchain build-5bb7a8ba, the only revision in pinned_cache_revisions.json, and the 5 newest, main head 60f9d036 among them.
    - Receipts and seeds are artifacts, not releases (ci_promote, ci_reuse and ci_sanitizer_seed read no releases). windows_toolchain fetch-recorded resolves through the pin: the archive de97be8a is the same on the newest, an older and the pinned release.
    - **--apply was NOT run.** Permanently deleting releases is an action I may not take on an agent's message; the owner runs `python tools/prune_cache_releases.py --repo falseywinchnet/gui_forms --apply` from a checkout.
  - **(B) #297 (claude/cache-evict-prune, base main, head 3c882e9b): plan steps 1 and 2.**
    - Adds tools/ci_cache.py (evict and prune) with 13 tests, a cache-prune job (actions: write), and evict steps before the sanitizer, native and programs saves.
    - Dependencies are now saved only on main.
    - Eviction uses a 300 s margin, because of a whole-second timestamp boundary seen in local trials.
    - The prune family is the key before its last hash.
    - The cache dry run (D:\gui_forms_w1\dev\prune\cache_dry.txt) would delete 3 entries, 1.17 GB.
    - Next step: after #297 merges, read the first main push's eviction logs (before/after sizes) and the cache listing, then propose steps 3–5 with measurements.
- **#306 (claude/version-0.2.0, base main, head ca1cc3f7): the 0.2.0 baseline, rebased on 8d18d3af (#305).**
  - The project VERSION goes to 0.2.0.
  - GUIFormsConfigVersion is SameMinorVersion, so every `find_package(GUIForms 0.1` becomes 0.2: README, consume-the-sdk, examples/reference, examples/paint_contract (2), and the file_manager_demoboard installed consumers (2).
  - Also updated: the MACOSX_BUNDLE_SHORT_VERSION_STRING defaults (packaging overrides them anyway), the Linux AT-SPI toolkit_version, the api_reference_render header, architecture.md and tools.md.
  - Left alone: the library VERSION/SOVERSION, the stx_* projects, the historical manifests and notes, and the tool versions.
  - Not done, flagged to the coordinator: packaging/RELEASE_NOTES.md still describes 0.1 and is appended to the v0.2.0 release; docs/release has no 0.2.md; the roadmap has 0.2 unmarked.
  - No tag is pushed: the coordinator pushes v0.2.0.
  - **Release notes drafted in #306 (head 9604241f):** the coordinator asked for them, and the owner reviews the text before the tag.
    - packaging/RELEASE_NOTES.md is rewritten for 0.2.
    - The 0.1 text moved to docs/release/0.1.md; the new docs/release/0.2.md includes RELEASE_NOTES.
    - The roadmap marks 0.2 shipped (STAGES, the roadmap.md section, and the SVG fill hand-edited because fontTools is not installed).
    - Next step: apply any edits the owner makes to the notes.
  - **#306 fix:** CI failed on gui_forms_api_reference_tests, whose golden digests (tests/api_reference_fixture_hashes.json) cover the version in the rendered header. The test runs locally once env.sh puts clang++ on PATH.
    - I rendered the fixture site with the 0.1 header and with the 0.2 header (dev\golden\regen.py). The 0.1 render reproduced every committed digest; the 20 HTML pages differ only in the header line.
    - The 20 HTML digests are updated and the test passes.
- **Build wall-time items (the owner: "it shouldnt take 16 frickin minutes"; run 40053146589 took 12.3 min). Approved as proposed; start only after the coordinator says the v0.2.0 release has finished.**
  1. Programs jobs per platform: split native into four calls of a per-platform reusable workflow (platform input). Each programs-<platform> job needs only its own native job and calls release.yml with a platform input that narrows its matrix. Publishing moves to a job that needs all four. engine-gate and cache-prune still need all four native jobs. Fallback if nesting blocks this: fold the programs build into the end of each native job. Report before/after wall time.
  2. A v* tag release reuses main's validated binaries through the exact-tree receipt and redoes only packaging and smoke tests with the tag's version. Never copy main's release under the tag name. Today release.yml has no reuse path and native.yml does not run on tags.
  3. List each native job's fixed-cost steps over 20 s (durations from run 40053146589) and propose cuts.
  - Each is a small PR with before/after wall clock.
- **CI speed (1): #307 (claude/ci-per-platform, head 6587c99a).** Adds native-platform.yml (called once per platform, job names kept, so ci_reuse still matches), a per-platform programs call (release.yml platform input, matrix chosen in prepare), and publish-release.yml (used on main by native.yml after all four programs jobs, and by release.yml for tags and dispatch).
  - Before: run 40053146589 took 12.3 min. Predicted after: about 11.0 min, with the Windows programs job on the critical path.
  - A watcher merges green PRs, so never push half-done work.
- **(3) Fixed-cost steps over 20 s** from run 40053146589 (raw data in D:\gui_forms_w1\dev\split\steps_before.txt):
  - Windows native: toolchain restore 40 s, text-mask test 39 s.
  - macOS native: brew deps 32 s, text-mask 26 s, prepared-frame exposure 44 s.
  - Linux arm deps: 29 s.
  - Programs jobs:
    - download of the native cache artifact: 67 s (Windows) and 82 s (linux-x64), because the archive is 530-643 MB and unevicted on PRs;
    - Windows toolchain: 46 s; macOS deps: 40 s; macOS configure: 33 s;
    - Windows Test: 95 s.
  - The tag run v0.2.0 (40054254373) took about 6 min. Its Test step was 136 s (macOS) and 149 s (Windows), because a tag tests everything; builds were 35-57 s from cache.
  - Proposed cuts:
    - (a) evict before archiving on every full native build, PRs too, so the artifact is about 25 MB;
    - (b) fold programs into the native job (fallback design), which removes the second toolchain restore, deps install and download;
    - (c) cache brew bottles on macOS;
    - (d) look at which Windows programs tests run.
- **(2) is pending a coordinator answer.** The tag's binaries come from a ccache rebuild, not reused artifacts. Reusing them through the receipt saves the Test step.
- **Coordinator decisions:**
  - Main is protected. The required checks are now the four "native (...) / build" names, plus house-style, docs, Sanitizers and engine-gate. Use auto-merge: `gh pr ready N && gh pr merge N --auto --merge`.
  - (2) is option 1.
  - (a) is approved.
- **Prepared locally, waiting for #307 to merge:**
  - claude/ci-evict-pr-archives (ac2f9c3e): evict before archiving on every native build.
  - claude/tag-reuse-tests (935c1931): tools/release_tests.py and its tests (8), plus the release.yml step "Carry main's tests of this tree" (tags only), which skips Test.
  - Both are based on #307's head. Rebase onto main after the merge, push, open each PR and turn on auto-merge.
- **#307 merged (e939125d), measured:**
  - PR run 40054828684: 11.4 min, against 12.3 before.
  - Main run 40055395145 was cancelled at 9.6 min by the next merge, because main's concurrency group has cancel-in-progress true. Its sanitizer and macOS programs jobs were cancelled, so it did not publish.
  - Main before (40051828253): 13.6 min. The native jobs were promoted and ended at about 3.6 min; the Windows programs job ran 7.9 min (full Test); release took 1.5 min.
  - **#309 (tag carries main's tests) merged at 89635628.**
  - **#308 (evict PR archives) is open with auto-merge.**
- **Ideas to propose:**
  - Main programs could carry the PR run's programs receipts for the same tree; strict up-to-date makes the trees equal.
  - Main's concurrency should not cancel the previous main run.
- **2026-10-23: exact-tree carry for programs, Sanitizers and docs: #310 (claude/ci-tree-reuse, 5ae41e21), auto-merge on.**
  - Owner's decision (option 3, then extended): main accepts the PR's narrowed evidence for programs and Sanitizers, and always for docs. A daily schedule (03:41 UTC) runs everything as the backstop, with force_validation, its own concurrency group and no publish.
  - New tools/ci_tree_receipt.py: write/lookup for the artifact gui-tree-<kind>-<tree>. A carried job records carried_from.
  - New release_tests.py `merged`: takes the merge commit's second parent, finds that head's PR run, checks the revision in packages-<p>.json has this tree, and carries the ctest receipt with a comment naming the PR run.
  - Before (main run 40056029128): 9.0 min of job wall time and 9.5 min end to end. Windows programs Test 156 s, macOS 122 s, Sanitizers 140+34 s, docs 88 s.
  - **Next:**
    - Measure #310's main push run, then report before and after to the coordinator.
    - When #308 merges, report its before and after, and the ccache hit rate of the first PR run seeded from an evicted archive. (#308 was BEHIND; update-branch was run on 2026-10-23.)
  - **Pending:** the standby (aff5356b32cbb8433) will send a full-tree state-machine checker command. Wire it into the daily scheduled run.
  - Still to raise with the coordinator: main's cancel-in-progress cancels the previous main run's publish.
  - #310 head is now b13d4e0d. It also sets main's cancel-in-progress to false in native.yml and release.yml (coordinator approved). The Sanitizers rule follows the owner's later update: narrowed PR evidence is accepted by exact tree, and the daily run runs the full suite.
- **#308 merged as 54a9b321. Its main run is 40060279674: 11.6 min of jobs, 12.3 min end to end, mostly queueing.** house-style started at 1.8 min and publish waited 1 min for a runner. Programs downloads were 2-3 s, and cache archives were 85-90 MB (macOS and Windows) and 43-44 MB (Linux).
  - The 530-643 MB "before" figure is out of date. The PR run 40055461176, before #308, already had 86 MB archives, because main's own eviction (#297) shrank the main cache that PR builds start from.
  - #308's last PR run 40059739352 ran eviction, for example linux-x64 going from 48.4 to 47.9 MiB. Its seeds came from main and PR archives with 98.6-100% hits.
  - The cleanest check of seeding from an evicted archive is #310's run 40060302097, now waiting for its result.
- **Seeding from evicted archives (#310's PR run 40060302097): every native job hit 100%.**
  - Windows seeded from 40059739352: 1113/1113.
  - macOS seeded from 40059739352: 1111/1111.
  - linux-arm64 seeded from main run 40060279674, which carries promoted copies of 40059739352's bytes: 1104/1104.
  - linux-x64 seeded from 40059827439, an archive that was not evicted: 1104/1104.
- **#310 merged as caeda828.** Its main run is 40060922612; waiting for its after-numbers.
- **#310's main run 40060922612 (caeda828): 9.2 min of jobs, 12.2 min end to end. All four programs, Sanitizers and docs jobs carried the PR's results.**
  - Job durations before (main run 40056029128) and after:

    | Job | Before | After |
    |---|---|---|
    | Sanitizers | 4.0 min | 0.2 min |
    | docs | 1.7 min | 0.2 min |
    | Windows programs | 5.2 min | 3.1 min |
    | macOS programs | 5.1 min | 3.1 min |
    | linux-arm64 programs | 3.1 min | 2.2 min |

  - The critical path this time was linux-x64 programs (4.8 min). Its build took 179 s with 85 of 796 compilations missed, because the job got a new runner image (20261021.618, against 20261012.601), which is a new compiler identity. That is not caused by #310, and it should clear once main saves a cache under the new image.
  - Without it, the critical path would have been Windows programs ending at 6.5 min plus publish, about 7.9 min.
  - Next: confirm from the next main run.
