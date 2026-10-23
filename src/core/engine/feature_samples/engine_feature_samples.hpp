#pragma once

#include "gui_forms/anatomy/types/anatomy_types.hpp"
#include "gui_forms/resources/types/image_resource_types.hpp"
#include "gui_forms/theme/types/theme_types.hpp"
#include "gui_forms/types.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

// The engine's feature samples (T2 gate, E11). The gate's reference images
// cover every registered primitive (builtin_primitive_registry()), each drawn
// with its own defaults. Most of the engine's features are not primitives
// but layers and parameters of one: a material's fill kinds, shapes,
// borders, filters, blends, finish and mask, the procedural patterns, the
// glows' placements, glass's colour filter, the sheen at the pointer and the
// presentational motion. Each sample here is one theme that draws one part
// with some of them, so that the gate draws every feature too: it lists the
// samples as it lists the primitives, and a sample added here joins its
// manifest by being listed.
//
// A sample is a theme writer: given the professional theme's definition and
// the image the gate has loaded into the sample's window, it sets the
// recipes the part reads and adds the rule that draws it. The rule selects
// the part by its stable id, feature_sample_subject, and by the sample's
// role. The gate checks that every feature of the engine's vocabulary (each
// fill kind, spread, image mode, procedural pattern, blend, shape, border
// style, glow placement, filter, mask kind, presentational property, curve
// and easing) appears in at least one sample, by reading the themes the
// samples write (tests/engine_gate_tests.cpp, "the census").
//
// Every sample is static data and named functions; the list lives for the
// whole program.

namespace gui_forms::engine_samples {

// The part the gate draws for a sample: 96 by 28 at (20, 20) in its
// control, corner radius 4, granted 8 units of overflow on every side (so
// that a presentational scale or translation has room to show).
inline constexpr std::string_view feature_sample_subject = "subject";
inline constexpr Rect feature_sample_bounds{20.0, 20.0, 96.0, 28.0};
inline constexpr double feature_sample_corner_radius = 4.0;
inline constexpr Insets feature_sample_overflow{8.0, 8.0, 8.0, 8.0};

// The image a sample may draw: 8 by 8 premultiplied BGRA, rows 32 bytes
// apart, four opaque quadrants (red, green, blue, white) with a
// half-transparent grey ring one pixel in. The gate loads it into each
// sample's window before the theme, and passes its id to the writer.
inline constexpr std::uint32_t feature_sample_image_side = 8U;
[[nodiscard]] std::vector<std::byte> feature_sample_image_pixels();

// How the gate drives a sample once its window is made:
// - rest: drawn and settled (procedural patterns generated, idle keyframes
//   baked); one image per scale.
// - pointer: an image at rest, then the part turns hot (the engine reads
//   the pointer only over a hot part) and the pointer moves to
//   feature_sample_pointer, and an image once the window has settled.
// - hot_motion: an image at rest, then the part turns hot and an image is
//   taken after each of feature_sample_motion_frames 16 ms frames, and once
//   more when it has settled.
enum class FeatureSampleDrive : std::uint8_t { rest, pointer, hot_motion };

inline constexpr Point feature_sample_pointer{88.0, 34.0};
inline constexpr std::array<std::uint32_t, 4> feature_sample_motion_frames{2U, 5U, 9U, 14U};

// What a writer is given.
struct FeatureSampleInputs final {
    ImageId image{};
};

// A sample's theme writer: it changes `definition` (the professional
// theme's) to draw the sample, and nothing else.
using FeatureSampleWriter = void (*)(const FeatureSampleInputs& inputs, ThemeDefinition& definition);

struct EngineFeatureSample final {
    // snake_case, unique: the manifest's key.
    std::string_view name{};
    // The registered primitive the sample's rule declares.
    std::string_view primitive{};
    // The role of the part the gate draws.
    PartRole role{PartRole::face};
    FeatureSampleDrive drive{FeatureSampleDrive::rest};
    FeatureSampleWriter write_theme{};
};

// The first-party samples, in a fixed order.
[[nodiscard]] std::span<const EngineFeatureSample> builtin_feature_samples() noexcept;

} // namespace gui_forms::engine_samples
