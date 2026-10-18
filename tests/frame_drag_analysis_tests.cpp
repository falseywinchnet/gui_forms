// The forensic frame-drag probe's analysis (stage DK decision 49), headless:
// percentiles, matching host frame reports to requests (exact and
// coalesced), unconfirmed requests, reports at positions nobody asked for,
// missed refreshes from time and from a compositor counter, input
// matching, verdicts and the JSON writer. The probe itself needs a desktop
// and is not run by ctest.
#include "frame_drag_probe/frame_drag_analysis.hpp"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {
using namespace gui_forms::probe;

void require(bool condition, const char* message) {
    if (condition) return;
    std::cerr << message << '\n';
    std::exit(1);
}

bool near(double left, double right) {
    const bool close = std::fabs(left - right) < 1.0e-6;
    return close;
}

constexpr std::uint64_t ms = 1000000U;
constexpr std::uint64_t base = 1000U * ms;

RefreshClock sixty_hertz() {
    RefreshClock clock{};
    clock.period_nanoseconds = 16666667U;
    clock.period_measured = true;
    return clock;
}

void add_request(RunRecording& recording, std::uint64_t at, double x, double y) {
    MoveRecord move{};
    move.received_nanoseconds = base + at;
    move.pointer = {x, y};
    move.requested = true;
    move.requested_nanoseconds = base + at;
    move.target = {x, y};
    recording.moves.push_back(move);
}

void add_report(RunRecording& recording, std::uint64_t at, double x, double y, std::uint64_t count = 0U) {
    ConfirmRecord report{};
    report.confirmed_nanoseconds = base + at;
    report.origin = {x, y};
    report.refresh_count = count;
    recording.confirmations.push_back(report);
}

RunRecording started(std::uint64_t end) {
    RunRecording recording{};
    recording.reserve(32U);
    recording.begin_nanoseconds = base;
    recording.end_nanoseconds = base + end;
    return recording;
}

void percentiles_use_nearest_rank() {
    std::vector<double> sample{};
    for (int value = 100; value >= 1; --value) sample.push_back(static_cast<double>(value));
    const Percentiles result = percentiles(sample);
    require(result.count == 100U && near(result.p50, 50.0) && near(result.p95, 95.0) && near(result.p99, 99.0) &&
                near(result.max, 100.0),
            "nearest-rank percentiles of 1 to 100");
    const Percentiles empty = percentiles({});
    require(empty.count == 0U && near(empty.max, 0.0), "an empty sample gives zeros");
    const Percentiles single = percentiles({7.0});
    require(near(single.p50, 7.0) && near(single.p99, 7.0), "one value is every percentile");
}

void every_request_reported_exactly() {
    RunRecording recording = started(40U * ms);
    add_request(recording, 0U, 0.0, 0.0);
    add_request(recording, 10U * ms, 10.0, 0.0);
    add_request(recording, 20U * ms, 20.0, 0.0);
    add_report(recording, 4U * ms, 0.0, 0.0);
    add_report(recording, 14U * ms, 10.0, 0.0);
    add_report(recording, 24U * ms, 20.0, 0.0);
    const RunAnalysis analysis = analyze(recording, sixty_hertz());
    require(analysis.requests == 3U && analysis.exact_confirmations == 3U && analysis.coalesced_requests == 0U &&
                analysis.unconfirmed_requests == 0U && analysis.foreign_confirmations == 0U,
            "three requests, three exact reports");
    require(near(analysis.request_to_confirm_ms.p50, 4.0) && near(analysis.request_to_confirm_ms.max, 4.0),
            "each report arrives 4 ms after its request");
    require(analysis.missed_refreshes == 0U, "no refresh passes without the window moving");
    require(near(analysis.final_position_error, 0.0) && near(analysis.position_error.max, 0.0),
            "the window ends where it was asked to be");
}

