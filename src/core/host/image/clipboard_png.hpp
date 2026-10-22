#pragma once

#include "gui_forms/host/types/host_types.hpp"

#include <cstddef>
#include <span>

namespace gui_forms::detail {
// Decodes a PNG another program placed on the clipboard into the host-service
// contract: tight straight-alpha RGBA8 rows, treated as sRGB. Uses the shared
// owned decoder with the clipboard purpose, so an embedded ICC profile is
// ignored (owner decision D1, option 2) and the image stays readable. Bounded
// by HostImage::maximum_pixels. Errors: undecodable bytes are invalid_argument,
// an image over the bound is too_large, exhausted storage is backend_failure;
// the result then holds no image. Every host's clipboard calls this one path.
[[nodiscard]] HostClipboardImageResult decode_clipboard_png(std::span<const std::byte> bytes);
} // namespace gui_forms::detail
