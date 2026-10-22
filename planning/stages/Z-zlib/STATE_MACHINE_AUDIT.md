# State-machine and Power-of-Ten audit of the input-reading code

Date: 2026-10-21. Branch `claude/z-s-safety` (stage Z-S). This audit is the
owner's revision of decision 6A (2026-10-20, recorded in [README §11](README.md)).
The decision replaces sustained fuzz campaigns with three things: a
safe-by-construction design, an audit of every input-reading function, and
deterministic evidence.

## Rules applied

Each rule is a JPL "Power of Ten" rule restated for this code, plus the house
style's two 2026-10-20 additions:

| Rule | Meaning here |
|---|---|
| R1 bounded loops | Every loop has a bound computed before it, or a monotone quota that each iteration charges. |
| R2 no allocation after initialization | The codec cores never allocate. Callers allocate before processing. |
| R3 no recursion | |
| R4 every return checked | Every `bool` or result is tested at each call site. |
| R5 every parameter validated | Enums, limits, spans (sizes, overlap) and records are checked before mutation. |
| R6 impossible states | House style §1 (`PROGRAMMING_HOUSE_STYLE.md` line 11): prefer a shape that cannot hold contradictory values to a check that rejects them. House style §9 (line 133): a switch or state machine sends every unexpected case to one defined fallback, without compensating checks along the way. |

Results are **pass**, **fixed** (changed in this PR), or **finding** (recorded,
with the remedy and the reason it is not done here).

## stx_zlib inflate (`stx_zlib/src/inflate.cpp`)

| Function | R1 | R2 | R3 | R4 | R5 | R6 |
|---|---|---|---|---|---|---|
| `reset` | n/a | pass | pass | n/a | pass: format, window 8–15, finite limits; an invalid reset keeps the old stream | pass |
| `process` | pass: every `step` costs at least one work unit from a finite quota | pass | pass | pass | pass: input/output/state overlap; idle → `invalid_state` | pass: `Phase` is closed; `failed` and `done` are absorbing |
| `step` | pass | pass | pass | pass | n/a | pass: the switch's fall-out goes to the single `fail(invalid_state)` |
| `bits` | pass: at most `count` ≤ 16 iterations | pass | pass | pass | finding (a) | pass |
| `symbol` | pass: at most 15 code lengths | pass | pass | pass | pass: peeks only live, budget-admitted bytes | pass: an exhausted code is `invalid_huffman_code` |
| `build_tree` | pass: fixed 15 + 2·n | pass | pass | pass | pass: Kraft over- and under-subscription refused | pass |
| `literal_run`, `stored_bytes`, `copy_match` | pass: capacity proved against every quota and span before the loop | pass | pass | pass | pass | pass |
| dictionary / trailer phases | pass | pass | pass | pass | pass | **fixed (b)** |

- **(a) Finding.** `bits(count)` requires `count ≤ 16`. Every call passes a
  constant or a table value (at most 13), so the precondition holds, but no
  type expresses it.
  - Remedy: a `BitCount` wrapper constructed only from those constants and
    tables.
  - Not done here because it touches every call in the hot path. It needs
    paired performance measurements, which the owner reserves for
    deliberate optimization PRs.
- **(b) Fixed.** The dictionary phase counted its four identity bytes in
  `trailer_bytes_`, so one field had two meanings. It now has its own
  `dictionary_bytes_`.
- **(c) Finding (§1).** Per-phase fields share one flat state:
  `stored_left_`, `match_length_`, `match_distance_`, `extra_bits_`,
  `repeat_symbol_`, `literal_byte_` and `length_index_`. A value that belongs
  to an inactive phase is representable. Every phase assigns its fields on
  entry before reading them, so no such value is ever read, but the shape
  does not rule it out.
  - Remedy: a `std::variant` of phase records, so each phase carries only its
    own fields.
  - That is a restructure of the resumable decoder; it needs the measurement
    gate above and owner review. Recorded for the owner.

## stx_zlib deflate (`stx_zlib/src/deflate.cpp`)

The input-handling paths are `reset` and `process`; the encoder reads only
caller bytes it has copied.

| Function | R1 | R2 | R3 | R4 | R5 | R6 |
|---|---|---|---|---|---|---|
| `reset` | n/a | pass | pass | n/a | pass: format, strategy, level −1..9, window 8–15, memory level 1–9, finite limits | pass |
| `process` | pass: each iteration moves input or output, or breaks; work quota | pass | pass | pass | pass: flush enum, three-way overlap, idle → `invalid_state` | pass: `Phase` closed; failure is absorbing |
| `build_tree` length limiting | pass: `used` falls by one per repair | pass | pass | pass | n/a | pass: an impossible repair is `workspace_overflow` |
| `tokenize`, `find_match`, `search_match` | pass: chain attempts and match length bounded by level tables and quota | pass | pass | pass | n/a | pass |

No finding beyond those listed for inflate.

## PNG decoder (`src/core/image/png_decoder.cpp`)

