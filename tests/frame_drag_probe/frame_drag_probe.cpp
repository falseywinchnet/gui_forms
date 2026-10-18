// Stage DK decision 49: the forensic frame-drag probe. It measures whether
// a native host keeps up with the framework's capture drag
// (WindowFrameDrag, decision 12) without anyone watching: it drives the
// drag with synthetic pointer input, records every input, every frame
// request and every frame the host reports, and writes one JSON document
// per run and a Markdown summary.
//
//   gui_forms_frame_drag_probe [--out DIR] [--input auto|os|in-process]
//
// --input os posts input through the operating system (Windows SendInput,
// macOS CGEventPost, X11 XTest); in-process warps the cursor and hands the
// pointer events to the window model itself; auto (the default) uses os
// where the host allows it. The results say which was used.
//
// It opens two windows, moves the cursor and presses the primary button on
// its own window, so it is built but never run by ctest. It closes its
// windows when done and exits 0 when every run kept up, 1 when a run did
// not, and 2 when a run could not be measured.
#include "frame_drag_analysis.hpp"
#include "probe/probe_input.hpp"

#include "gui_forms/application.hpp"
#include "gui_forms/basic_controls.hpp"
#include "gui_forms/control.hpp"
#include "gui_forms/threading.hpp"
#include "gui_forms/timer.hpp"

#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <memory>
#include <string>
#include <system_error>
#include <thread>
#include <utility>
#include <vector>

namespace {
using namespace gui_forms;

std::uint64_t now_nanoseconds() {
    const std::chrono::steady_clock::duration since = std::chrono::steady_clock::now().time_since_epoch();
    const std::uint64_t value = static_cast<std::uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(since).count());
    return value;
}

constexpr Size floating_size{240.0, 160.0};
constexpr double path_radius = 100.0;
// Margins around a run, in milliseconds: between the first move and the
// press, the press and the first drag step, and after the release while
// late frame reports arrive.
constexpr std::uint64_t settle_before_press_ms = 60U;
constexpr std::uint64_t settle_before_drag_ms = 60U;
constexpr std::uint64_t settle_after_release_ms = 300U;
// A run that does not finish within its own length plus this fails.
constexpr std::uint64_t run_timeout_margin_ms = 5000U;
// The whole probe is abandoned after this, whatever the event loop does.
constexpr std::uint64_t probe_deadline_ms = 120000U;

struct Profile final {
    const char* name{};
    std::uint64_t interval_ms{};
    double step_units{};
    std::size_t steps{};
};

// Pointer speeds: about 190, 750 and 3000 units a second.
constexpr std::array<Profile, 3> profiles{{{"slow", 16U, 3.0, 90U},
                                           {"medium", 8U, 6.0, 120U},
                                           {"fast", 4U, 12.0, 150U}}};

enum class InputChoice : std::uint8_t { automatic, os, in_process };

struct Options final {
    std::string out_directory{"frame-drag-results"};
    InputChoice input{InputChoice::automatic};
    bool valid{true};
};

Options parse_options(int argc, char** argv) {
    Options options{};
    for (int index = 1; index < argc; ++index) {
        const char* const argument = argv[index];
        const bool has_value = index + 1 < argc;
        if (std::strcmp(argument, "--out") == 0 && has_value) {
            options.out_directory = argv[index + 1];
            ++index;
        } else if (std::strcmp(argument, "--input") == 0 && has_value) {
            const char* const value = argv[index + 1];
            ++index;
            if (std::strcmp(value, "auto") == 0) options.input = InputChoice::automatic;
            else if (std::strcmp(value, "os") == 0) options.input = InputChoice::os;
            else if (std::strcmp(value, "in-process") == 0) options.input = InputChoice::in_process;
            else options.valid = false;
        } else {
            options.valid = false;
        }
    }
    return options;
}

