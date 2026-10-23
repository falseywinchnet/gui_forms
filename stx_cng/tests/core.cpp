// Round trips, the header probe, the encoded bound, strided input, workspace
// reuse and compatibility with the 0.1.0-rc1 reference bytes.
#include "common.hpp"

#include "stx_zlib/checksum.hpp"

#include <algorithm>
#include <memory>

namespace {

using stx_cng::CngChannels;
using stx_cng::CngError;
using stx_cng::CngImage;
using stx_cng_test::check;

constexpr CngImage shapes[] = {
    {1, 1, CngChannels::rgb}, {13, 7, CngChannels::rgb}, {1, 257, CngChannels::rgb}, {257, 1, CngChannels::rgb},
    {513, 1025, CngChannels::rgb}, {1, 1, CngChannels::rgba}, {13, 7, CngChannels::rgba},
    {1, 257, CngChannels::rgba}, {257, 1, CngChannels::rgba}, {513, 1025, CngChannels::rgba},
    {3, 2, CngChannels::rgba}, {640, 409, CngChannels::rgba}, {16384, 3, CngChannels::rgb}};

void round_trip(stx_cng::CngEncoder& encoder, stx_cng::CngDecoder& decoder, CngImage image,
                stx_cng_test::Pattern pattern) {
    std::vector<std::byte> samples = stx_cng_test::generate(image, pattern, image.width);
    stx_cng_test::Encoded encoded = stx_cng_test::encode(encoder, image, samples, stx_cng_test::tight_row_bytes(image));
    check(encoded.result.error == CngError::none, "encode succeeds");
    check(encoded.result.encoded_bytes <= stx_cng::cng_encoded_bound(image), "encoded within bound");
    stx_cng::CngInspectResult inspected = stx_cng::inspect_cng(encoded.bytes);
    check(inspected.error == CngError::none && inspected.image.width == image.width
        && inspected.image.height == image.height && inspected.image.channels == image.channels, "header probe");
    std::vector<std::byte> restored(samples.size());
    stx_cng::CngDecodeResult decoded = decoder.decode(encoded.bytes, restored);
    check(decoded.error == CngError::none, "decode succeeds");
    check(restored == samples, "exact samples");
    // A second encode of the same samples writes the same bytes.
    stx_cng_test::Encoded again = stx_cng_test::encode(encoder, image, samples, stx_cng_test::tight_row_bytes(image));
    check(again.bytes == encoded.bytes, "deterministic output");
}

void strided_input() {
    stx_cng::CngEncoder encoder;
    CngImage image{301, 211, CngChannels::rgba};
    std::vector<std::byte> tight = stx_cng_test::generate(image, stx_cng_test::Pattern::sparse_noise, 5);
    std::size_t row = static_cast<std::size_t>(stx_cng_test::tight_row_bytes(image));
    std::size_t stride = row + 13;
    // Padding bytes differ from every sample so a stride error shows.
    std::vector<std::byte> padded(stride * (image.height - 1) + row, std::byte{0xA5});
    for (std::size_t y = 0; y < image.height; ++y) {
        std::copy_n(tight.begin() + static_cast<std::ptrdiff_t>(y * row), row, padded.begin() + static_cast<std::ptrdiff_t>(y * stride));
    }
    stx_cng_test::Encoded from_tight = stx_cng_test::encode(encoder, image, tight, row);
    stx_cng_test::Encoded from_padded = stx_cng_test::encode(encoder, image, padded, stride);
    check(from_padded.result.error == CngError::none && from_padded.bytes == from_tight.bytes, "strided input encodes identically");
    std::vector<std::byte> short_input(padded.begin(), padded.end() - 1);
    std::vector<std::byte> destination(stx_cng::cng_encoded_bound(image));
    stx_cng::CngEncodeResult refused = encoder.encode(stx_cng::CngEncodeInput{image, stride, short_input}, destination);
    check(refused.error == CngError::invalid_argument && refused.encoded_bytes == 0, "short strided input refused");
    refused = encoder.encode(stx_cng::CngEncodeInput{image, row - 1, tight}, destination);
    check(refused.error == CngError::invalid_argument, "stride below a row refused");
}

bool untouched(std::span<const std::byte> bytes, std::byte value) {
    for (std::byte element : bytes) {
        if (element != value) {
            return false;
        }
    }
    return true;
}

void contract_refusals() {
    stx_cng::CngEncoder encoder;
    CngImage image{17, 9, CngChannels::rgb};
    std::vector<std::byte> samples = stx_cng_test::generate(image, stx_cng_test::Pattern::noise, 1);
    std::vector<std::byte> destination(stx_cng::cng_encoded_bound(image));
    std::vector<std::byte> small(destination.size() - 1);
    stx_cng::CngEncodeResult refused = encoder.encode(stx_cng::CngEncodeInput{image, 51, samples}, small);
    check(refused.error == CngError::insufficient_storage, "destination below the bound refused");
    // Destination overlapping the samples.
    std::vector<std::byte> shared(samples.size() + destination.size());
    std::copy(samples.begin(), samples.end(), shared.begin());
    std::span<const std::byte> inside(shared.data(), samples.size());
    std::span<std::byte> overlapping(shared.data() + 8, destination.size());
    refused = encoder.encode(stx_cng::CngEncodeInput{image, 51, inside}, overlapping);
    check(refused.error == CngError::overlap, "overlapping destination refused");
    refused = encoder.encode(stx_cng::CngEncodeInput{CngImage{0, 9, CngChannels::rgb}, 0, samples}, destination);
    check(refused.error == CngError::unsupported_dimensions, "zero width refused");
    refused = encoder.encode(stx_cng::CngEncodeInput{CngImage{17, 9, static_cast<CngChannels>(2)}, 34, samples}, destination);
    check(refused.error == CngError::unsupported_dimensions, "unknown channel count refused");

    stx_cng::CngDecoder decoder;
    stx_cng_test::Encoded encoded = stx_cng_test::encode(encoder, image, samples, 51);
    std::vector<std::byte> output(samples.size() + 1, std::byte{0x5A});
    stx_cng::CngDecodeResult decoded = decoder.decode(encoded.bytes, output);
    check(decoded.error == CngError::insufficient_storage, "wrong destination size refused");
    check(untouched(output, std::byte{0x5A}), "size refusal writes nothing");
    std::vector<std::byte> joined(encoded.bytes);
    joined.resize(encoded.bytes.size() + samples.size());
    std::span<std::byte> aliased(joined.data() + 4, samples.size());
    decoded = decoder.decode(std::span<const std::byte>(joined.data(), encoded.bytes.size()), aliased);
    check(decoded.error == CngError::overlap, "overlapping decode refused");
}

void header_refusals_leave_output() {
    stx_cng::CngEncoder encoder;
    stx_cng::CngDecoder decoder;
    CngImage image{9, 9, CngChannels::rgba};
    std::vector<std::byte> samples = stx_cng_test::generate(image, stx_cng_test::Pattern::ramp, 2);
    stx_cng_test::Encoded encoded = stx_cng_test::encode(encoder, image, samples, 36);
    std::vector<std::byte> damaged(encoded.bytes);
    damaged[1] = std::byte{0x44};
    std::vector<std::byte> output(samples.size(), std::byte{0x5A});
    stx_cng::CngDecodeResult decoded = decoder.decode(damaged, output);
    check(decoded.error == CngError::invalid_signature && untouched(output, std::byte{0x5A}), "signature refusal writes nothing");
}

void workspace_reuse() {
    stx_cng::CngEncoder encoder;
    stx_cng::CngDecoder decoder;
    check(decoder.workspace_bytes() == 0 && encoder.workspace_bytes() == 0, "no storage before first use");
    CngImage large{700, 600, CngChannels::rgba};
    CngImage small{16, 16, CngChannels::rgba};
    std::vector<std::byte> large_samples = stx_cng_test::generate(large, stx_cng_test::Pattern::sparse_noise, 3);
    std::vector<std::byte> small_samples = stx_cng_test::generate(small, stx_cng_test::Pattern::sparse_noise, 4);
    stx_cng_test::Encoded large_file = stx_cng_test::encode(encoder, large, large_samples, 2800);
    std::size_t encoder_grown = encoder.workspace_bytes();
    stx_cng_test::Encoded small_file = stx_cng_test::encode(encoder, small, small_samples, 64);
    check(encoder.workspace_bytes() == encoder_grown, "encoder storage reused for a smaller image");
    std::vector<std::byte> output(small_samples.size());
    stx_cng::CngDecodeResult decoded = decoder.decode(small_file.bytes, output);
    std::size_t small_workspace = decoder.workspace_bytes();
    check(decoded.error == CngError::none && small_workspace > 0, "decoder grows for a small image");
    std::vector<std::byte> large_output(large_samples.size());
    decoded = decoder.decode(large_file.bytes, large_output);
    std::size_t large_workspace = decoder.workspace_bytes();
    check(decoded.error == CngError::none && large_workspace > small_workspace, "decoder grows for a larger image");
    decoded = decoder.decode(small_file.bytes, output);
    check(decoded.error == CngError::none && decoder.workspace_bytes() == large_workspace, "decoder storage reused");
    decoder.release();
    check(decoder.workspace_bytes() == 0, "release frees storage");
    decoded = decoder.decode(small_file.bytes, output);
    check(decoded.error == CngError::none && output == small_samples, "decode after release");
}

void admission() {
    check(stx_cng::cng_encoded_bound(CngImage{16384, 4096, CngChannels::rgba}) != 0, "2^26 pixels admitted");
    check(stx_cng::cng_encoded_bound(CngImage{16384, 4097, CngChannels::rgba}) == 0, "beyond 2^26 pixels refused");
    check(stx_cng::cng_encoded_bound(CngImage{16385, 1, CngChannels::rgb}) == 0, "side beyond 16384 refused");
    check(stx_cng::cng_encoded_bound(CngImage{1, 16384, CngChannels::rgb}) != 0, "tall column admitted");
    check(stx_cng::cng_sample_bytes(CngImage{16384, 4096, CngChannels::rgba}) == (std::size_t{1} << 28), "sample bytes");
}

// Reference encodings from the 0.1.0-rc1 release (falseywinchnet/stx_cng
// a1797b84) of generate(shape, pattern, width): size and CRC32 of the file.
struct Reference final {
    CngImage image;
    stx_cng_test::Pattern pattern;
    std::size_t bytes;
    std::uint32_t crc;
};

#include "reference_encodings.inc"

void reference_compatibility() {
    stx_cng::CngEncoder encoder;
    for (const Reference& reference : references) {
        std::vector<std::byte> samples = stx_cng_test::generate(reference.image, reference.pattern, reference.image.width);
        stx_cng_test::Encoded encoded = stx_cng_test::encode(encoder, reference.image, samples,
                                                             stx_cng_test::tight_row_bytes(reference.image));
        std::uint32_t crc = stx_zlib::crc32(encoded.bytes);
        check(encoded.bytes.size() == reference.bytes && crc == reference.crc, "0.1.0-rc1 reference bytes");
    }
}

} // namespace

int main() {
    std::unique_ptr<stx_cng::CngEncoder> encoder = std::make_unique<stx_cng::CngEncoder>();
    std::unique_ptr<stx_cng::CngDecoder> decoder = std::make_unique<stx_cng::CngDecoder>();
    for (const CngImage& image : shapes) {
        for (stx_cng_test::Pattern pattern : stx_cng_test::patterns) {
            round_trip(*encoder, *decoder, image, pattern);
        }
    }
    strided_input();
    contract_refusals();
    header_refusals_leave_output();
    workspace_reuse();
    admission();
    reference_compatibility();
    return stx_cng_test::finish("stx_cng_core");
}
