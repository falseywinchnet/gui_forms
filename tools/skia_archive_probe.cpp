// The check of a packed Skia archive (stage SK1, tools/skia_archive.py probe).
// Compiled against the archive's headers with the archive's definitions and
// linked with its two libraries, it proves that the native 32-bit colour type
// is BGRA and that the libraries link and draw: one opaque red fill must
// store blue, green, red, alpha = 0, 0, 255, 255 in every pixel.

#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPixmap.h"
#include "include/core/SkRect.h"
#include "include/core/SkRefCnt.h"
#include "include/core/SkSurface.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>

static_assert(kN32_SkColorType == kBGRA_8888_SkColorType,
              "GUI.Forms stores BGRA on every platform; build and include Skia with SK_B32_SHIFT=0 "
              "and SK_R32_SHIFT=16");

namespace {

constexpr int probe_width = 4;
constexpr int probe_height = 3;

// One pixel's four bytes in memory order.
struct PixelBytes {
    std::uint8_t blue{};
    std::uint8_t green{};
    std::uint8_t red{};
    std::uint8_t alpha{};
};

int report_failure(const char* message) {
    std::fprintf(stderr, "skia_archive_probe: %s\n", message);
    return 1;
}

} // namespace

int main() {
    const SkImageInfo info = SkImageInfo::Make(probe_width, probe_height, kN32_SkColorType, kPremul_SkAlphaType);
    sk_sp<SkSurface> surface = SkSurfaces::Raster(info);
    if (surface == nullptr) {
        return report_failure("no raster surface");
    }
    SkCanvas* canvas = (*surface).getCanvas();
    (*canvas).clear(SK_ColorTRANSPARENT);
    SkPaint paint;
    paint.setColor(SkColorSetARGB(255, 255, 0, 0));
    (*canvas).drawRect(SkRect::MakeWH(static_cast<float>(probe_width), static_cast<float>(probe_height)), paint);
    SkPixmap pixels;
    if (!(*surface).peekPixels(&pixels)) {
        return report_failure("the surface's pixels are not addressable");
    }
    // Every row is probe_width four-byte pixels; rows are rowBytes() apart.
    const std::uint8_t* base = static_cast<const std::uint8_t*>(pixels.addr());
    const std::size_t row_bytes = pixels.rowBytes();
    for (int row = 0; row < probe_height; ++row) {
        const std::uint8_t* row_start = base + static_cast<std::size_t>(row) * row_bytes;
        for (int column = 0; column < probe_width; ++column) {
            const std::uint8_t* pixel = row_start + static_cast<std::size_t>(column) * 4U;
            const PixelBytes bytes{pixel[0], pixel[1], pixel[2], pixel[3]};
            if (bytes.blue != 0U || bytes.green != 0U || bytes.red != 255U || bytes.alpha != 255U) {
                std::fprintf(stderr, "pixel %d,%d holds bytes %u %u %u %u\n", column, row,
                             static_cast<unsigned>(bytes.blue), static_cast<unsigned>(bytes.green),
                             static_cast<unsigned>(bytes.red), static_cast<unsigned>(bytes.alpha));
                return report_failure("an opaque red fill is not stored as B, G, R, A bytes");
            }
        }
    }
    std::printf("skia_archive_probe: %d x %d BGRA pixels drawn and read back\n", probe_width, probe_height);
    return 0;
}
