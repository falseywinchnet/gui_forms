# Sustained inflater campaign

The campaign plan in `INFLATE_CAMPAIGN.json` locks the candidate, harness,
comparison manifest, seed files, supervision tools and workflow by SHA-256.
Changing a locked file requires a deliberately revised plan and new evidence.
The runner checks those files before compilation and after replay. It does not
credit an existing output directory, a smoke override, early completion,
interruption, failed test, sanitizer report, failed coverage export or changed
source as a completed shard.

The initial plan has six independently seeded four-hour shards for each of
x64/arm64 and ordinary/strict pinned-zlib comparisons: 24 fuzz-process hours per
configuration, 96 total if all finish. They are aggregate independent fuzz hours,
not a claim of one continuous 24-hour process. At most four jobs run together.
Setup, corpus merge and replay time do not count. GitHub's
[six-hour hosted-job limit](https://docs.github.com/en/actions/reference/limits)
is the reason for bounded shards; each job has a 330-minute outer timeout.
The campaign runs on standard public-repository Ubuntu x64 and arm64 runners.

A push changing the plan on `codex/png-owned-codec` starts the workflow. Later
PNG-only pushes neither start another campaign nor cancel the current one.
Manual dispatch is also declared for availability after the workflow is on the
default branch. Avoid manually duplicating an active campaign. There is no
scheduled recurring job or automatic retry of a failed shard. Preserve failures
and investigate before deciding whether a replacement run is warranted.

## Instrumentation and corpus

Both the owned inflater and pinned zlib comparison are compiled with LLVM
22.1.8, ASan, UBSan and source coverage instrumentation; LSan is enabled and
inherited leak suppressions are removed. The actual candidate receives
libFuzzer instrumentation. The unmodified oracle pin is verified by file hash.
Strict comparison adds `INFLATE_STRICT` to the reference build.

The runner first executes the native conformance/differential tests. It then
combines the ten checked-in malformed/valid seeds with generated zero, byte-ramp,
text/repetition and deterministic entropy streams. Generation covers raw/zlib,
five sizes through 32 KiB, four compression levels and five strategies. Python
zlib versions and every generated seed hash are recorded; the comparison oracle
remains the independently pinned C build. Duplicate seeds are eliminated by
content hash and libFuzzer merges the corpus before timing begins.

The 65,536-byte maximum input exercises the harness's full admitted input range;
the older smoke runner used only 4,096 bytes. Each fuzz input compares whole and
one-byte-buffer decoding, stream reset/reuse and the comparison oracle. Output
is bounded to 32 KiB, work to four million units and processing calls to 100,000.
Inputs that hit a resource limit are handled separately from format agreement.
This campaign does not add randomized option/limit/API-sequence coverage; those
protocol dimensions retain their deterministic tests and need separate expansion
against the stage's full fuzz specification. Silesia and broader PNG-derived
streams are not included in this initial generated corpus.

LLVM's internal RSS watchdog is disabled because the earlier isolated runner
identified a watchdog-thread shutdown leak. Leak detection itself stays enabled.
The parent supervisor samples the process tree's resident memory once per second,
stops its own process group above 1.5 GiB or the wall-time bound, and records peak
sampled RSS. This is a sampled bound, not a hard guarantee against subsecond
peaks. The per-input libFuzzer timeout is twenty seconds. Compiler operations,
corpus merge and replay also have explicit external limits.

## Evidence and accounting

Each artifact preserves commands, compiler versions, host and architecture,
source/oracle/corpus hashes, seed, process exit status, fuzz executions and time,
logs, crash/timeout artifacts, final corpus, raw profiles, candidate branch/region
coverage and a source-annotated report. Replay gets a separate profile so native
conformance-test instrumentation is not mixed into the fuzzer's coverage map.
A successful artifact is not a phase acceptance record.

`tools/check_inflate_campaign.py` rejects missing or duplicate configurations,
smoke runs, failed operations, short runs, source/plan mismatches and modified
logs/corpora. Its successful result is named `accounted`, not a safety pass.
Coverage must still be reviewed, unvisited parser/error paths exercised, and
corpus replay completed on Linux, macOS and Windows. The other inflate/PNG
conformance, performance, encoder, integration and packaging gates remain open.

Local smoke validation uses the same runner with `--seconds 60`; it is explicitly
assigned zero qualifying hours:

```sh
/Users/ultimussecundai/.local/bin/m4build -- env PATH=/opt/homebrew/opt/llvm@22/bin:/opt/homebrew/bin:/usr/bin:/bin python3 -B tools/run_inflate_campaign.py --oracle build-zlib-study-sources/upstream-zlib --output build/inflate-campaign-smoke-ordinary --oracle-mode ordinary --shard 0 --seconds 60
python3 -B -m unittest discover -s tools -p test_inflate_campaign.py
```
