#include "container.hpp"

#include "tables.hpp"

#include <algorithm>

namespace stx_cng::detail {
namespace {

// Offsets within the header fields (section 3.2).
constexpr std::size_t width_field = 0;
constexpr std::size_t height_field = 4;
constexpr std::size_t channels_field = 8;
constexpr std::size_t version_field = 9;
constexpr std::size_t segment_width_field = 10;
constexpr std::size_t segment_height_field = 11;
constexpr std::size_t mosaic_length_field = 12;
constexpr std::size_t mosaic_crc_field = 16;
constexpr std::size_t tables_length_field = 20;
constexpr std::size_t tables_crc_field = 24;
// The smallest file: signature, both header copies, and nothing between.
constexpr std::size_t minimum_file = front_bytes + field_block_bytes;

struct Fields final {
    Shape shape{};
    std::uint64_t mosaic_length = 0;
    std::uint32_t mosaic_crc = 0;
    std::uint64_t tables_length = 0;
    std::uint32_t tables_crc = 0;
};

// Parses the 28 field bytes of `block`; the CRC32 has been checked.
CngError parse_fields(std::span<const std::uint8_t> block, Fields& fields) noexcept {
    bool version = block[version_field] == detail::version;
    bool segments = block[segment_width_field] == segment_columns && block[segment_height_field] == segment_rows;
    if (!version || !segments) {
        return CngError::unsupported_version;
    }
    fields.shape.width = read_u32(block, width_field);
    fields.shape.height = read_u32(block, height_field);
    fields.shape.channels = block[channels_field];
    if (!admitted_shape(fields.shape)) {
        return CngError::invalid_header;
    }
    fields.mosaic_length = read_u32(block, mosaic_length_field);
    fields.mosaic_crc = read_u32(block, mosaic_crc_field);
    fields.tables_length = read_u32(block, tables_length_field);
    fields.tables_crc = read_u32(block, tables_crc_field);
    return CngError::none;
}

bool signature_matches(std::span<const std::uint8_t> file) noexcept {
    std::size_t compared = std::min(file.size(), image_signature.size());
    bool matches = std::equal(image_signature.begin(), image_signature.begin() + static_cast<std::ptrdiff_t>(compared), file.begin());
    return matches;
}

// The layout every part's position follows from (section 3).
struct Layout final {
    std::uint64_t directory_block = 0;
    std::uint64_t tables_copies = 0;
    // Every byte that is not a segment payload.
    std::uint64_t fixed_bytes = 0;
};

Layout layout_of(const Fields& fields, std::uint64_t segments) noexcept {
    Layout layout;
    layout.directory_block = directory_entry_bytes * segments + 4;
    layout.tables_copies = segments == 1 ? 1 : 2;
    layout.fixed_bytes = front_bytes + 2 * layout.directory_block + 2 * fields.mosaic_length
        + layout.tables_copies * fields.tables_length + field_block_bytes;
    return layout;
}

std::uint64_t payload_total(std::span<const std::uint8_t> directory, std::uint64_t segments) noexcept {
    std::uint64_t total = 0;
    for (std::uint64_t index = 0; index < segments; ++index) {
        total += read_u32(directory, static_cast<std::size_t>(index) * directory_entry_bytes);
    }
    return total;
}

} // namespace

CngError read_container(std::span<const std::uint8_t> file, ContainerReading reading, Container& container) noexcept {
    std::size_t size = file.size();
    if (reading == ContainerReading::decoder && (size < image_signature.size() || !signature_matches(file))) {
        return CngError::invalid_signature;
    }
    if (size < minimum_file) {
        return CngError::invalid_header;
    }
    std::span<const std::uint8_t> front = file.subspan(image_signature.size(), field_block_bytes);
    std::span<const std::uint8_t> tail = file.subspan(size - field_block_bytes, field_block_bytes);
    std::span<const std::uint8_t> chosen{};
    if (block_valid(front, field_bytes)) {
        chosen = front;
    } else if (block_valid(tail, field_bytes)) {
        chosen = tail;
    } else {
        return CngError::invalid_header;
    }
    Fields fields;
    CngError parsed = parse_fields(chosen, fields);
    if (parsed != CngError::none) {
        return parsed;
    }
    std::uint64_t segments = segment_count(fields.shape);
    Layout layout = layout_of(fields, segments);
    if (size < layout.fixed_bytes) {
        return CngError::invalid_directory;
    }
    std::size_t block = static_cast<std::size_t>(layout.directory_block);
    std::size_t entries = block - 4;
    std::span<const std::uint8_t> first = file.subspan(front_bytes, block);
    std::span<const std::uint8_t> mirror = file.subspan(size - field_block_bytes - block, block);
    if (block_valid(first, entries)) {
        container.directory = first.subspan(0, entries);
    } else if (block_valid(mirror, entries)) {
        container.directory = mirror.subspan(0, entries);
    } else {
        return CngError::invalid_directory;
    }
    std::size_t tables_length = static_cast<std::size_t>(fields.tables_length);
    std::size_t mosaic_length = static_cast<std::size_t>(fields.mosaic_length);
    std::size_t second_tables = layout.tables_copies == 2 ? tables_length : 0;
    std::size_t begin = front_bytes + block + tables_length + mosaic_length;
    // The second mosaic copy starts where the payload region ends.
    std::size_t end = size - field_block_bytes - block - second_tables - mosaic_length;
    std::span<const std::uint8_t> tables_front = file.subspan(front_bytes + block, tables_length);
    if (crc_of(tables_front) == fields.tables_crc) {
        container.tables = tables_front;
    } else if (layout.tables_copies == 2 && crc_of(file.subspan(end + mosaic_length, tables_length)) == fields.tables_crc) {
        container.tables = file.subspan(end + mosaic_length, tables_length);
    } else {
        return CngError::invalid_tables;
    }
    std::span<const std::uint8_t> mosaic_front = file.subspan(front_bytes + block + tables_length, mosaic_length);
    std::span<const std::uint8_t> mosaic_back = file.subspan(end, mosaic_length);
    if (crc_of(mosaic_front) == fields.mosaic_crc) {
        container.mosaic = mosaic_front;
    } else if (crc_of(mosaic_back) == fields.mosaic_crc) {
        container.mosaic = mosaic_back;
    } else {
        container.mosaic = {};
    }
    if (payload_total(container.directory, segments) != end - begin) {
        return CngError::invalid_directory;
    }
    container.shape = fields.shape;
    container.segments = segments;
    container.payload_begin = begin;
    container.payload_end = end;
    return CngError::none;
}

CngError validate_framing(std::span<const std::uint8_t> file, Container& container) noexcept {
    std::size_t size = file.size();
    if (!signature_matches(file)) {
        return CngError::invalid_signature;
    }
    if (size < front_bytes) {
        return CngError::truncated;
    }
    std::span<const std::uint8_t> front = file.subspan(image_signature.size(), field_block_bytes);
    if (!block_valid(front, field_bytes)) {
        return CngError::crc_mismatch;
    }
    Fields fields;
    CngError parsed = parse_fields(front, fields);
    if (parsed != CngError::none) {
        return parsed;
    }
    std::uint64_t segments = segment_count(fields.shape);
    Layout layout = layout_of(fields, segments);
    if (size - front_bytes < layout.directory_block) {
        return CngError::truncated;
    }
    std::size_t block = static_cast<std::size_t>(layout.directory_block);
    std::size_t entries = block - 4;
    std::span<const std::uint8_t> first = file.subspan(front_bytes, block);
    if (!block_valid(first, entries)) {
        return CngError::crc_mismatch;
    }
    std::uint64_t payloads = payload_total(first, segments);
    std::uint64_t implied = layout.fixed_bytes + payloads;
    if (size < implied) {
        return CngError::truncated;
    }
    if (size > implied) {
        return CngError::trailing_data;
    }
    std::size_t tables_length = static_cast<std::size_t>(fields.tables_length);
    std::size_t mosaic_length = static_cast<std::size_t>(fields.mosaic_length);
    std::size_t tables_at = front_bytes + block;
    std::size_t mosaic_at = tables_at + tables_length;
    std::size_t begin = mosaic_at + mosaic_length;
    std::size_t end = begin + static_cast<std::size_t>(payloads);
    std::span<const std::uint8_t> tables = file.subspan(tables_at, tables_length);
    std::span<const std::uint8_t> mosaic = file.subspan(mosaic_at, mosaic_length);
    if (crc_of(tables) != fields.tables_crc || crc_of(mosaic) != fields.mosaic_crc) {
        return CngError::crc_mismatch;
    }
    std::size_t position = begin;
    for (std::uint64_t index = 0; index < segments; ++index) {
        std::size_t offset = static_cast<std::size_t>(index) * directory_entry_bytes;
        std::size_t length = read_u32(first, offset);
        if (crc_of(file.subspan(position, length)) != read_u32(first, offset + 4)) {
            return CngError::crc_mismatch;
        }
        position += length;
    }
    // The second copies, in file order, byte for byte.
    std::size_t at = end;
    bool same = std::equal(mosaic.begin(), mosaic.end(), file.begin() + static_cast<std::ptrdiff_t>(at));
    at += mosaic_length;
    if (same && layout.tables_copies == 2) {
        same = std::equal(tables.begin(), tables.end(), file.begin() + static_cast<std::ptrdiff_t>(at));
        at += tables_length;
    }
    if (same) {
        same = std::equal(first.begin(), first.end(), file.begin() + static_cast<std::ptrdiff_t>(at));
        at += block;
    }
    if (same) {
        same = std::equal(front.begin(), front.end(), file.begin() + static_cast<std::ptrdiff_t>(at));
    }
    if (!same) {
        return CngError::copy_mismatch;
    }
    if (!single_frame(tables)) {
        return CngError::invalid_tables;
    }
    container.shape = fields.shape;
    container.segments = segments;
    container.directory = first.subspan(0, entries);
    container.tables = tables;
    container.mosaic = mosaic;
    container.payload_begin = begin;
    container.payload_end = end;
    return CngError::none;
}

} // namespace stx_cng::detail
