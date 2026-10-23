#include "font_metrics_differential.hpp"

#include <algorithm>
#include <array>
#include <cmath>

// The comparison kernel. It validates that both measurements follow one plan
// and one corpus shape before it compares anything, then derives each
// criterion of owner decision 8 from the mapping class. Every gated criterion
// compares the bundled engine with itself (owner, 2026-10-23: GUI.Forms never
// renders through a host, so a host renderer is never the reference); GDI's
// measurements are carried as information for stages R and S.

namespace gui_forms::tools::font_metrics {

namespace {

void validate_same_plan(const FaceMeasurement& reference, const FaceMeasurement& candidate) {
    if (reference.sizes.size() != measured_sizes_px.size() || candidate.sizes.size() != measured_sizes_px.size()) {
        throw InputRejected("a measurement does not cover the size plan");
    }
    if (reference.advances.size() != candidate.advances.size()) throw InputRejected("advance coverage differs");
    for (std::size_t index = 0U; index < reference.advances.size(); ++index) {
        if (reference.advances[index].codepoint != candidate.advances[index].codepoint) {
            throw InputRejected("advance coverage order differs");
        }
    }
    for (std::size_t index = 0U; index < measured_sizes_px.size(); ++index) {
        const EngineSizeMeasurement& left = reference.sizes[index];
        const EngineSizeMeasurement& right = candidate.sizes[index];
        if (left.size_px != measured_sizes_px[index] || right.size_px != measured_sizes_px[index]) {
            throw InputRejected("a measurement's sizes differ from the plan");
        }
        if (left.string_widths_px.size() != right.string_widths_px.size() ||
            left.line_starts.size() != right.line_starts.size()) {
            throw InputRejected("the measurements come from different corpora");
        }
    }
    const std::array<const FaceMeasurement*, 2> measurements{&reference, &candidate};
    for (const FaceMeasurement* measurement : measurements) {
        const GdiMeasurement& gdi = (*measurement).gdi;
        if (!gdi.available) continue;
        if (gdi.sizes.size() != measured_sizes_px.size()) throw InputRejected("GDI measurement misses sizes");
        for (std::size_t index = 0U; index < gdi.sizes.size(); ++index) {
            if (gdi.sizes[index].string_widths.size() != reference.sizes[index].string_widths_px.size()) {
                throw InputRejected("GDI measurement comes from a different corpus");
            }
        }
    }
}

double line_height(const EngineSizeMeasurement& size) {
    const double height = size.ascent_px + size.descent_px + size.line_gap_px;
    return height;
}

void compare_advances(const FaceMeasurement& reference, const FaceMeasurement& candidate, Differential& result) {
    const std::int64_t reference_em = reference.facts.units_per_em;
    const std::int64_t candidate_em = candidate.facts.units_per_em;
    for (std::size_t index = 0U; index < reference.advances.size(); ++index) {
        const CodepointAdvance& left = reference.advances[index];
        const CodepointAdvance& right = candidate.advances[index];
        if (left.mapped && !right.mapped) ++result.missing_in_candidate;
        if (!left.mapped && right.mapped) ++result.missing_in_reference;
        if (!left.mapped || !right.mapped) continue;
        ++result.advances_compared;
        // Equal in ems exactly: cross-multiplied integer units.
        const bool identical = static_cast<std::int64_t>(left.advance_units) * candidate_em ==
            static_cast<std::int64_t>(right.advance_units) * reference_em;
        const double left_em = static_cast<double>(left.advance_units) / static_cast<double>(reference_em);
        const double right_em = static_cast<double>(right.advance_units) / static_cast<double>(candidate_em);
        result.max_abs_advance_delta_em = std::max(result.max_abs_advance_delta_em, std::fabs(right_em - left_em));
        if (identical) {
            ++result.advances_identical;
        } else {
            AdvanceMismatch mismatch{};
            mismatch.codepoint = left.codepoint;
            mismatch.reference_units = left.advance_units;
            mismatch.candidate_units = right.advance_units;
            result.advance_mismatches.push_back(mismatch);
        }
    }
}

// Mean (engine - GDI) / GDI over the strings GDI gives a positive width.
double engine_versus_gdi(const EngineSizeMeasurement& engine, const GdiSizeMeasurement& gdi) {
    double sum = 0.0;
    std::size_t count = 0U;
    for (std::size_t index = 0U; index < gdi.string_widths.size(); ++index) {
        if (gdi.string_widths[index] <= 0) continue;
        const double gdi_width = static_cast<double>(gdi.string_widths[index]);
        sum += (engine.string_widths_px[index] - gdi_width) / gdi_width;
        ++count;
    }
    const double mean = count == 0U ? 0.0 : sum / static_cast<double>(count);
    return mean;
}

SizeDifferential compare_size(const FaceMeasurement& reference, const FaceMeasurement& candidate,
                              std::size_t size_index) {
    const EngineSizeMeasurement& left = reference.sizes[size_index];
    const EngineSizeMeasurement& right = candidate.sizes[size_index];
    SizeDifferential result{};
    result.size_px = left.size_px;
    double deviation_sum = 0.0;
    std::size_t deviation_count = 0U;
    for (std::size_t index = 0U; index < left.string_widths_px.size(); ++index) {
        const double reference_width = left.string_widths_px[index];
        if (reference_width <= 0.0) continue;
        const double deviation = (right.string_widths_px[index] - reference_width) / reference_width;
        deviation_sum += deviation;
        ++deviation_count;
        result.max_abs_width_deviation = std::max(result.max_abs_width_deviation, std::fabs(deviation));
    }
    result.mean_width_deviation = deviation_count == 0U ? 0.0 : deviation_sum / static_cast<double>(deviation_count);
    result.line_break_sets = left.line_starts.size();
    for (std::size_t index = 0U; index < left.line_starts.size(); ++index) {
        if (left.line_starts[index] == right.line_starts[index]) ++result.line_break_sets_identical;
    }
    result.reference_line_height_px = line_height(left);
    result.candidate_line_height_px = line_height(right);
    if (reference.gdi.available) {
        result.gdi_reference_height = reference.gdi.sizes[size_index].height;
        result.reference_engine_versus_gdi = engine_versus_gdi(left, reference.gdi.sizes[size_index]);
    }
    if (candidate.gdi.available) {
        result.gdi_candidate_height = candidate.gdi.sizes[size_index].height;
        result.candidate_engine_versus_gdi = engine_versus_gdi(right, candidate.gdi.sizes[size_index]);
    }
    if (reference.gdi.available && candidate.gdi.available) {
        const std::vector<std::int32_t>& reference_widths = reference.gdi.sizes[size_index].string_widths;
        const std::vector<std::int32_t>& candidate_widths = candidate.gdi.sizes[size_index].string_widths;
        double gdi_sum = 0.0;
        std::size_t gdi_count = 0U;
        for (std::size_t index = 0U; index < reference_widths.size(); ++index) {
            if (reference_widths[index] <= 0) continue;
            const double reference_width = static_cast<double>(reference_widths[index]);
            gdi_sum += (static_cast<double>(candidate_widths[index]) - reference_width) / reference_width;
            ++gdi_count;
        }
        result.gdi_mean_width_deviation = gdi_count == 0U ? 0.0 : gdi_sum / static_cast<double>(gdi_count);
    }
    return result;
}

Criterion judged(bool passed) {
    const Criterion criterion = passed ? Criterion::passed : Criterion::failed;
    return criterion;
}

bool gated_passes(Criterion criterion) {
    const bool passes = criterion == Criterion::passed || criterion == Criterion::recorded;
    return passes;
}

} // namespace

Differential compare_faces(const FaceMeasurement& reference, const FaceMeasurement& candidate, MappingClass mapping) {
    validate_same_plan(reference, candidate);
    Differential result{};
    result.mapping = mapping;
    compare_advances(reference, candidate, result);
    result.sizes.reserve(measured_sizes_px.size());
    for (std::size_t index = 0U; index < measured_sizes_px.size(); ++index) {
        result.sizes.push_back(compare_size(reference, candidate, index));
    }

    bool breaks_identical = true;
    bool widths_within = true;
    bool heights_within = true;
    for (const SizeDifferential& size : result.sizes) {
        breaks_identical = breaks_identical && size.line_break_sets_identical == size.line_break_sets;
        widths_within = widths_within && std::fabs(size.mean_width_deviation) <= nearest_mean_width_tolerance;
        const double difference = std::fabs(size.candidate_line_height_px - size.reference_line_height_px);
        heights_within = heights_within && difference <= compatible_line_height_tolerance_px;
    }
    if (mapping == MappingClass::metric_compatible) {
        const bool advances_identical = result.advances_compared != 0U &&
            result.advances_identical == result.advances_compared;
        result.advances = judged(advances_identical);
        result.line_breaks = judged(breaks_identical);
        result.line_height = judged(heights_within);
        result.line_width = Criterion::recorded;
    } else {
        result.advances = Criterion::recorded;
        result.line_breaks = Criterion::recorded;
        result.line_height = Criterion::recorded;
        result.line_width = judged(widths_within);
    }
    result.gate_passed = gated_passes(result.advances) && gated_passes(result.line_breaks) &&
        gated_passes(result.line_height) && gated_passes(result.line_width);
    return result;
}

} // namespace gui_forms::tools::font_metrics
