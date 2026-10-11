#pragma once

#include <cstddef>
#include <optional>
#include <string_view>
#include <vector>

namespace gui_drawing::render {

// The installed face CoreText matches for a family name, upright at normal
// width, at weight 700 when bold and 400 otherwise. This is the request
// Skia's CoreText font manager made before ADR-004 phase 7.
//
// The result is one self-contained sfnt (face index 0) rebuilt from the
// face's tables, so a face inside a collection needs no index. Nullopt when
// no installed family has that name, a listed table cannot be read, or the
// rebuilt font would exceed maximum_bytes.
[[nodiscard]] std::optional<std::vector<std::byte>> core_text_font_bytes(std::string_view family, bool bold,
                                                                         std::size_t maximum_bytes);

} // namespace gui_drawing::render
