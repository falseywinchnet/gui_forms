# PNG text validation, 2026-10-20

Parent commit: `d39be39d`. This advances the owned PNG phase's text-encoding
gate and fixes a demonstrated decoder conformance bug. It does not complete
ICC, language-tag or remaining ancillary semantics.

PNG requires a decoder to ignore the iTXt compression-method byte when the
compression flag is zero. The prior implementation rejected every nonzero
method even in that case. The new regression failed on the old decoder with
`uncompressed iTXt compression method was not ignored` after the 15,300-case
image matrix passed. The compressed case continues to require method zero.

The owned strict policy now rejects embedded NULs in tEXt/zTXt/iTXt text and
ill-formed UTF-8 in iTXt text and translated keywords. Empty strings, nonzero
Latin-1 bytes, Unicode noncharacters and otherwise well-formed control
characters are preserved. No text normalization, sanitization or display policy
is silently applied. The validator is independently written first-party code.
References: [PNG text clauses](https://www.w3.org/TR/2025/REC-png-3-20250624/#11iTXt)
and [RFC 3629](https://www.rfc-editor.org/rfc/rfc3629), UTF-8 byte syntax.

Parsing checks raw text before output mutation. Metadata extraction reuses that
proof through a private compile-time choice; only freshly inflated strings need
new validation. The loops allocate nothing and stay within live byte extents.
Encoded/metadata limits bound the added scan. The inflater's existing output and
work budgets remain intact; its work counter measures inflate, not text scans.

Tests exercise 256 methods in each compression mode, 23 malformed sequences,
translated-keyword failures, compressed/plain NULs, empty strings, every nonzero
Latin-1 byte and all 1,112,063 nonzero Unicode scalar values. An independent
scalar-to-UTF-8 encoder constructs the exhaustive valid corpus. The valid
corpus is tested as plain text, stored-deflate text and a translated keyword.
Six small named text seeds extend the fuzz corpus; the large exhaustive Unicode
fixture stays in deterministic tests. No claim is made that libpng rejects the
same malformed metadata: it serves as the independent pixel/sample oracle.

A short decoded-metadata buffer regression also checks `insufficient_storage`.
The private contract now correctly says that this failure can follow partial
image/metadata output. The caller must discard all output on failure; the prior
blanket statement that every storage error preceded mutation was inaccurate.

Commands (run sequentially from this worktree):

```sh
/Users/ultimussecundai/.local/bin/m4build -- env PATH=/opt/homebrew/opt/llvm@22/bin:/opt/homebrew/bin:/usr/bin:/bin python3 -B tools/validate_png.py --libpng build-zlib-study-sources/libpng --zlib build-zlib-study-sources/upstream-zlib --output build/png-text-check --seconds 60
/Users/ultimussecundai/.local/bin/m4build -- sh results/2026-10-19-zlib/png/full-suite-driver.sh
```

The actual combined invocation redirects the full-suite log to
`build/png-text-full.log`; raw results and source hashes are collected here.

The targeted LLVM 22.1.8 M4 sanitizer run passed the 15,300-case pixel matrix
and new text tests. Fuzzing completed 3,795,490 executions in 61 seconds, with
peak RSS 400 MiB and no reported sanitizer failure. `check/run.json` records
source hashes, oracle pins, commands and the 156 initial corpus hashes. This is
a smoke campaign, not the required sustained fuzz gate. Text-log trailing
whitespace is normalized without removing diagnostic lines.

`parent-ci.json` records successful native tests on macOS arm64, Windows x64,
Linux x64 and Linux arm64, plus Linux ASan/UBSan/LSan and consumer-program
checks, for parent `d39be39d`. Those checks do not qualify the new text change.

Both prescribed first-party checkers passed over 1,181 files, and the generated
API documentation check passed.

The whole-project M4 sanitizer suite passed **159/159 tests in 182.92 seconds**
with this change (`full-suite.log`). No sanitizer suppression was added. This
result covers the final source hashes in `check/run.json`; source and tests
were unchanged between the targeted run and whole-suite execution.
