#pragma once

#include "stx_png/pixels.hpp"

namespace gui_forms::image {

// The 8-bit pixel layout every image decode writes, whatever the codec: four
// channels in RGBA or BGRA memory order, straight or premultiplied alpha.
// The vocabulary and its checks are stx_png's (stx_png/pixels.hpp), where
// it was first defined; a decoder of any other format uses it unchanged.
using stx_png::PixelAlpha;
using stx_png::PixelOrder;
using stx_png::PixelTarget;
using stx_png::valid_pixel_destination;
using stx_png::valid_pixel_target;

} // namespace gui_forms::image
