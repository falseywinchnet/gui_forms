#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>

// CNG (Computational Network Graphics) 1.0 images: lossless BGR8 and BGRA8
// (docs/FORMAT.md). Samples are interleaved blue, green, red, then alpha,
// top row first, the order CNG stores, so a decode into a BGRA8 destination
// is not followed by any conversion. The codec keeps every sample, including
// the colour beneath alpha 0; it premultiplies nothing and converts no colour.
//
// Every entry point takes byte spans, reports failure as an explicit result,
// never throws, and retains no borrow after it returns. One execution context
// per encoder or decoder; distinct owners may run concurrently. A call given
// more than one thread starts that many workers less one, joins them all
// before it returns, and runs any worker that cannot be started on the
// calling thread instead; the result does not depend on the thread count.
namespace stx_cng {

inline constexpr std::uint8_t format_version = 1;
// Segments are 128 x 32 pixels, the last column and row smaller; the mosaic
// holds the mean colour of every 16 x 16 cell.
inline constexpr std::uint32_t segment_width = 128;
inline constexpr std::uint32_t segment_height = 32;
inline constexpr std::uint32_t mosaic_cell = 16;
// The format's extent: width * height * channels samples.
inline constexpr std::uint64_t maximum_samples = std::uint64_t{1} << 31;
// The largest thread count a call accepts.
inline constexpr std::uint32_t maximum_threads = 64;

enum class CngChannels : std::uint8_t { bgr = 3, bgra = 4 };

enum class CngError : std::uint8_t {
    none,
    // A caller argument broke the call's contract: shape, row stride, thread
    // count, collection form or member list.
    invalid_argument,
    // Destination storage is too small or has the wrong size.
    insufficient_storage,
    // Input and output storage overlap.
    overlap,
    // Not a CNG image (or collection) signature.
    invalid_signature,
    // No intact copy of the header fields, or fields outside the format.
    invalid_header,
    // A format version or segment size this library does not read.
    unsupported_version,
    // Strict framing (validate_cng_framing): the file is shorter or longer
    // than its header and directory imply, a checksum fails, or a second copy
    // differs from the first.
    truncated,
    trailing_data,
    crc_mismatch,
    copy_mismatch,
    // No intact copy of the directory, or payload lengths that do not
    // account for the payload region.
    invalid_directory,
    // No intact copy of the tables, a Zstandard frame other than exactly one,
    // or a tables body outside the format.
    invalid_tables,
    // A segment (or the mosaic) failed its checksum or its decoding.
    invalid_segment,
    // Both mosaic copies are damaged (decode_mosaic only).
    mosaic_unavailable,
    // A collection's structure: chunks, header, directory or member extents.
    invalid_collection,
    // Workspace growth failed; the owner keeps its previous storage.
    allocation_failed
};

struct CngImage final {
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    CngChannels channels = CngChannels::bgra;
};

// width * height * channels; zero for an image outside the format (a side of
// zero, channels other than 3 or 4, or more than 2^31 samples).
[[nodiscard]] std::uint64_t cng_sample_bytes(CngImage image) noexcept;

// ceil(width / 16) x ceil(height / 16), same channels; zero sides outside
// the format.
[[nodiscard]] CngImage cng_mosaic_image(CngImage image) noexcept;

// ceil(width / 128) * ceil(height / 32); zero outside the format.
[[nodiscard]] std::uint64_t cng_segment_count(CngImage image) noexcept;

// The largest file the encoder can write for the image; zero outside the
// format or beyond size_t. A destination of this size is never too small.
[[nodiscard]] std::uint64_t cng_encoded_bound(CngImage image) noexcept;

struct CngInspectResult final {
    CngError error = CngError::invalid_argument;
    CngImage image{};
    std::uint64_t segments = 0;
    // A mosaic copy's checksum matches.
    bool mosaic_available = false;
};

// The decoder's reading of the container (FORMAT.md section 3.4): the
// signature, an intact copy of the header fields and of the directory, the
// payload region's accounting and an intact tables copy. Decodes nothing and
// does not check segment checksums.
[[nodiscard]] CngInspectResult inspect_cng(std::span<const std::byte> encoded) noexcept;

// The same reading for salvage: the signature is not required.
[[nodiscard]] CngInspectResult inspect_cng_for_salvage(std::span<const std::byte> encoded) noexcept;

// Strict framing, for admitting a file before it is decoded: the signature,
// the front header and directory, the exact file length they imply
// (truncated, trailing_data), every checksum (tables, mosaic, every segment
// payload), every second copy byte for byte (copy_mismatch), and a tables
// part of exactly one Zstandard frame. Decodes nothing, so a file that passes
// can still be refused by decode.
[[nodiscard]] CngInspectResult validate_cng_framing(std::span<const std::byte> encoded) noexcept;

struct CngDecodeResult final {
    CngError error = CngError::invalid_argument;
    CngImage image{};
    // Owned storage after the call, the Zstandard context included.
    std::size_t workspace_bytes = 0;
};

struct CngSalvageResult final {
    CngError error = CngError::invalid_argument;
    CngImage image{};
    std::uint64_t segments = 0;
    std::uint64_t segments_lost = 0;
    // A mosaic copy decoded, and lost segments were filled from it; otherwise
    // they are filled with 128 in every channel.
    bool mosaic_used = false;
    std::size_t workspace_bytes = 0;
};

class CngDecoderStorage;

// Reusable decoder. Its storage grows to the largest image it has decoded and
// is then reused: a serial decode that fits allocates nothing. Moving is not
// supported; hold it by owner (for example std::unique_ptr).
//
// Destinations are `row_bytes` apart, at least width * channels, and need only
// width * channels bytes in the last row; bytes between rows are not written.
// Encoded bytes and the destination must be disjoint. Argument, signature,
// header, directory, tables and size refusals leave the destination
// untouched; a refused segment leaves it partly written, to be discarded.
class CngDecoder final {
public:
    CngDecoder() noexcept;
    ~CngDecoder();
    CngDecoder(const CngDecoder&) = delete;
    CngDecoder& operator=(const CngDecoder&) = delete;
    CngDecoder(CngDecoder&&) = delete;
    CngDecoder& operator=(CngDecoder&&) = delete;

