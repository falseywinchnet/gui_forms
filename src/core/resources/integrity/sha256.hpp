#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace gui_forms::detail {

// SHA-256 (FIPS 180-4) over bytes held in memory, for checking a bundled resource against the
// digest its install manifest records before any parser reads it.

inline constexpr std::size_t sha256_digest_bytes = 32U;
inline constexpr std::size_t sha256_block_bytes = 64U;

// The digest in the standard byte order (the first byte is the high byte of the first word).
struct Sha256Digest final {
    std::array<std::uint8_t, sha256_digest_bytes> bytes{};

    friend bool operator==(const Sha256Digest& left, const Sha256Digest& right) noexcept {
        const bool equal = left.bytes == right.bytes;
        return equal;
    }
    friend bool operator!=(const Sha256Digest& left, const Sha256Digest& right) noexcept {
        const bool different = !(left == right);
        return different;
    }
};

// The digest of the whole of `input`. Reads `input` once and allocates nothing; any length,
// including zero, is accepted.
[[nodiscard]] Sha256Digest sha256(std::span<const std::uint8_t> input) noexcept;

// Parses 64 lowercase or uppercase hexadecimal digits into `digest`. Returns false, leaving
// `digest` unchanged, for any other length or character.
[[nodiscard]] bool parse_sha256_hex(std::span<const char> text, Sha256Digest& digest) noexcept;

} // namespace gui_forms::detail
