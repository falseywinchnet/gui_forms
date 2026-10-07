# Concise state and collection qualification

Workload and provider source: GUI.Forms follow-up to File Manager #57, on the
independent provider's main (`268de730d7162fc097ed3e96672a584d2a1bfbc8`).
No PlaySuite source or measurements are included.

## Observed correction

The previous implementation compacted dead owner-held tokens on the next
registration/disposal. Destroying the last publisher left a token/store behind.
Owner-held subscriptions now use links in their existing slot and unlink on
publisher disconnection. The test creates/destroys 1,000 buttons under one owner,
checks zero entries after every destruction, and checks balanced live C++
allocation counts after the entire loop. No separate token store/capacity remains.
Mixed caller-held and owner-held subscriptions still revoke in reverse order.

## Focused acceptance

`gui_forms_owned_event_tests` covers the original #57 lifetime, exception,
registration-failure and dispatch cases plus immediate publisher reclamation,
middle-entry unlinking, mixed revocation ordering, omitted event arguments,
inline one/two-value handlers, Value/Command propagation, model destruction during
dispatch, source validation, item replacement during invocation, reused item
identity, geometry after Window layout, keyboard, named parts and accessibility.

The allocation probe measures 10,000 iterations each of member dispatch,
scalar propagation, command state propagation, choice changes and section changes.
All observed steady-state counts are zero. Registration failure injection now
covers two fresh registration allocations, versus five in #57. This is a count
of C++ allocator calls in the named fixture, not a whole-process memory profile.

Five deliberate C++ compilation failures check signature mismatch, a nontrivial
bound value, more than 16 payload bytes, more than two values, and mutation of a value event argument. Four installed
SDK examples demonstrate ownership, bound values, state and collections.

## Validation record

Local environment: M4 Mac mini, macOS 26.5 arm64, Apple Clang 21.0, Release;
separate renderer-free Debug ASan/UBSan build for focused lifetime tests.

Initial native pass: 87/87 CTest tests passed, including native application,
multi-window, idle/visibility and close tests. Follow-up review added geometry,
section-content swapping and source-validation checks; final exact-revision
validation and hosted Windows/Linux results are recorded below before promotion.

Measurements use `tools/owned_event_lab.cpp` and
`tools/concise_authoring_lab.cpp`. The latter compares the same 1060 × 618
retained Panel/Button/CheckBox/TrackBar scene at 1×, using ordinary state setters
on the preceding revision and bound state on the new revision. It reports seven
trials after warm-up, raster byte hash, and a million empty scheduler polls per
trial. Poll timing is not a claim about operating-system idle CPU. No new-control
versus old-control paint speedup is implied by that unchanged-scene comparison.

## Source review and compatibility

Review follows `planning/PROGRAMMING_HOUSE_STYLE.md`: explicit types and named
behavior; intrusive link ownership and mixed revocation order; retained event
slots/revisions during callbacks; token-guarded model borrowing; validation before
mutation; initialization, conversion and failure states; and storage outside
repeated dispatch/propagation. The spelling scanner supplements this review.
Changed native accessibility switch arms map the added portable group roles.
No whole-repository or vendored-source compliance is asserted.

Component/Revocable, Event slot, CommandState and affected control layouts change.
Rebuild all C++ consumers coherently. New models/collections do not alter the
frozen C ABI or host protocol. Complete skin delivery, common Grid, form records
and designer remain separately tracked requirements, not capabilities claimed here.

## Local qualification at implementation 07ca366

**MEASURED:** native Release CTest 87/87 passed, including the five negative
compiler fixtures. The four standalone installed-SDK examples passed 4/4. The
unchanged File Manager frontend at 964261f33031db3cec48fd4cacd84f78f3843548
rebuilt against this installed SDK and passed 16/16 tests. Its provider pin was
not changed. Focused Debug AddressSanitizer/UndefinedBehaviorSanitizer tests
passed; Apple LeakSanitizer is unavailable, so the retained-allocation assertion
uses the fixture's balanced C++ allocation counter rather than LeakSanitizer.

