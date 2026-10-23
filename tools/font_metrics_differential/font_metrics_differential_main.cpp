#include "font_metrics_differential.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

// Command line:
//   gui_forms_font_metrics_differential --reference <face> --candidate <face>
//       --class metric_compatible|nearest --corpus <file> --out <report.json> [--gdi]
// Exit status: 0 when the mapping's gate passes, 1 when it does not, 2 when an
// input is rejected or measurement fails (no report is written then).

namespace {

namespace font_metrics = gui_forms::tools::font_metrics;

struct Arguments final {
    std::filesystem::path reference{};
    std::filesystem::path candidate{};
    std::filesystem::path corpus{};
    std::filesystem::path output{};
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
    if (arguments.reference.empty() || arguments.candidate.empty() || arguments.corpus.empty() ||
        arguments.output.empty() || !arguments.mapping_given) {
        throw font_metrics::InputRejected("--reference, --candidate, --class, --corpus and --out are required");
    }
    return arguments;
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
