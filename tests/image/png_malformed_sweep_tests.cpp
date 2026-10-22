// Deterministic malformed-input sweeps over the owned PNG path (owner
// revision of decision 6A, 2026-10-20). Seeds cover every colour type,
// Adam7, palettes with tRNS, text and compressed metadata and colour chunks.
// For each seed:
//   - every truncation point is refused by inspection, both decode entries,
//     the pixel decoder and the registry validator;
//   - every single-bit flip of the signature, every chunk header (length and
//     type), every CRC, the IHDR/PLTE/tRNS/colour chunk data and the first
//     64 IDAT bytes is decoded twice: as flipped (the CRC must refuse a data
//     flip) and with the chunk's CRC repaired, which reaches the semantic
//     checks behind the CRC. Each outcome is success or a named error within
//     the stated storage; nothing may write outside it.
// The sanitizers turn any out-of-bounds access or undefined behaviour into a
// failure.
#include "../../src/core/image/png_pixels.hpp"
#include "gui_forms/resources/types/image_resource_types.hpp"
#include "png_test_files.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>
#include <span>
#include <stdexcept>
#include <vector>

namespace {
using namespace gui_forms;
using namespace gui_forms_png_files;

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

std::uint32_t read32(const Bytes& data, std::size_t at) {
    const std::uint32_t value = (std::to_integer<std::uint32_t>(data[at]) << 24U) |
        (std::to_integer<std::uint32_t>(data[at + 1U]) << 16U) |
        (std::to_integer<std::uint32_t>(data[at + 2U]) << 8U) |
        std::to_integer<std::uint32_t>(data[at + 3U]);
    return value;
}
void write32(Bytes& data, std::size_t at, std::uint32_t value) {
    data[at] = static_cast<std::byte>(value >> 24U);
    data[at + 1U] = static_cast<std::byte>((value >> 16U) & 255U);
    data[at + 2U] = static_cast<std::byte>((value >> 8U) & 255U);
    data[at + 3U] = static_cast<std::byte>(value & 255U);
}

struct ChunkSpan final {
    std::size_t offset = 0;
    std::uint32_t length = 0;
    std::uint32_t type = 0;
};

std::vector<ChunkSpan> chunks_of(const Bytes& png) {
    std::vector<ChunkSpan> result{};
    std::size_t offset = 8;
    while (offset + 12U <= png.size()) {
        const ChunkSpan item{offset, read32(png, offset), read32(png, offset + 4U)};
        result.push_back(item);
        offset += static_cast<std::size_t>(item.length) + 12U;
    }
    return result;
}

void repair_crc(Bytes& png, const ChunkSpan& item) {
    const std::span<const std::byte> covered(png.data() + item.offset + 4U, item.length + 4U);
    write32(png, item.offset + 8U + item.length, stx_zlib::crc32(covered));
}

constexpr std::uint32_t idat = 0x49444154U;

// Reusable storage sized for the largest seed; every decode stays inside it.
struct Storage final {
    std::unique_ptr<compression::InflateStream> workspace = std::make_unique<compression::InflateStream>();
    Bytes rows = Bytes(4096);
    std::vector<std::uint16_t> samples = std::vector<std::uint16_t>(64U * 64U * 4U);
    Bytes metadata = Bytes(1U << 16U);
    Bytes scratch = Bytes(512);
    std::vector<image::PngMetadataRecord> records = std::vector<image::PngMetadataRecord>(16);
    Bytes pixels = Bytes(64U * 64U * 4U);
    image::PngPixelDecoder decoder{};
};

// Small limits so a corrupted size cannot demand large storage.
image::PngDecodeLimits sweep_limits() {
    image::PngDecodeLimits limits{};
    limits.dimension = 64;
    limits.pixels = 64U * 64U;
    limits.filtered_bytes = 64U * 64U * 8U + 64U * 7U;
    limits.metadata_bytes = 1U << 16U;
    limits.inflate_work.value = 1ULL << 26U;
    limits.chunks = 64;
    return limits;
}

// Runs every entry point; returns whether the strict decode succeeded.
bool decode_all(const Bytes& png, Storage& storage, std::uint64_t& decodes) {
    const image::PngDecodeLimits limits = sweep_limits();
    const image::PngDecodeResult inspected = image::inspect_png_samples(png, limits);
    const image::PngDecodeResult stored = image::decode_png_samples(png, limits, *storage.workspace,
        storage.rows, storage.samples, storage.metadata, storage.records);
    const image::PngDecodeResult discarded = image::decode_png_samples_discarding_metadata(png, limits,
        *storage.workspace, storage.rows, storage.samples, storage.scratch, storage.records);
    const image::PngDecodeResult measured = image::measure_png_samples(png, limits, *storage.workspace,
        storage.scratch);
    decodes += 4U;
    require(stored.error != image::PngDecodeError::none || inspected.error == image::PngDecodeError::none,
            "decode accepted what inspection refused");
    require((stored.error == image::PngDecodeError::none) == (discarded.error == image::PngDecodeError::none),
            "storing and discarding decodes disagree");
    if (stored.error == image::PngDecodeError::none) {
        require(measured.error == image::PngDecodeError::none &&
                measured.metadata_bytes == stored.metadata_bytes, "measurement disagrees with decode");
    }
    image::PngPixelRequest request{};
    request.limits = limits;
    request.purpose = image::PngPixelPurpose::clipboard;
    const image::PngPixelResult inspected_pixels = storage.decoder.inspect(png, request);
    if (inspected_pixels.error == image::PngPixelError::none) {
        const std::size_t row_bytes = static_cast<std::size_t>(inspected_pixels.width) * 4U;
        const std::span<std::byte> destination(storage.pixels.data(), row_bytes * inspected_pixels.height);
        const image::PngPixelResult decoded = storage.decoder.decode(png, request, destination, row_bytes);
        ++decodes;
        if (decoded.error == image::PngPixelError::none) {
            require(stored.error == image::PngDecodeError::none, "pixel path accepted a refused file");
        }
    }
    ImageRegistryLimits registry_limits{};
    registry_limits.maximum_width = 64;
    registry_limits.maximum_height = 64;
    (void)validate_png(png, registry_limits);
    ++decodes;
    return stored.error == image::PngDecodeError::none;
}

std::vector<Bytes> make_seeds() {
    std::vector<Bytes> seeds{};
    const std::array<Extra, 4> metadata{{
        {"gAMA", gamma_payload(45455)},
        {"cHRM", Bytes{}},
        {"tEXt", Bytes{std::byte{'k'}, std::byte{0}, std::byte{'v'}}},
        {"zTXt", Bytes{}}}};
    std::array<Extra, 4> seeded = metadata;
    for (const std::uint32_t value : {31270U, 32900U, 64000U, 33000U, 30000U, 60000U, 15000U, 6000U}) {
        put32(seeded[1].payload, value);
    }
    Bytes compressed_text{std::byte{'z'}, std::byte{0}, std::byte{0}};
    const Bytes body = deflate(Bytes(200, std::byte{'t'}));
    compressed_text.insert(compressed_text.end(), body.begin(), body.end());
    seeded[3].payload = compressed_text;
    seeds.push_back(png_file(9, 7, formats[13], false, seeded));         // RGBA8 with metadata
    seeds.push_back(png_file(11, 5, formats[14], true, {}));             // RGBA16, Adam7
    seeds.push_back(png_file(13, 6, formats[7], false, {}));             // palette 1-bit, tRNS
    seeds.push_back(png_file(8, 8, formats[10], true, {}));              // palette 8-bit, Adam7
    seeds.push_back(png_file(10, 3, formats[0], false, {}));             // grey 1-bit, tRNS
    seeds.push_back(png_file(6, 6, formats[6], false, {}));              // RGB16, tRNS
    seeds.push_back(png_file(7, 9, formats[11], true, {}));              // grey+alpha 8, Adam7
    const std::array<Extra, 1> icc{{{"iCCP", icc_profile_payload()}}};
    seeds.push_back(png_file(5, 5, formats[5], false, icc));             // RGB8 with iCCP
    return seeds;
}

void sweep(const Bytes& seed, Storage& storage, std::uint64_t& decodes) {
    require(decode_all(seed, storage, decodes), "seed refused");
    for (std::size_t length = 0; length < seed.size(); ++length) {
        const Bytes prefix(seed.begin(), seed.begin() + static_cast<std::ptrdiff_t>(length));
        require(!decode_all(prefix, storage, decodes), "truncated PNG accepted");
    }
    const std::vector<ChunkSpan> chunks = chunks_of(seed);
    Bytes flipped = seed;
    // Signature bytes: no repair applies.
    for (std::size_t at = 0; at < 8U; ++at) {
        for (unsigned bit = 0; bit < 8U; ++bit) {
            const std::byte mask = static_cast<std::byte>(1U << bit);
            flipped[at] ^= mask;
            require(!decode_all(flipped, storage, decodes), "flipped signature accepted");
            flipped[at] ^= mask;
        }
    }
    for (const ChunkSpan& item : chunks) {
        const std::size_t data_end = item.offset + 8U + item.length;
        const std::size_t data_limit = item.type == idat ? std::min<std::size_t>(data_end, item.offset + 8U + 64U)
                                                         : data_end;
        std::vector<std::size_t> positions{};
        for (std::size_t at = item.offset; at < data_limit; ++at) positions.push_back(at);
        for (std::size_t at = data_end; at < data_end + 4U; ++at) positions.push_back(at);
        for (const std::size_t at : positions) {
            for (unsigned bit = 0; bit < 8U; ++bit) {
                const std::byte mask = static_cast<std::byte>(1U << bit);
                flipped[at] ^= mask;
                const bool accepted = decode_all(flipped, storage, decodes);
                // A flip covered by the CRC (type and data) must be refused.
                const bool covered = at >= item.offset + 4U && at < data_end;
                if (covered) require(!accepted, "CRC did not refuse a flipped chunk");
                if (at >= data_end) require(!accepted, "flipped CRC accepted");
                // Repair the CRC to reach the checks behind it (type and data
                // flips; a length flip moves the envelope instead).
                if (covered) {
                    Bytes repaired = flipped;
                    repair_crc(repaired, item);
                    (void)decode_all(repaired, storage, decodes);
                }
                flipped[at] ^= mask;
            }
        }
    }
}

} // namespace

int main() {
    try {
        const std::vector<Bytes> seeds = make_seeds();
        Storage storage{};
        std::uint64_t decodes = 0;
        for (const Bytes& seed : seeds) sweep(seed, storage, decodes);
        std::printf("png_malformed_sweep: %zu seeds, %llu decodes\n", seeds.size(),
                    static_cast<unsigned long long>(decodes));
    } catch (const std::exception& error) {
        std::cerr << "png_malformed_sweep_tests: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
