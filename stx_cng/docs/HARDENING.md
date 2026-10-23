# Hardening record — 2026-10-22

Intake of the owner's CNG codec (`falseywinchnet/stx_cng` 0.1.0-rc1) into
GUI.Forms, to the standard `stx_zlib`, `stx_png` and `stx_conv` were taken in
at: a design safe by construction, an audit of every function that reads
caller input, deterministic sweeps, sanitizer runs, and a short libFuzzer
smoke per change on CI only. There is no sustained fuzz campaign (owner
decision, 2026-10-20). Owner decisions D1 to D5 of 2026-10-22 set the scope:
standalone images only, the D2 extent, strict decoding, reusable owners.

## Rules applied

| Rule | Meaning here |
|---|---|
| R1 bounded loops | Every loop's bound is computed before it from validated counts: bands from the header, records from `N`, literals from `L`, descriptors from the packed size. |
| R2 no allocation once grown | An owner grows to the largest band it has seen; a call that fits allocates nothing. Zstandard contexts are static (`ZSTD_initStaticCCtx`, `ZSTD_initStaticDCtx`) inside owner storage. |
| R3 no recursion | |
| R4 every result checked | Every `CngError`, `bool` and Zstandard return is tested where it is produced. |
| R5 every parameter validated | Extent, channels, stride, sample and destination sizes, overlap, and every length, count and frame read from input are checked before the data they govern is written. |
| R6 impossible states | One validator owns the sequence grammar; restore kernels take only validated records. Every unexpected input in every state goes to one named `CngError`. |

Results are **pass**, **fixed** (changed in this intake) or **finding**
(recorded with its remedy).

## Owners and their states

| Owner | States | Transitions |
|---|---|---|
| `CngDecoder` | empty, grown(size) | Construction: empty (`noexcept`, no allocation). A decode whose largest band exceeds the storage grows it (exact reservations); on allocation failure the storage keeps its previous size and the call returns `allocation_failed`. `release()`: empty. Copy and move are deleted. |
| `CngEncoder` | empty, grown(size, strided) | As the decoder. The band copy for strided input is grown only when a strided input arrives. |

The format itself is a straight-line grammar: the header fixes the number of
bands, so there is no hidden parser state machine. Each call's response:

| Operation | Bad signature or header | Extent or channels | Wrong storage | Overlap | Bad band | After `CEND` |
|---|---|---|---|---|---|---|
| `inspect_cng` | `invalid_signature`, `truncated`, `invalid_chunk`, `crc_mismatch`, `invalid_header` | `unsupported_dimensions` | — | — | not examined | not examined |
| `decode` | as `inspect_cng`; samples untouched | as `inspect_cng` | `insufficient_storage` (size other than `cng_sample_bytes`); untouched | `overlap`; untouched | `invalid_band`, `entropy_failure`, `invalid_sequences`, `invalid_literals`, `truncated`, `invalid_chunk`, `crc_mismatch`; earlier bands written | `trailing_data` |
| `encode` | — | `unsupported_dimensions` | stride below a row or overflowing, short samples: `invalid_argument`; destination below the bound: `insufficient_storage` | `overlap` | — | — |

Refusals before storage is touched leave the owner unchanged. A band
refusal leaves the owner grown and reusable (`tests/malformed.cpp`,
"decoder reusable after refusals").

## Audit

| Function | R1 | R2 | R5 | R6 | Result |
|---|---|---|---|---|---|
| `admitted`, `cng_sample_bytes` | — | — | sides in [1, 16384], area at most 2^26, channels 3 or 4; products in 64 bits | — | **fixed** (D2; finding F5) |
| `cng_encoded_bound` | — | — | every term in `uint64_t`, refused above `SIZE_MAX` | — | pass |
| `band_shape` | — | — | admitted image only; rows >= 15 | — | pass |
| `read_chunk` | — | — | 12 framing bytes present; length within the remaining input (subtraction, no addition overflow); type; CRC | — | pass |
| `declared_frame` | — | — | non-skippable; one frame spanning the payload exactly; content size declared and within the limit | — | **fixed** (F6) |
| `inspect_cng` | — | — | signature, header length 16, version, channels, flags, reserved, budget, extent | — | **fixed** (F3: header probe) |
| `decode_band` | records `N`, literals `L` | none | `1 <= N <= P/4 + 1`, `L <= P`, `S` within the payload, sequence frame exactly `12N`; coding 0 payload exactly `L*C`; coding 1 needs `L > 0` and payload below `L*C` | all checks precede the band's first write | **fixed** (F6) |
| `valid_sequences` | `N` | none | literals within the band and within `L`; runs >= 4 and within the band; distance <= produced and <= 262,144; exact totals | the single grammar validator | **fixed** (F7: was three copies) |
| `restore_runs` | descriptors until the packed count | none | five groups at most, no redundant zero group, runs >= 4, cursors meet | — | pass |
| `unpack_literals` | `L` | none | padding nibbles of odd counts zero | — | **fixed** (F6) |
| `restore_band` / `restore<C>` | records, then pixels | none | (validated records only) | copies read only produced pixels; overlapping copies double from the prefix | **fixed** (F7: one kernel per channel count) |
| `CngDecoder::decode` | bands from the header | grows once | probe; storage size exact; overlap; `CEND` empty and last | previous row is the zero row or the output row above | **fixed** (F2, F3, F4) |
| `find_sequences` / `search<C>` | pixels of the band | none (index in owner) | (owned band) | bands under four pixels give one literal record | pass |
| `separate_runs` | packed bytes | none | (owned) | descriptors never overtake the literal region (proved in the source) | pass |
| `CngEncoder::encode` | bands | grows once | extent; stride at least a row and `stride * height` within `size_t`; sample span covers `stride*(H-1) + row`; destination at least the bound; overlap | compressed literals kept only when smaller than stored | **fixed** (F2, F4) |
| Owners' copy and move | — | — | — | deleted | **fixed** |

