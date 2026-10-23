#include "font_metrics_differential.hpp"

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <cmath>

// The GDI oracle: what a Windows Forms author saw for the same face, size and
// strings. The face's own bytes are added privately to this process
// (AddFontMemResourceEx), so the candidate need not be installed; GetFontData
// then proves which bytes GDI measured. Win32 types stay in this file.

namespace gui_forms::tools::font_metrics {

namespace {

// Owns a private font resource; RemoveFontMemResourceEx releases it.
class PrivateFontResource final {
public:
    explicit PrivateFontResource(std::span<const std::byte> encoded) {
        // AddFontMemResourceEx copies the bytes; its parameter is not const
        // but it does not write through it.
        void* const bytes = const_cast<std::byte*>(encoded.data());
        DWORD installed = 0U;
        handle_ = AddFontMemResourceEx(bytes, static_cast<DWORD>(encoded.size()), nullptr, &installed);
    }
    ~PrivateFontResource() {
        if (handle_ != nullptr) RemoveFontMemResourceEx(handle_);
    }
    PrivateFontResource(const PrivateFontResource&) = delete;
    PrivateFontResource& operator=(const PrivateFontResource&) = delete;
    [[nodiscard]] bool added() const noexcept { return handle_ != nullptr; }

private:
    HANDLE handle_{};
};

// Owns a memory device context.
class MemoryDevice final {
public:
    MemoryDevice() : handle_(CreateCompatibleDC(nullptr)) {}
    ~MemoryDevice() {
        if (handle_ != nullptr) DeleteDC(handle_);
    }
    MemoryDevice(const MemoryDevice&) = delete;
    MemoryDevice& operator=(const MemoryDevice&) = delete;
    [[nodiscard]] HDC handle() const noexcept { return handle_; }

private:
    HDC handle_{};
};

// Creates a font and keeps it selected into device until destruction, which
// restores the previous object before deleting the font.
class SelectedFont final {
public:
    SelectedFont(HDC device, const std::wstring& family, const FaceFacts& facts, double size_px)
        : device_(device) {
        const int height = -static_cast<int>(std::lround(size_px));
        font_ = CreateFontW(height, 0, 0, 0, static_cast<int>(facts.weight_class), facts.italic ? TRUE : FALSE,
                            FALSE, FALSE, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
                            ANTIALIASED_QUALITY, DEFAULT_PITCH, family.c_str());
        if (font_ == nullptr) return;
        const HGDIOBJ previous = SelectObject(device_, font_);
        if (previous != nullptr && previous != HGDI_ERROR) previous_ = previous;
    }
    ~SelectedFont() {
        if (previous_ != nullptr) SelectObject(device_, previous_);
        if (font_ != nullptr) DeleteObject(font_);
    }
    SelectedFont(const SelectedFont&) = delete;
    SelectedFont& operator=(const SelectedFont&) = delete;
    [[nodiscard]] bool selected() const noexcept { return previous_ != nullptr; }

private:
    HDC device_{};
    HFONT font_{};
    HGDIOBJ previous_{};
};

// Converts validated UTF-8 into output, replacing its contents.
bool widen(std::string_view text, std::wstring& output) {
    output.clear();
    if (text.empty()) return true;
    const int length = static_cast<int>(text.size());
    const int units = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), length, nullptr, 0);
    if (units <= 0) return false;
    output.resize(static_cast<std::size_t>(units));
    const int written = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), length, output.data(), units);
    const bool converted = written == units;
    return converted;
}

std::string narrow(const std::wstring& text) {
    if (text.empty()) return std::string{};
    const int length = static_cast<int>(text.size());
    const int bytes = WideCharToMultiByte(CP_UTF8, 0, text.data(), length, nullptr, 0, nullptr, nullptr);
    if (bytes <= 0) return std::string{};
    std::string output(static_cast<std::size_t>(bytes), '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.data(), length, output.data(), bytes, nullptr, nullptr);
    return output;
}

// True when the face selected into device has exactly the file's bytes.
bool selected_bytes_match(HDC device, const FaceFacts& facts) {
    const DWORD size = GetFontData(device, 0U, 0U, nullptr, 0U);
    if (size == GDI_ERROR || size != facts.byte_count) return false;
    std::vector<std::byte> bytes(static_cast<std::size_t>(size));
    if (GetFontData(device, 0U, 0U, bytes.data(), size) != size) return false;
    const bool matches = sha256_hex(bytes) == facts.sha256_hex;
    return matches;
}

} // namespace

GdiMeasurement measure_with_gdi(std::span<const std::byte> encoded, const FaceFacts& facts, const Corpus& corpus) {
    GdiMeasurement result{};
    std::wstring family{};
    if (!widen(facts.family, family) || family.empty() || family.size() >= LF_FACESIZE) {
        result.unavailable_reason = "family name unusable by GDI";
        return result;
    }
    const PrivateFontResource resource(encoded);
    if (!resource.added()) {
        result.unavailable_reason = "AddFontMemResourceEx refused the face";
        return result;
    }
    const MemoryDevice device{};
    if (device.handle() == nullptr) {
        result.unavailable_reason = "CreateCompatibleDC failed";
        return result;
    }
    std::wstring text{};
    result.sizes.reserve(measured_sizes_px.size());
    for (const double size_px : measured_sizes_px) {
        const SelectedFont font(device.handle(), family, facts, size_px);
        if (!font.selected()) {
            result.unavailable_reason = "CreateFontW or SelectObject failed";
            result.sizes.clear();
            return result;
        }
        if (result.sizes.empty()) {
            // GetTextFaceW writes a terminated name of at most LF_FACESIZE units.
            std::array<wchar_t, LF_FACESIZE> selected{};
            const int count = GetTextFaceW(device.handle(), LF_FACESIZE, selected.data());
            const std::wstring name = count > 0 ? std::wstring(selected.data()) : std::wstring{};
            result.selected_face = narrow(name);
            result.bytes_match_file = selected_bytes_match(device.handle(), facts);
        }
        TEXTMETRICW metrics{};
        if (GetTextMetricsW(device.handle(), &metrics) == 0) {
            result.unavailable_reason = "GetTextMetricsW failed";
            result.sizes.clear();
            return result;
        }
        GdiSizeMeasurement size{};
        size.size_px = size_px;
        size.ascent = metrics.tmAscent;
        size.descent = metrics.tmDescent;
        size.height = metrics.tmHeight;
        size.internal_leading = metrics.tmInternalLeading;
        size.external_leading = metrics.tmExternalLeading;
        size.string_widths.reserve(corpus.strings.size());
        for (const std::string& line : corpus.strings) {
            SIZE extent{};
            const bool converted = widen(line, text);
            const BOOL measured = converted ?
                GetTextExtentPoint32W(device.handle(), text.c_str(), static_cast<int>(text.size()), &extent) : FALSE;
            if (measured == 0) {
                result.unavailable_reason = "GetTextExtentPoint32W failed";
                result.sizes.clear();
                return result;
            }
            size.string_widths.push_back(extent.cx);
        }
        result.sizes.push_back(std::move(size));
    }
    result.available = true;
    return result;
}

} // namespace gui_forms::tools::font_metrics
