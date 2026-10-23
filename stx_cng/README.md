# stx_cng

Standalone C++20 CNG (Computational Network Graphics) image codec: lossless
RGB8 and RGBA8 in the CSP v2 byte format (`docs/FORMAT.md`). It depends on
`stx_zlib` (CRC-32 only) and a private, pinned Zstandard 1.5.7. No
GUI.Forms, operating-system, thread or GPU dependency; no global mutable
state.

On GUI assets CNG files are smaller than the best PNG and decode several
times faster; on small icons they are about a tenth larger. The intake's
measurements, with their workload and machine, are in
`results/2026-10-22-intake/`.

## Contract

- Byte spans in, byte spans out. Every entry point is `noexcept` and reports
  failure as a `CngError` in its result. No borrow outlives a call.
- `inspect_cng` checks the signature and the header chunk (checksum and every
  field) and returns the image's width, height and channels.
  `validate_cng_framing` also walks every chunk (the band count the header
  implies, lengths, types, checksums, the end chunk and end of input) without
  decoding, for admitting a file before it is drawn.
- `cng_sample_bytes(image)` is `width * height * channels`;
  `cng_encoded_bound(image)` is the largest file the encoder can write. Both
  are zero for an image outside the admitted extent: sides 1 to 16,384, at
  most 2^26 pixels, 3 or 4 channels.
- `CngDecoder::decode` writes exactly `cng_sample_bytes` tightly packed bytes,
  top row first. Signature, header, size and overlap refusals leave the
  destination untouched; a refusal while decoding bands leaves the earlier
  bands written and the rest unspecified, to be discarded. It refuses every
  truncation, trailing byte, checksum mismatch and non-canonical frame
  (`docs/FORMAT.md`, "Strict decoding").
- `CngEncoder::encode` takes samples with any row stride of at least
  `width * channels` bytes and writes one file to the destination's front; the
  destination must hold `cng_encoded_bound` bytes. Output is deterministic.
  On failure the destination is unspecified and `encoded_bytes` is zero.
- Input and output spans must be disjoint (`CngError::overlap`).
- Each owner grows its storage to the largest band it has seen and reuses it,
  so calls that fit allocate nothing; `release()` frees it, and
  `workspace_bytes()` reports it. Storage is bounded for every admitted image
  (`docs/HARDENING.md`): at most about 9.0 MB for an encoder and 6.5 MB for
  a decoder, Zstandard 1.5.7's contexts included. Allocation failure is
  `CngError::allocation_failed` and leaves the owner usable.
- Owners are neither copyable nor movable; hold them by `std::unique_ptr`.
  One execution context per owner; distinct owners may run concurrently.

## Example

```cpp
#include <stx_cng/codec.hpp>
#include <memory>
#include <vector>

std::vector<std::byte> encoded; // a complete CNG file
stx_cng::CngInspectResult probe = stx_cng::inspect_cng(encoded);
if (probe.error == stx_cng::CngError::none) {
    std::vector<std::byte> samples(stx_cng::cng_sample_bytes(probe.image));
    std::unique_ptr<stx_cng::CngDecoder> decoder = std::make_unique<stx_cng::CngDecoder>();
    stx_cng::CngDecodeResult decoded = (*decoder).decode(encoded, samples);
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

- `stx_cng_core`: round trips over shapes and patterns, determinism, strided
  input, refusals, storage reuse, and 78 rc1 encodings pinned by size and
  CRC-32.
- `stx_cng_malformed`: every proper prefix and every flipped byte of sample
  files refused, checksum-repaired flips handled, hand-built header, count,
  frame, grammar, literal and padding cases.
- `stx_cng_limits`: the widest, tallest and storage-maximal shapes, and the
  admitted area's edges, against the storage ceilings.
- `stx_cng_fuzz_harness`: the libFuzzer entry run as a fixed replay; CI runs
  the real fuzzer briefly per change (`fuzz/cng_fuzz.cpp`).
