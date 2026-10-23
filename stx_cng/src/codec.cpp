#include "stx_cng/codec.hpp"

#include "format.hpp"
#include "stx_zlib/checksum.hpp"

#include <algorithm>
#include <cstring>
#include <limits>
#include <new>
#include <vector>

#define ZSTD_STATIC_LINKING_ONLY
#include <zstd.h>
#include <zstd_errors.h>

namespace stx_cng {
namespace {

using detail::BandShape;
using detail::ChunkType;
using detail::LiteralCoding;
using detail::Sequence;

// The one compression level of the format's reference encoder.
constexpr int compression_level = 3;
// Ceiling on the static compression context. Zstandard 1.5.7 reports about
// 1.3 MB for level 3; the ceiling guards a different build of the library.
constexpr std::size_t compression_context_limit = 4194304;

bool admitted(CngImage image) {
    bool channels = image.channels == CngChannels::rgb || image.channels == CngChannels::rgba;
    bool sides = image.width >= 1 && image.height >= 1
        && image.width <= maximum_side && image.height <= maximum_side;
    bool area = sides && static_cast<std::uint64_t>(image.width) * image.height <= maximum_pixels;
    bool result = channels && area;
    return result;
}

std::size_t channel_count(CngChannels channels) {
    std::size_t count = static_cast<std::size_t>(channels);
    return count;
}

// Byte views of storage the codec owns as std::uint8_t.
std::span<const std::uint8_t> bytes_of(std::span<const std::byte> bytes) {
    std::span<const std::uint8_t> result(reinterpret_cast<const std::uint8_t*>(bytes.data()), bytes.size());
    return result;
}

std::span<std::uint8_t> bytes_of(std::span<std::byte> bytes) {
    std::span<std::uint8_t> result(reinterpret_cast<std::uint8_t*>(bytes.data()), bytes.size());
    return result;
}

bool overlapping(const void* first, std::size_t first_size, const void* second, std::size_t second_size) {
    if (first_size == 0 || second_size == 0) {
        return false;
    }
    std::uintptr_t first_begin = reinterpret_cast<std::uintptr_t>(first);
    std::uintptr_t second_begin = reinterpret_cast<std::uintptr_t>(second);
    bool result = first_begin <= second_begin
        ? second_begin - first_begin < first_size
        : first_begin - second_begin < second_size;
    return result;
}

std::uint32_t read_u32(std::span<const std::uint8_t> bytes, std::size_t offset) {
    std::uint32_t value = 0;
    for (std::size_t index = 0; index < 4; ++index) {
        value = (value << 8) | bytes[offset + index];
    }
    return value;
}

void write_u32(std::span<std::uint8_t> bytes, std::size_t offset, std::uint32_t value) {
    for (std::size_t index = 0; index < 4; ++index) {
        bytes[offset + index] = static_cast<std::uint8_t>(value >> (24 - 8 * index));
    }
}

// CRC32 of a chunk's type and payload, which are contiguous in the file.
std::uint32_t chunk_crc(std::span<const std::uint8_t> type_and_payload) {
    std::span<const std::byte> bytes(reinterpret_cast<const std::byte*>(type_and_payload.data()), type_and_payload.size());
    std::uint32_t crc = stx_zlib::crc32(bytes);
    return crc;
}

// A located chunk: its payload's offset and length in the file.
struct Chunk final {
    CngError error = CngError::truncated;
    std::size_t payload = 0;
    std::size_t length = 0;
    std::size_t next = 0;
};

// Reads the chunk at `offset`: its length must fit the remaining input, its
// type must be `type`, and its CRC must match.
Chunk read_chunk(std::span<const std::uint8_t> file, std::size_t offset, const ChunkType& type) {
    Chunk chunk;
    if (file.size() - offset < detail::chunk_framing_bytes) {
        return chunk;
    }
    std::size_t length = read_u32(file, offset);
    if (length > file.size() - offset - detail::chunk_framing_bytes) {
        return chunk;
    }
    if (!std::equal(type.begin(), type.end(), file.begin() + static_cast<std::ptrdiff_t>(offset + 4))) {
        chunk.error = CngError::invalid_chunk;
        return chunk;
    }
    std::uint32_t stored = read_u32(file, offset + 8 + length);
    if (stored != chunk_crc(file.subspan(offset + 4, 4 + length))) {
        chunk.error = CngError::crc_mismatch;
        return chunk;
    }
    chunk.error = CngError::none;
    chunk.payload = offset + 8;
    chunk.length = length;
    chunk.next = offset + detail::chunk_framing_bytes + length;
    return chunk;
}

// Strict entropy frame: exactly one complete, non-skippable Zstandard frame
// spanning `frame`, declaring its decompressed size, which must be `content`
// (or, for an unknown size, at most `limit`). Returns the declared size, or
// the maximum value when the frame is refused.
constexpr std::size_t refused_frame = std::numeric_limits<std::size_t>::max();

std::size_t declared_frame(std::span<const std::uint8_t> frame, std::size_t limit) {
    if (frame.empty() || ZSTD_isSkippableFrame(frame.data(), frame.size()) != 0) {
        return refused_frame;
    }
    std::size_t compressed = ZSTD_findFrameCompressedSize(frame.data(), frame.size());
    if (ZSTD_isError(compressed) != 0 || compressed != frame.size()) {
        return refused_frame;
    }
    unsigned long long declared = ZSTD_getFrameContentSize(frame.data(), frame.size());
    if (declared == ZSTD_CONTENTSIZE_UNKNOWN || declared == ZSTD_CONTENTSIZE_ERROR || declared > limit) {
        return refused_frame;
    }
    std::size_t result = static_cast<std::size_t>(declared);
    return result;
}

// Grows `storage` to at least `count` elements, reserving exactly that many
// (resize alone may over-allocate geometrically); false when allocation
// fails, leaving it as it was. New elements are zero.
template<typename Element>
bool grow(std::vector<Element>& storage, std::size_t count) {
    if (storage.size() >= count) {
        return true;
    }
    try {
        storage.reserve(count);
        storage.resize(count);
    } catch (const std::bad_alloc&) {
        return false;
    } catch (const std::length_error&) {
        return false;
    }
    return true;
}

template<typename Element>
std::size_t capacity_bytes(const std::vector<Element>& storage) {
    std::size_t bytes = storage.capacity() * sizeof(Element);
    return bytes;
}

// Static Zstandard context: an aligned workspace this owner allocates once,
// which Zstandard borrows and never resizes or frees.
struct ContextStorage final {
    std::unique_ptr<std::uint64_t[]> words;
    std::size_t bytes = 0;
};

bool reserve_context(ContextStorage& storage, std::size_t required) {
    if (storage.bytes != 0) {
        return true;
    }
    std::size_t words = required / sizeof(std::uint64_t) + 1;
    try {
        storage.words = std::make_unique_for_overwrite<std::uint64_t[]>(words);
    } catch (const std::bad_alloc&) {
        return false;
    }
    storage.bytes = words * sizeof(std::uint64_t);
    return true;
}

} // namespace

namespace detail {

BandShape band_shape(std::uint32_t width, std::uint32_t height, std::size_t channels) noexcept {
    // Width <= 16384 and channels <= 4 bound a row by 64 KiB, so a band holds
    // at least 15 rows and every product below stays far inside 32 bits.
    BandShape shape;
    shape.channels = channels;
    shape.row_bytes = static_cast<std::size_t>(width) * channels;
    shape.rows = (band_budget - channels) / shape.row_bytes;
    shape.band_rows = std::min<std::size_t>(shape.rows, height);
    shape.band_bytes = shape.band_rows * shape.row_bytes;
    shape.band_pixels = shape.band_bytes / channels;
    shape.sequence_capacity = shape.band_pixels / minimum_run + 1;
    shape.packed_capacity = packed_bytes(shape.band_pixels, channels);
    shape.separated_capacity = 2 * shape.packed_capacity + 4;
    return shape;
}

} // namespace detail

std::size_t cng_sample_bytes(CngImage image) noexcept {
    if (!admitted(image)) {
        return 0;
    }
    std::size_t bytes = static_cast<std::size_t>(static_cast<std::uint64_t>(image.width) * image.height * channel_count(image.channels));
    return bytes;
}

std::size_t cng_encoded_bound(CngImage image) noexcept {
    if (!admitted(image)) {
        return 0;
    }
    BandShape shape = detail::band_shape(image.width, image.height, channel_count(image.channels));
    std::uint64_t full_bands = image.height / shape.rows;
    std::uint64_t last_rows = image.height % shape.rows;
    std::uint64_t band_overhead = detail::chunk_framing_bytes + detail::band_fields_bytes + detail::band_coding_bytes
        + ZSTD_compressBound(shape.sequence_capacity * detail::sequence_record_bytes);
    std::uint64_t bands = full_bands + (last_rows != 0 ? 1 : 0);
    // Stored literals bound each band by its own samples; one entropy frame of
    // the largest band's separated literals is the encoder's working margin.
    std::uint64_t bound = detail::signature.size() + detail::chunk_framing_bytes + detail::header_payload_bytes
        + detail::chunk_framing_bytes + bands * band_overhead
        + static_cast<std::uint64_t>(image.width) * image.height * shape.channels
        + ZSTD_compressBound(shape.separated_capacity);
    if (bound > std::numeric_limits<std::size_t>::max()) {
        return 0;
    }
    std::size_t result = static_cast<std::size_t>(bound);
    return result;
}

CngInspectResult inspect_cng(std::span<const std::byte> encoded) noexcept {
    CngInspectResult result;
    std::span<const std::uint8_t> file = bytes_of(encoded);
    if (file.size() < detail::signature.size()
        || !std::equal(detail::signature.begin(), detail::signature.end(), file.begin())) {
        result.error = CngError::invalid_signature;
        return result;
    }
    Chunk header = read_chunk(file, detail::signature.size(), detail::header_type);
    if (header.error != CngError::none) {
        result.error = header.error;
        return result;
    }
    if (header.length != detail::header_payload_bytes) {
        result.error = CngError::invalid_header;
        return result;
    }
    std::span<const std::uint8_t> fields = file.subspan(header.payload, header.length);
    std::uint8_t channels = fields[1];
    bool known = fields[0] == detail::format_version && (channels == 3 || channels == 4)
        && fields[2] == 0 && fields[3] == 0 && read_u32(fields, 12) == detail::band_budget;
    if (!known) {
        result.error = CngError::invalid_header;
        return result;
    }
    CngImage image{read_u32(fields, 4), read_u32(fields, 8), static_cast<CngChannels>(channels)};
    if (!admitted(image)) {
        result.error = CngError::unsupported_dimensions;
        return result;
    }
    result.error = CngError::none;
    result.image = image;
    return result;
}

// ---------------------------------------------------------------- decoding

class CngDecoderStorage final {
public:
    ContextStorage context_storage;
    ZSTD_DCtx* context = nullptr;
    std::vector<std::uint8_t> zero_row;
    std::vector<std::uint8_t> raw_sequences;
    std::vector<Sequence> sequences;
    std::vector<std::uint8_t> separated;
    std::vector<std::uint8_t> packed;
    std::vector<std::uint8_t> literals;

