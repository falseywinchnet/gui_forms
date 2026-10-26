# Hardening record — 2026-10-26

Intake of the owner's CNG 1.0 codec (`falseywinchnet/stx_cng` at
`9fde861451367796fa11da8686e47d164b6c8042`, `PROVENANCE.md`) into GUI.Forms,
to the standard `stx_zlib`, `stx_png`, `stx_conv` and the 2026-10-22 intake of
CNG 0.1 were held to: a design safe by construction, an audit of every
function that reads caller input, deterministic sweeps, sanitizer runs, and a
short libFuzzer smoke per change on CI only. There is no sustained fuzz
campaign (owner decision, 2026-10-20).

CNG 1.0 replaces the 0.1 format (`\x89CSP` v2, banded LZ77 with Zstandard
literals) with a new one: 128 x 32 segments coded by a context-modelled range
coder, a 16 x 16-cell mosaic, one Zstandard frame of frequency tables, and a
second copy of every redundant part (`FORMAT.md`). The 0.1 record's audit
does not carry over; this one replaces it. The owner decisions of 2026-10-22
still in force are D3 (collections through caller-owned spans and a budget)
and D4 (reusable owners); D1 (the 0.1 signature) and D2 (the 0.1 extent) are
superseded by the 1.0 format, which fixes its own signature and extent.

## Rules applied

| Rule | Meaning here |
|---|---|
| R1 bounded loops | Every loop's bound is computed before it from validated counts: segments from the header, stream lengths from the payload, pixels from the tile, table entries from the bounded tables body, collection members from the checked header. The range decoder's renormalisation stops at the end of its stream. |
| R2 no allocation once grown | An owner grows to the largest segment, mosaic and table storage it has needed; a call that fits allocates nothing. Zstandard contexts are static (`ZSTD_initStaticCCtx`, `ZSTD_initStaticDCtx`) in owner storage. Worker threads are started per call and joined before it returns; their storage is the owner's. |
| R3 no recursion | |
| R4 every result checked | Every `CngError`, `bool` and Zstandard return is tested where it is produced; every worker's result is collected after the join. |
| R5 every parameter validated | Extent, channels, thread count, stride, sample and destination extents and overlap are checked before any write; every length, checksum, count, code and distance read from input is checked before the data it governs is used. |
| R6 impossible states | The container is read into one validated `Container` before any payload is touched; a tile decoder that meets an invalid code, a distance outside the produced pixels, a length past the tile or an exhausted stream sets one failure flag and stops at the row's end. Every unexpected input reaches one named `CngError`. |

Results are **pass**, **fixed** (changed in this intake) or **finding**
(recorded with its remedy).

## Owners and their states

| Owner | States | Transitions |
|---|---|---|
| `CngDecoder` | empty, grown(tables, tiles, salvage) | Construction: empty (`noexcept`, no allocation). A call that needs more storage grows it (exact reservations); on allocation failure the storage keeps what it had and the call returns `allocation_failed`. `release()`: empty. Copy and move are deleted. |
| `CngEncoder` | empty, grown(tiles, streams, payloads, context) | As the decoder. The per-tile streams and payloads grow with the image's segment count (see Storage). |
| `CngCollectionEncoder`, `CngCollectionDecoder` | empty, grown | Each owns one image owner and, for the composite form, the canvas; transitions as above. |

The format is a straight-line grammar: the header fixes the segment count,
the directory every payload's length, the payload its streams' lengths, and
the tile its pixel count, so there is no hidden parser state machine. Each
call's response:

| Operation | Signature or fields | Directory, tables | Wrong storage | Overlap | Bad payload | Damaged copy |
|---|---|---|---|---|---|---|
| `inspect_cng` | `invalid_signature` (empty input included); `invalid_header` (short prefix, no intact field block, shape outside the extent); `unsupported_version` | `invalid_directory`, `invalid_tables` | — | — | not examined | the other copy is used |
| `validate_cng_framing` | `invalid_signature`; `truncated`; `crc_mismatch` (front block); as `inspect_cng` for the fields | `crc_mismatch` (front directory, tables or mosaic); `invalid_tables` (not one Zstandard frame) | — | — | `crc_mismatch` (framing only; not decoded) | `truncated`, `trailing_data`, `copy_mismatch` |
| `decode` | as `inspect_cng`; samples untouched | as `inspect_cng`, and `invalid_tables` for a body outside the format; untouched | `insufficient_storage`, `invalid_argument` (stride, threads); untouched | `overlap`; untouched | `invalid_segment`; other segments written, the image to be discarded | the other copy is used |
| `decode_mosaic` | as `decode` | as `decode` | as `decode` | as `decode` | `invalid_segment` | `mosaic_unavailable` when both copies fail |
| `decode_salvage` | as `decode` | as `decode` | as `decode`, and a short `segment_intact` | as `decode` | the segment is filled from the mosaic (or 128) and reported lost | the other copy is used |
| `encode` | — | — | shape, threads, stride or short samples: `invalid_argument`; destination below what this file needs: `insufficient_storage` with `required_bytes` | `overlap` | — | — |

