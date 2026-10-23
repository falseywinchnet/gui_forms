// Extreme dimensions: the widest and tallest admitted shapes, a band-row
// count of exactly fifteen, the largest admitted area (header and bound
// only), one large image round trip, and the owned-storage ceilings.
#include "common.hpp"

#include <memory>

namespace {

using stx_cng::CngChannels;
using stx_cng::CngError;
using stx_cng::CngImage;
using stx_cng_test::check;

// Storage maxima enumerated over every admitted width and channel count (see
// docs/HARDENING.md): arrays, plus each context's measured size, which is
// below these ceilings for Zstandard 1.5.7.
constexpr std::size_t encoder_array_ceiling = 7649615;
constexpr std::size_t decoder_array_ceiling = 6338929;
constexpr std::size_t compression_context_ceiling = 4194304 + 8;
constexpr std::size_t decompression_context_ceiling = 262144;

void round_trip(stx_cng::CngEncoder& encoder, stx_cng::CngDecoder& decoder, CngImage image,
                stx_cng_test::Pattern pattern, std::uint64_t padding) {
    std::size_t row = static_cast<std::size_t>(stx_cng_test::tight_row_bytes(image));
    std::vector<std::byte> samples = stx_cng_test::generate(image, pattern, 11);
    std::vector<std::byte> input = samples;
    std::uint64_t stride = row + padding;
    if (padding != 0) {
        input.assign(static_cast<std::size_t>(stride) * (image.height - 1) + row, std::byte{0});
        for (std::size_t y = 0; y < image.height; ++y) {
            std::copy_n(samples.begin() + static_cast<std::ptrdiff_t>(y * row), row,
                        input.begin() + static_cast<std::ptrdiff_t>(y * stride));
        }
    }
    stx_cng_test::Encoded encoded = stx_cng_test::encode(encoder, image, input, stride);
    check(encoded.result.error == CngError::none, "extreme shape encodes");
    std::vector<std::byte> restored(samples.size());
    stx_cng::CngDecodeResult decoded = decoder.decode(encoded.bytes, restored);
    check(decoded.error == CngError::none && restored == samples, "extreme shape round trip");
    check(encoder.workspace_bytes() <= encoder_array_ceiling + compression_context_ceiling, "encoder storage ceiling");
    check(decoder.workspace_bytes() <= decoder_array_ceiling + decompression_context_ceiling, "decoder storage ceiling");
}

} // namespace

int main() {
    std::unique_ptr<stx_cng::CngEncoder> encoder = std::make_unique<stx_cng::CngEncoder>();
    std::unique_ptr<stx_cng::CngDecoder> decoder = std::make_unique<stx_cng::CngDecoder>();
    // Widest row: 16384 RGBA pixels, fifteen rows per band, five bands.
    round_trip(*encoder, *decoder, CngImage{16384, 64, CngChannels::rgba}, stx_cng_test::Pattern::sparse_noise, 0);
    round_trip(*encoder, *decoder, CngImage{16384, 1, CngChannels::rgb}, stx_cng_test::Pattern::noise, 0);
    // Tallest column, one band.
    round_trip(*encoder, *decoder, CngImage{1, 16384, CngChannels::rgb}, stx_cng_test::Pattern::ramp, 0);
    round_trip(*encoder, *decoder, CngImage{1, 16384, CngChannels::rgba}, stx_cng_test::Pattern::noise, 3);
    // The width that maximizes owned storage (docs/HARDENING.md), strided.
    round_trip(*encoder, *decoder, CngImage{15887, 66, CngChannels::rgb}, stx_cng_test::Pattern::noise, 5);
    // A large image: many full bands and a short final band.
    round_trip(*encoder, *decoder, CngImage{4096, 3001, CngChannels::rgb}, stx_cng_test::Pattern::sparse_noise, 0);
    // The largest admitted area is admitted by the bound; one pixel more is not.
    CngImage largest{8192, 8192, CngChannels::rgba};
    check(stx_cng::cng_encoded_bound(largest) > stx_cng::cng_sample_bytes(largest), "largest area bound");
    check(stx_cng::cng_encoded_bound(CngImage{8193, 8192, CngChannels::rgba}) == 0, "area above 2^26 refused");
    check(stx_cng::cng_encoded_bound(CngImage{16384, 16384, CngChannels::rgb}) == 0, "two maximal sides refused");
    return stx_cng_test::finish("stx_cng_limits");
}
