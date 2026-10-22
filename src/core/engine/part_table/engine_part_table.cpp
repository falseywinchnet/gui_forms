#include "gui_forms/engine/part_table/engine_part_table.hpp"

#include "gui_forms/window/window.hpp"

#include <cmath>
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

// Whether two values of an input are the same, an input not published
// (NaN) being the same as another not published.
bool same_input(double left, double right) noexcept {
    if (left == right) return true;
    const bool both_unpublished = std::isnan(left) && std::isnan(right);
    return both_unpublished;
}

// Whether the inputs the primitive reads have the same values in both
// targets.
bool same_read_inputs(const EnginePrimitive& primitive, const PrimitiveTarget& left,
                      const PrimitiveTarget& right) noexcept {
    for (const EngineInput input : primitive.descriptor().inputs_read) {
        const std::size_t index = static_cast<std::size_t>(input);
        if (!same_input(left.inputs[index], right.inputs[index])) return false;
    }
    return true;
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

PartMotion::PartMotion(Window& window, PartRecordHandle record) noexcept : window_(&window), record_(record) {}

bool PartMotion::moving() const noexcept {
    for (const PartTransitionChannel& channel : channels) {
        if (channel.value && !(*channel.value).settled()) return true;
    }
    return false;
}

void PartMotion::on_step(const MotionStep& step) {
    (*window_).engine_part_moved(record_, step.settled);
}

EnginePartTable::EnginePartTable(const std::size_t item_record_cap) noexcept
    : item_record_cap_(item_record_cap == 0U ? 1U : item_record_cap) {}

void EnginePartTable::link_newest(const std::uint32_t slot) noexcept {
    RecordSlot& linked = slots_[slot];
    linked.older = newest_item_;
    linked.newer = no_slot;
    if (newest_item_ != no_slot) slots_[newest_item_].newer = slot;
    newest_item_ = slot;
    if (oldest_item_ == no_slot) oldest_item_ = slot;
}

void EnginePartTable::unlink(const std::uint32_t slot) noexcept {
    RecordSlot& unlinked = slots_[slot];
    if (unlinked.older != no_slot) {
        slots_[unlinked.older].newer = unlinked.newer;
    } else {
        oldest_item_ = unlinked.newer;
    }
    if (unlinked.newer != no_slot) {
        slots_[unlinked.newer].older = unlinked.older;
    } else {
        newest_item_ = unlinked.older;
    }
    unlinked.older = no_slot;
    unlinked.newer = no_slot;
}

void EnginePartTable::release_slot(const std::uint32_t slot) noexcept {
    RecordSlot& released = slots_[slot];
    drop_raster(*released.record);
    count_readers((*released.record).inputs_read, 0U);
    if (released.item) {
        unlink(slot);
        released.item = false;
        item_records_ = item_records_ - 1U;
    }
    index_.erase(released.lookup);
    released.record.reset();
    released.lookup.clear();
    released.generation = released.generation + 1U;
    // A generation that wraps to 0 would match default handles; skip it.
    if (released.generation == 0U) released.generation = 1U;
    // free_slots_ holds fewer entries than slots_, and acquire() reserves its
    // capacity whenever slots_ grows, so this cannot allocate.
    free_slots_.push_back(slot);
    live_ = live_ - 1U;
}

std::size_t EnginePartTable::evict_items(const std::uint32_t keep) noexcept {
    std::size_t evicted = 0U;
    std::uint32_t candidate = oldest_item_;
    while (item_records_ > item_record_cap_ && candidate != no_slot) {
        const std::uint32_t next = slots_[candidate].newer;
        const bool unsettled = (*slots_[candidate].record).motion_unsettled;
        if (candidate != keep && !unsettled) {
            release_slot(candidate);
            evicted = evicted + 1U;
        }
        candidate = next;
    }
    return evicted;
}

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
        if (slots_[slot].item && newest_item_ != slot) {
            unlink(slot);
            link_newest(slot);
        }
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
    const bool item = !key.item.empty() || key.item_index.has_value();
    std::size_t evicted = 0U;
    if (item) {
        target.item = true;
        item_records_ = item_records_ + 1U;
        link_newest(slot);
        evicted = evict_items(slot);
    }
    const PartRecordAcquisition acquired{PartRecordHandle{slot, slots_[slot].generation}, true, evicted};
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
        const RecordSlot& slot = slots_[index];
        if (!slot.record || (*slot.record).control != control_id) continue;
        release_slot(static_cast<std::uint32_t>(index));
        released = released + 1U;
    }
    return released;
}

void EnginePartTable::drop_raster(EnginePartRecord& record) noexcept {
    if (!record.cached || !(*record.cached).raster) return;
    CachedPartRaster& cached = *record.cached;
    statistics_.bytes = statistics_.bytes - (*cached.raster).pixels().size();
    statistics_.rasters = statistics_.rasters - 1U;
    cached.raster.reset();
    cached.primitive = nullptr;
}

