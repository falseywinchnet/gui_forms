# Provenance

`stx_cng` is the GUI.Forms intake of the owner's CNG (Computational Network
Graphics) codec, to the standard of `stx_zlib`, `stx_png` and `stx_conv`. The
first intake (2026-10-22) took in the 0.1.0-rc1 CSP v2 format; this one
(2026-10-26) replaces it with CNG 1.0, a different format: 0.1 files are not
read.

## Upstream

Repository `falseywinchnet/stx_cng`, commit
`9fde861451367796fa11da8686e47d164b6c8042` ("CNG 1.0: store samples in B, G,
R, A order", 2026-10-26). The MIT licence is kept verbatim in `LICENSE`.
The files this package derives from, by SHA-256 of their contents as stored in
git (LF line endings):

| Upstream file | SHA-256 | Here |
| --- | --- | --- |
| `LICENSE` | `3db56fe025958b56bc7969f9f61787ace2b3106187789976c83ae9c6a6174a0b` | verbatim |
| `docs/FORMAT.md` | `658325aca08b3323aa126aabdfa473e09492492eb2decf22cfb6cecbafed556d` | verbatim |
| `docs/COLLECTIONS.md` | `c32d25fd707b398bd995e3a8208701038abf067a42d2d26a172e3f47125f95ec` | derived: "Access" replaced (D3) |
| `include/stx_cng/codec.hpp` | `23cb5a6430b0e3b099bf86f79a1e99a048153a6d4c85af0ba2c77f6c70e77ba7` | `include/stx_cng/codec.hpp`, rewritten API |
| `include/stx_cng/collection.hpp` | `d0c664b64bba7be158246b84de35157dd00adf50fc22435e23f4536b88c3daa5` | `include/stx_cng/collection.hpp`, rewritten API (D3) |
| `src/cng_codec.hpp` | `6856e752f8298a6f9b39f68e9a6c8554574b43db0b1d6362cbd38505f097b0ae` | `src/interface.hpp`; the owners in `include/stx_cng/codec.hpp` |
| `src/cng_decoder.cpp` | `4da234283ec1811aab951ba3657233f009d811ee45d6817ac6f206cbaa7f9cbc` | `src/container.cpp`, `src/tables.cpp`, `src/decoder.cpp`, the decode calls in `src/codec.cpp` |
| `src/cng_encoder.cpp` | `636f43da5589052f254d56c4bbd61330db34dfd6b38e5baec4dd53cbd41603ca` | `src/encoder.cpp` |
| `src/cng_format.hpp` | `8d1a090cb31372fd8d129c6ad8ae80722a3ec616096ee8725cb412c9043ad19c` | `src/format.hpp` |
| `src/cng_internal.hpp` | `073cd1fd9a83f75c1b3a5a2acf634679c22cf71e17c9e99b6ab70811350c8eee` | `src/container.hpp`, `src/tables.hpp`, `src/decoder.hpp` |
| `src/code_lengths.hpp` | `fcc1eee7fe4f2b99f561678620c5336474e35c939c08dda8ab200f5120f6ac0e` | verbatim |
| `src/codec.cpp` | `992d0262b8a27233d801ceb8e3ae3231b0026ae48f2bad19210f644740df7dda` | `src/codec.cpp`, `src/interface.hpp` |
| `src/collection.cpp` | `5769693bfdc2d1ef9c95e65848946fcfc3eba2ea54e7154e0d1c6747eb63fcb1` | `src/collection.cpp` |
| `tests/conformance.cpp` | `3ec848a025489e1ec3c2380fdfb851d445fc794524977415142c6cf010a40075` | `tests/conformance.cpp` |
| `tests/malformed.cpp` | `3b138cc8b145c6c24ce5a79b8e999421eed07b3eb929e9baf8211a9c37abc095` | `tests/malformed.cpp` |
| `tests/roundtrip.cpp` | `9f8eeb2c1ec548a8de559ac2f087a8256a4330e5bbf87781014073c599c25a0f` | `tests/core.cpp`, `tests/collection.cpp` |
| `tests/test_support.hpp` | `e1fe0bea24703a368f9e47f3c3f1a11671f381f178e53f4a3d34018951e441af` | `tests/common.hpp` |

The conformance vectors are taken in byte for byte as `tests/vectors/` (marked
binary in `.gitattributes`, so no checkout converts them):

| `tests/vectors/` file | SHA-256 |
| --- | --- |
| `LIST` | `e0de26b95cdf5105cb7b3b78509350e40cb777fc79c61249345ee3adf47867cf` |
| `column_9.cng` | `6d91acf12abf50fbf8ceb175347dcb8f129c280a39765319eaa5c4675bca39ed` |
| `column_9.raw` | `60931404d212d64ac4e3232e3f3962a9303573283fe36ae64eef73b708890993` |
| `corpus_checker.cng` | `7b8b334cbd2d022d1ad0e94938cf7ba330e3219d342d03b52ad4506fd57cd170` |
| `corpus_checker.raw` | `13dd3db0e5b1b8b516f47e085c840597b58c04352b916a53b3806eb62644e2e0` |
| `corpus_odd_surface_65_67.cng` | `a018aa44d1e9e15f283300f1d2da040785ef72fb21edb1b0d6c91c4712c92239` |
| `corpus_odd_surface_65_67.raw` | `df65e336ef8241e43167ac72f37ee1b6764233849b46785e2d44ba41e8ddeb5d` |
| `corpus_surface_64_noise_0.cng` | `a82c3f0ca8dab8650b5680aa9cb9da131ef98ff89d35c672eadba36a37d2bc16` |
| `corpus_surface_64_noise_0.raw` | `f24b814706e64dfc1d2675f5f284fe005d9a8af22ef725e6716f3ff6482dc8e1` |
| `corpus_tile_7_phase_0.cng` | `fcf65522b01342750b47435abe8d3f6c3a4033890477840b6f3026f241938085` |
| `corpus_tile_7_phase_0.raw` | `9029949107555ea508fbd4d7c656091fd7adc68d4ab09141749e23641dd4d731` |
| `flat_130x129.cng` | `04889695d16704ddec5926c5655b36903d23a31c091e88c953337bc6fe91b464` |
| `flat_130x129.raw` | `f95ae70e52bdad0b14813bc0bf4177972c9293d9f09ba062966ce3e76749c35f` |
| `manifest.json` | `7389721218135ff49d7bfeebc54d667c9a2d862f03458daa5df6dadc9d0242ec` |
| `noise_140x150.cng` | `1262da59fff9fb256b0ea32932f31b711a056f02c0e64c1c8f9b1aac14f61215` |
| `noise_140x150.raw` | `6160ab7139d9447422e1dd0554c2383604a3f3f53ca51fad8722a923bee04732` |
| `noise_5x7.cng` | `f8d09919b3419306f71f3bb70bc093de4fa302fdb024f8b848a103631a1fc869` |
| `noise_5x7.raw` | `a461cf30ff45f8bb9c1b605221014edf6894e927747cbe7c2871cb2fc367f902` |
| `noto_face.cng` | `3614d8bfa7c840c3f74b72e9142b94c2507725f62a83ba109c33c29627f42266` |
| `noto_face.raw` | `c27aa83ee9166ef2c88ae989d1f7b653b5308307e9273da2131a0c645805d190` |
| `one_pixel_rgb.cng` | `f98e07f00db3b7ed7f5ccfc7c9534060069c03b9fb817d509f6b9953ca35c13a` |
| `one_pixel_rgb.raw` | `cc76e4a328101e1cc89c185e12509259511589c81d8fe0e7ab845630a4ce332f` |
| `one_pixel_rgba.cng` | `18ab1db5f6c84d92b65848084a2a352943f18149a7c93a979ae1c109f65bac01` |
| `one_pixel_rgba.raw` | `1828f964001d879afea5e629dfe65a198f1a0a0917c076bc1fd07e1a8a5c5134` |
| `palette_rgba_90x100.cng` | `ef2582370fd5eeb568159188cd62c07d993d1bb61cdd3bd41f81de7f05329b69` |
| `palette_rgba_90x100.raw` | `d005473636695fd54b02e8b16cf85df4b864706d820a071479376f5bc00bd984` |
| `segment_exact_128x32.cng` | `fbe5912c7384c8a54c04aac3e1a344de86cf6a8f6ec1f2f1a0f2299098a0e62e` |
| `segment_exact_128x32.raw` | `41c90e7e2dcd61174833c3d5442ce966f572697fe132e07fc8833b40b9ea8f53` |
| `segment_partial_129x33.cng` | `868210912c97cee7b497370de566d696894374c4afe7c9184962602992587729` |
| `segment_partial_129x33.raw` | `84620ba1de2e5384572e01786a35e93b3f7a405d0cc2c34dfb2056a888c2444c` |
| `segment_ramp_260x97.cng` | `79d12cab024b6ba03922a3caa57e9bec9695e87c3e22578824c6e47e7bef2069` |
| `segment_ramp_260x97.raw` | `2a4a8064c97cb3c787a40cfa943f9dd3cd954dcc0fecfe234b9ebbc9a5518d2c` |
| `segment_rgba_70x65.cng` | `8630193779357d89f6d3069455c4de35bf79f3e4b5f8f9e7603ac52d2bc15edf` |
| `segment_rgba_70x65.raw` | `6ad2713f6bffcbe70fdd348b8387fd02c09c051902552406fc721759d0194ab8` |
| `smooth_300x200.cng` | `4dce2aa272c74ea7aa5ab485ea093f474a3682df8842510348808421e3d59b6f` |
| `smooth_300x200.raw` | `979458b8820739e7d7529af08d1eb3d6547192a5669a0951c28310c6f3c2b31a` |
| `smooth_alpha_300x200.cng` | `8fb10624c2813e65759691db91e33f2b1a0f39930e5c1fc98afa90fd4011ce62` |
| `smooth_alpha_300x200.raw` | `b6a9bed2db29b227b6a229cd2ff115bcd5b052745b85f99cdd09fe9db5dbf6a3` |
| `stripes_150x140.cng` | `4dc4dc570b0a239c1c1e2dff35ff6a6bd375b33e1b0fafb47a5b8afe2f2a59aa` |
| `stripes_150x140.raw` | `3c679b5eef622641006079175e16a83dfb6de419f5fc0e4cf9fa02640ee4244a` |
| `tile8_384x256.cng` | `4c05164be572381796ff70ac79b86900155a484f3a3f3fce6cfc70b8c3c3fb08` |
| `tile8_384x256.raw` | `dd7da520f08fd8c27ce7cc455f97ae2130d374a2a4e31134811f25449dbf637e` |

Upstream's agent instructions (`AGENTS.md`), origin metadata (`ORIGIN.json`),
contributor and change notes, its API, building, measurement and house-style
documents, its evidence records, experiments, `stx_cngcompress` tool and
helper scripts were read as data and are not part of this package.

## What changed

The byte format is upstream's CNG 1.0 unchanged, and so is the reference
encoder: `tests/conformance.cpp` re-encodes every conformance vector from its
samples and requires upstream's file byte for byte, at one and four threads,
with Zstandard 1.5.7. The encoder prices its parse in doubles in upstream's
order of operations, and the library is compiled with `-ffp-contract=off`
(`/fp:precise` on MSVC) so that no host fuses a multiply and an add and makes
another choice.

The code is a port of upstream's algorithms (MED prediction, activity
classes, the costed parse with repeats, specials and the green-keyed cache,
static rANS, the §3.4 copy choice and §10 salvage) to this repository's house
style:

