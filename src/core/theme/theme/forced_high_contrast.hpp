#pragma once

#include "gui_forms/engine/theme_rules/theme_rules.hpp"
#include "gui_forms/theme/types/theme_types.hpp"

#include <array>
#include <vector>

namespace gui_forms::detail {

// The material fields forced high contrast's pass (E1) keeps of a part's
// winning ones, which under the pass are its important inline style's: the
// border's styles and widths and the corners as written; the background
// colour, the border colours and the keylines with their colours remapped
// (high_contrast_colour, by origin and position); and none of the image
// layers, shadows, glows, filters, finish, mask or border image, which the
// built-in recipe's high-contrast records do not draw. `fields` points
// into the remapped copies, so the value is neither copied nor moved.
struct ForcedMaterialFields final {
    std::array<const MaterialDeclaration*, material_field_count> fields{};
    std::vector<MaterialDeclaration> remapped{};
    // Whether any field is kept.
    bool any{};

    ForcedMaterialFields() = default;
    ForcedMaterialFields(const ForcedMaterialFields&) = delete;
    ForcedMaterialFields& operator=(const ForcedMaterialFields&) = delete;
    ForcedMaterialFields(ForcedMaterialFields&&) = delete;
    ForcedMaterialFields& operator=(ForcedMaterialFields&&) = delete;
    ~ForcedMaterialFields() = default;
};

// Fills `result` from `resolved` for a part in `state`, `selected` or not.
void forced_material_fields(const std::array<const MaterialDeclaration*, material_field_count>& resolved,
                            ControlSurfaceState state, bool selected, ForcedMaterialFields& result);

} // namespace gui_forms::detail
