// Malformed and truncated input: deterministic sweeps over every prefix and
// every byte of encoded files, then one directed case per refusal rule,
// including the strict-form rules. All fixtures live in memory.
#include "common.hpp"

#include "../src/format.hpp"
#include "stx_zlib/checksum.hpp"

#define ZSTD_STATIC_LINKING_ONLY
#include <zstd.h>

#include <algorithm>
#include <memory>

namespace {

using stx_cng::CngChannels;
using stx_cng::CngError;
using stx_cng::CngImage;
using stx_cng::detail::Sequence;
using stx_cng_test::check;
using Bytes = std::vector<std::uint8_t>;

void append_u32(Bytes& bytes, std::uint32_t value) {
    for (std::size_t index = 0; index < 4; ++index) {
        bytes.push_back(static_cast<std::uint8_t>(value >> (24 - 8 * index)));
    }
}

std::uint32_t read_u32(const Bytes& bytes, std::size_t offset) {
    std::uint32_t value = 0;
    for (std::size_t index = 0; index < 4; ++index) {
        value = (value << 8) | bytes[offset + index];
    }
    return value;
}

void write_u32(Bytes& bytes, std::size_t offset, std::uint32_t value) {
    for (std::size_t index = 0; index < 4; ++index) {
        bytes[offset + index] = static_cast<std::uint8_t>(value >> (24 - 8 * index));
    }
}

std::uint32_t crc_of(const std::uint8_t* data, std::size_t size) {
    std::uint32_t crc = stx_zlib::crc32(std::span<const std::byte>(reinterpret_cast<const std::byte*>(data), size));
    return crc;
}

void append_chunk(Bytes& file, const stx_cng::detail::ChunkType& type, const Bytes& payload) {
    std::size_t start = file.size();
    append_u32(file, static_cast<std::uint32_t>(payload.size()));
    file.insert(file.end(), type.begin(), type.end());
    file.insert(file.end(), payload.begin(), payload.end());
    append_u32(file, crc_of(file.data() + start + 4, 4 + payload.size()));
}

// Recomputes the CRC of the chunk at `offset` after an edit of its payload.
void repair_crc(Bytes& file, std::size_t offset) {
    std::uint32_t length = read_u32(file, offset);
    write_u32(file, offset + 8 + length, crc_of(file.data() + offset + 4, 4 + length));
}

struct HeaderFields final {
    std::uint8_t version = 2;
    std::uint8_t channels = 4;
    std::uint8_t flags = 0;
    std::uint8_t reserved = 0;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t budget = 1048576;
};

Bytes start_file(const HeaderFields& fields) {
    Bytes file(stx_cng::detail::signature.begin(), stx_cng::detail::signature.end());
    Bytes payload{fields.version, fields.channels, fields.flags, fields.reserved};
    append_u32(payload, fields.width);
    append_u32(payload, fields.height);
    append_u32(payload, fields.budget);
    append_chunk(file, stx_cng::detail::header_type, payload);
    return file;
}

Bytes finish_file(Bytes file) {
    append_chunk(file, stx_cng::detail::end_type, Bytes{});
    return file;
}

// A level-3 Zstandard frame, with or without the declared content size.
Bytes zstd_frame(const std::uint8_t* data, std::size_t size, bool content_size) {
    ZSTD_CCtx* context = ZSTD_createCCtx();
    std::size_t level = ZSTD_CCtx_setParameter(context, ZSTD_c_compressionLevel, 3);
    std::size_t flag = ZSTD_CCtx_setParameter(context, ZSTD_c_contentSizeFlag, content_size ? 1 : 0);
    Bytes frame(ZSTD_compressBound(size));
    std::size_t written = ZSTD_compress2(context, frame.data(), frame.size(), data, size);
    ZSTD_freeCCtx(context);
    bool failed = ZSTD_isError(level) != 0 || ZSTD_isError(flag) != 0 || ZSTD_isError(written) != 0;
    check(!failed, "fixture: Zstandard frame");
    frame.resize(failed ? 0 : written);
    return frame;
}

Bytes sequence_frame(const std::vector<Sequence>& sequences, bool content_size) {
    Bytes raw(sequences.size() * stx_cng::detail::sequence_record_bytes);
    stx_cng::detail::pack_sequences(sequences, raw);
    Bytes frame = zstd_frame(raw.data(), raw.size(), content_size);
    return frame;
}

// One BAND payload from its parts; counts are written as given.
Bytes band_payload(std::size_t count, std::size_t literals, const Bytes& frame, std::uint8_t coding, const Bytes& literal_bytes) {
    Bytes payload;
    append_u32(payload, static_cast<std::uint32_t>(count));
    append_u32(payload, static_cast<std::uint32_t>(literals));
    append_u32(payload, static_cast<std::uint32_t>(frame.size()));
    payload.insert(payload.end(), frame.begin(), frame.end());
    payload.push_back(coding);
    payload.insert(payload.end(), literal_bytes.begin(), literal_bytes.end());
    return payload;
}

std::span<const std::byte> as_bytes(const Bytes& bytes) {
    std::span<const std::byte> view(reinterpret_cast<const std::byte*>(bytes.data()), bytes.size());
    return view;
}

CngError decode(stx_cng::CngDecoder& decoder, const Bytes& file, std::size_t sample_bytes) {
    std::vector<std::byte> output(sample_bytes);
    stx_cng::CngDecodeResult result = decoder.decode(as_bytes(file), output);
    return result.error;
}

Bytes encode_file(stx_cng::CngEncoder& encoder, CngImage image, stx_cng_test::Pattern pattern) {
    std::vector<std::byte> samples = stx_cng_test::generate(image, pattern, 7);
    stx_cng_test::Encoded encoded = stx_cng_test::encode(encoder, image, samples, stx_cng_test::tight_row_bytes(image));
    const std::uint8_t* begin = reinterpret_cast<const std::uint8_t*>(encoded.bytes.data());
    Bytes file(begin, begin + encoded.bytes.size());
    return file;
}

// Every proper prefix fails; every flipped byte fails (the CRC or the framing
// catches it); with the chunk CRC repaired the edit reaches the band logic,
// which returns some result (the sanitizer build checks it does so safely).
void sweep(stx_cng::CngDecoder& decoder, const Bytes& file, std::size_t sample_bytes) {
    check(decode(decoder, file, sample_bytes) == CngError::none, "sweep source decodes");
    // Small files are swept exhaustively, large ones at 256 evenly spaced bytes.
    std::size_t step = file.size() > 4096 ? file.size() / 256 : 1;
    for (std::size_t keep = 0; keep < file.size(); keep += step) {
        Bytes prefix(file.begin(), file.begin() + static_cast<std::ptrdiff_t>(keep));
        check(decode(decoder, prefix, sample_bytes) != CngError::none, "every proper prefix refused");
    }
    std::vector<std::size_t> chunks;
    for (std::size_t offset = stx_cng::detail::signature.size(); offset + 12 <= file.size(); offset += 12 + read_u32(file, offset)) {
        chunks.push_back(offset);
    }
    const std::uint8_t flips[] = {0x01, 0x80, 0xFF};
    for (std::size_t position = 0; position < file.size(); position += step) {
        for (std::uint8_t flip : flips) {
            Bytes damaged(file);
            damaged[position] = static_cast<std::uint8_t>(damaged[position] ^ flip);
            check(decode(decoder, damaged, sample_bytes) != CngError::none, "every flipped byte refused");
            for (std::size_t chunk : chunks) {
                std::size_t length = read_u32(file, chunk);
                if (position >= chunk + 8 && position < chunk + 8 + length) {
                    repair_crc(damaged, chunk);
                    check(decode(decoder, damaged, sample_bytes) != CngError::crc_mismatch, "repaired CRC accepted");
                }
            }
        }
    }
    Bytes extended(file);
    extended.push_back(0);
    check(decode(decoder, extended, sample_bytes) == CngError::trailing_data, "trailing byte refused");
}

struct HeaderCase final {
    HeaderFields fields;
    CngError expected;
    const char* what;
};

HeaderFields edited(std::uint8_t version, std::uint8_t channels, std::uint8_t flags, std::uint8_t reserved,
                    std::uint32_t width, std::uint32_t height, std::uint32_t budget) {
    HeaderFields fields{version, channels, flags, reserved, width, height, budget};
    return fields;
}

void header_cases(stx_cng::CngDecoder& decoder) {
    const HeaderCase cases[] = {
        {edited(3, 4, 0, 0, 8, 8, 1048576), CngError::invalid_header, "version refused"},
        {edited(2, 2, 0, 0, 8, 8, 1048576), CngError::invalid_header, "channel count refused"},
        {edited(2, 4, 1, 0, 8, 8, 1048576), CngError::invalid_header, "flags refused"},
        {edited(2, 4, 0, 1, 8, 8, 1048576), CngError::invalid_header, "reserved byte refused"},
        {edited(2, 4, 0, 0, 8, 8, 524288), CngError::invalid_header, "band budget refused"},
        {edited(2, 4, 0, 0, 0, 8, 1048576), CngError::unsupported_dimensions, "zero width refused"},
        {edited(2, 4, 0, 0, 8, 0, 1048576), CngError::unsupported_dimensions, "zero height refused"},
        {edited(2, 3, 0, 0, 16385, 1, 1048576), CngError::unsupported_dimensions, "side beyond 16384 refused"},
        {edited(2, 4, 0, 0, 16384, 4097, 1048576), CngError::unsupported_dimensions, "area beyond 2^26 refused"},
        {edited(2, 4, 0, 0, 16384, 4096, 1048576), CngError::none, "2^26 pixels admitted"}};
    for (const HeaderCase& entry : cases) {
        Bytes file = finish_file(start_file(entry.fields));
        check(stx_cng::inspect_cng(as_bytes(file)).error == entry.expected, entry.what);
    }
    const std::size_t lengths[] = {15, 17};
    for (std::size_t length : lengths) {
        Bytes file(stx_cng::detail::signature.begin(), stx_cng::detail::signature.end());
        Bytes payload(length, 0);
        payload[0] = 2;
        payload[1] = 4;
        append_chunk(file, stx_cng::detail::header_type, payload);
        check(stx_cng::inspect_cng(as_bytes(file)).error == CngError::invalid_header, "header length refused");
    }
    Bytes file(stx_cng::detail::signature.begin(), stx_cng::detail::signature.end());
    append_chunk(file, stx_cng::detail::band_type, Bytes(16, 0));
    check(stx_cng::inspect_cng(as_bytes(file)).error == CngError::invalid_chunk, "first chunk must be CHDR");
    Bytes overlong(stx_cng::detail::signature.begin(), stx_cng::detail::signature.end());
    append_u32(overlong, 0xFFFFFFFFU);
    check(stx_cng::inspect_cng(as_bytes(overlong)).error == CngError::truncated, "chunk length beyond the input refused");
    check(decode(decoder, Bytes{}, 0) == CngError::invalid_signature, "empty input refused");
}

// A 16 x 8 RGBA image (128 pixels, one band) whose band is built by hand.
constexpr std::uint32_t band_width = 16;
constexpr std::uint32_t band_height = 8;
constexpr std::size_t band_pixels = band_width * band_height;
constexpr std::size_t band_sample_bytes = band_pixels * 4;

Bytes one_band_file(std::uint32_t width, std::uint32_t height, const Bytes& payload) {
    HeaderFields fields;
    fields.width = width;
    fields.height = height;
    Bytes file = start_file(fields);
    append_chunk(file, stx_cng::detail::band_type, payload);
    return finish_file(file);
}

CngError decode_band(stx_cng::CngDecoder& decoder, const Bytes& payload) {
    CngError error = decode(decoder, one_band_file(band_width, band_height, payload), band_sample_bytes);
    return error;
}

struct GrammarCase final {
    std::vector<Sequence> sequences;
    std::size_t literals;
    const char* what;
};

void count_and_frame_cases(stx_cng::CngDecoder& decoder) {
    Bytes stored(band_sample_bytes, 0x33);
    std::vector<Sequence> all_literals{Sequence{.literals = static_cast<std::uint32_t>(band_pixels)}};
    Bytes frame = sequence_frame(all_literals, true);
    check(decode_band(decoder, band_payload(1, band_pixels, frame, 0, stored)) == CngError::none, "hand-built band decodes");
    check(decode_band(decoder, band_payload(0, band_pixels, frame, 0, stored)) == CngError::invalid_band, "zero sequence count refused");
    check(decode_band(decoder, band_payload(band_pixels / 4 + 2, band_pixels, frame, 0, stored)) == CngError::invalid_band,
          "sequence count beyond the band refused");
    check(decode_band(decoder, band_payload(1, band_pixels + 1, frame, 0, stored)) == CngError::invalid_band,
          "literal count beyond the band refused");
    Bytes overlong = band_payload(1, band_pixels, frame, 0, stored);
    write_u32(overlong, 8, static_cast<std::uint32_t>(overlong.size()));
    check(decode_band(decoder, overlong) == CngError::invalid_band, "frame length beyond the payload refused");
    check(decode_band(decoder, Bytes(12, 0)) == CngError::invalid_band, "short band refused");

    // Strict frames: exactly one complete frame declaring its size.
    Bytes skippable(frame);
    const std::uint8_t skip[] = {0x50, 0x2A, 0x4D, 0x18, 0, 0, 0, 0};
    skippable.insert(skippable.end(), std::begin(skip), std::end(skip));
    check(decode_band(decoder, band_payload(1, band_pixels, skippable, 0, stored)) == CngError::entropy_failure,
          "trailing skippable frame refused");
    Bytes concatenated(frame);
    Bytes empty_frame = zstd_frame(nullptr, 0, true);
    concatenated.insert(concatenated.end(), empty_frame.begin(), empty_frame.end());
    check(decode_band(decoder, band_payload(1, band_pixels, concatenated, 0, stored)) == CngError::entropy_failure,
          "second frame refused");
    Bytes unsized = sequence_frame(all_literals, false);
    check(decode_band(decoder, band_payload(1, band_pixels, unsized, 0, stored)) == CngError::entropy_failure,
          "frame without a declared size refused");
    check(decode_band(decoder, band_payload(1, band_pixels, Bytes{1, 2, 3}, 0, stored)) == CngError::entropy_failure,
          "garbage frame refused");
    check(decode_band(decoder, band_payload(1, band_pixels, Bytes{}, 0, stored)) == CngError::entropy_failure,
          "missing frame refused");
    std::vector<Sequence> two{Sequence{.literals = 124, .length = 4, .distance = 1}, Sequence{}};
    check(decode_band(decoder, band_payload(1, band_pixels, sequence_frame(two, true), 0, stored)) == CngError::entropy_failure,
          "frame size disagreeing with the count refused");

    const GrammarCase grammar[] = {
        {{Sequence{.literals = 4, .length = 3, .distance = 1}, Sequence{.literals = 121}}, 125, "run below four refused"},
        {{Sequence{.literals = 4, .length = 8, .distance = 5}, Sequence{.literals = 116}}, 120, "distance beyond output refused"},
        {{Sequence{.literals = 128, .length = 0, .distance = 1}}, 128, "terminal distance refused"},
        {{Sequence{.literals = 120, .length = 8}}, 120, "terminal run refused"},
        {{Sequence{.literals = 4}, Sequence{.literals = 124}}, 128, "early zero-length record refused"},
        {{Sequence{.literals = 127}}, 127, "band not covered refused"},
        {{Sequence{.literals = 4, .length = 121, .distance = 0}, Sequence{.literals = 4}}, 8, "band overrun refused"}};
    for (const GrammarCase& entry : grammar) {
        Bytes literal_bytes(entry.literals * 4, 0x11);
        Bytes payload = band_payload(entry.sequences.size(), entry.literals, sequence_frame(entry.sequences, true), 0, literal_bytes);
        check(decode_band(decoder, payload) == CngError::invalid_sequences, entry.what);
    }
    Bytes extra_literals(band_sample_bytes, 0x11);
    std::vector<Sequence> fewer{Sequence{.literals = 4, .length = 124, .distance = 0}, Sequence{}};
    check(decode_band(decoder, band_payload(2, 128, sequence_frame(fewer, true), 0, extra_literals)) == CngError::invalid_sequences,
          "literal count disagreeing with the records refused");
}

void literal_cases(stx_cng::CngDecoder& decoder) {
    Bytes stored(band_sample_bytes, 0x33);
    std::vector<Sequence> all_literals{Sequence{.literals = static_cast<std::uint32_t>(band_pixels)}};
    Bytes frame = sequence_frame(all_literals, true);
    check(decode_band(decoder, band_payload(1, band_pixels, frame, 2, stored)) == CngError::invalid_literals,
          "unknown literal coding refused");
    Bytes short_stored(stored.begin(), stored.end() - 1);
    check(decode_band(decoder, band_payload(1, band_pixels, frame, 0, short_stored)) == CngError::invalid_literals,
          "short stored literals refused");
    // Zero literals have only the stored form.
    std::vector<Sequence> predicted{Sequence{.length = static_cast<std::uint32_t>(band_pixels)}, Sequence{}};
    Bytes predicted_frame = sequence_frame(predicted, true);
    check(decode_band(decoder, band_payload(2, 0, predicted_frame, 0, Bytes{})) == CngError::none, "zero-literal stored band decodes");
    Bytes empty_frame = zstd_frame(nullptr, 0, true);
    check(decode_band(decoder, band_payload(2, 0, predicted_frame, 1, empty_frame)) == CngError::invalid_literals,
          "zero literals compressed refused");
    // Compressed literals must be smaller than the stored samples.
    Bytes packed(stx_cng::detail::packed_bytes(band_pixels, 4));
    std::uint32_t state = 12345U;
    for (std::uint8_t& value : packed) {
        state = state * 1664525U + 1013904223U;
        value = static_cast<std::uint8_t>(state >> 24);
    }
    Bytes separated(2 * packed.size() + 4);
    std::size_t separated_size = stx_cng::detail::separate_runs(packed, separated);
    Bytes incompressible = zstd_frame(separated.data(), separated_size, true);
    check(incompressible.size() >= band_sample_bytes, "fixture: incompressible literals");
    check(decode_band(decoder, band_payload(1, band_pixels, frame, 1, incompressible)) == CngError::invalid_literals,
          "compressed literals not smaller than stored refused");
    // Run descriptors that do not restore exactly the packed bytes.
    Bytes truncated_runs(separated.begin(), separated.begin() + static_cast<std::ptrdiff_t>(separated_size / 2));
    Bytes truncated_frame = zstd_frame(truncated_runs.data(), truncated_runs.size(), true);
    if (truncated_frame.size() < band_sample_bytes) {
        check(decode_band(decoder, band_payload(1, band_pixels, frame, 1, truncated_frame)) == CngError::invalid_literals,
              "malformed run descriptors refused");
    }
}

// Compressed literals with an odd count (a 127 x 1 image of flat residuals):
// valid, then the same band with a nonzero padding nibble.
void padding_case(stx_cng::CngDecoder& decoder) {
    std::uint32_t literals = 127;
    std::vector<Sequence> all{Sequence{.literals = literals}};
    Bytes frame = sequence_frame(all, true);
    Bytes packed(stx_cng::detail::packed_bytes(literals, 4), 0);
    Bytes separated(2 * packed.size() + 4);
    std::size_t separated_size = stx_cng::detail::separate_runs(packed, separated);
    Bytes literal_frame = zstd_frame(separated.data(), separated_size, true);
    Bytes file = one_band_file(literals, 1, band_payload(1, literals, frame, 1, literal_frame));
    check(decode(decoder, file, literals * 4) == CngError::none, "odd compressed literals decode");
    // The first plane's final byte: its high nibble is the padding slot.
    std::size_t groups = (literals + 1) / 2;
    packed[groups - 1] = 0x10;
    separated_size = stx_cng::detail::separate_runs(packed, separated);
    literal_frame = zstd_frame(separated.data(), separated_size, true);
    Bytes padded = one_band_file(literals, 1, band_payload(1, literals, frame, 1, literal_frame));
    check(decode(decoder, padded, literals * 4) == CngError::invalid_literals, "nonzero padding nibble refused");
}

void framing_cases(stx_cng::CngDecoder& decoder, stx_cng::CngEncoder& encoder) {
    CngImage image{40, 30, CngChannels::rgb};
    Bytes file = encode_file(encoder, image, stx_cng_test::Pattern::sparse_noise);
    std::size_t samples = stx_cng::cng_sample_bytes(image);
    Bytes without_end(file.begin(), file.end() - 12);
    Bytes long_end(without_end);
    append_chunk(long_end, stx_cng::detail::end_type, Bytes{0});
    check(decode(decoder, long_end, samples) == CngError::invalid_chunk, "nonempty end chunk refused");
    check(decode(decoder, without_end, samples) == CngError::truncated, "missing end chunk refused");
    Bytes extra_band(without_end);
    append_chunk(extra_band, stx_cng::detail::band_type, Bytes(13, 0));
    append_chunk(extra_band, stx_cng::detail::end_type, Bytes{});
    check(decode(decoder, extra_band, samples) == CngError::invalid_chunk, "extra band refused");
    check(decode(decoder, file, samples - 1) == CngError::insufficient_storage, "wrong sample storage refused");
    check(decode(decoder, file, samples) == CngError::none, "decoder reusable after refusals");
}

} // namespace

int main() {
    std::unique_ptr<stx_cng::CngEncoder> encoder = std::make_unique<stx_cng::CngEncoder>();
    std::unique_ptr<stx_cng::CngDecoder> decoder = std::make_unique<stx_cng::CngDecoder>();
    const CngImage sweep_shapes[] = {{5, 3, CngChannels::rgb}, {31, 17, CngChannels::rgba},
                                     {300, 1100, CngChannels::rgba}, {700, 520, CngChannels::rgb}};
    const stx_cng_test::Pattern sweep_patterns[] = {stx_cng_test::Pattern::ramp, stx_cng_test::Pattern::noise,
                                                    stx_cng_test::Pattern::sparse_noise};
    for (const CngImage& image : sweep_shapes) {
        for (stx_cng_test::Pattern pattern : sweep_patterns) {
            Bytes file = encode_file(*encoder, image, pattern);
            sweep(*decoder, file, stx_cng::cng_sample_bytes(image));
        }
    }
    header_cases(*decoder);
    count_and_frame_cases(*decoder);
    literal_cases(*decoder);
    padding_case(*decoder);
    framing_cases(*decoder, *encoder);
    return stx_cng_test::finish("stx_cng_malformed");
}
