# Code integrity checker

This tool reports C and C++ code shapes where the house style asks for an
explicit storage, lifetime or failure boundary, but the source does not make
that boundary evident. Each result gives a location, the related line when
available, the applicable house-style section, and a concrete review question.
The result is a **review candidate**, not a conclusion about runtime behavior.
The JSON field `house_style_status: invariant_not_evident_in_source` states why
the instance is reported: the local source does not display the invariant the
style requires. It does not claim the program fails for a particular input.

It complements `tools/check_house_style.py`, `tools/check_lifetime_rules.py`,
and `tools/check_state_machines.py`. Its rules grew from actual renderer and
container fixes, including self-aliased array mutation, construction interrupted
after a count changed, empty pointer endpoints, unchecked pixel-reference
bounds, narrowing, and a one-byte poison-boundary error. No rule asks a model
to classify the severity or exploitability of the pattern. The useful outcome
is a simpler implementation whose validity is evident from its construction.

Run a focused review from the GUI.Forms root:

```sh
python3 -B code_integrity_checker/check.py include src --json build/code-integrity.json
python3 -B code_integrity_checker/check.py src/core/changed.cpp --gate
python3 -B -m unittest discover -s code_integrity_checker -p test_check.py
```

Explicit file arguments can inspect imported reference code. Directory walks
skip build trees, preserved baselines and third-party source by default.
The default run reports candidates and returns success. An empty selection is
an error. The JSON report contains every finding, its review question and its
status.

## Statuses, guards and reviews

Every candidate ends in one status:

| Status | Meaning |
|---|---|
| raised | Nothing explains it yet. It needs a review. |
| guarded | A guard the checker recognizes protects it. It is listed apart. |
| reviewed | A decision recorded in `reviews.json` covers it. |
| ignored | It lies in a scope `reviews.json` excludes: the Skia renderer and its C ABI (owner, phase C). |

The checker recognizes three guards:

| Guard | What it looks for, before the site in the same function |
|---|---|
| `checked_arithmetic` | A checked helper: `checked_*`, `*_checked`, `__builtin_*_overflow`, `ckd_*`, `SafeMath`, `FitsIn`. |
| `dominating_validation` | An `if` that bounds an operand with a comparison and then leaves (`return`, `throw`, `break`, `continue`), or a limit constant (`numeric_limits`, `SIZE_MAX`, `INT_MAX`). An emptiness test is not a bound. |
| `named_failure` | A call that admits or charges the operand and fails by name (`admit`, `charge*`, `require*`, `validate*`, `check*`). |

A fourth guard, `trap`, is reserved for the owner's "maps to trap" class: sites that a trap already stops. It is not detected yet. Its definition waits for the owner and for the stx_skia sibling's list. Until then, a review marks such a site with `trap_candidate`.

`reviews.json` records a decision for each candidate a person has read: `defect`, `guarded` (with the guard and where it lives, when it is outside the function) or `false_shape`. A decision matches a finding by path, rule and evidence line. Editing the line raises the finding again, so a stale decision cannot hide new code.

`--gate` fails only on raised candidates. CI runs it over `include` and `src` (`.github/workflows/code-integrity.yml`). A new candidate therefore fails the job until it is guarded in code or reviewed in `reviews.json`, while reviewed and guarded sites stay listed.

| Rule | Uncertain source shape | House-style correction to consider |
|---|---|---|
| I01 | Borrow from `owner.data()` or `owner.begin()` is used after an operation that may replace or end owner storage | Finish reads first, snapshot overlapping input, or prove a retained borrow's lifetime. |
| I02 | Live count or metadata is advanced before construction, including a helper returning a slot used by placement `new` | Construct first and publish only the completed prefix. |
| I03 | A Debug assertion is the visible bound before indexed or pointer access | Validate the range in Release before forming the address. |
| I04 | An endpoint is formed by adding to `.data()` without an evident empty-input exit (not for `std::string`, `std::string_view`, `std::span`, `std::vector` or a non-empty `std::array`, whose empty endpoint is valid: C++ defines a null pointer plus zero) | Handle empty input before pointer arithmetic or use a length-bearing view. |
| I05 | A container size is narrowed to `int` or another small integer without an evident range check (not for a literal extent: `std::array<T, N>` or a vector built with N elements) | Check representability before converting. |
| I06 | A raw product reaches allocation, growth or copy without an evident whole-product check | Check the complete extent before the storage operation. |
| I07 | A poison call begins one byte after a named boundary | Check whether the first argument is already the exclusive live end. |

The scanner masks comments and literals, bounds ordinary function bodies and
does small source-order traces. It does not compile code, resolve overloads,
prove aliasing, model all branches, or know whether an assertion is active in a
particular build. A missing finding does not certify a function. A finding
should be resolved by reading the relevant contract and code, then changing
the source if the invariant is not explicit. The existing deterministic tests,
sanitizer suite and human review remain necessary.

The first full first-party report is in
[`reports/gui_forms-2026-10-26.json`](reports/gui_forms-2026-10-26.json).
Its scope and counts are recorded in [`reports/README.md`](reports/README.md).

The triage of that report, sorting each candidate into defect, guarded and false shape, is in
[`reports/README.md`](reports/README.md). Its decisions are recorded in [`reviews.json`](reviews.json).
