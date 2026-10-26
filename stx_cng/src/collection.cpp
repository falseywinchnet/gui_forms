// CNG collections (docs/COLLECTIONS.md): a packed set of standalone CNG
// images, or one composite CNG canvas sliced into its members.
#include "stx_cng/collection.hpp"

#include "format.hpp"
#include "interface.hpp"
#include "storage.hpp"

#include <algorithm>
#include <array>
#include <limits>
#include <vector>

namespace stx_cng {
namespace {

using detail::Shape;

constexpr std::array<std::uint8_t, 8> collection_signature{0x8A, 0x43, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
constexpr std::uint32_t schema_version = 1;
using Tag = std::array<std::uint8_t, 4>;
constexpr Tag header_tag{'S', 'H', 'D', 'R'};
constexpr Tag directory_tag{'D', 'I', 'R', 'E'};
constexpr Tag image_tag{'I', 'M', 'A', 'G'};
constexpr Tag end_tag{'S', 'E', 'N', 'D'};
// A chunk's length, tag and CRC32 around its body.
constexpr std::size_t chunk_framing = 12;
constexpr std::size_t header_body = 24;
constexpr std::size_t entry_bytes = 40;
constexpr std::uint32_t padding_unit = 128;

// ---------------------------------------------------------------- bytes
std::uint64_t read_u64(std::span<const std::uint8_t> bytes, std::size_t offset) noexcept {
    std::uint64_t high = detail::read_u32(bytes, offset);
    std::uint64_t low = detail::read_u32(bytes, offset + 4);
    std::uint64_t value = (high << 32) | low;
    return value;
}

void write_u64(std::span<std::uint8_t> bytes, std::size_t offset, std::uint64_t value) noexcept {
    detail::write_u32(bytes, offset, static_cast<std::uint32_t>(value >> 32));
    detail::write_u32(bytes, offset + 4, static_cast<std::uint32_t>(value));
}

std::uint32_t chunk_crc(const Tag& tag, std::span<const std::uint8_t> body) noexcept {
    std::uint32_t tagged = detail::crc_of(tag);
    std::uint32_t crc = detail::crc_extend(tagged, body);
    return crc;
}

// Writes a chunk's length and tag at `offset`, before a body of `body` bytes
// that is already in place, and its CRC32 after.
void frame_chunk(std::span<std::uint8_t> out, std::size_t offset, const Tag& tag, std::size_t body) noexcept {
    detail::write_u32(out, offset, static_cast<std::uint32_t>(body));
    std::copy(tag.begin(), tag.end(), out.begin() + static_cast<std::ptrdiff_t>(offset + 4));
    std::span<const std::uint8_t> written = out.subspan(offset + 8, body);
    detail::write_u32(out, offset + 8 + body, chunk_crc(tag, written));
}

// The body of the chunk at `offset`, which must have this tag, a length of at
// most `bound`, fit the file and match its CRC32. Empty with false otherwise.
bool read_chunk(std::span<const std::uint8_t> file, std::size_t offset, const Tag& tag, std::size_t bound,
                std::span<const std::uint8_t>& body) noexcept {
    if (offset > file.size() || file.size() - offset < chunk_framing) {
        return false;
    }
    std::size_t length = detail::read_u32(file, offset);
    bool tagged = std::equal(tag.begin(), tag.end(), file.begin() + static_cast<std::ptrdiff_t>(offset + 4));
    if (!tagged || length > bound || length > file.size() - offset - chunk_framing) {
        return false;
    }
    body = file.subspan(offset + 8, length);
    bool intact = detail::read_u32(file, offset + 8 + length) == chunk_crc(tag, body);
    return intact;
}

// ---------------------------------------------------------------- bounds
bool within_bounds(Shape shape) noexcept {
    bool channels = shape.channels == 3 || shape.channels == 4;
    bool sides = shape.width >= 1 && shape.height >= 1 && shape.width <= maximum_collection_side && shape.height <= maximum_collection_side;
    if (!channels || !sides) {
        return false;
    }
    std::uint64_t padded_width = (static_cast<std::uint64_t>(shape.width) + padding_unit - 1) / padding_unit * padding_unit;
    std::uint64_t padded_height = (static_cast<std::uint64_t>(shape.height) + padding_unit - 1) / padding_unit * padding_unit;
    bool area = padded_width * padded_height <= maximum_collection_padded_pixels;
    bool perimeter = padded_width + padded_height <= maximum_collection_side;
    bool within = area && perimeter;
    return within;
}

// The canvas of a composite: the widest member's width, the members' total
// height, four channels when any member has four. False beyond the bounds.
bool composite_canvas(std::span<const CngCollectionMember> members, Shape& canvas) noexcept {
    canvas = Shape{0, 0, 3};
    std::uint64_t height = 0;
    for (const CngCollectionMember& member : members) {
        canvas.width = std::max(canvas.width, member.image.width);
        height += member.image.height;
        canvas.channels = std::max(canvas.channels, static_cast<std::uint32_t>(member.image.channels));
    }
    if (height > maximum_collection_side) {
        return false;
    }
    canvas.height = static_cast<std::uint32_t>(height);
    bool within = within_bounds(canvas);
    return within;
}

// The member list's own checks: count, bounds, unique ids, sample extents.
CngError check_members(std::span<const CngCollectionMember> members, CngCollectionForm form) noexcept {
    bool count = !members.empty() && members.size() <= maximum_collection_members;
    bool known = form == CngCollectionForm::packed || form == CngCollectionForm::composite;
    if (!count || !known) {
        return CngError::invalid_argument;
    }
    for (std::size_t index = 0; index < members.size(); ++index) {
        const CngCollectionMember& member = members[index];
        Shape shape = detail::shape_of(member.image);
        if (!within_bounds(shape) || detail::check_extent(shape, member.samples.size(), member.row_bytes) != CngError::none) {
            return CngError::invalid_argument;
        }
        for (std::size_t prior = 0; prior < index; ++prior) {
            if (members[prior].id.value == member.id.value) {
                return CngError::invalid_argument;
            }
        }
    }
    return CngError::none;
}

std::uint64_t fixed_bytes(std::size_t members) noexcept {
    std::uint64_t bytes = collection_signature.size() + (chunk_framing + header_body) + (chunk_framing + entry_bytes * members)
        + chunk_framing;
    return bytes;
}

} // namespace

std::uint64_t cng_collection_bound(std::span<const CngCollectionMember> members, CngCollectionForm form) noexcept {
    if (check_members(members, form) != CngError::none) {
        return 0;
    }
    std::uint64_t bound = fixed_bytes(members.size());
    if (form == CngCollectionForm::packed) {
        for (const CngCollectionMember& member : members) {
            bound += chunk_framing + cng_encoded_bound(member.image);
        }
    } else {
        Shape canvas;
        if (!composite_canvas(members, canvas)) {
            return 0;
        }
        bound += chunk_framing + cng_encoded_bound(detail::image_of(canvas));
    }
    if (bound > std::numeric_limits<std::size_t>::max()) {
        return 0;
    }
    return bound;
}

// ---------------------------------------------------------------- inspection
CngCollectionInfo inspect_cng_collection(std::span<const std::byte> encoded, std::span<CngCollectionEntry> entries) noexcept {
    CngCollectionInfo info;
    std::span<const std::uint8_t> file = detail::bytes_of(encoded);
    std::size_t size = file.size();
    if (size < collection_signature.size() || !std::equal(collection_signature.begin(), collection_signature.end(), file.begin())) {
        info.error = CngError::invalid_signature;
        return info;
    }
    info.error = CngError::invalid_collection;
    std::span<const std::uint8_t> header{};
    std::size_t header_at = collection_signature.size();
    if (!read_chunk(file, header_at, header_tag, header_body, header) || header.size() != header_body) {
        return info;
    }
    std::uint32_t version = detail::read_u32(header, 0);
    std::uint32_t form = detail::read_u32(header, 4);
    std::uint32_t members = detail::read_u32(header, 8);
    Shape canvas{detail::read_u32(header, 12), detail::read_u32(header, 16), detail::read_u32(header, 20)};
    if (version != schema_version || form > 1 || members == 0 || members > maximum_collection_members) {
        return info;
    }
    info.form = static_cast<CngCollectionForm>(form);
    bool composite = info.form == CngCollectionForm::composite;
    bool canvas_valid = composite ? within_bounds(canvas) : canvas.width == 0 && canvas.height == 0 && canvas.channels == 0;
    if (!canvas_valid) {
        return info;
    }
    std::span<const std::uint8_t> directory{};
    std::size_t directory_at = header_at + chunk_framing + header_body;
    std::size_t directory_bytes = entry_bytes * members;
    if (!read_chunk(file, directory_at, directory_tag, directory_bytes, directory) || directory.size() != directory_bytes) {
        return info;
    }
    info.members = members;
    info.canvas = composite ? detail::image_of(canvas) : CngImage{0, 0, CngChannels::bgra};
    if (entries.size() < members) {
        info.error = CngError::insufficient_storage;
        return info;
    }
    std::uint64_t next = directory_at + chunk_framing + directory_bytes;
    std::uint64_t covered = 0;
    for (std::size_t index = 0; index < members; ++index) {
        std::size_t at = index * entry_bytes;
        CngCollectionEntry& entry = entries[index];
        entry.id.value = detail::read_u32(directory, at);
        Shape shape{detail::read_u32(directory, at + 4), detail::read_u32(directory, at + 8), detail::read_u32(directory, at + 12)};
        entry.x = detail::read_u32(directory, at + 16);
        entry.y = detail::read_u32(directory, at + 20);
        entry.chunk_offset = read_u64(directory, at + 24);
        entry.chunk_bytes = read_u64(directory, at + 32);
        if (!within_bounds(shape)) {
            return info;
        }
        entry.image = detail::image_of(shape);
        for (std::size_t prior = 0; prior < index; ++prior) {
            if (entries[prior].id.value == entry.id.value) {
                return info;
            }
        }
        if (!composite) {
            if (entry.x != 0 || entry.y != 0) {
                return info;
            }
        } else {
            bool stacked = entry.x == 0 && entry.y == covered && shape.height <= canvas.height - covered && shape.width <= canvas.width
                && shape.channels <= canvas.channels;
            if (!stacked) {
                return info;
            }
            covered += shape.height;
            if (index != 0) {
                bool shared = entry.chunk_offset == entries[0].chunk_offset && entry.chunk_bytes == entries[0].chunk_bytes;
                if (!shared) {
                    return info;
                }
                continue;
            }
        }
        bool placed = entry.chunk_offset == next && next <= size && entry.chunk_bytes >= chunk_framing && entry.chunk_bytes <= size - next
            && entry.chunk_bytes - chunk_framing <= std::numeric_limits<std::uint32_t>::max();
        if (!placed) {
            return info;
        }
        next += entry.chunk_bytes;
    }
    if (composite && covered != canvas.height) {
        return info;
    }
    if (next > size || size - next != chunk_framing) {
        return info;
    }
    std::span<const std::uint8_t> ending{};
    if (!read_chunk(file, static_cast<std::size_t>(next), end_tag, 0, ending)) {
        return info;
    }
    // Every image chunk's framing and CRC32, and its file's container and
    // dimensions.
    std::size_t images = composite ? 1 : members;
    for (std::size_t index = 0; index < images; ++index) {
        const CngCollectionEntry& entry = entries[index];
        std::size_t body_bytes = static_cast<std::size_t>(entry.chunk_bytes) - chunk_framing;
        std::span<const std::uint8_t> body{};
        if (!read_chunk(file, static_cast<std::size_t>(entry.chunk_offset), image_tag, body_bytes, body) || body.size() != body_bytes) {
            return info;
        }
        std::span<const std::byte> member(reinterpret_cast<const std::byte*>(body.data()), body.size());
        CngInspectResult read = inspect_cng(member);
        if (read.error != CngError::none) {
            info.error = read.error;
            return info;
        }
        CngImage expected = composite ? info.canvas : entry.image;
        bool matches = read.image.width == expected.width && read.image.height == expected.height && read.image.channels == expected.channels;
        if (!matches) {
            return info;
        }
    }
    info.error = CngError::none;
    return info;
}

CngError copy_cng_collection_member(std::span<const std::byte> canvas, CngImage canvas_image, std::uint64_t canvas_row_bytes,
                                    const CngCollectionEntry& entry, std::span<std::byte> samples, std::uint64_t row_bytes) noexcept {
    Shape whole = detail::shape_of(canvas_image);
    Shape member = detail::shape_of(entry.image);
    bool admitted = detail::admitted_shape(whole) && detail::admitted_shape(member);
    bool inside = admitted && entry.x == 0 && member.width <= whole.width && entry.y <= whole.height
        && member.height <= whole.height - entry.y && member.channels <= whole.channels;
    if (!inside || detail::check_extent(whole, canvas.size(), canvas_row_bytes) != CngError::none) {
        return CngError::invalid_argument;
    }
    CngError error = detail::check_destination(member, canvas, samples, row_bytes);
    if (error != CngError::none) {
        return error;
    }
    const std::byte* source = canvas.data();
    std::byte* destination = samples.data();
    std::size_t canvas_stride = static_cast<std::size_t>(canvas_row_bytes);
    std::size_t stride = static_cast<std::size_t>(row_bytes);
    std::size_t row = static_cast<std::size_t>(member.width) * member.channels;
    for (std::uint32_t y = 0; y < member.height; ++y) {
        const std::byte* from = source + static_cast<std::size_t>(entry.y + y) * canvas_stride;
        std::byte* to = destination + static_cast<std::size_t>(y) * stride;
        if (member.channels == whole.channels) {
            std::copy_n(from, row, to);
            continue;
        }
        // A BGR member of a BGRA canvas: drop the alpha the canvas added.
        for (std::uint32_t x = 0; x < member.width; ++x) {
            std::copy_n(from + static_cast<std::size_t>(x) * 4, 3, to + static_cast<std::size_t>(x) * 3);
        }
    }
    return CngError::none;
}

// ---------------------------------------------------------------- encoding
class CngCollectionEncoderStorage final {
public:
    CngEncoder encoder;
    // Composite: the canvas samples, tightly packed.
    std::vector<std::byte> canvas;
};

CngCollectionEncoder::CngCollectionEncoder() noexcept {}

CngCollectionEncoder::~CngCollectionEncoder() {}

void CngCollectionEncoder::release() noexcept {
    storage_.reset();
}

std::size_t CngCollectionEncoder::workspace_bytes() const noexcept {
    if (!storage_) {
        return 0;
    }
    std::size_t bytes = sizeof(CngCollectionEncoderStorage) + (*storage_).encoder.workspace_bytes()
        + detail::capacity_bytes((*storage_).canvas);
    return bytes;
}

CngCollectionEncodeResult CngCollectionEncoder::encode(std::span<const CngCollectionMember> members, CngCollectionForm form,
                                                       std::uint32_t threads, std::span<std::byte> budget) noexcept {
    CngCollectionEncodeResult result;
    result.workspace_bytes = workspace_bytes();
    result.error = check_members(members, form);
    Shape canvas{0, 0, 0};
    bool composite = form == CngCollectionForm::composite;
    if (result.error == CngError::none && composite && !composite_canvas(members, canvas)) {
        result.error = CngError::invalid_argument;
    }
    for (std::size_t index = 0; result.error == CngError::none && index < members.size(); ++index) {
        if (detail::overlapping(members[index].samples, budget)) {
            result.error = CngError::overlap;
        }
    }
    if (result.error != CngError::none) {
        return result;
    }
    std::uint64_t fixed = fixed_bytes(members.size());
    if (budget.size() < fixed + chunk_framing) {
        result.error = CngError::insufficient_storage;
        return result;
    }
    if (!storage_) {
        try {
            storage_ = std::make_unique<CngCollectionEncoderStorage>();
        } catch (const std::bad_alloc&) {
            result.error = CngError::allocation_failed;
            return result;
        }
    }
    CngCollectionEncoderStorage& storage = *storage_;
    std::span<std::uint8_t> out = detail::bytes_of(budget);
    std::size_t header_at = collection_signature.size();
    std::size_t directory_at = header_at + chunk_framing + header_body;
    std::size_t directory_bytes = entry_bytes * members.size();
    std::size_t position = directory_at + chunk_framing + directory_bytes;
    std::copy(collection_signature.begin(), collection_signature.end(), out.begin());
    std::span<std::uint8_t> header = out.subspan(header_at + 8, header_body);
    detail::write_u32(header, 0, schema_version);
    detail::write_u32(header, 4, static_cast<std::uint32_t>(form));
    detail::write_u32(header, 8, static_cast<std::uint32_t>(members.size()));
    detail::write_u32(header, 12, canvas.width);
    detail::write_u32(header, 16, canvas.height);
    detail::write_u32(header, 20, canvas.channels);
    frame_chunk(out, header_at, header_tag, header_body);
    std::span<std::uint8_t> directory = out.subspan(directory_at + 8, directory_bytes);
    // A composite's canvas: the members stacked at x = 0, zero to the right of
    // narrower ones, BGR members given alpha 255 on a BGRA canvas.
    std::size_t canvas_row = static_cast<std::size_t>(canvas.width) * canvas.channels;
    std::size_t canvas_bytes = canvas_row * canvas.height;
    if (composite) {
        if (!detail::grow(storage.canvas, canvas_bytes)) {
            result.error = CngError::allocation_failed;
            result.workspace_bytes = workspace_bytes();
            return result;
        }
        std::fill_n(storage.canvas.begin(), canvas_bytes, std::byte{0});
        std::size_t y0 = 0;
        for (const CngCollectionMember& member : members) {
            std::size_t channels = static_cast<std::size_t>(member.image.channels);
            for (std::uint32_t y = 0; y < member.image.height; ++y) {
                const std::byte* from = member.samples.data() + static_cast<std::size_t>(y) * member.row_bytes;
                std::byte* to = storage.canvas.data() + (y0 + y) * canvas_row;
                if (channels == canvas.channels) {
                    std::copy_n(from, static_cast<std::size_t>(member.image.width) * channels, to);
                    continue;
                }
                for (std::uint32_t x = 0; x < member.image.width; ++x) {
                    std::copy_n(from + static_cast<std::size_t>(x) * 3, 3, to + static_cast<std::size_t>(x) * 4);
                    to[static_cast<std::size_t>(x) * 4 + 3] = std::byte{255};
                }
            }
            y0 += member.image.height;
        }
    }
    // Every CNG file goes into the budget as an image chunk at `position`,
    // leaving room for the end chunk.
    std::size_t images = composite ? 1 : members.size();
    std::uint64_t canvas_y = 0;
    for (std::size_t image = 0; image < images; ++image) {
        CngEncodeInput input;
        input.threads = threads;
        if (composite) {
            input.image = detail::image_of(canvas);
            input.row_bytes = canvas_row;
            input.samples = std::span<const std::byte>(storage.canvas.data(), canvas_bytes);
        } else {
            input.image = members[image].image;
            input.row_bytes = members[image].row_bytes;
            input.samples = members[image].samples;
        }
        // Room for this chunk's framing and the end chunk after it.
        if (budget.size() - position < 2 * chunk_framing) {
            result.error = CngError::insufficient_storage;
            return result;
        }
        std::span<std::byte> body = budget.subspan(position + 8, budget.size() - position - 2 * chunk_framing);
        CngEncodeResult encoded = storage.encoder.encode(input, body);
        if (encoded.error != CngError::none) {
            result.error = encoded.error;
            result.workspace_bytes = workspace_bytes();
            return result;
        }
        if (encoded.encoded_bytes > std::numeric_limits<std::uint32_t>::max()) {
            result.error = CngError::invalid_argument;
            return result;
        }
        frame_chunk(out, position, image_tag, encoded.encoded_bytes);
        std::size_t chunk = chunk_framing + encoded.encoded_bytes;
        std::size_t entries = composite ? members.size() : 1;
        std::size_t first = composite ? 0 : image;
        for (std::size_t entry = first; entry < first + entries; ++entry) {
            const CngCollectionMember& member = members[entry];
            std::size_t at = entry * entry_bytes;
            detail::write_u32(directory, at, member.id.value);
            detail::write_u32(directory, at + 4, member.image.width);
            detail::write_u32(directory, at + 8, member.image.height);
            detail::write_u32(directory, at + 12, static_cast<std::uint32_t>(member.image.channels));
            detail::write_u32(directory, at + 16, 0);
            detail::write_u32(directory, at + 20, composite ? static_cast<std::uint32_t>(canvas_y) : 0U);
            write_u64(directory, at + 24, position);
            write_u64(directory, at + 32, chunk);
            if (composite) {
                canvas_y += member.image.height;
            }
        }
        position += chunk;
    }
    frame_chunk(out, directory_at, directory_tag, directory_bytes);
    frame_chunk(out, position, end_tag, 0);
    position += chunk_framing;
    result.encoded_bytes = position;
    result.workspace_bytes = workspace_bytes();
    result.error = CngError::none;
    return result;
}

// ---------------------------------------------------------------- decoding
class CngCollectionDecoderStorage final {
public:
    CngDecoder decoder;
    std::vector<CngCollectionEntry> entries;
    // Composite: the decoded canvas, tightly packed.
    std::vector<std::byte> canvas;
};

namespace {

// Inspects into the owner's directory storage, growing it to the count.
CngCollectionInfo inspect_into(CngCollectionDecoderStorage& storage, std::span<const std::byte> encoded) noexcept {
    std::span<CngCollectionEntry> entries(storage.entries);
    CngCollectionInfo info = inspect_cng_collection(encoded, entries);
    if (info.error == CngError::insufficient_storage) {
        if (!detail::grow(storage.entries, info.members)) {
            info.error = CngError::allocation_failed;
            return info;
        }
        info = inspect_cng_collection(encoded, std::span<CngCollectionEntry>(storage.entries));
    }
    return info;
}

std::span<const std::byte> image_body(std::span<const std::byte> encoded, const CngCollectionEntry& entry) noexcept {
    std::span<const std::byte> body = encoded.subspan(static_cast<std::size_t>(entry.chunk_offset) + 8,
                                                      static_cast<std::size_t>(entry.chunk_bytes) - chunk_framing);
    return body;
}

CngError make_storage(std::unique_ptr<CngCollectionDecoderStorage>& storage) noexcept {
    if (storage) {
        return CngError::none;
    }
    try {
        storage = std::make_unique<CngCollectionDecoderStorage>();
    } catch (const std::bad_alloc&) {
        return CngError::allocation_failed;
    }
    return CngError::none;
}

} // namespace

CngCollectionDecoder::CngCollectionDecoder() noexcept {}

CngCollectionDecoder::~CngCollectionDecoder() {}

void CngCollectionDecoder::release() noexcept {
    storage_.reset();
}

std::size_t CngCollectionDecoder::workspace_bytes() const noexcept {
    if (!storage_) {
        return 0;
    }
    std::size_t bytes = sizeof(CngCollectionDecoderStorage) + (*storage_).decoder.workspace_bytes()
        + detail::capacity_bytes((*storage_).entries) + detail::capacity_bytes((*storage_).canvas);
    return bytes;
}

CngDecodeResult CngCollectionDecoder::decode_canvas(std::span<const std::byte> encoded, std::span<std::byte> samples,
                                                    std::uint64_t row_bytes, std::uint32_t threads) noexcept {
    CngDecodeResult result;
    result.workspace_bytes = workspace_bytes();
    result.error = make_storage(storage_);
    if (result.error != CngError::none) {
        return result;
    }
    CngCollectionDecoderStorage& storage = *storage_;
    CngCollectionInfo info = inspect_into(storage, encoded);
    if (info.error == CngError::none && info.form != CngCollectionForm::composite) {
        info.error = CngError::invalid_argument;
    }
    if (info.error != CngError::none) {
        result.error = info.error;
        result.workspace_bytes = workspace_bytes();
        return result;
    }
    result = storage.decoder.decode(image_body(encoded, storage.entries[0]), samples, row_bytes, threads);
    result.workspace_bytes = workspace_bytes();
    return result;
}

CngDecodeResult CngCollectionDecoder::decode_member(std::span<const std::byte> encoded, CngMemberId id, std::span<std::byte> samples,
                                                    std::uint64_t row_bytes, std::uint32_t threads) noexcept {
    CngDecodeResult result;
    result.workspace_bytes = workspace_bytes();
    result.error = make_storage(storage_);
    if (result.error != CngError::none) {
        return result;
    }
    CngCollectionDecoderStorage& storage = *storage_;
    CngCollectionInfo info = inspect_into(storage, encoded);
    if (info.error != CngError::none) {
        result.error = info.error;
        result.workspace_bytes = workspace_bytes();
        return result;
    }
    std::size_t selected = info.members;
    for (std::size_t index = 0; index < info.members; ++index) {
        if (storage.entries[index].id.value == id.value) {
            selected = index;
            break;
        }
    }
    if (selected == info.members) {
        result.error = CngError::invalid_argument;
        return result;
    }
    const CngCollectionEntry& entry = storage.entries[selected];
    result.image = entry.image;
    if (info.form == CngCollectionForm::packed) {
        result = storage.decoder.decode(image_body(encoded, entry), samples, row_bytes, threads);
        result.workspace_bytes = workspace_bytes();
        return result;
    }
    // Composite: check the destination first, then decode the canvas into
    // owned storage and copy the member's rows out.
    result.error = detail::check_destination(detail::shape_of(entry.image), encoded, samples, row_bytes);
    if (result.error != CngError::none) {
        return result;
    }
    Shape canvas = detail::shape_of(info.canvas);
    std::size_t canvas_row = static_cast<std::size_t>(canvas.width) * canvas.channels;
    std::size_t canvas_bytes = canvas_row * canvas.height;
    if (!detail::grow(storage.canvas, canvas_bytes)) {
        result.error = CngError::allocation_failed;
        result.workspace_bytes = workspace_bytes();
        return result;
    }
    std::span<std::byte> decoded(storage.canvas.data(), canvas_bytes);
    CngDecodeResult whole = storage.decoder.decode(image_body(encoded, storage.entries[0]), decoded, canvas_row, threads);
    result.workspace_bytes = workspace_bytes();
    if (whole.error != CngError::none) {
        result.error = whole.error;
        return result;
    }
    result.error = copy_cng_collection_member(decoded, info.canvas, canvas_row, entry, samples, row_bytes);
    return result;
}

} // namespace stx_cng
