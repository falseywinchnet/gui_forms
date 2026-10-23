#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

// Private vocabulary shared by the codec's translation units. Every function
// here is called only with storage the codec owns or has already validated;
// each states its preconditions and none allocates, throws or keeps a borrow.
namespace stx_cng::detail {

inline constexpr std::array<std::uint8_t, 8> signature{0x89, 0x43, 0x53, 0x50, 0x0D, 0x0A, 0x1A, 0x0A};
using ChunkType = std::array<std::uint8_t, 4>;
inline constexpr ChunkType header_type{0x43, 0x48, 0x44, 0x52}; // CHDR
inline constexpr ChunkType band_type{0x42, 0x41, 0x4E, 0x44};   // BAND
inline constexpr ChunkType end_type{0x43, 0x45, 0x4E, 0x44};    // CEND

inline constexpr std::uint8_t format_version = 2;
// Chunk framing: length, type, payload, CRC32.
inline constexpr std::size_t chunk_framing_bytes = 12;
inline constexpr std::size_t header_payload_bytes = 16;
// BAND fields before the sequence frame: count N, literal count L, frame
// length S; and the literal-coding byte after the frame.
inline constexpr std::size_t band_fields_bytes = 12;
inline constexpr std::size_t band_coding_bytes = 1;
// Fixed band budget in sample bytes, recorded in every header.
inline constexpr std::size_t band_budget = 1048576;
// Nonterminal runs cover at least this many pixels.
inline constexpr std::size_t minimum_run = 4;
inline constexpr std::size_t maximum_distance = 262144;
// Encoder match index: one slot per 16-bit hash.
inline constexpr std::size_t index_slots = 65536;
inline constexpr std::size_t sequence_record_bytes = 12;

enum class LiteralCoding : std::uint8_t { stored = 0, compressed = 1 };

// One instruction: emit `literals` pixels, then a run of `length` pixels that
// copies from `distance` pixels back, or (distance zero) follows the gradient
// prediction exactly. The final record has length and distance zero.
struct Sequence final {
    std::uint32_t literals = 0;
    std::uint32_t length = 0;
    std::uint32_t distance = 0;
};

// The band arrangement an image's header determines.
struct BandShape final {
    std::size_t channels = 0;
    std::size_t row_bytes = 0;
    // Full rows per band, and the rows and sample bytes of the largest band.
    std::size_t rows = 0;
    std::size_t band_rows = 0;
    std::size_t band_bytes = 0;
    // Pixels and sequence records of the largest band.
    std::size_t band_pixels = 0;
    std::size_t sequence_capacity = 0;
    // Packed literal bytes, and the run-separated form, for a full band.
    std::size_t packed_capacity = 0;
    std::size_t separated_capacity = 0;
};

// Packed nibble-plane bytes for `literals` pixels: 2 * channels * ceil(L/2).
[[nodiscard]] std::size_t packed_bytes(std::size_t literals, std::size_t channels) noexcept;
// Validated dimensions only (sides and pixel count within the format).
[[nodiscard]] BandShape band_shape(std::uint32_t width, std::uint32_t height, std::size_t channels) noexcept;

// Encoder search (sequences.cpp). Source is one band of `channels`-byte
// pixels whose length is a multiple of previous.size(); previous is the row
// above the band (zeros at the image top). Index has index_slots entries;
// output has at least band pixels / 4 + 1 records. Returns the live record
// count and writes the band's literal pixel count.
[[nodiscard]] std::size_t find_sequences(std::span<const std::uint8_t> source,
                                         std::span<const std::uint8_t> previous,
                                         std::size_t channels,
                                         std::span<std::int32_t> index,
                                         std::span<Sequence> output,
                                         std::size_t& literal_pixels) noexcept;
// Byte-planar sequence frame of 12 * sequences.size() bytes (FORMAT.md).
void pack_sequences(std::span<const Sequence> sequences, std::span<std::uint8_t> raw) noexcept;
void unpack_sequences(std::span<const std::uint8_t> raw, std::span<Sequence> sequences) noexcept;
// The sequence grammar: every count agrees with the band. True when valid.
[[nodiscard]] bool valid_sequences(std::span<const Sequence> sequences,
                                   std::size_t band_pixels,
                                   std::size_t literal_pixels) noexcept;

// Literal coding (literals.cpp).
// Encoder: copies each literal's original samples to `samples` and its
// transformed residual nibbles to `packed` (packed_bytes() long), in
// consumption order.
void collect_literals(std::span<const std::uint8_t> source,
                      std::span<const std::uint8_t> previous,
                      std::size_t channels,
                      std::span<const Sequence> sequences,
                      std::span<std::uint8_t> samples,
                      std::span<std::uint8_t> packed) noexcept;
// Reverse-tail run separation. Separated holds at least 2 * packed + 4 bytes;
// returns the separated length.
[[nodiscard]] std::size_t separate_runs(std::span<const std::uint8_t> packed,
                                        std::span<std::uint8_t> separated) noexcept;
// Inverse of separate_runs into exactly packed.size() bytes. False when the
// descriptors are malformed or do not consume the input exactly.
[[nodiscard]] bool restore_runs(std::span<const std::uint8_t> separated,
                                std::span<std::uint8_t> packed) noexcept;
// Nibble planes to modular residual samples. False when an odd literal
// count leaves nonzero padding nibbles.
[[nodiscard]] bool unpack_literals(std::span<const std::uint8_t> packed,
                                   std::size_t channels,
                                   std::span<std::uint8_t> residuals) noexcept;

// Reconstruction (restore.cpp), after valid_sequences. Output is one band;
// previous is the row above it. Literals are residuals (compressed coding)
// or original samples (stored coding). Output is disjoint from every input.
void restore_band(std::span<const Sequence> sequences,
                  std::span<const std::uint8_t> literals,
                  LiteralCoding coding,
                  std::span<const std::uint8_t> previous,
                  std::size_t channels,
                  std::span<std::uint8_t> output) noexcept;

} // namespace stx_cng::detail
