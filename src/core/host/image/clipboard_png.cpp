#include "clipboard_png.hpp"

#include "../../image/png_pixels.hpp"
#include "../../image/png_writer.hpp"

#include <new>
#include <stdexcept>

namespace gui_forms::detail {

HostClipboardImageResult decode_clipboard_png(std::span<const std::byte> bytes, std::uint64_t generation) {
    if (bytes.empty()) return HostClipboardImageResult::failed(HostServiceError::invalid_argument);
    image::PngPixelRequest request{};
    request.limits = image::png_limits_for_pixels(HostImage::maximum_pixels, bytes.size());
    request.target = {image::PixelOrder::bgra, image::PixelAlpha::premultiplied};
    request.purpose = image::PngPixelPurpose::clipboard;
    image::PngPixelDecoder decoder{};
    const image::PngPixelResult inspected = decoder.inspect(bytes, request);
    if (inspected.error != image::PngPixelError::none) {
        const HostServiceError refused = inspected.decode_error == image::PngDecodeError::resource_limit
            ? HostServiceError::too_large : HostServiceError::invalid_argument;
        return HostClipboardImageResult::failed(refused);
    }
    // inspect bounded width * height by HostImage::maximum_pixels.
    const std::uint64_t row_bytes = static_cast<std::uint64_t>(inspected.width) * 4U;
    const std::uint64_t byte_count = row_bytes * inspected.height;
    HostImage image{};
    try {
        image.pixels.resize(static_cast<std::size_t>(byte_count));
    } catch (const std::bad_alloc&) {
        return HostClipboardImageResult::failed(HostServiceError::backend_failure);
    } catch (const std::length_error&) {
        return HostClipboardImageResult::failed(HostServiceError::too_large);
    }
    const image::PngPixelResult decoded = decoder.decode(bytes, request, image.pixels,
                                                         static_cast<std::size_t>(row_bytes));
    if (decoded.error != image::PngPixelError::none) {
        const HostServiceError refused = decoded.error == image::PngPixelError::allocation_failed
            ? HostServiceError::backend_failure : HostServiceError::invalid_argument;
        return HostClipboardImageResult::failed(refused);
    }
    image.width = decoded.width;
    image.height = decoded.height;
    image.row_bytes = row_bytes;
    return HostClipboardImageResult::image_read(std::move(image), generation);
}

std::vector<std::byte> encode_clipboard_png(HostImageView source) {
    std::vector<std::byte> png;
    if (validate_host_image(source) != HostImageError::none) return png;
    image::PngEncodeInput input{};
    input.width = source.width;
    input.height = source.height;
    input.row_bytes = source.row_bytes;
    input.pixels = source.pixels;
    input.order = image::PngChannelOrder::bgra;
    input.alpha = image::PngSourceAlpha::premultiplied;
    if (!image::encode_png(input, png)) png.clear();
    return png;
}

} // namespace gui_forms::detail
