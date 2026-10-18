#include "frame_drag_analysis.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <utility>

namespace gui_forms::probe {
namespace {

constexpr std::uint64_t assumed_period_nanoseconds = 16'666'667U;

double distance(Position left, Position right) {
    const double dx = left.x - right.x;
    const double dy = left.y - right.y;
    const double result = std::sqrt(dx * dx + dy * dy);
    return result;
}

double milliseconds_between(std::uint64_t earlier, std::uint64_t later) {
    const double nanoseconds =
        later >= earlier ? static_cast<double>(later - earlier) : -static_cast<double>(earlier - later);
    const double result = nanoseconds / 1.0e6;
    return result;
}

// Refreshes in a span, rounded to the nearest whole refresh.
std::size_t refreshes_in(std::uint64_t begin, std::uint64_t end, std::uint64_t period) {
    if (end <= begin || period == 0U) return 0U;
    const double count = static_cast<double>(end - begin) / static_cast<double>(period);
    const std::size_t result = static_cast<std::size_t>(std::floor(count + 0.5));
    return result;
}

// The input that produced a move: among the steps issued before the move
// arrived, the one whose pointer is nearest to where the move found it; the
// latest wins a tie.
bool match_input(const RunRecording& recording, const MoveRecord& move, std::size_t& input_index) {
    bool found = false;
    double best = 0.0;
    for (std::size_t index = 0U; index < recording.inputs.size(); ++index) {
        const InputRecord& input = recording.inputs[index];
        if (input.issued_nanoseconds > move.received_nanoseconds) break;
        if (!input.accepted) continue;
        const double gap = distance(input.pointer, move.pointer);
        if (!found || gap <= best) {
            found = true;
            best = gap;
            input_index = index;
        }
    }
    return found;
}

std::uint64_t relative(const RunRecording& recording, std::uint64_t time) {
    const std::uint64_t result = time >= recording.begin_nanoseconds ? time - recording.begin_nanoseconds : 0U;
    return result;
}

void write_escaped(std::FILE* file, const std::string& text) {
    std::fputc('"', file);
    for (const char character : text) {
        const unsigned char code = static_cast<unsigned char>(character);
        if (character == '"' || character == '\\') {
            std::fputc('\\', file);
            std::fputc(character, file);
        } else if (code < 0x20U) {
            std::fprintf(file, "\\u%04x", static_cast<unsigned int>(code));
        } else {
            std::fputc(character, file);
        }
    }
    std::fputc('"', file);
}

void write_percentiles(std::FILE* file, const char* name, const Percentiles& value) {
    std::fprintf(file, "\"%s\":{\"count\":%zu,\"p50\":%.3f,\"p95\":%.3f,\"p99\":%.3f,\"max\":%.3f}", name,
                 value.count, value.p50, value.p95, value.p99, value.max);
}

} // namespace

void RunRecording::reserve(std::size_t steps) {
    // A step yields at most one input, one move and, with late reports, a
    // few confirmations; the margin covers the press, release and settling.
    const std::size_t capacity = steps + 16U;
    inputs.reserve(capacity);
    moves.reserve(capacity);
    confirmations.reserve(capacity * 2U);
}

void RunRecording::clear() noexcept {
    inputs.clear();
    moves.clear();
    confirmations.clear();
    begin_nanoseconds = 0U;
    end_nanoseconds = 0U;
    totals = {};
}

Percentiles percentiles(std::vector<double> sample) {
    Percentiles result{};
    result.count = sample.size();
    if (sample.empty()) return result;
    std::sort(sample.begin(), sample.end());
    const std::size_t count = sample.size();
    // Nearest rank: the smallest value with at least p of the sample at or
    // below it.
    const std::array<double, 3> ranks{0.50, 0.95, 0.99};
    std::array<double, 3> values{0.0, 0.0, 0.0};
    for (std::size_t which = 0U; which < 3U; ++which) {
        const double rank = std::ceil(ranks[which] * static_cast<double>(count));
        const std::size_t position = rank < 1.0 ? 0U : static_cast<std::size_t>(rank) - 1U;
        values[which] = sample[std::min(position, count - 1U)];
    }
    result.p50 = values[0];
    result.p95 = values[1];
    result.p99 = values[2];
    result.max = sample[count - 1U];
    return result;
}

RunAnalysis analyze(const RunRecording& recording, const RefreshClock& clock) {
    RunAnalysis result{};
    result.clock = clock;
    if (result.clock.period_nanoseconds == 0U) {
        result.clock.period_nanoseconds = assumed_period_nanoseconds;
        result.clock.period_measured = false;
    }
    const std::uint64_t period = result.clock.period_nanoseconds;
    result.inputs_issued = recording.inputs.size();
    result.moves_received = recording.moves.size();

    // The requests, in order, as indices into the moves.
    std::vector<std::size_t> requests{};
    requests.reserve(recording.moves.size());
    for (std::size_t index = 0U; index < recording.moves.size(); ++index) {
        const MoveRecord& move = recording.moves[index];
        if (move.requested && !move.refused) requests.push_back(index);
    }
    result.requests = requests.size();
    result.outcomes.resize(requests.size());
    for (std::size_t request = 0U; request < requests.size(); ++request) {
        RequestOutcome& outcome = result.outcomes[request];
        outcome.move_index = requests[request];
        const MoveRecord& move = recording.moves[outcome.move_index];
        outcome.input_matched = match_input(recording, move, outcome.input_index);
        if (outcome.input_matched) {
            const InputRecord& input = recording.inputs[outcome.input_index];
            outcome.input_to_request_ms = milliseconds_between(input.issued_nanoseconds, move.requested_nanoseconds);
        }
    }

    // Each report confirms the latest request at its position issued before
    // it, and every earlier unconfirmed request with it (those the host
    // coalesced). The refresh accounting walks the same reports.
    std::vector<double> position_errors{};
    position_errors.reserve(recording.confirmations.size());
    std::size_t next_unconfirmed = 0U;
    std::size_t issued_before = 0U;
    bool any_advance = false;
    std::uint64_t previous_time = 0U;
    std::uint64_t previous_count = 0U;
    for (std::size_t index = 0U; index < recording.confirmations.size(); ++index) {
        const ConfirmRecord& report = recording.confirmations[index];
        while (issued_before < requests.size() &&
               recording.moves[requests[issued_before]].requested_nanoseconds <= report.confirmed_nanoseconds) {
            ++issued_before;
        }
        if (issued_before == 0U) continue;
        const Position latest = recording.moves[requests[issued_before - 1U]].target;
        position_errors.push_back(distance(report.origin, latest));
        bool matched = false;
        std::size_t matched_request = 0U;
        for (std::size_t candidate = issued_before; candidate > 0U; --candidate) {
            const Position target = recording.moves[requests[candidate - 1U]].target;
            if (distance(target, report.origin) <= position_tolerance) {
                matched = true;
                matched_request = candidate - 1U;
                break;
            }
        }
        if (!matched) {
            ++result.foreign_confirmations;
            continue;
        }
        // A repeated report of a position already confirmed advances nothing.
        if (matched_request < next_unconfirmed) continue;
        // Refreshes that passed while a request was waiting, minus the one
        // that showed this report.
        const std::uint64_t waiting_since =
            recording.moves[requests[next_unconfirmed]].requested_nanoseconds;
        const std::uint64_t gap_start = any_advance ? std::max(previous_time, waiting_since) : waiting_since;
        std::size_t frames = refreshes_in(gap_start, report.confirmed_nanoseconds, period);
        const bool counted = result.clock.counter_available && any_advance && gap_start == previous_time &&
            report.refresh_count >= previous_count && previous_count != 0U;
        if (counted) frames = static_cast<std::size_t>(report.refresh_count - previous_count);
        if (frames > 1U) result.missed_refreshes += frames - 1U;
        for (std::size_t request = next_unconfirmed; request <= matched_request; ++request) {
            RequestOutcome& outcome = result.outcomes[request];
            const MoveRecord& move = recording.moves[outcome.move_index];
            outcome.confirmed = true;
            outcome.exact = request == matched_request;
            outcome.confirmation_index = index;
            outcome.request_to_confirm_ms =
                milliseconds_between(move.requested_nanoseconds, report.confirmed_nanoseconds);
            if (outcome.input_matched) {
                const InputRecord& input = recording.inputs[outcome.input_index];
                outcome.input_to_confirm_ms =
                    milliseconds_between(input.issued_nanoseconds, report.confirmed_nanoseconds);
            }
            if (outcome.exact) ++result.exact_confirmations;
            else ++result.coalesced_requests;
        }
        next_unconfirmed = matched_request + 1U;
        any_advance = true;
        previous_time = report.confirmed_nanoseconds;
        previous_count = report.refresh_count;
    }
    result.unconfirmed_requests = requests.size() - next_unconfirmed;
    if (next_unconfirmed < requests.size()) {
        // The last requests were never shown: every refresh after they were
        // issued, to the end of the run, missed them.
        const std::uint64_t waiting_since =
            recording.moves[requests[next_unconfirmed]].requested_nanoseconds;
        const std::uint64_t gap_start = any_advance ? std::max(previous_time, waiting_since) : waiting_since;
        result.missed_refreshes += refreshes_in(gap_start, recording.end_nanoseconds, period);
    }
    if (!requests.empty()) {
        const std::uint64_t first = recording.moves[requests[0]].requested_nanoseconds;
        result.refresh_intervals = refreshes_in(first, recording.end_nanoseconds, period);
    }
    if (!requests.empty() && !recording.confirmations.empty()) {
        const Position last_target = recording.moves[requests.back()].target;
        result.final_position_error = distance(recording.confirmations.back().origin, last_target);
    }

    std::vector<double> request_to_confirm{};
    std::vector<double> input_to_request{};
    std::vector<double> input_to_confirm{};
    request_to_confirm.reserve(result.outcomes.size());
    input_to_request.reserve(result.outcomes.size());
    input_to_confirm.reserve(result.outcomes.size());
    for (const RequestOutcome& outcome : result.outcomes) {
        if (outcome.input_matched) input_to_request.push_back(outcome.input_to_request_ms);
        if (!outcome.confirmed) continue;
        request_to_confirm.push_back(outcome.request_to_confirm_ms);
        if (outcome.input_matched) input_to_confirm.push_back(outcome.input_to_confirm_ms);
    }
    result.request_to_confirm_ms = percentiles(std::move(request_to_confirm));
    result.input_to_request_ms = percentiles(std::move(input_to_request));
    result.input_to_confirm_ms = percentiles(std::move(input_to_confirm));
    result.position_error = percentiles(std::move(position_errors));
    return result;
}

const char* verdict_name(const RunDescription& description, const RunRecording& recording) {
    if (!description.error.empty() || recording.totals.frame_requests == 0U) return "not_measured";
    const char* result = recording.totals.keeps_up ? "keeps_up" : "falls_behind";
    return result;
}

bool write_run_json(std::FILE* file, const RunDescription& description, const RunRecording& recording,
                    const RunAnalysis& analysis) {
    std::fputs("{\"probe\":\"frame_drag_forensic\",\"schema\":1,\"platform\":", file);
    write_escaped(file, description.platform);
    std::fputs(",\"input_path\":", file);
    write_escaped(file, description.input_path);
    std::fputs(",\"input_api\":", file);
    write_escaped(file, description.input_api);
    std::fputs(",\"os_injection_unavailable\":", file);
    write_escaped(file, description.injection_unavailable_reason);
    std::fputs(",\"profile\":", file);
    write_escaped(file, description.profile);
    std::fprintf(file, ",\"interval_ms\":%.3f,\"step_units\":%.3f,\"steps\":%zu,\"window_scale\":%.4f",
                 description.interval_ms, description.step_units, description.steps, description.window_scale);
    std::fputs(",\"display\":{\"source\":", file);
    write_escaped(file, description.display_source);
    std::fprintf(file, ",\"refresh_period_ns\":%llu,\"period_measured\":%s,\"refresh_counter\":%s}",
                 static_cast<unsigned long long>(analysis.clock.period_nanoseconds),
                 analysis.clock.period_measured ? "true" : "false",
                 analysis.clock.counter_available ? "true" : "false");
    std::fputs(",\"error\":", file);
    write_escaped(file, description.error);
    std::fputs(",\"verdict\":", file);
    write_escaped(file, verdict_name(description, recording));
    const DragTotals& totals = recording.totals;
    std::fprintf(file,
                 ",\"window_frame_drag\":{\"pointer_moves\":%llu,\"frame_requests\":%llu,\"refused\":%llu,"
                 "\"frame_reports\":%llu,\"duration_s\":%.4f,\"pointer_hz\":%.2f,\"frames_hz\":%.2f,"
                 "\"keeps_up\":%s}",
                 static_cast<unsigned long long>(totals.pointer_moves),
                 static_cast<unsigned long long>(totals.frame_requests),
                 static_cast<unsigned long long>(totals.refused_requests),
                 static_cast<unsigned long long>(totals.frame_reports), totals.duration_seconds,
                 totals.pointer_rate_hz, totals.achieved_rate_hz, totals.keeps_up ? "true" : "false");
    std::fprintf(file,
                 ",\"counts\":{\"inputs\":%zu,\"moves\":%zu,\"requests\":%zu,\"exact\":%zu,\"coalesced\":%zu,"
                 "\"unconfirmed\":%zu,\"foreign_reports\":%zu,\"refresh_intervals\":%zu,"
                 "\"missed_refreshes\":%zu}",
                 analysis.inputs_issued, analysis.moves_received, analysis.requests, analysis.exact_confirmations,
                 analysis.coalesced_requests, analysis.unconfirmed_requests, analysis.foreign_confirmations,
                 analysis.refresh_intervals, analysis.missed_refreshes);
    std::fputs(",\"latency_ms\":{", file);
    write_percentiles(file, "request_to_frame", analysis.request_to_confirm_ms);
    std::fputc(',', file);
    write_percentiles(file, "input_to_request", analysis.input_to_request_ms);
    std::fputc(',', file);
    write_percentiles(file, "input_to_frame", analysis.input_to_confirm_ms);
    std::fputs("},\"position_error_units\":{", file);
    write_percentiles(file, "report_to_latest_target", analysis.position_error);
    std::fprintf(file, ",\"final\":%.3f}", analysis.final_position_error);
    // Records, times in nanoseconds from the drag's begin.
    std::fputs(",\"inputs\":[", file);
    for (std::size_t index = 0U; index < recording.inputs.size(); ++index) {
        const InputRecord& input = recording.inputs[index];
        std::fprintf(file, "%s[%llu,%.3f,%.3f,%s]", index == 0U ? "" : ",",
                     static_cast<unsigned long long>(relative(recording, input.issued_nanoseconds)), input.pointer.x,
                     input.pointer.y, input.accepted ? "true" : "false");
    }
    std::fputs("],\"requests\":[", file);
    for (std::size_t index = 0U; index < analysis.outcomes.size(); ++index) {
        const RequestOutcome& outcome = analysis.outcomes[index];
        const MoveRecord& move = recording.moves[outcome.move_index];
        std::fprintf(file, "%s{\"received_ns\":%llu,\"request_ns\":%llu,\"target\":[%.3f,%.3f]",
                     index == 0U ? "" : ",",
                     static_cast<unsigned long long>(relative(recording, move.received_nanoseconds)),
                     static_cast<unsigned long long>(relative(recording, move.requested_nanoseconds)), move.target.x,
                     move.target.y);
        if (outcome.input_matched) {
            const InputRecord& input = recording.inputs[outcome.input_index];
            std::fprintf(file, ",\"input_ns\":%llu",
                         static_cast<unsigned long long>(relative(recording, input.issued_nanoseconds)));
        }
        std::fprintf(file, ",\"confirmed\":%s,\"exact\":%s", outcome.confirmed ? "true" : "false",
                     outcome.exact ? "true" : "false");
        if (outcome.confirmed) {
            const ConfirmRecord& report = recording.confirmations[outcome.confirmation_index];
            std::fprintf(file, ",\"frame_ns\":%llu,\"frame_at\":[%.3f,%.3f],\"refresh_count\":%llu,\"vblank_ns\":%llu",
                         static_cast<unsigned long long>(relative(recording, report.confirmed_nanoseconds)),
                         report.origin.x, report.origin.y, static_cast<unsigned long long>(report.refresh_count),
                         static_cast<unsigned long long>(report.vblank_nanoseconds == 0U
                                                             ? 0U
                                                             : relative(recording, report.vblank_nanoseconds)));
        }
        std::fputc('}', file);
    }
    std::fputs("],\"reports\":[", file);
    for (std::size_t index = 0U; index < recording.confirmations.size(); ++index) {
        const ConfirmRecord& report = recording.confirmations[index];
        std::fprintf(file, "%s[%llu,%.3f,%.3f,%llu]", index == 0U ? "" : ",",
                     static_cast<unsigned long long>(relative(recording, report.confirmed_nanoseconds)),
                     report.origin.x, report.origin.y, static_cast<unsigned long long>(report.refresh_count));
    }
    std::fputs("]}\n", file);
    const bool written = std::ferror(file) == 0;
    return written;
}

bool write_summary_markdown(std::FILE* file, const std::vector<RunSummary>& runs) {
    std::fputs("# Frame-drag forensic probe\n\n", file);
    if (!runs.empty()) {
        const RunDescription& first = runs.front().description;
        std::fprintf(file, "Platform `%s`, input `%s` (%s), display timing: %s.\n\n", first.platform.c_str(),
                     first.input_path.c_str(), first.input_api.c_str(), first.display_source.c_str());
        if (!first.injection_unavailable_reason.empty()) {
            std::fprintf(file, "OS injection was not used: %s.\n\n", first.injection_unavailable_reason.c_str());
        }
    }
    std::fputs("| Profile | Pointer Hz | Frames Hz | Requests | Exact / coalesced / unconfirmed | "
               "Request to frame ms (p50 / p95 / p99) | Input to frame p95 ms | Missed refreshes | "
               "Max error | Verdict |\n",
               file);
    std::fputs("|---|---|---|---|---|---|---|---|---|---|\n", file);
    for (const RunSummary& run : runs) {
        const RunAnalysis& analysis = run.analysis;
        std::fprintf(file, "| %s | %.1f | %.1f | %zu | %zu / %zu / %zu | %.2f / %.2f / %.2f | %.2f | %zu of %zu | "
                           "%.2f | %s |\n",
                     run.description.profile.c_str(), run.totals.pointer_rate_hz, run.totals.achieved_rate_hz,
                     analysis.requests, analysis.exact_confirmations, analysis.coalesced_requests,
                     analysis.unconfirmed_requests, analysis.request_to_confirm_ms.p50,
                     analysis.request_to_confirm_ms.p95, analysis.request_to_confirm_ms.p99,
                     analysis.input_to_confirm_ms.p95, analysis.missed_refreshes, analysis.refresh_intervals,
                     analysis.position_error.max, run.verdict.c_str());
    }
    for (const RunSummary& run : runs) {
        if (!run.description.error.empty()) {
            std::fprintf(file, "\n%s: not measured: %s\n", run.description.profile.c_str(),
                         run.description.error.c_str());
        }
    }
    std::fputs("\nFrames are the host's frame reports (Windows WM_MOVE, macOS windowDidMove, "
               "X11 ConfigureNotify) as the probe received them; they show the window manager accepted "
               "the position, not that a refresh displayed it. Where the period is not measured the "
               "probe assumes 60 Hz.\n",
               file);
    const bool written = std::ferror(file) == 0;
    return written;
}

} // namespace gui_forms::probe
