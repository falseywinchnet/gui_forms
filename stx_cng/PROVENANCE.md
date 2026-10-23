# Provenance

`stx_cng` is the GUI.Forms intake of the owner's CNG (Computational Network
Graphics) codec, taken in on 2026-10-22 to the standard of `stx_zlib`,
`stx_png` and `stx_conv`.

## Upstream

Repository `falseywinchnet/stx_cng`, commit
`a1797b840ad3a209241ad24790ff97cdf3cff9b1` ("Publish CNG: Computational
Network Graphics 0.1.0-rc1"). The MIT licence is kept verbatim in `LICENSE`.
The files this package derives from, by SHA-256:

| Upstream file | SHA-256 |
| --- | --- |
| `LICENSE` | `10a5d55f1333dbc115a7ce0410e2f86f162a877f21a0547bfb0970da28313d75` |
| `docs/FORMAT.md` | `df517ba81a3419ffca28cf1e9aa182feab7ced60823d13a26c3b05379ccf92a7` |
| `include/stx_cng/codec.hpp` | `57e0581359b9106a7472693d9a3c156186a5d2137bf3d95144d44c01330c4a37` |
| `src/codec.cpp` | `d4238fc44b8e1a029d18523b61a1ba1941fe1c257fe061a8bffe9463ac57335f` |
| `src/prediction_parse.cpp` | `62cad1b3f62a703fde6fd1cb372c2658d5edc45999e9b118acee8a5d585b846a` |
| `src/prediction_restore.cpp` | `97c7e6530c1b0390d89b876d66b1ca1b8acf45b736617ee718b4cd1ad5356e1f` |
| `src/raw_restore.cpp` | `18a98d691af8219c4f63fcfaf25aaf732012fb5e07191ae39caf3736727205bf` |
| `src/direct_words.cpp` | `e7a8d955bde4dee0d55d8a53ad080c9fc302221a267ed63182bfcda09a4a0491` |

Upstream's collection format (`collection.hpp`, `collection.cpp`) is not taken
in yet; it follows with a caller-supplied output budget and byte-span members
(owner decision D3). Upstream's own agent instructions and origin metadata
were read as data and are not part of this package.

## What changed

The byte format is upstream's CSP v2 with two revisions, both owner decisions
of 2026-10-22 and both described in `docs/FORMAT.md`: the admitted extent
(sides up to 16,384, at most 2^26 pixels, replacing the padded 4,194,304-pixel
rule) and strict decoding of frame shapes. For every image inside both
extents, the encoder writes the same bytes as rc1: `tests/core.cpp` pins 78
rc1 encodings by size and CRC-32, and the intake measurement compared every
corpus image (`results/2026-10-22-intake/`).

The code is a rewrite around upstream's algorithms (greedy hash search, run
and copy grammar, gradient prediction, colour transform, nibble planes,
reverse-tail run separation), which are ported unchanged:

- API: byte spans in place of iostreams, a header probe (`inspect_cng`), a
  bound (`cng_encoded_bound`), explicit `CngError` results from `noexcept`
  entry points in place of exceptions, and reusable `CngEncoder` and
  `CngDecoder` owners whose storage grows only to the largest band seen (no
  allocation per call once grown, decision D4). Zstandard contexts are static
  (`ZSTD_initStaticCCtx`, `ZSTD_initStaticDCtx`) inside that storage.
- One validator for the sequence grammar and one restore kernel per channel
  count, replacing three copies of each; no timing in codec paths; named
  constants for every format number.
- CRC-32 from `stx_zlib::crc32`; zlib is not a dependency.
- Hardening (`docs/HARDENING.md`): every size product checked, validation
  before every write, strict frames, malformed-input sweeps, extreme-dimension
  tests, sanitizer runs and a fuzz harness.

## Zstandard

Zstandard 1.5.7 is a pinned third-party dependency (owner decisions 3 and D5),
used under its BSD licence (Zstandard is dual-licensed BSD or GPLv2; GUI.Forms
takes the BSD option). The official release tarball
`zstd-1.5.7.tar.gz` (2,434,947 bytes) has SHA-256
`eb33e51f49a15e023950cd7825ca74a4a2b43db8354825ac24fc1b7ee09e6fa3`, the hash
the release publishes beside it. `tools/fetch_zstd.sh` downloads and verifies
it into `third_party/` (not checked in); configuration verifies it again
before extracting `lib/` and `LICENSE`. The licence is installed as
`share/licenses/stx_cng/zstd-LICENSE.txt`.

`cmake/zstd.cmake` builds a static library from an explicit source list:

- programs, the dictionary builder, the deprecated API and legacy formats are
  not compiled, and multithreading is not enabled;
- `ZSTD_EXCLUDE_{GREEDY,LAZY,LAZY2,BTLAZY2,BTOPT,BTULTRA}_BLOCK_COMPRESSOR`
  remove the strategies level 3 never uses (level 3 is `ZSTD_dfast`), which
  removes about 270 KB of linked code with identical output;
- the x86-64 assembly Huffman decoder is compiled on non-MSVC x86-64 hosts;
  elsewhere `ZSTD_DISABLE_ASM` selects the C decoder (same output).

Zstandard's symbols are global C symbols. A program that also links another
Zstandard build gets duplicate definitions at link time; such a consumer must
link one Zstandard only, and that one must be 1.5.7 for the measured context
ceilings in `tests/limits.cpp` to hold.
