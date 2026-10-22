#include "png_color.hpp"

namespace gui_forms::image {
namespace {

constexpr std::uint32_t srgb = 0x73524742U;
constexpr std::uint32_t gama = 0x67414d41U;
constexpr std::uint32_t chrm = 0x6348524dU;
constexpr std::uint32_t iccp = 0x69434350U;
constexpr std::uint32_t cicp = 0x63494350U;
constexpr std::uint32_t plte = 0x504c5445U;
constexpr std::uint32_t idat = 0x49444154U;

std::uint32_t read32(std::span<const std::byte> data, std::size_t offset) noexcept {
    const std::uint32_t value = (std::to_integer<std::uint32_t>(data[offset]) << 24U) |
        (std::to_integer<std::uint32_t>(data[offset + 1U]) << 16U) |
        (std::to_integer<std::uint32_t>(data[offset + 2U]) << 8U) |
        std::to_integer<std::uint32_t>(data[offset + 3U]);
    return value;
}

// Data offset of the first PLTE or IDAT chunk, or encoded.size() when there
// is none. The decoder already proved the envelopes of an accepted file; an
// envelope that leaves the span ends the walk (the defined fallback), so a
// mismatched argument cannot read outside it.
std::size_t first_image_chunk(std::span<const std::byte> encoded) noexcept {
    std::size_t offset = 8U;
    while (offset <= encoded.size() && encoded.size() - offset >= 12U) {
        const std::uint32_t length = read32(encoded, offset);
        const std::uint32_t type = read32(encoded, offset + 4U);
        if (type == plte || type == idat) {
            const std::size_t data_offset = offset + 8U;
            return data_offset;
        }
        if (length > encoded.size() - offset - 12U) break;
        offset += static_cast<std::size_t>(length) + 12U;
    }
    return encoded.size();
}

// A record names a live range of encoded with its chunk's fixed length.
bool record_fits(std::span<const std::byte> encoded, const PngMetadataRecord& record,
                 std::size_t required) noexcept {
    const bool inside = record.encoded_offset <= encoded.size() &&
                        record.encoded_length <= encoded.size() - record.encoded_offset;
    const bool sized = required == 0U || record.encoded_length == required;
    const bool fits = inside && sized;
    return fits;
}

} // namespace

PngColorResolution resolve_png_color(std::span<const std::byte> encoded,
    std::span<const PngMetadataRecord> records, bool icc_ignored) noexcept {
    PngColorResolution result{};
    bool saw_srgb = false;
    bool saw_icc = false;
    bool saw_cicp = false;
    ColorEncoding& colour = result.encoding;
    const std::size_t image_start = first_image_chunk(encoded);
    for (const PngMetadataRecord& record : records) {
        // Records come from a successful decode of encoded; one that does not
        // fit is the one defined failure, never a read outside the span.
        std::size_t required = 0U;
        if (record.type == gama) required = 4U;
        if (record.type == chrm) required = 32U;
        if (!record_fits(encoded, record, required)) {
            result.error = PngColorError::invalid_color_metadata;
            return result;
        }
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
            colour.has_gamma = true;
        } else if (record.type == chrm) {
            for (std::size_t index = 0; index < colour.chromaticities.size(); ++index) {
                colour.chromaticities[index] = read32(data, index * 4U);
            }
            colour.has_chromaticities = true;
        }
    }
    if (saw_cicp) {
        colour = {};
        colour.source = ColorSource::srgb_declared;
        return result;
    }
    if (saw_icc) {
        // A refusing rule never reaches here; an ignored profile is sRGB.
        colour = {};
        colour.source = icc_ignored ? ColorSource::profile_ignored : ColorSource::srgb_default;
        return result;
    }
    if (saw_srgb) {
        colour = {};
        colour.source = ColorSource::srgb_declared;
        return result;
    }
    // Degenerate gamma or chromaticity values are dropped by the codec-neutral
    // conversion rules, which also choose between conversion and sRGB.
    settle_gamma_chromaticities(colour);
    return result;
}

} // namespace gui_forms::image
