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
`--gate` returns a failure for any candidate; use it on a reviewed scope, since
the rules are deliberately conservative and some candidates have a valid
contract outside the local procedure. The default run reports candidates and
returns success. An empty selection is an error. The JSON report contains every
printed finding and its review question.

| Rule | Uncertain source shape | House-style correction to consider |
|---|---|---|
| I01 | Borrow from `owner.data()` or `owner.begin()` is used after an operation that may replace or end owner storage | Finish reads first, snapshot overlapping input, or prove a retained borrow's lifetime. |
| I02 | Live count or metadata is advanced before construction, including a helper returning a slot used by placement `new` | Construct first and publish only the completed prefix. |
| I03 | A Debug assertion is the visible bound before indexed or pointer access | Validate the range in Release before forming the address. |
| I04 | An endpoint is formed by adding to `.data()` without an evident empty-input exit | Handle empty input before pointer arithmetic or use a length-bearing view. |
| I05 | A container size is narrowed to `int` or another small integer without an evident range check | Check representability before converting. |
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