void a_later_report_confirms_coalesced_requests() {
    RunRecording recording = started(20U * ms);
    add_request(recording, 0U, 0.0, 0.0);
    add_request(recording, 4U * ms, 5.0, 0.0);
    add_request(recording, 8U * ms, 10.0, 0.0);
    add_request(recording, 12U * ms, 15.0, 0.0);
    add_report(recording, 14U * ms, 15.0, 0.0);
    const RunAnalysis analysis = analyze(recording, sixty_hertz());
    require(analysis.exact_confirmations == 1U && analysis.coalesced_requests == 3U &&
                analysis.unconfirmed_requests == 0U,
            "one report confirms the last request and the three it superseded");
    require(near(analysis.outcomes[0].request_to_confirm_ms, 14.0) &&
                near(analysis.outcomes[3].request_to_confirm_ms, 2.0) && !analysis.outcomes[0].exact &&
                analysis.outcomes[3].exact,
            "coalesced requests take the superseding report's time");
    require(analysis.missed_refreshes == 0U, "a report within one refresh misses nothing");
}

void late_and_missing_reports_miss_refreshes() {
    RunRecording recording = started(120U * ms);
    add_request(recording, 0U, 0.0, 0.0);
    add_request(recording, 10U * ms, 10.0, 0.0);
    add_report(recording, 60U * ms, 10.0, 0.0);
    add_request(recording, 70U * ms, 20.0, 0.0);
    const RunAnalysis analysis = analyze(recording, sixty_hertz());
    require(analysis.unconfirmed_requests == 1U && analysis.coalesced_requests == 1U &&
                analysis.exact_confirmations == 1U,
            "the last request is never reported");
    // 60 ms is about 4 refreshes, so 3 passed with the window behind; the
    // unreported request waits 50 ms, 3 refreshes, to the end.
    require(analysis.missed_refreshes == 6U, "missed refreshes count both the late and the missing report");
    require(analysis.refresh_intervals == 7U, "120 ms is about 7 refreshes");
    require(near(analysis.final_position_error, 10.0), "the window ends 10 units short");
}

void reports_nobody_asked_for_are_foreign() {
    RunRecording recording = started(30U * ms);
    add_request(recording, 0U, 0.0, 0.0);
    add_report(recording, 5U * ms, 500.0, 300.0);
    add_report(recording, 10U * ms, 0.0, 0.0);
    add_report(recording, 12U * ms, 0.0, 0.0);
    const RunAnalysis analysis = analyze(recording, sixty_hertz());
    require(analysis.foreign_confirmations == 1U && analysis.exact_confirmations == 1U,
            "a clamped position is foreign and a repeated report advances nothing");
    require(near(analysis.position_error.max, std::sqrt(500.0 * 500.0 + 300.0 * 300.0)),
            "the foreign report's distance is the largest error");
}

void reports_before_any_request_are_ignored() {
    RunRecording recording = started(30U * ms);
    add_report(recording, 0U, 3.0, 3.0);
    add_request(recording, 5U * ms, 10.0, 0.0);
    const RunAnalysis analysis = analyze(recording, sixty_hertz());
    require(analysis.foreign_confirmations == 0U && analysis.position_error.count == 0U &&
                analysis.unconfirmed_requests == 1U,
            "a report before the first request is not an error");
}

void a_compositor_counter_counts_refreshes() {
    RefreshClock clock = sixty_hertz();
    clock.counter_available = true;
    RunRecording recording = started(80U * ms);
    add_request(recording, 0U, 0.0, 0.0);
    add_report(recording, 5U * ms, 0.0, 0.0, 100U);
    add_request(recording, 6U * ms, 10.0, 0.0);
    add_report(recording, 40U * ms, 10.0, 0.0, 103U);
    const RunAnalysis analysis = analyze(recording, clock);
    // The second request waited since 6 ms, after the first report, so time
    // decides (34 ms, two refreshes, one missed), not the counter.
    require(analysis.missed_refreshes == 1U, "a gap that starts after the last report is timed");
    RunRecording waiting = started(80U * ms);
    add_request(waiting, 0U, 0.0, 0.0);
    add_request(waiting, 2U * ms, 5.0, 0.0);
    add_report(waiting, 3U * ms, 0.0, 0.0, 100U);
    add_report(waiting, 40U * ms, 5.0, 0.0, 103U);
    const RunAnalysis counted = analyze(waiting, clock);
    require(counted.missed_refreshes == 2U, "three counted refreshes for one move miss two");
}

