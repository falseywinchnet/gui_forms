// Installed stx_cng consumer: encode an image, probe and decode it, and refuse
// a damaged copy, using only the installed header and the exported
// stx_cng::stx_cng target.
#include <stx_cng/codec.hpp>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>

namespace {
void require(bool condition, const char* message) {
    if (condition) {
        return;
    }
    std::fprintf(stderr, "stx_cng: %s\n", message);
    std::abort();
}
} // namespace

int main() {
    stx_cng::CngImage image{37, 23, stx_cng::CngChannels::rgba};
    std::vector<std::byte> samples(stx_cng::cng_sample_bytes(image));
    for (std::size_t index = 0; index < samples.size(); ++index) {
        samples[index] = static_cast<std::byte>((index * 29U + index / 97U) & 255U);
    }
    std::unique_ptr<stx_cng::CngEncoder> encoder = std::make_unique<stx_cng::CngEncoder>();
    std::vector<std::byte> file(stx_cng::cng_encoded_bound(image));
    stx_cng::CngEncodeInput input{image, static_cast<std::uint64_t>(image.width) * 4U, samples};
    stx_cng::CngEncodeResult encoded = (*encoder).encode(input, file);
    require(encoded.error == stx_cng::CngError::none, "encode refused a valid image");
    file.resize(encoded.encoded_bytes);

    stx_cng::CngInspectResult inspected = stx_cng::inspect_cng(file);
    require(inspected.error == stx_cng::CngError::none, "probe refused the encoded file");
    require(inspected.image.width == image.width && inspected.image.height == image.height, "dimensions changed");
    std::unique_ptr<stx_cng::CngDecoder> decoder = std::make_unique<stx_cng::CngDecoder>();
    std::vector<std::byte> restored(samples.size());
    require((*decoder).decode(file, restored).error == stx_cng::CngError::none, "decode refused the encoded file");
    require(restored == samples, "round trip changed samples");

    std::vector<std::byte> broken = file;
    broken[20] ^= std::byte{1};
    require(stx_cng::inspect_cng(broken).error == stx_cng::CngError::crc_mismatch, "corrupt header accepted");
    return 0;
}
