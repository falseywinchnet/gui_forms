#include "gui_forms/drawing/font_substitution/font_substitution.hpp"

#include <array>
#include <cstddef>
#include <string_view>

namespace gui_drawing {
namespace {

// generated_content_font_family, generated_bundled_font_faces,
// generated_font_substitutions and generated_gate_exceptions, from assets/fonts/font-substitution.json.
#include "font_substitution_table.inc"

[[nodiscard]] constexpr char ascii_folded(const char value) noexcept {
    const bool upper = value >= 'A' && value <= 'Z';
    const char result = upper ? static_cast<char>(value - 'A' + 'a') : value;
    return result;
}

[[nodiscard]] bool same_name(const std::string_view left, const std::string_view right) noexcept {
    if (left.size() != right.size()) return false;
    for (std::size_t index = 0; index < left.size(); ++index) {
        if (ascii_folded(left[index]) != ascii_folded(right[index])) return false;
    }
    return true;
}

// The faces tried for each requested style, nearest first. A missing italic
// keeps the weight upright; a missing bold keeps the slant. Nothing is
// synthesized (decision 7), so the order ends at the faces a family has.
constexpr std::array<std::array<FontFaceStyle, 4>, 4> nearest_styles{{
    {FontFaceStyle::regular, FontFaceStyle::bold, FontFaceStyle::italic, FontFaceStyle::bold_italic},
    {FontFaceStyle::bold, FontFaceStyle::regular, FontFaceStyle::bold_italic, FontFaceStyle::italic},
    {FontFaceStyle::italic, FontFaceStyle::regular, FontFaceStyle::bold_italic, FontFaceStyle::bold},
    {FontFaceStyle::bold_italic, FontFaceStyle::bold, FontFaceStyle::italic, FontFaceStyle::regular},
}};

// The family's face for a style, or null. Family names match exactly: they
// come from the table, not from a caller.
[[nodiscard]] const BundledFontFace* family_face(const std::string_view family, const FontFaceStyle style) noexcept {
    const BundledFontFace* result = nullptr;
    for (const BundledFontFace& face : generated_bundled_font_faces) {
        if (face.family == family && face.style == style) {
            result = &face;
            break;
        }
    }
    return result;
}

// Fills the family, face file and face style of a resolution whose source
// and family are known. Every listed family has a regular face (the
// generator rejects one without), so a face is always found.
void choose_face(FontFamilyResolution& resolution, const std::string_view family) noexcept {
    const std::array<FontFaceStyle, 4>& order = nearest_styles[static_cast<std::size_t>(resolution.requested_style)];
    for (const FontFaceStyle candidate : order) {
        const BundledFontFace* const face = family_face(family, candidate);
        if (face == nullptr) continue;
        resolution.family = (*face).family;
        resolution.face_file = (*face).file;
        resolution.face_style = (*face).style;
        return;
    }
}

} // namespace

FontFaceStyle font_face_style(const std::uint32_t font_style) noexcept {
    const FontFaceStyle result = static_cast<FontFaceStyle>(font_style & 3U);
    return result;
}

FontFamilyResolution resolve_font_family(const std::string_view requested, const FontFaceStyle style) noexcept {
    FontFamilyResolution result{};
    result.requested = requested;
    result.requested_style = style;
    result.face_style = style;
    for (const BundledFontFace& face : generated_bundled_font_faces) {
        if (!same_name(face.family, requested)) continue;
        result.source = FontResolutionSource::bundled;
        choose_face(result, face.family);
        return result;
    }
    for (const FontSubstitution& substitution : generated_font_substitutions) {
        if (!same_name(substitution.requested, requested)) continue;
        result.source = substitution.source;
        result.mapping = substitution.mapping;
        result.measured_as = substitution.measured_as;
        choose_face(result, substitution.family);
        return result;
    }
    return result;
}

FontFamilyResolution content_font_resolution(const std::string_view requested, const FontFaceStyle style) noexcept {
    FontFamilyResolution result{};
    result.requested = requested;
    result.source = FontResolutionSource::content;
    result.requested_style = style;
    result.face_style = style;
    choose_face(result, generated_content_font_family);
    return result;
}

std::string_view content_font_family() noexcept {
    return generated_content_font_family;
}

std::span<const BundledFontFace> bundled_font_faces() noexcept {
    return generated_bundled_font_faces;
}

std::span<const FontSubstitution> font_substitutions() noexcept {
    return generated_font_substitutions;
}

std::span<const FontGateException> font_gate_exceptions() noexcept {
    return generated_gate_exceptions;
}

const char* font_resolution_source_name(const FontResolutionSource source) noexcept {
    switch (source) {
    case FontResolutionSource::bundled: return "bundled";
    case FontResolutionSource::substitution: return "substitution";
    case FontResolutionSource::alias: return "alias";
    case FontResolutionSource::generic: return "generic";
    case FontResolutionSource::unmapped: return "unmapped";
    case FontResolutionSource::host: return "host";
    case FontResolutionSource::content: return "content";
    }
    return "unknown";
}

const char* font_mapping_class_name(const FontMappingClass mapping) noexcept {
    switch (mapping) {
    case FontMappingClass::none: return "none";
    case FontMappingClass::metric_compatible: return "metric_compatible";
    case FontMappingClass::nearest: return "nearest";
    }
    return "unknown";
}

const char* font_gate_criterion_name(const FontGateCriterion criterion) noexcept {
    switch (criterion) {
    case FontGateCriterion::advances: return "advances";
    case FontGateCriterion::line_breaks: return "line_breaks";
    case FontGateCriterion::line_height: return "line_height";
    case FontGateCriterion::line_width: return "line_width";
    }
    return "unknown";
}

} // namespace gui_drawing
