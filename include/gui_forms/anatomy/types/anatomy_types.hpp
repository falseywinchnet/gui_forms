#pragma once

#include "gui_forms/types.hpp"

#include <array>
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
    // The anatomy of one repeated item (a list row, a tree node), empty for
    // a control without items. Item part names never repeat the control's
    // own part names, so a document addresses either by name alone.
    std::span<const PartDeclaration> item_parts;
    std::span<const AnatomyState> item_states;
    // Application-defined finite states of one item, as for a control.
    std::span<const CustomStateDeclaration> item_custom_states;
};

// Geometry of one declared part in the control's local coordinate space
// after layout. `present` is false when the part does not exist in the
// control's current configuration (a track bar's fill in the classic style);
// `bounds` is then empty. Presence does not depend on transient state: a
// focus part is present while the control is unfocused.
struct PartGeometry final {
    Rect bounds{};
    bool present{};

    // A part that is present exactly when `bounds` is not empty.
    [[nodiscard]] static constexpr PartGeometry from_bounds(Rect bounds) noexcept {
        PartGeometry geometry;
        geometry.bounds = bounds;
        geometry.present = !bounds.empty();
        return geometry;
    }

    friend constexpr bool operator==(const PartGeometry& left,
                                     const PartGeometry& right) noexcept {
        return left.bounds == right.bounds && left.present == right.present;
    }
};

// Storage for one control's published part geometry: `count` live records,
// one per declared part, in declaration order.
struct PartGeometryBuffer final {
    std::array<PartGeometry, maximum_control_parts> geometry{};
    std::size_t count{};

