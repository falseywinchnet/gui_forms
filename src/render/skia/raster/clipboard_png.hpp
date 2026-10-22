#pragma once
#include "gui_forms/host/types/host_types.hpp"
namespace gui_forms::render {
// Clipboard PNG decoding is the shared owned path in
// src/core/host/image/clipboard_png.hpp; only encoding remains here.
std::vector<std::byte> encode_clipboard_png(HostImageView image);
} // namespace gui_forms::render
