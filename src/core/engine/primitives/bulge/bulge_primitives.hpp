#pragma once

#include "gui_forms/engine/primitive_registry/primitive_registry.hpp"

namespace gui_forms::engine_primitives {

// The ribbon button's cloth bulge, as the rendering reference specifies it
// (planning/stages/T4-default-theme/RENDERING_REFERENCE.md §2 and §3; design
// language §3.5). The plane itself swells: a lit slope toward the upper-left
// light, a shaded slope away from it and a faint crown, each a field
// 1 − t^q over its own ellipse. There is no edge of any kind. `pressed`
// turns the bulge into a dent and `active_tool` draws it in gold.
// `amplitude` scales the whole relief from 0 (the flat plane) to 1, which is
// the value the ribbon's rise under the pointer animates (RB decision 15).
//
// Both primitives paint an overlay: they write premultiplied pixels that are
// transparent wherever the plane beneath should show.

// The smooth form (reference §2): white, blue-grey and white layered over
// the plane in that order, each weighted by its field and peak.
class BulgePrimitive final : public EnginePrimitive {
public:
    [[nodiscard]] const PrimitiveDescriptor& descriptor() const noexcept override;

private:
    void fill(const PrimitiveParameterBlock& parameters, const PrimitiveTarget& target,
              const PartRasterView& destination) const noexcept override;
};

// The dithered form (reference §3): the signed tone of the same fields,
// sampled on a lattice of 1 logical px dots every 2 logical px, quantized
// onto three steps per side with an ordered 4×4 threshold. Between dots, and
// wherever the tone rounds to zero, the plane shows.
//
// At display scale s the lattice stays at exactly 2 logical px. The dot at
// logical (x, y) covers device pixels [floor(x·s), floor(x·s) + D) on each
// axis, with D = max(1, floor(s + 0.5)), and its tone is sampled at (x, y)
// as the reference formula reads. At integer scales the lattice is exact; at
// fractional scales a dot's position varies by up to one device pixel.
class DitheredBulgePrimitive final : public EnginePrimitive {
public:
    [[nodiscard]] const PrimitiveDescriptor& descriptor() const noexcept override;

private:
    void fill(const PrimitiveParameterBlock& parameters, const PrimitiveTarget& target,
              const PartRasterView& destination) const noexcept override;
};

} // namespace gui_forms::engine_primitives
