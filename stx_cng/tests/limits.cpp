// The format's extent and the owners' storage: the largest admitted shapes
// (header arithmetic only: 2^31 samples is 2 GiB), one sample more refused,
// long single rows and columns, one large image at several thread counts,
// and decoder storage that does not grow with the image.
#include "common.hpp"

#include <limits>

namespace {

using namespace stx_cng_test;
using stx_cng::CngChannels;
using stx_cng::CngError;
using stx_cng::CngImage;

void round_trip(stx_cng::CngEncoder& encoder, stx_cng::CngDecoder& decoder, CngImage image, Pattern pattern, std::uint32_t threads) {
    Bytes raw = generate(image, pattern, 11);
    Encoded encoded = encode(encoder, image, raw, tight_row_bytes(image), threads);
    check(encoded.result.error == CngError::none, "an extreme shape encodes");
    check(stx_cng::validate_cng_framing(encoded.bytes).error == CngError::none, "and passes strict framing");
    Decoded decoded = decode(decoder, encoded.bytes, threads);
    check(decoded.result.error == CngError::none && decoded.samples == raw, "an extreme shape round trip");
}

// Shapes at exactly 2^31 samples are admitted; the bound is defined and
// exceeds the samples. One more row or column is not.
void extent() {
    const CngImage largest[] = {{8192, 65536, CngChannels::bgra}, {536870912, 1, CngChannels::bgra},
                                {1, 536870912, CngChannels::bgra}, {32768, 21845, CngChannels::bgr}};
    for (CngImage image : largest) {
        std::uint64_t samples = stx_cng::cng_sample_bytes(image);
        check(samples != 0 && samples <= stx_cng::maximum_samples, "the largest shapes are admitted");
        check(stx_cng::cng_encoded_bound(image) > samples, "their bound is defined");
        check(stx_cng::cng_segment_count(image) != 0 && stx_cng::cng_mosaic_image(image).width != 0, "their grids are defined");
    }
    const CngImage beyond[] = {{8192, 65537, CngChannels::bgra}, {536870913, 1, CngChannels::bgra},
                               {65536, 10923, CngChannels::bgr},
                               {std::numeric_limits<std::uint32_t>::max(), std::numeric_limits<std::uint32_t>::max(), CngChannels::bgra}};
    for (CngImage image : beyond) {
        check(stx_cng::cng_sample_bytes(image) == 0 && stx_cng::cng_encoded_bound(image) == 0, "beyond 2^31 samples is refused");
        check(stx_cng::cng_segment_count(image) == 0 && stx_cng::cng_mosaic_image(image).width == 0, "and has no grid");
    }
}

} // namespace

int main() {
    std::unique_ptr<stx_cng::CngEncoder> encoder = std::make_unique<stx_cng::CngEncoder>();
    std::unique_ptr<stx_cng::CngDecoder> decoder = std::make_unique<stx_cng::CngDecoder>();
    extent();
    // Long single rows and columns: hundreds and thousands of segments.
    round_trip(*encoder, *decoder, CngImage{70000, 1, CngChannels::bgr}, Pattern::noise, 4);
    round_trip(*encoder, *decoder, CngImage{1, 70000, CngChannels::bgra}, Pattern::gradient, 4);
    round_trip(*encoder, *decoder, CngImage{65536, 2, CngChannels::bgra}, Pattern::patches, 2);
    // Decoder storage depends on the segment, not the image: one full segment
    // and a 1.5-megapixel image leave the same storage at one thread.
    std::unique_ptr<stx_cng::CngDecoder> fresh = std::make_unique<stx_cng::CngDecoder>();
    CngImage segment{128, 32, CngChannels::bgr};
    Bytes segment_file = encode_file(segment, generate(segment, Pattern::noise, 4));
    Decoded small = decode(*fresh, segment_file, 1);
    std::size_t segment_storage = (*fresh).workspace_bytes();
    check(small.result.error == CngError::none && segment_storage > 0, "a segment decodes");
    CngImage large{1536, 1000, CngChannels::bgr};
    Bytes large_raw = generate(large, Pattern::patches, 6);
    Encoded large_file = encode(*encoder, large, large_raw, tight_row_bytes(large), 4);
    for (std::uint32_t threads : {1U, 8U}) {
        Encoded again = encode(*encoder, large, large_raw, tight_row_bytes(large), threads);
        check(again.bytes == large_file.bytes, "a large image writes the same file at every thread count");
    }
    Decoded large_decoded = decode(*fresh, large_file.bytes, 1);
    check(large_decoded.samples == large_raw, "a large image round trip");
    check((*fresh).workspace_bytes() == segment_storage, "decoder storage does not grow with the image");
    Decoded threaded = decode(*fresh, large_file.bytes, 8);
    check(threaded.samples == large_raw, "a large image decodes at eight threads");
    return finish("stx_cng_limits");
}
