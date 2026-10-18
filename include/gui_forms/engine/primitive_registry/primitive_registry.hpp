#pragma once

#include "gui_forms/anatomy/types/anatomy_types.hpp"
#include "gui_forms/engine/types/engine_types.hpp"
#include "gui_forms/types/geometry/geometry.hpp"
#include "gui_forms/types/paint_types/paint_types.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>

namespace gui_forms {

// The engine's primitive registry: the extension seam of stage T2 (brief
// item 10). A primitive is one kind of effect, written in C++ under the house
// style. It declares, in the words of the atom vocabulary (design language
// §12.7), the part roles it paints and the states it reads, together with its
// parameters, its cost bound and how far it draws outside the part. Theme
// data selects a primitive by name and sets its parameters; it never adds
// code. Registration is explicit and checked.

enum class PrimitiveParameterKind : std::uint8_t {
    number,
    colour,
};

// One declared parameter. A number must lie in [minimum, maximum]. A colour's
// alpha scales whatever the primitive uses the colour for.
struct PrimitiveParameterDeclaration final {
    std::string_view name;
    PrimitiveParameterKind kind{PrimitiveParameterKind::number};
    double minimum{};
    double maximum{};
    double default_number{};
    Color default_colour{};
};

// A declared upper bound on the work of one rasterization: at most
// `per_device_pixel` arithmetic operations for each destination pixel, plus
// `fixed` for the whole part. Budgets are checked against it at load (E4).
struct PrimitiveCost final {
    std::uint32_t per_device_pixel{};
    std::uint32_t fixed{};
};

// What a primitive is. Every span refers to static storage owned by the
// primitive's translation unit.
struct PrimitiveDescriptor final {
    // Registered (snake_case) name, as theme documents write it.
    std::string_view name;
    // The engine version that introduced the primitive. A theme needing a
    // later version than the engine implements falls back (E4).
    EngineVersion introduced{};
    // The part roles the primitive paints: at least one.
    std::span<const PartRole> roles;
    // The published states the primitive's output depends on. A change to
    // any other state never repaints it.
    std::span<const AnatomyState> states_read;
    std::span<const PrimitiveParameterDeclaration> parameters;
    PrimitiveCost cost{};
    // How far the primitive draws outside the part's bounds, in logical
    // units; zero for a primitive that stays inside them.
    Insets outsets{};
};

inline constexpr std::size_t maximum_primitive_parameters = 24;

// One parameter's value. `number` is read for a number parameter and
// `colour` for a colour parameter.
struct PrimitiveParameterValue final {
    PrimitiveParameterKind kind{PrimitiveParameterKind::number};
    double number{};
    Color colour{};