Refusals before storage is touched leave the owner unchanged. A payload
refusal leaves the owner grown and reusable (`tests/malformed.cpp` reuses one
decoder across every damaged and crafted file).

## Audit

| Function | R1 | R2 | R5 | R6 | Result |
|---|---|---|---|---|---|
| `admitted_shape`, `cng_sample_bytes`, `check_extent` | — | — | sides at least 1, at most 2^31 samples, channels 3 or 4; products in 64 bits; `(height - 1) * row_bytes` formed only after a division shows it fits | — | pass |
| `cng_encoded_bound` | — | — | every term in `uint64_t`, zero above `SIZE_MAX` | — | pass |
| `parse_fields` | — | — | version 1, segment 128 x 32, shape admitted; lengths are 32-bit | — | pass |
| `read_container` | segments from the fields | none | length at least the minimum; front field block, else the back one; file at least the fixed bytes the fields imply; a directory copy whose CRC holds; a tables copy whose CRC holds; the payload lengths summing exactly to the payload region (sums in 64 bits of at most 2^26 entries of 32 bits) | every later read is within `[payload_begin, payload_end)` | pass |
| `validate_framing` | segments | none | front copies only; implied length exact; every CRC; every second copy byte-identical; the tables part exactly one frame | the image is reported only when the whole file passes | pass |
| `single_frame`, `read_tables` | contexts, then entries | static DCtx in owner storage | one non-skippable frame spanning the part; decompressed size at most the format's tables body; per context a count of at most `green_symbols` entries, each q at most 25; the body consumed exactly | an all-zero context is marked unused; a symbol read from it is a stream failure | **fixed** (upstream accepted trailing frames and skippable frames; `FORMAT.md` 1.0 requires exactly one) |
| `build_table` / `normalize` | entries | owner workspace | (validated q values) | normalised to exactly 4,096; the difference repaid from entries above 1 in a fixed order | pass |
| `split_streams`, `read_leb128` | channels - 1 lengths | none | at most five LEB128 bytes each; listed lengths at most what remains | the last stream takes the rest | pass |
| `start_range`, `renormalize`, `decode_symbol`, `decode_bits` | bytes of the stream | none | a stream under four bytes fails; renormalisation stops at the stream's end; raw bit counts at most 12 | failure is sticky and checked at every row's end and at the tile's end | pass |
| `TileState::match` | — | none | length code and distance code below 30; distance at least 1 and at most the pixels produced; length within the tile | a failed match writes nothing further | pass |
| `decode_row`, `decode_tile` | pixels of the tile | caller's workspace, grown by `reserve_tile` | (validated container and tables) | rows are written in order; matches and the activity read only pixels already written | pass |
| `run_segments`, `CngDecoder::decode` | segments, split into contiguous ranges | grows once per worker | threads 1 to 64; destination extent and overlap before any write; each payload's CRC before it is decoded | workers write disjoint rectangles; the first failure in segment order is reported | **fixed** (byte spans, caller row stride, owners and threads replace upstream's per-call vectors) |
| `decode_mosaic`, `decode_salvage` | mosaic cells; segments | as `decode` | as `decode`; `segment_intact` at least the segment count | salvage writes every pixel: intact segments exactly, the rest from the mosaic or 128 | **fixed** (salvage reports intact segments explicitly) |
| `CngEncoder::encode` and its passes | tiles; ops per stream bounded by three per sample | grows per image | shape, threads, stride, sample extent, overlap; destination checked against the exact size before the first byte is written | output identical at every thread count; workers' counts summed in job order | **fixed** (allocation failure is a result, not an exception) |
| `inspect_cng_collection` | members (at most 4,096) | none | signature; header chunk and its CRC; version, form, count, canvas; directory chunk; each member within the bounds, ids unique, chunks contiguous and inside the file, composite members stacked without gaps and sharing one chunk; the end chunk last; every image chunk's CRC and its file's inspected shape equal to the entry's | `entries` filled only up to the first refusal and meaningful only on success | **fixed** (D3: spans, caller entries; upstream used the filesystem) |
| `cng_collection_bound`, `CngCollectionEncoder` | members | grows per collection | member checks first; the bound in 64 bits, zero above `SIZE_MAX`; the budget's remaining room checked before each chunk, so nothing is written outside it | the canvas is the encoder's own storage; on failure the budget's contents are unspecified | **fixed** (D3 budget) |
| `copy_cng_collection_member` | rows of the member | none | canvas extent; the entry inside the canvas; destination extent and overlap | a BGR member of a BGRA canvas drops the added alpha | pass |
| Owners' copy and move | — | — | — | deleted | pass |

## Storage

Measured by `results/2026-10-26-cng-1.0-intake/storage_probe.cpp` on a
1,024 x 256 image (64 full segments), owners fresh for each call:

