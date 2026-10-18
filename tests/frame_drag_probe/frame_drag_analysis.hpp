#pragma once

// The forensic frame-drag probe's records and their analysis (stage DK
// decision 49). Renderer-free and host-free: the probe fills a
// RunRecording on the UI thread and the injector, and analyze turns it into
// latency percentiles, missed refreshes, position error and the verdict.
// gui_forms_frame_drag_analysis_tests checks the arithmetic headless.
//
// Times are steady-clock nanoseconds; positions are GUI.Forms desktop
// logical units.

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

namespace gui_forms::probe {

struct Position final {
    double x{};
    double y{};
};

// One synthetic pointer step, as the injector or the in-process driver
// issued it.
struct InputRecord final {
    std::uint64_t issued_nanoseconds{};
    // Where the step put the pointer, in logical units (the native point
    // mapped back through the calibration).
    Position pointer{};
    bool accepted{};
};

// One pointer move the dragged window's model received, and what the
// framework's WindowFrameDrag did with it.
struct MoveRecord final {
    std::uint64_t received_nanoseconds{};
    // ApplicationWindows::pointer_position when the move arrived.
    Position pointer{};
    // Whether WindowFrameDrag::move requested a frame; false when the target
    // did not change.
    bool requested{};
    bool refused{};
    // When move returned, with the frame request submitted.
    std::uint64_t requested_nanoseconds{};
    // The content origin it requested (WindowFrameDrag::requested_origin).
    Position target{};
};

// One frame report of the dragged window from the host (Windows WM_MOVE,
// sent from WM_WINDOWPOSCHANGED; macOS windowDidMove; X11 ConfigureNotify), as the
// probe's frame_changed handler received it.
struct ConfirmRecord final {
    std::uint64_t confirmed_nanoseconds{};
    Position origin{};
    // The compositor's refresh counter and latest vertical blank when the
    // report arrived, where the platform gives them (DWM); 0 otherwise.
    std::uint64_t refresh_count{};
    std::uint64_t vblank_nanoseconds{};
};

// WindowFrameDragStatistics as the drag reported it, copied so the analysis
// does not depend on the application library.
struct DragTotals final {
    std::uint64_t pointer_moves{};
    std::uint64_t frame_requests{};
    std::uint64_t refused_requests{};
    std::uint64_t frame_reports{};
    double duration_seconds{};
    double pointer_rate_hz{};
    double achieved_rate_hz{};
    bool keeps_up{};
};

struct RunRecording final {
    std::vector<InputRecord> inputs{};
    std::vector<MoveRecord> moves{};
    std::vector<ConfirmRecord> confirmations{};
    std::uint64_t begin_nanoseconds{};
    std::uint64_t end_nanoseconds{};
    DragTotals totals{};

    // Reserves every record vector before a run, so recording allocates
    // nothing while the drag is measured.
    void reserve(std::size_t steps);
    void clear() noexcept;
};

struct RefreshClock final {
    std::uint64_t period_nanoseconds{};
    // False when the period is the 60 Hz assumption.
    bool period_measured{};
    // Whether ConfirmRecord::refresh_count was measured.
    bool counter_available{};
};

// Nearest-rank percentiles of a sample, in milliseconds (or logical units
// for position error). All zero for an empty sample.
struct Percentiles final {
    std::size_t count{};
    double p50{};
    double p95{};
    double p99{};
    double max{};
};

// What happened to one frame request.
struct RequestOutcome final {
    std::size_t move_index{};
    // The input that produced the move (nearest issued pointer before it).
    bool input_matched{};
    std::size_t input_index{};
    bool confirmed{};
    // The host reported exactly this target; false when a later target's
    // report superseded it (the host coalesced moves).
    bool exact{};
    std::size_t confirmation_index{};
    double request_to_confirm_ms{};
    double input_to_request_ms{};
    double input_to_confirm_ms{};
};

struct RunAnalysis final {
    std::size_t inputs_issued{};
    std::size_t moves_received{};
    std::size_t requests{};
    std::size_t exact_confirmations{};
    std::size_t coalesced_requests{};
    std::size_t unconfirmed_requests{};
    // Reports at a position no request asked for (a window manager placing
    // or clamping the window).
    std::size_t foreign_confirmations{};
    Percentiles request_to_confirm_ms{};
    Percentiles input_to_request_ms{};
    Percentiles input_to_confirm_ms{};
    // Distance from each report to the latest target requested before it.
    Percentiles position_error{};
    // Distance from the last report to the last target.
    double final_position_error{};
    RefreshClock clock{};
    std::size_t refresh_intervals{};
    std::size_t missed_refreshes{};
    std::vector<RequestOutcome> outcomes{};
};

// Positions closer than this count as the same frame (host rounding to
// device pixels).
inline constexpr double position_tolerance = 0.75;

[[nodiscard]] Percentiles percentiles(std::vector<double> sample);

[[nodiscard]] RunAnalysis analyze(const RunRecording& recording, const RefreshClock& clock);

// Everything the result files say about a run besides its records.
struct RunDescription final {
    std::string platform{};
    std::string input_path{};
    std::string input_api{};
    std::string injection_unavailable_reason{};
    std::string profile{};
    double interval_ms{};
    double step_units{};
    std::size_t steps{};
    std::string display_source{};
    double window_scale{};
    // Empty for a measured run; otherwise why the run could not be measured.
    std::string error{};
};

[[nodiscard]] const char* verdict_name(const RunDescription& description, const RunRecording& recording);

// One JSON document for one run, records included. False when writing
// failed.
[[nodiscard]] bool write_run_json(std::FILE* file, const RunDescription& description,
                                  const RunRecording& recording, const RunAnalysis& analysis);

struct RunSummary final {
    RunDescription description{};
    DragTotals totals{};
    RunAnalysis analysis{};
    std::string verdict{};
    std::string json_file{};
};

// A short Markdown table of every run. False when writing failed.
[[nodiscard]] bool write_summary_markdown(std::FILE* file, const std::vector<RunSummary>& runs);

} // namespace gui_forms::probe
