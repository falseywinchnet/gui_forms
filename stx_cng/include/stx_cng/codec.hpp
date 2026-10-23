#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>

// CNG (Computational Network Graphics) standalone images: lossless RGB8 and
// RGBA8 in the CSP v2 byte format (docs/FORMAT.md). Every entry point takes
// byte spans, reports failure as an explicit result, never throws, and retains
// no borrow after it returns. One execution context per encoder or decoder.
namespace stx_cng {

// Admitted image extent (format revision of 2026-10-22, owner decision D2).
inline constexpr std::uint32_t maximum_side = 16384;
inline constexpr std::uint64_t maximum_pixels = std::uint64_t{1} << 26;

enum class CngChannels : std::uint8_t { rgb = 3, rgba = 4 };

enum class CngError : std::uint8_t {
    none,
    // A caller argument broke the call's contract (shape, row stride, sizes).
    invalid_argument,
    // Destination storage has the wrong size or is too small.
    insufficient_storage,
    // Input and output storage overlap.
    overlap,
    // Signature or header: not a CSP v2 image, or a field outside the format.
    invalid_signature,
    invalid_header,
    unsupported_dimensions,
    // Chunk framing: truncated input, wrong chunk type or length, checksum.
    truncated,
    invalid_chunk,
    crc_mismatch,
    // Band contents: counts, entropy frames, sequences or literals.
    invalid_band,
    invalid_sequences,
    invalid_literals,
    entropy_failure,
    // Bytes after the end chunk.
    trailing_data,
    // Workspace growth failed; the codec keeps its previous storage.
    allocation_failed
};

struct CngImage final {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    CngChannels channels = CngChannels::rgba;
};

// Sample bytes of a tightly packed image: width * height * channels. Zero for
// an image outside the admitted extent.
[[nodiscard]] std::size_t cng_sample_bytes(CngImage image) noexcept;

// Largest possible encoded size for an admitted image; zero otherwise. An
// encoder destination of this size can never be too small.
[[nodiscard]] std::size_t cng_encoded_bound(CngImage image) noexcept;

struct CngInspectResult final {
    CngError error = CngError::invalid_argument;
    CngImage image{};
};

// Header probe: checks the signature and the complete header chunk, including
// its checksum and every field. Band contents are not examined.
[[nodiscard]] CngInspectResult inspect_cng(std::span<const std::byte> encoded) noexcept;

struct CngDecodeResult final {
    CngError error = CngError::invalid_argument;
    CngImage image{};
    // Owned storage after the call, including the entropy context.
    std::size_t workspace_bytes = 0;
};

class CngDecoderStorage;

// Reusable decoder. Its storage grows to the largest band it has decoded and
// is then reused: a decode that fits allocates nothing. Moving is not
// supported; hold it by owner (for example std::unique_ptr).
class CngDecoder final {
public:
    CngDecoder() noexcept;
    ~CngDecoder();
    CngDecoder(const CngDecoder&) = delete;
    CngDecoder& operator=(const CngDecoder&) = delete;
    CngDecoder(CngDecoder&&) = delete;
    CngDecoder& operator=(CngDecoder&&) = delete;

    // Decodes one complete file into exactly cng_sample_bytes(image) bytes of
    // tightly packed samples, top row first. Encoded bytes and samples must be
    // disjoint. Signature, header and destination failures leave the samples
    // untouched; a later failure leaves unspecified samples, to be discarded.
    [[nodiscard]] CngDecodeResult decode(std::span<const std::byte> encoded,
                                         std::span<std::byte> samples) noexcept;
    // Frees all storage; the next decode grows it again.
    void release() noexcept;
    [[nodiscard]] std::size_t workspace_bytes() const noexcept;

private:
    std::unique_ptr<CngDecoderStorage> storage_;
};

struct CngEncodeInput final {
    CngImage image{};
    // Bytes between successive rows; at least width * channels. The final
    // row needs only width * channels bytes.
    std::uint64_t row_bytes = 0;
    std::span<const std::byte> samples{};
};

struct CngEncodeResult final {
    CngError error = CngError::invalid_argument;
    // Bytes written to the destination's front; zero on failure.
    std::size_t encoded_bytes = 0;
    std::size_t workspace_bytes = 0;
    // Bands whose literals were stored as samples or entropy coded.
    std::size_t stored_bands = 0;
    std::size_t compressed_bands = 0;
};

class CngEncoderStorage;

// Reusable encoder with the same storage rule as CngDecoder. Output is a
// deterministic function of the samples: the same bytes on every host.
class CngEncoder final {
public:
    CngEncoder() noexcept;
    ~CngEncoder();
    CngEncoder(const CngEncoder&) = delete;
    CngEncoder& operator=(const CngEncoder&) = delete;
    CngEncoder(CngEncoder&&) = delete;
    CngEncoder& operator=(CngEncoder&&) = delete;

    // Writes one complete file to the destination's front. Samples and
    // destination must be disjoint. On failure the destination's contents are
    // unspecified and encoded_bytes is zero.
    [[nodiscard]] CngEncodeResult encode(const CngEncodeInput& input,
                                         std::span<std::byte> destination) noexcept;
    void release() noexcept;
    [[nodiscard]] std::size_t workspace_bytes() const noexcept;

private:
    std::unique_ptr<CngEncoderStorage> storage_;
};

} // namespace stx_cng