    [[nodiscard]] std::size_t bytes() const {
        std::size_t total = context_storage.bytes + capacity_bytes(zero_row) + capacity_bytes(raw_sequences)
            + capacity_bytes(sequences) + capacity_bytes(separated) + capacity_bytes(packed) + capacity_bytes(literals);
        return total;
    }
};

namespace {

bool reserve_decoder(CngDecoderStorage& storage, const BandShape& shape) {
    if (storage.context == nullptr) {
        if (!reserve_context(storage.context_storage, ZSTD_estimateDCtxSize())) {
            return false;
        }
        storage.context = ZSTD_initStaticDCtx(storage.context_storage.words.get(), storage.context_storage.bytes);
        if (storage.context == nullptr) {
            return false;
        }
    }
    bool grown = grow(storage.zero_row, shape.row_bytes)
        && grow(storage.raw_sequences, shape.sequence_capacity * detail::sequence_record_bytes)
        && grow(storage.sequences, shape.sequence_capacity)
        && grow(storage.separated, shape.separated_capacity)
        && grow(storage.packed, shape.packed_capacity)
        && grow(storage.literals, shape.band_bytes);
    return grown;
}

// Decodes the band chunk at `offset` into `output` (the band's samples);
// `previous` is the row above it. Validation of every count and frame
// precedes the first write to `output`.
CngError decode_band(CngDecoderStorage& storage, const BandShape& shape, std::span<const std::uint8_t> file,
                     const Chunk& chunk, std::span<const std::uint8_t> previous, std::span<std::uint8_t> output) {
    if (chunk.length < detail::band_fields_bytes + detail::band_coding_bytes) {
        return CngError::invalid_band;
    }
    std::span<const std::uint8_t> payload = file.subspan(chunk.payload, chunk.length);
    std::size_t pixels = output.size() / shape.channels;
    std::size_t count = read_u32(payload, 0);
    std::size_t literal_pixels = read_u32(payload, 4);
    std::size_t frame_bytes = read_u32(payload, 8);
    bool counts = count >= 1 && count <= pixels / detail::minimum_run + 1 && literal_pixels <= pixels
        && frame_bytes <= payload.size() - detail::band_fields_bytes - detail::band_coding_bytes;
    if (!counts) {
        return CngError::invalid_band;
    }
    // Sequences: one frame of exactly 12 * N bytes.
    std::size_t raw_bytes = count * detail::sequence_record_bytes;
    std::span<const std::uint8_t> frame = payload.subspan(detail::band_fields_bytes, frame_bytes);
    if (declared_frame(frame, raw_bytes) != raw_bytes) {
        return CngError::entropy_failure;
    }
    std::size_t produced = ZSTD_decompressDCtx(storage.context, storage.raw_sequences.data(), raw_bytes, frame.data(), frame.size());
    if (ZSTD_isError(produced) != 0 || produced != raw_bytes) {
        return CngError::entropy_failure;
    }
    std::span<Sequence> sequences(storage.sequences.data(), count);
    detail::unpack_sequences(std::span<const std::uint8_t>(storage.raw_sequences.data(), raw_bytes), sequences);
    if (!detail::valid_sequences(sequences, pixels, literal_pixels)) {
        return CngError::invalid_sequences;
    }
    // Literals.
    std::size_t coding_offset = detail::band_fields_bytes + frame_bytes;
    std::uint8_t coding = payload[coding_offset];
    std::span<const std::uint8_t> literal_payload = payload.subspan(coding_offset + detail::band_coding_bytes);
    std::size_t literal_bytes = literal_pixels * shape.channels;
    std::span<const std::uint8_t> literals;
    LiteralCoding literal_coding = LiteralCoding::stored;
    if (coding == static_cast<std::uint8_t>(LiteralCoding::stored)) {
        if (literal_payload.size() != literal_bytes) {
            return CngError::invalid_literals;
        }
        literals = literal_payload;
    } else if (coding == static_cast<std::uint8_t>(LiteralCoding::compressed)) {
        // The reference encoder compresses only nonempty literals that end up
        // smaller than stored samples; anything else has one stored form.
        if (literal_pixels == 0 || literal_payload.size() >= literal_bytes) {
            return CngError::invalid_literals;
        }
        std::size_t packed_size = detail::packed_bytes(literal_pixels, shape.channels);
        std::size_t separated_size = declared_frame(literal_payload, 2 * packed_size + 4);
        if (separated_size == refused_frame) {
            return CngError::entropy_failure;
        }
        std::size_t restored = ZSTD_decompressDCtx(storage.context, storage.separated.data(), separated_size,
                                                   literal_payload.data(), literal_payload.size());
        if (ZSTD_isError(restored) != 0 || restored != separated_size) {
            return CngError::entropy_failure;
        }
        std::span<std::uint8_t> packed(storage.packed.data(), packed_size);
        std::span<std::uint8_t> residuals(storage.literals.data(), literal_bytes);
        if (!detail::restore_runs(std::span<const std::uint8_t>(storage.separated.data(), separated_size), packed)
            || !detail::unpack_literals(packed, shape.channels, residuals)) {
            return CngError::invalid_literals;
        }
        literals = residuals;
        literal_coding = LiteralCoding::compressed;
    } else {
        return CngError::invalid_literals;
    }
    detail::restore_band(sequences, literals, literal_coding, previous, shape.channels, output);
    return CngError::none;
}

} // namespace

CngDecoder::CngDecoder() noexcept {}

CngDecoder::~CngDecoder() {}

void CngDecoder::release() noexcept {
    storage_.reset();
}

std::size_t CngDecoder::workspace_bytes() const noexcept {
    std::size_t bytes = storage_ ? (*storage_).bytes() : 0;
    return bytes;
}

CngDecodeResult CngDecoder::decode(std::span<const std::byte> encoded, std::span<std::byte> samples) noexcept {
    CngDecodeResult result;
    CngInspectResult inspected = inspect_cng(encoded);
    result.workspace_bytes = workspace_bytes();
    if (inspected.error != CngError::none) {
        result.error = inspected.error;
        return result;
    }
    result.image = inspected.image;
    if (samples.size() != cng_sample_bytes(inspected.image)) {
        result.error = CngError::insufficient_storage;
        return result;
    }
    if (overlapping(encoded.data(), encoded.size(), samples.data(), samples.size())) {
        result.error = CngError::overlap;
        return result;
    }
    BandShape shape = detail::band_shape(inspected.image.width, inspected.image.height, channel_count(inspected.image.channels));
    if (!storage_) {
        try {
            storage_ = std::make_unique<CngDecoderStorage>();
        } catch (const std::bad_alloc&) {
            result.error = CngError::allocation_failed;
            return result;
        }
    }
    CngDecoderStorage& storage = *storage_;
    if (!reserve_decoder(storage, shape)) {
        result.error = CngError::allocation_failed;
        result.workspace_bytes = workspace_bytes();
        return result;
    }
    result.workspace_bytes = workspace_bytes();
    std::span<const std::uint8_t> file = bytes_of(encoded);
    std::span<std::uint8_t> output = bytes_of(samples);
    std::size_t offset = detail::signature.size() + detail::chunk_framing_bytes + detail::header_payload_bytes;
    std::span<const std::uint8_t> zero_row(storage.zero_row.data(), shape.row_bytes);
    for (std::size_t row = 0; row < inspected.image.height; row += shape.rows) {
        std::size_t rows = std::min<std::size_t>(shape.rows, inspected.image.height - row);
        Chunk chunk = read_chunk(file, offset, detail::band_type);
        if (chunk.error != CngError::none) {
            result.error = chunk.error;
            return result;
        }
        std::span<std::uint8_t> band = output.subspan(row * shape.row_bytes, rows * shape.row_bytes);
        std::span<const std::uint8_t> previous = row == 0
            ? zero_row : std::span<const std::uint8_t>(output.data() + (row - 1) * shape.row_bytes, shape.row_bytes);
        CngError band_error = decode_band(storage, shape, file, chunk, previous, band);
        if (band_error != CngError::none) {
            result.error = band_error;
            return result;
        }
        offset = chunk.next;
    }
    Chunk end = read_chunk(file, offset, detail::end_type);
    if (end.error != CngError::none) {
        result.error = end.error;
        return result;
    }
    if (end.length != 0) {
        result.error = CngError::invalid_chunk;
        return result;
    }
    if (end.next != file.size()) {
        result.error = CngError::trailing_data;
        return result;
    }
    result.error = CngError::none;
    return result;
}

// ---------------------------------------------------------------- encoding

class CngEncoderStorage final {
public:
    ContextStorage context_storage;
    ZSTD_CCtx* context = nullptr;
    std::vector<std::int32_t> index;
    std::vector<std::uint8_t> band;
    std::vector<std::uint8_t> previous;
    std::vector<Sequence> sequences;
    std::vector<std::uint8_t> raw_sequences;
    std::vector<std::uint8_t> literals;
    std::vector<std::uint8_t> packed;
    std::vector<std::uint8_t> separated;

