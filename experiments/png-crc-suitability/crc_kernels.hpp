#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace crc_study {

// Every update borrows exactly the live bytes for this call, allocates nothing,
// and returns a finalized IEEE CRC. Pass zero for a new stream.
using Update = std::uint32_t (*)(std::span<const std::byte>, std::uint32_t);
struct Candidate {
    const char* name;
    Update update;
    std::size_t table_bytes;
    std::size_t state_bytes;
    bool streaming;
};

extern const std::array<Candidate, 17> candidates;
using Matrix = std::array<std::uint32_t, 32>;
using Powers = std::array<Matrix, 64>;
extern const Powers zero_powers;

std::uint32_t reference(std::span<const std::byte> bytes, std::uint32_t seed);
std::uint32_t shift(std::uint32_t value, std::uint64_t bytes);
std::uint32_t combine(std::uint32_t left, std::uint32_t right, std::uint64_t right_size);
Matrix prepare_shift(std::uint64_t bytes);
std::uint32_t apply(const Matrix& matrix, std::uint32_t value);

} // namespace crc_study
