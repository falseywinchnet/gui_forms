// The shared PNG pixel path: the RGBA16 to 8-bit kernel's rounding rule, the
// owned decode through every target layout, the colour-chunk policy, and the
// cross-platform identity hashes. The same expected hashes are checked on
// every CI platform, so a pass there proves byte-identical decoded pixels.
#include "../../src/core/host/image/clipboard_png.hpp"
#include "../../src/core/image/png_pixels.hpp"
#include "png_test_files.hpp"

#include <algorithm>
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

struct IsByte final {
    std::byte expected{};
    bool operator()(std::byte value) const noexcept { return value == expected; }
};

// FNV-1a, 64-bit: a stable digest for identity, not a security property.
struct Fnv1a final {
    std::uint64_t value = 14695981039346656037ULL;
    void add(std::span<const std::byte> bytes) noexcept {
        for (const std::byte item : bytes) {
            value ^= std::to_integer<std::uint64_t>(item);
            value *= 1099511628211ULL;
        }
    }
};

// Independent nearest-integer division for the reference: quotient plus one
// when the remainder exceeds half the divisor (odd divisors have no tie).
std::uint64_t nearest(std::uint64_t numerator, std::uint64_t denominator) {
    const std::uint64_t quotient = numerator / denominator;
    const std::uint64_t remainder = numerator % denominator;
    const std::uint64_t result = remainder * 2U > denominator ? quotient + 1U : quotient;
    return result;
}

constexpr std::array<image::PixelTarget, 4> targets{{
    {image::PixelOrder::rgba, image::PixelAlpha::straight},
    {image::PixelOrder::rgba, image::PixelAlpha::premultiplied},
    {image::PixelOrder::bgra, image::PixelAlpha::straight},
    {image::PixelOrder::bgra, image::PixelAlpha::premultiplied}}};

Bytes decode_pixels(image::PngPixelDecoder& decoder, const Bytes& png,
                    const image::PngPixelRequest& request, image::PngPixelResult& result) {
    result = decoder.inspect(png, request);
    if (result.error != image::PngPixelError::none) return {};
    const std::size_t row_bytes = static_cast<std::size_t>(result.width) * 4U;
    Bytes pixels(row_bytes * result.height, std::byte{0xa5});
    result = decoder.decode(png, request, pixels, row_bytes);
    return pixels;
}

void kernel_rounding_is_exact() {
    // Every 8-bit colour/alpha pair, expanded as the decoder stores it.
    std::vector<std::uint16_t> samples(256U * 256U * 4U);
    for (unsigned alpha = 0; alpha < 256; ++alpha) {
        for (unsigned value = 0; value < 256; ++value) {
            const std::size_t at = (static_cast<std::size_t>(alpha) * 256U + value) * 4U;
            samples[at] = static_cast<std::uint16_t>(value * 257U);
            samples[at + 1U] = static_cast<std::uint16_t>((255U - value) * 257U);
            samples[at + 2U] = static_cast<std::uint16_t>(value * 257U);
            samples[at + 3U] = static_cast<std::uint16_t>(alpha * 257U);
        }
    }
    Bytes straight(samples.size());
    Bytes premultiplied(samples.size());
    require(image::convert_rgba16_samples(samples, 256, 256, targets[0], straight, 1024),
            "straight conversion refused");
    require(image::convert_rgba16_samples(samples, 256, 256, targets[1], premultiplied, 1024),
            "premultiplied conversion refused");
    for (unsigned alpha = 0; alpha < 256; ++alpha) {
        for (unsigned value = 0; value < 256; ++value) {
            const std::size_t at = (static_cast<std::size_t>(alpha) * 256U + value) * 4U;
            require(std::to_integer<unsigned>(straight[at]) == value, "8-bit straight value changed");
            require(std::to_integer<unsigned>(straight[at + 3U]) == alpha, "8-bit alpha changed");
            const std::uint64_t expected = nearest(static_cast<std::uint64_t>(value) * alpha, 255U);
            require(std::to_integer<unsigned>(premultiplied[at]) == expected, "8-bit premultiply not nearest");
            require(std::to_integer<unsigned>(premultiplied[at + 3U]) == alpha, "premultiplied alpha changed");
        }
    }
    // Sixteen-bit samples on a deterministic lattice that includes the extremes.
    std::vector<std::uint16_t> wide{};
    for (std::uint32_t step = 0; step < 4096; ++step) {
        const std::uint32_t colour = (step * 40503U) & 65535U;
        const std::uint32_t alpha = step == 0 ? 0U : (step == 1 ? 65535U : (step * 25717U) & 65535U);
        wide.push_back(static_cast<std::uint16_t>(colour));
        wide.push_back(static_cast<std::uint16_t>(65535U - colour));
        wide.push_back(static_cast<std::uint16_t>((colour * 3U) & 65535U));
        wide.push_back(static_cast<std::uint16_t>(alpha));
    }
    Bytes wide_straight(wide.size());
    Bytes wide_premultiplied(wide.size());
    require(image::convert_rgba16_samples(wide, 64, 64, targets[0], wide_straight, 256),
            "16-bit straight conversion refused");
    require(image::convert_rgba16_samples(wide, 64, 64, targets[1], wide_premultiplied, 256),
            "16-bit premultiplied conversion refused");
    for (std::size_t at = 0; at < wide.size(); at += 4U) {
        const std::uint64_t alpha = wide[at + 3U];
        for (std::size_t channel = 0; channel < 3U; ++channel) {
            const std::uint64_t value = wide[at + channel];
            require(std::to_integer<std::uint64_t>(wide_straight[at + channel]) ==
                    nearest(value * 255U, 65535U), "16-bit straight not nearest");
            require(std::to_integer<std::uint64_t>(wide_premultiplied[at + channel]) ==
                    nearest(value * alpha * 255U, 65535ULL * 65535ULL), "16-bit premultiply not nearest");
        }
        require(std::to_integer<std::uint64_t>(wide_straight[at + 3U]) == nearest(alpha * 255U, 65535U),
                "16-bit alpha not nearest");
    }
}

