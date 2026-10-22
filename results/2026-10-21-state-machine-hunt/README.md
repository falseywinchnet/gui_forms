# State-machine hunt, first report (2026-10-21)

The first full run of `tools/check_state_machines.py`, the checker for house
style section 9 (a switch over a closed set still has a default; every
unexpected input goes to one defined fallback) and section 1 (prefer a shape
that cannot hold contradictory values). These findings go to the owning
tracks. Nothing here is fixed, and there is no CI gate yet: the owner's plan
is to collect, then a review worker fixes, then gate.

## Run

- Source: main `3593b13d`, checkout `D:\gui_forms_w1\gui_forms`.
- Compiler facts: the commands of the configured Windows ordinary tree
  (`ninja -t compdb`), run as `clang++ -fsyntax-only -Wno-everything -Wswitch
  -Wswitch-enum -Wswitch-default` with LLVM 22.1.8. Only translation units
  whose source has a `switch`, or which include by name a first-party header
  that has one, were run: 117 of them, in 155 s at 6 jobs.
- Source rules: all 1,244 first-party files (`include src demo tests examples
  tools file_manager_demoboard stx_vorbis`, `third_party` and `_deps`
  excluded).

```text
python tools/check_state_machines.py --root <checkout> \
    --build-dir <checkout>/../.build/native-windows-x64/gui-forms --jobs 6 --json findings.json
```

Full output: [findings.txt](findings.txt) (grouped by area) and
[findings.json](findings.json).

## Rules

| Rule | Name | Source | Meaning |
|---|---|---|---|
| F1 | switch-without-default | compiler (`-Wswitch-default`, `-Wswitch`) plus source | a switch has no default label and is not in the compliant form (below) |
| F2 | default-hides-cases | compiler (`-Wswitch-enum`) plus source | an enum switch sends enumerators to a default that states no reason (no comment at it, no throw or failure call), or a vocabulary mapping leaves any enumerator out |
| C1 | input-ignored | source (heuristic) | a case or default whose body is empty, or a bare `break`, `return` or `continue`, with no comment |
| C2 | chain-without-fallback | source (heuristic) | an `if` / `else if` chain comparing one expression with two or more enumerators and no final `else` |
| C3 | contradictory-shape | source (heuristic) | `has_x`/`x_valid`/`x_set`/`x_known` beside `x` (including a mirrored `std::optional`), antonym flags of one subject (`x_open`/`x_closed`), or three or more lifecycle flags of one subject |

**Vocabulary mapping** (owner rule): a switch over an enumeration declared in
`anatomy/types/anatomy_types.hpp`, `semantics/types/semantic_types.hpp`,
`theme/part_painting/part_painting.hpp` or `theme/types/theme_types.hpp`
(PartRole, AnatomyState, SemanticRole, PartMark, ControlVisualRole and their
neighbours), where every case group's body only translates: a single
`return`, or one assignment then `break`. These must list every enumerator,
so that a new atom forces a decision there. Any other enum switch may leave
enumerators to a default that states its reason.

Reasons: a comment on the label's line, the line above, or in its body clears
C1 and F2's default. A comment on the head line, the line above or the
chain's last line clears C2, and a comment on the member line or the line
above clears C3. The records named `*States` and `*Parts` are skipped: they
are anatomy declarations (one bool per state or part name), not runtime state.

## Counts

| Area | F1 | F2 | C1 | C2 | C3 | compliant |
|---|---:|---:|---:|---:|---:|---:|
| core | 26 | 2 | 17 | 3 | 6 | 54 |
| controls | 18 | 5 | 21 | 44 | 4 | 24 |
| c-abi | 5 | 0 | 3 | 0 | 0 | 0 |
| host-windows | 3 | 0 | 2 | 0 | 0 | 6 |
| tests | 0 | 1 | 3 | 1 | 1 | 14 |
| stx_vorbis | 0 | 1 | 0 | 0 | 3 | 1 |
| render | 0 | 0 | 2 | 0 | 1 | 0 |
| host-linux | 0 | 0 | 1 | 0 | 0 | 0 |
| audio | 0 | 0 | 0 | 1 | 0 | 0 |
| examples | 0 | 0 | 0 | 1 | 0 | 0 |
| **total** | **52** | **9** | **49** | **50** | **15** | **99** |

**Compliant** (owner decision, 2026-10-21): an enumeration switch with no
default that lists every enumerator (the compiler reports none unhandled),
leaves from every case (return, throw, or a call to `abort`, `terminate`,
`unreachable`, `exit` or `quick_exit`), and whose next statement after the
closing brace is the one fallback, which also leaves. It keeps the compiler's
warning for a new enumerator. These 99 switches are tallied (rule `OK` in the
JSON) and are not findings. The F1 count is the non-compliant set: switches
with no default that are not in that form. For example, a case that `break`s
to code after the switch, or a fallback that is not the next statement.

One F2 is a vocabulary mapping, which must list every enumerator:
`src/core/theme/part_painting/part_painting.cpp:26`, a PartRole mapping
missing 23 enumerators (region, shadow, edge, ...).

## Precision (hand-reviewed sample)

Ten findings per rule, evenly spaced through each rule's list, plus all 15 C3
(file and line in `findings.txt`). These are my judgements.

| Rule | Sample | Right | Notes |
|---|---:|---:|---|
| F1 | 10 | 10 | compiler fact, and none of the 10 is in the compliant form (each breaks to code after the switch, or has no fallback) |
| F2 | 9 (all) | 9 | e.g. `ContentAlignment` alignments left to a bare `default: break;` |
| C1 | 10 | 5 | right: ignored pointer `move` in the C ABI field adapter, `count` sentinels, ignored `close_figure` path verbs; wrong: no-op is the meaning (`BorderStyle::none`, `AnimationDirection::normal`, `TextCaseTransform::none`, label style roles) |
| C2 | 10 | 7 | right: pointer and tray handlers that drop other actions silently, zones and behaviours with no else; wrong: chains after defaults set beforehand (image list, instrument rack colours, check box) |
| C3 | 15 (all) | 14 | right: `has_text`/`text`, `has_image`/`image`, `today_set_`/`today_`, `frame_known`/`frame` and others; wrong: the `stb_vorbis` compatibility record in `stx_vorbis/src/c_adapter.cpp`, which mirrors a foreign API |

Heuristic false positives found during development were fixed before this run
and are covered by unit tests: anatomy `States` records, antonyms of
different subjects, lifecycle flags of different subjects, and
brace-initialised members.

## Coverage gaps

- F1 and F2 cover only the code compiled on Windows. The macOS host
  (Objective-C++), the Linux host and other platform-only sources were
  scanned by C1 to C3 only. A run on macOS and Linux trees (same command)
  completes it.
- A switch in a template is diagnosed only where it is instantiated, and a
  header switch only through a translation unit that includes it by name.
- C2 follows braced chains only; a chain with an unbraced branch is skipped.

## Next

The review worker fixes the findings by area (core, controls, c-abi,
host-windows, tests, stx_vorbis, render, host-linux, audio, examples) after
this report merges. A baseline keyed by file, enclosing function and rule can
gate later.