| Owner | Image | Bytes |
|---|---|---:|
| Decoder, 1 thread | any | 552,488 |
| Decoder, 4 threads | any | 614,144 |
| Decoder, 64 threads | any | 1,847,264 |
| Encoder | BGR noise | 6,059,696 (7.71 per sample) |
| Encoder | BGRA noise | 7,115,870 (6.79 per sample) |
| Encoder | BGR flat | 1,841,824 (2.34 per sample) |

Over the intake corpus the decoder stayed at 614,144 bytes at four threads
and the encoder reached 209,770,188 bytes for a 3,840 x 2,400 BGR image (7.6
bytes per sample). The decoder's fixed part is its tables (38 contexts of
4,096 slots) and the static Zstandard context; each thread adds one segment's
residuals, energy rows and column sums.

The decoder's storage is fixed by the segment size and the thread count, not
the image: `tests/limits.cpp` requires the storage after a 128 x 32 file and
after a 1,536 x 1,000 file to be equal at one thread. Its Zstandard context is
static and bounded by the tables part's size. The encoder keeps every tile's
operations (at most three four-byte codes per sample) and payloads until the
container is written, so its storage grows with the image (finding F3 below).

## Sweeps

`tests/malformed.cpp`, on five files (1 x 1 BGR noise and 13 x 9 BGRA alpha
steps exhaustively; 129 x 33 BGR tiles, 140 x 70 BGRA patches and 300 x 64
BGR noise at about 400 sampled offsets), through `validate_cng_framing`,
`inspect_cng`, `decode` at one and four threads, `decode_mosaic` and
`decode_salvage`:

- every prefix and every single-bit flip; 60 random bursts of up to 600 bytes;
- 300 crafted files per case with every checksum repaired: random payloads,
  scrambled streams under valid stream lengths, random table bodies in valid
  Zstandard frames (truncated, oversized and zero contexts), random mosaics,
  and random dimensions with matching directories;
- every q value from 26 to 255, two frames, and a skippable frame.

A decode that reports success must reproduce the original samples, and a
salvage must reproduce every segment it reports intact. `tests/core.cpp`
damages each first and each second copy separately and checks that decoding
uses the other one and that strict framing refuses the file
(`crc_mismatch`, `copy_mismatch`); `tests/collection.cpp` sweeps 400 damaged
files per collection form. All fixtures are built in memory; no test writes a
file.

## Sanitizer and fuzzing

- `-DSTX_CNG_SANITIZE=ON`: AddressSanitizer and UndefinedBehaviorSanitizer
  with `-fno-sanitize-recover=undefined` over the library, Zstandard and every
  test; GUI.Forms' sanitizer tree (`tools/ci_build.py --sanitize`) runs the
  same tests and `resource_pixels`. Windows (pinned LLVM 22, MinGW): all pass;
  leak detection is not available there and is covered on Linux CI.
- `fuzz/cng_fuzz.cpp`: the first byte selects one of five modes: an arbitrary
  file through every reading call (salvage must agree with a successful
  decode, and a decoded image of at most 1 MiB must re-encode and decode
  exactly); a generated image's round trip with stride padding at a chosen
  thread count; a crafted file with valid checksums over the fuzzer's
  payloads and mosaic; an arbitrary collection; and a generated collection's
  round trip. Built standalone it replays 300 generated inputs. CI runs the
  libFuzzer build for 150 s per change on Linux x64 and arm64.

The sanitizer runs found one defect in this intake's own fuzz replay (the
standalone input generator read past a one-byte input; fixed) and none in the
library.

## Findings and their disposition

1. F1, the 0.1 record's findings. **Closed** by replacement: the 0.1 format,
   its sources and its tests are gone. Its fixes that still apply (no
   exceptions at the interface, byte spans, reusable owners, no filesystem
   access, no timing in codec paths, no fixtures written to disk) are kept in
   the 1.0 port.
2. F2, upstream's tables reader accepted bytes after the first Zstandard
   frame, and skippable frames. **Fixed**: exactly one frame (`FORMAT.md`
   1.0, section 5), in decoding and in strict framing.
3. F3, the encoder's storage grows with the image. **Recorded**: at most
   three four-byte operations per sample for the streams plus the payloads,
   all kept until the container is written. A streaming container writer
   would bound it by the segment and thread count, as the decoder is; not
   done, since GUI.Forms encodes only images it already holds in memory.
4. F4, the decoder's failure contract. **Recorded**: a refusal while decoding
   segments leaves the other segments written; the caller discards the image
   (README, GUI.Forms' `cng_pixels`). Upstream left it unstated.

Open:

- **Zstandard symbols are global.** A consumer that links another Zstandard
  gets duplicate symbols (`PROVENANCE.md`). Prefixing them would need a
  modified Zstandard and is not done.
- **Decoded pixels are not authenticated.** CRC-32 per part detects
  accidental damage only; a checksum-repaired edit decodes to other pixels by
  design (as in PNG).