void inputs_match_the_nearest_pointer() {
    RunRecording recording = started(30U * ms);
    InputRecord first{};
    first.issued_nanoseconds = base;
    first.pointer = {0.0, 0.0};
    first.accepted = true;
    InputRecord second = first;
    second.issued_nanoseconds = base + 2U * ms;
    second.pointer = {10.0, 0.0};
    recording.inputs.push_back(first);
    recording.inputs.push_back(second);
    add_request(recording, 5U * ms, 10.0, 0.0);
    add_report(recording, 9U * ms, 10.0, 0.0);
    const RunAnalysis analysis = analyze(recording, sixty_hertz());
    require(analysis.outcomes[0].input_matched && analysis.outcomes[0].input_index == 1U,
            "the move matches the input that put the pointer there");
    require(near(analysis.input_to_request_ms.p50, 3.0) && near(analysis.input_to_confirm_ms.p50, 7.0),
            "input latency runs from that input");
}

void an_unmeasured_period_assumes_sixty_hertz() {
    RunRecording recording = started(10U * ms);
    const RunAnalysis analysis = analyze(recording, RefreshClock{});
    require(analysis.clock.period_nanoseconds == 16666667U && !analysis.clock.period_measured,
            "the assumed period is 60 Hz and marked as assumed");
    require(analysis.requests == 0U && analysis.missed_refreshes == 0U, "an empty run has no outcomes");
}

void verdicts_and_json() {
    RunRecording recording = started(40U * ms);
    add_request(recording, 0U, 0.0, 0.0);
    add_report(recording, 4U * ms, 0.0, 0.0);
    recording.totals.frame_requests = 1U;
    recording.totals.frame_reports = 1U;
    recording.totals.keeps_up = true;
    RunDescription description{};
    description.platform = "test";
    description.input_path = "in_process";
    description.profile = "slow \"quoted\"";
    require(std::string(verdict_name(description, recording)) == "keeps_up", "a kept-up run");
    recording.totals.keeps_up = false;
    require(std::string(verdict_name(description, recording)) == "falls_behind", "a run that fell behind");
    description.error = "no press";
    require(std::string(verdict_name(description, recording)) == "not_measured", "a failed run");
    description.error.clear();
    const RunAnalysis analysis = analyze(recording, sixty_hertz());
    std::FILE* const file = std::tmpfile();
    require(file != nullptr, "a temporary file");
    require(write_run_json(file, description, recording, analysis), "the JSON is written");
    std::rewind(file);
    std::string text{};
    char buffer[512];
    std::size_t read = 0U;
    while ((read = std::fread(buffer, 1U, sizeof(buffer), file)) > 0U) text.append(buffer, read);
    std::fclose(file);
    require(text.rfind("{\"probe\":\"frame_drag_forensic\",\"schema\":1,", 0U) == 0U, "the document's header");
    require(text.find("\"profile\":\"slow \\\"quoted\\\"\"") != std::string::npos, "strings are escaped");
    require(text.find("\"verdict\":\"falls_behind\"") != std::string::npos, "the verdict is recorded");
    require(text.find("\"requests\":[{\"received_ns\":0,\"request_ns\":0,\"target\":[0.000,0.000]") !=
                std::string::npos,
            "records are relative to the drag's begin");
    require(text.back() == '\n' && text[text.size() - 2U] == '}', "the document is closed");
    std::vector<RunSummary> runs(1U);
    runs[0].description = description;
    runs[0].analysis = analysis;
    runs[0].verdict = "falls_behind";
    std::FILE* const summary = std::tmpfile();
    require(summary != nullptr && write_summary_markdown(summary, runs), "the summary is written");
    std::fclose(summary);
}

} // namespace

int main() {
    percentiles_use_nearest_rank();
    every_request_reported_exactly();
    a_later_report_confirms_coalesced_requests();
    late_and_missing_reports_miss_refreshes();
    reports_nobody_asked_for_are_foreign();
    reports_before_any_request_are_ignored();
    a_compositor_counter_counts_refreshes();
    inputs_match_the_nearest_pointer();
    an_unmeasured_period_assumes_sixty_hertz();
    verdicts_and_json();
    std::cout << "frame drag analysis tests passed\n";
    return 0;
}
