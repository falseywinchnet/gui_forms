#include "../../src/core/image/png_decoder.hpp"
#include "../../src/core/image/png_writer.hpp"
#include "png_dynamic_fixture.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <vector>

#if defined(GUI_FORMS_PNG_ORACLE)
extern "C" int gui_forms_png_oracle(const unsigned char*, std::size_t,
    unsigned char*, std::size_t, std::uint32_t, std::uint32_t);
#endif

namespace {
using namespace gui_forms;
using Bytes = std::vector<std::byte>;

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}
void put32(Bytes& data, std::uint32_t value) {
    for (unsigned shift = 32; shift != 0; shift -= 8) {
        data.push_back(static_cast<std::byte>((value >> (shift - 8)) & 255U));
    }
}
void chunk(Bytes& data, const char* name, const Bytes& payload) {
    put32(data, static_cast<std::uint32_t>(payload.size()));
    const std::size_t begin = data.size();
    for (unsigned index = 0; index < 4; ++index) data.push_back(static_cast<std::byte>(name[index]));
    data.insert(data.end(), payload.begin(), payload.end());
    const std::span<const std::byte> checked(data.data() + begin, payload.size() + 4);
    put32(data, image::png_crc32(checked));
}
Bytes stored(const Bytes& raw) {
    Bytes data{std::byte{0x78}, std::byte{0x01}};
    std::size_t offset = 0;
    do {
        const std::size_t count = std::min<std::size_t>(65535, raw.size() - offset);
        const bool final = count == raw.size() - offset;
        data.push_back(final ? std::byte{1} : std::byte{0});
        data.push_back(static_cast<std::byte>(count & 255));
        data.push_back(static_cast<std::byte>(count >> 8));
        data.push_back(static_cast<std::byte>((~count) & 255));
        data.push_back(static_cast<std::byte>(((~count) >> 8) & 255));
        data.insert(data.end(), raw.begin() + static_cast<std::ptrdiff_t>(offset),
                    raw.begin() + static_cast<std::ptrdiff_t>(offset + count));
        offset += count;
    } while (offset < raw.size());
    put32(data, image::png_adler32(raw));
    return data;
}
struct Format final { unsigned color = 0; unsigned depth = 0; unsigned channels = 0; };
constexpr std::array<Format, 15> formats{{
    {0,1,1}, {0,2,1}, {0,4,1}, {0,8,1}, {0,16,1},
    {2,8,3}, {2,16,3}, {3,1,1}, {3,2,1}, {3,4,1}, {3,8,1},
    {4,8,2}, {4,16,2}, {6,8,4}, {6,16,4}}};