// Native pointer coordinates against GUI.Forms desktop logical units,
// measured by warping the cursor: native = origin + (logical - logical
// origin) * scale, per axis.
struct Calibration final {
    probe::NativePoint native_origin{};
    Point logical_origin{};
    double scale_x{1.0};
    double scale_y{1.0};

    [[nodiscard]] probe::NativePoint native_for(Point logical) const {
        const probe::NativePoint native{native_origin.x + (logical.x - logical_origin.x) * scale_x,
                                        native_origin.y + (logical.y - logical_origin.y) * scale_y};
        return native;
    }
};

// One planned injection step; the plan is built on the UI thread before the
// injector starts and only read while it runs.
struct PlannedStep final {
    probe::PointerStep step{probe::PointerStep::move};
    probe::NativePoint native{};
    Point logical{};
    std::uint64_t offset_nanoseconds{};
};

// What the injector worker writes. `inputs` is sized before the worker
// starts; the worker fills entries in order and publishes the count; the UI
// thread reads them only after joining it.
struct InjectionLog final {
    std::vector<probe::InputRecord> inputs{};
    std::atomic<std::size_t> count{0U};
    std::atomic<bool> finished{false};
};

struct Injector final {
    const std::vector<PlannedStep>* plan{};
    InjectionLog* log{};
    std::uint64_t start_nanoseconds{};
};

void run_injector(const CancellationFlag& cancellation, void* context) {
    const Injector& injector = *static_cast<const Injector*>(context);
    const std::vector<PlannedStep>& plan = *injector.plan;
    InjectionLog& log = *injector.log;
    const std::chrono::steady_clock::time_point origin{
        std::chrono::nanoseconds(static_cast<std::int64_t>(injector.start_nanoseconds))};
    for (std::size_t index = 0U; index < plan.size(); ++index) {
        if (cancellation.requested()) break;
        const PlannedStep& planned = plan[index];
        std::this_thread::sleep_until(origin +
                                      std::chrono::nanoseconds(static_cast<std::int64_t>(planned.offset_nanoseconds)));
        const bool accepted = probe::inject(planned.step, planned.native);
        probe::InputRecord& record = log.inputs[index];
        record.issued_nanoseconds = now_nanoseconds();
        record.pointer = {planned.logical.x, planned.logical.y};
        record.accepted = accepted;
        log.count.store(index + 1U, std::memory_order_release);
    }
    log.finished.store(true, std::memory_order_release);
}

// Ends the process if the probe outlives its deadline (an event loop that
// stopped delivering timers, a host that never closes).
struct Watchdog final {
    std::uint64_t deadline_nanoseconds{};
};

