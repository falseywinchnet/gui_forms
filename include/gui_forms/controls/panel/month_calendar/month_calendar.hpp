#pragma once

#include "gui_forms/control.hpp"
#include "gui_forms/controls/panel/date_time_picker/date_time_format_provider/date_time_format_provider.hpp"
#include "gui_forms/controls/panel/date_time_picker/date_time_value/date_time_value.hpp"
#include "gui_forms/event.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace gui_forms {

// A day of the week, numbered as day_of_week numbers them (Sunday is 0).
enum class CalendarWeekday : std::uint8_t {
    sunday,
    monday,
    tuesday,
    wednesday,
    thursday,
    friday,
    saturday,
};

// A run of whole days, first to last inclusive; the time of day is zero.
struct CalendarDateRange final {
    DateTimeValue start{};
    DateTimeValue end{};

    friend constexpr bool operator==(const CalendarDateRange& left,
                                     const CalendarDateRange& right) noexcept {
        return left.start == right.start && left.end == right.end;
    }
};

// How many months the calendar shows: columns by rows, at most twelve.
struct CalendarDimensions final {
    std::uint32_t columns{1U};
    std::uint32_t rows{1U};

    friend constexpr bool operator==(CalendarDimensions left, CalendarDimensions right) noexcept {
        return left.columns == right.columns && left.rows == right.rows;
    }
};

// A month calendar (Windows Forms' MonthCalendar): one or more month pages
// on which the user selects a date or a run of dates. Each month shows its
// title, the weekday names, optionally week numbers, and its own days (days
// of neighbouring months are not shown). The arrows beside the first and
// last titles scroll the months. Days outside min_date..max_date are
// disabled. Months, weekday names, week numbers and days are its four item
// systems, and every part is painted through the by-role paint path.
//
// Dates are civil days: the time of day of every date the calendar takes is
// dropped, and every date it reports is at midnight.
class MonthCalendar final : public Control {
public:
    explicit MonthCalendar(StableId stable_id);

    // The selected run of days. set_selection_range orders its ends, keeps at
    // most max_selection_count days from the first, and shows the last.
    // Throws std::out_of_range for an invalid date or one outside
    // min_date..max_date.
    [[nodiscard]] CalendarDateRange selection_range() const noexcept { return selection_; }
    [[nodiscard]] DateTimeValue selection_start() const noexcept { return selection_.start; }
    [[nodiscard]] DateTimeValue selection_end() const noexcept { return selection_.end; }
    void set_selection_range(DateTimeValue first, DateTimeValue last);
    // Selects one day.
    void set_date(DateTimeValue date);
    // The most days a selection may hold; zero throws std::out_of_range.
    // Lowering it shortens the current selection.
    [[nodiscard]] std::uint32_t max_selection_count() const noexcept { return max_selection_count_; }
    void set_max_selection_count(std::uint32_t count);
    [[nodiscard]] DateTimeValue min_date() const noexcept { return minimum_; }
    [[nodiscard]] DateTimeValue max_date() const noexcept { return maximum_; }
    // Throws std::invalid_argument for an invalid or reversed range. The
    // selection is moved into the new range.
    void set_range(DateTimeValue minimum, DateTimeValue maximum);

    // Today's date: the system clock's date in UTC (GUI.Forms has no
    // time-zone service) until set_today_date sets it.
    [[nodiscard]] DateTimeValue today_date() const noexcept { return set_today_.value_or(system_today_); }
    void set_today_date(DateTimeValue date);
    [[nodiscard]] bool today_date_set() const noexcept { return set_today_.has_value(); }
    // Whether the "Today:" line is shown below the months.
    [[nodiscard]] bool show_today() const noexcept { return show_today_; }
    void set_show_today(bool show);
    // Whether today's day publishes the current state and its marker part.
    [[nodiscard]] bool show_today_circle() const noexcept { return show_today_circle_; }
    void set_show_today_circle(bool show);
    // Week numbers beside each row: week 1 is the week that holds 1 January
    // (Windows Forms' FirstDay rule), weeks starting on first_day_of_week.
    [[nodiscard]] bool show_week_numbers() const noexcept { return show_week_numbers_; }
    void set_show_week_numbers(bool show);
    [[nodiscard]] CalendarWeekday first_day_of_week() const noexcept { return first_day_of_week_; }
    void set_first_day_of_week(CalendarWeekday day);
    // Throws std::out_of_range for a zero dimension or more than twelve months.
    [[nodiscard]] CalendarDimensions calendar_dimensions() const noexcept { return dimensions_; }
    void set_calendar_dimensions(CalendarDimensions dimensions);
    // Months an arrow scrolls; zero (the default) scrolls by the number of
    // months shown.
    [[nodiscard]] std::uint32_t scroll_change() const noexcept { return scroll_change_; }
    void set_scroll_change(std::uint32_t months);
    // The first day of the first month shown.
    [[nodiscard]] DateTimeValue first_displayed_month() const noexcept;
    // Scrolls the months shown by `months` (negative: back), within
    // min_date..max_date. Returns whether the months changed.
    bool scroll_months(std::int32_t months);
    // Month and day names and the "Today:" date pattern.
    [[nodiscard]] const DateTimeFormatProvider& format_provider() const noexcept { return provider_; }
    void set_format_provider(DateTimeFormatProvider provider);
    [[nodiscard]] FontSpec font() const noexcept { return font_; }
    void set_font(FontSpec font);