void kernel_layout_and_refusals() {
    const std::array<std::uint16_t, 8> two{
        static_cast<std::uint16_t>(10U * 257U), static_cast<std::uint16_t>(20U * 257U),
        static_cast<std::uint16_t>(30U * 257U), 65535U,
        static_cast<std::uint16_t>(200U * 257U), static_cast<std::uint16_t>(100U * 257U),
        static_cast<std::uint16_t>(50U * 257U), static_cast<std::uint16_t>(128U * 257U)};
    // One pixel per row with four padding bytes that must stay untouched.
    std::array<std::byte, 12> padded{};
    std::fill(padded.begin(), padded.end(), std::byte{0x77});
    require(image::convert_rgba16_samples(two, 1, 2, targets[3], padded, 8), "padded rows refused");
    const std::array<unsigned, 12> expected{30, 20, 10, 255, 0x77, 0x77, 0x77, 0x77, 25, 50, 100, 128};
    for (std::size_t index = 0; index < expected.size(); ++index) {
        require(std::to_integer<unsigned>(padded[index]) == expected[index], "BGRA premultiplied layout");
    }
    std::array<std::byte, 8> tight{};
    require(!image::convert_rgba16_samples(two, 2, 1, targets[0], tight, 4), "short row accepted");
    require(!image::convert_rgba16_samples(two, 1, 2, targets[0], std::span<std::byte>(tight).first(7), 4),
            "short destination accepted");
    require(!image::convert_rgba16_samples(std::span<const std::uint16_t>(two).first(7), 1, 2,
            targets[0], tight, 4), "short samples accepted");
    require(!image::convert_rgba16_samples(two, 0, 1, targets[0], tight, 4), "empty image accepted");
    const image::PixelTarget invalid{static_cast<image::PixelOrder>(7), image::PixelAlpha::straight};
    require(!image::convert_rgba16_samples(two, 1, 2, invalid, tight, 4), "invalid order accepted");
    std::array<std::uint16_t, 8> shared{};
    const std::span<std::byte> aliased = std::as_writable_bytes(std::span<std::uint16_t>(shared));
    require(!image::convert_rgba16_samples(shared, 1, 2, targets[0], aliased, 4), "aliased storage accepted");
}