void run_watchdog(const CancellationFlag& cancellation, void* context) {
    const Watchdog& watchdog = *static_cast<const Watchdog*>(context);
    while (!cancellation.requested()) {
        if (now_nanoseconds() >= watchdog.deadline_nanoseconds) {
            std::fprintf(stderr, "frame drag probe: deadline passed; abandoning the run\n");
            std::fflush(stderr);
            std::_Exit(2);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

enum class Phase : std::uint8_t {
    opening,
    placing,
    calibrating,
    calibration_read,
    running,
    settling,
    finished,
};

class DragBody;

struct Probe final {
    Options options{};
    ApplicationWindowHandle main{};
    ApplicationWindowHandle floating{};
    // Borrowed from the floating window's ready callback; cleared when it
    // closes.
    Window* floating_model{};
    std::shared_ptr<DragBody> body{};
    std::string platform{};
    probe::InputSupport support{};
    probe::InputPath path{probe::InputPath::in_process};
    probe::DisplayTiming display{};
    Calibration calibration{};
    probe::NativePoint calibration_native{};
    Phase phase{Phase::opening};
    std::uint64_t phase_started{};
    std::size_t profile_index{};
    Rect home{};
    Point grab{};

    probe::RunRecording recording{};
    bool recording_active{};
    bool pressed{};
    bool released{};
    // In-process mode: the window model sees only events the probe
    // dispatches, never ones the host delivers from the cursor warp.
    bool dispatching{};
    std::string run_error{};

    std::vector<PlannedStep> plan{};
    InjectionLog log{};
    Injector injector{};
    // Declared after the plan and log it borrows.
    std::unique_ptr<Worker> injector_worker{};
    std::size_t driver_step{};

    std::vector<probe::RunSummary> summaries{};
    int exit_code{};
    std::unique_ptr<Timer> pump{};
    std::unique_ptr<Timer> driver{};
    SubscriptionToken pump_subscription{};
    SubscriptionToken driver_subscription{};
};

void enter(Probe& state, Phase phase) {
    state.phase = phase;
    state.phase_started = now_nanoseconds();
}

std::uint64_t phase_elapsed_ms(const Probe& state) {
    const std::uint64_t elapsed = (now_nanoseconds() - state.phase_started) / 1000000U;
    return elapsed;
}

// The floating window's body: the drag's owner, as a caption would be.
class DragBody final : public Control {
public:
    DragBody(StableId id, Probe& state) : Control(std::move(id)), state_(state) {}
    WindowFrameDrag drag{};

    void on_pointer(PointerEvent& event) override {
        Probe& state = state_;
        const bool own_input = state.path == probe::InputPath::os_injection || state.dispatching;
        if (!own_input || !state.recording_active) return;
        if (event.action == PointerAction::down && event.button == PointerButton::primary && !state.pressed) {
            set_pointer_capture(true);
            const std::uint64_t now = now_nanoseconds();
            state.recording.begin_nanoseconds = now;
            const HostServiceStatus begun = drag.begin(state.floating, now);
            if (!begun.accepted()) state.run_error = "WindowFrameDrag::begin was refused";
            state.pressed = true;
            event.handled = true;
        } else if (event.action == PointerAction::move && drag.active()) {
            record_move();
            event.handled = true;
        } else if (event.action == PointerAction::up && drag.active()) {
            const std::uint64_t now = now_nanoseconds();
            drag.end(now);
            set_pointer_capture(false);
            const WindowFrameDragStatistics& statistics = drag.statistics();
            probe::DragTotals& totals = state.recording.totals;
            totals.pointer_moves = statistics.pointer_moves;
            totals.frame_requests = statistics.frame_requests;
            totals.refused_requests = statistics.refused_requests;
            totals.frame_reports = statistics.frame_reports;
            totals.duration_seconds = statistics.duration_seconds();
            totals.pointer_rate_hz = statistics.pointer_rate_hz();
            totals.achieved_rate_hz = statistics.achieved_rate_hz();
            totals.keeps_up = statistics.keeps_up();
            state.recording.end_nanoseconds = now;
            state.released = true;
            event.handled = true;
        }
    }

private:
    void record_move() {
        Probe& state = state_;
        probe::MoveRecord record{};
        record.received_nanoseconds = now_nanoseconds();
        const HostScreenPointResult pointer = state.floating.windows().pointer_position();
        if (pointer.status.accepted()) record.pointer = {pointer.position.x, pointer.position.y};
        const std::uint64_t requests_before = drag.statistics().frame_requests;
        const std::uint64_t refused_before = drag.statistics().refused_requests;
        record.requested_nanoseconds = now_nanoseconds();
        static_cast<void>(drag.move(record.requested_nanoseconds));
        record.requested = drag.statistics().frame_requests != requests_before;
        record.refused = drag.statistics().refused_requests != refused_before;
        const Point target = drag.requested_origin();
        record.target = {target.x, target.y};
        // Reserved before the run; a run longer than planned stops recording
        // rather than allocate.
        if (state.recording.moves.size() < state.recording.moves.capacity()) state.recording.moves.push_back(record);
    }

    Probe& state_;
};

probe::RefreshClock refresh_clock(const Probe& state) {
    probe::RefreshClock clock{};
    clock.period_nanoseconds = state.display.period_available ? state.display.refresh_period_nanoseconds : 0U;
    clock.period_measured = state.display.period_available;
    clock.counter_available = state.display.refresh_counter;
    return clock;
}

probe::RunDescription describe(const Probe& state, const Profile& profile) {
    probe::RunDescription description{};
    description.platform = state.platform;
    description.input_path = probe::input_path_name(state.path);
    description.input_api =
        state.path == probe::InputPath::os_injection ? state.support.injection_api : state.support.warp_api;
    description.injection_unavailable_reason = state.support.os_injection ? std::string{} : state.support.unavailable_reason;
    description.profile = profile.name;
    description.interval_ms = static_cast<double>(profile.interval_ms);
    description.step_units = profile.step_units;
    description.steps = profile.steps;
    description.display_source = state.display.source;
    description.window_scale = state.floating_model != nullptr ? (*state.floating_model).scale() : 0.0;
    description.error = state.run_error;
    return description;
}

std::string result_path(const Probe& state, const std::string& name) {
    const std::filesystem::path path = std::filesystem::path(state.options.out_directory) / name;
    const std::string text = path.string();
    return text;
}

// Writes the run's JSON and keeps its summary. Leaves the recording as it
// was.
void finish_run(Probe& state, const Profile& profile) {
    const probe::RunDescription description = describe(state, profile);
    const probe::RunAnalysis analysis = probe::analyze(state.recording, refresh_clock(state));
    const std::string name = "frame-drag-" + state.platform + "-" + description.input_path + "-" + profile.name + ".json";
    const std::string path = result_path(state, name);
    std::FILE* const file = std::fopen(path.c_str(), "wb");
    bool written = false;
    if (file != nullptr) {
        written = probe::write_run_json(file, description, state.recording, analysis);
        written = std::fclose(file) == 0 && written;
    }
    probe::RunSummary summary{};
    summary.description = description;
    summary.totals = state.recording.totals;
    summary.analysis = analysis;
    summary.analysis.outcomes.clear();
    summary.verdict = probe::verdict_name(description, state.recording);
    summary.json_file = name;
    std::printf("frame drag probe: %s %s: %s, %s\n", profile.name, description.input_path.c_str(),
                summary.verdict.c_str(), written ? path.c_str() : "JSON NOT WRITTEN");
    std::fflush(stdout);
    if (!written || summary.verdict == "not_measured") state.exit_code = 2;
    else if (summary.verdict == "falls_behind" && state.exit_code == 0) state.exit_code = 1;
    state.summaries.push_back(std::move(summary));
}

void finish_probe(Probe& state) {
    enter(state, Phase::finished);
    if (state.pump) (*state.pump).stop();
    if (state.driver) (*state.driver).stop();
    const std::string path = result_path(state, "summary.md");
    std::FILE* const file = std::fopen(path.c_str(), "wb");
    bool written = false;
    if (file != nullptr) {
        written = probe::write_summary_markdown(file, state.summaries);
        written = std::fclose(file) == 0 && written;
    }
    if (!written) state.exit_code = 2;
    static_cast<void>(state.main.request_close());
}

// Stops a run that cannot finish: writes what was recorded, marked not
// measured, and ends the probe.
void fail_run(Probe& state, const char* reason) {
    if (state.run_error.empty()) state.run_error = reason;
    if (state.injector_worker) {
        (*state.injector_worker).request_cancel();
        (*state.injector_worker).join();
        state.injector_worker.reset();
    }
    if (state.driver) (*state.driver).stop();
    if (state.body && (*state.body).drag.active()) (*state.body).drag.end(now_nanoseconds());
    state.recording_active = false;
    const std::size_t index = state.profile_index < profiles.size() ? state.profile_index : 0U;
    finish_run(state, profiles[index]);
    finish_probe(state);
}

// The run's path: a circle through the grab point, `steps` steps of
// `step_units` along it.
void build_plan(Probe& state, const Profile& profile) {
    state.plan.clear();
    state.plan.reserve(profile.steps + 3U);
    PlannedStep first{};
    first.step = probe::PointerStep::move;
    first.logical = state.grab;
    first.native = state.calibration.native_for(state.grab);
    first.offset_nanoseconds = 0U;
    state.plan.push_back(first);
    PlannedStep press = first;
    press.step = probe::PointerStep::press;
    press.offset_nanoseconds = settle_before_press_ms * 1000000U;
    state.plan.push_back(press);
    const std::uint64_t drag_origin = press.offset_nanoseconds + settle_before_drag_ms * 1000000U;
    const double angle_step = profile.step_units / path_radius;
    Point last = state.grab;
    for (std::size_t index = 1U; index <= profile.steps; ++index) {
        const double angle = angle_step * static_cast<double>(index);
        PlannedStep step{};
        step.step = probe::PointerStep::drag;
        step.logical = {state.grab.x + path_radius * (std::cos(angle) - 1.0),
                        state.grab.y + path_radius * std::sin(angle)};
        step.native = state.calibration.native_for(step.logical);
        step.offset_nanoseconds = drag_origin + static_cast<std::uint64_t>(index - 1U) * profile.interval_ms * 1000000U;
        state.plan.push_back(step);
        last = step.logical;
    }
    PlannedStep release{};
    release.step = probe::PointerStep::release;
    release.logical = last;
    release.native = state.calibration.native_for(last);
    release.offset_nanoseconds = state.plan.back().offset_nanoseconds + profile.interval_ms * 1000000U;
    state.plan.push_back(release);
}

void start_run(Probe& state) {
    const Profile& profile = profiles[state.profile_index];
    state.recording.clear();
    state.recording.reserve(profile.steps + 8U);
    state.run_error.clear();
    state.pressed = false;
    state.released = false;
    state.driver_step = 0U;
    const HostWindowFrameResult frame = state.floating.frame();
    if (!frame.status.accepted()) {
        fail_run(state, "the floating window's frame could not be read");
        return;
    }
    const Rect content = frame.frame.content;
    state.grab = {content.x + content.width * 0.5, content.y + content.height * 0.5};
    build_plan(state, profile);
    state.recording_active = true;
    enter(state, Phase::running);
    if (state.path == probe::InputPath::os_injection) {
        state.log.inputs.assign(state.plan.size(), probe::InputRecord{});
        state.log.count.store(0U, std::memory_order_relaxed);
        state.log.finished.store(false, std::memory_order_relaxed);
        state.injector.plan = &state.plan;
        state.injector.log = &state.log;
        state.injector.start_nanoseconds = now_nanoseconds();
        state.injector_worker = std::make_unique<Worker>(run_injector, &state.injector);
        return;
    }
    (*state.driver).set_interval(std::chrono::milliseconds(profile.interval_ms));
    (*state.driver).start();
}

// In-process mode: one plan step per driver tick, on the UI thread.
struct DriverTick final {
    Probe* state{};
    void operator()() const {
        Probe& probe_state = *state;
        if (probe_state.phase != Phase::running || probe_state.floating_model == nullptr) return;
        if (probe_state.driver_step >= probe_state.plan.size()) {
            (*probe_state.driver).stop();
            return;
        }
        const PlannedStep& planned = probe_state.plan[probe_state.driver_step];
        ++probe_state.driver_step;
        probe::InputRecord record{};
        record.accepted = probe::warp_native_cursor(planned.native);
        record.issued_nanoseconds = now_nanoseconds();
        record.pointer = {planned.logical.x, planned.logical.y};
        if (probe_state.recording.inputs.size() < probe_state.recording.inputs.capacity()) {
            probe_state.recording.inputs.push_back(record);
        }
        PointerEvent event{};
        // The body fills the window; its centre is where the drag grabs it.
        event.position = {floating_size.width * 0.5, floating_size.height * 0.5};
        if (planned.step == probe::PointerStep::press) {
            event.action = PointerAction::down;
            event.button = PointerButton::primary;
        } else if (planned.step == probe::PointerStep::release) {
            event.action = PointerAction::up;
            event.button = PointerButton::primary;
        } else {
            event.action = PointerAction::move;
        }
        probe_state.dispatching = true;
        static_cast<void>((*probe_state.floating_model).dispatch_pointer(event));
        probe_state.dispatching = false;
    }
};

// Copies the injector's records into the recording once it has finished.
void collect_injection(Probe& state) {
    (*state.injector_worker).join();
    state.injector_worker.reset();
    const std::size_t count = state.log.count.load(std::memory_order_acquire);
    for (std::size_t index = 0U; index < count; ++index) {
        if (state.recording.inputs.size() >= state.recording.inputs.capacity()) break;
        state.recording.inputs.push_back(state.log.inputs[index]);
    }
}

bool calibrate_start(Probe& state) {
    if (!probe::native_cursor(state.calibration.native_origin)) return false;
    const HostScreenPointResult logical = state.main.windows().pointer_position();
    if (!logical.status.accepted()) return false;
    state.calibration.logical_origin = logical.position;
    state.calibration_native = {state.calibration.native_origin.x + 64.0, state.calibration.native_origin.y + 64.0};
    const bool warped = probe::warp_native_cursor(state.calibration_native);
    return warped;
}

bool calibrate_finish(Probe& state) {
    const HostScreenPointResult logical = state.main.windows().pointer_position();
    if (!logical.status.accepted()) return false;
    const double logical_dx = logical.position.x - state.calibration.logical_origin.x;
    const double logical_dy = logical.position.y - state.calibration.logical_origin.y;
    if (std::fabs(logical_dx) < 1.0 || std::fabs(logical_dy) < 1.0) return false;
    state.calibration.scale_x = (state.calibration_native.x - state.calibration.native_origin.x) / logical_dx;
    state.calibration.scale_y = (state.calibration_native.y - state.calibration.native_origin.y) / logical_dy;
    const bool plausible = std::isfinite(state.calibration.scale_x) && std::isfinite(state.calibration.scale_y) &&
        std::fabs(state.calibration.scale_x) >= 0.25 && std::fabs(state.calibration.scale_x) <= 8.0 &&
        std::fabs(state.calibration.scale_y) >= 0.25 && std::fabs(state.calibration.scale_y) <= 8.0;
    return plausible;
}

bool at_home(const Probe& state) {
    const HostWindowFrameResult frame = state.floating.frame();
    if (!frame.status.accepted()) return false;
    const bool placed = std::fabs(frame.frame.content.x - state.home.x) <= probe::position_tolerance &&
        std::fabs(frame.frame.content.y - state.home.y) <= probe::position_tolerance;
    return placed;
}

// The probe's sequence, one step per 16 ms tick of the main window's timer.
struct PumpTick final {
    Probe* state{};
    void operator()() const {
        Probe& probe_state = *state;
        switch (probe_state.phase) {
        case Phase::opening:
            if (phase_elapsed_ms(probe_state) > 5000U) fail_run(probe_state, "the floating window never opened");
            return;
        case Phase::placing:
            if (at_home(probe_state)) {
                enter(probe_state, Phase::calibrating);
                return;
            }
            // A window manager may refuse the position; start from wherever
            // the window is after a second.
            if (phase_elapsed_ms(probe_state) > 1000U) enter(probe_state, Phase::calibrating);
            return;
        case Phase::calibrating:
            if (!calibrate_start(probe_state)) {
                fail_run(probe_state, "the cursor could not be read or warped for calibration");
                return;
            }
            enter(probe_state, Phase::calibration_read);
            return;
        case Phase::calibration_read:
            if (phase_elapsed_ms(probe_state) < 50U) return;
            if (!calibrate_finish(probe_state)) {
                fail_run(probe_state, "the cursor warp did not move the GUI.Forms pointer position as expected");
                return;
            }
            start_run(probe_state);
            return;
        case Phase::running: {
            const Profile& profile = profiles[probe_state.profile_index];
            if (!probe_state.run_error.empty()) {
                fail_run(probe_state, probe_state.run_error.c_str());
                return;
            }
            const bool injected = probe_state.path != probe::InputPath::os_injection ||
                probe_state.log.finished.load(std::memory_order_acquire);
            if (probe_state.released && injected) {
                if (probe_state.injector_worker) collect_injection(probe_state);
                enter(probe_state, Phase::settling);
                return;
            }
            const std::uint64_t length_ms = settle_before_press_ms + settle_before_drag_ms +
                profile.interval_ms * static_cast<std::uint64_t>(profile.steps + 1U);
            if (!probe_state.pressed && phase_elapsed_ms(probe_state) > settle_before_press_ms + 2000U) {
                fail_run(probe_state, "the synthetic press never reached the window");
                return;
            }
            if (phase_elapsed_ms(probe_state) > length_ms + run_timeout_margin_ms) {
                fail_run(probe_state, "the run did not finish (the release never arrived)");
            }
            return;
        }
        case Phase::settling:
            if (phase_elapsed_ms(probe_state) < settle_after_release_ms) return;
            probe_state.recording_active = false;
            finish_run(probe_state, profiles[probe_state.profile_index]);
            ++probe_state.profile_index;
            if (probe_state.profile_index >= profiles.size()) {
                finish_probe(probe_state);
                return;
            }
            static_cast<void>(probe_state.floating.request_frame(probe_state.home));
            enter(probe_state, Phase::placing);
            return;
        case Phase::finished:
            return;
        }
    }
};

struct FloatingFrames final {
    Probe* state{};
    void operator()(const HostWindowFrameEvent& event) const {
        Probe& probe_state = *state;
        if (!probe_state.recording_active || event.window != probe_state.floating.id()) return;
        probe::ConfirmRecord record{};
        record.confirmed_nanoseconds = now_nanoseconds();
        record.origin = {event.frame.content.x, event.frame.content.y};
        if (probe_state.display.refresh_counter) {
            probe::RefreshSample sample{};
            if (probe::sample_refresh(sample)) {
                record.refresh_count = sample.refresh_count;
                record.vblank_nanoseconds = sample.last_vblank_nanoseconds;
            }
        }
        if (probe_state.recording.confirmations.size() < probe_state.recording.confirmations.capacity()) {
            probe_state.recording.confirmations.push_back(record);
        }
    }
};

struct FloatingClosed final {
    Probe* state{};
    void operator()() const { (*state).floating_model = nullptr; }
};

struct FloatingReady final {
    Probe* state{};
    void operator()(Window& window, ApplicationWindowHandle handle) const {
        Probe& probe_state = *state;
        probe_state.floating = handle;
        probe_state.floating_model = &window;
        static_cast<void>(handle.request_frame(probe_state.home));
        enter(probe_state, Phase::placing);
    }
};

// Releases the timers while the main window they belong to still exists.
struct MainClosed final {
    Probe* state{};
    void operator()() const {
        Probe& probe_state = *state;
        probe_state.pump_subscription.disconnect();
        probe_state.driver_subscription.disconnect();
        if (probe_state.pump) (*probe_state.pump).dispose();
        if (probe_state.driver) (*probe_state.driver).dispose();
        probe_state.pump.reset();
        probe_state.driver.reset();
    }
};

Rect home_frame(const ApplicationWindows& windows) {
    Rect area{100.0, 100.0, 800.0, 600.0};
    const HostMonitorResult monitors = windows.monitors();
    if (monitors.status.accepted()) {
        for (const HostMonitor& monitor : monitors.monitors) {
            if (monitor.primary) area = monitor.work_area;
        }
    }
    // Centred, with room for the path's circle to the left and below.
    const Rect home{area.x + area.width * 0.5 - floating_size.width * 0.5 + path_radius,
                    area.y + area.height * 0.5 - floating_size.height * 0.5 - path_radius * 0.5,
                    floating_size.width, floating_size.height};
    return home;
}

struct MainReady final {
    Probe* state{};
    void operator()(Window& window, ApplicationWindowHandle handle) const {
        Probe& probe_state = *state;
        probe_state.main = handle;
        const ApplicationWindows windows = handle.windows();
        const HostCapabilities capabilities = windows.capabilities();
        probe_state.platform = capabilities.platform;
        probe_state.support = probe::input_support();
        const bool os = probe_state.options.input == InputChoice::os ||
            (probe_state.options.input == InputChoice::automatic && probe_state.support.os_injection);
        probe_state.path = os ? probe::InputPath::os_injection : probe::InputPath::in_process;
        probe_state.display = probe::display_timing();
        probe_state.pump = std::make_unique<Timer>(window, std::chrono::milliseconds(16));
        probe_state.pump_subscription = (*probe_state.pump).tick().subscribe(PumpTick{state});
        probe_state.driver = std::make_unique<Timer>(window, std::chrono::milliseconds(16));
        probe_state.driver_subscription = (*probe_state.driver).tick().subscribe(DriverTick{state});
        enter(probe_state, Phase::opening);
        (*probe_state.pump).start();
        if (os && !probe_state.support.os_injection) {
            probe_state.run_error = "OS injection was requested but is unavailable: " + probe_state.support.unavailable_reason;
            fail_run(probe_state, probe_state.run_error.c_str());
            return;
        }
        if (WindowFrameDrag::mode_for(capabilities) != WindowFrameDragMode::framework_capture) {
            fail_run(probe_state, "the host offers only the native move loop (no window_frames, screen_pointer or "
                                  "pointer_capture)");
            return;
        }
        probe_state.home = home_frame(windows);
        probe_state.body = make_control<DragBody>(StableId("probe.body"), probe_state);
        ApplicationWindow floating{};
        floating.stable_id = "probe.floating";
        floating.owner_id = "main";
        floating.floating = HostWindowFloating::document;
        floating.model = std::make_unique<Window>(probe_state.body, floating_size, window.resources());
        floating.options.title = "GUI.Forms frame drag probe target";
        floating.options.initial_size = floating_size;
        floating.options.minimum_size = {120.0, 80.0};
        floating.options.ready = FloatingReady{state};
        floating.options.frame_changed = FloatingFrames{state};
        floating.options.closed = FloatingClosed{state};
        const ApplicationWindowOpenResult opened = windows.open_window(std::move(floating));
        if (!opened.status.accepted()) fail_run(probe_state, "the floating window was refused");
    }
};

} // namespace

int main(int argc, char** argv) {
    Probe state{};
    state.options = parse_options(argc, argv);
    if (!state.options.valid) {
        std::fprintf(stderr, "usage: gui_forms_frame_drag_probe [--out DIR] [--input auto|os|in-process]\n");
        return 2;
    }
    std::error_code created{};
    std::filesystem::create_directories(state.options.out_directory, created);
    if (created) {
        std::fprintf(stderr, "frame drag probe: cannot create %s\n", state.options.out_directory.c_str());
        return 2;
    }
    Watchdog watchdog{};
    watchdog.deadline_nanoseconds = now_nanoseconds() + probe_deadline_ms * 1000000U;
    Worker watchdog_worker(run_watchdog, &watchdog);

    ApplicationWindowOptions options{};
    options.title = "GUI.Forms frame drag probe";
    options.initial_size = {480.0, 320.0};
    options.ready = MainReady{&state};
    options.closed = MainClosed{&state};
    const std::shared_ptr<Panel> root = make_control<Panel>(StableId("probe.root"));
    const ApplicationResult result =
        Application::run(std::make_unique<Window>(root, Size{480.0, 320.0}), std::move(options));
    if (state.injector_worker) {
        (*state.injector_worker).request_cancel();
        (*state.injector_worker).join();
        state.injector_worker.reset();
    }
    probe::release();
    watchdog_worker.request_cancel();
    watchdog_worker.join();
    if (!result.accepted()) return 2;
    if (state.phase != Phase::finished) return 2;
    return state.exit_code;
}