    // Days shown in bold: on one date, on a month and day every year, or on
    // a day of every month (only the day of a monthly date counts, and only
    // the month and day of an annual one). Invalid dates throw
    // std::invalid_argument.
    [[nodiscard]] std::span<const DateTimeValue> bolded_dates() const noexcept { return bolded_; }
    void set_bolded_dates(std::vector<DateTimeValue> dates);
    void add_bolded_date(DateTimeValue date);
    void remove_bolded_date(DateTimeValue date);
    void remove_all_bolded_dates();
    [[nodiscard]] std::span<const DateTimeValue> annually_bolded_dates() const noexcept { return annually_bolded_; }
    void set_annually_bolded_dates(std::vector<DateTimeValue> dates);
    void add_annually_bolded_date(DateTimeValue date);
    void remove_annually_bolded_date(DateTimeValue date);
    void remove_all_annually_bolded_dates();
    [[nodiscard]] std::span<const DateTimeValue> monthly_bolded_dates() const noexcept { return monthly_bolded_; }
    void set_monthly_bolded_dates(std::vector<DateTimeValue> dates);
    void add_monthly_bolded_date(DateTimeValue date);
    void remove_monthly_bolded_date(DateTimeValue date);
    void remove_all_monthly_bolded_dates();
    [[nodiscard]] bool bolded(DateTimeValue date) const noexcept;

    // Raised after the selection changes, whatever the cause.
    [[nodiscard]] Event<const CalendarDateRange&>& date_changed() noexcept { return date_changed_; }
    // Raised when the user selects with the pointer (on release) or through
    // assistive technology, after date_changed.
    [[nodiscard]] Event<const CalendarDateRange&>& date_selected() noexcept { return date_selected_; }

    [[nodiscard]] Size measure(Size available) override;
    void on_paint(Painter& painter, Rect local_damage) override;
    void on_pointer(PointerEvent& event) override;
    void on_key(KeyEvent& event) override;
    void on_focus_changed(bool focused) override;
    [[nodiscard]] SemanticDescriptor semantic_descriptor() const override;
    [[nodiscard]] std::vector<SemanticNode> semantic_virtual_children() const override;
    bool on_semantic_action(SemanticAction action, std::string_view value) override;
    bool on_semantic_child_action(std::string_view stable_id, SemanticAction action,
                                  std::string_view value) override;
    [[nodiscard]] const ControlAnatomy& anatomy() const noexcept override;
    [[nodiscard]] std::size_t realized_item_count() const override;
    [[nodiscard]] std::string realized_item_stable_id(std::size_t item) const override;
    [[nodiscard]] AnatomyStateSet realized_item_anatomy_states(std::size_t item) const override;

    // Stage T1 anatomy, as dot members; the control's states are Control's.
    struct Parts final {
        PartGeometry region;
        PartGeometry face;
        PartGeometry previous_arrow;
        PartGeometry previous_glyph;
        PartGeometry next_arrow;
        PartGeometry next_glyph;
        PartGeometry today_label;
        PartGeometry focus;
    };
    [[nodiscard]] Parts parts() const;
    // Item part names are unique within the control, so each names its
    // system.
    struct MonthParts final {
        PartGeometry month_title;
        PartGeometry weekday_rule;
    };
    struct WeekdayParts final {
        PartGeometry weekday_label;
    };
    struct WeekParts final {
        PartGeometry week_label;
    };
    struct DayParts final {
        PartGeometry day_face;
        PartGeometry day_text;
        PartGeometry day_marker;
        PartGeometry day_focus;
    };
    struct DayStates final {
        bool hot{};
        bool pressed{};
        bool selected{};
        bool current{};
        bool disabled{};
        bool focus_visible{};
        bool bolded{};
    };
    struct Day final {
        DateTimeValue date{};
        std::string stable_id;
        DayParts parts;
        DayStates states;
    };
    // The days shown, month by month.
    [[nodiscard]] std::vector<Day> realized_days() const;

protected:
    void append_interactions(std::vector<InteractionDeclaration>& interactions) const override;
    void publish_part_geometry(std::span<PartGeometry> geometry) const override;
    [[nodiscard]] ItemSystemIndex publish_realized_item_system(std::size_t item) const override;
    void publish_realized_item_geometry(std::size_t item, std::span<PartGeometry> geometry) const override;

private:
    // The four item systems, in declaration and paint order.
    enum class ItemKind : std::uint8_t { month, weekday, week, day };
    // One realized item: its system, the month it belongs to (0 is the first
    // shown), and its index within the month (weekday column, page row, or
    // day of the month less one).
    struct ItemReference final {
        ItemKind kind{ItemKind::month};
        std::size_t month{};
        std::size_t index{};
    };
    // What a point is over.
    enum class Zone : std::uint8_t { none, previous, next, day };

