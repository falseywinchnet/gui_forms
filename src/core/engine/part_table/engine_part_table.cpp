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

} // namespace

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

} // namespace gui_forms
