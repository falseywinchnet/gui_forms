#include "gui_forms/schema/types/schema_types.hpp"

#include "gui_forms/anatomy/types/anatomy_types.hpp"

#include <array>

namespace gui_forms {
namespace {

constexpr std::array<std::string_view, 4> interaction_defect_names{
    "none",
    "too-many-interactions",
    "name-not-kebab-case",
    "duplicate-name",
};
static_assert(interaction_defect_names.size() ==
              static_cast<std::size_t>(InteractionDefect::duplicate_name) + 1U);

[[nodiscard]] InteractionValidation interaction_defect_at(InteractionDefect defect,
                                                          std::size_t index) noexcept {
    InteractionValidation result;
    result.defect = defect;
    result.index = index;
    return result;
}

} // namespace

std::string_view interaction_defect_name(InteractionDefect defect) noexcept {
    const std::size_t index = static_cast<std::size_t>(defect);
    if (index >= interaction_defect_names.size()) {
        return {};
    }
    return interaction_defect_names[index];
}

InteractionValidation validate_interactions(
    std::span<const InteractionDeclaration> interactions) noexcept {
    if (interactions.size() > maximum_interactions) {
        return interaction_defect_at(InteractionDefect::too_many_interactions, 0U);
    }
    for (std::size_t index = 0; index < interactions.size(); ++index) {
        const std::string_view name = interactions[index].name;
        if (!is_kebab_case_name(name)) {
            return interaction_defect_at(InteractionDefect::name_not_kebab_case, index);
        }
        for (std::size_t earlier = 0; earlier < index; ++earlier) {
            if (interactions[earlier].name == name) {
                return interaction_defect_at(InteractionDefect::duplicate_name, index);
            }
        }
    }
    InteractionValidation valid;
    return valid;
}

std::size_t find_interaction(std::span<const InteractionDeclaration> interactions,
                             std::string_view name) noexcept {
    for (std::size_t index = 0; index < interactions.size(); ++index) {
        if (interactions[index].name == name) {
            return index;
        }
    }
    return interactions.size();
}

} // namespace gui_forms
