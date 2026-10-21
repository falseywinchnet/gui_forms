# Z. Owned zlib replacement: phase 1 study and threat model

Date: 2026-10-19. Status: **phase 2 authorized and implemented for review; verification gate open**.
The owner subsequently authorized source reuse and a house-style rewrite of
both deflate and PNG. The six decisions were accepted, with PNG ownership
then revised to an owned codec. This authorizes phase 2; later phases still
receive their own reviewed PRs.
The owner requested one PR per phase, stacked while unmerged, no pushes to
`main`, and a stop after this study. No implementation or dependency change
is part of phase 1.

## 1. Purpose, authority and evidence

GUI.Forms needs an owned, efficient, auditable replacement for its pinned
Chromium zlib. The motivating HTTP 503 incident is reported by the owner;
the pinned fetch succeeded during this study. A mirror alone does not meet
the objective. The intended result includes a C++20 house-style core,
bounded streaming inflate, deterministic deflate, portable and vector
checksums, a libpng-compatible C subset, and one implementation on every host.
No performance claim is made before measurements.

**Given by the owner, 2026-10-19:** remove Windows WIC PNG decoding; use the
owned zlib consistently across platforms. **Accepted by the owner, 2026-10-19:** the recommendations in section 11,
with the subsequent owned-PNG revision recorded there.
The requested [light alternatives audit](ALTERNATIVES_LIGHT_AUDIT.md) examines
existing implementations before code begins; it does not select an import.

Read first: [AGENTS.md](../../../AGENTS.md), the complete
[house style](../../PROGRAMMING_HOUSE_STYLE.md),
[forward programme](../../FORWARD_PROGRAMME.md),
[stage guidance](../README.md), design language
[§4.5 and §12.3](../../DESIGN_LANGUAGE_SPECIFICATION.md),
[ADR-004](../../../decisions/ADR-004-one-text-rasterization-engine.md), and
[build contract](../../../docs/building/fast-builds.md).
This is a newly requested foundation study, not an implementation of a
previously gated theme stage. Its implementation depends on acceptance of
this study and recorded owner decisions. The external stage board could not
be inspected or updated: no Artifact tool is available in this session.
The checked-in programme and this brief remain authoritative.

Source audit base: GUI.Forms
`6a120a39aad43ae42ae9dcc35ecd7dbd7e220310`. All repository paths and line
references below refer to that revision unless stated otherwise.
The [phase evidence](../../../results/2026-10-19-zlib/README.md) records
machines, commands, negative observations and what has not been tested.