void decode_identity_hashes() {
    constexpr std::array<std::uint32_t, 6> sizes{1, 3, 8, 13, 33, 64};
    image::PngPixelDecoder decoder{};
    std::array<Fnv1a, 4> digests{};
    std::size_t images = 0;
    for (const Format& format : formats) {
        for (const bool interlaced : {false, true}) {
            for (const std::uint32_t width : sizes) {
                const std::uint32_t height = sizes[(images + width) % sizes.size()];
                const Bytes png = png_file(width, height, format, interlaced, {});
                for (std::size_t target = 0; target < targets.size(); ++target) {
                    image::PngPixelRequest request{};
                    request.target = targets[target];
                    image::PngPixelResult result{};
                    const Bytes pixels = decode_pixels(decoder, png, request, result);
                    require(result.error == image::PngPixelError::none, "valid matrix image refused");
                    require(result.width == width && result.height == height, "dimensions changed");
                    digests[target].add(pixels);
                }
                ++images;
            }
        }
    }
    require(images == 180U, "identity corpus size changed");
    // Expected digests over the 180-image corpus for each target, in the order
    // of targets. A change here is a change to decoded pixels on every host.
    constexpr std::array<std::uint64_t, 4> expected{
        0x9b21844a8797129dULL, 0xf28a34013f2df6c5ULL,
        0x23609b7cffc3e33dULL, 0x8a3c5e0b244e1ae5ULL};
    bool matched = true;
    for (std::size_t target = 0; target < targets.size(); ++target) {
        std::printf("png_pixels identity target %zu: 0x%016llxULL\n", target,
                    static_cast<unsigned long long>(digests[target].value));
        matched = matched && digests[target].value == expected[target];
    }
    require(matched, "decoded pixel identity hash changed");
}

void colour_chunk_policy() {
    const Format rgba{6, 8, 4};
    const Bytes plain = png_file(9, 7, rgba, false, {});
    image::PngPixelDecoder decoder{};
    image::PngPixelRequest request{};
    image::PngPixelResult result{};
    const Bytes reference = decode_pixels(decoder, plain, request, result);
    require(result.error == image::PngPixelError::none, "plain image refused");

    // An sRGB chunk passes the samples through unchanged; gAMA and cHRM
    // conversion is covered by gui_forms_png_color_tests.
    const std::array<Extra, 1> srgb{{{"sRGB", Bytes{std::byte{0}}}}};
    const Bytes srgb_file = png_file(9, 7, rgba, false, srgb);
    const Bytes srgb_pixels = decode_pixels(decoder, srgb_file, request, result);
    require(result.error == image::PngPixelError::none && srgb_pixels == reference,
            "sRGB chunk changed decoded pixels");

    // Decision D1, option 2: the one switch point, then each side of it.
    require(image::icc_profile_rule(image::PngPixelPurpose::resource) == image::IccProfileRule::refuse &&
            image::icc_profile_rule(image::PngPixelPurpose::drawing_decode) == image::IccProfileRule::refuse &&
            image::icc_profile_rule(image::PngPixelPurpose::clipboard) == image::IccProfileRule::ignore,
            "iCCP rule table changed");
    const std::array<Extra, 1> icc{{{"iCCP", icc_profile_payload()}}};
    const Bytes profiled = png_file(9, 7, rgba, false, icc);
    for (const image::PngPixelPurpose purpose :
         {image::PngPixelPurpose::resource, image::PngPixelPurpose::drawing_decode}) {
        request.purpose = purpose;
        result = decoder.inspect(profiled, request);
        require(result.error == image::PngPixelError::unsupported_color_profile, "iCCP inspected as decodable");
        Bytes pixels(9U * 7U * 4U, std::byte{0x11});
        result = decoder.decode(profiled, request, pixels, 36);
        require(result.error == image::PngPixelError::unsupported_color_profile, "iCCP decoded by refusing purpose");
        require(std::all_of(pixels.begin(), pixels.end(), IsByte{std::byte{0x11}}),
                "refused iCCP wrote pixels");
    }
    request.purpose = image::PngPixelPurpose::clipboard;
    const Bytes ignored = decode_pixels(decoder, profiled, request, result);
    require(result.error == image::PngPixelError::none && ignored == reference,
            "clipboard iCCP did not decode as sRGB samples");
}

