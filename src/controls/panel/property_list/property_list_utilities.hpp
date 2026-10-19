#pragma once

#include "gui_forms/text.hpp"

#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>

namespace gui_forms::detail {

inline constexpr double property_group_height = 25.0;
inline constexpr double property_row_height = 29.0;
inline constexpr double property_editor_height = 34.0;
inline constexpr double property_validation_height = 18.0;
inline constexpr double property_padding = 8.0;
inline constexpr double property_gap = 7.0;

inline void require_property_text(std::string_view value, const char* field) {
    if (!validate_utf8(value).valid()) {
        throw std::invalid_argument(std::string("PropertyList ") + field +
                                    " must be valid UTF-8");
    }
}

} // namespace gui_forms::detail
