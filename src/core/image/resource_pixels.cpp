#include "resource_pixels.hpp"

namespace gui_forms::image {
namespace {

ResourcePixelError png_error(PngPixelError error) noexcept {
    switch (error) {
    case PngPixelError::none: return ResourcePixelError::none;
    case PngPixelError::invalid_argument: return ResourcePixelError::invalid_argument;
    case PngPixelError::decode_failed: return ResourcePixelError::decode_failed;
    case PngPixelError::unsupported_color_profile: return ResourcePixelError::unsupported_color_profile;
    case PngPixelError::invalid_color_metadata: return ResourcePixelError::invalid_color_metadata;
    case PngPixelError::insufficient_storage: return ResourcePixelError::insufficient_storage;
    case PngPixelError::allocation_failed: return ResourcePixelError::allocation_failed;
    }
    return ResourcePixelError::decode_failed;
}

} // namespace

ResourcePixelDecoder::ResourcePixelDecoder() {}

ResourcePixelDecoder::~ResourcePixelDecoder() {}

void ResourcePixelDecoder::release() noexcept {
    png_.release();
}

ResourcePixelResult ResourcePixelDecoder::decode(const ImageResourceView& resource, PixelTarget target,
                                                 std::span<std::byte> pixels, std::size_t row_bytes) noexcept {
    ResourcePixelResult result;
    if (resource.encoding == ImageResourceEncoding::png) {
        PngPixelRequest request{};
        request.limits = png_limits_for_image(resource.metadata.width, resource.metadata.height,
                                              resource.encoded.size());
        request.target = target;
        request.purpose = PngPixelPurpose::resource;
        PngPixelResult decoded = png_.decode(resource.encoded, request, pixels, row_bytes);
        result.error = png_error(decoded.error);
        result.width = decoded.width;
        result.height = decoded.height;
    } else {
        result.error = ResourcePixelError::unsupported_encoding;
        return result;
    }
    bool admitted_size = result.width == resource.metadata.width && result.height == resource.metadata.height;
    if (result.error == ResourcePixelError::none && !admitted_size) {
        result.error = ResourcePixelError::decode_failed;
    }
    return result;
}

} // namespace gui_forms::image
