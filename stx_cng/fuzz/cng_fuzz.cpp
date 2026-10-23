// libFuzzer entry for stx_cng's public operations (hardening, 2026-10-22).
//
// The first input byte chooses the mode:
//   - even: the rest is a file. inspect_cng and decode must agree on the
//     header; an accepted file's samples must re-encode and decode exactly.
//   - odd: the rest describes an image (width, height, channels, stride
//     padding) and seeds its samples; encode then decode must be exact, and
//     a truncated copy of the file must be refused.
// One encoder and one decoder serve every input, so storage reuse across
// shapes is exercised too. CI runs it briefly per change (fuzz-smoke.yml).
// Built with STX_CNG_FUZZ_STANDALONE, main() replays a fixed pseudo-random
// set of mutated files and generated images instead, as an ordinary test.
#include "stx_cng/codec.hpp"
#include "stx_zlib/checksum.hpp"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <vector>

namespace {

// Decoded images above this many sample bytes are only inspected, which keeps
// each input within the smoke's memory limit.
constexpr std::size_t decode_sample_limit = std::size_t{16} << 20;

std::size_t accepted = 0;
std::size_t refused = 0;

void require(bool condition, const char* message) {
    if (condition) {
        return;
    }
    std::fprintf(stderr, "cng fuzz: %s\n", message);
    std::abort();
}

struct Codecs final {
    stx_cng::CngEncoder encoder;
    stx_cng::CngDecoder decoder;
};

Codecs& codecs() {
    static std::unique_ptr<Codecs> owner = std::make_unique<Codecs>();
    return *owner;
}

std::vector<std::byte> encode_exact(const stx_cng::CngEncodeInput& input) {
    std::vector<std::byte> file(stx_cng::cng_encoded_bound(input.image));
    stx_cng::CngEncodeResult result = codecs().encoder.encode(input, file);
    require(result.error == stx_cng::CngError::none, "admitted image encodes");
    require(result.encoded_bytes <= file.size(), "encoded within bound");
    file.resize(result.encoded_bytes);
    return file;
}

void decode_file(const std::uint8_t* data, std::size_t size) {
    std::span<const std::byte> file(reinterpret_cast<const std::byte*>(data), size);
    stx_cng::CngInspectResult inspected = stx_cng::inspect_cng(file);
    std::size_t sample_bytes = stx_cng::cng_sample_bytes(inspected.image);
    if (inspected.error != stx_cng::CngError::none || sample_bytes > decode_sample_limit) {
        ++refused;
        return;
    }
    std::vector<std::byte> samples(sample_bytes);
    stx_cng::CngDecodeResult decoded = codecs().decoder.decode(file, samples);
    if (decoded.error != stx_cng::CngError::none) {
        ++refused;
        return;
    }
    ++accepted;
    require(decoded.image.width == inspected.image.width && decoded.image.height == inspected.image.height
        && decoded.image.channels == inspected.image.channels, "probe and decode agree");
    std::uint64_t row_bytes = static_cast<std::uint64_t>(decoded.image.width) * static_cast<std::size_t>(decoded.image.channels);
    std::vector<std::byte> again = encode_exact(stx_cng::CngEncodeInput{decoded.image, row_bytes, samples});
    std::vector<std::byte> restored(sample_bytes);
    require(codecs().decoder.decode(again, restored).error == stx_cng::CngError::none && restored == samples,
            "re-encoded samples decode exactly");
}

void round_trip(const std::uint8_t* data, std::size_t size) {
    if (size < 5) {
        ++refused;
        return;
    }
    stx_cng::CngImage image{static_cast<std::uint32_t>(data[0] % 97 + 1), static_cast<std::uint32_t>(data[1] % 61 + 1),
                            (data[2] & 1U) != 0 ? stx_cng::CngChannels::rgba : stx_cng::CngChannels::rgb};
    std::size_t padding = data[2] >> 5;
    std::size_t row = static_cast<std::size_t>(image.width) * static_cast<std::size_t>(image.channels);
    std::size_t stride = row + padding;
    std::vector<std::byte> input(stride * (image.height - 1) + row);
    std::vector<std::byte> samples(row * image.height);
    // Samples repeat the remaining bytes, so short inputs give runs and
    // matches, and long ones give noise.
    const std::uint8_t* source = data + 3;
    std::size_t source_size = size - 3;
    for (std::size_t y = 0; y < image.height; ++y) {
        for (std::size_t x = 0; x < row; ++x) {
            std::byte value = static_cast<std::byte>(source[(y * row + x) % source_size]);
            input[y * stride + x] = value;
            samples[y * row + x] = value;
        }
    }
    std::vector<std::byte> file = encode_exact(stx_cng::CngEncodeInput{image, stride, input});
    std::vector<std::byte> restored(samples.size());
    require(codecs().decoder.decode(file, restored).error == stx_cng::CngError::none && restored == samples,
            "round trip is exact");
    std::size_t keep = static_cast<std::size_t>(data[3]) * file.size() / 256;
    std::vector<std::byte> truncated(file.begin(), file.begin() + static_cast<std::ptrdiff_t>(keep));
    require(codecs().decoder.decode(truncated, restored).error != stx_cng::CngError::none, "truncation refused");
    ++accepted;
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) {
    if (size == 0) {
        return 0;
    }
    if ((data[0] & 1U) == 0) {
        decode_file(data + 1, size - 1);
    } else {
        round_trip(data + 1, size - 1);
    }
    return 0;
}

#ifdef STX_CNG_FUZZ_STANDALONE
namespace {

std::uint32_t next(std::uint32_t& state) {
    state = state * 1664525U + 1013904223U;
    return state >> 8;
}

std::uint32_t read_u32(const std::vector<std::uint8_t>& bytes, std::size_t offset) {
    std::uint32_t value = 0;
    for (std::size_t index = 0; index < 4; ++index) {
        value = (value << 8) | bytes[offset + index];
    }
    return value;
}

// Recomputes the CRC of the chunk holding `position` (file offsets, after
// the 8-byte signature), so the edit reaches the band logic.
void repair_crc(std::vector<std::uint8_t>& file, std::size_t position) {
    std::size_t offset = 8;
    while (offset + 12 <= file.size()) {
        std::size_t length = read_u32(file, offset);
        if (length > file.size() - offset - 12) {
            return;
        }
        if (position >= offset + 8 && position < offset + 8 + length) {
            std::span<const std::byte> covered(reinterpret_cast<const std::byte*>(file.data() + offset + 4), 4 + length);
            std::uint32_t crc = stx_zlib::crc32(covered);
            for (std::size_t index = 0; index < 4; ++index) {
                file[offset + 8 + length + index] = static_cast<std::uint8_t>(crc >> (24 - 8 * index));
            }
            return;
        }
        offset += 12 + length;
    }
}

} // namespace

// Generated images, and their encodings with single-bit edits whose chunk
// CRC is repaired every other time, from a fixed seed.
int main() {
    std::uint32_t state = 20261022U;
    for (std::size_t round = 0; round < 400; ++round) {
        std::vector<std::uint8_t> input(1 + next(state) % 300);
        for (std::uint8_t& value : input) {
            value = static_cast<std::uint8_t>(next(state) >> (next(state) % 8));
        }
        input[0] = 1;
        LLVMFuzzerTestOneInput(input.data(), input.size());
        // The file just encoded from that image, mutated.
        stx_cng::CngImage image{static_cast<std::uint32_t>(input.size() > 1 ? input[1] % 97 + 1 : 1), 5,
                                stx_cng::CngChannels::rgba};
        std::vector<std::byte> samples(cng_sample_bytes(image));
        for (std::byte& value : samples) {
            value = static_cast<std::byte>(next(state) % 7 * 31);
        }
        std::vector<std::byte> file = encode_exact(stx_cng::CngEncodeInput{image, image.width * 4ULL, samples});
        for (std::size_t edit = 0; edit < 24; ++edit) {
            const std::uint8_t* bytes = reinterpret_cast<const std::uint8_t*>(file.data());
            std::vector<std::uint8_t> mutated(bytes, bytes + file.size());
            std::size_t position = next(state) % file.size();
            mutated[position] = static_cast<std::uint8_t>(mutated[position] ^ (1U << (next(state) % 8)));
            if (edit % 2 == 0) {
                repair_crc(mutated, position);
            }
            // The mode byte selects decoding.
            mutated.insert(mutated.begin(), std::uint8_t{0});
            LLVMFuzzerTestOneInput(mutated.data(), mutated.size());
        }
    }
    std::printf("stx_cng fuzz harness: %zu accepted, %zu refused\n", accepted, refused);
    require(accepted > 0 && refused > 0, "both outcomes reached");
    return 0;
}
#endif
