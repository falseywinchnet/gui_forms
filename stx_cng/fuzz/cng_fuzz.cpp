// libFuzzer entry for stx_cng's public operations (CNG 1.0, 2026-10-26).
//
// The first input byte chooses the mode (its value modulo 5); the rest is:
//   0  a file. inspect_cng and decode must agree on the image; a decoded
//      file must salvage to the same samples with every segment intact, and
//      its samples (up to 1 MiB) must re-encode and decode exactly. Mosaic and salvage run
//      on every file that admits them.
//   1  an image (width, height, channels, stride padding, threads) seeding
//      its samples; encode then decode must be exact at that thread count,
//      and a truncated copy must be refused.
//   2  a crafted file: an image encoded as in mode 1, then a payload, the
//      mosaic or every payload replaced by the remaining bytes, with every
//      checksum recomputed, so hostile data reaches segment decoding behind
//      valid tables.
//   3  a collection file: inspection, then every member and any canvas.
//   4  a collection (one to three members, either form) built from the
//      bytes; every member must decode exactly.
// One owner of each kind serves every input, so storage reuse across shapes
// is exercised too. CI runs it briefly per change (fuzz-smoke.yml). Built
// with STX_CNG_FUZZ_STANDALONE, main() replays a fixed pseudo-random set of
// inputs to every mode and mutated files instead, as an ordinary test.
#include "../tests/common.hpp"
#include "stx_cng/collection.hpp"

#include <array>
#include <cstring>