- API: byte spans with a caller row stride in place of `std::vector` and
  iostreams; `noexcept` entry points with explicit `CngError` results in place
  of exceptions; `inspect_cng`, `inspect_cng_for_salvage`, and
  `validate_cng_framing` for strict admission; reusable `CngEncoder` and
  `CngDecoder` owners whose storage grows only when an image needs more (no
  allocation per call once grown, decision D4). The decoder writes the
  caller's rows directly, so a BGRA8 destination needs no conversion pass.
- Collections under decision D3: members are byte spans with a row stride and
  the encoder writes into a caller-supplied output budget
  (`CngCollectionEncoder`, `CngCollectionDecoder`); the format is upstream's,
  and `docs/COLLECTIONS.md` replaces only upstream's "Access" section.
- Zstandard contexts are static (`ZSTD_initStaticCCtx`,
  `ZSTD_initStaticDCtx`) inside owner storage, and the tables part must be
  exactly one non-skippable frame.
- Threads are `std::thread` workers started and joined within the call; a
  worker that cannot be started runs on the calling thread. Output does not
  depend on the thread count.
- CRC-32 from `stx_zlib::crc32`; zlib is not a dependency.
- `src/storage.hpp` (growth without exceptions, the fork-join runner) is new.
- Hardening (`docs/HARDENING.md`): every size product checked, validation
  before every write, malformed-input sweeps, extreme-shape tests, sanitizer
  runs and a fuzz harness.

