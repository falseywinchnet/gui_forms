#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

// Private bounded native measurement storage. Disabled in ordinary use.
struct MacDisplayLinkTrace final {
    std::array<std::uint64_t, 2048> timestamps{};
    std::size_t count{0};
    bool overflow{false};
};