    // Decodes every segment; any damaged or invalid segment fails the call.
    [[nodiscard]] CngDecodeResult decode(std::span<const std::byte> encoded, std::span<std::byte> samples,
                                         std::uint64_t row_bytes, std::uint32_t threads) noexcept;
    // Decodes the mosaic alone (cng_mosaic_image of the image), from either
    // intact copy. The result's image is the mosaic's.
    [[nodiscard]] CngDecodeResult decode_mosaic(std::span<const std::byte> encoded, std::span<std::byte> samples,
                                                std::uint64_t row_bytes) noexcept;
    // Decodes a possibly damaged file as far as it goes (FORMAT.md section
    // 10): every segment whose checksum passes and that decodes, the others
    // filled from the mosaic. Needs inspect_cng_for_salvage to succeed.
    // segment_intact receives one byte per segment in raster order, 1 for a
    // segment decoded exactly; it must hold at least the segment count.
    [[nodiscard]] CngSalvageResult decode_salvage(std::span<const std::byte> encoded, std::span<std::byte> samples,
                                                  std::uint64_t row_bytes, std::span<std::uint8_t> segment_intact) noexcept;
    // Frees all storage; the next decode grows it again.
    void release() noexcept;
    [[nodiscard]] std::size_t workspace_bytes() const noexcept;

private:
    std::unique_ptr<CngDecoderStorage> storage_;
};

struct CngEncodeInput final {
    CngImage image{};
    // Bytes between successive rows; at least width * channels. The last row
    // needs only width * channels bytes.
    std::uint64_t row_bytes = 0;
    std::span<const std::byte> samples{};
    std::uint32_t threads = 1;
};

struct CngEncodeResult final {
    CngError error = CngError::invalid_argument;
    // Bytes written to the destination's front; zero on failure.
    std::size_t encoded_bytes = 0;
    // The file's size, also reported when the destination is too small.
    std::size_t required_bytes = 0;
    std::size_t workspace_bytes = 0;
};

class CngEncoderStorage;

// Reusable encoder: the reference encoder of FORMAT.md section 11. Output is
// a deterministic function of the samples, the same bytes for every thread
// count and on every host. Storage grows to the largest image encoded and is
// reused; between passes it holds the coded symbols of every segment, at
// most twelve bytes per sample (three four-byte codes).
class CngEncoder final {
public:
    CngEncoder() noexcept;
    ~CngEncoder();
    CngEncoder(const CngEncoder&) = delete;
    CngEncoder& operator=(const CngEncoder&) = delete;
    CngEncoder(CngEncoder&&) = delete;
    CngEncoder& operator=(CngEncoder&&) = delete;

    // Writes one complete file to the destination's front. Samples and
    // destination must be disjoint. On failure the destination's contents
    // are unspecified and encoded_bytes is zero.
    [[nodiscard]] CngEncodeResult encode(const CngEncodeInput& input, std::span<std::byte> destination) noexcept;
    void release() noexcept;
    [[nodiscard]] std::size_t workspace_bytes() const noexcept;

private:
    std::unique_ptr<CngEncoderStorage> storage_;
};

} // namespace stx_cng
