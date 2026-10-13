#include "gui_forms/anatomy/types/anatomy_types.hpp"

#include <array>

namespace gui_forms {
namespace {

// Indexed by the enumerator's value; the static_asserts keep the tables and
// the enumerations the same length.
constexpr std::array<std::string_view, part_role_count> part_role_names{
    "region",
    "shadow",
    "face",
    "edge",
    "keyline",
    "perimeter",
    "sheen",
    "seam",
    "divider",
    "caption",
    "cap",
    "well",
    "track",
    "fill",
    "thumb",
    "actuator",
    "indicator",
    "activity",
    "glyph",
    "icon",
    "label",
    "content",
    "caret",
    "disclosure",
    "grip",
    "chip",
    "focus",
    "preselect",
    "selection",
    "validation",
    "drop",
};
static_assert(part_role_names.size() == part_role_count);

constexpr std::array<std::string_view, anatomy_state_count> anatomy_state_names{
    "normal",
    "hot",
    "pressed",
    "captured",
    "focused",
    "focus-visible",
    "preselect",
    "selected",
    "checked",
    "indeterminate",
    "current",
    "active-tool",
    "enabled",
    "disabled",
    "unavailable",
    "read-only",
    "invalid",
    "pending",
    "open",
    "closed",
    "expanding",
    "collapsing",
    "interrupted",
    "drop-accepted",
    "drop-rejected",
    "key",
    "secondary-participating",
    "drag-proximate",
    "deactivated",
    "high-contrast",
    "reduced-motion",
};
static_assert(anatomy_state_names.size() == anatomy_state_count);

constexpr std::array<std::string_view, 24> anatomy_defect_names{
    "none",
    "control-id-not-kebab-case",
    "no-parts",
    "too-many-parts",
    "part-name-not-kebab-case",
    "duplicate-part-name",
    "part-role-out-of-range",
    "composed-from-not-kebab-case",
    "state-out-of-range",
    "duplicate-state",
    "too-many-custom-states",
    "custom-state-not-kebab-case",
    "custom-state-shadows-standard-state",
    "duplicate-custom-state",
    "too-many-item-parts",
    "item-part-name-not-kebab-case",
    "duplicate-item-part-name",
    "item-part-role-out-of-range",
    "item-state-out-of-range",
    "duplicate-item-state",
    "too-many-item-custom-states",
    "item-custom-state-not-kebab-case",
    "item-custom-state-shadows-standard-state",
    "duplicate-item-custom-state",
};
static_assert(anatomy_defect_names.size() ==
              static_cast<std::size_t>(AnatomyDefect::duplicate_item_custom_state) + 1U);

[[nodiscard]] bool is_lowercase_letter(char character) noexcept {
    return character >= 'a' && character <= 'z';
}

[[nodiscard]] bool is_digit(char character) noexcept {
    return character >= '0' && character <= '9';
}

[[nodiscard]] AnatomyValidation defect_at(AnatomyDefect defect,
                                          std::size_t index) noexcept {
    AnatomyValidation result;
    result.defect = defect;
    result.index = index;
    return result;
}

} // namespace

std::string_view part_role_name(PartRole role) noexcept {
    const std::size_t index = static_cast<std::size_t>(role);
    if (index >= part_role_names.size()) {
        return {};
    }
    return part_role_names[index];
}

std::string_view anatomy_state_name(AnatomyState state) noexcept {
    const std::size_t index = static_cast<std::size_t>(state);
    if (index >= anatomy_state_names.size()) {
        return {};
    }
    return anatomy_state_names[index];
}

std::optional<PartRole> part_role_from_name(std::string_view name) noexcept {
    for (std::size_t index = 0; index < part_role_names.size(); ++index) {
        if (part_role_names[index] == name) {
            return static_cast<PartRole>(index);
        }
    }
    return std::nullopt;
}

std::optional<AnatomyState> anatomy_state_from_name(std::string_view name) noexcept {
    for (std::size_t index = 0; index < anatomy_state_names.size(); ++index) {
        if (anatomy_state_names[index] == name) {
            return static_cast<AnatomyState>(index);
        }
    }
    return std::nullopt;
}

bool is_kebab_case_name(std::string_view name) noexcept {
    if (name.empty() || !is_lowercase_letter(name.front())) {
        return false;
    }
    if (name.back() == '-') {
        return false;
    }
    char previous = '\0';
    for (std::size_t index = 0; index < name.size(); ++index) {
        const char character = name[index];
        const bool word_character = is_lowercase_letter(character) || is_digit(character);
        if (character == '-') {
            if (previous == '-') {
                return false;
            }
        } else if (!word_character) {
            return false;
        }
        previous = character;
    }
    return true;
}

std::optional<PartIndex> find_declared_part(std::span<const PartDeclaration> declared,
                                            std::string_view part_name) noexcept {
    for (std::size_t index = 0; index < declared.size(); ++index) {
        if (declared[index].name == part_name) {
            PartIndex found;
            found.value = static_cast<std::uint8_t>(index);
            return found;
        }
    }
    return std::nullopt;
}

std::optional<PartIndex> find_part(const ControlAnatomy& anatomy,
                                   std::string_view part_name) noexcept {
    const std::optional<PartIndex> found = find_declared_part(anatomy.parts, part_name);
    return found;
}

std::optional<PartIndex> find_item_part(const ControlAnatomy& anatomy,
                                        std::string_view part_name) noexcept {
    const std::optional<PartIndex> found =
        find_declared_part(anatomy.item_parts, part_name);
    return found;
}

std::optional<CustomStateIndex> find_custom_state(
    const ControlAnatomy& anatomy, std::string_view state_name) noexcept {
    for (std::size_t index = 0; index < anatomy.custom_states.size(); ++index) {
        if (anatomy.custom_states[index].name == state_name) {
            CustomStateIndex found;
            found.value = static_cast<std::uint8_t>(index);
            return found;
        }
    }
    return std::nullopt;
}

std::optional<CustomStateIndex> find_item_custom_state(
    const ControlAnatomy& anatomy, std::string_view state_name) noexcept {
    for (std::size_t index = 0; index < anatomy.item_custom_states.size(); ++index) {
        if (anatomy.item_custom_states[index].name == state_name) {
            CustomStateIndex found;
            found.value = static_cast<std::uint8_t>(index);
            return found;
        }
    }
    return std::nullopt;
}

bool declares_state(const ControlAnatomy& anatomy, AnatomyState state) noexcept {
    for (std::size_t index = 0; index < anatomy.states.size(); ++index) {
        if (anatomy.states[index] == state) {
            return true;
        }
    }
    return false;
}

std::string_view anatomy_defect_name(AnatomyDefect defect) noexcept {
    const std::size_t index = static_cast<std::size_t>(defect);
    if (index >= anatomy_defect_names.size()) {
        return {};
    }
    return anatomy_defect_names[index];
}

AnatomyValidation validate_control_anatomy(const ControlAnatomy& anatomy) noexcept {
    if (!is_kebab_case_name(anatomy.control_id)) {
        return defect_at(AnatomyDefect::control_id_not_kebab_case, 0U);
    }
    if (anatomy.parts.empty()) {
        return defect_at(AnatomyDefect::no_parts, 0U);
    }
    if (anatomy.parts.size() > maximum_control_parts) {
        return defect_at(AnatomyDefect::too_many_parts, 0U);
    }
    for (std::size_t index = 0; index < anatomy.parts.size(); ++index) {
        const PartDeclaration& part = anatomy.parts[index];
        if (!is_kebab_case_name(part.name)) {
            return defect_at(AnatomyDefect::part_name_not_kebab_case, index);
        }
        if (static_cast<std::size_t>(part.role) >= part_role_count) {
            return defect_at(AnatomyDefect::part_role_out_of_range, index);
        }
        if (!part.composed_from.empty() && !is_kebab_case_name(part.composed_from)) {
            return defect_at(AnatomyDefect::composed_from_not_kebab_case, index);
        }
        for (std::size_t earlier = 0; earlier < index; ++earlier) {
            if (anatomy.parts[earlier].name == part.name) {
                return defect_at(AnatomyDefect::duplicate_part_name, index);
            }
        }
    }
    for (std::size_t index = 0; index < anatomy.states.size(); ++index) {
        const AnatomyState state = anatomy.states[index];
        if (static_cast<std::size_t>(state) >= anatomy_state_count) {
            return defect_at(AnatomyDefect::state_out_of_range, index);
        }
        for (std::size_t earlier = 0; earlier < index; ++earlier) {
            if (anatomy.states[earlier] == state) {
                return defect_at(AnatomyDefect::duplicate_state, index);
            }
        }
    }
    if (anatomy.custom_states.size() > maximum_custom_states) {
        return defect_at(AnatomyDefect::too_many_custom_states, 0U);
    }
    for (std::size_t index = 0; index < anatomy.custom_states.size(); ++index) {
        const std::string_view name = anatomy.custom_states[index].name;
        if (!is_kebab_case_name(name)) {
            return defect_at(AnatomyDefect::custom_state_not_kebab_case, index);
        }
        const std::optional<AnatomyState> standard = anatomy_state_from_name(name);
        if (standard.has_value()) {
            return defect_at(AnatomyDefect::custom_state_shadows_standard_state, index);
        }
        for (std::size_t earlier = 0; earlier < index; ++earlier) {
            if (anatomy.custom_states[earlier].name == name) {
                return defect_at(AnatomyDefect::duplicate_custom_state, index);
            }
        }
    }
    if (anatomy.item_parts.size() > maximum_control_parts) {
        return defect_at(AnatomyDefect::too_many_item_parts, 0U);
    }
    for (std::size_t index = 0; index < anatomy.item_parts.size(); ++index) {
        const PartDeclaration& part = anatomy.item_parts[index];
        if (!is_kebab_case_name(part.name)) {
            return defect_at(AnatomyDefect::item_part_name_not_kebab_case, index);
        }
        if (static_cast<std::size_t>(part.role) >= part_role_count) {
            return defect_at(AnatomyDefect::item_part_role_out_of_range, index);
        }
        if (!part.composed_from.empty() && !is_kebab_case_name(part.composed_from)) {
            return defect_at(AnatomyDefect::composed_from_not_kebab_case, index);
        }
        // An item part may repeat neither another item part nor one of the
        // control's own parts.
        const std::optional<PartIndex> control_part = find_part(anatomy, part.name);
        if (control_part.has_value()) {
            return defect_at(AnatomyDefect::duplicate_item_part_name, index);
        }
        for (std::size_t earlier = 0; earlier < index; ++earlier) {
            if (anatomy.item_parts[earlier].name == part.name) {
                return defect_at(AnatomyDefect::duplicate_item_part_name, index);
            }
        }
    }
    for (std::size_t index = 0; index < anatomy.item_states.size(); ++index) {
        const AnatomyState state = anatomy.item_states[index];
        if (static_cast<std::size_t>(state) >= anatomy_state_count) {
            return defect_at(AnatomyDefect::item_state_out_of_range, index);
        }
        for (std::size_t earlier = 0; earlier < index; ++earlier) {
            if (anatomy.item_states[earlier] == state) {
                return defect_at(AnatomyDefect::duplicate_item_state, index);
            }
        }
    }
    if (anatomy.item_custom_states.size() > maximum_custom_states) {
        return defect_at(AnatomyDefect::too_many_item_custom_states, 0U);
    }
    for (std::size_t index = 0; index < anatomy.item_custom_states.size(); ++index) {
        const std::string_view name = anatomy.item_custom_states[index].name;
        if (!is_kebab_case_name(name)) {
            return defect_at(AnatomyDefect::item_custom_state_not_kebab_case, index);
        }
        const std::optional<AnatomyState> standard = anatomy_state_from_name(name);
        if (standard.has_value()) {
            return defect_at(AnatomyDefect::item_custom_state_shadows_standard_state, index);
        }
        for (std::size_t earlier = 0; earlier < index; ++earlier) {
            if (anatomy.item_custom_states[earlier].name == name) {
                return defect_at(AnatomyDefect::duplicate_item_custom_state, index);
            }
        }
    }
    AnatomyValidation valid;
    return valid;
}

} // namespace gui_forms
