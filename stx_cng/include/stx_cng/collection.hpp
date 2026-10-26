#pragma once

#include "stx_cng/codec.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>

// CNG collections (docs/COLLECTIONS.md): 1 to 4,096 images in one file, in
// one of two forms the caller chooses.
//   packed     every member is a complete CNG file in its own chunk, decoded
//              on its own;
//   composite  one CNG file holds a canvas with the members stacked
//              vertically at x = 0, decoded in full, then sliced.
// Members keep their dimensions and channel counts; a composite canvas is BGR
// when every member is BGR, otherwise BGRA (BGR members then take alpha 255).
//
// Members are byte spans with a row stride, and the encoder writes into a
// caller-supplied output budget (owner decision D3): no file, stream or seek
// is involved, and nothing is written outside the budget. The rules of
// stx_cng/codec.hpp apply: explicit results, no exceptions, no retained
// borrow, one execution context per owner.
namespace stx_cng {

inline constexpr std::uint32_t maximum_collection_members = 4096;
// Every member, and a composite canvas, is at most 65,536 pixels on a side;
// rounded up to multiples of 128, at most 4,194,304 pixels and a width plus
// height of at most 65,536.
inline constexpr std::uint32_t maximum_collection_side = 65536;
inline constexpr std::uint64_t maximum_collection_padded_pixels = 4194304;

enum class CngCollectionForm : std::uint8_t { packed = 0, composite = 1 };

struct CngMemberId final {
    std::uint32_t value = 0;
};

// One member to encode: its identity and its samples, rows `row_bytes` apart.
struct CngCollectionMember final {
    CngMemberId id{};
    CngImage image{};
    std::uint64_t row_bytes = 0;
    std::span<const std::byte> samples{};
};

// One member as stored: its rectangle in the canvas (composite; packed
// members are at 0, 0) and the extent of the chunk that holds its file,
// framing included.
struct CngCollectionEntry final {
    CngMemberId id{};
    CngImage image{};
    std::uint32_t x = 0;
    std::uint32_t y = 0;
    std::uint64_t chunk_offset = 0;
    std::uint64_t chunk_bytes = 0;
};

struct CngCollectionInfo final {
    CngError error = CngError::invalid_argument;
    CngCollectionForm form = CngCollectionForm::packed;
    std::uint32_t members = 0;
    // Composite only; zero sides for packed.
    CngImage canvas{};
};

// Reads and checks a collection's whole structure without decoding: the
// signature, every chunk's framing and CRC32, the header, the directory and
// its extents (COLLECTIONS.md, "File layout"), and each member file's CNG
// container, whose dimensions must match the directory. Fills
// entries[0, members); when `entries` is shorter, the result is
// insufficient_storage with `members` set. invalid_signature for a file that
// is not a collection; invalid_collection for its structure; a member file's
// own refusal otherwise.
[[nodiscard]] CngCollectionInfo inspect_cng_collection(std::span<const std::byte> encoded,
                                                       std::span<CngCollectionEntry> entries) noexcept;

// The largest collection the encoder can write for these members; zero for
// a member list the encoder refuses. A budget of this size is never too small.
[[nodiscard]] std::uint64_t cng_collection_bound(std::span<const CngCollectionMember> members, CngCollectionForm form) noexcept;

// Copies member `entry` out of a decoded composite canvas (rows
// `canvas_row_bytes` apart) into `samples` (rows `row_bytes` apart), with the
// member's own channel count and every sample, the colour beneath alpha 0
// included. invalid_argument when the entry lies outside the canvas or has
// more channels; insufficient_storage or overlap as for a decode.
[[nodiscard]] CngError copy_cng_collection_member(std::span<const std::byte> canvas, CngImage canvas_image,
                                                  std::uint64_t canvas_row_bytes, const CngCollectionEntry& entry,
                                                  std::span<std::byte> samples, std::uint64_t row_bytes) noexcept;

struct CngCollectionEncodeResult final {
    CngError error = CngError::invalid_argument;
    // Bytes written to the budget's front; zero on failure.
    std::size_t encoded_bytes = 0;
    std::size_t workspace_bytes = 0;
};

class CngCollectionEncoderStorage;

// Reusable collection encoder: one CngEncoder and, for composites, the
// canvas it assembles (at most 4,194,304 pixels of four bytes).
class CngCollectionEncoder final {
public:
    CngCollectionEncoder() noexcept;
    ~CngCollectionEncoder();
    CngCollectionEncoder(const CngCollectionEncoder&) = delete;
    CngCollectionEncoder& operator=(const CngCollectionEncoder&) = delete;
    CngCollectionEncoder(CngCollectionEncoder&&) = delete;
    CngCollectionEncoder& operator=(CngCollectionEncoder&&) = delete;

    // Encodes the members, in order, into `budget`. invalid_argument for a
    // member count outside 1 to 4,096, a repeated id, a member outside the
    // bounds or with short samples, or a composite canvas outside the bounds
    // (it is refused, not switched to packed); insufficient_storage when the
    // collection does not fit the budget. Members and budget must be
    // disjoint. On failure the budget's contents are unspecified.
    [[nodiscard]] CngCollectionEncodeResult encode(std::span<const CngCollectionMember> members, CngCollectionForm form,
                                                   std::uint32_t threads, std::span<std::byte> budget) noexcept;
    void release() noexcept;
    [[nodiscard]] std::size_t workspace_bytes() const noexcept;

private:
    std::unique_ptr<CngCollectionEncoderStorage> storage_;
};

class CngCollectionDecoderStorage;

// Reusable collection decoder: one CngDecoder, the directory of the last
// collection read, and for composites the decoded canvas.
class CngCollectionDecoder final {
public:
    CngCollectionDecoder() noexcept;
    ~CngCollectionDecoder();
    CngCollectionDecoder(const CngCollectionDecoder&) = delete;
    CngCollectionDecoder& operator=(const CngCollectionDecoder&) = delete;
    CngCollectionDecoder(CngCollectionDecoder&&) = delete;
    CngCollectionDecoder& operator=(CngCollectionDecoder&&) = delete;

    // Inspects the collection, then decodes member `id` into `samples` with
    // its own channel count; the result's image is the member's.
    // invalid_argument when no member has that id. A composite's whole canvas
    // is decoded for every call: to extract many members, decode the canvas
    // once (decode_canvas) and use copy_cng_collection_member.
    [[nodiscard]] CngDecodeResult decode_member(std::span<const std::byte> encoded, CngMemberId id, std::span<std::byte> samples,
                                                std::uint64_t row_bytes, std::uint32_t threads) noexcept;
    // Inspects a composite collection and decodes its canvas into `samples`;
    // invalid_argument for a packed collection.
    [[nodiscard]] CngDecodeResult decode_canvas(std::span<const std::byte> encoded, std::span<std::byte> samples,
                                                std::uint64_t row_bytes, std::uint32_t threads) noexcept;
    void release() noexcept;
    [[nodiscard]] std::size_t workspace_bytes() const noexcept;

private:
    std::unique_ptr<CngCollectionDecoderStorage> storage_;
};

} // namespace stx_cng
