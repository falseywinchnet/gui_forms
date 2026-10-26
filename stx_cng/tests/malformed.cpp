// Malformed-input sweep through every public reading call. No input may
// crash, read outside its buffers or write outside the destination (the
// STX_CNG_SANITIZE build turns any violation into a failure); a decode that
// reports success reproduces the original samples, and a salvage reproduces
// every segment it reports intact.
//   1. damage to real files: every truncation and every flipped byte (small
//      files, sampled on larger ones), and random bursts;
//   2. crafted files whose checksums all match: random segment payloads,
//      random table bodies in valid Zstandard frames, random mosaics, random
//      dimensions with matching directories, so hostile data reaches every
//      decoding stage;
//   3. every quantized frequency above 25, and a tables part other than
//      exactly one Zstandard frame, refused as invalid tables.
#include "common.hpp"

#include <zstd.h>

namespace {

using namespace stx_cng_test;
using stx_cng::CngChannels;
using stx_cng::CngError;
using stx_cng::CngImage;

// Destinations larger than this are not allocated; a damaged header can
// claim up to 2^31 samples, and the sweep's own files are far smaller.
constexpr std::uint64_t largest_destination = std::uint64_t{1} << 24;

struct Original final {
    CngImage image{};
    const Bytes* samples = nullptr;
};

bool same_image(CngImage left, CngImage right) {
    bool same = left.width == right.width && left.height == right.height && left.channels == right.channels;
    return same;
}

// Whether segment `index` of tightly packed `left` and `right` agree.
bool segment_equal(const Bytes& left, const Bytes& right, CngImage image, std::uint64_t index) {
    std::uint64_t across = (image.width + 127) / 128;
    std::size_t x0 = static_cast<std::size_t>(index % across) * 128;
    std::size_t y0 = static_cast<std::size_t>(index / across) * 32;
    std::size_t x1 = std::min<std::size_t>(image.width, x0 + 128);
    std::size_t y1 = std::min<std::size_t>(image.height, y0 + 32);
    std::size_t channels = static_cast<std::size_t>(image.channels);
    bool equal = true;
    for (std::size_t y = y0; y < y1 && equal; ++y) {
        std::size_t from = (y * image.width + x0) * channels;
        std::size_t to = (y * image.width + x1) * channels;
        equal = std::equal(left.begin() + static_cast<std::ptrdiff_t>(from), left.begin() + static_cast<std::ptrdiff_t>(to),
                           right.begin() + static_cast<std::ptrdiff_t>(from));
    }
    return equal;
}

struct Readers final {
    std::unique_ptr<stx_cng::CngDecoder> decoder = std::make_unique<stx_cng::CngDecoder>();
    Bytes samples;
    Bytes mosaic;
    std::vector<std::uint8_t> intact;
};

// Runs every reading call over `file`; returns whether decode succeeded.
bool decode_checked(Readers& readers, const Bytes& file, const Original& original) {
    stx_cng::CngDecoder& decoder = *readers.decoder;
    stx_cng::CngInspectResult framing = stx_cng::validate_cng_framing(file);
    static_cast<void>(framing);
    bool decoded = false;
    stx_cng::CngInspectResult inspected = stx_cng::inspect_cng(file);
    std::uint64_t sample_bytes = stx_cng::cng_sample_bytes(inspected.image);
    if (inspected.error == CngError::none && sample_bytes <= largest_destination) {
        readers.samples.assign(static_cast<std::size_t>(sample_bytes), std::byte{0});
        for (std::uint32_t threads : {1U, 4U}) {
            stx_cng::CngDecodeResult result = decoder.decode(file, readers.samples, tight_row_bytes(inspected.image), threads);
            if (result.error == CngError::none && original.samples != nullptr) {
                bool reproduced = same_image(result.image, original.image) && readers.samples == *original.samples;
                check(reproduced, "a successful decode reproduces the original");
            }
            decoded = decoded || result.error == CngError::none;
        }
        CngImage cells = stx_cng::cng_mosaic_image(inspected.image);
        readers.mosaic.assign(static_cast<std::size_t>(stx_cng::cng_sample_bytes(cells)), std::byte{0});
        stx_cng::CngDecodeResult mosaic = decoder.decode_mosaic(file, readers.mosaic, tight_row_bytes(cells));
        check(mosaic.error != CngError::none || inspected.mosaic_available, "a mosaic decodes only from an intact copy");
    }
    stx_cng::CngInspectResult salvageable = stx_cng::inspect_cng_for_salvage(file);
    std::uint64_t salvage_bytes = stx_cng::cng_sample_bytes(salvageable.image);
    if (salvageable.error == CngError::none && salvage_bytes <= largest_destination) {
        readers.samples.assign(static_cast<std::size_t>(salvage_bytes), std::byte{0});
        readers.intact.assign(static_cast<std::size_t>(salvageable.segments), 0);
        stx_cng::CngSalvageResult salvage = decoder.decode_salvage(file, readers.samples, tight_row_bytes(salvageable.image), readers.intact);
        bool comparable = salvage.error == CngError::none && original.samples != nullptr && same_image(salvage.image, original.image);
        for (std::uint64_t index = 0; comparable && index < salvage.segments; ++index) {
            if (readers.intact[static_cast<std::size_t>(index)] == 1) {
                check(segment_equal(readers.samples, *original.samples, original.image, index),
                      "a segment salvage reports intact is the original's");
            }
        }
    }
    return decoded;
}

void damage_sweep(Readers& readers, const Bytes& file, const Original& original, Random& random, bool exhaustive) {
    std::size_t step = exhaustive ? 1 : std::max<std::size_t>(1, file.size() / 400);
    for (std::size_t length = 0; length < file.size(); length += step) {
        Bytes cut = slice(file, 0, length);
        decode_checked(readers, cut, original);
    }
    for (std::size_t at = 0; at < file.size(); at += step) {
        Bytes flipped = file;
        flipped[at] ^= static_cast<std::byte>(1U << (at % 8));
        decode_checked(readers, flipped, original);
    }
    for (int burst = 0; burst < 60; ++burst) {
        Bytes damaged = file;
        std::size_t at = below(random, static_cast<std::uint32_t>(damaged.size()));
        std::size_t span = std::min<std::size_t>(damaged.size() - at, 1 + below(random, 600));
        for (std::size_t index = 0; index < span; ++index) {
            damaged[at + index] = static_cast<std::byte>(next(random) & 255U);
        }
        decode_checked(readers, damaged, original);
    }
}

Bytes compress(const Bytes& body) {
    Bytes frame(ZSTD_compressBound(body.size()));
    std::size_t written = ZSTD_compress(frame.data(), frame.size(), body.data(), body.size(), 3);
    check(!ZSTD_isError(written), "zstd compresses");
    frame.resize(ZSTD_isError(written) ? 0 : written);
    return frame;
}

// A random tables body (38 contexts of random entry counts and q values) in
// one standard Zstandard frame; sometimes cut short.
Bytes random_tables(Random& random) {
    Bytes body;
    for (int context = 0; context < 38; ++context) {
        std::uint32_t entries = below(random, 4) == 0 ? below(random, 700) : below(random, 40);
        body.push_back(static_cast<std::byte>((entries >> 8) & 255U));
        body.push_back(static_cast<std::byte>(entries & 255U));
        for (std::uint32_t entry = 0; entry <= entries; ++entry) {
            std::uint32_t q = below(random, 4) == 0 ? 0 : below(random, 27);
            body.push_back(static_cast<std::byte>(q));
        }
    }
    if (below(random, 5) == 0) {
        body.resize(below(random, static_cast<std::uint32_t>(body.size())));
    }
    Bytes frame = compress(body);
    return frame;
}

void crafted_sweep(Readers& readers, const Bytes& file, Random& random, int rounds) {
    Parts base = split(file);
    Original none{};
    for (int round = 0; round < rounds; ++round) {
        Parts parts = base;
        std::uint32_t kind = below(random, 6);
        if (kind == 0 || kind == 5) {
            std::size_t index = below(random, static_cast<std::uint32_t>(parts.payloads.size()));
            std::size_t length = below(random, 4) == 0 ? below(random, 8) : below(random, 3000);
            parts.payloads[index] = random_bytes(random, length);
        }
        if (kind == 1) {
            parts.tables = random_tables(random);
        }
        if (kind == 2) {
            parts.mosaic = random_bytes(random, below(random, 400));
        }
        if (kind == 3) {
            // Each payload keeps its stream lengths; the streams are scrambled.
            for (Bytes& payload : parts.payloads) {
                for (std::size_t at = 4; at < payload.size(); ++at) {
                    if (below(random, 16) == 0) {
                        payload[at] = static_cast<std::byte>(next(random) & 255U);
                    }
                }
            }
        }
        if (kind == 4) {
            parts.width = 1 + below(random, 300);
            parts.height = 1 + below(random, 120);
            parts.channels = below(random, 2) == 0 ? 3 : 4;
            parts.payloads.resize(segments_of(parts.width, parts.height));
            for (Bytes& payload : parts.payloads) {
                if (payload.empty() || below(random, 3) == 0) {
                    payload = random_bytes(random, below(random, 500));
                }
            }
        }
        if (kind == 5 && below(random, 2) == 0) {
            parts.tables = random_tables(random);
        }
        decode_checked(readers, assemble(parts), none);
    }
}

CngError decode_error(Readers& readers, const Bytes& file) {
    stx_cng::CngInspectResult inspected = stx_cng::inspect_cng(file);
    if (inspected.error != CngError::none) {
        return inspected.error;
    }
    readers.samples.assign(static_cast<std::size_t>(stx_cng::cng_sample_bytes(inspected.image)), std::byte{0});
    stx_cng::CngDecodeResult result = (*readers.decoder).decode(file, readers.samples, tight_row_bytes(inspected.image), 1);
    return result.error;
}

// Every quantized frequency above 25 in an otherwise valid tables body, and a
// tables part of two frames or of a skippable frame, are invalid tables.
void table_sweep(Readers& readers, const Bytes& file) {
    Parts base = split(file);
    Bytes body(38 * 575);
    std::size_t count = ZSTD_decompress(body.data(), body.size(), base.tables.data(), base.tables.size());
    check(!ZSTD_isError(count), "the tables decompress");
    body.resize(ZSTD_isError(count) ? 0 : count);
    for (unsigned q = 26; q < 256 && body.size() > 2; ++q) {
        Bytes edited = body;
        edited[2] = static_cast<std::byte>(q);   // the first q byte of context 0
        Parts parts = base;
        parts.tables = compress(edited);
        check(decode_error(readers, assemble(parts)) == CngError::invalid_tables, "a quantized frequency above 25 is refused");
    }
    Parts twice = base;
    append(twice.tables, base.tables);
    check(decode_error(readers, assemble(twice)) == CngError::invalid_tables, "two frames are refused");
    check(stx_cng::validate_cng_framing(assemble(twice)).error == CngError::invalid_tables, "and strict framing refuses them");
    // A skippable frame of four zero bytes: the magic 0x184D2A50 and the
    // length 4, both little-endian.
    Parts skippable = base;
    Bytes frame;
    append_u32(frame, 0x502A4D18U);
    append_u32(frame, 0x04000000U);
    append_u32(frame, 0);
    skippable.tables = frame;
    check(decode_error(readers, assemble(skippable)) == CngError::invalid_tables, "a skippable frame is refused");
}

struct Case final {
    CngImage image{};
    Pattern pattern = Pattern::gradient;
    bool exhaustive = false;
};

} // namespace

int main() {
    Random random{20261025};
    Readers readers;
    const Case cases[] = {{{1, 1, CngChannels::bgr}, Pattern::noise, true},
                          {{13, 9, CngChannels::bgra}, Pattern::alpha_steps, true},
                          {{129, 33, CngChannels::bgr}, Pattern::tile, false},
                          {{140, 70, CngChannels::bgra}, Pattern::patches, false},
                          {{300, 64, CngChannels::bgr}, Pattern::noise, false}};
    unsigned files = 0;
    for (const Case& entry : cases) {
        Bytes raw = generate(entry.image, entry.pattern, entry.image.width);
        Bytes file = encode_file(entry.image, raw);
        Original original{entry.image, &raw};
        check(decode_checked(readers, file, original), "the intact file decodes");
        damage_sweep(readers, file, original, random, entry.exhaustive);
        crafted_sweep(readers, file, random, 300);
        table_sweep(readers, file);
        ++files;
    }
    std::printf("stx_cng malformed: %u files damaged and crafted\n", files);
    return finish("stx_cng_malformed");
}
