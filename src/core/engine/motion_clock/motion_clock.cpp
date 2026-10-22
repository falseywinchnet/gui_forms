// One window's motion clock (T2 plan, phase E3): its records of the moving
// values, its single frame request, the frame, and the named ways motion
// ends.

#include "gui_forms/engine/motion_clock/motion_clock.hpp"

#include "gui_forms/control.hpp"
#include "gui_forms/window.hpp"

#include <limits>
#include <optional>
#include <stdexcept>

namespace gui_forms {

MotionClock::MotionClock(Window& window) noexcept : window_(&window) {}

MotionClock::~MotionClock() {
    cancel();
}

FrameTime MotionClock::now() const noexcept {
    if (time_source_ != nullptr) {
        const FrameTime chosen = (*time_source_).now();
        return chosen;
    }
    const FrameTime steady = FrameClock::now();
    return steady;
}

bool MotionClock::suppressed() const noexcept {
    const bool reduced = (*window_).presentation_settings().reduced_motion;
    const bool hidden = (*window_).occluded();
    return reduced || hidden;
}

bool MotionClock::ambient_suppressed() const noexcept {
    const bool switched_off = !(*window_).presentation_settings().ambient_motion;
    const bool stopped = suppressed() || switched_off;
    return stopped;
}

void MotionClock::enlist_ambient(std::uint32_t part_slot, std::uint32_t part_generation, FrameInterval interval) {
    for (AmbientRecord& existing : ambient_) {
        if (existing.live && existing.part_slot == part_slot && existing.part_generation == part_generation) {
            existing.interval = interval;
            return;
        }
    }
    // Room and the request first, so that nothing below fails.
    std::size_t free = ambient_.size();
    for (std::size_t index = 0; index < ambient_.size(); ++index) {
        if (!ambient_[index].live) free = index;
    }
    if (free == ambient_.size()) ambient_.reserve(ambient_.size() + 1U);
    if (!request_.connected()) request_ = (*window_).request_motion_frames(now() + motion_frame_interval);
    if (free == ambient_.size()) ambient_.emplace_back();
    AmbientRecord& made = ambient_[free];
    made.part_slot = part_slot;
    made.part_generation = part_generation;
    made.interval = interval;
    // The first frame one interval after the paint that started it.
    made.next = now() + interval;
    made.seen = none_seen;
    made.live = true;
    ambient_live_ = ambient_live_ + 1U;
    align_to_ambient();
}

void MotionClock::withdraw_ambient(std::uint32_t part_slot, std::uint32_t part_generation) noexcept {
    for (AmbientRecord& existing : ambient_) {
        if (!existing.live || existing.part_slot != part_slot || existing.part_generation != part_generation) continue;
        existing.live = false;
        ambient_live_ = ambient_live_ - 1U;
    }
    disarm_when_idle();
}

void MotionClock::stop_ambient(bool repaint) {
    for (AmbientRecord& existing : ambient_) {
        if (!existing.live) continue;
        existing.live = false;
        ambient_live_ = ambient_live_ - 1U;
        if (repaint) (*window_).repaint_engine_part(existing.part_slot, existing.part_generation);
    }
    disarm_when_idle();
}

void MotionClock::advance_ambient(FrameTime now) {
    // By index: a repaint never enlists or withdraws an effect.
    for (std::size_t index = 0; index < ambient_.size(); ++index) {
        AmbientRecord& effect = ambient_[index];
        if (!effect.live || effect.next > now) continue;
        const bool shown = (*window_).engine_ambient_frame(effect.part_slot, effect.part_generation, effect.seen);
        if (!shown) {
            effect.live = false;
            ambient_live_ = ambient_live_ - 1U;
            continue;
        }
        // At least one interval from this frame: the cap holds however late
        // the frame came.
        effect.next = now + effect.interval;
    }
    disarm_when_idle();
    align_to_ambient();
}

void MotionClock::align_to_ambient() noexcept {
    if (moving_ != 0U || ambient_live_ == 0U || !request_.connected()) return;
    // Nothing settles, so the request need not run at the motion cadence:
    // its next frame is the earliest effect's.
    std::optional<FrameTime> earliest{};
    for (const AmbientRecord& effect : ambient_) {
        if (!effect.live) continue;
        if (!earliest || effect.next < *earliest) earliest = effect.next;
    }
    if (earliest) (*window_).set_motion_frame_deadline(*earliest);
}

MotionClock::Record* MotionClock::find(MotionRecordHandle handle) noexcept {
    if (handle.slot >= slots_.size()) return nullptr;
    Record& record = slots_[handle.slot];
    if (!record.live || record.generation != handle.generation) return nullptr;
    return &record;
}

void MotionClock::enlist(MotionValue& value) {
    if (value.clock_ == this) {
        (*window_).metrics().record_motion_started();
        return;
    }
    // A value enlisted on another clock (its owner moved windows without
    // leaving through release_owner) leaves that clock first.
    if (value.clock_ != nullptr) (*value.clock_).release(value);
    std::weak_ptr<Control> owner = (*value.owner_).weak_from_this();
    if (owner.expired()) {
        throw std::logic_error("GUI.Forms motion value owner must be held by a shared pointer");
    }
    // Room first, so that nothing below allocates: a new slot (when none is
    // free), the free list's capacity for every slot (release never
    // allocates), and the frame's handle list for every moving value.
    const bool reuse = !free_slots_.empty();
    if (!reuse && slots_.size() >= std::numeric_limits<std::uint32_t>::max()) {
        throw std::length_error("GUI.Forms motion clock record limit reached");
    }
    const std::size_t slot_count = reuse ? slots_.size() : slots_.size() + 1U;
    free_slots_.reserve(slot_count);
    handles_scratch_.reserve(moving_ + 1U);
    if (!reuse) slots_.emplace_back();
    if (!request_.connected()) {
        try {
            request_ = (*window_).request_motion_frames(now() + motion_frame_interval);
        } catch (...) {
            if (!reuse) slots_.pop_back();
            throw;
        }
    }
    std::uint32_t slot = 0U;
    if (reuse) {
        slot = free_slots_.back();
        free_slots_.pop_back();
    } else {
        slot = static_cast<std::uint32_t>(slots_.size() - 1U);
    }
    Record& record = slots_[slot];
    record.value = &value;
    record.owner = std::move(owner);
    record.owner_address = value.owner_;
    record.live = true;
    value.clock_ = this;
    value.record_ = MotionRecordHandle{slot, record.generation};
    ++moving_;
    (*window_).metrics().record_motion_started();
    publish_moving();
}

void MotionClock::release(MotionValue& value) noexcept {
    if (value.clock_ != this) return;
    const Record* const record = find(value.record_);
    if (record != nullptr && (*record).value == &value) {
        release_slot(value.record_.slot);
        return;
    }
    value.clock_ = nullptr;
    value.record_ = {};
}

void MotionClock::release_slot(std::uint32_t slot) noexcept {
    Record& record = slots_[slot];
    MotionValue* const value = record.value;
    record.value = nullptr;
    record.owner.reset();
    record.owner_address = nullptr;
    record.live = false;
    record.generation = record.generation + 1U;
    if (record.generation == 0U) record.generation = 1U;
    // Never allocates: enlist reserved a place for every slot.
    free_slots_.push_back(slot);
    if (value != nullptr) {
        (*value).clock_ = nullptr;
        (*value).record_ = {};
    }
    moving_ = moving_ - 1U;
    publish_moving();
    disarm_when_idle();
}

void MotionClock::collect_handles() {
    handles_scratch_.clear();
    for (std::size_t index = 0; index < slots_.size(); ++index) {
        const Record& record = slots_[index];
        if (!record.live) continue;
        // Never allocates: enlist reserved a place for every moving value.
        handles_scratch_.push_back(MotionRecordHandle{static_cast<std::uint32_t>(index), record.generation});
    }
}

void MotionClock::disarm_when_idle() noexcept {
    if (moving_ == 0U && ambient_live_ == 0U) request_.disconnect();
}

void MotionClock::publish_moving() noexcept {
    (*window_).metrics().set_motion_values_moving(static_cast<std::uint64_t>(moving_));
}

// Runs one value's step: the declared invalidation, then the listener. A
// throwing listener is a frame callback fault; its value (if it is still
// enlisted under `handle`) jumps to its end.
void MotionClock::notify(Control& owner, MotionValue& value, const MotionStep& step, MotionRecordHandle handle) {
    const Dirty invalidation = value.invalidation_;
    const MotionListener listener = value.listener_;
    try {
        if (invalidation != Dirty::none) owner.invalidate(invalidation);
        listener(step);
    } catch (...) {
        (*window_).metrics().record_frame_callback_fault();
        Record* const record = find(handle);
        if (record != nullptr) {
            MotionValue* const faulted = (*record).value;
            release_slot(handle.slot);
            (*faulted).stop();
        }
    }
}

void MotionClock::advance(FrameTime now) {
    const double scale = (*window_).scale();
    collect_handles();
    // By index, holding no reference across a listener: a listener may
    // start values (which may grow the list's capacity) or release any.
    const std::size_t count = handles_scratch_.size();
    for (std::size_t index = 0; index < count; ++index) {
        const MotionRecordHandle handle = handles_scratch_[index];
        Record* const record = find(handle);
        if (record == nullptr) continue;
        MotionValue& value = *(*record).value;
        const std::shared_ptr<Control> owner = (*record).owner.lock();
        if (!owner || !(*owner).is_alive() || (*owner).attached_window() != window_) {
            release_slot(handle.slot);
            value.stop();
            continue;
        }
        const double previous = value.current_;
        const bool settled = value.advance(now, scale);
        if (settled) {
            release_slot(handle.slot);
            (*window_).metrics().record_motion_settled();
        }
        MotionStep step{};
        step.value = &value;
        step.previous = previous;
        step.current = value.current_;
        step.settled = settled;
        step.cause = MotionStepCause::advanced;
        // The owner is held for the call, so its members (the value among
        // them) stay alive through the listener.
        notify(*owner, value, step, handle);
    }
    advance_ambient(now);
}

void MotionClock::finish_all(MotionStepCause cause) {
    collect_handles();
    const std::size_t count = handles_scratch_.size();
    for (std::size_t index = 0; index < count; ++index) {
        const MotionRecordHandle handle = handles_scratch_[index];
        Record* const record = find(handle);
        if (record == nullptr) continue;
        MotionValue& value = *(*record).value;
        const std::shared_ptr<Control> owner = (*record).owner.lock();
        const double previous = value.current_;
        release_slot(handle.slot);
        value.stop();
        (*window_).metrics().record_motion_finished();
        if (!owner || !(*owner).is_alive() || (*owner).attached_window() != window_) continue;
        MotionStep step{};
        step.value = &value;
        step.previous = previous;
        step.current = value.current_;
        step.settled = true;
        step.cause = cause;
        notify(*owner, value, step, MotionRecordHandle{});
    }
    // Ambient effects stop too; each part repaints at its static pose.
    stop_ambient(true);
    request_.disconnect();
}

void MotionClock::release_owner(const Control& owner) noexcept {
    if (moving_ == 0U) return;
    for (std::size_t index = 0; index < slots_.size(); ++index) {
        Record& record = slots_[index];
        if (!record.live || record.owner_address != &owner) continue;
        MotionValue* const value = record.value;
        release_slot(static_cast<std::uint32_t>(index));
        (*value).stop();
    }
}

void MotionClock::cancel() noexcept {
    stop_ambient(false);
    for (std::size_t index = 0; index < slots_.size(); ++index) {
        Record& record = slots_[index];
        if (!record.live) continue;
        MotionValue* const value = record.value;
        release_slot(static_cast<std::uint32_t>(index));
        (*value).stop();
    }
    request_.disconnect();
}

} // namespace gui_forms
