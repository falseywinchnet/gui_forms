# What's left to 0.5

Date: 2026-10-27, from main at the checkpoint `checkpoint-2026-10-27`
(14164ee5). Status: the list of remaining work from the checkpoint to 0.5 ABI
candidate, by release. `planning/FORWARD_PROGRAMME.md` stays authoritative for
scope and gates, and each stage brief under `planning/stages/` for its own
decisions. Where this page says **unknown**, nothing recorded settles it;
nothing here is a guess.

What has landed since v0.2.0 is in `docs/dev/roadmap.md` and in the stage
index of `planning/stages/README.md`.

## First item of the next leg: integrity findings (#401)

- **What:** #401 adds a source checker for code whose storage, lifetime or
  bounds invariants are not evident locally, and its first report. That
  report covers 963 first-party files and lists 44 candidates. They are a
  review queue, not confirmed defects.
- **Next:** land the checker, review the candidates, and fix the likely bugs
  it found.
- **Dependencies:** none.
- **Open owner decisions:** whether the checker becomes a gate, and over
  which scopes: unknown.
- **Parked:** #401 is open and set aside, and its branch is kept.

## 0.3 Themeable: the remainder

**T3, the theme language front end.**
- **Done:** P1 to P7a are on main:
  - tokens and diagnostics, the syntax tree and its round trip;
  - selectors, values and expressions;
  - the lowering onto the engine;
  - authored materials P6a to P6f (recipe fields, filters, images and
    borders);
  - transitions P7a.
- **Left:**
  - P7b (motion: `@keyframes`, `animation` and ambient loops);
  - P8 (colour roles, em, text roles and `@media`);
  - P9 (inline styles and `!important`);
  - P10 (packages, imports and atomic replacement);
  - P12 (custom properties through the cascade);
  - P11 (the gate). The plan runs P12 before P11.
- **Dependencies:** T1 and T2, both done.
- **Owner decisions:** P7b is "strict for now" (decided), as are the
  engine-version lower bound and spring() damping. Open: none recorded in
  the brief; whether later phases raise new ones is unknown.
- **Parked:** nothing.

**T4, the default theme and the Windows-familiar themes.**
- **State:** design decided; build not started.
- **Dependencies:** T1, T2, and T3. Whether T4 can start before T3's gate,
  on a subset of phases, is unknown.
- **Owner decisions:** decided in the design language §12.3 to §12.5 and the
  brief (2026-10-13). Open: none recorded.
- **Parked:** nothing.

**T5, compositor cooperation.**
- **Done:** C0 to C6 are on main: the Windows, macOS and X11 adapters, the
  XWayland probe, the engine join (C5) and full-fidelity glass (C6).
- **Left:**
  - C7 (reflection);
  - C8 (the stage gate: a compositor lab program on each host);
  - the Mac bench rerun with an active-window probe.
- **Dependencies:** C7 needs C6 (done). The bench rerun needs the owner's
  Mac mini M4.
- **Owner decisions:** the Wayland direction is an open owner concern, not
  a decision (T5 brief, with T5-Q9); T5 plans no Wayland-native work.
- **Parked:** nothing.

**SK, universal Skia.**
- **Done:** SK1a (the Windows CPU Skia archive built and published by CI),
  plus #386, the fix for the CI jobs that a skipped Skia job had skipped.
- **Left:**
  - SK1b (pin the archive; Skia mandatory on every platform);
  - SK2 (BGRA8 premultiplied everywhere, PNG only at the image seam);
  - SK3 (the Windows host on Skia);
  - SK4 (CoreGraphicsRaster removed);
  - SK5 (goldens re-recorded once).
- **Paused by the owner (2026-10-26).** When it resumes is the open owner
  decision.
- **Later:** the swap from Skia to `stx_skia` comes after SK; its release is
  unknown.
- **Dependencies:** SK1a (done).
- **Parked:** the SK1b worktree has no commits; its resume steps are in the
  state-machines worker's track file.