    [[nodiscard]] std::size_t month_count() const noexcept;
    [[nodiscard]] DateTimeValue month_start(std::size_t month) const noexcept;
    [[nodiscard]] std::size_t week_rows(std::size_t month) const noexcept;
    [[nodiscard]] ItemReference locate(std::size_t item) const;
    // Geometry, in local coordinates at the current text scale.
    [[nodiscard]] Rect month_block(std::size_t month) const noexcept;
    [[nodiscard]] Rect day_cell(std::size_t month, std::size_t day_index) const noexcept;
    [[nodiscard]] Parts control_parts() const;
    [[nodiscard]] MonthParts month_parts(std::size_t month) const;
    [[nodiscard]] WeekdayParts weekday_parts(std::size_t month, std::size_t column) const;
    [[nodiscard]] WeekParts week_parts(std::size_t month, std::size_t row) const;
    [[nodiscard]] DayParts day_parts(std::size_t month, std::size_t day_index) const;
    [[nodiscard]] AnatomyStateSet day_states(DateTimeValue date) const;
    [[nodiscard]] std::string weekday_text(std::size_t column) const;
    [[nodiscard]] std::string week_text(std::size_t month, std::size_t row) const;
    [[nodiscard]] std::string month_title_text(std::size_t month) const;
    [[nodiscard]] std::string today_text() const;
    [[nodiscard]] std::string day_stable_id(DateTimeValue date) const;
    [[nodiscard]] std::string month_stable_id(std::size_t month) const;
    [[nodiscard]] bool in_range(DateTimeValue date) const noexcept;
    [[nodiscard]] bool can_scroll(std::int32_t direction) const noexcept;
    [[nodiscard]] std::int32_t scroll_step() const noexcept;
    [[nodiscard]] Zone zone_at(Point local, DateTimeValue& date) const;
    // The run from `anchor` toward `target`, at most max_selection_count days.
    [[nodiscard]] CalendarDateRange bounded_range(DateTimeValue anchor, DateTimeValue target) const noexcept;
    void select(CalendarDateRange range, DateTimeValue focus);
    void move_focus(DateTimeValue target, bool extend);
    void ensure_visible(DateTimeValue date);
    void clamp_first_month();
    void paint_day(Painter& painter, std::size_t month, std::size_t day_index) const;
    void validate_bolded(DateTimeValue date) const;
    void bolded_changed();

    CalendarDateRange selection_{};
    DateTimeValue anchor_{};
    DateTimeValue focus_date_{};
    DateTimeValue minimum_{1753, 1U, 1U};
    DateTimeValue maximum_{9998, 12U, 31U};
    // The system clock's date, read once at construction; set_today_ replaces
    // it once set_today_date has run, so the two cannot disagree.
    DateTimeValue system_today_{};
    std::optional<DateTimeValue> set_today_{};
    std::int32_t first_year_{2000};
    std::uint8_t first_month_{1U};
    std::uint32_t max_selection_count_{7U};
    std::uint32_t scroll_change_{};
    CalendarDimensions dimensions_{};
    CalendarWeekday first_day_of_week_{CalendarWeekday::sunday};
    DateTimeFormatProvider provider_{DateTimeFormatProvider::english_united_states()};
    FontSpec font_{FontRole::control, 10.5, 400, false, 0.12};
    std::vector<DateTimeValue> bolded_{};
    std::vector<DateTimeValue> annually_bolded_{};
    std::vector<DateTimeValue> monthly_bolded_{};
    std::optional<DateTimeValue> hot_date_{};
    Zone hot_zone_{Zone::none};
    Zone pressed_zone_{Zone::none};
    bool dragging_{};
    bool show_today_{true};
    bool show_today_circle_{true};
    bool show_week_numbers_{};
    bool focused_{};
    Event<const CalendarDateRange&> date_changed_{};
    Event<const CalendarDateRange&> date_selected_{};
};

} // namespace gui_forms
