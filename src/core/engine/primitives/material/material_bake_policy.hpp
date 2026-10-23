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
    // Per device pixel, for each gradient stop past the first two.
    double gradient_stop{};
    // Per device pixel, for each outer shadow: its separable integrals.
    double outer_shadow{};
    // Per device pixel within a rounded outer shadow's corners and its
    // reach, for each such shadow: its signed distance (E9).
    double outer_corner{};
    double inset_shadow{};
    double border{};
    double border_edge{};
    double keyline{};
    // Per device pixel, for each image fill: compositing its prepared
    // variant's colour. The variant's decode and resampling are not the
    // bake's (image_variant.hpp).
    double image{};
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
inline constexpr MaterialBakeCosts measured_material_costs{
    .base = 26.0,
    .solid = 10.0,
    .linear = 15.0,
    .radial = 22.0,
    .gradient_stop = 0.5,
    .outer_shadow = 7.0,
    .outer_corner = 28.0,
    .inset_shadow = 60.0,
    .border = 26.0,
    .border_edge = 11.0,
    .keyline = 13.0,
    .image = 6.0,
};

} // namespace gui_forms::engine_primitives
