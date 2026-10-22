#pragma once

#include "gui_forms/engine/part_shape/part_shape.hpp"
#include "gui_forms/engine/primitive_registry/primitive_registry.hpp"

#include <span>

namespace gui_forms::engine_primitives {

// The relief set of design language §3.5, drawn by engine kernels (T2 plan,
// phase E8). The defaults are Plan Paint's values (`falseywinchnet/plan-paint`
// at 6b901fc, src/forms/ribbon.cpp: `action_material`, `gallery_material`,
// the gallery well and `paint_separator`; rendering reference §1 and §5),
// and every colour is a parameter a theme may change.
//
// Geometry common to the plates and the well, in device pixels at scale s:
// - k = max(1, floor(min(s, 2) + 0.5)) is the width of every line, border
//   and the contact shadow (§3.6 keyline rule);
// - the plate fills the part's box except its bottom k rows, where the
//   contact shadow (or the well's outer highlight) lies, so the part never
//   draws outside its bounds and its geometry does not move between states;
// - edges are anti-aliased by the shape's signed distance (shape_coverage),
//   and every layer is blended premultiplied in double, quantized once.

// The pill plate (§3.5 "Pill plate", "Pressed", "Default", "Disabled";
// Plan Paint `action_material`): a vertical tinted gradient face, one thin
// border, inner light top and left lines and dark bottom and right lines,
// and a one-pixel contact shadow. Hot brightens the face and darkens the
// border; pressed darkens the face, inverts the inner lines and drops the
// shadow; disabled pales the face and border and keeps only the top line;
// preselect (the default action) adds a second dark perimeter around the
// plate. Its shape is a parameter (`shape`, `corner_radius`).
class PillPlatePrimitive final : public EnginePrimitive {
public:
    [[nodiscard]] const PrimitiveDescriptor& descriptor() const noexcept override;
    [[nodiscard]] PartShape shape(const PrimitiveParameterBlock& parameters) const noexcept override;

private:
    void derive(const PrimitiveParameterBlock& parameters, const PrimitiveTarget& target,
                std::span<double> values) const noexcept override;
    void fill(const PrimitiveParameterBlock& parameters, const PrimitiveTarget& target,
              std::span<const double> prepared, const PartRasterView& destination) const noexcept override;
};

// The gold active plate (§3.5 "Active tool"; Plan Paint
// `gallery_material(true, ...)`): a warm gold gradient plate with a deeper
// gold border, a light top and left line and a faint bottom line, flat (no
// contact shadow), radius 2. Pressed deepens the gradient.
class GoldPlatePrimitive final : public EnginePrimitive {
public:
    [[nodiscard]] const PrimitiveDescriptor& descriptor() const noexcept override;
    [[nodiscard]] PartShape shape(const PrimitiveParameterBlock& parameters) const noexcept override;

private:
    void derive(const PrimitiveParameterBlock& parameters, const PrimitiveTarget& target,
                std::span<double> values) const noexcept override;
    void fill(const PrimitiveParameterBlock& parameters, const PrimitiveTarget& target,
              std::span<const double> prepared, const PartRasterView& destination) const noexcept override;
};

// The inset well (§3.5 "Inset well"; Plan Paint's gallery well): a face
// lightening downward, an inner top shade line, a thin border and an outer
// bottom highlight.
class InsetWellPrimitive final : public EnginePrimitive {
public:
    [[nodiscard]] const PrimitiveDescriptor& descriptor() const noexcept override;
    [[nodiscard]] PartShape shape(const PrimitiveParameterBlock& parameters) const noexcept override;

private:
    void derive(const PrimitiveParameterBlock& parameters, const PrimitiveTarget& target,
                std::span<double> values) const noexcept override;
    void fill(const PrimitiveParameterBlock& parameters, const PrimitiveTarget& target,
              std::span<const double> prepared, const PartRasterView& destination) const noexcept override;
};

// The etched divider (§3.5 "Etched"; Plan Paint `paint_separator`): a shade
// line and a light line side by side, along the part's long axis, each
// fading in over the first 16 % of the length and out over the last 17 %.
class EtchedDividerPrimitive final : public EnginePrimitive {
public:
    [[nodiscard]] const PrimitiveDescriptor& descriptor() const noexcept override;

private:
    void derive(const PrimitiveParameterBlock& parameters, const PrimitiveTarget& target,
                std::span<double> values) const noexcept override;
    void fill(const PrimitiveParameterBlock& parameters, const PrimitiveTarget& target,
              std::span<const double> prepared, const PartRasterView& destination) const noexcept override;
};

// The seam (§3.5 "Seam", §7.2): one thin line along the part's long axis,
// centred; the seam's wider hit area is its owner's.
class SeamPrimitive final : public EnginePrimitive {
public:
    [[nodiscard]] const PrimitiveDescriptor& descriptor() const noexcept override;

private:
    void derive(const PrimitiveParameterBlock& parameters, const PrimitiveTarget& target,
                std::span<double> values) const noexcept override;
    void fill(const PrimitiveParameterBlock& parameters, const PrimitiveTarget& target,
              std::span<const double> prepared, const PartRasterView& destination) const noexcept override;
};

// The grip (T2 GIVEN H2-1): the part's box filled with the dot pattern,
// two-unit dots on a four-unit pitch from one unit in, as the built-in
// recipe's grip mark draws them.
class GripPrimitive final : public EnginePrimitive {
public:
    [[nodiscard]] const PrimitiveDescriptor& descriptor() const noexcept override;

private:
    void derive(const PrimitiveParameterBlock& parameters, const PrimitiveTarget& target,
                std::span<double> values) const noexcept override;
    void fill(const PrimitiveParameterBlock& parameters, const PrimitiveTarget& target,
              std::span<const double> prepared, const PartRasterView& destination) const noexcept override;
};

} // namespace gui_forms::engine_primitives
