// The public CNG 1.0 entry points other than encoding: image geometry,
// inspection, strict framing and the CngDecoder owner.
#include "stx_cng/codec.hpp"

#include "container.hpp"
#include "decoder.hpp"
#include "interface.hpp"
#include "tables.hpp"

#include <algorithm>
#include <array>
#include <thread>

namespace stx_cng {
namespace detail {

// The segments one decoder worker decodes: [first, last), whose payloads
// start at `offset` in the file.
struct SegmentJob final {
    std::span<const std::uint8_t> file{};
    const Container* container = nullptr;
    const DecodeTables* tables = nullptr;
    TileWorkspace* workspace = nullptr;
    std::uint8_t* destination = nullptr;
    std::size_t row_bytes = 0;
    std::uint64_t first = 0;
    std::uint64_t last = 0;
    std::size_t offset = 0;
    CngError error = CngError::none;
};

namespace {

void run_segments(SegmentJob& job) noexcept {
    const Container& container = *job.container;
    std::size_t channels = container.shape.channels;
    std::size_t offset = job.offset;
    for (std::uint64_t index = job.first; index < job.last; ++index) {
        std::size_t length = payload_length(container, index);
        std::span<const std::uint8_t> payload = job.file.subspan(offset, length);
        offset += length;
        if (crc_of(payload) != payload_crc(container, index)) {
            job.error = CngError::invalid_segment;
            return;
        }
        SegmentRectangle rectangle = segment_rectangle(container.shape, index);
        std::uint8_t* origin = job.destination + static_cast<std::size_t>(rectangle.y) * job.row_bytes
            + static_cast<std::size_t>(rectangle.x) * channels;
        if (!decode_tile(*job.tables, payload, rectangle.shape, *job.workspace, origin, job.row_bytes)) {
            job.error = CngError::invalid_segment;
            return;
        }
    }
}

} // namespace
} // namespace detail

class CngDecoderStorage final {
public:
    detail::TablesReader reader{};
    detail::DecodeTables tables{};
    std::array<detail::TileWorkspace, maximum_threads> workspaces{};
    std::array<detail::SegmentJob, maximum_threads> jobs{};
    std::array<std::thread, maximum_threads> threads{};
    // Salvage: the mosaic's samples, and one segment's tight samples.
    std::vector<std::uint8_t> means;
    std::vector<std::uint8_t> scratch;
};

namespace {

using detail::Container;
using detail::ContainerReading;
using detail::Shape;

std::size_t storage_bytes(const CngDecoderStorage& storage) noexcept {
    std::size_t bytes = sizeof(CngDecoderStorage) + storage.reader.context_storage.bytes;
    for (const detail::TileWorkspace& workspace : storage.workspaces) {
        bytes += detail::capacity_bytes(workspace.residuals) + detail::capacity_bytes(workspace.energy)
            + detail::capacity_bytes(workspace.above);
    }
    bytes += detail::capacity_bytes(storage.means) + detail::capacity_bytes(storage.scratch);
    return bytes;
}

CngInspectResult inspected(CngError error, const Container& container) noexcept {
    CngInspectResult result;
    result.error = error;
    if (error == CngError::none) {
        result.image = detail::image_of(container.shape);
        result.segments = container.segments;
        result.mosaic_available = !container.mosaic.empty();
    }
    return result;
}

// Prepares the owner's storage and the image's tables; none on success.
CngError prepare(std::unique_ptr<CngDecoderStorage>& storage, const Container& container) noexcept {
    if (!storage) {
        try {
            storage = std::make_unique<CngDecoderStorage>();
        } catch (const std::bad_alloc&) {
            return CngError::allocation_failed;
        }
    }
    if (!detail::prepare_reader((*storage).reader)) {
        return CngError::allocation_failed;
    }
    CngError read = detail::read_tables(container.tables, (*storage).reader, (*storage).tables);
    return read;
}

} // namespace

std::uint64_t cng_sample_bytes(CngImage image) noexcept {
    Shape shape = detail::shape_of(image);
    if (!detail::admitted_shape(shape)) {
        return 0;
    }
    std::uint64_t bytes = static_cast<std::uint64_t>(shape.width) * shape.height * shape.channels;
    return bytes;
}

CngImage cng_mosaic_image(CngImage image) noexcept {
    Shape shape = detail::shape_of(image);
    CngImage mosaic{0, 0, image.channels};
    if (detail::admitted_shape(shape)) {
        mosaic = detail::image_of(detail::mosaic_shape(shape));
    }
    return mosaic;
}

std::uint64_t cng_segment_count(CngImage image) noexcept {
    Shape shape = detail::shape_of(image);
    std::uint64_t count = detail::admitted_shape(shape) ? detail::segment_count(shape) : 0;
    return count;
}

CngInspectResult inspect_cng(std::span<const std::byte> encoded) noexcept {
    Container container;
    CngError error = detail::read_container(detail::bytes_of(encoded), ContainerReading::decoder, container);
    CngInspectResult result = inspected(error, container);
    return result;
}

CngInspectResult inspect_cng_for_salvage(std::span<const std::byte> encoded) noexcept {
    Container container;
    CngError error = detail::read_container(detail::bytes_of(encoded), ContainerReading::salvage, container);
    CngInspectResult result = inspected(error, container);
    return result;
}

CngInspectResult validate_cng_framing(std::span<const std::byte> encoded) noexcept {
    Container container;
    CngError error = detail::validate_framing(detail::bytes_of(encoded), container);
    CngInspectResult result = inspected(error, container);
    return result;
}

CngDecoder::CngDecoder() noexcept {}

CngDecoder::~CngDecoder() {}

void CngDecoder::release() noexcept {
    storage_.reset();
}

std::size_t CngDecoder::workspace_bytes() const noexcept {
    std::size_t bytes = storage_ ? storage_bytes(*storage_) : 0;
    return bytes;
}

CngDecodeResult CngDecoder::decode(std::span<const std::byte> encoded, std::span<std::byte> samples, std::uint64_t row_bytes,
                                   std::uint32_t threads) noexcept {
    CngDecodeResult result;
    result.workspace_bytes = workspace_bytes();
    if (threads == 0 || threads > maximum_threads) {
        result.error = CngError::invalid_argument;
        return result;
    }
    std::span<const std::uint8_t> file = detail::bytes_of(encoded);
    Container container;
    result.error = detail::read_container(file, ContainerReading::decoder, container);
    if (result.error != CngError::none) {
        return result;
    }
    result.image = detail::image_of(container.shape);
    result.error = detail::check_destination(container.shape, encoded, samples, row_bytes);
    if (result.error != CngError::none) {
        return result;
    }
    result.error = prepare(storage_, container);
    result.workspace_bytes = workspace_bytes();
    if (result.error != CngError::none) {
        return result;
    }
    CngDecoderStorage& storage = *storage_;
    std::uint64_t workers = std::min<std::uint64_t>(threads, container.segments);
    Shape segment{std::min(detail::segment_columns, container.shape.width), std::min(detail::segment_rows, container.shape.height),
                  container.shape.channels};
    for (std::uint64_t worker = 0; worker < workers; ++worker) {
        if (!detail::reserve_tile(storage.workspaces[worker], segment)) {
            result.error = CngError::allocation_failed;
            result.workspace_bytes = workspace_bytes();
            return result;
        }
    }
    // Contiguous ranges of segments, and their first payload offsets from one
    // pass over the directory.
    std::size_t offset = container.payload_begin;
    std::uint64_t index = 0;
    for (std::uint64_t worker = 0; worker < workers; ++worker) {
        detail::SegmentJob& job = storage.jobs[worker];
        job.file = file;
        job.container = &container;
        job.tables = &storage.tables;
        job.workspace = &storage.workspaces[worker];
        job.destination = reinterpret_cast<std::uint8_t*>(samples.data());
        job.row_bytes = static_cast<std::size_t>(row_bytes);
        job.first = container.segments * worker / workers;
        job.last = container.segments * (worker + 1) / workers;
        job.error = CngError::none;
        while (index < job.first) {
            offset += detail::payload_length(container, index);
            ++index;
        }
        job.offset = offset;
    }
    std::span<detail::SegmentJob> jobs(storage.jobs.data(), static_cast<std::size_t>(workers));
    detail::run_jobs(jobs, std::span<std::thread>(storage.threads), detail::run_segments);
    result.error = CngError::none;
    for (const detail::SegmentJob& job : jobs) {
        if (job.error != CngError::none) {
            result.error = job.error;
            break;
        }
    }
    result.workspace_bytes = workspace_bytes();
    return result;
}

CngDecodeResult CngDecoder::decode_mosaic(std::span<const std::byte> encoded, std::span<std::byte> samples,
                                          std::uint64_t row_bytes) noexcept {
    CngDecodeResult result;
    result.workspace_bytes = workspace_bytes();
    Container container;
    result.error = detail::read_container(detail::bytes_of(encoded), ContainerReading::decoder, container);
    if (result.error != CngError::none) {
        return result;
    }
    Shape mosaic = detail::mosaic_shape(container.shape);
    result.image = detail::image_of(mosaic);
    if (container.mosaic.empty()) {
        result.error = CngError::mosaic_unavailable;
        return result;
    }
    result.error = detail::check_destination(mosaic, encoded, samples, row_bytes);
    if (result.error != CngError::none) {
        return result;
    }
    result.error = prepare(storage_, container);
    if (result.error == CngError::none && !detail::reserve_tile((*storage_).workspaces[0], mosaic)) {
        result.error = CngError::allocation_failed;
    }
    result.workspace_bytes = workspace_bytes();
    if (result.error != CngError::none) {
        return result;
    }
    std::uint8_t* destination = reinterpret_cast<std::uint8_t*>(samples.data());
    bool decoded = detail::decode_tile((*storage_).tables, container.mosaic, mosaic, (*storage_).workspaces[0], destination,
                                       static_cast<std::size_t>(row_bytes));
    result.error = decoded ? CngError::none : CngError::invalid_segment;
    return result;
}

CngSalvageResult CngDecoder::decode_salvage(std::span<const std::byte> encoded, std::span<std::byte> samples, std::uint64_t row_bytes,
                                            std::span<std::uint8_t> segment_intact) noexcept {
    CngSalvageResult result;
    result.workspace_bytes = workspace_bytes();
    std::span<const std::uint8_t> file = detail::bytes_of(encoded);
    Container container;
    result.error = detail::read_container(file, ContainerReading::salvage, container);
    if (result.error != CngError::none) {
        return result;
    }
    Shape shape = container.shape;
    result.image = detail::image_of(shape);
    result.segments = container.segments;
    result.error = detail::check_destination(shape, encoded, samples, row_bytes);
    if (result.error == CngError::none && segment_intact.size() < container.segments) {
        result.error = CngError::insufficient_storage;
    }
    if (result.error != CngError::none) {
        return result;
    }
    result.error = prepare(storage_, container);
    Shape mosaic = detail::mosaic_shape(shape);
    std::size_t mosaic_row = static_cast<std::size_t>(mosaic.width) * mosaic.channels;
    std::size_t mosaic_bytes = mosaic_row * mosaic.height;
    Shape segment{std::min(detail::segment_columns, shape.width), std::min(detail::segment_rows, shape.height), shape.channels};
    std::size_t segment_bytes = static_cast<std::size_t>(segment.width) * segment.height * segment.channels;
    if (result.error == CngError::none) {
        CngDecoderStorage& storage = *storage_;
        bool reserved = detail::reserve_tile(storage.workspaces[0], mosaic) && detail::reserve_tile(storage.workspaces[0], segment)
            && detail::grow(storage.means, mosaic_bytes) && detail::grow(storage.scratch, segment_bytes);
        result.error = reserved ? CngError::none : CngError::allocation_failed;
    }
    result.workspace_bytes = workspace_bytes();
    if (result.error != CngError::none) {
        return result;
    }
    CngDecoderStorage& storage = *storage_;
    // The fill: the mosaic's cell colours, or 128 without a mosaic.
    std::fill_n(storage.means.begin(), mosaic_bytes, std::uint8_t{128});
    if (!container.mosaic.empty()) {
        result.mosaic_used = detail::decode_tile(storage.tables, container.mosaic, mosaic, storage.workspaces[0], storage.means.data(),
                                                 mosaic_row);
        if (!result.mosaic_used) {
            std::fill_n(storage.means.begin(), mosaic_bytes, std::uint8_t{128});
        }
    }
    std::uint8_t* destination = reinterpret_cast<std::uint8_t*>(samples.data());
    std::size_t stride = static_cast<std::size_t>(row_bytes);
    std::size_t channels = shape.channels;
    for (std::uint32_t y = 0; y < shape.height; ++y) {
        const std::uint8_t* cells = storage.means.data() + static_cast<std::size_t>(y / detail::cell) * mosaic_row;
        std::uint8_t* row = destination + static_cast<std::size_t>(y) * stride;
        for (std::uint32_t x = 0; x < shape.width; ++x) {
            const std::uint8_t* colour = cells + static_cast<std::size_t>(x / detail::cell) * channels;
            std::copy_n(colour, channels, row + static_cast<std::size_t>(x) * channels);
        }
    }
    std::size_t offset = container.payload_begin;
    for (std::uint64_t index = 0; index < container.segments; ++index) {
        std::size_t length = detail::payload_length(container, index);
        std::span<const std::uint8_t> payload = file.subspan(offset, length);
        offset += length;
        segment_intact[static_cast<std::size_t>(index)] = 0;
        detail::SegmentRectangle rectangle = detail::segment_rectangle(shape, index);
        std::size_t tile_row = static_cast<std::size_t>(rectangle.shape.width) * channels;
        bool intact = detail::crc_of(payload) == detail::payload_crc(container, index)
            && detail::decode_tile(storage.tables, payload, rectangle.shape, storage.workspaces[0], storage.scratch.data(), tile_row);
        if (!intact) {
            ++result.segments_lost;
            continue;
        }
        for (std::uint32_t y = 0; y < rectangle.shape.height; ++y) {
            const std::uint8_t* from = storage.scratch.data() + static_cast<std::size_t>(y) * tile_row;
            std::uint8_t* to = destination + static_cast<std::size_t>(rectangle.y + y) * stride + static_cast<std::size_t>(rectangle.x) * channels;
            std::copy_n(from, tile_row, to);
        }
        segment_intact[static_cast<std::size_t>(index)] = 1;
    }
    result.error = CngError::none;
    return result;
}

} // namespace stx_cng
