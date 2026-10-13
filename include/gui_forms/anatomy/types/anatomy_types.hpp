#pragma once

#include "gui_forms/types.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>

namespace gui_forms {

// Control anatomy: the named parts and states a control type publishes
// (design language §8.1 and §8.4; stage T1). Part and state names are a
// versioned public contract. Within a major version names are only added,
// never renamed or removed. Theme and form documents spell them in
// kebab-case; the C++ enumerators below are snake_case and map one to one,
// an underscore for each hyphen. Names never reuse a C++ keyword: the
// default-action cue of §8.1 and §8.4 is named `preselect`.

// Shared part roles. A cue a theme binds to (role, state) reaches every part
// with that role in every control (§8.2).
enum class PartRole : std::uint8_t {
    region,
    shadow,
    face,
    edge,
    keyline,
    perimeter,
    sheen,
    seam,
    divider,
    caption,
    cap,
    well,
    track,
    fill,
    thumb,
    actuator,
    indicator,
    activity,
    glyph,
    icon,
    label,
    content,
    caret,
    disclosure,
    grip,
    chip,
    focus,
    preselect,
    selection,
    validation,
    drop,
    count,
};

// The §8.4 state set. Display scale, text scale and density are scalar
// environment inputs rather than states and are not listed here.
enum class AnatomyState : std::uint8_t {
    // interaction
    normal,
    hot,
    pressed,
    captured,
    // focus and default
    focused,
    focus_visible,
    preselect,
    // value
    selected,
    checked,
    indeterminate,
    current,
    active_tool,
    // availability
    enabled,
    disabled,
    unavailable,
    read_only,
    // validity
    invalid,
    pending,
    // disclosure
    open,
    closed,
    expanding,
    collapsing,
    interrupted,
    // drag
    drop_accepted,
    drop_rejected,
    // participation (§8.3)
    key,
    secondary_participating,
    drag_proximate,
    deactivated,
    // environment
    high_contrast,
    reduced_motion,
    count,
};

inline constexpr std::size_t part_role_count =
    static_cast<std::size_t>(PartRole::count);
inline constexpr std::size_t anatomy_state_count =
    static_cast<std::size_t>(AnatomyState::count);
// Upper bounds that size the fixed tables and bit sets below. A control type
// declaring more is rejected by validate_control_anatomy.
inline constexpr std::size_t maximum_control_parts = 32;
inline constexpr std::size_t maximum_custom_states = 16;
static_assert(anatomy_state_count <= 64,
              "AnatomyStateSet stores standard states in 64 bits");

// Position of a part in its control type's declaration list. Distinct from
// the part's role (shared vocabulary) and from its name (public identity).
struct PartIndex final {
    std::uint8_t value{};

    friend constexpr bool operator==(PartIndex left, PartIndex right) noexcept {
        return left.value == right.value;
    }
};

// Position of an application-defined state in its control type's
// declaration list.
struct CustomStateIndex final {
    std::uint8_t value{};

    friend constexpr bool operator==(CustomStateIndex left,
                                     CustomStateIndex right) noexcept {
        return left.value == right.value;
    }
};

// One declared part. `name` is the part's stable kebab-case identity within
// its control type, as written in `::part(name)`. `composed_from` names the
// control id of a simpler system the part reuses (a scroll arrow is a
// "button"), or is empty.
struct PartDeclaration final {
    std::string_view name;
    PartRole role{PartRole::region};
    std::string_view composed_from;
};

// One application-defined finite state, as written in `:state(name)`.
struct CustomStateDeclaration final {
    std::string_view name;
};

// The anatomy of one control type. Every span refers to static storage
// owned by the control type's translation unit, so a ControlAnatomy may be
// copied and retained for the life of the program.
struct ControlAnatomy final {
    // The control's id in manifests/controls.json, or "control" for the
    // anatomy every control inherits.
    std::string_view control_id;
    std::span<const PartDeclaration> parts;
    std::span<const AnatomyState> states;
    std::span<const CustomStateDeclaration> custom_states;
};

// Geometry of one declared part in the control's local coordinate space
// after layout. `present` is false when the part does not exist in the
// control's current configuration (a track bar's fill in the classic style);
// `bounds` is then empty. Presence does not depend on transient state: a
// focus part is present while the control is unfocused.
struct PartGeometry final {
    Rect bounds{};
    bool present{};

