#pragma once

#include <algorithm>
#include <span>

namespace gui_forms::detail {

// An inclusive range of Unicode scalar values a face maps to a glyph.
struct CodepointRange final {
    char32_t first{};
    char32_t last{};
};

// Orders a scalar before the ranges that start after it.
struct StartsAfter final {
    bool operator()(char32_t scalar, const CodepointRange& range) const noexcept {
        const bool after = scalar < range.first;
        return after;
    }
};

// Whether `scalar` lies in one of `ranges`, which are sorted by `first` and disjoint (as the
// generated coverage tables are). The range that can hold it is the last one starting at or
// before it.
[[nodiscard]] inline bool coverage_contains(std::span<const CodepointRange> ranges,
                                            char32_t scalar) noexcept {
    const std::span<const CodepointRange>::iterator after =
        std::upper_bound(ranges.begin(), ranges.end(), scalar, StartsAfter{});
    if (after == ranges.begin()) return false;
    const CodepointRange& candidate = *(after - 1);
    const bool contained = scalar <= candidate.last;
    return contained;
}

} // namespace gui_forms::detail
