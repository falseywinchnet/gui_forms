# CNG standalone format (CSP v2)

**Revision:** 2026-10-22 (GUI.Forms intake) · **Schema:** CSP v2 · **Derived
from:** the 0.1.0-rc1 format reference of `falseywinchnet/stx_cng`

This is the byte format `stx_cng` reads and writes. CNG means **Computational
Network Graphics**. The standalone signature `\x89CSP` is kept from the format's
development (owner decision D1, 2026-10-22). Collections (`\x89CNG`) are not
part of this package yet; they follow in a later change with a caller-supplied
output budget (decision D3).

The format is lossless for interleaved RGB8 and RGBA8 samples. It carries no
colour-profile metadata and implies no colour-space conversion. The coding
transform is reversible integer arithmetic on sample bytes.

Two changes from rc1, both in section 6: the admitted extent (sides up to
16,384, at most 2^26 pixels, decision D2) and strict decoding (one frame shape
per field, decision "strict decoding" accepted with D1 to D5).

## 1. Conventions and framing

Integers are unsigned and big-endian unless stated otherwise. A chunk is:

| Field | Bytes | Meaning |
| --- | ---: | --- |
| Length | 4 | Payload length, excluding type and checksum |
| Type | 4 | ASCII chunk type |
| Payload | Length | Chunk-specific data |
| CRC32 | 4 | CRC-32 (ISO-HDLC, the zlib and PNG polynomial) of type then payload |

The length field is not covered by the checksum. The checksum detects accidental
corruption; it is not authentication. Chunk order is fixed: a reader refuses
an unexpected chunk and any byte after the end chunk. The PNG-like framing
implies neither PNG ancillary-chunk semantics nor PNG compatibility.

## 2. File

The eight-byte signature is `89 43 53 50 0D 0A 1A 0A`. It is followed by one
`CHDR`, exactly as many `BAND` chunks as the header implies, and an empty
`CEND`, which is the last byte of the input.

### CHDR: image header

The payload is exactly 16 bytes.

| Offset | Bytes | Value |
| --- | ---: | --- |
| 0 | 1 | Version: 2 |
| 1 | 1 | Channels: 3 for RGB, 4 for RGBA |
| 2 | 1 | Flags: 0 |
| 3 | 1 | Reserved: 0 |
| 4 | 4 | Width `W` |
| 8 | 4 | Height `H` |
| 12 | 4 | Band budget `B`: 1,048,576 |

Admission: `1 <= W <= 16384`, `1 <= H <= 16384` and `W * H <= 2^26`
(67,108,864 pixels). Any other value of any field is refused.

For channels `C`, the number of rows in each band but the last is:

```text
rows = floor((B - C) / (W * C))
```

which is at least 15 for every admitted width. Bands follow raster order; the
last band holds the remaining rows. Bands carry no dimensions of their own: the
header determines each band's extent, and so the number of bands. Copy history
resets at each band. Gradient prediction uses the previous original row; above
the image's first row that row is all zero.

### BAND: instructions and literals

| Offset | Bytes | Meaning |
| --- | ---: | --- |
| 0 | 4 | Sequence count `N` |
| 4 | 4 | Literal pixel count `L` |
| 8 | 4 | Sequence-frame length `S` |
| 12 | S | Zstandard sequence frame |
| 12 + S | 1 | Literal coding: 0 stored, 1 compressed |
| 13 + S | rest | Literal payload |

For a band of `P` pixels, `1 <= N <= floor(P/4) + 1` and `L <= P`.

The sequence frame decompresses to exactly `12 * N` bytes: `N` records of
three unsigned 32-bit fields, literal count, run length and distance, all in
**pixels**. The raw frame is byte-planar: for record `i`, field `f` and
little-endian byte position `b`, the byte is at `(4*f + b)*N + i`. This is the
one exception to the big-endian convention.

Each record emits its literals, then its run:

- Positive length and positive distance copy earlier pixels of the current
  band. The source may overlap the destination; output repeats the
  reconstructed prefix.
- Positive length and distance zero emit exact gradient predictions.
- The last record has length zero and distance zero and may carry trailing
  literals. No earlier record has length zero.

Every run but the last has at least four pixels. A copy distance is at most
262,144 pixels and at most the number of pixels already produced in the band.
The records' literal counts sum to `L`, and literals plus runs cover the band
exactly.