**OBSERVED:** the installed native `collection_gallery` demonstrated two sliders
at 73 after changing one, two Command-bound checkboxes changing together, choice
selection, disabled command state surviving toolbar rebuild, and exclusive
section expansion. Native arrow navigation and Tab leaving the command bar were
exercised; the macOS accessibility tree reflected checked/selected/disabled
states. This is macOS interaction evidence, not a Windows/Linux screen-reader
qualification claim. The gallery is an optional executable in the installed-SDK
reference project, in addition to its four noninteractive examples.

**OBSERVED:** the first hosted run at 3e6b909 passed Windows and macOS but failed
Linux's allocation-free choice-change assertion. The synchronous control-change
helper constructed `std::function` even when no initialization deferral was
needed. libstdc++ allocated for that publication object. The correction preserves
the argument snapshot on the stack and creates a deferred callable only during
BeginInit. The exact latest hosted result is attached to
[GUI.Forms PR 2](https://github.com/falseywinchnet/gui_forms/pull/2); the earlier
Linux failure is not a waived test.

## Measurements

**MEASURED:** final quiet local runs, seven trials; medians shown. Raw CSVs are
in `experiments/concise-state/`. The dispatch baseline is provider main 268de730
(the #57 owner-held implementation); the paint baseline archives are c90eacf,
whose `include/` and `src/` trees are identical to 268de730. New implementation:
07ca366, same machine/compiler/Release settings and pinned Skia archives.

| Operation | #57 baseline | Follow-up |
|---|---:|---:|
| Caller-held delegate emission | 8.36171 ns | 8.35554 ns |
| Owner-held member emission | 7.43900 ns | 7.39758 ns |
| Omitted event arguments | unavailable | 7.35600 ns |
| One inline bound value | unavailable | 7.37842 ns |
| Two inline bound values | unavailable | 7.36996 ns |
| Forced retained paint, 1060 x 618 at 1x | 0.675117 ms | 0.678658 ms |
| Empty scheduler poll | 9.18842 ns | 9.04746 ns |

The historical #57 report recorded 7.38029 ns for owner-held dispatch on its
earlier run. Use the same-session baseline above for the current comparison.
The paint difference is +0.52%; the ranges overlap (baseline 0.671976-0.679419 ms,
new 0.676707-0.681677 ms). This small run does not establish a speedup or a
statistically significant regression. Both outputs have the exact raster hash
11164930560129037574. Empty polling produced no timers, wakes, mutations or
paints. It is not a whole-process CPU/power measurement.

The preserved `*-contended.csv` paint runs overlapped another compilation and
are rejected for before/after comparison. They are retained to expose the
measurement failure rather than silently selecting them away.

Reproduction: build/run `gui_forms_owned_event_lab` and
`gui_forms_concise_authoring_lab assets/fonts/PortsmouthRapids.ttf` in a native
Release Skia build. For the old dispatch build use the same lab source with
`GUI_FORMS_EVENT_LAB_UNBOUND_ONLY`; for old paint use
`GUI_FORMS_AUTHORING_BASELINE` and the old headers/libraries. Do not combine
headers and archives from different ABI revisions.

## Exact review scope

Source review covered every added/modified hunk against
`planning/PROGRAMMING_HOUSE_STYLE.md`: event/member templates; Component and
Revocable ownership/order; scalar and command connections; Button/CheckBox/range
state publication; collection revisions, layout, keyboard and semantic adapters;
tests; examples; benchmark/CI tooling; and build/export integration. Forty changed
C++ files contain 2,439 added lines at 07ca366; the spelling scanner found zero
violations on those added lines. The stricter full-file foundation/new-control
scanner passed 21 files. Python fixtures/tooling were reviewed for explicit
types, named behavior, operation order and failure propagation. Legacy untouched
hunks and third-party code are excluded from this compliance statement. No
unresolved house-style violation was identified in the reviewed change.

`sizeof(Component)` on arm64 increases from 48 to 64 bytes because revocation
order and intrusive ownership move into the object/slots; fresh owner-held
registration drops from five allocations to two. Bound data adds no separate
allocation. These are explicit layout/storage tradeoffs, not binary-size or
whole-process-memory claims.
