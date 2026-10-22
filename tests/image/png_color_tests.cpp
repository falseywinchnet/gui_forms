// Stage I6: gAMA and cHRM converted to sRGB by the stated deterministic
// pipeline, chunk precedence, degenerate values and cICP. Reference values
// come from the host's std::pow and the published BT.2087 matrix, with the
// stated tolerance; identity hashes are checked on every CI platform.
#include "../../src/core/host/image/clipboard_png.hpp"
#include "../../src/core/image/png_color.hpp"
#include "../../src/core/image/png_pixels.hpp"
#include "png_test_files.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <span>
#include <stdexcept>
#include <vector>

namespace {
using namespace gui_forms;
using namespace gui_forms_png_files;

void require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

struct Fnv1a final {
    std::uint64_t value = 14695981039346656037ULL;
    void add(std::span<const std::byte> bytes) noexcept {
        for (const std::byte item : bytes) {
            value ^= std::to_integer<std::uint64_t>(item);
            value *= 1099511628211ULL;
        }
    }
};

// Host-library reference curves (the reference may use libm; the code under
// test may not).
double reference_srgb_encode(double linear) {
    const double clipped = std::clamp(linear, 0.0, 1.0);
    if (clipped <= 0.0031308) return clipped * 12.92;
    const double encoded = 1.055 * std::pow(clipped, 1.0 / 2.4) - 0.055;
    return encoded;
}
double reference_srgb_decode(double encoded) {
    if (encoded <= 0.04045) return encoded / 12.92;
    const double linear = std::pow((encoded + 0.055) / 1.055, 2.4);
    return linear;
}
unsigned reference_byte(double encoded) {
    const double scaled = std::clamp(encoded, 0.0, 1.0) * 255.0;
    const unsigned result = static_cast<unsigned>(std::lround(scaled));
    return result;
}

Bytes chromaticities(const std::array<std::uint32_t, 8>& values) {
    Bytes payload{};
    for (const std::uint32_t value : values) put32(payload, value);
    return payload;
}
constexpr std::array<std::uint32_t, 8> srgb_primaries{31270, 32900, 64000, 33000, 30000, 60000, 15000, 6000};
constexpr std::array<std::uint32_t, 8> rec2020_primaries{31270, 32900, 70800, 29200, 17000, 79700, 13100, 4600};

// Opaque RGB8 with every byte value on each channel across rows.
Bytes decode_straight(const Bytes& png, image::PngPixelResult& result,
                      image::PngPixelPurpose purpose = image::PngPixelPurpose::resource) {
    image::PngPixelDecoder decoder{};
    image::PngPixelRequest request{};
    request.target = {image::PixelOrder::rgba, image::PixelAlpha::straight};
    request.purpose = purpose;
    result = decoder.inspect(png, request);
    if (result.error != image::PngPixelError::none) return {};
    Bytes pixels(static_cast<std::size_t>(result.width) * result.height * 4U);
    result = decoder.decode(png, request, pixels, static_cast<std::size_t>(result.width) * 4U);
    return pixels;
}

const Format rgb8{2, 8, 3};
const Format rgb16{2, 16, 3};

// A 256 x 3 image: row 0 a grey ramp, rows 1 and 2 coloured ramps.
Bytes ramp_file(std::span<const Extra> extras) {
    const Bytes png = png_file(256, 3, rgb8, false, extras);
    return png;
}

void check_against(const Bytes& pixels, const Bytes& plain, const char* message,
                   double (*transfer)(double, double), double parameter,
                   const std::array<std::array<double, 3>, 3>* matrix, unsigned tolerance) {
    unsigned worst = 0;
    for (std::size_t at = 0; at < pixels.size(); at += 4U) {
        std::array<double, 3> linear{};
        for (std::size_t channel = 0; channel < 3U; ++channel) {
            const double encoded = std::to_integer<unsigned>(plain[at + channel]) / 255.0;
            linear[channel] = transfer(encoded, parameter);
        }
        std::array<double, 3> converted = linear;
        if (matrix != nullptr) {
            for (std::size_t row = 0; row < 3U; ++row) {
                converted[row] = (*matrix)[row][0] * linear[0] + (*matrix)[row][1] * linear[1] +
                                 (*matrix)[row][2] * linear[2];
            }
        }
        for (std::size_t channel = 0; channel < 3U; ++channel) {
            const unsigned expected = reference_byte(reference_srgb_encode(converted[channel]));
            const unsigned actual = std::to_integer<unsigned>(pixels[at + channel]);
            const unsigned difference = actual > expected ? actual - expected : expected - actual;
            worst = std::max(worst, difference);
        }
        require(pixels[at + 3U] == plain[at + 3U], "alpha changed by colour conversion");
    }
    if (worst > tolerance) {
        std::fprintf(stderr, "%s: worst difference %u\n", message, worst);
        throw std::runtime_error(message);
    }
}

double power_transfer(double encoded, double exponent) {
    const double linear = std::pow(encoded, exponent);
    return linear;
}
double srgb_transfer(double encoded, double) {
    const double linear = reference_srgb_decode(encoded);
    return linear;
}

void reference_values() {
    image::PngPixelResult result{};
    const Bytes plain = decode_straight(ramp_file({}), result);
    require(result.error == image::PngPixelError::none, "plain ramp refused");

    // gAMA 1/2.2 (45455) with no cHRM: sRGB primaries, pure power decode.
    const std::array<Extra, 1> gamma22{{{"gAMA", gamma_payload(45455)}}};
    const Bytes converted22 = decode_straight(ramp_file(gamma22), result);
    require(result.error == image::PngPixelError::none, "gAMA 45455 refused");
    check_against(converted22, plain, "gAMA 45455", power_transfer, 100000.0 / 45455.0, nullptr, 1);
    require(converted22 != plain, "gAMA 45455 not converted");

    // gAMA 1.0: samples are linear light.
    const std::array<Extra, 1> gamma1{{{"gAMA", gamma_payload(100000)}}};
    const Bytes converted1 = decode_straight(ramp_file(gamma1), result);
    require(result.error == image::PngPixelError::none, "gAMA 1.0 refused");
    check_against(converted1, plain, "gAMA 1.0", power_transfer, 1.0, nullptr, 1);

    // Rec.709/sRGB chromaticities with gAMA 45455: the matrix is the identity
    // up to rounding, so the result matches the gAMA-only case within one.
    const std::array<Extra, 2> rec709{{{"gAMA", gamma_payload(45455)},
                                       {"cHRM", chromaticities(srgb_primaries)}}};
    const Bytes converted709 = decode_straight(ramp_file(rec709), result);
    require(result.error == image::PngPixelError::none, "Rec.709 cHRM refused");
    check_against(converted709, plain, "Rec.709 cHRM", power_transfer, 100000.0 / 45455.0, nullptr, 1);

    // Wide gamut: Rec.2020 primaries, D65, gAMA 45455; reference matrix from
    // ITU-R BT.2087 (four decimals, hence a tolerance of two 8-bit codes).
    const std::array<std::array<double, 3>, 3> bt2087{{
        {1.6605, -0.5876, -0.0728}, {-0.1246, 1.1329, -0.0083}, {-0.0182, -0.1006, 1.1187}}};
    const std::array<Extra, 2> rec2020{{{"gAMA", gamma_payload(45455)},
                                        {"cHRM", chromaticities(rec2020_primaries)}}};
    const Bytes converted2020 = decode_straight(ramp_file(rec2020), result);
    require(result.error == image::PngPixelError::none, "Rec.2020 cHRM refused");
    check_against(converted2020, plain, "Rec.2020 cHRM", power_transfer, 100000.0 / 45455.0, &bt2087, 2);

    // cHRM without gAMA assumes the sRGB curve: sRGB primaries then round-trip exactly.
    const std::array<Extra, 1> chroma_only{{{"cHRM", chromaticities(srgb_primaries)}}};
    const Bytes round_trip = decode_straight(ramp_file(chroma_only), result);
    require(result.error == image::PngPixelError::none, "cHRM-only refused");
    check_against(round_trip, plain, "cHRM without gAMA", srgb_transfer, 0.0, nullptr, 1);
}

void precedence_and_degenerate_values() {
    image::PngPixelResult result{};
    const Bytes plain = decode_straight(ramp_file({}), result);
    const Bytes gamma = gamma_payload(100000);
    // sRGB outranks gAMA and cHRM.
    const std::array<Extra, 3> with_srgb{{{"sRGB", Bytes{std::byte{0}}}, {"gAMA", gamma},
                                          {"cHRM", chromaticities(rec2020_primaries)}}};
    require(decode_straight(ramp_file(with_srgb), result) == plain, "sRGB did not take precedence");
    // cICP with sRGB-equivalent values outranks gAMA.
    const std::array<Extra, 2> cicp_srgb{{{"cICP", Bytes{std::byte{1}, std::byte{13}, std::byte{0}, std::byte{1}}},
                                          {"gAMA", gamma}}};
    require(decode_straight(ramp_file(cicp_srgb), result) == plain && result.error == image::PngPixelError::none,
            "sRGB cICP did not take precedence");
    // Any other cICP is refused; a malformed one is invalid.
    const std::array<Extra, 1> cicp_pq{{{"cICP", Bytes{std::byte{9}, std::byte{16}, std::byte{0}, std::byte{1}}}}};
    (void)decode_straight(ramp_file(cicp_pq), result);
    require(result.error == image::PngPixelError::unsupported_color_profile, "BT.2100 PQ cICP accepted");
    const std::array<Extra, 1> cicp_short{{{"cICP", Bytes{std::byte{1}, std::byte{13}, std::byte{0}}}}};
    (void)decode_straight(ramp_file(cicp_short), result);
    require(result.error == image::PngPixelError::invalid_color_metadata, "short cICP accepted");
    // An ignored iCCP (clipboard) means sRGB; gAMA beside it is not applied.
    const std::array<Extra, 2> icc_gamma{{{"iCCP", icc_profile_payload()}, {"gAMA", gamma}}};
    require(decode_straight(ramp_file(icc_gamma), result, image::PngPixelPurpose::clipboard) == plain &&
            result.error == image::PngPixelError::none, "ignored iCCP let gAMA apply");
    // Degenerate values are ignored as if absent.
    const std::array<Extra, 1> tiny_gamma{{{"gAMA", gamma_payload(15)}}};
    require(decode_straight(ramp_file(tiny_gamma), result) == plain, "out-of-range gAMA applied");
    const std::array<std::uint32_t, 8> zero_y{31270, 0, 64000, 33000, 30000, 60000, 15000, 6000};
    const std::array<std::uint32_t, 8> outside{90000, 9000, 64000, 33000, 30000, 60000, 15000, 6000};
    const std::array<std::uint32_t, 8> collinear{31270, 32900, 10000, 10000, 20000, 20000, 30000, 30000};
    for (const std::array<std::uint32_t, 8>* values : {&zero_y, &outside, &collinear}) {
        const std::array<Extra, 2> degenerate{{{"gAMA", gamma}, {"cHRM", chromaticities(*values)}}};
        const Bytes pixels = decode_straight(ramp_file(degenerate), result);
        require(result.error == image::PngPixelError::none, "degenerate cHRM refused");
        // gAMA 1.0 still applies; the primaries fall back to sRGB.
        check_against(pixels, plain, "degenerate cHRM", power_transfer, 1.0, nullptr, 1);
    }
    // The resolver reports what it ignored.
    const std::array<Extra, 2> reported{{{"gAMA", gamma_payload(15)}, {"cHRM", chromaticities(zero_y)}}};
    const Bytes file = ramp_file(reported);
    const image::PngDecodeResult inspected = image::inspect_png_samples(file, {});
    std::vector<image::PngMetadataRecord> records(inspected.layout.metadata_records);
    std::unique_ptr<compression::InflateStream> workspace = std::make_unique<compression::InflateStream>();
    Bytes rows(inspected.layout.row_storage_bytes);
    std::vector<std::uint16_t> samples(inspected.layout.rgba_samples);
    Bytes scratch(64);
    require(image::decode_png_samples_discarding_metadata(file, {}, *workspace, rows, samples, scratch,
            records).error == image::PngDecodeError::none, "degenerate fixture refused");
    const image::PngColorResolution resolution = image::resolve_png_color(file, records, false);
    require(resolution.error == image::PngColorError::none &&
            resolution.encoding.ignored_gamma && resolution.encoding.ignored_chromaticities &&
            resolution.encoding.source == image::PngColorSource::srgb_default, "ignored chunks not reported");
}

// Mismatched records (not from a decode of this file) are refused without
// reading outside the encoded span.
void mismatched_records_are_refused() {
    const Bytes file = ramp_file({});
    std::array<image::PngMetadataRecord, 1> records{};
    records[0].type = 0x67414d41U; // gAMA
    records[0].encoded_offset = file.size() - 2U;
    records[0].encoded_length = 4U;
    require(image::resolve_png_color(file, records, false).error == image::PngColorError::invalid_color_metadata,
            "record past the end was read");
    records[0].encoded_offset = 8U;
    records[0].encoded_length = 3U;
    require(image::resolve_png_color(file, records, false).error == image::PngColorError::invalid_color_metadata,
            "short gAMA record was read");
    const Bytes truncated(file.begin(), file.begin() + 20);
    const std::array<image::PngMetadataRecord, 0> none{};
    require(image::resolve_png_color(truncated, none, false).error == image::PngColorError::none,
            "truncated walk failed");
}

void identity_hashes() {
    std::array<Fnv1a, 3> digests{};
    const std::array<Extra, 1> gamma22{{{"gAMA", gamma_payload(45455)}}};
    const std::array<Extra, 2> rec2020{{{"gAMA", gamma_payload(45455)},
                                        {"cHRM", chromaticities(rec2020_primaries)}}};
    const std::array<Extra, 2> warm{{{"gAMA", gamma_payload(55555)},
        {"cHRM", chromaticities({34567, 35850, 64000, 33000, 30000, 60000, 15000, 6000})}}};
    const std::array<std::span<const Extra>, 3> cases{gamma22, rec2020, warm};
    for (std::size_t index = 0; index < cases.size(); ++index) {
        for (const Format* format : {&rgb8, &rgb16, &formats[14], &formats[7]}) {
            for (const bool interlaced : {false, true}) {
                const Bytes png = png_file(37, 29, *format, interlaced, cases[index]);
                image::PngPixelResult result{};
                const Bytes pixels = decode_straight(png, result);
                require(result.error == image::PngPixelError::none, "identity case refused");
                digests[index].add(pixels);
            }
        }
    }
    constexpr std::array<std::uint64_t, 3> expected{
        0x44de97c1f2726691ULL, 0xf284508a59809f75ULL, 0x66e109f833f56831ULL};
    bool matched = true;
    for (std::size_t index = 0; index < digests.size(); ++index) {
        std::printf("png_color identity case %zu: 0x%016llxULL\n", index,
                    static_cast<unsigned long long>(digests[index].value));
        matched = matched && digests[index].value == expected[index];
    }
    require(matched, "colour conversion identity hash changed");
}

} // namespace

int main() {
    try {
        reference_values();
        precedence_and_degenerate_values();
        mismatched_records_are_refused();
        identity_hashes();
    } catch (const std::exception& error) {
        std::cerr << "png_color_tests: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