### Gradient prediction

Per channel, prediction is `(left + above - upper_left) mod 256`. At column
zero, `left` and `upper_left` are zero. Above the first image row, `above` is
zero; later bands use the previous band's last row.

Compressed literals encode `(sample - prediction) mod 256`. Prediction runs
have zero residual. Stored literals hold **original samples** and bypass
residual inversion. Copy runs always copy original samples.

### Stored literals: coding 0

The payload is exactly `L * C` original sample bytes, interleaved by pixel, in
consumption order. A band with no literals uses this coding with an empty
payload.

### Compressed literals: coding 1

The payload is one Zstandard frame. Decompressed, it is the reverse-tail run
separation (below) of `2*C*ceil(L/2)` packed bytes, which hold paired nibbles
of zigzag-mapped colour residuals.

For each literal's three colour residuals, read each modular byte as a signed
value in `[-128, 127]`. Let `wrap(v) = ((v + 128) mod 256) - 128` with a
nonnegative remainder, and use floor division throughout:

```text
difference = wrap(red - green)
second     = wrap(blue - green - floor(difference / 2))
centre     = wrap(green + floor(difference / 2))
third      = floor((second + (difference mod 2) + 1) / 3)
brightness = wrap(centre + third)
```

The coded coordinates are `brightness, difference, second`; alpha, when
present, keeps its signed residual. The inverse:

```text
third  = floor((second + (difference mod 2) + 1) / 3)
centre = wrap(brightness - third)
green  = wrap(centre - floor(difference / 2))
red    = wrap(green + difference)
blue   = wrap(green + second + floor(difference / 2))
```

Each signed coordinate `v` maps to `2*v` when nonnegative, else `-2*v - 1`.
With `G = ceil(L/2)`, channel `c` has its low-nibble plane at offset `2*c*G`
and its high-nibble plane at `(2*c + 1)*G`. Literal `i` uses byte `floor(i/2)`
of each plane, shifted by `4*(i mod 2)`: even literals take low nibbles, odd
literals high nibbles. For odd `L`, the unused high nibbles of the last byte
of every plane are zero.

### Reverse-tail run separation

The decompressed frame holds un-repeated bytes at the front and descriptors
read backward from the end. A descriptor integer is read right to left, seven
payload bits per byte, least significant group first; bit 7 asks for another
byte. A descriptor has at most five groups and no redundant zero final group.

For descriptor value `code`, `length = floor(code/2) + 1`:

- Even `code`: copy `length` bytes from the front cursor.
- Odd `code`: read one more byte backward as the repeated value and emit it
  `length` times. Repeated runs have length at least four.

Decoding stops at the exact packed-byte count, where the front and back
cursors must meet. The frame is at most twice the packed size plus four bytes.

### CEND: end

`CEND` has an empty payload and is followed by end of input.

## 3. Strict decoding

A file that rc1 wrote is accepted, unless its extent is outside section 2.
The decoder also refuses encodings rc1 never writes but its reader accepted:

- a sequence or literal payload that is not exactly one complete,
  non-skippable Zstandard frame whose header declares its content size (for
  sequences, exactly `12 * N`; for literals, at most `2 * packed + 4`);
- compressed literals (coding 1) when `L` is zero, or when the payload is not
  smaller than the `L * C` bytes of stored samples;
- a nonzero padding nibble.

This makes every field's framing canonical. It does not make the whole file
canonical: the encoder's match choices and the bits inside each Zstandard
frame remain free, so two encoders can write different valid files for one
image. A decoder implements the grammar; it does not depend on one encoder's
choices. `stx_cng`'s encoder is deterministic (the same samples give the same
bytes on every host for one Zstandard version and this package's parameters:
compression level 3, no checksum, content size recorded).

## 4. Compatibility

- Extent: rc1 admitted widths and heights up to 65,536 with a padded-area rule
  (both sides rounded up to multiples of 128, product at most 4,194,304). This
  revision admits every rc1 extent with both sides at most 16,384 and refuses
  the rest: rc1 files wider or taller than 16,384 pixels (such as 20,000 x 100)
  are not readable here. It admits many more large images (all 4K and
  2736 x 1539 captures, which rc1 refused).
- Unknown versions, flags, channel counts, budgets and chunks are refused.
  Wider extents, metadata, previews and new sample types need deliberate format
  evolution under a new version.
