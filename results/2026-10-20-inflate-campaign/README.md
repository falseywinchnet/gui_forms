# Inflater campaign infrastructure and smoke validation

2026-10-20. The preceding goal turn produced independent current-source CRC
confirmation (`../2026-10-20-crc-confirmation/`). This increment makes sustained
inflater campaigns executable with bounded supervision and source-locked
accounting. No inflater or PNG production C++ changes in this increment.

## Smoke results

Apple M4/macOS arm64, LLVM 22.1.8; ordinary and `INFLATE_STRICT` upstream-zlib
oracles at `51b7f2abdade71cd9bb0e7a373ef2610ec6f9daf`, checked by file digest.
Candidate, oracle and tests receive ASan/UBSan and source coverage instrumentation;
LSan is enabled with no inherited suppression. Both native test runs passed,
including the 700-case generated upstream differential matrix.

| Oracle | Fuzz executions | Reported seconds | Candidate regions on replay | Candidate branches on replay |
| --- | ---: | ---: | ---: | ---: |
| Ordinary | 62,174 | 61 | 393/424 (92.69%) | 287/332 (86.45%) |
| Strict | 53,188 | 61 | 392/424 (92.45%) | 285/332 (85.84%) |

Both smoke runs explicitly record **zero qualifying campaign seconds**. The
larger input range and 171 unique starting seeds differ from prior smoke runs;
execution rates must not be interpreted as decoder performance comparisons.
The ordinary merge retained 74 initial corpus members and finished with 309.
The corpus is merged before timing and replayed separately for source coverage.
Source hashes, plan digest and evidence hashes were verified against the final
local files after copying the results back.

`ordinary-smoke/` and `strict-smoke/` retain run records, compiler/test/fuzzer
logs and coverage JSON/text. The complete smoke artifacts, including corpus and
profiles, are retained locally in `build/inflate-campaign-ordinary-evidence/`
and `build/inflate-campaign-strict-evidence/`, and on the M4 in the recorded output
directories. Long-run CI artifacts retain their complete corpus and profiles. The two
`*-smoke-raw.tar.gz` archives also preserve the complete exact smoke artifacts.
Readable log/text copies normalize trailing whitespace for repository checks;
`presentation.json` maps their original and display hashes. Extract the raw
archives when verifying `run.json` evidence hashes.

## Review of unvisited code

The ordinary replay does not exercise invalid reset options (line 55), work/input
quota failures (100, 116, 423), legal empty-distance-tree completion (202),
distance beyond a smaller window (686), or overlapping-buffer rejection (736).
Those are actionable corpus/protocol-coverage gaps, although deterministic
contract tests already exercise several of them. The fixed option/input budgets
of this harness prevent some failures from being reached by byte mutation alone.
Future API-sequence coverage must address that directly; more hours do not fix
an unreachable harness path.

`make_fixed_tree` (253–298) is used for compile-time constants, so its reported
zero runtime count has a different explanation. The unreachable internal
idle/failed/done dispatch cases must likewise be assessed against `process`'s
phase guards rather than invoked by an invalid harness access. The original
coverage denominators are preserved; no adjusted percentage is substituted.

## Runner verification and negative evidence

Seven Python test methods cover process-tree RSS accounting, successful/nonzero/
missing process launches, timeout termination, RSS termination, source-lock
rejection, completion parsing, and missing/duplicate/smoke/failed/short/changed
shard rejection. Tests passed. YAML parsed with Ruby's standard YAML parser and
the generated API documentation check passed.

The first M4 attempt failed before compilation because a mirrored Git worktree
file still referred to the source host's `.git/worktrees` path. The initial
runner attempted `git rev-parse HEAD` while constructing its record. That query
was removed; content digests are authoritative and CI commit identity is
optional environment metadata. The failed output directory remains preserved
as `build/inflate-campaign-smoke-ordinary` on the M4; the successful retry used
`build/inflate-campaign-smoke-ordinary-v2`. This was a runner defect, not an
inflater failure. There were no sanitizer failures in the successful runs.

Both prescribed checkers scanned 1,181 files with zero findings. The changed
scope's inflate harness and CRC experiment also passed the spelling checker.
An extra sweep of all fuzz sources found the pre-existing `const auto*` spelling
at `fuzz/png_registry_fuzz.cpp:20`; this increment does not change that file and
does not claim that extra sweep passed. No production C++ was changed merely to
make the campaign checks green.

`parent-ci.json` records completed native jobs on macOS arm64, Linux x64/arm64
and Windows x64, Linux sanitizers, docs and all consumer-program jobs for
`4648d730`. New workflow/tool commits require their own CI; prior-head results
are labelled separately.

## Long-run status and remaining gates

The [campaign plan](../../planning/stages/Z-zlib/INFLATE_CAMPAIGN.md) specifies
six four-hour shards per architecture and oracle mode. `dispatch.json`, when
present, records the launched GitHub run; it is dispatch evidence, not proof of
completion. The aggregate checker accepts only unique, complete, intact shards
matching the same source and plan. This accounts fuzz hours, not phase acceptance.

Sustained results, unvisited branch review, additional corpus/API-sequence scope,
Linux/macOS/Windows replay, performance comparisons and the remaining
PNG/encoder/integration/packaging gates are still required. The broader goal
remains active.
