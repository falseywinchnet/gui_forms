#include "crc_kernels.hpp"
#include "stx_zlib/checksum.hpp"

namespace crc_study {
namespace {

constexpr std::uint32_t polynomial = 0xedb88320U;

// Extend a little-endian register by zero bytes; a 64-bit register becomes
// the ordinary 32-bit remainder after at least four bytes of extension.
constexpr std::uint32_t advance(std::uint64_t value, std::size_t bytes) {
    for (std::size_t index = 0; index < bytes * 8U; ++index) {
        const std::uint64_t feedback = (value & 1U) * polynomial;
        value = (value >> 1U) ^ feedback;
    }
    return static_cast<std::uint32_t>(value);
}

template<std::size_t Width, std::size_t Stride, unsigned Bits = 8>
constexpr std::array<std::array<std::uint32_t, 1U << Bits>, Width * 8U / Bits>
make_table() {
    std::array<std::array<std::uint32_t, 1U << Bits>, Width * 8U / Bits> result{};
    for (std::size_t position = 0; position < result.size(); ++position) {
        for (std::size_t value = 0; value < (1U << Bits); ++value) {
            const std::uint64_t input = static_cast<std::uint64_t>(value) << (position * Bits);
            result[position][value] = advance(input, Stride);
        }
    }
    return result;
}

constexpr std::array<std::array<std::uint32_t, 256>, 16> make_slices() {
    std::array<std::array<std::uint32_t, 256>, 16> result{};
    for (std::size_t position = 0; position < 16; ++position) {
        for (std::size_t value = 0; value < 256; ++value) {
            result[position][value] = advance(value, position + 1U);
        }
    }
    return result;
}

constexpr std::array<std::array<std::uint32_t, 256>, 16> slices = make_slices();

template<std::size_t Width>
std::uint64_t load_word(std::span<const std::byte> bytes, std::size_t offset) {
    std::uint64_t word = 0;
    for (std::size_t index = 0; index < Width; ++index) {
        const std::uint64_t value = std::to_integer<std::uint64_t>(bytes[offset + index]);
        word |= value << (index * 8U);
    }
    return word;
}

template<std::size_t Width>
std::uint32_t reduce_word(std::uint64_t word) {
    std::uint32_t result = 0;
    for (std::size_t index = 0; index < Width; ++index) {
        result ^= slices[Width - index - 1U][(word >> (index * 8U)) & 255U];
    }
    return result;
}

std::uint32_t tail(std::span<const std::byte> bytes, std::size_t offset, std::uint32_t state) {
    for (; offset < bytes.size(); ++offset) {
        const std::uint32_t value = std::to_integer<std::uint32_t>(bytes[offset]);
        state = slices[0][(state ^ value) & 255U] ^ (state >> 8U);
    }
    const std::uint32_t result = ~state;
    return result;
}

template<std::size_t Width>
std::uint32_t sliced(std::span<const std::byte> bytes, std::uint32_t seed) {
    std::uint32_t state = ~seed;
    const std::size_t interior = bytes.size() - bytes.size() % Width;
    std::size_t offset = 0;
    for (; offset < interior; offset += Width) {
        const std::uint64_t word = load_word<4>(bytes, offset) ^ state;
        std::uint32_t next = 0;
        for (std::size_t index = 0; index < 4; ++index) {
            next ^= slices[Width - index - 1U][(word >> (index * 8U)) & 255U];
        }
        for (std::size_t index = 4; index < Width; ++index) {
            const std::size_t value = std::to_integer<std::size_t>(bytes[offset + index]);
            next ^= slices[Width - index - 1U][value];
        }
        state = next;
    }
    const std::uint32_t result = tail(bytes, offset, state);
    return result;
}

template<std::size_t Lanes, std::size_t Width, unsigned Bits = 8>
std::uint32_t braided(std::span<const std::byte> bytes, std::uint32_t seed) {
    constexpr std::size_t stride = Lanes * Width;
    if (bytes.size() < stride * 2U) {
        const std::uint32_t result = sliced<Width>(bytes, seed);
        return result;
    }
    static constexpr std::array<std::array<std::uint32_t, 1U << Bits>, Width * 8U / Bits>
        table = make_table<Width, stride, Bits>();
    std::array<std::uint32_t, Lanes> states{};
    states[0] = ~seed;
    const std::size_t interior = bytes.size() - bytes.size() % stride;
    std::size_t offset = 0;
    // Leave the final group for the ordered collapse. All preceding groups
    // advance each sparse lane by the same stride; the table is shared.
    for (; offset < interior - stride; offset += stride) {
        for (std::size_t lane = 0; lane < Lanes; ++lane) {
            const std::uint64_t word = load_word<Width>(bytes, offset + lane * Width) ^ states[lane];
            std::uint32_t next = 0;
            for (std::size_t position = 0; position < Width * 8U / Bits; ++position) {
                const std::size_t index = (word >> (position * Bits)) & ((1U << Bits) - 1U);
                next ^= table[position][index];
            }
            states[lane] = next;
        }
    }
    std::uint32_t state = 0;
    for (std::size_t lane = 0; lane < Lanes; ++lane) {
        const std::uint64_t word = load_word<Width>(bytes, offset + lane * Width);
        state = reduce_word<Width>(word ^ states[lane] ^ state);
    }
    const std::uint32_t result = tail(bytes, interior, state);
    return result;
}

constexpr Matrix make_jump() {
    Matrix result{};
    for (unsigned bit = 0; bit < 32; ++bit) {
        result[bit] = advance(std::uint32_t{1} << bit, 16);
    }
    return result;
}

// Same four-lane braid, expressed as an exact GF(2) matrix circuit.
// Four independent 32-bit values are the intended common 128-bit shape.
// No intrinsics, data-dependent lookups, or wider-vector requirement.
template<bool PreserveLanes>
std::uint32_t matrix_four(std::span<const std::byte> bytes, std::uint32_t seed) {
    if (bytes.size() < 32U) {
        const std::uint32_t result = sliced<4>(bytes, seed);
        return result;
    }
    static constexpr Matrix jump = make_jump();
    std::array<std::uint32_t, 4> states{~seed, 0U, 0U, 0U};
    const std::size_t interior = bytes.size() - bytes.size() % 16U;
    std::size_t offset = 0;
    for (; offset < interior - 16U; offset += 16U) {
        std::array<std::uint32_t, 4> inputs{};
        std::array<std::uint32_t, 4> next{};
        for (std::size_t lane = 0; lane < 4; ++lane) {
            inputs[lane] = static_cast<std::uint32_t>(load_word<4>(bytes, offset + lane * 4U));
            inputs[lane] ^= states[lane];
        }
        if constexpr (PreserveLanes) {
            // LLVM 22 otherwise vectorizes the 32-column reduction separately
            // for each state, introducing four horizontal reductions per group.
            // Preserve the bit loop so SLP can pack the four independent states.
            // This is a measured compiler-layout ablation, not a safety hint.
#if defined(__clang__)
#pragma clang loop unroll(disable) vectorize(disable) interleave(disable)
#endif
            for (unsigned bit = 0; bit < 32; ++bit) {
                for (std::size_t lane = 0; lane < 4; ++lane) {
                    const std::uint32_t mask = 0U - ((inputs[lane] >> bit) & 1U);
                    next[lane] ^= mask & jump[bit];
                }
            }
        } else {
            for (unsigned bit = 0; bit < 32; ++bit) {
                for (std::size_t lane = 0; lane < 4; ++lane) {
                    const std::uint32_t mask = 0U - ((inputs[lane] >> bit) & 1U);
                    next[lane] ^= mask & jump[bit];
                }
            }
        }
        states = next;
    }
    std::uint32_t state = 0;
    for (std::size_t lane = 0; lane < 4; ++lane) {
        const std::uint64_t word = load_word<4>(bytes, offset + lane * 4U);
        state = reduce_word<4>(word ^ states[lane] ^ state);
    }
    const std::uint32_t result = tail(bytes, interior, state);
    return result;
}

std::uint32_t baseline(std::span<const std::byte> bytes, std::uint32_t) {
    const std::uint32_t result = stx_zlib::crc32(bytes);
    return result;
}

constexpr std::uint32_t matrix_apply(const Matrix& matrix, std::uint32_t value) {
    std::uint32_t result = 0;
    for (unsigned bit = 0; bit < 32; ++bit) {
        const std::uint32_t mask = 0U - ((value >> bit) & 1U);
        result ^= matrix[bit] & mask;
    }
    return result;
}

constexpr Powers make_powers() {
    Powers result{};
    for (unsigned bit = 0; bit < 32; ++bit) {
        result[0][bit] = advance(std::uint32_t{1} << bit, 1);
    }
    for (std::size_t power = 1; power < 64; ++power) {
        for (unsigned bit = 0; bit < 32; ++bit) {
            result[power][bit] = matrix_apply(result[power - 1], result[power - 1][bit]);
        }
    }
    return result;
}

std::uint32_t reverse_bits(std::uint32_t input) {
    std::uint32_t result = 0;
    for (unsigned bit = 0; bit < 32; ++bit) {
        result = (result << 1U) | (input & 1U);
        input >>= 1U;
    }
    return result;
}

} // namespace

const Powers zero_powers = make_powers();

const std::array<Candidate, 17> candidates{{
    {"png-baseline", baseline, 8192, 4, false},
    {"slice4", sliced<4>, 4096, 4, true},
    {"slice8", sliced<8>, 8192, 4, true},
    {"slice16", sliced<16>, 16384, 4, true},
    {"braid2x4", braided<2, 4>, 8192, 8, true},
    {"braid3x4", braided<3, 4>, 8192, 12, true},
    {"braid4x4", braided<4, 4>, 8192, 16, true},
    {"braid5x4", braided<5, 4>, 8192, 20, true},
    {"braid6x4", braided<6, 4>, 8192, 24, true},
    {"braid2x8", braided<2, 8>, 16384, 8, true},
    {"braid3x8", braided<3, 8>, 16384, 12, true},
    {"braid4x8", braided<4, 8>, 16384, 16, true},
    {"braid5x8", braided<5, 8>, 16384, 20, true},
    {"braid6x8", braided<6, 8>, 16384, 24, true},
    {"nibble4x4", braided<4, 4, 4>, 4608, 16, true},
    {"matrix4x4", matrix_four<false>, 4224, 48, true},
    {"matrix-lanes", matrix_four<true>, 4224, 48, true},
}};

// Independent normal-polynomial, MSB-register oracle. Input bits still arrive
// least-significant first, as PNG specifies. It shares no production tables.
std::uint32_t reference(std::span<const std::byte> bytes, std::uint32_t seed) {
    std::uint32_t state = reverse_bits(~seed);
    for (const std::byte byte : bytes) {
        const unsigned value = std::to_integer<unsigned>(byte);
        for (unsigned bit = 0; bit < 8; ++bit) {
            const unsigned feedback = (state >> 31U) ^ ((value >> bit) & 1U);
            state <<= 1U;
            if (feedback != 0U) state ^= 0x04c11db7U;
        }
    }
    const std::uint32_t result = ~reverse_bits(state);
    return result;
}

std::uint32_t apply(const Matrix& matrix, std::uint32_t value) {
    const std::uint32_t result = matrix_apply(matrix, value);
    return result;
}

std::uint32_t shift(std::uint32_t value, std::uint64_t bytes) {
    std::size_t power = 0;
    while (bytes != 0U) {
        if ((bytes & 1U) != 0U) value = matrix_apply(zero_powers[power], value);
        bytes >>= 1U;
        ++power;
    }
    return value;
}

Matrix prepare_shift(std::uint64_t bytes) {
    Matrix result{};
    for (unsigned bit = 0; bit < 32; ++bit) {
        result[bit] = shift(std::uint32_t{1} << bit, bytes);
    }
    return result;
}

std::uint32_t combine(std::uint32_t left, std::uint32_t right, std::uint64_t right_size) {
    const std::uint32_t result = shift(left, right_size) ^ right;
    return result;
}

} // namespace crc_study
