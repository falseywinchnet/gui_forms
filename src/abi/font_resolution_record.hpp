#pragma once

// Stage F: the C record of a font family resolution, shared by the semantic
// drawing ABI (the table's answer) and the raster module (the face drawn).

#include "gui_forms/drawing_c_api.h"
#include "gui_forms/drawing/font_substitution/font_substitution.hpp"

#include <cstdint>

namespace gui_drawing::abi {

// The C enumerations carry the C++ values unchanged.
static_assert(static_cast<std::uint32_t>(FontResolutionSource::bundled) == GD_FONT_SOURCE_BUNDLED);
static_assert(static_cast<std::uint32_t>(FontResolutionSource::substitution) == GD_FONT_SOURCE_SUBSTITUTION);
static_assert(static_cast<std::uint32_t>(FontResolutionSource::alias) == GD_FONT_SOURCE_ALIAS);
static_assert(static_cast<std::uint32_t>(FontResolutionSource::generic) == GD_FONT_SOURCE_GENERIC);
static_assert(static_cast<std::uint32_t>(FontResolutionSource::unmapped) == GD_FONT_SOURCE_UNMAPPED);
static_assert(static_cast<std::uint32_t>(FontResolutionSource::host) == GD_FONT_SOURCE_HOST);
static_assert(static_cast<std::uint32_t>(FontResolutionSource::content) == GD_FONT_SOURCE_CONTENT);
static_assert(static_cast<std::uint32_t>(FontMappingClass::none) == GD_FONT_MAPPING_NONE);
static_assert(static_cast<std::uint32_t>(FontMappingClass::metric_compatible) == GD_FONT_MAPPING_METRIC_COMPATIBLE);
static_assert(static_cast<std::uint32_t>(FontMappingClass::nearest) == GD_FONT_MAPPING_NEAREST);

// Writes every field of output except struct_size, which the caller checked.
// The views point into this library's static table.
inline void fill_font_resolution(const FontFamilyResolution& resolution, const bool face_loaded,
                                 gd_font_resolution& output) noexcept {
    output.source = static_cast<std::uint32_t>(resolution.source);
    output.mapping = static_cast<std::uint32_t>(resolution.mapping);
    output.requested_style = static_cast<std::uint32_t>(resolution.requested_style);
    output.face_style = static_cast<std::uint32_t>(resolution.face_style);
    output.face_loaded = face_loaded ? 1U : 0U;
    output.family = {resolution.family.data(), resolution.family.size()};
    output.face_file = {resolution.face_file.data(), resolution.face_file.size()};
}

} // namespace gui_drawing::abi