    friend constexpr bool operator==(const PartGeometry& left,
                                     const PartGeometry& right) noexcept {
        return left.bounds == right.bounds && left.present == right.present;
    }
};

// A bounded set of active standard and custom states.
class AnatomyStateSet final {
public:
    constexpr void insert(AnatomyState state) noexcept {
        standard_bits_ |= standard_bit(state);
    }
    constexpr void erase(AnatomyState state) noexcept {
        standard_bits_ &= ~standard_bit(state);
    }
    [[nodiscard]] constexpr bool contains(AnatomyState state) const noexcept {
        return (standard_bits_ & standard_bit(state)) != 0U;
    }
    constexpr void insert(CustomStateIndex state) noexcept {
        custom_bits_ |= custom_bit(state);
    }
    [[nodiscard]] constexpr bool contains(CustomStateIndex state) const noexcept {
        return (custom_bits_ & custom_bit(state)) != 0U;
    }
    [[nodiscard]] constexpr std::uint64_t standard_bits() const noexcept {
        return standard_bits_;
    }
    [[nodiscard]] constexpr std::uint16_t custom_bits() const noexcept {
        return custom_bits_;
    }

    friend constexpr bool operator==(const AnatomyStateSet& left,
                                     const AnatomyStateSet& right) noexcept {
        return left.standard_bits_ == right.standard_bits_ &&
               left.custom_bits_ == right.custom_bits_;
    }

private:
    [[nodiscard]] static constexpr std::uint64_t standard_bit(
        AnatomyState state) noexcept {
        const std::uint64_t one = 1U;
        return one << static_cast<std::uint64_t>(state);
    }
    [[nodiscard]] static constexpr std::uint16_t custom_bit(
        CustomStateIndex state) noexcept {
        const unsigned int one = 1U;
        const unsigned int shifted = one << static_cast<unsigned int>(state.value);
        return static_cast<std::uint16_t>(shifted);
    }

    std::uint64_t standard_bits_{};
    std::uint16_t custom_bits_{};
};

// Kebab-case public names.
[[nodiscard]] std::string_view part_role_name(PartRole role) noexcept;
[[nodiscard]] std::string_view anatomy_state_name(AnatomyState state) noexcept;
[[nodiscard]] std::optional<PartRole> part_role_from_name(
    std::string_view name) noexcept;
[[nodiscard]] std::optional<AnatomyState> anatomy_state_from_name(
    std::string_view name) noexcept;

// True when `name` is non-empty lowercase ASCII words joined by single
// hyphens: [a-z][a-z0-9]*(-[a-z0-9]+)*.
[[nodiscard]] bool is_kebab_case_name(std::string_view name) noexcept;

// Lookup by declared name within one control type's anatomy.
[[nodiscard]] std::optional<PartIndex> find_part(
    const ControlAnatomy& anatomy, std::string_view part_name) noexcept;
[[nodiscard]] std::optional<CustomStateIndex> find_custom_state(
    const ControlAnatomy& anatomy, std::string_view state_name) noexcept;
[[nodiscard]] bool declares_state(const ControlAnatomy& anatomy,
                                  AnatomyState state) noexcept;

// Reasons a declared anatomy is malformed.
enum class AnatomyDefect : std::uint8_t {
    none,
    control_id_not_kebab_case,
    no_parts,
    too_many_parts,
    part_name_not_kebab_case,
    duplicate_part_name,
    part_role_out_of_range,
    composed_from_not_kebab_case,
    state_out_of_range,
    duplicate_state,
    too_many_custom_states,
    custom_state_not_kebab_case,
    custom_state_shadows_standard_state,
    duplicate_custom_state,
};

// The result of validate_control_anatomy: the first defect found, or none.
struct AnatomyValidation final {
    AnatomyDefect defect{AnatomyDefect::none};
    // Index into the list the defect concerns (parts, states or custom
    // states); zero when the defect concerns the anatomy as a whole.
    std::size_t index{};
};

[[nodiscard]] std::string_view anatomy_defect_name(AnatomyDefect defect) noexcept;
[[nodiscard]] AnatomyValidation validate_control_anatomy(
    const ControlAnatomy& anatomy) noexcept;

// The anatomy every control inherits until its type declares its own: the
// granted region and the focus part.
[[nodiscard]] const ControlAnatomy& base_control_anatomy() noexcept;

} // namespace gui_forms
