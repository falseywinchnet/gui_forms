# State-machine hunt, second run, and the gate (2026-10-25)

This is the second full run of `tools/check_state_machines.py`, the checker
for house style sections 9 and 1. The first run is
[2026-10-21-state-machine-hunt](../2026-10-21-state-machine-hunt/README.md),
which also defines the rules (F1, F2, C1 to C3). This run fixed the new F1
and F2 findings (PR #311). It also tuned the checker and added the
state-machine gate, a required CI check on new code only (owner, 2026-10-23).

## Runs

All runs used the Windows ordinary tree (`ninja -t compdb`, hosts: Windows)
with LLVM 22.1.8 and `--jobs 6`.

- **Before:** main `89635628`, with main's checker. The F1 count adds 3
  findings that the tuned checker found in directories main's checker does
  not scan: `stx_zlib`, `stx_png`, `stx_conv`, `stx_cng` and `fuzz`.
  Output: [findings_before.txt](findings_before.txt).
- **After:** #311 merged with main `24fb355d`, with the gate PR's checker.
  148 translation units compiled and 1,393 files parsed, in 92 s.
  Output: [findings.txt](findings.txt) and [findings.json](findings.json).

```text
python tools/check_state_machines.py --root <checkout> \
    --build-dir <tree> --jobs 6 --gate --json findings.json
```

## Counts

| Rule | 2026-10-21 | Before | After |
|---|---:|---:|---:|
| F1 | 52 | 14 | 0 |
| F2 | 9 | 2 | 0 |
| C1 | 49 | 12 | 10 |
| C2 | 50 | 31 | 31 |
| C3 | 15 | 2 | 2 |

Of the 14 F1 before, 7 were real and are fixed in code: `material_primitive`,
`procedural_pattern`, `relief_primitives`, `semantic_snapshot`,
`windows_text_automation`, `fuzz/inflate_protocol_fuzz` and
`stx_cng/tests/common.hpp`. The other 7 were checker false positives, which
the tuning below now accepts:

- the 3 known controls cases (`button_base`, `masked_text_box`,
  `container_layout_utilities`), where a fallback is computed and then
  returned at the function's end;
- `motion_value` (a `break` inside a nested loop);
- `material_primitive` `join_layer` and `procedural_pattern`
  `generation_weight` (a straight-line fallback after the switch);
- `stx_zlib/src/inflate.cpp` (`[[fallthrough]]` into a case that leaves).

Both F2 findings are fixed: `cng_pixels` lists every `CngError`, and the
`backplane_composite` default states its reason.

The two C3 findings are for the owner. No public API is reshaped:
`MonthCalendar::today_set_` (a private member) and the doc example
`Reminder::has_date`.

## Checker tuning

- A fallback of straight-line code after the switch is compliant when it
  ends in a leaving statement, or when it ends the function.
- A `break` inside a nested loop or switch does not count as leaving the case.
- `[[fallthrough]]` into a case that leaves is accepted.
- `stx_zlib`, `stx_png`, `stx_conv`, `stx_cng` and `fuzz` are now scanned.

## The gate

- **Workflow:** `.github/workflows/state-machines.yml`, job
  `state-machine-gate`, on Linux (ubuntu-24.04). It configures with every
  native host and Skia off, and does no build: the checker runs the front
  end (`-fsyntax-only`) and builds only the generated headers. It first runs
  the checker's unit tests (`tools/test_state_machines.py`).
- **Pull requests:** `--changed-from <base> --gate` checks the translation
  units whose source changed, or which include a changed first-party header
  (directly or through other headers). Changed paths come from
  `tools/change_selection.py`. With no changed C or C++ file, the job passes
  without configuring a tree. A change to the checker checks the whole tree.
- **Push to main:** the same, against the commit the push replaced.
- **Daily** (cron `41 3 * * *`, the same as `native.yml`), and manual runs:
  the whole tree.
- **Not gated:** F1 and F2 come from the compiler, so code that the Linux
  tree does not compile is read only by the C1 to C3 source rules. That
  covers the Windows, macOS and Linux host adapters and the Skia renderer.
  The run names any changed source it cannot compile.

### Local timings

These ran on Windows, on a tree configured like the CI job (all hosts off,
Skia off), at 4 jobs. The CI times are not measured yet.

| Scope | Translation units | Wall time |
|---|---:|---:|
| Whole tree | 140 | 93 s |
| One changed source (`semantic_snapshot.cpp`) | 1 | 12 s |
| No changed C or C++ file | 0 | 6 s (git only) |

The whole-tree run on that tree also reported F1 0 and F2 0.
