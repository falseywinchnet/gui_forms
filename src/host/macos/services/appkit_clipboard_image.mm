#include "appkit_clipboard_image.hpp"
#include "../../../core/host/image/clipboard_image_wire.hpp"
#include "../../../core/host/image/clipboard_png.hpp"

#include <vector>

namespace gui_forms::host::detail {
namespace {
NSString* const raw_type = @"org.guiforms.rgba8";

} // namespace

HostClipboardImageResult read_appkit_clipboard_image(NSPasteboard* pasteboard) {
    if (pasteboard == nil) return HostClipboardImageResult::failed(HostServiceError::backend_failure);
    const std::uint64_t generation = static_cast<std::uint64_t>(pasteboard.changeCount);
    HostClipboardImageResult result = HostClipboardImageResult::nothing_read(generation);
    NSData* raw = [pasteboard dataForType:raw_type];
    if (raw != nil) {
        result = gui_forms::detail::decode_clipboard_image(
            {static_cast<const std::byte*>(raw.bytes), raw.length}, generation, false);
    } else {
        // PNG through the shared owned decoder, as on every host. There is no
        // TIFF fallback (owner decision 4A): PNG and raw pixels only.
        NSData* native = [pasteboard dataForType:NSPasteboardTypePNG];
        if (native != nil) {
            result = gui_forms::detail::decode_clipboard_png(
                {static_cast<const std::byte*>(native.bytes), native.length}, generation);
        }
    }
    // Another program wrote the pasteboard while it was read.
    if (static_cast<std::uint64_t>(pasteboard.changeCount) != generation) {
        return HostClipboardImageResult::failed(HostServiceError::backend_failure);
    }
    return result;
}

HostServiceStatus write_appkit_clipboard_image(NSPasteboard* pasteboard, HostImageView image) {
    if (pasteboard == nil) return {HostServiceError::backend_failure};
    const std::vector<std::byte> raw = gui_forms::detail::encode_clipboard_image(image);
    // PNG through the shared owned encoder, as on every host (it writes an
    // sRGB chunk); no NSBitmapImageRep encoding.
    const std::vector<std::byte> encoded = gui_forms::detail::encode_clipboard_png(image);
    if (raw.empty() || encoded.empty()) return {HostServiceError::backend_failure};
    NSData* png = [NSData dataWithBytes:encoded.data() length:encoded.size()];
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