// The pixel path's metadata mode: compressed fields are validated once
// through bounded scratch and discarded; a malformed one still fails.
void metadata_is_validated_not_stored() {
    const Format rgba{6, 8, 4};
    Bytes text{std::byte{'k'}, std::byte{0}, std::byte{0}};
    const Bytes body = deflate(Bytes(20000, std::byte{'t'}));
    text.insert(text.end(), body.begin(), body.end());
    const std::array<Extra, 2> extras{{{"zTXt", text}, {"tEXt", Bytes{std::byte{'a'}, std::byte{0}, std::byte{'b'}}}}};
    const Bytes png = png_file(5, 5, rgba, false, extras);
    const image::PngDecodeResult inspected = image::inspect_png_samples(png, {});
    require(inspected.error == image::PngDecodeError::none && inspected.layout.metadata_records == 2,
            "metadata fixture refused");
    std::unique_ptr<compression::InflateStream> workspace = std::make_unique<compression::InflateStream>();
    Bytes rows(inspected.layout.row_storage_bytes);
    std::vector<std::uint16_t> samples(inspected.layout.rgba_samples);
    Bytes scratch(64);
    std::vector<image::PngMetadataRecord> records(2);
    const image::PngDecodeResult decoded = image::decode_png_samples_discarding_metadata(
        png, {}, *workspace, rows, samples, scratch, records);
    require(decoded.error == image::PngDecodeError::none, "discarding decode refused valid metadata");
    require(decoded.metadata_bytes == 20000U, "validated metadata size");
    require(records[0].compressed && records[0].decoded_length == 0U && records[0].decoded_offset == 0U,
            "discarded record carries decoded storage");
    require(!records[1].compressed && records[1].encoded_length == 3U, "uncompressed record");

    // The same samples as the storing decode.
    Bytes stored(20000);
    std::vector<std::uint16_t> stored_samples(inspected.layout.rgba_samples);
    std::vector<image::PngMetadataRecord> stored_records(2);
    const image::PngDecodeResult full = image::decode_png_samples(
        png, {}, *workspace, rows, stored_samples, stored, stored_records);
    require(full.error == image::PngDecodeError::none && stored_samples == samples, "modes disagree");

    // A NUL inside compressed Latin-1 text is refused without storing it.
    Bytes bad{std::byte{'k'}, std::byte{0}, std::byte{0}};
    Bytes bad_text(300, std::byte{'t'});
    bad_text[200] = std::byte{0};
    const Bytes bad_body = deflate(bad_text);
    bad.insert(bad.end(), bad_body.begin(), bad_body.end());
    const std::array<Extra, 1> bad_extras{{{"zTXt", bad}}};
    const Bytes bad_png = png_file(5, 5, rgba, false, bad_extras);
    std::vector<image::PngMetadataRecord> bad_records(1);
    const image::PngDecodeResult refused = image::decode_png_samples_discarding_metadata(
        bad_png, {}, *workspace, rows, samples, scratch, bad_records);
    require(refused.error == image::PngDecodeError::invalid_metadata, "compressed NUL accepted");

    // Image and metadata share one work quota.
    image::PngDecodeLimits tight{};
    tight.inflate_work.value = decoded.inflate_work.value - 1U;
    const image::PngDecodeResult starved = image::decode_png_samples_discarding_metadata(
        png, tight, *workspace, rows, samples, scratch, records);
    require(starved.error == image::PngDecodeError::inflate_failure, "work quota not shared");
    const image::PngDecodeResult empty_scratch = image::decode_png_samples_discarding_metadata(
        png, {}, *workspace, rows, samples, {}, records);
    require(empty_scratch.error == image::PngDecodeError::invalid_argument, "empty scratch accepted");
}

