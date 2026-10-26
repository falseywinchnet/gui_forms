# stx_cng

Standalone C++20 CNG (Computational Network Graphics) 1.0 image codec:
lossless BGR8 and BGRA8 (`docs/FORMAT.md`), and collections of up to 4,096
images in one file (`docs/COLLECTIONS.md`). It depends on `stx_zlib` (CRC-32
only), the C++ standard library's threads, and a private, pinned Zstandard
1.5.7. No GUI.Forms, operating-system or GPU dependency; no global mutable
state.

Samples are interleaved blue, green, red, then alpha, the order CNG stores, so
a decode into a BGRA8 destination needs no conversion. The codec keeps every
sample, the colour beneath alpha 0 included, and converts no colour.

The intake's measurements against `stx_png`, with their workload and machine,
are in `results/2026-10-26-cng-1.0-intake/`.

## Contract

- Byte spans in, byte spans out, with a caller row stride. Every entry point
  is `noexcept` and reports failure as a `CngError` in its result. No borrow
  outlives a call.
- `inspect_cng` reads the container as the decoder does (FORMAT.md section
  3.4: an intact copy of the header, directory and tables) and returns the
  image and its segment count. `validate_cng_framing` is the strict admission
  check: the signature, the front copies, the exact length they imply, every
  checksum, both copies of every redundant part identical, and a tables part
  of exactly one Zstandard frame, without decoding.
- `cng_sample_bytes(image)` is `width * height * channels`;
  `cng_encoded_bound(image)` is the largest file the encoder can write. Both
  are zero outside the format's extent: sides of at least 1, at most 2^31
  samples, 3 or 4 channels.
- `CngDecoder::decode` writes every row of the caller's destination, rows
  `row_bytes` apart (at least `width * channels`; the bytes between rows are
  not touched), at 1 to 64 threads. Argument, header and storage refusals
  leave the destination untouched; a refusal while decoding segments leaves
  an unverified partial image, to be discarded. `decode_mosaic` decodes the
  16 x 16-cell preview alone; `decode_salvage` decodes a damaged file as far
  as it goes and reports which segments are exact (FORMAT.md section 10).
- `CngEncoder::encode` takes samples with any row stride of at least
  `width * channels` bytes and writes one file to the destination's front; a
  destination of `cng_encoded_bound` bytes is always enough, and a shorter one
  is refused with the size it needed. Output is deterministic: the same bytes
  for every thread count and on every host.
- Input and output spans must be disjoint (`CngError::overlap`).
- Each owner grows its storage when an image needs more and reuses it, so
  calls that fit allocate nothing; `release()` frees it, and
  `workspace_bytes()` reports it. The decoder's storage depends on the segment
  size and thread count, not the image; the encoder's grows with the image
  (`docs/HARDENING.md`). Allocation failure is `CngError::allocation_failed`
  and leaves the owner usable.
- Owners are neither copyable nor movable; hold them by `std::unique_ptr`.
  One execution context per owner; distinct owners may run concurrently. A
  call given N threads starts N - 1 workers and joins them before it returns;
  a worker that cannot be started runs on the calling thread.

## Example

```cpp
#include <stx_cng/codec.hpp>
#include <memory>
#include <vector>

std::vector<std::byte> encoded; // a complete CNG file
stx_cng::CngInspectResult probe = stx_cng::inspect_cng(encoded);
if (probe.error == stx_cng::CngError::none) {
    std::vector<std::byte> samples(stx_cng::cng_sample_bytes(probe.image));
    std::uint64_t row_bytes = std::uint64_t{probe.image.width} * static_cast<std::uint64_t>(probe.image.channels);
    std::unique_ptr<stx_cng::CngDecoder> decoder = std::make_unique<stx_cng::CngDecoder>();
    stx_cng::CngDecodeResult decoded = (*decoder).decode(encoded, samples, row_bytes, 1);
}
```

## Building

```text
stx_cng/tools/fetch_zstd.sh      # once: fetch and verify zstd-1.5.7.tar.gz
cmake -S stx_cng -B build -DSTX_CNG_TESTS=ON -Dstx_zlib_DIR=<stx_zlib install>/lib/cmake/stx_zlib
cmake --build build
ctest --test-dir build --output-on-failure
```

`-DSTX_CNG_SANITIZE=ON` builds the library, Zstandard and the tests with
AddressSanitizer and UndefinedBehaviorSanitizer. Inside GUI.Forms the package
is added as a subdirectory and uses the in-tree `stx_zlib` target. The
installed package provides `find_package(stx_cng CONFIG)` with the target
`stx_cng::stx_cng`, and installs `LICENSE`, `PROVENANCE.md` and Zstandard's
licence under `share/licenses/stx_cng/`.

## Tests

- `stx_cng_conformance`: the format's 20 vectors decode exactly at one and
  four threads, their mosaics are the block means, and their samples
  re-encode to upstream's files byte for byte.
- `stx_cng_core`: 132 round trips over sizes, channel counts and patterns at
  one, two and four threads, with inspection, strict framing, mosaic and
  salvage; strided samples and destinations; thread counts up to 64; storage
  reuse and release; damage, salvage and the copy choice; every refusal.
- `stx_cng_malformed`: every truncation and flipped byte of small files,
  sampled ones and random bursts on larger files, crafted files with valid
  checksums over hostile payloads, tables, mosaics and dimensions, and the
  tables' range and single-frame rules.
- `stx_cng_limits`: the format's extent at 2^31 samples, long single rows and
  columns, a large image at several thread counts, and decoder storage that
  does not grow with the image.
- `stx_cng_collection`: both collection forms, mixed members, strided input,
  canvas slicing, every refusal, and a damage sweep.
- `stx_cng_fuzz_harness`: the libFuzzer entry run as a fixed replay; CI runs
  the real fuzzer briefly per change (`fuzz/cng_fuzz.cpp`).
