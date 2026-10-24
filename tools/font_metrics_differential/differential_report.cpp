#include "font_metrics_differential.hpp"

#include <iomanip>
#include <locale>
#include <ostream>

// The JSON report. Written by hand in one pass: the schema is small and fixed,
// and the tool takes no JSON dependency. Numbers use the classic locale and
// nine significant digits.

namespace gui_forms::tools::font_metrics {

namespace {

void write_string(std::ostream& output, std::string_view text) {
    output << '"';
    for (const char character : text) {
        const unsigned char octet = static_cast<unsigned char>(character);
        if (character == '"' || character == '\\') {
            output << '\\' << character;
        } else if (octet < 0x20U) {
            constexpr std::string_view digits = "0123456789abcdef";
            output << "\\u00" << digits[octet >> 4U] << digits[octet & 0x0FU];
        } else {
            output << character;
        }
    }
    output << '"';
}

std::string_view criterion_name(Criterion criterion) {
    switch (criterion) {
    case Criterion::passed: return "passed";
    case Criterion::failed: return "failed";
    case Criterion::not_measured: return "not_measured";
    case Criterion::recorded: return "recorded";
    }
    return "unknown";
}

std::string_view mapping_name(MappingClass mapping) {
    const std::string_view name = mapping == MappingClass::metric_compatible ? "metric_compatible" : "nearest";
    return name;
}

template <typename Number>
void write_numbers(std::ostream& output, const std::vector<Number>& numbers) {
    output << '[';
    for (std::size_t index = 0U; index < numbers.size(); ++index) {
        if (index != 0U) output << ',';
        output << numbers[index];
    }
    output << ']';
}

void write_facts(std::ostream& output, const FaceFacts& facts) {
    output << "{\"file\":";
    write_string(output, facts.file_name);
    output << ",\"bytes\":" << facts.byte_count << ",\"sha256\":";
    write_string(output, facts.sha256_hex);
    output << ",\"family\":";
    write_string(output, facts.family);
    output << ",\"version\":";
    write_string(output, facts.version);
    output << ",\"units_per_em\":" << facts.units_per_em << ",\"weight_class\":" << facts.weight_class
           << ",\"italic\":" << (facts.italic ? "true" : "false") << ",\"glyphs\":" << facts.glyph_count
           << ",\"hhea\":[" << facts.hhea_ascender << ',' << facts.hhea_descender << ',' << facts.hhea_line_gap
           << "],\"typo\":[" << facts.typo_ascender << ',' << facts.typo_descender << ',' << facts.typo_line_gap
           << "],\"win\":[" << facts.win_ascent << ',' << facts.win_descent << "]}";
}

void write_measurement(std::ostream& output, const FaceMeasurement& measurement) {
    output << "{\"facts\":";
    write_facts(output, measurement.facts);
    output << ",\"engine\":[";
    for (std::size_t index = 0U; index < measurement.sizes.size(); ++index) {
        const EngineSizeMeasurement& size = measurement.sizes[index];
        if (index != 0U) output << ',';
        output << "\n  {\"size_px\":" << size.size_px << ",\"ascent_px\":" << size.ascent_px
               << ",\"descent_px\":" << size.descent_px << ",\"line_gap_px\":" << size.line_gap_px
               << ",\"missing_clusters\":" << size.missing_clusters << ",\"string_widths_px\":";
        write_numbers(output, size.string_widths_px);
        output << ",\"line_starts\":[";
        for (std::size_t set = 0U; set < size.line_starts.size(); ++set) {
            if (set != 0U) output << ',';
            write_numbers(output, size.line_starts[set]);
        }
        output << "]}";
    }
    const GdiMeasurement& gdi = measurement.gdi;
    output << "],\n \"gdi\":{\"available\":" << (gdi.available ? "true" : "false") << ",\"reason\":";
    write_string(output, gdi.unavailable_reason);
    output << ",\"selected_face\":";
    write_string(output, gdi.selected_face);
    output << ",\"bytes_match_file\":" << (gdi.bytes_match_file ? "true" : "false") << ",\"sizes\":[";
    for (std::size_t index = 0U; index < gdi.sizes.size(); ++index) {
        const GdiSizeMeasurement& size = gdi.sizes[index];
        if (index != 0U) output << ',';
        output << "\n  {\"size_px\":" << size.size_px << ",\"ascent\":" << size.ascent << ",\"descent\":"
               << size.descent << ",\"height\":" << size.height << ",\"internal_leading\":"
               << size.internal_leading << ",\"external_leading\":" << size.external_leading
               << ",\"string_widths\":";
        write_numbers(output, size.string_widths);
        output << '}';
    }
    output << "]}}";
}

void write_differential(std::ostream& output, const Differential& differential) {
    output << "{\"class\":";
    write_string(output, mapping_name(differential.mapping));
    output << ",\"gate_passed\":" << (differential.gate_passed ? "true" : "false") << ",\"criteria\":{\"advances\":";
    write_string(output, criterion_name(differential.advances));
    output << ",\"line_breaks\":";
    write_string(output, criterion_name(differential.line_breaks));
    output << ",\"line_height\":";
    write_string(output, criterion_name(differential.line_height));
    output << ",\"line_width\":";
    write_string(output, criterion_name(differential.line_width));
    output << "},\"tolerance\":{\"nearest_mean_width\":" << nearest_mean_width_tolerance
           << ",\"compatible_line_height_px\":" << compatible_line_height_tolerance_px << '}'
           << ",\n \"advances\":{\"compared\":" << differential.advances_compared << ",\"identical\":"
           << differential.advances_identical << ",\"missing_in_candidate\":" << differential.missing_in_candidate
           << ",\"missing_in_reference\":" << differential.missing_in_reference << ",\"max_abs_delta_em\":"
           << differential.max_abs_advance_delta_em << ",\"mismatches\":[";
    for (std::size_t index = 0U; index < differential.advance_mismatches.size(); ++index) {
        const AdvanceMismatch& mismatch = differential.advance_mismatches[index];
        if (index != 0U) output << ',';
        output << '[' << static_cast<std::uint32_t>(mismatch.codepoint) << ',' << mismatch.reference_units << ','
               << mismatch.candidate_units << ']';
    }
    output << "]},\n \"sizes\":[";
    for (std::size_t index = 0U; index < differential.sizes.size(); ++index) {
        const SizeDifferential& size = differential.sizes[index];
        if (index != 0U) output << ',';
        output << "\n  {\"size_px\":" << size.size_px << ",\"mean_width_deviation\":" << size.mean_width_deviation
               << ",\"max_abs_width_deviation\":" << size.max_abs_width_deviation << ",\"line_break_sets\":"
               << size.line_break_sets << ",\"line_break_sets_identical\":" << size.line_break_sets_identical
               << ",\"reference_line_height_px\":" << size.reference_line_height_px
               << ",\"candidate_line_height_px\":" << size.candidate_line_height_px;
        if (size.gdi_reference_height) output << ",\"gdi_reference_height\":" << *size.gdi_reference_height;
        if (size.gdi_candidate_height) output << ",\"gdi_candidate_height\":" << *size.gdi_candidate_height;
        if (size.gdi_mean_width_deviation) {
            output << ",\"gdi_mean_width_deviation\":" << *size.gdi_mean_width_deviation;
        }
        if (size.reference_engine_versus_gdi) {
            output << ",\"reference_engine_versus_gdi\":" << *size.reference_engine_versus_gdi;
        }
        if (size.candidate_engine_versus_gdi) {
            output << ",\"candidate_engine_versus_gdi\":" << *size.candidate_engine_versus_gdi;
        }
        output << '}';
    }
    output << "]}";
}

} // namespace

void write_report(std::ostream& output, std::string_view corpus_name, const FaceMeasurement& reference,
                  const FaceMeasurement& candidate, const Differential& differential) {
    output.imbue(std::locale::classic());
    output << std::setprecision(9);
    output << "{\"schema\":\"gui_forms.font_metrics_differential.v1\",\"corpus\":";
    write_string(output, corpus_name);
    output << ",\"sizes_px\":";
    const std::vector<double> sizes(measured_sizes_px.begin(), measured_sizes_px.end());
    write_numbers(output, sizes);
    output << ",\"wrap_widths_em\":";
    const std::vector<double> widths(wrap_widths_em.begin(), wrap_widths_em.end());
    write_numbers(output, widths);
    output << ",\n\"differential\":";
    write_differential(output, differential);
    output << ",\n\"reference\":";
    write_measurement(output, reference);
    output << ",\n\"candidate\":";
    write_measurement(output, candidate);
    output << "}\n";
}

} // namespace gui_forms::tools::font_metrics
