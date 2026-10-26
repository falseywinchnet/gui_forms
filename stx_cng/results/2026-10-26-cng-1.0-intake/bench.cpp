// CNG 1.0 against stx_png on the intake corpus (results/2026-10-22-intake,
// inputs.tsv): size, and decode time at one and N threads.
//
// usage: bench <corpus dir> <output tsv> <threads>
// Every PNG under <corpus dir>/<category>/ is decoded by stx_png to 8-bit
// samples; opaque images become three-channel CNG and colour-type-2 PNG,
// others four-channel. Each image is encoded once by stx_png at levels 6 and
// 9 and once by CNG at N threads (single timed runs), and every CNG file must
// decode to its samples. Decode times are the minimum wall time over
// interleaved rounds (PNG level 6 file, CNG at one thread, CNG at N threads),
// at least three rounds and continued until 0.3 s or 200 rounds.
#include "stx_cng/codec.hpp"
#include "stx_png/decoder.hpp"
#include "stx_png/encoder.hpp"
#include "stx_zlib/checksum.hpp"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

namespace {

using Bytes = std::vector<std::byte>;
using Clock = std::chrono::steady_clock;

std::byte byte_of(char character) {
    std::byte value = static_cast<std::byte>(static_cast<unsigned char>(character));
    return value;
}

Bytes read_file(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    std::vector<char> characters((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    Bytes bytes(characters.size());
    std::transform(characters.begin(), characters.end(), bytes.begin(), byte_of);
    return bytes;
}

double microseconds_since(Clock::time_point start) {
    std::chrono::duration<double, std::micro> elapsed = Clock::now() - start;
    double value = elapsed.count();
    return value;
}

// stx_png's decode state, reused across images.
struct PngReader final {
    std::unique_ptr<stx_zlib::InflateStream> inflate = std::make_unique<stx_zlib::InflateStream>();
    Bytes rows;
    std::vector<std::uint16_t> rgba;
    Bytes scratch = Bytes(1 << 16);
    std::vector<stx_png::PngMetadataRecord> records = std::vector<stx_png::PngMetadataRecord>(4096);
};

stx_png::PngDecodeLimits wide_limits() {
    stx_png::PngDecodeLimits limits;
    limits.pixels = 8192ULL * 8192ULL;
    limits.dimension = 8192;
    limits.filtered_bytes = 1024ULL * 1024ULL * 1024ULL;
    limits.encoded_bytes = 256ULL * 1024ULL * 1024ULL;
    limits.inflate_work = stx_zlib::WorkCount{8ULL * 1024ULL * 1024ULL * 1024ULL};
    return limits;
}

bool decode_png(PngReader& reader, const Bytes& file, stx_png::PngSampleLayout& layout) {
    stx_png::PngDecodeLimits limits = wide_limits();
    stx_png::PngDecodeResult inspected = stx_png::inspect_png_samples(file, limits);
    if (inspected.error != stx_png::PngDecodeError::none) {
        return false;
    }
    layout = inspected.layout;
    reader.rows.resize(layout.row_storage_bytes);
    reader.rgba.resize(layout.rgba_samples);
    stx_png::PngDecodeResult decoded = stx_png::decode_png_samples_discarding_metadata(
        file, limits, *reader.inflate, reader.rows, reader.rgba, reader.scratch, reader.records);
    bool ok = decoded.error == stx_png::PngDecodeError::none;
    return ok;
}

struct Image final {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    bool opaque = true;
    Bytes rgba;      // 8-bit straight RGBA, for stx_png
    Bytes samples;   // B, G, R(, A), for CNG
};

Image image_of(const PngReader& reader, const stx_png::PngSampleLayout& layout) {
    Image image;
    image.width = layout.width;
    image.height = layout.height;
    std::size_t pixels = static_cast<std::size_t>(layout.width) * layout.height;
    image.rgba.resize(pixels * 4);
    for (std::size_t index = 0; index < pixels * 4; ++index) {
        unsigned wide = reader.rgba[index];
        image.rgba[index] = static_cast<std::byte>((wide + 128U) / 257U);
    }
    for (std::size_t pixel = 0; pixel < pixels && image.opaque; ++pixel) {
        image.opaque = image.rgba[pixel * 4 + 3] == std::byte{255};
    }
    std::size_t channels = image.opaque ? 3 : 4;
    image.samples.resize(pixels * channels);
    for (std::size_t pixel = 0; pixel < pixels; ++pixel) {
        const std::byte* from = image.rgba.data() + pixel * 4;
        std::byte* to = image.samples.data() + pixel * channels;
        to[0] = from[2];
        to[1] = from[1];
        to[2] = from[0];
        if (channels == 4) {
            to[3] = from[3];
        }
    }
    return image;
}

Bytes encode_png(const Image& image, int level, double& microseconds) {
    stx_png::PngEncodeInput input{};
    input.width = image.width;
    input.height = image.height;
    input.row_bytes = static_cast<std::uint64_t>(image.width) * 4U;
    input.pixels = image.rgba;
    input.alpha = image.opaque ? stx_png::PngSourceAlpha::opaque : stx_png::PngSourceAlpha::straight;
    input.compression_level = level;
    Bytes file;
    Clock::time_point start = Clock::now();
    bool encoded = stx_png::encode_png(input, file);
    microseconds = microseconds_since(start);
    if (!encoded) {
        file.clear();
    }
    return file;
}

struct Row final {
    std::string category;
    std::string name;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    unsigned channels = 0;
    std::size_t png6_bytes = 0;
    std::size_t png9_bytes = 0;
    std::size_t cng_bytes = 0;
    std::uint32_t cng_crc = 0;
    double png6_encode_us = 0;
    double cng_encode_us = 0;
    double png_decode_us = 0;
    double cng_decode_1_us = 0;
    double cng_decode_n_us = 0;
    unsigned rounds = 0;
    std::size_t encoder_workspace = 0;
    std::size_t decoder_workspace = 0;
};

} // namespace

int main(int argc, char** argv) {
    if (argc != 4) {
        std::fprintf(stderr, "usage: bench <corpus dir> <output tsv> <threads>\n");
        return EXIT_FAILURE;
    }
    std::filesystem::path corpus = argv[1];
    std::uint32_t threads = static_cast<std::uint32_t>(std::atoi(argv[3]));
    std::vector<std::filesystem::path> files;
    for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(corpus)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path());
        }
    }
    std::sort(files.begin(), files.end());
    std::unique_ptr<PngReader> reader = std::make_unique<PngReader>();
    std::unique_ptr<stx_cng::CngEncoder> encoder = std::make_unique<stx_cng::CngEncoder>();
    std::unique_ptr<stx_cng::CngDecoder> decoder = std::make_unique<stx_cng::CngDecoder>();
    std::FILE* out = std::fopen(argv[2], "w");
    if (out == nullptr) {
        return EXIT_FAILURE;
    }
    std::fprintf(out, "category\tname\twidth\theight\tchannels\tpng6_bytes\tpng9_bytes\tcng_bytes\tcng_crc\tpng6_encode_us\t"
                      "cng_encode_us_t%u\tpng_decode_us\tcng_decode_us_t1\tcng_decode_us_t%u\trounds\tcng_encoder_workspace\t"
                      "cng_decoder_workspace\n",
                 threads, threads);
    unsigned failures = 0;
    for (const std::filesystem::path& path : files) {
        Row row;
        row.category = path.parent_path().filename().string();
        row.name = path.filename().string();
        Bytes source = read_file(path);
        stx_png::PngSampleLayout layout{};
        if (!decode_png(*reader, source, layout)) {
            std::fprintf(out, "%s\t%s\tnot_png\n", row.category.c_str(), row.name.c_str());
            continue;
        }
        Image image = image_of(*reader, layout);
        row.width = image.width;
        row.height = image.height;
        row.channels = image.opaque ? 3 : 4;
        Bytes png6 = encode_png(image, 6, row.png6_encode_us);
        double unused = 0;
        Bytes png9 = encode_png(image, 9, unused);
        row.png6_bytes = png6.size();
        row.png9_bytes = png9.size();
        stx_cng::CngImage shape{image.width, image.height, image.opaque ? stx_cng::CngChannels::bgr : stx_cng::CngChannels::bgra};
        std::uint64_t row_bytes = static_cast<std::uint64_t>(image.width) * row.channels;
        Bytes file(static_cast<std::size_t>(stx_cng::cng_encoded_bound(shape)));
        stx_cng::CngEncodeInput input{shape, row_bytes, image.samples, threads};
        Clock::time_point start = Clock::now();
        stx_cng::CngEncodeResult encoded = (*encoder).encode(input, file);
        row.cng_encode_us = microseconds_since(start);
        file.resize(encoded.encoded_bytes);
        row.cng_bytes = file.size();
        row.cng_crc = stx_zlib::crc32(file);
        Bytes decoded(image.samples.size());
        stx_cng::CngDecodeResult check = (*decoder).decode(file, decoded, row_bytes, threads);
        if (encoded.error != stx_cng::CngError::none || check.error != stx_cng::CngError::none || decoded != image.samples) {
            std::fprintf(stderr, "FAIL: %s does not round trip\n", row.name.c_str());
            ++failures;
            continue;
        }
        stx_png::PngSampleLayout again{};
        row.png_decode_us = 1e300;
        row.cng_decode_1_us = 1e300;
        row.cng_decode_n_us = 1e300;
        double total = 0;
        while (row.rounds < 200 && (row.rounds < 3 || total < 300000.0)) {
            start = Clock::now();
            bool png_ok = decode_png(*reader, png6, again);
            double png_us = microseconds_since(start);
            start = Clock::now();
            stx_cng::CngDecodeResult one = (*decoder).decode(file, decoded, row_bytes, 1);
            double one_us = microseconds_since(start);
            start = Clock::now();
            stx_cng::CngDecodeResult many = (*decoder).decode(file, decoded, row_bytes, threads);
            double many_us = microseconds_since(start);
            if (!png_ok || one.error != stx_cng::CngError::none || many.error != stx_cng::CngError::none) {
                ++failures;
                break;
            }
            row.png_decode_us = std::min(row.png_decode_us, png_us);
            row.cng_decode_1_us = std::min(row.cng_decode_1_us, one_us);
            row.cng_decode_n_us = std::min(row.cng_decode_n_us, many_us);
            total += png_us + one_us + many_us;
            ++row.rounds;
        }
        row.encoder_workspace = (*encoder).workspace_bytes();
        row.decoder_workspace = (*decoder).workspace_bytes();
        std::fprintf(out, "%s\t%s\t%u\t%u\t%u\t%zu\t%zu\t%zu\t%08x\t%.1f\t%.1f\t%.1f\t%.1f\t%.1f\t%u\t%zu\t%zu\n", row.category.c_str(),
                     row.name.c_str(), row.width, row.height, row.channels, row.png6_bytes, row.png9_bytes, row.cng_bytes, row.cng_crc,
                     row.png6_encode_us, row.cng_encode_us, row.png_decode_us, row.cng_decode_1_us, row.cng_decode_n_us, row.rounds,
                     row.encoder_workspace, row.decoder_workspace);
        std::fflush(out);
    }
    std::fclose(out);
    std::printf("bench: %u failure(s)\n", failures);
    return failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