    [[nodiscard]] std::span<const PartGeometry> parts() const noexcept {
        const std::span<const PartGeometry> live(geometry.data(), count);
        return live;
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

// Lookup by declared name within one declaration list, within a control
// type's own parts, or within its item parts.
[[nodiscard]] std::optional<PartIndex> find_declared_part(
    std::span<const PartDeclaration> declared, std::string_view part_name) noexcept;
[[nodiscard]] std::optional<PartIndex> find_part(
    const ControlAnatomy& anatomy, std::string_view part_name) noexcept;
[[nodiscard]] std::optional<PartIndex> find_item_part(
    const ControlAnatomy& anatomy, std::string_view part_name) noexcept;
[[nodiscard]] std::optional<CustomStateIndex> find_custom_state(
    const ControlAnatomy& anatomy, std::string_view state_name) noexcept;
[[nodiscard]] std::optional<CustomStateIndex> find_item_custom_state(
    const ControlAnatomy& anatomy, std::string_view state_name) noexcept;
[[nodiscard]] bool declares_state(const ControlAnatomy& anatomy,
                                  AnatomyState state) noexcept;

// Typed records (stage T1). C++ reads a control's parts and states as dot
// members of a record (`parts.thumb.bounds`, `states.pressed`); kebab-case
// names are identities for documents and tools only. A field table pairs
// each record member with the declaration it publishes, so a type's
// declared names and its record members come from one table.

// One member of a parts record and the part it publishes.
template <typename Record>
struct PartField final {
    PartDeclaration declaration;
    PartGeometry Record::*member{};
};

// One member of a states record and the standard state it reports.
template <typename Record>
struct StateField final {
    AnatomyState state{AnatomyState::normal};
    bool Record::*member{};
};

template <typename Record, std::size_t Count>
[[nodiscard]] constexpr std::array<PartDeclaration, Count> part_declarations(
    const std::array<PartField<Record>, Count>& fields) {
    std::array<PartDeclaration, Count> declarations{};
    for (std::size_t index = 0; index < Count; ++index) {
        declarations[index] = fields[index].declaration;
    }
    return declarations;
}

template <std::size_t First, std::size_t Second>
[[nodiscard]] constexpr std::array<PartDeclaration, First + Second> joined_declarations(
    const std::array<PartDeclaration, First>& first,
    const std::array<PartDeclaration, Second>& second) {
    std::array<PartDeclaration, First + Second> joined{};
    for (std::size_t index = 0; index < First; ++index) {
        joined[index] = first[index];
    }
    for (std::size_t index = 0; index < Second; ++index) {
        joined[First + index] = second[index];
    }
    return joined;
}

template <typename Record, std::size_t Count>
[[nodiscard]] constexpr std::array<AnatomyState, Count> state_declarations(
    const std::array<StateField<Record>, Count>& fields) {
    std::array<AnatomyState, Count> states{};
    for (std::size_t index = 0; index < Count; ++index) {
        states[index] = fields[index].state;
    }
    return states;
}

// Publishes every member of `record` into the part `anatomy` declares with
// that member's name. A part the anatomy does not declare is skipped, so a
// base type's record publishes into a subclass's longer declaration list.
// `geometry` holds one record per declared part.
template <typename Record, std::size_t Count>
void write_declared_parts(std::span<const PartDeclaration> declared,
                          const std::array<PartField<Record>, Count>& fields,
                          const Record& record, std::span<PartGeometry> geometry) noexcept {
    for (std::size_t index = 0; index < Count; ++index) {
        const std::optional<PartIndex> part =
            find_declared_part(declared, fields[index].declaration.name);
        if (!part.has_value() || (*part).value >= geometry.size()) {
            continue;
        }
        const PartGeometry& published = record.*(fields[index].member);
        geometry[(*part).value] = published;
    }
}

// Reads the published geometry back into `record`; a member whose part the
// anatomy does not declare is left absent.
template <typename Record, std::size_t Count>
void read_declared_parts(std::span<const PartDeclaration> declared,
                         const std::array<PartField<Record>, Count>& fields,
                         std::span<const PartGeometry> geometry, Record& record) noexcept {
    for (std::size_t index = 0; index < Count; ++index) {
        const std::optional<PartIndex> part =
            find_declared_part(declared, fields[index].declaration.name);
        PartGeometry value;
        if (part.has_value() && (*part).value < geometry.size()) {
            value = geometry[(*part).value];
        }
        record.*(fields[index].member) = value;
    }
}

template <typename Record, std::size_t Count>
void write_parts(const ControlAnatomy& anatomy,
                 const std::array<PartField<Record>, Count>& fields,
                 const Record& record, std::span<PartGeometry> geometry) noexcept {
    write_declared_parts(anatomy.parts, fields, record, geometry);
}

template <typename Record, std::size_t Count>
void read_parts(const ControlAnatomy& anatomy,
                const std::array<PartField<Record>, Count>& fields,
                std::span<const PartGeometry> geometry, Record& record) noexcept {
    read_declared_parts(anatomy.parts, fields, geometry, record);
}

// The same mapping over one realized item's geometry and the anatomy's
// item parts.
template <typename Record, std::size_t Count>
void write_item_parts(const ControlAnatomy& anatomy,
                      const std::array<PartField<Record>, Count>& fields,
                      const Record& record, std::span<PartGeometry> geometry) noexcept {
    write_declared_parts(anatomy.item_parts, fields, record, geometry);
}

template <typename Record, std::size_t Count>
void read_item_parts(const ControlAnatomy& anatomy,
                     const std::array<PartField<Record>, Count>& fields,
                     std::span<const PartGeometry> geometry, Record& record) noexcept {
    read_declared_parts(anatomy.item_parts, fields, geometry, record);
}

template <typename Record, std::size_t Count>
void read_states(const std::array<StateField<Record>, Count>& fields,
                 const AnatomyStateSet& active, Record& record) noexcept {
    for (std::size_t index = 0; index < Count; ++index) {
        record.*(fields[index].member) = active.contains(fields[index].state);
    }
}

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
    too_many_item_parts,
    item_part_name_not_kebab_case,
    duplicate_item_part_name,
    item_part_role_out_of_range,
    item_state_out_of_range,
    duplicate_item_state,
    too_many_item_custom_states,
    item_custom_state_not_kebab_case,
    item_custom_state_shadows_standard_state,
    duplicate_item_custom_state,
};

// The result of validate_control_anatomy: the first defect found, or none.
struct AnatomyValidation final {
    AnatomyDefect defect{AnatomyDefect::none};
    // Index into the list the defect concerns (parts, states, custom states,
    // item parts or item states); zero when the defect concerns the anatomy
    // as a whole.
    std::size_t index{};
};

[[nodiscard]] std::string_view anatomy_defect_name(AnatomyDefect defect) noexcept;
[[nodiscard]] AnatomyValidation validate_control_anatomy(
    const ControlAnatomy& anatomy) noexcept;

// The anatomy every control inherits until its type declares its own: the
// granted region and the focus part.
[[nodiscard]] const ControlAnatomy& base_control_anatomy() noexcept;

} // namespace gui_forms