    [[nodiscard]] std::size_t bytes() const {
        std::size_t total = context_storage.bytes + capacity_bytes(index) + capacity_bytes(band) + capacity_bytes(previous)
            + capacity_bytes(sequences) + capacity_bytes(raw_sequences) + capacity_bytes(literals)
            + capacity_bytes(packed) + capacity_bytes(separated);
        return total;
    }
};

namespace {

bool reserve_encoder(CngEncoderStorage& storage, const BandShape& shape, bool strided) {
    if (storage.context == nullptr) {
        std::size_t required = ZSTD_estimateCCtxSize(compression_level);
        if (ZSTD_isError(required) != 0 || required == 0 || required > compression_context_limit
            || !reserve_context(storage.context_storage, required)) {
            return false;
        }
        storage.context = ZSTD_initStaticCCtx(storage.context_storage.words.get(), storage.context_storage.bytes);
        if (storage.context == nullptr) {
            return false;
        }
    }
    bool grown = grow(storage.index, detail::index_slots)
        && (!strided || grow(storage.band, shape.band_bytes))
        && grow(storage.previous, shape.row_bytes)
        && grow(storage.sequences, shape.sequence_capacity)
        && grow(storage.raw_sequences, shape.sequence_capacity * detail::sequence_record_bytes)
        && grow(storage.literals, shape.band_bytes)
        && grow(storage.packed, shape.packed_capacity)
        && grow(storage.separated, shape.separated_capacity);
    return grown;
}

void write_chunk_framing(std::span<std::uint8_t> destination, std::size_t offset, const ChunkType& type, std::size_t length) {
    write_u32(destination, offset, static_cast<std::uint32_t>(length));
    std::copy(type.begin(), type.end(), destination.begin() + static_cast<std::ptrdiff_t>(offset + 4));
    std::uint32_t crc = chunk_crc(destination.subspan(offset + 4, 4 + length));
    write_u32(destination, offset + 8 + length, crc);
}

// Encodes one band at `offset` (enough room is guaranteed by the bound).
// Returns the offset after the chunk, or zero on an entropy failure.
std::size_t encode_band(CngEncoderStorage& storage, const BandShape& shape, std::span<const std::uint8_t> source,
                        std::span<const std::uint8_t> previous, std::span<std::uint8_t> destination,
                        std::size_t offset, CngEncodeResult& result) {
    std::size_t literal_pixels = 0;
    std::span<std::int32_t> index(storage.index.data(), detail::index_slots);
    std::span<Sequence> capacity(storage.sequences.data(), shape.sequence_capacity);
    std::size_t count = detail::find_sequences(source, previous, shape.channels, index, capacity, literal_pixels);
    std::span<const Sequence> sequences(storage.sequences.data(), count);
    std::size_t raw_bytes = count * detail::sequence_record_bytes;
    std::span<std::uint8_t> raw(storage.raw_sequences.data(), raw_bytes);
    detail::pack_sequences(sequences, raw);
    std::size_t payload = offset + 8;
    std::size_t frame_offset = payload + detail::band_fields_bytes;
    std::size_t frame_bytes = ZSTD_compressCCtx(storage.context, destination.data() + frame_offset,
                                                destination.size() - frame_offset, raw.data(), raw.size(), compression_level);
    if (ZSTD_isError(frame_bytes) != 0) {
        return 0;
    }
    std::size_t coding_offset = frame_offset + frame_bytes;
    std::size_t literal_offset = coding_offset + detail::band_coding_bytes;
    std::size_t literal_bytes = literal_pixels * shape.channels;
    std::span<std::uint8_t> samples(storage.literals.data(), literal_bytes);
    std::size_t packed_size = detail::packed_bytes(literal_pixels, shape.channels);
    std::span<std::uint8_t> packed(storage.packed.data(), packed_size);
    detail::collect_literals(source, previous, shape.channels, sequences, samples, packed);
    std::size_t literal_size = literal_bytes;
    LiteralCoding coding = LiteralCoding::stored;
    if (literal_bytes != 0) {
        std::span<std::uint8_t> separated(storage.separated.data(), 2 * packed_size + 4);
        std::size_t separated_size = detail::separate_runs(packed, separated);
        std::size_t compressed = ZSTD_compressCCtx(storage.context, destination.data() + literal_offset,
                                                   destination.size() - literal_offset, separated.data(),
                                                   separated_size, compression_level);
        if (ZSTD_isError(compressed) != 0) {
            return 0;
        }
        if (compressed < literal_bytes) {
            literal_size = compressed;
            coding = LiteralCoding::compressed;
        }
    }
    if (coding == LiteralCoding::stored) {
        if (literal_bytes != 0) {
            std::memcpy(destination.data() + literal_offset, samples.data(), literal_bytes);
        }
        ++result.stored_bands;
    } else {
        ++result.compressed_bands;
    }
    destination[coding_offset] = static_cast<std::uint8_t>(coding);
    write_u32(destination, payload, static_cast<std::uint32_t>(count));
    write_u32(destination, payload + 4, static_cast<std::uint32_t>(literal_pixels));
    write_u32(destination, payload + 8, static_cast<std::uint32_t>(frame_bytes));
    std::size_t length = literal_offset + literal_size - payload;
    write_chunk_framing(destination, offset, detail::band_type, length);
    std::size_t next = offset + detail::chunk_framing_bytes + length;
    return next;
}

} // namespace

CngEncoder::CngEncoder() noexcept {}

CngEncoder::~CngEncoder() {}

void CngEncoder::release() noexcept {
    storage_.reset();
}

std::size_t CngEncoder::workspace_bytes() const noexcept {
    std::size_t bytes = storage_ ? (*storage_).bytes() : 0;
    return bytes;
}

CngEncodeResult CngEncoder::encode(const CngEncodeInput& input, std::span<std::byte> destination) noexcept {
    CngEncodeResult result;
    result.workspace_bytes = workspace_bytes();
    CngImage image = input.image;
    if (!admitted(image)) {
        result.error = CngError::unsupported_dimensions;
        return result;
    }
    std::size_t channels = channel_count(image.channels);
    std::uint64_t row_bytes = static_cast<std::uint64_t>(image.width) * channels;
    // The final row needs only its samples; earlier rows need the full stride.
    if (input.row_bytes < row_bytes || input.row_bytes > std::numeric_limits<std::size_t>::max() / image.height) {
        result.error = CngError::invalid_argument;
        return result;
    }
    std::uint64_t required = input.row_bytes * (image.height - 1) + row_bytes;
    if (input.samples.size() < required) {
        result.error = CngError::invalid_argument;
        return result;
    }
    std::size_t bound = cng_encoded_bound(image);
    if (bound == 0 || destination.size() < bound) {
        result.error = CngError::insufficient_storage;
        return result;
    }
    if (overlapping(input.samples.data(), static_cast<std::size_t>(required), destination.data(), destination.size())) {
        result.error = CngError::overlap;
        return result;
    }
    bool strided = input.row_bytes != row_bytes;
    BandShape shape = detail::band_shape(image.width, image.height, channels);
    if (!storage_) {
        try {
            storage_ = std::make_unique<CngEncoderStorage>();
        } catch (const std::bad_alloc&) {
            result.error = CngError::allocation_failed;
            return result;
        }
    }
    CngEncoderStorage& storage = *storage_;
    if (!reserve_encoder(storage, shape, strided)) {
        result.error = CngError::allocation_failed;
        result.workspace_bytes = workspace_bytes();
        return result;
    }
    result.workspace_bytes = workspace_bytes();
    std::span<const std::uint8_t> samples = bytes_of(input.samples);
    std::span<std::uint8_t> output = bytes_of(destination);
    std::copy(detail::signature.begin(), detail::signature.end(), output.begin());
    std::size_t header = detail::signature.size();
    std::span<std::uint8_t> fields = output.subspan(header + 8, detail::header_payload_bytes);
    std::fill(fields.begin(), fields.end(), std::uint8_t{0});
    fields[0] = detail::format_version;
    fields[1] = static_cast<std::uint8_t>(channels);
    write_u32(fields, 4, image.width);
    write_u32(fields, 8, image.height);
    write_u32(fields, 12, static_cast<std::uint32_t>(detail::band_budget));
    write_chunk_framing(output, header, detail::header_type, detail::header_payload_bytes);
    std::size_t offset = header + detail::chunk_framing_bytes + detail::header_payload_bytes;
    std::span<std::uint8_t> previous(storage.previous.data(), shape.row_bytes);
    std::fill(previous.begin(), previous.end(), std::uint8_t{0});
    std::size_t stride = static_cast<std::size_t>(input.row_bytes);
    for (std::size_t row = 0; row < image.height; row += shape.rows) {
        std::size_t rows = std::min<std::size_t>(shape.rows, image.height - row);
        std::size_t band_bytes = rows * shape.row_bytes;
        std::span<const std::uint8_t> source;
        if (strided) {
            for (std::size_t line = 0; line < rows; ++line) {
                std::memcpy(storage.band.data() + line * shape.row_bytes, samples.data() + (row + line) * stride, shape.row_bytes);
            }
            source = std::span<const std::uint8_t>(storage.band.data(), band_bytes);
        } else {
            source = samples.subspan(row * shape.row_bytes, band_bytes);
        }
        offset = encode_band(storage, shape, source, previous, output, offset, result);
        if (offset == 0) {
            result.error = CngError::entropy_failure;
            result.stored_bands = 0;
            result.compressed_bands = 0;
            return result;
        }
        std::memcpy(previous.data(), source.data() + band_bytes - shape.row_bytes, shape.row_bytes);
    }
    write_chunk_framing(output, offset, detail::end_type, 0);
    result.error = CngError::none;
    result.encoded_bytes = offset + detail::chunk_framing_bytes;
    return result;
}

} // namespace stx_cng