namespace {

using stx_cng::CngChannels;
using stx_cng::CngError;
using stx_cng::CngImage;
using stx_cng_test::Bytes;

// Images above this many sample bytes are only inspected, which keeps each
// input within the smoke's memory limit.
constexpr std::uint64_t decode_sample_limit = std::uint64_t{16} << 20;
// Accepted images above this many sample bytes are not re-encoded: the
// encoder's passes over a large image would exceed the smoke's per-input
// time limit under the sanitizers.
constexpr std::uint64_t encode_sample_limit = std::uint64_t{1} << 20;
constexpr std::size_t entry_capacity = 64;

std::size_t accepted = 0;
std::size_t refused = 0;

void require(bool condition, const char* message) {
    if (condition) {
        return;
    }
    std::fprintf(stderr, "cng fuzz: %s\n", message);
    std::abort();
}

struct Owners final {
    stx_cng::CngEncoder encoder;
    stx_cng::CngDecoder decoder;
    stx_cng::CngCollectionEncoder collection_encoder;
    stx_cng::CngCollectionDecoder collection_decoder;
    std::array<stx_cng::CngCollectionEntry, entry_capacity> entries{};
};

Owners& owners() {
    static std::unique_ptr<Owners> owner = std::make_unique<Owners>();
    return *owner;
}

Bytes encode_exact(CngImage image, std::span<const std::byte> samples, std::uint64_t row_bytes, std::uint32_t threads) {
    Bytes file(static_cast<std::size_t>(stx_cng::cng_encoded_bound(image)));
    stx_cng::CngEncodeInput input{image, row_bytes, samples, threads};
    stx_cng::CngEncodeResult result = owners().encoder.encode(input, file);
    require(result.error == CngError::none, "an admitted image encodes");
    require(result.encoded_bytes <= file.size(), "encoded within the bound");
    file.resize(result.encoded_bytes);
    return file;
}

void decode_file(std::span<const std::byte> file, std::uint32_t threads) {
    stx_cng::CngDecoder& decoder = owners().decoder;
    stx_cng::CngInspectResult framing = stx_cng::validate_cng_framing(file);
    static_cast<void>(framing);
    stx_cng::CngInspectResult salvageable = stx_cng::inspect_cng_for_salvage(file);
    std::uint64_t salvage_bytes = stx_cng::cng_sample_bytes(salvageable.image);
    Bytes salvaged;
    std::vector<std::uint8_t> intact;
    stx_cng::CngSalvageResult salvage{};
    if (salvageable.error == CngError::none && salvage_bytes <= decode_sample_limit) {
        salvaged.resize(static_cast<std::size_t>(salvage_bytes));
        intact.resize(static_cast<std::size_t>(salvageable.segments));
        salvage = decoder.decode_salvage(file, salvaged, stx_cng_test::tight_row_bytes(salvageable.image), intact);
    }
    stx_cng::CngInspectResult inspected = stx_cng::inspect_cng(file);
    std::uint64_t sample_bytes = stx_cng::cng_sample_bytes(inspected.image);
    if (inspected.error != CngError::none || sample_bytes > decode_sample_limit) {
        ++refused;
        return;
    }
    CngImage cells = stx_cng::cng_mosaic_image(inspected.image);
    Bytes means(static_cast<std::size_t>(stx_cng::cng_sample_bytes(cells)));
    stx_cng::CngDecodeResult mosaic = decoder.decode_mosaic(file, means, stx_cng_test::tight_row_bytes(cells));
    require(mosaic.error != CngError::none || inspected.mosaic_available, "a mosaic decodes only from an intact copy");
    Bytes samples(static_cast<std::size_t>(sample_bytes));
    std::uint64_t row_bytes = stx_cng_test::tight_row_bytes(inspected.image);
    stx_cng::CngDecodeResult decoded = decoder.decode(file, samples, row_bytes, threads);
    if (decoded.error != CngError::none) {
        ++refused;
        return;
    }
    ++accepted;
    bool same = decoded.image.width == inspected.image.width && decoded.image.height == inspected.image.height
        && decoded.image.channels == inspected.image.channels;
    require(same, "inspection and decode agree");
    // A file the decoder accepts has every segment intact for salvage too.
    bool whole = salvage.error == CngError::none && salvage.segments_lost == 0 && salvaged == samples;
    require(whole, "an accepted file salvages to the same samples");
    if (sample_bytes > encode_sample_limit) {
        return;
    }
    Bytes again = encode_exact(decoded.image, samples, row_bytes, threads);
    Bytes restored(samples.size());
    require(decoder.decode(again, restored, row_bytes, 1).error == CngError::none && restored == samples,
            "re-encoded samples decode exactly");
}

struct Generated final {
    CngImage image{};
    std::uint64_t stride = 0;
    std::uint32_t threads = 1;
    Bytes input;     // rows `stride` apart
    Bytes samples;   // tightly packed
};

// An image from four header bytes (data holds at least four); its samples
// repeat the remaining bytes, so short inputs give runs and matches and long
// ones give noise.
Generated generated(std::span<const std::uint8_t> data) {
    Generated out;
    out.image = CngImage{static_cast<std::uint32_t>(data[0] % 197 + 1), static_cast<std::uint32_t>(data[1] % 71 + 1),
                         (data[2] & 1U) != 0 ? CngChannels::bgra : CngChannels::bgr};
    out.threads = static_cast<std::uint32_t>(data[3] % 4 + 1);
    std::size_t padding = data[2] >> 5;
    std::size_t row = static_cast<std::size_t>(stx_cng_test::tight_row_bytes(out.image));
    out.stride = row + padding;
    out.input.resize(static_cast<std::size_t>(out.stride) * (out.image.height - 1) + row);
    out.samples.resize(row * out.image.height);
    std::span<const std::uint8_t> source = data.subspan(4);
    for (std::size_t y = 0; y < out.image.height; ++y) {
        for (std::size_t x = 0; x < row; ++x) {
            std::uint8_t value = source.empty() ? static_cast<std::uint8_t>(x * 7 + y) : source[(y * row + x) % source.size()];
            out.input[y * static_cast<std::size_t>(out.stride) + x] = static_cast<std::byte>(value);
            out.samples[y * row + x] = static_cast<std::byte>(value);
        }
    }
    return out;
}

void round_trip(std::span<const std::uint8_t> data) {
    if (data.size() < 5) {
        ++refused;
        return;
    }
    Generated image = generated(data);
    Bytes file = encode_exact(image.image, image.input, image.stride, image.threads);
    Bytes restored(image.samples.size());
    std::uint64_t row = stx_cng_test::tight_row_bytes(image.image);
    stx_cng::CngDecodeResult decoded = owners().decoder.decode(file, restored, row, image.threads);
    require(decoded.error == CngError::none && restored == image.samples, "a round trip is exact");
    std::size_t keep = static_cast<std::size_t>(data[4]) * file.size() / 256;
    std::span<const std::byte> truncated(file.data(), keep);
    require(stx_cng::validate_cng_framing(truncated).error != CngError::none, "strict framing refuses a truncation");
    require(owners().decoder.decode(truncated, restored, row, 1).error != CngError::none, "the decoder refuses a truncation");
    ++accepted;
}

void crafted(std::span<const std::uint8_t> data) {
    if (data.size() < 6) {
        ++refused;
        return;
    }
    Generated image = generated(data.first(5));
    Bytes file = encode_exact(image.image, image.input, image.stride, 1);
    stx_cng_test::Parts parts = stx_cng_test::split(file);
    std::span<const std::uint8_t> rest = data.subspan(5);
    Bytes replacement(rest.size());
    std::memcpy(replacement.data(), rest.data(), rest.size());
    std::uint32_t target = data[4] % 4;
    if (target == 0) {
        parts.mosaic = replacement;
    } else if (target == 1) {
        for (Bytes& payload : parts.payloads) {
            payload = replacement;
        }
    } else {
        parts.payloads[data[4] % parts.payloads.size()] = replacement;
    }
    Bytes assembled = stx_cng_test::assemble(parts);
    decode_file(assembled, image.threads);
}

void collection_file(std::span<const std::byte> file) {
    Owners& owner = owners();
    stx_cng::CngCollectionInfo info = stx_cng::inspect_cng_collection(file, owner.entries);
    if (info.error != CngError::none) {
        ++refused;
        return;
    }
    ++accepted;
    for (std::size_t index = 0; index < info.members; ++index) {
        const stx_cng::CngCollectionEntry& entry = owner.entries[index];
        std::uint64_t bytes = stx_cng::cng_sample_bytes(entry.image);
        if (bytes > decode_sample_limit) {
            continue;
        }
        Bytes samples(static_cast<std::size_t>(bytes));
        stx_cng::CngDecodeResult decoded =
            owner.collection_decoder.decode_member(file, entry.id, samples, stx_cng_test::tight_row_bytes(entry.image), 1);
        static_cast<void>(decoded);
    }
    std::uint64_t canvas_bytes = stx_cng::cng_sample_bytes(info.canvas);
    if (info.form == stx_cng::CngCollectionForm::composite && canvas_bytes <= decode_sample_limit) {
        Bytes canvas(static_cast<std::size_t>(canvas_bytes));
        stx_cng::CngDecodeResult decoded =
            owner.collection_decoder.decode_canvas(file, canvas, stx_cng_test::tight_row_bytes(info.canvas), 2);
        static_cast<void>(decoded);
    }
}

void collection_round_trip(std::span<const std::uint8_t> data) {
    if (data.size() < 6) {
        ++refused;
        return;
    }
    std::size_t count = data[0] % 3 + 1;
    stx_cng::CngCollectionForm form = (data[0] & 4U) != 0 ? stx_cng::CngCollectionForm::composite : stx_cng::CngCollectionForm::packed;
    std::vector<Generated> images;
    std::vector<stx_cng::CngCollectionMember> members;
    for (std::size_t index = 0; index < count; ++index) {
        std::size_t at = 1 + index % (data.size() - 5);
        images.push_back(generated(data.subspan(at)));
    }
    for (std::size_t index = 0; index < count; ++index) {
        const Generated& image = images[index];
        members.push_back(stx_cng::CngCollectionMember{stx_cng::CngMemberId{static_cast<std::uint32_t>(index * 3)}, image.image,
                                                       image.stride, image.input});
    }
    Owners& owner = owners();
    Bytes budget(static_cast<std::size_t>(stx_cng::cng_collection_bound(members, form)));
    stx_cng::CngCollectionEncodeResult encoded = owner.collection_encoder.encode(members, form, 2, budget);
    require(encoded.error == CngError::none, "a collection encodes");
    std::span<const std::byte> file(budget.data(), encoded.encoded_bytes);
    for (std::size_t index = 0; index < count; ++index) {
        Bytes restored(images[index].samples.size());
        stx_cng::CngDecodeResult decoded = owner.collection_decoder.decode_member(
            file, members[index].id, restored, stx_cng_test::tight_row_bytes(images[index].image), 1);
        require(decoded.error == CngError::none && restored == images[index].samples, "a collection member decodes exactly");
    }
    ++accepted;
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) {
    if (size == 0) {
        return 0;
    }
    std::span<const std::uint8_t> rest(data + 1, size - 1);
    std::span<const std::byte> bytes(reinterpret_cast<const std::byte*>(data + 1), size - 1);
    switch (data[0] % 5) {
    case 0: decode_file(bytes, static_cast<std::uint32_t>(size % 3 + 1)); break;
    case 1: round_trip(rest); break;
    case 2: crafted(rest); break;
    case 3: collection_file(bytes); break;
    default: collection_round_trip(rest); break;
    }
    return 0;
}

#ifdef STX_CNG_FUZZ_STANDALONE
namespace {

std::vector<std::uint8_t> random_input(stx_cng_test::Random& random, std::uint8_t mode) {
    // At least the six bytes every mode reads.
    std::vector<std::uint8_t> input(6 + stx_cng_test::below(random, 300));
    for (std::uint8_t& value : input) {
        value = static_cast<std::uint8_t>(stx_cng_test::next(random) >> stx_cng_test::below(random, 8));
    }
    input[0] = mode;
    return input;
}

// `file` with one flipped bit, behind the mode byte.
std::vector<std::uint8_t> mutated(stx_cng_test::Random& random, const Bytes& file, std::uint8_t mode) {
    std::vector<std::uint8_t> input(file.size() + 1);
    input[0] = mode;
    std::memcpy(input.data() + 1, file.data(), file.size());
    std::size_t position = 1 + stx_cng_test::below(random, static_cast<std::uint32_t>(file.size()));
    input[position] = static_cast<std::uint8_t>(input[position] ^ (1U << stx_cng_test::below(random, 8)));
    return input;
}

} // namespace

// Every mode from a fixed seed, and single-bit edits of real files and
// collections.
int main() {
    stx_cng_test::Random random{20261026};
    for (std::size_t round = 0; round < 300; ++round) {
        for (std::uint8_t mode = 0; mode < 5; ++mode) {
            std::vector<std::uint8_t> input = random_input(random, mode);
            LLVMFuzzerTestOneInput(input.data(), input.size());
        }
        std::vector<std::uint8_t> seed = random_input(random, 1);
        Generated image = generated(std::span<const std::uint8_t>(seed).subspan(1));
        Bytes file = encode_exact(image.image, image.input, image.stride, 1);
        stx_cng::CngCollectionMember member{stx_cng::CngMemberId{1}, image.image, image.stride, image.input};
        std::span<const stx_cng::CngCollectionMember> one(&member, 1);
        Bytes collection(static_cast<std::size_t>(stx_cng::cng_collection_bound(one, stx_cng::CngCollectionForm::composite)));
        stx_cng::CngCollectionEncodeResult encoded =
            owners().collection_encoder.encode(one, stx_cng::CngCollectionForm::composite, 1, collection);
        require(encoded.error == CngError::none, "a one-member collection encodes");
        collection.resize(encoded.encoded_bytes);
        for (std::size_t edit = 0; edit < 8; ++edit) {
            std::vector<std::uint8_t> edited = mutated(random, file, 0);
            LLVMFuzzerTestOneInput(edited.data(), edited.size());
            std::vector<std::uint8_t> edited_collection = mutated(random, collection, 3);
            LLVMFuzzerTestOneInput(edited_collection.data(), edited_collection.size());
        }
    }
    std::printf("stx_cng fuzz harness: %zu accepted, %zu refused\n", accepted, refused);
    require(accepted > 0 && refused > 0, "both outcomes reached");
    return 0;
}
#endif
