#pragma once

#include "gui_forms/anatomy/types/anatomy_types.hpp"
#include "gui_forms/engine/part_raster/part_raster.hpp"
#include "gui_forms/engine/primitive_registry/primitive_registry.hpp"
#include "gui_forms/engine/types/engine_types.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace gui_forms {

// One window's records of the parts the engine has painted with a key (T2
// plan, EnginePartTable). A record is the engine's memory of one part from
// frame to frame. E2 keeps its last states, its preparation, its cached
// raster and the paint on which it was last drawn; later phases add the
// time it entered each state (E3) and its transitions (E7).
//
// A part painted without a key has no record and is drawn statically: no
// cached raster either.
//
// The raster cache (GIVEN F2-1): a record keeps the raster its primitive
// last drew while what drew it is unchanged, and reuses the raster's
// storage when it must draw again at the same size. The table holds at most
// raster_cache_budget_bytes of cached rasters and evicts the least recently
// painted part's raster first to make room. A raster is invalidated by
// generation: invalidate_raster() for one part, invalidate_rasters() for the
// whole table (a theme change). A raster's transparency passes through as
// its own premultiplied alpha when it is composited.

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
    // before the one that drew it. The cache evicts the smallest first.
    std::uint64_t last_painted{};

    // The cached raster, or null. The table owns this reference; recorded
    // commands that composite the raster hold others.
    std::shared_ptr<PartRaster> raster{};
    // What drew `raster`: the primitive, every parameter (animated ones
    // included, since they change the pixels), the target with its states
    // restricted to those the primitive reads, and the extent.
    const EnginePrimitive* raster_primitive{};
    PrimitiveParameterBlock raster_parameters{};
    PrimitiveTarget raster_target{};
    DeviceExtent raster_extent{};
    // The record's and the table's raster generations when `raster` was
    // drawn; invalidation advances the current ones.
    std::uint64_t raster_generation{};
    std::uint64_t raster_table_generation{};
    // The record's current raster generation.
    std::uint64_t generation{1U};
};

// What a part needs drawn: the primitive a rule selected, its parameters,
// the part's target, and the destination extent in device pixels (normally
// the logical size times the scale, rounded up).
struct PartRasterRequest final {
    const EnginePrimitive* primitive{};
    PrimitiveParameterBlock parameters{};
    PrimitiveTarget target{};
    DeviceExtent extent{};
};

enum class PartRasterStatus : std::uint8_t {
    // The cached raster is still current; nothing was drawn.
    cached,
    // The primitive drew the part into the cached raster's own storage.
    reused_storage,
    // The primitive drew the part into new storage, now cached.
    new_storage,
    // The primitive drew the part, but the raster alone exceeds the cache
    // budget, so it is returned and not kept.
    uncached,
    // Nothing was drawn: the handle is stale.
    stale_record,
    // Nothing was drawn: no primitive, or the primitive refused the target,
    // the parameters or the extent. The cache is unchanged.
    refused,
};

struct PartRasterResult final {
    // The part's raster, or null when status is stale_record or refused.
    std::shared_ptr<const PartRaster> raster{};
    PartRasterStatus status{PartRasterStatus::refused};
};

// The raster cache's counters, for instrumentation.
struct RasterCacheStatistics final {
    // Rasters held and their bytes now.
    std::size_t rasters{};
    std::size_t bytes{};
    // Requests answered from the cache, drawn into reused storage, drawn
    // into new storage, and drawn without caching.
    std::uint64_t hits{};
    std::uint64_t storage_reuses{};
    std::uint64_t storage_allocations{};
    std::uint64_t uncached{};
    // Rasters evicted to keep within the budget.
    std::uint64_t evictions{};
};

// The most bytes of cached rasters one window keeps (GIVEN F2-1: 8 MB).
inline constexpr std::size_t raster_cache_budget_bytes = 8U * 1024U * 1024U;

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
    // leaves the window, and returns how many. Their handles become stale
    // and their cached rasters are released. The cost is one pass over the
    // table's slots.
    std::size_t release_control(std::string_view control_id) noexcept;

    // Live records.
    [[nodiscard]] std::size_t record_count() const noexcept { return live_; }

    // The part's raster for `request`, painted on window paint
    // `paint_sequence` (the record's new last_painted).
    // - The cached raster is returned unchanged while its primitive,
    //   parameters, target, read states and extent are the request's and
    //   neither generation has advanced.
    // - Otherwise the primitive draws the part. It draws into the cached
    //   raster's storage when the extent and scale are unchanged and no
    //   recorded command still holds that raster (a held raster is never
    //   rewritten, since a recording shows it), and into new storage
    //   otherwise.
    // - New storage is made room for by evicting other parts' rasters,
    //   least recently painted first, until the cache fits its budget. A
    //   raster larger than the whole budget is drawn and returned but not
    //   kept.
    // - The record's preparation is remade when the primitive refuses it as
    //   stale.
    // If the primitive refuses, nothing is evicted and the record keeps its
    // cached raster. Allocation failure propagates as std::bad_alloc, with
    // the record unchanged.
    [[nodiscard]] PartRasterResult raster_for(PartRecordHandle handle, const PartRasterRequest& request,
                                              std::uint64_t paint_sequence);

    // Makes one part's cached raster stale; the next raster_for draws it
    // again. A stale handle does nothing.
    void invalidate_raster(PartRecordHandle handle) noexcept;
    // Makes every cached raster stale, as when the theme changes.
    void invalidate_rasters() noexcept;

    [[nodiscard]] RasterCacheStatistics cache_statistics() const noexcept { return statistics_; }

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

    // Drops a record's cached raster from the cache and its counts.
    void drop_raster(EnginePartRecord& record) noexcept;
    // Evicts the least recently painted cached rasters, never `keep`'s,
    // until `incoming` more bytes fit the budget.
    void make_room(std::size_t incoming, const EnginePartRecord* keep) noexcept;
    // Whether the record's cached raster answers `request` now.
    [[nodiscard]] bool raster_current(const EnginePartRecord& record, const PartRasterRequest& request) const noexcept;

    std::vector<RecordSlot> slots_;
    std::vector<std::uint32_t> free_slots_;
    // Lookup text to slot, one entry per live record.
    std::unordered_map<std::string, std::uint32_t> index_;
    // Reused by every lookup, so that a paint of a known part allocates
    // nothing once the string has grown to the longest key.
    std::string lookup_scratch_;
    std::size_t live_{};
    // Advanced by invalidate_rasters; starts at 1 so a record that never
    // cached a raster (raster_table_generation 0) is never current.
    std::uint64_t raster_table_generation_{1U};
    RasterCacheStatistics statistics_{};
};

} // namespace gui_forms
