#include "gui_forms/docking/dock_split_solver/dock_split_solver.hpp"

#include <algorithm>
#include <cmath>

namespace gui_forms {

DockSplitSizes solve_dock_split(double span, double ratio, double first_minimum,
                                double second_minimum) noexcept {
    DockSplitSizes sizes{};
    if (!std::isfinite(span) || span <= 0.0) {
        sizes.minimums_met = span == 0.0 && first_minimum <= 0.0 && second_minimum <= 0.0;
        return sizes;
    }
    const bool ratio_valid = std::isfinite(ratio) && ratio > 0.0 && ratio < 1.0;
    const double share = ratio_valid ? ratio : 0.5;
    const double minimum_first = std::isfinite(first_minimum) ? std::max(first_minimum, 0.0) : 0.0;
    const double minimum_second = std::isfinite(second_minimum) ? std::max(second_minimum, 0.0) : 0.0;
    const double minimums = minimum_first + minimum_second;
    if (minimums <= span) {
        const double desired = span * share;
        const double largest_first = span - minimum_second;
        sizes.first = std::clamp(desired, minimum_first, largest_first);
        sizes.minimums_met = true;
    } else {
        // Too small for both: share the span in proportion to the minimums.
        sizes.first = span * (minimum_first / minimums);
        sizes.minimums_met = false;
    }
    sizes.second = span - sizes.first;
    return sizes;
}

} // namespace gui_forms