## 0.4 Flexible: the remainder

**L, real dynamic layout.**
- **Done:** L0 to L8a are on main:
  - the layout oracle;
  - FlexPanel, GridPanel and BlockPanel;
  - the `TextBlock` leaf;
  - the Windows Forms containers with full right-to-left mirroring;
  - the L7 box gate (181 fixtures, 0 over tolerance);
  - the resize fix #360.
- **Left:** L8b, the text corpus. This adds the text-scale dimension the
  box gate did not exercise.
- **Dependencies:** L8a (done).
- **Owner decisions:** open: none recorded.
- **Parked:** `claude/l8b-text-corpus`.

**FD, form documents.**
- **State:** not started; format decided.
- **Dependencies:** T1 (done) and T3.
- **Owner decisions:** decided 2026-10-13 (the CSS-family syntax, and the
  Gallery DML migrates). Open: none recorded.
- **Parked:** nothing.

**W, Windows Forms control coverage.** It lands across 0.4 to 0.6.
- **Done since v0.2.0:**
  - the Open, Save, FolderBrowser and Color dialogs, and
    ThreadExceptionDialog;
  - TabControl images and the DataGridView image column;
  - the ListView insertion mark and label editing;
  - content-only owner drawing;
  - Splitter.
- **Left:**
  - MessageBox;
  - the link role and link column;
  - menu letter mnemonics;
  - the slow-click label edit;
  - the RichTextBox run model;
  - the remaining ListView and TabControl gaps.
- **Dependencies:** T4, or the by-role paint path (ruling 2026-10-14). The
  RichTextBox run model reads `stx_rtf2md` (RT1 to RT3 done; RT4 is
  SwiftEdit's).
- **Owner decisions:** `Diagram` (DG) waits for the owner's interview.
  `DataLayout` waits as well (W brief). Which W rows count toward 0.4 rather
  than 0.5 or 0.6 is unknown.
- **Parked:** `claude/w-message-box`.

**DP, damage-preserving painting:** complete (see the stage index); nothing
left.

## 0.5 ABI candidate

**C, the stable installed C ABI.**
- **State:** an experimental 0.x ABI (version 0.26) is built and tested but
  not installed.
- **Left:** a versioned, installed ABI with an error model, exposing the T1
  schema and the T2 engine.
- **Dependencies:** T1 and T2 (done).
- **Owner decisions:** decided 2026-10-13 (C brief). Open: none recorded.
- **Parked:** nothing.

**The breaking-change audit.**
- **What:** an audit of the C function tables and error model, with the C++
  wrappers as RAII conveniences over them, not a second contract.
- **Dependencies:** C.
- **Owner decisions:** open: unknown.

**Two independent native consumers.**
- **What:** two consumers that pass the ownership, thread and callback
  conformance tests against the installed SDK.
- **Dependencies:** C.
- **Owner decisions:** the consumers for 1.0 were decided on 2026-10-13
  (PlaySuite and Plan Paint, then the interface builder and the demoboard
  program). Whether the same two count for 0.5 is unknown.
- **Supporting work:** `claude/consumer-contract` (the consumer tooling
  contract and its test) is parked for the template harness (plan phase D).

## Also listed, outside the releases above

- **A, accessibility verification:** complete in 0.2 Verified; nothing left
  is recorded.
- **"T6":** the forward programme defines no stage T6. What it refers to is
  unknown.
- **The Emscripten target:** planned and owner-approved, after `stx_skia`.
  WebGL is used only as its presentation backend; painting stays on the CPU.
  Its release is unknown.
- **After 0.5:**
  - S, the C# sibling (0.6), needs R and C;
  - R, SDRSharp reflection, needs T4 and F;
  - the visual tools (0.7): D, the interface builder, needs T3, L and FD;
    H, HTML import, needs L and T3; V, the theme designer, needs T4 and D's
    core.