unsigned value_at(unsigned x, unsigned y, unsigned channel, const Format& format) {
    const unsigned value = (x * 73 + y * 139 + channel * 17) & ((1U << format.depth) - 1);
    return value;
}
unsigned reference_predictor(unsigned filter, unsigned a, unsigned b, unsigned c) {
    if (filter == 0) return 0;
    if (filter == 1) return a;
    if (filter == 2) return b;
    if (filter == 3) return (a + b) / 2;
    // Independent form of Paeth distances: b-c, a-c, a+b-2c.
    const int pa = std::abs(static_cast<int>(b) - static_cast<int>(c));
    const int pb = std::abs(static_cast<int>(a) - static_cast<int>(c));
    const int pc = std::abs(static_cast<int>(a) + static_cast<int>(b) - 2 * static_cast<int>(c));
    if (pa <= pb && pa <= pc) return a;
    if (pb <= pc) return b;
    return c;
}
Bytes filtered(unsigned width, unsigned height, const Format& format, bool interlaced, unsigned filter) {
    constexpr std::array<unsigned, 7> sx{0,4,0,2,0,1,0};
    constexpr std::array<unsigned, 7> sy{0,0,4,0,2,0,1};
    constexpr std::array<unsigned, 7> dx{8,8,4,4,2,2,1};
    constexpr std::array<unsigned, 7> dy{8,8,8,4,4,2,2};
    Bytes raw{};
    const unsigned passes = interlaced ? 7 : 1;
    const unsigned stride = std::max(1U, format.channels * format.depth / 8);
    for (unsigned pass = 0; pass < passes; ++pass) {
        const unsigned start_x = interlaced ? sx[pass] : 0;
        const unsigned start_y = interlaced ? sy[pass] : 0;
        const unsigned step_x = interlaced ? dx[pass] : 1;
        const unsigned step_y = interlaced ? dy[pass] : 1;
        if (start_x >= width || start_y >= height) continue;
        const unsigned count = (width - start_x + step_x - 1) / step_x;
        const std::size_t row_bytes = (count * format.channels * format.depth + 7) / 8;
        Bytes previous(row_bytes, std::byte{0});
        for (unsigned y = start_y; y < height; y += step_y) {
            Bytes row(row_bytes, std::byte{0});
            std::size_t sample_index = 0;
            for (unsigned x = start_x; x < width; x += step_x) {
                for (unsigned channel = 0; channel < format.channels; ++channel) {
                    const unsigned value = value_at(x, y, channel, format);
                    if (format.depth == 16) {
                        row[sample_index * 2] = static_cast<std::byte>(value >> 8);
                        row[sample_index * 2 + 1] = static_cast<std::byte>(value & 255);
                    } else if (format.depth == 8) {
                        row[sample_index] = static_cast<std::byte>(value);
                    } else {
                        const std::size_t bit = sample_index * format.depth;
                        const unsigned shift = 8 - format.depth - static_cast<unsigned>(bit % 8);
                        row[bit / 8] |= static_cast<std::byte>(value << shift);
                    }
                    ++sample_index;
                }
            }
            raw.push_back(static_cast<std::byte>(filter));
            for (std::size_t index = 0; index < row_bytes; ++index) {
                const unsigned a = index < stride ? 0 : std::to_integer<unsigned>(row[index - stride]);
                const unsigned b = std::to_integer<unsigned>(previous[index]);
                const unsigned c = index < stride ? 0 : std::to_integer<unsigned>(previous[index - stride]);
                const unsigned predictor = reference_predictor(filter, a, b, c);
                raw.push_back(static_cast<std::byte>((std::to_integer<unsigned>(row[index]) - predictor) & 255U));
            }
            previous = row;
        }
    }
    return raw;
}
Bytes header(unsigned width, unsigned height, const Format& format, bool interlaced) {
    Bytes png{std::byte{137},std::byte{80},std::byte{78},std::byte{71},
              std::byte{13},std::byte{10},std::byte{26},std::byte{10}};
    Bytes ihdr{};
    put32(ihdr, width); put32(ihdr, height);
    ihdr.push_back(static_cast<std::byte>(format.depth));
    ihdr.push_back(static_cast<std::byte>(format.color));
    ihdr.push_back(std::byte{0}); ihdr.push_back(std::byte{0});
    ihdr.push_back(interlaced ? std::byte{1} : std::byte{0});
    chunk(png, "IHDR", ihdr);
    return png;
}
Bytes fixture(unsigned width, unsigned height, const Format& format, bool interlaced,
              unsigned filter, std::size_t split) {
    Bytes png = header(width, height, format, interlaced);
    if (format.color == 3) {
        Bytes palette{};
        Bytes alpha{};
        for (unsigned index = 0; index < (1U << format.depth); ++index) {
            palette.push_back(static_cast<std::byte>(index));
            palette.push_back(static_cast<std::byte>(255 - index));
            palette.push_back(static_cast<std::byte>((index * 17) & 255));
            alpha.push_back(static_cast<std::byte>(index));
        }
        chunk(png, "PLTE", palette); chunk(png, "tRNS", alpha);
    } else if (format.color == 0) {
        chunk(png, "tRNS", Bytes{std::byte{0}, std::byte{0}});
    } else if (format.color == 2) {
        chunk(png, "tRNS", Bytes{std::byte{0}, std::byte{0}, std::byte{0}, std::byte{17}, std::byte{0}, std::byte{34}});
    }
    const Bytes raw = filtered(width, height, format, interlaced, filter);
    const Bytes compressed = stored(raw);
    for (std::size_t offset = 0; offset < compressed.size(); offset += split) {
        const std::size_t count = std::min(split, compressed.size() - offset);
        const Bytes payload(compressed.begin() + static_cast<std::ptrdiff_t>(offset),
                            compressed.begin() + static_cast<std::ptrdiff_t>(offset + count));
        chunk(png, "IDAT", payload);
    }
    chunk(png, "IDAT", {}); // Empty final IDAT must not become a spurious EOF.
    chunk(png, "IEND", {});
    return png;
}
struct Decoded final {
    image::PngDecodeResult result{};
    std::vector<std::uint16_t> pixels{};
    Bytes metadata{};
    std::vector<image::PngMetadataRecord> records{};
};
Decoded decode(const Bytes& png, const image::PngDecodeLimits& limits = {}) {
    Decoded decoded{};
    decoded.result = image::inspect_png_samples(png, limits);
    if (decoded.result.error != image::PngDecodeError::none) return decoded;
    Bytes rows(decoded.result.layout.row_storage_bytes, std::byte{0});
    decoded.pixels.resize(decoded.result.layout.rgba_samples);
    decoded.records.resize(decoded.result.layout.metadata_records);
    decoded.metadata.resize(static_cast<std::size_t>(limits.metadata_bytes));
    std::unique_ptr<compression::InflateStream> workspace = std::make_unique<compression::InflateStream>();
    decoded.result = image::decode_png_samples(png, limits, *workspace, rows,
        decoded.pixels, decoded.metadata, decoded.records);
    return decoded;
}
void verify_pixels(const Decoded& decoded, unsigned width, unsigned height, const Format& format) {
    require(decoded.result.error == image::PngDecodeError::none, "valid fixture rejected");
    const unsigned scale = 65535 / ((1U << format.depth) - 1);
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            const unsigned first = value_at(x,y,0,format);
            std::array<unsigned,4> expected{first * scale, first * scale, first * scale, 65535};
            if (format.color == 3) {
                expected = {first * 257, (255 - first) * 257, ((first * 17) & 255) * 257, first * 257};
            }
            if (format.color == 2 || format.color == 6) {
                expected[1] = value_at(x,y,1,format) * scale;
                expected[2] = value_at(x,y,2,format) * scale;
            }
            if (format.color == 0 && first == 0) expected[3] = 0;
            if (format.color == 2 && first == 0 && value_at(x,y,1,format) == 17 && value_at(x,y,2,format) == 34) expected[3] = 0;
            if (format.color == 4 || format.color == 6) expected[3] = value_at(x,y,format.channels - 1,format) * scale;
            for (unsigned channel = 0; channel < 4; ++channel) {
                require(decoded.pixels[(y * width + x) * 4 + channel] == expected[channel], "sample mismatch");
            }
        }
    }
}
void compare_oracle(const Bytes& png, const Decoded& decoded) {
#if defined(GUI_FORMS_PNG_ORACLE)
    std::vector<unsigned char> output(decoded.pixels.size() * 2);
    const unsigned char* input = reinterpret_cast<const unsigned char*>(png.data());
    const int accepted = gui_forms_png_oracle(input, png.size(), output.data(), output.size(),
        decoded.result.layout.width, decoded.result.layout.height);
    require(accepted != 0, "pinned libpng rejected matrix fixture");
    for (std::size_t index = 0; index < decoded.pixels.size(); ++index) {
        const unsigned expected = static_cast<unsigned>(output[index * 2]) * 256 + output[index * 2 + 1];
        require(decoded.pixels[index] == expected, "pinned libpng sample mismatch");
    }
#else
    (void)png;
    (void)decoded;
#endif
}
void matrix() {
    const image::PngDecodeLimits limits{.metadata_bytes = 4096};
    constexpr std::array<unsigned, 10> widths{1,2,3,4,5,7,8,9,17,33};
    std::size_t cases = 0;
    for (const Format& format : formats) {
        for (bool interlaced : {false, true}) {
            for (unsigned width : widths) {
                for (unsigned height : {1U, 2U, 3U, 5U, 8U, 9U}) {
                    for (unsigned filter = 0; filter < 5; ++filter) {
                        const Bytes png = fixture(width, height, format, interlaced, filter, 37);
                        const Decoded decoded = decode(png, limits);
                        verify_pixels(decoded, width, height, format);
                        compare_oracle(png, decoded);
                        ++cases;
                    }
                }
            }
        }
    }
    for (const Format& format : formats) {
        if (format.color != 0 && format.color != 2) continue;
        Bytes png = header(9, 9, format, true);
        chunk(png, "IDAT", stored(filtered(9, 9, format, true, 4)));
        chunk(png, "IEND", {});
        const Decoded opaque = decode(png, limits);
        require(opaque.result.error == image::PngDecodeError::none, "opaque selector failed");
        for (std::size_t index = 3; index < opaque.pixels.size(); index += 4) {
            require(opaque.pixels[index] == 65535, "opaque alpha changed");
        }
        compare_oracle(png, opaque);
    }
    const Bytes split = fixture(9, 9, formats[14], true, 4, 1);
    const Decoded decoded = decode(split, limits);
    verify_pixels(decoded, 9, 9, formats[14]);
    Bytes dynamic = header(257, 63, formats[13], false);
    for (std::byte value : gui_forms_png_test::dynamic_idat) {
        chunk(dynamic, "IDAT", Bytes{value});
    }
    chunk(dynamic, "IEND", {});
    const Decoded dynamic_pixels = decode(dynamic, limits);
    verify_pixels(dynamic_pixels, 257, 63, formats[13]);
    compare_oracle(dynamic, dynamic_pixels);
    std::cout << "Format/filter/Adam7 matrix: " << cases << " cases, plus byte-split IDAT\n";
}
std::uint32_t reference_crc(std::span<const std::byte> bytes) {
    std::uint32_t crc = 0xffffffffU;
    for (std::byte value : bytes) {
        crc ^= std::to_integer<std::uint32_t>(value);
        for (unsigned bit = 0; bit < 8; ++bit) {
            const bool low_bit = (crc & 1U) != 0;
            crc >>= 1U;
            if (low_bit) crc ^= 0xedb88320U;
        }
    }
    crc ^= 0xffffffffU;
    return crc;
}
void checksum_boundaries() {
    Bytes storage(65551);
    std::uint32_t random = 1234567;
    for (std::byte& value : storage) {
        random = random * 1664525U + 1013904223U;
        value = static_cast<std::byte>(random >> 24U);
    }
    for (std::size_t offset = 0; offset < 16; ++offset) {
        for (std::size_t length = 0; length <= 257; ++length) {
            const std::span<const std::byte> bytes(storage.data() + offset, length);
            require(image::png_crc32(bytes) == reference_crc(bytes), "bulk CRC alignment/tail mismatch");
        }
        const std::span<const std::byte> long_input(storage.data() + offset, 65535);
        require(image::png_crc32(long_input) == reference_crc(long_input), "bulk CRC long input mismatch");
    }
    const Bytes check{std::byte{49},std::byte{50},std::byte{51},std::byte{52},std::byte{53},
                      std::byte{54},std::byte{55},std::byte{56},std::byte{57}};
    require(image::png_crc32(check) == 0xcbf43926U, "IEEE CRC check vector mismatch");
}
void malformed_and_limits() {
    const Bytes valid = fixture(3, 2, formats[13], false, 0, 1000);
    for (std::size_t size = 0; size < valid.size(); ++size) {
        const Bytes cut(valid.begin(), valid.begin() + static_cast<std::ptrdiff_t>(size));
        const Decoded decoded = decode(cut);
        require(decoded.result.error != image::PngDecodeError::none, "truncation accepted");
    }
    Bytes corrupt = valid;
    corrupt[29] ^= std::byte{1};
    require(decode(corrupt).result.error == image::PngDecodeError::crc_mismatch, "bad CRC accepted");
    corrupt = valid; corrupt.push_back(std::byte{0});
    require(decode(corrupt).result.error == image::PngDecodeError::trailing_data, "trailing file byte accepted");
    image::PngDecodeLimits limits{};
    const image::PngDecodeResult layout = image::inspect_png_samples(valid, limits);
    limits.filtered_bytes = layout.layout.filtered_bytes - 1;
    require(decode(valid, limits).result.error == image::PngDecodeError::resource_limit, "filtered bound ignored");
    limits = {}; limits.pixels = 5;
    require(decode(valid, limits).result.error == image::PngDecodeError::resource_limit, "pixel bound ignored");
    limits = {}; limits.inflate_work.value = 1;
    require(decode(valid, limits).result.inflate_error == compression::InflateError::work_limit_exceeded, "work bound ignored");
    limits = {}; limits.encoded_bytes = valid.size() - 1;
    require(decode(valid, limits).result.error == image::PngDecodeError::resource_limit, "encoded bound ignored");
    limits = {}; limits.chunks = 1;
    require(decode(valid, limits).result.error == image::PngDecodeError::resource_limit, "chunk bound ignored");
    for (unsigned mode = 0; mode < 6; ++mode) {
        Bytes png = header(1, 1, formats[13], false);
        Bytes raw{std::byte{0},std::byte{1},std::byte{2},std::byte{3},std::byte{4}};
        if (mode == 0) raw[0] = std::byte{5};
        if (mode == 1) raw.pop_back();
        if (mode == 2) raw.push_back(std::byte{0});
        Bytes stream = stored(raw);
        if (mode == 3) stream.push_back(std::byte{0});
        if (mode == 4) stream.back() ^= std::byte{1};
        if (mode == 5) stream.pop_back();
        chunk(png, "IDAT", stream); chunk(png, "IEND", {});
        require(decode(png).result.error != image::PngDecodeError::none, "malformed image stream accepted");
    }
    Bytes png = header(1, 1, formats[10], false);
    chunk(png, "PLTE", Bytes{std::byte{0},std::byte{0},std::byte{0}});
    chunk(png, "IDAT", stored(Bytes{std::byte{0},std::byte{1}}));
    chunk(png, "IEND", {});
    require(decode(png).result.error == image::PngDecodeError::invalid_palette_index, "palette overrun accepted");
}
void metadata_and_storage() {
    Bytes png = header(1, 1, formats[13], false);
    const Bytes content(64, std::byte{65});
    const Bytes compressed = stored(content);
    Bytes field{std::byte{65},std::byte{0},std::byte{0}};
    field.insert(field.end(), compressed.begin(), compressed.end());
    chunk(png, "iCCP", field);
    chunk(png, "zTXt", field);
    Bytes international{std::byte{65},std::byte{0},std::byte{1},std::byte{0},std::byte{0},std::byte{0}};
    international.insert(international.end(), compressed.begin(), compressed.end());
    chunk(png, "iTXt", international);
    chunk(png, "IDAT", stored(Bytes{std::byte{0},std::byte{1},std::byte{2},std::byte{3},std::byte{4}}));
    chunk(png, "IEND", {});
    const Decoded decoded = decode(png);
    require(decoded.result.error == image::PngDecodeError::none, "compressed metadata rejected");
    require(decoded.result.metadata_bytes == 192 && decoded.records.size() == 3, "metadata accounting mismatch");
    for (std::size_t index = 0; index < 192; ++index) require(decoded.metadata[index] == std::byte{65}, "metadata byte mismatch");
    image::PngDecodeLimits limits{};
    limits.inflate_work.value = decoded.result.inflate_work.value - 1;
    require(decode(png, limits).result.inflate_error == compression::InflateError::work_limit_exceeded, "metadata reset restored work budget");
    Bytes bomb = header(1,1,formats[13],false);
    // Python zlib 1.2.12, level 9: 4096 bytes of 'A'; oracle fixture only.
    const Bytes compact{std::byte{120},std::byte{218},std::byte{237},std::byte{193},std::byte{1},std::byte{13},std::byte{0},std::byte{0},std::byte{0},std::byte{194},std::byte{160},std::byte{108},std::byte{239},std::byte{95},std::byte{202},std::byte{30},std::byte{14},std::byte{40},std::byte{0},std::byte{0},std::byte{0},std::byte{224},std::byte{221},std::byte{0},std::byte{255},std::byte{173},std::byte{16},std::byte{61}};
    Bytes compressed_text{std::byte{65},std::byte{0},std::byte{0}};
    compressed_text.insert(compressed_text.end(), compact.begin(), compact.end());
    chunk(bomb, "zTXt", compressed_text);
    chunk(bomb, "zTXt", compressed_text);
    chunk(bomb, "IDAT", stored(Bytes{std::byte{0},std::byte{1},std::byte{2},std::byte{3},std::byte{4}}));
    chunk(bomb, "IEND", {});
    limits = {}; limits.metadata_bytes = 6000;
    require(decode(bomb, limits).result.inflate_error == compression::InflateError::output_limit_exceeded,
            "metadata reset restored output budget");
    Bytes policy_image = header(1, 1, formats[13], false);
    Bytes policy_field{std::byte{65}, std::byte{0}, std::byte{0}};
    const Bytes policy_stream = stored(Bytes(16, std::byte{65}));
    policy_field.insert(policy_field.end(), policy_stream.begin(), policy_stream.end());
    for (unsigned index = 0; index < 4; ++index) chunk(policy_image, "zTXt", policy_field);
    chunk(policy_image, "IDAT", stored(Bytes{std::byte{0},std::byte{1},std::byte{2},std::byte{3},std::byte{4}}));
    chunk(policy_image, "IEND", {});
    const Decoded policy_reference = decode(policy_image);
    require(policy_reference.result.error == image::PngDecodeError::none, "policy fixture failed");
    // Byte output may legally overwrite an external settings object's unsigned
    // fields. The active request's policy must already be a private value copy.
    struct AliasedPolicy final {
        image::PngDecodeLimits limits{};
        std::array<std::byte, 64> tail{};
    };
    AliasedPolicy policy{};
    policy.limits.inflate_work.value = policy_reference.result.inflate_work.value - 1;
    const std::span<AliasedPolicy> policy_objects(&policy, 1);
    const std::span<std::byte> policy_storage = std::as_writable_bytes(policy_objects);
    std::array<std::byte,10> policy_rows{};
    std::array<std::uint16_t,4> policy_pixels{};
    std::array<image::PngMetadataRecord,4> policy_records{};
    std::unique_ptr<compression::InflateStream> policy_workspace = std::make_unique<compression::InflateStream>();
    const image::PngDecodeResult policy_result = image::decode_png_samples(policy_image, policy.limits,
        *policy_workspace, policy_rows, policy_pixels, policy_storage, policy_records);
    require(policy_result.inflate_error == compression::InflateError::work_limit_exceeded,
            "output overwrote active decode policy");

    const image::PngDecodeResult inspection = image::inspect_png_samples(png, {});
    Bytes rows(inspection.layout.row_storage_bytes, std::byte{0});
    std::array<std::uint16_t,4> pixels{91,91,91,91};
    std::array<image::PngMetadataRecord,3> records{};
    Bytes output(192, std::byte{0});
    std::unique_ptr<compression::InflateStream> workspace = std::make_unique<compression::InflateStream>();
    const image::PngDecodeResult exact = image::decode_png_samples(png, {}, *workspace, rows, pixels, output, records);
    require(exact.error == image::PngDecodeError::none, "exact metadata buffer rejected");
    const image::PngDecodeResult short_rows = image::decode_png_samples(png, {}, *workspace,
        std::span<std::byte>(rows).first(rows.size() - 1), pixels, output, records);
    require(short_rows.error == image::PngDecodeError::insufficient_storage, "short rows accepted");
    const image::PngDecodeResult overlap = image::decode_png_samples(png, {}, *workspace, rows,
        pixels, rows, records);
    require(overlap.error == image::PngDecodeError::invalid_argument, "alias accepted");
}
} // namespace
void write_corpus(const std::filesystem::path& directory) {
    std::filesystem::create_directories(directory);
    unsigned index = 0;
    for (const Format& format : formats) {
        for (bool interlaced : {false, true}) {
            for (unsigned filter = 0; filter < 5; ++filter) {
                const Bytes png = fixture(17, 9, format, interlaced, filter, 37);
                const std::filesystem::path path = directory / ("matrix-" + std::to_string(index) + ".png");
                std::ofstream stream(path, std::ios::binary);
                stream.write(reinterpret_cast<const char*>(png.data()), static_cast<std::streamsize>(png.size()));
                require(static_cast<bool>(stream), "could not write corpus seed");
                ++index;
            }
        }
    }
}
void write_benchmarks(const std::filesystem::path& directory) {
    std::filesystem::create_directories(directory);
    for (unsigned format_index : {10U, 13U, 14U}) {
        const Format& format = formats[format_index];
        for (bool interlaced : {false, true}) {
            for (unsigned filter = 0; filter < 5; ++filter) {
                const Bytes png = fixture(256, 256, format, interlaced, filter, 65535);
                const std::string name = "color" + std::to_string(format.color) + "-depth" +
                    std::to_string(format.depth) + "-adam7-" + std::to_string(interlaced) +
                    "-filter" + std::to_string(filter) + ".png";
                const std::filesystem::path path = directory / name;
                std::ofstream stream(path, std::ios::binary);
                stream.write(reinterpret_cast<const char*>(png.data()), static_cast<std::streamsize>(png.size()));
                require(static_cast<bool>(stream), "could not write benchmark fixture");
            }
        }
    }
}
int main(int argc, char** argv) {
    try {
        matrix();
        checksum_boundaries();
        malformed_and_limits();
        metadata_and_storage();
        if (argc == 2) {
            const std::filesystem::path corpus(argv[1]);
            write_corpus(corpus);
            const std::filesystem::path workloads = corpus.parent_path() / "workloads";
            write_benchmarks(workloads);
        }
        std::cout << "Owned PNG sample decoder checks passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
