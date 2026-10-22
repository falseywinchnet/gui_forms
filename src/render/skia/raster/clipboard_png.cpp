#include "clipboard_png.hpp"
#include "include/core/SkData.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkPixmap.h"
#include "include/core/SkStream.h"
#include "include/encode/SkPngEncoder.h"
#include <cstring>
namespace gui_forms::render {
std::vector<std::byte> encode_clipboard_png(HostImageView image) {
  if (validate_host_image(image) != HostImageError::none)
    return {};
  const SkImageInfo info = SkImageInfo::Make(
      static_cast<int>(image.width), static_cast<int>(image.height),
      kRGBA_8888_SkColorType, kUnpremul_SkAlphaType);
  const SkPixmap pixels(info, image.pixels.data(),
                        static_cast<std::size_t>(image.row_bytes));
  SkDynamicMemoryWStream stream;
  if (!SkPngEncoder::Encode(&stream, pixels, {}))
    return {};
  const sk_sp<SkData> data = stream.detachAsData();
  std::vector<std::byte> result((*data).size());
  std::memcpy(result.data(), (*data).data(), result.size());
  return result;
}
} // namespace gui_forms::render
