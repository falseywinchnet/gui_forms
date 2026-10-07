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

Four deliberate C++ compilation failures check signature mismatch, a nontrivial
bound value, more than 16 payload bytes, and more than two values. Four installed
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
