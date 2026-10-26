// The format's conformance vectors (tests/vectors, FORMAT.md section 12):
// every <name>.cng decodes exactly to <name>.raw, its B, G, R(, A) samples,
// at one and four threads, and its mosaic to the block means of those
// samples; encoding the samples gives a file that passes strict framing and
// decodes to them, and, with the Zstandard version that wrote the vectors
// (named in LIST), the vector byte for byte at one and four threads.
#include "common.hpp"

#include <fstream>
#include <iterator>
#include <string>
#include <zstd.h>

namespace {

using namespace stx_cng_test;

std::byte byte_of(char character) {
    std::byte value = static_cast<std::byte>(static_cast<unsigned char>(character));
    return value;
}

Bytes read_file(const std::string& path) {
    std::ifstream input(path, std::ios::binary);
    std::vector<char> characters((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    Bytes bytes(characters.size());
    std::transform(characters.begin(), characters.end(), bytes.begin(), byte_of);
    return bytes;
}

} // namespace

int main(int argc, char** argv) {
    if (argc != 2) {
        std::fprintf(stderr, "usage: stx_cng_conformance <vector folder>\n");
        return EXIT_FAILURE;
    }
    std::string folder = argv[1];
    std::ifstream list(folder + "/LIST");
    check(static_cast<bool>(list), "the vector list opens");
    std::string word;
    unsigned zstd_version = 0;
    list >> word >> zstd_version;
    check(word == "zstd", "the vector list names its Zstandard version");
    bool same_zstd = ZSTD_versionNumber() == zstd_version;
    check(same_zstd, "the pinned Zstandard is the vectors' version");
    std::unique_ptr<stx_cng::CngDecoder> decoder = std::make_unique<stx_cng::CngDecoder>();
    std::unique_ptr<stx_cng::CngEncoder> encoder = std::make_unique<stx_cng::CngEncoder>();
    unsigned vectors = 0;
    std::string name;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    unsigned channels = 0;
    while (list >> name >> width >> height >> channels) {
        stx_cng::CngImage image{width, height, static_cast<stx_cng::CngChannels>(channels)};
        Bytes file = read_file(folder + "/" + name + ".cng");
        Bytes raw = read_file(folder + "/" + name + ".raw");
        check(raw.size() == stx_cng::cng_sample_bytes(image), name.c_str());
        stx_cng::CngInspectResult framing = stx_cng::validate_cng_framing(file);
        check(framing.error == stx_cng::CngError::none, "every vector passes strict framing");
        for (std::uint32_t threads : {1U, 4U}) {
            Decoded decoded = decode(*decoder, file, threads);
            check(decoded.result.error == stx_cng::CngError::none && decoded.samples == raw, name.c_str());
        }
        stx_cng::CngImage mosaic = stx_cng::cng_mosaic_image(image);
        Bytes means(static_cast<std::size_t>(stx_cng::cng_sample_bytes(mosaic)));
        stx_cng::CngDecodeResult decoded_mosaic = (*decoder).decode_mosaic(file, means, tight_row_bytes(mosaic));
        check(decoded_mosaic.error == stx_cng::CngError::none && means == block_means(raw, image), "the mosaic is the block means");
        for (std::uint32_t threads : {1U, 4U}) {
            Encoded encoded = encode(*encoder, image, raw, tight_row_bytes(image), threads);
            check(encoded.result.error == stx_cng::CngError::none, "every vector re-encodes");
            if (same_zstd) {
                check(encoded.bytes == file, name.c_str());
            }
            Decoded again = decode(*decoder, encoded.bytes, 1);
            check(again.samples == raw, "a re-encoded vector decodes to its samples");
        }
        ++vectors;
    }
    check(vectors == 20, "twenty vectors");
    std::printf("stx_cng conformance: %u vectors\n", vectors);
    return finish("stx_cng_conformance");
}
