A calendar of one or more months on which the user selects dates.

A month calendar shows month pages: each with its title, the weekday names,
optionally week numbers, and its days. The user selects a day by clicking
it, a run of days by dragging or with Shift and the arrow keys, and scrolls
the months with the arrows beside the titles. Use it when the user should see
whole months, for example to choose a range; use a `DateTimePicker` for a
single date in a compact field.

Parameters
----------
stable_id
    Identity of the control, unique within its window. Its days are named
    ``<stable_id>.day.<yyyy-mm-dd>`` in semantics and in the published
    anatomy.

Properties
----------
selection_range
    The selected run of days, first to last. `set_selection_range` orders its
    ends, keeps at most `max_selection_count` days from the first and scrolls
    the last into view; an invalid date or one outside `min_date` to
    `max_date` throws `std::out_of_range`. `set_date` selects one day.
selection_start
    The first selected day.
selection_end
    The last selected day.
max_selection_count : default ``7``
    The most days a selection may hold. Zero throws `std::out_of_range`;
    lowering it shortens the selection.
min_date : default 1 January 1753
    The first day that can be selected. `set_range` sets it with `max_date`;
    an invalid or reversed range throws `std::invalid_argument`, and the
    selection is moved into the new range. Days outside it are disabled.
max_date : default 31 December 9998
    The last day that can be selected.
today_date
    Today, which carries the current state and the today marker. Until
    `set_today_date` sets it, it is the system clock's date in UTC: GUI.Forms
    has no time-zone service.
today_date_set
    Whether `set_today_date` has set today.
show_today : default ``true``
    Whether the "Today:" line is shown below the months.
show_today_circle : default ``true``
    Whether today's day carries the current state and its marker part.
show_week_numbers : default ``false``
    Whether each row of days has its week number: week 1 is the week that
    holds 1 January, and weeks start on `first_day_of_week`.
first_day_of_week : default ``CalendarWeekday::sunday``
    The weekday each row starts on.
calendar_dimensions : default 1 by 1
    How many months are shown, as columns by rows. A zero dimension or more
    than twelve months throws `std::out_of_range`.
scroll_change : default ``0``
    How many months an arrow scrolls; zero scrolls by the number of months
    shown. More than 20000 throws `std::out_of_range`.
first_displayed_month
    The first day of the first month shown.
format_provider
    The month and day names and the date pattern of the "Today:" line.
font
    The font of the titles, names and days.
bolded_dates
    Days shown in bold. `set_bolded_dates`, `add_bolded_date`,
    `remove_bolded_date` and `remove_all_bolded_dates` change them; an
    invalid date throws `std::invalid_argument`.
annually_bolded_dates
    Days shown in bold every year: only their month and day count.
monthly_bolded_dates
    Days shown in bold every month: only their day counts.
realized_days
    The days shown, with their parts and states.

Events
------
date_changed
    Raised after the selection changes, whatever the cause, with the new
    selection.
date_selected
    Raised when the user selects with the pointer (on release) or through
    assistive technology, after `date_changed`.

Methods
-------
scroll_months
    Scrolls the months shown forward or (negative) back, within `min_date`
    and `max_date`, and returns whether they changed.
bolded
    Whether a day is shown in bold.

See Also
--------
DateTimePicker : A date field with a drop-down calendar.

Notes
-----
Dates are civil days: the time of day of a date the calendar takes is
dropped. Days of the neighbouring months are not shown.

Keys: the arrows move a day or a week, Page Up and Page Down a month, Home
and End to the first and last day of the month; with Shift they extend the
selection. Moving stops at `min_date` and `max_date`, and the months scroll
to show the day.

Months, weekday names, week numbers and days are four item systems
(``month``, ``weekday``, ``week``, ``day``). A day publishes ``hot``,
``pressed``, ``selected``, ``current`` (today), ``disabled`` (outside the
range) and ``focus_visible``, and the custom state ``bolded``. Today's
``day_marker`` part has the perimeter role; the current paint path draws no
perimeter, so today shows only through its current state.

The control publishes the accessibility role `calendar`, each day as a
`date_cell` with select and press actions, and the arrows as buttons.
`SemanticAction::set_value` takes an ISO date (``2026-08-12``).

`DateTimePicker`'s drop-down calendar shares the calendar model and day-grid
layout with this control.

Examples
--------
A calendar that selects up to a week in the rest of the year:

.. code-block:: cpp

   std::shared_ptr<gf::MonthCalendar> calendar =
       gf::make_control<gf::MonthCalendar>(gf::StableId("booking.calendar"));
   (*calendar).set_range({2026, 8U, 1U}, {2026, 12U, 31U});
   (*calendar).set_today_date({2026, 8U, 12U});
   (*calendar).set_selection_range({2026, 8U, 14U}, {2026, 8U, 30U});
   // Seven days at most: the selection is 14 to 20 August.
   (*calendar).add_bolded_date({2026, 8U, 21U});
