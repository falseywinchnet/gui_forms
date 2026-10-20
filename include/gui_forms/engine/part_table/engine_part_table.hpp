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
};

// What acquire() did.
struct PartRecordAcquisition final {
    PartRecordHandle handle{};
    // A new record was made for the key.
    bool created{};
};

// The table. It owns its records. Every member runs on the window's UI
// thread; the table is not shared between windows.
class EnginePartTable final {
public:
    // The record for `control_id` and `key`, made if there is none. A new
    // record reuses a released slot before the table grows. If an
    // allocation fails, std::bad_alloc propagates and the table is
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

    // Live records.
    [[nodiscard]] std::size_t record_count() const noexcept { return live_; }

private:
    struct RecordSlot final {
        std::optional<EnginePartRecord> record{};
        // The record's lookup text, the key of its index_ entry; empty while
        // the slot is free.
        std::string lookup;
        // Starts at 1, so that a default handle (generation 0) is never live.
        std::uint32_t generation{1U};
    };

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
};

} // namespace gui_forms
