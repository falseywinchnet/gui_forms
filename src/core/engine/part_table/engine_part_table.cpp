#include "gui_forms/engine/part_table/engine_part_table.hpp"

#include <limits>
#include <new>
#include <utility>

namespace gui_forms {
namespace {

// One field of a lookup text: its byte length, a colon, its bytes.
void append_field(std::string& text, std::string_view field) {
    text += std::to_string(field.size());
    text += ':';
    text += field;
}

// The target's states that the primitive declares it reads: a change to
// any other state leaves its raster current.
AnatomyStateSet read_states_of(const EnginePrimitive& primitive, const PrimitiveTarget& target) noexcept {
    AnatomyStateSet read{};
    for (const AnatomyState state : primitive.descriptor().states_read) {
        if (target.states.contains(state)) read.insert(state);
    }
    return read;
}

bool same_parameters(const PrimitiveParameterBlock& left, const PrimitiveParameterBlock& right) noexcept {
    if (left.count != right.count) return false;
    for (std::size_t index = 0U; index < left.count; ++index) {
        if (!(left.values[index] == right.values[index])) return false;
    }
    return true;
}

// The bytes of a raster of `extent`. Each side is at most
// maximum_part_raster_side, so the product cannot overflow.
std::size_t raster_bytes(DeviceExtent extent) noexcept {
    const std::size_t bytes = static_cast<std::size_t>(extent.width) * static_cast<std::size_t>(extent.height) *
                              part_raster_bytes_per_pixel;
    return bytes;
}

} // namespace

void EnginePartTable::write_lookup_text(std::string& text, std::string_view control_id, const PartKey& key) {
    text.clear();
    append_field(text, control_id);
    append_field(text, key.part);
    append_field(text, key.item);
    // The index is identity only for the fallback; an item with a stable ID
    // keeps its record wherever it moves.
    if (key.index_fallback()) {
        text += '#';
        text += std::to_string(*key.item_index);
    }
}

PartRecordAcquisition EnginePartTable::acquire(std::string_view control_id, const PartKey& key) {
    write_lookup_text(lookup_scratch_, control_id, key);
    const std::string& lookup = lookup_scratch_;
    const std::unordered_map<std::string, std::uint32_t>::const_iterator found = index_.find(lookup);
    if (found != index_.end()) {
        const std::uint32_t slot = (*found).second;
        const PartRecordAcquisition existing{PartRecordHandle{slot, slots_[slot].generation}, false};
        return existing;
    }

    // Prepare everything that can fail before the table changes.
    EnginePartRecord made{};
    made.control = std::string(control_id);
    made.part = std::string(key.part);
    made.item = std::string(key.item);
    if (key.index_fallback()) made.item_index = key.item_index;
    std::string owned_lookup = lookup;
    const bool reuse = !free_slots_.empty();
    if (!reuse && slots_.size() >= static_cast<std::size_t>(std::numeric_limits<std::uint32_t>::max())) {
        throw std::bad_alloc();
    }
    if (!reuse) {
        // Room for the new slot, and for its entry in free_slots_ when it is
        // released, so that release_control never allocates.
        slots_.reserve(slots_.size() + 1U);
        free_slots_.reserve(slots_.size() + 1U);
    }
    const std::uint32_t slot = reuse ? free_slots_.back() : static_cast<std::uint32_t>(slots_.size());
    index_.emplace(owned_lookup, slot);

    // Commit: nothing below allocates.
    if (reuse) {
        free_slots_.pop_back();
    } else {
        slots_.emplace_back();
    }
    RecordSlot& target = slots_[slot];
    target.record = std::move(made);
    target.lookup = std::move(owned_lookup);
    live_ = live_ + 1U;
    const PartRecordAcquisition acquired{PartRecordHandle{slot, target.generation}, true};
    return acquired;
}

std::optional<PartRecordHandle> EnginePartTable::find(std::string_view control_id, const PartKey& key) const {
    std::string lookup;
    write_lookup_text(lookup, control_id, key);
    const std::unordered_map<std::string, std::uint32_t>::const_iterator found = index_.find(lookup);
    if (found == index_.end()) return std::nullopt;
    const std::uint32_t slot = (*found).second;
    const PartRecordHandle handle{slot, slots_[slot].generation};
    return handle;
}

EnginePartRecord* EnginePartTable::record(PartRecordHandle handle) noexcept {
    if (handle.slot >= slots_.size()) return nullptr;
    RecordSlot& slot = slots_[handle.slot];
    if (slot.generation != handle.generation || !slot.record) return nullptr;
    EnginePartRecord* const live = &*slot.record;
    return live;
}

const EnginePartRecord* EnginePartTable::record(PartRecordHandle handle) const noexcept {
    if (handle.slot >= slots_.size()) return nullptr;
    const RecordSlot& slot = slots_[handle.slot];
    if (slot.generation != handle.generation || !slot.record) return nullptr;
    const EnginePartRecord* const live = &*slot.record;
    return live;
}

std::size_t EnginePartTable::release_control(std::string_view control_id) noexcept {
    std::size_t released = 0U;
    for (std::size_t index = 0U; index < slots_.size(); ++index) {
        RecordSlot& slot = slots_[index];
        if (!slot.record || (*slot.record).control != control_id) continue;
        drop_raster(*slot.record);
        index_.erase(slot.lookup);
        slot.record.reset();
        slot.lookup.clear();
        slot.generation = slot.generation + 1U;
        // A generation that wraps to 0 would match default handles; skip it.
        if (slot.generation == 0U) slot.generation = 1U;
        // free_slots_ holds fewer entries than slots_, and acquire() reserves
        // its capacity whenever slots_ grows, so this cannot allocate.
        free_slots_.push_back(static_cast<std::uint32_t>(index));
        live_ = live_ - 1U;
        released = released + 1U;
    }
    return released;
}

void EnginePartTable::drop_raster(EnginePartRecord& record) noexcept {
    if (!record.raster) return;
    statistics_.bytes = statistics_.bytes - (*record.raster).pixels().size();
    statistics_.rasters = statistics_.rasters - 1U;
    record.raster.reset();
    record.raster_primitive = nullptr;
}

void EnginePartTable::make_room(const std::size_t incoming, const EnginePartRecord* const keep) noexcept {
    while (statistics_.bytes + incoming > raster_cache_budget_bytes) {
        // The least recently painted record holding a raster, other than
        // `keep`; ties go to the lower slot, so eviction order is fixed.
        EnginePartRecord* oldest = nullptr;
        for (RecordSlot& slot : slots_) {
            if (!slot.record || !(*slot.record).raster || &*slot.record == keep) continue;
            if (oldest == nullptr || (*slot.record).last_painted < (*oldest).last_painted) oldest = &*slot.record;
        }
        if (oldest == nullptr) return;
        drop_raster(*oldest);
        statistics_.evictions = statistics_.evictions + 1U;
    }
}

bool EnginePartTable::raster_current(const EnginePartRecord& record, const PartRasterRequest& request) const noexcept {
    if (!record.raster || record.raster_primitive != request.primitive) return false;
    if (record.raster_generation != record.generation) return false;
    if (record.raster_table_generation != raster_table_generation_) return false;
    if (!(record.raster_extent == request.extent)) return false;
    if (!(record.raster_target.logical_size == request.target.logical_size)) return false;
    if (!(record.raster_target.scale == request.target.scale)) return false;
    const AnatomyStateSet read = read_states_of(*request.primitive, request.target);
    if (!(record.raster_target.states == read)) return false;
    const bool same = same_parameters(record.raster_parameters, request.parameters);
    return same;
}

PartRasterResult EnginePartTable::raster_for(const PartRecordHandle handle, const PartRasterRequest& request,
                                             const std::uint64_t paint_sequence) {
    EnginePartRecord* const found = record(handle);
    if (found == nullptr) return PartRasterResult{nullptr, PartRasterStatus::stale_record};
    if (request.primitive == nullptr) return PartRasterResult{nullptr, PartRasterStatus::refused};
    EnginePartRecord& part = *found;
    part.last_painted = paint_sequence;
    if (raster_current(part, request)) {
        statistics_.hits = statistics_.hits + 1U;
        return PartRasterResult{part.raster, PartRasterStatus::cached};
    }

    // Choose the storage. The cached raster is rewritten only when the cache
    // holds its one reference: a recorded command still showing it keeps
    // its pixels.
    const EnginePrimitive& primitive = *request.primitive;
    const std::size_t bytes = raster_bytes(request.extent);
    const bool reusable = part.raster && part.raster_extent == request.extent &&
                          (*part.raster).scale() == request.target.scale && part.raster.use_count() == 1;
    std::shared_ptr<PartRaster> storage{};
    PartRasterStatus status = PartRasterStatus::reused_storage;
    if (reusable) {
        storage = part.raster;
    } else {
        storage = PartRaster::create(request.extent, request.target.scale);
        if (!storage) return PartRasterResult{nullptr, PartRasterStatus::refused};
        status = bytes > raster_cache_budget_bytes ? PartRasterStatus::uncached : PartRasterStatus::new_storage;
    }

    // Draw, preparing again when the record's preparation is stale.
    const PartRasterView destination = (*storage).view();
    RasterOutcome outcome = primitive.rasterize(request.parameters, request.target, part.preparation, destination);
    if (outcome == RasterOutcome::stale_preparation) {
        PrimitivePreparation prepared{};
        if (primitive.prepare(request.parameters, request.target, prepared) != PreparationOutcome::prepared) {
            return PartRasterResult{nullptr, PartRasterStatus::refused};
        }
        part.preparation = prepared;
        outcome = primitive.rasterize(request.parameters, request.target, part.preparation, destination);
    }
    if (outcome != RasterOutcome::painted && outcome != RasterOutcome::empty) {
        return PartRasterResult{nullptr, PartRasterStatus::refused};
    }

    // Commit. The part's former raster no longer shows it either way.
    if (status == PartRasterStatus::uncached) {
        drop_raster(part);
        statistics_.uncached = statistics_.uncached + 1U;
        return PartRasterResult{storage, status};
    }
    if (status == PartRasterStatus::new_storage) {
        drop_raster(part);
        make_room(bytes, &part);
        part.raster = storage;
        statistics_.bytes = statistics_.bytes + bytes;
        statistics_.rasters = statistics_.rasters + 1U;
        statistics_.storage_allocations = statistics_.storage_allocations + 1U;
    } else {
        statistics_.storage_reuses = statistics_.storage_reuses + 1U;
    }
    part.raster_primitive = request.primitive;
    part.raster_parameters = request.parameters;
    part.raster_target = request.target;
    part.raster_target.states = read_states_of(primitive, request.target);
    part.raster_extent = request.extent;
    part.raster_generation = part.generation;
    part.raster_table_generation = raster_table_generation_;
    return PartRasterResult{storage, status};
}

void EnginePartTable::invalidate_raster(const PartRecordHandle handle) noexcept {
    EnginePartRecord* const found = record(handle);
    if (found == nullptr) return;
    (*found).generation = (*found).generation + 1U;
}

void EnginePartTable::invalidate_rasters() noexcept {
    raster_table_generation_ = raster_table_generation_ + 1U;
}

} // namespace gui_forms
