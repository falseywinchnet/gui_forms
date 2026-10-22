#include "png_pixels.hpp"

#include <algorithm>
#include <limits>
#include <new>
#include <stdexcept>

namespace gui_forms::image {
namespace {

constexpr std::uint32_t iccp = 0x69434350U;
// Compressed metadata is streamed through this much scratch and discarded.
constexpr std::size_t scratch_bytes = 4096U;
bool overlaps(std::span<const std::byte> left, std::span<const std::byte> right) noexcept {
    if (left.empty() || right.empty()) return false;
    const std::uintptr_t a = reinterpret_cast<std::uintptr_t>(left.data());
    const std::uintptr_t b = reinterpret_cast<std::uintptr_t>(right.data());
    const bool overlap = a <= b ? b - a < left.size() : a - b < right.size();
    return overlap;
}

std::uint32_t read32(std::span<const std::byte> data, std::size_t offset) noexcept {
    const std::uint32_t value = (std::to_integer<std::uint32_t>(data[offset]) << 24U) |
        (std::to_integer<std::uint32_t>(data[offset + 1U]) << 16U) |
        (std::to_integer<std::uint32_t>(data[offset + 2U]) << 8U) |
        std::to_integer<std::uint32_t>(data[offset + 3U]);
    return value;
}

// Requires a file whose chunk envelopes the decoder's inspection accepted:
// every length then lies inside the span, and the walk ends at IEND.
bool contains_icc_profile(std::span<const std::byte> encoded) noexcept {
    std::size_t offset = 8U;
    while (offset < encoded.size()) {
        const std::uint32_t length = read32(encoded, offset);
        const std::uint32_t type = read32(encoded, offset + 4U);
        if (type == iccp) return true;
        offset += static_cast<std::size_t>(length) + 12U;
    }
    return false;
}

PngPixelResult decode_failure(const PngDecodeResult& decoded) noexcept {
    PngPixelResult result{};
    result.error = PngPixelError::decode_failed;
    result.decode_error = decoded.error;
    result.inflate_error = decoded.inflate_error;
    if (decoded.error == PngDecodeError::invalid_argument) {
        result.error = PngPixelError::invalid_argument;
    }
    return result;
}

// Structural inspection plus the purpose's iCCP rule; reports the layout.
PngPixelResult inspect_request(std::span<const std::byte> encoded,
                               const PngPixelRequest& request, PngSampleLayout& layout) noexcept {
    PngPixelResult result{};
    const PngDecodeResult inspected = inspect_png_samples(encoded, request.limits);
    if (inspected.error != PngDecodeError::none) {
        result = decode_failure(inspected);
        return result;
    }
    const IccProfileRule icc = icc_profile_rule(request.purpose);
    if (icc == IccProfileRule::refuse && contains_icc_profile(encoded)) {
        result.error = PngPixelError::unsupported_color_profile;
        return result;
    }
    layout = inspected.layout;
    result.width = layout.width;
    result.height = layout.height;
    return result;
}

} // namespace

IccProfileRule icc_profile_rule(PngPixelPurpose purpose) noexcept {
    switch (purpose) {
        case PngPixelPurpose::resource: return IccProfileRule::refuse;
        case PngPixelPurpose::drawing_decode: return IccProfileRule::refuse;
        case PngPixelPurpose::clipboard: return IccProfileRule::ignore;
    }
    // An out-of-range purpose takes the stricter rule.
    return IccProfileRule::refuse;
}

PngDecodeLimits png_limits_for_image(std::uint32_t width, std::uint32_t height,
                                     std::size_t encoded_bytes) noexcept {
    PngDecodeLimits limits{};
    const std::uint64_t pixels = static_cast<std::uint64_t>(width) * height;
    limits.encoded_bytes = encoded_bytes;
    limits.dimension = std::min<std::uint32_t>(std::max(width, height), 0x7fffffffU);
    limits.pixels = pixels;
    // Sixteen-bit RGBA stores eight bytes per pixel. Each of at most seven
    // Adam7 passes adds one filter byte per pass row, and no pass has more
    // rows than the image; 7 * height bounds them together. The registry's
    // own dimension limits keep these products far from wrapping; the guard
    // (filtered <= 15 * pixels, work = 4 * filtered) keeps the helper safe
    // for any argument.
    const std::uint64_t maximum = std::numeric_limits<std::uint64_t>::max() / 64U;
    if (pixels <= maximum) {
        const std::uint64_t filtered = pixels * 8U + 7U * static_cast<std::uint64_t>(height);
        limits.filtered_bytes = filtered;
        const std::uint64_t scaled_work = filtered * 4U;
        limits.inflate_work.value = std::max(limits.inflate_work.value, scaled_work);
    } else {
        // Refused at inspection: a zero filtered allowance admits no pass.
        limits.filtered_bytes = 0U;
    }
    return limits;
}

PngDecodeLimits png_limits_for_pixels(std::uint64_t maximum_pixels,
                                      std::size_t encoded_bytes) noexcept {
    PngDecodeLimits limits{};
    limits.encoded_bytes = encoded_bytes;
    limits.dimension = 0x7fffffffU;
    const std::uint64_t maximum = std::numeric_limits<std::uint64_t>::max() / 64U;
    if (maximum_pixels <= maximum) {
        limits.pixels = maximum_pixels;
        const std::uint64_t filtered = maximum_pixels * 15U;
        limits.filtered_bytes = filtered;
        const std::uint64_t scaled_work = filtered * 4U;
        limits.inflate_work.value = std::max(limits.inflate_work.value, scaled_work);
    } else {
        // Out of range: zero pixels admits no image.
        limits.pixels = 0U;
        limits.filtered_bytes = 0U;
    }
    return limits;
}

PngPixelDecoder::PngPixelDecoder() = default;
PngPixelDecoder::~PngPixelDecoder() = default;

PngPixelResult PngPixelDecoder::inspect(std::span<const std::byte> encoded,
                                        const PngPixelRequest& request) const noexcept {
    PngSampleLayout layout{};
    const PngPixelResult result = inspect_request(encoded, request, layout);
    return result;
}

PngPixelResult PngPixelDecoder::decode(std::span<const std::byte> encoded,
    const PngPixelRequest& request, std::span<std::byte> pixels,
    std::size_t row_bytes) noexcept {
    PngSampleLayout layout{};
    PngPixelResult result = inspect_request(encoded, request, layout);
    if (result.error != PngPixelError::none) return result;
    if (!valid_pixel_target(request.target) || overlaps(encoded, pixels)) {
        result.error = PngPixelError::invalid_argument;
        return result;
    }
    if (!valid_pixel_destination(result.width, result.height, pixels.size(), row_bytes)) {
        result.error = PngPixelError::insufficient_storage;
        return result;
    }
    try {
        if (!workspace_) workspace_ = std::make_unique<compression::InflateStream>();
        if (scratch_.empty()) scratch_.resize(scratch_bytes);
        // Reset-size: decode overwrites every live element it reads.
        rows_.resize(layout.row_storage_bytes);
        samples_.resize(layout.rgba_samples);
        records_.resize(layout.metadata_records);
    } catch (const std::bad_alloc&) {
        result.error = PngPixelError::allocation_failed;
        return result;
    } catch (const std::length_error&) {
        result.error = PngPixelError::allocation_failed;
        return result;
    }
    const PngDecodeResult decoded = decode_png_samples_discarding_metadata(encoded,
        request.limits, *workspace_, rows_, samples_, scratch_, records_);
    if (decoded.error != PngDecodeError::none) {
        result = decode_failure(decoded);
        return result;
    }
    // Stage I6: resolve the colour chunks by precedence and convert to sRGB.
    const bool icc_ignored = icc_profile_rule(request.purpose) == IccProfileRule::ignore;
    const PngColorResolution colour = resolve_png_color(encoded, records_, icc_ignored);
    if (colour.error == PngColorError::unsupported_color_profile) {
        result.error = PngPixelError::unsupported_color_profile;
        return result;
    }
    if (colour.error != PngColorError::none) {
        result.error = PngPixelError::invalid_color_metadata;
        return result;
    }
    if (colour.encoding.converts()) {
        try {
            const bool color_converted = convert_samples_to_srgb(colour.encoding,
                layout.depth, samples_, color_work_);
            if (!color_converted) {
                result.error = PngPixelError::invalid_color_metadata;
                return result;
            }
        } catch (const std::bad_alloc&) {
            result.error = PngPixelError::allocation_failed;
            return result;
        }
    }
    const bool converted = convert_rgba16_samples(samples_, result.width, result.height,
                                                  request.target, pixels, row_bytes);
    if (!converted) {
        result.error = PngPixelError::invalid_argument;
        return result;
    }
    return result;
}

void PngPixelDecoder::release() noexcept {
    workspace_.reset();
    rows_ = {};
    samples_ = {};
    scratch_ = {};
    records_ = {};
    color_work_ = {};
}

} // namespace gui_forms::image
