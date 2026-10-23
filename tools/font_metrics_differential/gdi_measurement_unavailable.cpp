#include "font_metrics_differential.hpp"

// Off Windows there is no GDI measurement; the report records why. Nothing
// gated depends on it.

namespace gui_forms::tools::font_metrics {

GdiMeasurement measure_with_gdi(std::span<const std::byte>, const FaceFacts&, const Corpus&) {
    GdiMeasurement result{};
    result.unavailable_reason = "GDI exists only on Windows";
    return result;
}

} // namespace gui_forms::tools::font_metrics
