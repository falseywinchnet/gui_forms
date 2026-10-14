#include "calendar_grid.hpp"

#include "../date_time_utilities.hpp"

#include <algorithm>

namespace gui_forms::calendar_detail {

CalendarMonth month_of(DateTimeValue value) noexcept {
    const CalendarMonth month{value.year, value.month};
    return month;
}

CalendarMonth shifted_month(CalendarMonth month, std::int32_t offset) noexcept {
    // Months counted from year zero, so that division floors for any sign.
    const std::int64_t index = static_cast<std::int64_t>(month.year) * 12 +
                               static_cast<std::int64_t>(month.month) - 1 + offset;
    const std::int64_t year = index >= 0 ? index / 12 : (index - 11) / 12;
    const std::int64_t month_index = index - year * 12;
    const CalendarMonth result{static_cast<std::int32_t>(year),
                               static_cast<std::uint8_t>(month_index + 1)};
    return result;
}

std::int64_t months_between(CalendarMonth from, CalendarMonth to) noexcept {
    const std::int64_t first = static_cast<std::int64_t>(from.year) * 12 + from.month;
    const std::int64_t last = static_cast<std::int64_t>(to.year) * 12 + to.month;
    const std::int64_t result = last - first;
    return result;
}

DateTimeValue in_month(DateTimeValue value, CalendarMonth month) noexcept {
    DateTimeValue result = value;
    result.year = month.year;
    result.month = month.month;
    result.day = std::min(value.day, days_in_month(month.year, month.month));
    return result;
}

DateTimeValue first_day(CalendarMonth month) noexcept {
    const DateTimeValue result{month.year, month.month, 1U};
    return result;
}

bool same_day(DateTimeValue left, DateTimeValue right) noexcept {
    const bool result = left.year == right.year && left.month == right.month && left.day == right.day;
    return result;
}

DateTimeValue civil_date(DateTimeValue value) noexcept {
    const DateTimeValue result{value.year, value.month, value.day};
    return result;
}

std::array<DateTimeValue, grid_cells> month_page_dates(DateTimeValue month_start,
                                                       std::uint8_t first_day_of_week) noexcept {
    const std::uint8_t weekday = day_of_week(month_start);
    // Days from the page's first day to the month's first day: 0 to 6.
    const int leading = (static_cast<int>(weekday) - static_cast<int>(first_day_of_week) + 7) % 7;
    const DateTimeValue start = add_days(month_start, -leading);
    std::array<DateTimeValue, grid_cells> dates{};
    for (std::size_t index = 0U; index < dates.size(); ++index) {
        dates[index] = add_days(start, static_cast<std::int64_t>(index));
    }
    return dates;
}

std::optional<std::size_t> month_page_cell(DateTimeValue month_start, std::uint8_t first_day_of_week,
                                           DateTimeValue date) noexcept {
    const std::uint8_t weekday = day_of_week(month_start);
    const std::int64_t leading = (static_cast<int>(weekday) - static_cast<int>(first_day_of_week) + 7) % 7;
    const std::int64_t start = date_time_detail::days_from_civil(month_start.year, month_start.month,
                                                                 month_start.day) - leading;
    const std::int64_t offset = date_time_detail::days_from_civil(date.year, date.month, date.day) - start;
    if (offset < 0 || offset >= static_cast<std::int64_t>(grid_cells)) return std::nullopt;
    return static_cast<std::size_t>(offset);
}

Rect month_grid_cell_bounds(const MonthGridMetrics& metrics, std::size_t cell, double scale) noexcept {
    const double row = static_cast<double>(cell / grid_columns);
    const double column = static_cast<double>(cell % grid_columns);
    const Rect bounds{(metrics.cell_left + column * metrics.column_pitch) * scale,
                      (metrics.cell_top + row * metrics.row_pitch) * scale,
                      metrics.cell_width * scale, metrics.cell_height * scale};
    return bounds;
}

std::optional<std::size_t> month_grid_cell_at(const MonthGridMetrics& metrics, Point point,
                                              double scale) noexcept {
    const double left = metrics.cell_left * scale;
    const double top = metrics.cell_top * scale;
    const double right = (metrics.cell_left + metrics.column_pitch * static_cast<double>(grid_columns)) * scale;
    const double bottom = (metrics.cell_top + metrics.row_pitch * static_cast<double>(grid_rows)) * scale;
    if (point.x < left || point.y < top || point.x >= right || point.y >= bottom) return std::nullopt;
    const std::size_t column = static_cast<std::size_t>((point.x - left) / (metrics.column_pitch * scale));
    const std::size_t row = static_cast<std::size_t>((point.y - top) / (metrics.row_pitch * scale));
    if (column >= grid_columns || row >= grid_rows) return std::nullopt;
    return row * grid_columns + column;
}

} // namespace gui_forms::calendar_detail
