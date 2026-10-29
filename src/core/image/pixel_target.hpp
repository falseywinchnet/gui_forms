#pragma once

#include "stx_png/pixels.hpp"

namespace gui_forms::image {

// The 8-bit pixel layout an image decode writes, whatever the codec: four
// channels in memory order and their alpha. GUI.Forms decodes into
// premultiplied BGRA8; stx_png also offers RGBA order and straight alpha.
// The vocabulary and its checks are stx_png's (stx_png/pixels.hpp), where
// it was first defined; a decoder of any other format uses it unchanged.
using stx_png::PixelAlpha;
using stx_png::PixelOrder;
using stx_png::PixelTarget;
using stx_png::valid_pixel_destination;
using stx_png::valid_pixel_target;

} // namespace gui_forms::image
