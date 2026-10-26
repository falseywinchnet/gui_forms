#pragma once

// Conversions between the public vocabulary (stx_cng/codec.hpp) and the
// engine's, and the checks every public entry point makes on caller storage.
#include "format.hpp"
#include "stx_cng/codec.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>

namespace stx_cng::detail {

[[nodiscard]] inline Shape shape_of(CngImage image) noexcept {
    Shape shape{image.width, image.height, static_cast<std::uint32_t>(image.channels)};
    return shape;
}

[[nodiscard]] inline CngImage image_of(Shape shape) noexcept {
    CngImage image{shape.width, shape.height, static_cast<CngChannels>(shape.channels)};
    return image;
}

// Byte views of caller storage as the engine's std::uint8_t.
[[nodiscard]] inline std::span<const std::uint8_t> bytes_of(std::span<const std::byte> bytes) noexcept {
    std::span<const std::uint8_t> view(reinterpret_cast<const std::uint8_t*>(bytes.data()), bytes.size());
    return view;
}

[[nodiscard]] inline std::span<std::uint8_t> bytes_of(std::span<std::byte> bytes) noexcept {
    std::span<std::uint8_t> view(reinterpret_cast<std::uint8_t*>(bytes.data()), bytes.size());
    return view;
}

[[nodiscard]] inline bool overlapping(std::span<const std::byte> left, std::span<const std::byte> right) noexcept {
    if (left.empty() || right.empty()) {
        return false;
    }
    std::less<const std::byte*> before{};
    const std::byte* left_end = left.data() + left.size();
    const std::byte* right_end = right.data() + right.size();
    bool overlap = before(left.data(), right_end) && before(right.data(), left_end);
    return overlap;
}

// The extent of an image whose rows are `row_bytes` apart: invalid_argument
// for a stride below width * channels, insufficient_storage for storage
// shorter than (height - 1) * row_bytes + width * channels.
[[nodiscard]] inline CngError check_extent(Shape shape, std::size_t storage, std::uint64_t row_bytes) noexcept {
    std::uint64_t row = static_cast<std::uint64_t>(shape.width) * shape.channels;
    if (row_bytes < row) {
        return CngError::invalid_argument;
    }
    std::uint64_t rows = shape.height - 1U;
    // rows * row_bytes cannot be formed when row_bytes alone exceeds storage.
    if (rows != 0 && row_bytes > storage / rows) {
        return CngError::insufficient_storage;
    }
    std::uint64_t required = rows * row_bytes + row;
    CngError error = required > storage ? CngError::insufficient_storage : CngError::none;
    return error;
}

// A decoder destination: its extent, then disjointness from the input.
[[nodiscard]] inline CngError check_destination(Shape shape, std::span<const std::byte> encoded, std::span<std::byte> samples,
                                                std::uint64_t row_bytes) noexcept {
    CngError error = check_extent(shape, samples.size(), row_bytes);
    if (error == CngError::none && overlapping(encoded, samples)) {
        error = CngError::overlap;
    }
    return error;
}

} // namespace stx_cng::detail
