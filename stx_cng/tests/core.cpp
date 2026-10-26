// Round trips through every public call of stx_cng/codec.hpp: images of every
// size class, channel count and pattern at one, two and four threads;
// inspection, strict framing, the mosaic and salvage; strided samples;
// storage reuse and release; the copy choice of FORMAT.md section 3.4; and
// every documented refusal. All fixtures are generated in memory.
#include "common.hpp"

#include <array>
#include <limits>

namespace {

using namespace stx_cng_test;
using stx_cng::CngChannels;
using stx_cng::CngError;
using stx_cng::CngImage;

struct Size final {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
};

// The upstream round-trip sizes: single pixels, single rows and columns,
// segment edges exactly and one past, and several segment grids.
constexpr std::array<Size, 12> sizes = {{{1, 1}, {9, 1}, {1, 9}, {7, 5}, {127, 31}, {128, 32}, {129, 33}, {300, 64},
                                         {260, 97}, {300, 200}, {513, 3}, {2, 130}}};

void image_case(stx_cng::CngEncoder& encoder, stx_cng::CngDecoder& decoder, CngImage image, Pattern pattern) {
    Bytes raw = generate(image, pattern, image.width * 31 + image.height + static_cast<std::uint32_t>(pattern));
    std::uint64_t row = tight_row_bytes(image);
    Encoded encoded = encode(encoder, image, raw, row, 1);
    check(encoded.result.error == CngError::none, "encode succeeds");
    check(encoded.result.required_bytes == encoded.result.encoded_bytes, "the required size is the file's");
    check(encoded.bytes.size() <= stx_cng::cng_encoded_bound(image), "the file is within the bound");
    Encoded four = encode(encoder, image, raw, row, 4);
    check(four.bytes == encoded.bytes, "four threads write the same file");
    const Bytes& file = encoded.bytes;
    bool header = file.size() > 72 && std::to_integer<unsigned>(file[0]) == 0x89 && std::to_integer<unsigned>(file[17]) == 1;
    check(header, "signature and version");
    stx_cng::CngInspectResult inspected = stx_cng::inspect_cng(file);
    bool same = inspected.error == CngError::none && inspected.image.width == image.width && inspected.image.height == image.height
        && inspected.image.channels == image.channels && inspected.mosaic_available;
    check(same, "inspection reads the image");
    check(inspected.segments == stx_cng::cng_segment_count(image), "inspection counts the segments");
    check(stx_cng::validate_cng_framing(file).error == CngError::none, "an encoded file passes strict framing");
    for (std::uint32_t threads : {1U, 2U, 4U}) {
        Decoded decoded = decode(decoder, file, threads);
        check(decoded.result.error == CngError::none && decoded.samples == raw, "decode");
    }
    CngImage mosaic = stx_cng::cng_mosaic_image(image);
    Bytes means(static_cast<std::size_t>(stx_cng::cng_sample_bytes(mosaic)));
    stx_cng::CngDecodeResult decoded_mosaic = decoder.decode_mosaic(file, means, tight_row_bytes(mosaic));
    check(decoded_mosaic.error == CngError::none && means == block_means(raw, image), "mosaic");
    Bytes salvaged(raw.size());
    std::vector<std::uint8_t> intact(static_cast<std::size_t>(inspected.segments));
    stx_cng::CngSalvageResult salvage = decoder.decode_salvage(file, salvaged, row, intact);
    bool all_intact = std::count(intact.begin(), intact.end(), std::uint8_t{1}) == static_cast<std::ptrdiff_t>(intact.size());
    bool whole = salvage.error == CngError::none && salvage.segments_lost == 0 && salvage.mosaic_used && all_intact;
    check(whole && salvaged == raw, "salvage of an intact file");
}

// Samples rows `row_bytes` apart; the padding holds `fill`.
Bytes strided(const Bytes& tight, CngImage image, std::uint64_t row_bytes, std::byte fill) {
    std::size_t row = static_cast<std::size_t>(tight_row_bytes(image));
    std::size_t stride = static_cast<std::size_t>(row_bytes);
    Bytes out(stride * (image.height - 1) + row, fill);
    for (std::size_t y = 0; y < image.height; ++y) {
        std::copy_n(tight.begin() + static_cast<std::ptrdiff_t>(y * row), row, out.begin() + static_cast<std::ptrdiff_t>(y * stride));
    }
    return out;
}

// Encoding from strided samples writes the tight samples' file; decoding into
// a strided destination fills the rows and leaves the padding alone.
void strided_case(stx_cng::CngEncoder& encoder, stx_cng::CngDecoder& decoder, CngImage image) {
    Bytes raw = generate(image, Pattern::patches, 5);
    std::uint64_t row_bytes = tight_row_bytes(image) + 13;
    Bytes input = strided(raw, image, row_bytes, std::byte{0x5A});
    Bytes file = encode_file(image, raw);
    Encoded encoded = encode(encoder, image, input, row_bytes, 2);
    check(encoded.result.error == CngError::none && encoded.bytes == file, "strided samples encode as tight ones");
    Bytes destination(input.size(), std::byte{0xA5});
    Bytes expected = strided(raw, image, row_bytes, std::byte{0xA5});
    for (std::uint32_t threads : {1U, 3U}) {
        std::fill(destination.begin(), destination.end(), std::byte{0xA5});
        stx_cng::CngDecodeResult decoded = decoder.decode(file, destination, row_bytes, threads);
        check(decoded.error == CngError::none && destination == expected, "a strided decode leaves the padding");
    }
    CngImage mosaic = stx_cng::cng_mosaic_image(image);
    std::uint64_t mosaic_row = tight_row_bytes(mosaic) + 3;
    Bytes means(static_cast<std::size_t>(mosaic_row * (mosaic.height - 1) + tight_row_bytes(mosaic)), std::byte{0xA5});
    stx_cng::CngDecodeResult decoded_mosaic = decoder.decode_mosaic(file, means, mosaic_row);
    Bytes expected_means = strided(block_means(raw, image), mosaic, mosaic_row, std::byte{0xA5});
    check(decoded_mosaic.error == CngError::none && means == expected_means, "a strided mosaic");
    std::vector<std::uint8_t> intact(static_cast<std::size_t>(stx_cng::cng_segment_count(image)));
    std::fill(destination.begin(), destination.end(), std::byte{0xA5});
    stx_cng::CngSalvageResult salvage = decoder.decode_salvage(file, destination, row_bytes, intact);
    check(salvage.error == CngError::none && destination == expected, "a strided salvage");
}

// More workers than tiles, and the maximum, write the same file and decode.
void thread_counts(stx_cng::CngEncoder& encoder, stx_cng::CngDecoder& decoder) {
    CngImage image{700, 90, CngChannels::bgra};
    Bytes raw = generate(image, Pattern::alpha_steps, 3);
    Bytes file = encode_file(image, raw);
    for (std::uint32_t threads : {3U, 7U, stx_cng::maximum_threads}) {
        Encoded encoded = encode(encoder, image, raw, tight_row_bytes(image), threads);
        check(encoded.bytes == file, "every thread count writes the same file");
        Decoded decoded = decode(decoder, file, threads);
        check(decoded.result.error == CngError::none && decoded.samples == raw, "every thread count decodes");
    }
}

// Storage grows to the largest image and is reused; release frees it all and
// the next call grows it again.
void storage_reuse() {
    std::unique_ptr<stx_cng::CngEncoder> encoder = std::make_unique<stx_cng::CngEncoder>();
    std::unique_ptr<stx_cng::CngDecoder> decoder = std::make_unique<stx_cng::CngDecoder>();
    check((*encoder).workspace_bytes() == 0 && (*decoder).workspace_bytes() == 0, "owners start empty");
    CngImage large{400, 100, CngChannels::bgr};
    CngImage small{20, 10, CngChannels::bgr};
    Bytes large_raw = generate(large, Pattern::noise, 1);
    Bytes small_raw = generate(small, Pattern::steps, 2);
    Encoded first = encode(*encoder, large, large_raw, tight_row_bytes(large), 1);
    std::size_t encoder_bytes = (*encoder).workspace_bytes();
    check(first.result.workspace_bytes == encoder_bytes && encoder_bytes > 0, "the encoder reports its storage");
    Encoded second = encode(*encoder, small, small_raw, tight_row_bytes(small), 1);
    check(second.result.error == CngError::none && (*encoder).workspace_bytes() == encoder_bytes, "a smaller image reuses storage");
    Decoded decoded = decode(*decoder, first.bytes, 1);
    std::size_t decoder_bytes = (*decoder).workspace_bytes();
    check(decoded.result.workspace_bytes == decoder_bytes && decoder_bytes > 0, "the decoder reports its storage");
    Decoded again = decode(*decoder, second.bytes, 1);
    check(again.samples == small_raw && (*decoder).workspace_bytes() == decoder_bytes, "the decoder reuses storage");
    (*encoder).release();
    (*decoder).release();
    check((*encoder).workspace_bytes() == 0 && (*decoder).workspace_bytes() == 0, "release frees all storage");
    Encoded after = encode(*encoder, large, large_raw, tight_row_bytes(large), 1);
    check(after.bytes == first.bytes, "an encoder works again after release");
    Decoded after_decode = decode(*decoder, after.bytes, 1);
    check(after_decode.samples == large_raw, "a decoder works again after release");
}

// Both copies of the 28 field bytes and their CRC.
void write_field_blocks(Bytes& file, const Bytes& fields) {
    std::copy(fields.begin(), fields.end(), file.begin() + 8);
    std::copy(fields.begin(), fields.end(), file.end() - static_cast<std::ptrdiff_t>(field_block_bytes));
}

// Damage, salvage, the copy choice and version refusal on a 256 x 96 image
// of six segments (FORMAT.md sections 3.4 and 10).
void damage_case(stx_cng::CngDecoder& decoder) {
    CngImage image{256, 96, CngChannels::bgr};
    Bytes raw = generate(image, Pattern::gradient, 7);
    Bytes file = encode_file(image, raw);
    std::size_t segments = static_cast<std::size_t>(stx_cng::cng_segment_count(image));
    check(segments == 6, "segment grid");
    std::size_t tables = read_u32(file, 28);
    std::size_t mosaic = read_u32(file, 20);
    std::size_t directory = 8 * segments + 4;
    std::size_t tables_at = front_bytes + directory;
    std::size_t mosaic_at = tables_at + tables;
    std::size_t first = mosaic_at + mosaic;
    // The second copies: mosaic, tables, directory, field block.
    std::size_t field_copy_at = file.size() - field_block_bytes;
    std::size_t directory_copy_at = field_copy_at - directory;
    std::size_t tables_copy_at = directory_copy_at - tables;
    std::size_t mosaic_copy_at = tables_copy_at - mosaic;
    std::uint64_t row = tight_row_bytes(image);
    std::vector<std::uint8_t> intact(segments);
    Bytes out(raw.size());

    // A damaged segment and signature: not CNG for the decoder; salvage keeps
    // the other five segments and fills the lost one from the mosaic.
    Bytes damaged = file;
    damaged[first + 3] ^= std::byte{0x10};
    damaged[0] ^= std::byte{0xFF};
    check(decode(decoder, damaged, 1).result.error == CngError::invalid_signature, "a damaged signature is not CNG");
    check(stx_cng::inspect_cng_for_salvage(damaged).error == CngError::none, "salvage does not need the signature");
    stx_cng::CngSalvageResult salvage = decoder.decode_salvage(damaged, out, row, intact);
    check(salvage.error == CngError::none && salvage.segments_lost == 1 && intact[0] == 0 && salvage.mosaic_used,
          "salvage reports the lost segment");
    Bytes means = block_means(raw, image);
    CngImage cells = stx_cng::cng_mosaic_image(image);
    bool filled = true;
    for (std::size_t y = 0; y < image.height; ++y) {
        for (std::size_t x = 0; x < image.width; ++x) {
            for (std::size_t channel = 0; channel < 3; ++channel) {
                bool lost = y < 32 && x < 128;
                std::size_t at = (y * image.width + x) * 3 + channel;
                std::byte want = lost ? means[((y / 16) * cells.width + x / 16) * 3 + channel] : raw[at];
                filled = filled && out[at] == want;
            }
        }
    }
    check(filled, "salvage fills the lost segment from the mosaic");
    damaged[0] ^= std::byte{0xFF};
    check(decode(decoder, damaged, 1).result.error == CngError::invalid_segment, "a damaged segment fails the decoder");
    check(decode(decoder, damaged, 4).result.error == CngError::invalid_segment, "and the threaded decoder");
    check(stx_cng::validate_cng_framing(damaged).error == CngError::crc_mismatch, "and strict framing");

    // Without a mosaic, lost segments take 128 in every channel.
    Bytes no_mosaic = damaged;
    no_mosaic[mosaic_at] ^= std::byte{0x01};
    no_mosaic[mosaic_copy_at] ^= std::byte{0x01};
    salvage = decoder.decode_salvage(no_mosaic, out, row, intact);
    check(salvage.error == CngError::none && !salvage.mosaic_used && salvage.segments_lost == 1, "salvage without a mosaic");
    check(out[0] == std::byte{128} && out[(31 * image.width + 127) * 3 + 2] == std::byte{128}, "the fill without a mosaic is 128");
    Bytes cells_out(static_cast<std::size_t>(stx_cng::cng_sample_bytes(cells)));
    check(decoder.decode_mosaic(no_mosaic, cells_out, tight_row_bytes(cells)).error == CngError::mosaic_unavailable,
          "no intact mosaic copy");
    check(!stx_cng::inspect_cng(no_mosaic).mosaic_available, "inspection reports the mosaic lost");

    // The copy choice: each first copy damaged alone is read from its second
    // copy; strict framing refuses every one.
    std::array<std::size_t, 4> first_copies = {9, front_bytes + 2, tables_at + 1, mosaic_at + 1};
    for (std::size_t at : first_copies) {
        Bytes one = file;
        one[at] ^= std::byte{0x04};
        Decoded decoded = decode(decoder, one, 2);
        check(decoded.result.error == CngError::none && decoded.samples == raw, "a damaged first copy is read from the second");
        check(stx_cng::validate_cng_framing(one).error == CngError::crc_mismatch, "strict framing refuses a damaged first copy");
    }
    std::array<std::size_t, 4> second_copies = {field_copy_at + 2, directory_copy_at + 1, tables_copy_at + 1, mosaic_copy_at + 1};
    for (std::size_t at : second_copies) {
        Bytes one = file;
        one[at] ^= std::byte{0x04};
        Decoded decoded = decode(decoder, one, 1);
        check(decoded.result.error == CngError::none && decoded.samples == raw, "a damaged second copy is not read");
        check(stx_cng::validate_cng_framing(one).error == CngError::copy_mismatch, "strict framing compares the second copies");
    }

    // Another format version, with both header copies and their CRC made to
    // match.
    Bytes version = file;
    version[17] = std::byte{2};
    Bytes fields = slice(version, 8, field_block_bytes);
    write_u32(fields, 28, crc_of(slice(fields, 0, 28)));
    write_field_blocks(version, fields);
    check(decode(decoder, version, 1).result.error == CngError::unsupported_version, "another version is refused");
    check(stx_cng::inspect_cng(version).error == CngError::unsupported_version, "inspection refuses another version");
}

void encoder_refusals(stx_cng::CngEncoder& encoder) {
    CngImage image{4, 4, CngChannels::bgr};
    Bytes raw = generate(image, Pattern::gradient, 1);
    Bytes destination(static_cast<std::size_t>(stx_cng::cng_encoded_bound(image)));
    stx_cng::CngEncodeInput input{image, 12, raw, 1};
    CngImage bad_channels{4, 4, static_cast<CngChannels>(2)};
    CngImage no_width{0, 4, CngChannels::bgr};
    CngImage too_many{65536, 32769, CngChannels::bgr};
    for (CngImage inadmissible : {bad_channels, no_width, too_many}) {
        stx_cng::CngEncodeInput shape = input;
        shape.image = inadmissible;
        check(encoder.encode(shape, destination).error == CngError::invalid_argument, "an inadmissible shape");
        check(stx_cng::cng_encoded_bound(inadmissible) == 0 && stx_cng::cng_sample_bytes(inadmissible) == 0, "no bound outside the format");
    }
    for (std::uint32_t threads : {0U, stx_cng::maximum_threads + 1}) {
        stx_cng::CngEncodeInput count = input;
        count.threads = threads;
        check(encoder.encode(count, destination).error == CngError::invalid_argument, "a thread count outside 1 to 64");
    }
    stx_cng::CngEncodeInput narrow = input;
    narrow.row_bytes = 11;
    check(encoder.encode(narrow, destination).error == CngError::invalid_argument, "a stride below the row");
    stx_cng::CngEncodeInput short_samples = input;
    short_samples.samples = std::span<const std::byte>(raw.data(), raw.size() - 1);
    check(encoder.encode(short_samples, destination).error == CngError::invalid_argument, "short samples");
    stx_cng::CngEncodeResult whole = encoder.encode(input, destination);
    check(whole.error == CngError::none, "the reference encode");
    Bytes small(whole.encoded_bytes - 1);
    stx_cng::CngEncodeResult refused = encoder.encode(input, small);
    check(refused.error == CngError::insufficient_storage && refused.required_bytes == whole.encoded_bytes && refused.encoded_bytes == 0,
          "a short destination reports the size it needs");
    Bytes exact(whole.encoded_bytes);
    check(encoder.encode(input, exact).encoded_bytes == whole.encoded_bytes, "an exact destination suffices");
    Bytes shared(raw.size() + destination.size());
    std::copy(raw.begin(), raw.end(), shared.begin());
    stx_cng::CngEncodeInput overlapping = input;
    overlapping.samples = std::span<const std::byte>(shared.data(), raw.size());
    std::span<std::byte> over(shared.data() + raw.size() - 1, destination.size());
    check(encoder.encode(overlapping, over).error == CngError::overlap, "overlapping samples and destination");
}

void decoder_refusals(stx_cng::CngDecoder& decoder) {
    CngImage image{40, 40, CngChannels::bgra};
    Bytes raw = generate(image, Pattern::alpha_steps, 2);
    Bytes file = encode_file(image, raw);
    std::uint64_t row = tight_row_bytes(image);
    Bytes out(raw.size());
    Bytes garbage(100, std::byte{7});
    check(decoder.decode(garbage, out, row, 1).error == CngError::invalid_signature, "not a CNG file");
    check(decoder.decode(std::span<const std::byte>(), out, row, 1).error == CngError::invalid_signature, "an empty file");
    // The decoder reads the header from either end, so a prefix has no
    // header copy at its end; strict framing names the truncation.
    std::span<const std::byte> prefix(file.data(), 60);
    check(decoder.decode(prefix, out, row, 1).error == CngError::invalid_header, "a truncated file");
    check(stx_cng::validate_cng_framing(prefix).error == CngError::truncated, "strict framing reports a truncation");
    Bytes trailing = file;
    trailing.push_back(std::byte{0});
    check(stx_cng::validate_cng_framing(trailing).error == CngError::trailing_data, "strict framing refuses trailing data");
    check(decoder.decode(file, std::span<std::byte>(out.data(), out.size() - 1), row, 1).error == CngError::insufficient_storage,
          "a short destination");
    check(decoder.decode(file, out, row - 1, 1).error == CngError::invalid_argument, "a stride below the row");
    for (std::uint32_t threads : {0U, stx_cng::maximum_threads + 1}) {
        check(decoder.decode(file, out, row, threads).error == CngError::invalid_argument, "a thread count outside 1 to 64");
    }
    Bytes shared = file;
    shared.resize(file.size() + out.size());
    std::span<const std::byte> encoded(shared.data(), file.size());
    std::span<std::byte> inside(shared.data() + file.size() - 4, out.size());
    check(decoder.decode(encoded, inside, row, 1).error == CngError::overlap, "overlapping file and destination");
    std::vector<std::uint8_t> no_report;
    check(decoder.decode_salvage(file, out, row, no_report).error == CngError::insufficient_storage, "a short segment report");
    // A single row admits any stride: only width * channels bytes are needed.
    CngImage line{50, 1, CngChannels::bgr};
    Bytes line_raw = generate(line, Pattern::noise, 9);
    Bytes line_file = encode_file(line, line_raw);
    Bytes line_out(line_raw.size());
    std::uint64_t widest = std::numeric_limits<std::uint64_t>::max();
    check(decoder.decode(line_file, line_out, widest, 1).error == CngError::none && line_out == line_raw, "a single row's stride");
    check(decoder.decode(file, out, widest, 1).error == CngError::insufficient_storage, "a huge stride does not overflow");
}

} // namespace

int main() {
    std::unique_ptr<stx_cng::CngEncoder> encoder = std::make_unique<stx_cng::CngEncoder>();
    std::unique_ptr<stx_cng::CngDecoder> decoder = std::make_unique<stx_cng::CngDecoder>();
    unsigned cases = 0;
    for (const Size& size : sizes) {
        for (CngChannels channels : {CngChannels::bgr, CngChannels::bgra}) {
            for (Pattern pattern : patterns) {
                if (pattern == Pattern::alpha_steps && channels == CngChannels::bgr) {
                    continue;
                }
                image_case(*encoder, *decoder, CngImage{size.width, size.height, channels}, pattern);
                ++cases;
            }
        }
    }
    strided_case(*encoder, *decoder, CngImage{200, 70, CngChannels::bgra});
    strided_case(*encoder, *decoder, CngImage{129, 33, CngChannels::bgr});
    thread_counts(*encoder, *decoder);
    storage_reuse();
    damage_case(*decoder);
    encoder_refusals(*encoder);
    decoder_refusals(*decoder);
    std::printf("stx_cng core: %u image cases\n", cases);
    return finish("stx_cng_core");
}
