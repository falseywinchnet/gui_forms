#include "font_metrics_differential.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

// Command line:
//   gui_forms_font_metrics_differential --reference <face> --candidate <face>
//       --class metric_compatible|nearest --corpus <file> --out <report.json> [--gdi]
//   gui_forms_font_metrics_differential --reference <face> --candidate <face>
//       --corpus <file> --request-family <name> --request-style <style>
//       --snapshot <snapshot.txt>
// Exit status: 0 when the mapping's gate passes (or the snapshot is written),
// 1 when it does not, 2 when an input is rejected or measurement fails (no
// report or snapshot is written then).

namespace {

namespace font_metrics = gui_forms::tools::font_metrics;

struct Arguments final {
    std::filesystem::path reference{};
    std::filesystem::path candidate{};
    std::filesystem::path corpus{};
    std::filesystem::path output{};
    // Snapshot mode (brief decision 18) when non-empty.
    std::filesystem::path snapshot{};
    std::string request_family{};
    std::string request_style{};
    font_metrics::MappingClass mapping{font_metrics::MappingClass::nearest};
    bool mapping_given{};
    bool with_gdi{};
};

Arguments parse_arguments(int count, char** values) {
    Arguments arguments{};
    for (int index = 1; index < count; ++index) {
        const std::string_view flag = values[index];
        if (flag == "--gdi") {
            arguments.with_gdi = true;
            continue;
        }
        if (index + 1 >= count) throw font_metrics::InputRejected("missing value after " + std::string(flag));
        const std::string_view value = values[index + 1];
        ++index;
        if (flag == "--reference") {
            arguments.reference = value;
        } else if (flag == "--candidate") {
            arguments.candidate = value;
        } else if (flag == "--corpus") {
            arguments.corpus = value;
        } else if (flag == "--out") {
            arguments.output = value;
        } else if (flag == "--snapshot") {
            arguments.snapshot = value;
        } else if (flag == "--request-family") {
            arguments.request_family = value;
        } else if (flag == "--request-style") {
            arguments.request_style = value;
        } else if (flag == "--class" && value == "metric_compatible") {
            arguments.mapping = font_metrics::MappingClass::metric_compatible;
            arguments.mapping_given = true;
        } else if (flag == "--class" && value == "nearest") {
            arguments.mapping = font_metrics::MappingClass::nearest;
            arguments.mapping_given = true;
        } else {
            throw font_metrics::InputRejected("unknown argument " + std::string(flag));
        }
    }
    if (!arguments.snapshot.empty()) {
        const bool known_style = arguments.request_style == "regular" || arguments.request_style == "bold" ||
                                 arguments.request_style == "italic" || arguments.request_style == "bold_italic";
        if (arguments.reference.empty() || arguments.candidate.empty() || arguments.corpus.empty() ||
            arguments.request_family.empty() || !known_style || !arguments.output.empty() || arguments.with_gdi) {
            throw font_metrics::InputRejected(
                "--snapshot needs --reference, --candidate, --corpus, --request-family and a --request-style of "
                "regular, bold, italic or bold_italic, and takes no --out or --gdi");
        }
        return arguments;
    }
    if (arguments.reference.empty() || arguments.candidate.empty() || arguments.corpus.empty() ||
        arguments.output.empty() || !arguments.mapping_given) {
        throw font_metrics::InputRejected("--reference, --candidate, --class, --corpus and --out are required");
    }
    return arguments;
}

// Writes the reference's snapshot, naming the candidate's measurement digest.
// The text is composed in memory first, so a failure leaves no partial file.
void write_snapshot_file(const Arguments& arguments, const font_metrics::Corpus& corpus,
                         const font_metrics::FaceMeasurement& reference,
                         const font_metrics::FaceMeasurement& candidate) {
    font_metrics::MetricSnapshot snapshot{};
    snapshot.request_style = arguments.request_style;
    snapshot.request_family = arguments.request_family;
    snapshot.corpus_name = arguments.corpus.filename().string();
    snapshot.corpus_digest = font_metrics::corpus_digest(corpus);
    snapshot.candidate_file = candidate.facts.file_name;
    snapshot.candidate_digest = font_metrics::measurement_digest(candidate);
    snapshot.reference = reference;
    std::ostringstream text{};
    font_metrics::write_snapshot(text, snapshot);
    // The snapshot must read back as the measurement it was written from.
    const font_metrics::MetricSnapshot read_back = font_metrics::parse_snapshot(text.view());
    if (font_metrics::measurement_digest(read_back.reference) != font_metrics::measurement_digest(reference)) {
        throw std::runtime_error("the snapshot does not read back as its measurement");
    }
    std::ofstream output(arguments.snapshot, std::ios::binary | std::ios::trunc);
    if (!output) throw font_metrics::InputRejected("cannot write " + arguments.snapshot.string());
    output << text.view();
    output.close();
    if (!output) throw std::runtime_error("snapshot write failed");
}

} // namespace

int main(int count, char** values) {
    try {
        const Arguments arguments = parse_arguments(count, values);
        const std::vector<std::byte> corpus_bytes =
            font_metrics::read_bounded_file(arguments.corpus, font_metrics::maximum_corpus_bytes);
        const std::string_view corpus_text(reinterpret_cast<const char*>(corpus_bytes.data()), corpus_bytes.size());
        const font_metrics::Corpus corpus = font_metrics::parse_corpus(corpus_text);
        const std::vector<std::byte> reference_bytes =
            font_metrics::read_bounded_file(arguments.reference, font_metrics::maximum_face_bytes);
        const std::vector<std::byte> candidate_bytes =
            font_metrics::read_bounded_file(arguments.candidate, font_metrics::maximum_face_bytes);
        // Both faces are validated before either is measured.
        font_metrics::validate_face_bytes(reference_bytes);
        font_metrics::validate_face_bytes(candidate_bytes);
        const std::string reference_name = arguments.reference.filename().string();
        const std::string candidate_name = arguments.candidate.filename().string();
        const font_metrics::FaceMeasurement reference =
            font_metrics::measure_face(reference_name, reference_bytes, corpus, arguments.with_gdi);
        const font_metrics::FaceMeasurement candidate =
            font_metrics::measure_face(candidate_name, candidate_bytes, corpus, arguments.with_gdi);
        if (!arguments.snapshot.empty()) {
            write_snapshot_file(arguments, corpus, reference, candidate);
            std::cout << arguments.request_style << ' ' << arguments.request_family << ": snapshot of "
                      << reference.facts.file_name << " written\n";
            return 0;
        }
        const font_metrics::Differential differential =
            font_metrics::compare_faces(reference, candidate, arguments.mapping);
        // The report is composed in memory first, so a failed measurement never
        // leaves a partial file.
        std::ostringstream report{};
        const std::string corpus_name = arguments.corpus.filename().string();
        font_metrics::write_report(report, corpus_name, reference, candidate, differential);
        std::ofstream output(arguments.output, std::ios::binary | std::ios::trunc);
        if (!output) throw font_metrics::InputRejected("cannot write " + arguments.output.string());
        output << report.view();
        output.close();
        if (!output) throw std::runtime_error("report write failed");
        std::cout << reference.facts.family << " -> " << candidate.facts.family << ": gate "
                  << (differential.gate_passed ? "passed" : "not passed") << '\n';
        const int status = differential.gate_passed ? 0 : 1;
        return status;
    } catch (const std::exception& failure) {
        std::cerr << "font_metrics_differential: " << failure.what() << '\n';
        return 2;
    }
}
