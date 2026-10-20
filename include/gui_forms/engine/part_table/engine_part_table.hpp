#pragma once

#include "gui_forms/anatomy/types/anatomy_types.hpp"
#include "gui_forms/engine/primitive_registry/primitive_registry.hpp"
#include "gui_forms/engine/types/engine_types.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace gui_forms {

// One window's records of the parts the engine has painted with a key (T2
// plan, EnginePartTable). A record is the engine's memory of one part from
// frame to frame. E2 keeps its last states, its preparation and the paint
// on which it was last drawn; later phases add the time it entered each
// state (E3), its transitions (E7) and its cached raster (E2c).
//
// A part painted without a key has no record and is drawn statically.
//
// Bounds. A part that is not an item's is one of its control's declared
// parts, so those records are bounded by the window's controls and released
// with them. Item parts are bounded by maximum_item_part_records: a
// virtualized control can paint any number of items over time, so beyond
// the cap the least recently painted item record is evicted, as if
// released. A record whose motion is unsettled (motion_unsettled) is never
// evicted; the table then holds more than the cap until its motion settles.
// No control publishes when it unrealizes an item yet, so records are
// released only by eviction and by their control leaving the window.

// Which record a handle names: a slot of the table and the slot's
// generation when the record was made. Releasing a record advances its
// slot's generation, so a handle kept past the release is stale and finds
// nothing, even after the slot is reused.
struct PartRecordHandle final {
    std::uint32_t slot{};
    std::uint32_t generation{};

    friend constexpr bool operator==(PartRecordHandle left, PartRecordHandle right) noexcept {
        return left.slot == right.slot && left.generation == right.generation;
    }
};

// One painted part's record. The identity strings are owned copies of the
// key the part was first painted with.
struct EnginePartRecord final {
    std::string control;
    std::string part;
    std::string item;
    // Set only for the index fallback (PartKey::index_fallback).
    std::optional<std::size_t> item_index{};
    // The states the part was last painted in.
    AnatomyStateSet states{};
    // What the part's primitive derived for its geometry; empty until a
    // primitive draws the part (never, before themes select one in T3).
    PrimitivePreparation preparation{};
    // When the part was last painted: the number of window paints completed
    // before the one that drew it.
    std::uint64_t last_painted{};
    // True while a transition or motion value of the part has not settled
    // (E3, E7): such a record is never evicted. Nothing sets it before E3.
    bool motion_unsettled{};
};

// The most item-part records one window keeps before it evicts the least
// recently painted (see EnginePartTable). A record is 792 bytes on 64-bit
// hosts (its preparation is about half of that); with its slot, its lookup
// text and its index entry it takes about 1 KB, so the cap costs at most
// about 4 MB per window. It covers far more item parts than a window shows
// at once: a record evicted while its part is still on screen is only made
// again on the next paint. Each eviction is O(1).
inline constexpr std::size_t maximum_item_part_records = 4096;

// What acquire() did.
struct PartRecordAcquisition final {
    PartRecordHandle handle{};
    // A new record was made for the key.
    bool created{};
    // Item records evicted to keep within the cap; their handles are stale.
    std::size_t evicted{};
};

// The table. It owns its records. Every member runs on the window's UI
// thread; the table is not shared between windows.
class EnginePartTable final {
public:
    // The table keeps at most `item_record_cap` item-part records (at least
    // 1); the window's table uses maximum_item_part_records.
    explicit EnginePartTable(std::size_t item_record_cap = maximum_item_part_records) noexcept;

    // The record for `control_id` and `key`, made if there is none, and
    // marked the most recently painted item record when it is an item's. A
    // new record reuses a released slot before the table grows; a new item
    // record beyond the cap evicts the least recently painted settled item
    // record (O(1) unless records with unsettled motion are passed over).
    // If an allocation fails, std::bad_alloc propagates and the table is
    // unchanged.
    [[nodiscard]] PartRecordAcquisition acquire(std::string_view control_id, const PartKey& key);

    // The live record for `control_id` and `key`, if there is one. For
    // inspection: it builds its lookup text in a string of its own.
    [[nodiscard]] std::optional<PartRecordHandle> find(std::string_view control_id, const PartKey& key) const;

    // The record a handle names, or null when the handle is stale (its
    // record was released) or names no slot. The pointer is valid until the
    // next acquire() or release on this table.
    [[nodiscard]] EnginePartRecord* record(PartRecordHandle handle) noexcept;
    [[nodiscard]] const EnginePartRecord* record(PartRecordHandle handle) const noexcept;

    // Releases every record of the control with this stable ID, as when it
    // leaves the window, and returns how many. Their handles become stale.
    // The cost is one pass over the table's slots.
    std::size_t release_control(std::string_view control_id) noexcept;

    // Live records, and those of item parts among them.
    [[nodiscard]] std::size_t record_count() const noexcept { return live_; }
    [[nodiscard]] std::size_t item_record_count() const noexcept { return item_records_; }
    // Record storage allocated, live or free: released and evicted slots are
    // reused before it grows, so it stays near the most records live at once.
    [[nodiscard]] std::size_t slot_count() const noexcept { return slots_.size(); }

private:
    static constexpr std::uint32_t no_slot = 0xFFFFFFFFU;

    struct RecordSlot final {
        std::optional<EnginePartRecord> record{};
        // The record's lookup text, the key of its index_ entry; empty while
        // the slot is free.
        std::string lookup;
        // Starts at 1, so that a default handle (generation 0) is never live.
        std::uint32_t generation{1U};
        // An item record's neighbours in recency order, older and newer, or
        // no_slot at either end; unused for other records.
        bool item{};
        std::uint32_t older{no_slot};
        std::uint32_t newer{no_slot};
    };

    // Recency list of item records: link at the newest end, unlink anywhere.
    void link_newest(std::uint32_t slot) noexcept;
    void unlink(std::uint32_t slot) noexcept;
    // Ends a live record's lifetime: its handles become stale and its slot
    // is free for reuse. Never allocates.
    void release_slot(std::uint32_t slot) noexcept;
    // Evicts least recently painted settled item records, never `keep`,
    // until the cap holds; returns how many.
    std::size_t evict_items(std::uint32_t keep) noexcept;

    // Writes the lookup text for `control_id` and `key` into `text`,
    // replacing its contents. Each field is written as its byte length, a
    // colon and its bytes, so that no two keys share a text whatever bytes
    // their fields hold.
    static void write_lookup_text(std::string& text, std::string_view control_id, const PartKey& key);

    std::vector<RecordSlot> slots_;
    std::vector<std::uint32_t> free_slots_;
    // Lookup text to slot, one entry per live record.
    std::unordered_map<std::string, std::uint32_t> index_;
    // Reused by every lookup, so that a paint of a known part allocates
    // nothing once the string has grown to the longest key.
    std::string lookup_scratch_;
    std::size_t live_{};
    std::size_t item_records_{};
    std::size_t item_record_cap_{};
    std::uint32_t oldest_item_{no_slot};
    std::uint32_t newest_item_{no_slot};
};

} // namespace gui_forms
