# PNG and deflate alternatives: precursor light audit

Date: 2026-10-19. Scope: primary documentation, licences and selected source/API
inspection, prioritizing non-GPL C++ implementations. This is a non-exhaustive
feasibility screen, not a completed security audit or benchmark. No candidate
was built, imported or selected. The owner accepted all six recommendations in
[the study](README.md#11-owner-decisions-before-implementation); this audit
fulfils the additional requested precursor without changing those decisions.

## Finding

**Boost.Beast is the leading C++ implementation to investigate before writing
another zlib port.** It already supplies refactored C++ streaming inflate and
deflate engines derived from zlib. Substantial work remains for our bounded
workspace, wrapper/checksum, compatibility, house-style and verification
contracts. No screened implementation is established here as a complete match.

For PNG, keep the accepted pinned-libpng direction unless the owner changes
that choice. LodePNG is the compact combined PNG/deflate candidate; libspng is
the dedicated PNG alternative; Wuffs is the safety-oriented decode comparison.
Replacing libpng also requires changes to the private renderer integration.
A C++ convenience API alone does not establish a native C++ implementation.

## Candidate screen

The licences below describe the inspected core/project notices. They do not
certify all optional tools, examples or transitive dependencies. Preserve
per-file notices and review the exact source closure before any adoption.

| Candidate and primary evidence | Language / licence | Existing work and fit |
|---|---|---|
| [Boost.Beast zlib](https://www.boost.org/doc/libs/latest/libs/beast/doc/html/beast/ref/boost__beast__zlib__inflate_stream.html), [deflate](https://www.boost.org/doc/libs/latest/libs/beast/doc/html/beast/ref/boost__beast__zlib__deflate_stream.html) | Native C++; Boost 1.0 with retained zlib-derived notices | Resumable raw-deflate inflate/deflate with caller input/output buffers. Best C++ starting candidate. Missing the zlib framing and C API required by libpng; allocation/exception behavior also needs adaptation. |
| [Crypto++](https://cryptopp.com/), [inflate API](https://raw.githubusercontent.com/weidai11/cryptopp/master/zinflate.h), [zlib API](https://raw.githubusercontent.com/weidai11/cryptopp/master/zlib.h), [licence](https://raw.githubusercontent.com/weidai11/cryptopp/master/License.txt) | Native C++; Boost 1.0 compilation, public-domain compression source notices | Implements deflate and zlib framing, including Adler-32. Its filter/attachment pipeline, queues, allocations and exceptions are a larger mismatch with explicit caller storage and result contracts. Useful secondary C++ reference. |
| [miniz](https://github.com/richgel999/miniz), [header](https://raw.githubusercontent.com/richgel999/miniz/master/miniz.h), [licence](https://raw.githubusercontent.com/richgel999/miniz/master/LICENSE) | C; MIT | Low-level tinfl/tdefl require no dynamic allocation; zlib-like convenience API and checksums already exist. Strong small-C candidate. Compatibility gaps prevent assuming an unchanged libpng substitution. |
| [zlib-ng](https://github.com/zlib-ng/zlib-ng), [licence](https://github.com/zlib-ng/zlib-ng/blob/860e4cff7917d93f54f5d7f0bc1d0e8b1a3cb988/LICENSE.md) | C11; zlib | Streaming implementation, compatibility build and optimized architecture paths. Broad existing work and a useful performance comparator; not the requested owned C++20 implementation. Our budgets and determinism gates remain unproven. |
| [libdeflate](https://github.com/ebiggers/libdeflate), [licence](https://github.com/ebiggers/libdeflate/blob/96836d7d9d10e3e0d53e6edb54eb908514e336c4/COPYING) | C; MIT | Raw/zlib/gzip whole-buffer compression and decompression with caller output buffers. Useful batch comparator; no streaming or compatible zlib API, so it does not satisfy the accepted streaming contract. |
| [LodePNG](https://github.com/lvandeve/lodepng), [API and licence notice](https://raw.githubusercontent.com/lvandeve/lodepng/master/lodepng.h) | C90-compatible implementation, C++ convenience interface; zlib | Includes PNG encoding/decoding and its own zlib/deflate engine with no external compression dependency. Strong compact combined candidate; allocation-oriented whole-buffer APIs and incomplete hard-bound semantics need substantial work. |
| [libspng](https://libspng.org/docs/), [context API](https://libspng.org/docs/context/), [build dependencies](https://libspng.org/docs/build/), [licence](https://raw.githubusercontent.com/randy408/libspng/master/LICENSE) | C; BSD-2-Clause | General PNG encoder/decoder with progressive operation, custom allocation and limit controls. Replaces the PNG layer, but still depends on zlib. A potential libpng alternative, not a combined replacement or native C++ engine. |
| [Wuffs](https://github.com/google/wuffs), [licence](https://raw.githubusercontent.com/google/wuffs/main/LICENSE) | Wuffs language compiled to C; MIT / Apache-2.0 | PNG/deflate decode candidate with caller-supplied memory and a language designed for safety. Consumers can use generated C without installing the compiler. Different implementation model; this pass does not establish a complete PNG encode/decode and zlib writer replacement. |
| [fpng](https://github.com/richgel999/fpng), [implementation](https://raw.githubusercontent.com/richgel999/fpng/main/src/fpng.cpp) | Native C++; Unlicense/public domain for fpng sources | Self-contained RGB/RGBA 8-bit PNG writer and restricted decoder. Decoder accepts its own output class, not arbitrary PNGs. Too narrow for the shared general PNG path. |
| [fpnge](https://github.com/veluca93/fpnge), [licence](https://raw.githubusercontent.com/veluca93/fpnge/master/LICENSE) | Native C++; Apache-2.0 | Specialized PNG encoder work; no corresponding general decoder. Useful writer reference only, not a replacement for both existing libraries. |

## Source observations that affect the decision

**Beast:** the inspected [inflate implementation](https://raw.githubusercontent.com/boostorg/beast/develop/include/boost/beast/zlib/detail/inflate_stream.ipp)
identifies its zlib derivation. The [window implementation](https://raw.githubusercontent.com/boostorg/beast/develop/include/boost/beast/zlib/detail/window.hpp)
owns heap storage and lazily allocates on write; invalid window settings can
throw during reset. An error-code write interface therefore does not by itself
meet a no-allocation/no-exception decode contract. A targeted next audit should
measure the source/dependency closure, deflate workspace, reset and suspension
semantics, and feasibility of caller-owned storage. Add zlib framing, checksums,
our finite work/input/output limits and the required C shim; then apply all
existing differential, sanitizer, fuzz and cross-platform identity gates.
Native C++ does not imply conformance to our house style or proven SIMD speed.

**miniz:** the inspected convenience header accepts only positive/negative
15-bit default window settings; its documentation says the memory-level
argument is checked but ignored. It does not declare `inflateReset2` in that
header. The pinned libpng subset requires that entry point and settings such
as window-bits zero, so an explicit compatibility adaptation would be needed.
The useful allocation-free low-level kernels are a separate question from
whether its zlib-looking surface is interchangeable.

**LodePNG:** its `max_output_size` documentation explicitly says the limit is
not a hard bound, may be exceeded, and is ignored by the PNG decoder. Therefore
that setting is not evidence that the accepted resource limits are met. The
C++ vector interface does not change the underlying allocation model. Evaluate
peak memory, malformed PNG handling and stream suspension before considering
it as an alternative to the accepted libpng-plus-owned-deflate architecture.

**Wuffs:** restrict any future build to admitted PNG/deflate components. Its
language constraints are a useful safety design, but do not establish that the
compiler, generated code and integration are defect-free; the project's own
README describes limitations in verification rigor. No whole-application
security guarantee is inferred here.

## Recommendation and evidence limits

1. Investigate Beast's extraction/adaptation cost before beginning a fresh C++
   port. This may avoid duplicate work on the most intricate state machines.
2. Keep miniz and Crypto++ as secondary implementation references, and zlib-ng
   and libdeflate as differently scoped comparators. Selection requires an
   explicit provenance and source-closure decision; this report imports none.
3. Retain the accepted libpng choice. If that decision is reopened, compare
   libspng and LodePNG first, with Wuffs as the decode-side safety alternative.
   The specialized C++ PNG writers do not cover general input requirements.

Observed repository heads during the pass (reference points, not dependency
pins): Beast develop `d96126e9e4e561137d860d61cd22607d8a11f8c0`, LodePNG master
`e584a8490db1b76b81bf298c11c09d3b55d9c270`, miniz master
`2948eebafc5b9f1c69bebed80333e403a09bd62d`, libspng master
`46c81dc2b3747fa43aac33704c890a468f195d6e`. Online branch/latest links can move;
re-pin and recheck any selected source before import. Other candidates were
screened at their linked documentation/source, not exhaustively pinned.

No CVE-history survey, exhaustive dependency/licence audit, build, fuzz run,
benchmark, peak-memory measurement or compatibility execution was performed.
“Candidate” means worth investigating, not security-approved. The accepted
64 KiB / 1 MiB workspace ceilings and 24-hour fuzz campaigns remain requirements,
not results demonstrated by any library in this table.
