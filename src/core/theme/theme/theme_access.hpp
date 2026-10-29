#pragma once

#include "gui_forms/theme/theme/theme.hpp"

#include <memory>
#include <vector>

// Internal: how the theme package compiler makes a theme that owns the
// images it loaded (src/core/theme/theme_compiler).

namespace gui_forms::detail {

struct ThemeAccess final {
    // As Theme::create, the theme also holding `images` until it is
    // released.
    [[nodiscard]] static std::shared_ptr<const Theme> create_holding(
        const ThemeDefinition& definition, std::vector<std::shared_ptr<const ThemeImageLease>> images);
};

} // namespace gui_forms::detail
