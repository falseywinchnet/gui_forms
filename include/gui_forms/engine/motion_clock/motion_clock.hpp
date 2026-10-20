#pragma once

#include "gui_forms/engine/motion_value/motion_value.hpp"
#include "gui_forms/scheduler/frame_request_token/frame_request_token.hpp"
#include "gui_forms/scheduler/types/scheduler_types.hpp"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace gui_forms {

class Control;
class Window;

// Where a clock reads the time a motion starts. Steady-clock time unless a
// source is installed: tests install one so that motion is sampled from a
// time they choose, never from the wall clock. Its times must be in the
// same domain as the times the window is polled with.
class MotionTimeSource {
public:
    virtual ~MotionTimeSource() = default;
    [[nodiscard]] virtual FrameTime now() const noexcept = 0;
};

// The clock's frame cadence while something moves.
inline constexpr FrameInterval motion_frame_interval = std::chrono::milliseconds(16);

// One window's motion clock (T2 plan, MotionClock; GIVEN D1-1). The window
// owns it; it observes the window. It holds at most one frame request,
// armed only while some value it holds has not settled, however many values
// move; the request is disconnected once the last one settles, so a window
// at rest schedules no frames.
//
// Execution: every member runs on the window's UI thread. Frames, settling
// and the listeners' damage all happen inside the window's frame poll.
//
// Event order on a frame: each value enlisted before the frame starts is
// advanced to the frame's time, in the order of its record's slot (the same
// for the same sequence of starts and releases); a value that
// settles is released before its listener runs; then the owner is
// invalidated as the value declared, then its listener runs. Values started
// by a listener are first advanced on the next frame. A listener that
// throws is counted as a frame callback fault; its value jumps to its end.
//
// Ending motion (all by name):
// - finish_all: reduced motion turned on (GIVEN D2-1) or the window
//   occluded or minimized (GIVEN D4-1). Every value jumps to its end and its
//   listener runs once, with the cause. While either holds, a new move
//   jumps to its end at once and nothing is scheduled.
// - release_owner: a control left the window. Its values jump to their
//   ends without listeners.
// - cancel: the window closes (or cancels its frame requests). Every value
//   jumps to its end without listeners, and the request is disconnected.
//
// A record is named by its handle; a stale handle (its record released) is
// rejected, so a value whose owner has gone is never advanced.
class MotionClock final {
public:
    explicit MotionClock(Window& window) noexcept;
    ~MotionClock();
    MotionClock(const MotionClock&) = delete;
    MotionClock& operator=(const MotionClock&) = delete;

    // Borrows `source` (it must outlive its installation); null restores the
    // steady clock.
    void set_time_source(const MotionTimeSource* source) noexcept { time_source_ = source; }
    // The time a move starting now starts at.
    [[nodiscard]] FrameTime now() const noexcept;

    // Whether the clock holds its frame request.
    [[nodiscard]] bool armed() const noexcept { return request_.connected(); }
    // Values moving on the clock now.
    [[nodiscard]] std::size_t moving_count() const noexcept { return moving_; }
    // Whether a move would jump to its end at once: reduced motion, or an
    // occluded or minimized window.
    [[nodiscard]] bool suppressed() const noexcept;

private:
    friend class MotionValue;
    friend class Window;

    struct Record final {
        MotionValue* value{};
        // The owner, observed: checked before every step, so a value whose
        // owner has gone or left the window is released, never advanced.
        std::weak_ptr<Control> owner{};
        const Control* owner_address{};
        // Starts at 1, so that a default handle (generation 0) is never live.
        std::uint32_t generation{1U};
        bool live{};
    };

    // For MotionValue. enlist() makes the value's record (if it has none)
    // and arms the frame request; on failure (allocation, the window's
    // request limit) it changes nothing and rethrows. release() frees the
    // value's record; it never allocates.
    void enlist(MotionValue& value);
    void release(MotionValue& value) noexcept;

    // For Window.
    // The frame at `now`: advances every value (see the event order).
    void advance(FrameTime now);
    void finish_all(MotionStepCause cause);
    void release_owner(const Control& owner) noexcept;
    void cancel() noexcept;

    [[nodiscard]] Record* find(MotionRecordHandle handle) noexcept;
    // Ends a record's lifetime and the value's link to it.
    void release_slot(std::uint32_t slot) noexcept;
    // Collects the live records' handles into handles_scratch_, in slot
    // order.
    void collect_handles();
    void disarm_when_idle() noexcept;
    void publish_moving() noexcept;
    // One step's invalidation and listener; `handle` is the value's record
    // if it is still enlisted (a default handle when it is not).
    void notify(Control& owner, MotionValue& value, const MotionStep& step, MotionRecordHandle handle);

    Window* window_;
    const MotionTimeSource* time_source_{};
    std::vector<Record> slots_;
    std::vector<std::uint32_t> free_slots_;
    // Reused by every frame, so that a frame allocates nothing once it has
    // grown to the most values moving at once.
    std::vector<MotionRecordHandle> handles_scratch_;
    std::size_t moving_{};
    FrameRequestToken request_{};
};

} // namespace gui_forms
