# Style guide

GUI.Forms sources follow one house style: an orthodox, explicitly spelled
subset of C++20 in which types, ownership, mutation and execution order are
visible at the point of use. The canonical text of the complete style is
[`HOUSE_STYLE.md`](https://github.com/malkuth-software/notes/blob/main/HOUSE_STYLE.md)
on the `main` branch of the project's notes repository. Its sections are
ordered by importance, most important first. Read it in full before writing
or changing code.

Part of the style is checked mechanically. The `house-style` job in
`.github/workflows/native.yml` runs `tools/check_house_style.py` over every
first-party directory and fails on any finding. The scanner ignores
comments and literals and reports `auto`, `->` (pointer member access and
trailing return types), coroutine keywords, `std::ranges` and `std::views`
pipelines, defaulted comparison operators, and bracket sequences that may be
lambdas. `tools/house_policy_check` is a Clang LibTooling checker that applies
the same subset semantically over a build's compilation database; maintainers
run it locally, CI does not (see [Repository tools](tools.md)). Everything
else here, including ownership, lifetimes, failure states and kernel
structure, is checked in review.
