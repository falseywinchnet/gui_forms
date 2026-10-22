#include "appkit_clipboard_image.hpp"
#include "../../../core/host/image/clipboard_image_wire.hpp"
#include "../../../core/host/image/clipboard_png.hpp"

#include <algorithm>
#include <cstring>
#include <limits>

namespace gui_forms::host::detail {
namespace {
NSString* const raw_type = @"org.guiforms.rgba8";

} // namespace

HostClipboardImageResult read_appkit_clipboard_image(NSPasteboard* pasteboard) {
    HostClipboardImageResult result;
    if (pasteboard == nil) {
        result.status.error = HostServiceError::backend_failure;
        return result;
    }
    const std::uint64_t generation = static_cast<std::uint64_t>(pasteboard.changeCount);
    NSData* raw = [pasteboard dataForType:raw_type];
    if (raw != nil) {
        result = gui_forms::detail::decode_clipboard_image(
            {static_cast<const std::byte*>(raw.bytes), raw.length});
    } else {
        // PNG through the shared owned decoder, as on every host. There is no
        // TIFF fallback (owner decision 4A): PNG and raw pixels only.
        NSData* native = [pasteboard dataForType:NSPasteboardTypePNG];
        if (native != nil) {
            result = gui_forms::detail::decode_clipboard_png(
                {static_cast<const std::byte*>(native.bytes), native.length});
        }
    }
    result.generation = generation;
    if (static_cast<std::uint64_t>(pasteboard.changeCount) != generation) {
        result = {};
        result.status.error = HostServiceError::backend_failure;
    }
    return result;
}

HostServiceStatus write_appkit_clipboard_image(NSPasteboard* pasteboard, HostImageView image) {
    if (pasteboard == nil) return {HostServiceError::backend_failure};
    const std::vector<std::byte> raw = gui_forms::detail::encode_clipboard_image(image);
    const std::size_t row_bytes = static_cast<std::size_t>(image.width) * 4U;
    NSBitmapImageRep* bitmap = [[NSBitmapImageRep alloc]
        initWithBitmapDataPlanes:nullptr pixelsWide:image.width pixelsHigh:image.height
        bitsPerSample:8 samplesPerPixel:4 hasAlpha:YES isPlanar:NO
        colorSpaceName:NSDeviceRGBColorSpace bitmapFormat:NSBitmapFormatAlphaNonpremultiplied
        bytesPerRow:row_bytes bitsPerPixel:32];
    if (bitmap == nil || bitmap.bitmapData == nullptr) return {HostServiceError::backend_failure};
    for (std::uint32_t row = 0; row < image.height; ++row) {
        std::memcpy(bitmap.bitmapData + static_cast<std::size_t>(bitmap.bytesPerRow) * row,
                    image.pixels.data() + static_cast<std::size_t>(image.row_bytes) * row, row_bytes);
    }
    NSBitmapImageRep* tagged = [bitmap bitmapImageRepByRetaggingWithColorSpace:[NSColorSpace sRGBColorSpace]];
    NSData* png = [tagged representationUsingType:NSBitmapImageFileTypePNG properties:@{}];
    NSData* exact = [NSData dataWithBytes:raw.data() length:raw.size()];
    if (png == nil || exact == nil) return {HostServiceError::backend_failure};
    NSPasteboardItem* item = [[NSPasteboardItem alloc] init];
    if (![item setData:exact forType:raw_type] || ![item setData:png forType:NSPasteboardTypePNG]) {
        return {HostServiceError::backend_failure};
    }
    [pasteboard clearContents];
    return [pasteboard writeObjects:@[item]] ? HostServiceStatus{} :
        HostServiceStatus{HostServiceError::backend_failure};
}
} // namespace gui_forms::host::detail
