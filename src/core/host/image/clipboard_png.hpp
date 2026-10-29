#pragma once

#include "gui_forms/host/types/host_types.hpp"

#include <cstddef>
#include <span>

namespace gui_forms::detail {
// Decodes a PNG another program placed on the clipboard into the host-service
// contract: tight premultiplied BGRA8 rows, treated as sRGB. Uses the shared
// owned decoder with the clipboard purpose, so an embedded ICC profile is
// ignored (owner decision D1, option 2) and the image stays readable. Bounded
// by HostImage::maximum_pixels. Errors: undecodable bytes are invalid_argument,
// an image over the bound is too_large, exhausted storage is backend_failure;
// the result then holds no image. Every host's clipboard calls this one path.
// The result is read at `generation`.
[[nodiscard]] HostClipboardImageResult decode_clipboard_png(std::span<const std::byte> bytes,
                                                            std::uint64_t generation);

// Encodes a valid host image (premultiplied BGRA8) as the PNG every host
// offers on the clipboard, with the shared owned encoder at its default level:
// the encoder unpremultiplies, so a PNG read back gives the same pixels. Returns
// an empty vector for an invalid image, an image wider or taller than the
// encoder's 16384-pixel limit, or an encoder failure.
[[nodiscard]] std::vector<std::byte> encode_clipboard_png(HostImageView source);
} // namespace gui_forms::detail
