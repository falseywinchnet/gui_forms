// CNG collections (docs/COLLECTIONS.md) through stx_cng/collection.hpp: both
// forms round trip members of mixed sizes and channel counts, strided and in
// any id order; inspection reads the directory; a composite canvas is
// decoded once and sliced; every refusal of the member list, the budget and
// the reader; and a damage sweep that must never crash.
#include "common.hpp"
#include "stx_cng/collection.hpp"

#include <array>

namespace {

using namespace stx_cng_test;
using stx_cng::CngChannels;
using stx_cng::CngCollectionEntry;
using stx_cng::CngCollectionForm;
using stx_cng::CngCollectionMember;
using stx_cng::CngError;
using stx_cng::CngImage;
using stx_cng::CngMemberId;

struct Fixture final {
    std::vector<Bytes> raws;
    std::vector<Bytes> stored;    // what each member's span points into
    std::vector<CngCollectionMember> members;
};

// Four members: an RGB block, a tall BGRA strip, one pixel, and a BGRA image
// whose samples are strided; ids out of order.
Fixture fixture() {
    Fixture out;
    const std::array<CngImage, 4> images = {CngImage{30, 20, CngChannels::bgr}, CngImage{17, 40, CngChannels::bgra},
                                            CngImage{1, 1, CngChannels::bgr}, CngImage{140, 70, CngChannels::bgra}};
    const std::array<std::uint32_t, 4> ids = {103, 7, 55, 100};
    for (std::size_t index = 0; index < images.size(); ++index) {
        CngImage image = images[index];
        Pattern pattern = patterns[index % 6];
        out.raws.push_back(generate(image, image.channels == CngChannels::bgra ? Pattern::alpha_steps : pattern,
                                    static_cast<std::uint32_t>(index)));
    }
    for (std::size_t index = 0; index < images.size(); ++index) {
        CngImage image = images[index];
        std::uint64_t row = tight_row_bytes(image);
        std::uint64_t stride = index == 3 ? row + 9 : row;
        Bytes samples(static_cast<std::size_t>(stride * (image.height - 1) + row), std::byte{0x77});
        for (std::size_t y = 0; y < image.height; ++y) {
            std::copy_n(out.raws[index].begin() + static_cast<std::ptrdiff_t>(y * row), static_cast<std::size_t>(row),
                        samples.begin() + static_cast<std::ptrdiff_t>(y * stride));
        }
        out.stored.push_back(samples);
        out.members.push_back(CngCollectionMember{CngMemberId{ids[index]}, image, stride, {}});
    }
    for (std::size_t index = 0; index < images.size(); ++index) {
        out.members[index].samples = out.stored[index];
    }
    return out;
}

Bytes encode_collection(stx_cng::CngCollectionEncoder& encoder, std::span<const CngCollectionMember> members, CngCollectionForm form,
                        std::uint32_t threads) {
    Bytes budget(static_cast<std::size_t>(stx_cng::cng_collection_bound(members, form)));
    stx_cng::CngCollectionEncodeResult result = encoder.encode(members, form, threads, budget);
    check(result.error == CngError::none, "a collection encodes");
    budget.resize(result.encoded_bytes);
    return budget;
}

void round_trip(stx_cng::CngCollectionEncoder& encoder, stx_cng::CngCollectionDecoder& decoder, const Fixture& fixture,
                CngCollectionForm form) {
    Bytes file = encode_collection(encoder, fixture.members, form, 1);
    check(encode_collection(encoder, fixture.members, form, 3) == file, "every thread count writes the same collection");
    check(file.size() > 8 && std::to_integer<unsigned>(file[0]) == 0x8A && std::to_integer<unsigned>(file[1]) == 'C',
          "collection signature");
    std::array<CngCollectionEntry, 4> entries{};
    stx_cng::CngCollectionInfo info = stx_cng::inspect_cng_collection(file, entries);
    check(info.error == CngError::none && info.form == form && info.members == 4, "inspection reads the header");
    std::array<CngCollectionEntry, 2> short_entries{};
    stx_cng::CngCollectionInfo partial = stx_cng::inspect_cng_collection(file, short_entries);
    check(partial.error == CngError::insufficient_storage && partial.members == 4, "short entries report the member count");
    std::uint32_t y = 0;
    for (std::size_t index = 0; index < entries.size(); ++index) {
        const CngCollectionEntry& entry = entries[index];
        const CngCollectionMember& member = fixture.members[index];
        bool same = entry.id.value == member.id.value && entry.image.width == member.image.width
            && entry.image.height == member.image.height && entry.image.channels == member.image.channels;
        check(same, "the directory keeps the members in order");
        bool placed = form == CngCollectionForm::packed ? entry.x == 0 && entry.y == 0 : entry.x == 0 && entry.y == y;
        check(placed, "the members' rectangles");
        check(entry.chunk_offset + entry.chunk_bytes <= file.size(), "a member chunk lies inside the file");
        y += member.image.height;
    }
    for (std::size_t index = 0; index < fixture.members.size(); ++index) {
        const CngCollectionMember& member = fixture.members[index];
        Bytes out(fixture.raws[index].size());
        stx_cng::CngDecodeResult decoded = decoder.decode_member(file, member.id, out, tight_row_bytes(member.image), 2);
        check(decoded.error == CngError::none && out == fixture.raws[index], "a member decodes");
        check(decoded.image.width == member.image.width && decoded.image.channels == member.image.channels, "with its own shape");
    }
    Bytes out(16);
    check(decoder.decode_member(file, CngMemberId{9999}, out, 3, 1).error == CngError::invalid_argument, "an unknown id");
    if (form == CngCollectionForm::packed) {
        check(decoder.decode_canvas(file, out, 3, 1).error == CngError::invalid_argument, "a packed collection has no canvas");
        check(info.canvas.width == 0 && info.canvas.height == 0, "and reports none");
        return;
    }
    check(info.canvas.width == 140 && info.canvas.height == 131 && info.canvas.channels == CngChannels::bgra, "the composite canvas");
    Bytes canvas(static_cast<std::size_t>(stx_cng::cng_sample_bytes(info.canvas)));
    stx_cng::CngDecodeResult whole = decoder.decode_canvas(file, canvas, tight_row_bytes(info.canvas), 4);
    check(whole.error == CngError::none, "the canvas decodes");
    for (std::size_t index = 0; index < entries.size(); ++index) {
        Bytes member(fixture.raws[index].size());
        CngError copied = stx_cng::copy_cng_collection_member(canvas, info.canvas, tight_row_bytes(info.canvas), entries[index], member,
                                                              tight_row_bytes(entries[index].image));
        check(copied == CngError::none && member == fixture.raws[index], "a member sliced from the canvas");
    }
    CngCollectionEntry outside = entries[3];
    outside.y = 100;
    Bytes member(fixture.raws[3].size());
    check(stx_cng::copy_cng_collection_member(canvas, info.canvas, tight_row_bytes(info.canvas), outside, member,
                                              tight_row_bytes(outside.image)) == CngError::invalid_argument,
          "an entry outside the canvas");
}

void refusals(stx_cng::CngCollectionEncoder& encoder, const Fixture& fixture) {
    Bytes budget(static_cast<std::size_t>(stx_cng::cng_collection_bound(fixture.members, CngCollectionForm::packed)));
    std::span<const CngCollectionMember> none;
    check(encoder.encode(none, CngCollectionForm::packed, 1, budget).error == CngError::invalid_argument, "no members");
    check(stx_cng::cng_collection_bound(none, CngCollectionForm::packed) == 0, "no bound without members");
    std::vector<CngCollectionMember> many(stx_cng::maximum_collection_members + 1, fixture.members[2]);
    for (std::size_t index = 0; index < many.size(); ++index) {
        many[index].id.value = static_cast<std::uint32_t>(index);
    }
    check(encoder.encode(many, CngCollectionForm::packed, 1, budget).error == CngError::invalid_argument, "more than 4,096 members");
    std::vector<CngCollectionMember> repeated = fixture.members;
    repeated[1].id = repeated[0].id;
    check(encoder.encode(repeated, CngCollectionForm::packed, 1, budget).error == CngError::invalid_argument, "a repeated id");
    std::vector<CngCollectionMember> short_member = fixture.members;
    short_member[0].samples = short_member[0].samples.first(short_member[0].samples.size() - 1);
    check(encoder.encode(short_member, CngCollectionForm::packed, 1, budget).error == CngError::invalid_argument, "short samples");
    check(encoder.encode(fixture.members, static_cast<CngCollectionForm>(2), 1, budget).error == CngError::invalid_argument,
          "an unknown form");
    // Two 2,048 x 2,048 members fit packed, but their composite canvas exceeds
    // 4,194,304 padded pixels and is refused rather than packed.
    CngImage square{2048, 2048, CngChannels::bgr};
    Bytes square_samples(static_cast<std::size_t>(stx_cng::cng_sample_bytes(square)), std::byte{1});
    std::array<CngCollectionMember, 2> squares = {CngCollectionMember{CngMemberId{1}, square, 6144, square_samples},
                                                  CngCollectionMember{CngMemberId{2}, square, 6144, square_samples}};
    check(stx_cng::cng_collection_bound(squares, CngCollectionForm::packed) != 0, "large members fit packed");
    check(stx_cng::cng_collection_bound(squares, CngCollectionForm::composite) == 0, "but not as one canvas");
    check(encoder.encode(squares, CngCollectionForm::composite, 1, budget).error == CngError::invalid_argument, "a canvas too large");
    CngImage wide{65537, 1, CngChannels::bgr};
    Bytes wide_samples(static_cast<std::size_t>(65537 * 3));
    CngCollectionMember wide_member{CngMemberId{1}, wide, 65537 * 3, wide_samples};
    check(stx_cng::cng_collection_bound(std::span<const CngCollectionMember>(&wide_member, 1), CngCollectionForm::packed) == 0,
          "a side above 65,536");
    // The budget: one byte short, then overlapping a member.
    Bytes whole = encode_collection(encoder, fixture.members, CngCollectionForm::packed, 1);
    Bytes small(whole.size() - 1);
    check(encoder.encode(fixture.members, CngCollectionForm::packed, 1, small).error == CngError::insufficient_storage,
          "a short budget");
    Bytes exact(whole.size());
    stx_cng::CngCollectionEncodeResult fits = encoder.encode(fixture.members, CngCollectionForm::packed, 1, exact);
    check(fits.error == CngError::none && exact == whole, "an exact budget suffices");
    std::vector<CngCollectionMember> overlapping = fixture.members;
    Bytes shared(budget.size() + 64);
    overlapping[2].samples = std::span<const std::byte>(shared.data(), 3);
    std::span<std::byte> over(shared.data() + 1, budget.size());
    check(encoder.encode(overlapping, CngCollectionForm::packed, 1, over).error == CngError::overlap, "a budget overlapping a member");
    encoder.release();
    check(encoder.workspace_bytes() == 0, "release frees the collection encoder");
}

void reader_refusals(stx_cng::CngCollectionDecoder& decoder, const Bytes& file) {
    std::array<CngCollectionEntry, 4> entries{};
    Bytes image_file = encode_file(CngImage{3, 3, CngChannels::bgr}, generate(CngImage{3, 3, CngChannels::bgr}, Pattern::noise, 1));
    check(stx_cng::inspect_cng_collection(image_file, entries).error == CngError::invalid_signature, "an image is not a collection");
    Bytes cut = slice(file, 0, file.size() - 1);
    check(stx_cng::inspect_cng_collection(cut, entries).error == CngError::invalid_collection, "a truncated collection");
    Bytes trailing = file;
    trailing.push_back(std::byte{0});
    check(stx_cng::inspect_cng_collection(trailing, entries).error == CngError::invalid_collection, "trailing bytes");
    Bytes out(16);
    check(decoder.decode_member(cut, CngMemberId{7}, out, 3, 1).error == CngError::invalid_collection, "decoding refuses it too");
}

// Damage anywhere: inspection and decoding never crash and either refuse or
// decode the member exactly.
void damage_sweep(stx_cng::CngCollectionDecoder& decoder, const Bytes& file, const Bytes& raw, CngMemberId id, Random& random) {
    std::array<CngCollectionEntry, 1> entries{};
    Bytes out(raw.size());
    for (int round = 0; round < 400; ++round) {
        Bytes damaged = file;
        std::size_t at = below(random, static_cast<std::uint32_t>(damaged.size()));
        damaged[at] = static_cast<std::byte>(next(random) & 255U);
        if (round % 7 == 0) {
            damaged.resize(below(random, static_cast<std::uint32_t>(damaged.size())));
        }
        stx_cng::CngCollectionInfo info = stx_cng::inspect_cng_collection(damaged, entries);
        static_cast<void>(info);
        stx_cng::CngDecodeResult decoded = decoder.decode_member(damaged, id, out, tight_row_bytes(CngImage{30, 20, CngChannels::bgra}), 1);
        check(decoded.error != CngError::none || out == raw, "a damaged collection decodes exactly or not at all");
    }
}

} // namespace

