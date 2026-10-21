#pragma once

#include "gui_forms/anatomy/types/anatomy_types.hpp"
#include "gui_forms/engine/engine_expression/engine_expression.hpp"
#include "gui_forms/engine/motion_value/motion_value.hpp"
#include "gui_forms/engine/part_raster/part_raster.hpp"
#include "gui_forms/engine/primitive_registry/primitive_registry.hpp"
#include "gui_forms/engine/theme_rules/theme_rules.hpp"
#include "gui_forms/engine/types/engine_types.hpp"
#include "gui_forms/scheduler/types/scheduler_types.hpp"
#include "gui_forms/types/geometry/geometry.hpp"

#include <array>
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
// Bounds. A part that is not an item's is one of its control's declared
// parts, so those records are bounded by the window's controls and released
// with them. Item parts are bounded by maximum_item_part_records: a
// virtualized control can paint any number of items over time, so beyond
// the cap the least recently painted item record is evicted, as if
// released. A record whose motion is unsettled (motion_unsettled) is never
// evicted; the table then holds more than the cap until its motion settles.
// No control publishes when it unrealizes an item yet, so records are
// released only by eviction and by their control leaving the window.
//
// The raster cache (GIVEN F2-1): a record keeps the raster its primitive
// last drew while what drew it is unchanged, and reuses the raster's
// storage when it must draw again at the same size. The table holds at most
// raster_cache_budget_bytes of cached rasters and evicts the least recently
// painted part's raster first to make room. A raster is invalidated by
// generation: invalidate_raster() for one part, invalidate_rasters() for the
// whole table (a theme change). A raster's transparency passes through as
// its own premultiplied alpha when it is composited. Releasing or evicting a
// record releases its raster.

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

// A part's cached raster and what drew it: the primitive, every parameter
// (animated ones included, since they change the pixels), the target with
// its states restricted to those the primitive reads, and the extent. The
// record owns it; `raster` is the cache's reference, and recorded commands
// that composite the raster hold others.
struct CachedPartRaster final {
    std::shared_ptr<PartRaster> raster{};
    const EnginePrimitive* primitive{};
    PrimitiveParameterBlock parameters{};
    PrimitiveTarget target{};
    DeviceExtent extent{};
    // The record's and the table's raster generations when `raster` was
    // drawn; invalidation advances the current ones.
    std::uint64_t raster_generation{};
    std::uint64_t raster_table_generation{};
};

class Window;

// One property's transition on a part (E7): opacity or the parameter at
// `parameter`, the state of its machine the part was last painted in (none
// when the part's states matched none), and the property's motion value,
// made the first time the part is painted with the transition.
struct PartTransitionChannel final {
    bool opacity{};
    std::size_t parameter{};
    std::optional<std::size_t> state{};
    std::optional<MotionValue> value{};
};

// A part's running transitions (E7; T2 plan, "How motion is driven"). The
// part's record owns it through a pointer, so that its motion values, which
// never move, keep their addresses however the table's storage grows. Each
// value is owned by the part's control (MotionValue's owner), runs on the
// window's MotionClock, and reports each step to on_step, which repaints
// the part's extent on the control that recorded it. The record is released
// with its control, which releases the values from the clock.
class PartMotion final {
public:
    PartMotion(Window& window, PartRecordHandle record) noexcept;
    PartMotion(const PartMotion&) = delete;
    PartMotion& operator=(const PartMotion&) = delete;

    // Whether any of the part's values has not settled.
    [[nodiscard]] bool moving() const noexcept;
    // A value's step, from the clock: the part's extent repaints.
    void on_step(const MotionStep& step);

    std::array<PartTransitionChannel, maximum_part_transitions> channels{};

private:
    // Observed: the window owns the table that owns this record.
    Window* window_;
    PartRecordHandle record_;
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
    // True while a transition or motion value of the part has not settled
    // (E3, E7): such a record is never evicted. Nothing sets it before E3.
    bool motion_unsettled{};