## Storage maxima

`tools/storage_bound.py` enumerates the owners' array reservations for every
admitted width and both channel counts, at the largest band height:

| Owner | Arrays at most | At | Context (Zstandard 1.5.7) |
|---|---:|---|---|
| Encoder, tight input | 6,601,073 B | width 15,887, RGB | level-3 CCtx, about 1.30 MB |
| Encoder, strided input | 7,649,615 B | width 15,887, RGB | as above |
| Decoder | 6,338,929 B | width 15,887, RGB | DCtx, about 96 KB |

`tests/limits.cpp` round-trips the maximal shape and checks the measured
`workspace_bytes()` against these array maxima plus context ceilings
(4 MiB + 8 for the CCtx, guarding another Zstandard build; 256 KiB for the
DCtx). Upstream's per-call contexts and arrays (finding F4) are gone, and the
decoder's context is no longer unbounded.

## Sweeps

`tests/malformed.cpp`, on four shapes (5 x 3 RGB, 31 x 17 RGBA, 300 x 1100
RGBA with five bands, 700 x 520 RGB with four) and three patterns (ramp,
noise, sparse noise: stored and compressed literals), at every byte offset,
or at 256 evenly spaced offsets for files above 4 KB:

- every proper prefix is refused;
- every byte flipped by 0x01, 0x80 and 0xFF is refused; with the chunk CRC
  then repaired, the decoder runs to a defined result without a checksum
  error (under the sanitizer, with no memory or UB finding);
- one trailing byte is refused as `trailing_data`;
- hand-built cases for each header field, each band count, each strict-frame
  rule (skippable, concatenated, unsized, garbage, missing and mismatched
  frames), each grammar rule, each literal-coding rule, the padding nibble and
  the chunk order.

All fixtures are built in memory; no test writes a file (finding F11).

## Sanitizer and fuzzing

- `-DSTX_CNG_SANITIZE=ON`: AddressSanitizer and UndefinedBehaviorSanitizer
  with `-fno-sanitize-recover=undefined` over the library, Zstandard and every
  test. Windows (pinned LLVM 22, MinGW): all four tests pass; leak detection
  is not available there and is covered on Linux CI.
- `fuzz/cng_fuzz.cpp`: the first byte selects decoding an arbitrary file (an
  accepted file must re-encode and decode exactly, and agree with the probe)
  or encoding a generated image with stride padding (exact round trip, and a
  truncated copy refused). One encoder and one decoder serve every input, so
  storage reuse across shapes is exercised. Built standalone it replays 400
  generated images with 24 single-bit edits each (half CRC-repaired) and
  requires both outcomes. CI runs the libFuzzer build for 150 s per change on
  Linux x64 and arm64.

## Findings from the intake review and their disposition

1. F1, collections have no output budget. **Recorded**: collections are not
   taken in; the follow-up adds a caller-supplied budget and byte-span members
   (D3).
2. F2, exceptions as the failure channel. **Fixed**: `noexcept` entry points
   and `CngError` results.
3. F3, iostream-only API with no header probe. **Fixed**: byte spans and
   `inspect_cng`.
4. F4, allocation per call. **Fixed**: reusable owners, static contexts (D4).
5. F5, padded 4,194,304-pixel extent. **Fixed**: D2 extent.
6. F6, non-canonical framing accepted. **Fixed**: strict decoding
   (`docs/FORMAT.md`, section 3). Whole files are still not canonical; the
   encoder's choices and Zstandard's bits remain free.
7. F7, triplicated validation and restore. **Fixed**.
8. F8, magic numbers and dead code. **Fixed**: `src/format.hpp` names every
   format constant.
9. F9, filesystem access in the collection encoder. **Recorded** with F1.
10. F10, invariant-free decoded aggregate. **Fixed**: decoding fills caller
    storage; no decoded aggregate remains.
11. F11, no malformed-input tests; fixtures left behind. **Fixed**.
12. F12, development signature. **Closed** by D1: `\x89CSP` v2 is kept.
13. F13, timing in codec paths. **Fixed**: removed; callers time calls.

Open:

- **Zstandard symbols are global.** A consumer that links another Zstandard
  gets duplicate symbols (`PROVENANCE.md`). Prefixing them would need a
  modified Zstandard and is not done.
- **Decoded pixels are not authenticated.** CRC-32 per chunk detects
  accidental damage only; a CRC-repaired edit decodes to other pixels by
  design (as in PNG).