| Source | Exact revision and role |
|---|---|
| [Skia](https://skia.googlesource.com/skia/+/2a9b593bab4b2fd019fa494c8d401ff1fab0b883/) | `2a9b593bab4b2fd019fa494c8d401ff1fab0b883`; admitted CPU/PNG build graph |
| [libpng](https://skia.googlesource.com/third_party/libpng/+/d5515b5b8be3901aac04e5bd8bd5c89f287bcd33/) | `d5515b5b8be3901aac04e5bd8bd5c89f287bcd33`; actual caller subset |
| [Chromium zlib](https://chromium.googlesource.com/chromium/src/third_party/zlib/+/646b7f569718921d7d4b5b8e22572ff6c76f2596/) | `646b7f569718921d7d4b5b8e22572ff6c76f2596`; existing implementation and proposed import source |
| [GUI.Forms FreeType](https://gitlab.freedesktop.org/freetype/freetype/-/tree/f4205da14867c5387cd6a329b90ee10a6df6eeff) | `f4205da14867c5387cd6a329b90ee10a6df6eeff`; active text-stack dependency |
| Skia DEPS FreeType | `264b5fbf5b912b39f98d038bf75d39be0a73f21b`; fetched by the script, but Skia's font backend is disabled |
| Upstream zlib comparison | v1.3.1, `51b7f2abdade71cd9bb0e7a373ef2610ec6f9daf`; inspected reference, not a new production dependency |
| zlib-ng comparison | 2.2.4, `860e4cff7917d93f54f5d7f0bc1d0e8b1a3cb988`; tag resolved, licence inspected; no code imported |
| libdeflate comparison | v1.24, `96836d7d9d10e3e0d53e6edb54eb908514e336c4`; tag resolved, licence inspected; no code imported |

The comparison pins are reproducible baselines, not assertions that these
are the newest releases or security endorsements. Refreshing them requires
a separately recorded pin and rerunning comparisons; never silently replace
a baseline during a measurement series.

## 2. What the repository actually does

| Path | Verified behavior and implication |
|---|---|
| `third_party/fetch_skia_cpu.sh:7–9,46–56` | Fetches the exact libpng, Chromium zlib and Skia FreeType pins. No alternate fetch host exists in this script. |
| `third_party/skia_cpu_args.gn:43,55–61` | PNG read/write and bundled zlib enabled; system zlib and Skia FreeType disabled. Linux derives its configuration from this profile. |
| `CMakeLists.txt:785–837,886–896,3386–3387` | Requires `libpng.a` and `libzlib.a`, creates imported archive targets, links them privately, and installs the root zlib `LICENSE` as `zlib-LICENSE.txt`. |
| `src/render/skia/raster/skia_raster.cpp:546–560,637`; `executor/skia_executor.cpp:1054–1150` | PNG decode and encode use `SkPngDecoder` / `SkPngEncoder`, which use pinned libpng. `raster/clipboard_png.cpp` supplies another PNG read/write path. |
| `src/core/image/png_writer.cpp:53–76,112–137` | First-party writer emits stored deflate blocks, at most 65,535 payload bytes each, with its own CRC-32 and Adler-32. No compression library is called. |
| `src/core/resources/image_registry/image_registry.cpp:51–74,165–350` | A second first-party CRC implementation validates PNG structure/chunks and size limits; this validator does not validate the deflate payload. Its existing fuzz target therefore does not prove inflate safety. |
| `src/host/windows/application/windows_host.cpp:838–881,2140–2188` | Windows application painter decodes PNG with WIC into premultiplied BGRA. This path does not call project zlib. |
| `third_party/skia_cpu_windows_mingw_args.gn:76–82`; `CMakeLists.txt` Skia targets | A separate Windows Skia/GUI.Drawing configuration does include libpng/zlib. Thus “Windows uses no zlib” is true of the WIC application path, not every Windows target. |
| `src/host/macos/services/appkit_clipboard_image.mm:25–83,101–108,119–142` | Clipboard native PNG decode uses ImageIO; it also falls back to TIFF. Native PNG encoding uses `NSBitmapImageRep`. A private raw RGBA format bypasses compression. Both PNG read and write need migration to achieve one library. |
| `src/render/coregraphics/raster/coregraphics_raster.cpp:373–379` | The CoreGraphics comparison/smoke path decodes through ImageIO. Either migrate its PNG decode or explicitly retire that path; leaving it untouched prevents a universal PNG claim. |
| `CMakeLists.txt:240–248`; pinned FreeType `CMakeLists.txt:185–189,296,361–365,439`; `include/freetype/config/ftoption.h:198,227` | `FT_DISABLE_ZLIB=ON` disables discovery of system zlib. It leaves `FT_CONFIG_OPTION_USE_ZLIB` defined and compiles `ftgzip.c`, which includes bundled inflate and checksums. |

FreeType's `src/gzip/ftgzip.c:41–120` compiles the bundled implementation
with internal symbol visibility; absence of public zlib symbols is not proof
of its removal. `src/sfnt/sfwoff.c:354–365` uses `FT_Gzip_Uncompress` for WOFF 1
table compression. The gzip font wrapper initializes **raw deflate** with
`-MAX_WBITS`; `FT_Gzip_Uncompress` initializes **zlib/gzip autodetection** with
`MAX_WBITS | 32` (47). Redirecting FreeType unchanged therefore expands the
required modes beyond PNG.

Bundled stock font assets are TTF/OTF, and no `.gz` or WOFF font asset was
found. However, `SkiaExecutor::register_typeface` accepts encoded bytes and
passes them through face selection to `FT_New_Memory_Face`; it does not
establish a TTF/OTF-only gate. The bundled inflate is compiled in, but “never
exercised” is not a security guarantee. Final binary retention also depends
on linking; this study has not produced a link map.

The registry defaults are 32 MiB encoded per image, 4096×4096 dimensions,
64 MiB decoded per image, 4096 chunks, and 256 MiB aggregate decoded storage
(`include/gui_forms/resources/types/image_resource_types.hpp:22–31`).
Clipboard `HostImage` permits 100,000,000 pixels / 400,000,000 bytes.
These are distinct existing policies, not interchangeable inflate limits.
An RGBA8 destination bound alone does not bound a 16-bit PNG's filtered data
or compressed ICC/text metadata.

## 3. Exact C subset and compatibility envelope

The admitted Skia PNG source calls libpng, not zlib directly. Skia's direct
zlib user `src/pdf/SkDeflate.cpp` is behind `skia_enable_pdf`, which is false
(`BUILD.gn:1257–1273`). ZIP/minizip, gzip file APIs and the ANGLE compression
utility are not PNG consumers and are not part of the shim proposal.

Skia's `third_party/libpng/BUILD.gn` compiles 15 libpng C units and uses its
wrapper `pnglibconf.h`, which includes the pinned prebuilt configuration.
Preprocessing those units with the pinned zlib header confirms **ten distinct
called entry points**. The [call evidence](../../../results/2026-10-19-zlib/libpng-active-calls.txt)
retains the actual `Cr_z_` names; the source API names follow.

| Entry point | Call sites in pinned libpng | Required behavior |
|---|---|---|
| `crc32` | `png.c:143,186` | Incremental PNG chunk CRC, including `crc32(0, Z_NULL, 0)` initialization and count splitting to `uInt`. |
| `inflateInit2_` (through `inflateInit2`) | `pngrutil.c:488` | Initialize `z_stream` with allocator callbacks, version and structure-size checks; `windowBits=0` derives window from header, or 15 under Skia's maximum-window option. |
| `inflateReset2` | `pngrutil.c:479` | Reuse allocation between separate compressed chunks, adopt new window setting, discard old history/checksum and progress. |
| `inflateReset` | `png.c:985`; `pngrutil.c:724` | Reuse state/settings for another stream, including the two-pass compressed metadata path; never retain dictionary/history from the prior stream. |
| `inflate` | `pngrutil.c:538` through `PNG_INFLATE` | Streaming IDAT and iCCP/zTXt/iTXt, progressive and sequential reads; `Z_NO_FLUSH`, `Z_SYNC_FLUSH`, `Z_FINISH`; precise consumed/produced counts and no-progress behavior. |
| `inflateEnd` | `pngread.c:797` | Release internal allocations on success and read-error cleanup. Never release caller buffers. |
| `deflateInit2_` (through `deflateInit2`) | `pngwutil.c:434` | Level, method, windowBits, memLevel, strategy, version and stream-size validation; libpng reduces windowBits for small data. |
| `deflateReset` | `pngwutil.c:430` | Reuse allocations when settings are unchanged; new stream with clean history. |
| `deflate` | `pngwutil.c:597,982` | Incremental rows and compressed metadata; `Z_NO_FLUSH`, `Z_SYNC_FLUSH`, `Z_FINISH`, including repeated finish calls with tiny output buffers. |
| `deflateEnd` | `pngwutil.c:412`; `pngwrite.c:993` | Cleanup/reinitialization when compression settings change and at destruction/error. |

`inflateInit` and `inflateInit_` are old-version conditional branches, not
active calls with this pin. `inflateValidate` is under
`PNG_DISABLE_ADLER32_CHECK_SUPPORTED`, which the prebuilt configuration leaves
undefined. `deflateBound` occurs in a comment, not a call. `compress`,
`compress2`, `uncompress`, `zlibVersion`, `zError`, `adler32`, dictionary APIs,
copy APIs, combine APIs and `gz*` are not called by the admitted PNG units.
Adler-32 remains mandatory internally and in the native checksum API.
Do not advertise unused entry points as implemented to make a header look
complete. A changed libpng configuration must regenerate this inventory.

The compatibility header must supply `Byte`, `Bytef`, `uInt`, `uLong`,
`voidpf`, `alloc_func`, `free_func`, `z_stream`, `z_streamp`, init macros,
`ZLIB_VERSION` / `ZLIB_VERNUM`, the relevant flush/strategy/error constants,
and matching C linkage and calling convention. Preserve target ABI types:
`uLong` is `unsigned long`, not forcibly 64 bits on Windows. Keep exact
internal counters separate from ABI totals, whose representational behavior
must match the reference. Allocation multiplication must be checked before
calling `zalloc(opaque, items, size)`; match every successful allocation with
the same owner's `zfree`. Handle null callbacks according to the declared
default allocator contract. A copied live `z_stream` is not a second owner.

For every entry point, the implementation PR must document pointer/count
validity, null-with-zero cases, retained state, output mutation, reset/end
rules, errors and callback lifetime. Input and output ranges must be disjoint;
do not form out-of-bounds pointers even while checking them. The native core
borrows buffers only for a call and keeps its own bounded history. Shim
`next_in` / `next_out` are advanced as required, but never dereferenced after
their call without a new valid caller-provided extent. Callbacks cannot throw
or reenter the same stream; no C++ exception may cross C.

Recommended compatibility scope includes levels -1 and 0–9, method 8,
memLevel 1–9 and all five standard strategies (default, filtered, Huffman-only,
RLE, fixed), since libpng exposes customization. Pin -1 to level 6. Honor
zlib's positive deflate windowBits=8 promotion to 9; test actual headers and
round trips accordingly. Native inflate should handle format windows 8–15;
raw mode and gzip depend on the FreeType decision. Reject unsupported modes
or flush values explicitly; never reinterpret them as a supported mode.

Chromium's header prefixes symbols with `Cr_z_`. Recommend our own private
prefix through source-compatible macros and rebuilding all consumers against
our `zlib.h`; do not promise compatibility with already-built Chromium
objects. This avoids collisions with system zlib and the differential oracle.
Expose no global `zlib.h` through GUI.Forms' public include directory, and
do not freeze the eventual GUI.Forms stable C ABI through this private shim.

## 4. Formats and trust boundaries

[RFC 1950](https://www.rfc-editor.org/rfc/rfc1950) defines the zlib wrapper:
validate method/window, header check bits, dictionary request and Adler-32.
PNG uses this wrapper. [RFC 1951](https://www.rfc-editor.org/rfc/rfc1951)
defines stored, fixed-Huffman and dynamic-Huffman deflate blocks. The decoder
needs all three, including matches spanning input/output calls and block
boundaries. Raw deflate is a separate API mode, never format autodetection
for PNG.

[RFC 1952](https://www.rfc-editor.org/rfc/rfc1952) is unnecessary for PNG.
There is no identified stock gzip-font workload. If the owner keeps
FreeType compressed-font support unchanged, its raw and auto-detect calls
make gzip support required; include bounded optional headers, CRC/ISIZE
verification, truncation, and an explicit concatenated-member policy. PNG
must still reject gzip. Compiling out FreeType compression avoids that scope
without pretending its current code is inactive by construction.

The [PNG compression specification](https://www.w3.org/TR/2025/REC-png-3-20250624/#10Compression)
also compresses metadata independently of IDAT. Treat each stream as hostile
and charge every reset/replay to the enclosing image's aggregate budget.
PNG does not allow preset dictionaries. Return a named dictionary result
after validating/reading the request; the PNG boundary rejects it. Neither
CRC nor Adler is authentication: an attacker can recompute both.

## 5. Proposed inflate contract and threat model

Protected assets are memory integrity, availability, process isolation,
consistent decoded bytes, and ownership of published images. Attackers can
supply resource bytes, clipboard bytes, metadata and arbitrarily split
streams. They control headers, block counts, code lengths, distances,
checksums and termination. Public buffer pointers remain an in-process
caller contract; stream validation cannot make an arbitrary invalid address
safe. One mutable stream belongs to one execution context.

Proposed native result record: status, input bytes consumed, output bytes
produced, total input/output counts and a stable detailed error. Distinguish
`need_input`, `need_output`, `stream_end` and errors. Initialization validates
settings/storage before mutating a live stream. Stream processing may publish
an explicitly counted **unverified prefix** before a malformed suffix or
checksum failure; it never promises transactional output. Callers must not
publish an image until the wrapper checksum and PNG validation finish.
Terminal errors freeze the stream until reset or destruction. Reset clears
state, not the caller's output. Init failure leaves no live state or leaked
allocation. End frees only owned state. No exceptions, allocation or logging
inside decode processing.

| Hostile case | Required guard and proposed named error |
|---|---|
| Invalid method, window or header check | Validate wrapper before decoding; `invalid_method`, `invalid_window`, `invalid_header_check`. |
| Preset dictionary | Read bounded DICTID; `dictionary_required` (mapped to `Z_NEED_DICT`); PNG rejects. No implicit dictionary or prior-stream history. |
| Reserved block type | Reject BTYPE=3 with `invalid_block_type`. |
| Stored-block length mismatch / truncation | Validate LEN/NLEN complement and available bytes; `stored_length_mismatch` or `truncated_input`. LEN is only 16 bits; never invent a larger stored payload from remaining input. |
| Oversubscribed or incomplete trees | Check counts/Kraft space and bounded table capacity before publication; distinct `oversubscribed_tree` / `incomplete_tree`. Accept the valid degenerate cases described below. |
| Bad code-length repeat | Reject repeat-previous without a prior length; prevent repeats past the declared array; `invalid_code_length_repeat`. |
| Missing EOB, invalid/reserved literal or distance symbols | Validate literal tree's EOB and decoded symbols before use; `missing_end_of_block`, `invalid_literal_length`, `invalid_distance_code`. |
| Distance before produced output or beyond valid history | Check available initialized history before subtracting/reading; `distance_before_history`. Never synthesize zero-filled missing history. |
| Distance beyond permitted window | Check stream/configured window explicitly; `distance_exceeds_window`. |
| Length/output overrun | Checked remaining-count arithmetic before copying; bounded partial match state across calls; `output_limit_exceeded` for exhausted total quota, `need_output` only for temporary buffer exhaustion. |
| Decompression bomb | Total output, input and work quotas; no growth in response to stream bytes. Charge bytes actually emitted and work even when no output is emitted. |
| Empty-block/tree-construction CPU bomb | Debit block parsing and table construction from work quota; `work_limit_exceeded`; no zero-progress spin. |
| Truncated final input | `need_input` while more input is possible; an explicit end-of-input declaration converts unfinished parsing to `truncated_input`. Zero-length input is not implicit EOF. |
| Corrupt trailer | `adler_mismatch`; already-written prefix is invalid for publication. |
| Counter/size overflow or invalid call | Check additions/products/conversions; `size_overflow`, `invalid_argument`, `invalid_state`, `workspace_too_small`. Never rely on release-disabled assertions. |
| Stream tails / concatenation | Return exact boundary and leave unused bytes available; PNG integration rejects excess compressed payload under the agreed strict policy. Do not silently start another stream. |
| Vector edges and overlapping LZ matches | No speculative read/write outside supplied live ranges. Copy short-distance repetitions in deflate semantics; ordinary `memcpy`/`memmove` is not sufficient for all expanding matches. |

Incomplete-tree handling needs deliberate tests. Upstream `inftrees.c`
rejects incomplete code-length alphabets, but permits certain one-symbol,
one-bit literal/distance alphabets. A distance alphabet with no usable code
can describe a literal-only block; using it for a match is an error. Rejecting
every incomplete tree would reject streams the reference accepts. Test unused
invalid table entries, symbol 256, repeats across the literal/distance boundary,
and reserved fixed-tree symbols separately.

**Proposed storage bound:** at most 64 KiB per native inflate stream,
excluding caller-owned input/output. Reserve at most 32 KiB history, 16 KiB
bounded decode tables, 4 KiB code-length/work arrays, 4 KiB state and 8 KiB
headroom. These are design ceilings, not measured sizes. Phase 2 must derive
table capacities, prove all indices and publish actual `sizeof`/peak scratch
measurements, including stack. SIMD must not silently add readable padding
to the caller contract. The shim may allocate this bounded workspace at init
through its allocator; the native interface takes caller-supplied workspace.
If the implementation cannot meet the ceiling, return to the owner before
changing it. Deflate needs its own preflight storage bound by window/memLevel;
propose a 1 MiB maximum, subject to the same measured gate in phase 3.

**Proposed time bound:** finite configured input limit I, output limit O,
work limit W. Charge work before each input-bit extraction, decoded symbol,
table-entry initialization and emitted/copied byte, with checked counters.
Precharge a proven bounded batch for an interior kernel and reconcile actual
use afterward, so accounting need not add an increment to every byte loop.
Bound table construction by the format's fixed alphabet/code-length limits.
Every inner loop consumes input, emits output, completes a bounded state
transition or exhausts work. The resulting algorithmic bound is linear in
consumed input + output + bounded work per block, additionally stopped by W;
it is not a hard wall-clock deadline. Empty calls return without spinning.
Cancellation is checked at bounded call/work boundaries by the owner of the
stream. Default product quotas and any per-call scheduling budget must be
recorded and calibrated against the corpus; no unlimited sentinel is allowed
on an untrusted PNG path.

For PNG, derive filtered output length from IHDR with checked arithmetic:
for each nonempty Adam7 pass (or the single noninterlaced pass), sum
`pass_height * (1 + ceil(pass_width * bits_per_pixel / 8))`. Validate the
expected filtered count independently of the destination RGBA byte count.
Metadata needs separate per-stream and aggregate decompressed limits, plus
allocation/chunk limits. Two-pass metadata decode spends work twice.
Keep a request-level budget outside stream resets so a file cannot reset its
quota by adding chunks. The exact product limits remain owner-reviewed;
the core always requires explicit finite maxima.

**Compatibility tension requiring recorded resolution:** upstream's
advertised-window distance check is guarded by `INFLATE_STRICT`, and Skia
sets `PNG_MAXIMUM_INFLATE_WINDOW` to tolerate incorrect CMF bytes. An ordinary
upstream build is therefore not an unquestionable strict-format oracle.
Also test the reference's rejection of dynamic HDIST counts above 30 even
when reserved distance codes would be unused. Recommend strict PNG handling
and differential runs against both the pinned ordinary and
`-DINFLATE_STRICT` reference builds. Any accept/reject discrepancy must be
minimized, classified against the format and approved; it cannot be counted
as passing or silently allowlisted. Resource-limit failures are compared
under equal harness limits, separately from malformed-stream rejection.
The owner must settle these compatibility cases before the conformance gate
closes; no assertion here relaxes the requested agreement requirement.

## 6. Chromium additions: benefit, risk and disposition

The actual pin identifies itself as `1.3.0.1-motley`; its root licence and
README contain older version statements. Use commit identities and per-file
notices, not those labels, to establish provenance.

| Addition at the pin | What it offers | Proposed treatment |
|---|---|---|
| `adler32_simd.c`, SSSE3 / NEON | Parallel byte sums and weighted sums reduce checksum work for large buffers; bounded reduction prevents accumulator overflow. | Portable baseline first, then measured vector implementations with exact checksum equality. Tiny input may favor baseline. |
| `crc32_simd.c`, x86 PCLMUL and Arm CRC/PMULL machinery | Polynomial folding / accelerated IEEE CRC reduces chunk-check cost. AVX-512 source also exists. | The Skia GN graph selects SSE4.2/PCLMUL on x64 and Arm CRC path on arm64; it does not enable the AVX-512 path. Do not attribute unbuilt variants to current performance. |
| `contrib/optimizations/inflate.c`, `inffast_chunk.c`, `chunkcopy.h` | Wider bit reads and 16-byte match copies reduce branches/copy overhead in interior loops. | Useful design candidates after scalar correctness. Upstream assumptions include extra history padding and relaxed copies; some helpers suppress MSan. Do not transplant those assumptions or suppressions into the new core. |
| `contrib/optimizations/insert_string.h` | Current ANZAC++ multiply/add hash changes dictionary collision distribution and match-search cost. A compatibility macro selects the older rolling hash. | Current path loads a native 32-bit word and explicitly notes little-endian validation. Fresh fixed-endian arithmetic and fixed search/tie rules are necessary for identity. Compression quality/throughput must both be measured. |
| `slide_hash_simd.h` | Vector subtract/saturate updates dictionary positions when the window slides. | Can preserve scalar semantics; test rollover, zero sentinel and window edges on every path. |
| `crc_folding.c` | Intel copy-with-CRC path serves gzip deflate in this pin. | No demonstrated PNG benefit from that particular path; exclude if gzip deflate is unnecessary. |
| `cpu_features.c`, `chromeconf.h` | Feature selection, symbol isolation and build integration. | Keep feature detection private and thread-safe; publish immutable selected kernels. Use a project prefix and platform-neutral semantic contracts. |

No numerical speedup is established by this inspection. Measure each change
alone against the portable path and the pinned baseline. An upstream patch's
speed claim is not a GUI.Forms result.

The x86 SSE4.2 `CRC32` instruction computes **CRC-32C**, not PNG's IEEE
CRC-32. PNG needs the IEEE polynomial (reflected `0xedb88320`); use correct
PCLMUL folding/reduction, not that instruction as a drop-in replacement.
NEON/SSSE3/AVX2 are candidates for Adler arithmetic; PMULL/PCLMUL for CRC.
Check runtime CPU and OS vector-state support before selecting a kernel,
keep baseline objects free of unsupported instructions, and select once
before processing loops. Forced-path tests must skip unsupported hardware
honestly, not execute illegal instructions or claim coverage they lack.

## 7. Deflate identity and checksum plan

Levels must retain zlib's meaning: 0 stores, 1 favors speed, 9 favors
compression, -1 means the pinned default 6, and intermediate levels have
specified search/lazy-match budgets. A higher level need not produce a
smaller file for every input. Record each level's search limits and memory;
do not promise upstream byte identity merely because level numbers match.

The proposed identity rule is stronger than choosing the same algorithm:
for fixed input, level and explicit settings, the one-shot API emits identical
bytes across hosts, compilers and available vector paths. Fix byte order,
unsigned arithmetic widths, hash, candidate order, equal-length match tie
break, Huffman tie break, block boundaries, block-type tie break and flush
policy. No pointer-address hashing, randomized containers, native-word loads
with unspecified endian interpretation, floating-point cost decisions, or
vector-width-dependent match choices. Any output-changing improvement needs
a versioned identity profile and owner review.

For streaming, transport segmentation must not choose block boundaries:
buffer bounded lookahead until the canonical decision is possible. The same
bytes/settings and logical flush positions must yield the same stream for
one-byte and larger input/output spans. Explicit `Z_SYNC_FLUSH` positions are
part of the input contract and may change bytes; “same input and level” means
the canonical finish-only profile unless another flush schedule is stated.
This applies ADR-004's identity principle to compression without claiming
that the text-specific ADR already specifies deflate.

CRC-32 and Adler-32 get portable incremental implementations first. Tests
cover empty input, initialization, arbitrary prior state, concatenation by
incremental calls, every short length/alignment, reduction boundaries, page
ends and long overflow-sensitive inputs. Each vector path must equal the
portable path and the pinned oracle, including tails. Existing first-party
writer and registry checksums should converge on this implementation during
integration. Do not choose the writer's compression level until throughput,
size and memory measurements exist.

## 8. Provenance and licences

The [pinned zlib notice](https://chromium.googlesource.com/chromium/src/third_party/zlib/+/646b7f569718921d7d4b5b8e22572ff6c76f2596/LICENSE)
requires retained notice, accurate origin and plainly marked alterations.
Chromium-authored files such as `adler32_simd.c`, `crc32_simd.c`,
`chunkcopy.h`, `insert_string.h` and `slide_hash_simd.h` refer to the
[Chromium BSD-3-Clause notice](https://chromium.googlesource.com/chromium/src/+/main/LICENSE).
Preserve source notices and reproduce required binary notices; do not use
upstream names as endorsement. Retain a pinned copy/hash of the applicable
Chromium notice in the eventual import manifest. The current packaging rule
installs only the root zlib notice; the replacement intake must explicitly
inventory the additional notices.

Not every modification follows the same notice: `inffast_chunk.c` names
Mark Adler and Chromium but points to `zlib.h`; `crc_folding.c` names Intel
and also points to `zlib.h`. Preserve each file's actual terms instead of
relicensing the whole import as BSD. This is an engineering provenance
inventory; ambiguous files block their import until resolved.

| Material | Phase 1 status | Proposed later classification |
|---|---|---|
| Chromium zlib source snapshot | Read-only audit checkout; not vendored or linked by this PR | Import the exact pinned source, hashes and notices in a clearly isolated reference area, then track every changed/retired file. A reference import alone is not completion. |
| House-style translation of upstream algorithms/code | None written | **Derived from** the named source/revision; preserve notices and mark alteration even after substantial refactoring. |
| New stream ownership, budgets, result API and state orchestration | Design only | **Written fresh against the RFCs** only where that is accurate; record sources consulted and avoid calling a port a clean-room implementation. |
| Fresh checksum/table generation | None written | Record derivation equations, generation recipe and tests. Copied tables or RFC sample code require their own attribution. |
| zlib-ng / libdeflate implementation | No code copied | Comparison only. Owner approval is required before any code reuse. |

[zlib-ng's pinned licence](https://github.com/zlib-ng/zlib-ng/blob/860e4cff7917d93f54f5d7f0bc1d0e8b1a3cb988/LICENSE.md)
is zlib; it provides a useful optimized zlib-compatible comparison.
[libdeflate's pinned licence](https://github.com/ebiggers/libdeflate/blob/96836d7d9d10e3e0d53e6edb54eb908514e336c4/COPYING)
is MIT; its [documented whole-buffer focus](https://github.com/ebiggers/libdeflate/blob/96836d7d9d10e3e0d53e6edb54eb908514e336c4/README.md)
makes it a useful throughput comparison, not a substitute for the required
streaming contract. Their per-file notices still need review if reuse is ever
approved. No new production third-party dependency is proposed.

## 9. Proposed ownership, PNG choice and integration

Recommend the working name **GUI.Forms Deflate**, target
`gui_forms_deflate`, with public native declarations under
`include/gui_forms/compression/`, implementation under
`src/core/compression/`, and private compatibility headers under
`src/core/compression/zlib_compat/`. Keep algorithm tables, CPU dispatch,
allocator details and zlib structs private. This follows the existing public
seam; it is a proposal, not a chosen product name or stable ABI.

The core owns no filesystem, font, renderer or host dependency. Callers own
storage and budgets; the state retains only its workspace/history, settings,
counters and named function targets. Distinct wrapper types separate byte
counts, byte offsets, bit counts and window distances. Allocation and format
choice precede repeated kernels. Named callbacks have explicit context;
destroy/reset cannot leave live borrows or permit stale callbacks.

| PNG choice | Advantages | Costs and acceptance obligations |
|---|---|---|
| **Pinned libpng everywhere (recommended)** | Existing pinned parser and Skia integration; mature filtering/interlace/bit-depth behavior; one dependency already admitted. Windows can use libpng without adopting Skia as its painter. | Shared private adapter must enforce budgets across IDAT and metadata; isolate C `setjmp`/`longjmp` so it cannot bypass C++ object destruction. Specify pixel order, premultiplication, transparency, colour profile and error behavior identically on every host. Bound libpng's own allocations as well as inflate. |
| First-party PNG codec on the owned deflate | Direct ownership of parsing, budgets and error reporting; can simplify the common call path and remove libpng from that path. | A second security-sensitive parser project: chunk ordering/CRC, all admitted colour types/depths, filtering, Adam7, tRNS/palette and metadata/profile handling. Must preserve the existing admitted PNG capability, not reduce it to RGBA8/noninterlaced. Skia's PNG integration still needs explicit routing or a build patch. Separate staged review is needed. |

Whichever is chosen, keep native clipboard transport and raw RGBA/DIB
interchange, but move **PNG decoding and encoding** into the shared path.
WIC removal is decided. ImageIO PNG decode, `NSBitmapImageRep` PNG encoding,
and the CoreGraphics smoke path must be accounted for. The current TIFF
clipboard fallback is an owner decision because removing it changes existing
interoperability; retaining it must be an explicit host-only exception and
must not be described as PNG-only or as using the owned zlib.

An unchanged zlib `inflate` signature has no total-output-limit argument.
Passing small `avail_out` buffers does not solve that. Propose a **private,
named decode-budget scope** at the PNG adapter boundary: initialized before
calling libpng, borrowed by shim states during that synchronous decode,
including metadata streams created before image allocation. It has one
execution context, explicit nested/reentrant rules, request-wide counters
that survive reset, and cannot outlive its stream states. Missing budget on
an untrusted decode is an initialization error. A thread-local implementation
of the scope is a candidate, not permission for a process-global mutable
limit. Phase 5 must prove budget propagation through Skia/libpng; a C++ facade
that merely caps its final pixel buffer is insufficient.

The build integration should patch Skia's private zlib target to expose the
shim headers and resolve to the one first-party archive. Do not flip
`skia_use_system_zlib=true` and accidentally resolve a host package. Plan
archive order, C++ runtime linkage from C consumers, symbol prefixing and
installed-package closure together; avoid two separately compiled copies
with different flags. Keep all GPU and non-PNG exclusions. Produce generated
GN/CMake dependency lists and link maps; check for both external zlib and
FreeType's private bundled inflater, which cannot be ruled out by a public
symbol search alone. Update `third_party/SKIA_BUILD_EVIDENCE.md` with new
measurements; its 2026-08-03 results are historical, not proof of this change.

FreeType options: compile out `FT_CONFIG_OPTION_USE_ZLIB` with a reproducible
private configuration/patch (recommended for current bundled stock fonts),
or use `FT_CONFIG_OPTION_SYSTEM_ZLIB` with explicit include/link resolution
to **our** target, never arbitrary system lookup. The latter requires raw
inflate and mode 47 behavior, plus bounds for its font callers. Prove compressed
font rejection if disabled, or round-trip/differential safety if retained.

## 10. Bounded PR sequence and verification gates

Every code phase runs applicable conformance, differential, fuzz, sanitizer,
checker and measurement gates. Keep exact commands, seeds, pins, machine,
compiler, flags, elapsed hours, coverage and negative results under
`results/<date>-zlib/`. A missing platform is pending evidence, never a pass.
Owner approval is required before starting the next phase. Stack each branch
on its accepted predecessor while unmerged; retarget after merges without
pulling unrelated changes into the stack.

| Phase / separate PR | Work and gate |
|---|---|
| **1 — this PR** | Study, source inventory, threat model, proposals and recorded unknowns. Gate: owner accepts the study and resolves decisions needed for phase 2. |
| **2 — inflate first** | Exact pinned import/provenance record and C++20 bounded scalar inflate, with portable Adler needed for wrapper validation. No production switch. Native result/storage/streaming contract, malformed vectors, differential harness and inflate libFuzzer target pass. |
| **3 — deflate** | Levels/settings, finite workspace, streaming and canonical identity. Cross-platform compressed-byte hashes plus round trips against the oracle. Initial compression measurements. |
| **4 — checksums and measured acceleration** | Consolidated portable CRC/Adler and x64/arm64 vector paths; every path matches baseline. Add optimized inflate/deflate kernels only with preserved bounds/identity and isolated measurements. |
| **5 — C shim and candidate integration** | Ten-entry subset, ABI/allocator/error tests and shim libFuzzer target. Build unmodified pinned libpng against shim; prove budget propagation and sanitized candidate Skia graph. Retain old fetch/baseline until replacement acceptance is complete. |
| **5b — owned PNG codec, separate PR** | Owner revision: implement the full admitted PNG decode/encode path in house style on owned deflate. Review PNG chunk/order/CRC rules, all admitted colour depths and Adam7, exact filtered-size arithmetic, metadata/resource limits and failure publication before coding; test malformed PNGs and differential pixels. libspng/LodePNG remain design references with per-file provenance required before copying. |
| **6 — every platform and final cutover** | Chosen shared PNG path; remove WIC, migrate other PNG paths, apply FreeType decision, consolidate writer/registry checksums, select writer compression level from results. Full acceptance matrix and packaging/link-map audit precede removing Chromium's fetch. |

### Conformance and differential

Use hand-authored vectors derived from RFC fields, not a claim that the RFCs
ship a comprehensive test suite. Cover empty streams; all block types;
code-length repeats; maximal codes, matches and distances; circular history
wrap; zero-length stored blocks; LEN=65,535; every error in §5; dictionaries;
checksum errors; trailing data; and all truncation positions.
For small vectors enumerate every input split point and output split point,
their cross-product, and one-byte-in/one-byte-out schedules; use recorded
random schedules for larger vectors. A pause may occur in any header, extra
bit field, Huffman code, match, checksum or finish operation. Zero-capacity
calls, resets, repeated finish and allocator failure at every allocation
must have tests.

Round-trip every level and supported window (including the 8→9 encoder
promotion), memLevel and strategy. Compare decoded bytes exactly against
upstream zlib at the recorded pin on generated and fuzzed inputs; compare
terminal acceptance/rejection after explicit EOF and equal limits. Preserve
the first differing consumed/produced count and minimized input. Test both
directions: our deflate into upstream inflate and upstream deflate into ours.
Compare native detailed errors separately from coarse C return codes.

The shim maps malformed streams/checksum errors to `Z_DATA_ERROR`, dictionary
requests to `Z_NEED_DICT`, no progress to `Z_BUF_ERROR`, invalid state/settings
to `Z_STREAM_ERROR`, allocation failure to `Z_MEM_ERROR`, and ABI/version
mismatch to `Z_VERSION_ERROR`. Native quota errors have no exact zlib code:
propose `Z_MEM_ERROR` plus a stable private detailed reason, and test libpng's
handling. Temporary buffer shortage must not become a quota error, and EOF
must not turn a truncated stream into `Z_STREAM_END`. These extensions require
explicit documentation; they are not claims of full zlib compatibility.

### Fuzzing and sanitizers

Add named-target libFuzzer harnesses under `fuzz/`, following the existing
build layout but instrumenting the actual new implementation. Inflate fuzzing
varies bytes, splits, EOF, limits and reset; shim fuzzing additionally varies
legal/illegal API sequences, allocator failures and partial initialization.
Do not manufacture invalid raw pointers in the harness and mistake harness
UB for a library result. Retain crashes, timeouts, OOMs, minimized reproducers,
corpus hashes and coverage reports. A standalone mutation fallback is useful
but is not a libFuzzer-hours result.

Planned corpora, authorized by this named plan: Silesia from its
[maintainer's corpus page](https://sun.aei.polsl.pl/~sdeor/index.php?page=silesia),
repository PNGs, PNG byte fixtures in tests/fuzz, generated flat/gradient/text
UI rasters, and crafted malformed zlib/deflate streams. **Observed gap:**
`assets/` contains no `.png`/`.PNG` files at the audit revision. Do not report
an empty assets corpus as tested. Inventory repository PNGs separately and
add a deliberate `assets/` fixture set in a code phase for owner review;
hash all corpus members and preserve source/provenance. No corpus was
downloaded in phase 1. Ask before any additional download beyond the pinned
sources, these corpora and comparison libraries.

Proposed minimum campaigns per relevant code gate: 24 hours per active
libFuzzer target on x64 and arm64 after corpus merge, plus deterministic
sanitized corpus replay on Linux, macOS and Windows. This is a proposed
minimum, not a safety proof. Record actual hours/execs/edges/features and
source region/branch coverage; inspect unvisited parser/error branches and
add targeted vectors. Phase 1 hours: **0**; coverage: **not measured**.

Use the repository's pinned LLVM 22.1.8 and build helpers. Required targeted
command once tests exist:

```text
python3 -B tools/ci_build.py --sanitize --tests 'gui_forms_(inflate|deflate|checksum|zlib_shim|png_codec).*'
```

Those test names are proposed, not existing targets. Also run the whole suite
with `python3 -B tools/ci_build.py --sanitize` before closing a code phase,
as required by the house style. Do this on Linux, macOS and Windows; LSan
coverage is available in the current helper on Linux/macOS, not its Windows
MinGW runtime. `tools/ci_build.py:41–54` explicitly excludes the prebuilt Skia
archive from instrumentation. Therefore the shim/candidate PNG integration
needs an instrumented dependency build or separately instrumented integration
target, with evidence that the new core and shim actually received the flags.
No suppressions for our code or tests; any report is a defect.

Run both full first-party checker commands from AGENTS.md, and include every
new first-party directory (especially `fuzz/`) explicitly if outside that
list. Manually review all eleven house-style sections. Remote CPU-heavy
repository builds use `/Users/ultimussecundai/.local/bin/m4build -- <command>`;
read-only Mini inspection uses the route returned by `m4host`. Do not modify
the mirrored tree as the authoritative copy or change remote software.

### Measurements and final removal gate

Compare pinned Chromium zlib, upstream zlib, zlib-ng, and the candidate on the
same machine/flags/settings. Include libdeflate as a separately labelled
whole-buffer comparison; its level numbers/API do not establish equivalence.
For Silesia, operate on original bytes. For PNG compression, separately measure
extracted filtered IDAT data and end-to-end PNG encode/decode; recompressing
the already-compressed PNG file is a different workload. Hold filter/colour
settings constant when isolating deflate. Generated rasters include flat
colours, gradients, alpha edges, repeated icons and text at fixed sizes and
seed, using bundled fonts.

Record deflate and inflate throughput for streams produced at each level
0–9/default, compressed size and ratio (compressed/original), setup and
steady-state costs, peak library allocation and process RSS, small-buffer
latency, and scalar/vector variants. Record CPU/ISA, OS, RAM, compiler version,
optimization/LTO/architecture flags, corpus hashes, warm-up, repetitions,
median and spread; keep regressions and failed runs. Use multiple sizes and
one-byte through large streaming buffers. Establish cross-platform identity
separately from same-machine speed comparisons.

Remove the Chromium fetch only after all required platform tests, differential
agreement, fuzz campaigns, sanitizers, identity checks, benchmark tables,
licence packaging and link/dependency audits are complete and accepted.
The source import must be self-contained and traceable; normal builds must
no longer contact googlesource for zlib. No threshold for calling the library
“high efficiency” is invented here: present the measured trade-offs to the
owner, choose the writer level from them, and do not conceal a regression.

## 11. Owner decisions and subsequent source-use authorization

The owner accepted all six recommended A options on 2026-10-19, and required
a precursor light audit of non-GPL open-source replacements, prioritizing C++.
That [audit is recorded separately](ALTERNATIVES_LIGHT_AUDIT.md). The alternatives
below remain as decision history. Subsequently the owner authorized taking
suitable reference material for both libraries and rewriting it in house
style. This changes choice 2 to B (owned PNG), permits source reuse despite
the lack of a verified leak audit, and starts phase 2. It does not waive
verification or licence/provenance requirements.

1. **Name and boundary.** A — **recommend** GUI.Forms Deflate,
   `gui_forms_deflate`, public native declarations in
   `include/gui_forms/compression/`, private implementation/shim in
   `src/core/compression/`. B — a separately named standalone library subtree
   with its own packaging contract. C — owner-supplied name/location.
2. **PNG ownership (revised).** A — initially recommended and accepted:
   pinned libpng on owned zlib on every host. **B — now accepted:** first-party
   full admitted PNG codec, with an additional parser study and acceptance
   stage. The owner's later instruction to rewrite both supersedes A.
3. **FreeType.** A — **recommend** compile out `FT_CONFIG_OPTION_USE_ZLIB`
   and explicitly reject compressed font formats. B — redirect FreeType to
   our shim and include raw deflate/gzip/autodetect support and font budgets.
4. **macOS TIFF clipboard fallback.** A — **recommend** PNG plus existing
   raw-pixel interchange only, removing the TIFF fallback. B — retain the
   existing host-only TIFF conversion as an explicit exception, while all PNG
   operations use the shared path. Neither option introduces a new codec.
5. **Strictness and differential oracle.** A — **recommend** strict format
   validation, ordinary plus strict pinned-zlib comparisons, with every
   discrepancy resolved and approved before passing the gate. B — preserve
   existing Skia/ordinary-zlib tolerance where demonstrably memory-safe,
   specifying each permitted malformed case before implementation.
6. **Initial resource/fuzz gates.** A — **recommend** the proposed 64 KiB
   inflate / 1 MiB deflate workspace ceilings, explicit finite per-request
   input/output/work limits, and 24-hour-per-target-per-architecture campaigns;
   return with corpus-derived PNG metadata/default budgets before integration.
   B — owner supplies different ceilings/campaign duration before coding.

Five A answers remain accepted; choice 2 is superseded by the owned-PNG
rewrite direction. Phase 2 now contains a bounded streaming inflate derived
from the pinned upstream puff canonical-Huffman algorithm, with new ownership,
quota and streaming state. [Provenance](../../../src/core/compression/PROVENANCE.md)
and [phase-2 evidence](../../../results/2026-10-19-zlib/phase-two/README.md)
record the source decision and open gate. House style is mandatory; it is not
a substitute for the accepted executable verification.

## 12. Direct source references for review

These links supplement the repository-relative pointers and exact pins above:

- [Skia's zlib GN target](https://skia.googlesource.com/skia/+/2a9b593bab4b2fd019fa494c8d401ff1fab0b883/third_party/zlib/BUILD.gn)
  and [libpng GN target](https://skia.googlesource.com/skia/+/2a9b593bab4b2fd019fa494c8d401ff1fab0b883/third_party/libpng/BUILD.gn).
- [libpng inflate claims and processing](https://skia.googlesource.com/third_party/libpng/+/d5515b5b8be3901aac04e5bd8bd5c89f287bcd33/pngrutil.c),
  [deflate claims and processing](https://skia.googlesource.com/third_party/libpng/+/d5515b5b8be3901aac04e5bd8bd5c89f287bcd33/pngwutil.c),
  and [prebuilt feature configuration](https://skia.googlesource.com/third_party/libpng/+/d5515b5b8be3901aac04e5bd8bd5c89f287bcd33/scripts/pnglibconf.h.prebuilt).
- [Chromium match hash](https://chromium.googlesource.com/chromium/src/third_party/zlib/+/646b7f569718921d7d4b5b8e22572ff6c76f2596/contrib/optimizations/insert_string.h)
  and [chunk-copy contract](https://chromium.googlesource.com/chromium/src/third_party/zlib/+/646b7f569718921d7d4b5b8e22572ff6c76f2596/contrib/optimizations/chunkcopy.h).
- [Upstream Huffman table validation](https://github.com/madler/zlib/blob/51b7f2abdade71cd9bb0e7a373ef2610ec6f9daf/inftrees.c),
  [inflate state machine](https://github.com/madler/zlib/blob/51b7f2abdade71cd9bb0e7a373ef2610ec6f9daf/inflate.c),
  and [C API contract](https://github.com/madler/zlib/blob/51b7f2abdade71cd9bb0e7a373ef2610ec6f9daf/zlib.h).

## 13. Owner-requested early optimization

The owner subsequently requested optimal decoders, BFFT-style compiler
visibility, reduced nested branching, useful vectorization and appropriate
hinting. This authorizes measured inflate/Adler acceleration within the current
phase-2 PR. It does not turn a synthetic benchmark into the full acceptance
matrix. The [kernel inspection and measurements](../../../results/2026-10-19-zlib/phase-two/optimization/README.md)
record bounded copy/literal kernels, canonical root tables, compiler-vectorized
Adler, and the PNG filter reference inspection for the later owned parser.