// Owner decision D1, option 2, on the clipboard side: every host's clipboard
// read decodes a profile-tagged PNG, treating its samples as sRGB, into the
// straight-alpha RGBA8 host-service contract.
void clipboard_reads_ignore_profile() {
    const Format rgba{6, 8, 4};
    const Bytes plain = png_file(9, 7, rgba, false, {});
    const std::array<Extra, 1> icc{{{"iCCP", icc_profile_payload()}}};
    const Bytes profiled = png_file(9, 7, rgba, false, icc);
    const HostClipboardImageResult reference = detail::decode_clipboard_png(plain, 0);
    const HostClipboardImageResult read = detail::decode_clipboard_png(profiled, 0);
    require(reference.image() != nullptr && reference.status().error == HostServiceError::none, "clipboard PNG refused");
    require(read.image() != nullptr && read.status().error == HostServiceError::none, "clipboard refused an iCCP PNG");
    require((*read.image()).width == 9U && (*read.image()).height == 7U && (*read.image()).row_bytes == 36U,
            "clipboard image shape");
    require((*read.image()).pixels == (*reference.image()).pixels, "clipboard iCCP changed samples");
    // Straight alpha, as the host contract states.
    for (std::uint32_t y = 0; y < 7U; ++y) {
        for (std::uint32_t x = 0; x < 9U; ++x) {
            const std::size_t at = (static_cast<std::size_t>(y) * 9U + x) * 4U;
            for (unsigned channel = 0; channel < 4U; ++channel) {
                require(std::to_integer<unsigned>((*read.image()).pixels[at + channel]) ==
                        sample_at(x, y, channel, rgba), "clipboard sample is not straight RGBA8");
            }
        }
    }
    Bytes corrupt = plain;
    corrupt[20] ^= std::byte{1};
    const HostClipboardImageResult refused = detail::decode_clipboard_png(corrupt, 0);
    require(refused.image() == nullptr && refused.status().error == HostServiceError::invalid_argument,
            "corrupt clipboard PNG accepted");
    require(detail::decode_clipboard_png({}, 0).image() == nullptr, "empty clipboard PNG accepted");
}

void decode_failures() {
    const Format rgba{6, 8, 4};
    Bytes png = png_file(4, 4, rgba, false, {});
    image::PngPixelDecoder decoder{};
    image::PngPixelRequest request{};
    Bytes pixels(64);
    image::PngPixelResult result = decoder.decode(png, request, pixels, 16);
    require(result.error == image::PngPixelError::none, "valid image refused");
    result = decoder.decode(png, request, std::span<std::byte>(pixels).first(63), 16);
    require(result.error == image::PngPixelError::insufficient_storage, "short destination accepted");
    result = decoder.decode(png, request, pixels, 15);
    require(result.error == image::PngPixelError::insufficient_storage, "short stride accepted");
    request.limits.dimension = 3;
    result = decoder.decode(png, request, pixels, 16);
    require(result.error == image::PngPixelError::decode_failed &&
            result.decode_error == image::PngDecodeError::resource_limit, "dimension limit ignored");
    request.limits = {};
    png[png.size() - 20U] ^= std::byte{1};
    result = decoder.decode(png, request, pixels, 16);
    require(result.error == image::PngPixelError::decode_failed &&
            result.decode_error == image::PngDecodeError::crc_mismatch, "corrupt IDAT accepted");
    decoder.release();
    png[png.size() - 20U] ^= std::byte{1};
    result = decoder.decode(png, request, pixels, 16);
    require(result.error == image::PngPixelError::none, "decode after release failed");
    const std::span<std::byte> overlapping(png);
    result = decoder.decode(png, request, overlapping, 16);
    require(result.error == image::PngPixelError::invalid_argument, "overlapping destination accepted");
}

void limits_for_image() {
    const image::PngDecodeLimits limits = image::png_limits_for_image(300, 200, 5000);
    require(limits.dimension == 300U && limits.pixels == 60000U && limits.encoded_bytes == 5000U,
            "image limits");
    require(limits.filtered_bytes == 60000U * 8U + 7U * 200U, "filtered bound");
    const Format rgba16{6, 16, 4};
    const Bytes png = png_file(300, 200, rgba16, true, {});
    image::PngPixelDecoder decoder{};
    image::PngPixelRequest request{};
    request.limits = image::png_limits_for_image(300, 200, png.size());
    image::PngPixelResult result{};
    const Bytes pixels = decode_pixels(decoder, png, request, result);
    require(result.error == image::PngPixelError::none, "worst-case layout refused by its own limits");
    request.limits = image::png_limits_for_image(299, 200, png.size());
    (void)decode_pixels(decoder, png, request, result);
    require(result.error == image::PngPixelError::decode_failed, "wider image accepted");
}

} // namespace

int main() {
    try {
        kernel_rounding_is_exact();
        kernel_layout_and_refusals();
        decode_identity_hashes();
        colour_chunk_policy();
        metadata_is_validated_not_stored();
        clipboard_reads_ignore_profile();
        decode_failures();
        limits_for_image();
    } catch (const std::exception& error) {
        std::cerr << "png_pixels_tests: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
