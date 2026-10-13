#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

namespace gui_forms {

// Interactions: the events a control or component type raises, as the one
// schema names them (stage T1). C++ registers a handler through the type's
// dot API (`(*button).clicked().subscribe(...)`); form documents and tools
// name the same interaction in kebab-case (`clicked`, `value-changed`), a
// hyphen for each underscore in the accessor's name. Names are a versioned
// public contract: within a major version they are only added.

// One interaction. `payload` spells the handler's argument types as the
// event accessor declares them, without `const` or reference marks and
// separated by ", " ("ButtonBase", "std::size_t, CheckState"); it is empty
// for an interaction without arguments. `mutable_payload` is true when some
// argument reaches the handler by mutable reference, as a validation the
// handler may cancel does.
struct InteractionDeclaration final {
    std::string_view name;
    std::string_view payload;
    bool mutable_payload{};

    friend constexpr bool operator==(const InteractionDeclaration& left,
                                     const InteractionDeclaration& right) noexcept {
        return left.name == right.name && left.payload == right.payload &&
               left.mutable_payload == right.mutable_payload;
    }
};

// Upper bound on one type's interactions, its inherited ones included.
inline constexpr std::size_t maximum_interactions = 64;

// Reasons a type's interaction list is malformed.
enum class InteractionDefect : std::uint8_t {
    none,
    too_many_interactions,
    name_not_kebab_case,
    duplicate_name,
};

// The result of validate_interactions: the first defect found and the index
// of the interaction it concerns, or none.
struct InteractionValidation final {
    InteractionDefect defect{InteractionDefect::none};
    std::size_t index{};
};

[[nodiscard]] std::string_view interaction_defect_name(InteractionDefect defect) noexcept;
[[nodiscard]] InteractionValidation validate_interactions(
    std::span<const InteractionDeclaration> interactions) noexcept;
// The position of the interaction named `name`, or `interactions.size()`.
[[nodiscard]] std::size_t find_interaction(std::span<const InteractionDeclaration> interactions,
                                           std::string_view name) noexcept;

} // namespace gui_forms
