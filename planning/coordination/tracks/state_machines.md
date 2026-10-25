# State-machine hunter fixes track (worker w3)

Workspace: `D:\gui_forms_w3` (checkout `/d/gui_forms_w3/gui_forms`). House style 1 and 9.
Read `D:\gui_forms_workers\BRIEF.md` first. Moved here from `tracks/stage_a.md` on 2026-10-22.

## History: core routing and Windows (w3, 2026-10-22)

State: A-core, A-W2 (UIA provider), A-W3 (UIA Gallery client) and #211 merged.
Since then: the state-machine hunter's findings (#232) fixed in four draft PRs,
each verified locally on touched tests + sanitizer + both checkers; CI pending:
- #243 core/C ABI/render/examples, head 1f1b5dcd (on main b1e37c1f)
- #246 controls/demo, head 24f6cfbc; #247 hosts, head 27f37948;
  #248 tests/stx_vorbis, head 0bc6c348 (all on main 7ce8f0b2)
Hunter on core alone after #243: core F1 3 (all `#if` false positives), F2 0.
Next step: merge the four heads locally (detached), rerun the hunter
(`scratchpad/tools/hunt.sh final`) and report F1/F2 counts; propose hunter
tuning on #232 (drop preprocessor lines in body_statements; accept a fallback
that runs to the function's end, masked_text_box); public-API C3 decisions for
the owner: ErrorProviderChange::has_error, ImageAttributesSnapshot::
has_color_matrix, HostClipboardImageResult::has_image, stx_vorbis
OggPacket/OggStreamEnd::has_granule.

## Owner decisions (relayed by the coordinator, 2026-10-22)

1. TextMaskLimit: give the text-mask cache's entry count its own specific name instead of
   reporting it as `metadata_bytes`.
2. Public `has_*` fields (ErrorProviderChange::has_error, ImageAttributesSnapshot::has_color_matrix,
   HostClipboardImageResult::has_image, OggPacket/OggStreamEnd::has_granule): reshape only where
   the flag can disagree with the value beside it; keep a flag that is the only record of its
   fact. The PR says which changed and why.
Both go in their own PR, with docs updated, after the restack/conflict/hunter steps.

## State (2026-10-22, after restack/conflict/hunter steps)

- Cause of #246-#248 failing everywhere: not a dependency. CI checks out the PR head SHA but
  runs main's workflow, which calls `tools/audit_link_map.py` and `tools/ci_sanitizer_seed.py`
  (added on main after 7ce8f0b2). All builds and 171 tests passed on Linux. Fixed by
  `gh pr update-branch` on #246, #247, #248; no restack needed, each builds alone on main.
- #243 merged with main (b458bb1b): replay `begin_group`/`end_group` added in the compliant form;
  the PNG colour reshape moved to `src/core/image/color_conversion.{hpp,cpp}` (`ColorEncoding`).
  Plus 31c65d11/325c6ada: `apply_operation` and `corner_radii` (new on main) fixed. Head 325c6ada.
- #248 head 37453290: paint_group_tests and part_composite_scene.hpp switches (new on main) fixed.
- #250 (`claude/state-machine-hunter-preprocessor`, on main): hunter drops preprocessor lines in
  body_statements; unit test added.
- Hunter over main + #243 + #246..#248 + #250 hunter: F1 3, F2 0. The 3 are the listed false
  positives in controls (fallback computed then returned at function end: masked_text_box,
  button_base, container_layout_utilities); left unedited.
- Tool gap (for the build worker): `ci_build.py --sanitize --tests <regex>` leaves some matching
  tests unbuilt (Not Run: png_malformed_sweep, stx_png_core, engine_expression/input/motion/relief,
  paint_group). Worked around by building those targets in the sanitizer tree by hand.
- Windows "Expected LLVM 22.1.8; got clang 23.1.3" CI failures are a toolchain-pin issue the build
  worker is fixing (coordinator, 2026-10-22).

## Open PRs (2026-10-22, end of this pass; all drafts, the coordinator merges)

| PR | Branch | Head | Base |
|---|---|---|---|
| #243 | `claude/state-machine-fixes-core` | 325c6ada | main |
| #246 | `claude/state-machine-fixes-controls` | update-branch merge | main |
| #247 | `claude/state-machine-fixes-hosts` | update-branch merge | main |
| #248 | `claude/state-machine-fixes-tests` | 37453290 | main |
| #250 | `claude/state-machine-hunter-preprocessor` | (tools only) | main |
| #258 | `claude/state-machine-owner-decisions` | 87b4471b | #243 |

