// Installed stx_cng consumer: encode an image, probe and decode it, refuse a
// damaged copy, and read a member back from a collection, using only the
// installed headers and the exported stx_cng::stx_cng target.
#include <stx_cng/codec.hpp>
#include <stx_cng/collection.hpp>

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
    stx_cng::CngImage image{37, 23, stx_cng::CngChannels::bgra};
    std::vector<std::byte> samples(stx_cng::cng_sample_bytes(image));
    for (std::size_t index = 0; index < samples.size(); ++index) {
        samples[index] = static_cast<std::byte>((index * 29U + index / 97U) & 255U);
    }
    std::unique_ptr<stx_cng::CngEncoder> encoder = std::make_unique<stx_cng::CngEncoder>();
    std::vector<std::byte> file(stx_cng::cng_encoded_bound(image));
    std::uint64_t row_bytes = static_cast<std::uint64_t>(image.width) * 4U;
    stx_cng::CngEncodeInput input{image, row_bytes, samples, 2};
    stx_cng::CngEncodeResult encoded = (*encoder).encode(input, file);
    require(encoded.error == stx_cng::CngError::none, "encode refused a valid image");
    file.resize(encoded.encoded_bytes);

    stx_cng::CngInspectResult inspected = stx_cng::inspect_cng(file);
    require(inspected.error == stx_cng::CngError::none, "probe refused the encoded file");
    require(inspected.image.width == image.width && inspected.image.height == image.height, "dimensions changed");
    std::unique_ptr<stx_cng::CngDecoder> decoder = std::make_unique<stx_cng::CngDecoder>();
    std::vector<std::byte> restored(samples.size());
    require((*decoder).decode(file, restored, row_bytes, 2).error == stx_cng::CngError::none, "decode refused the encoded file");
    require(restored == samples, "round trip changed samples");

    std::vector<std::byte> broken = file;
    broken[60] ^= std::byte{1};
    require(stx_cng::validate_cng_framing(broken).error == stx_cng::CngError::crc_mismatch, "corrupt file admitted");

    stx_cng::CngCollectionMember member{stx_cng::CngMemberId{5}, image, row_bytes, samples};
    std::span<const stx_cng::CngCollectionMember> members(&member, 1);
    std::vector<std::byte> collection(stx_cng::cng_collection_bound(members, stx_cng::CngCollectionForm::packed));
    std::unique_ptr<stx_cng::CngCollectionEncoder> collector = std::make_unique<stx_cng::CngCollectionEncoder>();
    stx_cng::CngCollectionEncodeResult packed = (*collector).encode(members, stx_cng::CngCollectionForm::packed, 1, collection);
    require(packed.error == stx_cng::CngError::none, "collection encode refused a valid member");
    collection.resize(packed.encoded_bytes);
    std::unique_ptr<stx_cng::CngCollectionDecoder> reader = std::make_unique<stx_cng::CngCollectionDecoder>();
    std::vector<std::byte> member_samples(samples.size());
    stx_cng::CngDecodeResult read = (*reader).decode_member(collection, member.id, member_samples, row_bytes, 1);
    require(read.error == stx_cng::CngError::none && member_samples == samples, "collection member changed");
    return 0;
}