int main() {
    std::unique_ptr<stx_cng::CngCollectionEncoder> encoder = std::make_unique<stx_cng::CngCollectionEncoder>();
    std::unique_ptr<stx_cng::CngCollectionDecoder> decoder = std::make_unique<stx_cng::CngCollectionDecoder>();
    Fixture members = fixture();
    round_trip(*encoder, *decoder, members, CngCollectionForm::packed);
    round_trip(*encoder, *decoder, members, CngCollectionForm::composite);
    refusals(*encoder, members);
    Bytes packed = encode_collection(*encoder, members.members, CngCollectionForm::packed, 1);
    reader_refusals(*decoder, packed);
    Random random{20261026};
    CngImage image{30, 20, CngChannels::bgra};
    Bytes raw = generate(image, Pattern::alpha_steps, 3);
    CngCollectionMember single{CngMemberId{9}, image, tight_row_bytes(image), raw};
    std::span<const CngCollectionMember> one(&single, 1);
    for (CngCollectionForm form : {CngCollectionForm::packed, CngCollectionForm::composite}) {
        Bytes file = encode_collection(*encoder, one, form, 1);
        damage_sweep(*decoder, file, raw, single.id, random);
    }
    (*decoder).release();
    check((*decoder).workspace_bytes() == 0, "release frees the collection decoder");
    return finish("stx_cng_collection");
}
