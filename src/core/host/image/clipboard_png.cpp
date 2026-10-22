#include "clipboard_png.hpp"

#include "../../image/png_pixels.hpp"

#include <new>
#include <stdexcept>

namespace gui_forms::detail {

HostClipboardImageResult decode_clipboard_png(std::span<const std::byte> bytes) {
    HostClipboardImageResult result;
    result.status.error = HostServiceError::invalid_argument;
    if (bytes.empty()) return result;
    image::PngPixelRequest request{};
    request.limits = image::png_limits_for_pixels(HostImage::maximum_pixels, bytes.size());
    request.target = {image::PixelOrder::rgba, image::PixelAlpha::straight};
    request.purpose = image::PngPixelPurpose::clipboard;
    image::PngPixelDecoder decoder{};
    const image::PngPixelResult inspected = decoder.inspect(bytes, request);
    if (inspected.error != image::PngPixelError::none) {
        if (inspected.decode_error == image::PngDecodeError::resource_limit) {
            result.status.error = HostServiceError::too_large;
        }
        return result;
    }
    // inspect bounded width * height by HostImage::maximum_pixels.
    const std::uint64_t row_bytes = static_cast<std::uint64_t>(inspected.width) * 4U;
    const std::uint64_t byte_count = row_bytes * inspected.height;
    try {
        result.image.pixels.resize(static_cast<std::size_t>(byte_count));
    } catch (const std::bad_alloc&) {
        result.status.error = HostServiceError::backend_failure;
        return result;
    } catch (const std::length_error&) {
        result.status.error = HostServiceError::too_large;
        return result;
    }
    const image::PngPixelResult decoded = decoder.decode(bytes, request, result.image.pixels,
                                                         static_cast<std::size_t>(row_bytes));
    if (decoded.error != image::PngPixelError::none) {
        result.image = {};
        result.status.error = decoded.error == image::PngPixelError::allocation_failed
            ? HostServiceError::backend_failure : HostServiceError::invalid_argument;
        return result;
    }
    result.image.width = decoded.width;
    result.image.height = decoded.height;
    result.image.row_bytes = row_bytes;
    result.has_image = true;
    result.status = {};
    return result;
}

} // namespace gui_forms::detail
