#include "font_metrics_differential.hpp"
#include "gui_forms/drawing/font_substitution/font_substitution.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

// Stage F's metric gate (step F4; owner decision F4-1, brief decision 18). For
// every committed reference snapshot the bundled engine measures the candidate
// the substitution table names, on this host, and compares it with the
// reference's snapshot under the mapping class's tolerances and the table's
// named exceptions. The candidate's measurement must reproduce the digest
// recorded when the snapshot was made, so every host measures the same thing.
// On Windows an installed reference whose SHA-256 matches its snapshot is
// measured again and must reproduce the snapshot exactly.
// Arguments: the bundled font directory, the corpus file and the snapshot
// directory.

namespace {

namespace font_metrics = gui_forms::tools::font_metrics;

constexpr std::size_t maximum_snapshot_bytes = font_metrics::maximum_corpus_bytes * 4U;

void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

gui_drawing::FontFaceStyle style_of(const std::string& name) {
    if (name == "regular") return gui_drawing::FontFaceStyle::regular;
    if (name == "bold") return gui_drawing::FontFaceStyle::bold;
    if (name == "italic") return gui_drawing::FontFaceStyle::italic;
    if (name == "bold_italic") return gui_drawing::FontFaceStyle::bold_italic;
    throw std::runtime_error("unknown snapshot style " + name);
}

font_metrics::MappingClass mapping_of(const gui_drawing::FontMappingClass mapping, const std::string& request) {
    if (mapping == gui_drawing::FontMappingClass::metric_compatible) return font_metrics::MappingClass::metric_compatible;
    if (mapping == gui_drawing::FontMappingClass::nearest) return font_metrics::MappingClass::nearest;
    throw std::runtime_error(request + " has no mapping class");
}

// One criterion's result for one snapshot, in the criterion's own unit (see
// FontGateCriterion).
struct CriterionResult final {
    gui_drawing::FontGateCriterion criterion{gui_drawing::FontGateCriterion::advances};
    font_metrics::Criterion verdict{font_metrics::Criterion::not_measured};
    double measured{};
};

std::vector<CriterionResult> criterion_results(const font_metrics::Differential& differential) {
    std::size_t differing_break_sets = 0U;
    double largest_height_difference = 0.0;
    double largest_mean_width = 0.0;
    for (const font_metrics::SizeDifferential& size : differential.sizes) {
        differing_break_sets += size.line_break_sets - size.line_break_sets_identical;
        const double height_difference = std::fabs(size.candidate_line_height_px - size.reference_line_height_px);
        largest_height_difference = std::max(largest_height_difference, height_difference);
        largest_mean_width = std::max(largest_mean_width, std::fabs(size.mean_width_deviation));
    }
    const std::size_t differing_advances = differential.advances_compared - differential.advances_identical;
    std::vector<CriterionResult> results{};
    results.push_back(CriterionResult{gui_drawing::FontGateCriterion::advances, differential.advances,
                                      static_cast<double>(differing_advances)});
    results.push_back(CriterionResult{gui_drawing::FontGateCriterion::line_breaks, differential.line_breaks,
                                      static_cast<double>(differing_break_sets)});
    results.push_back(CriterionResult{gui_drawing::FontGateCriterion::line_height, differential.line_height,
                                      largest_height_difference});
    results.push_back(CriterionResult{gui_drawing::FontGateCriterion::line_width, differential.line_width,
                                      largest_mean_width});
    return results;
}

// The table's exception for one request, style and criterion, or null.
const gui_drawing::FontGateException* exception_for(const std::string& request, gui_drawing::FontFaceStyle style,
                                                     gui_drawing::FontGateCriterion criterion) {
    for (const gui_drawing::FontGateException& exception : gui_drawing::font_gate_exceptions()) {
        if (exception.requested == request && exception.style == style && exception.criterion == criterion) {
            return &exception;
        }
    }
    return nullptr;
}

const char* verdict_name(font_metrics::Criterion verdict) {
    switch (verdict) {
    case font_metrics::Criterion::passed: return "passed";
    case font_metrics::Criterion::failed: return "failed";
    case font_metrics::Criterion::not_measured: return "not_measured";
    case font_metrics::Criterion::recorded: return "recorded";
    }
    return "unknown";
}

// The installed reference's directory, or empty: Windows only.
std::filesystem::path installed_font_directory() {
#if defined(_WIN32)
    const char* windows = std::getenv("WINDIR");
    if (windows != nullptr && windows[0] != '\0') return std::filesystem::path(windows) / "Fonts";
#endif
    return {};
}

struct GateTotals final {
    std::size_t snapshots{};
    std::size_t exceptions_used{};
    std::size_t references_remeasured{};
    std::vector<std::string> requests_covered{};
};

// Checks one snapshot; throws on any failure. Prints one evidence line.
void check_snapshot(const std::filesystem::path& path, const std::filesystem::path& fonts,
                    const font_metrics::Corpus& corpus, const std::string& corpus_digest, GateTotals& totals) {
    const std::string name = path.filename().string();
    const std::vector<std::byte> snapshot_bytes = font_metrics::read_bounded_file(path, maximum_snapshot_bytes);
    const std::string_view snapshot_text(reinterpret_cast<const char*>(snapshot_bytes.data()), snapshot_bytes.size());
    const font_metrics::MetricSnapshot snapshot = font_metrics::parse_snapshot(snapshot_text);
    require(snapshot.corpus_digest == corpus_digest, name + ": measured with a different corpus");

    const gui_drawing::FontFaceStyle style = style_of(snapshot.request_style);
    const gui_drawing::FontFamilyResolution resolution =
        gui_drawing::resolve_font_family(snapshot.request_family, style);
    require(resolution.source == gui_drawing::FontResolutionSource::substitution,
            name + ": the table does not substitute " + snapshot.request_family);
    require(resolution.face_style == style, name + ": the candidate family lacks the requested face");
    require(resolution.face_file == snapshot.candidate_file,
            name + ": the table names " + std::string(resolution.face_file) + ", the snapshot " +
                snapshot.candidate_file);
    const font_metrics::MappingClass mapping = mapping_of(resolution.mapping, snapshot.request_family);

    const std::vector<std::byte> candidate_bytes =
        font_metrics::read_bounded_file(fonts / snapshot.candidate_file, font_metrics::maximum_face_bytes);
    font_metrics::validate_face_bytes(candidate_bytes);
    const font_metrics::FaceMeasurement candidate =
        font_metrics::measure_face(snapshot.candidate_file, candidate_bytes, corpus, false);
    const std::string candidate_digest = font_metrics::measurement_digest(candidate);
    require(candidate_digest == snapshot.candidate_digest,
            name + ": this host measures " + snapshot.candidate_file + " as " + candidate_digest +
                ", the snapshot recorded " + snapshot.candidate_digest);

    const font_metrics::Differential differential = font_metrics::compare_faces(snapshot.reference, candidate, mapping);
    std::string evidence = name + " " + gui_drawing::font_mapping_class_name(resolution.mapping) + " " +
                           snapshot.candidate_file;
    for (const CriterionResult& result : criterion_results(differential)) {
        const char* criterion_name = gui_drawing::font_gate_criterion_name(result.criterion);
        const gui_drawing::FontGateException* exception =
            exception_for(snapshot.request_family, style, result.criterion);
        evidence += std::string(" ") + criterion_name + "=" + verdict_name(result.verdict) + ":" +
                    std::to_string(result.measured);
        if (exception == nullptr) {
            require(result.verdict != font_metrics::Criterion::failed,
                    name + ": " + criterion_name + " failed (" + std::to_string(result.measured) + ")");
            require(result.verdict != font_metrics::Criterion::not_measured, name + ": " + criterion_name + " not measured");
            continue;
        }
        // A named exception must still be needed and must hold its limit.
        require(result.verdict == font_metrics::Criterion::failed,
                name + ": the exception for " + criterion_name + " is stale: the criterion passes");
        require(result.measured <= (*exception).limit,
                name + ": " + criterion_name + " measures " + std::to_string(result.measured) +
                    ", beyond the exception's limit " + std::to_string((*exception).limit));
        evidence += "(exception)";
        ++totals.exceptions_used;
    }

    const std::filesystem::path installed_directory = installed_font_directory();
    if (!installed_directory.empty()) {
        const std::filesystem::path installed = installed_directory / snapshot.reference.facts.file_name;
        std::error_code status{};
        if (std::filesystem::is_regular_file(installed, status)) {
            const std::vector<std::byte> reference_bytes =
                font_metrics::read_bounded_file(installed, font_metrics::maximum_face_bytes);
            if (font_metrics::sha256_hex(reference_bytes) == snapshot.reference.facts.sha256_hex) {
                font_metrics::validate_face_bytes(reference_bytes);
                const font_metrics::FaceMeasurement reference =
                    font_metrics::measure_face(snapshot.reference.facts.file_name, reference_bytes, corpus, false);
                require(font_metrics::measurement_digest(reference) ==
                            font_metrics::measurement_digest(snapshot.reference),
                        name + ": the installed reference no longer measures as its snapshot");
                evidence += " remeasured";
                ++totals.references_remeasured;
            }
        }
    }
    std::cout << evidence << '\n';
    ++totals.snapshots;
    totals.requests_covered.push_back(snapshot.request_family + "/" + snapshot.request_style);
}

bool covered(const GateTotals& totals, const std::string& key) {
    const bool found =
        std::find(totals.requests_covered.begin(), totals.requests_covered.end(), key) != totals.requests_covered.end();
    return found;
}

const char* style_name(gui_drawing::FontFaceStyle style) {
    switch (style) {
    case gui_drawing::FontFaceStyle::regular: return "regular";
    case gui_drawing::FontFaceStyle::bold: return "bold";
    case gui_drawing::FontFaceStyle::italic: return "italic";
    case gui_drawing::FontFaceStyle::bold_italic: return "bold_italic";
    }
    return "unknown";
}

// Every substitution has evidence and every exception is exercised.
void check_coverage(const GateTotals& totals) {
    for (const gui_drawing::FontSubstitution& substitution : gui_drawing::font_substitutions()) {
        if (substitution.source != gui_drawing::FontResolutionSource::substitution) continue;
        const std::string key = std::string(substitution.requested) + "/regular";
        require(covered(totals, key), "no snapshot measures " + key);
    }
    for (const gui_drawing::FontGateException& exception : gui_drawing::font_gate_exceptions()) {
        const std::string key = std::string(exception.requested) + "/" + style_name(exception.style);
        require(covered(totals, key), "no snapshot exercises the exception for " + key);
    }
}

} // namespace

