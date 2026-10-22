#include "png_color.hpp"

#include "gui_forms/engine/engine_math/engine_math.hpp"

#include <algorithm>
#include <cmath>
#include <new>
#include <stdexcept>

// Clang contracts a*b + c into a fused multiply-add by default on arm64 and
// not on baseline x86-64; every host must compute the same bits.
#pragma clang fp contract(off)

namespace gui_forms::image {
namespace {

constexpr std::uint32_t srgb = 0x73524742U;
constexpr std::uint32_t gama = 0x67414d41U;
constexpr std::uint32_t chrm = 0x6348524dU;
constexpr std::uint32_t iccp = 0x69434350U;
constexpr std::uint32_t cicp = 0x63494350U;
constexpr std::uint32_t plte = 0x504c5445U;
constexpr std::uint32_t idat = 0x49444154U;

// libpng's accepted gAMA range, kept so formerly accepted files keep their gamma.
constexpr std::uint32_t gamma_minimum = 16U;
constexpr std::uint32_t gamma_maximum = 625000000U;

// sRGB primaries and D65 white, as cHRM stores them.
constexpr std::array<std::uint32_t, 8> srgb_chromaticities{
    31270U, 32900U, 64000U, 33000U, 30000U, 60000U, 15000U, 6000U};

using Vector3 = std::array<double, 3>;
// Row-major: rows[r][c].
struct Matrix3 final {
    std::array<Vector3, 3> rows{};
};

std::uint32_t read32(std::span<const std::byte> data, std::size_t offset) noexcept {
    const std::uint32_t value = (std::to_integer<std::uint32_t>(data[offset]) << 24U) |
        (std::to_integer<std::uint32_t>(data[offset + 1U]) << 16U) |
        (std::to_integer<std::uint32_t>(data[offset + 2U]) << 8U) |
        std::to_integer<std::uint32_t>(data[offset + 3U]);
    return value;
}

// Data offset of the first PLTE or IDAT chunk. Requires a file the decoder
// accepted, so every chunk envelope lies inside the span.
std::size_t first_image_chunk(std::span<const std::byte> encoded) noexcept {
    std::size_t offset = 8U;
    while (offset < encoded.size()) {
        const std::uint32_t length = read32(encoded, offset);
        const std::uint32_t type = read32(encoded, offset + 4U);
        if (type == plte || type == idat) {
            const std::size_t data_offset = offset + 8U;
            return data_offset;
        }
        offset += static_cast<std::size_t>(length) + 12U;
    }
    return encoded.size();
}

Vector3 multiply(const Matrix3& m, const Vector3& v) noexcept {
    Vector3 result{};
    for (std::size_t row = 0; row < 3U; ++row) {
        const double first = m.rows[row][0] * v[0];
        const double second = m.rows[row][1] * v[1];
        const double third = m.rows[row][2] * v[2];
        result[row] = first + second + third;
    }
    return result;
}

Matrix3 multiply(const Matrix3& a, const Matrix3& b) noexcept {
    Matrix3 result{};
    for (std::size_t row = 0; row < 3U; ++row) {
        for (std::size_t column = 0; column < 3U; ++column) {
            const double first = a.rows[row][0] * b.rows[0][column];
            const double second = a.rows[row][1] * b.rows[1][column];
            const double third = a.rows[row][2] * b.rows[2][column];
            result.rows[row][column] = first + second + third;
        }
    }
    return result;
}

double determinant(const Matrix3& m) noexcept {
    const double a = m.rows[1][1] * m.rows[2][2] - m.rows[1][2] * m.rows[2][1];
    const double b = m.rows[1][0] * m.rows[2][2] - m.rows[1][2] * m.rows[2][0];
    const double c = m.rows[1][0] * m.rows[2][1] - m.rows[1][1] * m.rows[2][0];
    const double result = m.rows[0][0] * a - m.rows[0][1] * b + m.rows[0][2] * c;
    return result;
}

// Cofactor inverse; the caller has rejected a determinant near zero.
Matrix3 inverse(const Matrix3& m, double det) noexcept {
    Matrix3 result{};
    const std::array<std::array<double, 3>, 3>& r = m.rows;
    result.rows[0][0] = (r[1][1] * r[2][2] - r[1][2] * r[2][1]) / det;
    result.rows[0][1] = (r[0][2] * r[2][1] - r[0][1] * r[2][2]) / det;
    result.rows[0][2] = (r[0][1] * r[1][2] - r[0][2] * r[1][1]) / det;
    result.rows[1][0] = (r[1][2] * r[2][0] - r[1][0] * r[2][2]) / det;
    result.rows[1][1] = (r[0][0] * r[2][2] - r[0][2] * r[2][0]) / det;
    result.rows[1][2] = (r[0][2] * r[1][0] - r[0][0] * r[1][2]) / det;
    result.rows[2][0] = (r[1][0] * r[2][1] - r[1][1] * r[2][0]) / det;
    result.rows[2][1] = (r[0][1] * r[2][0] - r[0][0] * r[2][1]) / det;
    result.rows[2][2] = (r[0][0] * r[1][1] - r[0][1] * r[1][0]) / det;
    return result;
}

// XYZ of a chromaticity with Y = 1.
Vector3 xyz_of(std::uint32_t x_value, std::uint32_t y_value) noexcept {
    const double x = static_cast<double>(x_value) / 100000.0;
    const double y = static_cast<double>(y_value) / 100000.0;
    const Vector3 result{x / y, 1.0, (1.0 - x - y) / y};
    return result;
}

// RGB to XYZ for primaries and white, or false when they are degenerate.
bool primaries_to_xyz(const std::array<std::uint32_t, 8>& values, Matrix3& output,
                      Vector3& white) noexcept {
    for (std::size_t index = 0; index < values.size(); index += 2U) {
        const std::uint64_t x = values[index];
        const std::uint64_t y = values[index + 1U];
        if (y == 0U || x + y > 100000U) return false;
    }
    white = xyz_of(values[0], values[1]);
    Matrix3 primaries{};
    for (std::size_t column = 0; column < 3U; ++column) {
        const Vector3 primary = xyz_of(values[2U + column * 2U], values[3U + column * 2U]);
        for (std::size_t row = 0; row < 3U; ++row) primaries.rows[row][column] = primary[row];
    }
    const double det = determinant(primaries);
    if (!(std::fabs(det) > 1e-9)) return false;
    const Vector3 scale = multiply(inverse(primaries, det), white);
    // A white outside the primaries' triangle needs a negative primary.
    if (!(scale[0] > 0.0) || !(scale[1] > 0.0) || !(scale[2] > 0.0)) return false;
    for (std::size_t row = 0; row < 3U; ++row) {
        for (std::size_t column = 0; column < 3U; ++column) {
            primaries.rows[row][column] = primaries.rows[row][column] * scale[column];
        }
    }
    output = primaries;
    return true;
}

// Bradford chromatic adaptation from source white to destination white.
Matrix3 bradford(const Vector3& source, const Vector3& destination) noexcept {
    const Matrix3 cone{{{{0.8951, 0.2664, -0.1614}, {-0.7502, 1.7135, 0.0367},
                          {0.0389, -0.0685, 1.0296}}}};
    const Matrix3 cone_inverse = inverse(cone, determinant(cone));
    const Vector3 from = multiply(cone, source);
    const Vector3 to = multiply(cone, destination);
    Matrix3 gain{};
    for (std::size_t index = 0; index < 3U; ++index) gain.rows[index][index] = to[index] / from[index];
    const Matrix3 result = multiply(cone_inverse, multiply(gain, cone));
    return result;
}

double srgb_decode(double encoded) noexcept {
    if (encoded <= 0.04045) {
        const double linear = encoded / 12.92;
        return linear;
    }
    const double base = (encoded + 0.055) / 1.055;
    const double linear = engine_power(base, 2.4);
    return linear;
}

// thresholds[k] is the linear value halfway, in the encoded domain, between
// codes k and k + 1: srgb_decode((k + 0.5) / 65535). Strictly increasing.
// Built once, immutable afterwards; thread-safe static initialization.
struct EncodeThresholds final {
    std::vector<double> values;
    EncodeThresholds() : values(65535U) {
        for (std::size_t code = 0; code < values.size(); ++code) {
            const double midpoint = (static_cast<double>(code) + 0.5) / 65535.0;
            values[code] = srgb_decode(midpoint);
        }
    }
};
const EncodeThresholds& encode_thresholds() {
    static const EncodeThresholds thresholds{};
    return thresholds;
}

std::uint16_t srgb_encode(const std::vector<double>& thresholds, double linear) noexcept {
    // Clip to [0, 1]; NaN cannot arise from finite tables and matrices.
    const double clipped = std::clamp(linear, 0.0, 1.0);
    const std::vector<double>::const_iterator above =
        std::upper_bound(thresholds.begin(), thresholds.end(), clipped);
    const std::uint16_t code = static_cast<std::uint16_t>(above - thresholds.begin());
    return code;
}

} // namespace

PngColorResolution resolve_png_color(std::span<const std::byte> encoded,
    std::span<const PngMetadataRecord> records, bool icc_ignored) noexcept {
    PngColorResolution result{};
    bool saw_srgb = false;
    bool saw_icc = false;
    bool saw_cicp = false;
    PngColorEncoding& colour = result.encoding;
    const std::size_t image_start = first_image_chunk(encoded);
    for (const PngMetadataRecord& record : records) {
        const std::span<const std::byte> data = encoded.subspan(record.encoded_offset, record.encoded_length);
        if (record.type == cicp) {
            // Structure: four bytes, once, before PLTE and IDAT. The decoder
            // treats cICP as an unknown ancillary chunk, so it is checked here.
            if (saw_cicp || data.size() != 4U || record.encoded_offset > image_start) {
                result.error = PngColorError::invalid_color_metadata;
                return result;
            }
            saw_cicp = true;
            const bool srgb_equivalent = data[0] == std::byte{1} && data[1] == std::byte{13} &&
                                         data[2] == std::byte{0} && data[3] == std::byte{1};
            if (!srgb_equivalent) {
                result.error = PngColorError::unsupported_color_profile;
                return result;
            }
        } else if (record.type == iccp) {
            saw_icc = true;
        } else if (record.type == srgb) {
            saw_srgb = true;
        } else if (record.type == gama) {
            colour.gamma = read32(data, 0);
        } else if (record.type == chrm) {
            std::array<std::uint32_t, 8> stored{};
            for (std::size_t index = 0; index < stored.size(); ++index) {
                stored[index] = read32(data, index * 4U);
            }
            colour.chromaticities = stored;
        }
    }
    if (saw_cicp) {
        colour = {};
        colour.source = PngColorSource::srgb_cicp;
        return result;
    }
    if (saw_icc) {
        // A refusing rule never reaches here; an ignored profile is sRGB.
        colour = {};
        colour.source = icc_ignored ? PngColorSource::icc_ignored : PngColorSource::srgb_default;
        return result;
    }
    if (saw_srgb) {
        colour = {};
        colour.source = PngColorSource::srgb_chunk;
        return result;
    }
    if (colour.gamma.has_value() && (*colour.gamma < gamma_minimum || *colour.gamma > gamma_maximum)) {
        colour.gamma.reset();
        colour.ignored_gamma = true;
    }
    if (colour.chromaticities.has_value()) {
        Matrix3 unused{};
        Vector3 white{};
        if (!primaries_to_xyz(*colour.chromaticities, unused, white)) {
            colour.chromaticities.reset();
            colour.ignored_chromaticities = true;
        }
    }
    colour.source = colour.gamma.has_value() || colour.chromaticities.has_value()
        ? PngColorSource::gamma_chromaticity : PngColorSource::srgb_default;
    return result;
}

bool convert_png_samples_to_srgb(const PngColorEncoding& encoding, unsigned depth,
    std::span<std::uint16_t> samples, PngColorWorkspace& work) {
    if (!encoding.converts() || samples.size() % 4U != 0U) return false;
    const std::vector<double>& thresholds = encode_thresholds().values;

    // 1. Decode table over every sample value the image can hold.
    const bool wide = depth == 16U;
    const std::size_t entries = wide ? 65536U : 256U;
    const std::uint32_t step = wide ? 1U : 257U;
    work.linear.resize(entries);
    const bool stated_gamma = encoding.gamma.has_value();
    const double exponent = stated_gamma ? 100000.0 / static_cast<double>(*encoding.gamma) : 0.0;
    for (std::size_t index = 0; index < entries; ++index) {
        const double encoded = static_cast<double>(index * step) / 65535.0;
        work.linear[index] = stated_gamma ? engine_power(encoded, exponent) : srgb_decode(encoded);
    }

    // 2. Gamut matrix, or none.
    bool matrix_step = false;
    Matrix3 to_srgb{};
    if (encoding.chromaticities.has_value()) {
        Matrix3 image_primaries{};
        Vector3 image_white{};
        Matrix3 srgb_primaries{};
        Vector3 d65{};
        const bool image_valid = primaries_to_xyz(*encoding.chromaticities, image_primaries, image_white);
        const bool srgb_valid = primaries_to_xyz(srgb_chromaticities, srgb_primaries, d65);
        if (!image_valid || !srgb_valid) return false;
        const Matrix3 adapted = multiply(bradford(image_white, d65), image_primaries);
        to_srgb = multiply(inverse(srgb_primaries, determinant(srgb_primaries)), adapted);
        matrix_step = true;
    }

    // 3 and 4, chosen before the pixel loop.
    const std::size_t pixel_count = samples.size() / 4U;
    if (!matrix_step) {
        work.direct.resize(entries);
        for (std::size_t index = 0; index < entries; ++index) {
            work.direct[index] = srgb_encode(thresholds, work.linear[index]);
        }
        for (std::size_t pixel = 0; pixel < pixel_count; ++pixel) {
            const std::size_t at = pixel * 4U;
            for (std::size_t channel = 0; channel < 3U; ++channel) {
                const std::size_t index = samples[at + channel] / step;
                samples[at + channel] = work.direct[index];
            }
        }
        return true;
    }
    for (std::size_t pixel = 0; pixel < pixel_count; ++pixel) {
        const std::size_t at = pixel * 4U;
        const Vector3 linear{work.linear[samples[at] / step], work.linear[samples[at + 1U] / step],
                             work.linear[samples[at + 2U] / step]};
        const Vector3 converted = multiply(to_srgb, linear);
        samples[at] = srgb_encode(thresholds, converted[0]);
        samples[at + 1U] = srgb_encode(thresholds, converted[1]);
        samples[at + 2U] = srgb_encode(thresholds, converted[2]);
    }
    return true;
}

} // namespace gui_forms::image
