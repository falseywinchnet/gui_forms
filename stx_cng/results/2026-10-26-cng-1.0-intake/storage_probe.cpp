// Owner storage after one call: the decoder at 1, 4 and 64 threads on an
// image of at least 64 full segments, and the encoder on noise and on a flat
// image, per sample.
#include "stx_cng/codec.hpp"

#include <cstdio>
#include <memory>
#include <vector>

int main() {
    const stx_cng::CngChannels kinds[] = {stx_cng::CngChannels::bgr, stx_cng::CngChannels::bgra};
    for (stx_cng::CngChannels channels : kinds) {
        stx_cng::CngImage image{1024, 256, channels};
        std::size_t count = static_cast<std::size_t>(stx_cng::cng_sample_bytes(image));
        std::uint64_t row = static_cast<std::uint64_t>(image.width) * static_cast<std::size_t>(channels);
        for (int flat = 0; flat < 2; ++flat) {
            std::vector<std::byte> samples(count);
            std::uint32_t state = 12345;
            for (std::byte& value : samples) {
                state = state * 1664525U + 1013904223U;
                value = flat != 0 ? std::byte{90} : static_cast<std::byte>(state >> 24);
            }
            std::unique_ptr<stx_cng::CngEncoder> encoder = std::make_unique<stx_cng::CngEncoder>();
            std::vector<std::byte> file(static_cast<std::size_t>(stx_cng::cng_encoded_bound(image)));
            stx_cng::CngEncodeResult encoded = (*encoder).encode(stx_cng::CngEncodeInput{image, row, samples, 1}, file);
            file.resize(encoded.encoded_bytes);
            std::printf("channels %d %s: file %zu B, encoder %zu B (%.2f per sample), bound %llu\n", static_cast<int>(channels),
                        flat != 0 ? "flat" : "noise", file.size(), (*encoder).workspace_bytes(),
                        static_cast<double>((*encoder).workspace_bytes()) / static_cast<double>(count),
                        static_cast<unsigned long long>(stx_cng::cng_encoded_bound(image)));
            for (std::uint32_t threads : {1U, 4U, 64U}) {
                std::unique_ptr<stx_cng::CngDecoder> decoder = std::make_unique<stx_cng::CngDecoder>();
                std::vector<std::byte> out(count);
                stx_cng::CngDecodeResult decoded = (*decoder).decode(file, out, row, threads);
                std::printf("  decoder at %u threads: %zu B (%s)\n", threads, (*decoder).workspace_bytes(),
                            decoded.error == stx_cng::CngError::none && out == samples ? "exact" : "FAILED");
            }
        }
    }
    return 0;
}