#258 does both owner decisions: `TextMaskLimit::cache_entries`; has_* reshapes (all four listed
records changed: ErrorProviderChange::has_error removed; ImageAttributesSnapshot::color_matrix,
HostClipboardImageResult::image, OggPacket::granule/page_granule, OggStreamEnd::granule optional).
Verified: 13 touched tests plain + ASan/UBSan; text-mask tests in `.build/textmask`
(GUI_FORMS_BUILD_TEXT_MASKS=ON, plain only); stx_vorbis standalone `.build/stx_vorbis_{OFF,ON}`
7/7 each (`sh /d/gui_forms_w3/extra.sh`). Worktrees: wt_controls, wt_hosts, wt_tests, wt_hunter.

## Next step

Wait for the coordinator: fix CI failures on #243/#246/#247/#248/#250/#258 when woken (Windows
LLVM-pin failures are the build worker's). When #243 merges, retarget #258 to main.
Optional hunter tuning still open: accept a fallback computed and then returned at the function's
end (the 3 listed controls false positives); not done, not asked.

## Standby: Window::take_damage() leaves paint_dirty_ stale (2026-10-22)

Confirmed by test (fails on main): after replace_png (damage on every plane, no dirty control), taking
all damage left needs_frame() true and wake requests armed. #266 (`claude/window-damage-paint-dirty`,
on main, head 535f132f): paint_dirty_ removed; `Window::paint_pending()` derives from
dirty_after_render_, root/popup paint-dirty subtree flags and plane damage. 25 paint/frame/host tests
plain + ASan/UBSan. macOS #249 "frame pending" is needs_frame(); consistent with this bug, but
confirmation needs #249's CI on top of #266. Possible second route (unconfirmed): a paint-dirty control
whose damage bounds are empty keeps needs_frame true with nothing for the host to draw.
2026-10-22 later: main (c123b6e9, with E7r #261) merged into #266; E7r's idle keyframe bake now reads
paint_pending() (head 17784ac4). 40 window/scheduler/engine/paint tests plain + ASan/UBSan pass.
#249's macOS dump: "frame pending yes ... paint lease dirty_queued content 259 rendered 259
presented 259": everything rendered and presented, yet the stored flag said dirty, which is the stale
paint_dirty_ this PR removes.

## Standby: paint lease stuck dirty_queued with nothing owed (2026-10-22)

#249 on macOS, after #266 merged: "dirty_queued, content = rendered = presented = 259". Cause: owed
work included the root's paint-dirty subtree flag; a control no paint reaches (outside the client
area, or clipped by a viewport) keeps its mark forever. #275 (`claude/paint-lease-owed`, on main,
head c3fb6ff1): paint_pending() = dirty_after_render_ || plane damage; paint_lease_state_ removed,
paint_lease_state() derives it (no hand-assigned states). Test an_unreachable_dirty_control_owes_no_frame
fails on main. 43 tests plain + ASan/UBSan, 26 wider plain. Windows/Linux hosts never read lease
state or needs_frame. Next: #249's macOS CI on top of #275 confirms.
Standing check (coordinator, 2026-10-22): when a change removes or renames a member, run
`python tools/generate_library_docs.py` then `--check` (manual.json method explanations go stale).
#275 docs fix pushed.

## Hunt 2 and the gate (2026-10-23, WIP, interrupted by shutdown)

Branch claude/state-machine-hunt-2 (draft PR, head ec782bc9, NOT verified; no auto-merge).
Hunter over main 89635628 (Windows tree): F1 11 + 3 in newly scanned stx/fuzz dirs, F2 2;
after fixes + tuning the full Windows run is F1 0, F2 0 (C1 10, C2 31, C3 2).
Fixed files: material_primitive, procedural_pattern, relief_primitives, semantic_snapshot,
windows_text_automation, cng_pixels, backplane_composite, fuzz/inflate_protocol_fuzz, stx_cng/tests/common.hpp.
Hunter: tuning (fallback at block/function end; nested-loop break; fallthrough), stx/fuzz dirs,
--changed-from/--list-changed/--gate; 26 unit tests pass. Workflow .github/workflows/state-machines.yml,
job name `state-machine-gate` (Linux, Skia and hosts off).
Owner approved the gate as a required check, new code only; coordinator adds it after first green main run.
Left: touched tests plain + ASan/UBSan, house-style + lifetime checkers, first CI run (full tree since
checker changed; Linux Skia-off configure untested), timings (one-file PR, no-source PR), daily full-tree
command to the build worker (ae67c70cef46d7057): `python3 -B gui_forms/tools/check_state_machines.py
--root gui_forms --build-dir <tree> --jobs 4 --gate --json state-machines.json`, results dir
results/2026-10-23-state-machine-hunt-2/ (raw outputs in D:\gui_forms_w3\dev\hunt2\), C3 list for owner
(MonthCalendar::today_set_, doc example Reminder::has_date). PR body: D:\gui_forms_w3\dev\pr_bodies\hunt2.md.