int main(int count, char** values) {
    try {
        require(count == 4, "usage: font_metrics_gate_tests <font directory> <corpus file> <snapshot directory>");
        const std::filesystem::path fonts = values[1];
        const std::filesystem::path snapshots = values[3];
        const std::vector<std::byte> corpus_bytes =
            font_metrics::read_bounded_file(values[2], font_metrics::maximum_corpus_bytes);
        const std::string_view corpus_text(reinterpret_cast<const char*>(corpus_bytes.data()), corpus_bytes.size());
        const font_metrics::Corpus corpus = font_metrics::parse_corpus(corpus_text);
        const std::string corpus_digest = font_metrics::corpus_digest(corpus);

        // Snapshot paths are sorted, so the evidence lines read the same on
        // every host.
        std::vector<std::filesystem::path> paths{};
        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(snapshots)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") paths.push_back(entry.path());
        }
        std::sort(paths.begin(), paths.end());
        require(!paths.empty(), "no snapshots in " + snapshots.string());

        GateTotals totals{};
        for (const std::filesystem::path& path : paths) {
            check_snapshot(path, fonts, corpus, corpus_digest, totals);
        }
        check_coverage(totals);
        std::cout << "font metric gate passed: " << totals.snapshots << " snapshots, " << totals.exceptions_used
                  << " named exceptions held, " << totals.references_remeasured << " installed references remeasured\n";
        return 0;
    } catch (const std::exception& failure) {
        std::cerr << failure.what() << '\n';
        return 1;
    }
}