    // What the part's last paint read, and where it was drawn (E6). An
    // input change damages exactly this extent of this control.
    // - `inputs_read`: the inputs its primitive and its expressions read,
    //   as expression input bits (expression_input_bit); zero for a part
    //   the built-in recipe draws. Set only through
    //   EnginePartTable::note_paint, which keeps the table's reader counts.
    // - `painted_by`: the stable ID of the control whose display chunk
    //   recorded the part. It is the control painting the part, or one
    //   that paints on its behalf, such as a docking site's drag layer.
    // - `extent`: the part's bounds and its primitive's outsets, in that
    //   control's local coordinates.
    std::uint64_t inputs_read{};
    std::string painted_by{};
    Rect extent{};
    // When the part entered the states it was last painted in, on the
    // window's motion clock; time_in_state counts from it.
    FrameTime state_entered{};

    // The part's transitions (E7), made the first time it is painted with
    // one; null for a part without any.
    std::unique_ptr<PartMotion> motion{};
    // Whether the part's last paint had an ambient effect, and whether that
    // effect was refused for its area (E7). An ambient part repaints when
    // ambient motion resumes; a refusal is counted once, when it begins.
    bool ambient{};
    bool ambient_refused{};

    // The cached raster and what drew it, made the first time a primitive
    // draws the part, so that a record no primitive draws stays small.
    std::unique_ptr<CachedPartRaster> cached{};
    // The record's current raster generation; invalidate_raster advances it.
    std::uint64_t generation{1U};
};

// The most item-part records one window keeps before it evicts the least
// recently painted (see EnginePartTable). A record is about 800 bytes on
// 64-bit hosts (its preparation is about half of that); with its slot, its
// lookup text and its index entry it takes about 1 KB, so the cap costs at
// most about 4 MB per window. A part a primitive draws also has its
// CachedPartRaster (about 450 bytes) and its raster, which the raster
// cache's budget bounds. The cap covers far more item parts than a window
// shows at once: a record evicted while its part is still on screen is only
// made again on the next paint. Each eviction is O(1).
inline constexpr std::size_t maximum_item_part_records = 4096;

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
    // leaves the window, and returns how many. Their handles become stale
    // and their cached rasters are released. The cost is one pass over the
    // table's slots.
    std::size_t release_control(std::string_view control_id) noexcept;

    // Live records, and those of item parts among them.
    [[nodiscard]] std::size_t record_count() const noexcept { return live_; }
    [[nodiscard]] std::size_t item_record_count() const noexcept { return item_records_; }
    // Record storage allocated, live or free: released and evicted slots are
    // reused before it grows, so it stays near the most records live at once.
    [[nodiscard]] std::size_t slot_count() const noexcept { return slots_.size(); }

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

    // Records what a part's paint read and where it was drawn (E6; see
    // EnginePartRecord::inputs_read). A stale handle does nothing. If the
    // control's ID cannot be copied, std::bad_alloc propagates and the
    // record is unchanged.
    void note_paint(PartRecordHandle handle, std::uint64_t inputs_read, std::string_view painted_by, Rect extent);
    // Whether any live record reads any input of `inputs`, from the
    // table's reader counts, without a scan.
    [[nodiscard]] bool reads_any(std::uint64_t inputs) const noexcept;
    // The live record in `slot` (below slot_count()), or null for a free
    // slot: for scans over the table. Valid until the next acquire() or
    // release.
    [[nodiscard]] const EnginePartRecord* record_in_slot(std::size_t slot) const noexcept;

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

    // Drops a record's cached raster from the cache and its counts.
    void drop_raster(EnginePartRecord& record) noexcept;
    // Moves the reader counts from a record's old inputs to `inputs`.
    void count_readers(std::uint64_t old_inputs, std::uint64_t inputs) noexcept;
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
    std::size_t item_records_{};
    std::size_t item_record_cap_{};
    std::uint32_t oldest_item_{no_slot};
    std::uint32_t newest_item_{no_slot};
    // Advanced by invalidate_rasters; starts at 1 so a record that never
    // cached a raster (raster_table_generation 0) is never current.
    std::uint64_t raster_table_generation_{1U};
    RasterCacheStatistics statistics_{};
    // Live records reading each expression input, by position.
    std::array<std::uint32_t, maximum_expression_inputs> readers_{};
};

} // namespace gui_forms
