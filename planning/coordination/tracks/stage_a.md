# Stage A track (worker w4)

Updated 2026-10-22. Workspace: `D:\gui_forms_w4` (checkout `/d/gui_forms_w4/gui_forms`).
Read `D:\gui_forms_workers\BRIEF.md` (rules, including "Verification and CI": never wait on CI,
touched tests + sanitizer only, `gh pr update-branch` to sync, stacks at most two deep).

## Open PRs (all drafts; the coordinator merges)

| PR | Branch | Head | State |
|---|---|---|---|
| #224 | `claude/llvm-static-runtimes` | `000b8bf7` | Windows binaries link the LLVM runtimes and winpthreads statically; GCC cross path retired. Green. |
| #229 | `claude/third-party-notices` | `34e42fdf` | THIRD_PARTY_NOTICES.md, FreeType FTL credit (2026), packaging check. Stacked on #224; green. Retarget to main once #224 merges. |
| #245 | `claude/a-vocabulary` | `b3696f35` | MERGED 2026-10-22 (main 005909b6). |
| #249 | `claude/a-ax-settled-marker` | `cb3b8111` | Main merged (after #275); conflict was #271's text check in the client run(), now inside the settled branch and waiting via settled_dump(..., "text ranges"). CI at cb3b8111: native macOS PASS, Windows, linux-arm64 and the Linux sanitizers pass; linux-x64 was still pending when last checked. The job log could not be read (GitHub log egress 503). Merges together with #277. |
| #257 | `claude/a-vocabulary-controls` | `59496299` | MERGED (main fe257189). |
| #263 | `claude/a-text-core` | `fc5a2ef6` | MERGED (main fe257189). |
| #273 | `claude/a-uia-text` | `26979c9b` | MERGED (UIA TextPattern + TextPattern2). |
| #271 | `claude/a-ax-text` | `c2cf549b` | MERGED (macOS text attributes). |
| #277 | `claude/a-advertise` | `78482c6d` | Item 6: macOS, Windows and Linux advertise HostCapability::accessibility; each host's test checks the bit; docs (capability table, roadmap, FORWARD_PROGRAMME §3 A Today, stage brief). Worktree `D:\gui_forms_w4\advertise`. Draft, HELD until #249 passes (coordinator chose option 2, 2026-10-22: the gate claim rests on all three hosts' out-of-process client tests). Windows tests + sanitizer 2/2. CI pending (first macOS/Linux compile of the test lines). |

## Decisions in force

- Owner vocabulary interview (`planning/interview/A_VOCABULARY_ROUND_001.md`, design language 12.13):
  every recommended option. `modal` is a node **field** (bool), not a state; `checkable` and
  `multiselectable` are states. Live region role is `live_region` (DK-51).
- Indices (`level`, `row_index`, `column_index`) count from one; `access_key` is the mnemonic's
  character only (hosts add Alt: MSAA/UIA "Alt+F", ATK "<Alt>f;;").
- A-T1/A-T2 (text): `text_selection` (UTF-8 anchor/caret, absent for password boxes) and
  `multiline` fields are in the snapshot (done in #245). Still to build: the two Window queries
  (text layout: visual lines with grapheme boundaries and x positions plus the control's word
  starts; set selection with refusals: stale id, focus scope, during paint, off a grapheme
  boundary). Words by the control's own rule; visual lines; sentences unsupported; a caret
  inside a combining sequence refused; scope TextBox (single/multi), MaskedTextBox, TokenBox's editor.
- Host mappings are listed in design language 8.5 (the paragraph "From 2026-10-21").
- Part 2 gaps, owner accepted all seven recommendations (2026-10-22, via the coordinator; recorded
  in design language 8.5 after the host list), done in #257 commit f4b88201:
  1. DataGridView cells publish `cell` with row/column whatever the kind; toggle/value/choice/press
     stay on the cell as states and actions (UIA patterns follow states/actions, not role).
  2. A sortable ObjectView column header publishes press (focuses the header, `request_header_sort`);
     non-sortable headers publish no action and refuse press.
  3. CorrespondenceView stays a list (it shows no real columns); a view with real columns is a table.
  4. Drawn row headers are published (DataGridView): `row_header`, first child of the row, id
     `<grid>.row_header.<row id>`, row_index only, press = the row-header click.
  5. Ribbon KeyTips = its mnemonics (the ribbon has no KeyTips, RB-16): File tab, tabs and
     non-hosted commands publish `access_key`. The application menu does not process mnemonics,
     so its rows publish none.
  6. Live-region announcements on all hosts: UIA LiveRegionChanged (part 2); ATK "announcement"
     signal looked up at run time (ATK >= 2.46); AppKit NSAccessibilityAnnouncementRequested at
     medium priority, posted from updateWithNode; the view republishes its AX tree on each
     semantic change while the tree holds a live region. No client checks the events yet.
  7. NumericUpDown has no larger step, so it publishes no `large_step`.
- MinGW-w64 LGPL finding is recorded in THIRD_PARTY_NOTICES.md (#229): no code, only COM
  type-info records and libuuid GUID constants. Owner to judge.

## Exact next step

Stage A items 1-5 are merged on all three hosts (#245, #257, #263, #273, #271). Item 6 is #277 (advertise the capability), a draft on main, HELD until #249 passes
(coordinator decision, option 2). #275 (the standby's fix: paint is owed only for damage a paint can
clear; the lease state is derived) merges first, then the coordinator syncs #249; if #249 passes, #277
merges with it. If #249 still fails, diagnose it from the dump fields (paint passes, frame pending,
motion armed, freeze pending, lease state and the content, rendered and presented revisions). Also fix
any macOS or Linux compile failure in #277's test lines.
After #277 merges, stage A's gate is met. Record it in results/ if the coordinator asks.

Core routing and Windows (w3): the state-machine hunter fixes moved on 2026-10-22 to `tracks/state_machines.md`.
