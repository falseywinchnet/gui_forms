#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

// The engine's integer blur (T2 plan, E9.3, shared by E9.6): three box
// passes across and three down, an approximation of a Gaussian, over
// 8-bit lanes held as 32-bit sums: four (premultiplied RGBA) or one (an
// alpha). Glass blurs the backplane with it; a material blurs its own
// raster and its drop shadow's alpha with it. Defined in box_blur.cpp;
// integer arithmetic, so every host gives the same bytes.

namespace gui_forms::engine_primitives {

// Four lanes, premultiplied RGBA, per pixel.
inline constexpr std::size_t box_blur_lanes = 4U;

// The half width h of the box filter for a blur of `blur` logical units at
// display `scale`, where σ = blur / 2 as a shadow's: three passes of width
// 2h + 1 have the variance of σ. 0 for no blur; at most 1024.
[[nodiscard]] std::uint32_t box_blur_half(double blur, double scale) noexcept;

// The work storage of one blur, owned by the caller and reusable from one
// blur to the next.
struct BoxBlurWork final {
    // Two images of columns × rows pixels, lanes interleaved, rows
    // columns × lanes elements apart: the passes go back and forth between
    // them.
    std::vector<std::uint32_t> first{};
    std::vector<std::uint32_t> second{};
    // One window sum per element of a row, for the passes down, which read
    // the images row by row.
    std::vector<std::uint32_t> sums{};
};

// Sizes `work` for an image of `columns` × `rows` pixels of `lanes` lanes
// (1 to 4), every element of `first` and `second` 0. Throws
// std::bad_alloc (or std::length_error) when the storage cannot be held;
// the sizes are then unspecified, and `work` must be prepared again before
// use. The caller bounds columns × rows × lanes by its raster bounds.
void prepare_box_blur(BoxBlurWork& work, std::size_t columns, std::size_t rows, std::size_t lanes);

// Blurs the image the caller loaded at the start of `work.first`, `work`
// prepared for these `columns`, `rows` and `lanes`; `second` and `sums`
// are overwritten. Each pass shortens its axis by 2h, so the result is
// (columns − 6h) × (rows − 6h) pixels, rows still columns × lanes elements
// apart, at the start of `first`: an image grown by 3h on every side gives
// back its own size. Requires columns and rows greater than 6h, and every
// element at most 255. Each average is rounded once, as
// (sum + h) / (2h + 1).
void blur_three_boxes(BoxBlurWork& work, std::size_t columns, std::size_t rows, std::size_t lanes,
                      std::uint32_t half) noexcept;

} // namespace gui_forms::engine_primitives