    friend constexpr bool operator==(const PrimitiveParameterValue& left,
                                     const PrimitiveParameterValue& right) noexcept {
        return left.kind == right.kind && left.number == right.number && left.colour == right.colour;
    }
};

// The values of one part's parameters: `count` live values, one per
// declared parameter, in declaration order.
struct PrimitiveParameterBlock final {
    std::array<PrimitiveParameterValue, maximum_primitive_parameters> values{};
    std::size_t count{};
};

enum class ParameterDefect : std::uint8_t {
    none,
    wrong_count,
    wrong_kind,
    not_finite,
    out_of_range,
};

// The first defect found and the index of the parameter it concerns.
struct ParameterValidation final {
    ParameterDefect defect{ParameterDefect::none};
    std::size_t parameter{};
};

// The block of a descriptor's declared defaults.
[[nodiscard]] PrimitiveParameterBlock default_parameters(const PrimitiveDescriptor& descriptor) noexcept;

// Checks every value against its declaration.
[[nodiscard]] ParameterValidation validate_parameters(const PrimitiveDescriptor& descriptor,
                                                      const PrimitiveParameterBlock& block) noexcept;

[[nodiscard]] std::optional<std::size_t> find_parameter(const PrimitiveDescriptor& descriptor,
                                                        std::string_view name) noexcept;

enum class ParameterAssignment : std::uint8_t {
    assigned,
    unknown_name,
    wrong_kind,
    not_finite,
    out_of_range,
};

// Set one parameter of `block` by name. Anything but `assigned` leaves the
// block unchanged.
[[nodiscard]] ParameterAssignment set_number_parameter(PrimitiveParameterBlock& block,
                                                       const PrimitiveDescriptor& descriptor,
                                                       std::string_view name, double value) noexcept;
[[nodiscard]] ParameterAssignment set_colour_parameter(PrimitiveParameterBlock& block,
                                                       const PrimitiveDescriptor& descriptor,
                                                       std::string_view name, Color value) noexcept;

// The part a primitive draws: its size in logical units (the box the
// primitive's geometry is measured in), the display scale, and its
// published states. The destination's extent is the caller's choice,
// normally the logical size times the scale, rounded up.
struct PrimitiveTarget final {
    Size logical_size{};
    DeviceScale scale{};
    AnatomyStateSet states{};
};

enum class RasterOutcome : std::uint8_t {
    // Every pixel of the destination was written.
    painted,
    // The destination has no pixels; nothing was written.
    empty,
    // Nothing was written: the destination's shape does not fit its buffer.
    invalid_view,
    // Nothing was written: the size or scale is out of range.
    invalid_target,
    // Nothing was written: the parameters do not match the declarations.
    invalid_parameters,
};

// One kind of effect. A primitive is immutable and holds no per-part state:
// one instance serves every part, on the UI thread.
class EnginePrimitive {
public:
    virtual ~EnginePrimitive() = default;

    [[nodiscard]] virtual const PrimitiveDescriptor& descriptor() const noexcept = 0;

    // Validates the destination, the target and the parameters once, then
    // fills every pixel of the destination with premultiplied RGBA8.
    // Anything but `painted` leaves the destination's bytes unchanged.
    [[nodiscard]] RasterOutcome rasterize(const PrimitiveParameterBlock& parameters,
                                          const PrimitiveTarget& target,
                                          const PartRasterView& destination) const noexcept;

private:
    // Called only with a validated, non-empty destination, a target whose
    // size is positive and finite and whose scale is in range, and
    // parameters that match the descriptor.
    virtual void fill(const PrimitiveParameterBlock& parameters, const PrimitiveTarget& target,
                      const PartRasterView& destination) const noexcept = 0;
};

enum class RegistrationOutcome : std::uint8_t {
    registered,
    registry_full,
    name_not_snake_case,
    duplicate_name,
    no_roles,
    role_out_of_range,
    state_out_of_range,
    too_many_parameters,
    parameter_name_not_snake_case,
    duplicate_parameter_name,
    parameter_range_invalid,
    default_out_of_range,
    introduced_after_engine,
};

[[nodiscard]] const char* registration_outcome_name(RegistrationOutcome outcome) noexcept;

inline constexpr std::size_t maximum_registered_primitives = 64;

// A fixed table of primitives, in registration order. It borrows each
// primitive, which must outlive the registry; the first-party primitives
// have static storage.
class PrimitiveRegistry final {
public:
    // Checks the primitive's descriptor and adds it. Anything but
    // `registered` leaves the registry unchanged.
    [[nodiscard]] RegistrationOutcome register_primitive(const EnginePrimitive& primitive) noexcept;

    // The primitive registered under `name`, or null.
    [[nodiscard]] const EnginePrimitive* find(std::string_view name) const noexcept;

    [[nodiscard]] std::span<const EnginePrimitive* const> primitives() const noexcept;

private:
    std::array<const EnginePrimitive*, maximum_registered_primitives> entries_{};
    std::size_t count_{};
};

// The first-party primitives, registered once in a fixed order on first use.
// The registry and its primitives live for the rest of the program.
[[nodiscard]] const PrimitiveRegistry& builtin_primitive_registry();

} // namespace gui_forms
