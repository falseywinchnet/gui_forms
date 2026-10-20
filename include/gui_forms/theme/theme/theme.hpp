#pragma once

#include "gui_forms/anatomy/types/anatomy_types.hpp"
#include "gui_forms/engine/theme_engine/theme_engine.hpp"
#include "gui_forms/theme/types/theme_types.hpp"

#include <cstddef>

#include <memory>
#include <string_view>
#include <utility>
#include <vector>

namespace gui_forms {

class Theme final {
public:
    [[nodiscard]] static std::shared_ptr<const Theme> create(
        ThemeDefinition definition);
    [[nodiscard]] std::string_view id() const noexcept { return definition_.id; }
    [[nodiscard]] const BasicControlStyle& basic_style() const noexcept {
        return definition_.compatibility;
    }
    [[nodiscard]] const ThemeStructureTokens& structure() const noexcept {
        return definition_.structure;
    }
    [[nodiscard]] const ControlVisualRecipe& resolve(
        ControlVisualRole role, ControlVisualContext context) const noexcept;
    [[nodiscard]] const ThemeDefinition& definition() const noexcept {
        return definition_;
    }
    // The ramp colour for an accent index; an index past the end of the
    // ramp wraps around it.
    [[nodiscard]] Color accent(AccentIndex index) const noexcept {
        const std::size_t position = static_cast<std::size_t>(index.value) % definition_.accent_ramp.size();
        return definition_.accent_ramp[position];
    }
    // The overlay shadow's layers (owner decision RB-37) for ordinary or
    // high-contrast presentation; empty when the theme casts none.
    [[nodiscard]] const std::vector<MaterialShadow>& overlay_shadow(bool high_contrast) const noexcept {
        if (high_contrast) return definition_.high_contrast_overlay_shadow;
        return definition_.overlay_shadow;
    }
    // The theme's engine, which paint_part calls for every part. The theme
    // owns it, so every window showing the theme shares it (T2 plan).
    [[nodiscard]] const ThemeEngine& engine() const noexcept { return engine_; }

private:
    explicit Theme(ThemeDefinition definition)
        : definition_(std::move(definition)) {}
    ThemeDefinition definition_;
    ThemeEngine engine_;
};

[[nodiscard]] ThemeDefinition windows_professional_theme_definition();
[[nodiscard]] std::shared_ptr<const Theme> default_theme();

} // namespace gui_forms