## Zstandard

Zstandard 1.5.7 is a pinned third-party dependency (owner decisions 3 and D5),
used under its BSD licence (Zstandard is dual-licensed BSD or GPLv2; GUI.Forms
takes the BSD option). The official release tarball
`zstd-1.5.7.tar.gz` (2,434,947 bytes) has SHA-256
`eb33e51f49a15e023950cd7825ca74a4a2b43db8354825ac24fc1b7ee09e6fa3`, the hash
the release publishes beside it. `tools/fetch_zstd.sh` downloads and verifies
it into `third_party/` (not checked in); configuration verifies it again
before extracting `lib/` and `LICENSE`. `LICENSE-zstd.txt` is a copy of
that licence, checked against the extracted one at configuration, and is
installed as `share/licenses/stx_cng/zstd-LICENSE.txt`.

`cmake/zstd.cmake` builds a static library from an explicit source list:

- programs, the dictionary builder, the deprecated API and legacy formats are
  not compiled, and multithreading is not enabled;
- the format compresses its tables at level 19, which is `ZSTD_btultra2` for
  every tables body (at most 21,850 bytes);
  `ZSTD_EXCLUDE_{GREEDY,LAZY,LAZY2,BTLAZY2,BTOPT}_BLOCK_COMPRESSOR` remove the
  strategies it never uses, with identical output;
- the x86-64 assembly Huffman decoder is compiled on non-MSVC x86-64 hosts;
  elsewhere `ZSTD_DISABLE_ASM` selects the C decoder (same output).

Zstandard's symbols are global C symbols. A program that also links another
Zstandard build gets duplicate definitions at link time; such a consumer must
link one Zstandard only, and that one must be 1.5.7 for the encoder to write
the conformance vectors' bytes and for the static context sizes to hold.
