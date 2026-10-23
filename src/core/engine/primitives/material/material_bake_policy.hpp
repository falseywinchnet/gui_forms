#pragma once

// The material primitive's bake policy: the one place where the bound and
// the costs it is checked with are set (owner direction, 2026-10-22).
//
// A part's raster is baked once, when it is rebuilt after an invalidation
// (a change of state, size, scale or theme), and then cached; compositing
// it afterwards costs a blit, plus a group blend where one applies, however
// complex the material. So a material's complexity costs only at bake time,
// and the bake is bounded so that no invalidation can stall: a material
// whose estimated bake exceeds maximum_material_bake_nanoseconds is refused,
// the part is drawn by the built-in recipe instead, and the refusal is
// counted (Metrics::record_engine_primitive_refusal).
//
// The estimate is the destination's device pixels (the part's box grown by
// the primitive's outsets) times the per-pixel cost of the material's
// layers, plus, for each image fill, its decode and its separable
// resampling. The costs are measured bake times, not guesses; see
// measured_material_costs for the workload and the machine.

namespace gui_forms::engine_primitives {

// At most 4 ms of baking for one part on the reference machine: a quarter
// of a 60 Hz frame (accepted by the owner, 2026-10-22; change it here).
inline constexpr double maximum_material_bake_nanoseconds = 4'000'000.0;

// Bake costs, in nanoseconds on the reference machine.
struct MaterialBakeCosts final {
    // Per device pixel of the destination: the pixel loop, the box's
    // coverage and the store, with no layer drawn.
    double base{};
    // Per device pixel, for each layer of its kind.
    double solid{};
    double linear{};
    double radial{};
    double conic{};
    // Per device pixel, for each gradient stop past the first two.
    double gradient_stop{};
    // Per device pixel, for each outer shadow: its separable integrals.
    double outer_shadow{};
    // Per device pixel within a rounded outer shadow's corners and its
    // reach, for each such shadow: its signed distance (E9).
    double outer_corner{};
    double inset_shadow{};
    double border{};
    // Per device pixel, for a dashed or dotted border, past a solid one's.
    double border_pattern{};
    double border_edge{};
    double keyline{};
    // Per device pixel, for each image fill: compositing its prepared
    // variant's colour. The variant's decode and resampling are not the
    // bake's (image_variant.hpp).
    double image{};
    // Per device pixel of the destination, for each part filter (E9.6): the
    // colour matrix, the drop shadow (its alpha blurred and composited) and
    // the blur (every channel blurred). A box blur's cost per pixel does
    // not depend on its radius, but it blurs an image grown by 3h on every
    // side, which is not charged: on a small part with a wide blur the
    // charge is low by that ratio, on a part far within the bound.
    double colour_filter{};
    double drop_shadow{};
    double filter_blur{};
    // Per device pixel, once for a material whose fills blend other than
    // source-over (E9.6): gathering them into one face.
    double fill_blend{};
};

// Measured by gui_forms_engine_material_tests --measure: for each layer
// kind, the median of nine bakes of a 240-unit square part (a 256 × 256
// destination with the outsets) at scale 1, less the median bake of the base
// material, divided by the pixels; an image is a prepared variant of the
// box composited (until 2026-10-22 it was resampled in the bake).
// Release build (clang 22, -O2) on Windows 11, AMD EPYC 9354 virtual
// machine with 8 logical CPUs, 2026-10-22, while other builds held the CPUs
// at 88-97% load. Each figure is the median of seven runs, rounded up; the
// load makes them pessimistic. image is below the noise at this workload
// (single runs measured below zero), so it carries
// a conservative upper figure rather than a measurement. The runs are
// recorded in planning/stages/T2-engine-core/README.md, "E8c bake costs".
// outer_shadow and outer_corner are PROVISIONAL: they were measured again
// for E9's separable outer shadow, on the same machine on 2026-10-22, with
// the CPU load not sampled (lighter than before, judged by the unchanged
// inset shadow: 48 ns against 59). Each is the median of seven runs scaled
// by 1.25 (that ratio) and rounded up; see "E9 shadow costs" there. The
// quiet-machine re-measure, with the load recorded, replaces every figure.
// conic and border_pattern (E9.6) are PROVISIONAL too: measured on the same
// machine on 2026-10-22 at 100 % CPU load (sampled), the inset shadow at
// 57 ns, as in the E8c runs, so unscaled. Each is the median of seven runs,
// rounded up. border_pattern is per pixel of a dashed ring whose every
// pixel is on an arc, its dearest; it is charged per destination pixel.
// See "E9.6 G1 costs" there. colour_filter, drop_shadow, filter_blur and
// fill_blend (E9.6 G2) are PROVISIONAL, measured the same way on 2026-10-23
// at 96-97 % CPU load (sampled), the inset shadow at 59 ns, so unscaled;
// see "E9.6 G2 costs" there.
inline constexpr MaterialBakeCosts measured_material_costs{
    .base = 26.0,
    .solid = 10.0,
    .linear = 15.0,
    .radial = 22.0,
    .conic = 52.0,
    .gradient_stop = 0.5,
    .outer_shadow = 7.0,
    .outer_corner = 28.0,
    .inset_shadow = 60.0,
    .border = 26.0,
    .border_pattern = 54.0,
    .border_edge = 11.0,
    .keyline = 13.0,
    .image = 6.0,
    .colour_filter = 13.0,
    .drop_shadow = 36.0,
    .filter_blur = 55.0,
    .fill_blend = 22.0,
};

} // namespace gui_forms::engine_primitives