void EnginePartTable::make_room(const std::size_t incoming, const EnginePartRecord* const keep) noexcept {
    while (statistics_.bytes + incoming > raster_cache_budget_bytes) {
        // The least recently painted record holding a raster, other than
        // `keep`; ties go to the lower slot, so eviction order is fixed.
        EnginePartRecord* oldest = nullptr;
        for (RecordSlot& slot : slots_) {
            if (!slot.record || &*slot.record == keep) continue;
            if (!(*slot.record).cached || !(*(*slot.record).cached).raster) continue;
            if (oldest == nullptr || (*slot.record).last_painted < (*oldest).last_painted) oldest = &*slot.record;
        }
        if (oldest == nullptr) return;
        drop_raster(*oldest);
        statistics_.evictions = statistics_.evictions + 1U;
    }
}

bool EnginePartTable::raster_current(const EnginePartRecord& record, const PartRasterRequest& request) const noexcept {
    if (!record.cached) return false;
    const CachedPartRaster& cached = *record.cached;
    if (!cached.raster || cached.primitive != request.primitive) return false;
    if (cached.raster_generation != record.generation) return false;
    if (cached.raster_table_generation != raster_table_generation_) return false;
    if (!(cached.extent == request.extent)) return false;
    if (!(cached.target.logical_size == request.target.logical_size)) return false;
    if (!(cached.target.scale == request.target.scale)) return false;
    const AnatomyStateSet read = read_states_of(*request.primitive, request.target);
    if (!(cached.target.states == read)) return false;
    if (!same_read_inputs(*request.primitive, cached.target, request.target)) return false;
    const bool same = same_parameters(cached.parameters, request.parameters);
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
        return PartRasterResult{(*part.cached).raster, PartRasterStatus::cached};
    }
    // The cache record is made once per part, before anything is drawn, so
    // that an allocation failure leaves the record as it was.
    if (!part.cached) part.cached = std::make_unique<CachedPartRaster>();
    CachedPartRaster& cached = *part.cached;

    // Choose the storage. The cached raster is rewritten only when the cache
    // holds its one reference: a recorded command still showing it keeps
    // its pixels.
    const EnginePrimitive& primitive = *request.primitive;
    const std::size_t bytes = raster_bytes(request.extent);
    const bool reusable = cached.raster && cached.extent == request.extent &&
                          (*cached.raster).scale() == request.target.scale && cached.raster.use_count() == 1;
    std::shared_ptr<PartRaster> storage{};
    PartRasterStatus status = PartRasterStatus::reused_storage;
    if (reusable) {
        storage = cached.raster;
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
        cached.raster = storage;
        statistics_.bytes = statistics_.bytes + bytes;
        statistics_.rasters = statistics_.rasters + 1U;
        statistics_.storage_allocations = statistics_.storage_allocations + 1U;
    } else {
        statistics_.storage_reuses = statistics_.storage_reuses + 1U;
    }
    cached.primitive = request.primitive;
    cached.parameters = request.parameters;
    cached.target = request.target;
    cached.target.states = read_states_of(primitive, request.target);
    cached.extent = request.extent;
    cached.raster_generation = part.generation;
    cached.raster_table_generation = raster_table_generation_;
    return PartRasterResult{storage, status};
}

void EnginePartTable::count_readers(const std::uint64_t old_inputs, const std::uint64_t inputs) noexcept {
    if (old_inputs == inputs) return;
    for (std::size_t position = 0; position < maximum_expression_inputs; ++position) {
        const std::uint64_t bit = std::uint64_t{1} << position;
        if ((old_inputs & bit) != 0U) readers_[position] = readers_[position] - 1U;
        if ((inputs & bit) != 0U) readers_[position] = readers_[position] + 1U;
    }
}

void EnginePartTable::note_paint(const PartRecordHandle handle, const std::uint64_t inputs_read,
                                 std::string_view painted_by, const Rect extent) {
    EnginePartRecord* const found = record(handle);
    if (found == nullptr) return;
    EnginePartRecord& part = *found;
    // The copy first: it may allocate, and nothing has changed yet.
    if (part.painted_by != painted_by) part.painted_by.assign(painted_by.data(), painted_by.size());
    part.extent = extent;
    count_readers(part.inputs_read, inputs_read);
    part.inputs_read = inputs_read;
}

bool EnginePartTable::reads_any(const std::uint64_t inputs) const noexcept {
    for (std::size_t position = 0; position < maximum_expression_inputs; ++position) {
        const std::uint64_t bit = std::uint64_t{1} << position;
        if ((inputs & bit) != 0U && readers_[position] != 0U) return true;
    }
    return false;
}

const EnginePartRecord* EnginePartTable::record_in_slot(const std::size_t slot) const noexcept {
    if (slot >= slots_.size() || !slots_[slot].record) return nullptr;
    const EnginePartRecord* const live = &*slots_[slot].record;
    return live;
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