| Function | R1 | R2 | R3 | R4 | R5 | R6 |
|---|---|---|---|---|---|---|
| `parse` | pass: chunk count limit; offset strictly increases | pass | pass | pass | pass: limits finite, lengths checked against the span, CRC before interpretation | **fixed (d)** |
| `make_layout` | pass: at most 7 passes | pass | pass | pass | pass: depth/colour table; dimension, pixel and filtered-byte limits with overflow guards | pass |
| `keyword_end`, `valid_png_text`, `TextScan` | pass: at most 80 bytes; one pass | pass | pass | pass | pass | pass: incomplete UTF-8 at the end is refused |
| `read_image_bytes` | pass: progress required each call, or a named error | pass | pass | pass | pass | pass |
| `unfilter*`, `scatter` | pass: row length | pass | pass | n/a | pass: palette index checked | pass: an unknown filter is `invalid_filter` |
| `decode_metadata`, `measure_metadata` | pass: chunk walk plus inflate quotas | pass | pass | pass | pass: shared work and metadata quotas | pass |
| `decode_png_samples`, `decode_png_samples_discarding_metadata` | pass | pass | pass | pass | pass: six-way disjoint storage, capacity before writes, nonempty scratch | pass |
| `list_metadata` (new) | pass | pass | pass | n/a | precondition: a successful parse | pass: the one unreachable case keeps the record uncompressed and is commented as the fallback |

- **(d) Fixed (§1).** The colour-chunk order flags were the bare numbers 1,
  2, 4 and 8. They are now named (`seen_srgb`, `seen_gamma`,
  `seen_chromaticities`, `seen_icc`).

## Pixel and colour path (`png_pixels.cpp`, `png_color.cpp`, `clipboard_png.cpp`)

| Function | R1 | R2 | R3 | R4 | R5 | R6 |
|---|---|---|---|---|---|---|
| `PngPixelDecoder::decode` | pass | pass: storage is sized once per request, before decoding; `bad_alloc` becomes `allocation_failed` at this boundary | pass | pass | pass: target enum, destination shape, encoded/destination overlap | pass |
| `convert_rgba16_samples` | pass | pass | pass | n/a | pass: shape, capacity, overlap; kernel chosen before the loop | pass |
| `icc_profile_rule` | n/a | pass | pass | n/a | n/a | pass: an out-of-range purpose takes the stricter rule (§9 fallback) |
| `resolve_png_color` | pass | pass | pass | pass | **fixed (e)** | pass: cICP/iCCP/sRGB/gAMA/cHRM precedence; degenerate values ignored and reported |
| `convert_png_samples_to_srgb` | pass: 256 or 65536 table entries, then one pass | pass: tables sized before the pass | pass | pass | pass: any sample indexes the table in range | pass |
| `decode_clipboard_png` | pass | pass: one destination allocation before decode | pass | pass | pass: the HostImage pixel bound | pass |

- **(e) Fixed (R5).** `resolve_png_color` trusted that its records came from
  a decode of the same bytes. A mismatched record could have read outside
  the span. Records are now checked to lie inside it, and gAMA/cHRM records
  to have their fixed lengths. A misfit is `invalid_color_metadata`. The
  PLTE/IDAT walk stops at an envelope that leaves the span.
  `png_color_tests` covers both cases.

## PNG encoder (`src/core/image/png_writer.cpp`)

| Function | R1 | R2 | R3 | R4 | R5 | R6 |
|---|---|---|---|---|---|---|
| `encode_png` | pass | finding (f) | pass | pass | pass: order, alpha mode, level, dimensions, stride by division, alias | pass |

- **(f) Finding.** The compressed output grows in 64 KiB steps. The working
  rows and the deflate stream are allocated before the row loop. The output
  size is unknown in advance, and preallocating the worst case (the stored
  bound) would cost as much as the raw image. Growth is per 64 KiB of
  output, never per element, which house style §5 admits for growable
  collections. It is recorded as a deliberate deviation from R2.

## Other readers

| Function | Result |
|---|---|
| `validate_png` (registry) | R1 pass (chunk limit, increasing offset); R5 pass (lengths, CRC, order, sizes with overflow guards); R6 pass (every malformed case is a named `ImageResourceError`). cICP added (I6). |
| `decode_clipboard_image` (private raw format) | R5 pass: magic, version, pixel bound in 64 bits, exact or padded size, then `validate_host_image`. Allocation happens once, at the copy. |

## Evidence added with this audit
- **`gui_forms_malformed_sweep_tests` (zlib/deflate).** Eight seeds from the
  owned encoder, covering zlib and raw, levels 0, 1, 6 and 9, and the fixed,
  Huffman-only and RLE strategies, plus an empty stream. The test checks
  every truncation point, every single-bit flip of each seed's header and
  table region, and one-byte schedules: 11,141 decodes.
- **`gui_forms_png_malformed_sweep_tests`.** Eight seeds: every colour type
  family, Adam7, palettes with tRNS, text, compressed text, colour chunks
  and iCCP. The test checks every truncation point, and every single-bit
  flip of the signature, chunk headers, CRCs and chunk data (the first 64
  IDAT bytes), both as flipped and with the CRC repaired. Each file goes
  through inspection, both decode entries, measurement, the pixel decoder
  and the registry validator: 183,358 decodes.
- **`gui_forms_deflate_fuzz_tests`.** The deflate contract harness's fixed
  inputs: whole versus split encoding gives identical bytes, and inflate
  restores the input.

All three run in the normal suite, under ASan and UBSan in the sanitizer
tree and the Linux CI job. `.github/workflows/fuzz-smoke.yml` runs each
libFuzzer target for 150 s per change on x64 and arm64: inflate, inflate
against the pinned zlib (ordinary and strict), the inflate protocol harness,
deflate, and PNG decode. That run is a build check, not a certificate.
